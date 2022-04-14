#include "z_boss_goma.h"
#include "objects/object_goma/object_goma.h"
#include "overlays/actors/ovl_En_Goma/z_en_goma.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

// IRIS_FOLLOW: gohma looks towards the player (iris rotation)
// BONUS_IFRAMES: gain invincibility frames when the player does something (throwing things?), or
// randomly (see BossGoma_UpdateEye)
typedef enum {
    EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES, // default, allows not drawing lens and iris when eye is closed
    EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES,
    EYESTATE_IRIS_FOLLOW_NO_IFRAMES
} GohmaEyeState;

typedef enum {
    VISUALSTATE_RED,         // main/eye: red
    VISUALSTATE_DEFAULT,     // main: greenish cyan, blinks with dark gray every 16 frames; eye: white
    VISUALSTATE_DEFEATED,    // main/eye: dark gray
    VISUALSTATE_STUNNED = 4, // main: greenish cyan, alternates with blue; eye: greenish cyan
    VISUALSTATE_HIT          // main: greenish cyan, alternates with red; eye: greenish cyan
} GohmaVisualState;

void BossGoma_Init(Actor* thisx, GlobalContext* globalCtx);
void BossGoma_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossGoma_Update(Actor* thisx, GlobalContext* globalCtx);
void BossGoma_Draw(Actor* thisx, GlobalContext* globalCtx);

void BossGoma_SetupEncounter(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_Encounter(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_Defeated(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorAttackPosture(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorPrepareAttack(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorAttack(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorDamaged(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorLandStruckDown(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorLand(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorStunned(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FallJump(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FallStruckDown(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_CeilingSpawnGohmas(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_CeilingPrepareSpawnGohmas(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorIdle(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_CeilingIdle(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_FloorMain(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_WallClimb(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_CeilingMoveToCenter(BossGoma* thisv, GlobalContext* globalCtx);
void BossGoma_SpawnChildGohma(BossGoma* thisv, GlobalContext* globalCtx, s16 i);

const ActorInit Boss_Goma_InitVars = {
    ACTOR_BOSS_GOMA,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GOMA,
    sizeof(BossGoma),
    (ActorFunc)BossGoma_Init,
    (ActorFunc)BossGoma_Destroy,
    (ActorFunc)BossGoma_Update,
    (ActorFunc)BossGoma_Draw,
    NULL,
};

static ColliderJntSphElementInit sColliderJntSphElementInit[13] = {
    {
        {
            ELEMTYPE_UNK3,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_EYE, { { 0, 0, 1200 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL4, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL3, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL2, { { 0, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL1, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_FEET, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_SHIN, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_THIGH_SHELL, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_ANTENNA_CLAW, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_ANTENNA_CLAW, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_FEET, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_SHIN, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_THIGH_SHELL, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sColliderJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    13,
    sColliderJntSphElementInit,
};

static u8 sClearPixelTableFirstPass[16 * 16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
};

static u8 sClearPixelTableSecondPass[16 * 16] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

// indexed by limb (where the root limb is 1)
static u8 sDeadLimbLifetime[] = {
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    30, // tail end/last part
    40, // tail 2nd to last part
    0,  0, 0, 0, 0, 0, 0, 0,
    10, // back of right claw/hand
    15, // front of right claw/hand
    21, // part of right arm (inner)
    0,  0,
    25, // part of right arm (shell)
    0,  0,
    31, // part of right arm (shell on shoulder)
    35, // part of right arm (shoulder)
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    43, // end of left antenna
    48, // middle of left antenna
    53, // start of left antenna
    0,  0, 0, 0,
    42, // end of right antenna
    45, // middle of right antenna
    53, // start of right antenna
    0,  0, 0, 0, 0, 0,
    11, // back of left claw/hand
    15, // front of left claw/hand
    21, // part of left arm (inner)
    0,  0,
    25, // part of left arm (shell)
    0,  0,
    30, // part of left arm (shell on shoulder)
    35, // part of left arm (shoulder)
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/**
 * Sets the `i`th pixel of a 16x16 RGBA16 image to 0 (transparent black)
 * according to the `clearPixelTable`
 */
void BossGoma_ClearPixels16x16Rgba16(const char* rgba16imageName, u8* clearPixelTable, s16 i) 
{
    s16* rgba16image = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(rgba16imageName));
    if (clearPixelTable[i]) {
        rgba16image[i] = 0;
    }
}

/**
 * Sets the `i`th 2x2 pixels block of a 32x32 RGBA16 image to 0 (transparent black)
 * according to the `clearPixelTable`
 */
void BossGoma_ClearPixels32x32Rgba16(const char* rgba16imageName, u8* clearPixelTable, s16 i) {
    s16* targetPixel;

    s16* rgba16image = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(rgba16imageName));

    if (clearPixelTable[i]) {
        // address of the top left pixel in a 2x2 pixels block located at
        // (i & 0xF, i >> 4) in a 16x16 grid of 2x2 pixels
        targetPixel = reinterpret_cast<s16*>( reinterpret_cast<std::uintptr_t>(rgba16image) + (s16)((i & 0xF) * 2 + (i & 0xF0) * 4) * 2 );
        // set the 2x2 block of pixels to 0
        targetPixel[0] = 0;
        targetPixel[1] = 0;
        targetPixel[32 + 0] = 0;
        targetPixel[32 + 1] = 0;
    }
}

/**
 * Clear pixels from Gohma's textures
 */
void BossGoma_ClearPixels(u8* clearPixelTable, s16 i) {
    BossGoma_ClearPixels16x16Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaBodyTex), clearPixelTable, i);
    BossGoma_ClearPixels16x16Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaShellUndersideTex), clearPixelTable, i);
    BossGoma_ClearPixels16x16Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaDarkShellTex), clearPixelTable, i);
    BossGoma_ClearPixels16x16Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaEyeTex), clearPixelTable, i);

    BossGoma_ClearPixels32x32Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaShellTex), clearPixelTable, i);
    BossGoma_ClearPixels32x32Rgba16(SEGMENTED_TO_VIRTUAL(gGohmaIrisTex), clearPixelTable, i);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x01, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void BossGoma_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossGoma* thisv = (BossGoma*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 4000.0f, ActorShadow_DrawCircle, 150.0f);
    SkelAnime_Init(globalCtx, &thisv->skelanime, &gGohmaSkel, &gGohmaIdleCrouchedAnim, NULL, NULL, 0);
    Animation_PlayLoop(&thisv->skelanime, &gGohmaIdleCrouchedAnim);
    thisv->actor.shape.rot.x = -0x8000; // upside-down
    thisv->eyeIrisScaleX = 1.0f;
    thisv->eyeIrisScaleY = 1.0f;
    thisv->unusedInitX = thisv->actor.world.pos.x;
    thisv->unusedInitZ = thisv->actor.world.pos.z;
    thisv->actor.world.pos.y = -300.0f; // ceiling
    thisv->actor.gravity = 0.0f;
    BossGoma_SetupEncounter(thisv, globalCtx);
    thisv->actor.colChkInfo.health = 10;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sColliderJntSphInit, thisv->colliderItems);

    if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
        Actor_Kill(&thisv->actor);
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, 0.0f, -640.0f, 0.0f, 0, 0,
                           0, WARP_DUNGEON_CHILD);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, 141.0f, -640.0f, -84.0f, 0, 0, 0, 0);
    }
}

void BossGoma_PlayEffectsAndSfx(BossGoma* thisv, GlobalContext* globalCtx, s16 arg2, s16 amountMinus1) {
    if (arg2 == 0 || arg2 == 1 || arg2 == 3) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightHandBackLimbWorldPos, 25.0f, amountMinus1, 8.0f,
                                 500, 10, 1);
    }

    if (arg2 == 0 || arg2 == 2 || arg2 == 3) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftHandBackLimbWorldPos, 25.0f, amountMinus1, 8.0f,
                                 500, 10, 1);
    }

    if (arg2 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DOWN);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_WALK);
    }
}

void BossGoma_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BossGoma* thisv = (BossGoma*)thisx;

    SkelAnime_Free(&thisv->skelanime, globalCtx);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

/**
 * When Gohma is hit and its health drops to 0
 */
void BossGoma_SetupDefeated(BossGoma* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelanime, &gGohmaDeathAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaDeathAnim),
                     ANIMMODE_ONCE, -2.0f);
    thisv->actionFunc = BossGoma_Defeated;
    thisv->disableGameplayLogic = true;
    thisv->decayingProgress = 0;
    thisv->noBackfaceCulling = false;
    thisv->framesUntilNextAction = 1200;
    thisv->actionState = 0;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.shape.shadowScale = 0.0f;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DEAD);
}

/**
 * Initial action setup, with Gohma waiting on the ceiling for the fight to start.
 */
void BossGoma_SetupEncounter(BossGoma* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gGohmaWalkAnim);

    Animation_Change(&thisv->skelanime, &gGohmaWalkAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -15.0f);
    thisv->actionFunc = BossGoma_Encounter;
    thisv->actionState = 0;
    thisv->disableGameplayLogic = true;
    globalCtx->envCtx.unk_BF = 4;
    globalCtx->envCtx.unk_D6 = 0xFF;
}

/**
 * On the floor and not doing anything for 20-30 frames, before going back to BossGoma_FloorMain
 */
void BossGoma_SetupFloorIdle(BossGoma* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGohmaIdleCrouchedAnim);

    thisv->framesUntilNextAction = Rand_S16Offset(20, 30);
    Animation_Change(&thisv->skelanime, &gGohmaIdleCrouchedAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = BossGoma_FloorIdle;
}

