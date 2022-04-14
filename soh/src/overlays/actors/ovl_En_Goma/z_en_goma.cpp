#include "z_en_goma.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_gol/object_gol.h"
#include "overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnGoma_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGoma_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGoma_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGoma_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnGoma_Reset(void);

void EnGoma_Flee(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_EggFallToGround(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Egg(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Hatch(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Hurt(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Die(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Dead(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_PrepareJump(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Land(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Jump(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Stand(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_ChasePlayer(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Stunned(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_LookAtPlayer(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_UpdateHit(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_Debris(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_SpawnHatchDebris(EnGoma* thisv, GlobalContext* globalCtx2);
void EnGoma_BossLimb(EnGoma* thisv, GlobalContext* globalCtx);

void EnGoma_SetupFlee(EnGoma* thisv);
void EnGoma_SetupHatch(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_SetupHurt(EnGoma* thisv, GlobalContext* globalCtx);
void EnGoma_SetupDie(EnGoma* thisv);
void EnGoma_SetupDead(EnGoma* thisv);
void EnGoma_SetupStand(EnGoma* thisv);
void EnGoma_SetupChasePlayer(EnGoma* thisv);
void EnGoma_SetupPrepareJump(EnGoma* thisv);
void EnGoma_SetupLand(EnGoma* thisv);
void EnGoma_SetupJump(EnGoma* thisv);
void EnGoma_SetupStunned(EnGoma* thisv, GlobalContext* globalCtx);

const ActorInit En_Goma_InitVars = {
    ACTOR_BOSS_GOMA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GOL,
    sizeof(EnGoma),
    (ActorFunc)EnGoma_Init,
    (ActorFunc)EnGoma_Destroy,
    (ActorFunc)EnGoma_Update,
    (ActorFunc)EnGoma_Draw,
    (ActorResetFunc)EnGoma_Reset,
};

static ColliderCylinderInit D_80A4B7A0 = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit D_80A4B7CC = {
    {
        COLTYPE_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static u8 sSpawnNum = 0;
static Vec3f sDeadEffectVel = { 0.0f, 0.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 3, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x03, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 20, ICHAIN_STOP),
};

void EnGoma_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGoma* thisv = (EnGoma*)thisx;
    s16 params;

    thisv->eggTimer = Rand_ZeroOne() * 200.0f;
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.01f);
    params = thisv->actor.params;

    if (params >= 100) { // piece of boss goma
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_BOSS);
        thisv->actionFunc = EnGoma_BossLimb;
        thisv->gomaType = ENGOMA_BOSSLIMB;
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
        thisv->actionTimer = thisv->actor.params + 150;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    } else if (params >= 10) { // Debris when hatching
        thisv->actor.gravity = -1.3f;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->actionTimer = 50;
        thisv->gomaType = ENGOMA_HATCH_DEBRIS;
        thisv->eggScale = 1.0f;
        thisv->actor.velocity.y = Rand_ZeroOne() * 5.0f + 5.0f;
        thisv->actionFunc = EnGoma_Debris;
        thisv->actor.speedXZ = Rand_ZeroOne() * 2.3f + 1.5f;
        thisv->actionTimer = 30;
        thisv->actor.scale.x = Rand_ZeroOne() * 0.005f + 0.01f;
        thisv->actor.scale.y = Rand_ZeroOne() * 0.005f + 0.01f;
        thisv->actor.scale.z = Rand_ZeroOne() * 0.005f + 0.01f;
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    } else { // Egg
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
        SkelAnime_Init(globalCtx, &thisv->skelanime, &gObjectGolSkel, &gObjectGolStandAnim, thisv->jointTable,
                       thisv->morphTable, GOMA_LIMB_MAX);
        Animation_PlayLoop(&thisv->skelanime, &gObjectGolStandAnim);
        thisv->actor.colChkInfo.health = 2;

        if (thisv->actor.params < 3) { // Spawned by boss
            thisv->actionFunc = EnGoma_EggFallToGround;
            thisv->invincibilityTimer = 10;
            thisv->actor.speedXZ = 1.5f;
        } else if (thisv->actor.params == 8 || thisv->actor.params == 6) {
            thisv->actionFunc = EnGoma_Egg;
            thisv->spawnNum = sSpawnNum++;
        } else if (thisv->actor.params == 9 || thisv->actor.params == 7) {
            thisv->actionFunc = EnGoma_Egg;
        }

        if (thisv->actor.params >= 8) { // on ceiling
            thisv->eggYOffset = -1500.0f;
        } else {
            thisv->eggYOffset = 1500.0f;
        }

        thisv->gomaType = ENGOMA_EGG;
        thisv->eggScale = 1.0f;
        thisv->eggSquishAngle = Rand_ZeroOne() * 1000.0f;
        thisv->actionTimer = 50;
        Collider_InitCylinder(globalCtx, &thisv->colCyl1);
        Collider_SetCylinder(globalCtx, &thisv->colCyl1, &thisv->actor, &D_80A4B7A0);
        Collider_InitCylinder(globalCtx, &thisv->colCyl2);
        Collider_SetCylinder(globalCtx, &thisv->colCyl2, &thisv->actor, &D_80A4B7CC);
    }
}

void EnGoma_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGoma* thisv = (EnGoma*)thisx;

    if (thisv->actor.params < 10) {
        Collider_DestroyCylinder(globalCtx, &thisv->colCyl1);
        Collider_DestroyCylinder(globalCtx, &thisv->colCyl2);
    }
}

void EnGoma_SetupFlee(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolRunningAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolRunningAnim), ANIMMODE_LOOP, -2.0f);
    thisv->actionFunc = EnGoma_Flee;
    thisv->actionTimer = 20;

    if (thisv->actor.params < 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_DAM2);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_DAM2);
    }
}

void EnGoma_Flee(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachF(&thisv->actor.speedXZ, 20.0f / 3.0f, 0.5f, 2.0f);
    Math_ApproachS(&thisv->actor.world.rot.y,
                   Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) + 0x8000, 3, 2000);
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 2, 3000);

    if (thisv->actionTimer == 0) {
        EnGoma_SetupStand(thisv);
    }
}

void EnGoma_EggFallToGround(EnGoma* thisv, GlobalContext* globalCtx) {
    thisv->actor.gravity = -1.3f;
    thisv->eggSquishAccel += 0.03f;
    thisv->eggSquishAngle += 1.0f + thisv->eggSquishAccel;
    Math_ApproachZeroF(&thisv->eggSquishAmount, 1.0f, 0.005f);
    Math_ApproachF(&thisv->eggYOffset, 1500.0f, 1.0f, 150.0f);

    switch (thisv->hatchState) {
        case 0:
            if (thisv->actor.bgCheckFlags & 1) { // floor
                if (thisv->actor.params < 6) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_EGG1);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_EGG1);
                }

                if (thisv->actor.params > 5) {
                    EnGoma_SetupHatch(thisv, globalCtx);
                } else {
                    thisv->hatchState = 1;
                    thisv->actionTimer = 3;
                    Math_ApproachF(&thisv->eggScale, 1.5f, 0.5f, 1.0f);
                }
            }
            break;

        case 1:
            if (thisv->actionTimer == 0) {
                thisv->hatchState = 2;
                thisv->actionTimer = 3;
                Math_ApproachF(&thisv->eggScale, 0.75f, 0.5f, 1.0f);
                thisv->actor.velocity.y = 5.0f;
                thisv->actor.speedXZ = 2.0f;
            } else {
                Math_ApproachF(&thisv->eggScale, 1.5f, 0.5f, 1.0f);
            }
            break;

        case 2:
            if (thisv->actionTimer == 0) {
                thisv->hatchState = 3;
                thisv->actionTimer = 80;
            } else {
                Math_ApproachF(&thisv->eggScale, 0.75f, 0.5f, 1.0f);
            }
            break;

        case 3:
            Math_ApproachF(&thisv->eggScale, 1.0f, 0.1f, 0.1f);
            if (thisv->actionTimer == 0) {
                EnGoma_SetupHatch(thisv, globalCtx);
            }
            break;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Math_ApproachZeroF(&thisv->actor.speedXZ, 0.2f, 0.05f);
    }
    thisv->eggPitch += (thisv->actor.speedXZ * 0.1f);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnGoma_Egg(EnGoma* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 i;

    thisv->eggSquishAngle += 1.0f;
    Math_ApproachF(&thisv->eggSquishAmount, 0.1f, 1.0f, 0.005f);
    if (fabsf(thisv->actor.world.pos.x - player->actor.world.pos.x) < 100.0f &&
        fabsf(thisv->actor.world.pos.z - player->actor.world.pos.z) < 100.0f) {
        if (++thisv->playerDetectionTimer > 9) {
            thisv->actionFunc = EnGoma_EggFallToGround;
        }
    } else {
        thisv->playerDetectionTimer = 0;
    }

    if (!(thisv->eggTimer & 0xF) && Rand_ZeroOne() < 0.5f) {
        for (i = 0; i < 2; i++) {
            Vec3f vel = { 0.0f, 0.0f, 0.0f };
            Vec3f acc = { 0.0f, -0.5f, 0.0f };
            Vec3f pos;

            pos.x = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.x;
            pos.y = Rand_ZeroFloat(30.0f) + thisv->actor.world.pos.y;
            pos.z = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.z;
            EffectSsHahen_Spawn(globalCtx, &pos, &vel, &acc, 0, (s16)(Rand_ZeroOne() * 5.0f) + 10, HAHEN_OBJECT_DEFAULT,
                                10, NULL);
        }
    }
}

void EnGoma_SetupHatch(EnGoma* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelanime, &gObjectGolJumpHeadbuttAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolJumpHeadbuttAnim), ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = EnGoma_Hatch;
    Actor_SetScale(&thisv->actor, 0.005f);
    thisv->gomaType = ENGOMA_NORMAL;
    thisv->actionTimer = 5;
    thisv->actor.shape.rot.y = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnGoma_SpawnHatchDebris(thisv, globalCtx);
    thisv->eggScale = 1.0f;
    thisv->actor.speedXZ = 0.0f;
}

void EnGoma_Hatch(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    if (thisv->actionTimer == 0) {
        EnGoma_SetupStand(thisv);
    }
}

void EnGoma_SetupHurt(EnGoma* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelanime, &gObjectGolDamagedAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolDamagedAnim), ANIMMODE_ONCE, -2.0f);
    thisv->actionFunc = EnGoma_Hurt;

    if ((s8)thisv->actor.colChkInfo.health <= 0) {
        thisv->actionTimer = 5;
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
    } else {
        thisv->actionTimer = 10;
    }

    thisv->actor.speedXZ = 20.0f;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer + 0x8000;
    if (thisv->actor.params < 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_DAM1);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_DAM1);
    }
}

void EnGoma_Hurt(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->actor.bgCheckFlags & 1) {
        Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 2.0f);
    }

    if (thisv->actionTimer == 0) {
        if ((s8)thisv->actor.colChkInfo.health <= 0) {
            EnGoma_SetupDie(thisv);
        } else {
            EnGoma_SetupFlee(thisv);
        }
    }
}

