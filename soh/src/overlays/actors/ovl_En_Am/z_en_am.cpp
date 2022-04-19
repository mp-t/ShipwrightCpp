/*
 * File: z_en_am.c
 * Overlay: ovl_En_Am
 * Description: Armos
 */

#include "z_en_am.h"
#include "objects/object_am/object_am.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_26)

void EnAm_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAm_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAm_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnAm_SetupStatue(EnAm* thisv);
void EnAm_SetupSleep(EnAm* thisv);
void EnAm_Statue(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_Sleep(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_Lunge(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_RotateToHome(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_MoveToHome(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_RotateToInit(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_Cooldown(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_Ricochet(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_Stunned(EnAm* thisv, GlobalContext* globalCtx);
void EnAm_RecoilFromDamage(EnAm* thisv, GlobalContext* globalCtx);

typedef enum {
    /* 00 */ AM_BEHAVIOR_NONE,
    /* 01 */ AM_BEHAVIOR_DAMAGED,
    /* 03 */ AM_BEHAVIOR_DO_NOTHING = 3,
    /* 05 */ AM_BEHAVIOR_5 = 5, // checked but never set
    /* 06 */ AM_BEHAVIOR_STUNNED,
    /* 07 */ AM_BEHAVIOR_GO_HOME,
    /* 08 */ AM_BEHAVIOR_RICOCHET,
    /* 10 */ AM_BEHAVIOR_AGGRO = 10
} ArmosBehavior;

ActorInit En_Am_InitVars = {
    ACTOR_EN_AM,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_AM,
    sizeof(EnAm),
    (ActorFunc)EnAm_Init,
    (ActorFunc)EnAm_Destroy,
    (ActorFunc)EnAm_Update,
    (ActorFunc)EnAm_Draw,
    NULL,
};

static ColliderCylinderInit sHurtCylinderInit = {
    {
        COLTYPE_HIT5,
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
    { 15, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sBlockCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00400106, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 00 */ AM_DMGEFF_NONE, // used by anything that cant kill the armos
    /* 01 */ AM_DMGEFF_NUT,
    /* 06 */ AM_DMGEFF_STUN = 6, // doesnt include deku nuts
    /* 13 */ AM_DMGEFF_ICE = 13,
    /* 14 */ AM_DMGEFF_MAGIC_FIRE_LIGHT,
    /* 15 */ AM_DMGEFF_KILL // any damage source that can kill the armos (and isnt a special case)
} ArmosDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, AM_DMGEFF_NUT),
    /* Deku stick    */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Boomerang     */ DMG_ENTRY(0, AM_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Hammer swing  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Hookshot      */ DMG_ENTRY(0, AM_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Giant's Knife */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Fire arrow    */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Ice arrow     */ DMG_ENTRY(4, AM_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Unk arrow 1   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(0, AM_DMGEFF_MAGIC_FIRE_LIGHT),
    /* Ice magic     */ DMG_ENTRY(3, AM_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, AM_DMGEFF_MAGIC_FIRE_LIGHT),
    /* Shield        */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, AM_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Master spin   */ DMG_ENTRY(2, AM_DMGEFF_KILL),
    /* Kokiri jump   */ DMG_ENTRY(2, AM_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, AM_DMGEFF_KILL),
    /* Master jump   */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Unknown 1     */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, AM_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, AM_DMGEFF_KILL),
    /* Unknown 2     */ DMG_ENTRY(0, AM_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x13, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
};

void EnAm_SetupAction(EnAm* thisv, EnAmActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

/**
 * Checks bgCheckFlags in the direction of current yaw at the specified distance.
 *
 * Returns true if the armos would land on the ground in the resulting position.
 *
 * If it won't land on the ground, it will return true anyway if the floor the armos will be on
 * is no more than 20 units lower than the home position. This prevents the armos from going down steep slopes.
 */
s32 EnAm_CanMove(EnAm* thisv, GlobalContext* globalCtx, f32 distance, s16 yaw) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    // save current position and bgCheckFlags to restore later
    curPos = thisv->dyna.actor.world.pos;
    curBgCheckFlags = thisv->dyna.actor.bgCheckFlags;

    sin = Math_SinS(yaw) * distance;
    cos = Math_CosS(yaw) * distance;

    thisv->dyna.actor.world.pos.x += sin;
    thisv->dyna.actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 0.0f, 0.0f, 0.0f, 4);
    thisv->dyna.actor.world.pos = curPos;
    ret = thisv->dyna.actor.bgCheckFlags & 1;

    if (!ret && (thisv->dyna.actor.floorHeight >= (thisv->dyna.actor.home.pos.y - 20.0f))) {
        ret = true;
    }

    thisv->dyna.actor.bgCheckFlags = curBgCheckFlags;

    return ret;
}

void EnAm_Init(Actor* thisx, GlobalContext* globalCtx) {
    const CollisionHeader* colHeader = NULL;
    s32 pad;
    EnAm* thisv = (EnAm*)thisx;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gArmosSkel, &gArmosRicochetAnim, thisv->jointTable, thisv->morphTable,
                   14);
    Actor_SetScale(&thisv->dyna.actor, 0.01f);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    Collider_InitCylinder(globalCtx, &thisv->hurtCollider);
    Collider_InitCylinder(globalCtx, &thisv->blockCollider);
    Collider_SetCylinder(globalCtx, &thisv->hurtCollider, &thisv->dyna.actor, &sHurtCylinderInit);

    if (thisv->dyna.actor.params == ARMOS_STATUE) {
        thisv->dyna.actor.colChkInfo.health = 1;
        Collider_SetCylinder(globalCtx, &thisv->blockCollider, &thisv->dyna.actor, &sHurtCylinderInit);
        thisv->hurtCollider.base.ocFlags1 = (OC1_ON | OC1_NO_PUSH | OC1_TYPE_1 | OC1_TYPE_2);
        thisv->blockCollider.base.ocFlags1 = (OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER);
        CollisionHeader_GetVirtual(&gArmosCol, &colHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->dyna.actor, ACTORCAT_BG);
        EnAm_SetupStatue(thisv);
    } else {
        Collider_SetCylinder(globalCtx, &thisv->blockCollider, &thisv->dyna.actor, &sBlockCylinderInit);
        Collider_InitQuad(globalCtx, &thisv->hitCollider);
        Collider_SetQuad(globalCtx, &thisv->hitCollider, &thisv->dyna.actor, &sQuadInit);
        thisv->dyna.actor.colChkInfo.health = 1;
        thisv->dyna.actor.colChkInfo.damageTable = &sDamageTable;
        EnAm_SetupSleep(thisv);
        thisv->unk_258 = 0;
    }

    thisv->dyna.actor.colChkInfo.mass = MASS_HEAVY;
}

void EnAm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnAm* thisv = (EnAm*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &thisv->hurtCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->blockCollider);
    //! @bug Quad collider is not destroyed (though destroy doesnt really do anything anyway)
}

void EnAm_SpawnEffects(EnAm* thisv, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, -1.5f, 0.0f };
    static Vec3f accel = { 0.0f, -0.2f, 0.0f };
    s32 i;
    Vec3f pos;
    Color_RGBA8 primColor = { 100, 100, 100, 0 };
    Color_RGBA8 envColor = { 40, 40, 40, 0 };
    s32 pad;

    for (i = 4; i > 0; i--) {
        pos.x = thisv->dyna.actor.world.pos.x + ((Rand_ZeroOne() - 0.5f) * 65.0f);
        pos.y = (thisv->dyna.actor.world.pos.y + 40.0f) + ((Rand_ZeroOne() - 0.5f) * 10.0f);
        pos.z = thisv->dyna.actor.world.pos.z + ((Rand_ZeroOne() - 0.5f) * 65.0f);

        EffectSsKiraKira_SpawnSmall(globalCtx, &pos, &velocity, &accel, &primColor, &envColor);
    }

    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_WALK);
    Actor_SpawnFloorDustRing(globalCtx, &thisv->dyna.actor, &thisv->dyna.actor.world.pos, 4.0f, 3, 8.0f, 0x12C, 0xF, 0);
}

void EnAm_SetupSleep(EnAm* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gArmosRicochetAnim);

    Animation_Change(&thisv->skelAnime, &gArmosRicochetAnim, 0.0f, lastFrame, lastFrame, ANIMMODE_LOOP, 0.0f);
    thisv->behavior = AM_BEHAVIOR_DO_NOTHING;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->unk_258 = (thisv->textureBlend == 255) ? 0 : 1;
    EnAm_SetupAction(thisv, EnAm_Sleep);
}

void EnAm_SetupStatue(EnAm* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gArmosRicochetAnim);

    Animation_Change(&thisv->skelAnime, &gArmosRicochetAnim, 0.0f, lastFrame, lastFrame, ANIMMODE_LOOP, 0.0f);
    thisv->dyna.actor.flags &= ~ACTOR_FLAG_0;
    thisv->behavior = AM_BEHAVIOR_DO_NOTHING;
    thisv->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(thisv, EnAm_Statue);
}

void EnAm_SetupLunge(EnAm* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gArmosHopAnim, 4.0f);
    thisv->unk_258 = 3;
    thisv->behavior = AM_BEHAVIOR_AGGRO;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.shape.rot.y;
    EnAm_SetupAction(thisv, EnAm_Lunge);
}

void EnAm_SetupCooldown(EnAm* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gArmosHopAnim, 4.0f);
    thisv->unk_258 = 3;
    thisv->cooldownTimer = 40;
    thisv->behavior = AM_BEHAVIOR_AGGRO;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.shape.rot.y;
    EnAm_SetupAction(thisv, EnAm_Cooldown);
}

void EnAm_SetupMoveToHome(EnAm* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gArmosHopAnim, 4.0f);
    thisv->behavior = AM_BEHAVIOR_GO_HOME;
    thisv->unk_258 = 1;
    thisv->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(thisv, EnAm_MoveToHome);
}

