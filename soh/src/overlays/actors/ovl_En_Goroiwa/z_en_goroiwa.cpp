/*
 * File: z_en_goroiwa.c
 * Overlay: ovl_En_Goroiwa
 * Description: Rolling boulders
 */

#include "z_en_goroiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_goroiwa/object_goroiwa.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

typedef s32 (*EnGoroiwaUnkFunc1)(EnGoroiwa* thisv, GlobalContext* globalCtx);
typedef void (*EnGoroiwaUnkFunc2)(EnGoroiwa* thisv);

#define ENGOROIWA_ENABLE_AT (1 << 0)
#define ENGOROIWA_ENABLE_OC (1 << 1)
#define ENGOROIWA_PLAYER_IN_THE_WAY (1 << 2)
#define ENGOROIWA_RETAIN_ROT_SPEED (1 << 3)
#define ENGOROIWA_IN_WATER (1 << 4)

#define ENGOROIWA_LOOPMODE_ONEWAY 0
/* same as ENGOROIWA_LOOPMODE_ONEWAY but display rock fragments as if the boulder broke at the end of the path*/
#define ENGOROIWA_LOOPMODE_ONEWAY_BREAK 1
#define ENGOROIWA_LOOPMODE_ROUNDTRIP 3

void EnGoroiwa_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGoroiwa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGoroiwa_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGoroiwa_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnGoroiwa_SetupRoll(EnGoroiwa* thisv);
void EnGoroiwa_Roll(EnGoroiwa* thisv, GlobalContext* globalCtx);
void EnGoroiwa_SetupMoveAndFallToGround(EnGoroiwa* thisv);
void EnGoroiwa_MoveAndFallToGround(EnGoroiwa* thisv, GlobalContext* globalCtx);
void EnGoroiwa_SetupWait(EnGoroiwa* thisv);
void EnGoroiwa_Wait(EnGoroiwa* thisv, GlobalContext* globalCtx);
void EnGoroiwa_SetupMoveUp(EnGoroiwa* thisv);
void EnGoroiwa_MoveUp(EnGoroiwa* thisv, GlobalContext* globalCtx);
void EnGoroiwa_SetupMoveDown(EnGoroiwa* thisv);
void EnGoroiwa_MoveDown(EnGoroiwa* thisv, GlobalContext* globalCtx);

const ActorInit En_Goroiwa_InitVars = {
    ACTOR_EN_GOROIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GOROIWA,
    sizeof(EnGoroiwa),
    (ActorFunc)EnGoroiwa_Init,
    (ActorFunc)EnGoroiwa_Destroy,
    (ActorFunc)EnGoroiwa_Update,
    (ActorFunc)EnGoroiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_HEAVY };

static f32 sUnused[] = { 10.0f, 9.2f };

void EnGoroiwa_UpdateCollider(EnGoroiwa* thisv) {
    static f32 yOffsets[] = { 0.0f, 59.5f };
    Sphere16* worldSphere = &thisv->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = thisv->actor.world.pos.x;
    worldSphere->center.y = thisv->actor.world.pos.y + yOffsets[(thisv->actor.params >> 10) & 1];
    worldSphere->center.z = thisv->actor.world.pos.z;
}

void EnGoroiwa_InitCollider(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
    EnGoroiwa_UpdateCollider(thisv);
    thisv->collider.elements[0].dim.worldSphere.radius = 58;
}

void EnGoroiwa_UpdateFlags(EnGoroiwa* thisv, u8 setFlags) {
    thisv->stateFlags &= ~(ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    thisv->stateFlags |= setFlags;
}

s32 EnGoroiwa_Vec3fNormalize(Vec3f* ret, Vec3f* a) {
    f32 magnitude = Math3D_Vec3fMagnitude(a);
    f32 scale;

    if (magnitude < 0.001f) {
        return false;
    }

    scale = 1.0f / magnitude;

    ret->x = a->x * scale;
    ret->y = a->y * scale;
    ret->z = a->z * scale;

    return true;
}

void EnGoroiwa_SetSpeed(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT04) {
        thisv->isInKokiri = true;
        R_EN_GOROIWA_SPEED = 920;
    } else {
        thisv->isInKokiri = false;
        R_EN_GOROIWA_SPEED = 1000;
    }
}

