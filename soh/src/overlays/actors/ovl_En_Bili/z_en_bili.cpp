/*
 * File: z_en_bili.c
 * Overlay: ovl_En_Bili
 * Description: Biri (small jellyfish-like enemy)
 */

#include "z_en_bili.h"
#include "objects/object_bl/object_bl.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_12 | ACTOR_FLAG_14)

void EnBili_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBili_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBili_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBili_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBili_SetupFloatIdle(EnBili* thisv);
void EnBili_SetupSpawnedFlyApart(EnBili* thisv);
void EnBili_FloatIdle(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_SpawnedFlyApart(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_DischargeLightning(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Climb(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_ApproachPlayer(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_SetNewHomeHeight(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Recoil(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Burnt(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Die(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Stunned(EnBili* thisv, GlobalContext* globalCtx);
void EnBili_Frozen(EnBili* thisv, GlobalContext* globalCtx);

const ActorInit En_Bili_InitVars = {
    ACTOR_EN_BILI,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BL,
    sizeof(EnBili),
    (ActorFunc)EnBili_Init,
    (ActorFunc)EnBili_Destroy,
    (ActorFunc)EnBili_Update,
    (ActorFunc)EnBili_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT8,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 9, 28, -20, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 9, 28, -20, 30 };

typedef enum {
    /* 0x0 */ BIRI_DMGEFF_NONE,
    /* 0x1 */ BIRI_DMGEFF_DEKUNUT,
    /* 0x2 */ BIRI_DMGEFF_FIRE,
    /* 0x3 */ BIRI_DMGEFF_ICE,
    /* 0xE */ BIRI_DMGEFF_SLINGSHOT = 0xE,
    /* 0xF */ BIRI_DMGEFF_SWORD
} BiriDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, BIRI_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(0, BIRI_DMGEFF_SLINGSHOT),
    /* Explosive     */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Normal arrow  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, BIRI_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, BIRI_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, BIRI_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, BIRI_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x17, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_STOP),
};

void EnBili_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBili* thisv = (EnBili*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 17.0f);
    thisv->actor.shape.shadowAlpha = 155;
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gBiriSkel, &gBiriDefaultAnim, thisv->jointTable, thisv->morphTable,
                   EN_BILI_LIMB_MAX);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->playFlySound = false;

    if (thisv->actor.params == EN_BILI_TYPE_NORMAL) {
        EnBili_SetupFloatIdle(thisv);
    } else {
        EnBili_SetupSpawnedFlyApart(thisv);
    }
}

void EnBili_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBili* thisv = (EnBili*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

// Setup Action Functions

void EnBili_SetupFloatIdle(EnBili* thisv) {
    thisv->actor.speedXZ = 0.7f;
    thisv->collider.info.bumper.effect = 1; // Shock?
    thisv->timer = 32;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->collider.base.acFlags |= AC_ON;
    thisv->actionFunc = EnBili_FloatIdle;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
}

/**
 * Separates the Biri spawned by a dying EnVali.
 */
void EnBili_SetupSpawnedFlyApart(EnBili* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gBiriDefaultAnim);
    thisv->timer = 25;
    thisv->actor.velocity.y = 6.0f;
    thisv->actor.gravity = -0.3f;
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->actionFunc = EnBili_SpawnedFlyApart;
    thisv->actor.speedXZ = 3.0f;
}

/**
 * Used for both touching player/player's shield and being hit with sword. What to do next is determined by params.
 */
void EnBili_SetupDischargeLightning(EnBili* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gBiriDischargeLightningAnim);
    thisv->timer = 10;
    thisv->actionFunc = EnBili_DischargeLightning;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = -1.0f;
}

void EnBili_SetupClimb(EnBili* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gBiriClimbAnim);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->actionFunc = EnBili_Climb;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
}

void EnBili_SetupApproachPlayer(EnBili* thisv) {
    thisv->actor.speedXZ = 1.2f;
    thisv->actionFunc = EnBili_ApproachPlayer;
}

void EnBili_SetupSetNewHomeHeight(EnBili* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gBiriDefaultAnim);
    thisv->timer = 96;
    thisv->actor.speedXZ = 0.9f;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->actionFunc = EnBili_SetNewHomeHeight;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
}

void EnBili_SetupRecoil(EnBili* thisv) {
    if (thisv->skelAnime.animation != &gBiriDefaultAnim) {
        Animation_PlayLoop(&thisv->skelAnime, &gBiriDefaultAnim);
    }

    thisv->actor.world.rot.y = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->collider.base.ac->prevPos) + 0x8000;
    thisv->actor.world.rot.x = Actor_WorldPitchTowardPoint(&thisv->actor, &thisv->collider.base.ac->prevPos);
    thisv->actionFunc = EnBili_Recoil;
    thisv->actor.speedXZ = 5.0f;
}

/**
 * Used for both fire damage and generic damage
 */
void EnBili_SetupBurnt(EnBili* thisv) {
    if (thisv->actionFunc == EnBili_Climb) {
        Animation_PlayLoop(&thisv->skelAnime, &gBiriDefaultAnim);
    }

    thisv->timer = 20;
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0x2000, 0x14);
    thisv->actionFunc = EnBili_Burnt;
}

