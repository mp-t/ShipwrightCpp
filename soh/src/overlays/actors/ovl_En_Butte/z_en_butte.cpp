/*
 * File: z_en_butte.c
 * Overlay: ovl_En_Butte
 * Description: Butterfly
 */

#include "z_en_butte.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void EnButte_Init(Actor* thisx, GlobalContext* globalCtx);
void EnButte_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnButte_Update(Actor* thisx, GlobalContext* globalCtx);
void EnButte_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnButte_SetupFlyAround(EnButte* thisv);
void EnButte_FlyAround(EnButte* thisv, GlobalContext* globalCtx);
void EnButte_SetupFollowLink(EnButte* thisv);
void EnButte_FollowLink(EnButte* thisv, GlobalContext* globalCtx);
void EnButte_SetupTransformIntoFairy(EnButte* thisv);
void EnButte_TransformIntoFairy(EnButte* thisv, GlobalContext* globalCtx);
void EnButte_SetupWaitToDie(EnButte* thisv);
void EnButte_WaitToDie(EnButte* thisv, GlobalContext* globalCtx);

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    { {
          ELEMTYPE_UNK0,
          { 0x00000000, 0x00, 0x00 },
          { 0xFFCFFFFF, 0x000, 0x00 },
          TOUCH_NONE,
          BUMP_NONE,
          OCELEM_ON,
      },
      { 0, { { 0, 0, 0 }, 5 }, 100 } },
};
static ColliderJntSphInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

const ActorInit En_Butte_InitVars = {
    ACTOR_EN_BUTTE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(EnButte),
    (ActorFunc)EnButte_Init,
    (ActorFunc)EnButte_Destroy,
    (ActorFunc)EnButte_Update,
    (ActorFunc)EnButte_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ s16 minTime;
    /* 0x02 */ s16 maxTime;
    /* 0x04 */ f32 speedXZTarget;
    /* 0x08 */ f32 speedXZScale;
    /* 0x0C */ f32 speedXZStep;
    /* 0x10 */ s16 rotYStep;
} EnButteFlightParams; // size = 0x14

static EnButteFlightParams sFlyAroundParams[] = {
    { 5, 35, 0.0f, 0.1f, 0.5f, 0 },
    { 10, 45, 1.1f, 0.1f, 0.25f, 1000 },
    { 10, 40, 1.5f, 0.1f, 0.3f, 2000 },
};
static EnButteFlightParams sFollowLinkParams[] = {
    { 3, 3, 0.8f, 0.1f, 0.2f, 0 },
    { 10, 20, 2.0f, 0.3f, 1.0f, 0 },
    { 10, 20, 2.4f, 0.3f, 1.0f, 0 },
};

void EnButte_SelectFlightParams(EnButte* thisv, EnButteFlightParams* flightParams) {
    if (thisv->flightParamsIdx == 0) {
        if (Rand_ZeroOne() < 0.6f) {
            thisv->flightParamsIdx = 1;
        } else {
            thisv->flightParamsIdx = 2;
        }
    } else {
        thisv->flightParamsIdx = 0;
    }

    thisv->timer = Rand_S16Offset(flightParams->minTime, flightParams->maxTime);
}

static f32 sTransformationEffectScale = 0.0f;
static s16 sTransformationEffectAlpha = 0;

void EnButte_ResetTransformationEffect(void) {
    sTransformationEffectScale = 0.0f;
    sTransformationEffectAlpha = 0;
}

void EnButte_UpdateTransformationEffect(void) {
    sTransformationEffectScale += 0.003f;
    sTransformationEffectAlpha += 4000;
}

