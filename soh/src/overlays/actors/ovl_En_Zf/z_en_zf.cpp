/*
 * File: z_en_zf.c
 * Overlay: ovl_En_Zf
 * Description: Lizalfos and Dinolfos
 */

#include "z_en_zf.h"
#include "objects/object_zf/object_zf.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnZf_Init(Actor* thisx, GlobalContext* globalCtx);
void EnZf_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnZf_Update(Actor* thisx, GlobalContext* globalCtx);
void EnZf_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnZf_Reset(void);

s16 EnZf_FindPlatform(Vec3f* pos, s16 preferredIndex);
void EnZf_SetupDropIn(EnZf* thisv);
void EnZf_DropIn(EnZf* thisv, GlobalContext* globalCtx);
void func_80B45384(EnZf* thisv);
void func_80B4543C(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupApproachPlayer(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_ApproachPlayer(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupJumpForward(EnZf* thisv);
void EnZf_JumpForward(EnZf* thisv, GlobalContext* globalCtx);
void func_80B4604C(EnZf* thisv);
void func_80B46098(EnZf* thisv, GlobalContext* globalCtx);
void func_80B462E4(EnZf* thisv, GlobalContext* globalCtx);
void func_80B463E4(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupSlash(EnZf* thisv);
void EnZf_Slash(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_RecoilFromBlockedSlash(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupJumpBack(EnZf* thisv);
void EnZf_JumpBack(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_Stunned(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupSheatheSword(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SheatheSword(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_HopAndTaunt(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupHopAway(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_HopAway(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_DrawSword(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_Damaged(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupJumpUp(EnZf* thisv);
void EnZf_JumpUp(EnZf* thisv, GlobalContext* globalCtx);
void func_80B483E4(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_CircleAroundPlayer(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupDie(EnZf* thisv);
void EnZf_Die(EnZf* thisv, GlobalContext* globalCtx);
void EnZf_SetupCircleAroundPlayer(EnZf* thisv, f32 speed);
s32 EnZf_DodgeRangedEngaging(GlobalContext* globalCtx, EnZf* thisv);
s32 EnZf_DodgeRangedWaiting(GlobalContext* globalCtx, EnZf* thisv);

#define PLATFORM_INDEX_DOWNSTAIRS_MIN 0
#define PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX 5
#define PLATFORM_INDEX_DOWNSTAIRS_MAX 7
#define PLATFORM_INDEX_UPSTAIRS_MIN (PLATFORM_INDEX_DOWNSTAIRS_MAX + 1)
#define PLATFORM_INDEX_UPSTAIRS_INNER_MAX 15

/**
 * Array of platform positions in Dodongo's Cavern miniboss room.
 *  0 -  7 : Downstairs
 *  8 - 15 : Upstairs inner platforms
 * 16 - 23 : Upstairs outer platforms (including several points on the long thin one)
 */
static Vec3f sPlatformPositions[] = {
    // Downstairs
    { 3560.0f, 100.0f, -1517.0f },
    { 3170.0f, 100.0f, -1767.0f },
    { 3165.0f, 100.0f, -2217.0f },
    { 3563.0f, 100.0f, -2437.0f },
    { 3946.0f, 100.0f, -2217.0f },
    { 3942.0f, 100.0f, -1765.0f },
    // Westmost downstairs two
    { 2861.0f, 100.0f, -2394.0f },
    { 2776.0f, 100.0f, -1987.0f },

    // Upstairs inner
    { 4527.0f, 531.0f, -1146.0f },
    { 4442.0f, 531.0f, -1405.0f },
    { 4170.0f, 531.0f, -1395.0f },
    { 4030.0f, 531.0f, -1162.0f },
    { 4010.0f, 531.0f, -883.0f },
    { 4270.0f, 531.0f, -810.0f },
    { 4520.0f, 531.0f, -880.0f },
    { 4260.0f, 531.0f, -1035.0f },

    // Upstairs outer
    { 4757.0f, 531.0f, -1146.0f },
    { 3850.0f, 531.0f, -883.0f },
    { 4380.0f, 531.0f, -690.0f },
    { 4197.0f, 531.0f, -646.0f },
    { 4070.0f, 531.0f, -1575.0f },
    { 3930.0f, 531.0f, -1705.0f },
    { 3780.0f, 531.0f, -1835.0f },
    { 3560.0f, 531.0f, -1985.0f },
};

// These seem to relate to the tagging in/out the minibosses do
static s16 D_80B4A1B0 = 0;
static s16 D_80B4A1B4 = 1;

const ActorInit En_Zf_InitVars = {
    ACTOR_EN_ZF,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ZF,
    sizeof(EnZf),
    (ActorFunc)EnZf_Init,
    (ActorFunc)EnZf_Destroy,
    (ActorFunc)EnZf_Update,
    (ActorFunc)EnZf_Draw,
    (ActorResetFunc)EnZf_Reset,
};

static ColliderCylinderInit sBodyCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sSwordQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ ENZF_DMGEFF_NONE,
    /* 0x1 */ ENZF_DMGEFF_STUN,
    /* 0x6 */ ENZF_DMGEFF_IMMUNE = 6,       // Skips damage code, but also skips the top half of Update
    /* 0xD */ ENZF_DMGEFF_PROJECTILE = 0xD, // Projectiles that don't have another damageeffect
    /* 0xF */ ENZF_DMGEFF_ICE = 0xF
} EnZfDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, ENZF_DMGEFF_PROJECTILE),
    /* Explosive     */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant's Knife */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Fire arrow    */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Ice arrow     */ DMG_ENTRY(4, ENZF_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 1   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Fire magic    */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Ice magic     */ DMG_ENTRY(3, ENZF_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Shield        */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, ENZF_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

static AnimationHeader* sHoppingAnims[] = { &gZfHopCrouchingAnim, &gZfHopLeapingAnim, &gZfHopLandingAnim };

static s32 D_80B4AB30; // Set to 0 and incremented in EnZf_HopAway, but not actually used

void EnZf_SetupAction(EnZf* thisv, EnZfActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

/**
 * Tests if it will still be on a floor after moving forwards a distance determined by dist, in the shape forward
 * direction. If `dist` is 0, it defaults to a dist depending on speed direction, and params.
 */
s32 EnZf_PrimaryFloorCheck(EnZf* thisv, GlobalContext* globalCtx, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if (dist == 0.0f) {
        dist = ((thisv->actor.speedXZ >= 0.0f) ? 1.0f : -1.0f);
        dist = ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) ? dist * 45.0f : dist * 30.0f);
    }

    // Save currents to restore later
    curPos = thisv->actor.world.pos;
    curBgCheckFlags = thisv->actor.bgCheckFlags;

    sin = Math_SinS(thisv->actor.world.rot.y) * dist;
    cos = Math_CosS(thisv->actor.world.rot.y) * dist;

    thisv->actor.world.pos.x += sin;
    thisv->actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 0x1C);
    thisv->actor.world.pos = curPos;
    ret = !(thisv->actor.bgCheckFlags & 1);
    thisv->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

/**
 * Supplementary floor test.
 */
s16 EnZf_SecondaryFloorCheck(EnZf* thisv, GlobalContext* globalCtx, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if ((thisv->actor.speedXZ != 0.0f) && EnZf_PrimaryFloorCheck(thisv, globalCtx, thisv->actor.speedXZ)) {
        return true;
    }

    // Save currents to restore later
    curPos = thisv->actor.world.pos;
    curBgCheckFlags = thisv->actor.bgCheckFlags;

    sin = Math_SinS(thisv->actor.shape.rot.y) * dist;
    cos = Math_CosS(thisv->actor.shape.rot.y) * dist;

    thisv->actor.world.pos.x += sin;
    thisv->actor.world.pos.z += cos;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 0x1C);
    thisv->actor.world.pos = curPos;
    ret = !(thisv->actor.bgCheckFlags & 1);
    thisv->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

void EnZf_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZf* thisv = (EnZf*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    EffectBlureInit1 blureInit;
    f32 posDiff;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->targetMode = 3;
    thisv->clearFlag = (thisx->params & 0xFF00) >> 8;
    /* Strip the top byte of params */
    thisx->params &= 0xFF;

    /* Return the params to their original value if they were originally negative, i.e. 0xFFFF or 0xFFFE */
    if (thisx->params & 0x80) {
        thisx->params |= 0xFF00;
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawFeet, 90.0f);
    thisv->unk_3E0 = 0;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.damageTable = &sDamageTable;

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    Effect_Add(globalCtx, &thisv->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);

    Actor_UpdateBgCheckInfo(globalCtx, thisx, 75.0f, 45.0f, 45.0f, 0x1D);

    thisv->alpha = 255;
    thisx->colChkInfo.cylRadius = 40;
    thisx->colChkInfo.cylHeight = 100;
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, thisx, &sBodyCylinderInit);
    Collider_InitQuad(globalCtx, &thisv->swordCollider);
    Collider_SetQuad(globalCtx, &thisv->swordCollider, thisx, &sSwordQuadInit);

    if (thisx->params == ENZF_TYPE_DINOLFOS) {
        thisx->colChkInfo.health = 12;
        thisx->naviEnemyId = 0x10;
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gZfDinolfosSkel, &gZfCryingAnim, thisv->jointTable,
                       thisv->morphTable, ENZF_LIMB_MAX);
    } else { // Lizalfos
        thisx->colChkInfo.health = 6;
        thisx->naviEnemyId = 0x0F;
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gZfLizalfosSkel, &gZfCryingAnim, thisv->jointTable,
                       thisv->morphTable, ENZF_LIMB_MAX);
    }

    if (thisx->params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Not minibosses
        thisv->homePlatform = thisv->curPlatform = -1;
        D_80B4A1B4 = -1;
        thisv->hopAnimIndex = 1;
        if (thisx->params == ENZF_TYPE_LIZALFOS_LONE) {
            EnZf_SetupDropIn(thisv);
        } else { // Dinolfos
            func_80B45384(thisv);
        }
    } else { // Minibosses
        posDiff = player->actor.world.pos.y - thisx->world.pos.y;

        if ((ABS(posDiff) <= 100.0f) && !Flags_GetSwitch(globalCtx, thisv->clearFlag)) {
            thisv->homePlatform = thisv->curPlatform = EnZf_FindPlatform(&thisx->world.pos, 0);
            EnZf_SetupDropIn(thisv);
            D_80B4A1B4 = 1;
        } else {
            Actor_Kill(thisx);
        }
    }
}

void EnZf_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnZf* thisv = (EnZf*)thisx;

    if ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
        (Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_ZF, ACTORCAT_ENEMY, 10000.0f) == NULL)) {
        func_800F5B58();
    }

    Effect_Delete(globalCtx, thisv->blureIndex);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyQuad(globalCtx, &thisv->swordCollider);
}

/**
 * Finds the index of the platform position in `sPlatformPositions` that is sufficiently close to `pos`.
 * Returns `preferedIndex` if the associated position is close enough.
 */
s16 EnZf_FindPlatform(Vec3f* pos, s16 preferredIndex) {
    f32 rangeXZ;
    s16 i;

    rangeXZ = 210.0f;

    // Upstairs has a smaller range
    if (pos->y >= 420.0f) {
        rangeXZ = 110.0f;
    }

    if (preferredIndex != -1) {
        i = preferredIndex;
        if (((sPlatformPositions[i].y - 150.0f) <= pos->y) && (pos->y <= (sPlatformPositions[i].y + 150.0f)) &&
            ((sPlatformPositions[i].x - rangeXZ) <= pos->x) && (pos->x <= (sPlatformPositions[i].x + rangeXZ)) &&
            ((sPlatformPositions[i].z - rangeXZ) <= pos->z) && (pos->z <= (sPlatformPositions[i].z + rangeXZ))) {
            return preferredIndex;
        }
    }

    for (i = ARRAY_COUNT(sPlatformPositions) - 1; i > -1; i--) {
        if (((sPlatformPositions[i].y - 150.0f) <= pos->y) && (pos->y <= (sPlatformPositions[i].y + 150.0f)) &&
            ((sPlatformPositions[i].x - rangeXZ) <= pos->x) && (pos->x <= (sPlatformPositions[i].x + rangeXZ)) &&
            ((sPlatformPositions[i].z - rangeXZ) <= pos->z) && (pos->z <= (sPlatformPositions[i].z + rangeXZ))) {
            break;
        }
    }
    return i;
}

s16 EnZf_FindNextPlatformAwayFromPlayer(Vec3f* pos, s16 curPlatform, s16 arg2, GlobalContext* globalCtx) {
    f32 distToCurLoopPlatform;
    f32 platformMinDist = 585.0f;
    Player* player = GET_PLAYER(globalCtx);
    s16 initialPlatform = curPlatform;
    f32 playerMaxDist = 400.0f;
    f32 smallMaxRange = 99998.0f;
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX; // Will never retreat to the last two
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    f32 largeMaxRange = 99999.0f;
    s16 altNextPlatform = -1;
    s16 nextPlatform = -1;
    s16 playerPlatform = EnZf_FindPlatform(&player->actor.world.pos, initialPlatform);

    // Set up search constraints
    // Upstairs
    if (pos->y > 420.0f) {
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        playerMaxDist = 50.0f;

        // Upstairs outer
        if (initialPlatform >= PLATFORM_INDEX_UPSTAIRS_INNER_MAX) {
            curLoopPlatform = ARRAY_COUNT(sPlatformPositions) - 1;
            platformMinDist = 400.0f;
        } else { // upstairs inner
            curLoopPlatform = PLATFORM_INDEX_UPSTAIRS_INNER_MAX - 1;
            platformMinDist = 380.0f;
        }
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if ((curLoopPlatform == initialPlatform) || (curLoopPlatform == playerPlatform)) {
            continue;
        }
        if ((playerPlatform == -1) &&
            (Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[curLoopPlatform]) < playerMaxDist)) {
            continue;
        }
        distToCurLoopPlatform = Math_Vec3f_DistXYZ(pos, &sPlatformPositions[curLoopPlatform]);

        if (platformMinDist < distToCurLoopPlatform) {
            continue;
        }
        if (distToCurLoopPlatform < smallMaxRange) {
            largeMaxRange = smallMaxRange;
            altNextPlatform = nextPlatform;
            smallMaxRange = distToCurLoopPlatform;
            nextPlatform = curLoopPlatform;
        } else if (distToCurLoopPlatform < largeMaxRange) {
            largeMaxRange = distToCurLoopPlatform;
            altNextPlatform = curLoopPlatform;
        }
    }

    // These functions have no side effects, so these two calls do nothing
    Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[nextPlatform]);
    Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[altNextPlatform]);

    if (altNextPlatform > 0) {
        s16 nextPlatformToPlayerYaw =
            Math_Vec3f_Yaw(pos, &sPlatformPositions[nextPlatform]) - Math_Vec3f_Yaw(pos, &player->actor.world.pos);

        if (ABS(nextPlatformToPlayerYaw) < 0x36B0) {
            nextPlatform = altNextPlatform;
        }
    }

    if (nextPlatform < 0) {
        nextPlatform = arg2;
    }
    return nextPlatform;
}

s16 EnZf_FindNextPlatformTowardsPlayer(Vec3f* pos, s16 curPlatform, s16 arg2, GlobalContext* globalCtx) {
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_MAX;
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    Player* player = GET_PLAYER(globalCtx);
    s16 nextPlatform = EnZf_FindPlatform(&player->actor.world.pos, -1);
    f32 minRange = 500.0f;
    f32 smallMaxRange = 99998.0f;
    f32 largeMaxRange = 99999.0f;
    s16 phi_s2 = curPlatform;
    s16 phi_s3 = arg2;

    // Upstairs
    if (pos->y > 200.0f) {
        curLoopPlatform = ARRAY_COUNT(sPlatformPositions) - 1;
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        minRange = 290.0f;
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if (minRange < Math_Vec3f_DistXYZ(pos, &sPlatformPositions[curLoopPlatform])) {
            continue;
        }
        if (curLoopPlatform != nextPlatform) {
            f32 curPlatformDistToPlayer =
                Math_Vec3f_DistXYZ(&player->actor.world.pos, &sPlatformPositions[curLoopPlatform]);

            if (curPlatformDistToPlayer < smallMaxRange) {
                largeMaxRange = smallMaxRange;
                phi_s3 = phi_s2;
                smallMaxRange = curPlatformDistToPlayer;
                phi_s2 = curLoopPlatform;
            } else if (curPlatformDistToPlayer < largeMaxRange) {
                largeMaxRange = curPlatformDistToPlayer;
                phi_s3 = curLoopPlatform;
            }
        } else {
            phi_s2 = nextPlatform;
            break;
        }
    }

    if (phi_s3 != nextPlatform) {
        nextPlatform = phi_s2;
    } else {
        nextPlatform = phi_s3;
    }

    return nextPlatform;
}

// Player not targeting thisv or another EnZf?
s32 EnZf_CanAttack(GlobalContext* globalCtx, EnZf* thisv) {
    Actor* targetedActor;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        if (player->stateFlags1 & 0x6000) {                    // Hanging or climbing
            return false;
        } else {
            return true;
        }
    } else {
        if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            return true;
        }
        if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
            targetedActor = player->unk_664;
            if (targetedActor == NULL) {
                return false;
            } else {
                if (targetedActor->category != ACTORCAT_ENEMY) {
                    return true;
                }
                if (targetedActor->id != ACTOR_EN_ZF) {
                    return false;
                } else if (targetedActor->colorFilterTimer != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void func_80B44DC4(EnZf* thisv, GlobalContext* globalCtx) {
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }

    if (angleDiff >= 0x1B58) {
        func_80B483E4(thisv, globalCtx);
    } else if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 8) != 0) &&
               EnZf_CanAttack(globalCtx, thisv)) {
        EnZf_SetupSlash(thisv);
    } else {
        func_80B45384(thisv);
    }
}

s32 EnZf_ChooseAction(GlobalContext* globalCtx, EnZf* thisv) {
    s16 angleToWall;
    Actor* explosive;

    angleToWall = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
    angleToWall = ABS(angleToWall);

    if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x5DC0, 0x2AA8, thisv->actor.shape.rot.y)) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

        if ((thisv->actor.bgCheckFlags & 8) && (ABS(angleToWall) < 0x2EE0) && (thisv->actor.xzDistToPlayer < 80.0f)) {
            EnZf_SetupJumpUp(thisv);
            return true;
        } else if ((thisv->actor.xzDistToPlayer < 90.0f) && ((globalCtx->gameplayFrames % 2) != 0)) {
            EnZf_SetupJumpUp(thisv);
            return true;
        } else {
            EnZf_SetupJumpBack(thisv);
            return true;
        }
    }

    explosive = Actor_FindNearby(globalCtx, &thisv->actor, -1, ACTORCAT_EXPLOSIVE, 80.0f);

    if (explosive != NULL) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        if (((thisv->actor.bgCheckFlags & 8) && (angleToWall < 0x2EE0)) || (explosive->id == ACTOR_EN_BOM_CHU)) {
            if ((explosive->id == ACTOR_EN_BOM_CHU) && (Actor_WorldDistXYZToActor(&thisv->actor, explosive) < 80.0f) &&
                ((s16)((thisv->actor.shape.rot.y - explosive->world.rot.y) + 0x8000) < 0x3E80)) {
                EnZf_SetupJumpUp(thisv);
                return true;
            } else {
                EnZf_SetupCircleAroundPlayer(thisv, 4.0f);
                return true;
            }
        } else {
            EnZf_SetupJumpBack(thisv);
            return true;
        }
    }
    return false;
}

