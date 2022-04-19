/**
 * File: z_en_horse.c
 * Overlay: ovl_En_Horse
 * Description: Rideable horses
 */

#include "z_en_horse.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
#include "objects/object_horse/object_horse.h"
#include "objects/object_hni/object_hni.h"
#include "scenes/overworld/spot09/spot09_scene.h"

#define FLAGS ACTOR_FLAG_4

typedef void (*EnHorseCsFunc)(EnHorse*, GlobalContext*, const CsCmdActorAction*);
typedef void (*EnHorseActionFunc)(EnHorse*, GlobalContext*);

void EnHorse_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHorse_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHorse_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHorse_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnHorse_InitCutscene(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_InitHorsebackArchery(EnHorse* thisv);
void EnHorse_InitFleePlayer(EnHorse* thisv);
void EnHorse_ResetIdleAnimation(EnHorse* thisv);
void EnHorse_StartIdleRidable(EnHorse* thisv);
void EnHorse_InitInactive(EnHorse* thisv);
void EnHorse_InitIngoHorse(EnHorse* thisv);

void EnHorse_Frozen(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_Inactive(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_Idle(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_FollowPlayer(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_UpdateIngoRace(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedIdle(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedIdleWhinneying(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedTurn(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedWalk(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedTrot(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedGallop(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_MountedRearing(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_Stopping(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_Reverse(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_LowJump(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_HighJump(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_BridgeJump(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_CutsceneUpdate(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_UpdateHorsebackArchery(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_FleePlayer(EnHorse* thisv, GlobalContext* globalCtx);

static const AnimationHeader* sEponaAnimHeaders[] = {
    &gEponaIdleAnim,     &gEponaWhinnyAnim,    &gEponaRefuseAnim,  &gEponaRearingAnim,     &gEponaWalkingAnim,
    &gEponaTrottingAnim, &gEponaGallopingAnim, &gEponaJumpingAnim, &gEponaJumpingHighAnim,
};

static const AnimationHeader* sHniAnimHeaders[] = {
    &gHorseIngoIdleAnim,      &gHorseIngoWhinnyAnim,  &gHorseIngoRefuseAnim,
    &gHorseIngoRearingAnim,   &gHorseIngoWalkingAnim, &gHorseIngoTrottingAnim,
    &gHorseIngoGallopingAnim, &gHorseIngoJumpingAnim, &gHorseIngoJumpingHighAnim,
};

static const AnimationHeader** sAnimationHeaders[] = { sEponaAnimHeaders, sHniAnimHeaders };

static f32 sPlaybackSpeeds[] = { 2.0f / 3.0f, 2.0f / 3.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f / 3.0f, 2.0f / 3.0f };

static const SkeletonHeader* sSkeletonHeaders[] = { &gEponaSkel, &gHorseIngoSkel };

ActorInit En_Horse_InitVars = {
    ACTOR_EN_HORSE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE,
    sizeof(EnHorse),
    (ActorFunc)EnHorse_Init,
    (ActorFunc)EnHorse_Destroy,
    (ActorFunc)EnHorse_Update,
    (ActorFunc)EnHorse_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000400, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphItemsInit,
};

static CollisionCheckInfoInit D_80A65F38 = { 10, 35, 100, MASS_HEAVY };

typedef struct {
    s16 scene;
    Vec3s pos;
    s16 angle;
} EnHorseSpawnpoint;

static EnHorseSpawnpoint sHorseSpawns[] = {
    // Hyrule Field
    { SCENE_SPOT00, 16, 0, 1341, 0 },
    { SCENE_SPOT00, -1297, 0, 1459, 0 },
    { SCENE_SPOT00, -5416, -300, 1296, 0 },
    { SCENE_SPOT00, -4667, -300, 3620, 0 },
    { SCENE_SPOT00, -3837, 81, 5537, 0 },
    { SCENE_SPOT00, -5093, -226, 6661, 0 },
    { SCENE_SPOT00, -6588, -79, 5053, 0 },
    { SCENE_SPOT00, -7072, -500, 7538, 0 },
    { SCENE_SPOT00, -6139, -500, 8910, 0 },
    { SCENE_SPOT00, -8497, -300, 7802, 0 },
    { SCENE_SPOT00, -5481, -499, 12127, 0 },
    { SCENE_SPOT00, -4808, -700, 13583, 0 },
    { SCENE_SPOT00, -3416, -490, 12092, 0 },
    { SCENE_SPOT00, -2915, 100, 8339, 0 },
    { SCENE_SPOT00, -2277, -500, 13247, 0 },
    { SCENE_SPOT00, -1026, -500, 12101, 0 },
    { SCENE_SPOT00, 1427, -500, 13341, 0 },
    { SCENE_SPOT00, -200, -486, 10205, 0 },
    { SCENE_SPOT00, -1469, 100, 7496, 0 },
    { SCENE_SPOT00, -995, 168, 5652, 0 },
    { SCENE_SPOT00, 1938, 89, 6232, 0 },
    { SCENE_SPOT00, 1387, -105, 9206, 0 },
    { SCENE_SPOT00, 1571, -223, 7701, 0 },
    { SCENE_SPOT00, 3893, -121, 7068, 0 },
    { SCENE_SPOT00, 3179, 373, 5221, 0 },
    { SCENE_SPOT00, 4678, -20, 3869, 0 },
    { SCENE_SPOT00, 3460, 246, 4207, 0 },
    { SCENE_SPOT00, 3686, 128, 2366, 0 },
    { SCENE_SPOT00, 1791, 18, 152, 0 },
    { SCENE_SPOT00, 3667, -16, 1399, 0 },
    { SCENE_SPOT00, 1827, -15, 983, 0 },
    { SCENE_SPOT00, 1574, 399, 4318, 0 },
    { SCENE_SPOT00, 716, 95, 3391, 0 },
    { SCENE_SPOT00, -1189, -41, 4739, 0 },
    { SCENE_SPOT00, -1976, -171, 4172, 0 },
    { SCENE_SPOT00, 1314, 391, 5665, 0 },
    { SCENE_SPOT00, 112, 0, 1959, 0 },
    { SCENE_SPOT00, -3011, -111, 9397, 0 },
    { SCENE_SPOT00, -5674, -270, 8585, 0 },
    { SCENE_SPOT00, -8861, -300, 7836, 0 },
    { SCENE_SPOT00, -6056, -500, 7810, 0 },
    { SCENE_SPOT00, -7306, -500, 5994, 0 },
    { SCENE_SPOT00, -7305, -500, 7605, 0 },
    { SCENE_SPOT00, -7439, -300, 7600, 0 },
    { SCENE_SPOT00, -7464, -300, 6268, 0 },
    { SCENE_SPOT00, -8080, -300, 7553, 0 },
    { SCENE_SPOT00, -8091, -300, 7349, 0 },
    { SCENE_SPOT00, -8785, -300, 7383, 0 },
    { SCENE_SPOT00, -8745, -300, 7508, 0 },
    { SCENE_SPOT00, -8777, -300, 7788, 0 },
    { SCENE_SPOT00, -8048, -299, 7738, 0 },
    { SCENE_SPOT00, -7341, -184, 7730, 0 },
    { SCENE_SPOT00, -6410, -288, 7824, 0 },
    { SCENE_SPOT00, -6326, -290, 8205, 0 },
    { SCENE_SPOT00, -6546, -292, 8400, 0 },
    { SCENE_SPOT00, -7533, -180, 8459, 0 },
    { SCENE_SPOT00, -8024, -295, 7903, 0 },
    { SCENE_SPOT00, -8078, -308, 7994, 0 },
    { SCENE_SPOT00, -9425, -287, 7696, 0 },
    { SCENE_SPOT00, -9322, -292, 7577, 0 },
    { SCENE_SPOT00, -9602, -199, 7160, 0 },
    { SCENE_SPOT00, -9307, -300, 7758, 0 },
    { SCENE_SPOT00, -9230, -300, 7642, 0 },
    { SCENE_SPOT00, -7556, -499, 8695, 0 },
    { SCENE_SPOT00, -6438, -500, 8606, 0 },
    { SCENE_SPOT00, -6078, -500, 8258, 0 },
    { SCENE_SPOT00, -6233, -500, 7613, 0 },
    { SCENE_SPOT00, -5035, -205, 7814, 0 },
    { SCENE_SPOT00, -5971, -500, 8501, 0 },
    { SCENE_SPOT00, -5724, -498, 10123, 0 },
    { SCENE_SPOT00, -5094, -392, 11106, 0 },
    { SCENE_SPOT00, -5105, -393, 11312, 0 },
    { SCENE_SPOT00, -4477, -314, 11132, 0 },
    { SCENE_SPOT00, -3867, -380, 11419, 0 },
    { SCENE_SPOT00, -2952, -500, 11944, 0 },
    { SCENE_SPOT00, -2871, -488, 11743, 0 },
    { SCENE_SPOT00, -3829, -372, 11327, 0 },
    { SCENE_SPOT00, -4439, -293, 10989, 0 },
    { SCENE_SPOT00, -5014, -381, 11086, 0 },
    { SCENE_SPOT00, -5113, -188, 10968, 0 },
    { SCENE_SPOT00, -5269, -188, 11156, 0 },
    { SCENE_SPOT00, -5596, -178, 9972, 0 },
    { SCENE_SPOT00, -5801, -288, 8518, 0 },
    { SCENE_SPOT00, -4910, -178, 7931, 0 },
    { SCENE_SPOT00, -3586, 73, 8140, 0 },
    { SCENE_SPOT00, -4487, -106, 9362, 0 },
    { SCENE_SPOT00, -4339, -112, 6412, 0 },
    { SCENE_SPOT00, -3417, 105, 8194, 0 },
    { SCENE_SPOT00, -4505, -20, 6608, 0 },
    { SCENE_SPOT00, -5038, -199, 6603, 0 },
    { SCENE_SPOT00, -4481, 1, 6448, 0 },
    { SCENE_SPOT00, -5032, -168, 6418, 0 },
    { SCENE_SPOT00, -5256, -700, 14329, 0 },
    { SCENE_SPOT00, -5749, -820, 15380, 0 },
    { SCENE_SPOT00, -3122, -700, 13608, 0 },
    { SCENE_SPOT00, -3758, -525, 13228, 0 },
    { SCENE_SPOT00, -3670, -500, 13123, 0 },
    { SCENE_SPOT00, -2924, -500, 13526, 0 },
    { SCENE_SPOT00, 1389, -115, 9370, 0 },
    { SCENE_SPOT00, 548, -116, 8889, 0 },
    { SCENE_SPOT00, -106, -107, 8530, 0 },
    { SCENE_SPOT00, -1608, 85, 7646, 0 },
    { SCENE_SPOT00, -5300, -700, 13772, 0 },
    { SCENE_SPOT00, -5114, -700, 13400, 0 },
    { SCENE_SPOT00, -4561, -700, 13700, 0 },
    { SCENE_SPOT00, -4762, -700, 14084, 0 },
    { SCENE_SPOT00, -2954, 100, 8216, 0 },
    { SCENE_SPOT00, 1460, -104, 9246, 0 },
    { SCENE_SPOT00, 629, -105, 8791, 0 },
    { SCENE_SPOT00, -10, -90, 8419, 0 },
    { SCENE_SPOT00, -1462, 100, 7504, 0 },
    { SCENE_SPOT00, -3018, -500, 12493, 0 },
    { SCENE_SPOT00, -2994, -311, 10331, 0 },
    { SCENE_SPOT00, -4006, -700, 14152, 0 },
    { SCENE_SPOT00, -4341, -500, 12743, 0 },
    { SCENE_SPOT00, -5879, -497, 6799, 0 },
    { SCENE_SPOT00, 22, -473, 10103, 0 },
    { SCENE_SPOT00, -1389, -192, 8874, 0 },
    { SCENE_SPOT00, -4, 92, 6866, 0 },
    { SCENE_SPOT00, 483, 104, 6637, 0 },
    { SCENE_SPOT00, 1580, 183, 5987, 0 },
    { SCENE_SPOT00, 1548, 308, 5077, 0 },
    { SCENE_SPOT00, 1511, 399, 4267, 0 },
    { SCENE_SPOT00, 1358, 385, 4271, 0 },
    { SCENE_SPOT00, 1379, 395, 5063, 0 },
    { SCENE_SPOT00, 1360, 394, 5870, 0 },
    { SCENE_SPOT00, 813, 283, 6194, 0 },
    { SCENE_SPOT00, -57, 101, 6743, 0 },
    { SCENE_SPOT00, 91, 325, 5143, 0 },
    { SCENE_SPOT00, 1425, -214, 7659, 0 },
    { SCENE_SPOT00, 3487, -19, 880, 0 },
    { SCENE_SPOT00, 2933, 152, 2094, 0 },
    { SCENE_SPOT00, 2888, -145, 6862, 0 },
    { SCENE_SPOT00, 1511, 67, 6471, 0 },
    { SCENE_SPOT00, 4051, -47, 1722, 0 },
    { SCENE_SPOT00, -7335, -500, 8627, 0 },
    { SCENE_SPOT00, -7728, -462, 8498, 0 },
    { SCENE_SPOT00, -7791, -446, 8832, 0 },
    { SCENE_SPOT00, -2915, -435, 11334, 0 },
    { SCENE_SPOT00, 165, -278, 3352, 0 },

    // Lake Hylia
    { SCENE_SPOT06, -2109, -882, 1724, 0 },
    { SCENE_SPOT06, -328, -1238, 2705, 0 },
    { SCENE_SPOT06, -3092, -1033, 3527, 0 },

    // Gerudo Valley
    { SCENE_SPOT09, 2687, -269, 753, 0 },
    { SCENE_SPOT09, 2066, -132, 317, 0 },
    { SCENE_SPOT09, 523, -8, 635, 0 },
    { SCENE_SPOT09, 558, 36, -323, 0 },
    { SCENE_SPOT09, 615, 51, -839, 0 },
    { SCENE_SPOT09, -614, 32, 29, 0 },
    { SCENE_SPOT09, -639, -3, 553, 0 },
    { SCENE_SPOT09, -540, 10, -889, 0 },
    { SCENE_SPOT09, -1666, 58, 378, 0 },
    { SCENE_SPOT09, -3044, 210, -648, 0 },

    // Gerudo's Fortress
    { SCENE_SPOT12, -678, 21, -623, 0 },
    { SCENE_SPOT12, 149, 333, -2499, 0 },
    { SCENE_SPOT12, 499, 581, -547, 0 },
    { SCENE_SPOT12, 3187, 1413, -3775, 0 },
    { SCENE_SPOT12, 3198, 1413, 307, 0 },
    { SCENE_SPOT12, 3380, 1413, -1200, 0 },
    { SCENE_SPOT12, -966, 1, -56, 0 },
    { SCENE_SPOT12, -966, 24, -761, 0 },
    { SCENE_SPOT12, -694, 174, -2820, 0 },

    // Lon Lon Ranch
    { SCENE_SPOT20, 1039, 0, 2051, 0 },
    { SCENE_SPOT20, -1443, 0, 1429, 0 },
    { SCENE_SPOT20, 856, 0, -918, 0 }, // Hardcoded to always load in lon lon
    { SCENE_SPOT20, 882, 0, -2256, 0 },
    { SCENE_SPOT20, -1003, 0, -755, 0 }, // Hardcoded to always load in lon lon
    { SCENE_SPOT20, -2254, 0, -629, 0 },
    { SCENE_SPOT20, 907, 0, -2336, 0 },
};

typedef struct {
    s16 zMin;
    s16 zMax;

    s16 xMin;
    s16 xMax;
    s16 xOffset;

    s16 angle;
    s16 angleRange;

    Vec3s pos;
} BridgeJumpPoint;

static BridgeJumpPoint sBridgeJumps[] = {
    { -195, -40, 225, 120, 360, -0x4000, 0x7D0, -270, -52, -117 },
    { -195, -40, -240, -120, -360, 0x4000, 0x7D0, 270, -52, -117 },
};

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 speed;
    s16 angle;
} RaceWaypoint;

typedef struct {
    s32 numWaypoints;
    RaceWaypoint* waypoints;
} RaceInfo;

static RaceWaypoint sIngoRaceWaypoints[] = {
    { 1056, 1, -1540, 11, 0x2A8D },  { 1593, 1, -985, 10, 0xFC27 },   { 1645, 1, -221, 11, 0xE891 },
    { 985, 1, 403, 10, 0xBB9C },     { -1023, 1, 354, 11, 0xA37D },   { -1679, 1, -213, 10, 0x889C },
    { -1552, 1, -1008, 11, 0x638D }, { -947, -1, -1604, 10, 0x4002 },
};

static RaceInfo sIngoRace = { 8, sIngoRaceWaypoints };
static s32 sAnimSoundFrames[] = { 0, 16 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 300, ICHAIN_STOP),
};

static u8 sResetNoInput[] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 };

static s32 sIdleAnimIds[] = { 1, 3, 0, 3, 1, 0 };

static s16 sIngoAnimations[] = { 7, 6, 2, 2, 1, 1, 0, 0, 0, 0 };

void EnHorse_CsMoveInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsJumpInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsRearingInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_WarpMoveInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsWarpRearingInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);

static EnHorseCsFunc sCutsceneInitFuncs[] = {
    NULL,
    EnHorse_CsMoveInit,
    EnHorse_CsJumpInit,
    EnHorse_CsRearingInit,
    EnHorse_WarpMoveInit,
    EnHorse_CsWarpRearingInit,
};

void EnHorse_CsMoveToPoint(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsJump(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsRearing(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsWarpMoveToPoint(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);
void EnHorse_CsWarpRearing(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action);

static EnHorseCsFunc sCutsceneActionFuncs[] = {
    NULL, EnHorse_CsMoveToPoint, EnHorse_CsJump, EnHorse_CsRearing, EnHorse_CsWarpMoveToPoint, EnHorse_CsWarpRearing,
};

typedef struct {
    s32 csAction;
    s32 csFuncIdx;
} CsActionEntry;

static CsActionEntry sCsActionTable[] = {
    { 36, 1 }, { 37, 2 }, { 38, 3 }, { 64, 4 }, { 65, 5 },
};

static RaceWaypoint sHbaWaypoints[] = {
    { 3600, 1413, -5055, 11, 0x8001 }, { 3360, 1413, -5220, 5, 0xC000 }, { 3100, 1413, -4900, 5, 0x0000 },
    { 3600, 1413, -4100, 11, 0x0000 }, { 3600, 1413, 360, 11, 0x0000 },
};

static RaceInfo sHbaInfo = { 5, sHbaWaypoints };

static EnHorseActionFunc sActionFuncs[] = {
    EnHorse_Frozen,
    EnHorse_Inactive,
    EnHorse_Idle,
    EnHorse_FollowPlayer,
    EnHorse_UpdateIngoRace,
    EnHorse_MountedIdle,
    EnHorse_MountedIdleWhinneying,
    EnHorse_MountedTurn,
    EnHorse_MountedWalk,
    EnHorse_MountedTrot,
    EnHorse_MountedGallop,
    EnHorse_MountedRearing,
    EnHorse_Stopping,
    EnHorse_Reverse,
    EnHorse_LowJump,
    EnHorse_HighJump,
    EnHorse_BridgeJump,
    EnHorse_CutsceneUpdate,
    EnHorse_UpdateHorsebackArchery,
    EnHorse_FleePlayer,
};

s32 EnHorse_BgCheckBridgeJumpPoint(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (globalCtx->sceneNum != SCENE_SPOT09) {
        return false;
    }
    if (thisv->actor.speedXZ < 12.8f) {
        return false;
    }
    if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        return false;
    }

    for (i = 0; i < 2; i++) {
        xMin = sBridgeJumps[i].xMin;
        xMax = (xMin + sBridgeJumps[i].xMax) + sBridgeJumps[i].xOffset;
        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }
        if (sBridgeJumps[i].zMin < thisv->actor.world.pos.z && thisv->actor.world.pos.z < sBridgeJumps[i].zMax) {
            if (xMin < thisv->actor.world.pos.x && thisv->actor.world.pos.x < xMax) {
                if (sBridgeJumps[i].angle - sBridgeJumps[i].angleRange < thisv->actor.world.rot.y &&
                    thisv->actor.world.rot.y < sBridgeJumps[i].angle + sBridgeJumps[i].angleRange) {
                    return true;
                }
            }
        }
    }
    return false;
}

void EnHorse_StartBridgeJump(EnHorse* thisv, GlobalContext* globalCtx);

s32 EnHorse_CheckBridgeJumps(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (thisv->actor.speedXZ < 12.8f) {
        return false;
    }

    for (i = 0; i != 2; i++) {
        xMin = sBridgeJumps[i].xMin;
        xMax = sBridgeJumps[i].xMax + xMin;

        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }

        if (sBridgeJumps[i].zMin < thisv->actor.world.pos.z && thisv->actor.world.pos.z < sBridgeJumps[i].zMax) {
            if (xMin < thisv->actor.world.pos.x && thisv->actor.world.pos.x < xMax) {
                if (sBridgeJumps[i].angle - sBridgeJumps[i].angleRange < thisv->actor.world.rot.y &&
                    thisv->actor.world.rot.y < sBridgeJumps[i].angle + sBridgeJumps[i].angleRange) {
                    thisv->bridgeJumpIdx = i;
                    EnHorse_StartBridgeJump(thisv, globalCtx);
                    return true;
                }
            }
        }
    }

    return false;
}

void EnHorse_RaceWaypointPos(RaceWaypoint* waypoints, s32 idx, Vec3f* pos) {
    pos->x = waypoints[idx].x;
    pos->y = waypoints[idx].y;
    pos->z = waypoints[idx].z;
}

void EnHorse_RotateToPoint(EnHorse* thisv, GlobalContext* globalCtx, Vec3f* pos, s16 turnAmount) {
    func_8006DD9C(&thisv->actor, pos, turnAmount);
}

void EnHorse_UpdateIngoRaceInfo(EnHorse* thisv, GlobalContext* globalCtx, RaceInfo* raceInfo) {
    Vec3f curWaypointPos;
    Vec3f prevWaypointPos;
    f32 playerDist;
    f32 sp50;
    s16 relPlayerYaw;
    f32 px;
    f32 pz;
    f32 d;
    f32 dist;
    s32 prevWaypoint;

    EnHorse_RaceWaypointPos(raceInfo->waypoints, thisv->curRaceWaypoint, &curWaypointPos);
    Math3D_RotateXZPlane(&curWaypointPos, raceInfo->waypoints[thisv->curRaceWaypoint].angle, &px, &pz, &d);
    if (((thisv->actor.world.pos.x * px) + (pz * thisv->actor.world.pos.z) + d) > 0.0f) {
        thisv->curRaceWaypoint++;
        if (thisv->curRaceWaypoint >= raceInfo->numWaypoints) {
            thisv->curRaceWaypoint = 0;
        }
    }

    EnHorse_RaceWaypointPos(raceInfo->waypoints, thisv->curRaceWaypoint, &curWaypointPos);

    prevWaypoint = thisv->curRaceWaypoint - 1;
    if (prevWaypoint < 0) {
        prevWaypoint = raceInfo->numWaypoints - 1;
    }
    EnHorse_RaceWaypointPos(raceInfo->waypoints, prevWaypoint, &prevWaypointPos);
    Math3D_PointDistToLine2D(thisv->actor.world.pos.x, thisv->actor.world.pos.z, prevWaypointPos.x, prevWaypointPos.z,
                             curWaypointPos.x, curWaypointPos.z, &dist);
    EnHorse_RotateToPoint(thisv, globalCtx, &curWaypointPos, 400);

    if (dist < 90000.0f) {
        playerDist = thisv->actor.xzDistToPlayer;
        if (playerDist < 130.0f || thisv->jntSph.elements[0].info.ocElemFlags & 2) {
            if (Math_SinS(thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y) > 0.0f) {
                thisv->actor.world.rot.y = thisv->actor.world.rot.y - 280;
            } else {
                thisv->actor.world.rot.y = thisv->actor.world.rot.y + 280;
            }
        } else if (playerDist < 300.0f) {
            if (Math_SinS(thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y) > 0.0f) {
                thisv->actor.world.rot.y = thisv->actor.world.rot.y + 280;
            } else {
                thisv->actor.world.rot.y = thisv->actor.world.rot.y - 280;
            }
        }
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }

    sp50 = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
    relPlayerYaw = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;
    if (sp50 <= 200.0f || (fabsf(Math_SinS(relPlayerYaw)) < 0.8f && Math_CosS(relPlayerYaw) > 0.0f)) {
        if (thisv->actor.speedXZ < thisv->ingoHorseMaxSpeed) {
            thisv->actor.speedXZ += 0.47f;
        } else {
            thisv->actor.speedXZ -= 0.47f;
        }
        thisv->ingoRaceFlags |= 1;
        return;
    }

    if (thisv->actor.speedXZ < raceInfo->waypoints[thisv->curRaceWaypoint].speed) {
        thisv->actor.speedXZ = thisv->actor.speedXZ + 0.4f;
    } else {
        thisv->actor.speedXZ = thisv->actor.speedXZ - 0.4f;
    }
    thisv->ingoRaceFlags &= ~0x1;
}

void EnHorse_PlayWalkingSound(EnHorse* thisv) {
    if (sAnimSoundFrames[thisv->soundTimer] < thisv->curFrame) {
        if (thisv->soundTimer == 0 && (sAnimSoundFrames[1] < thisv->curFrame)) {
            return;
        }

        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_WALK, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        if (++thisv->soundTimer > 1) {
            thisv->soundTimer = 0;
        }
    }
}

void EnHorse_PlayTrottingSound(EnHorse* thisv) {
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void EnHorse_PlayGallopingSound(EnHorse* thisv) {
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

f32 EnHorse_SlopeSpeedMultiplier(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 multiplier = 1.0f;

    if (Math_CosS(thisv->actor.shape.rot.x) < 0.939262f && Math_SinS(thisv->actor.shape.rot.x) < 0.0f) {
        multiplier = 0.7f;
    }
    return multiplier;
}

void func_80A5BB90(GlobalContext* globalCtx, Vec3f* vec, Vec3f* arg2, f32* arg3) {
    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, vec, arg2, arg3);
}

s32 func_80A5BBBC(GlobalContext* globalCtx, EnHorse* thisv, Vec3f* pos) {
    Vec3f sp24;
    f32 sp20;
    f32 eyeDist;

    func_80A5BB90(globalCtx, pos, &sp24, &sp20);
    if (fabsf(sp20) < 0.008f) {
        return false;
    }
    eyeDist = Math3D_Vec3f_DistXYZ(pos, &globalCtx->view.eye);
    return func_800314D4(globalCtx, &thisv->actor, &sp24, sp20) || eyeDist < 100.0f;
}

void EnHorse_IdleAnimSounds(EnHorse* thisv, GlobalContext* globalCtx) {
    if (thisv->animationIdx == ENHORSE_ANIM_IDLE &&
        ((thisv->curFrame > 35.0f && thisv->type == HORSE_EPONA) ||
         (thisv->curFrame > 28.0f && thisv->type == HORSE_HNI)) &&
        !(thisv->stateFlags & ENHORSE_SANDDUST_SOUND)) {
        thisv->stateFlags |= ENHORSE_SANDDUST_SOUND;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (thisv->animationIdx == ENHORSE_ANIM_REARING && thisv->curFrame > 25.0f &&
               !(thisv->stateFlags & ENHORSE_LAND2_SOUND)) {
        thisv->stateFlags |= ENHORSE_LAND2_SOUND;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

s32 EnHorse_Spawn(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 minDist = 1e38f;
    s32 spawn = false;
    f32 dist;
    s32 i;
    Player* player;
    Vec3f spawnPos;

    for (i = 0; i < 169; i++) {
        if (sHorseSpawns[i].scene == globalCtx->sceneNum) {
            player = GET_PLAYER(globalCtx);
            if (globalCtx->sceneNum != SCENE_SPOT20 ||
                //! Same flag checked twice
                (Flags_GetEventChkInf(0x18) && ((gSaveContext.eventInf[0] & 0xF) != 6 || Flags_GetEventChkInf(0x18))) ||
                // always load two spawns inside lon lon
                ((sHorseSpawns[i].pos.x == 856 && sHorseSpawns[i].pos.y == 0 && sHorseSpawns[i].pos.z == -918) ||
                 (sHorseSpawns[i].pos.x == -1003 && sHorseSpawns[i].pos.y == 0 && sHorseSpawns[i].pos.z == -755))) {

                spawnPos.x = sHorseSpawns[i].pos.x;
                spawnPos.y = sHorseSpawns[i].pos.y;
                spawnPos.z = sHorseSpawns[i].pos.z;
                dist = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &spawnPos);

                if (globalCtx->sceneNum) {}
                if (!(minDist < dist) && !func_80A5BBBC(globalCtx, thisv, &spawnPos)) {
                    minDist = dist;
                    thisv->actor.world.pos.x = sHorseSpawns[i].pos.x;
                    thisv->actor.world.pos.y = sHorseSpawns[i].pos.y;
                    thisv->actor.world.pos.z = sHorseSpawns[i].pos.z;
                    thisv->actor.prevPos = thisv->actor.world.pos;
                    thisv->actor.world.rot.y = sHorseSpawns[i].angle;
                    thisv->actor.shape.rot.y = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
                    spawn = true;
                    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->actor.world.pos,
                                                 &thisv->actor.projectedPos, &thisv->actor.projectedW);
                }
            }
        }
    }

    return spawn;
}

void EnHorse_ResetCutscene(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->cutsceneAction = -1;
    thisv->cutsceneFlags = 0;
}

void EnHorse_ResetRace(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->inRace = false;
}

s32 EnHorse_PlayerCanMove(EnHorse* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((player->stateFlags1 & 1) || func_8002DD78(GET_PLAYER(globalCtx)) == 1 || (player->stateFlags1 & 0x100000) ||
        ((thisv->stateFlags & ENHORSE_FLAG_19) && !thisv->inRace) || thisv->action == ENHORSE_ACT_HBA ||
        player->actor.flags & ACTOR_FLAG_8 || globalCtx->csCtx.state != 0) {
        return false;
    }
    return true;
}

void EnHorse_ResetHorsebackArchery(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->unk_39C = 0;
    thisv->hbaStarted = 0;
    thisv->hbaFlags = 0;
}

void EnHorse_ClearDustFlags(u16* dustFlags) {
    *dustFlags = 0;
}

void EnHorse_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnHorse* thisv = (EnHorse*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    AREG(6) = 0;
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    EnHorse_ClearDustFlags(&thisv->dustFlags);
    DREG(53) = 0;
    thisv->riderPos = thisv->actor.world.pos;
    thisv->noInputTimer = 0;
    thisv->noInputTimerMax = 0;
    thisv->riderPos.y = thisv->riderPos.y + 70.0f;

    if (DREG(4) == 0) {
        DREG(4) = 70;
    }

    if (thisv->actor.params & 0x8000) {
        thisv->actor.params &= ~0x8000;
        thisv->type = HORSE_HNI;

        if ((thisv->bankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_HNI)) < 0) {
            Actor_Kill(&thisv->actor);
            return;
        }

        do {
        } while (!Object_IsLoaded(&globalCtx->objectCtx, thisv->bankIndex));

        thisv->actor.objBankIndex = thisv->bankIndex;
        Actor_SetObjectDependency(globalCtx, &thisv->actor);
        thisv->boostSpeed = 12;
    } else {
        thisv->type = HORSE_EPONA;
        thisv->boostSpeed = 14;
    }

    // params was -1
    if (thisv->actor.params == 0x7FFF) {
        thisv->actor.params = 1;
    }

    if (globalCtx->sceneNum == SCENE_SOUKO) {
        thisv->stateFlags = ENHORSE_UNRIDEABLE;
    } else if (globalCtx->sceneNum == SCENE_SPOT12 && thisv->type == HORSE_HNI) {
        thisv->stateFlags = ENHORSE_FLAG_18 | ENHORSE_UNRIDEABLE;
    } else {
        if (thisv->actor.params == 3) {
            thisv->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP | ENHORSE_UNRIDEABLE;
        } else if (thisv->actor.params == 6) {
            thisv->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP;
            if (Flags_GetEventChkInf(0x18) || DREG(1) != 0) {
                thisv->stateFlags &= ~ENHORSE_CANT_JUMP;
                thisv->stateFlags |= ENHORSE_FLAG_26;
            } else if (gSaveContext.eventInf[0] & 0x40 && thisv->type == HORSE_HNI) {
                thisv->stateFlags |= ENHORSE_FLAG_21 | ENHORSE_FLAG_20;
            }
        } else if (thisv->actor.params == 1) {
            thisv->stateFlags = ENHORSE_FLAG_7;
        } else {
            thisv->stateFlags = 0;
        }
    }

    if (globalCtx->sceneNum == SCENE_SPOT20 && (gSaveContext.eventInf[0] & 0xF) == 6 &&
        Flags_GetEventChkInf(0x18) == 0 && !DREG(1)) {
        thisv->stateFlags |= ENHORSE_FLAG_25;
    }

    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.gravity = -3.5f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    thisv->action = ENHORSE_ACT_IDLE;
    thisv->actor.speedXZ = 0.0f;
    Collider_InitCylinder(globalCtx, &thisv->cyl1);
    Collider_SetCylinder(globalCtx, &thisv->cyl1, &thisv->actor, &sCylinderInit1);
    Collider_InitCylinder(globalCtx, &thisv->cyl2);
    Collider_SetCylinder(globalCtx, &thisv->cyl2, &thisv->actor, &sCylinderInit2);
    Collider_InitJntSph(globalCtx, &thisv->jntSph);
    Collider_SetJntSph(globalCtx, &thisv->jntSph, &thisv->actor, &sJntSphInit, &thisv->jntSphList);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, DamageTable_Get(0xB), &D_80A65F38);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    thisv->playerControlled = false;

    if ((globalCtx->sceneNum == SCENE_SPOT20) && (gSaveContext.sceneSetupIndex < 4)) {
        if (thisv->type == HORSE_HNI) {
            if (thisv->actor.world.rot.z == 0 || !IS_DAY) {
                Actor_Kill(&thisv->actor);
                return;
            }
            if (Flags_GetEventChkInf(0x18)) {
                Actor_Kill(&thisv->actor);
                return;
            }
            if (thisv->actor.world.rot.z != 5) {
                Actor_Kill(&thisv->actor);
                return;
            }
        } else if (!Flags_GetEventChkInf(0x18) && !DREG(1) && !IS_DAY) {
            Actor_Kill(&thisv->actor);
            return;
        }
    } else if (globalCtx->sceneNum == SCENE_MALON_STABLE) {
        if (IS_DAY || Flags_GetEventChkInf(0x18) || DREG(1) != 0 || !LINK_IS_ADULT) {
            Actor_Kill(&thisv->actor);
            return;
        }
        thisv->stateFlags |= ENHORSE_UNRIDEABLE;
    }

    Skin_Init(globalCtx, &thisv->skin, sSkeletonHeaders[thisv->type], sAnimationHeaders[thisv->type][ENHORSE_ANIM_IDLE]);
    thisv->animationIdx = ENHORSE_ANIM_IDLE;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
    thisv->numBoosts = 6;
    thisv->blinkTimer = thisv->boostRegenTime = 0;
    thisv->postDrawFunc = nullptr;
    EnHorse_ResetCutscene(thisv, globalCtx);
    EnHorse_ResetRace(thisv, globalCtx);
    EnHorse_ResetHorsebackArchery(thisv, globalCtx);

    if (thisv->actor.params == 2) {
        EnHorse_InitInactive(thisv);
    } else if (thisv->actor.params == 3) {
        EnHorse_InitIngoHorse(thisv);
        thisv->rider =
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_IN, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                        thisv->actor.world.pos.z, thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, 1, 1);
        if (thisv->rider == NULL) {
            __assert("thisv->race.rider != NULL", "../z_en_horse.c", 3077);
        }
        if (!(gSaveContext.eventInf[0] & 0x40)) {
            thisv->ingoHorseMaxSpeed = 12.07f;
        } else {
            thisv->ingoHorseMaxSpeed = 12.625f;
        }
    } else if (thisv->actor.params == 7) {
        EnHorse_InitCutscene(thisv, globalCtx);
    } else if (thisv->actor.params == 8) {
        EnHorse_InitHorsebackArchery(thisv);
        Interface_InitHorsebackArchery(globalCtx);
    } else if (globalCtx->sceneNum == SCENE_SPOT20 && !Flags_GetEventChkInf(0x18) && !DREG(1)) {
        EnHorse_InitFleePlayer(thisv);
    } else {
        if (globalCtx->sceneNum == SCENE_SOUKO) {
            EnHorse_ResetIdleAnimation(thisv);
        } else if (globalCtx->sceneNum == SCENE_SPOT12 && thisv->type == HORSE_HNI) {
            EnHorse_ResetIdleAnimation(thisv);
        } else {
            EnHorse_StartIdleRidable(thisv);
        }
    }
    thisv->actor.home.rot.z = thisv->actor.world.rot.z = thisv->actor.shape.rot.z = 0;
}

void EnHorse_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHorse* thisv = (EnHorse*)thisx;

    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_StopSfxByPos(&thisv->unk_21C);
    }
    Skin_Free(globalCtx, &thisv->skin);
    Collider_DestroyCylinder(globalCtx, &thisv->cyl1);
    Collider_DestroyCylinder(globalCtx, &thisv->cyl2);
    Collider_DestroyJntSph(globalCtx, &thisv->jntSph);
}

void EnHorse_RotateToPlayer(EnHorse* thisv, GlobalContext* globalCtx) {
    EnHorse_RotateToPoint(thisv, globalCtx, &GET_PLAYER(globalCtx)->actor.world.pos, 400);
    if (thisv->stateFlags & ENHORSE_OBSTACLE) {
        thisv->actor.world.rot.y += 800.0f;
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnHorse_Freeze(EnHorse* thisv) {
    if (thisv->action != ENHORSE_ACT_CS_UPDATE && thisv->action != ENHORSE_ACT_HBA) {
        if (sResetNoInput[thisv->actor.params] != 0 && thisv->actor.params != 4) {
            thisv->noInputTimer = 0;
            thisv->noInputTimerMax = 0;
        }
        thisv->prevAction = thisv->action;
        thisv->action = ENHORSE_ACT_FROZEN;
        thisv->cyl1.base.ocFlags1 &= ~OC1_ON;
        thisv->cyl2.base.ocFlags1 &= ~OC1_ON;
        thisv->jntSph.base.ocFlags1 &= ~OC1_ON;
        thisv->animationIdx = ENHORSE_ANIM_IDLE;
    }
}

void EnHorse_ChangeIdleAnimation(EnHorse* thisv, s32 arg1, f32 arg2);
void EnHorse_StartMountedIdleResetAnim(EnHorse* thisv);
void EnHorse_StartMountedIdle(EnHorse* thisv);
void EnHorse_StartGalloping(EnHorse* thisv);

void EnHorse_Frozen(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;
    thisv->noInputTimer--;
    if (thisv->noInputTimer < 0) {
        thisv->cyl1.base.ocFlags1 |= OC1_ON;
        thisv->cyl2.base.ocFlags1 |= OC1_ON;
        thisv->jntSph.base.ocFlags1 |= OC1_ON;
        if (thisv->playerControlled == true) {
            thisv->stateFlags &= ~ENHORSE_FLAG_7;
            if (thisv->actor.params == 4) {
                EnHorse_StartMountedIdleResetAnim(thisv);
            } else if (thisv->actor.params == 9) {
                thisv->actor.params = 5;
                if (globalCtx->csCtx.state != 0) {
                    EnHorse_StartMountedIdle(thisv);
                } else {
                    thisv->actor.speedXZ = 8.0f;
                    EnHorse_StartGalloping(thisv);
                }
            } else if (thisv->prevAction == 2) {
                EnHorse_StartMountedIdle(thisv);
            } else {
                EnHorse_StartMountedIdleResetAnim(thisv);
            }
            if (thisv->actor.params != 0) {
                thisv->actor.params = 0;
                return;
            }
        } else {
            if (thisv->prevAction == 5) {
                EnHorse_ChangeIdleAnimation(thisv, 0, 0);
                return;
            }
            if (thisv->prevAction == 6) {
                EnHorse_ChangeIdleAnimation(thisv, 0, 0);
                return;
            }
            EnHorse_ChangeIdleAnimation(thisv, 0, 0);
        }
    }
}

void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle);

void EnHorse_UpdateSpeed(EnHorse* thisv, GlobalContext* globalCtx, f32 brakeDecel, f32 brakeAngle, f32 minStickMag,
                         f32 decel, f32 baseSpeed, s16 turnSpeed) {
    s16* stickAnglePtr; // probably fake
    f32 stickMag;
    s16 stickAngle;
    f32 temp_f12;
    f32 traction;
    s16 turn;

    if (!EnHorse_PlayerCanMove(thisv, globalCtx)) {
        if (thisv->actor.speedXZ > 8) {
            thisv->actor.speedXZ -= decel;
        } else if (thisv->actor.speedXZ < 0) {
            thisv->actor.speedXZ = 0;
        }

        return;
    }

    stickAnglePtr = &stickAngle;

    baseSpeed *= EnHorse_SlopeSpeedMultiplier(thisv, globalCtx);
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (Math_CosS(stickAngle) <= brakeAngle) {
        thisv->actor.speedXZ -= brakeDecel;
        thisv->actor.speedXZ = thisv->actor.speedXZ < 0.0f ? 0.0f : thisv->actor.speedXZ;
        return;
    }

    if (stickMag < minStickMag) {
        thisv->stateFlags &= ~ENHORSE_BOOST;
        thisv->stateFlags &= ~ENHORSE_BOOST_DECEL;
        thisv->actor.speedXZ -= decel;
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ = 0.0f;
        }

        return;
    }

    if (thisv->stateFlags & ENHORSE_BOOST) {
        if ((16 - thisv->boostTimer) > 0) {
            thisv->actor.speedXZ =
                (EnHorse_SlopeSpeedMultiplier(thisv, globalCtx) * thisv->boostSpeed - thisv->actor.speedXZ) /
                    (16 - thisv->boostTimer) +
                thisv->actor.speedXZ;
        } else {
            thisv->actor.speedXZ = EnHorse_SlopeSpeedMultiplier(thisv, globalCtx) * thisv->boostSpeed;
        }

        if ((EnHorse_SlopeSpeedMultiplier(thisv, globalCtx) * thisv->boostSpeed) <= thisv->actor.speedXZ) {
            thisv->stateFlags &= ~ENHORSE_BOOST;
            thisv->stateFlags |= ENHORSE_BOOST_DECEL;
        }

    } else if (thisv->stateFlags & ENHORSE_BOOST_DECEL) {
        if (baseSpeed < thisv->actor.speedXZ) {
            temp_f12 = thisv->actor.speedXZ;
            thisv->actor.speedXZ = temp_f12 - 0.06f;
        } else if (thisv->actor.speedXZ < baseSpeed) {
            thisv->actor.speedXZ = baseSpeed;
            thisv->stateFlags &= ~ENHORSE_BOOST_DECEL;
        }
    } else {
        thisv->actor.speedXZ +=
            (thisv->actor.speedXZ <= baseSpeed * (1.0f / 54.0f) * stickMag ? 1.0f : -1.0f) * 50.0f * 0.01f;
        if (baseSpeed < thisv->actor.speedXZ) {
            thisv->actor.speedXZ = thisv->actor.speedXZ - decel;
            if (thisv->actor.speedXZ < baseSpeed) {
                thisv->actor.speedXZ = baseSpeed;
            }
        }
    }

    temp_f12 = *stickAnglePtr * (1 / 32236.f);
    traction = 2.2f - (thisv->actor.speedXZ * (1.0f / thisv->boostSpeed));
    turn = *stickAnglePtr * temp_f12 * temp_f12 * traction;
    turn = CLAMP(turn, -turnSpeed * traction, turnSpeed * traction);
    thisv->actor.world.rot.y += turn;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnHorse_StartMountedIdleResetAnim(EnHorse* thisv) {
    thisv->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartMountedIdle(thisv);
    thisv->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
}

void EnHorse_StartMountedIdle(EnHorse* thisv) {
    f32 curFrame;

    thisv->action = ENHORSE_ACT_MOUNTED_IDLE;
    thisv->animationIdx = ENHORSE_ANIM_IDLE;
    if ((thisv->curFrame > 35.0f && thisv->type == HORSE_EPONA) || (thisv->curFrame > 28.0f && thisv->type == HORSE_HNI)) {
        if (!(thisv->stateFlags & ENHORSE_SANDDUST_SOUND)) {
            thisv->stateFlags |= ENHORSE_SANDDUST_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    curFrame = thisv->skin.skelAnime.curFrame;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_StartReversingInterruptable(EnHorse* thisv);
void EnHorse_StartTurning(EnHorse* thisv);
void EnHorse_StartWalkingFromIdle(EnHorse* thisv);
void EnHorse_MountedIdleAnim(EnHorse* thisv);
void EnHorse_StartReversing(EnHorse* thisv);
void EnHorse_StartWalkingInterruptable(EnHorse* thisv);
void EnHorse_MountedIdleWhinney(EnHorse* thisv);
void EnHorse_StartWalking(EnHorse* thisv);

void EnHorse_MountedIdle(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 mag;
    s16 angle = 0;

    thisv->actor.speedXZ = 0;
    EnHorse_StickDirection(&thisv->curStick, &mag, &angle);
    if (mag > 10.0f && EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
        if (Math_CosS(angle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(thisv);
        } else if (Math_CosS(angle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(thisv);
        } else {
            EnHorse_StartWalkingFromIdle(thisv);
        }
    }
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_MountedIdleAnim(thisv);
    }
}

void EnHorse_MountedIdleAnim(EnHorse* thisv) {
    thisv->skin.skelAnime.curFrame = 0.0f;
    EnHorse_MountedIdleWhinney(thisv);
}

void EnHorse_MountedIdleWhinney(EnHorse* thisv) {
    f32 curFrame;

    thisv->action = ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING;
    thisv->animationIdx = ENHORSE_ANIM_WHINNEY;
    curFrame = thisv->skin.skelAnime.curFrame;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorse_MountedIdleWhinneying(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle = 0;

    thisv->actor.speedXZ = 0;
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f && EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
        if (Math_CosS(stickAngle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(thisv);
        } else if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(thisv);
        } else {
            EnHorse_StartWalkingFromIdle(thisv);
        }
    }
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_StartMountedIdleResetAnim(thisv);
    }
}

void EnHorse_StartTurning(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_TURN;
    thisv->soundTimer = 0;
    thisv->animationIdx = ENHORSE_ANIM_WALK;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTurn(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 clampedYaw;
    s16 stickAngle;

    thisv->actor.speedXZ = 0;
    EnHorse_PlayWalkingSound(thisv);
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f) {
        if (!EnHorse_PlayerCanMove(thisv, globalCtx)) {
            EnHorse_StartMountedIdleResetAnim(thisv);
        } else if (Math_CosS(stickAngle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(thisv);
        } else if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            clampedYaw = CLAMP(stickAngle, -800.0f, 800.0f);
            thisv->actor.world.rot.y = thisv->actor.world.rot.y + clampedYaw;
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        } else {
            EnHorse_StartWalkingInterruptable(thisv);
        }
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(thisv);
        } else {
            EnHorse_StartMountedIdleResetAnim(thisv);
        }
    }
}

void EnHorse_StartWalkingFromIdle(EnHorse* thisv) {
    EnHorse_StartWalkingInterruptable(thisv);

    if (!(thisv->stateFlags & ENHORSE_FLAG_8) && !(thisv->stateFlags & ENHORSE_FLAG_9)) {
        thisv->stateFlags |= ENHORSE_FLAG_9;
        thisv->waitTimer = 8;
        return;
    }
    thisv->waitTimer = 0;
}

void EnHorse_StartWalkingInterruptable(EnHorse* thisv) {
    thisv->noInputTimer = 0;
    thisv->noInputTimerMax = 0;
    EnHorse_StartWalking(thisv);
}

void EnHorse_StartWalking(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_WALK;
    thisv->soundTimer = 0;
    thisv->animationIdx = ENHORSE_ANIM_WALK;
    thisv->waitTimer = 0;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedWalkingReset(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_WALK;
    thisv->soundTimer = 0;
    thisv->animationIdx = ENHORSE_ANIM_WALK;
    thisv->waitTimer = 0;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
}

void EnHorse_StartTrotting(EnHorse* thisv);

void EnHorse_MountedWalk(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_PlayWalkingSound(thisv);
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (thisv->noInputTimerMax == 0.0f ||
        (thisv->noInputTimer > 0.0f && thisv->noInputTimer < thisv->noInputTimerMax - 20.0f)) {
        EnHorse_UpdateSpeed(thisv, globalCtx, 0.3f, -0.5f, 10.0f, 0.06f, 3.0f, 400);
    } else {
        thisv->actor.speedXZ = 3.0f;
    }

    if (thisv->actor.speedXZ == 0.0f) {
        thisv->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartMountedIdleResetAnim(thisv);
        thisv->noInputTimer = 0;
        thisv->noInputTimerMax = 0;
    } else if (thisv->actor.speedXZ > 3.0f) {
        thisv->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartTrotting(thisv);
        thisv->noInputTimer = 0;
        thisv->noInputTimerMax = 0;
    }

    if (thisv->noInputTimer > 0.0f) {
        thisv->noInputTimer--;
        if (thisv->noInputTimer <= 0.0f) {
            thisv->noInputTimerMax = 0;
        }
    }

    if (thisv->waitTimer <= 0) {
        thisv->stateFlags &= ~ENHORSE_FLAG_9;
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.75f;
        if (SkelAnime_Update(&thisv->skin.skelAnime) || thisv->actor.speedXZ == 0.0f) {
            if (thisv->noInputTimer <= 0.0f) {
                if (thisv->actor.speedXZ > 3.0f) {
                    EnHorse_StartTrotting(thisv);
                    thisv->noInputTimer = 0;
                    thisv->noInputTimerMax = 0;
                } else if ((stickMag < 10.0f) || (Math_CosS(stickAngle) <= -0.5f)) {
                    EnHorse_StartMountedIdleResetAnim(thisv);
                    thisv->noInputTimer = 0;
                    thisv->noInputTimerMax = 0;
                } else {
                    EnHorse_MountedWalkingReset(thisv);
                }
            }
        }
    } else {
        thisv->actor.speedXZ = 0.0f;
        thisv->waitTimer--;
    }
}

void EnHorse_StartTrotting(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_TROT;
    thisv->animationIdx = ENHORSE_ANIM_TROT;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTrotReset(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_TROT;
    thisv->animationIdx = ENHORSE_ANIM_TROT;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
}

void EnHorse_StartGallopingInterruptable(EnHorse* thisv);

void EnHorse_MountedTrot(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_UpdateSpeed(thisv, globalCtx, 0.3f, -0.5f, 10.0f, 0.06f, 6.0f, 400);
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (thisv->actor.speedXZ < 3.0f) {
        EnHorse_StartWalkingInterruptable(thisv);
    }

    thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.375f;
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_PlayTrottingSound(thisv);
        func_800AA000(0.0f, 60, 8, 255);
        if (thisv->actor.speedXZ >= 6.0f) {
            EnHorse_StartGallopingInterruptable(thisv);
        } else if (thisv->actor.speedXZ < 3.0f) {
            EnHorse_StartWalkingInterruptable(thisv);
        } else {
            EnHorse_MountedTrotReset(thisv);
        }
    }
}

void EnHorse_StartGallopingInterruptable(EnHorse* thisv) {
    thisv->noInputTimerMax = 0;
    thisv->noInputTimer = 0;
    EnHorse_StartGalloping(thisv);
}

void EnHorse_StartGalloping(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_GALLOP;
    thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    thisv->unk_234 = 0;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedGallopReset(EnHorse* thisv) {
    thisv->noInputTimerMax = 0;
    thisv->noInputTimer = 0;
    thisv->action = ENHORSE_ACT_MOUNTED_GALLOP;
    thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    thisv->unk_234 = 0;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
}

void EnHorse_JumpLanding(EnHorse* thisv, GlobalContext* globalCtx) {
    Vec3s* jointTable;
    f32 y;

    thisv->action = ENHORSE_ACT_MOUNTED_GALLOP;
    thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
    jointTable = thisv->skin.skelAnime.jointTable;
    y = jointTable->y;
    thisv->riderPos.y += y * 0.01f;
    thisv->postDrawFunc = NULL;
}

void EnHorse_StartBraking(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_MountedGallop(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);

    if (thisv->noInputTimer <= 0.0f) {
        EnHorse_UpdateSpeed(thisv, globalCtx, 0.3f, -0.5f, 10.0f, 0.06f, 8.0f, 0x190);
    } else if (thisv->noInputTimer > 0.0f) {
        thisv->noInputTimer -= 1;
        thisv->actor.speedXZ = 8.0f;
    }
    if (thisv->actor.speedXZ < 6.0f) {
        EnHorse_StartTrotting(thisv);
    }

    thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.3f;
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(thisv);
        func_800AA000(0, 120, 8, 255);
        if (EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
            if (stickMag >= 10.0f && Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartBraking(thisv, globalCtx);
            } else if (thisv->actor.speedXZ < 6.0f) {
                EnHorse_StartTrotting(thisv);
            } else {
                EnHorse_MountedGallopReset(thisv);
            }
            return;
        }
        EnHorse_MountedGallopReset(thisv);
    }
}

void EnHorse_StartRearing(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_MOUNTED_REARING;
    thisv->animationIdx = ENHORSE_ANIM_REARING;
    thisv->stateFlags &= ~ENHORSE_LAND2_SOUND;
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    func_800AA000(0.0f, 180, 20, 100);
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedRearing(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle;

    thisv->actor.speedXZ = 0;
    if (thisv->curFrame > 25.0f) {
        if (!(thisv->stateFlags & ENHORSE_LAND2_SOUND)) {
            thisv->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            func_800AA000(0, 180, 20, 100);
        }
    }

    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
            if (thisv->stateFlags & ENHORSE_FORCE_REVERSING) {
                thisv->noInputTimer = 100;
                thisv->noInputTimerMax = 100;
                thisv->stateFlags &= ~ENHORSE_FORCE_REVERSING;
                EnHorse_StartReversing(thisv);
            } else if (thisv->stateFlags & ENHORSE_FORCE_WALKING) {
                thisv->noInputTimer = 100;
                thisv->noInputTimerMax = 100;
                thisv->stateFlags &= ~ENHORSE_FORCE_WALKING;
                EnHorse_StartWalking(thisv);
            } else if (Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(thisv);
            } else {
                EnHorse_StartMountedIdleResetAnim(thisv);
            }
            return;
        }
        EnHorse_StartMountedIdleResetAnim(thisv);
    }
}

void EnHorse_StartBraking(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->action = ENHORSE_ACT_STOPPING;
    thisv->animationIdx = ENHORSE_ANIM_STOPPING;

    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SLIP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);

    thisv->stateFlags |= ENHORSE_STOPPING_NEIGH_SOUND;
    thisv->stateFlags &= ~ENHORSE_BOOST;
}

void EnHorse_Stopping(EnHorse* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.speedXZ > 0.0f) {
        thisv->actor.speedXZ = thisv->actor.speedXZ - 0.6f;
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ = 0.0f;
        }
    }

    if (thisv->stateFlags & ENHORSE_STOPPING_NEIGH_SOUND && thisv->skin.skelAnime.curFrame > 29.0f) {
        thisv->actor.speedXZ = 0.0f;
        if (Rand_ZeroOne() > 0.5) {
            thisv->unk_21C = thisv->unk_228;
            if (thisv->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            func_800AA000(0.0f, 180, 20, 100);
            thisv->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        } else {
            EnHorse_StartMountedIdleResetAnim(thisv);
        }
    }

    if (thisv->skin.skelAnime.curFrame > 29.0f) {
        thisv->actor.speedXZ = 0.0f;
    } else if (thisv->actor.speedXZ > 3.0f && thisv->stateFlags & ENHORSE_FORCE_REVERSING) {
        thisv->actor.speedXZ = 3.0f;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (thisv->stateFlags & ENHORSE_FORCE_REVERSING) {
            thisv->noInputTimer = 100;
            thisv->noInputTimerMax = 100;
            EnHorse_StartReversing(thisv);
            thisv->stateFlags &= ~ENHORSE_FORCE_REVERSING;
        } else {
            EnHorse_StartMountedIdleResetAnim(thisv);
        }
    }
}

void EnHorse_StartReversingInterruptable(EnHorse* thisv) {
    thisv->noInputTimerMax = 0;
    thisv->noInputTimer = 0;
    EnHorse_StartReversing(thisv);
}

void EnHorse_StartReversing(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_REVERSE;
    thisv->animationIdx = ENHORSE_ANIM_WALK;
    thisv->soundTimer = 0;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_LOOP, -3.0f);
}

void EnHorse_Reverse(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 stickMag;
    s16 stickAngle;
    s16 turnAmount;
    Player* player = GET_PLAYER(globalCtx);

    EnHorse_PlayWalkingSound(thisv);
    EnHorse_StickDirection(&thisv->curStick, &stickMag, &stickAngle);
    if (EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
        if (thisv->noInputTimerMax == 0.0f ||
            (thisv->noInputTimer > 0.0f && thisv->noInputTimer < thisv->noInputTimerMax - 20.0f)) {
            if (stickMag < 10.0f && thisv->noInputTimer <= 0.0f) {
                EnHorse_StartMountedIdleResetAnim(thisv);
                thisv->actor.speedXZ = 0.0f;
                return;
            }
            if (stickMag < 10.0f) {
                stickAngle = -0x7FFF;
            } else if (Math_CosS(stickAngle) > -0.5f) {
                thisv->noInputTimerMax = 0;
                EnHorse_StartMountedIdleResetAnim(thisv);
                thisv->actor.speedXZ = 0.0f;
                return;
            }
        } else if (stickMag < 10.0f) {
            stickAngle = -0x7FFF;
        }
    } else if (player->actor.flags & ACTOR_FLAG_8) {
        EnHorse_StartMountedIdleResetAnim(thisv);
        thisv->actor.speedXZ = 0.0f;
        return;
    } else {
        stickAngle = -0x7FFF;
    }

    thisv->actor.speedXZ = -2.0f;
    turnAmount = 0x7FFF - stickAngle;
    turnAmount = CLAMP(turnAmount, -1200.0f, 1200.0f);
    thisv->actor.world.rot.y += turnAmount;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (thisv->noInputTimer > 0.0f) {
        thisv->noInputTimer--;
        if (thisv->noInputTimer <= 0.0f) {
            thisv->noInputTimerMax = 0;
        }
    }
    thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.5f * 1.5f;
    if (SkelAnime_Update(&thisv->skin.skelAnime) && (f32)thisv->noInputTimer <= 0.0f &&
        EnHorse_PlayerCanMove(thisv, globalCtx) == true) {
        if (stickMag > 10.0f && Math_CosS(stickAngle) <= -0.5f) {
            thisv->noInputTimerMax = 0;
            EnHorse_StartReversingInterruptable(thisv);
        } else if (stickMag < 10.0f) {
            thisv->noInputTimerMax = 0;
            EnHorse_StartMountedIdleResetAnim(thisv);
        } else {
            EnHorse_StartReversing(thisv);
        }
    }
}

void EnHorse_StartLowJump(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_LowJumpInit(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartLowJump(thisv, globalCtx);
}

void EnHorse_StartLowJump(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    thisv->action = ENHORSE_ACT_LOW_JUMP;
    thisv->animationIdx = ENHORSE_ANIM_LOW_JUMP;
    curFrame = thisv->skin.skelAnime.curFrame;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);

    thisv->postDrawFunc = NULL;
    thisv->jumpStartY = thisv->actor.world.pos.y;

    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0;

    jointTable = thisv->skin.skelAnime.jointTable;
    y = jointTable->y;
    thisv->riderPos.y -= y * 0.01f;

    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_Stub1(EnHorse* thisv) {
}

void EnHorse_LowJump(EnHorse* thisv, GlobalContext* globalCtx) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = thisv->skin.skelAnime.curFrame;
    thisv->stateFlags |= ENHORSE_JUMPING;
    thisv->actor.speedXZ = 12.0f;
    if (curFrame > 17.0f) {
        thisv->actor.gravity = -3.5f;
        if (thisv->actor.velocity.y == 0) {
            thisv->actor.velocity.y = -6.0f;
        }
        if (thisv->actor.world.pos.y < thisv->actor.floorHeight + 90.0f) {
            thisv->skin.skelAnime.playSpeed = 1.5f;
        } else {
            thisv->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = thisv->skin.skelAnime.jointTable;
        y = jointTable->y;
        thisv->actor.world.pos.y = thisv->jumpStartY + y * 0.01f;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime) ||
        (curFrame > 17.0f && thisv->actor.world.pos.y < thisv->actor.floorHeight - thisv->actor.velocity.y + 80.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        thisv->stateFlags &= ~ENHORSE_JUMPING;
        thisv->actor.gravity = -3.5f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        func_80028A54(globalCtx, 25.0f, &thisv->actor.world.pos);
        EnHorse_JumpLanding(thisv, globalCtx);
    }
}

void EnHorse_StartHighJump(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_HighJumpInit(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartHighJump(thisv, globalCtx);
}

void EnHorse_StartHighJump(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    thisv->action = ENHORSE_ACT_HIGH_JUMP;
    thisv->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = thisv->skin.skelAnime.curFrame;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);

    thisv->jumpStartY = thisv->actor.world.pos.y;
    thisv->postDrawFunc = NULL;

    thisv->actor.gravity = 0;
    thisv->actor.velocity.y = 0.0f;

    jointTable = thisv->skin.skelAnime.jointTable;
    y = jointTable->y;
    thisv->riderPos.y -= y * 0.01f;

    thisv->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_Stub2(EnHorse* thisv) {
}

void EnHorse_HighJump(EnHorse* thisv, GlobalContext* globalCtx) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = thisv->skin.skelAnime.curFrame;
    thisv->stateFlags |= ENHORSE_JUMPING;
    thisv->actor.speedXZ = 13.0f;
    if (curFrame > 23.0f) {
        thisv->actor.gravity = -3.5f;
        if (thisv->actor.velocity.y == 0) {
            thisv->actor.velocity.y = -10.5f;
        }

        if (thisv->actor.world.pos.y < thisv->actor.floorHeight + 90.0f) {
            thisv->skin.skelAnime.playSpeed = 1.5f;
        } else {
            thisv->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = thisv->skin.skelAnime.jointTable;
        y = jointTable->y;
        thisv->actor.world.pos.y = thisv->jumpStartY + y * 0.01f;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime) ||
        (curFrame > 23.0f && thisv->actor.world.pos.y < thisv->actor.floorHeight - thisv->actor.velocity.y + 80.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        thisv->stateFlags &= ~ENHORSE_JUMPING;
        thisv->actor.gravity = -3.5f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        func_80028A54(globalCtx, 25.0f, &thisv->actor.world.pos);
        EnHorse_JumpLanding(thisv, globalCtx);
    }
}

void EnHorse_InitInactive(EnHorse* thisv) {
    thisv->cyl1.base.ocFlags1 &= ~OC1_ON;
    thisv->cyl2.base.ocFlags1 &= ~OC1_ON;
    thisv->jntSph.base.ocFlags1 &= ~OC1_ON;
    thisv->action = ENHORSE_ACT_INACTIVE;
    thisv->animationIdx = ENHORSE_ANIM_WALK;
    thisv->stateFlags |= ENHORSE_INACTIVE;
    thisv->followTimer = 0;
}

void EnHorse_SetFollowAnimation(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_Inactive(EnHorse* thisv, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;

    if (DREG(53) != 0 && thisv->type == HORSE_EPONA) {
        DREG(53) = 0;
        if (EnHorse_Spawn(thisv, globalCtx) != 0) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            thisv->stateFlags &= ~ENHORSE_INACTIVE;
            gSaveContext.horseData.scene = globalCtx->sceneNum;

            // Focus the camera on Epona
            Camera_SetParam(globalCtx->cameraPtrs[0], 8, thisv);
            Camera_ChangeSetting(globalCtx->cameraPtrs[0], 0x38);
            Camera_SetCameraData(globalCtx->cameraPtrs[0], 4, NULL, NULL, 0x51, 0, 0);
        }
    }
    if (!(thisv->stateFlags & ENHORSE_INACTIVE)) {
        thisv->followTimer = 0;
        EnHorse_SetFollowAnimation(thisv, globalCtx);
        thisv->actor.params = 0;
        thisv->cyl1.base.ocFlags1 |= OC1_ON;
        thisv->cyl2.base.ocFlags1 |= OC1_ON;
        thisv->jntSph.base.ocFlags1 |= OC1_ON;
    }
}

void EnHorse_PlayIdleAnimation(EnHorse* thisv, s32 anim, f32 morphFrames, f32 startFrame) {
    thisv->action = ENHORSE_ACT_IDLE;
    thisv->actor.speedXZ = 0.0f;
    if (anim != ENHORSE_ANIM_IDLE && anim != ENHORSE_ANIM_WHINNEY && anim != ENHORSE_ANIM_REARING) {
        anim = ENHORSE_ANIM_IDLE;
    }
    if (anim != thisv->animationIdx) {
        thisv->animationIdx = anim;
        if (thisv->animationIdx == ENHORSE_ANIM_IDLE) {
            thisv->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
        } else if (thisv->animationIdx == ENHORSE_ANIM_WHINNEY) {
            thisv->unk_21C = thisv->unk_228;
            if (thisv->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        } else if (thisv->animationIdx == ENHORSE_ANIM_REARING) {
            thisv->unk_21C = thisv->unk_228;
            if (thisv->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            thisv->stateFlags &= ~ENHORSE_LAND2_SOUND;
        }

        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    }
}

void EnHorse_ChangeIdleAnimation(EnHorse* thisv, s32 anim, f32 morphFrames) {
    EnHorse_PlayIdleAnimation(thisv, anim, morphFrames, thisv->curFrame);
}

void EnHorse_ResetIdleAnimation(EnHorse* thisv) {
    thisv->animationIdx = ENHORSE_ANIM_WALK; // thisv forces anim 0 to play from the beginning
    EnHorse_PlayIdleAnimation(thisv, thisv->animationIdx, 0, 0);
}

void EnHorse_StartIdleRidable(EnHorse* thisv) {
    EnHorse_ResetIdleAnimation(thisv);
    thisv->stateFlags &= ~ENHORSE_UNRIDEABLE;
}

void EnHorse_StartMovingAnimation(EnHorse* thisv, s32 arg1, f32 arg2, f32 arg3);

void EnHorse_Idle(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;
    EnHorse_IdleAnimSounds(thisv, globalCtx);

    if (DREG(53) && thisv->type == HORSE_EPONA) {
        DREG(53) = 0;
        if (!func_80A5BBBC(globalCtx, thisv, &thisv->actor.world.pos)) {
            if (EnHorse_Spawn(thisv, globalCtx)) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                thisv->followTimer = 0;
                EnHorse_SetFollowAnimation(thisv, globalCtx);
                Camera_SetParam(globalCtx->cameraPtrs[0], 8, thisv);
                Camera_ChangeSetting(globalCtx->cameraPtrs[0], 0x38);
                Camera_SetCameraData(globalCtx->cameraPtrs[0], 4, NULL, NULL, 0x51, 0, 0);
            }
        } else {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            thisv->followTimer = 0;
            EnHorse_StartMovingAnimation(thisv, 6, -3.0f, 0.0f);
        }
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        s32 idleAnimIdx = 0;

        if (thisv->animationIdx != ENHORSE_ANIM_IDLE) {
            if (thisv->animationIdx == ENHORSE_ANIM_WHINNEY) {
                idleAnimIdx = 1;
            } else if (thisv->animationIdx == ENHORSE_ANIM_REARING) {
                idleAnimIdx = 2;
            }
        }

        // Play one of the two other idle animations
        EnHorse_PlayIdleAnimation(thisv, sIdleAnimIds[(Rand_ZeroOne() > 0.5f ? 0 : 1) + idleAnimIdx * 2], 0.0f, 0.0f);
    }
}

void EnHorse_StartMovingAnimation(EnHorse* thisv, s32 animId, f32 morphFrames, f32 startFrame) {
    thisv->action = ENHORSE_ACT_FOLLOW_PLAYER;
    thisv->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
    if (animId != ENHORSE_ANIM_TROT && animId != ENHORSE_ANIM_GALLOP && animId != ENHORSE_ANIM_WALK) {
        animId = ENHORSE_ANIM_WALK;
    }
    if (thisv->animationIdx != animId) {
        thisv->animationIdx = animId;
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    } else {
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                         0.0f);
    }
}

void EnHorse_SetFollowAnimation(EnHorse* thisv, GlobalContext* globalCtx) {
    s32 animId = ENHORSE_ANIM_WALK;
    f32 distToPlayer;

    distToPlayer = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
    if (distToPlayer > 400.0f) {
        animId = ENHORSE_ANIM_GALLOP;
    } else if (!(distToPlayer <= 300.0f)) {
        if (distToPlayer <= 400.0f) {
            animId = ENHORSE_ANIM_TROT;
        }
    }

    if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else if (distToPlayer < 300.0f) {
            animId = ENHORSE_ANIM_WALK;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        if (distToPlayer > 300.0f) {
            animId = ENHORSE_ANIM_TROT;
        } else {
            animId = ENHORSE_ANIM_WALK;
        }
    }
    EnHorse_StartMovingAnimation(thisv, animId, -3.0f, 0.0f);
}

void EnHorse_FollowPlayer(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 distToPlayer;
    f32 angleDiff;

    DREG(53) = 0;
    distToPlayer = Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);

    // First rotate if the player is behind
    if ((thisv->playerDir == PLAYER_DIR_BACK_R || thisv->playerDir == PLAYER_DIR_BACK_L) &&
        (distToPlayer > 300.0f && !(thisv->stateFlags & ENHORSE_TURNING_TO_PLAYER))) {
        thisv->animationIdx = ENHORSE_ANIM_REARING;
        thisv->stateFlags |= ENHORSE_TURNING_TO_PLAYER;
        thisv->angleToPlayer = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor);
        angleDiff = (f32)thisv->angleToPlayer - (f32)thisv->actor.world.rot.y;
        if (angleDiff > 32767.f) {
            angleDiff -= 32767.0f;
        } else if (angleDiff < -32767) {
            angleDiff += 32767;
        }

        thisv->followPlayerTurnSpeed =
            angleDiff / Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]);
        Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx]);
        thisv->skin.skelAnime.playSpeed = 1.0f;
        thisv->stateFlags &= ~ENHORSE_LAND2_SOUND;
        thisv->unk_21C = thisv->unk_228;
    } else if (thisv->stateFlags & ENHORSE_TURNING_TO_PLAYER) {
        thisv->actor.world.rot.y = thisv->actor.world.rot.y + thisv->followPlayerTurnSpeed;
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        if (thisv->curFrame > 25.0f) {
            if (!(thisv->stateFlags & ENHORSE_LAND2_SOUND)) {
                thisv->stateFlags |= ENHORSE_LAND2_SOUND;
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }
        }
    } else {
        EnHorse_RotateToPlayer(thisv, globalCtx);
    }

    if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        thisv->actor.speedXZ = 8;
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.3f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        thisv->actor.speedXZ = 6;
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.375f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        thisv->actor.speedXZ = 3;
        EnHorse_PlayWalkingSound(thisv);
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.75f;
    } else {
        thisv->actor.speedXZ = 0;
        thisv->skin.skelAnime.playSpeed = 1.0f;
    }

    if (!(thisv->stateFlags & ENHORSE_TURNING_TO_PLAYER) && ++thisv->followTimer > 300) {
        EnHorse_StartIdleRidable(thisv);
        thisv->unk_21C = thisv->unk_228;

        if (thisv->stateFlags & ENHORSE_DRAW) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
            EnHorse_PlayGallopingSound(thisv);
        } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
            EnHorse_PlayTrottingSound(thisv);
        }
        thisv->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
        if (distToPlayer < 100.0f) {
            EnHorse_StartIdleRidable(thisv);
        } else {
            EnHorse_SetFollowAnimation(thisv, globalCtx);
        }
    }
}

void EnHorse_UpdateIngoHorseAnim(EnHorse* thisv);

void EnHorse_InitIngoHorse(EnHorse* thisv) {
    thisv->curRaceWaypoint = 0;
    thisv->soundTimer = 0;
    thisv->actor.speedXZ = 0.0f;
    EnHorse_UpdateIngoHorseAnim(thisv);
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_IT_INGO_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorse_SetIngoAnimation(s32 idx, f32 curFrame, s32 arg2, s16* animIdxOut, f32* curFrameOut) {
    *animIdxOut = sIngoAnimations[idx];
    *curFrameOut = curFrame;
    if (idx == 3 || idx == 7 || idx == 8 || idx == 4) {
        *curFrameOut = 0.0f;
    }
    if (arg2 == 1) {
        if (idx == 5) {
            *animIdxOut = 4;
            *curFrameOut = curFrame;
        } else if (idx == 6) {
            *animIdxOut = 3;
            *curFrameOut = curFrame;
        }
    }
}

void EnHorse_UpdateIngoHorseAnim(EnHorse* thisv) {
    s32 animChanged = 0;
    f32 animSpeed;

    thisv->action = ENHORSE_ACT_INGO_RACE;
    thisv->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
    if (thisv->actor.speedXZ == 0.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (thisv->actor.speedXZ <= 3.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_WALK;
    } else if (thisv->actor.speedXZ <= 6.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (thisv->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = thisv->actor.speedXZ * 0.5f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = thisv->actor.speedXZ * 0.25f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = thisv->actor.speedXZ * 0.2f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                         sPlaybackSpeeds[thisv->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3);
    } else {
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                         sPlaybackSpeeds[thisv->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void EnHorse_UpdateIngoRace(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 playSpeed;

    if (thisv->animationIdx == ENHORSE_ANIM_IDLE || thisv->animationIdx == ENHORSE_ANIM_WHINNEY) {
        EnHorse_IdleAnimSounds(thisv, globalCtx);
    } else if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(thisv);
    }

    EnHorse_UpdateIngoRaceInfo(thisv, globalCtx, &sIngoRace);
    if (!thisv->inRace) {
        thisv->actor.speedXZ = 0.0f;
        thisv->rider->speedXZ = 0.0f;
        if (thisv->animationIdx != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateIngoHorseAnim(thisv);
        }
    }

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = thisv->actor.speedXZ * 0.5f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = thisv->actor.speedXZ * 0.25f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = thisv->actor.speedXZ * 0.2f;
    } else {
        playSpeed = 1.0f;
    }
    thisv->skin.skelAnime.playSpeed = playSpeed;
    if (SkelAnime_Update(&thisv->skin.skelAnime) ||
        (thisv->animationIdx == ENHORSE_ANIM_IDLE && thisv->actor.speedXZ != 0.0f)) {
        EnHorse_UpdateIngoHorseAnim(thisv);
    }

    if (thisv->stateFlags & ENHORSE_INGO_WON) {
        ((EnIn*)thisv->rider)->animationIdx = 7;
        ((EnIn*)thisv->rider)->unk_1E0 = 0;
        return;
    }

    EnHorse_SetIngoAnimation(thisv->animationIdx, thisv->skin.skelAnime.curFrame, thisv->ingoRaceFlags & 1,
                             &((EnIn*)thisv->rider)->animationIdx, &((EnIn*)thisv->rider)->unk_1E0);
}

void EnHorse_CsMoveInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    thisv->cutsceneAction = 1;
    Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                               thisv->actor.speedXZ * 0.3f);
}

void EnHorse_CsPlayHighJumpAnim(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_CsMoveToPoint(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    Vec3f endPos;
    f32 speed = 8.0f;

    endPos.x = action->endPos.x;
    endPos.y = action->endPos.y;
    endPos.z = action->endPos.z;
    if (Math3D_Vec3f_DistXYZ(&endPos, &thisv->actor.world.pos) > speed) {
        EnHorse_RotateToPoint(thisv, globalCtx, &endPos, 400);
        thisv->actor.speedXZ = speed;
        thisv->skin.skelAnime.playSpeed = speed * 0.3f;
    } else {
        thisv->actor.world.pos = endPos;
        thisv->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(thisv);
        func_800AA000(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                                   thisv->actor.speedXZ * 0.3f);
    }
}

void EnHorse_CsSetAnimHighJump(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->skin.skelAnime.curFrame = 0.0f;
    EnHorse_CsPlayHighJumpAnim(thisv, globalCtx);
}

void EnHorse_CsPlayHighJumpAnim(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    f32 y;
    Vec3s* jointTable;

    thisv->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = thisv->skin.skelAnime.curFrame;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
    thisv->postDrawFunc = NULL;
    thisv->jumpStartY = thisv->actor.world.pos.y;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0;

    jointTable = thisv->skin.skelAnime.jointTable;
    y = jointTable->y;
    thisv->riderPos.y -= y * 0.01f;

    thisv->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_CsJumpInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    EnHorse_CsSetAnimHighJump(thisv, globalCtx);
    thisv->cutsceneAction = 2;
    thisv->cutsceneFlags &= ~1;
}

void EnHorse_CsJump(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    f32 temp_f2;

    if (thisv->cutsceneFlags & 1) {
        EnHorse_CsMoveToPoint(thisv, globalCtx, action);
        return;
    }
    temp_f2 = thisv->skin.skelAnime.curFrame;
    thisv->stateFlags |= ENHORSE_JUMPING;
    thisv->actor.speedXZ = 13.0f;
    if (temp_f2 > 19.0f) {
        thisv->actor.gravity = -3.5f;
        if (thisv->actor.velocity.y == 0.0f) {
            thisv->actor.velocity.y = -10.5f;
        }
        if (thisv->actor.world.pos.y < (thisv->actor.floorHeight + 90.0f)) {
            thisv->skin.skelAnime.playSpeed = 1.5f;
        } else {
            thisv->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        Vec3s* jointTable;
        f32 y;

        jointTable = thisv->skin.skelAnime.jointTable;
        y = jointTable->y;
        thisv->actor.world.pos.y = thisv->jumpStartY + y * 0.01f;
    }
    if (SkelAnime_Update(&thisv->skin.skelAnime) ||
        (temp_f2 > 19.0f && thisv->actor.world.pos.y < (thisv->actor.floorHeight - thisv->actor.velocity.y) + 80.0f)) {
        Vec3s* jointTable;
        f32 y;

        thisv->cutsceneFlags |= 1;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        thisv->stateFlags &= ~ENHORSE_JUMPING;
        thisv->actor.gravity = -3.5f;
        thisv->actor.velocity.y = 0;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        func_80028A54(globalCtx, 25.0f, &thisv->actor.world.pos);
        thisv->animationIdx = ENHORSE_ANIM_GALLOP;
        Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                                   sPlaybackSpeeds[6]);
        jointTable = thisv->skin.skelAnime.jointTable;
        y = jointTable->y;
        thisv->riderPos.y += y * 0.01f;
        thisv->postDrawFunc = NULL;
    }
}

void EnHorse_CsRearingInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->animationIdx = ENHORSE_ANIM_REARING;
    thisv->cutsceneAction = 3;
    thisv->cutsceneFlags &= ~4;
    thisv->stateFlags &= ~ENHORSE_LAND2_SOUND;
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsRearing(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->actor.speedXZ = 0.0f;
    if (thisv->curFrame > 25.0f) {
        if (!(thisv->stateFlags & ENHORSE_LAND2_SOUND)) {
            thisv->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        thisv->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(thisv->cutsceneFlags & 4)) {
            thisv->cutsceneFlags |= 4;
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), 0, 0.0f);
        }
    }
}

void EnHorse_WarpMoveInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->actor.world.pos.x = action->startPos.x;
    thisv->actor.world.pos.y = action->startPos.y;
    thisv->actor.world.pos.z = action->startPos.z;
    thisv->actor.prevPos = thisv->actor.world.pos;
    thisv->actor.world.rot.y = action->urot.y;
    thisv->actor.shape.rot = thisv->actor.world.rot;
    thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    thisv->cutsceneAction = 4;
    Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                               thisv->actor.speedXZ * 0.3f);
}

void EnHorse_CsWarpMoveToPoint(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    Vec3f endPos;
    f32 speed = 8.0f;

    endPos.x = action->endPos.x;
    endPos.y = action->endPos.y;
    endPos.z = action->endPos.z;
    if (Math3D_Vec3f_DistXYZ(&endPos, &thisv->actor.world.pos) > speed) {
        EnHorse_RotateToPoint(thisv, globalCtx, &endPos, 400);
        thisv->actor.speedXZ = speed;
        thisv->skin.skelAnime.playSpeed = speed * 0.3f;
    } else {
        thisv->actor.world.pos = endPos;
        thisv->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(thisv);
        func_800AA000(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                                   thisv->actor.speedXZ * 0.3f);
    }
}

void EnHorse_CsWarpRearingInit(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->actor.world.pos.x = action->startPos.x;
    thisv->actor.world.pos.y = action->startPos.y;
    thisv->actor.world.pos.z = action->startPos.z;
    thisv->actor.prevPos = thisv->actor.world.pos;
    thisv->actor.world.rot.y = action->urot.y;
    thisv->actor.shape.rot = thisv->actor.world.rot;
    thisv->animationIdx = ENHORSE_ANIM_REARING;
    thisv->cutsceneAction = 5;
    thisv->cutsceneFlags &= ~4;
    thisv->stateFlags &= ~ENHORSE_LAND2_SOUND;
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsWarpRearing(EnHorse* thisv, GlobalContext* globalCtx, const CsCmdActorAction* action) {
    thisv->actor.speedXZ = 0.0f;
    if (thisv->curFrame > 25.0f) {
        if (!(thisv->stateFlags & ENHORSE_LAND2_SOUND)) {
            thisv->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        thisv->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(thisv->cutsceneFlags & 4)) {
            thisv->cutsceneFlags |= 4;
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), 0, 0.0f);
        }
    }
}

void EnHorse_InitCutscene(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->playerControlled = false;
    thisv->action = ENHORSE_ACT_CS_UPDATE;
    thisv->cutsceneAction = 0;
    thisv->actor.speedXZ = 0.0f;
}

s32 EnHorse_GetCutsceneFunctionIndex(s32 csAction) {
    s32 numActions = ARRAY_COUNT(sCsActionTable); // prevents unrolling
    s32 i;

    for (i = 0; i < numActions; i++) {
        if (csAction == sCsActionTable[i].csAction) {
            return sCsActionTable[i].csFuncIdx;
        }
        if (csAction < sCsActionTable[i].csAction) {
            return 0;
        }
    }
    return 0;
}

void EnHorse_CutsceneUpdate(EnHorse* thisv, GlobalContext* globalCtx) {
    s32 csFunctionIdx;
    const CsCmdActorAction* linkCsAction = globalCtx->csCtx.linkAction;

    if (globalCtx->csCtx.state == 3) {
        thisv->playerControlled = 1;
        thisv->actor.params = 10;
        thisv->action = ENHORSE_ACT_IDLE;
        EnHorse_Freeze(thisv);
        return;
    }
    if (linkCsAction != 0 && reinterpret_cast<std::uintptr_t>(linkCsAction) != 0xABABABAB) {
        csFunctionIdx = EnHorse_GetCutsceneFunctionIndex(linkCsAction->action); 
        if (csFunctionIdx != 0) {
            if (thisv->cutsceneAction != csFunctionIdx) {
                if (thisv->cutsceneAction == 0) {
                    thisv->actor.world.pos.x = linkCsAction->startPos.x;
                    thisv->actor.world.pos.y = linkCsAction->startPos.y;
                    thisv->actor.world.pos.z = linkCsAction->startPos.z;
                    thisv->actor.world.rot.y = linkCsAction->urot.y;
                    thisv->actor.shape.rot = thisv->actor.world.rot;
                    thisv->actor.prevPos = thisv->actor.world.pos;
                }
                thisv->cutsceneAction = csFunctionIdx;
                sCutsceneInitFuncs[thisv->cutsceneAction](thisv, globalCtx, linkCsAction);
            }
            sCutsceneActionFuncs[thisv->cutsceneAction](thisv, globalCtx, linkCsAction);
        }
    }
}

s32 EnHorse_UpdateHbaRaceInfo(EnHorse* thisv, GlobalContext* globalCtx, RaceInfo* raceInfo) {
    Vec3f pos;
    f32 px;
    f32 pz;
    f32 d;

    EnHorse_RaceWaypointPos(raceInfo->waypoints, thisv->curRaceWaypoint, &pos);
    Math3D_RotateXZPlane(&pos, raceInfo->waypoints[thisv->curRaceWaypoint].angle, &px, &pz, &d);

    if (thisv->curRaceWaypoint >= raceInfo->numWaypoints - 1 &&
        Math3D_Vec3f_DistXYZ(&pos, &thisv->actor.world.pos) < DREG(8)) {
        thisv->hbaFlags |= 2;
    }

    if (((thisv->actor.world.pos.x * px) + (pz * thisv->actor.world.pos.z) + d) > 0.0f) {
        thisv->curRaceWaypoint++;
        if (thisv->curRaceWaypoint >= raceInfo->numWaypoints) {
            thisv->hbaFlags |= 1;
            return 1;
        }
    }

    if (!(thisv->hbaFlags & 1)) {
        EnHorse_RotateToPoint(thisv, globalCtx, &pos, 800);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if (thisv->actor.speedXZ < raceInfo->waypoints[thisv->curRaceWaypoint].speed && !(thisv->hbaFlags & 1)) {
        thisv->actor.speedXZ += 0.4f;
    } else {
        thisv->actor.speedXZ -= 0.4f;
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ = 0.0f;
        }
    }
    return 0;
}

void EnHorse_UpdateHbaAnim(EnHorse* thisv);

void EnHorse_InitHorsebackArchery(EnHorse* thisv) {
    thisv->hbaStarted = 0;
    thisv->soundTimer = 0;
    thisv->curRaceWaypoint = 0;
    thisv->hbaTimer = 0;
    thisv->actor.speedXZ = 0.0f;
    EnHorse_UpdateHbaAnim(thisv);
}

void EnHorse_UpdateHbaAnim(EnHorse* thisv) {
    s32 animChanged = 0;
    f32 animSpeed;

    thisv->action = ENHORSE_ACT_HBA;
    if (thisv->actor.speedXZ == 0.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (thisv->actor.speedXZ <= 3.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_WALK;
    } else if (thisv->actor.speedXZ <= 6.0f) {
        if (thisv->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (thisv->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        thisv->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = thisv->actor.speedXZ * 0.5f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = thisv->actor.speedXZ * 0.25f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        func_800AA000(0.0f, 60, 8, 255);
    } else if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = thisv->actor.speedXZ * 0.2f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        func_800AA000(0.0f, 120, 8, 255);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                         sPlaybackSpeeds[thisv->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                         -3.0f);
    } else {
        Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx],
                         sPlaybackSpeeds[thisv->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void EnHorse_UpdateHorsebackArchery(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 playSpeed;
    s32 sp20;

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(thisv);
    }
    if (globalCtx->interfaceCtx.hbaAmmo == 0) {
        thisv->hbaTimer++;
    }

    sp20 = func_800F5A58(NA_BGM_HORSE_GOAL);
    EnHorse_UpdateHbaRaceInfo(thisv, globalCtx, &sHbaInfo);
    if (thisv->hbaFlags & 1 || thisv->hbaTimer >= 46) {
        if (sp20 != 1 && gSaveContext.minigameState != 3) {
            gSaveContext.cutsceneIndex = 0;
            globalCtx->nextEntranceIndex = 0x3B0;
            globalCtx->sceneLoadFlag = 0x14;
            globalCtx->fadeTransition = 0x20;
        }
    }

    if (globalCtx->interfaceCtx.hbaAmmo != 0) {
        if (!(thisv->hbaFlags & 2)) {
            if (gSaveContext.infTable[25] & 1) {
                if ((s32)gSaveContext.minigameScore >= 1500) {
                    thisv->hbaFlags |= 4;
                }
            } else {
                if ((s32)gSaveContext.minigameScore >= 1000) {
                    thisv->hbaFlags |= 4;
                }
            }
        }
    }

    if ((globalCtx->interfaceCtx.hbaAmmo == 0) || (thisv->hbaFlags & 2)) {
        if (thisv->hbaFlags & 4) {
            thisv->hbaFlags &= ~4;
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
        }
    }

    if (!thisv->hbaStarted) {
        thisv->actor.speedXZ = 0.0f;
        if (thisv->animationIdx != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateHbaAnim(thisv);
        }
    }

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = thisv->actor.speedXZ * 0.5f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = thisv->actor.speedXZ * 0.25f;
    } else if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = thisv->actor.speedXZ * 0.2f;
    } else {
        playSpeed = 1.0f;
    }

    thisv->skin.skelAnime.playSpeed = playSpeed;
    if (SkelAnime_Update(&thisv->skin.skelAnime) ||
        (thisv->animationIdx == ENHORSE_ANIM_IDLE && thisv->actor.speedXZ != 0.0f)) {
        EnHorse_UpdateHbaAnim(thisv);
    }
}

void EnHorse_InitFleePlayer(EnHorse* thisv) {
    thisv->action = ENHORSE_ACT_FLEE_PLAYER;
    thisv->stateFlags |= ENHORSE_UNRIDEABLE;
    thisv->actor.speedXZ = 0.0f;
}

void EnHorse_FleePlayer(EnHorse* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 distToHome;
    f32 playerDistToHome;
    f32 distToPlayer;
    s32 nextAnim = thisv->animationIdx;
    s32 animFinished;
    s16 yaw;

    if (DREG(53) || thisv->type == HORSE_HNI) {
        EnHorse_StartIdleRidable(thisv);
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }

    distToHome = Math3D_Vec3f_DistXYZ(&thisv->actor.home.pos, &thisv->actor.world.pos);
    playerDistToHome = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &thisv->actor.home.pos);
    distToPlayer = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &thisv->actor.world.pos);

    // Run home
    if (playerDistToHome > 300.0f) {
        if (distToHome > 150.0f) {
            thisv->actor.speedXZ += 0.4f;
            if (thisv->actor.speedXZ > 8.0f) {
                thisv->actor.speedXZ = 8.0f;
            }
        } else {
            thisv->actor.speedXZ -= 0.47f;
            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ = 0.0f;
            }
        }
    } else {
        // Run away from Link
        if (distToPlayer < 300.0f) {
            thisv->actor.speedXZ += 0.4f;
            if (thisv->actor.speedXZ > 8.0f) {
                thisv->actor.speedXZ = 8.0f;
            }
        } else {
            thisv->actor.speedXZ -= 0.47f;
            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ = 0.0f;
            }
        }
    }

    if (thisv->actor.speedXZ >= 6.0f) { // hoof it
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.3f;
        nextAnim = ENHORSE_ANIM_GALLOP;
    } else if (thisv->actor.speedXZ >= 3.0f) { // trot
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.375f;
        nextAnim = ENHORSE_ANIM_TROT;
    } else if (thisv->actor.speedXZ > 0.1f) { // walk
        thisv->skin.skelAnime.playSpeed = thisv->actor.speedXZ * 0.75f;
        nextAnim = ENHORSE_ANIM_WALK;
        EnHorse_PlayWalkingSound(thisv);
    } else { // idle
        nextAnim = Rand_ZeroOne() > 0.5f ? 1 : 0;
        EnHorse_IdleAnimSounds(thisv, globalCtx);
        thisv->skin.skelAnime.playSpeed = 1.0f;
    }

    // Turn away from Link, or towards home
    if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
        if (playerDistToHome < 300.0f) {
            yaw = player->actor.shape.rot.y;
            yaw += (Actor_WorldYawTowardActor(&thisv->actor, &player->actor) > 0 ? 1 : -1) * 0x3FFF;
        } else {
            yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos) - thisv->actor.world.rot.y;
        }

        if (yaw > 400) {
            thisv->actor.world.rot.y += 400;
        } else if (yaw < -400) {
            thisv->actor.world.rot.y -= 400;
        } else {
            thisv->actor.world.rot.y += yaw;
        }

        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }

    animFinished = SkelAnime_Update(&thisv->skin.skelAnime);

    if (thisv->animationIdx == ENHORSE_ANIM_IDLE || thisv->animationIdx == ENHORSE_ANIM_WHINNEY) {
        if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
            thisv->animationIdx = nextAnim;
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
            if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
                EnHorse_PlayGallopingSound(thisv);
            } else if (thisv->animationIdx == ENHORSE_ANIM_TROT) {
                EnHorse_PlayTrottingSound(thisv);
            }
            return;
        }
    }

    if (animFinished) {
        if (nextAnim == ENHORSE_ANIM_GALLOP) {
            EnHorse_PlayGallopingSound(thisv);
        } else if (nextAnim == ENHORSE_ANIM_TROT) {
            EnHorse_PlayTrottingSound(thisv);
        }

        if (thisv->animationIdx == ENHORSE_ANIM_IDLE || thisv->animationIdx == ENHORSE_ANIM_WHINNEY) {
            if (nextAnim != thisv->animationIdx) {
                thisv->animationIdx = nextAnim;
                Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            } else {
                if (Rand_ZeroOne() > 0.5f) {
                    thisv->animationIdx = ENHORSE_ANIM_IDLE;
                    thisv->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
                } else {
                    thisv->animationIdx = ENHORSE_ANIM_WHINNEY;
                    thisv->unk_21C = thisv->unk_228;
                    if (thisv->stateFlags & ENHORSE_DRAW) {
                        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
                }
                Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            }
        }

        if (nextAnim != thisv->animationIdx) {
            thisv->animationIdx = nextAnim;
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             0.0f);
        }
        return;
    }

    if (thisv->animationIdx == ENHORSE_ANIM_WALK) {
        if (nextAnim == ENHORSE_ANIM_IDLE || nextAnim == ENHORSE_ANIM_WHINNEY) {
            thisv->animationIdx = nextAnim;
            Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        }
    }
}

