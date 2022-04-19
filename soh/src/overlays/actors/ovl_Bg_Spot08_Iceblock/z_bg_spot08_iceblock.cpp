/*
 * File: z_bg_spot08_iceblock.c
 * Overlay: ovl_Bg_Spot08_Iceblock
 * Description: Floating ice platforms
 */

#include "z_bg_spot08_iceblock.h"
#include "objects/object_spot08_obj/object_spot08_obj.h"

#define FLAGS 0

void BgSpot08Iceblock_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot08Iceblock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot08Iceblock_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot08Iceblock_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgSpot08Iceblock_SetupFloatNonrotating(BgSpot08Iceblock* thisv);
void BgSpot08Iceblock_FloatNonrotating(BgSpot08Iceblock* thisv, GlobalContext* globalCtx);
void BgSpot08Iceblock_SetupFloatRotating(BgSpot08Iceblock* thisv);
void BgSpot08Iceblock_FloatRotating(BgSpot08Iceblock* thisv, GlobalContext* globalCtx);
void BgSpot08Iceblock_SetupFloatOrbitingTwins(BgSpot08Iceblock* thisv);
void BgSpot08Iceblock_FloatOrbitingTwins(BgSpot08Iceblock* thisv, GlobalContext* globalCtx);
void BgSpot08Iceblock_SetupNoAction(BgSpot08Iceblock* thisv);

ActorInit Bg_Spot08_Iceblock_InitVars = {
    ACTOR_BG_SPOT08_ICEBLOCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT08_OBJ,
    sizeof(BgSpot08Iceblock),
    (ActorFunc)BgSpot08Iceblock_Init,
    (ActorFunc)BgSpot08Iceblock_Destroy,
    (ActorFunc)BgSpot08Iceblock_Update,
    (ActorFunc)BgSpot08Iceblock_Draw,
    NULL,
};