/**
 * On the ceiling and not doing anything for 20-30 frames, leads to spawning children gohmas
 */
void BossGoma_SetupCeilingIdle(BossGoma* thisv) {
    thisv->framesUntilNextAction = Rand_S16Offset(20, 30);
    Animation_Change(&thisv->skelanime, &gGohmaHangAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaHangAnim),
                     ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = BossGoma_CeilingIdle;
}

/**
 * When the player killed all children gohmas
 */
void BossGoma_SetupFallJump(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaLandAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossGoma_FallJump;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -2.0f;
}

/**
 * When the player successfully hits Gohma on the ceiling
 */
void BossGoma_SetupFallStruckDown(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaCrashAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossGoma_FallStruckDown;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -2.0f;
}

void BossGoma_SetupCeilingSpawnGohmas(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaLayEggsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaLayEggsAnim),
                     ANIMMODE_LOOP, -15.0f);
    thisv->actionFunc = BossGoma_CeilingSpawnGohmas;
    thisv->spawnGohmasActionTimer = 0;
}

void BossGoma_SetupCeilingPrepareSpawnGohmas(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaPrepareEggsAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gGohmaPrepareEggsAnim), ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = BossGoma_CeilingPrepareSpawnGohmas;
    thisv->framesUntilNextAction = 70;
}

void BossGoma_SetupWallClimb(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaClimbAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaClimbAnim),
                     ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = BossGoma_WallClimb;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = 0.0f;
}

/**
 * Gohma either reached the ceiling after climbing a wall, or is waiting for the player to kill the (children) Gohmas.
 */
void BossGoma_SetupCeilingMoveToCenter(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaWalkAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaWalkAnim),
                     ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = BossGoma_CeilingMoveToCenter;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->framesUntilNextAction = Rand_S16Offset(30, 60);
}

/**
 * Root action when on the floor, leads to attacking or climbing.
 */
void BossGoma_SetupFloorMain(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaWalkCrouchedAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gGohmaWalkCrouchedAnim), ANIMMODE_LOOP, -5.0f);
    thisv->actionFunc = BossGoma_FloorMain;
    thisv->framesUntilNextAction = Rand_S16Offset(70, 110);
}

/**
 * Gohma jumped to the floor on its own, after the player has killed its children Gohmas.
 */
void BossGoma_SetupFloorLand(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaLandAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaLandAnim),
                     ANIMMODE_ONCE, -2.0f);
    thisv->actionFunc = BossGoma_FloorLand;
    thisv->currentAnimFrameCount = Animation_GetLastFrame(&gGohmaLandAnim);
}

/**
 * Gohma was shot by the player down from the ceiling.
 */
void BossGoma_SetupFloorLandStruckDown(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaCrashAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaCrashAnim),
                     ANIMMODE_ONCE, -2.0f);
    thisv->currentAnimFrameCount = Animation_GetLastFrame(&gGohmaCrashAnim);
    thisv->actionFunc = BossGoma_FloorLandStruckDown;
    thisv->currentAnimFrameCount = Animation_GetLastFrame(&gGohmaCrashAnim);
}

/**
 * Gohma is vulnerable, from being struck down from the ceiling or on the ground.
 */
void BossGoma_SetupFloorStunned(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaStunnedAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaStunnedAnim),
                     ANIMMODE_LOOP, -2.0f);
    thisv->actionFunc = BossGoma_FloorStunned;
}

/**
 * Take an attack posture, when the player is close enough.
 */
void BossGoma_SetupFloorAttackPosture(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaPrepareAttackAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gGohmaPrepareAttackAnim), ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = BossGoma_FloorAttackPosture;
}

/**
 * Leads to BossGoma_FloorAttack after 1 frame
 */
void BossGoma_SetupFloorPrepareAttack(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaStandAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaStandAnim),
                     ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = BossGoma_FloorPrepareAttack;
    thisv->framesUntilNextAction = 0;
}

void BossGoma_SetupFloorAttack(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaAttackAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaAttackAnim),
                     ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = BossGoma_FloorAttack;
    thisv->actionState = 0;
    thisv->framesUntilNextAction = 0;
}

/**
 * Plays an animation for Gohma being hit (while stunned)
 * The setup and the action preserve timers apart from the patience one, notably `framesUntilNextAction` which is used
 * as the stun duration
 */
void BossGoma_SetupFloorDamaged(BossGoma* thisv) {
    Animation_Change(&thisv->skelanime, &gGohmaDamageAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaDamageAnim),
                     ANIMMODE_ONCE, -2.0f);
    thisv->actionFunc = BossGoma_FloorDamaged;
}

void BossGoma_UpdateCeilingMovement(BossGoma* thisv, GlobalContext* globalCtx, f32 dz, f32 targetSpeedXZ,
                                    s16 rotateTowardsCenter) {
    static Vec3f velInit = { 0.0f, 0.0f, 0.0f };
    static Vec3f accelInit = { 0.0f, -0.5f, 0.0f };
    static Vec3f roomCenter = { -150.0f, 0.0f, -350.0f };
    Vec3f* basePos = NULL;
    s16 i;
    Vec3f vel;
    Vec3f accel;
    Vec3f pos;

    roomCenter.z += dz; // dz is always 0
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachF(&thisv->actor.speedXZ, targetSpeedXZ, 0.5f, 2.0f);

    if (rotateTowardsCenter) {
        Math_ApproachS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &roomCenter) + 0x8000, 3,
                       0x3E8);
    }

    if (Animation_OnFrame(&thisv->skelanime, 9.0f)) {
        basePos = &thisv->rightHandBackLimbWorldPos;
    } else if (Animation_OnFrame(&thisv->skelanime, 1.0f)) {
        basePos = &thisv->leftHandBackLimbWorldPos;
    }

    if (basePos != NULL) {
        for (i = 0; i < 5; i++) {
            vel = velInit;
            accel = accelInit;
            pos.x = Rand_CenteredFloat(70.0f) + basePos->x;
            pos.y = Rand_ZeroFloat(30.0f) + basePos->y;
            pos.z = Rand_CenteredFloat(70.0f) + basePos->z;
            EffectSsHahen_Spawn(globalCtx, &pos, &vel, &accel, 0, (s16)(Rand_ZeroOne() * 5.0f) + 10, -1, 10, NULL);
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_HIGH);
    }
}

void BossGoma_SetupEncounterState4(BossGoma* thisv, GlobalContext* globalCtx) {
    Player* player;
    Camera* camera;

    camera = Gameplay_GetCamera(globalCtx, 0);
    player = GET_PLAYER(globalCtx);
    thisv->actionState = 4;
    thisv->actor.flags |= ACTOR_FLAG_0;
    func_80064520(globalCtx, &globalCtx->csCtx);
    func_8002DF54(globalCtx, &thisv->actor, 1);
    thisv->subCameraId = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, 0, 3);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->subCameraId, 7);
    Animation_Change(&thisv->skelanime, &gGohmaEyeRollAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaEyeRollAnim),
                     ANIMMODE_ONCE, 0.0f);
    thisv->currentAnimFrameCount = Animation_GetLastFrame(&gGohmaEyeRollAnim);

    // room center (todo: defines for hardcoded positions relative to room center)
    thisv->actor.world.pos.x = -150.0f;
    thisv->actor.world.pos.z = -350.0f;

    // room entrance, towards center
    player->actor.world.pos.x = 150.0f;
    player->actor.world.pos.z = 300.0f;

    player->actor.world.rot.y = player->actor.shape.rot.y = -0x705C;
    thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) + 0x8000;

    // room entrance, closer to room center
    thisv->subCameraEye.x = 90.0f;
    thisv->subCameraEye.z = 170.0f;
    thisv->subCameraEye.y = camera->eye.y + 20.0f;

    thisv->framesUntilNextAction = 50;

    thisv->subCameraAt.x = thisv->actor.world.pos.x;
    thisv->subCameraAt.y = thisv->actor.world.pos.y;
    thisv->subCameraAt.z = thisv->actor.world.pos.z;

    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
}

/**
 * Spawns the door once the player entered
 * Wait for the player to look at Gohma on the ceiling
 * Handles the "meeting Gohma" cutscene, including boss card
 *
 * Skips the door and look-at-Gohma puzzle if the player already reached the boss card part before
 */
