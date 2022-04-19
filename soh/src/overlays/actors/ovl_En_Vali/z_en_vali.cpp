/*
 * File: z_en_vali.c
 * Overlay: ovl_En_Vali
 * Description: Bari (Big Jellyfish)
 */

#include "z_en_vali.h"
#include "objects/object_vali/object_vali.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_12)

void EnVali_Init(Actor* thisx, GlobalContext* globalCtx);
void EnVali_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnVali_Update(Actor* thisx, GlobalContext* globalCtx);
void EnVali_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnVali_SetupLurk(EnVali* thisv);
void EnVali_SetupDropAppear(EnVali* thisv);

void EnVali_Lurk(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_DropAppear(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_FloatIdle(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_Attacked(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_Retaliate(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_MoveArmsDown(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_Burnt(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_DivideAndDie(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_Stunned(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_Frozen(EnVali* thisv, GlobalContext* globalCtx);
void EnVali_ReturnToLurk(EnVali* thisv, GlobalContext* globalCtx);

ActorInit En_Vali_InitVars = {
    ACTOR_EN_VALI,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_VALI,
    sizeof(EnVali),
    (ActorFunc)EnVali_Init,
    (ActorFunc)EnVali_Destroy,
    (ActorFunc)EnVali_Update,
    (ActorFunc)EnVali_Draw,
    NULL,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x07, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
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
        { 0xFFCFFFFF, 0x07, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 17, 35, -15, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 18, 32, MASS_HEAVY };

typedef enum {
    /* 0x0 */ BARI_DMGEFF_NONE,
    /* 0x1 */ BARI_DMGEFF_STUN,
    /* 0x2 */ BARI_DMGEFF_FIRE,
    /* 0x3 */ BARI_DMGEFF_ICE,
    /* 0xE */ BARI_DMGEFF_SLINGSHOT = 0xE,
    /* 0xF */ BARI_DMGEFF_SWORD
} BariDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, BARI_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(0, BARI_DMGEFF_SLINGSHOT),
    /* Explosive     */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, BARI_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, BARI_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, BARI_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, BARI_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, BARI_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, BARI_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, BARI_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(4, BARI_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, BARI_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, BARI_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, BARI_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, BARI_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, BARI_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x18, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5000, ICHAIN_STOP),
};

void EnVali_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnVali* thisv = (EnVali*)thisx;
    s32 bgId;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 27.0f);
    thisv->actor.shape.shadowAlpha = 155;
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gBariSkel, &gBariLurkingAnim, thisv->jointTable, thisv->morphTable,
                   EN_VALI_LIMB_MAX);

    Collider_InitQuad(globalCtx, &thisv->leftArmCollider);
    Collider_SetQuad(globalCtx, &thisv->leftArmCollider, &thisv->actor, &sQuadInit);
    Collider_InitQuad(globalCtx, &thisv->rightArmCollider);
    Collider_SetQuad(globalCtx, &thisv->rightArmCollider, &thisv->actor, &sQuadInit);
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    EnVali_SetupLurk(thisv);

    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                          &thisv->actor, &thisv->actor.world.pos);
    thisv->actor.params = BARI_TYPE_NORMAL;

    if (thisv->actor.floorHeight == BGCHECK_Y_MIN) {
        Actor_Kill(&thisv->actor);
    }
}

void EnVali_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnVali* thisv = (EnVali*)thisx;

    Collider_DestroyQuad(globalCtx, &thisv->leftArmCollider);
    Collider_DestroyQuad(globalCtx, &thisv->rightArmCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
}

void EnVali_SetupLurk(EnVali* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gBariLurkingAnim);
    thisv->actor.draw = NULL;
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnVali_Lurk;
}

void EnVali_SetupDropAppear(EnVali* thisv) {
    thisv->actor.draw = EnVali_Draw;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actor.velocity.y = 1.0f;
    thisv->actionFunc = EnVali_DropAppear;
}

void EnVali_SetupFloatIdle(EnVali* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBariWaitingAnim, -3.0f);
    thisv->leftArmCollider.dim.quad[2] = thisv->leftArmCollider.dim.quad[3] = thisv->rightArmCollider.dim.quad[2] =
        thisv->rightArmCollider.dim.quad[3] = thisv->leftArmCollider.dim.quad[0] = thisv->leftArmCollider.dim.quad[1] =
            thisv->rightArmCollider.dim.quad[0] = thisv->rightArmCollider.dim.quad[1] = thisv->actor.world.pos;

    thisv->leftArmCollider.dim.quad[2].y = thisv->leftArmCollider.dim.quad[3].y = thisv->rightArmCollider.dim.quad[2].y =
        thisv->rightArmCollider.dim.quad[3].y = thisv->leftArmCollider.dim.quad[0].y =
            thisv->leftArmCollider.dim.quad[1].y = thisv->rightArmCollider.dim.quad[0].y =
                thisv->rightArmCollider.dim.quad[1].y = thisv->actor.world.pos.y - 10.0f;

    thisv->actor.flags &= ~ACTOR_FLAG_4;
    thisv->bodyCollider.base.acFlags |= AC_ON;
    thisv->slingshotReactionTimer = 0;
    thisv->floatHomeHeight = thisv->actor.world.pos.y;
    thisv->actionFunc = EnVali_FloatIdle;
}

/**
 * Used for both touching player/player's shield and being hit with sword. What to do next is determined by params.
 */
void EnVali_SetupAttacked(EnVali* thisv) {
    thisv->lightningTimer = 20;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnVali_Attacked;
}

void EnVali_SetupRetaliate(EnVali* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBariRetaliatingAnim, -5.0f);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 150, 0x2000, 30);
    thisv->actor.params = BARI_TYPE_NORMAL;
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnVali_Retaliate;
}

void EnVali_SetupMoveArmsDown(EnVali* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gBariMovingArmsDownAnim);
    thisv->actionFunc = EnVali_MoveArmsDown;
}