void EnBili_SetupDie(EnBili* thisv) {
    thisv->timer = 18;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = EnBili_Die;
    thisv->actor.speedXZ = 0.0f;
}

/**
 * Falls to ground
 */
void EnBili_SetupStunned(EnBili* thisv) {
    thisv->timer = 80;
    thisv->collider.info.bumper.effect = 0;
    thisv->actor.gravity = -1.0f;
    thisv->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 0x96, 0x2000, 0x50);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->actionFunc = EnBili_Stunned;
}

void EnBili_SetupFrozen(EnBili* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f effectPos;

    if (!(thisv->actor.flags & ACTOR_FLAG_15)) {
        thisv->actor.gravity = -1.0f;
    }

    thisv->actor.velocity.y = 0.0f;
    effectPos.y = thisv->actor.world.pos.y - 15.0f;

    for (i = 0; i < 8; i++) {

        effectPos.x = thisv->actor.world.pos.x + ((i & 1) ? 7.0f : -7.0f);
        effectPos.y += 2.5f;
        effectPos.z = thisv->actor.world.pos.z + ((i & 4) ? 7.0f : -7.0f);

        EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255,
                                       (Rand_ZeroOne() * 0.2f) + 0.7f);
    }

    thisv->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 0x96, 0x2000, 0xA);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->timer = 300;
    thisv->actionFunc = EnBili_Frozen;
}

// Miscellaneous

/**
 * Changes the texture displayed on the oral arms limb using the current frame.
 */
void EnBili_UpdateTentaclesIndex(EnBili* thisv) {
    s16 curFrame = thisv->skelAnime.curFrame;
    s16 temp; // Not strictly necessary, but avoids a few s16 casts

    if (thisv->actionFunc == EnBili_DischargeLightning) {
        temp = 3 - curFrame;
        thisv->tentaclesTexIndex = (ABS(temp) + 5) % 8;
    } else if (thisv->actionFunc == EnBili_Climb) {
        if (curFrame <= 9) {
            temp = curFrame >> 1;
            thisv->tentaclesTexIndex = CLAMP_MAX(temp, 3);
        } else if (curFrame <= 18) {
            temp = 17 - curFrame;
            thisv->tentaclesTexIndex = CLAMP_MIN(temp, 0) >> 1;
        } else if (curFrame <= 36) {
            thisv->tentaclesTexIndex = ((36 - curFrame) / 3) + 2;
        } else {
            thisv->tentaclesTexIndex = (40 - curFrame) >> 1;
        }
    } else {
        thisv->tentaclesTexIndex = curFrame >> 1;
    }
}

/**
 * Tracks Player height, with oscillation, and moves away from walls
 */
void EnBili_UpdateFloating(EnBili* thisv) {
    f32 playerHeight = thisv->actor.world.pos.y + thisv->actor.yDistToPlayer;
    f32 heightOffset = ((thisv->actionFunc == EnBili_SetNewHomeHeight) ? 100.0f : 40.0f);
    f32 baseHeight = CLAMP_MIN(thisv->actor.floorHeight, playerHeight);

    Math_StepToF(&thisv->actor.home.pos.y, baseHeight + heightOffset, 1.0f);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (sinf(thisv->timer * (std::numbers::pi_v<float> / 16)) * 3.0f);

    // Turn around if touching wall
    if (thisv->actor.bgCheckFlags & 8) {
        thisv->actor.world.rot.y = thisv->actor.wallYaw;
    }
}

// Action functions

