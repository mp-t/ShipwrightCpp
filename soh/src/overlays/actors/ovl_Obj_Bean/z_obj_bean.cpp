/*
 * File: z_obj_bean.c
 * Overlay: ovl_Obj_Bean
 * Description: Bean plant spot
 */

#include "z_obj_bean.h"
#include "objects/object_mamenoki/object_mamenoki.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_22

void ObjBean_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjBean_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjBean_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjBean_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjBean_WaitForPlayer(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_Fly(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupFly(ObjBean* thisv);
void ObjBean_WaitForWater(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupWaitForWater(ObjBean* thisv);
void ObjBean_SetupGrowWaterPhase1(ObjBean* thisv);
void ObjBean_GrowWaterPhase1(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_GrowWaterPhase2(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupGrowWaterPhase2(ObjBean* thisv);
void ObjBean_GrowWaterPhase3(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupGrowWaterPhase3(ObjBean* thisv);
void ObjBean_SetupGrown(ObjBean* thisv);
void ObjBean_FlattenLeaves(ObjBean* thisv);
void ObjBean_Grown(ObjBean* thisv);
void ObjBean_LeavesStill(ObjBean* thisv);
void ObjBean_SetupShakeLeaves(ObjBean* thisv);
void ObjBean_ShakeLeaves(ObjBean* thisv);
void ObjBean_SetupWaitForBean(ObjBean* thisv);
void ObjBean_WaitForBean(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B8FE3C(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B8FE00(ObjBean* thisv);
void func_80B8FE6C(ObjBean* thisv);
void func_80B8FEAC(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B8FF50(ObjBean* thisv);
void ObjBean_SetupGrowWaterPhase4(ObjBean* thisv);
void func_80B8FF8C(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B90050(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B90010(ObjBean* thisv);
void func_80B908EC(ObjBean* thisv);
void func_80B90918(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B90970(ObjBean* thisv);
void func_80B909B0(ObjBean* thisv, GlobalContext* globalCtx);
void func_80B909F8(ObjBean* thisv);
void func_80B90A34(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupWaitForPlayer(ObjBean* thisv);
void ObjBean_GrowWaterPhase4(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_GrowWaterPhase5(ObjBean* thisv, GlobalContext* globalCtx);
void ObjBean_SetupGrowWaterPhase5(ObjBean* thisv);
void ObjBean_SetupShakeLeavesFast(ObjBean* thisv);
void ObjBean_ShakeLeavesFast(ObjBean* thisv);
void ObjBean_Grow(ObjBean* thisv);
void ObjBean_SetupGrow(ObjBean* thisv);
void ObjBean_SetupWaitForStepOff(ObjBean* thisv);
void ObjBean_WaitForStepOff(ObjBean* thisv, GlobalContext* globalCtx);

#define BEAN_STATE_DRAW_LEAVES (1 << 0)
#define BEAN_STATE_DRAW_SOIL (1 << 1)
#define BEAN_STATE_DRAW_PLANT (1 << 2)
#define BEAN_STATE_DRAW_STALK (1 << 3)
#define BEAN_STATE_COLLIDER_SET (1 << 4)
#define BEAN_STATE_DYNAPOLY_SET (1 << 5)
#define BEAN_STATE_BEEN_WATERED (1 << 6)
#define BEAN_STATE_PLAYER_ON_TOP (1 << 7)

static ObjBean* D_80B90E30 = NULL;

ActorInit Obj_Bean_InitVars = {
    ACTOR_OBJ_BEAN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MAMENOKI,
    sizeof(ObjBean),
    (ActorFunc)ObjBean_Init,
    (ActorFunc)ObjBean_Destroy,
    (ActorFunc)ObjBean_Update,
    (ActorFunc)ObjBean_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 64, 30, -31, { 0, 0, 0 } },
};

typedef struct {
    f32 velocity;
    f32 accel;
} BeenSpeedInfo;

static BeenSpeedInfo sBeanSpeeds[] = {
    { 3.0f, 0.3f },
    { 10.0f, 0.5f },
    { 30.0f, 0.5f },
    { 3.0f, 0.3f },
};

static const Gfx* sBreakDlists[] = { gCuttableShrubStalkDL, gCuttableShrubTipDL };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1600, ICHAIN_STOP),
};

void ObjBean_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    ObjBean* thisv = (ObjBean*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->collider);
}

void ObjBean_InitDynaPoly(ObjBean* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader;
    s32 pad2;

    colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_bean.c", 374,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void ObjBean_FindFloor(ObjBean* thisv, GlobalContext* globalCtx) {
    Vec3f vec;
    s32 sp20;

    vec.x = thisv->dyna.actor.world.pos.x;
    vec.y = thisv->dyna.actor.world.pos.y + 29.999998f;
    vec.z = thisv->dyna.actor.world.pos.z;
    thisv->dyna.actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->dyna.actor.floorPoly, &sp20, &thisv->dyna.actor, &vec);
}

void func_80B8EBC8(ObjBean* thisv) {
    thisv->unk_1B6.x = thisv->unk_1B6.y = thisv->unk_1B6.z = 0;
    thisv->unk_1E4 = 0.0f;
}

void ObjBean_UpdatePosition(ObjBean* thisv) {
    f32 temp_f20;

    thisv->unk_1B6.x += 0xB6;
    thisv->unk_1B6.y += 0xFB;
    thisv->unk_1B6.z += 0x64;

    Math_StepToF(&thisv->unk_1E4, 2.0f, 0.1f);
    temp_f20 = Math_SinS(thisv->unk_1B6.x * 3);
    thisv->posOffsetX = (Math_SinS(((thisv->unk_1B6.y * 3))) + temp_f20) * thisv->unk_1E4;
    temp_f20 = Math_CosS(thisv->unk_1B6.x * 4);
    thisv->posOffsetZ = (Math_CosS((thisv->unk_1B6.y * 4)) + temp_f20) * thisv->unk_1E4;
    temp_f20 = Math_SinS(thisv->unk_1B6.z * 5);

    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z =
        ((Math_SinS((thisv->unk_1B6.y * 8)) * 0.01f) + (temp_f20 * 0.06f) + 1.07f) * 0.1f;

    thisv->dyna.actor.scale.y = ((Math_CosS(((thisv->unk_1B6.z * 10))) * 0.2f) + 1.0f) * 0.1f;
    temp_f20 = Math_SinS(thisv->unk_1B6.x * 3);
    thisv->dyna.actor.shape.rot.y =
        (Math_SinS((s16)(thisv->unk_1B6.z * 2)) * 2100.0f) + ((f32)thisv->dyna.actor.home.rot.y + (temp_f20 * 1000.0f));
}

void func_80B8EDF4(ObjBean* thisv) {
    thisv->unk_1B6.x = thisv->unk_1B6.y = thisv->unk_1B6.z = 0;

    Actor_SetScale(&thisv->dyna.actor, 0.0f);
}

void func_80B8EE24(ObjBean* thisv) {
    thisv->unk_1B6.x += 0x384;
    if (thisv->unk_1B6.x > 0x5FFF) {
        thisv->unk_1B6.x = 0x5FFF;
    }
    thisv->unk_1B6.y += 0x258;
    if (thisv->unk_1B6.y > 0x4000) {
        thisv->unk_1B6.y = 0x4000;
    }
    thisv->dyna.actor.scale.y = Math_SinS(thisv->unk_1B6.x) * 0.16970563f;

    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_SinS(thisv->unk_1B6.y) * 0.10700001f;

    Math_StepToF(&thisv->posOffsetX, 0.0f, 0.1f);
    Math_StepToF(&thisv->posOffsetZ, 0.0f, 0.1f);
    Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.y, thisv->dyna.actor.home.rot.y, 0x64);
}

void ObjBean_Move(ObjBean* thisv) {
    thisv->dyna.actor.world.pos.x = thisv->pathPoints.x + thisv->posOffsetX;
    thisv->dyna.actor.world.pos.y = thisv->pathPoints.y;
    thisv->dyna.actor.world.pos.z = thisv->pathPoints.z + thisv->posOffsetZ;
}

void ObjBean_SetDrawMode(ObjBean* thisv, u8 drawFlag) {
    thisv->stateFlags &=
        ~(BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_PLANT | BEAN_STATE_DRAW_STALK | BEAN_STATE_DRAW_SOIL);
    thisv->stateFlags |= drawFlag;
}

void ObjBean_SetupPathCount(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->pathCount = globalCtx->setupPathList[(thisv->dyna.actor.params >> 8) & 0x1F].count - 1;
    thisv->currentPointIndex = 0;
    thisv->nextPointIndex = 1;
}

void ObjBean_SetupPath(ObjBean* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[(thisv->dyna.actor.params >> 8) & 0x1F];
    Math_Vec3s_ToVec3f(&thisv->pathPoints, SEGMENTED_TO_VIRTUAL(path->points));
}

void ObjBean_FollowPath(ObjBean* thisv, GlobalContext* globalCtx) {
    Path* path;
    Vec3f acell;
    Vec3f pathPointsFloat;
    f32 speed;
    Vec3s* nextPathPoint;
    Vec3s* currentPoint;
    Vec3s* sp4C;
    Vec3f sp40;
    Vec3f sp34;
    f32 sp30;
    f32 mag;

    Math_StepToF(&thisv->dyna.actor.speedXZ, sBeanSpeeds[thisv->unk_1F6].velocity, sBeanSpeeds[thisv->unk_1F6].accel);
    path = &globalCtx->setupPathList[(thisv->dyna.actor.params >> 8) & 0x1F];
    nextPathPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[thisv->nextPointIndex];

    Math_Vec3s_ToVec3f(&pathPointsFloat, nextPathPoint);

    Math_Vec3f_Diff(&pathPointsFloat, &thisv->pathPoints, &acell);
    mag = Math3D_Vec3fMagnitude(&acell);
    speed = CLAMP_MIN(thisv->dyna.actor.speedXZ, 0.5f);
    if (speed > mag) {
        currentPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[thisv->currentPointIndex];

        Math_Vec3f_Copy(&thisv->pathPoints, &pathPointsFloat);
        thisv->currentPointIndex = thisv->nextPointIndex;

        if (thisv->pathCount <= thisv->currentPointIndex) {
            thisv->nextPointIndex = 0;
        } else {
            thisv->nextPointIndex++;
        }
        sp4C = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[thisv->nextPointIndex];
        Math_Vec3s_DiffToVec3f(&sp40, nextPathPoint, currentPoint);
        Math_Vec3s_DiffToVec3f(&sp34, sp4C, nextPathPoint);
        if (Math3D_CosOut(&sp40, &sp34, &sp30)) {
            thisv->dyna.actor.speedXZ = 0.0f;
        } else {
            thisv->dyna.actor.speedXZ *= (sp30 + 1.0f) * 0.5f;
        }
    } else {
        Math_Vec3f_Scale(&acell, thisv->dyna.actor.speedXZ / mag);
        thisv->pathPoints.x += acell.x;
        thisv->pathPoints.y += acell.y;
        thisv->pathPoints.z += acell.z;
    }
}

s32 ObjBean_CheckForHorseTrample(ObjBean* thisv, GlobalContext* globalCtx) {
    Actor* currentActor = globalCtx->actorCtx.actorLists[ACTORCAT_BG].head;

    while (currentActor != NULL) {
        if ((currentActor->id == ACTOR_EN_HORSE) &&
            (Math3D_Vec3fDistSq(&currentActor->world.pos, &thisv->dyna.actor.world.pos) < 10000.0f)) {
            return true;
        }
        currentActor = currentActor->next;
    }

    return false;
}

void ObjBean_Break(ObjBean* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f20;
    s16 angle;
    s32 scale;
    s32 i;
    s16 gravity;
    s16 arg5;

    angle = 0;
    for (i = 0; i < 36; i++) {
        angle += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 60.0f;

        pos.x = (Math_SinS(angle) * temp_f20) + thisv->dyna.actor.world.pos.x;
        pos.y = thisv->dyna.actor.world.pos.y;
        pos.z = (Math_CosS(angle) * temp_f20) + thisv->dyna.actor.world.pos.z;

        velocity.x = Math_SinS(angle) * 3.5f;
        velocity.y = Rand_ZeroOne() * 13.0f;
        velocity.z = Math_CosS(angle) * 3.5f;

        velocity.x += thisv->dyna.actor.world.pos.x - thisv->dyna.actor.prevPos.x;
        velocity.y += thisv->dyna.actor.world.pos.y - thisv->dyna.actor.prevPos.y;
        velocity.z += thisv->dyna.actor.world.pos.z - thisv->dyna.actor.prevPos.z;

        scale = (s32)(Rand_ZeroOne() * 180.0f) + 30;
        if (scale < 90) {
            if (Rand_ZeroOne() < 0.1f) {
                gravity = -80;
                arg5 = 96;
            } else {
                gravity = -80;
                arg5 = 64;
            }
        } else {
            gravity = -100;
            arg5 = 64;
        }
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, gravity, arg5, 40, 3, 0, scale, 0, 0,
                             (s16)((scale >> 3) + 40), -1, 1, sBreakDlists[i & 1]);
    }
}

void ObjBean_UpdateLeaves(ObjBean* thisv) {
    Math_StepToS(&thisv->unk_1C2, thisv->unk_1C4, thisv->unk_1C6);
    Math_StepToS(&thisv->unk_1C8, thisv->unk_1CA, thisv->unk_1CC);
    thisv->unk_1CE += thisv->unk_1C8;
    thisv->leafRotFactor = 6372.0f - Math_SinS(thisv->unk_1CE) * (f32)thisv->unk_1C2;
    thisv->dyna.actor.scale.y = Math_SinS(thisv->leafRotFactor) * 0.17434467f;
    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_CosS(thisv->leafRotFactor) * 0.12207746f;
}

void ObjBean_SetupLeavesStill(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_LeavesStill;
    thisv->unk_1C0 = Rand_S16Offset(12, 40);
    thisv->unk_1C4 = Rand_S16Offset(0xC8, 0x190);
    thisv->unk_1C6 = 0x14;
    thisv->unk_1CA = Rand_S16Offset(0x64, 0x320);
    thisv->unk_1CC = 0x14;
}

void ObjBean_LeavesStill(ObjBean* thisv) {
    thisv->unk_1C0--;
    ObjBean_UpdateLeaves(thisv);
    if (thisv->unk_1C0 < 0) {
        ObjBean_SetupShakeLeaves(thisv);
    }
}

void ObjBean_SetupShakeLeaves(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_ShakeLeaves;
    thisv->unk_1C0 = Rand_S16Offset(30, 4);
    thisv->unk_1C4 = Rand_S16Offset(0x7D0, 0x3E8);
    thisv->unk_1C6 = 0xC8;
    thisv->unk_1CA = Rand_S16Offset(0x36B0, 0x1770);
    thisv->unk_1CC = 0xFA0;
    thisv->leafRotFactor = 0x18E4;
}

void ObjBean_ShakeLeaves(ObjBean* thisv) {
    thisv->unk_1C0 += -1;
    if (thisv->unk_1C0 == 14) {
        thisv->unk_1C4 = Rand_S16Offset(0xC8, 0x190);
        thisv->unk_1CA = Rand_S16Offset(0x64, 0x1F4);
        thisv->unk_1CC = 0x7D0;
    }
    ObjBean_UpdateLeaves(thisv);
    if (thisv->unk_1C0 < 0) {
        ObjBean_SetupLeavesStill(thisv);
    }
}

void ObjBean_SetupShakeLeavesFast(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_ShakeLeavesFast;
    thisv->unk_1C0 = 0x28;
    thisv->unk_1C4 = 0xBB8;
    thisv->unk_1C6 = 0x12C;
    thisv->unk_1CA = 0x3A98;
    thisv->unk_1CC = 0xFA0;
    thisv->leafRotFactor = 0x18E4;
}

void ObjBean_ShakeLeavesFast(ObjBean* thisv) {
    thisv->unk_1C0 += -1;
    if (Rand_ZeroOne() < 0.1f) {
        thisv->unk_1C4 = Rand_S16Offset(0x898, 0x3E8);
        thisv->unk_1CA = Rand_S16Offset(0x2EE0, 0x1F40);
    }
    ObjBean_UpdateLeaves(thisv);
    if ((s32)thisv->unk_1C0 < 0) {
        ObjBean_SetupGrow(thisv);
    }
}

void ObjBean_SetupGrow(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_Grow;
}

void ObjBean_Grow(ObjBean* thisv) {
    Math_StepToS(&thisv->leafRotFactor, 0x33E9, 0x168);
    thisv->dyna.actor.scale.y = Math_SinS(thisv->leafRotFactor) * 0.17434467f;
    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_CosS(thisv->leafRotFactor) * 0.12207746f;
    ;
}

void ObjBean_SetupFlattenLeaves(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_FlattenLeaves;
    thisv->leafRotFactor = 0x33E9;
}

void ObjBean_FlattenLeaves(ObjBean* thisv) {
    thisv->leafRotFactor -= 0x960;
    thisv->dyna.actor.scale.y = Math_SinS(thisv->leafRotFactor) * 0.17434467f;
    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_CosS(thisv->leafRotFactor) * 0.12207746f;

    if (thisv->leafRotFactor < 0x18E4) {
        ObjBean_SetupGrown(thisv);
    }
}

void ObjBean_SetupGrown(ObjBean* thisv) {
    thisv->transformFunc = ObjBean_Grown;
    thisv->unk_1C2 = 0xBB8;
    thisv->unk_1C4 = 0;
    thisv->unk_1C6 = 0xC8;
    thisv->unk_1C8 = 0x3E80;
    thisv->unk_1CA = 0x1F4;
    thisv->unk_1CC = 0;
    thisv->unk_1C0 = 0x10;
}

void ObjBean_Grown(ObjBean* thisv) {
    thisv->unk_1C0--;
    if (thisv->unk_1C0 == 6) {
        thisv->unk_1CC = 0x7D0;
    }
    ObjBean_UpdateLeaves(thisv);
    if (thisv->unk_1C2 <= 0) {
        ObjBean_SetupLeavesStill(thisv);
    }
}

void ObjBean_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 path;
    s32 linkAge;
    ObjBean* thisv = (ObjBean*)thisx;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F) || (mREG(1) == 1)) {
            path = (thisv->dyna.actor.params >> 8) & 0x1F;
            if (path == 0x1F) {
                osSyncPrintf(VT_COL(RED, WHITE));
                // "No path data?"
                osSyncPrintf("パスデータが無い？(%s %d)(arg_data %xH)\n", "../z_obj_bean.c", 909,
                             thisv->dyna.actor.params);
                osSyncPrintf(VT_RST);
                Actor_Kill(&thisv->dyna.actor);
                return;
            }
            if (globalCtx->setupPathList[path].count < 3) {
                osSyncPrintf(VT_COL(RED, WHITE));
                // "Incorrect number of path data"
                osSyncPrintf("パスデータ数が不正(%s %d)(arg_data %xH)\n", "../z_obj_bean.c", 921,
                             thisv->dyna.actor.params);
                osSyncPrintf(VT_RST);
                Actor_Kill(&thisv->dyna.actor);
                return;
            }
            ObjBean_SetupPathCount(thisv, globalCtx);
            ObjBean_SetupPath(thisv, globalCtx);
            ObjBean_Move(thisv);
            ObjBean_SetupWaitForPlayer(thisv);

            ObjBean_InitDynaPoly(thisv, globalCtx, &gMagicBeanPlatformCol, DPM_UNK3);
            thisv->stateFlags |= BEAN_STATE_DYNAPOLY_SET;
            ObjBean_InitCollider(&thisv->dyna.actor, globalCtx);
            thisv->stateFlags |= BEAN_STATE_COLLIDER_SET;

            ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 8.8f);
            ObjBean_FindFloor(thisv, globalCtx);
            thisv->unk_1F6 = thisv->dyna.actor.home.rot.z & 3;
        } else {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
    } else if ((Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F) != 0) || (mREG(1) == 1)) {
        ObjBean_SetupWaitForWater(thisv);
    } else {
        ObjBean_SetupWaitForBean(thisv);
    }
    thisv->dyna.actor.world.rot.z = thisv->dyna.actor.home.rot.z = thisv->dyna.actor.shape.rot.z = 0;
    // "Magic bean tree lift"
    osSyncPrintf("(魔法の豆の木リフト)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void ObjBean_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjBean* thisv = (ObjBean*)thisx;

    if (thisv->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
    if (thisv->stateFlags & BEAN_STATE_COLLIDER_SET) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
    if (D_80B90E30 == thisv) {
        D_80B90E30 = NULL;
    }
}

void ObjBean_SetupWaitForBean(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_WaitForBean;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES);
    thisv->dyna.actor.textId = 0x2F;
}

void ObjBean_WaitForBean(ObjBean* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->dyna.actor, globalCtx)) {
        if (func_8002F368(globalCtx) == EXCH_ITEM_BEAN) {
            func_80B8FE00(thisv);
            Flags_SetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
        }
    } else {
        func_8002F298(&thisv->dyna.actor, globalCtx, 40.0f, EXCH_ITEM_BEAN);
    }
}

void func_80B8FE00(ObjBean* thisv) {
    thisv->actionFunc = func_80B8FE3C;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES);
    thisv->timer = 60;
}