void EnGoma_SetupDie(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolDeathAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gObjectGolDeathAnim),
                     ANIMMODE_ONCE, -2.0f);
    thisv->actionFunc = EnGoma_Die;
    thisv->actionTimer = 30;

    if (thisv->actor.params < 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_DEAD);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_DEAD);
    }

    thisv->invincibilityTimer = 100;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void EnGoma_Die(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->actor.bgCheckFlags & 1) {
        Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 2.0f);
    }

    if (thisv->actionTimer == 17) {
        if (thisv->actor.params < 6) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_LAND);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_LAND);
        }
    }

    if (thisv->actionTimer == 0) {
        EnGoma_SetupDead(thisv);
    }
}

void EnGoma_SetupDead(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolDeadTwitchingAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolDeadTwitchingAnim), ANIMMODE_LOOP, -2.0f);
    thisv->actionFunc = EnGoma_Dead;
    thisv->actionTimer = 3;
}

void EnGoma_Dead(EnGoma* thisv, GlobalContext* globalCtx) {
    Vec3f accel;
    Vec3f pos;

    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 2.0f);

    if (thisv->actionTimer == 2) {
        pos.x = thisv->actor.world.pos.x;
        pos.y = (thisv->actor.world.pos.y + 5.0f) - 10.0f;
        pos.z = thisv->actor.world.pos.z;
        accel = sDeadEffectVel;
        accel.y = 0.03f;
        EffectSsKFire_Spawn(globalCtx, &pos, &sDeadEffectVel, &accel, 40, 0);
    }

    if (thisv->actionTimer == 0 && Math_SmoothStepToF(&thisv->actor.scale.y, 0.0f, 0.5f, 0.00225f, 0.00001f) <= 0.001f) {
        if (thisv->actor.params < 6) {
            BossGoma* parent = (BossGoma*)thisv->actor.parent;

            parent->childrenGohmaState[thisv->actor.params] = -1;
        }
        Audio_PlaySoundGeneral(NA_SE_EN_EXTINCT, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        Actor_Kill(&thisv->actor);
        Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0x30);
    }
    thisv->visualState = 2;
}