void EnHorse_BridgeJumpInit(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 y;

    func_80028A54(globalCtx, 25.0f, &thisv->actor.world.pos);
    thisv->action = ENHORSE_ACT_BRIDGE_JUMP;
    thisv->stateFlags |= ENHORSE_JUMPING;
    thisv->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    y = thisv->skin.skelAnime.jointTable->y;
    y = y * 0.01f;
    thisv->bridgeJumpStart = thisv->actor.world.pos;
    thisv->bridgeJumpStart.y += y;
    thisv->bridgeJumpYVel =
        (((sBridgeJumps[thisv->bridgeJumpIdx].pos.y + 48.7f) - thisv->bridgeJumpStart.y) - -360.0f) / 30.0f;
    thisv->riderPos.y -= y;
    thisv->stateFlags |= ENHORSE_CALC_RIDER_POS;
    thisv->bridgeJumpRelAngle = thisv->actor.world.rot.y - sBridgeJumps[thisv->bridgeJumpIdx].angle;
    thisv->bridgeJumpTimer = 0;
    thisv->actor.gravity = 0.0f;
    thisv->actor.speedXZ = 0;
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->type][thisv->animationIdx], 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->type][thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
    thisv->unk_21C = thisv->unk_228;
    if (thisv->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
    thisv->postDrawFunc = NULL;
}

void EnHorse_StartBridgeJump(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->postDrawFunc = EnHorse_BridgeJumpInit;
    if (thisv->bridgeJumpIdx == 0) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFieldFortressCs);
        gSaveContext.cutsceneTrigger = 1;
    } else {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFortressToFieldCs);
        gSaveContext.cutsceneTrigger = 1;
    }
}