// Link is looking at the soft soil
void func_80B8FE3C(ObjBean* thisv, GlobalContext* globalCtx) {
    if (thisv->timer <= 0) {
        func_80B8FE6C(thisv);
    }
}

void func_80B8FE6C(ObjBean* thisv) {
    thisv->actionFunc = func_80B8FEAC;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_SetScale(&thisv->dyna.actor, 0.01f);
}

// The leaves are visable and growing
void func_80B8FEAC(ObjBean* thisv, GlobalContext* globalCtx) {
    s32 temp_v1 = true;

    temp_v1 &= Math_StepToF(&thisv->dyna.actor.scale.y, 0.16672663f, 0.01f);
    temp_v1 &= Math_StepToF(&thisv->dyna.actor.scale.x, 0.03569199f, 0.00113f);

    thisv->dyna.actor.scale.z = thisv->dyna.actor.scale.x;
    if (temp_v1) {
        if (thisv->timer <= 0) {
            func_80B8FF50(thisv);
        }
    } else {
        thisv->timer = 1;
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_PL_PLANT_GROW_UP - SFX_FLAG);
}

void func_80B8FF50(ObjBean* thisv) {
    thisv->actionFunc = func_80B8FF8C;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    thisv->unk_1B6.x = 0x33E9;
}

