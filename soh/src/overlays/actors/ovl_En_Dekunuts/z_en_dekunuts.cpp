/*
 * File: z_en_dekunuts.c
 * Overlay: ovl_En_Dekunuts
 * Description: Mad Scrub
 */

#include "z_en_dekunuts.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_dekunuts/object_dekunuts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

#define DEKUNUTS_FLOWER 10

void EnDekunuts_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDekunuts_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDekunuts_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDekunuts_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDekunuts_SetupWait(EnDekunuts* thisv);
void EnDekunuts_Wait(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_LookAround(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_Stand(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_ThrowNut(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_Burrow(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_BeginRun(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_Run(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_Gasp(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_BeDamaged(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_BeStunned(EnDekunuts* thisv, GlobalContext* globalCtx);
void EnDekunuts_Die(EnDekunuts* thisv, GlobalContext* globalCtx);

ActorInit En_Dekunuts_InitVars = {
    ACTOR_EN_DEKUNUTS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DEKUNUTS,
    sizeof(EnDekunuts),
    (ActorFunc)EnDekunuts_Init,
    (ActorFunc)EnDekunuts_Destroy,
    (ActorFunc)EnDekunuts_Update,
    (ActorFunc)EnDekunuts_Draw,
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
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0x01, 0x0012, 0x0020, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4D, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2600, ICHAIN_STOP),
};

void EnDekunuts_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDekunuts* thisv = (EnDekunuts*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    if (thisx->params == DEKUNUTS_FLOWER) {
        thisx->flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
    } else {
        ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDekuNutsSkel, &gDekuNutsStandAnim, thisv->jointTable,
                       thisv->morphTable, 25);
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
        thisv->shotsPerRound = ((thisx->params >> 8) & 0xFF);
        thisx->params &= 0xFF;
        if ((thisv->shotsPerRound == 0xFF) || (thisv->shotsPerRound == 0)) {
            thisv->shotsPerRound = 1;
        }
        EnDekunuts_SetupWait(thisv);
        Actor_SpawnAsChild(&globalCtx->actorCtx, thisx, globalCtx, ACTOR_EN_DEKUNUTS, thisx->world.pos.x,
                           thisx->world.pos.y, thisx->world.pos.z, 0, thisx->world.rot.y, 0, DEKUNUTS_FLOWER);
    }
}

void EnDekunuts_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDekunuts* thisv = (EnDekunuts*)thisx;

    if (thisv->actor.params != DEKUNUTS_FLOWER) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void EnDekunuts_SetupWait(EnDekunuts* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gDekuNutsUpAnim, 0.0f);
    thisv->animFlagAndTimer = Rand_S16Offset(100, 50);
    thisv->collider.dim.height = 5;
    Math_Vec3f_Copy(&thisv->actor.world.pos, &thisv->actor.home.pos);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnDekunuts_Wait;
}

void EnDekunuts_SetupLookAround(EnDekunuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gDekuNutsLookAroundAnim);
    thisv->animFlagAndTimer = 2;
    thisv->actionFunc = EnDekunuts_LookAround;
}

void EnDekunuts_SetupThrowNut(EnDekunuts* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gDekuNutsSpitAnim);
    thisv->animFlagAndTimer = thisv->shotsPerRound;
    thisv->actionFunc = EnDekunuts_ThrowNut;
}

void EnDekunuts_SetupStand(EnDekunuts* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gDekuNutsStandAnim, -3.0f);
    if (thisv->actionFunc == EnDekunuts_ThrowNut) {
        thisv->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        thisv->animFlagAndTimer = 1;
    }
    thisv->actionFunc = EnDekunuts_Stand;
}

void EnDekunuts_SetupBurrow(EnDekunuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDekuNutsBurrowAnim, -5.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DOWN);
    thisv->actionFunc = EnDekunuts_Burrow;
}

void EnDekunuts_SetupBeginRun(EnDekunuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDekuNutsUnburrowAnim, -3.0f);
    thisv->collider.dim.height = 37;
    thisv->actor.colChkInfo.mass = 0x32;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnDekunuts_BeginRun;
}

void EnDekunuts_SetupRun(EnDekunuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gDekuNutsRunAnim);
    thisv->animFlagAndTimer = 2;
    thisv->playWalkSound = false;
    thisv->collider.base.acFlags |= AC_ON;
    thisv->actionFunc = EnDekunuts_Run;
}

void EnDekunuts_SetupGasp(EnDekunuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gDekuNutsGaspAnim);
    thisv->animFlagAndTimer = 3;
    thisv->actor.speedXZ = 0.0f;
    if (thisv->runAwayCount != 0) {
        thisv->runAwayCount--;
    }
    thisv->actionFunc = EnDekunuts_Gasp;
}

void EnDekunuts_SetupBeDamaged(EnDekunuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    if ((thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824) != 0) {
        thisv->actor.world.rot.y = thisv->collider.base.ac->world.rot.y;
    } else {
        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, thisv->collider.base.ac) + 0x8000;
    }
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnDekunuts_BeDamaged;
    thisv->actor.speedXZ = 10.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_CUTBODY);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gDekuNutsDamageAnim));
}

