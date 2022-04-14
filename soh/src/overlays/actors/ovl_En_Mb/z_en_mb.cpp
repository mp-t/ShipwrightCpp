/*
 * File: z_en_mb.c
 * Overlay: ovl_En_Mb
 * Description: Moblins
 */

#include "z_en_mb.h"
#include "objects/object_mb/object_mb.h"

/*
 * This actor can have three behaviors:
 * - "Spear Guard" (variable -1): uses a spear, walks around home point, charges player if too close
 * - "Club" (variable 0): uses a club, stands still and smashes its club on the ground when the player approaches
 * - "Spear Patrol" (variable 0xPP00 PP=pathId): uses a spear, patrols following a path, charges
 */

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

typedef enum {
    /* -1 */ ENMB_TYPE_SPEAR_GUARD = -1,
    /*  0 */ ENMB_TYPE_CLUB,
    /*  1 */ ENMB_TYPE_SPEAR_PATROL
} EnMbType;

#define ENMB_ATTACK_NONE 0
#define ENMB_ATTACK_SPEAR 1
#define ENMB_ATTACK_CLUB_RIGHT 1
#define ENMB_ATTACK_CLUB_MIDDLE 2
#define ENMB_ATTACK_CLUB_LEFT 3

/* Spear and Club moblins use a different skeleton but the limbs are organized the same */
typedef enum {
    /*  1 */ ENMB_LIMB_ROOT = 1,
    /*  3 */ ENMB_LIMB_WAIST = 3,
    /*  6 */ ENMB_LIMB_CHEST = 6,
    /*  7 */ ENMB_LIMB_HEAD,
    /*  9 */ ENMB_LIMB_LSHOULDER = 9,
    /* 11 */ ENMB_LIMB_LFOREARM = 11,
    /* 12 */ ENMB_LIMB_LHAND,
    /* 14 */ ENMB_LIMB_RSHOULDER = 14,
    /* 16 */ ENMB_LIMB_RFOREARM = 16,
    /* 17 */ ENMB_LIMB_RHAND,
    /* 20 */ ENMB_LIMB_LTHIGH = 20,
    /* 21 */ ENMB_LIMB_LSHIN,
    /* 22 */ ENMB_LIMB_LFOOT,
    /* 25 */ ENMB_LIMB_RTHIGH = 25,
    /* 26 */ ENMB_LIMB_RSHIN,
    /* 27 */ ENMB_LIMB_RFOOT
} EnMbLimb;

void EnMb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMb_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMb_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Mb_InitVars = {
    ACTOR_EN_MB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_MB,
    sizeof(EnMb),
    (ActorFunc)EnMb_Init,
    (ActorFunc)EnMb_Destroy,
    (ActorFunc)EnMb_Update,
    (ActorFunc)EnMb_Draw,
    NULL,
};

void EnMb_SetupSpearPatrolTurnTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SetupClubWaitPlayerNear(EnMb* thisv);
void EnMb_SpearGuardLookAround(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SetupSpearGuardLookAround(EnMb* thisv);
void EnMb_SetupSpearDamaged(EnMb* thisv);
void EnMb_SpearGuardWalk(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearGuardPrepareAndCharge(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearPatrolPrepareAndCharge(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearEndChargeQuick(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_Stunned(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubDead(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubDamagedWhileKneeling(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubWaitPlayerNear(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubAttack(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearDead(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearDamaged(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SetupSpearDead(EnMb* thisv);
void EnMb_SpearPatrolTurnTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearPatrolWalkTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearPatrolEndCharge(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_SpearPatrolImmediateCharge(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubWaitAfterAttack(EnMb* thisv, GlobalContext* globalCtx);
void EnMb_ClubDamaged(EnMb* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sHitboxInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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

static ColliderTrisElementInit sFrontShieldingTrisInit[2] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sFrontShieldingInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sFrontShieldingTrisInit,
};

static ColliderQuadInit sAttackColliderInit = {
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
    /* 0x0 */ ENMB_DMGEFF_IGNORE,
    /* 0x1 */ ENMB_DMGEFF_STUN,
    /* 0x5 */ ENMB_DMGEFF_FREEZE = 0x5,
    /* 0x6 */ ENMB_DMGEFF_STUN_ICE,
    /* 0xF */ ENMB_DMGEFF_DEFAULT = 0xF
} EnMbDamageEffect;

static DamageTable sSpearMoblinDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Deku stick    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Slingshot     */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Explosive     */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Boomerang     */ DMG_ENTRY(0, ENMB_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hammer swing  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hookshot      */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri sword  */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Master sword  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant's Knife */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Fire arrow    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Ice arrow     */ DMG_ENTRY(4, ENMB_DMGEFF_STUN_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 1   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Fire magic    */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Ice magic     */ DMG_ENTRY(3, ENMB_DMGEFF_STUN_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Shield        */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Giant spin    */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Master spin   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri jump   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant jump    */ DMG_ENTRY(8, ENMB_DMGEFF_DEFAULT),
    /* Master jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 1     */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Unblockable   */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Hammer jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 2     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
};

static DamageTable sClubMoblinDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Deku stick    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Slingshot     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Explosive     */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Boomerang     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Normal arrow  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hammer swing  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Hookshot      */ DMG_ENTRY(0, ENMB_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Master sword  */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant's Knife */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Fire arrow    */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Ice arrow     */ DMG_ENTRY(4, ENMB_DMGEFF_STUN_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 1   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Fire magic    */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Ice magic     */ DMG_ENTRY(3, ENMB_DMGEFF_STUN_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Shield        */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENMB_DMGEFF_DEFAULT),
    /* Giant spin    */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Master spin   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Kokiri jump   */ DMG_ENTRY(2, ENMB_DMGEFF_DEFAULT),
    /* Giant jump    */ DMG_ENTRY(8, ENMB_DMGEFF_DEFAULT),
    /* Master jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 1     */ DMG_ENTRY(0, ENMB_DMGEFF_FREEZE),
    /* Unblockable   */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
    /* Hammer jump   */ DMG_ENTRY(4, ENMB_DMGEFF_DEFAULT),
    /* Unknown 2     */ DMG_ENTRY(0, ENMB_DMGEFF_IGNORE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4A, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5300, ICHAIN_STOP),
};

void EnMb_SetupAction(EnMb* thisv, EnMbActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnMb_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMb* thisv = (EnMb*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s16 relYawFromPlayer;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 46.0f);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.colChkInfo.damageTable = &sSpearMoblinDamageTable;
    Collider_InitCylinder(globalCtx, &thisv->hitbox);
    Collider_SetCylinder(globalCtx, &thisv->hitbox, &thisv->actor, &sHitboxInit);
    Collider_InitTris(globalCtx, &thisv->frontShielding);
    Collider_SetTris(globalCtx, &thisv->frontShielding, &thisv->actor, &sFrontShieldingInit, thisv->frontShieldingTris);
    Collider_InitQuad(globalCtx, &thisv->attackCollider);
    Collider_SetQuad(globalCtx, &thisv->attackCollider, &thisv->actor, &sAttackColliderInit);

    switch (thisv->actor.params) {
        case ENMB_TYPE_SPEAR_GUARD:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gEnMbSpearSkel, &gEnMbSpearStandStillAnim,
                               thisv->jointTable, thisv->morphTable, 28);
            thisv->actor.colChkInfo.health = 2;
            thisv->actor.colChkInfo.mass = MASS_HEAVY;
            thisv->maxHomeDist = 1000.0f;
            thisv->playerDetectionRange = 1750.0f;
            EnMb_SetupSpearGuardLookAround(thisv);
            break;
        case ENMB_TYPE_CLUB:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gEnMbClubSkel, &gEnMbClubStandStillClubDownAnim,
                               thisv->jointTable, thisv->morphTable, 28);

            thisv->actor.colChkInfo.health = 6;
            thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
            thisv->actor.colChkInfo.damageTable = &sClubMoblinDamageTable;
            Actor_SetScale(&thisv->actor, 0.02f);
            thisv->hitbox.dim.height = 170;
            thisv->hitbox.dim.radius = 45;
            thisv->actor.uncullZoneForward = 4000.0f;
            thisv->actor.uncullZoneScale = 800.0f;
            thisv->actor.uncullZoneDownward = 1800.0f;
            thisv->playerDetectionRange = 710.0f;
            thisv->attackCollider.info.toucher.dmgFlags = 0x20000000;

            relYawFromPlayer =
                thisv->actor.world.rot.y - Math_Vec3f_Yaw(&thisv->actor.world.pos, &player->actor.world.pos);
            if (ABS(relYawFromPlayer) > 0x4000) {
                thisv->actor.world.rot.y = thisx->world.rot.y + 0x8000;
                thisv->actor.shape.rot.y = thisx->world.rot.y;
                thisv->actor.world.pos.z = thisx->world.pos.z + 600.0f;
            }

            ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->actor.naviEnemyId += 1;
            EnMb_SetupClubWaitPlayerNear(thisv);
            break;
        default: /* Spear Patrol */
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gEnMbSpearSkel, &gEnMbSpearStandStillAnim,
                               thisv->jointTable, thisv->morphTable, 28);

            Actor_SetScale(&thisv->actor, 0.014f);
            thisv->path = (thisx->params & 0xFF00) >> 8;
            thisv->actor.params = ENMB_TYPE_SPEAR_PATROL;
            thisv->waypoint = 0;
            thisv->actor.colChkInfo.health = 1;
            thisv->actor.colChkInfo.mass = MASS_HEAVY;
            thisv->maxHomeDist = 350.0f;
            thisv->playerDetectionRange = 1750.0f;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            EnMb_SetupSpearPatrolTurnTowardsWaypoint(thisv, globalCtx);
            break;
    }
}

void EnMb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMb* thisv = (EnMb*)thisx;

    Collider_DestroyTris(globalCtx, &thisv->frontShielding);
    Collider_DestroyCylinder(globalCtx, &thisv->hitbox);
    Collider_DestroyQuad(globalCtx, &thisv->attackCollider);
}

void EnMb_FaceWaypoint(EnMb* thisv, GlobalContext* globalCtx) {
    s16 yawToWaypoint = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->waypointPos);

    thisv->actor.shape.rot.y = yawToWaypoint;
    thisv->actor.world.rot.y = yawToWaypoint;
}

void EnMb_NextWaypoint(EnMb* thisv, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* waypointPos;

    path = &globalCtx->setupPathList[thisv->path];

    if (thisv->waypoint == 0) {
        thisv->direction = 1;
    } else if (thisv->waypoint == (s8)(path->count - 1)) {
        thisv->direction = -1;
    }

    thisv->waypoint += thisv->direction;
    waypointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->waypoint;
    thisv->waypointPos.x = waypointPos->x;
    thisv->waypointPos.y = waypointPos->y;
    thisv->waypointPos.z = waypointPos->z;
}

/**
 * Checks if the player is in a 800*74 units XZ area centered on thisv actor,
 * the area being directed along its line of sight snapped to a cardinal angle.
 * Note: the longest corridor in Sacred Forest Meadows is 800 units long,
 *       and they all are 100 units wide.
 */
s32 EnMb_IsPlayerInCorridor(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 xFromPlayer;
    f32 zFromPlayer;
    f32 cos;
    f32 sin;
    f32 xFromPlayerAbs;
    f32 zFromPlayerAbs;
    s16 alignedYaw = 0;

    if ((thisv->actor.world.rot.y < -0x62A2) || (thisv->actor.world.rot.y > 0x62A2)) {
        alignedYaw = -0x8000;
    } else if (thisv->actor.world.rot.y < -0x20E0) {
        alignedYaw = -0x4000;
    } else if (thisv->actor.world.rot.y > 0x20E0) {
        alignedYaw = 0x4000;
    }

    cos = Math_CosS(alignedYaw);
    sin = Math_SinS(alignedYaw);
    cos = ABS(cos);
    sin = ABS(sin);
    xFromPlayer = thisv->actor.world.pos.x - player->actor.world.pos.x;
    zFromPlayer = thisv->actor.world.pos.z - player->actor.world.pos.z;
    xFromPlayerAbs = ABS(xFromPlayer);
    if (xFromPlayerAbs < (cos * 37.0f + sin * 400.0f)) {
        zFromPlayerAbs = ABS(zFromPlayer);
        if (zFromPlayerAbs < (sin * 37.0f + cos * 400.0f)) {
            return true;
        }
    }
    return false;
}

void EnMb_FindWaypointTowardsPlayer(EnMb* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->path];
    s16 yawToWaypoint;
    Vec3f waypointPosF;
    Vec3s* waypointPosS;
    s16 yawPlayerToWaypoint;
    s32 i;
    s32 waypoint;

    for (waypoint = 0, i = path->count - 1; i >= 0; i--, waypoint++) {
        waypointPosS = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + waypoint;
        waypointPosF.x = waypointPosS->x;
        waypointPosF.y = waypointPosS->y;
        waypointPosF.z = waypointPosS->z;
        yawToWaypoint = Math_Vec3f_Yaw(&thisv->actor.world.pos, &waypointPosF);
        yawPlayerToWaypoint = yawToWaypoint - thisv->actor.yawTowardsPlayer;
        if (ABS(yawPlayerToWaypoint) <= 0x1770) {
            thisv->actor.world.rot.y = yawToWaypoint;
            if (waypoint == thisv->waypoint) {
                thisv->direction = -thisv->direction;
            }
            thisv->waypointPos = waypointPosF;
            thisv->waypoint = waypoint;
            break;
        }
    }
}

void EnMb_SetupSpearGuardLookAround(EnMb* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gEnMbSpearLookLeftAndRightAnim, -4.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->timer1 = Rand_S16Offset(30, 50);
    thisv->state = ENMB_STATE_IDLE;
    EnMb_SetupAction(thisv, EnMb_SpearGuardLookAround);
}

void EnMb_SetupClubWaitPlayerNear(EnMb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gEnMbClubStandStillClubDownAnim);
    thisv->actor.speedXZ = 0.0f;
    thisv->timer1 = Rand_S16Offset(30, 50);
    thisv->state = ENMB_STATE_IDLE;
    EnMb_SetupAction(thisv, EnMb_ClubWaitPlayerNear);
}

void EnMb_SetupSpearPatrolTurnTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx) {
    Animation_MorphToLoop(&thisv->skelAnime, &gEnMbSpearLookLeftAndRightAnim, -4.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->timer1 = Rand_S16Offset(40, 80);
    thisv->state = ENMB_STATE_IDLE;
    EnMb_NextWaypoint(thisv, globalCtx);
    EnMb_SetupAction(thisv, EnMb_SpearPatrolTurnTowardsWaypoint);
}

void EnMb_SetupSpearGuardWalk(EnMb* thisv) {
    Animation_Change(&thisv->skelAnime, &gEnMbSpearWalkAnim, 0.0f, 0.0f, Animation_GetLastFrame(&gEnMbSpearWalkAnim),
                     ANIMMODE_LOOP, -4.0f);
    thisv->actor.speedXZ = 0.59999996f;
    thisv->timer1 = Rand_S16Offset(50, 70);
    thisv->unk_332 = 1;
    thisv->state = ENMB_STATE_WALK;
    EnMb_SetupAction(thisv, EnMb_SpearGuardWalk);
}

void EnMb_SetupSpearPatrolWalkTowardsWaypoint(EnMb* thisv) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbSpearWalkAnim);

    thisv->actor.speedXZ = 0.59999996f;
    thisv->timer1 = Rand_S16Offset(50, 70);
    thisv->unk_332 = 1;
    thisv->state = ENMB_STATE_WALK;
    Animation_Change(&thisv->skelAnime, &gEnMbSpearWalkAnim, 0.0f, 0.0f, frameCount, ANIMMODE_LOOP_INTERP, -4.0f);
    EnMb_SetupAction(thisv, EnMb_SpearPatrolWalkTowardsWaypoint);
}

void EnMb_SetupSpearPrepareAndCharge(EnMb* thisv) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim);

    Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbSpearPrepareChargeAnim, -4.0f);
    thisv->state = ENMB_STATE_ATTACK;
    thisv->actor.speedXZ = 0.0f;
    thisv->timer3 = (s16)frameCount + 6;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_SPEAR_AT);
    if (thisv->actor.params == ENMB_TYPE_SPEAR_GUARD) {
        EnMb_SetupAction(thisv, EnMb_SpearGuardPrepareAndCharge);
    } else {
        EnMb_SetupAction(thisv, EnMb_SpearPatrolPrepareAndCharge);
    }
}

void EnMb_SetupSpearPatrolImmediateCharge(EnMb* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gEnMbSpearChargeAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_ATTACK);
    thisv->attack = ENMB_ATTACK_SPEAR;
    thisv->state = ENMB_STATE_ATTACK;
    thisv->timer3 = 3;
    thisv->actor.speedXZ = 10.0f;
    EnMb_SetupAction(thisv, EnMb_SpearPatrolImmediateCharge);
}