void EnVali_SetupBurnt(EnVali* thisv) {
    thisv->timer = 2;
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 150, 0x2000, 30);
    thisv->actionFunc = EnVali_Burnt;
}

void EnVali_SetupDivideAndDie(EnVali* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 3; i++) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BILI, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                    thisv->actor.world.pos.z, 0, thisv->actor.world.rot.y, 0, 0);

        thisv->actor.world.rot.y += 0x10000 / 3;
    }

    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x50);
    thisv->timer = Rand_S16Offset(10, 10);
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_BARI_SPLIT);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.draw = NULL;
    thisv->actionFunc = EnVali_DivideAndDie;
}

void EnVali_SetupStunned(EnVali* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBariWaitingAnim, 10.0f);
    thisv->timer = 80;
    thisv->actor.velocity.y = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 255, 0x2000, 80);
    thisv->bodyCollider.info.bumper.effect = 0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->actor.velocity.y = 1.0f;
    thisv->actionFunc = EnVali_Stunned;
}

void EnVali_SetupFrozen(EnVali* thisv) {
    thisv->actor.velocity.y = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 255, 0x2000, 36);
    thisv->bodyCollider.base.acFlags &= ~AC_ON;
    thisv->timer = 36;
    thisv->actionFunc = EnVali_Frozen;
}

void EnVali_SetupReturnToLurk(EnVali* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBariLurkingAnim, 10.0f);
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = EnVali_ReturnToLurk;
}

void EnVali_DischargeLightning(EnVali* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 200, 255, 255, 255 };
    Vec3f pos;
    s32 i;
    f32 cos;
    f32 sin;
    s16 yaw;

    for (i = 0; i < 4; i++) {
        cos = -Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)));
        sin = Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)));
        if (!((thisv->lightningTimer + (i << 1)) % 4)) {
            yaw = (s16)Rand_CenteredFloat(12288.0f) + (i * 0x4000) + 0x2000;
            pos.x = thisv->actor.world.pos.x + (Math_SinS(yaw) * 12.0f * cos);
            pos.y = thisv->actor.world.pos.y - (Math_CosS(yaw) * 12.0f) + 10.0f;
            pos.z = thisv->actor.world.pos.z + (Math_SinS(yaw) * 12.0f * sin);

            EffectSsLightning_Spawn(globalCtx, &pos, &primColor, &envColor, 17, yaw, 6, 2);
        }
    }

    func_8002F974(&thisv->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
}

void EnVali_Lurk(EnVali* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer < 150.0f) {
        EnVali_SetupDropAppear(thisv);
    }
}

