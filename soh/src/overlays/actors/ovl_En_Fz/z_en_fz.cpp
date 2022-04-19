#include "z_en_fz.h"
#include "objects/object_fz/object_fz.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_10)

void EnFz_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFz_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFz_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFz_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFz_UpdateTargetPos(EnFz* thisv, GlobalContext* globalCtx);

// Stationary Freezard
void EnFz_SetupBlowSmokeStationary(EnFz* thisv);
void EnFz_BlowSmokeStationary(EnFz* thisv, GlobalContext* globalCtx);

// Moving Freezard that can vanish and reappear
void EnFz_Wait(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupAppear(EnFz* thisv);
void EnFz_Appear(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupAimForMove(EnFz* thisv);
void EnFz_AimForMove(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupMoveTowardsPlayer(EnFz* thisv);
void EnFz_MoveTowardsPlayer(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupAimForFreeze(EnFz* thisv);
void EnFz_AimForFreeze(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupBlowSmoke(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_BlowSmoke(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupDisappear(EnFz* thisv);
void EnFz_Disappear(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_SetupWait(EnFz* thisv);

// Killed with fire source
void EnFz_SetupMelt(EnFz* thisv);
void EnFz_Melt(EnFz* thisv, GlobalContext* globalCtx);

// Death
void EnFz_SetupDespawn(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_Despawn(EnFz* thisv, GlobalContext* globalCtx);

// Ice Smoke Effects
void EnFz_SpawnIceSmokeNoFreeze(EnFz* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale);
void EnFz_SpawnIceSmokeFreeze(EnFz* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                              s16 primAlpha, u8 isTimerMod8);
void EnFz_UpdateIceSmoke(EnFz* thisv, GlobalContext* globalCtx);
void EnFz_DrawIceSmoke(EnFz* thisv, GlobalContext* globalCtx);

ActorInit En_Fz_InitVars = {
    ACTOR_EN_FZ,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_FZ,
    sizeof(EnFz),
    (ActorFunc)EnFz_Init,
    (ActorFunc)EnFz_Destroy,
    (ActorFunc)EnFz_Update,
    (ActorFunc)EnFz_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCE0FDB, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x0001F024, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 35, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit3 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x02, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0xF),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(0, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(0, 0xF),
    /* Light arrow   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0xF),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x3B, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnFz_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFz* thisv = (EnFz*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    thisv->actor.colChkInfo.health = 6;

    Collider_InitCylinder(globalCtx, &thisv->collider1);
    Collider_SetCylinderType1(globalCtx, &thisv->collider1, &thisv->actor, &sCylinderInit1);

    Collider_InitCylinder(globalCtx, &thisv->collider2);
    Collider_SetCylinderType1(globalCtx, &thisv->collider2, &thisv->actor, &sCylinderInit2);

    Collider_InitCylinder(globalCtx, &thisv->collider3);
    Collider_SetCylinderType1(globalCtx, &thisv->collider3, &thisv->actor, &sCylinderInit3);

    Actor_SetScale(&thisv->actor, 0.008f);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->unusedTimer1 = 0;
    thisv->unusedCounter = 0;
    thisv->updateBgInfo = true;
    thisv->isMoving = false;
    thisv->isFreezing = false;
    thisv->isActive = true;
    thisv->isDespawning = false;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->posOrigin.y = thisv->actor.world.pos.y;
    thisv->iceSmokeFreezingSpawnHeight = thisv->actor.world.pos.y;
    thisv->posOrigin.x = thisv->actor.world.pos.x;
    thisv->posOrigin.z = thisv->actor.world.pos.z;
    thisv->unusedFloat = 135.0f;

    if (thisv->actor.params < 0) {
        thisv->envAlpha = 0;
        thisv->actor.scale.y = 0.0f;
        EnFz_SetupWait(thisv);
    } else {
        thisv->envAlpha = 255;
        EnFz_SetupBlowSmokeStationary(thisv);
    }

    EnFz_UpdateTargetPos(thisv, globalCtx);
}

void EnFz_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFz* thisv = (EnFz*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider1);
    Collider_DestroyCylinder(globalCtx, &thisv->collider2);
    Collider_DestroyCylinder(globalCtx, &thisv->collider3);
}

void EnFz_UpdateTargetPos(EnFz* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f hitPos;
    Vec3f vec1;
    s32 bgId;
    CollisionPoly* hitPoly;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 20.0f;
    pos.z = thisv->actor.world.pos.z;

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = 0.0f;
    vec1.z = 220.0f;
    Matrix_MultVec3f(&vec1, &thisv->wallHitPos);

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &pos, &thisv->wallHitPos, &hitPos, &hitPoly, true, false, false,
                                true, &bgId)) {
        Math_Vec3f_Copy(&thisv->wallHitPos, &hitPos);
    }

    pos.x = thisv->actor.world.pos.x - thisv->wallHitPos.x;
    pos.z = thisv->actor.world.pos.z - thisv->wallHitPos.z;

    thisv->distToTargetSq = SQ(pos.x) + SQ(pos.z);
}

s32 EnFz_ReachedTarget(EnFz* thisv, Vec3f* vec) {
    if (thisv->distToTargetSq <= (SQ(thisv->actor.world.pos.x - vec->x) + SQ(thisv->actor.world.pos.z - vec->z))) {
        return true;
    } else {
        return false;
    }
}

void EnFz_Damaged(EnFz* thisv, GlobalContext* globalCtx, Vec3f* vec, s32 numEffects, f32 unkFloat) {
    s32 i;
    Vec3f pos;
    Vec3f vel;
    Vec3f accel;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    f32 scale;
    s32 life;

    primColor.r = 155;
    primColor.g = 255;
    primColor.b = 255;
    primColor.a = 255;
    envColor.r = 200;
    envColor.g = 200;
    envColor.b = 200;
    accel.x = accel.z = 0.0f;
    accel.y = -1.0f;

    for (i = 0; i < numEffects; i++) {
        scale = Rand_CenteredFloat(0.3f) + 0.6f;
        life = (s32)Rand_CenteredFloat(5.0f) + 12;
        pos.x = Rand_CenteredFloat(unkFloat) + vec->x;
        pos.y = Rand_ZeroFloat(unkFloat) + vec->y;
        pos.z = Rand_CenteredFloat(unkFloat) + vec->z;
        vel.x = Rand_CenteredFloat(10.0f);
        vel.y = Rand_ZeroFloat(10.0f) + 2.0f;
        vel.z = Rand_CenteredFloat(10.0f);
        EffectSsEnIce_Spawn(globalCtx, &pos, scale, &vel, &accel, &primColor, &envColor, life);
    }

    CollisionCheck_SpawnShieldParticles(globalCtx, vec);
}

void EnFz_SpawnIceSmokeHiddenState(EnFz* thisv) {
}

// Fully grown
void EnFz_SpawnIceSmokeGrowingState(EnFz* thisv) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((thisv->counter % 16) == 0) {
        pos.x = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.x;
        pos.y = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.y + 30.0f;
        pos.z = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.z;
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        EnFz_SpawnIceSmokeNoFreeze(thisv, &pos, &velocity, &accel, Rand_ZeroFloat(7.5f) + 15.0f);
    }
}

// (2) Growing or Shrinking to/from hiding or (3) melting from fire
void EnFz_SpawnIceSmokeActiveState(EnFz* thisv) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((thisv->counter % 4) == 0) {
        pos.x = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.x;
        pos.y = thisv->iceSmokeFreezingSpawnHeight;
        pos.z = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.z;
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        EnFz_SpawnIceSmokeNoFreeze(thisv, &pos, &velocity, &accel, Rand_ZeroFloat(7.5f) + 15.0f);
    }
}

void EnFz_ApplyDamage(EnFz* thisv, GlobalContext* globalCtx) {
    Vec3f vec;

    if (thisv->isMoving &&
        ((thisv->actor.bgCheckFlags & 8) ||
         (Actor_TestFloorInDirection(&thisv->actor, globalCtx, 60.0f, thisv->actor.world.rot.y) == 0))) {
        thisv->actor.bgCheckFlags &= ~8;
        thisv->isMoving = false;
        thisv->speedXZ = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->isFreezing) {
        if ((thisv->actor.params < 0) && (thisv->collider1.base.atFlags & 2)) {
            thisv->isMoving = false;
            thisv->collider1.base.acFlags &= ~2;
            thisv->actor.speedXZ = thisv->speedXZ = 0.0f;
            thisv->timer = 10;
            EnFz_SetupDisappear(thisv);
        } else if (thisv->collider2.base.acFlags & 0x80) {
            thisv->collider2.base.acFlags &= ~0x80;
            thisv->collider1.base.acFlags &= ~2;
        } else if (thisv->collider1.base.acFlags & 2) {
            thisv->collider1.base.acFlags &= ~2;
            if (thisv->actor.colChkInfo.damageEffect != 2) {
                if (thisv->actor.colChkInfo.damageEffect == 0xF) {
                    Actor_ApplyDamage(&thisv->actor);
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 8);
                    if (thisv->actor.colChkInfo.health) {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FREEZAD_DAMAGE);
                        vec.x = thisv->actor.world.pos.x;
                        vec.y = thisv->actor.world.pos.y;
                        vec.z = thisv->actor.world.pos.z;
                        EnFz_Damaged(thisv, globalCtx, &vec, 10, 0.0f);
                        thisv->unusedCounter++;
                    } else {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FREEZAD_DEAD);
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_ICE_BROKEN);
                        vec.x = thisv->actor.world.pos.x;
                        vec.y = thisv->actor.world.pos.y;
                        vec.z = thisv->actor.world.pos.z;
                        EnFz_Damaged(thisv, globalCtx, &vec, 30, 10.0f);
                        EnFz_SetupDespawn(thisv, globalCtx);
                    }
                }
            } else {
                Actor_ApplyDamage(&thisv->actor);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 8);
                if (thisv->actor.colChkInfo.health == 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FREEZAD_DEAD);
                    EnFz_SetupMelt(thisv);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FREEZAD_DAMAGE);
                }
            }
        }
    }
}

void EnFz_SetYawTowardsPlayer(EnFz* thisv) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 10, 2000, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
}

void EnFz_SetupDisappear(EnFz* thisv) {
    thisv->state = 2;
    thisv->isFreezing = false;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = EnFz_Disappear;
}

void EnFz_Disappear(EnFz* thisv, GlobalContext* globalCtx) {
    thisv->envAlpha -= 16;

    if (thisv->envAlpha > 255) {
        thisv->envAlpha = 0;
    }

    if (Math_SmoothStepToF(&thisv->actor.scale.y, 0.0f, 1.0f, 0.0005f, 0) == 0.0f) {
        EnFz_SetupWait(thisv);
    }
}

void EnFz_SetupWait(EnFz* thisv) {
    thisv->state = 0;
    thisv->unusedNum2 = 0;
    thisv->unusedNum1 = 0;
    thisv->timer = 100;
    thisv->actionFunc = EnFz_Wait;
    thisv->actor.world.pos.x = thisv->posOrigin.x;
    thisv->actor.world.pos.y = thisv->posOrigin.y;
    thisv->actor.world.pos.z = thisv->posOrigin.z;
}

void EnFz_Wait(EnFz* thisv, GlobalContext* globalCtx) {
    if ((thisv->timer == 0) && (thisv->actor.xzDistToPlayer < 400.0f)) {
        EnFz_SetupAppear(thisv);
    }
}

void EnFz_SetupAppear(EnFz* thisv) {
    thisv->state = 2;
    thisv->timer = 20;
    thisv->unusedNum2 = 4000;
    thisv->actionFunc = EnFz_Appear;
}

void EnFz_Appear(EnFz* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        thisv->envAlpha += 8;
        if (thisv->envAlpha > 255) {
            thisv->envAlpha = 255;
        }

        if (Math_SmoothStepToF(&thisv->actor.scale.y, 0.008f, 1.0f, 0.0005f, 0.0f) == 0.0f) {
            EnFz_SetupAimForMove(thisv);
        }
    }
}

void EnFz_SetupAimForMove(EnFz* thisv) {
    thisv->state = 1;
    thisv->timer = 40;
    thisv->updateBgInfo = true;
    thisv->isFreezing = true;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = EnFz_AimForMove;
    thisv->actor.gravity = -1.0f;
}

void EnFz_AimForMove(EnFz* thisv, GlobalContext* globalCtx) {
    EnFz_SetYawTowardsPlayer(thisv);

    if (thisv->timer == 0) {
        EnFz_SetupMoveTowardsPlayer(thisv);
    }
}

void EnFz_SetupMoveTowardsPlayer(EnFz* thisv) {
    thisv->state = 1;
    thisv->isMoving = true;
    thisv->timer = 100;
    thisv->actionFunc = EnFz_MoveTowardsPlayer;
    thisv->speedXZ = 4.0f;
}

void EnFz_MoveTowardsPlayer(EnFz* thisv, GlobalContext* globalCtx) {
    if ((thisv->timer == 0) || !thisv->isMoving) {
        EnFz_SetupAimForFreeze(thisv);
    }
}

void EnFz_SetupAimForFreeze(EnFz* thisv) {
    thisv->state = 1;
    thisv->timer = 40;
    thisv->actionFunc = EnFz_AimForFreeze;
    thisv->speedXZ = 0.0f;
    thisv->actor.speedXZ = 0.0f;
}

void EnFz_AimForFreeze(EnFz* thisv, GlobalContext* globalCtx) {
    EnFz_SetYawTowardsPlayer(thisv);

    if (thisv->timer == 0) {
        EnFz_SetupBlowSmoke(thisv, globalCtx);
    }
}

void EnFz_SetupBlowSmoke(EnFz* thisv, GlobalContext* globalCtx) {
    thisv->state = 1;
    thisv->timer = 80;
    thisv->actionFunc = EnFz_BlowSmoke;
    EnFz_UpdateTargetPos(thisv, globalCtx);
}

void EnFz_BlowSmoke(EnFz* thisv, GlobalContext* globalCtx) {
    Vec3f vec1;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 isTimerMod8;
    s16 primAlpha;

    if (thisv->timer == 0) {
        EnFz_SetupDisappear(thisv);
    } else if (thisv->timer >= 11) {
        isTimerMod8 = false;
        primAlpha = 150;
        func_8002F974(&thisv->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

        if ((thisv->timer - 10) < 16) { // t < 26
            primAlpha = (thisv->timer * 10) - 100;
        }

        accel.x = accel.z = 0.0f;
        accel.y = 0.6f;

        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + 20.0f;
        pos.z = thisv->actor.world.pos.z;

        Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);

        vec1.x = 0.0f;
        vec1.y = -2.0f;
        vec1.z = 20.0f; // xz velocity

        Matrix_MultVec3f(&vec1, &velocity);

        if ((thisv->timer % 8) == 0) {
            isTimerMod8 = true;
        }

        EnFz_SpawnIceSmokeFreeze(thisv, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, isTimerMod8);

        pos.x += (velocity.x * 0.5f);
        pos.y += (velocity.y * 0.5f);
        pos.z += (velocity.z * 0.5f);

        EnFz_SpawnIceSmokeFreeze(thisv, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
    }
}

void EnFz_SetupDespawn(EnFz* thisv, GlobalContext* globalCtx) {
    thisv->state = 0;
    thisv->updateBgInfo = true;
    thisv->isFreezing = false;
    thisv->isDespawning = true;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->isActive = false;
    thisv->timer = 60;
    thisv->speedXZ = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x60);
    thisv->actionFunc = EnFz_Despawn;
}

void EnFz_Despawn(EnFz* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnFz_SetupMelt(EnFz* thisv) {
    thisv->state = 3;
    thisv->isFreezing = false;
    thisv->isDespawning = true;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = EnFz_Melt;
    thisv->actor.speedXZ = 0.0f;
    thisv->speedXZ = 0.0f;
}

void EnFz_Melt(EnFz* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.scale.y, 0.0006f, 0.0002f);

    if (thisv->actor.scale.y < 0.006f) {
        thisv->actor.scale.x += 0.0004f;
        thisv->actor.scale.z += 0.0004f;
    }

    if (thisv->actor.scale.y < 0.004f) {
        thisv->envAlpha -= 8;
        if (thisv->envAlpha > 255) {
            thisv->envAlpha = 0;
        }
    }

    if (thisv->envAlpha == 0) {
        EnFz_SetupDespawn(thisv, globalCtx);
    }
}

void EnFz_SetupBlowSmokeStationary(EnFz* thisv) {
    thisv->state = 1;
    thisv->timer = 40;
    thisv->updateBgInfo = true;
    thisv->isFreezing = true;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = EnFz_BlowSmokeStationary;
    thisv->actor.gravity = -1.0f;
}

void EnFz_BlowSmokeStationary(EnFz* thisv, GlobalContext* globalCtx) {
    Vec3f vec1;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 isTimerMod8;
    s16 primAlpha;

    if (thisv->counter & 0xC0) {
        EnFz_SetYawTowardsPlayer(thisv);
        EnFz_UpdateTargetPos(thisv, globalCtx);
    } else {
        isTimerMod8 = false;
        primAlpha = 150;
        func_8002F974(&thisv->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

        if ((thisv->counter & 0x3F) >= 48) {
            primAlpha = 630 - ((thisv->counter & 0x3F) * 10);
        }

        accel.x = accel.z = 0.0f;
        accel.y = 0.6f;

        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + 20.0f;
        pos.z = thisv->actor.world.pos.z;

        Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);

        vec1.x = 0.0f;
        vec1.y = -2.0f;
        vec1.z = 20.0f;

        Matrix_MultVec3f(&vec1, &velocity);

        if ((thisv->counter % 8) == 0) {
            isTimerMod8 = true;
        }

        EnFz_SpawnIceSmokeFreeze(thisv, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, isTimerMod8);

        pos.x += (velocity.x * 0.5f);
        pos.y += (velocity.y * 0.5f);
        pos.z += (velocity.z * 0.5f);

        EnFz_SpawnIceSmokeFreeze(thisv, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
    }
}

static EnFzSpawnIceSmokeFunc iceSmokeSpawnFuncs[] = {
    EnFz_SpawnIceSmokeHiddenState,
    EnFz_SpawnIceSmokeGrowingState,
    EnFz_SpawnIceSmokeActiveState,
    EnFz_SpawnIceSmokeActiveState,
};

void EnFz_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFz* thisv = (EnFz*)thisx;
    s32 pad;

    thisv->counter++;

    if (thisv->unusedTimer1 != 0) {
        thisv->unusedTimer1--;
    }

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->unusedTimer2 != 0) {
        thisv->unusedTimer2--;
    }

    Actor_SetFocus(&thisv->actor, 50.0f);
    EnFz_ApplyDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->isDespawning == false) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider1);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider2);
        if (thisv->isFreezing) {
            if (thisv->actor.colorFilterTimer == 0) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
            }
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        }
    }

    Math_StepToF(&thisv->actor.speedXZ, thisv->speedXZ, 0.2f);
    Actor_MoveForward(&thisv->actor);

    if (thisv->updateBgInfo) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 20.0f, 5);
    }

    iceSmokeSpawnFuncs[thisv->state](thisv);
    EnFz_UpdateIceSmoke(thisv, globalCtx);
}

void EnFz_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const Gfx* displayLists[] = {
        gFreezardIntactDL,              // Body fully intact           (5 or 6 health)
        gFreezardTopRightHornChippedDL, // Top right horn chipped off  (from Freezards perspective)   (3 or 4 health)
        gFreezardHeadChippedDL,         // Entire head chipped off     (1 or 2 health)
    };
    EnFz* thisv = (EnFz*)thisx;
    s32 pad;
    s32 index;

    index = (6 - thisv->actor.colChkInfo.health) >> 1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fz.c", 1167);

    if (1) {}

    if (thisv->actor.colChkInfo.health == 0) {
        index = 2;
    }

    if (thisv->isActive) {
        func_8002ED80(&thisv->actor, globalCtx, 0);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, globalCtx->state.frames & 0x7F, 32, 32, 1, 0,
                                    (2 * globalCtx->state.frames) & 0x7F, 32, 32));
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fz.c", 1183),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, ENVIRONMENT, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, 155, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, thisv->envAlpha);
        gSPDisplayList(POLY_XLU_DISP++, displayLists[index]);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fz.c", 1200);
    EnFz_DrawIceSmoke(thisv, globalCtx);
}

void EnFz_SpawnIceSmokeNoFreeze(EnFz* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale) {
    EnFzEffectSsIceSmoke* iceSmoke = thisv->iceSmoke;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->iceSmoke); i++) {
        if (iceSmoke->type == 0) {
            iceSmoke->type = 1;
            iceSmoke->pos = *pos;
            iceSmoke->velocity = *velocity;
            iceSmoke->accel = *accel;
            iceSmoke->primAlphaState = 0;
            iceSmoke->xyScale = xyScale / 1000.0f;
            iceSmoke->primAlpha = 0;
            iceSmoke->timer = 0;
            break;
        }

        iceSmoke++;
    }
}

void EnFz_SpawnIceSmokeFreeze(EnFz* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                              s16 primAlpha, u8 isTimerMod8) {
    EnFzEffectSsIceSmoke* iceSmoke = thisv->iceSmoke;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->iceSmoke); i++) {
        if (iceSmoke->type == 0) {
            iceSmoke->type = 2;
            iceSmoke->pos = *pos;
            iceSmoke->velocity = *velocity;
            iceSmoke->accel = *accel;
            iceSmoke->primAlphaState = 0;
            iceSmoke->xyScale = xyScale / 1000.0f;
            iceSmoke->xyScaleTarget = xyScaleTarget / 1000.0f;
            iceSmoke->primAlpha = primAlpha;
            iceSmoke->timer = 0;
            iceSmoke->isTimerMod8 = isTimerMod8;
            break;
        }

        iceSmoke++;
    }
}

void EnFz_UpdateIceSmoke(EnFz* thisv, GlobalContext* globalCtx) {
    EnFzEffectSsIceSmoke* iceSmoke = thisv->iceSmoke;
    s16 i;
    Vec3f pos;

    for (i = 0; i < ARRAY_COUNT(thisv->iceSmoke); i++) {
        if (iceSmoke->type) {
            iceSmoke->pos.x += iceSmoke->velocity.x;
            iceSmoke->pos.y += iceSmoke->velocity.y;
            iceSmoke->pos.z += iceSmoke->velocity.z;
            iceSmoke->timer++;
            iceSmoke->velocity.x += iceSmoke->accel.x;
            iceSmoke->velocity.y += iceSmoke->accel.y;
            iceSmoke->velocity.z += iceSmoke->accel.z;
            if (iceSmoke->type == 1) {
                if (iceSmoke->primAlphaState == 0) { // Becoming more opaque
                    iceSmoke->primAlpha += 10;
                    if (iceSmoke->primAlpha >= 100) {
                        iceSmoke->primAlphaState++;
                    }
                } else { // Becoming more transparent
                    iceSmoke->primAlpha -= 3;
                    if (iceSmoke->primAlpha <= 0) {
                        iceSmoke->primAlpha = 0;
                        iceSmoke->type = 0;
                    }
                }
            } else if (iceSmoke->type == 2) { // Freezing
                Math_ApproachF(&iceSmoke->xyScale, iceSmoke->xyScaleTarget, 0.1f, iceSmoke->xyScaleTarget / 10.0f);
                if (iceSmoke->primAlphaState == 0) { // Becoming more opaque
                    if (iceSmoke->timer >= 7) {
                        iceSmoke->primAlphaState++;
                    }
                } else { // Becoming more transparent, slows down
                    iceSmoke->accel.y = 2.0f;
                    iceSmoke->primAlpha -= 17;
                    iceSmoke->velocity.x *= 0.75f;
                    iceSmoke->velocity.z *= 0.75f;
                    if (iceSmoke->primAlpha <= 0) {
                        iceSmoke->primAlpha = 0;
                        iceSmoke->type = 0;
                    }
                }

                if ((thisv->unusedTimer2 == 0) && (iceSmoke->primAlpha >= 101) && iceSmoke->isTimerMod8) {
                    thisv->collider3.dim.pos.x = (s16)iceSmoke->pos.x;
                    thisv->collider3.dim.pos.y = (s16)iceSmoke->pos.y;
                    thisv->collider3.dim.pos.z = (s16)iceSmoke->pos.z;
                    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider3.base);
                }

                pos.x = iceSmoke->pos.x;
                pos.y = iceSmoke->pos.y + 10.0f;
                pos.z = iceSmoke->pos.z;

                if ((iceSmoke->primAlphaState != 2) && EnFz_ReachedTarget(thisv, &pos)) {
                    iceSmoke->primAlphaState = 2;
                    iceSmoke->velocity.x = 0.0f;
                    iceSmoke->velocity.z = 0.0f;
                }
            }
        }
        iceSmoke++;
    }
}

void EnFz_DrawIceSmoke(EnFz* thisv, GlobalContext* globalCtx) {
    EnFzEffectSsIceSmoke* iceSmoke = thisv->iceSmoke;
    s16 i;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    u8 texLoaded = false;

    OPEN_DISPS(gfxCtx, "../z_en_fz.c", 1384);

    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->iceSmoke); i++) {
        if (iceSmoke->type > 0) {
            gDPPipeSync(POLY_XLU_DISP++);

            if (!texLoaded) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamStartDL));
                texLoaded++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, iceSmoke->primAlpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 3 * (iceSmoke->timer + (3 * i)),
                                        15 * (iceSmoke->timer + (3 * i)), 32, 64, 1, 0, 0, 32, 32));
            Matrix_Translate(iceSmoke->pos.x, iceSmoke->pos.y, iceSmoke->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(iceSmoke->xyScale, iceSmoke->xyScale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_fz.c", 1424),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamDL));
        }

        iceSmoke++;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_fz.c", 1430);
}