void EnMb_SetupClubAttack(EnMb* thisv) {
    f32 frames = Animation_GetLastFrame(&gEnMbClubLiftClubAnim);
    s16 relYawFromPlayer;

    thisv->state = ENMB_STATE_ATTACK;
    Animation_Change(&thisv->skelAnime, &gEnMbClubLiftClubAnim, 3.0f, 0.0f, frames, ANIMMODE_ONCE_INTERP, 0.0f);
    thisv->timer3 = 1;
    relYawFromPlayer = thisv->actor.world.rot.y - thisv->actor.yawTowardsPlayer;

    if (ABS(relYawFromPlayer) <= 0x258) {
        thisv->attack = ENMB_ATTACK_CLUB_MIDDLE;
    } else if (relYawFromPlayer >= 0) {
        thisv->attack = ENMB_ATTACK_CLUB_RIGHT;
    } else {
        thisv->attack = ENMB_ATTACK_CLUB_LEFT;
    }

    EnMb_SetupAction(thisv, EnMb_ClubAttack);
}

void EnMb_SetupSpearEndChargeQuick(EnMb* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gEnMbSpearSlowDownAnim);
    thisv->state = ENMB_STATE_ATTACK_END;
    thisv->timer1 = 0;
    thisv->timer3 = 5;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_SLIDE);
    EnMb_SetupAction(thisv, EnMb_SpearEndChargeQuick);
}

void EnMb_SetupSpearPatrolEndCharge(EnMb* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gEnMbSpearSlowDownAnim);
    thisv->state = ENMB_STATE_ATTACK_END;
    thisv->actor.bgCheckFlags &= ~1;
    thisv->timer1 = 0;
    thisv->timer3 = 50;
    thisv->actor.speedXZ = -8.0f;
    thisv->actor.velocity.y = 6.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_SLIDE);
    EnMb_SetupAction(thisv, EnMb_SpearPatrolEndCharge);
}

void EnMb_SetupClubWaitAfterAttack(EnMb* thisv) {
    f32 frameCount = Animation_GetLastFrame(&gEnMbClubStandStillClubDownAnim);

    thisv->state = ENMB_STATE_ATTACK_END;
    Animation_Change(&thisv->skelAnime, &gEnMbClubStandStillClubDownAnim, 5.0f, 0.0f, frameCount, ANIMMODE_ONCE_INTERP,
                     0.0f);
    EnMb_SetupAction(thisv, EnMb_ClubWaitAfterAttack);
}

void EnMb_SetupClubDamaged(EnMb* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gEnMbClubDamagedKneelAnim);
    thisv->state = ENMB_STATE_CLUB_KNEELING;
    thisv->timer1 = 0;
    thisv->timer3 = 20;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(thisv, EnMb_ClubDamaged);
}

void EnMb_SetupClubDamagedWhileKneeling(EnMb* thisv) {
    f32 frames = Animation_GetLastFrame(&gEnMbClubBeatenKneelingAnim);

    thisv->state = ENMB_STATE_CLUB_KNEELING_DAMAGED;
    thisv->timer1 = 0;
    thisv->timer3 = 6;
    Animation_Change(&thisv->skelAnime, &gEnMbClubBeatenKneelingAnim, 1.0f, 4.0f, frames, ANIMMODE_ONCE_INTERP, 0.0f);
    EnMb_SetupAction(thisv, EnMb_ClubDamagedWhileKneeling);
}

void EnMb_SetupClubDead(EnMb* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbClubFallOnItsBackAnim, -4.0f);
    thisv->state = ENMB_STATE_CLUB_DEAD;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->hitbox.dim.height = 80;
    thisv->hitbox.dim.radius = 95;
    thisv->timer1 = 30;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(thisv, EnMb_ClubDead);
}

void EnMb_SetupStunned(EnMb* thisv) {
    thisv->state = ENMB_STATE_STUNNED;
    thisv->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);
    if (thisv->damageEffect == ENMB_DMGEFF_STUN_ICE) {
        thisv->iceEffectTimer = 40;
    } else {
        if (thisv->actor.params != ENMB_TYPE_CLUB) {
            Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gEnMbSpearDamagedFromFrontAnim, 0.0f);
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }
    EnMb_SetupAction(thisv, EnMb_Stunned);
}

void EnMb_Stunned(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.world.rot.y, 4.0f);
        thisv->attack = ENMB_ATTACK_NONE;
    }

    if (thisv->actor.colorFilterTimer == 0) {
        if (thisv->actor.params == ENMB_TYPE_CLUB) {
            if (thisv->actor.colChkInfo.health == 0) {
                EnMb_SetupClubDead(thisv);
            } else if (thisv->state == ENMB_STATE_CLUB_KNEELING) {
                /* dead code: the setup for thisv action sets state to something else */
                EnMb_SetupClubDamagedWhileKneeling(thisv);
            } else {
                EnMb_SetupClubWaitPlayerNear(thisv);
            }
        } else {
            if (thisv->actor.colChkInfo.health == 0) {
                EnMb_SetupSpearDead(thisv);
            } else {
                EnMb_SetupSpearDamaged(thisv);
            }
        }
    }
}