// Setup functions and action functions

/**
 * Set position 300 units above ground and invisible, fade in and drop to ground, fully solid when on ground
 */
void EnZf_SetupDropIn(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfJumpingAnim, 0.0f, 9.0f, Animation_GetLastFrame(&gZfJumpingAnim),
                     ANIMMODE_LOOP, 0.0f);

    thisv->actor.world.pos.y = thisv->actor.floorHeight + 300.0f;
    thisv->alpha = thisv->actor.shape.shadowAlpha = 0;
    thisv->unk_3F0 = 10;
    thisv->hopAnimIndex = 1;
    thisv->action = ENZF_ACTION_DROP_IN;
    thisv->actor.bgCheckFlags &= ~2;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    EnZf_SetupAction(thisv, EnZf_DropIn);
}

void EnZf_DropIn(EnZf* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_3F0 == 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        thisv->actor.flags |= ACTOR_FLAG_0;

        if (thisv->actor.params == ENZF_TYPE_LIZALFOS_MINIBOSS_A) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
    }

    if (thisv->unk_3F0 != 0) {
        if (thisv->actor.params != ENZF_TYPE_LIZALFOS_LONE) {
            thisv->unk_3F0--;
        } else if (thisv->actor.xzDistToPlayer <= 160.0f) {
            thisv->unk_3F0 = 0;
            thisv->actor.flags |= ACTOR_FLAG_0;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        }

        thisv->actor.world.pos.y = thisv->actor.floorHeight + 300.0f;
    } else if (thisv->alpha < 255) {
        thisv->alpha += 255 / 5;
    }

    if ((thisv->actor.bgCheckFlags & 3) && (thisv->hopAnimIndex != 0)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_ONGND);
        Animation_Change(&thisv->skelAnime, &gZfLandingAnim, 1.0f, 0.0f, 17.0f, ANIMMODE_ONCE, 0.0f);
        thisv->hopAnimIndex = 0;
        thisv->actor.bgCheckFlags &= ~2;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->actor.velocity.y = 0.0f;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->alpha = 255;
        if (thisv->actor.params > ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Only miniboss B
            EnZf_SetupSheatheSword(thisv, globalCtx);
        } else {
            func_80B45384(thisv);
        }
    }
    thisv->actor.shape.shadowAlpha = thisv->alpha;
}