void EnHorse_BridgeJumpMove(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 interp;
    f32 timeSq;

    interp = thisv->bridgeJumpTimer / 30.0f;
    timeSq = (thisv->bridgeJumpTimer * thisv->bridgeJumpTimer);

    thisv->actor.world.pos.x =
        ((sBridgeJumps[thisv->bridgeJumpIdx].pos.x - thisv->bridgeJumpStart.x) * interp) + thisv->bridgeJumpStart.x;
    thisv->actor.world.pos.z =
        ((sBridgeJumps[thisv->bridgeJumpIdx].pos.z - thisv->bridgeJumpStart.z) * interp) + thisv->bridgeJumpStart.z;

    thisv->actor.world.pos.y =
        (thisv->bridgeJumpStart.y + (thisv->bridgeJumpYVel * thisv->bridgeJumpTimer) + (-0.4f * timeSq));

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y =
        (sBridgeJumps[thisv->bridgeJumpIdx].angle + ((1.0f - interp) * thisv->bridgeJumpRelAngle));
    thisv->skin.skelAnime.curFrame = 23.0f * interp;
    SkelAnime_Update(&thisv->skin.skelAnime);
    if (thisv->bridgeJumpTimer < 30) {
        thisv->stateFlags |= ENHORSE_FLAG_24;
    }
}

