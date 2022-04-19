/*
 * File: z_boss_ganondrof.c
 * Overlay: ovl_Boss_Ganondrof
 * Description: Phantom Ganon
 */

#include "z_boss_ganondrof.h"
#include "objects/object_gnd/object_gnd.h"
#include "overlays/actors/ovl_En_fHG/z_en_fhg.h"
#include "overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ NOT_DEAD,
    /* 1 */ DEATH_START,
    /* 2 */ DEATH_THROES,
    /* 3 */ DEATH_WARP,
    /* 4 */ DEATH_SCREAM,
    /* 5 */ DEATH_DISINTEGRATE,
    /* 6 */ DEATH_FINISH
} BossGanondrofDeathState;

typedef enum {
    /* 0 */ THROW_NORMAL,
    /* 1 */ THROW_SLOW
} BossGanondrofThrowAction;

typedef enum {
    /* 0 */ STUNNED_FALL,
    /* 1 */ STUNNED_GROUND
} BossGanondrofStunnedAction;

typedef enum {
    /* 0 */ CHARGE_WINDUP,
    /* 1 */ CHARGE_START,
    /* 2 */ CHARGE_ATTACK,
    /* 3 */ CHARGE_FINISH
} BossGanondrofChargeAction;

typedef enum {
    /* 0 */ DEATH_SPASM,
    /* 1 */ DEATH_LIMP,
    /* 2 */ DEATH_HUNCHED
} BossGanondrofDeathAction;

void BossGanondrof_Init(Actor* thisx, GlobalContext* globalCtx);
void BossGanondrof_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossGanondrof_Update(Actor* thisx, GlobalContext* globalCtx);
void BossGanondrof_Draw(Actor* thisx, GlobalContext* globalCtx);

void BossGanondrof_SetupIntro(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Intro(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupPaintings(BossGanondrof* thisv);
void BossGanondrof_Paintings(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupNeutral(BossGanondrof* thisv, f32 arg1);
void BossGanondrof_Neutral(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupThrow(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Throw(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupBlock(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Block(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupReturn(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Return(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_SetupCharge(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Charge(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Stunned(BossGanondrof* thisv, GlobalContext* globalCtx);
void BossGanondrof_Death(BossGanondrof* thisv, GlobalContext* globalCtx);

const ActorInit Boss_Ganondrof_InitVars = {
    ACTOR_BOSS_GANONDROF,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GND,
    sizeof(BossGanondrof),
    (ActorFunc)BossGanondrof_Init,
    (ActorFunc)BossGanondrof_Destroy,
    (ActorFunc)BossGanondrof_Update,
    (ActorFunc)BossGanondrof_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInitBody = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 90, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitSpear = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x30 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, -20, { 0, 0, 0 } },
};

// clang-format off
static u8 sDecayMaskHigh[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
    1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,1,
    1,0,1,1,1,0,0,0,0,1,1,1,1,1,0,1,
    1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,
    1,0,0,1,1,1,1,1,0,0,0,1,1,0,0,1,
    1,0,1,1,1,1,0,0,0,0,0,1,0,0,0,1,
    1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,
    1,0,1,1,1,1,1,0,0,0,0,1,1,1,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,
    1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,
    1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,
    1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1,
    1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

static u8 sDecayMaskLow[16 * 16] = {
    1,1,1,0,1,0,0,1,0,0,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,
    1,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,
    1,0,0,1,1,0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,1,
    0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,
    1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    1,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,
    1,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,
    0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
    1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,
};

static u8 sDecayMaskTotal[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
// clang-format on

// These are Phantom Ganon's body textures, but I don't know which is which.
static const void* sLimbTex_rgba16_8x8[] = {
    gPhantomGanonLimbTex_00A800, gPhantomGanonLimbTex_00AE80, gPhantomGanonLimbTex_00AF00,
    gPhantomGanonLimbTex_00C180, gPhantomGanonLimbTex_00C400,
};
static const void* sLimbTex_rgba16_16x8[] = {
    gPhantomGanonLimbTex_00B980, gPhantomGanonLimbTex_00C480, gPhantomGanonLimbTex_00BC80,
    gPhantomGanonLimbTex_00BD80, gPhantomGanonLimbTex_00C080,
};
static const void* sLimbTex_rgba16_16x16[] = {
    gPhantomGanonLimbTex_00C200, gPhantomGanonLimbTex_00A000, gPhantomGanonLimbTex_00A200,
    gPhantomGanonLimbTex_00A400, gPhantomGanonLimbTex_00A600, gPhantomGanonLimbTex_00A880,
    gPhantomGanonLimbTex_00B780, gPhantomGanonLimbTex_00BA80, gPhantomGanonLimbTex_00BE80,
};
static const void* sLimbTex_rgba16_16x32[] = { gPhantomGanonLimbTex_00AA80, gPhantomGanonLimbTex_00AF80 };

static const void* sMouthTex_ci8_16x16[] = { gPhantomGanonMouthTex, gPhantomGanonSmileTex };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x2B, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

static Vec3f sAudioVec = { 0.0f, 0.0f, 50.0f };

// OTRTODO: This code appears to cause the game to gradually crash...
// Might be an OoB write. For now it's disabled.
void BossGanondrof_ClearPixels8x8(const s16* texture, u8* mask, s16 index) 
{
    //texture = ResourceMgr_LoadTexByName(texture);
    if (mask[index]) {
        //texture[index / 4] = 0;
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), index / 4, 0);
    }
}

void BossGanondrof_ClearPixels16x8(const s16* texture, u8* mask, s16 index) {
    //texture = ResourceMgr_LoadTexByName(texture);
    if (mask[index]) {
        //texture[index / 2] = 0;
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), index / 2, 0);

    }
}

void BossGanondrof_ClearPixels16x16(const s16* texture, u8* mask, s16 index, s16 bpp) {
    //texture = ResourceMgr_LoadTexByName(texture);
    if (mask[index]) {
        //texture[index] = 0;
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), index, 0);
    }
}

void BossGanondrof_ClearPixels32x16(const s16* texture, u8* mask, s16 index) {
    //texture = ResourceMgr_LoadTexByName(texture);
    if (mask[index]) {
        s16 i = (index & 0xF) + ((index & 0xF0) << 1);

        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), i + 0x10, 0);
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), i, 0);
        //texture[i + 0x10] = 0;
        //texture[i] = 0;
    }
}

void BossGanondrof_ClearPixels16x32(const s16* texture, u8* mask, s16 index) { 
    //texture = ResourceMgr_LoadTexByName(texture);
    if (mask[index]) {
        s16 i = ((index & 0xF) * 2) + ((index & 0xF0) * 2);
        
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), i + 1, 0);
        ResourceMgr_WriteTexS16ByName(reinterpret_cast<const char*>(texture), i, 0);

        //texture[i + 1] = 0;
        //texture[i] = 0;
    }
    
}

void BossGanondrof_ClearPixels(u8* mask, s16 index) {
    s16 i;

    for (i = 0; i < 5; i++) {
        // ARRAY_COUNT can't be used here because the arrays aren't guaranteed to be the same size.
        BossGanondrof_ClearPixels8x8(static_cast<const s16*>( SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_8x8[i]) ), mask, index);
        BossGanondrof_ClearPixels16x8(static_cast<const s16*>( SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x8[i]) ), mask, index);
    }

    for (i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x16); i++) {
        BossGanondrof_ClearPixels16x16(static_cast<const s16*>(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x16[i])), mask, index, 2);
    }

    for (i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x32); i++) {
        BossGanondrof_ClearPixels16x32(static_cast<const s16*>(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x32[i])), mask, index);
    }

    BossGanondrof_ClearPixels32x16(reinterpret_cast<const s16*>(SEGMENTED_TO_VIRTUAL(gPhantomGanonLimbTex_00B380)), mask, index);
    BossGanondrof_ClearPixels16x32(reinterpret_cast<const s16*>(SEGMENTED_TO_VIRTUAL(gPhantomGanonEyeTex)), mask, index);
    for (i = 0; i < ARRAY_COUNT(sMouthTex_ci8_16x16); i++) {
        BossGanondrof_ClearPixels16x16(static_cast<const s16*>(SEGMENTED_TO_VIRTUAL(sMouthTex_ci8_16x16[i])), mask, index, 1);
    }
}