// stop? and choose an action
void func_80B45384(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfCryingAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gZfCryingAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    thisv->action = ENZF_ACTION_3;
    thisv->unk_3F0 = Rand_ZeroOne() * 10.0f + 5.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnZf_SetupAction(thisv, func_80B4543C);
}

void func_80B4543C(EnZf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s16 angleToPlayer = (thisv->actor.yawTowardsPlayer - thisv->headRot) - thisv->actor.shape.rot.y;

    angleToPlayer = ABS(angleToPlayer);
    SkelAnime_Update(&thisv->skelAnime);

    if (!EnZf_DodgeRangedEngaging(globalCtx, thisv)) {
        if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
            if (thisv->unk_3F4 != 0) {
                thisv->unk_3F4--;
                if (angleToPlayer >= 0x1FFE) {
                    return;
                }
                thisv->unk_3F4 = 0;

            } else if (EnZf_ChooseAction(globalCtx, thisv)) {
                return;
            }
        }
        angleToPlayer = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        angleToPlayer = ABS(angleToPlayer);

        if ((thisv->actor.xzDistToPlayer < 100.0f) && (player->swordState != 0) && (angleToPlayer >= 0x1F40)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            func_80B483E4(thisv, globalCtx);
        } else if (thisv->unk_3F0 != 0) {
            thisv->unk_3F0--;
        } else {
            if (Actor_IsFacingPlayer(&thisv->actor, 30 * 0x10000 / 360)) {
                if ((thisv->actor.xzDistToPlayer < 200.0f) && (thisv->actor.xzDistToPlayer > 100.0f) &&
                    (Rand_ZeroOne() < 0.3f)) {
                    if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
                        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
                        EnZf_SetupJumpForward(thisv);
                    } else {
                        func_80B483E4(thisv, globalCtx);
                    }
                } else if (Rand_ZeroOne() > 0.3f) {
                    EnZf_SetupApproachPlayer(thisv, globalCtx);
                } else {
                    func_80B483E4(thisv, globalCtx);
                }
            } else {
                func_80B4604C(thisv);
            }

            if ((globalCtx->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

void EnZf_SetupApproachPlayer(EnZf* thisv, GlobalContext* globalCtx) {
    Animation_MorphToLoop(&thisv->skelAnime, &gZfWalkingAnim, -4.0f);
    thisv->action = ENZF_ACTION_APPROACH_PLAYER;

    if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
        thisv->nextPlatform = EnZf_FindNextPlatformTowardsPlayer(&thisv->actor.world.pos, thisv->curPlatform,
                                                                thisv->homePlatform, globalCtx);
        thisv->hopAnimIndex = 0;
    }
    thisv->actor.speedXZ = 0.0f;
    EnZf_SetupAction(thisv, EnZf_ApproachPlayer);
}

void EnZf_ApproachPlayer(EnZf* thisv, GlobalContext* globalCtx) {
    s32 sp54;
    s32 sp50;
    s32 temp;
    s16 temp_v1;
    s16 sp48 = -1;
    f32 sp44 = 350.0f;
    f32 sp40 = 0.0f;
    Player* player = GET_PLAYER(globalCtx);
    s32 sp30;

    if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        sp48 = EnZf_FindPlatform(&player->actor.world.pos, sp48);
        thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, sp48);

        if (thisv->actor.world.pos.y >= 420.0f) {
            sp44 = 270.0f;
        }
    }

    if (!EnZf_DodgeRangedEngaging(globalCtx, thisv)) {
        if (sp48 != thisv->curPlatform) {
            thisv->nextPlatform = EnZf_FindNextPlatformTowardsPlayer(&thisv->actor.world.pos, thisv->curPlatform,
                                                                    thisv->homePlatform, globalCtx);

            if ((sp48 < 0) && (thisv->nextPlatform == thisv->curPlatform)) {
                sp48 = thisv->curPlatform;
                thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            } else {
                thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer =
                    Actor_WorldYawTowardPoint(&thisv->actor, &sPlatformPositions[thisv->nextPlatform]);

                temp_v1 = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
                temp_v1 = ABS(temp_v1);

                if ((thisv->unk_3F8 && (thisv->actor.speedXZ > 0.0f)) ||
                    ((thisv->actor.bgCheckFlags & 8) && (temp_v1 >= 0x5C19))) {
                    if ((Actor_WorldDistXZToPoint(&thisv->actor, &sPlatformPositions[thisv->nextPlatform]) < sp44) &&
                        !EnZf_PrimaryFloorCheck(thisv, globalCtx, 191.9956f)) {
                        EnZf_SetupJumpForward(thisv);

                        if (thisv->actor.bgCheckFlags & 8) {
                            thisv->actor.velocity.y = 20.0f;
                        }

                        return;
                    } else {
                        thisv->actor.world.rot.y =
                            Actor_WorldYawTowardPoint(&thisv->actor, &sPlatformPositions[thisv->curPlatform]);
                    }
                } else {
                    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                    Math_SmoothStepToF(&thisv->actor.speedXZ, 8.0f, 1.0f, 1.5f, 0.0f);
                }
            }
        }

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            sp40 = 100.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (70.0f + sp40)) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, -8.0f, 1.0f, 0.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 8.0f, 1.0f, 0.5f, 0.0f);
        }

        thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 1.2f;

        temp_v1 = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        temp_v1 = ABS(temp_v1);

        if ((sp48 == thisv->curPlatform) && (thisv->actor.xzDistToPlayer < 150.0f) && (player->swordState != 0) &&
            (temp_v1 >= 0x1F40)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                func_80B483E4(thisv, globalCtx);
                return;
            }
        }

        sp54 = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        sp50 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        sp30 = (f32)ABS(thisv->skelAnime.playSpeed);

        if (sp48 == thisv->curPlatform) {
            if (!Actor_IsFacingPlayer(&thisv->actor, 0x11C7)) {
                if (Rand_ZeroOne() > 0.5f) {
                    func_80B462E4(thisv, globalCtx);
                } else {
                    func_80B45384(thisv);
                }
            } else if (thisv->actor.xzDistToPlayer < 100.0f) {
                if ((Rand_ZeroOne() > 0.05f) && EnZf_CanAttack(globalCtx, thisv)) {
                    EnZf_SetupSlash(thisv);
                } else if (Rand_ZeroOne() > 0.5f) {
                    func_80B483E4(thisv, globalCtx);
                } else {
                    func_80B45384(thisv);
                }
            } else {
                if (thisv->unk_3F8) {
                    func_80B462E4(thisv, globalCtx);
                } else if (Rand_ZeroOne() < 0.1f) {
                    func_80B45384(thisv);
                }
            }
        }

        if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
            if (EnZf_ChooseAction(globalCtx, thisv)) {
                return;
            }

            if ((thisv->actor.xzDistToPlayer < 180.0f) && (thisv->actor.xzDistToPlayer > 160.0f) &&
                Actor_IsFacingPlayer(&thisv->actor, 0x71C)) {
                if (Actor_IsTargeted(globalCtx, &thisv->actor)) {
                    if (Rand_ZeroOne() < 0.1f) {
                        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
                        EnZf_SetupJumpForward(thisv);
                        return;
                    }
                } else {
                    func_80B483E4(thisv, globalCtx);
                    return;
                }
            }
        }

        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        }

        if (sp54 != (s32)thisv->skelAnime.curFrame) {
            temp = sp30 + sp54;

            if (((sp50 < 2) && (temp >= 4)) || ((sp50 < 32) && (temp >= 34))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_WALK);
            }
        }
    }
}

void EnZf_SetupJumpForward(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, -3.0f);
    thisv->unk_3F0 = 0;
    thisv->hopAnimIndex = 1;
    thisv->actor.velocity.y = 15.0f;

    if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        thisv->actor.speedXZ = 16.0f;
    } else {
        thisv->actor.speedXZ = 10.0f;
    }

    thisv->action = ENZF_ACTION_JUMP_FORWARD;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_JUMP);
    EnZf_SetupAction(thisv, EnZf_JumpForward);
}