void EnHorse_CheckBridgeJumpLanding(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 8.0f;
    thisv->skin.skelAnime.playSpeed = 1.5f;
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        thisv->stateFlags &= ~ENHORSE_JUMPING;
        thisv->actor.gravity = -3.5f;
        thisv->actor.world.pos.y = sBridgeJumps[thisv->bridgeJumpIdx].pos.y;
        func_80028A54(globalCtx, 25.0f, &thisv->actor.world.pos);
        EnHorse_JumpLanding(thisv, globalCtx);
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
    }
}

void EnHorse_BridgeJump(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->bridgeJumpTimer++;
    if (thisv->bridgeJumpTimer < 30) {
        EnHorse_BridgeJumpMove(thisv, globalCtx);
        return;
    }
    EnHorse_CheckBridgeJumpLanding(thisv, globalCtx);
}

void EnHorse_Vec3fOffset(Vec3f* src, s16 yaw, f32 dist, f32 height, Vec3f* dst) {
    dst->x = src->x + Math_SinS(yaw) * dist;
    dst->y = src->y + height;
    dst->z = src->z + Math_CosS(yaw) * dist;
}

s32 EnHorse_CalcFloorHeight(EnHorse* thisv, GlobalContext* globalCtx, Vec3f* pos, CollisionPoly** floorPoly,
                            f32* floorHeight) {
    s32 bgId;
    f32 waterY;
    WaterBox* waterBox;

    *floorPoly = NULL;
    *floorHeight = BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, floorPoly, &bgId, pos);

    if (*floorHeight == BGCHECK_Y_MIN) {
        return 1; // No floor
    }

    if (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, pos->x, pos->z, &waterY, &waterBox) == 1 &&
        *floorHeight < waterY) {
        return 2; // Water
    }

    if ((*floorPoly)->normal.y * COLPOLY_NORMAL_FRAC < 0.81915206f || // cos(35 degrees)
        SurfaceType_IsHorseBlocked(&globalCtx->colCtx, *floorPoly, bgId) ||
        func_80041D4C(&globalCtx->colCtx, *floorPoly, bgId) == 7) {
        return 3; // Horse blocked surface
    }
    return 0;
}