void EnAm_SetupRotateToInit(EnAm* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gArmosHopAnim, 4.0f);
    thisv->behavior = AM_BEHAVIOR_GO_HOME;
    thisv->unk_258 = 1;
    thisv->dyna.actor.speedXZ = 0.0f;
    EnAm_SetupAction(thisv, EnAm_RotateToInit);
}

void EnAm_SetupRotateToHome(EnAm* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gArmosHopAnim, 4.0f);
    thisv->behavior = AM_BEHAVIOR_GO_HOME;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.shape.rot.y;
    EnAm_SetupAction(thisv, EnAm_RotateToHome);
}

void EnAm_SetupRecoilFromDamage(EnAm* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gArmosDamagedAnim, 1.0f, 4.0f,
                     Animation_GetLastFrame(&gArmosDamagedAnim) - 6.0f, ANIMMODE_ONCE, 0.0f);
    thisv->behavior = AM_BEHAVIOR_DAMAGED;
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.yawTowardsPlayer;
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_DAMAGE);

    if (EnAm_CanMove(thisv, globalCtx, -6.0f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = -6.0f;
    }

    thisv->dyna.actor.colorFilterTimer = 0;
    Enemy_StartFinishingBlow(globalCtx, &thisv->dyna.actor);
    EnAm_SetupAction(thisv, EnAm_RecoilFromDamage);
}

