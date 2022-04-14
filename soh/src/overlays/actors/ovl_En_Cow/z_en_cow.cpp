/*
 * File: z_en_cow.c
 * Overlay: ovl_En_Cow
 * Description: Cow
 */

#include "z_en_cow.h"
#include "objects/object_cow/object_cow.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnCow_Init(Actor* thisx, GlobalContext* globalCtx);
void EnCow_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnCow_Update(Actor* thisx, GlobalContext* globalCtx);
void EnCow_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_809DFE98(Actor* thisx, GlobalContext* globalCtx);
void func_809E0070(Actor* thisx, GlobalContext* globalCtx);

void func_809DF494(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF6BC(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF778(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF7D8(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF870(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF8FC(EnCow* thisv, GlobalContext* globalCtx);
void func_809DF96C(EnCow* thisv, GlobalContext* globalCtx);
void func_809DFA84(EnCow* thisv, GlobalContext* globalCtx);

const ActorInit En_Cow_InitVars = {
    ACTOR_EN_COW,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_COW,
    sizeof(EnCow),
    (ActorFunc)EnCow_Init,
    (ActorFunc)EnCow_Destroy,
    (ActorFunc)EnCow_Update,
    (ActorFunc)EnCow_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static Vec3f D_809E010C = { 0.0f, -1300.0f, 1100.0f };

void func_809DEE00(Vec3f* vec, s16 rotY) {
    f32 xCalc;
    f32 rotCalcTemp;

    rotCalcTemp = Math_CosS(rotY);
    xCalc = (Math_SinS(rotY) * vec->z) + (rotCalcTemp * vec->x);
    rotCalcTemp = Math_SinS(rotY);
    vec->z = (Math_CosS(rotY) * vec->z) + (-rotCalcTemp * vec->x);
    vec->x = xCalc;
}

void func_809DEE9C(EnCow* thisv) {
    Vec3f vec;

    vec.y = 0.0f;
    vec.x = 0.0f;
    vec.z = 30.0f;
    func_809DEE00(&vec, thisv->actor.shape.rot.y);
    thisv->colliders[0].dim.pos.x = thisv->actor.world.pos.x + vec.x;
    thisv->colliders[0].dim.pos.y = thisv->actor.world.pos.y;
    thisv->colliders[0].dim.pos.z = thisv->actor.world.pos.z + vec.z;

    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = -20.0f;
    func_809DEE00(&vec, thisv->actor.shape.rot.y);
    thisv->colliders[1].dim.pos.x = thisv->actor.world.pos.x + vec.x;
    thisv->colliders[1].dim.pos.y = thisv->actor.world.pos.y;
    thisv->colliders[1].dim.pos.z = thisv->actor.world.pos.z + vec.z;
}

void func_809DEF94(EnCow* thisv) {
    Vec3f vec;

    VEC_SET(vec, 0.0f, 57.0f, -36.0f);

    func_809DEE00(&vec, thisv->actor.shape.rot.y);
    thisv->actor.world.pos.x += vec.x;
    thisv->actor.world.pos.y += vec.y;
    thisv->actor.world.pos.z += vec.z;
}

void EnCow_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnCow* thisv = (EnCow*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 72.0f);
    switch (thisv->actor.params) {
        case 0:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gCowBodySkel, NULL, thisv->jointTable, thisv->morphTable, 6);
            Animation_PlayLoop(&thisv->skelAnime, &gCowBodyChewAnim);
            Collider_InitCylinder(globalCtx, &thisv->colliders[0]);
            Collider_SetCylinder(globalCtx, &thisv->colliders[0], &thisv->actor, &sCylinderInit);
            Collider_InitCylinder(globalCtx, &thisv->colliders[1]);
            Collider_SetCylinder(globalCtx, &thisv->colliders[1], &thisv->actor, &sCylinderInit);
            func_809DEE9C(thisv);
            thisv->actionFunc = func_809DF96C;
            if (globalCtx->sceneNum == SCENE_LINK_HOME) {
                if (!LINK_IS_ADULT) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
                if (!(gSaveContext.eventChkInf[1] & 0x4000)) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            }
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_COW, thisv->actor.world.pos.x,
                               thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, thisv->actor.shape.rot.y, 0, 1);
            thisv->unk_278 = Rand_ZeroFloat(1000.0f) + 40.0f;
            thisv->unk_27A = 0;
            thisv->actor.targetMode = 6;
            DREG(53) = 0;
            break;
        case 1:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gCowTailSkel, NULL, thisv->jointTable, thisv->morphTable, 6);
            Animation_PlayLoop(&thisv->skelAnime, &gCowTailIdleAnim);
            thisv->actor.update = func_809DFE98;
            thisv->actor.draw = func_809E0070;
            thisv->actionFunc = func_809DFA84;
            func_809DEF94(thisv);
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->unk_278 = ((u32)(Rand_ZeroFloat(1000.0f)) & 0xFFFF) + 40.0f;
            break;
    }
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->unk_276 = 0;
}

void EnCow_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnCow* thisv = (EnCow*)thisx;

    if (thisv->actor.params == 0) {
        Collider_DestroyCylinder(globalCtx, &thisv->colliders[0]);
        Collider_DestroyCylinder(globalCtx, &thisv->colliders[1]);
    }
}

void func_809DF494(EnCow* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_278 > 0) {
        thisv->unk_278 -= 1;
    } else {
        thisv->unk_278 = Rand_ZeroFloat(500.0f) + 40.0f;
        Animation_Change(&thisv->skelAnime, &gCowBodyChewAnim, 1.0f, thisv->skelAnime.curFrame,
                         Animation_GetLastFrame(&gCowBodyChewAnim), ANIMMODE_ONCE, 1.0f);
    }

    if ((thisv->actor.xzDistToPlayer < 150.0f) && (!(thisv->unk_276 & 2))) {
        thisv->unk_276 |= 2;
        if (thisv->skelAnime.animation == &gCowBodyChewAnim) {
            thisv->unk_278 = 0;
        }
    }

    thisv->unk_27A += 1;
    if (thisv->unk_27A >= 0x31) {
        thisv->unk_27A = 0;
    }

    // (1.0f / 100.0f) instead of 0.01f below is necessary so 0.01f doesn't get reused mistakenly
    if (thisv->unk_27A < 0x20) {
        thisv->actor.scale.x = ((Math_SinS(thisv->unk_27A << 0xA) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        thisv->actor.scale.x = 0.01f;
    }

    if (thisv->unk_27A >= 0x11) {
        thisv->actor.scale.y = ((Math_SinS((thisv->unk_27A << 0xA) - 0x4000) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        thisv->actor.scale.y = 0.01f;
    }
}

void func_809DF6BC(EnCow* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = func_809DF96C;
    }
}

void func_809DF730(EnCow* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = func_809DF96C;
    }
}

void func_809DF778(EnCow* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_809DF730;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_MILK, 10000.0f, 100.0f);
    }
}