void BossGanondrof_SetColliderPos(Vec3f* pos, ColliderCylinder* collider) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

void BossGanondrof_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossGanondrof* thisv = (BossGanondrof*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPhantomGanonSkel, &gPhantomGanonRideAnim, NULL, NULL, 0);
    if (thisv->actor.params < GND_FAKE_BOSS) {
        thisv->actor.params = GND_REAL_BOSS;
        thisv->actor.colChkInfo.health = 30;
        thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                  thisv->actor.world.pos.z, 255, 255, 255, 255);
        BossGanondrof_SetupIntro(thisv, globalCtx);
    } else {
        BossGanondrof_SetupPaintings(thisv);
    }

    Collider_InitCylinder(globalCtx, &thisv->colliderBody);
    Collider_InitCylinder(globalCtx, &thisv->colliderSpear);
    Collider_SetCylinder(globalCtx, &thisv->colliderBody, &thisv->actor, &sCylinderInitBody);
    Collider_SetCylinder(globalCtx, &thisv->colliderSpear, &thisv->actor, &sCylinderInitSpear);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
        Actor_Kill(&thisv->actor);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                    GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, 200.0f + GND_BOSSROOM_CENTER_X,
                    GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z, 0, 0, 0, 0);
    } else {
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG, thisv->actor.world.pos.x,
                           thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, thisv->actor.params);
    }
}

void BossGanondrof_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossGanondrof* thisv = (BossGanondrof*)thisx;

    osSyncPrintf("DT1\n");
    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderBody);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderSpear);
    if (thisv->actor.params == GND_REAL_BOSS) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    }

    osSyncPrintf("DT2\n");
}

void BossGanondrof_SetupIntro(BossGanondrof* thisv, GlobalContext* globalCtx) {
    Animation_PlayLoop(&thisv->skelAnime, &gPhantomGanonRidePoseAnim);
    thisv->actionFunc = BossGanondrof_Intro;
    thisv->work[GND_MASK_OFF] = true;
}

void BossGanondrof_Intro(BossGanondrof* thisv, GlobalContext* globalCtx) {
    s16 i;
    s32 pad;
    EnfHG* horse = (EnfHG*)thisv->actor.child;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.world.pos = horse->actor.world.pos;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = horse->actor.world.rot.y;

    osSyncPrintf("SW %d------------------------------------------------\n", horse->bossGndSignal);

    if ((thisv->timers[1] != 0) && (thisv->timers[1] < 25)) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        pos.x = thisv->bodyPartsPos[14].x + Rand_CenteredFloat(10.0f);
        pos.y = thisv->bodyPartsPos[14].y + Rand_ZeroFloat(-5.0f);
        pos.z = thisv->bodyPartsPos[14].z + Rand_CenteredFloat(10.0f) + 5.0f;
        accel.y = 0.03f;
        EffectSsKFire_Spawn(globalCtx, &pos, &vel, &accel, (s16)Rand_ZeroFloat(10.0f) + 5, 0);
    }

    if (thisv->timers[1] == 20) {
        thisv->work[GND_MASK_OFF] = false;
    }

    if (thisv->timers[1] == 30) {
        func_80078914(&sAudioVec, NA_SE_EN_FANTOM_TRANSFORM);
    }

    if (horse->bossGndSignal == FHG_LIGHTNING) {
        Animation_Change(&thisv->skelAnime, &gPhantomGanonMaskOnAnim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gPhantomGanonMaskOnAnim), ANIMMODE_ONCE_INTERP, 0.0f);
        thisv->timers[1] = 40;
    }

    if (horse->bossGndSignal == FHG_REAR) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonHorseRearingAnim, -3.0f);
    }

    if (horse->bossGndSignal == FHG_RIDE) {
        Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonRidePoseAnim, -13.0f);
    }

    if (horse->bossGndSignal == FHG_SPUR) {
        EnfHG* horseTemp;

        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -7.0f);
        horseTemp = (EnfHG*)thisv->actor.child;
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->spearTip.x,
                           thisv->spearTip.y, thisv->spearTip.z, 50, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        thisv->actor.child = &horseTemp->actor;
    }

    if (horse->bossGndSignal == FHG_FINISH) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonRideSpearResetAnim, -5.0f);
    }

    switch (thisv->work[GND_EYE_STATE]) {
        case GND_EYESTATE_FADE:
            thisv->fwork[GND_EYE_ALPHA] += 40.0f;
            if (thisv->fwork[GND_EYE_ALPHA] >= 255.0f) {
                thisv->fwork[GND_EYE_ALPHA] = 255.0f;
            }
            break;

        case GND_EYESTATE_BRIGHTEN:
            thisv->fwork[GND_EYE_BRIGHTNESS] += 20.0f;
            if (thisv->fwork[GND_EYE_BRIGHTNESS] > 255.0f) {
                thisv->fwork[GND_EYE_BRIGHTNESS] = 255.0f;
            }
            break;
    }

    thisv->armRotY = Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x6E8) * 0;
    thisv->armRotZ = Math_CosS(thisv->work[GND_VARIANCE_TIMER] * 0x8DC) * 300.0f;
    for (i = 0; i < 30; i++) {
        thisv->rideRotY[i] = Math_SinS(thisv->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x7B0)) * 100.0f;
        thisv->rideRotZ[i] = Math_CosS(thisv->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x8DC)) * 100.0f;
    }

    if (horse->bossGndSignal == FHG_START_FIGHT) {
        BossGanondrof_SetupPaintings(thisv);
        for (i = 0; i < 30; i++) {
            thisv->rideRotY[i] = thisv->rideRotZ[i] = 0.0f;
        }
    }

    horse->bossGndSignal = FHG_NO_SIGNAL;
}

void BossGanondrof_SetupPaintings(BossGanondrof* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonRideAnim, -5.0f);
    thisv->actionFunc = BossGanondrof_Paintings;
}

void BossGanondrof_Paintings(BossGanondrof* thisv, GlobalContext* globalCtx) {
    EnfHG* horse = (EnfHG*)thisv->actor.child;

    osSyncPrintf("RUN 1\n");
    SkelAnime_Update(&thisv->skelAnime);
    osSyncPrintf("RUN 2\n");

    if (horse->bossGndSignal == FHG_RAISE_SPEAR) {
        EnfHG* horseTemp;

        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -2.0f);
        thisv->actor.flags |= ACTOR_FLAG_0;
        horseTemp = (EnfHG*)thisv->actor.child;
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->spearTip.x,
                           thisv->spearTip.y, thisv->spearTip.z, 30, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        thisv->actor.child = &horseTemp->actor;
    } else if (horse->bossGndSignal == FHG_LIGHTNING) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonRideSpearStrikeAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RESET) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonRideSpearResetAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RIDE) {
        Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonRideAnim, -2.0f);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }

    osSyncPrintf("RUN 3\n");
    thisv->actor.world.pos = horse->actor.world.pos;
    thisv->actor.world.pos.y = horse->actor.world.pos.y;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = horse->actor.world.rot.y;
    if (thisv->flyMode != GND_FLY_PAINTING) {
        BossGanondrof_SetupNeutral(thisv, -20.0f);
        thisv->timers[0] = 100;
        thisv->colliderBody.dim.radius = 20;
        thisv->colliderBody.dim.height = 60;
        thisv->colliderBody.dim.yShift = -33;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_LAUGH);
        thisv->actor.naviEnemyId = 0x1A;
    } else {
        horse->bossGndSignal = FHG_NO_SIGNAL;
        thisv->actor.scale.x = horse->actor.scale.x / 1.15f;
        thisv->actor.scale.y = horse->actor.scale.y / 1.15f;
        thisv->actor.scale.z = horse->actor.scale.z / 1.15f;
        osSyncPrintf("RUN 4\n");
    }
}