void EnGoma_SetupStand(EnGoma* thisv) {
    f32 lastFrame;

    lastFrame = Animation_GetLastFrame(&gObjectGolStandAnim);
    thisv->actionTimer = Rand_S16Offset(10, 30);
    Animation_Change(&thisv->skelanime, &gObjectGolStandAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = EnGoma_Stand;
    thisv->gomaType = ENGOMA_NORMAL;
}

void EnGoma_SetupChasePlayer(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolRunningAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolRunningAnim), ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = EnGoma_ChasePlayer;
    thisv->actionTimer = Rand_S16Offset(70, 110);
}

void EnGoma_SetupPrepareJump(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolPrepareJumpAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolPrepareJumpAnim), ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = EnGoma_PrepareJump;
    thisv->actionTimer = 30;
}

void EnGoma_PrepareJump(EnGoma* thisv, GlobalContext* globalCtx) {
    s16 targetAngle;

    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);

    targetAngle = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
    Math_ApproachS(&thisv->actor.world.rot.y, targetAngle, 2, 4000);
    Math_ApproachS(&thisv->actor.shape.rot.y, targetAngle, 2, 3000);

    if (thisv->actionTimer == 0) {
        EnGoma_SetupJump(thisv);
    }
    thisv->visualState = 0;
}