void EnAm_SetupRicochet(EnAm* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gArmosRicochetAnim, 1.0f, 0.0f, 8.0f, ANIMMODE_ONCE, 0.0f);
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.yawTowardsPlayer;

    if (EnAm_CanMove(thisv, globalCtx, -6.0f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = -6.0f;
    }

    thisv->unk_264 = 0;
    thisv->unk_258 = 0;
    thisv->cooldownTimer = 5;
    thisv->behavior = AM_BEHAVIOR_RICOCHET;
    EnAm_SetupAction(thisv, EnAm_Ricochet);
}

void EnAm_Sleep(EnAm* thisv, GlobalContext* globalCtx) {
    f32 cos;
    s32 pad;
    f32 rand;
    f32 sin;
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->unk_258 != 0) ||
        ((thisv->hurtCollider.base.ocFlags1 & OC1_HIT) && (thisv->hurtCollider.base.oc == &player->actor)) ||
        (thisv->hurtCollider.base.acFlags & AC_HIT)) {
        thisv->hurtCollider.base.acFlags &= ~AC_HIT;

        if (thisv->textureBlend == 0) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_WAVE);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_VOICE);
            Actor_SetColorFilter(&thisv->dyna.actor, 0x4000, 255, 0, 8);
        }

        if (thisv->textureBlend >= 240) {
            thisv->attackTimer = 200;
            thisv->textureBlend = 255;
            thisv->dyna.actor.flags |= ACTOR_FLAG_0;
            thisv->dyna.actor.shape.yOffset = 0.0f;
            EnAm_SetupLunge(thisv);
        } else {
            // shake randomly about the origin while waking up
            rand = (Rand_ZeroOne() - 0.5f) * 10.0f;

            cos = Math_CosS(thisv->dyna.actor.shape.rot.y) * rand;
            sin = Math_SinS(thisv->dyna.actor.shape.rot.y) * rand;

            thisv->dyna.actor.world.pos.x = thisv->shakeOrigin.x + cos;
            thisv->dyna.actor.world.pos.z = thisv->shakeOrigin.z + sin;

            thisv->textureBlend += 20;
            thisv->unk_258 = 1;
        }
    } else {
        if (thisv->textureBlend > 10) {
            thisv->textureBlend -= 10;
        } else {
            thisv->textureBlend = 0;
            thisv->dyna.actor.flags &= ~ACTOR_FLAG_0;

            if (thisv->dyna.bgId < 0) {
                thisv->unk_264 = 0;
            }

            thisv->dyna.actor.speedXZ += thisv->dyna.unk_150;
            thisv->shakeOrigin = thisv->dyna.actor.world.pos;
            thisv->dyna.actor.world.rot.y = thisv->dyna.unk_158;
            thisv->dyna.actor.speedXZ = CLAMP(thisv->dyna.actor.speedXZ, -2.5f, 2.5f);
            Math_SmoothStepToF(&thisv->dyna.actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

            if (thisv->dyna.actor.speedXZ != 0.0f) {
                Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
            }

            thisv->dyna.unk_154 = 0.0f;
            thisv->dyna.unk_150 = 0.0f;
        }
    }
}