void EnGoroiwa_FaceNextWaypoint(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    Vec3s* nextPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;
    Vec3f nextPosF;

    nextPosF.x = nextPos->x;
    nextPosF.y = nextPos->y;
    nextPosF.z = nextPos->z;

    thisv->actor.world.rot.y = Math_Vec3f_Yaw(&thisv->actor.world.pos, &nextPosF);
}

void EnGoroiwa_GetPrevWaypointDiff(EnGoroiwa* thisv, GlobalContext* globalCtx, Vec3f* dest) {
    s16 loopMode = (thisv->actor.params >> 8) & 3;
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    s16 prevWaypoint = thisv->currentWaypoint - thisv->pathDirection;
    Vec3s* prevPointPos;
    Vec3s* currentPointPos;

    if (prevWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = thisv->endWaypoint;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = 1;
        }
    } else if (prevWaypoint > thisv->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = 0;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = thisv->endWaypoint - 1;
        }
    }

    currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->currentWaypoint;
    prevPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + prevWaypoint;
    dest->x = currentPointPos->x - prevPointPos->x;
    dest->y = currentPointPos->x - prevPointPos->y;
    dest->z = currentPointPos->x - prevPointPos->z;
}

void EnGoroiw_CheckEndOfPath(EnGoroiwa* thisv) {
    s16 loopMode = (thisv->actor.params >> 8) & 3;

    if (thisv->nextWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            thisv->currentWaypoint = thisv->endWaypoint;
            thisv->nextWaypoint = thisv->endWaypoint - 1;
            thisv->pathDirection = -1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            thisv->currentWaypoint = 0;
            thisv->nextWaypoint = 1;
            thisv->pathDirection = 1;
        }
    } else if (thisv->nextWaypoint > thisv->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            thisv->currentWaypoint = 0;
            thisv->nextWaypoint = 1;
            thisv->pathDirection = 1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            thisv->currentWaypoint = thisv->endWaypoint;
            thisv->nextWaypoint = thisv->endWaypoint - 1;
            thisv->pathDirection = -1;
        }
    }
}

void EnGoroiwa_SetNextWaypoint(EnGoroiwa* thisv) {
    thisv->currentWaypoint = thisv->nextWaypoint;
    thisv->nextWaypoint += thisv->pathDirection;
    EnGoroiw_CheckEndOfPath(thisv);
}

void EnGoroiwa_ReverseDirection(EnGoroiwa* thisv) {
    thisv->pathDirection *= -1;
    thisv->currentWaypoint = thisv->nextWaypoint;
    thisv->nextWaypoint += thisv->pathDirection;
}

void EnGoroiwa_InitPath(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    thisv->endWaypoint = globalCtx->setupPathList [thisv->actor.params & 0xFF].count - 1;
    thisv->currentWaypoint = 0;
    thisv->nextWaypoint = 1;
    thisv->pathDirection = 1;
}

void EnGoroiwa_TeleportToWaypoint(EnGoroiwa* thisv, GlobalContext* globalCtx, s32 waypoint) {
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    Vec3s* pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + waypoint;

    thisv->actor.world.pos.x = pointPos->x;
    thisv->actor.world.pos.y = pointPos->y;
    thisv->actor.world.pos.z = pointPos->z;
}

void EnGoroiwa_InitRotation(EnGoroiwa* thisv) {
    thisv->prevUnitRollAxis.x = 1.0f;
    thisv->rollRotSpeed = 1.0f;
}