void func_80B8FF8C(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->unk_1B6.x -= 0x960;
    thisv->dyna.actor.scale.y = Math_SinS(thisv->unk_1B6.x) * 0.17434467f;
    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_CosS(thisv->unk_1B6.x) * 0.12207746f;
    if (thisv->unk_1B6.x < 0x18E4) {
        func_80B90010(thisv);
    }
}

void func_80B90010(ObjBean* thisv) {
    thisv->actionFunc = func_80B90050;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    thisv->unk_1B6.x = 0;
    thisv->unk_1B6.y = 0xBB8;
}

// Control is returned to the player and the leaves start to flatten out
void func_80B90050(ObjBean* thisv, GlobalContext* globalCtx) {
    s16 temp_a0;
    f32 temp_f2;

    thisv->unk_1B6.x += 0x3E80;
    thisv->unk_1B6.y += -0xC8;
    temp_a0 = 6372.0f - Math_SinS(thisv->unk_1B6.x) * thisv->unk_1B6.y;

    thisv->dyna.actor.scale.y = Math_SinS(temp_a0) * 0.17434467f;
    thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.z = Math_CosS(temp_a0) * 0.12207746f;
    if (thisv->unk_1B6.y < 0) {
        ObjBean_SetupWaitForWater(thisv);
    }
}