void BgSpot08Iceblock_SetupAction(BgSpot08Iceblock* thisv, BgSpot08IceblockActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgSpot08Iceblock_InitDynaPoly(BgSpot08Iceblock* thisv, GlobalContext* globalCtx, const CollisionHeader* collision,
                                   s32 flags) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning: move BG registration failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot08_iceblock.c", 0xD9,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

// Sets params to 0x10 (medium, nonrotating) if not in the cases listed.
void BgSpot08Iceblock_CheckParams(BgSpot08Iceblock* thisv) {
    switch (thisv->dyna.actor.params & 0xFF) {
        case 0xFF:
            thisv->dyna.actor.params = 0x10;
            break;
        default:
            // "Error: arg_data setting error"
            osSyncPrintf("Error : arg_data 設定ミスです。(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot08_iceblock.c", 0xF6,
                         thisv->dyna.actor.params);
            thisv->dyna.actor.params = 0x10;
            break;
        case 1:
        case 4:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x14:
        case 0x20:
        case 0x23:
        case 0x24:
            break;
    }
}

void BgSpot08Iceblock_Bobbing(BgSpot08Iceblock* thisv) {
    thisv->bobOffset = (Math_SinS(thisv->bobPhaseSlow) * 4.0f) + (Math_SinS(thisv->bobPhaseFast) * 3.0f);
}

void BgSpot08Iceblock_SinkUnderPlayer(BgSpot08Iceblock* thisv) {
    f32 target;
    f32 step;

    switch (thisv->dyna.actor.params & 0xF0) {
        case 0:
            step = 0.15f;
            break;
        case 0x10:
            step = 0.2f;
            break;
        case 0x20:
            step = 0.4f;
            break;
    }

    // Sink under Player's weight if standing on it
    target = (func_80043548(&thisv->dyna) ? -4.0f : 0.0f);

    Math_StepToF(&thisv->sinkOffset, target, step);
}

void BgSpot08Iceblock_SetWaterline(BgSpot08Iceblock* thisv) {
    thisv->dyna.actor.world.pos.y = thisv->sinkOffset + thisv->bobOffset + thisv->dyna.actor.home.pos.y;
}

void BgSpot08Iceblock_MultVectorScalar(Vec3f* dest, Vec3f* v, f32 scale) {
    dest->x = v->x * scale;
    dest->y = v->y * scale;
    dest->z = v->z * scale;
}

void BgSpot08Iceblock_CrossProduct(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

s32 BgSpot08Iceblock_NormalizeVector(Vec3f* dest, Vec3f* v) {
    f32 magnitude;

    magnitude = Math3D_Vec3fMagnitude(v);
    if (magnitude < 0.001f) {
        dest->x = dest->y = 0.0f;
        dest->z = 1.0f;
        return false;
    } else {
        dest->x = v->x * (1.0f / magnitude);
        dest->y = v->y * (1.0f / magnitude);
        dest->z = v->z * (1.0f / magnitude);
        return true;
    }
}

static Vec3f sVerticalVector = { 0.0f, 1.0f, 0.0f };
static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };
static f32 sInertias[] = { 1.0f / 70000000, 1.0f / 175000000, 1.0f / 700000000 };
static f32 sDampingFactors[] = { 0.96f, 0.96f, 0.98f };

static f32 sRollSins[] = {
    0.22495104f, // sin(13 degrees)
    0.22495104f, // sin(13 degrees)
    0.03489947f, // sin(2 degrees)
};

static f32 sRollCoss[] = {
    0.97437006f, // cos(13 degrees)
    0.97437006f, // cos(13 degrees)
    0.99939084f, // cos(2 degrees)
};

/**
 *  Handles all the factors that influence rolling: inertia, random oscillations, and most significantly, player weight,
 * and combines them to produce a matrix that rotates the actor to match the surface normal
 */
void BgSpot08Iceblock_Roll(BgSpot08Iceblock* thisv, GlobalContext* globalCtx) {
    f32 deviationFromVertSq;
    f32 stabilityCorrection;
    Vec3f surfaceNormalHorizontal;
    Vec3f playerCentroidDiff;
    Vec3f playerMoment;
    Vec3f surfaceNormalHorizontalScaled;
    Vec3f randomNutation;
    Vec3f tempVec; // reused with different meanings
    Vec3f torqueDirection;
    f32 playerCentroidDist;
    s32 rollDataIndex;
    MtxF mtx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    switch (thisv->dyna.actor.params & 0xFF) {
        case 0x11: // Medium nonrotating
            rollDataIndex = 0;
            break;
        case 1:
            rollDataIndex = 1; // Large nonrotating
            break;
        default:
            rollDataIndex = 2;
            break;
    }

    Math_Vec3f_Diff(&player->actor.world.pos, &thisv->dyna.actor.world.pos, &playerCentroidDiff);
    playerCentroidDiff.y -= (150.0f * thisv->dyna.actor.scale.y);
    playerCentroidDist = Math3D_Vec3fMagnitude(&playerCentroidDiff);

    randomNutation.x = (Rand_ZeroOne() - 0.5f) * (1.0f / 625);
    randomNutation.y = 0.0f;
    randomNutation.z = (Rand_ZeroOne() - 0.5f) * (1.0f / 625);

    surfaceNormalHorizontal.x = thisv->surfaceNormal.x;
    surfaceNormalHorizontal.y = 0.0f;
    surfaceNormalHorizontal.z = thisv->surfaceNormal.z;

    // If player is standing on it or holding the edge
    if (func_8004356C(&thisv->dyna) && (playerCentroidDist > 3.0f)) {
        Math_Vec3f_Diff(&playerCentroidDiff, &surfaceNormalHorizontal, &playerMoment);
        BgSpot08Iceblock_MultVectorScalar(&playerMoment, &playerMoment,
                                          (sInertias[rollDataIndex] * playerCentroidDist) / thisv->dyna.actor.scale.x);
    } else {
        playerMoment = sZeroVector;
    }

    BgSpot08Iceblock_MultVectorScalar(&surfaceNormalHorizontalScaled, &surfaceNormalHorizontal, -0.01f);

    // Add all three deviations
    Math_Vec3f_Sum(&thisv->normalDelta, &playerMoment, &thisv->normalDelta);
    Math_Vec3f_Sum(&thisv->normalDelta, &surfaceNormalHorizontalScaled, &thisv->normalDelta);
    Math_Vec3f_Sum(&thisv->normalDelta, &randomNutation, &thisv->normalDelta);

    thisv->normalDelta.y = 0.0f;

    Math_Vec3f_Sum(&thisv->surfaceNormal, &thisv->normalDelta, &tempVec);

    tempVec.x *= sDampingFactors[rollDataIndex];
    tempVec.z *= sDampingFactors[rollDataIndex];

    // Set up roll axis and final new angle
    if (BgSpot08Iceblock_NormalizeVector(&thisv->surfaceNormal, &tempVec)) {
        deviationFromVertSq = Math3D_Dist1DSq(thisv->surfaceNormal.z, thisv->surfaceNormal.x);

        // Prevent overrolling
        if (sRollSins[rollDataIndex] < deviationFromVertSq) {
            stabilityCorrection = sRollSins[rollDataIndex] / deviationFromVertSq;

            thisv->surfaceNormal.x *= stabilityCorrection;
            thisv->surfaceNormal.y = sRollCoss[rollDataIndex];
            thisv->surfaceNormal.z *= stabilityCorrection;
        }

        BgSpot08Iceblock_CrossProduct(&tempVec, &sVerticalVector, &thisv->surfaceNormal);

        if (BgSpot08Iceblock_NormalizeVector(&torqueDirection, &tempVec)) {
            thisv->rotationAxis = torqueDirection;
        }
    } else {
        thisv->surfaceNormal = sVerticalVector;
    }

    // Rotation by the angle between surfaceNormal and the vertical about rotationAxis
    Matrix_RotateAxis(Math_FAcosF(Math3D_Cos(&sVerticalVector, &thisv->surfaceNormal)), &thisv->rotationAxis,
                      MTXMODE_NEW);
    Matrix_RotateY(thisv->dyna.actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &thisv->dyna.actor.shape.rot, 0);
}

void BgSpot08Iceblock_SpawnTwinFloe(BgSpot08Iceblock* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    f32 sin;
    f32 cos;

    sin = Math_SinS(thisv->dyna.actor.home.rot.y) * 100.0f;
    cos = Math_CosS(thisv->dyna.actor.home.rot.y) * 100.0f;

    if (!(thisv->dyna.actor.params & 0x100)) {
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_SPOT08_ICEBLOCK,
                           thisv->dyna.actor.home.pos.x, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.home.pos.z,
                           thisv->dyna.actor.home.rot.x, thisv->dyna.actor.home.rot.y, thisv->dyna.actor.home.rot.z,
                           0x123);

        thisv->dyna.actor.world.pos.x += sin;
        thisv->dyna.actor.world.pos.z += cos;
    } else {
        thisv->dyna.actor.world.pos.x -= sin;
        thisv->dyna.actor.world.pos.z -= cos;
    }
    BgSpot08Iceblock_SetupFloatOrbitingTwins(thisv);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2200, ICHAIN_STOP),
};