/**
 * obstacleType:
 *  1: Water in front
 *  2: Water behind?
 *  3: ?
 *  4: Obstructed in front
 *  5: Obstructed behind
 */
void EnHorse_ObstructMovement(EnHorse* thisv, GlobalContext* globalCtx, s32 obstacleType, s32 galloping) {
    if (thisv->action == ENHORSE_ACT_CS_UPDATE || EnHorse_BgCheckBridgeJumpPoint(thisv, globalCtx)) {
        return;
    }

    thisv->actor.world.pos = thisv->lastPos;
    thisv->actor.shape.rot.y = thisv->lastYaw;
    thisv->actor.world.rot.y = thisv->lastYaw;
    thisv->stateFlags |= ENHORSE_OBSTACLE;

    if (!thisv->playerControlled) {
        if (thisv->animationIdx != ENHORSE_ANIM_REARING) {
            return;
        }
    } else if (thisv->action != ENHORSE_ACT_MOUNTED_REARING) {
        if (thisv->stateFlags & ENHORSE_JUMPING) {
            thisv->stateFlags &= ~ENHORSE_JUMPING;
            thisv->actor.gravity = -3.5f;
            thisv->actor.world.pos.y = thisv->actor.floorHeight;
        }
        if (obstacleType == 1 || obstacleType == 4) {
            thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
        } else if (obstacleType == 2 || obstacleType == 5) {
            thisv->stateFlags |= ENHORSE_FORCE_WALKING;
        }
        if (galloping == true) {
            EnHorse_StartRearing(thisv);
        }
    }
}