void func_809DF7D8(EnCow* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = func_809DF778;
        func_8002F434(&thisv->actor, globalCtx, GI_MILK, 10000.0f, 100.0f);
    }
}

void func_809DF870(EnCow* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        if (Inventory_HasEmptyBottle()) {
            Message_ContinueTextbox(globalCtx, 0x2007);
            thisv->actionFunc = func_809DF7D8;
        } else {
            Message_ContinueTextbox(globalCtx, 0x2013);
            thisv->actionFunc = func_809DF6BC;
        }
    }
}

void func_809DF8FC(EnCow* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_809DF870;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F2CC(&thisv->actor, globalCtx, 170.0f);
        thisv->actor.textId = 0x2006;
    }
    func_809DF494(thisv, globalCtx);
}

void func_809DF96C(EnCow* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_00) || (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04)) {
        if (DREG(53) != 0) {
            if (thisv->unk_276 & 4) {
                thisv->unk_276 &= ~0x4;
                DREG(53) = 0;
            } else {
                if ((thisv->actor.xzDistToPlayer < 150.0f) &&
                    (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) < 0x61A8)) {
                    DREG(53) = 0;
                    thisv->actionFunc = func_809DF8FC;
                    thisv->actor.flags |= ACTOR_FLAG_16;
                    func_8002F2CC(&thisv->actor, globalCtx, 170.0f);
                    thisv->actor.textId = 0x2006;
                } else {
                    thisv->unk_276 |= 4;
                }
            }
        } else {
            thisv->unk_276 &= ~0x4;
        }
    }
    func_809DF494(thisv, globalCtx);
}