void BossGoma_Encounter(BossGoma* thisv, GlobalContext* globalCtx) {
    Camera* cam;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad[2];

    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);

    switch (thisv->actionState) {
        case 0: // wait for the player to enter the room
            // entrance of the boss room
            if (fabsf(player->actor.world.pos.x - 150.0f) < 60.0f &&
                fabsf(player->actor.world.pos.z - 350.0f) < 60.0f) {
                if (gSaveContext.eventChkInf[7] & 1) {
                    BossGoma_SetupEncounterState4(thisv, globalCtx);
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_SHUTTER, 164.72f,
                                       -480.0f, 397.68002f, 0, -0x705C, 0, 0x180);
                } else {
                    func_8002DF54(globalCtx, &thisv->actor, 8);
                    thisv->actionState = 1;
                }
            }
            break;

        case 1: // player entered the room
            func_80064520(globalCtx, &globalCtx->csCtx);
            thisv->subCameraId = Gameplay_CreateSubCamera(globalCtx);
            osSyncPrintf("MAKE CAMERA !!!   1   !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Gameplay_ChangeCameraStatus(globalCtx, 0, 1);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->subCameraId, 7);
            thisv->actionState = 2;
            // ceiling center
            thisv->actor.world.pos.x = -150.0f;
            thisv->actor.world.pos.y = -320.0f;
            thisv->actor.world.pos.z = -350.0f;
            // room entrance
            player->actor.world.pos.x = 150.0f;
            player->actor.world.pos.z = 300.0f;
            // near ceiling center
            thisv->subCameraEye.x = -350.0f;
            thisv->subCameraEye.y = -310.0f;
            thisv->subCameraEye.z = -350.0f;
            // below room entrance
            thisv->subCameraAt.x = player->actor.world.pos.x;
            thisv->subCameraAt.y = player->actor.world.pos.y - 200.0f + 25.0f;
            thisv->subCameraAt.z = player->actor.world.pos.z;
            thisv->framesUntilNextAction = 50;
            thisv->timer = 80;
            thisv->frameCount = 0;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            // fall-through
        case 2: // zoom on player from room center
            // room entrance, towards center
            player->actor.shape.rot.y = -0x705C;
            player->actor.world.pos.x = 150.0f;
            player->actor.world.pos.z = 300.0f;
            player->actor.world.rot.y = player->actor.shape.rot.y;
            player->actor.speedXZ = 0.0f;

            if (thisv->framesUntilNextAction == 0) {
                // (-20, 25, -65) is towards room center
                Math_ApproachF(&thisv->subCameraEye.x, player->actor.world.pos.x - 20.0f, 0.049999997f,
                               thisv->subCameraFollowSpeed * 50.0f);
                Math_ApproachF(&thisv->subCameraEye.y, player->actor.world.pos.y + 25.0f, 0.099999994f,
                               thisv->subCameraFollowSpeed * 130.0f);
                Math_ApproachF(&thisv->subCameraEye.z, player->actor.world.pos.z - 65.0f, 0.049999997f,
                               thisv->subCameraFollowSpeed * 30.0f);
                Math_ApproachF(&thisv->subCameraFollowSpeed, 0.29999998f, 1.0f, 0.0050000004f);
                if (thisv->timer == 0) {
                    Math_ApproachF(&thisv->subCameraAt.y, player->actor.world.pos.y + 35.0f, 0.099999994f,
                                   thisv->subCameraFollowSpeed * 30.0f);
                }
                thisv->subCameraAt.x = player->actor.world.pos.x;
                thisv->subCameraAt.z = player->actor.world.pos.z;
            }

            Gameplay_CameraSetAtEye(globalCtx, 0, &thisv->subCameraAt, &thisv->subCameraEye);

            if (thisv->frameCount == 176) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_SHUTTER, 164.72f, -480.0f,
                                   397.68002f, 0, -0x705C, 0, SHUTTER_GOHMA_BLOCK << 6);
            }

            if (thisv->frameCount == 176) {
                globalCtx->envCtx.unk_BF = 3;
                globalCtx->envCtx.unk_D6 = 0xFFFF;
            }

            if (thisv->frameCount == 190) {
                func_8002DF54(globalCtx, &thisv->actor, 2);
            }

            if (thisv->frameCount >= 228) {
                cam = Gameplay_GetCamera(globalCtx, 0);
                cam->eye = thisv->subCameraEye;
                cam->eyeNext = thisv->subCameraEye;
                cam->at = thisv->subCameraAt;
                func_800C08AC(globalCtx, thisv->subCameraId, 0);
                thisv->subCameraId = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                thisv->actionState = 3;
            }
            break;

        case 3: // wait for the player to look at Gohma
            if (fabsf(thisv->actor.projectedPos.x) < 150.0f && fabsf(thisv->actor.projectedPos.y) < 250.0f &&
                thisv->actor.projectedPos.z < 800.0f && thisv->actor.projectedPos.z > 0.0f) {
                thisv->lookedAtFrames++;
                Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);
                Math_ApproachS(&thisv->actor.world.rot.y,
                               Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) + 0x8000, 2,
                               0xBB8);
                thisv->eyeLidBottomRotX = thisv->eyeLidTopRotX = thisv->eyeIrisRotX = thisv->eyeIrisRotY = 0;
            } else {
                thisv->lookedAtFrames = 0;
                BossGoma_UpdateCeilingMovement(thisv, globalCtx, 0.0f, -5.0f, true);
            }

            if (thisv->lookedAtFrames > 15) {
                BossGoma_SetupEncounterState4(thisv, globalCtx);
            }
            break;

        case 4: // focus Gohma on the ceiling
            if (Animation_OnFrame(&thisv->skelanime, 15.0f)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DEMO_EYE);
            }

            if (thisv->framesUntilNextAction <= 40) {
                // (22, -25, 45) is towards room entrance
                Math_ApproachF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + 22.0f, 0.2f, 100.0f);
                Math_ApproachF(&thisv->subCameraEye.y, thisv->actor.world.pos.y - 25.0f, 0.2f, 100.0f);
                Math_ApproachF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + 45.0f, 0.2f, 100.0f);
                Math_ApproachF(&thisv->subCameraAt.x, thisv->actor.world.pos.x, 0.2f, 100.0f);
                Math_ApproachF(&thisv->subCameraAt.y, thisv->actor.world.pos.y + 5.0f, 0.2f, 100.0f);
                Math_ApproachF(&thisv->subCameraAt.z, thisv->actor.world.pos.z, 0.2f, 100.0f);

                if (thisv->framesUntilNextAction == 30) {
                    globalCtx->envCtx.unk_BF = 4;
                }

                if (thisv->framesUntilNextAction < 20) {
                    SkelAnime_Update(&thisv->skelanime);
                    Math_ApproachF(&thisv->eyeIrisScaleX, 1.0f, 0.8f, 0.4f);
                    Math_ApproachF(&thisv->eyeIrisScaleY, 1.0f, 0.8f, 0.4f);

                    if (Animation_OnFrame(&thisv->skelanime, 36.0f)) {
                        thisv->eyeIrisScaleX = 1.8f;
                        thisv->eyeIrisScaleY = 1.8f;
                    }

                    if (Animation_OnFrame(&thisv->skelanime, thisv->currentAnimFrameCount)) {
                        thisv->actionState = 5;
                        Animation_Change(&thisv->skelanime, &gGohmaWalkAnim, 2.0f, 0.0f,
                                         Animation_GetLastFrame(&gGohmaWalkAnim), ANIMMODE_LOOP, -5.0f);
                        thisv->framesUntilNextAction = 30;
                        thisv->subCameraFollowSpeed = 0.0f;
                    }
                }
            }
            break;

        case 5: // running on the ceiling
            // (98, 0, 85) is towards room entrance
            Math_ApproachF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.y, player->actor.world.pos.y, 0.1f, thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraFollowSpeed, 1.0f, 1.0f, 0.05f);
            thisv->subCameraAt.x = thisv->actor.world.pos.x;
            thisv->subCameraAt.y = thisv->actor.world.pos.y;
            thisv->subCameraAt.z = thisv->actor.world.pos.z;

            if (thisv->framesUntilNextAction < 0) {
                //! @bug ? unreachable, timer is >= 0
                SkelAnime_Update(&thisv->skelanime);
                Math_ApproachZeroF(&thisv->actor.speedXZ, 1.0f, 2.0f);
            } else {
                BossGoma_UpdateCeilingMovement(thisv, globalCtx, 0.0f, -7.5f, false);
            }

            if (thisv->framesUntilNextAction == 0) {
                Animation_Change(&thisv->skelanime, &gGohmaHangAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGohmaHangAnim),
                                 ANIMMODE_LOOP, -5.0f);
            }

            if (thisv->framesUntilNextAction == 0) {
                thisv->actionState = 9;
                thisv->actor.speedXZ = 0.0f;
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.gravity = -2.0f;
                Animation_Change(&thisv->skelanime, &gGohmaInitialLandingAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gGohmaInitialLandingAnim), ANIMMODE_ONCE, -5.0f);
                player->actor.world.pos.x = 0.0f;
                player->actor.world.pos.z = -30.0f;
            }
            break;

        case 9: // falling from the ceiling
            Math_ApproachF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.y, player->actor.world.pos.y + 10.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            thisv->subCameraAt.x = thisv->actor.world.pos.x;
            thisv->subCameraAt.y = thisv->actor.world.pos.y;
            thisv->subCameraAt.z = thisv->actor.world.pos.z;
            SkelAnime_Update(&thisv->skelanime);
            Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);
            Math_ApproachS(&thisv->actor.world.rot.y,
                           Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor), 2, 0x7D0);

            if (thisv->actor.bgCheckFlags & 1) {
                thisv->actionState = 130;
                thisv->actor.velocity.y = 0.0f;
                Animation_Change(&thisv->skelanime, &gGohmaInitialLandingAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gGohmaInitialLandingAnim), ANIMMODE_ONCE, -2.0f);
                thisv->currentAnimFrameCount = Animation_GetLastFrame(&gGohmaInitialLandingAnim);
                BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 0, 5);
                thisv->framesUntilNextAction = 15;
                func_800A9F6C(0.0f, 0xC8, 0x14, 0x14);
            }
            break;

        case 130: // focus Gohma on the ground
            Math_ApproachF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.y, player->actor.world.pos.y + 10.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           thisv->subCameraFollowSpeed * 30.0f);
            Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);
            Math_ApproachS(&thisv->actor.world.rot.y,
                           Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor), 2, 0x7D0);
            SkelAnime_Update(&thisv->skelanime);
            thisv->subCameraAt.x = thisv->actor.world.pos.x;
            thisv->subCameraAt.z = thisv->actor.world.pos.z;

            if (thisv->framesUntilNextAction != 0) {
                f32 s = sinf(thisv->framesUntilNextAction * 3.1415f * 0.5f);

                thisv->subCameraAt.y = thisv->framesUntilNextAction * s * 0.7f + thisv->actor.world.pos.y;
            } else {
                Math_ApproachF(&thisv->subCameraAt.y, thisv->actor.focus.pos.y, 0.1f, 10.0f);
            }

            if (Animation_OnFrame(&thisv->skelanime, 40.0f)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_CRY1);

                if (!(gSaveContext.eventChkInf[7] & 1)) {
                    TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gGohmaTitleCardTex), 0xA0, 0xB4, 0x80, 0x28);
                }

                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
                gSaveContext.eventChkInf[7] |= 1;
            }

            if (Animation_OnFrame(&thisv->skelanime, thisv->currentAnimFrameCount)) {
                thisv->actionState = 140;
                Animation_Change(&thisv->skelanime, &gGohmaStandAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gGohmaStandAnim), ANIMMODE_LOOP, -10.0f);
                thisv->framesUntilNextAction = 20;
            }
            break;

        case 140:
            SkelAnime_Update(&thisv->skelanime);
            Math_ApproachF(&thisv->subCameraAt.y, thisv->actor.focus.pos.y, 0.1f, 10.0f);

            if (thisv->framesUntilNextAction == 0) {
                thisv->framesUntilNextAction = 30;
                thisv->actionState = 150;
                Gameplay_ChangeCameraStatus(globalCtx, 0, 3);
            }
            break;

        case 150:
            SkelAnime_Update(&thisv->skelanime);
            Math_SmoothStepToF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + 150.0f, 0.2f, 100.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.y, thisv->actor.world.pos.y + 20.0f, 0.2f, 100.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + 220.0f, 0.2f, 100.0f, 0.1f);

            if (thisv->framesUntilNextAction == 0) {
                cam = Gameplay_GetCamera(globalCtx, 0);
                cam->eye = thisv->subCameraEye;
                cam->eyeNext = thisv->subCameraEye;
                cam->at = thisv->subCameraAt;
                func_800C08AC(globalCtx, thisv->subCameraId, 0);
                thisv->subCameraId = 0;
                BossGoma_SetupFloorMain(thisv);
                thisv->disableGameplayLogic = false;
                thisv->patienceTimer = 200;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
            }
            break;
    }

    if (thisv->subCameraId != 0) {
        Gameplay_CameraSetAtEye(globalCtx, thisv->subCameraId, &thisv->subCameraAt, &thisv->subCameraEye);
    }
}