void EnButte_DrawTransformationEffect(EnButte* thisv, GlobalContext* globalCtx) {
    static Vec3f D_809CE3C4 = { 0.0f, 0.0f, -3.0f };
    Vec3f sp5C;
    s32 alpha;
    Vec3s camDir;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_choo.c", 295);

    func_80093C14(globalCtx->state.gfxCtx);

    alpha = Math_SinS(sTransformationEffectAlpha) * 250;
    alpha = CLAMP(alpha, 0, 255);

    Camera_GetCamDir(&camDir, GET_ACTIVE_CAM(globalCtx));
    Matrix_RotateY(camDir.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
    Matrix_RotateX(camDir.x * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(camDir.z * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_MultVec3f(&D_809CE3C4, &sp5C);
    Matrix_SetTranslateRotateYXZ(thisv->actor.focus.pos.x + sp5C.x, thisv->actor.focus.pos.y + sp5C.y,
                                 thisv->actor.focus.pos.z + sp5C.z, &camDir);
    Matrix_Scale(sTransformationEffectScale, sTransformationEffectScale, sTransformationEffectScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_choo.c", 317),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 180, alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 210, 255);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffFlash1DL));

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_choo.c", 326);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 600, ICHAIN_STOP),
};

void EnButte_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnButte* thisv = (EnButte*)thisx;

    if (thisv->actor.params == -1) {
        thisv->actor.params = 0;
    }

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    if ((thisv->actor.params & 1) == 1) {
        thisv->actor.uncullZoneScale = 200.0f;
    }

    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gButterflySkel, &gButterflyAnim, thisv->jointTable, thisv->morphTable,
                   8);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sColliderInit, thisv->colliderItems);
    thisv->actor.colChkInfo.mass = 0;
    thisv->unk_25C = Rand_ZeroOne() * 0xFFFF;
    thisv->unk_25E = Rand_ZeroOne() * 0xFFFF;
    thisv->unk_260 = Rand_ZeroOne() * 0xFFFF;
    Animation_Change(&thisv->skelAnime, &gButterflyAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
    EnButte_SetupFlyAround(thisv);
    thisv->actor.shape.rot.x -= 0x2320;
    thisv->drawSkelAnime = true;
    // "field keep butterfly"
    osSyncPrintf("(field keep 蝶)(%x)(arg_data 0x%04x)\n", thisv, thisv->actor.params);
}

void EnButte_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnButte* thisv = (EnButte*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void func_809CD56C(EnButte* thisv) {
    static f32 D_809CE3E0[] = { 50.0f, 80.0f, 100.0f };
    static f32 D_809CE3EC[] = { 30.0f, 40.0f, 50.0f };

    thisv->actor.shape.yOffset += Math_SinS(thisv->unk_25C) * D_809CE3E0[thisv->flightParamsIdx] +
                                 Math_SinS(thisv->unk_25E) * D_809CE3EC[thisv->flightParamsIdx];
    thisv->actor.shape.yOffset = CLAMP(thisv->actor.shape.yOffset, -2000.0f, 2000.0f);
}

void func_809CD634(EnButte* thisv) {
    static f32 D_809CE3F8[] = { 15.0f, 20.0f, 25.0f };
    static f32 D_809CE404[] = { 7.5f, 10.0f, 12.5f };

    thisv->actor.shape.yOffset += Math_SinS(thisv->unk_25C) * D_809CE3F8[thisv->flightParamsIdx] +
                                 Math_SinS(thisv->unk_25E) * D_809CE404[thisv->flightParamsIdx];
    thisv->actor.shape.yOffset = CLAMP(thisv->actor.shape.yOffset, -500.0f, 500.0f);
}

void EnButte_Turn(EnButte* thisv) {
    s16 target = thisv->actor.world.rot.y + 0x8000;
    s16 diff = target - thisv->actor.shape.rot.y;

    Math_ScaledStepToS(&thisv->actor.shape.rot.y, target, ABS(diff) >> 3);
    thisv->actor.shape.rot.x = (s16)(sinf(thisv->unk_260) * 600.0f) - 0x2320;
}

void EnButte_SetupFlyAround(EnButte* thisv) {
    EnButte_SelectFlightParams(thisv, &sFlyAroundParams[thisv->flightParamsIdx]);
    thisv->actionFunc = EnButte_FlyAround;
}

void EnButte_FlyAround(EnButte* thisv, GlobalContext* globalCtx) {
    EnButteFlightParams* flightParams = &sFlyAroundParams[thisv->flightParamsIdx];
    s16 yaw;
    Player* player = GET_PLAYER(globalCtx);
    f32 distSqFromHome;
    f32 maxDistSqFromHome;
    f32 minAnimSpeed;
    f32 animSpeed;
    s16 rotStep;

    distSqFromHome = Math3D_Dist2DSq(thisv->actor.world.pos.x, thisv->actor.world.pos.z, thisv->actor.home.pos.x,
                                     thisv->actor.home.pos.z);
    func_809CD56C(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, flightParams->speedXZTarget, flightParams->speedXZScale,
                       flightParams->speedXZStep, 0.0f);

    if (thisv->unk_257 == 1) {
        maxDistSqFromHome = SQ(100.0f);
        rotStep = 1000;
    } else {
        maxDistSqFromHome = SQ(35.0f);
        rotStep = 600;
    }

    minAnimSpeed = 0.0f;
    thisv->posYTarget = thisv->actor.home.pos.y;

    if ((thisv->flightParamsIdx != 0) && ((distSqFromHome > maxDistSqFromHome) || (thisv->timer < 4))) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        if (Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, flightParams->rotYStep) == 0) {
            minAnimSpeed = 0.5f;
        }
    } else if ((thisv->unk_257 == 0) && (thisv->actor.child != NULL) && (thisv->actor.child != &thisv->actor)) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.child->world.pos);
        if (Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, rotStep) == 0) {
            minAnimSpeed = 0.3f;
        }
    } else if (thisv->unk_257 == 1) {
        yaw = thisv->actor.yawTowardsPlayer + 0x8000 + (s16)((Rand_ZeroOne() - 0.5f) * 0x6000);
        if (Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, rotStep) == 0) {
            minAnimSpeed = 0.4f;
        }
    } else {
        thisv->actor.world.rot.y += (s16)(sinf(thisv->unk_25C) * 100.0f);
    }

    EnButte_Turn(thisv);

    animSpeed = thisv->actor.speedXZ / 2.0f + Rand_ZeroOne() * 0.2f + (1.0f - Math_SinS(thisv->unk_260)) * 0.15f +
                (1.0f - Math_SinS(thisv->unk_25E)) * 0.3f + minAnimSpeed;
    thisv->skelAnime.playSpeed = CLAMP(animSpeed, 0.2f, 1.5f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        EnButte_SelectFlightParams(thisv, &sFlyAroundParams[thisv->flightParamsIdx]);
    }

    if (((thisv->actor.params & 1) == 1) && (player->heldItemActionParam == PLAYER_AP_STICK) &&
        (thisv->swordDownTimer <= 0) &&
        ((Math3D_Dist2DSq(player->actor.world.pos.x, player->actor.world.pos.z, thisv->actor.home.pos.x,
                          thisv->actor.home.pos.z) < SQ(120.0f)) ||
         (thisv->actor.xzDistToPlayer < 60.0f))) {
        EnButte_SetupFollowLink(thisv);
        thisv->unk_257 = 2;
    } else if (thisv->actor.xzDistToPlayer < 120.0) {
        thisv->unk_257 = 1;
    } else {
        thisv->unk_257 = 0;
    }
}