void EnDekunuts_SetupBeStunned(EnDekunuts* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gDekuNutsDamageAnim, -3.0f);
    thisv->animFlagAndTimer = 5;
    thisv->actionFunc = EnDekunuts_BeStunned;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0,
                         Animation_GetLastFrame(&gDekuNutsDamageAnim) * thisv->animFlagAndTimer);
}

void EnDekunuts_SetupDie(EnDekunuts* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gDekuNutsDieAnim);
    thisv->actionFunc = EnDekunuts_Die;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DEAD);
}

void EnDekunuts_Wait(EnDekunuts* thisv, GlobalContext* globalCtx) {
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

    thisv->collider.dim.height = ((CLAMP(thisv->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (thisv->actor.xzDistToPlayer < 120.0f)) {
        EnDekunuts_SetupBurrow(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.xzDistToPlayer < 120.0f) {
            EnDekunuts_SetupBurrow(thisv);
        } else if ((thisv->animFlagAndTimer == 0) && (thisv->actor.xzDistToPlayer > 320.0f)) {
            EnDekunuts_SetupLookAround(thisv);
        } else {
            EnDekunuts_SetupStand(thisv);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((thisv->actor.xzDistToPlayer > 160.0f) && (fabsf(thisv->actor.yDistToPlayer) < 120.0f)) &&
        ((thisv->animFlagAndTimer == 0) || (thisv->actor.xzDistToPlayer < 480.0f))) {
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnDekunuts_LookAround(EnDekunuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if ((thisv->actor.xzDistToPlayer < 120.0f) || (thisv->animFlagAndTimer == 0)) {
        EnDekunuts_SetupBurrow(thisv);
    }
}

void EnDekunuts_Stand(EnDekunuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (!(thisv->animFlagAndTimer & 0x1000)) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if (thisv->animFlagAndTimer == 0x1000) {
        if ((thisv->actor.xzDistToPlayer > 480.0f) || (thisv->actor.xzDistToPlayer < 120.0f)) {
            EnDekunuts_SetupBurrow(thisv);
        } else {
            EnDekunuts_SetupThrowNut(thisv);
        }
    } else if (thisv->animFlagAndTimer == 0) {
        EnDekunuts_SetupThrowNut(thisv);
    }
}

void EnDekunuts_ThrowNut(EnDekunuts* thisv, GlobalContext* globalCtx) {
    Vec3f spawnPos;

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDekunuts_SetupStand(thisv);
    } else if (Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        spawnPos.x = thisv->actor.world.pos.x + (Math_SinS(thisv->actor.shape.rot.y) * 23.0f);
        spawnPos.y = thisv->actor.world.pos.y + 12.0f;
        spawnPos.z = thisv->actor.world.pos.z + (Math_CosS(thisv->actor.shape.rot.y) * 23.0f);
        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 0) != NULL) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
        }
    } else if ((thisv->animFlagAndTimer > 1) && Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gDekuNutsSpitAnim, -3.0f);
        if (thisv->animFlagAndTimer != 0) {
            thisv->animFlagAndTimer--;
        }
    }
}

void EnDekunuts_Burrow(EnDekunuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDekunuts_SetupWait(thisv);
    } else {
        thisv->collider.dim.height = ((3.0f - CLAMP(thisv->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f) + 5.0f;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 4.0f)) {
        thisv->collider.base.acFlags &= ~AC_ON;
    }
    Math_ApproachF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 0.5f, 3.0f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 0.5f, 3.0f);
}

void EnDekunuts_BeginRun(EnDekunuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->runDirection = thisv->actor.yawTowardsPlayer + 0x8000;
        thisv->runAwayCount = 3;
        EnDekunuts_SetupRun(thisv);
    }
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
}

void EnDekunuts_Run(EnDekunuts* thisv, GlobalContext* globalCtx) {
    s16 diffRotInit;
    s16 diffRot;
    f32 phi_f0;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (thisv->playWalkSound) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
        thisv->playWalkSound = false;
    } else {
        thisv->playWalkSound = true;
    }

    Math_StepToF(&thisv->actor.speedXZ, 7.5f, 1.0f);
    if (Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->runDirection, 1, 0xE38, 0xB6) == 0) {
        if (thisv->actor.bgCheckFlags & 0x20) {
            thisv->runDirection = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
        } else if (thisv->actor.bgCheckFlags & 8) {
            thisv->runDirection = thisv->actor.wallYaw;
        } else if (thisv->runAwayCount == 0) {
            diffRotInit = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
            diffRot = diffRotInit - thisv->actor.yawTowardsPlayer;
            if (ABS(diffRot) > 0x2000) {
                thisv->runDirection = diffRotInit;
            } else {
                phi_f0 = (diffRot >= 0.0f) ? 1.0f : -1.0f;
                thisv->runDirection = (phi_f0 * -0x2000) + thisv->actor.yawTowardsPlayer;
            }
        } else {
            thisv->runDirection = thisv->actor.yawTowardsPlayer + 0x8000;
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
    if ((thisv->runAwayCount == 0) && Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) < 20.0f &&
        fabsf(thisv->actor.world.pos.y - thisv->actor.home.pos.y) < 2.0f) {
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        thisv->actor.speedXZ = 0.0f;
        EnDekunuts_SetupBurrow(thisv);
    } else if (thisv->animFlagAndTimer == 0) {
        EnDekunuts_SetupGasp(thisv);
    }
}