void EnVali_DropAppear(EnVali* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.velocity.y *= 1.5f;
    thisv->actor.velocity.y = CLAMP_MAX(thisv->actor.velocity.y, 40.0f);

    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, thisv->actor.velocity.y)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        EnVali_SetupFloatIdle(thisv);
    }
}

void EnVali_FloatIdle(EnVali* thisv, GlobalContext* globalCtx) {
    s32 curFrame;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->slingshotReactionTimer != 0) {
        thisv->slingshotReactionTimer--;
    }

    curFrame = thisv->skelAnime.curFrame;

    Math_StepToF(&thisv->floatHomeHeight, thisv->actor.floorHeight + 40.0f, 1.2f);
    thisv->actor.world.pos.y = thisv->floatHomeHeight - (sinf(curFrame * std::numbers::pi_v<float> * 0.0125f) * 8.0f);

    if (thisv->slingshotReactionTimer) {
        thisv->actor.shape.rot.y += 0x800;

        if (((thisv->slingshotReactionTimer % 6) == 0) && (curFrame > 15) && (curFrame <= 55)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BARI_ROLL);
        }
    } else if ((curFrame == 16) || (curFrame == 30) || (curFrame == 42) || (curFrame == 55)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BARI_ROLL);
    }

    curFrame = ((curFrame > 40) ? (80 - curFrame) : curFrame);

    thisv->actor.shape.rot.y += (s16)((curFrame + 4) * 0.4f * (0x10000 / 360.0f));
    if (thisv->actor.xzDistToPlayer > 250.0f) {
        EnVali_SetupReturnToLurk(thisv);
    }
}

void EnVali_Attacked(EnVali* thisv, GlobalContext* globalCtx) {
    if (thisv->lightningTimer != 0) {
        thisv->lightningTimer--;
    }

    EnVali_DischargeLightning(thisv, globalCtx);

    if (thisv->lightningTimer == 0) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->bodyCollider.base.acFlags |= AC_ON;
        if (thisv->actor.params == BARI_TYPE_SWORD_DAMAGE) {
            EnVali_SetupRetaliate(thisv);
        } else {
            thisv->actionFunc = EnVali_FloatIdle;
        }
    } else if ((thisv->lightningTimer % 2) != 0) {
        thisv->actor.world.pos.y += 1.0f;
    } else {
        thisv->actor.world.pos.y -= 1.0f;
    }
}

void EnVali_Retaliate(EnVali* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.colChkInfo.health != 0) {
            EnVali_SetupMoveArmsDown(thisv);
        } else {
            EnVali_SetupDivideAndDie(thisv, globalCtx);
        }
    }
}

void EnVali_MoveArmsDown(EnVali* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnVali_SetupFloatIdle(thisv);
    }
}

void EnVali_Burnt(EnVali* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        EnVali_SetupDivideAndDie(thisv, globalCtx);
    }
}

void EnVali_DivideAndDie(EnVali* thisv, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s16 scale;
    Vec3f pos;
    s32 i;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    for (i = 0; i < 2; i++) {
        pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(20.0f);
        pos.y = thisv->actor.world.pos.y + Rand_CenteredFloat(8.0f);
        pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(20.0f);
        velocity.y = (Rand_ZeroOne() + 1.0f);
        scale = Rand_S16Offset(40, 40);

        if (Rand_ZeroOne() < 0.7f) {
            EffectSsDtBubble_SpawnColorProfile(globalCtx, &pos, &velocity, &accel, scale, 25, 2, 1);
        } else {
            EffectSsDtBubble_SpawnColorProfile(globalCtx, &pos, &velocity, &accel, scale, 25, 0, 1);
        }
    }

    if (thisv->timer == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnVali_Stunned(EnVali* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->actor.velocity.y != 0.0f) {
        if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, thisv->actor.velocity.y)) {
            thisv->actor.velocity.y = 0.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        } else {
            thisv->actor.velocity.y += 1.0f;
        }
    }

    if (thisv->timer == 0) {
        thisv->bodyCollider.info.bumper.effect = 1; // Shock?
        EnVali_SetupFloatIdle(thisv);
    }
}