void EnMb_SpearGuardLookAround(EnMb* thisv, GlobalContext* globalCtx) {
    s16 timer1;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer1 == 0) {
        timer1 = 0;
    } else {
        thisv->timer1--;
        timer1 = thisv->timer1;
    }
    if (timer1 == 0 && Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        EnMb_SetupSpearGuardWalk(thisv);
    }
}

void EnMb_SpearPatrolTurnTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx) {
    s16 relYawFromPlayer;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer1 == 0) {
        thisv->yawToWaypoint = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->waypointPos);
        if (Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->yawToWaypoint, 1, 0x3E8, 0) == 0) {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
            EnMb_SetupSpearPatrolWalkTowardsWaypoint(thisv);
        }
    } else {
        thisv->timer1--;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, 1, 0x3E8, 0);
    }

    if (ABS(thisv->actor.yDistToPlayer) <= 20.0f && EnMb_IsPlayerInCorridor(thisv, globalCtx)) {
        relYawFromPlayer = thisv->actor.shape.rot.y - thisv->actor.yawTowardsPlayer;
        if (ABS(relYawFromPlayer) <= 0x4000 || (func_8002DDE4(globalCtx) && thisv->actor.xzDistToPlayer < 160.0f)) {
            EnMb_FindWaypointTowardsPlayer(thisv, globalCtx);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_VOICE);
            EnMb_SetupSpearPrepareAndCharge(thisv);
        }
    }
}

/**
 * Slow down and resume walking.
 */
void EnMb_SpearEndChargeQuick(EnMb* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.5f, 1.0f, 0.0f);
    if (thisv->actor.speedXZ > 1.0f) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer1 == 0) {
            thisv->timer3--;
            if (thisv->timer3 == 0) {
                /* Play the charge animation in reverse: let go of the spear and stand normally */
                Animation_Change(&thisv->skelAnime, &gEnMbSpearPrepareChargeAnim, -1.0f,
                                 Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
                thisv->timer1 = 1;
                thisv->actor.speedXZ = 0.0f;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_SPEAR_NORM);
            }
        } else {
            if (thisv->actor.params <= ENMB_TYPE_SPEAR_GUARD) {
                EnMb_SetupSpearGuardWalk(thisv);
                thisv->timer1 = thisv->timer2 = thisv->timer3 = 80;
            } else {
                EnMb_SetupSpearPatrolTurnTowardsWaypoint(thisv, globalCtx);
            }
        }
    }
}

void EnMb_ClubWaitAfterAttack(EnMb* thisv, GlobalContext* globalCtx) {
    thisv->attack = ENMB_ATTACK_NONE;
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnMb_SetupClubWaitPlayerNear(thisv);
    }
}

/**
 * Slow down, charge again if the player is near, or resume walking.
 */
void EnMb_SpearPatrolEndCharge(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 lastFrame;
    s16 relYawFromPlayer;
    s16 yawPlayerToWaypoint;

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.world.rot.y, 4.0f);
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.5f, 0.0f);

        if (thisv->actor.speedXZ > 1.0f) {
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        }

        if (thisv->timer1 != 0) {
            thisv->timer3--;
            if (thisv->timer3 == 0) {
                relYawFromPlayer = thisv->actor.shape.rot.y - thisv->actor.yawTowardsPlayer;

                if (ABS(thisv->actor.yDistToPlayer) <= 20.0f && EnMb_IsPlayerInCorridor(thisv, globalCtx) &&
                    ABS(relYawFromPlayer) <= 0x4000 && thisv->actor.xzDistToPlayer <= 200.0f) {
                    EnMb_SetupSpearPrepareAndCharge(thisv);
                } else {
                    lastFrame = Animation_GetLastFrame(&gEnMbSpearPrepareChargeAnim);
                    /* Play the charge animation in reverse: let go of the spear and stand normally */
                    Animation_Change(&thisv->skelAnime, &gEnMbSpearPrepareChargeAnim, -1.0f, lastFrame, 0.0f,
                                     ANIMMODE_ONCE, 0.0f);
                    thisv->actor.speedXZ = 0.0f;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_SPEAR_NORM);
                }
            } else {
                if (thisv->actor.xzDistToPlayer <= 160.0f) {
                    thisv->actor.speedXZ = -5.0f;
                } else {
                    thisv->actor.speedXZ = 0.0f;
                }
            }
        }

        if (SkelAnime_Update(&thisv->skelAnime)) {
            if (thisv->timer1 == 0) {
                lastFrame = Animation_GetLastFrame(&gEnMbSpearChargeAnim);
                Animation_Change(&thisv->skelAnime, &gEnMbSpearChargeAnim, 0.5f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                                 0.0f);
                thisv->timer1 = 1;
            } else {
                yawPlayerToWaypoint =
                    Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->waypointPos) - thisv->actor.yawTowardsPlayer;

                if (ABS(yawPlayerToWaypoint) <= 0x4000) {
                    EnMb_SetupSpearPatrolTurnTowardsWaypoint(thisv, globalCtx);
                } else {
                    EnMb_SetupSpearPatrolWalkTowardsWaypoint(thisv);
                }
            }
        }
    }
}

/**
 * Prepare charge (animation), then charge until the player isn't in front.
 */
void EnMb_SpearGuardPrepareAndCharge(EnMb* thisv, GlobalContext* globalCtx) {
    s32 prevFrame;
    s16 relYawTowardsPlayerAbs = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (relYawTowardsPlayerAbs < 0) {
        relYawTowardsPlayerAbs = -relYawTowardsPlayerAbs;
    }

    prevFrame = thisv->skelAnime.curFrame;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_PlayLoop(&thisv->skelAnime, &gEnMbSpearChargeAnim);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_ATTACK);
    }

    if (thisv->timer3 != 0) {
        thisv->timer3--;
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    } else {
        thisv->actor.speedXZ = 10.0f;
        thisv->attack = ENMB_ATTACK_SPEAR;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        if (prevFrame != (s32)thisv->skelAnime.curFrame &&
            ((s32)thisv->skelAnime.curFrame == 2 || (s32)thisv->skelAnime.curFrame == 6)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DASH);
        }
    }

    if (relYawTowardsPlayerAbs > 0x1388) {
        thisv->attack = ENMB_ATTACK_NONE;
        EnMb_SetupSpearEndChargeQuick(thisv);
    }
}

