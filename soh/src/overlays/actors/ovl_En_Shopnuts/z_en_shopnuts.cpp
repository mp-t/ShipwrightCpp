#include "z_en_shopnuts.h"
#include "objects/object_shopnuts/object_shopnuts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnShopnuts_Init(Actor* thisx, GlobalContext* globalCtx);
void EnShopnuts_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnShopnuts_Update(Actor* thisx, GlobalContext* globalCtx);
void EnShopnuts_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnShopnuts_SetupWait(EnShopnuts* thisv);
void EnShopnuts_Wait(EnShopnuts* thisv, GlobalContext* globalCtx);
void EnShopnuts_LookAround(EnShopnuts* thisv, GlobalContext* globalCtx);
void EnShopnuts_Stand(EnShopnuts* thisv, GlobalContext* globalCtx);
void EnShopnuts_ThrowNut(EnShopnuts* thisv, GlobalContext* globalCtx);
void EnShopnuts_Burrow(EnShopnuts* thisv, GlobalContext* globalCtx);
void EnShopnuts_SpawnSalesman(EnShopnuts* thisv, GlobalContext* globalCtx);

const ActorInit En_Shopnuts_InitVars = {
    ACTOR_EN_SHOPNUTS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SHOPNUTS,
    sizeof(EnShopnuts),
    (ActorFunc)EnShopnuts_Init,
    (ActorFunc)EnShopnuts_Destroy,
    (ActorFunc)EnShopnuts_Update,
    (ActorFunc)EnShopnuts_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
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
    { 20, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 20, 40, 0xFE };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4E, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2600, ICHAIN_STOP),
};

void EnShopnuts_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBusinessScrubSkel, &gBusinessScrubAnim_4574, thisv->jointTable,
                       thisv->morphTable, 18);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if (((thisv->actor.params == 0x0002) && (gSaveContext.itemGetInf[0] & 0x800)) ||
        ((thisv->actor.params == 0x0009) && (gSaveContext.infTable[25] & 4)) ||
        ((thisv->actor.params == 0x000A) && (gSaveContext.infTable[25] & 8))) {
        Actor_Kill(&thisv->actor);
    } else {
        EnShopnuts_SetupWait(thisv);
    }
}

void EnShopnuts_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnShopnuts_SetupWait(EnShopnuts* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gBusinessScrubAnim_139C, 0.0f);
    thisv->animFlagAndTimer = Rand_S16Offset(100, 50);
    thisv->collider.dim.height = 5;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnShopnuts_Wait;
}

void EnShopnuts_SetupLookAround(EnShopnuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gBusinessScrubLookAroundAnim);
    thisv->animFlagAndTimer = 2;
    thisv->actionFunc = EnShopnuts_LookAround;
}

void EnShopnuts_SetupThrowNut(EnShopnuts* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gBusinessScrubAnim_1EC);
    thisv->actionFunc = EnShopnuts_ThrowNut;
}

void EnShopnuts_SetupStand(EnShopnuts* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBusinessScrubAnim_4574, -3.0f);
    if (thisv->actionFunc == EnShopnuts_ThrowNut) {
        thisv->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        thisv->animFlagAndTimer = 1;
    }
    thisv->actionFunc = EnShopnuts_Stand;
}

void EnShopnuts_SetupBurrow(EnShopnuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBusinessScrubAnim_39C, -5.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DOWN);
    thisv->actionFunc = EnShopnuts_Burrow;
}

void EnShopnuts_SetupSpawnSalesman(EnShopnuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBusinessScrubRotateAnim, -3.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnShopnuts_SpawnSalesman;
}