void BossGanondrof_SetupNeutral(BossGanondrof* thisv, f32 arg1) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonNeutralAnim, arg1);
    thisv->actionFunc = BossGanondrof_Neutral;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->fwork[GND_FLOAT_SPEED] = 0.0f;
    thisv->timers[0] = (s16)(Rand_ZeroOne() * 64.0f) + 30;
}

void BossGanondrof_Neutral(BossGanondrof* thisv, GlobalContext* globalCtx) {
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    Player* player = GET_PLAYER(globalCtx);
    Actor* playerx = &player->actor;
    Actor* thisx = &thisv->actor;
    f32 rand01;
    s16 i;

    SkelAnime_Update(&thisv->skelAnime);
    switch (thisv->flyMode) {
        case GND_FLY_NEUTRAL:
            if (thisv->timers[0] == 0) {
                thisv->timers[0] = (s16)(Rand_ZeroOne() * 64.0f) + 30;
                rand01 = Rand_ZeroOne();
                if (thisx->colChkInfo.health < 5) {
                    if (rand01 < 0.25f) {
                        BossGanondrof_SetupThrow(thisv, globalCtx);
                    } else if (rand01 >= 0.8f) {
                        thisv->flyMode = GND_FLY_CHARGE;
                        thisv->timers[0] = 60;
                        thisv->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                    } else {
                        thisv->flyMode = GND_FLY_VOLLEY;
                        thisv->timers[0] = 60;
                        thisv->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                    }
                } else if ((rand01 < 0.5f) || (thisv->work[GND_THROW_COUNT] < 5)) {
                    BossGanondrof_SetupThrow(thisv, globalCtx);
                } else {
                    thisv->flyMode = GND_FLY_VOLLEY;
                    thisv->timers[0] = 60;
                    thisv->fwork[GND_FLOAT_SPEED] = 0.0f;
                    Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                }
            }

            if (thisv->timers[1] != 0) {
                targetX = GND_BOSSROOM_CENTER_X;
                targetZ = GND_BOSSROOM_CENTER_Z;
            } else {
                targetX = playerx->world.pos.x + (180.0f * Math_SinS(playerx->shape.rot.y));
                targetZ = playerx->world.pos.z + (180.0f * Math_CosS(playerx->shape.rot.y));
                if (sqrtf(SQ(targetX - GND_BOSSROOM_CENTER_X) + SQ(targetZ - GND_BOSSROOM_CENTER_Z)) > 280.0f) {
                    thisv->timers[1] = 50;
                    thisv->fwork[GND_FLOAT_SPEED] = 0.0f;
                }
            }

            targetY = playerx->world.pos.y + 100.0f + 0.0f;
            targetX += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(thisv->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            break;
        case GND_FLY_VOLLEY:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(thisv->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (thisv->timers[0] == 0) {
                thisv->flyMode = GND_FLY_RETURN;
                thisv->returnSuccess = false;
                BossGanondrof_SetupThrow(thisv, globalCtx);
                thisv->timers[0] = 80;
            }
            break;
        case GND_FLY_RETURN:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x500) * 50.0f;
            targetZ += Math_CosS(thisv->work[GND_VARIANCE_TIMER] * 0x700) * 50.0f;
            if (thisv->returnSuccess) {
                thisv->returnSuccess = false;
                BossGanondrof_SetupReturn(thisv, globalCtx);
                thisv->timers[0] = 80;
            }

            if (thisv->timers[0] == 0) {
                thisv->flyMode = GND_FLY_NEUTRAL;
            }
            break;
        case GND_FLY_CHARGE:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 215.0f;

            targetY = playerx->world.pos.y + 100.0f + 50.0f;
            targetX += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(thisv->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (thisv->timers[0] == 0) {
                BossGanondrof_SetupCharge(thisv, globalCtx);
            }
            break;
    }

    Math_ApproachF(&thisx->world.pos.x, targetX, 0.05f, thisv->fwork[GND_FLOAT_SPEED]);
    if (thisv->timers[2] != 0) {
        Math_ApproachF(&thisx->world.pos.y, targetY + 100.0f, 0.1f, 50.0f);
    } else {
        Math_ApproachF(&thisx->world.pos.y, targetY, 0.05f, 10.0f);
    }

    Math_ApproachF(&thisx->world.pos.z, targetZ, 0.05f, thisv->fwork[GND_FLOAT_SPEED]);
    Math_ApproachF(&thisv->fwork[GND_FLOAT_SPEED], 50.0f, 1.0f, 0.5f);
    thisx->velocity.x = thisx->world.pos.x - thisx->prevPos.x;
    thisx->velocity.z = thisx->world.pos.z - thisx->prevPos.z;
    thisx->world.pos.y += 2.0f * Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
    Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0xBB8);
    if ((thisv->work[GND_VARIANCE_TIMER] & 1) == 0) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        for (i = 0; i < 3; i++) {
            pos.x = Rand_CenteredFloat(20.0f) + thisv->spearTip.x;
            pos.y = Rand_CenteredFloat(20.0f) + thisv->spearTip.y;
            pos.z = Rand_CenteredFloat(20.0f) + thisv->spearTip.z;
            accel.y = -0.08f;
            EffectSsFhgFlash_SpawnLightBall(globalCtx, &pos, &vel, &accel, (s16)(Rand_ZeroOne() * 80.0f) + 150,
                                            FHGFLASH_LIGHTBALL_GREEN);
        }
    }

    if (player->unk_A73 != 0) {
        BossGanondrof_SetupBlock(thisv, globalCtx);
    }

    Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
}

void BossGanondrof_SetupThrow(BossGanondrof* thisv, GlobalContext* globalCtx) {
    EnfHG* horseTemp;
    s16 lightTime;

    thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonThrowAnim);
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonThrowAnim, -5.0f);
    thisv->actionFunc = BossGanondrof_Throw;
    if ((Rand_ZeroOne() <= 0.1f) && (thisv->work[GND_THROW_COUNT] >= 10) && (thisv->flyMode == GND_FLY_NEUTRAL)) {
        thisv->work[GND_ACTION_STATE] = THROW_SLOW;
        thisv->work[GND_THROW_FRAME] = 1000;
        lightTime = 32;
    } else {
        thisv->work[GND_ACTION_STATE] = THROW_NORMAL;
        thisv->work[GND_THROW_FRAME] = 25;
        lightTime = 25;
    }

    horseTemp = (EnfHG*)thisv->actor.child;
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->spearTip.x,
                       thisv->spearTip.y, thisv->spearTip.z, lightTime, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
    thisv->actor.child = &horseTemp->actor;
    thisv->work[GND_THROW_COUNT]++;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_STICK);
}