void EnGoma_SetupLand(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolLandFromJumpAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolLandFromJumpAnim), ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = EnGoma_Land;
    thisv->actionTimer = 10;
}

void EnGoma_Land(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->actor.bgCheckFlags & 1) {
        Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 2.0f);
    }
    if (thisv->actionTimer == 0) {
        EnGoma_SetupStand(thisv);
    }
}

void EnGoma_SetupJump(EnGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gObjectGolJumpHeadbuttAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gObjectGolJumpHeadbuttAnim), ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = EnGoma_Jump;
    thisv->actor.velocity.y = 8.0f;

    if (thisv->actor.params < 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_CRY);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_CRY);
    }
}

void EnGoma_Jump(EnGoma* thisv, GlobalContext* globalCtx) {
    thisv->actor.flags |= ACTOR_FLAG_24;
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachF(&thisv->actor.speedXZ, 10.0f, 0.5f, 5.0f);

    if (thisv->actor.velocity.y <= 0.0f && (thisv->actor.bgCheckFlags & 1)) {
        EnGoma_SetupLand(thisv);
        if (thisv->actor.params < 6) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_LAND2);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_LAND2);
        }
    }
    thisv->visualState = 0;
}

void EnGoma_Stand(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);
    Math_ApproachS(&thisv->actor.shape.rot.y, Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor), 2,
                   3000);

    if (thisv->actionTimer == 0) {
        EnGoma_SetupChasePlayer(thisv);
    }
}

void EnGoma_ChasePlayer(EnGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, 1.0f) || Animation_OnFrame(&thisv->skelanime, 5.0f)) {
        if (thisv->actor.params < 6) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_WALK);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_WALK);
        }
    }

    Math_ApproachF(&thisv->actor.speedXZ, 10.0f / 3.0f, 0.5f, 2.0f);
    Math_ApproachS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 3, 2000);
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 2, 3000);

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.velocity.y = 0.0f;
    }
    if (thisv->actor.xzDistToPlayer <= 150.0f) {
        EnGoma_SetupPrepareJump(thisv);
    }
}

void EnGoma_SetupStunned(EnGoma* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = EnGoma_Stunned;
    thisv->stunTimer = 100;
    Animation_MorphToLoop(&thisv->skelanime, &gObjectGolStandAnim, -5.0f);
    thisv->actionTimer = (s16)Rand_ZeroFloat(15.0f) + 3;

    if (thisv->actor.params < 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_BJR_FREEZE);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }
}