void EnButte_SetupFollowLink(EnButte* thisv) {
    EnButte_SelectFlightParams(thisv, &sFollowLinkParams[thisv->flightParamsIdx]);
    thisv->actionFunc = EnButte_FollowLink;
}

void EnButte_FollowLink(EnButte* thisv, GlobalContext* globalCtx) {
    static s32 D_809CE410 = 1500;
    EnButteFlightParams* flightParams = &sFollowLinkParams[thisv->flightParamsIdx];
    Player* player = GET_PLAYER(globalCtx);
    f32 distSqFromHome;
    Vec3f swordTip;
    f32 animSpeed;
    f32 minAnimSpeed;
    f32 distSqFromSword;
    s16 yaw;

    func_809CD634(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, flightParams->speedXZTarget, flightParams->speedXZScale,
                       flightParams->speedXZStep, 0.0f);
    minAnimSpeed = 0.0f;

    if ((thisv->flightParamsIdx != 0) && (thisv->timer < 12)) {
        swordTip.x = player->swordInfo[0].tip.x + Math_SinS(player->actor.shape.rot.y) * 10.0f;
        swordTip.y = player->swordInfo[0].tip.y;
        swordTip.z = player->swordInfo[0].tip.z + Math_CosS(player->actor.shape.rot.y) * 10.0f;

        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &swordTip) + (s16)(Rand_ZeroOne() * D_809CE410);
        if (Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, 2000) != 0) {
            if (globalCtx->gameplayFrames % 2) {
                thisv->actor.world.rot.y += (s16)(sinf(thisv->unk_25C) * 60.0f);
            }
        } else {
            minAnimSpeed = 0.3f;
        }
    }

    thisv->posYTarget = MAX(player->actor.world.pos.y + 30.0f, player->swordInfo[0].tip.y);

    EnButte_Turn(thisv);

    animSpeed = thisv->actor.speedXZ / 2.0f + Rand_ZeroOne() * 0.2f + (1.0f - Math_SinS(thisv->unk_260)) * 0.15f +
                (1.0f - Math_SinS(thisv->unk_25E)) * 0.3f + minAnimSpeed;
    thisv->skelAnime.playSpeed = CLAMP(animSpeed, 0.2f, 1.5f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        EnButte_SelectFlightParams(thisv, &sFollowLinkParams[thisv->flightParamsIdx]);
        D_809CE410 = -D_809CE410;
    }

    distSqFromHome = Math3D_Dist2DSq(thisv->actor.world.pos.x, thisv->actor.world.pos.z, thisv->actor.home.pos.x,
                                     thisv->actor.home.pos.z);
    if (!((player->heldItemActionParam == PLAYER_AP_STICK) && (fabsf(player->actor.speedXZ) < 1.8f) &&
          (thisv->swordDownTimer <= 0) && (distSqFromHome < SQ(320.0f)))) {
        EnButte_SetupFlyAround(thisv);
    } else if (distSqFromHome > SQ(240.0f)) {
        distSqFromSword = Math3D_Dist2DSq(player->swordInfo[0].tip.x, player->swordInfo[0].tip.z,
                                          thisv->actor.world.pos.x, thisv->actor.world.pos.z);
        if (distSqFromSword < SQ(60.0f)) {
            EnButte_SetupTransformIntoFairy(thisv);
        }
    }
}