void EnShopnuts_Wait(EnShopnuts* thisv, GlobalContext* globalCtx) {
    s32 hasSlowPlaybackSpeed = false;

    if (thisv->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 9.0f)) {
        thisv->collider.base.acFlags |= AC_ON;
    } else if (Animation_OnFrame(&thisv->skelAnime, 8.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
    }

    thisv->collider.dim.height = ((CLAMP(thisv->skelAnime.curFrame, 9.0f, 13.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (thisv->actor.xzDistToPlayer < 120.0f)) {
        EnShopnuts_SetupBurrow(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.xzDistToPlayer < 120.0f) {
            EnShopnuts_SetupBurrow(thisv);
        } else if ((thisv->animFlagAndTimer == 0) && (thisv->actor.xzDistToPlayer > 320.0f)) {
            EnShopnuts_SetupLookAround(thisv);
        } else {
            EnShopnuts_SetupStand(thisv);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((thisv->actor.xzDistToPlayer > 160.0f) && (fabsf(thisv->actor.yDistToPlayer) < 120.0f)) &&
        ((thisv->animFlagAndTimer == 0) || (thisv->actor.xzDistToPlayer < 480.0f))) {
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnShopnuts_LookAround(EnShopnuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if ((thisv->actor.xzDistToPlayer < 120.0f) || (thisv->animFlagAndTimer == 0)) {
        EnShopnuts_SetupBurrow(thisv);
    }
}

void EnShopnuts_Stand(EnShopnuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (!(thisv->animFlagAndTimer & 0x1000)) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if ((thisv->actor.xzDistToPlayer < 120.0f) || (thisv->animFlagAndTimer == 0x1000)) {
        EnShopnuts_SetupBurrow(thisv);
    } else if (thisv->animFlagAndTimer == 0) {
        EnShopnuts_SetupThrowNut(thisv);
    }
}

void EnShopnuts_ThrowNut(EnShopnuts* thisv, GlobalContext* globalCtx) {
    Vec3f spawnPos;

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    if (thisv->actor.xzDistToPlayer < 120.0f) {
        EnShopnuts_SetupBurrow(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        EnShopnuts_SetupStand(thisv);
    } else if (Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        spawnPos.x = thisv->actor.world.pos.x + (Math_SinS(thisv->actor.shape.rot.y) * 23.0f);
        spawnPos.y = thisv->actor.world.pos.y + 12.0f;
        spawnPos.z = thisv->actor.world.pos.z + (Math_CosS(thisv->actor.shape.rot.y) * 23.0f);
        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 2) != NULL) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
        }
    }
}

void EnShopnuts_Burrow(EnShopnuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnShopnuts_SetupWait(thisv);
    } else {
        thisv->collider.dim.height = ((4.0f - CLAMP_MAX(thisv->skelAnime.curFrame, 4.0f)) * 10.0f) + 5.0f;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 4.0f)) {
        thisv->collider.base.acFlags &= ~AC_ON;
    }
}

void EnShopnuts_SpawnSalesman(EnShopnuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_DNS, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                    thisv->actor.world.pos.z, thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z,
                    thisv->actor.params);
        Actor_Kill(&thisv->actor);
    } else {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    }
}

void EnShopnuts_ColliderCheck(EnShopnuts* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        EnShopnuts_SetupSpawnSalesman(thisv);
    } else if (globalCtx->actorCtx.unk_02 != 0) {
        EnShopnuts_SetupSpawnSalesman(thisv);
    }
}

void EnShopnuts_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    EnShopnuts_ColliderCheck(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, thisv->collider.dim.radius, thisv->collider.dim.height, 4);
    if (thisv->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    if (thisv->actionFunc == EnShopnuts_Wait) {
        Actor_SetFocus(&thisv->actor, thisv->skelAnime.curFrame);
    } else if (thisv->actionFunc == EnShopnuts_Burrow) {
        Actor_SetFocus(&thisv->actor,
                       20.0f - ((thisv->skelAnime.curFrame * 20.0f) / Animation_GetLastFrame(&gBusinessScrubAnim_39C)));
    } else {
        Actor_SetFocus(&thisv->actor, 20.0f);
    }
}

s32 EnShopnuts_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    if ((limbIndex == 9) && (thisv->actionFunc == EnShopnuts_ThrowNut)) {
        *dList = NULL;
    }
    return 0;
}

void EnShopnuts_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    f32 curFrame;
    f32 x;
    f32 y;
    f32 z;

    if ((limbIndex == 9) && (thisv->actionFunc == EnShopnuts_ThrowNut)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_shopnuts.c", 682);
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            y = 1.0f - (curFrame * 0.0833f);
            x = z = (curFrame * 0.1167f) + 1.0f;
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            y = 0.5f + curFrame;
            x = z = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            y = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            x = z = 1.0f;
        } else {
            x = y = z = 1.0f;
        }

        Matrix_Scale(x, y, z, MTXMODE_APPLY);
        if (1) {}
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_shopnuts.c", 714),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gBusinessScrubNoseDL);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_shopnuts.c", 717);
    }
}

void EnShopnuts_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnShopnuts* thisv = (EnShopnuts*)thisx;

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnShopnuts_OverrideLimbDraw, EnShopnuts_PostLimbDraw, thisv);
}
