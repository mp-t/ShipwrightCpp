/*
 * File: z_en_bom.c
 * Overlay: ovl_En_Bom
 * Description: Bomb
 */

#include "z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnBom_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBom_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBom_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBom_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBom_Move(EnBom* thisv, GlobalContext* globalCtx);
void EnBom_WaitForRelease(EnBom* thisv, GlobalContext* globalCtx);

const ActorInit En_Bom_InitVars = {
    ACTOR_EN_BOM,
    ACTORCAT_EXPLOSIVE,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBom),
    (ActorFunc)EnBom_Init,
    (ActorFunc)EnBom_Destroy,
    (ActorFunc)EnBom_Update,
    (ActorFunc)EnBom_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x0003F828, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 6, 11, 14, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000008, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_STOP),
};

void EnBom_SetupAction(EnBom* thisv, EnBomActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBom_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBom* thisv = (EnBom*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 700.0f, ActorShadow_DrawCircle, 16.0f);
    thisx->colChkInfo.mass = 200;
    thisx->colChkInfo.cylRadius = 5;
    thisx->colChkInfo.cylHeight = 10;
    thisv->timer = 70;
    thisv->flashSpeedScale = 7;
    Collider_InitCylinder(globalCtx, &thisv->bombCollider);
    Collider_InitJntSph(globalCtx, &thisv->explosionCollider);
    Collider_SetCylinder(globalCtx, &thisv->bombCollider, thisx, &sCylinderInit);
    Collider_SetJntSph(globalCtx, &thisv->explosionCollider, thisx, &sJntSphInit, &thisv->explosionColliderItems[0]);
    thisv->explosionColliderItems[0].info.toucher.damage += (thisx->shape.rot.z & 0xFF00) >> 8;

    thisx->shape.rot.z &= 0xFF;
    if (thisx->shape.rot.z & 0x80) {
        thisx->shape.rot.z |= 0xFF00;
    }

    EnBom_SetupAction(thisv, EnBom_Move);
}

void EnBom_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBom* thisv = (EnBom*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->explosionCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->bombCollider);
}

void EnBom_Move(EnBom* thisv, GlobalContext* globalCtx) {
    // if bomb has a parent actor, the bomb hasnt been released yet
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        EnBom_SetupAction(thisv, EnBom_WaitForRelease);
        thisv->actor.room = -1;
        return;
    }

    if ((thisv->actor.velocity.y > 0.0f) && (thisv->actor.bgCheckFlags & 0x10)) {
        thisv->actor.velocity.y = -thisv->actor.velocity.y;
    }

    // rebound bomb off the wall it hits
    if ((thisv->actor.speedXZ != 0.0f) && (thisv->actor.bgCheckFlags & 8)) {
        if (ABS((s16)(thisv->actor.wallYaw - thisv->actor.world.rot.y)) > 0x4000) {
            thisv->actor.world.rot.y = ((thisv->actor.wallYaw - thisv->actor.world.rot.y) + thisv->actor.wallYaw) - 0x8000;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BOMB_BOUND);
        Actor_MoveForward(&thisv->actor);
        thisv->actor.speedXZ *= 0.7f;
        thisv->actor.bgCheckFlags &= ~8;
    }

    if (!(thisv->actor.bgCheckFlags & 1)) {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.08f);
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 1.0f);
        if ((thisv->actor.bgCheckFlags & 2) && (thisv->actor.velocity.y < -3.0f)) {
            func_8002F850(globalCtx, &thisv->actor);
            thisv->actor.velocity.y *= -0.3f;
            thisv->actor.bgCheckFlags &= ~2;
        } else if (thisv->timer >= 4) {
            func_8002F580(&thisv->actor, globalCtx);
        }
    }

    Actor_MoveForward(&thisv->actor);
}