/**
 * Spin toward the direction of the home position to start moving back to it.
 */
void EnAm_RotateToHome(EnAm* thisv, GlobalContext* globalCtx) {
    s16 yawToHome = Math_Vec3f_Yaw(&thisv->dyna.actor.world.pos, &thisv->dyna.actor.home.pos);

    if (thisv->skelAnime.curFrame == 8.0f) {
        Math_SmoothStepToS(&thisv->dyna.actor.world.rot.y, yawToHome, 1, 0x1F40, 0);
        thisv->dyna.actor.velocity.y = 12.0f;
    } else if (thisv->skelAnime.curFrame > 11.0f) {
        if (!(thisv->dyna.actor.bgCheckFlags & 1)) {
            thisv->skelAnime.curFrame = 11;
        } else {
            EnAm_SpawnEffects(thisv, globalCtx);

            if (thisv->dyna.actor.world.rot.y == yawToHome) {
                thisv->unk_258 = 0;
            }

            thisv->dyna.actor.velocity.y = 0.0f;
            thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
        }
    }

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_258 == 0) {
        EnAm_SetupMoveToHome(thisv);
    }

    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
}

/**
 * After reaching the home position, spin back to the starting rotation.
 */
void EnAm_RotateToInit(EnAm* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame == 8.0f) {
        if ((thisv->dyna.actor.world.pos.x == thisv->dyna.actor.home.pos.x) &&
            (thisv->dyna.actor.world.pos.z == thisv->dyna.actor.home.pos.z)) {
            Math_SmoothStepToS(&thisv->dyna.actor.world.rot.y, thisv->dyna.actor.home.rot.y, 1, 0x1F40, 0);
        }
        thisv->unk_258 = 2;
        thisv->dyna.actor.velocity.y = 12.0f;
    } else if (thisv->skelAnime.curFrame > 11.0f) {
        if (!(thisv->dyna.actor.bgCheckFlags & 1)) {
            thisv->skelAnime.curFrame = 11;
        } else {
            thisv->unk_258 = 1;
            EnAm_SpawnEffects(thisv, globalCtx);

            if (thisv->dyna.actor.home.rot.y == thisv->dyna.actor.world.rot.y) {
                thisv->unk_258 = 0;
            }

            thisv->dyna.actor.velocity.y = 0.0f;
            thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
        }
    }

    if (thisv->unk_258 == 2) {
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.x, thisv->dyna.actor.home.pos.x, 1.0f, 8.0f, 0.0f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.z, thisv->dyna.actor.home.pos.z, 1.0f, 8.0f, 0.0f);
    }

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_258 == 0) {
        EnAm_SetupSleep(thisv);
    }

    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
}

void EnAm_MoveToHome(EnAm* thisv, GlobalContext* globalCtx) {
    s16 yawToHome = Math_Vec3f_Yaw(&thisv->dyna.actor.world.pos, &thisv->dyna.actor.home.pos);

    if (thisv->skelAnime.curFrame == 8.0f) {
        thisv->dyna.actor.velocity.y = 12.0f;
        thisv->dyna.actor.speedXZ = 6.0f;
    } else if (thisv->skelAnime.curFrame > 11.0f) {
        if (!(thisv->dyna.actor.bgCheckFlags & 1)) {
            thisv->skelAnime.curFrame = 11;
        } else {
            Math_SmoothStepToS(&thisv->dyna.actor.world.rot.y, yawToHome, 1, 0xBB8, 0);

            if (thisv->dyna.actor.bgCheckFlags & 2) {
                thisv->unk_258--;
            }

            thisv->dyna.actor.velocity.y = 0.0f;
            thisv->dyna.actor.speedXZ = 0.0f;
            thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
            EnAm_SpawnEffects(thisv, globalCtx);

            if (Actor_WorldDistXYZToPoint(&thisv->dyna.actor, &thisv->dyna.actor.home.pos) < 80.0f) {
                EnAm_SetupRotateToInit(thisv);
            }
        }
    }

    // turn away from a wall if touching one
    if ((thisv->dyna.actor.speedXZ != 0.0f) && (thisv->dyna.actor.bgCheckFlags & 8)) {
        thisv->dyna.actor.world.rot.y = thisv->dyna.actor.wallYaw;
        Actor_MoveForward(&thisv->dyna.actor);
    }

    SkelAnime_Update(&thisv->skelAnime);

    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
}