/**
 * Handles the "Gohma defeated" cutscene and effects
 * Spawns the heart container and blue warp actors
 */
void BossGoma_Defeated(BossGoma* thisv, GlobalContext* globalCtx) {
    static Vec3f roomCenter = { -150.0f, 0.0f, -350.0f };
    f32 dx;
    f32 dz;
    s16 j;
    Vec3f vel1 = { 0.0f, 0.0f, 0.0f };
    Vec3f accel1 = { 0.0f, 1.0f, 0.0f };
    Color_RGBA8 color1 = { 255, 255, 255, 255 };
    Color_RGBA8 color2 = { 0, 100, 255, 255 };
    Vec3f vel2 = { 0.0f, 0.0f, 0.0f };
    Vec3f accel2 = { 0.0f, -0.5f, 0.0f };
    Vec3f pos;
    Camera* camera;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f childPos;
    s16 i;

    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);

    if (Animation_OnFrame(&thisv->skelanime, 107.0f)) {
        BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 0, 8);
        func_800A9F6C(0.0f, 0x96, 0x14, 0x14);
    }

    thisv->visualState = VISUALSTATE_DEFEATED;
    thisv->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;

    if (thisv->framesUntilNextAction == 1001) {
        for (i = 0; i < 90; i++) {
            if (sDeadLimbLifetime[i] != 0) {
                thisv->deadLimbsState[i] = 1;
            }
        }
    }

    if (thisv->framesUntilNextAction < 1200 && thisv->framesUntilNextAction > 1100 &&
        thisv->framesUntilNextAction % 8 == 0) {
        EffectSsSibuki_SpawnBurst(globalCtx, &thisv->actor.focus.pos);
    }

    if (thisv->framesUntilNextAction < 1080 && thisv->actionState < 3) {
        if (thisv->framesUntilNextAction < 1070) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_LAST - SFX_FLAG);
        }

        for (i = 0; i < 4; i++) {
            //! @bug thisv 0-indexes into thisv->defeatedLimbPositions which is initialized with
            // thisv->defeatedLimbPositions[limb], but limb is 1-indexed in skelanime callbacks, thisv means effects
            // should spawn at thisv->defeatedLimbPositions[0] too, which is uninitialized, so map origin?
            j = (s16)(Rand_ZeroOne() * (BOSSGOMA_LIMB_MAX - 1));
            if (thisv->defeatedLimbPositions[j].y < 10000.0f) {
                pos.x = Rand_CenteredFloat(20.0f) + thisv->defeatedLimbPositions[j].x;
                pos.y = Rand_CenteredFloat(10.0f) + thisv->defeatedLimbPositions[j].y;
                pos.z = Rand_CenteredFloat(20.0f) + thisv->defeatedLimbPositions[j].z;
                func_8002836C(globalCtx, &pos, &vel1, &accel1, &color1, &color2, 500, 10, 10);
            }
        }

        for (i = 0; i < 15; i++) {
            //! @bug same as above
            j = (s16)(Rand_ZeroOne() * (BOSSGOMA_LIMB_MAX - 1));
            if (thisv->defeatedLimbPositions[j].y < 10000.0f) {
                pos.x = Rand_CenteredFloat(20.0f) + thisv->defeatedLimbPositions[j].x;
                pos.y = Rand_CenteredFloat(10.0f) + thisv->defeatedLimbPositions[j].y;
                pos.z = Rand_CenteredFloat(20.0f) + thisv->defeatedLimbPositions[j].z;
                EffectSsHahen_Spawn(globalCtx, &pos, &vel2, &accel2, 0, (s16)(Rand_ZeroOne() * 5.0f) + 10, -1, 10,
                                    NULL);
            }
        }
    }

    switch (thisv->actionState) {
        case 0:
            thisv->actionState = 1;
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 1);
            thisv->subCameraId = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, 0, 3);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->subCameraId, 7);
            camera = Gameplay_GetCamera(globalCtx, 0);
            thisv->subCameraEye.x = camera->eye.x;
            thisv->subCameraEye.y = camera->eye.y;
            thisv->subCameraEye.z = camera->eye.z;
            thisv->subCameraAt.x = camera->at.x;
            thisv->subCameraAt.y = camera->at.y;
            thisv->subCameraAt.z = camera->at.z;
            dx = thisv->subCameraEye.x - thisv->actor.world.pos.x;
            dz = thisv->subCameraEye.z - thisv->actor.world.pos.z;
            thisv->defeatedCameraEyeDist = sqrtf(SQ(dx) + SQ(dz));
            thisv->defeatedCameraEyeAngle = Math_FAtan2F(dx, dz);
            thisv->timer = 270;
            break;

        case 1:
            dx = Math_SinS(thisv->actor.shape.rot.y) * 100.0f;
            dz = Math_CosS(thisv->actor.shape.rot.y) * 100.0f;
            Math_ApproachF(&player->actor.world.pos.x, thisv->actor.world.pos.x + dx, 0.5f, 5.0f);
            Math_ApproachF(&player->actor.world.pos.z, thisv->actor.world.pos.z + dz, 0.5f, 5.0f);

            if (thisv->framesUntilNextAction < 1080) {
                thisv->noBackfaceCulling = true;

                for (i = 0; i < 4; i++) {
                    BossGoma_ClearPixels(sClearPixelTableFirstPass, thisv->decayingProgress);
                    //! @bug thisv allows thisv->decayingProgress = 0x100 = 256 which is out of bounds when accessing
                    // sClearPixelTableFirstPass, though timers may prevent thisv from ever happening?
                    if (thisv->decayingProgress < 0xFF) {
                        thisv->decayingProgress++;
                    }
                }
            }

            if (thisv->framesUntilNextAction < 1070 && thisv->frameCount % 4 == 0 && Rand_ZeroOne() < 0.5f) {
                thisv->blinkTimer = 3;
            }

            thisv->defeatedCameraEyeAngle += 0.022f;
            Math_ApproachF(&thisv->defeatedCameraEyeDist, 150.0f, 0.1f, 5.0f);
            dx = sinf(thisv->defeatedCameraEyeAngle);
            dx = dx * thisv->defeatedCameraEyeDist;
            dz = cosf(thisv->defeatedCameraEyeAngle);
            dz = dz * thisv->defeatedCameraEyeDist;
            Math_SmoothStepToF(&thisv->subCameraEye.x, thisv->actor.world.pos.x + dx, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.y, thisv->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.z, thisv->actor.world.pos.z + dz, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.x, thisv->firstTailLimbWorldPos.x, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.y, thisv->actor.focus.pos.y, 0.5f, 100.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.z, thisv->firstTailLimbWorldPos.z, 0.2f, 50.0f, 0.1f);

            if (thisv->timer == 80) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
            }

            if (thisv->timer == 0) {
                thisv->actionState = 2;
                Gameplay_ChangeCameraStatus(globalCtx, 0, 3);
                thisv->timer = 70;
                thisv->decayingProgress = 0;
                thisv->subCameraFollowSpeed = 0.0f;
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, thisv->actor.world.pos.x,
                            thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
            }
            break;

        case 2:
            camera = Gameplay_GetCamera(globalCtx, 0);
            Math_SmoothStepToF(&thisv->subCameraEye.x, camera->eye.x, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.y, camera->eye.y, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraEye.z, camera->eye.z, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.x, camera->at.x, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.y, camera->at.y, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraAt.z, camera->at.z, 0.2f, thisv->subCameraFollowSpeed * 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->subCameraFollowSpeed, 1.0f, 1.0f, 0.02f, 0.0f);

            if (thisv->timer == 0) {
                childPos = roomCenter;
                thisv->timer = 30;
                thisv->actionState = 3;

                for (i = 0; i < 10000; i++) {
                    if ((fabsf(childPos.x - player->actor.world.pos.x) < 100.0f &&
                         fabsf(childPos.z - player->actor.world.pos.z) < 100.0f) ||
                        (fabsf(childPos.x - thisv->actor.world.pos.x) < 150.0f &&
                         fabsf(childPos.z - thisv->actor.world.pos.z) < 150.0f)) {
                        childPos.x = Rand_CenteredFloat(400.0f) + -150.0f;
                        childPos.z = Rand_CenteredFloat(400.0f) + -350.0f;
                    } else {
                        break;
                    }
                }

                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, childPos.x,
                                   thisv->actor.world.pos.y, childPos.z, 0, 0, 0, WARP_DUNGEON_CHILD);
                Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
            }

            for (i = 0; i < 4; i++) {
                BossGoma_ClearPixels(sClearPixelTableSecondPass, thisv->decayingProgress);
                //! @bug same as sClearPixelTableFirstPass
                if (thisv->decayingProgress < 0xFF) {
                    thisv->decayingProgress++;
                }
            }
            break;

        case 3:
            for (i = 0; i < 4; i++) {
                BossGoma_ClearPixels(sClearPixelTableSecondPass, thisv->decayingProgress);
                //! @bug same as sClearPixelTableFirstPass
                if (thisv->decayingProgress < 0xFF) {
                    thisv->decayingProgress++;
                }
            }

            if (thisv->timer == 0) {
                if (Math_SmoothStepToF(&thisv->actor.scale.y, 0, 1.0f, 0.00075f, 0.0f) <= 0.001f) {
                    camera = Gameplay_GetCamera(globalCtx, 0);
                    camera->eye = thisv->subCameraEye;
                    camera->eyeNext = thisv->subCameraEye;
                    camera->at = thisv->subCameraAt;
                    func_800C08AC(globalCtx, thisv->subCameraId, 0);
                    thisv->subCameraId = 0;
                    func_80064534(globalCtx, &globalCtx->csCtx);
                    func_8002DF54(globalCtx, &thisv->actor, 7);
                    Actor_Kill(&thisv->actor);
                }

                thisv->actor.scale.x = thisv->actor.scale.z = thisv->actor.scale.y;
            }
            break;
    }

    if (thisv->subCameraId != 0) {
        Gameplay_CameraSetAtEye(globalCtx, thisv->subCameraId, &thisv->subCameraAt, &thisv->subCameraEye);
    }

    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
        globalCtx->envCtx.adjAmbientColor[0] += 40;
        globalCtx->envCtx.adjAmbientColor[1] += 40;
        globalCtx->envCtx.adjAmbientColor[2] += 80;
        globalCtx->envCtx.adjFogColor[0] += 10;
        globalCtx->envCtx.adjFogColor[1] += 10;
        globalCtx->envCtx.adjFogColor[2] += 20;
    } else {
        globalCtx->envCtx.adjAmbientColor[0] -= 20;
        globalCtx->envCtx.adjAmbientColor[1] -= 20;
        globalCtx->envCtx.adjAmbientColor[2] -= 40;
        globalCtx->envCtx.adjFogColor[0] -= 5;
        globalCtx->envCtx.adjFogColor[1] -= 5;
        globalCtx->envCtx.adjFogColor[2] -= 10;
    }

    if (globalCtx->envCtx.adjAmbientColor[0] > 200) {
        globalCtx->envCtx.adjAmbientColor[0] = 200;
    }
    if (globalCtx->envCtx.adjAmbientColor[1] > 200) {
        globalCtx->envCtx.adjAmbientColor[1] = 200;
    }
    if (globalCtx->envCtx.adjAmbientColor[2] > 200) {
        globalCtx->envCtx.adjAmbientColor[2] = 200;
    }
    if (globalCtx->envCtx.adjFogColor[0] > 70) {
        globalCtx->envCtx.adjFogColor[0] = 70;
    }
    if (globalCtx->envCtx.adjFogColor[1] > 70) {
        globalCtx->envCtx.adjFogColor[1] = 70;
    }
    if (globalCtx->envCtx.adjFogColor[2] > 140) {
        globalCtx->envCtx.adjFogColor[2] = 140;
    }

    if (globalCtx->envCtx.adjAmbientColor[0] < 0) {
        globalCtx->envCtx.adjAmbientColor[0] = 0;
    }
    if (globalCtx->envCtx.adjAmbientColor[1] < 0) {
        globalCtx->envCtx.adjAmbientColor[1] = 0;
    }
    if (globalCtx->envCtx.adjAmbientColor[2] < 0) {
        globalCtx->envCtx.adjAmbientColor[2] = 0;
    }
    if (globalCtx->envCtx.adjFogColor[0] < 0) {
        globalCtx->envCtx.adjFogColor[0] = 0;
    }
    if (globalCtx->envCtx.adjFogColor[1] < 0) {
        globalCtx->envCtx.adjFogColor[1] = 0;
    }
    if (globalCtx->envCtx.adjFogColor[2] < 0) {
        globalCtx->envCtx.adjFogColor[2] = 0;
    }
}