void BossGanondrof_Throw(BossGanondrof* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    osSyncPrintf("thisv->fwork[GND_END_FRAME] = %d\n", (s16)thisv->fwork[GND_END_FRAME]);
    osSyncPrintf("thisv->work[GND_SHOT_FRAME] = %d\n", thisv->work[GND_THROW_FRAME]);
    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
        BossGanondrof_SetupNeutral(thisv, -6.0f);
    }

    if ((thisv->work[GND_ACTION_STATE] != THROW_NORMAL) && Animation_OnFrame(&thisv->skelAnime, 21.0f)) {
        thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonThrowEndAnim);
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonThrowEndAnim, 0.0f);
        thisv->work[GND_THROW_FRAME] = 10;
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->work[GND_THROW_FRAME])) {
        if (thisv->flyMode <= GND_FLY_NEUTRAL) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_MASIC2);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_MASIC1);
        }

        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_VOICE);
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->work[GND_THROW_FRAME])) {
        EnfHG* horseTemp = (EnfHG*)thisv->actor.child;

        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->spearTip.x,
                           thisv->spearTip.y, thisv->spearTip.z, thisv->work[GND_ACTION_STATE], 0, 0, FHGFIRE_ENERGY_BALL);
        thisv->actor.child = &horseTemp->actor;
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 0x7D0);
    thisv->actor.world.pos.x += thisv->actor.velocity.x;
    thisv->actor.world.pos.z += thisv->actor.velocity.z;
    Math_ApproachZeroF(&thisv->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&thisv->actor.velocity.z, 1.0f, 0.5f);
    thisv->actor.world.pos.y += 2.0f * Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
}

void BossGanondrof_SetupReturn(BossGanondrof* thisv, GlobalContext* globalCtx) {
    static const AnimationHeader* returnAnim[] = { &gPhantomGanonReturn1Anim, &gPhantomGanonReturn2Anim };
    s16 rand = Rand_ZeroOne() * 1.99f;

    thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(returnAnim[rand]);
    Animation_MorphToPlayOnce(&thisv->skelAnime, returnAnim[rand], 0.0f);
    thisv->actionFunc = BossGanondrof_Return;
}

void BossGanondrof_Return(BossGanondrof* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 5.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_VOICE);
        osSyncPrintf("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        osSyncPrintf("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
        BossGanondrof_SetupNeutral(thisv, 0.0f);
    }

    thisv->actor.world.pos.x += thisv->actor.velocity.x;
    thisv->actor.world.pos.z += thisv->actor.velocity.z;
    Math_ApproachZeroF(&thisv->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&thisv->actor.velocity.z, 1.0f, 0.5f);
    thisv->actor.world.pos.y += 2.0f * Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
    if (thisv->returnSuccess) {
        thisv->returnSuccess = false;
        BossGanondrof_SetupReturn(thisv, globalCtx);
        thisv->timers[0] = 80;
    }
}

void BossGanondrof_SetupStunned(BossGanondrof* thisv, GlobalContext* globalCtx) {
    if (thisv->actionFunc != BossGanondrof_Stunned) {
        thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonAirDamageAnim);
        Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonAirDamageAnim, 0.0f);
        thisv->timers[0] = 50;
        thisv->shockTimer = 60;
    } else {
        thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonGroundDamageAnim);
        Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonGroundDamageAnim, 0.0f);
    }

    thisv->actionFunc = BossGanondrof_Stunned;
    thisv->work[GND_ACTION_STATE] = STUNNED_FALL;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.z = 0.0f;
}

void BossGanondrof_Stunned(BossGanondrof* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("DAMAGE   .................................\n");
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.gravity = -0.2f;
    if (thisv->actor.world.pos.y <= 5.0f) {
        if (thisv->work[GND_ACTION_STATE] == STUNNED_FALL) {
            thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonStunnedAnim);
            Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonStunnedAnim, -10.0f);
            thisv->work[GND_ACTION_STATE] = STUNNED_GROUND;
        }

        thisv->actor.velocity.y = 0.0f;
        thisv->actor.gravity = 0.0f;
        if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_DAMAGE2);
        }

        thisv->actor.flags |= ACTOR_FLAG_10;
    }

    osSyncPrintf("TIME0 %d ********************************************\n", thisv->timers[0]);
    if (thisv->timers[0] == 0) {
        BossGanondrof_SetupNeutral(thisv, -5.0f);
        thisv->timers[0] = 30;
        thisv->timers[2] = 30;
        thisv->flyMode = GND_FLY_NEUTRAL;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.gravity = 0.0f;
    }

    Actor_MoveForward(&thisv->actor);
}

void BossGanondrof_SetupBlock(BossGanondrof* thisv, GlobalContext* globalCtx) {
    thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonBlockAnim);
    Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonBlockAnim, -3.0f);
    thisv->actionFunc = BossGanondrof_Block;
    thisv->timers[0] = 10;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_STICK);
}

void BossGanondrof_Block(BossGanondrof* thisv, GlobalContext* globalCtx) {
    thisv->colliderBody.base.colType = COLTYPE_METAL;
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.world.pos.x += thisv->actor.velocity.x;
    thisv->actor.world.pos.z += thisv->actor.velocity.z;
    Math_ApproachZeroF(&thisv->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&thisv->actor.velocity.z, 1.0f, 0.5f);
    thisv->actor.world.pos.y += 2.0f * Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
    if (thisv->timers[0] == 0) {
        BossGanondrof_SetupNeutral(thisv, -5.0f);
        thisv->timers[0] = 10;
        thisv->flyMode = GND_FLY_NEUTRAL;
    }
}

void BossGanondrof_SetupCharge(BossGanondrof* thisv, GlobalContext* globalCtx) {
    thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeWindupAnim);
    Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonChargeWindupAnim, -3.0f);
    thisv->actionFunc = BossGanondrof_Charge;
    thisv->timers[0] = 20;
    thisv->work[GND_ACTION_STATE] = CHARGE_WINDUP;
}