void EnZf_JumpForward(EnZf* thisv, GlobalContext* globalCtx) {
    if ((thisv->unk_3F0 != 0) && (thisv->actor.world.pos.y <= thisv->actor.floorHeight)) {
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->hopAnimIndex = 0;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->unk_3F0 == 0) {
            Animation_Change(&thisv->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            thisv->unk_3F0 = 10;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_JUMP);
        } else {
            thisv->actor.speedXZ = 0.0f;
            thisv->hopAnimIndex = 0;
            EnZf_SetupApproachPlayer(thisv, globalCtx);
        }
    }
    if ((globalCtx->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
    }

    if ((thisv->actor.params == ENZF_TYPE_DINOLFOS) && (thisv->actor.bgCheckFlags & 3)) {
        if (EnZf_CanAttack(globalCtx, thisv)) {
            EnZf_SetupSlash(thisv);
        } else {
            func_80B483E4(thisv, globalCtx);
        }
    }
}

void func_80B4604C(EnZf* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gZfWalkingAnim, -4.0f);
    thisv->action = ENZF_ACTION_6;
    EnZf_SetupAction(thisv, func_80B46098);
}

void func_80B46098(EnZf* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 phi_f2;
    Player* player = GET_PLAYER(globalCtx);
    s16 temp_v0;
    s16 phi_v1;

    if (!EnZf_DodgeRangedEngaging(globalCtx, thisv)) {
        if ((thisv->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(globalCtx, thisv)) {
            temp_v0 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_v1 = temp_v0 * 0.25f + 2000.0f;
            } else {
                phi_v1 = temp_v0 * 0.25f - 2000.0f;
            }

            thisv->actor.shape.rot.y += phi_v1;
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 > 2.0f) {
                    phi_f2 = 2.0f;
                }
            } else {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 < -2.0f) {
                    phi_f2 = -2.0f;
                }
            }

            thisv->skelAnime.playSpeed = -phi_f2;
            SkelAnime_Update(&thisv->skelAnime);

            if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
                thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
                if (thisv->curPlatform != EnZf_FindPlatform(&player->actor.world.pos, -1)) {
                    EnZf_SetupApproachPlayer(thisv, globalCtx);
                    return;
                }
            }

            if (Actor_IsFacingPlayer(&thisv->actor, 30 * 0x10000 / 360)) {
                if (Rand_ZeroOne() > 0.8f) {
                    func_80B462E4(thisv, globalCtx);
                } else {
                    EnZf_SetupApproachPlayer(thisv, globalCtx);
                }
            }
            if ((globalCtx->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

// Conditional setup function
void func_80B462E4(EnZf* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ ||
        Actor_TestFloorInDirection(&thisv->actor, globalCtx, 40.0f, (s16)(thisv->actor.shape.rot.y + 0x3FFF)) ||
        Actor_TestFloorInDirection(&thisv->actor, globalCtx, -40.0f, (s16)(thisv->actor.shape.rot.y + 0x3FFF))) {
        Animation_PlayLoop(&thisv->skelAnime, &gZfSidesteppingAnim);
        thisv->actor.speedXZ = Rand_CenteredFloat(12.0f);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        thisv->unk_3F0 = Rand_ZeroOne() * 10.0f + 20.0f;
        thisv->hopAnimIndex = 0;
        thisv->action = ENZF_ACTION_7;
        thisv->unk_408 = 0.0f;
        EnZf_SetupAction(thisv, func_80B463E4);
    } else {
        EnZf_SetupApproachPlayer(thisv, globalCtx);
    }
}

void func_80B463E4(EnZf* thisv, GlobalContext* globalCtx) {
    s16 angleBehindPlayer;
    s16 phi_v0_3;
    s32 pad;
    s32 curKeyFrame;
    s32 prevKeyFrame;
    s32 playSpeed;
    Player* player = GET_PLAYER(globalCtx);
    f32 baseRange = 0.0f;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4000, 1);

    if (!EnZf_DodgeRangedEngaging(globalCtx, thisv) &&
        ((thisv->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(globalCtx, thisv))) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3A98;
        angleBehindPlayer = player->actor.shape.rot.y + 0x8000;

        if (Math_SinS(angleBehindPlayer - thisv->actor.shape.rot.y) >= 0.0f) {
            thisv->actor.speedXZ -= 0.25f;
            if (thisv->actor.speedXZ < -8.0f) {
                thisv->actor.speedXZ = -8.0f;
            }
        } else if (Math_SinS(angleBehindPlayer - thisv->actor.shape.rot.y) < 0.0f) { // Superfluous check
            thisv->actor.speedXZ += 0.25f;
            if (thisv->actor.speedXZ > 8.0f) {
                thisv->actor.speedXZ = 8.0f;
            }
        }

        if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
            if (thisv->unk_3F8) {
                thisv->actor.speedXZ = -thisv->actor.speedXZ;
            }
        } else if ((thisv->actor.bgCheckFlags & 8) ||
                   !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ,
                                               thisv->actor.shape.rot.y + 0x3FFF)) {
            if (thisv->actor.bgCheckFlags & 8) {
                if (thisv->actor.speedXZ >= 0.0f) {
                    phi_v0_3 = thisv->actor.shape.rot.y + 0x3FFF;
                } else {
                    phi_v0_3 = thisv->actor.shape.rot.y - 0x3FFF;
                }
                phi_v0_3 = thisv->actor.wallYaw - phi_v0_3;
            } else {
                thisv->actor.speedXZ *= -0.8f;
                phi_v0_3 = 0;
            }

            if (ABS(phi_v0_3) > 0x4000) {
                thisv->actor.speedXZ *= -0.8f;
                if (thisv->actor.speedXZ < 0.0f) {
                    thisv->actor.speedXZ -= 0.5f;
                } else {
                    thisv->actor.speedXZ += 0.5f;
                }
            }
        }

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            baseRange = 100.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (70.0f + baseRange)) {
            Math_SmoothStepToF(&thisv->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if ((90.0f + baseRange) < thisv->actor.xzDistToPlayer) {
            Math_SmoothStepToF(&thisv->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
        }

        if ((thisv->unk_408 != 0.0f) && !EnZf_SecondaryFloorCheck(thisv, globalCtx, thisv->unk_408)) {
            thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->unk_408;
            thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->unk_408;
        }

        if (ABS(thisv->actor.speedXZ) >= ABS(thisv->unk_408)) {
            thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.75f;
        } else if (thisv->skelAnime.playSpeed < 0.0f) {
            thisv->skelAnime.playSpeed = thisv->unk_408 * -0.75f;
        } else {
            thisv->skelAnime.playSpeed = thisv->unk_408 * 0.75f;
        }

        curKeyFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        prevKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        playSpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if (curKeyFrame != (s32)thisv->skelAnime.curFrame) {
            s32 nextKeyFrame = playSpeed + curKeyFrame;

            if (((prevKeyFrame < 14) && (nextKeyFrame > 15)) || ((prevKeyFrame < 27) && (nextKeyFrame > 28))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_WALK);
            }
        }

        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        }

        if ((Math_CosS(angleBehindPlayer - thisv->actor.shape.rot.y) < -0.85f) || (thisv->unk_3F0 == 0)) {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

            if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) == 0) &&
                EnZf_CanAttack(globalCtx, thisv)) {
                EnZf_SetupSlash(thisv);
            } else {
                func_80B45384(thisv);
            }
        } else if (thisv->unk_3F0 != 0) {
            thisv->unk_3F0--;
        }
    }
}

void EnZf_SetupSlash(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfSlashAnim, 1.25f, 0.0f, Animation_GetLastFrame(&gZfSlashAnim), ANIMMODE_ONCE,
                     -4.0f);

    if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
        thisv->skelAnime.playSpeed = 1.75f;
    }

    thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
    thisv->action = ENZF_ACTION_SLASH;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
    thisv->actor.speedXZ = 0.0f;
    EnZf_SetupAction(thisv, EnZf_Slash);
}

void EnZf_Slash(EnZf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 rotDiff;
    s16 yawDiff;

    thisv->actor.speedXZ = 0.0f;

    if ((s32)thisv->skelAnime.curFrame == 10) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_ATTACK);
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIndex));

        if ((thisv->actor.params == ENZF_TYPE_DINOLFOS) && !Actor_IsFacingPlayer(&thisv->actor, 5460)) {
            func_80B45384(thisv);
            thisv->unk_3F0 = Rand_ZeroOne() * 5.0f + 5.0f;
            thisv->unk_3F4 = Rand_ZeroOne() * 20.0f + 100.0f;
        } else if ((Rand_ZeroOne() > 0.7f) || (thisv->actor.xzDistToPlayer >= 120.0f)) {
            func_80B45384(thisv);
            thisv->unk_3F0 = Rand_ZeroOne() * 5.0f + 5.0f;
        } else {
            thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                func_80B483E4(thisv, globalCtx);
            } else {
                rotDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
                rotDiff = ABS(rotDiff);

                if (rotDiff <= 10000) {
                    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
                    yawDiff = ABS(yawDiff);

                    if (yawDiff > 16000) {
                        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                        func_80B483E4(thisv, globalCtx);
                    } else if (player->stateFlags1 & 0x6010) {
                        if (thisv->actor.isTargeted) {
                            EnZf_SetupSlash(thisv);
                        } else {
                            func_80B483E4(thisv, globalCtx);
                        }
                    } else {
                        EnZf_SetupSlash(thisv);
                    }
                } else {
                    func_80B483E4(thisv, globalCtx);
                }
            }
        }
    }
}

void EnZf_SetupRecoilFromBlockedSlash(EnZf* thisv) {
    f32 frame = thisv->skelAnime.curFrame - 3.0f;

    Animation_Change(&thisv->skelAnime, &gZfSlashAnim, -1.0f, frame, 0.0f, ANIMMODE_ONCE, 0.0f);
    thisv->action = ENZF_ACTION_RECOIL_FROM_BLOCKED_SLASH;
    EnZf_SetupAction(thisv, EnZf_RecoilFromBlockedSlash);
}

void EnZf_RecoilFromBlockedSlash(EnZf* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (Rand_ZeroOne() > 0.7f) {
            func_80B45384(thisv);
        } else if ((Rand_ZeroOne() > 0.2f) && EnZf_CanAttack(globalCtx, thisv)) {
            EnZf_SetupSlash(thisv);
        } else {
            func_80B483E4(thisv, globalCtx);
        }
    }
}

