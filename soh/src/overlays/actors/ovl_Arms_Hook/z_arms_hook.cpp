#include "z_arms_hook.h"
#include "objects/object_link_boy/object_link_boy.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void ArmsHook_Init(Actor* thisvx, GlobalContext* globalCtx);
void ArmsHook_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void ArmsHook_Update(Actor* thisvx, GlobalContext* globalCtx);
void ArmsHook_Draw(Actor* thisvx, GlobalContext* globalCtx);

void ArmsHook_Wait(ArmsHook* thisv, GlobalContext* globalCtx);
void ArmsHook_Shoot(ArmsHook* thisv, GlobalContext* globalCtx);

ActorInit Arms_Hook_InitVars = {
    ACTOR_ARMS_HOOK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_LINK_BOY,
    sizeof(ArmsHook),
    (ActorFunc)ArmsHook_Init,
    (ActorFunc)ArmsHook_Destroy,
    (ActorFunc)ArmsHook_Update,
    (ActorFunc)ArmsHook_Draw,
    NULL,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000080, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f sUnusedVec1 = { 0.0f, 0.5f, 0.0f };
static Vec3f sUnusedVec2 = { 0.0f, 0.5f, 0.0f };

static Color_RGB8 sUnusedColors[] = {
    { 255, 255, 100 },
    { 255, 255, 50 },
};

static Vec3f D_80865B70 = { 0.0f, 0.0f, 0.0f };
static Vec3f D_80865B7C = { 0.0f, 0.0f, 900.0f };
static Vec3f D_80865B88 = { 0.0f, 500.0f, -3000.0f };
static Vec3f D_80865B94 = { 0.0f, -500.0f, -3000.0f };
static Vec3f D_80865BA0 = { 0.0f, 500.0f, 1200.0f };
static Vec3f D_80865BAC = { 0.0f, -500.0f, 1200.0f };

void ArmsHook_SetupAction(ArmsHook* thisv, ArmsHookActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ArmsHook_Init(Actor* thisvx, GlobalContext* globalCtx) {
    ArmsHook* thisv = (ArmsHook*)thisvx;

    Collider_InitQuad(globalCtx, &thisv->collider);
    Collider_SetQuad(globalCtx, &thisv->collider, &thisv->actor, &sQuadInit);
    ArmsHook_SetupAction(thisv, ArmsHook_Wait);
    thisv->unk_1E8 = thisv->actor.world.pos;
}

void ArmsHook_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    ArmsHook* thisv = (ArmsHook*)thisvx;

    if (thisv->grabbed != NULL) {
        thisv->grabbed->flags &= ~ACTOR_FLAG_13;
    }
    Collider_DestroyQuad(globalCtx, &thisv->collider);
}

void ArmsHook_Wait(ArmsHook* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.parent == NULL) {
        Player* player = GET_PLAYER(globalCtx);
        // get correct timer length for hookshot or longshot
        s32 length = (player->heldItemActionParam == PLAYER_AP_HOOKSHOT) ? 13 : 26;

        ArmsHook_SetupAction(thisv, ArmsHook_Shoot);
        func_8002D9A4(&thisv->actor, 20.0f);
        thisv->actor.parent = &GET_PLAYER(globalCtx)->actor;
        thisv->timer = length;
    }
}

void func_80865044(ArmsHook* thisv) {
    thisv->actor.child = thisv->actor.parent;
    thisv->actor.parent->parent = &thisv->actor;
}

s32 ArmsHook_AttachToPlayer(ArmsHook* thisv, Player* player) {
    player->actor.child = &thisv->actor;
    player->heldActor = &thisv->actor;
    if (thisv->actor.child != NULL) {
        player->actor.parent = NULL;
        thisv->actor.child = NULL;
        return true;
    }
    return false;
}

void ArmsHook_DetachHookFromActor(ArmsHook* thisv) {
    if (thisv->grabbed != NULL) {
        thisv->grabbed->flags &= ~ACTOR_FLAG_13;
        thisv->grabbed = NULL;
    }
}

s32 ArmsHook_CheckForCancel(ArmsHook* thisv) {
    Player* player = (Player*)thisv->actor.parent;

    if (Player_HoldsHookshot(player)) {
        if ((player->itemActionParam != player->heldItemActionParam) || (player->actor.flags & ACTOR_FLAG_8) ||
            ((player->stateFlags1 & 0x4000080))) {
            thisv->timer = 0;
            ArmsHook_DetachHookFromActor(thisv);
            Math_Vec3f_Copy(&thisv->actor.world.pos, &player->unk_3C8);
            return 1;
        }
    }
    return 0;
}

void ArmsHook_AttachHookToActor(ArmsHook* thisv, Actor* actor) {
    actor->flags |= ACTOR_FLAG_13;
    thisv->grabbed = actor;
    Math_Vec3f_Diff(&actor->world.pos, &thisv->actor.world.pos, &thisv->grabbedDistDiff);
}