/**
 * If the player backs off, cancel the attack, or attack.
 */
void BossGoma_FloorAttackPosture(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);

    if (thisv->skelanime.curFrame >= (19.0f + 1.0f / 3.0f) && thisv->skelanime.curFrame <= 30.0f) {
        Math_ApproachS(&thisv->actor.world.rot.y, Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor),
                       3, 0xBB8);
    }

    if (Animation_OnFrame(&thisv->skelanime, Animation_GetLastFrame(&gGohmaPrepareAttackAnim))) {
        if (thisv->actor.xzDistToPlayer < 250.0f) {
            BossGoma_SetupFloorPrepareAttack(thisv);
        } else {
            BossGoma_SetupFloorMain(thisv);
        }
    }

    thisv->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    thisv->visualState = VISUALSTATE_RED;
}

/**
 * Only lasts 1 frame. Plays a sound.
 */
void BossGoma_FloorPrepareAttack(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->framesUntilNextAction == 0) {
        BossGoma_SetupFloorAttack(thisv);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_CRY1);
    }

    thisv->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    thisv->visualState = VISUALSTATE_RED;
}

/**
 * Gohma attacks, then the action eventually goes back to BossGoma_FloorMain
 */
void BossGoma_FloorAttack(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 i;

    thisv->actor.flags |= ACTOR_FLAG_24;
    SkelAnime_Update(&thisv->skelanime);

    switch (thisv->actionState) {
        case 0:
            for (i = 0; i < thisv->collider.count; i++) {
                if (thisv->collider.elements[i].info.toucherFlags & 2) {
                    thisv->framesUntilNextAction = 10;
                    break;
                }
            }

            if (Animation_OnFrame(&thisv->skelanime, 10.0f)) {
                BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 3, 5);
                func_80033E88(&thisv->actor, globalCtx, 5, 15);
            }

            if (Animation_OnFrame(&thisv->skelanime, Animation_GetLastFrame(&gGohmaAttackAnim))) {
                thisv->actionState = 1;
                Animation_Change(&thisv->skelanime, &gGohmaRestAfterAttackAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gGohmaRestAfterAttackAnim), ANIMMODE_LOOP, -1.0f);

                if (thisv->framesUntilNextAction == 0) {
                    thisv->timer = (s16)(Rand_ZeroOne() * 30.0f) + 30;
                }
            }
            break;

        case 1:
            if (Animation_OnFrame(&thisv->skelanime, 3.0f)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_UNARI2);
            }

            if (thisv->timer == 0) {
                thisv->actionState = 2;
                Animation_Change(&thisv->skelanime, &gGohmaRecoverAfterAttackAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gGohmaRecoverAfterAttackAnim), ANIMMODE_ONCE, -5.0f);
            }
            break;

        case 2:
            if (Animation_OnFrame(&thisv->skelanime, Animation_GetLastFrame(&gGohmaRecoverAfterAttackAnim))) {
                BossGoma_SetupFloorIdle(thisv);
            }
            break;
    }

    thisv->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    thisv->visualState = VISUALSTATE_RED;
}

/**
 * Plays the animation to its end, then goes back to BossGoma_FloorStunned
 */
void BossGoma_FloorDamaged(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, Animation_GetLastFrame(&gGohmaDamageAnim))) {
        BossGoma_SetupFloorStunned(thisv);
        thisv->patienceTimer = 0;
    }

    thisv->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    Math_ApproachF(&thisv->eyeIrisScaleX, 0.4f, 0.5f, 0.2f);
    thisv->visualState = VISUALSTATE_HIT;
}

/**
 * Gohma is back on the floor after the player struck it down from the ceiling.
 * Sets patience to 0
 * Gohma is then stunned (BossGoma_FloorStunned)
 */