void EnAm_RecoilFromDamage(EnAm* thisv, GlobalContext* globalCtx) {
    if (thisv->dyna.actor.speedXZ < 0.0f) {
        thisv->dyna.actor.speedXZ += 0.5f;
    }

    if ((thisv->dyna.actor.velocity.y <= 0.0f) && !EnAm_CanMove(thisv, globalCtx, -8.0f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnAm_SetupLunge(thisv);
        thisv->deathTimer = 64;
        thisv->panicSpinRot = 0;
    }
}

/**
 * After doing 3 lunges, wait for 2 seconds before attacking again.
 * Turn toward the player before lunging again.
 */
void EnAm_Cooldown(EnAm* thisv, GlobalContext* globalCtx) {
    s16 yawDiff = thisv->dyna.actor.yawTowardsPlayer - thisv->dyna.actor.world.rot.y;

    yawDiff = ABS(yawDiff);

    if (thisv->cooldownTimer != 0) {
        thisv->cooldownTimer--;
    } else {
        if (thisv->skelAnime.curFrame == 8.0f) {
            Math_SmoothStepToS(&thisv->dyna.actor.world.rot.y, thisv->dyna.actor.yawTowardsPlayer, 1, 0x1F40, 0);
            thisv->dyna.actor.velocity.y = 12.0f;
        } else if (thisv->skelAnime.curFrame > 11.0f) {
            if (!(thisv->dyna.actor.bgCheckFlags & 1)) {
                thisv->skelAnime.curFrame = 11;
            } else {
                if (yawDiff < 3500) {
                    thisv->unk_258 = 0;
                }
                thisv->dyna.actor.velocity.y = 0.0f;
                thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
                EnAm_SpawnEffects(thisv, globalCtx);
            }
        }

        SkelAnime_Update(&thisv->skelAnime);

        if (thisv->unk_258 == 0) {
            EnAm_SetupLunge(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_VOICE);
        }

        thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
    }
}

/**
 * Lunge toward the player in an attempt to deal damage. Hop 3 times.
 * Used for both normal attacks and the death sequence.
 */
void EnAm_Lunge(EnAm* thisv, GlobalContext* globalCtx) {
    if (thisv->deathTimer < 52) {
        if (thisv->skelAnime.curFrame == 8.0f) {
            thisv->dyna.actor.velocity.y = 12.0f;

            if (EnAm_CanMove(thisv, globalCtx, 80.0f, thisv->dyna.actor.world.rot.y)) {
                thisv->dyna.actor.speedXZ = 6.0f;
            } else {
                thisv->dyna.actor.speedXZ = 0.0f;
            }

            thisv->unk_264 = 1;
            thisv->hitCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        } else if (thisv->skelAnime.curFrame > 11.0f) {
            if (!(thisv->dyna.actor.bgCheckFlags & 1)) {
                thisv->skelAnime.curFrame = 11;
            } else {
                Math_SmoothStepToS(&thisv->dyna.actor.world.rot.y, thisv->dyna.actor.yawTowardsPlayer, 1, 0x1770, 0);

                if (thisv->dyna.actor.bgCheckFlags & 2) {
                    thisv->unk_258--;
                }

                thisv->dyna.actor.velocity.y = 0.0f;
                thisv->dyna.actor.speedXZ = 0.0f;
                thisv->unk_264 = 0;
                thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
                EnAm_SpawnEffects(thisv, globalCtx);

                if (((Actor_WorldDistXZToPoint(&thisv->dyna.actor, &thisv->dyna.actor.home.pos) > 240.0f) ||
                     (thisv->attackTimer == 0)) &&
                    (thisv->deathTimer == 0)) {
                    EnAm_SetupRotateToHome(thisv);
                }
            }
        }

        // turn and move away from a wall if contact is made with one
        if ((thisv->dyna.actor.speedXZ != 0.0f) && (thisv->dyna.actor.bgCheckFlags & 8)) {
            thisv->dyna.actor.world.rot.y =
                (thisv->dyna.actor.wallYaw - thisv->dyna.actor.world.rot.y) + thisv->dyna.actor.wallYaw;
            Actor_MoveForward(&thisv->dyna.actor);
            thisv->dyna.actor.bgCheckFlags &= ~8;
        }

        SkelAnime_Update(&thisv->skelAnime);

        if ((thisv->unk_258 == 0) && (thisv->deathTimer == 0)) {
            EnAm_SetupCooldown(thisv);
        }

        if (thisv->deathTimer == 0) {
            thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
        } else {
            if (thisv->panicSpinRot < 8000) {
                thisv->panicSpinRot += 800;
            }
            thisv->dyna.actor.shape.rot.y += thisv->panicSpinRot;
        }
    }
}

void EnAm_Statue(EnAm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 temp158f = thisv->dyna.unk_158;
    s16 moveDir = 0;

    if (thisv->unk_258 == 0) {
        if (thisv->dyna.unk_150 != 0.0f) {
            thisv->unk_258 = 0x8000;
        }
    } else {
        thisv->unk_258 -= 0x800;
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);

        if (thisv->dyna.unk_150 < 0.0f) {
            temp158f = thisv->dyna.unk_158 + 0x8000;
        }

        if (thisv->hurtCollider.base.ocFlags1 & OC1_HIT) {
            moveDir = Math_Vec3f_Yaw(&thisv->dyna.actor.world.pos, &thisv->hurtCollider.base.oc->world.pos) - temp158f;
        }

        if ((thisv->dyna.unk_150 == 0.0f) || (thisv->unk_258 == 0) || !(thisv->dyna.actor.bgCheckFlags & 1) ||
            !func_800435D8(globalCtx, &thisv->dyna, 0x14,
                           (Math_SinS(thisv->unk_258) * (thisv->dyna.unk_150 * 0.5f)) + 40.0f, 0xA) ||
            ((thisv->hurtCollider.base.ocFlags1 & OC1_HIT) && (ABS(moveDir) <= 0x2000))) {

            thisv->unk_258 = 0;
            player->stateFlags2 &= ~0x151;
            player->actor.speedXZ = 0.0f;
            thisv->dyna.unk_150 = thisv->dyna.unk_154 = 0.0f;
        }

        thisv->dyna.actor.world.rot.y = thisv->dyna.unk_158;
        thisv->dyna.actor.speedXZ = Math_SinS(thisv->unk_258) * (thisv->dyna.unk_150 * 0.5f);
    }

    if (thisv->dyna.actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    }

    thisv->dyna.unk_150 = thisv->dyna.unk_154 = 0.0f;
}