void EnBom_WaitForRelease(EnBom* thisv, GlobalContext* globalCtx) {
    // if parent is NULL bomb has been released
    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        EnBom_SetupAction(thisv, EnBom_Move);
        EnBom_Move(thisv, globalCtx);
    }
}

void EnBom_Explode(EnBom* thisv, GlobalContext* globalCtx) {
    Player* player;

    if (thisv->explosionCollider.elements[0].dim.modelSphere.radius == 0) {
        thisv->actor.flags |= ACTOR_FLAG_5;
        func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
    }

    thisv->explosionCollider.elements[0].dim.worldSphere.radius += thisv->actor.shape.rot.z + 8;

    if (thisv->actor.params == BOMB_EXPLOSION) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->explosionCollider.base);
    }

    if (globalCtx->envCtx.adjLight1Color[0] != 0) {
        globalCtx->envCtx.adjLight1Color[0] -= 25;
    }

    if (globalCtx->envCtx.adjLight1Color[1] != 0) {
        globalCtx->envCtx.adjLight1Color[1] -= 25;
    }

    if (globalCtx->envCtx.adjLight1Color[2] != 0) {
        globalCtx->envCtx.adjLight1Color[2] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[0] != 0) {
        globalCtx->envCtx.adjAmbientColor[0] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[1] != 0) {
        globalCtx->envCtx.adjAmbientColor[1] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[2] != 0) {
        globalCtx->envCtx.adjAmbientColor[2] -= 25;
    }

    if (thisv->timer == 0) {
        player = GET_PLAYER(globalCtx);

        if ((player->stateFlags1 & 0x800) && (player->heldActor == &thisv->actor)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            player->stateFlags1 &= ~0x800;
        }

        Actor_Kill(&thisv->actor);
    }
}