void ArmsHook_Shoot(ArmsHook* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* touchedActor;
    Actor* grabbed;
    Vec3f bodyDistDiffVec;
    Vec3f newPos;
    f32 bodyDistDiff;
    f32 phi_f16;
    DynaPolyActor* dynaPolyActor;
    f32 sp94;
    f32 sp90;
    s32 pad;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f sp78;
    Vec3f prevFrameDiff;
    Vec3f sp60;
    f32 sp5C;
    f32 sp58;
    f32 velocity;
    s32 pad1;

    if ((thisv->actor.parent == NULL) || (!Player_HoldsHookshot(player))) {
        ArmsHook_DetachHookFromActor(thisv);
        Actor_Kill(&thisv->actor);
        return;
    }

    func_8002F8F0(&player->actor, NA_SE_IT_HOOKSHOT_CHAIN - SFX_FLAG);
    ArmsHook_CheckForCancel(thisv);

    if ((thisv->timer != 0) && (thisv->collider.base.atFlags & AT_HIT) &&
        (thisv->collider.info.atHitInfo->elemType != ELEMTYPE_UNK4)) {
        touchedActor = thisv->collider.base.at;
        if ((touchedActor->update != NULL) && (touchedActor->flags & (ACTOR_FLAG_9 | ACTOR_FLAG_10))) {
            if (thisv->collider.info.atHitInfo->bumperFlags & BUMP_HOOKABLE) {
                ArmsHook_AttachHookToActor(thisv, touchedActor);
                if (CHECK_FLAG_ALL(touchedActor->flags, ACTOR_FLAG_10)) {
                    func_80865044(thisv);
                }
            }
        }
        thisv->timer = 0;
        Audio_PlaySoundGeneral(NA_SE_IT_ARROW_STICK_CRE, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (DECR(thisv->timer) == 0) {
        grabbed = thisv->grabbed;
        if (grabbed != NULL) {
            if ((grabbed->update == NULL) || !CHECK_FLAG_ALL(grabbed->flags, ACTOR_FLAG_13)) {
                grabbed = NULL;
                thisv->grabbed = NULL;
            } else if (thisv->actor.child != NULL) {
                sp94 = Actor_WorldDistXYZToActor(&thisv->actor, grabbed);
                sp90 = sqrtf(SQ(thisv->grabbedDistDiff.x) + SQ(thisv->grabbedDistDiff.y) + SQ(thisv->grabbedDistDiff.z));
                Math_Vec3f_Diff(&grabbed->world.pos, &thisv->grabbedDistDiff, &thisv->actor.world.pos);
                if (50.0f < (sp94 - sp90)) {
                    ArmsHook_DetachHookFromActor(thisv);
                    grabbed = NULL;
                }
            }
        }

        bodyDistDiff = Math_Vec3f_DistXYZAndStoreDiff(&player->unk_3C8, &thisv->actor.world.pos, &bodyDistDiffVec);
        if (bodyDistDiff < 30.0f) {
            velocity = 0.0f;
            phi_f16 = 0.0f;
        } else {
            if (thisv->actor.child != NULL) {
                velocity = 30.0f;
            } else if (grabbed != NULL) {
                velocity = 50.0f;
            } else {
                velocity = 200.0f;
            }
            phi_f16 = bodyDistDiff - velocity;
            if (bodyDistDiff <= velocity) {
                phi_f16 = 0.0f;
            }
            velocity = phi_f16 / bodyDistDiff;
        }

        newPos.x = bodyDistDiffVec.x * velocity;
        newPos.y = bodyDistDiffVec.y * velocity;
        newPos.z = bodyDistDiffVec.z * velocity;

        if (thisv->actor.child == NULL) {
            if ((grabbed != NULL) && (grabbed->id == ACTOR_BG_SPOT06_OBJECTS)) {
                Math_Vec3f_Diff(&grabbed->world.pos, &thisv->grabbedDistDiff, &thisv->actor.world.pos);
                phi_f16 = 1.0f;
            } else {
                Math_Vec3f_Sum(&player->unk_3C8, &newPos, &thisv->actor.world.pos);
                if (grabbed != NULL) {
                    Math_Vec3f_Sum(&thisv->actor.world.pos, &thisv->grabbedDistDiff, &grabbed->world.pos);
                }
            }
        } else {
            Math_Vec3f_Diff(&bodyDistDiffVec, &newPos, &player->actor.velocity);
            player->actor.world.rot.x =
                Math_Atan2S(sqrtf(SQ(bodyDistDiffVec.x) + SQ(bodyDistDiffVec.z)), -bodyDistDiffVec.y);
        }

        if (phi_f16 < 50.0f) {
            ArmsHook_DetachHookFromActor(thisv);
            if (phi_f16 == 0.0f) {
                ArmsHook_SetupAction(thisv, ArmsHook_Wait);
                if (ArmsHook_AttachToPlayer(thisv, player)) {
                    Math_Vec3f_Diff(&thisv->actor.world.pos, &player->actor.world.pos, &player->actor.velocity);
                    player->actor.velocity.y -= 20.0f;
                }
            }
        }
    } else {
        Actor_MoveForward(&thisv->actor);
        Math_Vec3f_Diff(&thisv->actor.world.pos, &thisv->actor.prevPos, &prevFrameDiff);
        Math_Vec3f_Sum(&thisv->unk_1E8, &prevFrameDiff, &thisv->unk_1E8);
        thisv->actor.shape.rot.x = Math_Atan2S(thisv->actor.speedXZ, -thisv->actor.velocity.y);
        sp60.x = thisv->unk_1F4.x - (thisv->unk_1E8.x - thisv->unk_1F4.x);
        sp60.y = thisv->unk_1F4.y - (thisv->unk_1E8.y - thisv->unk_1F4.y);
        sp60.z = thisv->unk_1F4.z - (thisv->unk_1E8.z - thisv->unk_1F4.z);
        if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp60, &thisv->unk_1E8, &sp78, &poly, true, true, true, true,
                                    &bgId) &&
            !func_8002F9EC(globalCtx, &thisv->actor, poly, bgId, &sp78)) {
            sp5C = COLPOLY_GET_NORMAL(poly->normal.x);
            sp58 = COLPOLY_GET_NORMAL(poly->normal.z);
            Math_Vec3f_Copy(&thisv->actor.world.pos, &sp78);
            thisv->actor.world.pos.x += 10.0f * sp5C;
            thisv->actor.world.pos.z += 10.0f * sp58;
            thisv->timer = 0;
            if (SurfaceType_IsHookshotSurface(&globalCtx->colCtx, poly, bgId)) {
                if (bgId != BGCHECK_SCENE) {
                    dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, bgId);
                    if (dynaPolyActor != NULL) {
                        ArmsHook_AttachHookToActor(thisv, &dynaPolyActor->actor);
                    }
                }
                func_80865044(thisv);
                Audio_PlaySoundGeneral(NA_SE_IT_HOOKSHOT_STICK_OBJ, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            } else {
                CollisionCheck_SpawnShieldParticlesMetal(globalCtx, &thisv->actor.world.pos);
                Audio_PlaySoundGeneral(NA_SE_IT_HOOKSHOT_REFLECT, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
        } else if (CHECK_BTN_ANY(globalCtx->state.input[0].press.button,
                                 (BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN))) {
            thisv->timer = 0;
        }
    }
}

void ArmsHook_Update(Actor* thisvx, GlobalContext* globalCtx) {
    ArmsHook* thisv = (ArmsHook*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
    thisv->unk_1F4 = thisv->unk_1E8;
}

void ArmsHook_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    ArmsHook* thisv = (ArmsHook*)thisvx;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;
    f32 sp5C;
    f32 sp58;

    if ((player->actor.draw != NULL) && (player->rightHandType == 15)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_arms_hook.c", 850);

        if ((ArmsHook_Shoot != thisv->actionFunc) || (thisv->timer <= 0)) {
            Matrix_MultVec3f(&D_80865B70, &thisv->unk_1E8);
            Matrix_MultVec3f(&D_80865B88, &sp6C);
            Matrix_MultVec3f(&D_80865B94, &sp60);
            thisv->hookInfo.active = 0;
        } else {
            Matrix_MultVec3f(&D_80865B7C, &thisv->unk_1E8);
            Matrix_MultVec3f(&D_80865BA0, &sp6C);
            Matrix_MultVec3f(&D_80865BAC, &sp60);
        }

        func_80090480(globalCtx, &thisv->collider, &thisv->hookInfo, &sp6C, &sp60);
        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_arms_hook.c", 895),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gLinkAdultHookshotTipDL);
        Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
        Math_Vec3f_Diff(&player->unk_3C8, &thisv->actor.world.pos, &sp78);
        sp58 = SQ(sp78.x) + SQ(sp78.z);
        sp5C = sqrtf(sp58);
        Matrix_RotateY(Math_FAtan2F(sp78.x, sp78.z), MTXMODE_APPLY);
        Matrix_RotateX(Math_FAtan2F(-sp78.y, sp5C), MTXMODE_APPLY);
        Matrix_Scale(0.015f, 0.015f, sqrtf(SQ(sp78.y) + sp58) * 0.01f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_arms_hook.c", 910),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gLinkAdultHookshotChainDL);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_arms_hook.c", 913);
    }
}