s32 EnGoroiwa_GetAscendDirection(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->currentWaypoint;

    if (nextPointPos->x == currentPointPos->x && nextPointPos->z == currentPointPos->z) {
        if (nextPointPos->y == currentPointPos->y) {
            // "Error: Invalid path data (points overlap)"
            osSyncPrintf("Error : レールデータ不正(点が重なっている)");
            osSyncPrintf("(%s %d)(arg_data 0x%04x)\n", "../z_en_gr.c", 559, thisv->actor.params);
        }

        if (nextPointPos->y > currentPointPos->y) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

void EnGoroiwa_SpawnDust(GlobalContext* globalCtx, Vec3f* pos) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f randPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 8; i++) {
        angle += 0x4E20;
        randPos.x = pos->x + (47.0f * (Rand_ZeroOne() * 0.5f + 0.5f)) * Math_SinS(angle);
        randPos.y = pos->y + (Rand_ZeroOne() - 0.5f) * 40.0f;
        randPos.z = pos->z + ((47.0f * (Rand_ZeroOne() * 0.5f + 0.5f))) * Math_CosS(angle);
        func_800286CC(globalCtx, &randPos, &velocity, &accel, (s16)(Rand_ZeroOne() * 30.0f) + 100, 80);
        func_800286CC(globalCtx, &randPos, &velocity, &accel, (s16)(Rand_ZeroOne() * 20.0f) + 80, 80);
    }
}

void EnGoroiwa_SpawnWaterEffects(GlobalContext* globalCtx, Vec3f* contactPos) {
    Vec3f splashPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 11; i++) {
        angle += 0x1746;
        splashPos.x = contactPos->x + (Math_SinS(angle) * 55.0f);
        splashPos.y = contactPos->y;
        splashPos.z = contactPos->z + (Math_CosS(angle) * 55.0f);
        EffectSsGSplash_Spawn(globalCtx, &splashPos, 0, 0, 0, 350);
    }

    EffectSsGRipple_Spawn(globalCtx, contactPos, 300, 700, 0);
    EffectSsGRipple_Spawn(globalCtx, contactPos, 500, 900, 4);
    EffectSsGRipple_Spawn(globalCtx, contactPos, 500, 1300, 8);
}

s32 EnGoroiwa_MoveAndFall(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    Path* path;
    s32 result;
    s32 pad;
    Vec3s* nextPointPos;

    Math_StepToF(&thisv->actor.speedXZ, R_EN_GOROIWA_SPEED * 0.01f, 0.3f);
    func_8002D868(&thisv->actor);
    path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;
    result = true;
    result &= Math_StepToF(&thisv->actor.world.pos.x, nextPointPos->x, fabsf(thisv->actor.velocity.x));
    result &= Math_StepToF(&thisv->actor.world.pos.z, nextPointPos->z, fabsf(thisv->actor.velocity.z));
    thisv->actor.world.pos.y += thisv->actor.velocity.y;
    return result;
}

s32 EnGoroiwa_Move(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    s32 pad;
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->currentWaypoint;
    s32 nextPointReached;
    Vec3f posDiff;
    Vec3f nextPointPosF;

    nextPointPosF.x = nextPointPos->x;
    nextPointPosF.y = nextPointPos->y;
    nextPointPosF.z = nextPointPos->z;
    Math_StepToF(&thisv->actor.speedXZ, R_EN_GOROIWA_SPEED * 0.01f, 0.3f);
    if (Math3D_Vec3fDistSq(&nextPointPosF, &thisv->actor.world.pos) < SQ(5.0f)) {
        Math_Vec3f_Diff(&nextPointPosF, &thisv->actor.world.pos, &posDiff);
    } else {
        posDiff.x = nextPointPosF.x - currentPointPos->x;
        posDiff.y = nextPointPosF.y - currentPointPos->y;
        posDiff.z = nextPointPosF.z - currentPointPos->z;
    }
    EnGoroiwa_Vec3fNormalize(&thisv->actor.velocity, &posDiff);
    thisv->actor.velocity.x *= thisv->actor.speedXZ;
    thisv->actor.velocity.y *= thisv->actor.speedXZ;
    thisv->actor.velocity.z *= thisv->actor.speedXZ;
    nextPointReached = true;
    nextPointReached &= Math_StepToF(&thisv->actor.world.pos.x, nextPointPosF.x, fabsf(thisv->actor.velocity.x));
    nextPointReached &= Math_StepToF(&thisv->actor.world.pos.y, nextPointPosF.y, fabsf(thisv->actor.velocity.y));
    nextPointReached &= Math_StepToF(&thisv->actor.world.pos.z, nextPointPosF.z, fabsf(thisv->actor.velocity.z));
    return nextPointReached;
}