void BgSpot08Iceblock_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot08Iceblock* thisv = (BgSpot08Iceblock*)thisx;
    const CollisionHeader* colHeader;

    // "spot08 ice floe"
    osSyncPrintf("(spot08 流氷)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
    BgSpot08Iceblock_CheckParams(thisv);

    switch (thisv->dyna.actor.params & 0x200) {
        case 0:
            colHeader = &gZorasFountainIcebergCol;
            break;
        case 0x200:
            colHeader = &gZorasFountainIceRampCol;
            break;
    }

    switch (thisv->dyna.actor.params & 0xF) {
        case 2:
        case 3:
            BgSpot08Iceblock_InitDynaPoly(thisv, globalCtx, colHeader, DPM_UNK3);
            break;
        default:
            BgSpot08Iceblock_InitDynaPoly(thisv, globalCtx, colHeader, DPM_UNK);
            break;
    }

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    switch (thisv->dyna.actor.params & 0xF0) {
        case 0:
            Actor_SetScale(&thisv->dyna.actor, 0.2f);
            break;
        case 0x10:
            Actor_SetScale(&thisv->dyna.actor, 0.1f);
            break;
        case 0x20:
            Actor_SetScale(&thisv->dyna.actor, 0.05f);
            break;
    }

    thisv->bobPhaseSlow = (s32)(Rand_ZeroOne() * (0xFFFF + 0.5f));
    thisv->bobPhaseFast = (s32)(Rand_ZeroOne() * (0xFFFF + 0.5f));
    thisv->surfaceNormal.y = 1.0f;
    thisv->rotationAxis.x = 1.0f;

    switch (thisv->dyna.actor.params & 0xF) {
        case 0:
        case 1:
            BgSpot08Iceblock_SetupFloatNonrotating(thisv);
            break;
        case 2:
            BgSpot08Iceblock_SetupFloatRotating(thisv);
            break;
        case 3:
            BgSpot08Iceblock_SpawnTwinFloe(thisv, globalCtx);
            break;
        case 4:
            BgSpot08Iceblock_SetupNoAction(thisv);
            break;
    }
}