void EnBili_FloatIdle(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (!(thisv->timer % 4)) {
        thisv->actor.world.rot.y += Rand_CenteredFloat(1820.0f);
    }

    EnBili_UpdateFloating(thisv);

    if (thisv->timer == 0) {
        thisv->timer = 32;
    }

    if ((thisv->actor.xzDistToPlayer < 160.0f) && (fabsf(thisv->actor.yDistToPlayer) < 45.0f)) {
        EnBili_SetupApproachPlayer(thisv);
    }
}

void EnBili_SpawnedFlyApart(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        EnBili_SetupFloatIdle(thisv);
    }
}

void EnBili_DischargeLightning(EnBili* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 200, 255, 255, 255 };
    s32 i;
    Vec3f effectPos;
    s16 effectYaw;

    for (i = 0; i < 4; i++) {
        if (!((thisv->timer + (i << 1)) % 4)) {
            effectYaw = (s16)Rand_CenteredFloat(12288.0f) + (i * 0x4000) + 0x2000;
            effectPos.x = Rand_CenteredFloat(5.0f) + thisv->actor.world.pos.x;
            effectPos.y = (Rand_ZeroOne() * 5.0f) + thisv->actor.world.pos.y + 2.5f;
            effectPos.z = Rand_CenteredFloat(5.0f) + thisv->actor.world.pos.z;
            EffectSsLightning_Spawn(globalCtx, &effectPos, &primColor, &envColor, 15, effectYaw, 6, 2);
        }
    }

    SkelAnime_Update(&thisv->skelAnime);
    func_8002F974(&thisv->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.velocity.y *= -1.0f;

    if ((thisv->timer == 0) && Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        if (thisv->actor.params == EN_BILI_TYPE_DYING) {
            EnBili_SetupDie(thisv);
        } else {
            EnBili_SetupClimb(thisv);
        }
    }
}

void EnBili_Climb(EnBili* thisv, GlobalContext* globalCtx) {
    s32 skelAnimeUpdate = SkelAnime_Update(&thisv->skelAnime);
    f32 curFrame = thisv->skelAnime.curFrame;

    if (Animation_OnFrame(&thisv->skelAnime, 9.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIRI_JUMP);
    }

    if (curFrame > 9.0f) {
        Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.world.pos.y + thisv->actor.yDistToPlayer + 100.0f, 0.5f,
                       5.0f);
    }

    if (skelAnimeUpdate) {
        EnBili_SetupSetNewHomeHeight(thisv);
    }
}

void EnBili_ApproachPlayer(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 2, 1820);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    EnBili_UpdateFloating(thisv);

    if (thisv->timer == 0) {
        thisv->timer = 32;
    }

    if (thisv->actor.xzDistToPlayer > 200.0f) {
        EnBili_SetupFloatIdle(thisv);
    }
}

void EnBili_SetNewHomeHeight(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, (s16)(thisv->actor.yawTowardsPlayer + 0x8000), 910);
    EnBili_UpdateFloating(thisv);

    if (thisv->timer == 0) {
        EnBili_SetupFloatIdle(thisv);
    }
}

void EnBili_Recoil(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.3f)) {
        thisv->actor.world.rot.y += 0x8000;
        EnBili_SetupFloatIdle(thisv);
    }
}

void EnBili_Burnt(EnBili* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.flags & ACTOR_FLAG_15) {
        thisv->actor.colorFilterTimer = 20;
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            EnBili_SetupDie(thisv);
        }
    }
}

void EnBili_Die(EnBili* thisv, GlobalContext* globalCtx) {
    static Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f effectAccel = { 0.0f, 0.0f, 0.0f };
    s16 effectScale;
    Vec3f effectPos;
    s32 i;

    if (thisv->actor.draw != NULL) {
        if (thisv->actor.flags & ACTOR_FLAG_15) {
            return;
        }
        thisv->actor.draw = NULL;
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x50);
    }

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer != 0) {
        for (i = 0; i < 2; i++) {
            effectPos.x = ((Rand_ZeroOne() * 10.0f) + thisv->actor.world.pos.x) - 5.0f;
            effectPos.y = ((Rand_ZeroOne() * 5.0f) + thisv->actor.world.pos.y) - 2.5f;
            effectPos.z = ((Rand_ZeroOne() * 10.0f) + thisv->actor.world.pos.z) - 5.0f;

            effectVelocity.y = Rand_ZeroOne() + 1.0f;
            effectScale = Rand_S16Offset(40, 40);

            if (Rand_ZeroOne() < 0.7f) {
                EffectSsDtBubble_SpawnColorProfile(globalCtx, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                                   25, 2, 1);
            } else {
                EffectSsDtBubble_SpawnColorProfile(globalCtx, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                                   25, 0, 1);
            }
        }
    } else {
        Actor_Kill(&thisv->actor);
    }

    if (thisv->timer == 14) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_BIRI_BUBLE);
    }
}