void BossGoma_FloorLandStruckDown(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, thisv->currentAnimFrameCount)) {
        BossGoma_SetupFloorStunned(thisv);
        thisv->sfxFaintTimer = 92;
        thisv->patienceTimer = 0;
        thisv->framesUntilNextAction = 150;
    }

    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 55.0f, 4, 8.0f, 500, 10, 1);
}

/**
 * Gohma is back on the floor after the player has killed its children Gohmas.
 * Plays an animation then goes to usual floor behavior, with refilled patience.
 */
void BossGoma_FloorLand(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, thisv->currentAnimFrameCount)) {
        BossGoma_SetupFloorIdle(thisv);
        thisv->patienceTimer = 200;
    }
}

/**
 * Gohma is stunned and vulnerable. It can only be damaged during thisv action.
 */
void BossGoma_FloorStunned(BossGoma* thisv, GlobalContext* globalCtx) {
    if (thisv->sfxFaintTimer <= 90) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_FAINT - 0x800);
    }
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->timer == 1) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 55.0f, 4, 8.0f, 500, 10, 1);
    }

    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 1.0f);

    if (thisv->framesUntilNextAction == 0) {
        BossGoma_SetupFloorMain(thisv);
        if (thisv->patienceTimer == 0 && thisv->actor.xzDistToPlayer < 130.0f) {
            thisv->timer = 20;
        }
    }

    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);
    thisv->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    Math_ApproachF(&thisv->eyeIrisScaleX, 0.4f, 0.5f, 0.2f);
    thisv->visualState = VISUALSTATE_STUNNED;
}

/**
 * Gohma goes back to the floor after the player killed the three gohmas it spawned
 */
void BossGoma_FallJump(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);
    Math_ApproachS(&thisv->actor.world.rot.y, Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor), 2,
                   0x7D0);

    if (thisv->actor.bgCheckFlags & 1) {
        BossGoma_SetupFloorLand(thisv);
        thisv->actor.velocity.y = 0.0f;
        BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 0, 8);
        func_80033E88(&thisv->actor, globalCtx, 5, 0xF);
    }
}

/**
 * Gohma falls to the floor after the player hit it
 */
void BossGoma_FallStruckDown(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);
    Math_ApproachS(&thisv->actor.world.rot.y, Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor), 3,
                   0x7D0);

    if (thisv->actor.bgCheckFlags & 1) {
        BossGoma_SetupFloorLandStruckDown(thisv);
        thisv->actor.velocity.y = 0.0f;
        BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 0, 8);
        func_80033E88(&thisv->actor, globalCtx, 0xA, 0xF);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DAM1);
    }
}

/**
 * Spawn three gohmas, one after the other. Cannot be interrupted
 */
void BossGoma_CeilingSpawnGohmas(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 i;

    SkelAnime_Update(&thisv->skelanime);

    if (thisv->frameCount % 16 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_UNARI);
    }

    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);
    thisv->spawnGohmasActionTimer++;

    switch (thisv->spawnGohmasActionTimer) {
        case 24:
            // BOSSGOMA_LIMB_TAIL1, the tail limb closest to the body
            thisv->tailLimbsScaleTimers[3] = 10;
            break;
        case 32:
            // BOSSGOMA_LIMB_TAIL2
            thisv->tailLimbsScaleTimers[2] = 10;
            break;
        case 40:
            // BOSSGOMA_LIMB_TAIL3
            thisv->tailLimbsScaleTimers[1] = 10;
            break;
        case 48:
            // BOSSGOMA_LIMB_TAIL4, the furthest from the body
            thisv->tailLimbsScaleTimers[0] = 10;
            break;
    }

    if (thisv->tailLimbsScaleTimers[0] == 2) {
        for (i = 0; i < ARRAY_COUNT(thisv->childrenGohmaState); i++) {
            if (thisv->childrenGohmaState[i] == 0) {
                BossGoma_SpawnChildGohma(thisv, globalCtx, i);
                break;
            }
        }

        if (thisv->childrenGohmaState[0] == 0 || thisv->childrenGohmaState[1] == 0 || thisv->childrenGohmaState[2] == 0) {
            thisv->spawnGohmasActionTimer = 23;
        }
    }

    if (thisv->spawnGohmasActionTimer >= 64) {
        BossGoma_SetupCeilingIdle(thisv);
    }

    thisv->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
}

/**
 * Prepare to spawn children gohmas, red eye for 70 frames
 * During thisv time, the player can interrupt by hitting Gohma and make it fall from the ceiling
 */
void BossGoma_CeilingPrepareSpawnGohmas(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->framesUntilNextAction == 0) {
        BossGoma_SetupCeilingSpawnGohmas(thisv);
    }

    thisv->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    thisv->visualState = VISUALSTATE_RED;
}

/**
 * On the floor, not doing anything special.
 */
void BossGoma_FloorIdle(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);
    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 2, 0xBB8);

    if (thisv->framesUntilNextAction == 0) {
        BossGoma_SetupFloorMain(thisv);
    }
}

/**
 * On the ceiling, not doing anything special.
 * Eventually spawns children gohmas, jumping down to the floor when they are killed, or staying on the ceiling as long
 * as any is still alive.
 */
void BossGoma_CeilingIdle(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 i;

    SkelAnime_Update(&thisv->skelanime);
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.5f, 2.0f);

    if (thisv->framesUntilNextAction == 0) {
        if (thisv->childrenGohmaState[0] == 0 && thisv->childrenGohmaState[1] == 0 && thisv->childrenGohmaState[2] == 0) {
            // if no child gohma has been spawned
            BossGoma_SetupCeilingPrepareSpawnGohmas(thisv);
        } else if (thisv->childrenGohmaState[0] < 0 && thisv->childrenGohmaState[1] < 0 &&
                   thisv->childrenGohmaState[2] < 0) {
            // if all children gohmas are dead
            BossGoma_SetupFallJump(thisv);
        } else {
            for (i = 0; i < ARRAY_COUNT(thisv->childrenGohmaState); i++) {
                if (thisv->childrenGohmaState[i] == 0) {
                    // if any child gohma hasn't been spawned
                    // thisv seems unreachable since BossGoma_CeilingSpawnGohmas spawns all three and can't be
                    // interrupted
                    BossGoma_SetupCeilingSpawnGohmas(thisv);
                    return;
                }
            }
            // if all children gohmas have been spawned
            BossGoma_SetupCeilingMoveToCenter(thisv);
        }
    }
}

/**
 * Gohma approaches the player as long as it has patience (see patienceTimer), then moves away from the player
 * Gohma climbs any wall it collides with
 * Uses the "walk cautiously" animation
 */
void BossGoma_FloorMain(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 rot;

    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, 1.0f)) {
        thisv->doNotMoveThisFrame = true;
    } else if (Animation_OnFrame(&thisv->skelanime, 30.0f)) {
        thisv->doNotMoveThisFrame = true;
    } else if (Animation_OnFrame(&thisv->skelanime, 15.0f)) {
        thisv->doNotMoveThisFrame = true;
    } else if (Animation_OnFrame(&thisv->skelanime, 16.0f)) {
        thisv->doNotMoveThisFrame = true;
    }

    if (Animation_OnFrame(&thisv->skelanime, 15.0f)) {
        BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 1, 3);
    } else if (Animation_OnFrame(&thisv->skelanime, 30.0f)) {
        BossGoma_PlayEffectsAndSfx(thisv, globalCtx, 2, 3);
    }

    if (thisv->frameCount % 64 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_CRY2);
    }

    if (!thisv->doNotMoveThisFrame) {
        rot = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);

        if (thisv->patienceTimer != 0) {
            thisv->patienceTimer--;

            if (thisv->actor.xzDistToPlayer < 150.0f) {
                BossGoma_SetupFloorAttackPosture(thisv);
            }

            Math_ApproachF(&thisv->actor.speedXZ, 10.0f / 3.0f, 0.5f, 2.0f);
            Math_ApproachS(&thisv->actor.world.rot.y, rot, 5, 0x3E8);
        } else {
            if (thisv->timer != 0) {
                // move away from the player, walking backwards
                Math_ApproachF(&thisv->actor.speedXZ, -10.0f, 0.5f, 2.0f);
                thisv->skelanime.playSpeed = -3.0f;
                if (thisv->timer == 1) {
                    thisv->actor.speedXZ = 0.0f;
                }
            } else {
                // move away from the player, walking forwards
                Math_ApproachF(&thisv->actor.speedXZ, 20.0f / 3.0f, 0.5f, 2.0f);
                thisv->skelanime.playSpeed = 2.0f;
                rot += 0x8000;
            }

            Math_ApproachS(&thisv->actor.world.rot.y, rot, 3, 0x9C4);
        }
    }

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.velocity.y = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 8) {
        BossGoma_SetupWallClimb(thisv);
    }

    if (thisv->framesUntilNextAction == 0 && thisv->patienceTimer != 0) {
        BossGoma_SetupFloorIdle(thisv);
    }
}

/**
 * Gohma moves up until it reaches the ceiling
 */
void BossGoma_WallClimb(BossGoma* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->frameCount % 8 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_CLIM);
    }

    Math_ApproachF(&thisv->actor.velocity.y, 5.0f, 0.5f, 2.0f);
    Math_ApproachS(&thisv->actor.shape.rot.x, -0x4000, 2, 0x7D0);
    Math_ApproachS(&thisv->actor.world.rot.y, thisv->actor.wallYaw + 0x8000, 2, 0x5DC);

    // -320 is a bit below boss room ceiling
    if (thisv->actor.world.pos.y > -320.0f) {
        BossGoma_SetupCeilingMoveToCenter(thisv);
        // allow new spawns
        thisv->childrenGohmaState[0] = thisv->childrenGohmaState[1] = thisv->childrenGohmaState[2] = 0;
    }
}