void EnMb_ClubAttack(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    Vec3f effSpawnPos;
    Vec3f effWhiteShockwaveDynamics = { 0.0f, 0.0f, 0.0f };
    f32 flamesParams[] = { 18.0f, 18.0f, 0.0f };
    s16 flamesUnused[] = { 20, 40, 0 };
    s16 relYawTarget[] = { -0x9C4, 0, 0xDAC };

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, relYawTarget[thisv->attack - 1] + thisv->actor.world.rot.y, 1, 0x2EE, 0);

    if (thisv->attackCollider.base.atFlags & AT_HIT) {
        thisv->attackCollider.base.atFlags &= ~AT_HIT;
        if (thisv->attackCollider.base.at == &player->actor) {
            u8 prevPlayerInvincibilityTimer = player->invincibilityTimer;

            if (player->invincibilityTimer < 0) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    globalCtx->damagePlayer(globalCtx, -8);
                }
            }

            func_8002F71C(globalCtx, &thisv->actor, (650.0f - thisv->actor.xzDistToPlayer) * 0.04f + 4.0f,
                          thisv->actor.world.rot.y, 8.0f);

            player->invincibilityTimer = prevPlayerInvincibilityTimer;
        }
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer3 != 0) {
            thisv->timer3--;
            if (thisv->timer3 == 0) {
                f32 lastAnimFrame = Animation_GetLastFrame(&gEnMbClubStrikeDownAnim);
                Animation_Change(&thisv->skelAnime, &gEnMbClubStrikeDownAnim, 1.5f, 0.0f, lastAnimFrame,
                                 ANIMMODE_ONCE_INTERP, 0.0f);
            }
        } else {
            effSpawnPos = thisv->effSpawnPos;
            effSpawnPos.y = thisv->actor.floorHeight;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MONBLIN_HAM_LAND);
            func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
            EffectSsBlast_SpawnWhiteShockwave(globalCtx, &effSpawnPos, &effWhiteShockwaveDynamics,
                                              &effWhiteShockwaveDynamics);
            func_80033480(globalCtx, &effSpawnPos, 2.0f, 3, 0x12C, 0xB4, 1);
            Camera_AddQuake(&globalCtx->mainCamera, 2, 0x19, 5);
            func_800358DC(&thisv->actor, &effSpawnPos, &thisv->actor.world.rot, flamesParams, 20, flamesUnused, globalCtx,
                          -1, 0);
            EnMb_SetupClubWaitAfterAttack(thisv);
        }
    } else {
        if (thisv->timer3 != 0 && thisv->skelAnime.curFrame == 6.0f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MONBLIN_HAM_UP);

        } else if (thisv->timer3 == 0 && thisv->skelAnime.curFrame == 3.0f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MONBLIN_HAM_DOWN);
        }
    }
}

/**
 * Prepare charge (animation), then charge to the end of the floor collision.
 */
void EnMb_SpearPatrolPrepareAndCharge(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 prevFrame;
    s32 hasHitPlayer = false;
    s32 endCharge = !Actor_TestFloorInDirection(&thisv->actor, globalCtx, 110.0f, thisv->actor.world.rot.y);

    prevFrame = (s32)thisv->skelAnime.curFrame;
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_PlayLoop(&thisv->skelAnime, &gEnMbSpearChargeAnim);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_ATTACK);
    }

    if (thisv->timer3 != 0) {
        thisv->timer3--;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 1, 0x1F40, 0);
        endCharge = false;
    } else {
        thisv->actor.speedXZ = 10.0f;
        thisv->attack = ENMB_ATTACK_SPEAR;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
        if (prevFrame != (s32)thisv->skelAnime.curFrame &&
            ((s32)thisv->skelAnime.curFrame == 2 || (s32)thisv->skelAnime.curFrame == 6)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DASH);
        }
    }

    if (thisv->attackCollider.base.atFlags & AT_HIT) {
        if (thisv->attackCollider.base.at == &player->actor) {
            if (!endCharge && !(player->stateFlags2 & 0x80)) {
                if (player->invincibilityTimer < 0) {
                    if (player->invincibilityTimer < -39) {
                        player->invincibilityTimer = 0;
                    } else {
                        player->invincibilityTimer = 0;
                        globalCtx->damagePlayer(globalCtx, -8);
                    }
                }
                if (!(thisv->attackCollider.base.atFlags & AT_BOUNCED)) {
                    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                }
                if (globalCtx->grabPlayer(globalCtx, player)) {
                    player->actor.parent = &thisv->actor;
                }
            }
            hasHitPlayer = true;
        } else {
            thisv->attackCollider.base.atFlags &= ~AT_HIT;
        }
    }

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
        player->actor.world.pos.x = thisv->actor.world.pos.x + Math_CosS(thisv->actor.shape.rot.y) * 10.0f +
                                    Math_SinS(thisv->actor.shape.rot.y) * 89.0f;
        hasHitPlayer = true;
        player->actor.world.pos.z = thisv->actor.world.pos.z + Math_SinS(thisv->actor.shape.rot.y) * 10.0f +
                                    Math_CosS(thisv->actor.shape.rot.y) * 89.0f;
        player->unk_850 = 0;
        player->actor.speedXZ = 0.0f;
        player->actor.velocity.y = 0.0f;
    }

    if (endCharge) {
        if (hasHitPlayer || (player->stateFlags2 & 0x80)) {
            thisv->attackCollider.base.atFlags &= ~AT_HIT;
            if (player->stateFlags2 & 0x80) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.world.rot.y, 4.0f);
            }
        }
        thisv->attack = ENMB_ATTACK_NONE;
        thisv->actor.speedXZ = -10.0f;
        EnMb_SetupSpearPatrolEndCharge(thisv);
    }
}

/**
 * Charge and follow the path, until hitting the player or, after some time, reaching home.
 */
void EnMb_SpearPatrolImmediateCharge(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 prevFrame;
    s32 hasHitPlayer = false;
    s32 endCharge = !Actor_TestFloorInDirection(&thisv->actor, globalCtx, 110.0f, thisv->actor.world.rot.y);

    prevFrame = (s32)thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);

    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 5.0f, 3, 4.0f, 100, 15, false);
    if (prevFrame != (s32)thisv->skelAnime.curFrame &&
        ((s32)thisv->skelAnime.curFrame == 2 || (s32)thisv->skelAnime.curFrame == 6)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DASH);
    }

    if (thisv->attackCollider.base.atFlags & AT_HIT) {
        if (thisv->attackCollider.base.at == &player->actor) {
            if (!endCharge && !(player->stateFlags2 & 0x80)) {
                if (player->invincibilityTimer < 0) {
                    if (player->invincibilityTimer <= -40) {
                        player->invincibilityTimer = 0;
                    } else {
                        player->invincibilityTimer = 0;
                        globalCtx->damagePlayer(globalCtx, -8);
                    }
                }
                if (!(thisv->attackCollider.base.atFlags & AT_BOUNCED)) {
                    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                }
                if (globalCtx->grabPlayer(globalCtx, player)) {
                    player->actor.parent = &thisv->actor;
                }
            }
            hasHitPlayer = true;
        } else {
            thisv->attackCollider.base.atFlags &= ~AT_HIT;
        }
    }

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
        player->actor.world.pos.x = thisv->actor.world.pos.x + Math_CosS(thisv->actor.shape.rot.y) * 10.0f +
                                    Math_SinS(thisv->actor.shape.rot.y) * 89.0f;
        hasHitPlayer = true;
        player->actor.world.pos.z = thisv->actor.world.pos.z + Math_SinS(thisv->actor.shape.rot.y) * 10.0f +
                                    Math_CosS(thisv->actor.shape.rot.y) * 89.0f;
        player->unk_850 = 0;
        player->actor.speedXZ = 0.0f;
        player->actor.velocity.y = 0.0f;
    }

    if (endCharge) {
        if (hasHitPlayer || (player->stateFlags2 & 0x80)) {
            thisv->attackCollider.base.atFlags &= ~AT_HIT;
            if (player->stateFlags2 & 0x80) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.world.rot.y, 4.0f);
            }
            thisv->attack = ENMB_ATTACK_NONE;
            thisv->actor.speedXZ = -10.0f;
            EnMb_SetupSpearPatrolEndCharge(thisv);
            thisv->timer3 = 1;
        } else {
            thisv->timer3--;
            EnMb_NextWaypoint(thisv, globalCtx);
        }
    }

    EnMb_FaceWaypoint(thisv, globalCtx);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (thisv->timer3 == 0 && Math_Vec3f_DistXZ(&thisv->actor.home.pos, &thisv->actor.world.pos) < 80.0f) {
        thisv->attack = ENMB_ATTACK_NONE;
        EnMb_SetupSpearEndChargeQuick(thisv);
    }
}