void BossGanondrof_Charge(BossGanondrof* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* playerx = &player->actor;
    Actor* thisx = &thisv->actor;
    f32 dxCenter = thisx->world.pos.x - GND_BOSSROOM_CENTER_X;
    f32 dzCenter = thisx->world.pos.z - GND_BOSSROOM_CENTER_Z;

    thisv->colliderBody.base.colType = COLTYPE_METAL;
    SkelAnime_Update(&thisv->skelAnime);
    switch (thisv->work[GND_ACTION_STATE]) {
        case CHARGE_WINDUP:
            if (thisv->timers[0] == 218) {
                Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_STICK);
            }

            if (thisv->timers[0] == 19) {
                Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_ATTACK);
            }

            thisx->world.pos.x += thisx->velocity.x;
            thisx->world.pos.z += thisx->velocity.z;
            Math_ApproachZeroF(&thisx->velocity.x, 1.0f, 0.5f);
            Math_ApproachZeroF(&thisx->velocity.z, 1.0f, 0.5f);
            if (thisv->timers[0] == 0) {
                thisv->work[GND_ACTION_STATE] = CHARGE_START;
                thisv->timers[0] = 10;
                thisx->speedXZ = 0.0f;
                thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeStartAnim);
                Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonChargeStartAnim, 0.0f);
            }

            Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
            break;
        case CHARGE_START:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
                thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeAnim);
                Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonChargeAnim, 0.0f);
                thisv->work[GND_ACTION_STATE] = CHARGE_ATTACK;
            }
        case CHARGE_ATTACK:
            if (thisv->timers[0] != 0) {
                Vec3f vecToLink;

                Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                vecToLink.x = playerx->world.pos.x - thisx->world.pos.x;
                vecToLink.y = playerx->world.pos.y + 40.0f - thisx->world.pos.y;
                vecToLink.z = playerx->world.pos.z - thisx->world.pos.z;
                thisx->world.rot.y = thisx->shape.rot.y;
                thisx->world.rot.x =
                    Math_FAtan2F(vecToLink.y, sqrtf(SQ(vecToLink.x) + SQ(vecToLink.z))) * (0x8000 / std::numbers::pi_v<float>);
            }

            func_8002D908(thisx);
            func_8002D7EC(thisx);
            Math_ApproachF(&thisx->speedXZ, 10.0f, 1.0f, 0.5f);
            if ((sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) || (thisx->xyzDistToPlayerSq < SQ(100.0f))) {
                thisv->work[GND_ACTION_STATE] = CHARGE_FINISH;
                thisv->timers[0] = 20;
            }
            break;
        case CHARGE_FINISH:
            thisx->gravity = 0.2f;
            Actor_MoveForward(thisx);
            osSyncPrintf("YP %f @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", thisx->world.pos.y);
            if (thisx->world.pos.y < 5.0f) {
                thisx->world.pos.y = 5.0f;
                thisx->velocity.y = 0.0f;
            }

            if (sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) {
                Math_ApproachZeroF(&thisx->speedXZ, 1.0f, 2.0f);
                thisv->timers[0] = 0;
            }

            if (thisv->timers[0] == 0) {
                Math_ApproachZeroF(&thisx->speedXZ, 1.0f, 2.0f);
                Math_ApproachZeroF(&thisx->velocity.y, 1.0f, 2.0f);
                Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                if ((thisx->speedXZ <= 0.5f) && (fabsf(thisx->velocity.y) <= 0.1f)) {
                    BossGanondrof_SetupNeutral(thisv, -10.0f);
                    thisv->timers[0] = 30;
                    thisv->flyMode = GND_FLY_NEUTRAL;
                }
            }
            break;
    }

    if (thisx->world.pos.y > (GND_BOSSROOM_CENTER_Y + 83.0f)) {
        thisx->world.pos.y += 2.0f * Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
    }
    {
        s16 i;
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Vec3f baseOffset = { 0.0f, 50.0f, 0.0f };
        Vec3f offset;

        baseOffset.y = 10.0f;
        for (i = 0; i < 10; i++) {
            Matrix_Push();
            Matrix_RotateY((thisx->shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);
            Matrix_RotateX((thisx->shape.rot.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateZ((thisv->work[GND_PARTICLE_ANGLE] / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_MultVec3f(&baseOffset, &offset);
            Matrix_Pop();
            pos.x = thisv->spearTip.x + offset.x;
            pos.y = thisv->spearTip.y + offset.y;
            pos.z = thisv->spearTip.z + offset.z;
            vel.x = (offset.x * 500.0f) / 1000.0f;
            vel.y = (offset.y * 500.0f) / 1000.0f;
            vel.z = (offset.z * 500.0f) / 1000.0f;
            accel.x = (offset.x * -50.0f) / 1000.0f;
            accel.y = (offset.y * -50.0f) / 1000.0f;
            accel.z = (offset.z * -50.0f) / 1000.0f;
            EffectSsFhgFlash_SpawnLightBall(globalCtx, &pos, &vel, &accel, 150, i % 7);
            thisv->work[GND_PARTICLE_ANGLE] += 0x1A5C;
        }
    }

    if (!(thisv->work[GND_VARIANCE_TIMER] & 7)) {
        EnfHG* horse = (EnfHG*)thisx->child;

        Actor_SpawnAsChild(&globalCtx->actorCtx, thisx, globalCtx, ACTOR_EN_FHG_FIRE, thisv->spearTip.x,
                           thisv->spearTip.y, thisv->spearTip.z, 8, FHGFIRE_LIGHT_BLUE, 0, FHGFIRE_SPEAR_LIGHT);
        thisx->child = &horse->actor;
    }
}

void BossGanondrof_SetupDeath(BossGanondrof* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gPhantomGanonDeathBlowAnim);
    thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonDeathBlowAnim);
    thisv->actionFunc = BossGanondrof_Death;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_DEAD);
    thisv->deathState = DEATH_START;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->work[GND_VARIANCE_TIMER] = 0;
    thisv->shockTimer = 50;
}

void BossGanondrof_Death(BossGanondrof* thisv, GlobalContext* globalCtx) {
    u8 holdCamera = false;
    u8 bodyDecayLevel = 0;
    f32 camX;
    f32 camZ;
    f32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Camera* camera = Gameplay_GetCamera(globalCtx, 0);

    osSyncPrintf("PYP %f\n", player->actor.floorHeight);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->work[GND_DEATH_SFX_TIMER]++;
    if (((60 < thisv->work[GND_DEATH_SFX_TIMER]) && (thisv->work[GND_DEATH_SFX_TIMER] < 500)) ||
        ((501 < thisv->work[GND_DEATH_SFX_TIMER]) && (thisv->work[GND_DEATH_SFX_TIMER] < 620))) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_LAST - SFX_FLAG);
    }

    switch (thisv->deathState) {
        case DEATH_START:
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 1);
            thisv->deathCamera = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
            osSyncPrintf("7\n");
            Gameplay_ChangeCameraStatus(globalCtx, thisv->deathCamera, CAM_STAT_ACTIVE);
            osSyncPrintf("8\n");
            thisv->deathState = DEATH_THROES;
            player->actor.speedXZ = 0.0f;
            thisv->timers[0] = 50;
            thisv->cameraEye = camera->eye;
            thisv->cameraAt = camera->at;
            thisv->cameraNextEye.x = thisv->targetPos.x;
            thisv->cameraNextEye.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            thisv->cameraNextEye.z = (thisv->targetPos.z + 100.0f) + 50;
            thisv->cameraNextAt.x = thisv->targetPos.x;
            thisv->cameraNextAt.y = thisv->targetPos.y - 10.0f;
            thisv->cameraNextAt.z = thisv->targetPos.z;
            thisv->cameraEyeVel.x = fabsf(camera->eye.x - thisv->cameraNextEye.x);
            thisv->cameraEyeVel.y = fabsf(camera->eye.y - thisv->cameraNextEye.y);
            thisv->cameraEyeVel.z = fabsf(camera->eye.z - thisv->cameraNextEye.z);
            thisv->cameraAtVel.x = fabsf(camera->at.x - thisv->cameraNextAt.x);
            thisv->cameraAtVel.y = fabsf(camera->at.y - thisv->cameraNextAt.y);
            thisv->cameraAtVel.z = fabsf(camera->at.z - thisv->cameraNextAt.z);
            thisv->cameraAccel = 0.02f;
            thisv->cameraEyeMaxVel.x = thisv->cameraEyeMaxVel.y = thisv->cameraEyeMaxVel.z = 0.05f;
            thisv->work[GND_ACTION_STATE] = DEATH_SPASM;
            thisv->timers[0] = 150;
            thisv->cameraAtMaxVel.x = 0.2f;
            thisv->cameraAtMaxVel.y = 0.2f;
            thisv->cameraAtMaxVel.z = 0.2f;
        case DEATH_THROES:
            switch (thisv->work[GND_ACTION_STATE]) {
                case DEATH_SPASM:
                    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
                        thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonAirDamageAnim);
                        Animation_Change(&thisv->skelAnime, &gPhantomGanonAirDamageAnim, 0.5f, 0.0f,
                                         thisv->fwork[GND_END_FRAME], ANIMMODE_ONCE_INTERP, 0.0f);
                        thisv->work[GND_ACTION_STATE] = DEATH_LIMP;
                    }
                    break;
                case DEATH_LIMP:
                    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[GND_END_FRAME])) {
                        thisv->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonLimpAnim);
                        Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonLimpAnim, -20.0f);
                        thisv->work[GND_ACTION_STATE] = DEATH_HUNCHED;
                    }
                case DEATH_HUNCHED:
                    bodyDecayLevel = 1;
                    break;
            }
            Math_ApproachS(&thisv->actor.shape.rot.y, thisv->work[GND_VARIANCE_TIMER] * -100, 5, 0xBB8);
            Math_ApproachF(&thisv->cameraNextEye.z, thisv->targetPos.z + 60.0f, 0.02f, 0.5f);
            Math_ApproachF(&thisv->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 133.0f, 0.05f, 100.0f);
            thisv->actor.world.pos.y += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
            thisv->cameraNextAt.x = thisv->targetPos.x;
            thisv->cameraNextAt.y = thisv->targetPos.y - 10.0f;
            thisv->cameraNextAt.z = thisv->targetPos.z;
            if (thisv->timers[0] == 0) {
                thisv->deathState = DEATH_WARP;
                thisv->timers[0] = 350;
                thisv->timers[1] = 50;
                thisv->fwork[GND_CAMERA_ZOOM] = 300.0f;
                thisv->cameraNextEye.y = GND_BOSSROOM_CENTER_Y + 233.0f;
                player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
                player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                holdCamera = true;
                bodyDecayLevel = 1;
            }
            break;
        case DEATH_WARP:
            if (thisv->timers[1] == 1) {
                EnfHG* horseTemp = (EnfHG*)thisv->actor.child;

                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                   GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y + 3.0f, GND_BOSSROOM_CENTER_Z, 0x4000,
                                   0, 0, FHGFIRE_WARP_DEATH);
                thisv->actor.child = &horseTemp->actor;
                Message_StartTextbox(globalCtx, 0x108E, NULL);
            }

            thisv->actor.shape.rot.y -= 0xC8;
            thisv->actor.world.pos.y += Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 1500);
            thisv->fwork[GND_CAMERA_ANGLE] += 0x78;
            camX = Math_SinS(thisv->fwork[GND_CAMERA_ANGLE]) * thisv->fwork[GND_CAMERA_ZOOM];
            camZ = Math_CosS(thisv->fwork[GND_CAMERA_ANGLE]) * thisv->fwork[GND_CAMERA_ZOOM];
            thisv->cameraEye.x = GND_BOSSROOM_CENTER_X + camX;
            thisv->cameraEye.y = thisv->cameraNextEye.y;
            thisv->cameraEye.z = GND_BOSSROOM_CENTER_Z + camZ;
            thisv->cameraAt.x = GND_BOSSROOM_CENTER_X;
            thisv->cameraAt.y = GND_BOSSROOM_CENTER_Y + 23.0f;
            thisv->cameraAt.z = GND_BOSSROOM_CENTER_Z;
            Math_ApproachF(&thisv->cameraNextEye.y, GND_BOSSROOM_CENTER_Y + 33.0f, 0.05f, 0.5f);
            Math_ApproachF(&thisv->fwork[GND_CAMERA_ZOOM], 170.0f, 0.05f, 1.0f);
            Math_ApproachF(&thisv->actor.world.pos.x, GND_BOSSROOM_CENTER_X, 0.05f, 1.5f);
            Math_ApproachF(&thisv->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 83.0f, 0.05f, 1.0f);
            Math_ApproachF(&thisv->actor.world.pos.z, GND_BOSSROOM_CENTER_Z, 0.05f, 1.5f);
            if (thisv->timers[0] == 0) {
                thisv->deathState = DEATH_SCREAM;
                thisv->timers[0] = 50;
                Animation_MorphToLoop(&thisv->skelAnime, &gPhantomGanonScreamAnim, -10.0f);
                thisv->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
                thisv->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
                thisv->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                thisv->actor.shape.rot.y = 0;
                thisv->work[GND_BODY_DECAY_INDEX] = 0;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_LAST);
            }

            holdCamera = true;
            bodyDecayLevel = 1;
            break;
        case DEATH_SCREAM:
            holdCamera = true;
            bodyDecayLevel = 2;
            thisv->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            thisv->cameraEye.x = GND_BOSSROOM_CENTER_X;
            thisv->cameraEye.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            thisv->cameraEye.z = GND_BOSSROOM_CENTER_Z + 50.0f;
            thisv->cameraAt.x = GND_BOSSROOM_CENTER_X;
            thisv->cameraAt.y = GND_BOSSROOM_CENTER_Y + 103.0f;
            thisv->cameraAt.z = GND_BOSSROOM_CENTER_Z;
            if (thisv->timers[0] == 0) {
                thisv->deathState = DEATH_DISINTEGRATE;
                Animation_MorphToPlayOnce(&thisv->skelAnime, &gPhantomGanonLastPoseAnim, -10.0f);
                thisv->work[GND_BODY_DECAY_INDEX] = 0;
                thisv->timers[0] = 40;
            }
            break;
        case DEATH_DISINTEGRATE:
            holdCamera = true;
            bodyDecayLevel = 3;
            Math_ApproachZeroF(&thisv->cameraEye.y, 0.05f, 1.0f); // approaches GND_BOSSROOM_CENTER_Y + 33.0f
            Math_ApproachF(&thisv->cameraEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            Math_ApproachF(&thisv->cameraAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (thisv->timers[0] == 0) {
                thisv->timers[0] = 250;
                thisv->deathState = DEATH_FINISH;
            }
            break;
        case DEATH_FINISH:
            holdCamera = true;
            bodyDecayLevel = 10;
            if (thisv->timers[0] == 150) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X,
                            GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
            }

            Math_ApproachZeroF(&thisv->cameraEye.y, 0.05f, 1.0f); // GND_BOSSROOM_CENTER_Y + 33.0f
            Math_ApproachF(&thisv->cameraEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            Math_ApproachF(&thisv->cameraAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (thisv->timers[0] == 0) {
                EnfHG* horse = (EnfHG*)thisv->actor.child;

                camera->eye = thisv->cameraEye;
                camera->eyeNext = thisv->cameraEye;
                camera->at = thisv->cameraAt;
                func_800C08AC(globalCtx, thisv->deathCamera, 0);
                thisv->deathCamera = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, GND_BOSSROOM_CENTER_X,
                            GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z + 200.0f, 0, 0, 0, 0);
                thisv->actor.child = &horse->actor;
                thisv->killActor = true;
                horse->killActor = true;
                Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
                Flags_SetSwitch(globalCtx, 0x22);
            }
            break;
    }

    if (bodyDecayLevel) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accelKFire = { 0.0f, 0.0f, 0.0f };
        Vec3f accelHahen = { 0.0f, -0.5f, 0.0f };
        s16 limbDecayIndex;
        s16 i;

        vel.x = thisv->actor.world.pos.x - thisv->actor.prevPos.x;
        vel.z = thisv->actor.world.pos.z - thisv->actor.prevPos.z;
        if (bodyDecayLevel < 10) {
            if (thisv->work[GND_DEATH_ENV_TIMER] == 0) {
                if (globalCtx->envCtx.unk_BF == 0) {
                    globalCtx->envCtx.unk_BF = 3;
                    thisv->work[GND_DEATH_ENV_TIMER] = (s16)Rand_ZeroFloat(5.0f) + 4.0f;
                    globalCtx->envCtx.unk_D6 = 0x28;
                } else {
                    globalCtx->envCtx.unk_BF = 0;
                    thisv->work[GND_DEATH_ENV_TIMER] = (s16)Rand_ZeroFloat(2.0f) + 2.0f;
                    globalCtx->envCtx.unk_D6 = 0x14;
                }
            } else {
                thisv->work[GND_DEATH_ENV_TIMER]--;
            }

            for (i = 0; i <= 0; i++) {
                limbDecayIndex = thisv->work[GND_LIMB_DECAY_INDEX];
                thisv->work[GND_LIMB_DECAY_INDEX]++;
                thisv->work[GND_LIMB_DECAY_INDEX] %= 25;
                pos.x = thisv->bodyPartsPos[limbDecayIndex].x + Rand_CenteredFloat(5.0f);
                pos.y = thisv->bodyPartsPos[limbDecayIndex].y + Rand_CenteredFloat(5.0f);
                pos.z = thisv->bodyPartsPos[limbDecayIndex].z + Rand_CenteredFloat(5.0f);
                accelKFire.y = 0.0f;

                if (bodyDecayLevel == 3) {
                    accelKFire.y = -0.2f;
                    accelKFire.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.002f;
                    accelKFire.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.002f;
                    accelHahen.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.001f;
                    accelHahen.y = -1.0f;
                    accelHahen.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.001f;
                }

                EffectSsKFire_Spawn(globalCtx, &pos, &vel, &accelKFire, (s16)Rand_ZeroFloat(20.0f) + 15,
                                    bodyDecayLevel);
                if ((Rand_ZeroOne() < 0.5f) || (bodyDecayLevel == 3)) {
                    EffectSsHahen_Spawn(globalCtx, &pos, &vel, &accelHahen, 0, (s16)Rand_ZeroFloat(4.0f) + 7,
                                        HAHEN_OBJECT_DEFAULT, 10, NULL);
                }
            }
        } else {
            globalCtx->envCtx.unk_BF = 0;
            globalCtx->envCtx.unk_D6 = 0x14;
        }

        thisv->work[GND_BODY_DECAY_FLAG] = true;
        for (i = 0; i < 5; i++) {
            if (bodyDecayLevel == 1) {
                BossGanondrof_ClearPixels(sDecayMaskLow, thisv->work[GND_BODY_DECAY_INDEX]);
            } else if (bodyDecayLevel == 2) {
                BossGanondrof_ClearPixels(sDecayMaskHigh, thisv->work[GND_BODY_DECAY_INDEX]);
            } else {
                BossGanondrof_ClearPixels(sDecayMaskTotal, thisv->work[GND_BODY_DECAY_INDEX]);
            }

            if (thisv->work[GND_BODY_DECAY_INDEX] < 0xFF) {
                thisv->work[GND_BODY_DECAY_INDEX]++;
            }
        }
    }

    if (thisv->deathCamera != 0) {
        if (!holdCamera) {
            Math_ApproachF(&thisv->cameraEye.x, thisv->cameraNextEye.x, thisv->cameraEyeMaxVel.x,
                           thisv->cameraEyeVel.x * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraEye.y, thisv->cameraNextEye.y, thisv->cameraEyeMaxVel.y,
                           thisv->cameraEyeVel.y * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraEye.z, thisv->cameraNextEye.z, thisv->cameraEyeMaxVel.z,
                           thisv->cameraEyeVel.z * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraAt.x, thisv->cameraNextAt.x, thisv->cameraAtMaxVel.x,
                           thisv->cameraAtVel.x * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraAt.y, thisv->cameraNextAt.y, thisv->cameraAtMaxVel.y,
                           thisv->cameraAtVel.y * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraAt.z, thisv->cameraNextAt.z, thisv->cameraAtMaxVel.z,
                           thisv->cameraAtVel.z * thisv->cameraSpeedMod);
            Math_ApproachF(&thisv->cameraSpeedMod, 1.0f, 1.0f, thisv->cameraAccel);
        }

        Gameplay_CameraSetAtEye(globalCtx, thisv->deathCamera, &thisv->cameraAt, &thisv->cameraEye);
    }
}