void EnVali_Frozen(EnVali* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    s32 temp_v0;
    s32 temp_v1;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    temp_v1 = thisv->timer - 20;
    thisv->actor.colorFilterTimer = 36;

    if (temp_v1 > 0) {
        temp_v0 = temp_v1 >> 1;

        if ((thisv->timer % 2) != 0) {
            pos.y = thisv->actor.world.pos.y - 20.0f + (-temp_v0 * 5 + 40);
            pos.x = thisv->actor.world.pos.x + ((temp_v0 & 2) ? 12.0f : -12.0f);
            pos.z = thisv->actor.world.pos.z + ((temp_v0 & 1) ? 12.0f : -12.0f);

            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &pos, 150, 150, 150, 250, 235, 245, 255,
                                           (Rand_ZeroOne() * 0.2f) + 1.3f);
        }
    } else if (thisv->timer == 0) {
        thisv->actor.velocity.y += 1.0f;
        if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, thisv->actor.velocity.y)) {
            EnVali_SetupDivideAndDie(thisv, globalCtx);
            thisv->actor.colorFilterTimer = 0;
        }
    }
}

void EnVali_ReturnToLurk(EnVali* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 15.0f, 0.1f) < 0.01f) {
        EnVali_SetupLurk(thisv);
    }
}

void EnVali_UpdateDamage(EnVali* thisv, GlobalContext* globalCtx) {
    if (thisv->bodyCollider.base.acFlags & AC_HIT) {
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->bodyCollider.info, 1);

        if ((thisv->actor.colChkInfo.damageEffect != BARI_DMGEFF_NONE) || (thisv->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BARI_DEAD);
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            } else if ((thisv->actor.colChkInfo.damageEffect != BARI_DMGEFF_STUN) &&
                       (thisv->actor.colChkInfo.damageEffect != BARI_DMGEFF_SLINGSHOT)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BARI_DAMAGE);
            }

            if (thisv->actor.colChkInfo.damageEffect == BARI_DMGEFF_STUN) {
                if (thisv->actionFunc != EnVali_Stunned) {
                    EnVali_SetupStunned(thisv);
                }
            } else if (thisv->actor.colChkInfo.damageEffect == BARI_DMGEFF_SWORD) {
                if (thisv->actionFunc != EnVali_Stunned) {
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 150, 0x2000, 30);
                    thisv->actor.params = BARI_TYPE_SWORD_DAMAGE;
                    EnVali_SetupAttacked(thisv);
                } else {
                    EnVali_SetupRetaliate(thisv);
                }
            } else if (thisv->actor.colChkInfo.damageEffect == BARI_DMGEFF_FIRE) {
                EnVali_SetupBurnt(thisv);
            } else if (thisv->actor.colChkInfo.damageEffect == BARI_DMGEFF_ICE) {
                EnVali_SetupFrozen(thisv);
            } else if (thisv->actor.colChkInfo.damageEffect == BARI_DMGEFF_SLINGSHOT) {
                if (thisv->slingshotReactionTimer == 0) {
                    thisv->slingshotReactionTimer = 20;
                }
            } else {
                EnVali_SetupRetaliate(thisv);
            }
        }
    }
}

void EnVali_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnVali* thisv = (EnVali*)thisx;

    if ((thisv->bodyCollider.base.atFlags & AT_HIT) || (thisv->leftArmCollider.base.atFlags & AT_HIT) ||
        (thisv->rightArmCollider.base.atFlags & AT_HIT)) {
        thisv->leftArmCollider.base.atFlags &= ~AT_HIT;
        thisv->rightArmCollider.base.atFlags &= ~AT_HIT;
        thisv->bodyCollider.base.atFlags &= ~AT_HIT;
        EnVali_SetupAttacked(thisv);
    }

    EnVali_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if ((thisv->actionFunc != EnVali_DivideAndDie) && (thisv->actionFunc != EnVali_Lurk)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);

        if (thisv->actionFunc == EnVali_FloatIdle) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->leftArmCollider.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->rightArmCollider.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
        }

        if (thisv->bodyCollider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
        }

        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
        Actor_SetFocus(&thisv->actor, 0.0f);
    }
}

// Draw and associated functions