void EnAm_SetupStunned(EnAm* thisv, GlobalContext* globalCtx) {
    // animation is set but SkelAnime_Update is not called in the action
    // likely copy pasted from EnAm_SetupRecoilFromDamage
    Animation_Change(&thisv->skelAnime, &gArmosDamagedAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gArmosDamagedAnim),
                     ANIMMODE_ONCE, 0.0f);

    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.yawTowardsPlayer;

    if (EnAm_CanMove(thisv, globalCtx, -6.0f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = -6.0f;
    }

    Actor_SetColorFilter(&thisv->dyna.actor, 0, 120, 0, 100);

    if (thisv->damageEffect == AM_DMGEFF_ICE) {
        thisv->iceTimer = 48;
    }

    thisv->behavior = AM_BEHAVIOR_STUNNED;
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnAm_SetupAction(thisv, EnAm_Stunned);
}

void EnAm_Stunned(EnAm* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->dyna.actor.shape.rot.y, thisv->dyna.actor.world.rot.y, 1, 0xFA0, 0);

    if (thisv->dyna.actor.speedXZ < 0.0f) {
        thisv->dyna.actor.speedXZ += 0.5f;
    }

    if ((thisv->dyna.actor.velocity.y <= 0.0f) && !EnAm_CanMove(thisv, globalCtx, -9.0f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = 0.0f;
    }

    if (thisv->dyna.actor.colorFilterTimer == 0) {
        if (thisv->dyna.actor.colChkInfo.health != 0) {
            EnAm_SetupLunge(thisv);
        } else {
            EnAm_SetupRecoilFromDamage(thisv, globalCtx);
        }
    }
}

void EnAm_Ricochet(EnAm* thisv, GlobalContext* globalCtx) {
    if (thisv->dyna.actor.speedXZ < 0.0f) {
        thisv->dyna.actor.speedXZ += 0.5f;
    }

    if ((thisv->dyna.actor.velocity.y <= 0.0f) &&
        !EnAm_CanMove(thisv, globalCtx, thisv->dyna.actor.speedXZ * 1.5f, thisv->dyna.actor.world.rot.y)) {
        thisv->dyna.actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->dyna.actor.speedXZ = 0.0f;
        EnAm_SetupLunge(thisv);
    }
}