void EnHorse_CheckFloors(EnHorse* thisv, GlobalContext* globalCtx) {
    s32 status;
    CollisionPoly* frontFloor;
    CollisionPoly* backFloor;
    s16 floorSlope;
    Vec3f frontPos;
    Vec3f backPos;
    Vec3f pos;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 galloping = thisv->actor.speedXZ > 8;
    f32 dist;
    f32 waterHeight;
    WaterBox* waterBox;
    s32 pad;

    if (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                &waterHeight, &waterBox) == 1 &&
        thisv->actor.floorHeight < waterHeight) {
        EnHorse_ObstructMovement(thisv, globalCtx, 1, galloping);
        return;
    }

    EnHorse_Vec3fOffset(&thisv->actor.world.pos, thisv->actor.shape.rot.y, 30.0f, 60.0f, &frontPos);
    status = EnHorse_CalcFloorHeight(thisv, globalCtx, &frontPos, &frontFloor, &thisv->yFront);
    if (status == 1) {
        thisv->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(thisv, globalCtx, 4, galloping);
        return;
    }
    if (status == 2) {
        EnHorse_ObstructMovement(thisv, globalCtx, 4, galloping);
        return;
    }
    if (status == 3) {
        EnHorse_ObstructMovement(thisv, globalCtx, 4, galloping);
        return;
    }

    EnHorse_Vec3fOffset(&thisv->actor.world.pos, thisv->actor.shape.rot.y, -30.0f, 60.0f, &backPos);
    status = EnHorse_CalcFloorHeight(thisv, globalCtx, &backPos, &backFloor, &thisv->yBack);
    if (status == 1) {
        thisv->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(thisv, globalCtx, 5, galloping);
        return;
    }
    if (status == 2) {
        EnHorse_ObstructMovement(thisv, globalCtx, 5, galloping);
        return;
    }
    if (status == 3) {
        EnHorse_ObstructMovement(thisv, globalCtx, 5, galloping);
        return;
    }

    floorSlope = Math_FAtan2F(thisv->yBack - thisv->yFront, 60.0f) * (0x8000 / std::numbers::pi_v<float>);
    if (thisv->actor.floorPoly != 0) {
        nx = thisv->actor.floorPoly->normal.x * COLPOLY_NORMAL_FRAC;
        ny = thisv->actor.floorPoly->normal.y * COLPOLY_NORMAL_FRAC;
        nz = thisv->actor.floorPoly->normal.z * COLPOLY_NORMAL_FRAC;
        pos = frontPos;
        pos.y = thisv->yFront;
        dist = Math3D_DistPlaneToPos(nx, ny, nz, thisv->actor.floorPoly->dist, &pos);
        if ((frontFloor != thisv->actor.floorPoly) && (thisv->actor.speedXZ >= 0.0f)) {
            if ((!(thisv->stateFlags & ENHORSE_JUMPING) && dist < -40.0f) ||
                (thisv->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
                EnHorse_ObstructMovement(thisv, globalCtx, 4, galloping);
                return;
            }
        }

        pos = backPos;
        pos.y = thisv->yBack;
        dist = Math3D_DistPlaneToPos(nx, ny, nz, thisv->actor.floorPoly->dist, &pos);
        if (((backFloor != thisv->actor.floorPoly) && (thisv->actor.speedXZ <= 0.0f) &&
             !(thisv->stateFlags & ENHORSE_JUMPING) && (dist < -40.0f)) ||
            (thisv->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
            EnHorse_ObstructMovement(thisv, globalCtx, 5, galloping);
            return;
        }

        if (ny < 0.81915206f || // cos(35 degrees)
            SurfaceType_IsHorseBlocked(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId) ||
            func_80041D4C(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId) == 7) {
            if ((thisv->actor.speedXZ >= 0.0f)) {
                EnHorse_ObstructMovement(thisv, globalCtx, 4, galloping);
            } else {
                EnHorse_ObstructMovement(thisv, globalCtx, 5, galloping);
            }
            return;
        }

        if (thisv->stateFlags & ENHORSE_JUMPING) {
            thisv->actor.shape.rot.x = 0;
            return;
        }

        if (thisv->actor.floorHeight + 4.0f < thisv->actor.world.pos.y) {
            thisv->actor.shape.rot.x = 0;
            return;
        }

        if (fabsf(floorSlope) > 8191.0f) {
            return;
        }

        thisv->actor.shape.rot.x = floorSlope;
        thisv->actor.shape.yOffset =
            (thisv->yFront + (((thisv->yBack - thisv->yFront) * 20.0f) / 45.0f)) - thisv->actor.floorHeight;
    }
}

s32 EnHorse_GetMountSide(EnHorse* thisv, GlobalContext* globalCtx);

void EnHorse_MountDismount(EnHorse* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s32 mountSide;
    Player* player = GET_PLAYER(globalCtx);

    mountSide = EnHorse_GetMountSide(thisv, globalCtx);
    if (mountSide != 0 && !(thisv->stateFlags & ENHORSE_UNRIDEABLE) && player->rideActor == NULL) {
        Actor_SetRideActor(globalCtx, &thisv->actor, mountSide);
    }

    if (thisv->playerControlled == false && Actor_IsMounted(globalCtx, &thisv->actor) == true) {
        thisv->noInputTimer = 55;
        thisv->noInputTimerMax = 55;
        thisv->playerControlled = 1;
        EnHorse_Freeze(thisv);
    } else if (thisv->playerControlled == true && Actor_NotMounted(globalCtx, &thisv->actor) == true) {
        thisv->noInputTimer = 35;
        thisv->noInputTimerMax = 35;
        thisv->stateFlags &= ~ENHORSE_UNRIDEABLE;
        thisv->playerControlled = 0;
        EnHorse_Freeze(thisv);
    }
}

void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle) {
    f32 dist;
    f32 y;
    f32 x;

    x = curStick->x;
    y = curStick->y;
    dist = sqrtf(SQ(x) + SQ(y));

    *stickMag = dist;
    if (dist > 60.0f) {
        *stickMag = 60.0f;
    } else {
        *stickMag = *stickMag;
    }

    *angle = Math_FAtan2F(-curStick->x, curStick->y) * (32768.0f / std::numbers::pi_v<float>);
}