void EnVali_PulseOutside(EnVali* thisv, f32 curFrame, Vec3f* scale) {
    f32 scaleChange;

    if (thisv->actionFunc == EnVali_Attacked) {
        s32 scalePhase = 20 - (thisv->lightningTimer % 20);

        if (scalePhase >= 10) {
            scalePhase -= 10;
        }

        scale->y -= 0.2f * sinf((std::numbers::pi_v<float> / 10) * scalePhase);
    } else if (thisv->actionFunc == EnVali_Retaliate) {
        scaleChange = sinf((std::numbers::pi_v<float> / 10) * curFrame);
        scale->y -= 0.24f * scaleChange;
        scale->x -= 0.13f * scaleChange;
        scale->z = scale->x;
    } else if (thisv->actionFunc == EnVali_MoveArmsDown) {
        scaleChange = cosf((std::numbers::pi_v<float> / 50) * curFrame);
        scale->y -= 0.24f * scaleChange;
        scale->x -= 0.13f * scaleChange;
        scale->z = scale->x;
    } else if (thisv->actionFunc == EnVali_Stunned) {
        scaleChange = sinf((std::numbers::pi_v<float> / 10) * thisv->timer) * 0.08f;
        scale->x += scaleChange;
        scale->y -= scaleChange;
        scale->z += scaleChange;
    } else {
        if (curFrame >= 40.0f) {
            curFrame -= 40.0f;
        }

        scale->y -= 0.2f * sinf((std::numbers::pi_v<float> / 40) * curFrame);
    }
}

void EnVali_PulseInsides(EnVali* thisv, f32 curFrame, Vec3f* scale) {
    f32 scaleChange;

    if (thisv->actionFunc == EnVali_Attacked) {
        s32 scalePhase = 20 - (thisv->lightningTimer % 20);

        if (scalePhase >= 10) {
            scalePhase -= 10;
        }

        scale->y -= 0.13f * sinf((std::numbers::pi_v<float> / 10) * scalePhase);
    } else if (thisv->actionFunc == EnVali_Retaliate) {
        scaleChange = sinf((std::numbers::pi_v<float> / 10) * curFrame);
        scale->y -= 0.18f * scaleChange;
        scale->x -= 0.1f * scaleChange;
        scale->z = scale->x;
    } else if (thisv->actionFunc == EnVali_MoveArmsDown) {
        scaleChange = cosf((std::numbers::pi_v<float> / 50) * curFrame);
        scale->y -= 0.18f * scaleChange;
        scale->x -= 0.1f * scaleChange;
        scale->z = scale->x;
    } else if (thisv->actionFunc == EnVali_Stunned) {
        scaleChange = sinf((std::numbers::pi_v<float> / 10) * thisv->timer) * 0.08f;
        scale->x -= scaleChange;
        scale->y += scaleChange;
        scale->z -= scaleChange;
    } else {
        if (curFrame >= 40.0f) {
            curFrame -= 40.0f;
        }

        scale->y -= 0.13f * sinf((std::numbers::pi_v<float> / 40) * curFrame);
    }
}

s32 EnVali_SetArmLength(EnVali* thisv, f32 curFrame) {
    f32 targetArmScale;

    if (thisv->actionFunc == EnVali_FloatIdle) {
        if (curFrame <= 10.0f) {
            targetArmScale = curFrame * 0.05f + 1.0f;
        } else if (curFrame > 70.0f) {
            targetArmScale = (80.0f - curFrame) * 0.05f + 1.0f;
        } else {
            targetArmScale = 1.5f;
        }
    } else if (thisv->actionFunc == EnVali_Retaliate) {
        targetArmScale = 1.0f - sinf((std::numbers::pi_v<float> / 10) * curFrame) * 0.35f;
    } else if (thisv->actionFunc == EnVali_MoveArmsDown) {
        targetArmScale = 1.0f - cosf((std::numbers::pi_v<float> / 50) * curFrame) * 0.35f;
    } else if ((thisv->actionFunc == EnVali_Attacked) || (thisv->actionFunc == EnVali_Frozen)) {
        targetArmScale = thisv->armScale;
    } else {
        targetArmScale = 1.0f;
    }

    Math_StepToF(&thisv->armScale, targetArmScale, 0.1f);

    if (thisv->armScale == 1.0f) {
        return false;
    } else {
        return true;
    }
}