void BossGanondrof_CollisionCheck(BossGanondrof* thisv, GlobalContext* globalCtx) {
    s32 acHit;
    EnfHG* horse = (EnfHG*)thisv->actor.child;
    ColliderInfo* hurtbox;

    if (thisv->work[GND_INVINC_TIMER] != 0) {
        thisv->work[GND_INVINC_TIMER]--;
        thisv->returnCount = 0;
        thisv->colliderBody.base.acFlags &= ~AC_HIT;
    } else {
        acHit = thisv->colliderBody.base.acFlags & AC_HIT;
        if ((acHit && ((s8)thisv->actor.colChkInfo.health > 0)) || (thisv->returnCount != 0)) {
            if (acHit) {
                thisv->colliderBody.base.acFlags &= ~AC_HIT;
                hurtbox = thisv->colliderBody.info.acHitInfo;
            }
            if (thisv->flyMode != GND_FLY_PAINTING) {
                if (acHit && (thisv->actionFunc != BossGanondrof_Stunned) && (hurtbox->toucher.dmgFlags & 0x0001F8A4)) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_WALK_GROUND - SFX_FLAG);
                    osSyncPrintf("hit != 0 \n");
                } else if (thisv->actionFunc != BossGanondrof_Charge) {
                    if (thisv->returnCount == 0) {
                        u8 dmg;
                        u8 canKill = false;
                        s32 dmgFlags = hurtbox->toucher.dmgFlags;

                        if (dmgFlags & 0x80) {
                            return;
                        }
                        dmg = CollisionCheck_GetSwordDamage(dmgFlags);
                        (dmg == 0) ? (dmg = 2) : (canKill = true);
                        if (((s8)thisv->actor.colChkInfo.health > 2) || canKill) {
                            thisv->actor.colChkInfo.health -= dmg;
                        }

                        if ((s8)thisv->actor.colChkInfo.health <= 0 || 1) {
                            BossGanondrof_SetupDeath(thisv, globalCtx);
                            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                            return;
                        }
                    }
                    BossGanondrof_SetupStunned(thisv, globalCtx);
                    if (thisv->returnCount >= 2) {
                        thisv->timers[0] = 120;
                    }
                    thisv->work[GND_INVINC_TIMER] = 10;
                    horse->hitTimer = 20;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_DAMAGE);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_WALK_GROUND - SFX_FLAG);
                }
            } else if (acHit && (hurtbox->toucher.dmgFlags & 0x0001F8A4)) {
                thisv->work[GND_INVINC_TIMER] = 10;
                thisv->actor.colChkInfo.health -= 2;
                horse->hitTimer = 20;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_DAMAGE);
            }
            thisv->returnCount = 0;
        }
    }
}