void EnHorse_UpdateStick(EnHorse* thisv, GlobalContext* globalCtx) {
    thisv->lastStick = thisv->curStick;
    thisv->curStick.x = globalCtx->state.input[0].rel.stick_x;
    thisv->curStick.y = globalCtx->state.input[0].rel.stick_y;
}

void EnHorse_ResolveCollision(EnHorse* thisv, GlobalContext* globalCtx, CollisionPoly* colPoly) {
    f32 dist;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 offset;

    nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
    ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
    nz = COLPOLY_GET_NORMAL(colPoly->normal.z);
    if (!(Math_CosS(thisv->actor.world.rot.y -
                    (s16)(Math_FAtan2F(colPoly->normal.x, colPoly->normal.z) * (0x8000 / std::numbers::pi_v<float>)) - 0x7FFF) <
          0.7071f)) { // cos(45 degrees)
        dist = Math3D_DistPlaneToPos(nx, ny, nz, colPoly->dist, &thisv->actor.world.pos);
        offset = (1.0f / sqrtf(SQ(nx) + SQ(nz)));
        offset = (30.0f - dist) * offset;
        thisv->actor.world.pos.x += offset * nx;
        thisv->actor.world.pos.z += offset * nz;
    }
}

void EnHorse_BgCheckSlowMoving(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 yOffset;
    Vec3f start;
    Vec3f end;
    Vec3f intersect;
    CollisionPoly* colPoly;
    s32 bgId;

    if (globalCtx->sceneNum == SCENE_SPOT20) {
        yOffset = 19.0f;
    } else {
        yOffset = 40.0f;
    }
    Math_Vec3f_Copy(&start, &thisv->actor.world.pos);
    start.y = start.y + yOffset;

    Math_Vec3f_Copy(&end, &start);
    end.x += 30.0f * Math_SinS(thisv->actor.world.rot.y);
    end.y += 30.0f * Math_SinS(-thisv->actor.shape.rot.x);
    end.z += 30.0f * Math_CosS(thisv->actor.world.rot.y);
    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &start, &end, &intersect, &colPoly, 1, 0, 0, 1, &bgId) != 0) {
        EnHorse_ResolveCollision(thisv, globalCtx, colPoly);
    }
}

void EnHorse_HighJumpInit(EnHorse* thisv, GlobalContext* globalCtx);
void EnHorse_Stub2(EnHorse* thisv);
void EnHorse_Stub1(EnHorse* thisv);

void EnHorse_UpdateBgCheckInfo(EnHorse* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f obstaclePos;
    f32 pad3;
    f32 intersectDist;
    CollisionPoly* wall = NULL;
    CollisionPoly* obstacleFloor = NULL;
    s32 bgId;
    f32 obstacleHeight;
    f32 behindObstacleHeight;
    f32 ny;
    s32 movingFast;
    s32 pad5;
    DynaPolyActor* dynaPoly;
    Vec3f intersect;
    Vec3f obstacleTop;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, globalCtx->sceneNum == SCENE_SPOT20 ? 19.0f : 40.0f, 35.0f, 100.0f,
                            29);

    if (EnHorse_BgCheckBridgeJumpPoint(thisv, globalCtx)) {
        return;
    }

    // void 0 trick required to match, but is surely not real. revisit at a later time
    if (thisv->actor.bgCheckFlags & 8 && Math_CosS(thisv->actor.wallYaw - ((void)0, thisv->actor.world).rot.y) < -0.3f) {
        if (thisv->actor.speedXZ > 4.0f) {
            thisv->actor.speedXZ -= 1.0f;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }

    if (thisv->stateFlags & ENHORSE_JUMPING || !thisv->playerControlled) {
        return;
    }

    if (thisv->actor.speedXZ < 0.0f) {
        return;
    }

    // Braking or rearing from obstacle
    if (thisv->action == ENHORSE_ACT_STOPPING || thisv->action == ENHORSE_ACT_MOUNTED_REARING) {
        return;
    }

    if (thisv->actor.speedXZ > 8.0f) {
        if (thisv->actor.speedXZ < 12.8f) {
            intersectDist = 160.0f;
            movingFast = 0;
        } else {
            intersectDist = 230.0f;
            movingFast = 1;
        }
    } else {
        EnHorse_BgCheckSlowMoving(thisv, globalCtx);
        return;
    }

    startPos = thisv->actor.world.pos;
    startPos.y += 19.0f;
    endPos = startPos;
    endPos.x += (intersectDist * Math_SinS(thisv->actor.world.rot.y));
    endPos.y += (intersectDist * Math_SinS(-thisv->actor.shape.rot.x));
    endPos.z += (intersectDist * Math_CosS(thisv->actor.world.rot.y));
    intersect = endPos;
    wall = NULL;
    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &startPos, &endPos, &intersect, &wall, 1, 0, 0, 1, &bgId) == 1) {
        intersectDist = sqrt(Math3D_Vec3fDistSq(&startPos, &intersect));
        thisv->stateFlags |= ENHORSE_OBSTACLE;
    }

    if (wall != NULL) {
        if (intersectDist < 30.0f) {
            EnHorse_ResolveCollision(thisv, globalCtx, wall);
        }
        if ((Math_CosS(thisv->actor.world.rot.y - (s16)(Math_FAtan2F(wall->normal.x, wall->normal.z) * (0x8000 / std::numbers::pi_v<float>)) -
                       0x7FFF) < 0.5f) ||
            SurfaceType_IsHorseBlocked(&globalCtx->colCtx, wall, bgId) != 0) {
            return;
        }

        // too close to jump
        if ((movingFast == false && intersectDist < 80.0f) || (movingFast == true && intersectDist < 150.0f)) {
            if (movingFast == false) {
                thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
                EnHorse_StartBraking(thisv, globalCtx);
            }
            return;
        }

        dynaPoly = DynaPoly_GetActor(&globalCtx->colCtx, bgId);
        if ((thisv->stateFlags & ENHORSE_FLAG_26) && ((dynaPoly && dynaPoly->actor.id != 0x108) || dynaPoly == 0)) {
            if (movingFast == false) {
                thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
                EnHorse_StartBraking(thisv, globalCtx);
            }
            return;
        }
    }

    // Get obstacle's height
    intersectDist += 5.0f;
    obstaclePos = startPos;
    obstaclePos.x += intersectDist * Math_SinS(thisv->actor.world.rot.y);
    obstaclePos.y = thisv->actor.world.pos.y + 120.0f;
    obstaclePos.z += intersectDist * Math_CosS(thisv->actor.world.rot.y);
    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }
    obstacleHeight = obstacleTop.y - thisv->actor.world.pos.y;
    if (thisv->actor.floorPoly == NULL || obstacleFloor == NULL) {
        return;
    }

    if (Math3D_DistPlaneToPos(thisv->actor.floorPoly->normal.x * COLPOLY_NORMAL_FRAC,
                              thisv->actor.floorPoly->normal.y * COLPOLY_NORMAL_FRAC,
                              thisv->actor.floorPoly->normal.z * COLPOLY_NORMAL_FRAC, thisv->actor.floorPoly->dist,
                              &obstacleTop) < -40.0f &&
        Math3D_DistPlaneToPos(
            obstacleFloor->normal.x * COLPOLY_NORMAL_FRAC, obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC,
            obstacleFloor->normal.z * COLPOLY_NORMAL_FRAC, obstacleFloor->dist, &thisv->actor.world.pos) > 40.0f) {
        if (movingFast == true && thisv->action != ENHORSE_ACT_STOPPING) {
            thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(thisv, globalCtx);
        }
        thisv->stateFlags |= ENHORSE_OBSTACLE;
        return;
    }

    ny = obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC;
    if (ny < 0.81915206f || // cos(35 degrees)
        (SurfaceType_IsHorseBlocked(&globalCtx->colCtx, obstacleFloor, bgId) != 0) ||
        (func_80041D4C(&globalCtx->colCtx, obstacleFloor, bgId) == 7)) {
        if (movingFast == true && thisv->action != ENHORSE_ACT_STOPPING) {
            thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(thisv, globalCtx);
        }
        return;
    }

    if (wall == NULL || obstacleTop.y < intersect.y || (thisv->stateFlags & ENHORSE_CANT_JUMP)) {
        return;
    }

    obstaclePos = startPos;
    obstaclePos.y = thisv->actor.world.pos.y + 120.0f;
    if (movingFast == false) {
        obstaclePos.x += (276.0f * Math_SinS(thisv->actor.world.rot.y));
        obstaclePos.z += (276.0f * Math_CosS(thisv->actor.world.rot.y));
    } else {
        obstaclePos.x += (390.0f * Math_SinS(thisv->actor.world.rot.y));
        obstaclePos.z += (390.0f * Math_CosS(thisv->actor.world.rot.y));
    }

    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }

    behindObstacleHeight = obstacleTop.y - thisv->actor.world.pos.y;

    if (obstacleFloor == NULL) {
        return;
    }

    ny = obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC;
    if (ny < 0.81915206f || // cos(35 degrees)
        SurfaceType_IsHorseBlocked(&globalCtx->colCtx, obstacleFloor, bgId) ||
        func_80041D4C(&globalCtx->colCtx, obstacleFloor, bgId) == 7) {
        if (movingFast == true && thisv->action != ENHORSE_ACT_STOPPING) {
            thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(thisv, globalCtx);
        }
    } else if (behindObstacleHeight < -DREG(4)) { // -70
        if (movingFast == true && thisv->action != ENHORSE_ACT_STOPPING) {
            thisv->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(thisv, globalCtx);
        }
    } else if (movingFast == false && obstacleHeight > 19.0f && obstacleHeight <= 40.0f) {
        EnHorse_Stub1(thisv);
        thisv->postDrawFunc = EnHorse_LowJumpInit;
    } else if ((movingFast == true && thisv->actor.speedXZ < 13.8f && obstacleHeight > 19.0f &&
                obstacleHeight <= 72.0f) ||
               (thisv->actor.speedXZ > 13.8f && obstacleHeight <= 112.0f)) {

        EnHorse_Stub2(thisv);
        thisv->postDrawFunc = EnHorse_HighJumpInit;
    }
}