void EnGoma_Stunned(EnGoma* thisv, GlobalContext* globalCtx) {
    Actor_SetColorFilter(&thisv->actor, 0, 180, 0, 2);
    thisv->visualState = 2;

    if (thisv->actionTimer != 0) {
        SkelAnime_Update(&thisv->skelanime);
    }

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.velocity.y = 0.0f;
        Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);
    }

    if (thisv->stunTimer == 0) {
        EnGoma_SetupStand(thisv);
    } else if (--thisv->stunTimer < 30) {
        if (thisv->stunTimer & 1) {
            thisv->actor.world.pos.x += 1.5f;
            thisv->actor.world.pos.z += 1.5f;
        } else {
            thisv->actor.world.pos.x -= 1.5f;
            thisv->actor.world.pos.z -= 1.5f;
        }
    }
}

void EnGoma_LookAtPlayer(EnGoma* thisv, GlobalContext* globalCtx) {
    s16 eyePitch;
    s16 eyeYaw;

    eyeYaw = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.shape.rot.y;
    eyePitch = Actor_WorldPitchTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.shape.rot.x;

    if (eyeYaw > 6000) {
        eyeYaw = 6000;
    }
    if (eyeYaw < -6000) {
        eyeYaw = -6000;
    }
    Math_ApproachS(&thisv->eyeYaw, eyeYaw, 3, 2000);
    Math_ApproachS(&thisv->eyePitch, eyePitch, 3, 2000);
}

void EnGoma_UpdateHit(EnGoma* thisv, GlobalContext* globalCtx) {
    static Vec3f sShieldKnockbackVel = { 0.0f, 0.0f, 20.0f };
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->hurtTimer != 0) {
        thisv->hurtTimer--;
    } else {
        ColliderInfo* acHitInfo;
        u8 swordDamage;

        if ((thisv->colCyl1.base.atFlags & 2) && thisv->actionFunc == EnGoma_Jump) {
            EnGoma_SetupLand(thisv);
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.velocity.y = 0.0f;
        }

        if ((thisv->colCyl2.base.acFlags & AC_HIT) && (s8)thisv->actor.colChkInfo.health > 0) {
            acHitInfo = thisv->colCyl2.info.acHitInfo;
            thisv->colCyl2.base.acFlags &= ~AC_HIT;

            if (thisv->gomaType == ENGOMA_NORMAL) {
                u32 dmgFlags = acHitInfo->toucher.dmgFlags;

                if (dmgFlags & 0x100000) {
                    if (thisv->actionFunc == EnGoma_Jump) {
                        EnGoma_SetupLand(thisv);
                        thisv->actor.velocity.y = 0.0f;
                        thisv->actor.speedXZ = -5.0f;
                    } else {
                        Matrix_RotateY(player->actor.shape.rot.y / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_NEW);
                        Matrix_MultVec3f(&sShieldKnockbackVel, &thisv->shieldKnockbackVel);
                        thisv->invincibilityTimer = 5;
                    }
                } else if (dmgFlags & 1) { // stun
                    if (thisv->actionFunc != EnGoma_Stunned) {
                        EnGoma_SetupStunned(thisv, globalCtx);
                        thisv->hurtTimer = 8;
                    }
                } else {
                    swordDamage = CollisionCheck_GetSwordDamage(dmgFlags);

                    if (swordDamage) {
                        EffectSsSibuki_SpawnBurst(globalCtx, &thisv->actor.focus.pos);
                    } else {
                        swordDamage = 1;
                    }

                    thisv->actor.colChkInfo.health -= swordDamage;
                    EnGoma_SetupHurt(thisv, globalCtx);
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 5);
                    thisv->hurtTimer = 13;
                }
            } else {
                // die if still an egg
                if (thisv->actor.params <= 5) { //! BossGoma only has 3 children
                    BossGoma* parent = (BossGoma*)thisv->actor.parent;

                    parent->childrenGohmaState[thisv->actor.params] = -1;
                }

                EnGoma_SpawnHatchDebris(thisv, globalCtx);
                Actor_Kill(&thisv->actor);
            }
        }
    }
}

void EnGoma_UpdateEyeEnvColor(EnGoma* thisv) {
    static f32 sTargetEyeEnvColors[][3] = {
        { 255.0f, 0.0f, 50.0f },
        { 17.0f, 255.0f, 50.0f },
        { 0.0f, 170.0f, 50.0f },
    };

    Math_ApproachF(&thisv->eyeEnvColor[0], sTargetEyeEnvColors[0][thisv->visualState], 0.5f, 20.0f);
    Math_ApproachF(&thisv->eyeEnvColor[1], sTargetEyeEnvColors[1][thisv->visualState], 0.5f, 20.0f);
    Math_ApproachF(&thisv->eyeEnvColor[2], sTargetEyeEnvColors[2][thisv->visualState], 0.5f, 20.0f);
}