void EnZf_SetupJumpBack(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfJumpingAnim, -1.0f, 3.0f, 0.0f, ANIMMODE_ONCE, -3.0f);
    thisv->unk_3F0 = 0;
    thisv->hopAnimIndex = 1;
    thisv->action = ENZF_ACTION_JUMP_BACK;
    thisv->actor.velocity.y = 15.0f;
    thisv->actor.speedXZ = -15.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_JUMP);
    EnZf_SetupAction(thisv, EnZf_JumpBack);
}

void EnZf_JumpBack(EnZf* thisv, GlobalContext* globalCtx) {
    if ((thisv->unk_3F0 != 0) && (thisv->actor.world.pos.y <= thisv->actor.floorHeight)) {
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->hopAnimIndex = 0;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->unk_3F0 == 0) {
            Animation_Change(&thisv->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            thisv->unk_3F0 = 10;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_JUMP);
        } else if ((globalCtx->gameplayFrames % 2) != 0) {
            func_80B483E4(thisv, globalCtx);
        } else {
            func_80B45384(thisv);
        }
    }

    if ((globalCtx->state.frames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
    }
}

void EnZf_SetupStunned(EnZf* thisv) {
    if ((thisv->actor.bgCheckFlags & 1) && ((thisv->actor.velocity.y == 0.0f) || (thisv->actor.velocity.y == -4.0f))) {
        thisv->actor.speedXZ = 0.0f;
        thisv->hopAnimIndex = 0;
    } else {
        thisv->hopAnimIndex = 1;
    }

    if (thisv->damageEffect == ENZF_DMGEFF_ICE) {
        thisv->iceTimer = 36;
    } else {
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gZfKnockedBackAnim, 0.0f);
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->action = ENZF_ACTION_STUNNED;
    EnZf_SetupAction(thisv, EnZf_Stunned);
}

void EnZf_Stunned(EnZf* thisv, GlobalContext* globalCtx) {
    s16 angleToWall;

    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }
        thisv->hopAnimIndex = 0;
    }

    if ((thisv->actor.colorFilterTimer == 0) && (thisv->actor.bgCheckFlags & 1)) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnZf_SetupDie(thisv);
        } else if ((thisv->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(globalCtx, thisv)) {
            if (D_80B4A1B4 != -1) {
                func_80B44DC4(thisv, globalCtx);
            } else {
                angleToWall = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
                angleToWall = ABS(angleToWall);

                if ((thisv->actor.params == ENZF_TYPE_DINOLFOS) && (thisv->actor.bgCheckFlags & 8) &&
                    (ABS(angleToWall) < 0x2EE0) && (thisv->actor.xzDistToPlayer < 90.0f)) {
                    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
                    EnZf_SetupJumpUp(thisv);
                } else if (!EnZf_DodgeRangedEngaging(globalCtx, thisv)) {
                    if (thisv->actor.params != ENZF_TYPE_DINOLFOS) {
                        func_80B44DC4(thisv, globalCtx);
                    } else if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) != 0) &&
                               EnZf_CanAttack(globalCtx, thisv)) {
                        EnZf_SetupSlash(thisv);
                    } else {
                        func_80B44DC4(thisv, globalCtx);
                    }
                }
            }
        }
    }
}

void EnZf_SetupSheatheSword(EnZf* thisv, GlobalContext* globalCtx) {
    f32 morphFrames = 0.0f;
    f32 lastFrame = Animation_GetLastFrame(&gZfSheathingSwordAnim);

    if (thisv->action <= ENZF_ACTION_DAMAGED) {
        morphFrames = -4.0f;
    }

    Animation_Change(&thisv->skelAnime, &gZfSheathingSwordAnim, 2.0f, 0.0f, lastFrame, ANIMMODE_ONCE, morphFrames);
    thisv->action = ENZF_ACTION_SHEATHE_SWORD;
    thisv->actor.speedXZ = 0.0f;
    thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
    thisv->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&thisv->actor.world.pos, thisv->curPlatform, thisv->homePlatform, globalCtx);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnZf_SetupAction(thisv, EnZf_SheatheSword);
}

void EnZf_SheatheSword(EnZf* thisv, GlobalContext* globalCtx) {
    s16 yaw = Actor_WorldYawTowardPoint(&thisv->actor, &sPlatformPositions[thisv->nextPlatform]) + 0x8000;

    Math_SmoothStepToS(&thisv->actor.world.rot.y, yaw, 1, 1000, 0);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.world.rot.y = yaw - 0x8000;
        EnZf_SetupHopAway(thisv, globalCtx);
        thisv->swordSheathed = true;
    }
}

void EnZf_SetupHopAndTaunt(EnZf* thisv) {
    thisv->hopAnimIndex = 0;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHoppingAnims[0], -4.0f);
    thisv->action = ENZF_ACTION_HOP_AND_TAUNT;
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_40C = 0.0f;
    thisv->unk_408 = 0.0f;
    EnZf_SetupAction(thisv, EnZf_HopAndTaunt);
}

void EnZf_HopAndTaunt(EnZf* thisv, GlobalContext* globalCtx) {
    f32 lastFrame;
    f32 maxDist = 400.0f;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 1, 4000, 0);

    // Upstairs
    if (thisv->actor.world.pos.y >= 420.0f) {
        maxDist = 250.0f;
    }

    // If player gets too close, run away
    if ((thisv->actor.xzDistToPlayer < maxDist) && (thisv->hopAnimIndex != 1)) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        EnZf_SetupSheatheSword(thisv, globalCtx);
    } else {
        if (thisv->hopAnimIndex != 1) {
            EnZf_DodgeRangedWaiting(globalCtx, thisv);
        }

        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->hopAnimIndex++; // move on to next animation

            // Loop back to beginning
            if (thisv->hopAnimIndex >= ARRAY_COUNT(sHoppingAnims)) {
                thisv->hopAnimIndex = 0;
            }

            if ((thisv->unk_408 != 0.0f) || (thisv->unk_40C != 0.0f)) {
                thisv->hopAnimIndex = 1;
            }

            lastFrame = Animation_GetLastFrame(sHoppingAnims[thisv->hopAnimIndex]);

            switch (thisv->hopAnimIndex) {
                case 0:
                    thisv->actor.velocity.y = 0.0f;
                    thisv->actor.world.pos.y = thisv->actor.floorHeight;
                    break;

                case 1:
                    thisv->actor.velocity.y = thisv->unk_40C + 10.0f;
                    thisv->actor.speedXZ = thisv->unk_408;
                    thisv->unk_408 = 0.0f;
                    thisv->unk_40C = 0.0f;
                    break;

                case 2:
                    thisv->actor.world.pos.y = thisv->actor.floorHeight;
                    lastFrame = 3.0f;
                    break;

                default:
                    break;
            }

            Animation_Change(&thisv->skelAnime, sHoppingAnims[thisv->hopAnimIndex], 1.5f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             0.0f);
        }

        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void EnZf_SetupHopAway(EnZf* thisv, GlobalContext* globalCtx) {
    thisv->hopAnimIndex = 0;
    Animation_PlayOnce(&thisv->skelAnime, sHoppingAnims[0]);
    thisv->action = ENZF_ACTION_HOP_AWAY;
    thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
    thisv->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&thisv->actor.world.pos, thisv->curPlatform, thisv->homePlatform, globalCtx);
    EnZf_SetupAction(thisv, EnZf_HopAway);
}

void EnZf_HopAway(EnZf* thisv, GlobalContext* globalCtx) {
    f32 sp74;
    f32 sp70 = 1.0f;
    f32 phi_f20 = 550.0f;
    s32 pad;
    f32 phi_f20_2;
    f32 phi_f0;
    s32 pad2;
    s16 sp5A;
    s32 sp54;
    s32 temp_v1_2;
    s32 phi_v1;

    sp74 = Actor_WorldDistXZToPoint(&thisv->actor, &sPlatformPositions[thisv->nextPlatform]);
    sp54 = thisv->hopAnimIndex;

    if ((globalCtx->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
    }

    // Upstairs
    if (thisv->actor.world.pos.y >= 420.0f) {
        phi_f20 = 280.0f;
    }

    sp5A = Actor_WorldYawTowardPoint(&thisv->actor, &sPlatformPositions[thisv->nextPlatform]);

    switch (thisv->hopAnimIndex) {
        case 0:
            thisv->actor.world.rot.y = sp5A;
            thisv->actor.shape.rot.y = sp5A + 0x8000;
            D_80B4AB30 = 0;
            thisv->homePlatform = thisv->curPlatform;
            temp_v1_2 = !EnZf_PrimaryFloorCheck(thisv, globalCtx, 107.0f);
            temp_v1_2 |= !EnZf_PrimaryFloorCheck(thisv, globalCtx, 220.0f) << 1;
            thisv->hopAnimIndex++;

            switch (temp_v1_2) {
                case 1:
                case 1 | 2:
                    thisv->actor.velocity.y = 12.0f;
                    if (thisv->actor.bgCheckFlags & 8) {
                        thisv->actor.velocity.y += 8.0f;
                    }

                    thisv->actor.speedXZ = 8.0f;
                    break;

                case 2:
                    thisv->actor.velocity.y = 15.0f;
                    thisv->actor.speedXZ = 20.0f;
                    break;

                default: // 0
                    phi_f20_2 = 107.0f;
                    phi_f20_2 += 10.0f;
                    phi_f0 = 8.0f;
                    phi_f0 += 1.2f;

                    for (phi_v1 = 20; phi_v1 >= 0; phi_v1--, phi_f20_2 += 10.0f, phi_f0 += 1.2f) {

                        if (!EnZf_PrimaryFloorCheck(thisv, globalCtx, phi_f20_2)) {
                            thisv->actor.speedXZ = phi_f0;
                            thisv->actor.velocity.y = 12.0f;
                            break;
                        }
                    }
                    if (thisv->actor.speedXZ == 0.0f) {
                        EnZf_SetupHopAndTaunt(thisv);
                    }
            }
            break;

        case 1:
            if ((thisv->actor.bgCheckFlags & 2) || (thisv->actor.bgCheckFlags & 1)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_ONGND);
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.world.pos.y = thisv->actor.floorHeight;
                thisv->actor.speedXZ = 0.0f;
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 3.0f, 2, 2.0f, 0, 0, 0);

                if (phi_f20 <= thisv->actor.xzDistToPlayer) {
                    EnZf_SetupHopAndTaunt(thisv);
                } else if (sp74 < 80.0f) {
                    thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
                    thisv->nextPlatform = EnZf_FindNextPlatformAwayFromPlayer(&thisv->actor.world.pos, thisv->curPlatform,
                                                                             thisv->homePlatform, globalCtx);
                }

                thisv->hopAnimIndex = 0;
                sp70 = 2.0f;
            } else {
                Math_SmoothStepToS(&thisv->actor.world.rot.y, sp5A, 1, 0xFA0, 0);
                thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
                D_80B4AB30++;
            }
            break;

        case 2:
            if (thisv->skelAnime.curFrame == thisv->skelAnime.endFrame) {
                thisv->hopAnimIndex = 0;
            }
            break;
    }

    if (sp54 != thisv->hopAnimIndex) {
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, sHoppingAnims[thisv->hopAnimIndex], sp70);
    }

    SkelAnime_Update(&thisv->skelAnime);
}