void ObjBean_SetupWaitForWater(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_WaitForWater;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_SetScale(&thisv->dyna.actor, 0.1f);
    ObjBean_SetupLeavesStill(thisv);
}

void ObjBean_WaitForWater(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->transformFunc(thisv);

    if (!(thisv->stateFlags & BEAN_STATE_BEEN_WATERED) && Flags_GetEnv(globalCtx, 5) && (D_80B90E30 == NULL) &&
        (thisv->dyna.actor.xzDistToPlayer < 50.0f)) {
        ObjBean_SetupGrowWaterPhase1(thisv);
        D_80B90E30 = thisv;
        OnePointCutscene_Init(globalCtx, 2210, -99, &thisv->dyna.actor, MAIN_CAM);
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
        return;
    }

    if ((D_80B90E30 == thisv) && !Flags_GetEnv(globalCtx, 5)) {
        D_80B90E30 = NULL;
        if (D_80B90E30) {}
    }
}

void ObjBean_SetupGrowWaterPhase1(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_GrowWaterPhase1;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    ObjBean_SetupShakeLeavesFast(thisv);
    thisv->timer = 50;
}

// Camera moves and leaves move quickly
void ObjBean_GrowWaterPhase1(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->transformFunc(thisv);
    if (thisv->timer <= 0) {
        ObjBean_SetupGrowWaterPhase2(thisv);
    }
}