void EnMb_ClubDamaged(EnMb* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer3 != 0) {
            Animation_PlayOnce(&thisv->skelAnime, &gEnMbClubStandUpAnim);
            thisv->timer3 = 0;
            func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
            Camera_AddQuake(&globalCtx->mainCamera, 2, 25, 5);
        } else {
            EnMb_SetupClubWaitPlayerNear(thisv);
        }
    }
}

void EnMb_ClubDamagedWhileKneeling(EnMb* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer3 != 0) {
            thisv->timer3--;
            if (thisv->timer3 == 0) {
                if (thisv->timer1 == 0) {
                    Animation_Change(&thisv->skelAnime, &gEnMbClubStandUpAnim, 3.0f, 0.0f,
                                     Animation_GetLastFrame(&gEnMbClubStandUpAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                    thisv->timer1 = 1;
                    thisv->timer3 = 6;
                } else {
                    Animation_Change(&thisv->skelAnime, &gEnMbClubStandUpAnim, 3.0f, 0.0f,
                                     Animation_GetLastFrame(&gEnMbClubStandUpAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                }
            }
        } else {
            EnMb_SetupClubWaitPlayerNear(thisv);
        }
    }
}

void EnMb_ClubDead(EnMb* thisv, GlobalContext* globalCtx) {
    Vec3f effPos;
    Vec3f effPosBase;

    effPos = thisv->actor.world.pos;
    effPos.x += Math_SinS(thisv->actor.shape.rot.y) * -70.0f;
    effPos.z += Math_CosS(thisv->actor.shape.rot.y) * -70.0f;
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    effPosBase = effPos;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer1 > 0) {
            Vec3f effZeroVec = { 0.0f, 0.0f, 0.0f };
            s32 i;

            thisv->timer1--;
            for (i = 4; i >= 0; i--) {
                effPos.x = Rand_CenteredFloat(240.0f) + effPosBase.x;
                effPos.y = Rand_CenteredFloat(15.0f) + (effPosBase.y + 20.0f);
                effPos.z = Rand_CenteredFloat(240.0f) + effPosBase.z;
                EffectSsDeadDb_Spawn(globalCtx, &effPos, &effZeroVec, &effZeroVec, 230, 7, 255, 255, 255, 255, 0, 255,
                                     0, 1, 9, true);
            }
        } else {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &effPos, 0xC0);
            Actor_Kill(&thisv->actor);
        }
    } else if ((s32)thisv->skelAnime.curFrame == 15 || (s32)thisv->skelAnime.curFrame == 22) {
        func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &effPos, 50.0f, 10, 3.0f, 400, 60, false);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        Camera_AddQuake(&globalCtx->mainCamera, 2, 25, 5);
    }
}

/**
 * Walk around the home point, face and charge the player if close.
 */
void EnMb_SpearGuardWalk(EnMb* thisv, GlobalContext* globalCtx) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 pad1;
    s32 pad2;
    Player* player = GET_PLAYER(globalCtx);
    s16 relYawTowardsPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    s16 yawTowardsHome;
    f32 playSpeedAbs;

    relYawTowardsPlayer = ABS(relYawTowardsPlayer);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.59999996f, 0.1f, 1.0f, 0.0f);
    thisv->skelAnime.playSpeed = thisv->actor.speedXZ;
    prevFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);

    playSpeedAbs = ABS(thisv->skelAnime.playSpeed);
    beforeCurFrame = thisv->skelAnime.curFrame - playSpeedAbs;
    playSpeedAbs = ABS(thisv->skelAnime.playSpeed);
    if (thisv->timer3 == 0 &&
        Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < thisv->playerDetectionRange) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x2EE, 0);
        thisv->actor.flags |= ACTOR_FLAG_0;
        if (thisv->actor.xzDistToPlayer < 500.0f && relYawTowardsPlayer < 0x1388) {
            EnMb_SetupSpearPrepareAndCharge(thisv);
        }
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        if (Math_Vec3f_DistXZ(&thisv->actor.world.pos, &thisv->actor.home.pos) > thisv->maxHomeDist || thisv->timer2 != 0) {
            yawTowardsHome = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
            Math_SmoothStepToS(&thisv->actor.world.rot.y, yawTowardsHome, 1, 0x2EE, 0);
        }
        if (thisv->timer2 != 0) {
            thisv->timer2--;
        }
        if (thisv->timer3 != 0) {
            thisv->timer3--;
        }
        if (thisv->timer2 == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_VOICE);
        }
        thisv->timer1--;
        if (thisv->timer1 == 0) {
            if (Rand_ZeroOne() > 0.7f) {
                thisv->timer1 = Rand_S16Offset(50, 70);
                thisv->timer2 = Rand_S16Offset(15, 40);
            } else {
                EnMb_SetupSpearGuardLookAround(thisv);
            }
        }
    }

    if (prevFrame != (s32)thisv->skelAnime.curFrame) {
        if ((beforeCurFrame <= 1 && prevFrame + (s32)playSpeedAbs >= 1) ||
            (beforeCurFrame <= 20 && prevFrame + (s32)playSpeedAbs >= 20)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnMb_SpearPatrolWalkTowardsWaypoint(EnMb* thisv, GlobalContext* globalCtx) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s16 relYawTowardsPlayer;
    f32 yDistToPlayerAbs;
    f32 playSpeedABS;

    if (Math_Vec3f_DistXZ(&thisv->waypointPos, &thisv->actor.world.pos) <= 8.0f ||
        (Rand_ZeroOne() < 0.1f && Math_Vec3f_DistXZ(&thisv->actor.home.pos, &thisv->actor.world.pos) <= 4.0f)) {
        EnMb_SetupSpearPatrolTurnTowardsWaypoint(thisv, globalCtx);
    } else {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.59999996f, 0.1f, 1.0f, 0.0f);
        thisv->skelAnime.playSpeed = 2.0f * thisv->actor.speedXZ;
    }

    thisv->yawToWaypoint = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->waypointPos);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->yawToWaypoint, 1, 0x5DC, 0);

    yDistToPlayerAbs = (thisv->actor.yDistToPlayer >= 0.0f) ? thisv->actor.yDistToPlayer : -thisv->actor.yDistToPlayer;
    if (yDistToPlayerAbs <= 20.0f && EnMb_IsPlayerInCorridor(thisv, globalCtx)) {
        relYawTowardsPlayer = (thisv->actor.shape.rot.y - thisv->actor.yawTowardsPlayer);
        if (ABS(relYawTowardsPlayer) <= 0x4000 || (func_8002DDE4(globalCtx) && thisv->actor.xzDistToPlayer < 160.0f)) {
            EnMb_FindWaypointTowardsPlayer(thisv, globalCtx);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_VOICE);
            EnMb_SetupSpearPrepareAndCharge(thisv);
            return;
        }
    }

    if (thisv->timer2 != 0) {
        thisv->timer2--;
    }
    if (thisv->timer3 != 0) {
        thisv->timer3--;
    }
    if (thisv->timer2 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_VOICE);
        thisv->timer2 = Rand_S16Offset(30, 70);
    }

    prevFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    playSpeedABS = ABS(thisv->skelAnime.playSpeed);
    beforeCurFrame = thisv->skelAnime.curFrame - playSpeedABS;
    playSpeedABS = (thisv->skelAnime.playSpeed >= 0.0f) ? thisv->skelAnime.playSpeed : -thisv->skelAnime.playSpeed;
    if (prevFrame != (s32)thisv->skelAnime.curFrame) {
        if ((beforeCurFrame <= 1 && (s32)playSpeedABS + prevFrame >= 1) ||
            (beforeCurFrame <= 20 && (s32)playSpeedABS + prevFrame >= 20)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnMb_ClubWaitPlayerNear(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s16 relYawFromPlayer = thisv->actor.world.rot.y - thisv->actor.yawTowardsPlayer;

    SkelAnime_Update(&thisv->skelAnime);
    if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < thisv->playerDetectionRange &&
        !(player->stateFlags1 & 0x4000000) && ABS(relYawFromPlayer) < 0x3E80) {
        EnMb_SetupClubAttack(thisv);
    }
}

void EnMb_SetupSpearDamaged(EnMb* thisv) {
    s16 relYawTowardsPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(relYawTowardsPlayer) <= 0x4000) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbSpearDamagedFromFrontAnim, -4.0f);
        thisv->actor.speedXZ = -8.0f;
    } else {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbSpearDamagedFromBehindAnim, -4.0f);
        thisv->actor.speedXZ = 8.0f;
    }

    thisv->timer1 = 30;
    thisv->state = ENMB_STATE_SPEAR_SPEARPATH_DAMAGED;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DEAD);
    EnMb_SetupAction(thisv, EnMb_SpearDamaged);
}