void EnZf_SetupDrawSword(EnZf* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gZfDrawingSwordAnim);
    thisv->actor.world.rot.y += 0x8000;
    thisv->action = ENZF_ACTION_DRAW_SWORD;
    thisv->actor.speedXZ = 0.0f;
    thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);
    thisv->nextPlatform =
        EnZf_FindNextPlatformAwayFromPlayer(&thisv->actor.world.pos, thisv->curPlatform, thisv->homePlatform, globalCtx);
    EnZf_SetupAction(thisv, EnZf_DrawSword);
}

void EnZf_DrawSword(EnZf* thisv, GlobalContext* globalCtx) {
    s16 yawTowardsPlayer = thisv->actor.yawTowardsPlayer;

    if (thisv->skelAnime.curFrame >= 26.0f) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, yawTowardsPlayer, 1, 6000, 0);
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        thisv->actor.world.rot.y = yawTowardsPlayer;
        thisv->hopAnimIndex = -1;
        func_80B45384(thisv);
    }

    if (thisv->skelAnime.curFrame == 22.0f) {
        thisv->swordSheathed = false;
    }
}

void EnZf_SetupDamaged(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfKnockedBackAnim, 1.5f, 0.0f, Animation_GetLastFrame(&gZfKnockedBackAnim),
                     ANIMMODE_ONCE, -4.0f);

    if ((thisv->actor.bgCheckFlags & 1) && ((thisv->actor.velocity.y == 0.0f) || (thisv->actor.velocity.y == -4.0f))) {
        thisv->actor.speedXZ = -4.0f;
        thisv->hopAnimIndex = 0;
    } else {
        thisv->hopAnimIndex = 1;
    }

    if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
        thisv->skelAnime.playSpeed = 4.5f;
    }

    if (thisv->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // not miniboss
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DAMAGE);
    thisv->action = ENZF_ACTION_DAMAGED;
    EnZf_SetupAction(thisv, EnZf_Damaged);
}

void EnZf_Damaged(EnZf* thisv, GlobalContext* globalCtx) {
    s16 wallYawDiff;

    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }
        thisv->hopAnimIndex = 0;
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4500, 0);

    if (((thisv->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(globalCtx, thisv)) &&
        SkelAnime_Update(&thisv->skelAnime) && (thisv->actor.bgCheckFlags & 1)) {

        if (D_80B4A1B4 != -1) {
            if (thisv->damageEffect == ENZF_DMGEFF_PROJECTILE) {
                D_80B4A1B0++;
            } else {
                thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

                if (!EnZf_PrimaryFloorCheck(thisv, globalCtx, 135.0f) && (thisv->actor.xzDistToPlayer < 90.0f)) {
                    EnZf_SetupJumpUp(thisv);
                } else if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) == 0)) {
                    EnZf_SetupSlash(thisv);
                } else {
                    func_80B44DC4(thisv, globalCtx);
                }
            }
        } else {

            wallYawDiff = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
            wallYawDiff = ABS(wallYawDiff);

            if ((thisv->actor.params == ENZF_TYPE_DINOLFOS) && (thisv->actor.bgCheckFlags & 8) &&
                (ABS(wallYawDiff) < 12000) && (thisv->actor.xzDistToPlayer < 90.0f)) {
                EnZf_SetupJumpUp(thisv);
            } else if (!EnZf_DodgeRangedEngaging(globalCtx, thisv)) {
                if (thisv->actor.params != ENZF_TYPE_DINOLFOS) {
                    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

                    if (!EnZf_PrimaryFloorCheck(thisv, globalCtx, 135.0f) && (thisv->actor.xzDistToPlayer < 90.0f)) {
                        EnZf_SetupJumpUp(thisv);
                    } else if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) == 0)) {
                        EnZf_SetupSlash(thisv);
                    } else {
                        func_80B44DC4(thisv, globalCtx);
                    }
                } else if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) == 0) &&
                           EnZf_CanAttack(globalCtx, thisv)) {
                    EnZf_SetupSlash(thisv);
                } else {
                    func_80B44DC4(thisv, globalCtx);
                }
            }
        }
    }
}

void EnZf_SetupJumpUp(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, 0.0f);
    thisv->unk_3F0 = 0;
    thisv->hopAnimIndex = 1;
    thisv->action = ENZF_ACTION_JUMP_UP;
    thisv->actor.velocity.y = 22.0f;
    thisv->actor.speedXZ = 7.5f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_JUMP);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnZf_SetupAction(thisv, EnZf_JumpUp);
}

void EnZf_JumpUp(EnZf* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4000, 1);
    if (thisv->actor.velocity.y >= 5.0f) {
        func_800355B8(globalCtx, &thisv->leftFootPos);
        func_800355B8(globalCtx, &thisv->rightFootPos);
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->unk_3F0 == 0) {
            Animation_Change(&thisv->skelAnime, &gZfSlashAnim, 3.0f, 0.0f, 13.0f, ANIMMODE_ONCE, -4.0f);
            thisv->unk_3F0 = 10;
        } else if (thisv->actor.bgCheckFlags & 3) {
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.world.pos.y = thisv->actor.floorHeight;
            EnZf_SetupSlash(thisv);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_ATTACK);
            thisv->skelAnime.curFrame = 13.0f;
        }
    }
}

// Conditional setup function
void func_80B483E4(EnZf* thisv, GlobalContext* globalCtx) {
    s16 playerRotY;
    Player* player;

    if ((thisv->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
        Actor_TestFloorInDirection(&thisv->actor, globalCtx, 40.0f, (s16)(thisv->actor.shape.rot.y + 0x3FFF)) ||
        Actor_TestFloorInDirection(&thisv->actor, globalCtx, -40.0f, (s16)(thisv->actor.shape.rot.y + 0x3FFF))) {
        Animation_PlayLoop(&thisv->skelAnime, &gZfSidesteppingAnim);
        player = GET_PLAYER(globalCtx);
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4000, 1);
        playerRotY = player->actor.shape.rot.y;

        if (Math_SinS(playerRotY - thisv->actor.shape.rot.y) >= 0.0f) {
            thisv->actor.speedXZ = -6.0f;
        } else if (Math_SinS(playerRotY - thisv->actor.shape.rot.y) < 0.0f) { // Superfluous check
            thisv->actor.speedXZ = 6.0f;
        }

        thisv->unk_408 = 0.0f;
        thisv->hopAnimIndex = 0;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
        thisv->unk_3F0 = Rand_ZeroOne() * 10.0f + 5.0f;
        thisv->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
        EnZf_SetupAction(thisv, EnZf_CircleAroundPlayer);
    } else {
        EnZf_SetupApproachPlayer(thisv, globalCtx);
    }
}