void ObjBean_SetupGrowWaterPhase2(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_GrowWaterPhase2;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    thisv->stalkSizeMultiplier = 0.0f;
}

// BeanStalk is visable and is growing
void ObjBean_GrowWaterPhase2(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->transformFunc(thisv);
    thisv->stalkSizeMultiplier += 0.001f;
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.home.rot.y + (s16)(thisv->stalkSizeMultiplier * 700000.0f);
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + thisv->stalkSizeMultiplier * 800.0f;
    if (thisv->stalkSizeMultiplier >= 0.1f) { // 100 Frames
        ObjBean_SetupGrowWaterPhase3(thisv);
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_PL_PLANT_TALLER - SFX_FLAG);
}

void ObjBean_SetupGrowWaterPhase3(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_GrowWaterPhase3;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    thisv->timer = 60;
}

// Fully grown and drops items
void ObjBean_GrowWaterPhase3(ObjBean* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f itemDropPos;

    thisv->transformFunc(thisv);
    if (thisv->timer == 40) {
        ObjBean_SetupFlattenLeaves(thisv);
    } else if (thisv->timer == 30) {
        if (!(thisv->stateFlags & BEAN_STATE_BEEN_WATERED)) {
            itemDropPos.x = thisv->dyna.actor.world.pos.x;
            itemDropPos.y = thisv->dyna.actor.world.pos.y - 25.0f;
            itemDropPos.z = thisv->dyna.actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_DropCollectible(globalCtx, &itemDropPos, ITEM00_FLEXIBLE);
            }
            thisv->stateFlags |= BEAN_STATE_BEEN_WATERED;
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (thisv->timer <= 0) {
        ObjBean_SetupGrowWaterPhase4(thisv);
    }
}

void ObjBean_SetupGrowWaterPhase4(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_GrowWaterPhase4;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    ObjBean_SetupGrow(thisv);
}

// Return control back to the player and start to shrink back down
void ObjBean_GrowWaterPhase4(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->transformFunc(thisv);
    thisv->stalkSizeMultiplier -= 0.001f;
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.home.rot.y + (s16)(thisv->stalkSizeMultiplier * 700000.0f);
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + (thisv->stalkSizeMultiplier * 800.0f);
    if (thisv->stalkSizeMultiplier <= 0.0f) {
        thisv->stalkSizeMultiplier = 0.0f;
        thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.home.rot.y;
        ObjBean_SetupGrowWaterPhase5(thisv);
    }
}

void ObjBean_SetupGrowWaterPhase5(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_GrowWaterPhase5;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    thisv->timer = 30;
}

void ObjBean_GrowWaterPhase5(ObjBean* thisv, GlobalContext* globalCtx) {
    thisv->transformFunc(thisv);
    if (thisv->timer <= 0) {
        func_80B8FF50(thisv);
        thisv->dyna.actor.flags &= ~ACTOR_FLAG_4;
    }
}

void ObjBean_SetupWaitForPlayer(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_WaitForPlayer;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_PLANT);
}