void EnMb_SpearDamaged(EnMb* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.params <= ENMB_TYPE_SPEAR_GUARD) {
            EnMb_SetupSpearGuardLookAround(thisv);
        } else {
            EnMb_SetupSpearPatrolImmediateCharge(thisv);
        }
    }
}

void EnMb_SetupSpearDead(EnMb* thisv) {
    s16 relYawTowardsPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(relYawTowardsPlayer) <= 0x4000) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbSpearFallOnItsBackAnim, -4.0f);
        thisv->actor.speedXZ = -8.0f;
    } else {
        /* The gEnMbSpearFallFaceDownAnim animation was probably meant to be used here */
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gEnMbSpearFallOnItsBackAnim, -4.0f);
        thisv->actor.speedXZ = 8.0f;
    }

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->timer1 = 30;
    thisv->state = ENMB_STATE_SPEAR_SPEARPATH_DAMAGED;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_DEAD);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnMb_SetupAction(thisv, EnMb_SpearDead);
}

void EnMb_SpearDead(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);

    if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
        player->stateFlags2 &= ~0x80;
        player->actor.parent = NULL;
        player->unk_850 = 200;
        func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.world.rot.y, 4.0f);
        thisv->attack = ENMB_ATTACK_NONE;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer1 > 0) {
            Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
            s32 i;
            Vec3f effPos;

            thisv->actor.shape.shadowScale = 0.0f;
            thisv->timer1--;
            for (i = 4; i >= 0; i--) {
                effPos.x = Rand_CenteredFloat(110.0f) + thisv->actor.world.pos.x;
                effPos.y = Rand_CenteredFloat(15.0f) + (thisv->actor.world.pos.y + 20.0f);
                effPos.z = Rand_CenteredFloat(110.0f) + thisv->actor.world.pos.z;

                EffectSsDeadDb_Spawn(globalCtx, &effPos, &zeroVec, &zeroVec, 100, 7, 255, 255, 255, 255, 0, 255, 0, 1,
                                     9, true);
            }
        } else {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xE0);
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnMb_SpearUpdateAttackCollider(Actor* thisx, GlobalContext* globalCtx) {
    Vec3f quadModel0 = { 1000.0f, 1500.0f, 0.0f };
    Vec3f quadModel1 = { -1000.0f, 1500.0f, 0.0f };
    Vec3f quadModel2 = { 1000.0f, 1500.0f, 4500.0f };
    Vec3f quadModel3 = { -1000.0f, 1500.0f, 4500.0f };
    EnMb* thisv = (EnMb*)thisx;

    if (thisv->actor.params >= ENMB_TYPE_SPEAR_PATROL) {
        quadModel0.x += 2000.0f;
        quadModel0.z = -4000.0f;
        quadModel1.z = -4000.0f;
        quadModel2.x += 2000.0f;
        quadModel1.x -= 2000.0f;
        quadModel3.x -= 2000.0f;
        quadModel2.z += 4000.0f;
        quadModel3.z += 4000.0f;
    }
    Matrix_MultVec3f(&quadModel0, &thisv->attackCollider.dim.quad[1]);
    Matrix_MultVec3f(&quadModel1, &thisv->attackCollider.dim.quad[0]);
    Matrix_MultVec3f(&quadModel2, &thisv->attackCollider.dim.quad[3]);
    Matrix_MultVec3f(&quadModel3, &thisv->attackCollider.dim.quad[2]);
    Collider_SetQuadVertices(&thisv->attackCollider, &thisv->attackCollider.dim.quad[0],
                             &thisv->attackCollider.dim.quad[1], &thisv->attackCollider.dim.quad[2],
                             &thisv->attackCollider.dim.quad[3]);
}

void EnMb_ClubUpdateAttackCollider(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f quadModel[] = { { 1000.0f, 0.0f, 0.0f },
                                 { 1000.0f, 0.0f, 0.0f },
                                 { 1000.0f, -8000.0f, -1500.0f },
                                 { 1000.0f, -9000.0f, 2000.0f } };
    EnMb* thisv = (EnMb*)thisx;

    Matrix_MultVec3f(&quadModel[0], &thisv->attackCollider.dim.quad[1]);
    Matrix_MultVec3f(&quadModel[1], &thisv->attackCollider.dim.quad[0]);
    Matrix_MultVec3f(&quadModel[2], &thisv->attackCollider.dim.quad[3]);
    Matrix_MultVec3f(&quadModel[3], &thisv->attackCollider.dim.quad[2]);
    Collider_SetQuadVertices(&thisv->attackCollider, &thisv->attackCollider.dim.quad[0],
                             &thisv->attackCollider.dim.quad[1], &thisv->attackCollider.dim.quad[2],
                             &thisv->attackCollider.dim.quad[3]);
}