void EnZf_CircleAroundPlayer(EnZf* thisv, GlobalContext* globalCtx) {
    s16 playerRot;
    s16 phi_v0_4;
    Player* player = GET_PLAYER(globalCtx);
    s32 curKeyFrame;
    s32 prevKeyFrame;
    s32 playSpeed;
    f32 baseRange = 0.0f;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xBB8, 1);
    playerRot = player->actor.shape.rot.y;

    if (thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        if (thisv->unk_3F8) {
            thisv->actor.speedXZ = -thisv->actor.speedXZ;
        }
    } else if ((thisv->actor.bgCheckFlags & 8) ||
               !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ,
                                           thisv->actor.shape.rot.y + 0x3FFF)) {
        if (thisv->actor.bgCheckFlags & 8) {
            if (thisv->actor.speedXZ >= 0.0f) {
                phi_v0_4 = thisv->actor.shape.rot.y + 0x3FFF;
            } else {
                phi_v0_4 = thisv->actor.shape.rot.y - 0x3FFF;
            }

            phi_v0_4 = thisv->actor.wallYaw - phi_v0_4;
        } else {
            thisv->actor.speedXZ *= -0.8f;
            phi_v0_4 = 0;
        }

        if (ABS(phi_v0_4) > 0x4000) {
            thisv->actor.speedXZ *= -0.8f;
            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ -= 0.5f;
            } else {
                thisv->actor.speedXZ += 0.5f;
            }
        }
    }

    if (Math_SinS(playerRot - thisv->actor.shape.rot.y) >= 0.0f) {
        thisv->actor.speedXZ += 0.125f;
    } else {
        thisv->actor.speedXZ -= 0.125f;
    }

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x4000;

    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
        baseRange = 100.0f;
    }

    if (thisv->actor.xzDistToPlayer <= (70.0f + baseRange)) {
        Math_SmoothStepToF(&thisv->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if ((90.0f + baseRange) < thisv->actor.xzDistToPlayer) {
        Math_SmoothStepToF(&thisv->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
    }

    if ((thisv->unk_408 != 0.0f) && !EnZf_SecondaryFloorCheck(thisv, globalCtx, thisv->unk_408)) {
        thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->unk_408;
        thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->unk_408;
    }

    if (ABS(thisv->actor.speedXZ) >= ABS(thisv->unk_408)) {
        thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.75f;
    } else if (thisv->skelAnime.playSpeed < 0.0f) {
        thisv->skelAnime.playSpeed = thisv->unk_408 * -0.75f;
    } else {
        thisv->skelAnime.playSpeed = thisv->unk_408 * 0.75f;
    }

    curKeyFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    prevKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
    playSpeed = (f32)ABS(thisv->skelAnime.playSpeed);

    thisv->curPlatform = EnZf_FindPlatform(&thisv->actor.world.pos, thisv->curPlatform);

    if (EnZf_FindPlatform(&player->actor.world.pos, -1) != thisv->curPlatform) {
        thisv->actor.speedXZ = 0.0f;

        if ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
            (D_80B4A1B4 == thisv->actor.params)) {
            EnZf_SetupHopAndTaunt(thisv);
        } else {
            EnZf_SetupApproachPlayer(thisv, globalCtx);
        }
    } else if ((thisv->actor.params != ENZF_TYPE_DINOLFOS) || !EnZf_ChooseAction(globalCtx, thisv)) {
        if (thisv->unk_3F0 == 0) {
            phi_v0_4 = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

            phi_v0_4 = ABS(phi_v0_4);

            if (phi_v0_4 >= 0x3A98) {
                if ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (D_80B4A1B4 == thisv->actor.params)) {
                    EnZf_SetupHopAndTaunt(thisv);
                } else {
                    func_80B45384(thisv);
                    thisv->unk_3F0 = Rand_ZeroOne() * 5.0f + 1.0f;
                }
            } else if ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (D_80B4A1B4 == thisv->actor.params)) {
                EnZf_SetupHopAndTaunt(thisv);
            } else {
                thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

                if ((thisv->actor.xzDistToPlayer <= 100.0f) && ((globalCtx->gameplayFrames % 4) == 0) &&
                    EnZf_CanAttack(globalCtx, thisv)) {
                    EnZf_SetupSlash(thisv);
                } else if ((thisv->actor.xzDistToPlayer < 280.0f) && (thisv->actor.xzDistToPlayer > 240.0f) &&
                           !EnZf_PrimaryFloorCheck(thisv, globalCtx, 191.9956f) &&
                           ((globalCtx->gameplayFrames % 2) == 0)) {
                    EnZf_SetupJumpForward(thisv);
                } else {
                    EnZf_SetupApproachPlayer(thisv, globalCtx);
                }
            }
        } else {
            thisv->unk_3F0--;
        }
        if (curKeyFrame != (s32)thisv->skelAnime.curFrame) {
            s32 nextKeyFrame = playSpeed + curKeyFrame;
            if (((prevKeyFrame < 14) && (nextKeyFrame >= 16)) || ((prevKeyFrame < 27) && (nextKeyFrame >= 29))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_WALK);
            }
        }
        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void EnZf_SetupDie(EnZf* thisv) {
    Animation_Change(&thisv->skelAnime, &gZfDyingAnim, 1.5f, 0.0f, Animation_GetLastFrame(&gZfDyingAnim), ANIMMODE_ONCE,
                     -4.0f);

    if ((thisv->actor.bgCheckFlags & 1) && ((thisv->actor.velocity.y == 0.0f) || (thisv->actor.velocity.y == -4.0f))) {
        thisv->actor.speedXZ = 0.0f;
        thisv->hopAnimIndex = 0;
    } else {
        thisv->hopAnimIndex = 1;
    }

    thisv->action = ENZF_ACTION_DIE;
    thisv->actor.flags &= ~ACTOR_FLAG_0;

    if (D_80B4A1B4 != -1) {
        if (thisv->actor.prev != NULL) {
            ((EnZf*)thisv->actor.prev)->unk_3F4 = 90;

            if (thisv->actor.prev->colChkInfo.health < 3) {
                thisv->actor.prev->colChkInfo.health = 3;
            }
        } else {
            ((EnZf*)thisv->actor.next)->unk_3F4 = 90;

            if (thisv->actor.next->colChkInfo.health < 3) {
                thisv->actor.next->colChkInfo.health = 3;
            }
        }
    }

    D_80B4A1B0 = 0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DEAD);
    EnZf_SetupAction(thisv, EnZf_Die);
}

void EnZf_Die(EnZf* thisv, GlobalContext* globalCtx) {

    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.15f, 0.0f);
        thisv->hopAnimIndex = 0;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.category != ACTORCAT_PROP) {
            if ((thisv->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ && (D_80B4A1B4 == -1)) {
                Flags_SetSwitch(globalCtx, thisv->clearFlag);
                func_800F5B58();
            } else {
                D_80B4A1B4 = -1;
            }
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
        }

        if (thisv->alpha != 0) {
            thisv->actor.shape.shadowAlpha = thisv->alpha -= 5;

        } else {
            Actor_Kill(&thisv->actor);
        }
    } else {
        s32 curFrame = thisv->skelAnime.curFrame;

        if ((curFrame == 10) || (curFrame == 18)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
}

void EnZf_UpdateHeadRotation(EnZf* thisv, GlobalContext* globalCtx) {
    s16 angleTemp;

    if ((thisv->actor.params == ENZF_TYPE_DINOLFOS) && (thisv->action == ENZF_ACTION_3) && (thisv->unk_3F4 != 0)) {
        thisv->headRot = Math_SinS(thisv->unk_3F4 * 1400) * 0x2AA8;
    } else {
        angleTemp = thisv->actor.yawTowardsPlayer;
        angleTemp -= (s16)(thisv->headRot + thisv->actor.shape.rot.y);
        thisv->headRotTemp = CLAMP(angleTemp, -0x7D0, 0x7D0);
        thisv->headRot += thisv->headRotTemp;
        thisv->headRot = CLAMP(thisv->headRot, -0x1CD7, 0x1CD7);
    }
}

void EnZf_UpdateDamage(EnZf* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 dropParams;

    if ((thisv->bodyCollider.base.acFlags & AC_HIT) && (thisv->action <= ENZF_ACTION_STUNNED)) {
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;

        if (((thisv->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
             (D_80B4A1B4 != thisv->actor.params)) &&
            (thisv->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE)) {
            thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
            Actor_SetDropFlag(&thisv->actor, &thisv->bodyCollider.info, 0);

            if ((thisv->actor.colChkInfo.damageEffect == ENZF_DMGEFF_STUN) ||
                (thisv->actor.colChkInfo.damageEffect == ENZF_DMGEFF_ICE)) {
                if (thisv->action != ENZF_ACTION_STUNNED) {
                    Actor_SetColorFilter(&thisv->actor, 0, 120, 0, 80);
                    Actor_ApplyDamage(&thisv->actor);
                    EnZf_SetupStunned(thisv);
                }
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_CRY);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 8);

                if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    dropParams = 0x40;
                    EnZf_SetupDie(thisv);

                    if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
                        dropParams = 0xE0;
                    }

                    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, dropParams);
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                } else {
                    if ((D_80B4A1B4 != -1) && ((thisv->actor.colChkInfo.health + thisv->actor.colChkInfo.damage) >= 4) &&
                        (thisv->actor.colChkInfo.health < 4)) {
                        thisv->damageEffect = ENZF_DMGEFF_PROJECTILE;
                    }

                    EnZf_SetupDamaged(thisv);
                }
            }
        }
    }
}

void EnZf_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZf* thisv = (EnZf*)thisx;
    s32 pad2;

    EnZf_UpdateDamage(thisv, globalCtx);
    if (thisv->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE) {
        thisv->unk_3F8 = false;
        if ((thisv->hopAnimIndex != 1) && (thisv->action != ENZF_ACTION_HOP_AWAY)) {
            if (thisv->actor.speedXZ != 0.0f) {
                thisv->unk_3F8 = EnZf_PrimaryFloorCheck(thisv, globalCtx, thisv->actor.speedXZ * 1.5f);
            }
            if (!thisv->unk_3F8) {
                thisv->unk_3F8 = EnZf_PrimaryFloorCheck(thisv, globalCtx, 0.0f);
            }
        }

        if (!thisv->unk_3F8) {
            Actor_MoveForward(&thisv->actor);
        }

        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 25.0f, 30.0f, 60.0f, 0x1D);

        if (!(thisv->actor.bgCheckFlags & 1)) {
            thisv->hopAnimIndex = 1;
        }

        thisv->actionFunc(thisv, globalCtx);
    }

    if (thisv->actor.colChkInfo.health > 0) {
        if ((thisv->action != ENZF_ACTION_SLASH) && (thisv->action != ENZF_ACTION_STUNNED)) {
            EnZf_UpdateHeadRotation(thisv, globalCtx);
        }

        if ((D_80B4A1B0 != 0) && (D_80B4A1B4 != thisv->actor.params)) {
            EnZf_SetupSheatheSword(thisv, globalCtx);
            D_80B4A1B4 = thisv->actor.params;
            D_80B4A1B0 = 0;

            if (thisv->actor.prev != NULL) {
                ((EnZf*)thisv->actor.prev)->unk_3F4 = 90;
            } else {
                ((EnZf*)thisv->actor.next)->unk_3F4 = 90;
            }
        }
    }

    if (thisv->action >= ENZF_ACTION_DIE) {
        Math_SmoothStepToS(&thisv->headRot, 0, 1, 2000, 0);

        if (thisv->action <= ENZF_ACTION_HOP_AND_TAUNT) {
            if ((thisv->unk_3F4 == 1) && (thisv->actor.bgCheckFlags & 1)) {
                if (thisv->actor.colChkInfo.health > 0) {
                    EnZf_SetupDrawSword(thisv, globalCtx);
                }
                thisv->unk_3F4--;
            }
        }

        if (thisv->unk_3F4 >= 2) {
            thisv->unk_3F4--;
        }
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 40.0f;

    if ((thisv->actor.colChkInfo.health > 0) && (thisv->alpha == 255)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);

        if ((thisv->actor.world.pos.y == thisv->actor.floorHeight) && (thisv->action <= ENZF_ACTION_DAMAGED)) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
        }

        if ((thisv->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
            (D_80B4A1B4 != thisv->actor.params)) {
            if ((thisv->actor.colorFilterTimer == 0) || !(thisv->actor.colorFilterParams & 0x4000)) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
            }
        }
    }

    if ((thisv->action == ENZF_ACTION_SLASH) && (thisv->skelAnime.curFrame >= 14.0f) &&
        (thisv->skelAnime.curFrame <= 20.0f)) {
        if (!(thisv->swordCollider.base.atFlags & AT_BOUNCED) && !(thisv->swordCollider.base.acFlags & AC_HIT)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->swordCollider.base);
        } else {
            thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
            thisv->swordCollider.base.acFlags &= ~AC_HIT;
            EnZf_SetupRecoilFromBlockedSlash(thisv);
        }
    }
}