void ObjBean_WaitForPlayer(ObjBean* thisv, GlobalContext* globalCtx) {
    if (func_8004356C(&thisv->dyna)) { // Player is standing on
        ObjBean_SetupFly(thisv);
        if (globalCtx->sceneNum == SCENE_SPOT10) { // Lost woods
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_LOST_WOODS);
        } else {
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_GENERIC);
        }
    }
    ObjBean_UpdatePosition(thisv);
}

void ObjBean_SetupFly(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_Fly;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_PLANT);
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.flags |= ACTOR_FLAG_4; // Never stop updating
}

void ObjBean_Fly(ObjBean* thisv, GlobalContext* globalCtx) {
    Camera* camera;

    ObjBean_FollowPath(thisv, globalCtx);
    if (thisv->currentPointIndex == thisv->pathCount) {
        ObjBean_SetupPathCount(thisv, globalCtx);
        ObjBean_SetupPath(thisv, globalCtx);
        ObjBean_SetupWaitForStepOff(thisv);

        thisv->dyna.actor.flags &= ~ACTOR_FLAG_4; // Never stop updating (disable)
        camera = globalCtx->cameraPtrs[MAIN_CAM];

        if ((camera->setting == CAM_SET_BEAN_LOST_WOODS) || (camera->setting == CAM_SET_BEAN_GENERIC)) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }

    } else if (func_8004356C(&thisv->dyna) != 0) { // Player is on top

        func_8002F974(&thisv->dyna.actor, NA_SE_PL_PLANT_MOVE - SFX_FLAG);

        if (globalCtx->sceneNum == SCENE_SPOT10) {
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_LOST_WOODS);
        } else {
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_GENERIC);
        }
    } else if (thisv->stateFlags & BEAN_STATE_PLAYER_ON_TOP) {
        camera = globalCtx->cameraPtrs[MAIN_CAM];

        if ((camera->setting == CAM_SET_BEAN_LOST_WOODS) || (camera->setting == CAM_SET_BEAN_GENERIC)) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }
    }

    ObjBean_UpdatePosition(thisv);
}