void EnGoma_SetFloorRot(EnGoma* thisv) {
    f32 nx;
    f32 ny;
    f32 nz;

    if (thisv->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.z);
        Math_ApproachS(&thisv->slopePitch, -Math_FAtan2F(-nz * ny, 1.0f) * (0x8000 / std::numbers::pi_v<float>), 1, 1000);
        Math_ApproachS(&thisv->slopeRoll, Math_FAtan2F(-nx * ny, 1.0f) * (0x8000 / std::numbers::pi_v<float>), 1, 1000);
    }
}

void EnGoma_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGoma* thisv = (EnGoma*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->invincibilityTimer != 0) {
        thisv->invincibilityTimer--;
    }

    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    thisv->actor.world.pos.x = thisv->actor.world.pos.x + thisv->shieldKnockbackVel.x;
    thisv->actor.world.pos.z = thisv->actor.world.pos.z + thisv->shieldKnockbackVel.z;
    Math_ApproachZeroF(&thisv->shieldKnockbackVel.x, 1.0f, 3.0f);
    Math_ApproachZeroF(&thisv->shieldKnockbackVel.z, 1.0f, 3.0f);

    if (thisv->actor.params < 10) {
        thisv->eggTimer++;
        Math_SmoothStepToF(&thisv->actor.scale.x, 0.01f, 0.5f, 0.00075f, 0.000001f);
        Math_SmoothStepToF(&thisv->actor.scale.y, 0.01f, 0.5f, 0.00075f, 0.000001f);
        Math_SmoothStepToF(&thisv->actor.scale.z, 0.01f, 0.5f, 0.00075f, 0.000001f);
        EnGoma_UpdateHit(thisv, globalCtx);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 5);
        EnGoma_SetFloorRot(thisv);
        Actor_SetFocus(&thisv->actor, 20.0f);
        EnGoma_LookAtPlayer(thisv, globalCtx);
        EnGoma_UpdateEyeEnvColor(thisv);
        thisv->visualState = 1;
        if (player->swordState != 0) {
            thisv->colCyl2.dim.radius = 35;
            thisv->colCyl2.dim.height = 35;
            thisv->colCyl2.dim.yShift = 0;
        } else {
            thisv->colCyl2.dim.radius = 15;
            thisv->colCyl2.dim.height = 30;
            thisv->colCyl2.dim.yShift = 10;
        }
        if (thisv->invincibilityTimer == 0) {
            Collider_UpdateCylinder(&thisv->actor, &thisv->colCyl1);
            Collider_UpdateCylinder(&thisv->actor, &thisv->colCyl2);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colCyl1.base);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCyl2.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colCyl1.base);
        }
    }
}

s32 EnGoma_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGoma* thisv = (EnGoma*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_goma.c", 1976);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->eyeEnvColor[0], (s16)thisv->eyeEnvColor[1], (s16)thisv->eyeEnvColor[2],
                   255);

    if (limbIndex == GOMA_LIMB_EYE_IRIS_ROOT1) {
        rot->x += thisv->eyePitch;
        rot->y += thisv->eyeYaw;
    } else if (limbIndex == GOMA_LIMB_BODY && thisv->hurtTimer != 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)(Rand_ZeroOne() * 255.0f), (s16)(Rand_ZeroOne() * 255.0f),
                       (s16)(Rand_ZeroOne() * 255.0f), 255);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_goma.c", 2011);
    return 0;
}

Gfx* EnGoma_NoBackfaceCullingDlist(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dListHead = dList = Graph_Alloc(gfxCtx, sizeof(Gfx) * 4);
    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);
    return dList;
}