void func_809DFA84(EnCow* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_278 > 0) {
        thisv->unk_278--;
    } else {
        thisv->unk_278 = Rand_ZeroFloat(200.0f) + 40.0f;
        Animation_Change(&thisv->skelAnime, &gCowTailIdleAnim, 1.0f, thisv->skelAnime.curFrame,
                         Animation_GetLastFrame(&gCowTailIdleAnim), ANIMMODE_ONCE, 1.0f);
    }

    if ((thisv->actor.xzDistToPlayer < 150.0f) &&
        (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) >= 0x61A9) && (!(thisv->unk_276 & 2))) {
        thisv->unk_276 |= 2;
        if (thisv->skelAnime.animation == &gCowTailIdleAnim) {
            thisv->unk_278 = 0;
        }
    }
}

void EnCow_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnCow* thisv = (EnCow*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s16 targetX;
    s16 targetY;
    Player* player = GET_PLAYER(globalCtx);

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[0].base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliders[1].base);
    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(globalCtx, thisx, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->skelAnime.animation == &gCowBodyChewAnim) {
            Audio_PlayActorSound2(thisx, NA_SE_EV_COW_CRY);
            Animation_Change(&thisv->skelAnime, &gCowBodyMoveHeadAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gCowBodyMoveHeadAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Animation_Change(&thisv->skelAnime, &gCowBodyChewAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCowBodyChewAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    if ((thisx->xzDistToPlayer < 150.0f) &&
        (ABS(Math_Vec3f_Yaw(&thisx->world.pos, &player->actor.world.pos)) < 0xC000)) {
        targetX = Math_Vec3f_Pitch(&thisx->focus.pos, &player->actor.focus.pos);
        targetY = Math_Vec3f_Yaw(&thisx->focus.pos, &player->actor.focus.pos) - thisx->shape.rot.y;

        if (targetX > 0x1000) {
            targetX = 0x1000;
        } else if (targetX < -0x1000) {
            targetX = -0x1000;
        }

        if (targetY > 0x2500) {
            targetY = 0x2500;
        } else if (targetY < -0x2500) {
            targetY = -0x2500;
        }

    } else {
        targetY = 0;
        targetX = 0;
    }
    Math_SmoothStepToS(&thisv->someRot.x, targetX, 0xA, 0xC8, 0xA);
    Math_SmoothStepToS(&thisv->someRot.y, targetY, 0xA, 0xC8, 0xA);
}

void func_809DFE98(Actor* thisx, GlobalContext* globalCtx) {
    EnCow* thisv = (EnCow*)thisx;
    s32 pad;

    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->skelAnime.animation == &gCowTailIdleAnim) {
            Animation_Change(&thisv->skelAnime, &gCowTailSwishAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gCowTailSwishAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Animation_Change(&thisv->skelAnime, &gCowTailIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCowTailIdleAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnCow_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCow* thisv = (EnCow*)thisx;

    if (limbIndex == 2) {
        rot->y += thisv->someRot.y;
        rot->x += thisv->someRot.x;
    }
    if (limbIndex == 5) {
        *dList = NULL;
    }
    return false;
}

void EnCow_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnCow* thisv = (EnCow*)thisx;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&D_809E010C, &thisv->actor.focus.pos);
    }
}

void EnCow_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnCow* thisv = (EnCow*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnCow_OverrideLimbDraw, EnCow_PostLimbDraw, thisv);
}

void func_809E0070(Actor* thisx, GlobalContext* globalCtx) {
    EnCow* thisv = (EnCow*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
}