void EnAm_TransformSwordHitbox(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f D_809B0074 = { 2500.0f, 7000.0f, 0.0f };
    static Vec3f D_809B0080 = { -2500.0f, 0.0f, 0.0f };
    static Vec3f D_809B008C = { 2500.0f, 7000.0f, 4000.0f };
    static Vec3f D_809B0098 = { -2500.0f, 0.0f, 4000.0f };
    EnAm* thisv = (EnAm*)thisx;

    Matrix_MultVec3f(&D_809B0074, &thisv->hitCollider.dim.quad[1]);
    Matrix_MultVec3f(&D_809B0080, &thisv->hitCollider.dim.quad[0]);
    Matrix_MultVec3f(&D_809B008C, &thisv->hitCollider.dim.quad[3]);
    Matrix_MultVec3f(&D_809B0098, &thisv->hitCollider.dim.quad[2]);

    Collider_SetQuadVertices(&thisv->hitCollider, &thisv->hitCollider.dim.quad[0], &thisv->hitCollider.dim.quad[1],
                             &thisv->hitCollider.dim.quad[2], &thisv->hitCollider.dim.quad[3]);
}

void EnAm_UpdateDamage(EnAm* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sparkPos;

    if (thisv->deathTimer == 0) {
        if (thisv->blockCollider.base.acFlags & AC_BOUNCED) {
            thisv->blockCollider.base.acFlags &= ~(AC_HIT | AC_BOUNCED);
            thisv->hurtCollider.base.acFlags &= ~AC_HIT;

            if (thisv->behavior >= AM_BEHAVIOR_5) {
                EnAm_SetupRicochet(thisv, globalCtx);
            }
        } else if ((thisv->hurtCollider.base.acFlags & AC_HIT) && (thisv->behavior >= AM_BEHAVIOR_5)) {
            thisv->hurtCollider.base.acFlags &= ~AC_HIT;

            if (thisv->dyna.actor.colChkInfo.damageEffect != AM_DMGEFF_MAGIC_FIRE_LIGHT) {
                thisv->unk_264 = 0;
                thisv->damageEffect = thisv->dyna.actor.colChkInfo.damageEffect;
                Actor_SetDropFlag(&thisv->dyna.actor, &thisv->hurtCollider.info, 0);

                if ((thisv->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_NUT) ||
                    (thisv->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_STUN) ||
                    (thisv->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_ICE)) {
                    if (thisv->behavior != AM_BEHAVIOR_STUNNED) {
                        EnAm_SetupStunned(thisv, globalCtx);

                        if (thisv->dyna.actor.colChkInfo.damage != 0) {
                            thisv->dyna.actor.colChkInfo.health = 0;
                        }
                    } else if (thisv->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_STUN) {
                        sparkPos = thisv->dyna.actor.world.pos;
                        sparkPos.y += 50.0f;
                        CollisionCheck_SpawnShieldParticlesMetal(globalCtx, &sparkPos);
                    }
                } else if ((thisv->dyna.actor.colChkInfo.damageEffect == AM_DMGEFF_KILL) ||
                           (thisv->behavior == AM_BEHAVIOR_STUNNED)) {
                    thisv->dyna.actor.colChkInfo.health = 0;

                    EnAm_SetupRecoilFromDamage(thisv, globalCtx);
                } else {
                    EnAm_SetupRicochet(thisv, globalCtx);
                }
            }
        }
    }
}