void ObjBean_SetupWaitForStepOff(ObjBean* thisv) {
    thisv->actionFunc = ObjBean_WaitForStepOff;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_PLANT);
}

void ObjBean_WaitForStepOff(ObjBean* thisv, GlobalContext* globalCtx) {
    if (!func_80043590(&thisv->dyna)) {
        ObjBean_SetupWaitForPlayer(thisv);
    }
    ObjBean_UpdatePosition(thisv);
}

void func_80B908EC(ObjBean* thisv) {
    thisv->actionFunc = func_80B90918;
    ObjBean_SetDrawMode(thisv, 0);
}

void func_80B90918(ObjBean* thisv, GlobalContext* globalCtx) {
    if (!func_8004356C(&thisv->dyna)) {
        ObjBean_SetupPathCount(thisv, globalCtx);
        ObjBean_SetupPath(thisv, globalCtx);
        ObjBean_Move(thisv);
        func_80B90970(thisv);
    }
}

void func_80B90970(ObjBean* thisv) {
    thisv->actionFunc = func_80B909B0;
    ObjBean_SetDrawMode(thisv, 0);
    thisv->timer = 100;
    func_80B8EDF4(thisv);
}

void func_80B909B0(ObjBean* thisv, GlobalContext* globalCtx) {
    if (ObjBean_CheckForHorseTrample(thisv, globalCtx)) {
        thisv->timer = 100;
    } else if (thisv->timer <= 0) {
        func_80B909F8(thisv);
    }
}