s32 EnGoroiwa_MoveUpToNextWaypoint(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;

    Math_StepToF(&thisv->actor.velocity.y, (R_EN_GOROIWA_SPEED * 0.01f) * 0.5f, 0.18f);
    thisv->actor.world.pos.x = nextPointPos->x;
    thisv->actor.world.pos.z = nextPointPos->z;
    return Math_StepToF(&thisv->actor.world.pos.y, nextPointPos->y, fabsf(thisv->actor.velocity.y));
}

s32 EnGoroiwa_MoveDownToNextWaypoint(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->nextWaypoint;
    f32 nextPointY;
    f32 thisY;
    f32 yDistToFloor;
    s32 quakeIdx;
    CollisionPoly* floorPoly;
    Vec3f raycastFrom;
    f32 floorY;
    s32 pad2;
    s32 floorBgId;
    Vec3f dustPos;
    WaterBox* waterBox;
    f32 ySurface;
    Vec3f waterHitPos;

    nextPointY = nextPointPos->y;
    Math_StepToF(&thisv->actor.velocity.y, -14.0f, 1.0f);
    thisv->actor.world.pos.x = nextPointPos->x;
    thisv->actor.world.pos.z = nextPointPos->z;
    thisY = thisv->actor.world.pos.y;
    if (1) {}
    thisv->actor.world.pos.y += thisv->actor.velocity.y;
    if (thisv->actor.velocity.y < 0.0f && thisv->actor.world.pos.y <= nextPointY) {
        if (thisv->bounceCount == 0) {
            if (thisv->actor.xzDistToPlayer < 600.0f) {
                quakeIdx = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
                Quake_SetSpeed(quakeIdx, -0x3CB0);
                Quake_SetQuakeValues(quakeIdx, 3, 0, 0, 0);
                Quake_SetCountdown(quakeIdx, 7);
            }
            thisv->rollRotSpeed = 0.0f;
            if (!(thisv->stateFlags & ENGOROIWA_IN_WATER)) {
                raycastFrom.x = thisv->actor.world.pos.x;
                raycastFrom.y = thisv->actor.world.pos.y + 50.0f;
                raycastFrom.z = thisv->actor.world.pos.z;
                floorY = BgCheck_EntityRaycastFloor5(globalCtx, &globalCtx->colCtx, &floorPoly, &floorBgId,
                                                     &thisv->actor, &raycastFrom);
                yDistToFloor = floorY - (thisv->actor.world.pos.y - 59.5f);
                if (fabsf(yDistToFloor) < 15.0f) {
                    dustPos.x = thisv->actor.world.pos.x;
                    dustPos.y = floorY + 10.0f;
                    dustPos.z = thisv->actor.world.pos.z;
                    EnGoroiwa_SpawnDust(globalCtx, &dustPos);
                }
            }
        }
        if (thisv->bounceCount >= 1) {
            return true;
        }
        thisv->bounceCount++;
        thisv->actor.velocity.y *= -0.3f;
        thisv->actor.world.pos.y = nextPointY - ((thisv->actor.world.pos.y - nextPointY) * 0.3f);
    }
    if (thisv->bounceCount == 0 &&
        WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                &ySurface, &waterBox) &&
        thisv->actor.world.pos.y <= ySurface) {
        thisv->stateFlags |= ENGOROIWA_IN_WATER;
        if (ySurface < thisY) {
            waterHitPos.x = thisv->actor.world.pos.x;
            waterHitPos.y = ySurface;
            waterHitPos.z = thisv->actor.world.pos.z;
            EnGoroiwa_SpawnWaterEffects(globalCtx, &waterHitPos);
            thisv->actor.velocity.y *= 0.2f;
        }
        if (thisv->actor.velocity.y < -8.0f) {
            thisv->actor.velocity.y = -8.0f;
        }
    }
    return false;
}