/**
 * Goes to BossGoma_CeilingIdle after enough time and after being close enough to the center of the ceiling.
 */
void BossGoma_CeilingMoveToCenter(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 angle;
    s16 absDiff;

    BossGoma_UpdateCeilingMovement(thisv, globalCtx, 0.0f, -5.0f, true);

    if (thisv->frameCount % 64 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_CRY2);
    }

    Math_ApproachS(&thisv->actor.shape.rot.x, -0x8000, 3, 0x3E8);

    // avoid walking into a wall?
    if (thisv->actor.bgCheckFlags & 8) {
        angle = thisv->actor.shape.rot.y + 0x8000;

        if (angle < thisv->actor.wallYaw) {
            absDiff = thisv->actor.wallYaw - angle;
            angle = angle + absDiff / 2;
        } else {
            absDiff = angle - thisv->actor.wallYaw;
            angle = thisv->actor.wallYaw + absDiff / 2;
        }

        thisv->actor.world.pos.z += Math_CosS(angle) * (5.0f + Rand_ZeroOne() * 5.0f) + Rand_CenteredFloat(2.0f);
        thisv->actor.world.pos.x += Math_SinS(angle) * (5.0f + Rand_ZeroOne() * 5.0f) + Rand_CenteredFloat(2.0f);
    }

    // timer setup to 30-60
    if (thisv->framesUntilNextAction == 0 && fabsf(-150.0f - thisv->actor.world.pos.x) < 100.0f &&
        fabsf(-350.0f - thisv->actor.world.pos.z) < 100.0f) {
        BossGoma_SetupCeilingIdle(thisv);
    }
}

/**
 * Update eye-related properties
 *  - open/close (eye lid rotation)
 *  - look at the player (iris rotation)
 *  - iris scale, when menacing or damaged
 */
void BossGoma_UpdateEye(BossGoma* thisv, GlobalContext* globalCtx) {
    s16 targetEyeIrisRotX;
    s16 targetEyeIrisRotY;

    if (!thisv->disableGameplayLogic) {
        Player* player = GET_PLAYER(globalCtx);

        if (thisv->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES) {
            // player + 0xA73 seems to be related to "throwing something"
            if (player->unk_A73 != 0) {
                player->unk_A73 = 0;
                thisv->eyeClosedTimer = 12;
            }

            if (thisv->frameCount % 16 == 0 && Rand_ZeroOne() < 0.3f) {
                thisv->eyeClosedTimer = 7;
            }
        }

        if (thisv->childrenGohmaState[0] > 0 || thisv->childrenGohmaState[1] > 0 || thisv->childrenGohmaState[2] > 0) {
            thisv->eyeClosedTimer = 7;
        }

        if (thisv->eyeClosedTimer != 0) {
            thisv->eyeClosedTimer--;
            // close eye
            Math_ApproachS(&thisv->eyeLidBottomRotX, -0xA98, 1, 0x7D0);
            Math_ApproachS(&thisv->eyeLidTopRotX, 0x1600, 1, 0x7D0);
        } else {
            // open eye
            Math_ApproachS(&thisv->eyeLidBottomRotX, 0, 1, 0x7D0);
            Math_ApproachS(&thisv->eyeLidTopRotX, 0, 1, 0x7D0);
        }

        if (thisv->eyeState != EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES) {
            targetEyeIrisRotY =
                Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.shape.rot.y;
            targetEyeIrisRotX =
                Actor_WorldPitchTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.shape.rot.x;

            if (thisv->actor.shape.rot.x > 0x4000 || thisv->actor.shape.rot.x < -0x4000) {
                targetEyeIrisRotY = -(s16)(targetEyeIrisRotY + 0x8000);
                targetEyeIrisRotX = -0xBB8;
            }

            if (targetEyeIrisRotY > 0x1770) {
                targetEyeIrisRotY = 0x1770;
            }

            if (targetEyeIrisRotY < -0x1770) {
                targetEyeIrisRotY = -0x1770;
            }

            Math_ApproachS(&thisv->eyeIrisRotY, targetEyeIrisRotY, 3, 0x7D0);
            Math_ApproachS(&thisv->eyeIrisRotX, targetEyeIrisRotX, 3, 0x7D0);
        } else {
            Math_ApproachS(&thisv->eyeIrisRotY, 0, 3, 0x3E8);
            Math_ApproachS(&thisv->eyeIrisRotX, 0, 3, 0x3E8);
        }

        Math_ApproachF(&thisv->eyeIrisScaleX, 1.0f, 0.2f, 0.07f);
        Math_ApproachF(&thisv->eyeIrisScaleY, 1.0f, 0.2f, 0.07f);
    }
}

/**
 * Part of achieving visual effects when spawning children gohmas,
 * inflating each tail limb one after the other.
 */
void BossGoma_UpdateTailLimbsScale(BossGoma* thisv) {
    s16 i;

    if (thisv->frameCount % 128 == 0) {
        thisv->unusedTimer++;
        if (thisv->unusedTimer >= 3) {
            thisv->unusedTimer = 0;
        }
    }

    // See BossGoma_CeilingSpawnGohmas for `tailLimbsScaleTimers` usage
    for (i = 0; i < ARRAY_COUNT(thisv->tailLimbsScaleTimers); i++) {
        if (thisv->tailLimbsScaleTimers[i] != 0) {
            thisv->tailLimbsScaleTimers[i]--;
            Math_ApproachF(&thisv->tailLimbsScale[i], 1.5f, 0.2f, 0.1f);
        } else {
            Math_ApproachF(&thisv->tailLimbsScale[i], 1.0f, 0.2f, 0.1f);
        }
    }
}

void BossGoma_UpdateHit(BossGoma* thisv, GlobalContext* globalCtx) {
    if (thisv->invincibilityFrames != 0) {
        thisv->invincibilityFrames--;
    } else {
        ColliderInfo* acHitInfo = thisv->collider.elements[0].info.acHitInfo;
        s32 damage;

        if (thisv->eyeClosedTimer == 0 && thisv->actionFunc != BossGoma_CeilingSpawnGohmas &&
            (thisv->collider.elements[0].info.bumperFlags & BUMP_HIT)) {
            thisv->collider.elements[0].info.bumperFlags &= ~BUMP_HIT;

            if (thisv->actionFunc == BossGoma_CeilingMoveToCenter || thisv->actionFunc == BossGoma_CeilingIdle ||
                thisv->actionFunc == BossGoma_CeilingPrepareSpawnGohmas) {
                BossGoma_SetupFallStruckDown(thisv);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DAM2);
            } else if (thisv->actionFunc == BossGoma_FloorStunned &&
                       (damage = CollisionCheck_GetSwordDamage(acHitInfo->toucher.dmgFlags)) != 0) {
                thisv->actor.colChkInfo.health -= damage;

                if ((s8)thisv->actor.colChkInfo.health > 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DAM1);
                    BossGoma_SetupFloorDamaged(thisv);
                    EffectSsSibuki_SpawnBurst(globalCtx, &thisv->actor.focus.pos);
                } else {
                    BossGoma_SetupDefeated(thisv, globalCtx);
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                }

                thisv->invincibilityFrames = 10;
            } else if (thisv->actionFunc != BossGoma_FloorStunned && thisv->patienceTimer != 0 &&
                       (acHitInfo->toucher.dmgFlags & 0x00000005)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_DAM2);
                Audio_StopSfxById(NA_SE_EN_GOMA_CRY1);
                thisv->invincibilityFrames = 10;
                BossGoma_SetupFloorStunned(thisv);
                thisv->sfxFaintTimer = 100;

                if (acHitInfo->toucher.dmgFlags & 1) {
                    thisv->framesUntilNextAction = 40;
                } else {
                    thisv->framesUntilNextAction = 90;
                }

                thisv->timer = 4;
                func_80033E88(&thisv->actor, globalCtx, 4, 0xC);
            }
        }
    }
}

void BossGoma_UpdateMainEnvColor(BossGoma* thisv) {
    static f32 colors1[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 0.0f, 255.0f, 170.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f },
    };
    static f32 colors2[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 0.0f, 255.0f, 170.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 0.0f, 255.0f },   { 255.0f, 17.0f, 0.0f },
    };

    if (thisv->visualState == VISUALSTATE_DEFAULT && thisv->frameCount & 0x10) {
        Math_ApproachF(&thisv->mainEnvColor[0], 50.0f, 0.5f, 20.0f);
        Math_ApproachF(&thisv->mainEnvColor[1], 50.0f, 0.5f, 20.0f);
        Math_ApproachF(&thisv->mainEnvColor[2], 50.0f, 0.5f, 20.0f);
    } else if (thisv->invincibilityFrames != 0) {
        if (thisv->invincibilityFrames & 2) {
            thisv->mainEnvColor[0] = colors2[thisv->visualState][0];
            thisv->mainEnvColor[1] = colors2[thisv->visualState][1];
            thisv->mainEnvColor[2] = colors2[thisv->visualState][2];
        } else {
            thisv->mainEnvColor[0] = colors1[thisv->visualState][0];
            thisv->mainEnvColor[1] = colors1[thisv->visualState][1];
            thisv->mainEnvColor[2] = colors1[thisv->visualState][2];
        }
    } else {
        Math_ApproachF(&thisv->mainEnvColor[0], colors1[thisv->visualState][0], 0.5f, 20.0f);
        Math_ApproachF(&thisv->mainEnvColor[1], colors1[thisv->visualState][1], 0.5f, 20.0f);
        Math_ApproachF(&thisv->mainEnvColor[2], colors1[thisv->visualState][2], 0.5f, 20.0f);
    }
}