void func_80B909F8(ObjBean* thisv) {
    thisv->actionFunc = func_80B90A34;
    ObjBean_SetDrawMode(thisv, BEAN_STATE_DRAW_PLANT);
    thisv->timer = 30;
}

void func_80B90A34(ObjBean* thisv, GlobalContext* globalCtx) {
    s32 trampled = ObjBean_CheckForHorseTrample(thisv, globalCtx);

    func_80B8EE24(thisv);
    if (trampled) {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    } else {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
    if ((thisv->timer <= 0) && (!trampled)) {
        func_80B8EBC8(thisv);
        ObjBean_SetupWaitForPlayer(thisv);
    }
}
void ObjBean_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjBean* thisv = (ObjBean*)thisx;

    if (thisv->timer > 0) {
        thisv->timer--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->stateFlags & BEAN_STATE_DRAW_PLANT) {
        ObjBean_Move(thisv);
        if (thisv->dyna.actor.xzDistToPlayer < 150.0f) {
            thisv->collider.dim.radius = thisv->dyna.actor.scale.x * 640.0f + 0.5f;
            Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->collider);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        ObjBean_FindFloor(thisv, globalCtx);

        thisv->dyna.actor.shape.shadowDraw = ActorShadow_DrawCircle;
        thisv->dyna.actor.shape.shadowScale = thisv->dyna.actor.scale.x * 88.0f;

        if (ObjBean_CheckForHorseTrample(thisv, globalCtx)) {
            osSyncPrintf(VT_FGCOL(CYAN));
            // "Horse and bean tree lift collision"
            osSyncPrintf("馬と豆の木リフト衝突！！！\n");
            osSyncPrintf(VT_RST);
            ObjBean_Break(thisv, globalCtx);
            func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            func_80B908EC(thisv);
        }
    } else {
        thisv->dyna.actor.shape.shadowDraw = NULL;
    }
    Actor_SetFocus(&thisv->dyna.actor, 6.0f);
    if (thisv->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        if (func_8004356C(&thisv->dyna)) {
            thisv->stateFlags |= BEAN_STATE_PLAYER_ON_TOP;
        } else {
            thisv->stateFlags &= ~BEAN_STATE_PLAYER_ON_TOP;
        }
    }
}

void ObjBean_DrawSoftSoilSpot(ObjBean* thisv, GlobalContext* globalCtx) {
    Matrix_Translate(thisv->dyna.actor.home.pos.x, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->dyna.actor.home.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Gfx_DrawDListOpa(globalCtx, gMagicBeanSoftSoilDL);
}

void ObjBean_DrawBeanstalk(ObjBean* thisv, GlobalContext* globalCtx) {
    Matrix_Translate(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->dyna.actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.1f, thisv->stalkSizeMultiplier, 0.1f, MTXMODE_APPLY);
    Gfx_DrawDListOpa(globalCtx, gMagicBeanStemDL);
}

void ObjBean_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjBean* thisv = (ObjBean*)thisx;

    if (thisv->stateFlags & BEAN_STATE_DRAW_SOIL) {
        Gfx_DrawDListOpa(globalCtx, gMagicBeanSeedlingDL);
    }
    if (thisv->stateFlags & BEAN_STATE_DRAW_PLANT) {
        Gfx_DrawDListOpa(globalCtx, gMagicBeanPlatformDL);
    }
    if (thisv->stateFlags & BEAN_STATE_DRAW_LEAVES) {
        ObjBean_DrawSoftSoilSpot(thisv, globalCtx);
    }
    if (thisv->stateFlags & BEAN_STATE_DRAW_STALK) {
        ObjBean_DrawBeanstalk(thisv, globalCtx);
    }
}