s32 EnZf_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnZf* thisv = (EnZf*)thisx;

    switch (limbIndex) {
        case ENZF_LIMB_HEAD_ROOT:
            rot->y -= thisv->headRot;
            break;
        case ENZF_LIMB_SWORD:
            if (thisv->swordSheathed) {
                *dList = gZfEmptyHandDL;
            }
            break;
        case ENZF_LIMB_SCABBARD:
            if (thisv->swordSheathed) {
                *dList = gZfSheathedSwordDL;
            }
            break;
        default:
            break;
    }

    return false;
}

void EnZf_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f sUnused = { 1100.0f, -700.0f, 0.0f };
    static Vec3f footOffset = { 300.0f, 0.0f, 0.0f };
    static Vec3f D_80B4A2A4 = { 300.0f, -1700.0f, 0.0f }; // Sword tip?
    static Vec3f D_80B4A2B0 = { -600.0f, 300.0f, 0.0f };  // Sword hilt?
    static Vec3f swordQuadOffset1 = { 0.0f, 1500.0f, 0.0f };
    static Vec3f swordQuadOffset0 = { -600.0f, -3000.0f, 1000.0f };
    static Vec3f swordQuadOffset3 = { -600.0f, -3000.0f, -1000.0f };
    static Vec3f swordQuadOffset2 = { 1500.0f, -3000.0f, 0.0f };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f sp54;
    Vec3f sp48;
    EnZf* thisv = (EnZf*)thisx;
    s32 bodyPart = -1;

    if (limbIndex == ENZF_LIMB_SWORD) {
        Matrix_MultVec3f(&swordQuadOffset1, &thisv->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&swordQuadOffset0, &thisv->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&swordQuadOffset3, &thisv->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&swordQuadOffset2, &thisv->swordCollider.dim.quad[2]);
        Collider_SetQuadVertices(&thisv->swordCollider, &thisv->swordCollider.dim.quad[0],
                                 &thisv->swordCollider.dim.quad[1], &thisv->swordCollider.dim.quad[2],
                                 &thisv->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_80B4A2A4, &sp54);
        Matrix_MultVec3f(&D_80B4A2B0, &sp48);

        if (thisv->action == ENZF_ACTION_SLASH) {
            if (thisv->skelAnime.curFrame < 14.0f) {
                EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIndex));
            } else if (thisv->skelAnime.curFrame < 20.0f) {
                EffectBlure_AddVertex(Effect_GetByIndex(thisv->blureIndex), &sp54, &sp48);
            }
        }
    } else {
        Actor_SetFeetPos(&thisv->actor, limbIndex, ENZF_LIMB_LEFT_FOOT, &footOffset, ENZF_LIMB_RIGHT_FOOT, &footOffset);
    }

    switch (limbIndex) {
        case ENZF_LIMB_LEFT_FOOT:
            Matrix_MultVec3f(&footOffset, &thisv->leftFootPos);
            break;
        case ENZF_LIMB_RIGHT_FOOT:
            Matrix_MultVec3f(&footOffset, &thisv->rightFootPos);
            break;
    }

    if (thisv->iceTimer != 0) {
        switch (limbIndex) {
            case ENZF_LIMB_HEAD:
                bodyPart = 0;
                break;
            case ENZF_LIMB_NECK:
                bodyPart = 1;
                break;
            case ENZF_LIMB_CHEST_ARMOR:
                bodyPart = 2;
                break;
            case ENZF_LIMB_RIGHT_FOREARM:
                bodyPart = 3;
                break;
            case ENZF_LIMB_LEFT_FOREARM:
                bodyPart = 4;
                break;
            case ENZF_LIMB_TRUNK:
                bodyPart = 5;
                break;
            case ENZF_LIMB_SWORD_ROOT:
                bodyPart = 6;
                break;
            case ENZF_LIMB_RIGHT_SHIN:
                bodyPart = 7;
                break;
            case ENZF_LIMB_LEFT_SHIN_ROOT:
                bodyPart = 8;
                break;
            default:
                break;
        }
        if (bodyPart >= 0) {
            Matrix_MultVec3f(&zeroVec, &thisv->bodyPartsPos[bodyPart]);
        }
    }
}

static Gfx D_80B4A2F8[] = {
    gsSPTexture(0x0A00, 0x0A00, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

void EnZf_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZf* thisv = (EnZf*)thisx;
    ; // Extra ";" required for matching. Cannot be if (1) {} or the like. Typo?

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zf.c", 3533);

    func_8002EBCC(thisx, globalCtx, 1);

    gSPTexture(D_80B4A2F8, IREG(0), IREG(1), 0, G_TX_RENDERTILE, G_ON);

    gSPSegment(POLY_OPA_DISP++, 0x08, D_80B4A2F8);

    if (thisv->alpha == 255) {
        func_80093D18(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, thisv->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x09, &D_80116280[2]);

        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnZf_OverrideLimbDraw, EnZf_PostLimbDraw, thisv, POLY_OPA_DISP);

        if (thisv->iceTimer != 0) {
            thisx->colorFilterTimer++;
            thisv->iceTimer--;

            if ((thisv->iceTimer % 4) == 0) {
                s32 icePosIndex = thisv->iceTimer >> 2;

                EffectSsEnIce_SpawnFlyingVec3f(globalCtx, thisx, &thisv->bodyPartsPos[icePosIndex], 150, 150, 150, 250,
                                               235, 245, 255, 1.4f);
            }
            if (1) {}
        }
    } else { // fades out when dead
        func_80093D84(globalCtx->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x09, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnZf_OverrideLimbDraw, EnZf_PostLimbDraw, thisv, POLY_XLU_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zf.c", 3601);
}

void EnZf_SetupCircleAroundPlayer(EnZf* thisv, f32 speed) {
    Animation_MorphToLoop(&thisv->skelAnime, &gZfSidesteppingAnim, -1.0f);
    thisv->unk_3F0 = Rand_ZeroOne() * 10.0f + 8.0f;

    if (thisv->actor.params == ENZF_TYPE_DINOLFOS) {
        thisv->actor.speedXZ = 2.0f * speed;
        thisv->unk_3F0 /= 2;
    } else {
        thisv->actor.speedXZ = speed;
    }

    thisv->hopAnimIndex = 0;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
    thisv->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
    EnZf_SetupAction(thisv, EnZf_CircleAroundPlayer);
}

s32 EnZf_DodgeRangedEngaging(GlobalContext* globalCtx, EnZf* thisv) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 phi_v1;

    projectileActor = Actor_GetProjectileActor(globalCtx, &thisv->actor, 600.0f);

    if (projectileActor != NULL) {
        yawToProjectile =
            Actor_WorldYawTowardActor(&thisv->actor, projectileActor) - (s16)(u16)(thisv->actor.shape.rot.y);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;

        phi_t0 = 0;

        if (EnZf_PrimaryFloorCheck(thisv, globalCtx, -8.0f)) {
            phi_t0 = 1;
        }

        if (EnZf_PrimaryFloorCheck(thisv, globalCtx, 8.0f)) {
            phi_t0 |= 2;
        }

        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

        if ((((thisv->actor.xzDistToPlayer < 90.0f) || (phi_t0 == 3)) &&
             !EnZf_PrimaryFloorCheck(thisv, globalCtx, 135.0f)) ||
            (projectileActor->id == ACTOR_ARMS_HOOK)) {
            EnZf_SetupJumpUp(thisv);
            return true;
        }

        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;

        if (phi_t0 == 0) {
            phi_v1 = globalCtx->gameplayFrames % 2;
        } else {
            phi_v1 = phi_t0;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_v1 & 1) {
                EnZf_SetupCircleAroundPlayer(thisv, 8.0f);
                return true;
            }
            EnZf_SetupCircleAroundPlayer(thisv, -8.0f);
            return true;
        }
        if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_v1 & 1) {
                EnZf_SetupCircleAroundPlayer(thisv, 4.0f);
                return true;
            }
            EnZf_SetupCircleAroundPlayer(thisv, -4.0f);
        }
        return true;
    }
    return false;
}

s32 EnZf_DodgeRangedWaiting(GlobalContext* globalCtx, EnZf* thisv) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 sp1E;
    s16 sp1C = 0;

    projectileActor = Actor_GetProjectileActor(globalCtx, &thisv->actor, 600.0f);
    if (projectileActor != NULL) {
        yawToProjectile = Actor_WorldYawTowardActor(&thisv->actor, projectileActor) - (s16)(u16)thisv->actor.shape.rot.y;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF; // Set to move sideways

        phi_t0 = 0;

        if (EnZf_PrimaryFloorCheck(thisv, globalCtx, -70.0f)) {
            phi_t0 = 1;
        }

        if (EnZf_PrimaryFloorCheck(thisv, globalCtx, 70.0f)) {
            phi_t0 |= 2;
        }

        thisv->actor.speedXZ = 0.0f;

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_t0 == 0) {
                if ((globalCtx->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 5;
                        sp1E = 0;
                        break;
                }
            }
        } else if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_t0 == 0) {
                if ((globalCtx->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 10;
                        sp1E = 0;
                        break;
                }
            }
        }

        thisv->unk_408 = sp1E;
        thisv->unk_40C = sp1C;
        return true;
    }
    return false;
}

void EnZf_Reset(void) {
    D_80B4A1B0 = 0;
    D_80B4A1B4 = 1;
}