s32 EnVali_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                            Gfx** gfx) {
    EnVali* thisv = (EnVali*)thisx;
    f32 curFrame;

    if ((limbIndex == EN_VALI_LIMB_NUCLEUS) || (limbIndex == EN_VALI_LIMB_OUTER_HOOD) ||
        (limbIndex == EN_VALI_LIMB_INNER_HOOD)) {
        *dList = NULL;
        return false;
    } else {
        curFrame = thisv->skelAnime.curFrame;

        if ((limbIndex == EN_VALI_LIMB_LEFT_ARM_BASE) || (limbIndex == EN_VALI_LIMB_RIGHT_ARM_BASE)) {
            if (EnVali_SetArmLength(thisv, curFrame)) {
                Matrix_Scale(thisv->armScale, 1.0f, 1.0f, MTXMODE_APPLY);
            }
        }

        return false;
    }
}

void EnVali_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f D_80B28970 = { 3000.0f, 0.0f, 0.0f };
    static Vec3f D_80B2897C = { -1000.0f, 0.0f, 0.0f };
    Vec3f sp3C;
    Vec3f sp30;
    EnVali* thisv = (EnVali*)thisx;

    if (thisv->actionFunc == EnVali_FloatIdle) {
        if ((limbIndex == EN_VALI_LIMB_LEFT_FOREARM_BASE) || (limbIndex == EN_VALI_LIMB_RIGHT_FOREARM_BASE)) {
            Matrix_MultVec3f(&D_80B28970, &sp3C);
            Matrix_MultVec3f(&D_80B2897C, &sp30);

            if (limbIndex == EN_VALI_LIMB_LEFT_FOREARM_BASE) {
                Collider_SetQuadVertices(&thisv->leftArmCollider, &sp30, &sp3C, &thisv->leftArmCollider.dim.quad[0],
                                         &thisv->leftArmCollider.dim.quad[1]);
            } else {
                Collider_SetQuadVertices(&thisv->rightArmCollider, &sp30, &sp3C, &thisv->rightArmCollider.dim.quad[0],
                                         &thisv->rightArmCollider.dim.quad[1]);
            }
        }
    }
}

void EnVali_DrawBody(EnVali* thisv, GlobalContext* globalCtx) {
    MtxF mtx;
    f32 cos;
    f32 sin;
    f32 curFrame;
    Vec3f scale = { 1.0f, 1.0f, 1.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_vali.c", 1428);

    Matrix_Get(&mtx);
    curFrame = thisv->skelAnime.curFrame;
    EnVali_PulseInsides(thisv, curFrame, &scale);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vali.c", 1436),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariInnerHoodDL);

    Matrix_Put(&mtx);
    Matrix_RotateY(-thisv->actor.shape.rot.y * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);

    cos = Math_CosS(thisv->actor.shape.rot.y);
    sin = Math_SinS(thisv->actor.shape.rot.y);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vali.c", 1446),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Translate((506.0f * cos) + (372.0f * sin), 1114.0f, (372.0f * cos) - (506.0f * sin), MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vali.c", 1455),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Translate((-964.0f * cos) - (804.0f * sin), -108.0f, (-804.0f * cos) + (964.0f * sin), MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vali.c", 1463),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariNucleusDL);

    Matrix_Put(&mtx);

    scale.x = scale.y = scale.z = 1.0f;

    EnVali_PulseOutside(thisv, curFrame, &scale);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vali.c", 1471),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBariOuterHoodDL);

    Matrix_Put(&mtx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_vali.c", 1477);
}

static Gfx D_80B28998[] = {
    gsDPSetCombineLERP(1, TEXEL0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0, TEXEL1, 0,
                       PRIM_LOD_FRAC, COMBINED),
    gsSPEndDisplayList(),
};

static Gfx D_80B289A8[] = {
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0, TEXEL1, 0,
                       PRIM_LOD_FRAC, COMBINED),
    gsSPEndDisplayList(),
};

void EnVali_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnVali* thisv = (EnVali*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_vali.c", 1505);
    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TexScroll(globalCtx->state.gfxCtx, 0, (127 - (globalCtx->gameplayFrames * 12)) % 128, 32, 32));

    if ((thisv->lightningTimer % 2) != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80B28998);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80B289A8);
    }

    EnVali_DrawBody(thisv, globalCtx);

    POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                   EnVali_OverrideLimbDraw, EnVali_PostLimbDraw, thisv, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_vali.c", 1538);
}