void EnBom_Update(Actor* thisx, GlobalContext* globalCtx2) {
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f bomb2Accel = { 0.0f, 0.1f, 0.0f };
    Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f dustAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 dustColor = { 255, 255, 255, 255 };
    s32 pad;
    GlobalContext* globalCtx = globalCtx2;
    EnBom* thisv = (EnBom*)thisx;

    thisx->gravity = -1.2f;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 67) {
        Audio_PlayActorSound2(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
        Actor_SetScale(thisx, 0.01f);
    }

    if ((thisx->xzDistToPlayer >= 20.0f) || (ABS(thisx->yDistToPlayer) >= 80.0f)) {
        thisv->bumpOn = true;
    }

    thisv->actionFunc(thisv, globalCtx);

    Actor_UpdateBgCheckInfo(globalCtx, thisx, 5.0f, 10.0f, 15.0f, 0x1F);

    if (thisx->params == BOMB_BODY) {
        if (thisv->timer < 63) {
            dustAccel.y = 0.2f;

            // spawn spark effect on even frames
            effPos = thisx->world.pos;
            effPos.y += 17.0f;
            if ((globalCtx->gameplayFrames % 2) == 0) {
                EffectSsGSpk_SpawnFuse(globalCtx, thisx, &effPos, &effVelocity, &effAccel);
            }

            Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);

            effPos.y += 3.0f;
            func_8002829C(globalCtx, &effPos, &effVelocity, &dustAccel, &dustColor, &dustColor, 50, 5);
        }

        if ((thisv->bombCollider.base.acFlags & AC_HIT) || ((thisv->bombCollider.base.ocFlags1 & OC1_HIT) &&
                                                           (thisv->bombCollider.base.oc->category == ACTORCAT_ENEMY))) {
            thisv->timer = 0;
            thisx->shape.rot.z = 0;
        } else {
            // if a lit stick touches the bomb, set timer to 100
            // these bombs never have a timer over 70, so thisv isnt used
            if ((thisv->timer > 100) && Player_IsBurningStickInRange(globalCtx, &thisx->world.pos, 30.0f, 50.0f)) {
                thisv->timer = 100;
            }
        }

        dustAccel.y = 0.2f;
        effPos = thisx->world.pos;
        effPos.y += 10.0f;

        // double bomb flash speed and adjust red color at certain times during the countdown
        if ((thisv->timer == 3) || (thisv->timer == 20) || (thisv->timer == 40)) {
            thisx->shape.rot.z = 0;
            thisv->flashSpeedScale >>= 1;
        }

        if ((thisv->timer < 100) && ((thisv->timer & (thisv->flashSpeedScale + 1)) != 0)) {
            Math_SmoothStepToF(&thisv->flashIntensity, 140.0f, 1.0f, 140.0f / thisv->flashSpeedScale, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->flashIntensity, 0.0f, 1.0f, 140.0f / thisv->flashSpeedScale, 0.0f);
        }

        if (thisv->timer < 3) {
            Actor_SetScale(thisx, thisx->scale.x + 0.002f);
        }

        if (thisv->timer == 0) {
            effPos = thisx->world.pos;

            effPos.y += 10.0f;
            if (Actor_HasParent(thisx, globalCtx)) {
                effPos.y += 30.0f;
            }

            EffectSsBomb2_SpawnLayered(globalCtx, &effPos, &effVelocity, &bomb2Accel, 100,
                                       (thisx->shape.rot.z * 6) + 19);

            effPos.y = thisx->floorHeight;
            if (thisx->floorHeight > BGCHECK_Y_MIN) {
                EffectSsBlast_SpawnWhiteShockwave(globalCtx, &effPos, &effVelocity, &effAccel);
            }

            Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_EXPLOSION);

            globalCtx->envCtx.adjLight1Color[0] = globalCtx->envCtx.adjLight1Color[1] =
                globalCtx->envCtx.adjLight1Color[2] = 250;

            globalCtx->envCtx.adjAmbientColor[0] = globalCtx->envCtx.adjAmbientColor[1] =
                globalCtx->envCtx.adjAmbientColor[2] = 250;

            Camera_AddQuake(&globalCtx->mainCamera, 2, 0xB, 8);
            thisx->params = BOMB_EXPLOSION;
            thisv->timer = 10;
            thisx->flags |= ACTOR_FLAG_5;
            EnBom_SetupAction(thisv, EnBom_Explode);
        }
    }

    Actor_SetFocus(thisx, 20.0f);

    if (thisx->params <= BOMB_BODY) {
        Collider_UpdateCylinder(thisx, &thisv->bombCollider);

        // if link is not holding the bomb anymore and bump conditions are met, subscribe to OC
        if (!Actor_HasParent(thisx, globalCtx) && thisv->bumpOn) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bombCollider.base);
        }

        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bombCollider.base);
    }

    if ((thisx->scale.x >= 0.01f) && (thisx->params != BOMB_EXPLOSION)) {
        if (thisx->yDistToWater >= 20.0f) {
            EffectSsDeadSound_SpawnStationary(globalCtx, &thisx->projectedPos, NA_SE_IT_BOMB_UNEXPLOSION, true,
                                              DEADSOUND_REPEAT_MODE_OFF, 10);
            Actor_Kill(thisx);
            return;
        }
        if (thisx->bgCheckFlags & 0x40) {
            thisx->bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(thisx, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

void EnBom_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBom* thisv = (EnBom*)thisx;

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bom.c", 913);

    if (thisx->params == BOMB_BODY) {
        func_80093D18(globalCtx->state.gfxCtx);
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
        func_8002EBCC(thisx, globalCtx, 0);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bom.c", 928),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gBombCapDL);
        Matrix_RotateZYX(0x4000, 0, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bom.c", 934),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->flashIntensity, 0, 40, 255);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, (s16)thisv->flashIntensity, 0, 40, 255);
        gSPDisplayList(POLY_OPA_DISP++, gBombBodyDL);
        Collider_UpdateSpheres(0, &thisv->explosionCollider);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bom.c", 951);
}