void EnBili_Stunned(EnBili* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    }

    if (thisv->timer == 0) {
        EnBili_SetupFloatIdle(thisv);
    }
}

void EnBili_Frozen(EnBili* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (!(thisv->actor.flags & ACTOR_FLAG_15)) {
        thisv->actor.gravity = -1.0f;
    }

    if ((thisv->actor.bgCheckFlags & 1) || (thisv->actor.floorHeight == BGCHECK_Y_MIN)) {
        thisv->actor.colorFilterTimer = 0;
        EnBili_SetupDie(thisv);
    } else {
        thisv->actor.colorFilterTimer = 10;
    }
}

void EnBili_UpdateDamage(EnBili* thisv, GlobalContext* globalCtx) {
    u8 damageEffect;

    if ((thisv->actor.colChkInfo.health != 0) && (thisv->collider.base.acFlags & AC_HIT)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);

        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIRI_DEAD);
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            }

            damageEffect = thisv->actor.colChkInfo.damageEffect;

            if (damageEffect == BIRI_DMGEFF_DEKUNUT) {
                if (thisv->actionFunc != EnBili_Stunned) {
                    EnBili_SetupStunned(thisv);
                }
            } else if (damageEffect == BIRI_DMGEFF_SWORD) {
                if (thisv->actionFunc != EnBili_Stunned) {
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0x2000, 0xA);

                    if (thisv->actor.colChkInfo.health == 0) {
                        thisv->actor.params = EN_BILI_TYPE_DYING;
                    }
                    EnBili_SetupDischargeLightning(thisv);
                } else {
                    EnBili_SetupBurnt(thisv);
                }
            } else if (damageEffect == BIRI_DMGEFF_FIRE) {
                EnBili_SetupBurnt(thisv);
                thisv->timer = 2;
            } else if (damageEffect == BIRI_DMGEFF_ICE) {
                EnBili_SetupFrozen(thisv, globalCtx);
            } else if (damageEffect == BIRI_DMGEFF_SLINGSHOT) {
                EnBili_SetupRecoil(thisv);
            } else {
                EnBili_SetupBurnt(thisv);
            }

            if (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x1F820) { // DMG_ARROW
                thisv->actor.flags |= ACTOR_FLAG_4;
            }
        }
    }
}

void EnBili_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBili* thisv = (EnBili*)thisx;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        EnBili_SetupDischargeLightning(thisv);
    }

    EnBili_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actionFunc != EnBili_Die) {
        EnBili_UpdateTentaclesIndex(thisv);
        if (Animation_OnFrame(&thisv->skelAnime, 9.0f)) {
            if ((thisv->actionFunc == EnBili_FloatIdle) || (thisv->actionFunc == EnBili_SetNewHomeHeight) ||
                (thisv->actionFunc == EnBili_ApproachPlayer) || (thisv->actionFunc == EnBili_Recoil)) {
                if (thisv->playFlySound) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIRI_FLY);
                    thisv->playFlySound = false;
                } else {
                    thisv->playFlySound = true;
                }
            }
        }
        if (thisv->actionFunc == EnBili_Recoil) {
            func_8002D97C(&thisv->actor);
        } else {
            Actor_MoveForward(&thisv->actor);
        }

        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, thisv->collider.dim.radius, thisv->collider.dim.height, 7);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

        if (thisv->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        Actor_SetFocus(&thisv->actor, 0.0f);
    }
}

// Draw and associated functions