void EnMb_CheckColliding(EnMb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->frontShielding.base.acFlags & AC_HIT) {
        thisv->frontShielding.base.acFlags &= ~(AC_HIT | AC_BOUNCED);
        thisv->hitbox.base.acFlags &= ~AC_HIT;
    } else if ((thisv->hitbox.base.acFlags & AC_HIT) && thisv->state >= ENMB_STATE_STUNNED) {
        thisv->hitbox.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != ENMB_DMGEFF_IGNORE &&
            thisv->actor.colChkInfo.damageEffect != ENMB_DMGEFF_FREEZE) {
            if ((player->stateFlags2 & 0x80) && player->actor.parent == &thisv->actor) {
                player->stateFlags2 &= ~0x80;
                player->actor.parent = NULL;
                player->unk_850 = 200;
                func_8002F71C(globalCtx, &thisv->actor, 6.0f, thisv->actor.world.rot.y, 6.0f);
            }
            thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
            thisv->attack = ENMB_ATTACK_NONE;
            Actor_SetDropFlag(&thisv->actor, &thisv->hitbox.info, false);
            if (thisv->actor.colChkInfo.damageEffect == ENMB_DMGEFF_STUN ||
                thisv->actor.colChkInfo.damageEffect == ENMB_DMGEFF_STUN_ICE) {
                if (thisv->state != ENMB_STATE_STUNNED) {
                    Actor_ApplyDamage(&thisv->actor);
                    EnMb_SetupStunned(thisv);
                }
            } else {
                Actor_ApplyDamage(&thisv->actor);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFA, 0, 0xC);
                if (thisv->actor.params == ENMB_TYPE_CLUB) {
                    if (thisv->actor.colChkInfo.health == 0) {
                        EnMb_SetupClubDead(thisv);
                    } else if (thisv->state != ENMB_STATE_CLUB_KNEELING) {
                        EnMb_SetupClubDamaged(thisv);
                    }
                } else {
                    if (thisv->actor.colChkInfo.health == 0) {
                        EnMb_SetupSpearDead(thisv);
                    } else {
                        EnMb_SetupSpearDamaged(thisv);
                    }
                }
            }
        }
    }
}

void EnMb_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMb* thisv = (EnMb*)thisx;
    s32 pad;

    EnMb_CheckColliding(thisv, globalCtx);
    if (thisx->colChkInfo.damageEffect != ENMB_DMGEFF_FREEZE) {
        thisv->actionFunc(thisv, globalCtx);
        Actor_MoveForward(thisx);
        Actor_UpdateBgCheckInfo(globalCtx, thisx, 40.0f, 40.0f, 70.0f, 0x1D);
        Actor_SetFocus(thisx, thisx->scale.x * 4500.0f);
        Collider_UpdateCylinder(thisx, &thisv->hitbox);
        if (thisx->colChkInfo.health <= 0) {
            thisv->hitbox.dim.pos.x += Math_SinS(thisx->shape.rot.y) * (-4400.0f * thisx->scale.y);
            thisv->hitbox.dim.pos.z += Math_CosS(thisx->shape.rot.y) * (-4400.0f * thisx->scale.y);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->hitbox.base);
        if (thisv->state >= ENMB_STATE_STUNNED &&
            (thisx->params == ENMB_TYPE_CLUB || thisv->state != ENMB_STATE_ATTACK)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->hitbox.base);
        }
        if (thisv->state >= ENMB_STATE_IDLE) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->frontShielding.base);
        }
        if (thisv->attack > ENMB_ATTACK_NONE) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->attackCollider.base);
        }
    }
}

void EnMb_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f unused = { 1100.0f, -700.0f, 0.0f };
    static Vec3f feetPos = { 0.0f, 0.0f, 0.0f };
    static Vec3f effSpawnPosModel = { 0.0f, -8000.0f, 0.0f };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 bodyPart = -1;
    EnMb* thisv = (EnMb*)thisx;
    Vec3f bodyPartPos;

    if (thisv->actor.params == ENMB_TYPE_CLUB) {
        if (limbIndex == ENMB_LIMB_LHAND) {
            Matrix_MultVec3f(&effSpawnPosModel, &thisv->effSpawnPos);
            if (thisv->attack > ENMB_ATTACK_NONE) {
                EnMb_ClubUpdateAttackCollider(&thisv->actor, globalCtx);
            }
        }
        Actor_SetFeetPos(&thisv->actor, limbIndex, ENMB_LIMB_LFOOT, &feetPos, ENMB_LIMB_RFOOT, &feetPos);
    }

    if (thisv->iceEffectTimer != 0) {
        switch (limbIndex) {
            case ENMB_LIMB_HEAD:
                bodyPart = 0;
                break;
            case ENMB_LIMB_LHAND:
                bodyPart = 1;
                break;
            case ENMB_LIMB_RHAND:
                bodyPart = 2;
                break;
            case ENMB_LIMB_LSHOULDER:
                bodyPart = 3;
                break;
            case ENMB_LIMB_RSHOULDER:
                bodyPart = 4;
                break;
            case ENMB_LIMB_CHEST:
                bodyPart = 5;
                break;
            case ENMB_LIMB_LTHIGH:
                bodyPart = 6;
                break;
            case ENMB_LIMB_RTHIGH:
                bodyPart = 7;
                break;
            case ENMB_LIMB_LFOOT:
                bodyPart = 8;
                break;
            case ENMB_LIMB_RFOOT:
                bodyPart = 9;
                break;
        }
        if (bodyPart >= 0) {
            Matrix_MultVec3f(&zeroVec, &bodyPartPos);
            thisv->bodyPartsPos[bodyPart].x = bodyPartPos.x;
            thisv->bodyPartsPos[bodyPart].y = bodyPartPos.y;
            thisv->bodyPartsPos[bodyPart].z = bodyPartPos.z;
        }
    }
}

void EnMb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f frontShieldingTriModel0[] = {
        { 4000.0f, 7000.0f, 3500.0f },
        { 4000.0f, 0.0f, 3500.0f },
        { -4000.0f, 7000.0f, 3500.0f },
    };
    static Vec3f frontShieldingTriModel1[] = {
        { -4000.0f, 7000.0f, 3500.0f },
        { -4000.0f, 0.0f, 3500.0f },
        { 4000.0f, 0.0f, 3500.0f },
    };
    s32 i;
    f32 scale;
    Vec3f frontShieldingTri0[3];
    Vec3f frontShieldingTri1[3];
    s32 bodyPartIdx;
    EnMb* thisv = (EnMb*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, EnMb_PostLimbDraw, thisx);

    if (thisx->params != ENMB_TYPE_CLUB) {
        if (thisv->attack > ENMB_ATTACK_NONE) {
            EnMb_SpearUpdateAttackCollider(thisx, globalCtx);
        }
        for (i = 0; i < 3; i++) {
            Matrix_MultVec3f(&frontShieldingTriModel0[i], &frontShieldingTri0[i]);
            Matrix_MultVec3f(&frontShieldingTriModel1[i], &frontShieldingTri1[i]);
        }
        Collider_SetTrisVertices(&thisv->frontShielding, 0, &frontShieldingTri0[0], &frontShieldingTri0[1],
                                 &frontShieldingTri0[2]);
        Collider_SetTrisVertices(&thisv->frontShielding, 1, &frontShieldingTri1[0], &frontShieldingTri1[1],
                                 &frontShieldingTri1[2]);
    }

    if (thisv->iceEffectTimer != 0) {
        thisx->colorFilterTimer++;
        if (thisv->iceEffectTimer >= 0) {
            thisv->iceEffectTimer--;
        }
        if ((thisv->iceEffectTimer % 4) == 0) {
            scale = 2.5f;
            if (thisx->params == ENMB_TYPE_CLUB) {
                scale = 4.0f;
            }
            bodyPartIdx = thisv->iceEffectTimer >> 2;
            EffectSsEnIce_SpawnFlyingVec3s(globalCtx, thisx, &thisv->bodyPartsPos[bodyPartIdx], 150, 150, 150, 250, 235,
                                           245, 255, scale);
        }
    }
}