void EnGoroiwa_UpdateRotation(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    static Vec3f unitY = { 0.0f, 1.0f, 0.0f };
    s32 pad;
    Vec3f* rollAxisPtr;
    f32 rollAngleDiff;
    Vec3f rollAxis;
    Vec3f unitRollAxis;
    MtxF mtx;
    Vec3f unusedDiff;

    if (thisv->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        rollAngleDiff = thisv->prevRollAngleDiff;
    } else {
        thisv->prevRollAngleDiff = Math3D_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->actor.prevPos) * (1.0f / 59.5f);
        rollAngleDiff = thisv->prevRollAngleDiff;
    }
    rollAngleDiff *= thisv->rollRotSpeed;
    rollAxisPtr = &rollAxis;
    if (thisv->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        /*
         * EnGoroiwa_GetPrevWaypointDiff has no side effects and its result goes unused,
         * its result was probably meant to be used instead of the actor's velocity in the
         * Math3D_Vec3f_Cross call.
         */
        EnGoroiwa_GetPrevWaypointDiff(thisv, globalCtx, &unusedDiff);
        Math3D_Vec3f_Cross(&unitY, &thisv->actor.velocity, rollAxisPtr);
    } else {
        Math3D_Vec3f_Cross(&unitY, &thisv->actor.velocity, rollAxisPtr);
    }

    if (EnGoroiwa_Vec3fNormalize(&unitRollAxis, rollAxisPtr)) {
        thisv->prevUnitRollAxis = unitRollAxis;
    } else {
        unitRollAxis = thisv->prevUnitRollAxis;
    }

    Matrix_RotateAxis(rollAngleDiff, &unitRollAxis, MTXMODE_NEW);
    Matrix_RotateY(thisv->actor.shape.rot.y * (2.0f * std::numbers::pi_v<float> / 0x10000), MTXMODE_APPLY);
    Matrix_RotateX(thisv->actor.shape.rot.x * (2.0f * std::numbers::pi_v<float> / 0x10000), MTXMODE_APPLY);
    Matrix_RotateZ(thisv->actor.shape.rot.z * (2.0f * std::numbers::pi_v<float> / 0x10000), MTXMODE_APPLY);
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &thisv->actor.shape.rot, 0);
}

void EnGoroiwa_NextWaypoint(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    s16 loopMode = (thisv->actor.params >> 8) & 3;

    EnGoroiwa_SetNextWaypoint(thisv);

    if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
        if (thisv->currentWaypoint == 0 || thisv->currentWaypoint == thisv->endWaypoint) {
            EnGoroiwa_TeleportToWaypoint(thisv, globalCtx, thisv->currentWaypoint);
        }
    }

    EnGoroiwa_FaceNextWaypoint(thisv, globalCtx);
}