void EnDekunuts_Gasp(EnDekunuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (thisv->animFlagAndTimer == 0) {
        EnDekunuts_SetupRun(thisv);
    }
}

void EnDekunuts_BeDamaged(EnDekunuts* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 1.0f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDekunuts_SetupDie(thisv);
    }
}

void EnDekunuts_BeStunned(EnDekunuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        if (thisv->animFlagAndTimer != 0) {
            thisv->animFlagAndTimer--;
        }
        if (thisv->animFlagAndTimer == 0) {
            EnDekunuts_SetupRun(thisv);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_FAINT);
        }
    }
}

void EnDekunuts_Die(EnDekunuts* thisv, GlobalContext* globalCtx) {
    static Vec3f effectVelAndAccel = { 0.0f, 0.0f, 0.0f };

    s32 pad;
    Vec3f effectPos;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        effectPos.x = thisv->actor.world.pos.x;
        effectPos.y = thisv->actor.world.pos.y + 18.0f;
        effectPos.z = thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &effectPos, &effectVelAndAccel, &effectVelAndAccel, 200, 0, 255, 255, 255, 255,
                             150, 150, 150, 1, 13, 1);
        effectPos.y = thisv->actor.world.pos.y + 10.0f;
        EffectSsHahen_SpawnBurst(globalCtx, &effectPos, 3.0f, 0, 12, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x30);
        if (thisv->actor.child != NULL) {
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisv->actor.child, ACTORCAT_PROP);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnDekunuts_ColliderCheck(EnDekunuts* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if (thisv->actor.colChkInfo.mass == 0x32) {
            if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
                if (thisv->actor.colChkInfo.damageEffect != 1) {
                    if (thisv->actor.colChkInfo.damageEffect == 2) {
                        EffectSsFCircle_Spawn(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40, 50);
                    }
                    EnDekunuts_SetupBeDamaged(thisv);
                    if (Actor_ApplyDamage(&thisv->actor) == 0) {
                        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                    }
                } else if (thisv->actionFunc != EnDekunuts_BeStunned) {
                    EnDekunuts_SetupBeStunned(thisv);
                }
            }
        } else {
            EnDekunuts_SetupBeginRun(thisv);
        }
    } else if ((thisv->actor.colChkInfo.mass == MASS_IMMOVABLE) && (globalCtx->actorCtx.unk_02 != 0)) {
        EnDekunuts_SetupBeginRun(thisv);
    }
}

void EnDekunuts_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDekunuts* thisv = (EnDekunuts*)thisx;
    s32 pad;

    if (thisv->actor.params != DEKUNUTS_FLOWER) {
        EnDekunuts_ColliderCheck(thisv, globalCtx);
        thisv->actionFunc(thisv, globalCtx);
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, thisv->collider.dim.radius, thisv->collider.dim.height,
                                0x1D);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        if (thisv->actionFunc == EnDekunuts_Wait) {
            Actor_SetFocus(&thisv->actor, thisv->skelAnime.curFrame);
        } else if (thisv->actionFunc == EnDekunuts_Burrow) {
            Actor_SetFocus(&thisv->actor,
                           20.0f - ((thisv->skelAnime.curFrame * 20.0f) / Animation_GetLastFrame(&gDekuNutsBurrowAnim)));
        } else {
            Actor_SetFocus(&thisv->actor, 20.0f);
        }
    }
}

s32 EnDekunuts_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnDekunuts* thisv = (EnDekunuts*)thisx;
    f32 x;
    f32 y;
    f32 z;
    f32 curFrame;

    if ((limbIndex == 7) && (thisv->actionFunc == EnDekunuts_ThrowNut)) {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            x = 1.0f - (curFrame * 0.0833f);
            z = 1.0f + (curFrame * 0.1167f);
            y = 1.0f + (curFrame * 0.1167f);
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            x = 0.5f + curFrame;
            z = 1.7f - (curFrame * 0.7f);
            y = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            x = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            z = 1.0f;
            y = 1.0f;
        } else {
            return false;
        }
        Matrix_Scale(x, y, z, MTXMODE_APPLY);
    }
    return false;
}

void EnDekunuts_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDekunuts* thisv = (EnDekunuts*)thisx;

    if (thisv->actor.params == DEKUNUTS_FLOWER) {
        Gfx_DrawDListOpa(globalCtx, gDekuNutsFlowerDL);
    } else {
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnDekunuts_OverrideLimbDraw,
                          NULL, thisv);
    }
}