void BossGoma_UpdateEyeEnvColor(BossGoma* thisv) {
    static f32 targetEyeEnvColors[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 255.0f, 255.0f, 255.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f },   { 0.0f, 255.0f, 170.0f },
    };

    Math_ApproachF(&thisv->eyeEnvColor[0], targetEyeEnvColors[thisv->visualState][0], 0.5f, 20.0f);
    Math_ApproachF(&thisv->eyeEnvColor[1], targetEyeEnvColors[thisv->visualState][1], 0.5f, 20.0f);
    Math_ApproachF(&thisv->eyeEnvColor[2], targetEyeEnvColors[thisv->visualState][2], 0.5f, 20.0f);
}

void BossGoma_Update(Actor* thisx, GlobalContext* globalCtx) {
    BossGoma* thisv = (BossGoma*)thisx;
    s32 pad;

    thisv->visualState = VISUALSTATE_DEFAULT;
    thisv->frameCount++;

    if (thisv->framesUntilNextAction != 0) {
        thisv->framesUntilNextAction--;
    }

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->sfxFaintTimer != 0) {
        thisv->sfxFaintTimer--;
    }

    if (1) {}

    thisv->eyeState = EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES;
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (!thisv->doNotMoveThisFrame) {
        Actor_MoveForward(&thisv->actor);
    } else {
        thisv->doNotMoveThisFrame = false;
    }

    if (thisv->actor.world.pos.y < -400.0f) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 30.0f, 30.0f, 80.0f, 5);
    } else {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 30.0f, 80.0f, 1);
    }

    BossGoma_UpdateEye(thisv, globalCtx);
    BossGoma_UpdateMainEnvColor(thisv);
    BossGoma_UpdateEyeEnvColor(thisv);
    BossGoma_UpdateTailLimbsScale(thisv);

    if (!thisv->disableGameplayLogic) {
        BossGoma_UpdateHit(thisv, globalCtx);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

        if (thisv->actionFunc != BossGoma_FloorStunned && thisv->actionFunc != BossGoma_FloorDamaged &&
            (thisv->actionFunc != BossGoma_FloorMain || thisv->timer == 0)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

s32 BossGoma_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    BossGoma* thisv = (BossGoma*)thisx;
    s32 doNotDrawLimb = false;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_goma.c", 4685);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->mainEnvColor[0], (s16)thisv->mainEnvColor[1], (s16)thisv->mainEnvColor[2],
                   255);

    if (thisv->deadLimbsState[limbIndex] >= 2) {
        *dList = NULL;
    }

    switch (limbIndex) {
        case BOSSGOMA_LIMB_EYE:
            if (thisv->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES && thisv->eyeLidBottomRotX < -0xA8C) {
                *dList = NULL;
            } else if (thisv->invincibilityFrames != 0) {
                gDPSetEnvColor(POLY_OPA_DISP++, (s16)(Rand_ZeroOne() * 255.0f), (s16)(Rand_ZeroOne() * 255.0f),
                               (s16)(Rand_ZeroOne() * 255.0f), 63);
            } else {
                gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->eyeEnvColor[0], (s16)thisv->eyeEnvColor[1],
                               (s16)thisv->eyeEnvColor[2], 63);
            }
            break;

        case BOSSGOMA_LIMB_EYE_LID_BOTTOM_ROOT2:
            rot->x += thisv->eyeLidBottomRotX;
            break;

        case BOSSGOMA_LIMB_EYE_LID_TOP_ROOT2:
            rot->x += thisv->eyeLidTopRotX;
            break;

        case BOSSGOMA_LIMB_IRIS_ROOT2:
            rot->x += thisv->eyeIrisRotX;
            rot->y += thisv->eyeIrisRotY;
            break;

        case BOSSGOMA_LIMB_IRIS:
            if (thisv->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES && thisv->eyeLidBottomRotX < -0xA8C) {
                *dList = NULL;
            } else {
                if (thisv->visualState == VISUALSTATE_DEFEATED) {
                    gDPSetEnvColor(POLY_OPA_DISP++, 50, 50, 50, 255);
                } else {
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
                }

                Matrix_TranslateRotateZYX(pos, rot);

                if (*dList != NULL) {
                    Matrix_Push();
                    Matrix_Scale(thisv->eyeIrisScaleX, thisv->eyeIrisScaleY, 1.0f, MTXMODE_APPLY);
                    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_goma.c", 4815),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(POLY_OPA_DISP++, *dList);
                    Matrix_Pop();
                }

                doNotDrawLimb = true;
            }
            break;

        case BOSSGOMA_LIMB_TAIL4:
        case BOSSGOMA_LIMB_TAIL3:
        case BOSSGOMA_LIMB_TAIL2:
        case BOSSGOMA_LIMB_TAIL1:
            Matrix_TranslateRotateZYX(pos, rot);

            if (*dList != NULL) {
                Matrix_Push();
                Matrix_Scale(thisv->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4],
                             thisv->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4],
                             thisv->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4], MTXMODE_APPLY);
                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_goma.c", 4836),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, *dList);
                Matrix_Pop();
            }

            doNotDrawLimb = true;
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_goma.c", 4858);

    return doNotDrawLimb;
}

void BossGoma_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f tailZero = { 0.0f, 0.0f, 0.0f };
    static Vec3f clawBackLocalPos = { 0.0f, 0.0f, 0.0f };
    static Vec3f focusEyeLocalPos = { 0.0f, 300.0f, 2650.0f }; // in the center of the surface of the lens
    static Vec3f zero = { 0.0f, 0.0f, 0.0f };
    Vec3f childPos;
    Vec3s childRot;
    EnGoma* babyGohma;
    BossGoma* thisv = (BossGoma*)thisx;
    s32 pad;
    MtxF mtx;

    if (limbIndex == BOSSGOMA_LIMB_TAIL4) { // tail end/last part
        Matrix_MultVec3f(&tailZero, &thisv->lastTailLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_TAIL1) { // tail start/first part
        Matrix_MultVec3f(&tailZero, &thisv->firstTailLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_EYE) {
        Matrix_MultVec3f(&focusEyeLocalPos, &thisv->actor.focus.pos);
    } else if (limbIndex == BOSSGOMA_LIMB_R_FEET_BACK) {
        Matrix_MultVec3f(&clawBackLocalPos, &thisv->rightHandBackLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_L_FEET_BACK) {
        Matrix_MultVec3f(&clawBackLocalPos, &thisv->leftHandBackLimbWorldPos);
    }

    if (thisv->visualState == VISUALSTATE_DEFEATED) {
        if (*dList != NULL) {
            Matrix_MultVec3f(&clawBackLocalPos, &thisv->defeatedLimbPositions[limbIndex]);
        } else {
            thisv->defeatedLimbPositions[limbIndex].y = 10000.0f;
        }
    }

    if (thisv->deadLimbsState[limbIndex] == 1) {
        thisv->deadLimbsState[limbIndex] = 2;
        Matrix_MultVec3f(&zero, &childPos);
        Matrix_Get(&mtx);
        Matrix_MtxFToYXZRotS(&mtx, &childRot, 0);
        // These are the pieces of Gohma as it falls apart. It appears to use the same actor as the baby gohmas.
        babyGohma = (EnGoma*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_GOMA,
                                                childPos.x, childPos.y, childPos.z, childRot.x, childRot.y, childRot.z,
                                                sDeadLimbLifetime[limbIndex] + 100);
        if (babyGohma != NULL) {
            babyGohma->bossLimbDl = *dList;
            babyGohma->actor.objBankIndex = thisv->actor.objBankIndex;
        }
    }

    Collider_UpdateSpheres(limbIndex, &thisv->collider);
}

Gfx* BossGoma_EmptyDlist(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = dListHead = static_cast<Gfx*>( Graph_Alloc(gfxCtx, sizeof(Gfx) * 1) );

    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* BossGoma_NoBackfaceCullingDlist(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = dListHead = static_cast<Gfx*>( Graph_Alloc(gfxCtx, sizeof(Gfx) * 4) );

    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void BossGoma_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BossGoma* thisv = (BossGoma*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_goma.c", 4991);

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Translate(0.0f, -4000.0f, 0.0f, MTXMODE_APPLY);

    // Invalidate Texture Cache since Goma modifies her own texture
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaBodyTex));
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaShellUndersideTex));
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaDarkShellTex));
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaEyeTex));
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaShellTex));
    gSPInvalidateTexCache(POLY_OPA_DISP++, reinterpret_cast<std::uintptr_t>(gGohmaIrisTex));

    if (thisv->noBackfaceCulling) {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGoma_NoBackfaceCullingDlist(globalCtx->state.gfxCtx));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGoma_EmptyDlist(globalCtx->state.gfxCtx));
    }

    SkelAnime_DrawOpa(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, BossGoma_OverrideLimbDraw,
                      BossGoma_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_goma.c", 5012);
}

void BossGoma_SpawnChildGohma(BossGoma* thisv, GlobalContext* globalCtx, s16 i) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_GOMA, thisv->lastTailLimbWorldPos.x,
                       thisv->lastTailLimbWorldPos.y - 50.0f, thisv->lastTailLimbWorldPos.z, 0, i * (0x10000 / 3), 0, i);

    thisv->childrenGohmaState[i] = 1;
}