void EnAm_Update(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 dustPrimColor = { 150, 150, 150, 255 };
    static Color_RGBA8 dustEnvColor = { 100, 100, 100, 150 };
    s32 pad;
    EnAm* thisv = (EnAm*)thisx;
    EnBom* bomb;
    Vec3f dustPos;
    s32 i;
    f32 dustPosScale;
    s32 pad1;

    if (thisv->dyna.actor.params != ARMOS_STATUE) {
        EnAm_UpdateDamage(thisv, globalCtx);
    }

    if (thisv->dyna.actor.colChkInfo.damageEffect != AM_DMGEFF_MAGIC_FIRE_LIGHT) {
        if (thisv->attackTimer != 0) {
            thisv->attackTimer--;
        }

        thisv->actionFunc(thisv, globalCtx);

        if (thisv->deathTimer != 0) {
            thisv->deathTimer--;

            if (thisv->deathTimer == 0) {
                dustPosScale = globalCtx->gameplayFrames * 10;

                EnAm_SpawnEffects(thisv, globalCtx);
                bomb =
                    (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->dyna.actor.world.pos.x,
                                        thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z, 0, 0, 2, BOMB_BODY);
                if (bomb != NULL) {
                    bomb->timer = 0;
                }

                Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EN_AMOS_DEAD);
                Item_DropCollectibleRandom(globalCtx, &thisv->dyna.actor, &thisv->dyna.actor.world.pos, 0xA0);

                for (i = 9; i >= 0; i--) {
                    dustPos.x = (sinf(dustPosScale) * 7.0f) + thisv->dyna.actor.world.pos.x;
                    dustPos.y = (Rand_CenteredFloat(10.0f) * 6.0f) + (thisv->dyna.actor.world.pos.y + 40.0f);
                    dustPos.z = (cosf(dustPosScale) * 7.0f) + thisv->dyna.actor.world.pos.z;

                    func_8002836C(globalCtx, &dustPos, &zeroVec, &zeroVec, &dustPrimColor, &dustEnvColor, 200, 45, 12);
                    dustPosScale += 60.0f;
                }

                Actor_Kill(&thisv->dyna.actor);
                return;
            }

            if ((thisv->deathTimer % 4) == 0) {
                Actor_SetColorFilter(&thisv->dyna.actor, 0x4000, 255, 0, 4);
            }
        }

        Actor_MoveForward(&thisv->dyna.actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 20.0f, 28.0f, 80.0f, 0x1D);
    }

    Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->hurtCollider);
    Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->blockCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->hurtCollider.base);

    if (thisv->dyna.actor.params != ARMOS_STATUE) {
        Actor_SetFocus(&thisv->dyna.actor, thisv->dyna.actor.scale.x * 4500.0f);

        if (thisv->dyna.actor.colorFilterTimer == 0) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->blockCollider.base);
        }

        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->hurtCollider.base);

        if ((thisv->behavior >= 4) && (thisv->unk_264 > 0)) {
            if (!(thisv->hitCollider.base.atFlags & AT_BOUNCED)) {
                if (thisv->hitCollider.base.atFlags & AT_HIT) {
                    Player* player = GET_PLAYER(globalCtx);

                    if (thisv->hitCollider.base.at == &player->actor) {
                        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                    }
                }
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->hitCollider.base);
            } else {
                thisv->hitCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
                thisv->hitCollider.base.at = NULL;
                EnAm_SetupRicochet(thisv, globalCtx);
            }
        }
    } else {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->blockCollider.base);
    }
}

static Vec3f sUnused1 = { 1100.0f, -700.0f, 0.0f };
static Vec3f sUnused2 = { 0.0f, 0.0f, 0.0f };

void EnAm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnAm* thisv = (EnAm*)thisx;

    if ((limbIndex == 1) && (thisv->unk_264 != 0)) {
        EnAm_TransformSwordHitbox(&thisv->dyna.actor, globalCtx);
    }
}

static Vec3f sIcePosOffsets[] = {
    { 20.0f, 40.0f, 0.0f },   { 10.0f, 60.0f, 10.0f },   { -10.0f, 60.0f, 10.0f }, { -20.0f, 40.0f, 0.0f },
    { 10.0f, 60.0f, -10.0f }, { -10.0f, 60.0f, -10.0f }, { 0.0f, 40.0f, -20.0f },  { 10.0f, 20.0f, 10.0f },
    { 10.0f, 20.0f, -10.0f }, { 0.0f, 40.0f, 20.0f },    { -10.0f, 20.0f, 10.0f }, { -10.0f, 20.0f, -10.0f },
};

void EnAm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp68;
    EnAm* thisv = (EnAm*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_am.c", 1580);

    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, thisv->textureBlend);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, EnAm_PostLimbDraw, thisv);

    if (thisv->iceTimer != 0) {
        thisv->dyna.actor.colorFilterTimer++;
        if (1) {};
        thisv->iceTimer--;

        if ((thisv->iceTimer % 4) == 0) {
            s32 index;

            index = thisv->iceTimer >> 2;

            sp68.x = thisv->dyna.actor.world.pos.x + sIcePosOffsets[index].x;
            sp68.y = thisv->dyna.actor.world.pos.y + sIcePosOffsets[index].y;
            sp68.z = thisv->dyna.actor.world.pos.z + sIcePosOffsets[index].z;

            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->dyna.actor, &sp68, 150, 150, 150, 250, 235, 245, 255,
                                           1.4f);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_am.c", 1605);
}