void BgSpot08Iceblock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot08Iceblock* thisv = (BgSpot08Iceblock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgSpot08Iceblock_SetupFloatNonrotating(BgSpot08Iceblock* thisv) {
    BgSpot08Iceblock_SetupAction(thisv, BgSpot08Iceblock_FloatNonrotating);
}

void BgSpot08Iceblock_FloatNonrotating(BgSpot08Iceblock* thisv, GlobalContext* globalCtx) {
    BgSpot08Iceblock_Bobbing(thisv);
    BgSpot08Iceblock_SinkUnderPlayer(thisv);
    BgSpot08Iceblock_SetWaterline(thisv);
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.home.rot.y;
    BgSpot08Iceblock_Roll(thisv, globalCtx);
}

void BgSpot08Iceblock_SetupFloatRotating(BgSpot08Iceblock* thisv) {
    BgSpot08Iceblock_SetupAction(thisv, BgSpot08Iceblock_FloatRotating);
}

void BgSpot08Iceblock_FloatRotating(BgSpot08Iceblock* thisv, GlobalContext* globalCtx) {
    BgSpot08Iceblock_Bobbing(thisv);
    BgSpot08Iceblock_SinkUnderPlayer(thisv);
    BgSpot08Iceblock_SetWaterline(thisv);
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.world.rot.y + 0x190;
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y;
    BgSpot08Iceblock_Roll(thisv, globalCtx);
}

void BgSpot08Iceblock_SetupFloatOrbitingTwins(BgSpot08Iceblock* thisv) {
    BgSpot08Iceblock_SetupAction(thisv, BgSpot08Iceblock_FloatOrbitingTwins);
}

void BgSpot08Iceblock_FloatOrbitingTwins(BgSpot08Iceblock* thisv, GlobalContext* globalCtx) {
    f32 cos;
    f32 sin;

    BgSpot08Iceblock_Bobbing(thisv);
    BgSpot08Iceblock_SinkUnderPlayer(thisv);
    BgSpot08Iceblock_SetWaterline(thisv);

    // parent handles rotations of both
    if (!(thisv->dyna.actor.params & 0x100)) {
        thisv->dyna.actor.world.rot.y += 0x190;
        sin = Math_SinS(thisv->dyna.actor.world.rot.y) * 100.0f;
        cos = Math_CosS(thisv->dyna.actor.world.rot.y) * 100.0f;

        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x + sin;
        thisv->dyna.actor.world.pos.z = thisv->dyna.actor.home.pos.z + cos;

        if (thisv->dyna.actor.child != NULL) {
            thisv->dyna.actor.child->world.pos.x = thisv->dyna.actor.home.pos.x - sin;
            thisv->dyna.actor.child->world.pos.z = thisv->dyna.actor.home.pos.z - cos;
        }
    }

    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.home.rot.y;
    BgSpot08Iceblock_Roll(thisv, globalCtx);
}

void BgSpot08Iceblock_SetupNoAction(BgSpot08Iceblock* thisv) {
    BgSpot08Iceblock_SetupAction(thisv, NULL);
}

void BgSpot08Iceblock_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot08Iceblock* thisv = (BgSpot08Iceblock*)thisx;

    if (Rand_ZeroOne() < 0.05f) {
        thisv->bobIncrSlow = Rand_S16Offset(300, 100);
        thisv->bobIncrFast = Rand_S16Offset(800, 400);
    }

    thisv->bobPhaseSlow += thisv->bobIncrSlow;
    thisv->bobPhaseFast += thisv->bobIncrFast;
    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgSpot08Iceblock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    const Gfx* dList;
    BgSpot08Iceblock* thisv = (BgSpot08Iceblock*)thisx;

    switch (thisv->dyna.actor.params & 0x200) {
        case 0:
            dList = gZorasFountainIcebergDL;
            break;
        case 0x200:
            dList = gZorasFountainIceRampDL;
            break;
    }

    Gfx_DrawDListOpa(globalCtx, dList);
}