void EnBili_PulseLimb3(EnBili* thisv, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (thisv->actionFunc == EnBili_DischargeLightning) {
        arg2->y = 1.0f - (sinf((std::numbers::pi_v<float> * 0.16667f) * frame) * 0.26f);
    } else if (thisv->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            arg2->y = (cosf((std::numbers::pi_v<float> * 0.125f) * frame) * 0.15f) + 0.85f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (std::numbers::pi_v<float> * 0.1f));
            arg2->y = 1.0f - (0.3f * cos);
            arg2->x = (0.2f * cos) + 0.8f;
        } else {
            cos = cosf((frame - 18.0f) * (std::numbers::pi_v<float> * 0.0227f));
            arg2->y = (0.31f * cos) + 1.0f;
            arg2->x = 1.0f - (0.4f * cos);
        }

        arg2->z = arg2->x;
    } else if (thisv->actionFunc == EnBili_Stunned) {
        sin = sinf((std::numbers::pi_v<float> * 0.1f) * thisv->timer) * 0.08f;
        arg2->x -= sin;
        arg2->y += sin;
        arg2->z -= sin;
    } else {
        arg2->y = (cosf((std::numbers::pi_v<float> * 0.125f) * frame) * 0.13f) + 0.87f;
    }
}

void EnBili_PulseLimb2(EnBili* thisv, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (thisv->actionFunc == EnBili_DischargeLightning) {
        arg2->y = (sinf((std::numbers::pi_v<float> * 0.16667f) * frame) * 0.2f) + 1.0f;
    } else if (thisv->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            arg2->x = 1.125f - (cosf((std::numbers::pi_v<float> * 0.125f) * frame) * 0.125f);
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (std::numbers::pi_v<float> * 0.1f));
            arg2->x = (0.275f * cos) + 0.975f;
            arg2->y = 1.25f - (0.25f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (std::numbers::pi_v<float> * 0.0227f));
            arg2->x = 1.0f - (0.3f * cos);
            arg2->y = (0.48f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    } else if (thisv->actionFunc == EnBili_Stunned) {
        sin = sinf((std::numbers::pi_v<float> * 0.1f) * thisv->timer) * 0.08f;
        arg2->x += sin;
        arg2->y -= sin;
        arg2->z += sin;
    } else {
        arg2->y = 1.1f - (cosf((std::numbers::pi_v<float> * 0.125f) * frame) * 0.1f);
    }
}

void EnBili_PulseLimb4(EnBili* thisv, f32 frame, Vec3f* arg2) {
    f32 cos;

    if (thisv->actionFunc == EnBili_Climb) {
        if (frame <= 8.0f) {
            cos = cosf((std::numbers::pi_v<float> * 0.125f) * frame);
            arg2->x = 1.125f - (0.125f * cos);
            arg2->y = (0.3f * cos) + 0.7f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (std::numbers::pi_v<float> * 0.1f));
            arg2->x = (0.325f * cos) + 0.925f;
            arg2->y = 0.95f - (0.55f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (std::numbers::pi_v<float> * 0.0227f));
            arg2->x = 1.0f - (0.4f * cos);
            arg2->y = (0.52f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    }
}

s32 EnBili_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                            Gfx** gfx) {
    EnBili* thisv = (EnBili*)thisx;
    Vec3f limbScale = { 1.0f, 1.0f, 1.0f };
    f32 curFrame = thisv->skelAnime.curFrame;

    if (limbIndex == EN_BILI_LIMB_OUTER_HOOD) {
        EnBili_PulseLimb3(thisv, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_INNER_HOOD) {
        EnBili_PulseLimb2(thisv, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_TENTACLES) {
        EnBili_PulseLimb4(thisv, curFrame, &limbScale);
        rot->y = (Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y) + 0x8000;
    }

    Matrix_Scale(limbScale.x, limbScale.y, limbScale.z, MTXMODE_APPLY);
    return false;
}

static void* sTentaclesTextures[] = {
    gBiriTentacles0Tex, gBiriTentacles1Tex, gBiriTentacles2Tex, gBiriTentacles3Tex,
    gBiriTentacles4Tex, gBiriTentacles5Tex, gBiriTentacles6Tex, gBiriTentacles7Tex,
};

#include "overlays/ovl_En_Bili/ovl_En_Bili.h"

void EnBili_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBili* thisv = (EnBili*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bili.c", 1521);
    func_80093D84(globalCtx->state.gfxCtx);

    thisv->tentaclesTexIndex = CLAMP_MAX(thisv->tentaclesTexIndex, 7);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTentaclesTextures[thisv->tentaclesTexIndex]));

    if ((thisv->actionFunc == EnBili_DischargeLightning) && ((thisv->timer & 1) != 0)) {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_809C16F0);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_809C1700);
    }

    POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                   EnBili_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bili.c", 1552);
}