void EnGoroiwa_SpawnFragments(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    static f32 yOffsets[] = { 0.0f, 59.5f };
    s16 angle1;
    s16 angle2;
    s32 pad;
    Vec3f* thisPos = &thisv->actor.world.pos;
    Vec3f effectPos;
    Vec3f fragmentVelocity;
    f32 cos1;
    f32 sin1;
    f32 sin2;
    s16 yOffsetIdx = (thisv->actor.params >> 10) & 1;
    s32 i;

    for (i = 0, angle1 = 0; i < 16; i++, angle1 += 0x4E20) {
        sin1 = Math_SinS(angle1);
        cos1 = Math_CosS(angle1);
        angle2 = Rand_ZeroOne() * 0xFFFF;
        effectPos.x = Rand_ZeroOne() * 50.0f * sin1 * Math_SinS(angle2);
        sin2 = Math_SinS(angle2);
        effectPos.y = (Rand_ZeroOne() - 0.5f) * 100.0f * sin2 + yOffsets[yOffsetIdx];
        effectPos.z = Rand_ZeroOne() * 50.0f * cos1 * Math_SinS(angle2);
        fragmentVelocity.x = effectPos.x * 0.2f;
        fragmentVelocity.y = Rand_ZeroOne() * 15.0f + 2.0f;
        fragmentVelocity.z = effectPos.z * 0.2f;
        Math_Vec3f_Sum(&effectPos, thisPos, &effectPos);
        EffectSsKakera_Spawn(globalCtx, &effectPos, &fragmentVelocity, &effectPos, -340, 33, 28, 2, 0,
                             Rand_ZeroOne() * 7.0f + 1.0f, 1, 0, 70, KAKERA_COLOR_NONE, 1, gBoulderFragmentsDL);
    }

    effectPos.x = thisPos->x;
    effectPos.y = thisPos->y + yOffsets[yOffsetIdx];
    effectPos.z = thisPos->z;
    func_80033480(globalCtx, &effectPos, 80.0f, 5, 70, 110, 1);
    func_80033480(globalCtx, &effectPos, 90.0f, 5, 110, 160, 1);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -860, ICHAIN_CONTINUE), ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneForward, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 150, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

void EnGoroiwa_Init(Actor* thisx, GlobalContext* globalCtx) {
    static f32 yOffsets[] = { 0.0f, 595.0f };
    EnGoroiwa* thisv = (EnGoroiwa*)thisx;
    s32 pathIdx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    EnGoroiwa_InitCollider(thisv, globalCtx);
    pathIdx = thisv->actor.params & 0xFF;
    if (pathIdx == 0xFF) {
        // "Error: Invalid arg_data"
        osSyncPrintf("Ｅｒｒｏｒ : arg_data が不正(%s %d)(arg_data 0x%04x)\n", "../z_en_gr.c", 1033,
                     thisv->actor.params);
        Actor_Kill(&thisv->actor);
        return;
    }
    if (globalCtx->setupPathList[pathIdx].count < 2) {
        // "Error: Invalid Path Data"
        osSyncPrintf("Ｅｒｒｏｒ : レールデータ が不正(%s %d)\n", "../z_en_gr.c", 1043);
        Actor_Kill(&thisv->actor);
        return;
    }
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&thisv->actor.shape, yOffsets[(thisv->actor.params >> 10) & 1], ActorShadow_DrawCircle, 9.4f);
    thisv->actor.shape.shadowAlpha = 200;
    EnGoroiwa_SetSpeed(thisv, globalCtx);
    EnGoroiwa_InitPath(thisv, globalCtx);
    EnGoroiwa_TeleportToWaypoint(thisv, globalCtx, 0);
    EnGoroiwa_InitRotation(thisv);
    EnGoroiwa_FaceNextWaypoint(thisv, globalCtx);
    EnGoroiwa_SetupRoll(thisv);
    // "(Goroiwa)"
    osSyncPrintf("(ごろ岩)(arg 0x%04x)(rail %d)(end %d)(bgc %d)(hit %d)\n", thisv->actor.params,
                 thisv->actor.params & 0xFF, (thisv->actor.params >> 8) & 3, (thisv->actor.params >> 10) & 1,
                 thisv->actor.home.rot.z & 1);
}