void BossGanondrof_Update(Actor* thisx, GlobalContext* globalCtx) {
    f32 cs;
    f32 sn;
    f32 legRotTargetY;
    f32 legRotTargetZ;
    f32 legSplitTarget;
    s32 pad2;
    s16 i;
    s32 pad;
    BossGanondrof* thisv = (BossGanondrof*)thisx;
    EnfHG* horse;

    osSyncPrintf("MOVE START %d\n", thisv->actor.params);
    thisv->actor.flags &= ~ACTOR_FLAG_10;
    thisv->colliderBody.base.colType = COLTYPE_HIT3;
    if (thisv->killActor) {
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->work[GND_VARIANCE_TIMER]++;
    horse = (EnfHG*)thisv->actor.child;
    osSyncPrintf("MOVE START EEEEEEEEEEEEEEEEEEEEEE%d\n", thisv->actor.params);

    thisv->actionFunc(thisv, globalCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->timers); i++) {
        if (thisv->timers[i]) {
            thisv->timers[i]--;
        }
    }
    if (thisv->work[GND_UNKTIMER_1]) {
        thisv->work[GND_UNKTIMER_1]--;
    }
    if (thisv->work[GND_UNKTIMER_2]) {
        thisv->work[GND_UNKTIMER_2]--;
    }

    if (thisv->actionFunc != BossGanondrof_Death) {
        BossGanondrof_CollisionCheck(thisv, globalCtx);
    }

    osSyncPrintf("MOVE END\n");
    BossGanondrof_SetColliderPos(&thisv->targetPos, &thisv->colliderBody);
    BossGanondrof_SetColliderPos(&thisv->spearTip, &thisv->colliderSpear);
    if ((thisv->flyMode == GND_FLY_PAINTING) && !horse->bossGndInPainting) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    }
    if ((thisv->actionFunc == BossGanondrof_Stunned) && (thisv->timers[0] > 1)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    } else if (thisv->actionFunc == BossGanondrof_Block) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
    } else if (thisv->actionFunc == BossGanondrof_Charge) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderBody.base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSpear.base);
    }

    thisv->actor.focus.pos = thisv->targetPos;

    sn = Math_SinS(-thisv->actor.shape.rot.y);
    cs = Math_CosS(-thisv->actor.shape.rot.y);
    legRotTargetY = ((sn * thisv->actor.velocity.z) + (cs * thisv->actor.velocity.x)) * 300.0f;
    legRotTargetZ = ((-sn * thisv->actor.velocity.x) + (cs * thisv->actor.velocity.z)) * 300.0f;
    Math_ApproachF(&thisv->legRotY, legRotTargetY, 1.0f, 600.0f);
    Math_ApproachF(&thisv->legRotZ, legRotTargetZ, 1.0f, 600.0f);
    if ((thisv->flyMode != GND_FLY_PAINTING) && (thisv->actionFunc != BossGanondrof_Stunned) &&
        (thisv->deathState == NOT_DEAD)) {
        legSplitTarget = (Math_SinS(thisv->work[GND_VARIANCE_TIMER] * 0x8DC) * -500.0f) - 500.0f;
    } else {
        legSplitTarget = 0.0f;
    }

    Math_ApproachF(&thisv->legSplitY, legSplitTarget, 1.0f, 100.0f);
    if (thisv->shockTimer != 0) {
        s16 j;

        thisv->shockTimer--;
        osSyncPrintf("F 1\n");
        for (j = 0; j < 7; j++) {
            osSyncPrintf("F 15\n");
            EffectSsFhgFlash_SpawnShock(globalCtx, &thisv->actor, &thisv->actor.world.pos, 45, FHGFLASH_SHOCK_PG);
        }
        osSyncPrintf("F 2\n");
    }

    if (thisv->actor.params == GND_REAL_BOSS) {
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->spearTip.x, thisv->spearTip.y, thisv->spearTip.z, 255, 255, 255,
                                  200);
    }
}