void EnButte_SetupTransformIntoFairy(EnButte* thisv) {
    thisv->timer = 9;
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->skelAnime.playSpeed = 1.0f;
    EnButte_ResetTransformationEffect();
    thisv->actionFunc = EnButte_TransformIntoFairy;
}

void EnButte_TransformIntoFairy(EnButte* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    EnButte_UpdateTransformationEffect();

    if (thisv->timer == 5) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 60, NA_SE_EV_BUTTERFRY_TO_FAIRY);
    } else if (thisv->timer == 4) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ELF, thisv->actor.focus.pos.x, thisv->actor.focus.pos.y,
                    thisv->actor.focus.pos.z, 0, thisv->actor.shape.rot.y, 0, FAIRY_HEAL_TIMED);
        thisv->drawSkelAnime = false;
    } else if (thisv->timer <= 0) {
        EnButte_SetupWaitToDie(thisv);
    }
}

void EnButte_SetupWaitToDie(EnButte* thisv) {
    thisv->timer = 64;
    thisv->actionFunc = EnButte_WaitToDie;
    thisv->actor.draw = NULL;
}

void EnButte_WaitToDie(EnButte* thisv, GlobalContext* globalCtx) {
    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnButte_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnButte* thisv = (EnButte*)thisx;

    if ((thisv->actor.child != NULL) && (thisv->actor.child->update == NULL) && (thisv->actor.child != &thisv->actor)) {
        thisv->actor.child = NULL;
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    }

    thisv->unk_25C += 0x222;
    thisv->unk_25E += 0x1000;
    thisv->unk_260 += 0x600;

    if ((thisv->actor.params & 1) == 1) {
        if (GET_PLAYER(globalCtx)->swordState == 0) {
            if (thisv->swordDownTimer > 0) {
                thisv->swordDownTimer--;
            }
        } else {
            thisv->swordDownTimer = 80;
        }
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.update != NULL) {
        Actor_MoveForward(&thisv->actor);
        Math_StepToF(&thisv->actor.world.pos.y, thisv->posYTarget, 0.6f);
        if (thisv->actor.xyzDistToPlayerSq < 5000.0f) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        Actor_SetFocus(&thisv->actor, thisv->actor.shape.yOffset * thisv->actor.scale.y);
    }
}

void EnButte_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnButte* thisv = (EnButte*)thisx;

    if (thisv->drawSkelAnime) {
        func_80093D18(globalCtx->state.gfxCtx);
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, NULL);
        Collider_UpdateSpheres(0, &thisv->collider);
    }

    if (((thisv->actor.params & 1) == 1) && (thisv->actionFunc == EnButte_TransformIntoFairy)) {
        EnButte_DrawTransformationEffect(thisv, globalCtx);
    }
}