void EnHorse_CheckBoost(EnHorse* thisx, GlobalContext* globalCtx2) {
    EnHorse* thisv = (EnHorse*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s32 pad;

    if (thisv->action == ENHORSE_ACT_MOUNTED_WALK || thisv->action == ENHORSE_ACT_MOUNTED_TROT ||
        thisv->action == ENHORSE_ACT_MOUNTED_GALLOP) {
        if (CHECK_BTN_ALL(globalCtx->state.input[0].press.button, BTN_A) && (globalCtx->interfaceCtx.unk_1EE == 8)) {
            if (!(thisv->stateFlags & ENHORSE_BOOST) && !(thisv->stateFlags & ENHORSE_FLAG_8) &&
                !(thisv->stateFlags & ENHORSE_FLAG_9)) {
                if (thisv->numBoosts > 0) {
                    func_800AA000(0.0f, 180, 20, 100);
                    thisv->stateFlags |= ENHORSE_BOOST;
                    thisv->stateFlags |= ENHORSE_FIRST_BOOST_REGEN;
                    thisv->stateFlags |= ENHORSE_FLAG_8;
                    thisv->numBoosts--;
                    thisv->boostTimer = 0;
                    if (thisv->numBoosts == 0) {
                        thisv->boostRegenTime = 140;
                        return;
                    }
                    if (thisv->type == HORSE_EPONA) {
                        if (thisv->stateFlags & ENHORSE_FIRST_BOOST_REGEN) {
                            thisv->boostRegenTime = 60;
                            thisv->stateFlags &= ~ENHORSE_FIRST_BOOST_REGEN;
                        } else {
                            thisv->boostRegenTime = 8; // Never happens
                        }
                    } else {
                        thisv->boostRegenTime = 70;
                    }
                    return;
                }
                thisv->unk_21C = thisv->unk_228;
                if (thisv->stateFlags & ENHORSE_DRAW) {
                    if (Rand_ZeroOne() < 0.1f) {
                        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
                }
            }
        }
    }
}

void EnHorse_RegenBoost(EnHorse* thisv, GlobalContext* globalCtx) {
    if (thisv->numBoosts < 6 && thisv->numBoosts > 0) {
        thisv->boostRegenTime--;
        thisv->boostTimer++;

        if (thisv->boostRegenTime <= 0) {
            thisv->numBoosts = thisv->numBoosts + 1;

            if (!EN_HORSE_CHECK_4(thisv)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CARROT_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }

            if (thisv->numBoosts < 6) {
                thisv->boostRegenTime = 11;
            }
        }
    } else if (thisv->numBoosts == 0) {
        thisv->boostRegenTime--;
        thisv->boostTimer++;

        if (thisv->boostRegenTime <= 0) {
            thisv->boostRegenTime = 0;
            thisv->numBoosts = 6;

            if (!EN_HORSE_CHECK_4(thisv)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CARROT_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    }

    if (thisv->boostTimer == 8 && Rand_ZeroOne() < 0.25f) {
        thisv->unk_21C = thisv->unk_228;
        if (thisv->stateFlags & ENHORSE_DRAW) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
    globalCtx->interfaceCtx.numHorseBoosts = thisv->numBoosts;
}

void EnHorse_UpdatePlayerDir(EnHorse* thisv, GlobalContext* globalCtx) {
    EnHorse* pad;
    s16 angle;
    f32 s;
    f32 c;

    angle = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;
    s = Math_SinS(angle);
    c = Math_CosS(angle);
    if (s > 0.8660254f) { // sin(60 degrees)
        thisv->playerDir = PLAYER_DIR_SIDE_L;
    } else if (s < -0.8660254f) { // -sin(60 degrees)
        thisv->playerDir = PLAYER_DIR_SIDE_R;
    } else {
        if (c > 0.0f) {
            if (s > 0) {
                thisv->playerDir = PLAYER_DIR_FRONT_L;
            } else {
                thisv->playerDir = PLAYER_DIR_FRONT_R;
            }
        } else {
            if (s > 0) {
                thisv->playerDir = PLAYER_DIR_BACK_L;
            } else {
                thisv->playerDir = PLAYER_DIR_BACK_R;
            }
        }
    }
}

void EnHorse_TiltBody(EnHorse* thisv, GlobalContext* globalCtx) {
    f32 speed;
    f32 rollDiff;
    s32 targetRoll;
    s16 turnVel;

    speed = thisv->actor.speedXZ / thisv->boostSpeed;
    turnVel = thisv->actor.shape.rot.y - thisv->lastYaw;
    targetRoll = -((s16)((1820.0f * speed) * (turnVel / 480.00003f)));
    rollDiff = targetRoll - thisv->actor.world.rot.z;

    if (fabsf(targetRoll) < 100.0f) {
        thisv->actor.world.rot.z = 0;
    } else if (fabsf(rollDiff) < 100.0f) {
        thisv->actor.world.rot.z = targetRoll;
    } else if (rollDiff > 0.0f) {
        thisv->actor.world.rot.z += 100;
    } else {
        thisv->actor.world.rot.z -= 100;
    }

    thisv->actor.shape.rot.z = thisv->actor.world.rot.z;
}

s32 EnHorse_UpdateConveyors(EnHorse* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 conveyorDir;

    if ((thisv->actor.floorPoly == NULL) && (thisv != (EnHorse*)player->rideActor)) {
        return 0;
    }
    conveyorDir = SurfaceType_GetConveyorDirection(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
    conveyorDir = (conveyorDir << 10) - thisv->actor.world.rot.y;
    if (conveyorDir > 800.0f) {
        thisv->actor.world.rot.y += 800.0f;
    } else if (conveyorDir < -800.0f) {
        thisv->actor.world.rot.y -= 800.0f;
    } else {
        thisv->actor.world.rot.y += conveyorDir;
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    return 1;
}

s32 EnHorse_RandInt(f32 range) {
    return Rand_ZeroOne() * range;
}

void EnHorse_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnHorse* thisv = (EnHorse*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
    Vec3f dustVel = { 0.0f, 1.0f, 0.0f };
    Player* player = GET_PLAYER(globalCtx);

    thisv->lastYaw = thisx->shape.rot.y;
    EnHorse_UpdateStick(thisv, globalCtx);
    EnHorse_UpdatePlayerDir(thisv, globalCtx);

    if (!(thisv->stateFlags & ENHORSE_INACTIVE)) {
        EnHorse_MountDismount(thisv, globalCtx);
    }

    if (thisv->stateFlags & ENHORSE_FLAG_19) {
        if (thisv->stateFlags & ENHORSE_FLAG_20 && thisv->inRace == true) {
            thisv->stateFlags &= ~ENHORSE_FLAG_20;
            EnHorse_StartRearing(thisv);
        } else if (!(thisv->stateFlags & ENHORSE_FLAG_20) && thisv->stateFlags & ENHORSE_FLAG_21 &&
                   thisv->action != ENHORSE_ACT_MOUNTED_REARING && thisv->inRace == true) {
            thisv->stateFlags &= ~ENHORSE_FLAG_21;
            EnHorse_StartRearing(thisv);
        }
    }

    sActionFuncs[thisv->action](thisv, globalCtx);
    thisv->stateFlags &= ~ENHORSE_OBSTACLE;
    thisv->curFrame = thisv->skin.skelAnime.curFrame;
    thisv->lastPos = thisx->world.pos;
    if (!(thisv->stateFlags & ENHORSE_INACTIVE)) {
        if (thisv->action == ENHORSE_ACT_MOUNTED_GALLOP || thisv->action == ENHORSE_ACT_MOUNTED_TROT ||
            thisv->action == ENHORSE_ACT_MOUNTED_WALK) {
            EnHorse_CheckBoost(thisv, globalCtx);
        }
        if (thisv->playerControlled == true) {
            EnHorse_RegenBoost(thisv, globalCtx);
        }
        Actor_MoveForward(thisx);
        if (thisv->action == ENHORSE_ACT_INGO_RACE) {
            if (thisv->rider != NULL) {
                thisv->rider->world.pos.x = thisx->world.pos.x;
                thisv->rider->world.pos.y = thisx->world.pos.y + 10.0f;
                thisv->rider->world.pos.z = thisx->world.pos.z;
                thisv->rider->shape.rot.x = thisx->shape.rot.x;
                thisv->rider->shape.rot.y = thisx->shape.rot.y;
            }
        }
        if (thisv->jntSph.elements[0].info.ocElemFlags & 2) {
            if (thisx->speedXZ > 6.0f) {
                thisx->speedXZ -= 1.0f;
            }
        }
        if (thisv->jntSph.base.acFlags & 2) {
            thisv->unk_21C = thisv->unk_228;
            if (thisv->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
        if (thisv->action != ENHORSE_ACT_INGO_RACE) {
            EnHorse_TiltBody(thisv, globalCtx);
        }
        Collider_UpdateCylinder(thisx, &thisv->cyl1);
        Collider_UpdateCylinder(thisx, &thisv->cyl2);

        // Required to match
        thisv->cyl1.dim.pos.x = thisv->cyl1.dim.pos.x + (s16)(Math_SinS(thisx->shape.rot.y) * 11.0f);
        thisv->cyl1.dim.pos.z = thisv->cyl1.dim.pos.z + (s16)(Math_CosS(thisx->shape.rot.y) * 11.0f);
        thisv->cyl2.dim.pos.x = thisv->cyl2.dim.pos.x + (s16)(Math_SinS(thisx->shape.rot.y) * -18.0f);
        thisv->cyl2.dim.pos.z = thisv->cyl2.dim.pos.z + (s16)(Math_CosS(thisx->shape.rot.y) * -18.0f);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->cyl1.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cyl1.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cyl2.base);
        if ((player->stateFlags1 & 1) && player->rideActor != NULL) {
            if (globalCtx->sceneNum != SCENE_SPOT20 ||
                (globalCtx->sceneNum == SCENE_SPOT20 && (thisx->world.pos.z < -2400.0f))) {
                EnHorse_UpdateConveyors(thisv, globalCtx);
            }
        }
        if (!(thisv->stateFlags & ENHORSE_FLAG_24)) {
            EnHorse_UpdateBgCheckInfo(thisv, globalCtx);
            EnHorse_CheckFloors(thisv, globalCtx);
            if (thisx->world.pos.y < thisv->yFront && thisx->world.pos.y < thisv->yBack) {
                if (thisv->yBack < thisv->yFront) {
                    thisx->world.pos.y = thisv->yBack;
                } else {
                    thisx->world.pos.y = thisv->yFront;
                }
            }

        } else {
            thisv->stateFlags &= ~ENHORSE_FLAG_24;
        }

        if (globalCtx->sceneNum == SCENE_SPOT09 && (gSaveContext.eventChkInf[9] & 0xF) != 0xF) {
            EnHorse_CheckBridgeJumps(thisv, globalCtx);
        }

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 70.0f;
        if ((Rand_ZeroOne() < 0.025f) && thisv->blinkTimer == 0) {
            thisv->blinkTimer++;
        } else if (thisv->blinkTimer > 0) {
            thisv->blinkTimer++;
            if (thisv->blinkTimer >= 4) {
                thisv->blinkTimer = 0;
            }
        }

        if (thisx->speedXZ == 0.0f && !(thisv->stateFlags & ENHORSE_FLAG_19)) {
            thisx->colChkInfo.mass = 0xFF;
        } else {
            thisx->colChkInfo.mass = 0xFE;
        }

        if (thisx->speedXZ >= 5.0f) {
            thisv->cyl1.base.atFlags |= 1;
        } else {
            thisv->cyl1.base.atFlags &= ~1;
        }

        if (gSaveContext.entranceIndex != 343 || gSaveContext.sceneSetupIndex != 9) {
            if (thisv->dustFlags & 1) {
                thisv->dustFlags &= ~1;
                func_800287AC(globalCtx, &thisv->frontRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (thisv->dustFlags & 2) {
                thisv->dustFlags &= ~2;
                func_800287AC(globalCtx, &thisv->frontLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (thisv->dustFlags & 4) {
                thisv->dustFlags &= ~4;
                func_800287AC(globalCtx, &thisv->backRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (thisv->dustFlags & 8) {
                thisv->dustFlags &= ~8;
                func_800287AC(globalCtx, &thisv->backLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            }
        }
        thisv->stateFlags &= ~ENHORSE_DRAW;
    }
}

s32 EnHorse_PlayerDirToMountSide(EnHorse* thisv, GlobalContext* globalCtx, Player* player) {
    if (thisv->playerDir == PLAYER_DIR_SIDE_L) {
        return -1;
    }
    if (thisv->playerDir == PLAYER_DIR_SIDE_R) {
        return 1;
    }
    return 0;
}

s32 EnHorse_MountSideCheck(EnHorse* thisv, GlobalContext* globalCtx, Player* player) {
    s32 mountSide;

    if (Actor_WorldDistXZToActor(&thisv->actor, &player->actor) > 75.0f) {
        return 0;
    } else if (fabsf(thisv->actor.world.pos.y - player->actor.world.pos.y) > 30.0f) {
        return 0;
    } else if (Math_CosS(Actor_WorldYawTowardActor(&player->actor, &thisv->actor) - player->actor.world.rot.y) <
               0.17364818f) { // cos(80 degrees)
        return 0;
    } else {
        mountSide = EnHorse_PlayerDirToMountSide(thisv, globalCtx, player);
        if (mountSide == -1) {
            return -1;
        }
        if (mountSide == 1) {
            return 1;
        }
    }
    return 0;
}

s32 EnHorse_GetMountSide(EnHorse* thisv, GlobalContext* globalCtx) {
    if (thisv->action != ENHORSE_ACT_IDLE) {
        return 0;
    }
    if ((thisv->animationIdx != ENHORSE_ANIM_IDLE) && (thisv->animationIdx != ENHORSE_ANIM_WHINNEY)) {
        return 0;
    }
    return EnHorse_MountSideCheck(thisv, globalCtx, GET_PLAYER(globalCtx));
}

void EnHorse_RandomOffset(Vec3f* src, f32 dist, Vec3f* dst) {
    dst->x = (Rand_ZeroOne() * (dist * 2.0f) + src->x) - dist;
    dst->y = (Rand_ZeroOne() * (dist * 2.0f) + src->y) - dist;
    dst->z = (Rand_ZeroOne() * (dist * 2.0f) + src->z) - dist;
}

void EnHorse_PostDraw(Actor* thisx, GlobalContext* globalCtx, Skin* skin) {
    EnHorse* thisv = (EnHorse*)thisx;
    s32 pad;
    Vec3f sp94 = { 0.0f, 0.0f, 0.0f };
    Vec3f hoofOffset = { 5.0f, -4.0f, 5.0f };
    Vec3f riderOffset = { 600.0f, -1670.0f, 0.0f };
    Vec3f sp70;
    Vec3f sp64 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp58 = { 0.0f, -1.0f, 0.0f };

    f32 frame = thisv->skin.skelAnime.curFrame;
    Vec3f center;
    Vec3f newCenter;
    s32 i;
    Vec3f sp2C;
    f32 sp28;

    if (!(thisv->stateFlags & ENHORSE_CALC_RIDER_POS)) {
        Skin_GetLimbPos(skin, 30, &riderOffset, &thisv->riderPos);
        thisv->riderPos.x = thisv->riderPos.x - thisv->actor.world.pos.x;
        thisv->riderPos.y = thisv->riderPos.y - thisv->actor.world.pos.y;
        thisv->riderPos.z = thisv->riderPos.z - thisv->actor.world.pos.z;
    } else {
        thisv->stateFlags &= ~ENHORSE_CALC_RIDER_POS;
    }

    Skin_GetLimbPos(skin, 13, &sp94, &sp2C);
    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &sp2C, &thisv->unk_228, &sp28);
    if ((thisv->animationIdx == ENHORSE_ANIM_IDLE && thisv->action != ENHORSE_ACT_FROZEN) &&
        ((frame > 40.0f && frame < 45.0f && thisv->type == HORSE_EPONA) ||
         (frame > 28.0f && frame < 33.0f && thisv->type == HORSE_HNI))) {
        if (Rand_ZeroOne() < 0.6f) {
            thisv->dustFlags |= 1;
            Skin_GetLimbPos(skin, 28, &hoofOffset, &thisv->frontRightHoof);
            thisv->frontRightHoof.y = thisv->frontRightHoof.y - 5.0f;
        }
    } else {
        if (thisv->action == ENHORSE_ACT_STOPPING) {
            if ((frame > 10.0f && frame < 13.0f) || (frame > 25.0f && frame < 33.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    thisv->dustFlags |= 2;
                    Skin_GetLimbPos(skin, 20, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &thisv->frontLeftHoof);
                }
                if (Rand_ZeroOne() < 0.7f) {
                    thisv->dustFlags |= 1;
                    Skin_GetLimbPos(skin, 28, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &thisv->frontRightHoof);
                }
            }

            if ((frame > 6.0f && frame < 10.0f) || (frame > 23.0f && frame < 29.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    thisv->dustFlags |= 8;
                    Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backLeftHoof);
                }
            }

            if ((frame > 7.0f && frame < 14.0f) || (frame > 26.0f && frame < 30.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    thisv->dustFlags |= 4;
                    Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backRightHoof);
                }
            }
        } else if (thisv->animationIdx == ENHORSE_ANIM_GALLOP) {
            if ((frame > 14.0f) && (frame < 16.0f)) {
                thisv->dustFlags |= 1;
                Skin_GetLimbPos(skin, 28, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 5.0f, &thisv->frontRightHoof);
            } else if (frame > 8.0f && frame < 10.0f) {
                thisv->dustFlags |= 2;
                Skin_GetLimbPos(skin, 20, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->frontLeftHoof);
            } else if (frame > 1.0f && frame < 3.0f) {
                thisv->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backRightHoof);
            } else if ((frame > 26.0f) && (frame < 28.0f)) {
                thisv->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backLeftHoof);
            }
        } else if (thisv->action == ENHORSE_ACT_LOW_JUMP && frame > 6.0f &&
                   Rand_ZeroOne() < 1.0f - (frame - 6.0f) * (1.0f / 17.0f)) {
            if (Rand_ZeroOne() < 0.5f) {
                thisv->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backLeftHoof);
            }
            if (Rand_ZeroOne() < 0.5f) {
                thisv->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backRightHoof);
            }
        } else if (thisv->action == ENHORSE_ACT_HIGH_JUMP && frame > 5.0f &&
                   Rand_ZeroOne() < 1.0f - (frame - 5.0f) * (1.0f / 25.0f)) {
            if (Rand_ZeroOne() < 0.5f) {
                thisv->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backLeftHoof);
            }
            if (Rand_ZeroOne() < 0.5f) {
                thisv->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backRightHoof);
            }
        } else if (thisv->action == ENHORSE_ACT_BRIDGE_JUMP && Rand_ZeroOne() < 0.5f) {
            if (Rand_ZeroOne() < 0.5f) {
                thisv->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backLeftHoof);
            } else {
                thisv->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &thisv->backRightHoof);
            }
        }
    }

    for (i = 0; i < thisv->jntSph.count; i++) {
        center.x = thisv->jntSph.elements[i].dim.modelSphere.center.x;
        center.y = thisv->jntSph.elements[i].dim.modelSphere.center.y;
        center.z = thisv->jntSph.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, thisv->jntSph.elements[i].dim.limb, &center, &newCenter);
        thisv->jntSph.elements[i].dim.worldSphere.center.x = newCenter.x;
        thisv->jntSph.elements[i].dim.worldSphere.center.y = newCenter.y;
        thisv->jntSph.elements[i].dim.worldSphere.center.z = newCenter.z;
        thisv->jntSph.elements[i].dim.worldSphere.radius =
            thisv->jntSph.elements[i].dim.modelSphere.radius * thisv->jntSph.elements[i].dim.scale;
    }

    //! @bug Setting colliders in a draw function allows for duplicate entries to be added to their respective lists
    //! under certain conditions, like when pausing and unpausing the game.
    //! Actors will draw for a couple of frames between the pauses, but some important logic updates will not occur.
    //! In the case of OC, thisv can cause unwanted effects such as a very large amount of displacement being applied to
    //! a colliding actor.
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->jntSph.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->jntSph.base);
}

// unused
static s32 D_80A667DC[] = { 0, 3, 7, 14 };

s32 EnHorse_OverrideLimbDraw(Actor* thisx, GlobalContext* globalCtx, s32 limbIndex, Skin* arg3) {
    static const void* eyeTextures[] = {
        gEponaEyeOpenTex,
        gEponaEyeHalfTex,
        gEponaEyeClosedTex,
    };
    static u8 eyeBlinkIndexes[] = { 0, 1, 2, 1 };
    EnHorse* thisv = (EnHorse*)thisx;
    s32 drawOriginalLimb = true;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_horse.c", 8582);
    if (limbIndex == 13 && thisv->type == HORSE_EPONA) {
        u8 index = eyeBlinkIndexes[thisv->blinkTimer];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[index]));
    } else if (thisv->type == HORSE_HNI && thisv->stateFlags & ENHORSE_FLAG_18 && limbIndex == 30) {
        Skin_DrawLimb(globalCtx->state.gfxCtx, &thisv->skin, limbIndex, gHorseIngoGerudoSaddleDL, 0);
        drawOriginalLimb = false;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_horse.c", 8601);
    return drawOriginalLimb;
}

void EnHorse_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHorse* thisv = (EnHorse*)thisx;

    if (!(thisv->stateFlags & ENHORSE_INACTIVE)) {
        func_80093D18(globalCtx->state.gfxCtx);
        thisv->stateFlags |= ENHORSE_DRAW;
        if (thisv->stateFlags & ENHORSE_JUMPING) {
            func_800A6360(thisx, globalCtx, &thisv->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, false);
        } else {
            func_800A6360(thisx, globalCtx, &thisv->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, true);
        }
        if (thisv->postDrawFunc != NULL) {
            thisv->postDrawFunc(thisv, globalCtx);
        }
    }
}