s32 BossGanondrof_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void* thisx) {
    BossGanondrof* thisv = (BossGanondrof*)thisx;

    switch (limbIndex) {
        case 15:
            if ((thisv->actionFunc == BossGanondrof_Intro) && thisv->work[GND_MASK_OFF]) {
                *dList = gPhantomGanonFaceDL;
            }
            rot->y += thisv->rideRotY[limbIndex];
            rot->z += thisv->rideRotZ[limbIndex];
            break;

        case 19:
            rot->y += thisv->legRotY + thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 20:
            rot->y += thisv->legRotY + thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 21:
            rot->y += thisv->legRotY + thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 23:
            rot->y += thisv->legRotY - thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 24:
            rot->y += thisv->legRotY - thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 25:
            rot->y += thisv->legRotY - thisv->legSplitY;
            rot->z += thisv->legRotZ;
            break;

        case 5:
        case 6:
        case 7:
            rot->y += thisv->armRotY;
            rot->z += thisv->armRotZ;
            break;

        case 8:
        case 9:
        case 10:
            rot->y += thisv->armRotY;
            rot->z += thisv->armRotZ;
            break;

        case 13:
            if (thisv->deathState != NOT_DEAD) {
                *dList = NULL;
            }
        default:
            rot->y += thisv->rideRotY[limbIndex];
            rot->z += thisv->rideRotZ[limbIndex];
            break;
    }

    return 0;
}

void BossGanondrof_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Vec3f spearVec = { 0.0f, 0.0f, 6000.0f };

    BossGanondrof* thisv = (BossGanondrof*)thisx;

    if (limbIndex == 14) {
        Matrix_MultVec3f(&zeroVec, &thisv->targetPos);
    } else if (limbIndex == 13) {
        Matrix_MultVec3f(&spearVec, &thisv->spearTip);
    }

    if (((thisv->flyMode != GND_FLY_PAINTING) || (thisv->actionFunc == BossGanondrof_Intro)) && (limbIndex <= 25)) {
        Matrix_MultVec3f(&zeroVec, &thisv->bodyPartsPos[limbIndex - 1]);
    }
}

Gfx* BossGanondrof_GetClearPixelDList(GraphicsContext* gfxCtx) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx) * 4);
    Gfx* dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* BossGanondrof_GetNullDList(GraphicsContext* gfxCtx) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx) * 1);
    Gfx* dListHead = dList;

    gSPEndDisplayList(dListHead++);
    return dList;
}

void BossGanondrof_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossGanondrof* thisv = (BossGanondrof*)thisx;
    EnfHG* horse;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_ganondrof.c", 3716);
    osSyncPrintf("MOVE P = %x\n", thisv->actor.update);
    osSyncPrintf("STOP TIMER = %d ==============\n", thisv->actor.freezeTimer);
    horse = (EnfHG*)thisv->actor.child;
    if (thisv->flyMode == GND_FLY_PAINTING) {
        Matrix_RotateY((horse->turnRot * 3.1416f) / (f32)0x8000, MTXMODE_APPLY);
    }

    osSyncPrintf("YP %f\n", thisv->actor.world.pos.y);
    func_80093D18(globalCtx->state.gfxCtx);
    if (thisv->work[GND_INVINC_TIMER] & 4) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
    } else {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, (u32)horse->warpColorFilterR, (u32)horse->warpColorFilterG,
                                   (u32)horse->warpColorFilterB, 0, (s32)horse->warpColorFilterUnk1 + 995,
                                   (s32)horse->warpColorFilterUnk2 + 1000);
    }

    osSyncPrintf("DRAW 11\n");
    osSyncPrintf("EYE_COL %d\n", (s16)thisv->fwork[GND_EYE_BRIGHTNESS]);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->fwork[GND_EYE_BRIGHTNESS], (s16)thisv->fwork[GND_EYE_BRIGHTNESS],
                   (s16)thisv->fwork[GND_EYE_BRIGHTNESS], (s16)thisv->fwork[GND_EYE_ALPHA]);
    if (thisv->work[GND_BODY_DECAY_FLAG]) {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGanondrof_GetClearPixelDList(globalCtx->state.gfxCtx));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGanondrof_GetNullDList(globalCtx->state.gfxCtx));
    }

    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, BossGanondrof_OverrideLimbDraw,
                      BossGanondrof_PostLimbDraw, thisv);
    osSyncPrintf("DRAW 22\n");
    POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_ganondrof.c", 3814);
    osSyncPrintf("DRAW END %d\n", thisv->actor.params);
}