void EnGoroiwa_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnGoroiwa* thisv = (EnGoroiwa*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnGoroiwa_SetupRoll(EnGoroiwa* thisv) {
    thisv->actionFunc = EnGoroiwa_Roll;
    EnGoroiwa_UpdateFlags(thisv, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    thisv->rollRotSpeed = 1.0f;
}

void EnGoroiwa_Roll(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    static EnGoroiwaUnkFunc1 moveFuncs[] = { EnGoroiwa_Move, EnGoroiwa_MoveAndFall };
    static EnGoroiwaUnkFunc2 onHitSetupFuncs[] = { EnGoroiwa_SetupWait, EnGoroiwa_SetupMoveAndFallToGround };

    s32 ascendDirection;
    s16 yawDiff;
    s16 loopMode;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        thisv->stateFlags &= ~ENGOROIWA_PLAYER_IN_THE_WAY;
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y;
        if (yawDiff > -0x4000 && yawDiff < 0x4000) {
            thisv->stateFlags |= ENGOROIWA_PLAYER_IN_THE_WAY;
            if (((thisv->actor.params >> 10) & 1) || (thisv->actor.home.rot.z & 1) != 1) {
                EnGoroiwa_ReverseDirection(thisv);
                EnGoroiwa_FaceNextWaypoint(thisv, globalCtx);
            }
        }
        func_8002F6D4(globalCtx, &thisv->actor, 2.0f, thisv->actor.yawTowardsPlayer, 0.0f, 0);
        osSyncPrintf(VT_FGCOL(CYAN));
        osSyncPrintf("Player ぶっ飛ばし\n"); // "Player knocked down"
        osSyncPrintf(VT_RST);
        onHitSetupFuncs[(thisv->actor.params >> 10) & 1](thisv);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
        if ((thisv->actor.home.rot.z & 1) == 1) {
            thisv->collisionDisabledTimer = 50;
        }
    } else if (moveFuncs[(thisv->actor.params >> 10) & 1](thisv, globalCtx)) {
        loopMode = (thisv->actor.params >> 8) & 3;
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK &&
            (thisv->nextWaypoint == 0 || thisv->nextWaypoint == thisv->endWaypoint)) {
            EnGoroiwa_SpawnFragments(thisv, globalCtx);
        }
        EnGoroiwa_NextWaypoint(thisv, globalCtx);
        if ((loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) &&
            (thisv->currentWaypoint == 0 || thisv->currentWaypoint == thisv->endWaypoint)) {
            EnGoroiwa_SetupWait(thisv);
        } else if (!((thisv->actor.params >> 10) & 1) && thisv->currentWaypoint != 0 &&
                   thisv->currentWaypoint != thisv->endWaypoint) {
            ascendDirection = EnGoroiwa_GetAscendDirection(thisv, globalCtx);
            if (ascendDirection > 0) {
                EnGoroiwa_SetupMoveUp(thisv);
            } else if (ascendDirection < 0) {
                EnGoroiwa_SetupMoveDown(thisv);
            } else {
                EnGoroiwa_SetupRoll(thisv);
            }
        } else {
            EnGoroiwa_SetupRoll(thisv);
        }
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

void EnGoroiwa_SetupMoveAndFallToGround(EnGoroiwa* thisv) {
    thisv->actionFunc = EnGoroiwa_MoveAndFallToGround;
    EnGoroiwa_UpdateFlags(thisv, ENGOROIWA_ENABLE_OC);
    thisv->actor.gravity = -0.86f;
    thisv->actor.minVelocityY = -15.0f;
    thisv->actor.speedXZ *= 0.15f;
    thisv->actor.velocity.y = 5.0f;
    thisv->rollRotSpeed = 1.0f;
}

void EnGoroiwa_MoveAndFallToGround(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    EnGoroiwa_MoveAndFall(thisv, globalCtx);
    if ((thisv->actor.bgCheckFlags & 1) && thisv->actor.velocity.y < 0.0f) {
        if ((thisv->stateFlags & ENGOROIWA_PLAYER_IN_THE_WAY) && (thisv->actor.home.rot.z & 1) == 1) {
            EnGoroiwa_ReverseDirection(thisv);
            EnGoroiwa_FaceNextWaypoint(thisv, globalCtx);
        }
        EnGoroiwa_SetupWait(thisv);
    }
}

void EnGoroiwa_SetupWait(EnGoroiwa* thisv) {
    static s16 waitDurations[] = { 20, 6 };

    thisv->actionFunc = EnGoroiwa_Wait;
    thisv->actor.speedXZ = 0.0f;
    EnGoroiwa_UpdateFlags(thisv, ENGOROIWA_ENABLE_OC);
    thisv->waitTimer = waitDurations[thisv->actor.home.rot.z & 1];
    thisv->rollRotSpeed = 0.0f;
}

void EnGoroiwa_Wait(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    if (thisv->waitTimer > 0) {
        thisv->waitTimer--;
    } else {
        thisv->collider.base.atFlags &= ~AT_HIT;
        EnGoroiwa_SetupRoll(thisv);
    }
}

void EnGoroiwa_SetupMoveUp(EnGoroiwa* thisv) {
    thisv->actionFunc = EnGoroiwa_MoveUp;
    EnGoroiwa_UpdateFlags(thisv, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    thisv->rollRotSpeed = 0.0f;
    thisv->actor.velocity.y = fabsf(thisv->actor.speedXZ) * 0.1f;
}

void EnGoroiwa_MoveUp(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        func_8002F6D4(globalCtx, &thisv->actor, 2.0f, thisv->actor.yawTowardsPlayer, 0.0f, 4);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
        if ((thisv->actor.home.rot.z & 1) == 1) {
            thisv->collisionDisabledTimer = 50;
        }
    } else if (EnGoroiwa_MoveUpToNextWaypoint(thisv, globalCtx)) {
        EnGoroiwa_NextWaypoint(thisv, globalCtx);
        EnGoroiwa_SetupRoll(thisv);
        thisv->actor.speedXZ = 0.0f;
    }
}

void EnGoroiwa_SetupMoveDown(EnGoroiwa* thisv) {
    thisv->actionFunc = EnGoroiwa_MoveDown;
    EnGoroiwa_UpdateFlags(thisv, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    thisv->rollRotSpeed = 0.3f;
    thisv->bounceCount = 0;
    thisv->actor.velocity.y = fabsf(thisv->actor.speedXZ) * -0.3f;
    thisv->stateFlags |= ENGOROIWA_RETAIN_ROT_SPEED;
    thisv->stateFlags &= ~ENGOROIWA_IN_WATER;
}

void EnGoroiwa_MoveDown(EnGoroiwa* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        func_8002F6D4(globalCtx, &thisv->actor, 2.0f, thisv->actor.yawTowardsPlayer, 0.0f, 4);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
        if ((thisv->actor.home.rot.z & 1) == 1) {
            thisv->collisionDisabledTimer = 50;
        }
    } else if (EnGoroiwa_MoveDownToNextWaypoint(thisv, globalCtx)) {
        EnGoroiwa_NextWaypoint(thisv, globalCtx);
        EnGoroiwa_SetupRoll(thisv);
        thisv->stateFlags &= ~ENGOROIWA_RETAIN_ROT_SPEED;
        thisv->actor.speedXZ = 0.0f;
    }
}

void EnGoroiwa_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGoroiwa* thisv = (EnGoroiwa*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s32 sp30;

    if (!(player->stateFlags1 & 0x300000C0)) {
        if (thisv->collisionDisabledTimer > 0) {
            thisv->collisionDisabledTimer--;
        }
        thisv->actionFunc(thisv, globalCtx);
        switch ((thisv->actor.params >> 10) & 1) {
            case 1:
                Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 0x1C);
                break;
            case 0:
                thisv->actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &sp30,
                                                                      &thisv->actor, &thisv->actor.world.pos);
                break;
        }
        EnGoroiwa_UpdateRotation(thisv, globalCtx);
        if (thisv->actor.xzDistToPlayer < 300.0f) {
            EnGoroiwa_UpdateCollider(thisv);
            if ((thisv->stateFlags & ENGOROIWA_ENABLE_AT) && thisv->collisionDisabledTimer <= 0) {
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
            if ((thisv->stateFlags & ENGOROIWA_ENABLE_OC) && thisv->collisionDisabledTimer <= 0) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
        }
    }
}

void EnGoroiwa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gRollingRockDL);
}