void EnGoma_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnGoma* thisv = (EnGoma*)thisx;
    s32 y;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_goma.c", 2040);
    func_80093D18(globalCtx->state.gfxCtx);

    switch (thisv->gomaType) {
        case ENGOMA_NORMAL:
            thisv->actor.naviEnemyId = 0x03;
            Matrix_Translate(thisv->actor.world.pos.x,
                             thisv->actor.world.pos.y + ((thisv->actor.shape.yOffset * thisv->actor.scale.y) +
                                                        globalCtx->mainCamera.skyboxOffset.y),
                             thisv->actor.world.pos.z, MTXMODE_NEW);
            Matrix_RotateX(thisv->slopePitch / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->slopeRoll / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateY(thisv->actor.shape.rot.y / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateX(thisv->actor.shape.rot.x / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->actor.shape.rot.z / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
            SkelAnime_DrawOpa(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, EnGoma_OverrideLimbDraw,
                              NULL, thisv);
            break;

        case ENGOMA_EGG:
            thisv->actor.naviEnemyId = 0x02;
            y = (s16)(sinf((thisv->eggTimer * 5.0f * 3.1415f) / 180.0f) * 31.9f);
            y = (s16)(y + 31);
            gSPSegment(POLY_OPA_DISP++, 0x08, func_80094E78(globalCtx->state.gfxCtx, 0, y));
            Matrix_Push();
            Matrix_Scale(thisv->eggScale, 1.0f / thisv->eggScale, thisv->eggScale, MTXMODE_APPLY);
            Matrix_RotateY(thisv->eggSquishAngle * 0.15f, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->eggSquishAngle * 0.1f, MTXMODE_APPLY);
            Matrix_Scale(0.95f - thisv->eggSquishAmount, thisv->eggSquishAmount + 1.05f, 0.95f - thisv->eggSquishAmount,
                         MTXMODE_APPLY);
            Matrix_RotateZ(-(thisv->eggSquishAngle * 0.1f), MTXMODE_APPLY);
            Matrix_RotateY(-(thisv->eggSquishAngle * 0.15f), MTXMODE_APPLY);
            Matrix_Translate(0.0f, thisv->eggYOffset, 0.0f, MTXMODE_APPLY);
            Matrix_RotateX(thisv->eggPitch, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_goma.c", 2101),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gObjectGolEggDL);
            Matrix_Pop();
            break;

        case ENGOMA_HATCH_DEBRIS:
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_goma.c", 2107),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gBrownFragmentDL);
            break;

        case ENGOMA_BOSSLIMB:
            if (thisv->bossLimbDl != NULL) {
                gSPSegment(POLY_OPA_DISP++, 0x08, EnGoma_NoBackfaceCullingDlist(globalCtx->state.gfxCtx));
                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_goma.c", 2114),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, thisv->bossLimbDl);
            }
            break;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_goma.c", 2119);
}

void EnGoma_Debris(EnGoma* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += 2500;
    thisv->actor.shape.rot.x += 3500;
    if (thisv->actionTimer == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnGoma_SpawnHatchDebris(EnGoma* thisv, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    s16 i;

    if (thisv->actor.params < 6) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_GOMA_BJR_EGG2);
    } else {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_GOMA_EGG2);
    }

    for (i = 0; i < 15; i++) {
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_GOMA,
                           Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.x,
                           Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y + 15.0f,
                           Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.z, 0, Rand_CenteredFloat(0x10000 - 0.01f),
                           0, i + 10);
    }
}

void EnGoma_BossLimb(EnGoma* thisv, GlobalContext* globalCtx) {
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 1.0f, 0.0f };
    Color_RGBA8 primColor = { 255, 255, 255, 255 };
    Color_RGBA8 envColor = { 0, 100, 255, 255 };
    Vec3f pos;

    thisv->actor.world.pos.y -= 5.0f;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 4);
    thisv->actor.world.pos.y += 5.0f;

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.velocity.y = 0.0f;
    } else if (thisv->actionTimer < 250) {
        thisv->actor.shape.rot.y += 2000;
    }

    if (thisv->actionTimer == 250) {
        thisv->actor.gravity = -1.0f;
    }

    if (thisv->actionTimer < 121) {
        if (Math_SmoothStepToF(&thisv->actor.scale.y, 0.0f, 1.0f, 0.00075f, 0) <= 0.001f) {
            Actor_Kill(&thisv->actor);
        }
        thisv->actor.scale.x = thisv->actor.scale.z = thisv->actor.scale.y;
    }

    if (thisv->actionTimer % 8 == 0 && thisv->actionTimer != 0) {
        pos.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
        pos.y = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y;
        pos.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
        func_8002836C(globalCtx, &pos, &vel, &accel, &primColor, &envColor, 500, 10, 10);
    }
}

void EnGoma_Reset(void) {
    sSpawnNum = 0;
}