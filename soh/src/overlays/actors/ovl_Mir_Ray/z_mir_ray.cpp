/*
 * File: z_mir_ray.c
 * Overlay: ovl_Mir_Ray
 * Description: Reflectable Light Beam and reflections
 */

#include "z_mir_ray.h"
#include "objects/object_mir_ray/object_mir_ray.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void MirRay_Init(Actor* thisx, GlobalContext* globalCtx);
void MirRay_Destroy(Actor* thisx, GlobalContext* globalCtx);
void MirRay_Update(Actor* thisx, GlobalContext* globalCtx);
void MirRay_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 MirRay_CheckInFrustum(Vec3f* vecA, Vec3f* vecB, f32 pointx, f32 pointy, f32 pointz, s16 radiusA, s16 radiusB);

// Locations of light beams in sMirRayData
typedef enum {
    /* 0 */ MIRRAY_SPIRIT_BOMBCHUIWAROOM_DOWNLIGHT,
    /* 1 */ MIRRAY_SPIRIT_SUNBLOCKROOM_DOWNLIGHT,
    /* 2 */ MIRRAY_SPIRIT_SINGLECOBRAROOM_DOWNLIGHT,
    /* 3 */ MIRRAY_SPIRIT_ARMOSROOM_DOWNLIGHT,
    /* 4 */ MIRRAY_SPIRIT_TOPROOM_DOWNLIGHT,
    /* 5 */ MIRRAY_SPIRIT_TOPROOM_CEILINGMIRROR,
    /* 6 */ MIRRAY_SPIRIT_SINGLECOBRAROOM_COBRA,
    /* 7 */ MIRRAY_SPIRIT_TOPROOM_COBRA1,
    /* 8 */ MIRRAY_SPIRIT_TOPROOM_COBRA2,
    /* 9 */ MIRRAY_GANONSCASTLE_SPIRITTRIAL_DOWNLIGHT
} MirRayBeamLocations;

ActorInit Mir_Ray_InitVars = {
    ACTOR_MIR_RAY,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_MIR_RAY,
    sizeof(MirRay),
    (ActorFunc)MirRay_Init,
    (ActorFunc)MirRay_Destroy,
    (ActorFunc)MirRay_Update,
    (ActorFunc)MirRay_Draw,
    NULL,
};

static u8 D_80B8E670 = 0;

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00200000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00200000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static MirRayDataEntry sMirRayData[] = {
    { { -1160, 686, -880 }, { -920, 480, -889 }, 30, 50, 1.0f, 50, 150, 0.8f, 255, 255, 255, 0x02 },
    { { -1856, 1092, -190 }, { -1703, 841, -186 }, 30, 70, 0.88f, 54, 150, 0.8f, 255, 255, 255, 0x02 },
    { { 1367, 738, -860 }, { 1091, 476, -860 }, 30, 85, 0.0f, 0, 150, 0.8f, 255, 255, 255, 0x00 },
    { { 2200, 1103, -220 }, { 2040, 843, -220 }, 30, 60, 0.0f, 0, 150, 0.8f, 255, 255, 255, 0x01 },
    { { -560, 2169, -310 }, { -560, 1743, -310 }, 30, 70, 0.0f, 0, 150, 0.8f, 255, 255, 255, 0x00 },
    { { 60, 1802, -1090 }, { 60, 973, -1090 }, 30, 70, 0.0f, 0, 150, 0.9f, 255, 255, 255, 0x0D },
    { { 1140, 480, -860 }, { 1140, 480, -860 }, 30, 30, 1.0f, 10, 100, 0.9f, 255, 255, 255, 0x0E },
    { { -560, 1743, -310 }, { -560, 1743, -310 }, 30, 30, 0.0f, 0, 100, 0.94f, 255, 255, 255, 0x0C },
    { { 60, 1743, -310 }, { 60, 1743, -310 }, 30, 30, 0.0f, 0, 100, 0.94f, 255, 255, 255, 0x0C },
    { { -1174, 448, 1194 }, { -1174, 148, 1194 }, 50, 100, 1.0f, 50, 150, 0.8f, 255, 255, 255, 0x03 }
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void MirRay_SetupCollider(MirRay* thisv) {
    Vec3f colliderOffset;
    MirRayDataEntry* dataEntry = &sMirRayData[thisv->actor.params];

    colliderOffset.x = (thisv->poolPt.x - thisv->sourcePt.x) * dataEntry->unk_10;
    colliderOffset.y = (thisv->poolPt.y - thisv->sourcePt.y) * dataEntry->unk_10;
    colliderOffset.z = (thisv->poolPt.z - thisv->sourcePt.z) * dataEntry->unk_10;
    thisv->colliderSph.elements[0].dim.worldSphere.center.x = colliderOffset.x + thisv->sourcePt.x;
    thisv->colliderSph.elements[0].dim.worldSphere.center.y = colliderOffset.y + thisv->sourcePt.y;
    thisv->colliderSph.elements[0].dim.worldSphere.center.z = colliderOffset.z + thisv->sourcePt.z;
    thisv->colliderSph.elements[0].dim.worldSphere.radius = dataEntry->unk_14 * thisv->colliderSph.elements->dim.scale;
}

// Set up a light point between source point and reflection point. Reflection point is the pool point (for windows) or
// at the player position (for mirrors)
void MirRay_MakeShieldLight(MirRay* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    MirRayDataEntry* dataEntry = &sMirRayData[thisv->actor.params];
    Vec3f reflectionPt;
    Vec3s lightPt;

    if (MirRay_CheckInFrustum(&thisv->sourcePt, &thisv->poolPt, player->actor.world.pos.x,
                              player->actor.world.pos.y + 30.0f, player->actor.world.pos.z, thisv->sourceEndRad,
                              thisv->poolEndRad)) {

        if (dataEntry->params & 8) { // Light beams from mirrors
            Math_Vec3f_Diff(&player->actor.world.pos, &thisv->sourcePt, &reflectionPt);
        } else { // Light beams from windows
            Math_Vec3f_Diff(&thisv->poolPt, &thisv->sourcePt, &reflectionPt);
        }

        lightPt.x = (dataEntry->unk_18 * reflectionPt.x) + thisv->sourcePt.x;
        lightPt.y = (dataEntry->unk_18 * reflectionPt.y) + thisv->sourcePt.y;
        lightPt.z = (dataEntry->unk_18 * reflectionPt.z) + thisv->sourcePt.z;

        // Fade up
        Math_StepToS(&thisv->lightPointRad, dataEntry->lgtPtMaxRad, 6);
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, lightPt.x, lightPt.y, lightPt.z, dataEntry->color.r,
                                  dataEntry->color.g, dataEntry->color.b, thisv->lightPointRad);
    } else {
        // Fade down
        Math_StepToS(&thisv->lightPointRad, 0, 6);
        Lights_PointSetColorAndRadius(&thisv->lightInfo, dataEntry->color.r, dataEntry->color.g, dataEntry->color.b,
                                      thisv->lightPointRad);
    }
}

void MirRay_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    MirRay* thisv = (MirRay*)thisx;
    MirRayDataEntry* dataEntry = &sMirRayData[thisv->actor.params];

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    // "Generation of reflectable light!"
    osSyncPrintf("反射用 光の発生!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    LOG_NUM("thisv->actor.arg_data", thisv->actor.params, "../z_mir_ray.c", 518);

    if (thisv->actor.params >= 0xA) {
        // "Reflected light generation failure"
        LOG_STRING("反射光 発生失敗", "../z_mir_ray.c", 521);
        Actor_Kill(&thisv->actor);
    }

    thisv->sourcePt.x = dataEntry->sourcePoint.x;
    thisv->sourcePt.y = dataEntry->sourcePoint.y;
    thisv->sourcePt.z = dataEntry->sourcePoint.z;
    thisv->sourceEndRad = dataEntry->sourceEndRadius;

    thisv->poolPt.x = dataEntry->poolPoint.x;
    thisv->poolPt.y = dataEntry->poolPoint.y;
    thisv->poolPt.z = dataEntry->poolPoint.z;
    thisv->poolEndRad = dataEntry->poolEndRadius;

    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->sourcePt.x, thisv->sourcePt.y, thisv->sourcePt.z, 255, 255, 255,
                              100);
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);

    thisv->shieldCorners[0].x = -536.0f;
    thisv->shieldCorners[0].y = -939.0f;

    thisv->shieldCorners[1].x = -1690.0f;
    thisv->shieldCorners[1].y = 0.0f;

    thisv->shieldCorners[2].x = -536.0f;
    thisv->shieldCorners[2].y = 938.0f;

    thisv->shieldCorners[3].x = 921.0f;
    thisv->shieldCorners[3].y = 0.0f;

    thisv->shieldCorners[4].x = 758.0f;
    thisv->shieldCorners[4].y = 800.0f;

    thisv->shieldCorners[5].x = 758.0f;
    thisv->shieldCorners[5].y = -800.0f;

    if (dataEntry->params & 2) {
        Collider_InitJntSph(globalCtx, &thisv->colliderSph);
        Collider_SetJntSph(globalCtx, &thisv->colliderSph, &thisv->actor, &sJntSphInit, &thisv->colliderSphItem);
        if (!(dataEntry->params & 4)) { // Beams not from mirrors
            MirRay_SetupCollider(thisv);
        }
    }

    Collider_InitQuad(globalCtx, &thisv->shieldRay);
    Collider_SetQuad(globalCtx, &thisv->shieldRay, &thisv->actor, &sQuadInit);

    // Spirit Temple top room mirrors
    if ((thisv->actor.params == 5) || (thisv->actor.params == 7) || (thisv->actor.params == 8)) {
        thisv->actor.room = -1;
    }
}

void MirRay_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    MirRay* thisv = (MirRay*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);

    if (sMirRayData[thisv->actor.params].params & 2) {
        Collider_DestroyJntSph(globalCtx, &thisv->colliderSph);
    }

    Collider_DestroyQuad(globalCtx, &thisv->shieldRay);
}

void MirRay_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    MirRay* thisv = (MirRay*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    D_80B8E670 = 0;

    if (!thisv->unLit) {
        if (sMirRayData[thisv->actor.params].params & 2) {
            if (sMirRayData[thisv->actor.params].params & 4) { // Beams from mirrors
                MirRay_SetupCollider(thisv);
            }
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSph.base);
        }
        if (thisv->reflectIntensity > 0.0f) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->shieldRay.base);
        }
        MirRay_MakeShieldLight(thisv, globalCtx);

        if (thisv->reflectIntensity > 0.0f) {
            func_8002F8F0(&player->actor, NA_SE_IT_SHIELD_BEAM - SFX_FLAG);
        }
    }
}

void MirRay_SetIntensity(MirRay* thisv, GlobalContext* globalCtx) {
    f32 sp4C[3];
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f2_2;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    MtxF* shieldMtx = &player->shieldMf;

    thisv->reflectIntensity = 0.0f;

    if (MirRay_CheckInFrustum(&thisv->sourcePt, &thisv->poolPt, shieldMtx->mf_raw.xw, shieldMtx->mf_raw.yw,
                              shieldMtx->mf_raw.zw,
                              thisv->sourceEndRad, thisv->poolEndRad)) {

        temp_f0 = sqrtf(SQ(shieldMtx->mf_raw.zz) + (SQ(shieldMtx->mf_raw.xz) + SQ(shieldMtx->mf_raw.yz)));

        if (temp_f0 == 0.0f) {
            thisv->reflectRange = 1.0f;
        } else {
            thisv->reflectRange = 1.0f / temp_f0;
        }

        // If light beam is adirectional, always reflect, else only reflect if shield is pointing in correct direction
        if (sMirRayData[thisv->actor.params].params & 1) {
            thisv->reflectIntensity = 1.0f;
        } else {
            sp4C[0] = thisv->poolPt.x - thisv->sourcePt.x;
            sp4C[1] = thisv->poolPt.y - thisv->sourcePt.y;
            sp4C[2] = thisv->poolPt.z - thisv->sourcePt.z;

            temp_f2_2 =
                -shieldMtx->mf_raw.xz * sp4C[0] - shieldMtx->mf_raw.yz * sp4C[1] - shieldMtx->mf_raw.zz * sp4C[2];

            if (temp_f2_2 < 0.0f) {
                temp_f0_2 = sqrtf(SQ(sp4C[0]) + SQ(sp4C[1]) + SQ(sp4C[2]));
                if ((temp_f0 != 0.0f) && (temp_f0_2 != 0.0f)) {
                    thisv->reflectIntensity = -temp_f2_2 / (temp_f0 * temp_f0_2);
                }
            }
        }
    }
}

// Draws six images, one for each corner of the shield, by finding the intersection of a line segment from the corner
// perpendicular to the shield with the nearest collision (if any).
void MirRay_SetupReflectionPolys(MirRay* thisv, GlobalContext* globalCtx, MirRayShieldReflection* reflection) {
    Player* player = GET_PLAYER(globalCtx);
    MtxF* shieldMtx;
    s32 i;
    Vec3f posA;
    Vec3f posB;
    Vec3f posResult;
    CollisionPoly* outPoly;
    Vec3f sp60;

    shieldMtx = &player->shieldMf;

    sp60.x = -((*shieldMtx).mf_raw.xz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;
    sp60.y = -((*shieldMtx).mf_raw.yz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;
    sp60.z = -((*shieldMtx).mf_raw.zz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;

    sp60 = sp60; // Need something involving sp60 or the whole function changes

    for (i = 0; i < 6; i++) {
        posA.x = ((*shieldMtx).mf_raw.xw + (thisv->shieldCorners[i].x * (*shieldMtx).mf_raw.xx)) +
                 (thisv->shieldCorners[i].y * (*shieldMtx).mf_raw.xy);
        posA.y = ((*shieldMtx).mf_raw.yw + (thisv->shieldCorners[i].x * (*shieldMtx).mf_raw.yx)) +
                 (thisv->shieldCorners[i].y * (*shieldMtx).mf_raw.yy);
        posA.z = ((*shieldMtx).mf_raw.zw + (thisv->shieldCorners[i].x * (*shieldMtx).mf_raw.zx)) +
                 (thisv->shieldCorners[i].y * (*shieldMtx).mf_raw.zy);
        posB.x = sp60.x + posA.x;
        posB.y = sp60.y + posA.y;
        posB.z = sp60.z + posA.z;
        if (BgCheck_AnyLineTest1(&globalCtx->colCtx, &posA, &posB, &posResult, &outPoly, 1)) {
            reflection[i].reflectionPoly = outPoly;
        } else {
            reflection[i].reflectionPoly = NULL;
        }
    }
}

// Remove reflections that are in the same position and are sufficiently near to the same plane
void MirRay_RemoveSimilarReflections(MirRayShieldReflection* reflection) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = i + 1; j < 6; j++) {
            if (reflection[i].reflectionPoly != NULL) {
                if ((reflection[j].reflectionPoly != NULL) &&
                    (ABS(reflection[i].reflectionPoly->normal.x - reflection[j].reflectionPoly->normal.x) < 100) &&
                    (ABS(reflection[i].reflectionPoly->normal.y - reflection[j].reflectionPoly->normal.y) < 100) &&
                    (ABS(reflection[i].reflectionPoly->normal.z - reflection[j].reflectionPoly->normal.z) < 100) &&
                    (reflection[i].reflectionPoly->dist == reflection[j].reflectionPoly->dist)) {
                    reflection[j].reflectionPoly = NULL;
                }
            }
        }
    }
}

// Creates the reflected beam's collider (to interact with objects) and places and orients the shield images
void MirRay_ReflectedBeam(MirRay* thisv, GlobalContext* globalCtx, MirRayShieldReflection* reflection) {
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    f32 temp_f0;
    Vec3f vecB;
    Vec3f vecD;
    Vec3f sp118;
    Vec3f sp10C;
    Vec3f sp100;
    Vec3f intersection;
    Vec3f spE8;
    Vec3f normalVec;
    MtxF* shieldMtx;
    Vec3f vecA;
    Vec3f vecC;
    MirRayShieldReflection* currentReflection;

    shieldMtx = &player->shieldMf;

    spE8.x = -(shieldMtx->mf_raw.xz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;
    spE8.y = -(shieldMtx->mf_raw.yz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;
    spE8.z = -(shieldMtx->mf_raw.zz * thisv->reflectRange) * thisv->reflectIntensity * 400.0f;

    vecB.x = shieldMtx->mf_raw.xw;
    vecB.y = shieldMtx->mf_raw.yw;
    vecB.z = shieldMtx->mf_raw.zw;

    vecD.x = spE8.x + vecB.x;
    vecD.y = spE8.y + vecB.y;
    vecD.z = spE8.z + vecB.z;

    vecA.x = vecB.x + (shieldMtx->mf_raw.xx * 300.0f);
    vecA.y = vecB.y + (shieldMtx->mf_raw.yx * 300.0f);
    vecA.z = vecB.z + (shieldMtx->mf_raw.zx * 300.0f);

    vecC.x = vecD.x + (shieldMtx->mf_raw.xx * 300.0f);
    vecC.y = vecD.y + (shieldMtx->mf_raw.yx * 300.0f);
    vecC.z = vecD.z + (shieldMtx->mf_raw.zx * 300.0f);

    Collider_SetQuadVertices(&thisv->shieldRay, &vecA, &vecB, &vecC, &vecD);

    for (i = 0; i < 6; i++) {
        currentReflection = &reflection[i];
        if (currentReflection->reflectionPoly != NULL) {
            normalVec.x = COLPOLY_GET_NORMAL(currentReflection->reflectionPoly->normal.x);
            normalVec.y = COLPOLY_GET_NORMAL(currentReflection->reflectionPoly->normal.y);
            normalVec.z = COLPOLY_GET_NORMAL(currentReflection->reflectionPoly->normal.z);

            if (Math3D_LineSegVsPlane(normalVec.x, normalVec.y, normalVec.z, currentReflection->reflectionPoly->dist,
                                      &vecB, &vecD, &sp118, 1)) {

                currentReflection->pos.x = sp118.x;
                currentReflection->pos.y = sp118.y;
                currentReflection->pos.z = sp118.z;

                temp_f0 = sqrtf(SQ(sp118.x - vecB.x) + SQ(sp118.y - vecB.y) + SQ(sp118.z - vecB.z));

                if (temp_f0 < (thisv->reflectIntensity * 600.0f)) {
                    currentReflection->opacity = 200;
                } else {
                    currentReflection->opacity = (s32)(800.0f - temp_f0);
                }

                spE8 = spE8; // Required to match

                sp10C.x = (shieldMtx->mf_raw.xx * 100.0f) + vecB.x;
                sp10C.y = (shieldMtx->mf_raw.yx * 100.0f) + vecB.y;
                sp10C.z = (shieldMtx->mf_raw.zx * 100.0f) + vecB.z;

                sp100.x = (spE8.x * 4.0f) + sp10C.x;
                sp100.y = (spE8.y * 4.0f) + sp10C.y;
                sp100.z = (spE8.z * 4.0f) + sp10C.z;

                normalVec = normalVec; // Required to match

                currentReflection->mtx.mf_raw.zw = 0.0f;

                if (1) {}
                if (1) {}
                if (1) {}
                if (1) {} // All four required to match

                currentReflection->mtx.mf_raw.xx = currentReflection->mtx.mf_raw.yy = currentReflection->mtx.mf_raw.zz =
                    currentReflection->mtx.mf_raw.ww = 1.0f;
                currentReflection->mtx.mf_raw.yx = currentReflection->mtx.mf_raw.zx = currentReflection->mtx.mf_raw.wx =
                    currentReflection->mtx.mf_raw.xy = currentReflection->mtx.mf_raw.zy =
                        currentReflection->mtx.mf_raw.wy = currentReflection->mtx.mf_raw.xz =
                            currentReflection->mtx.mf_raw.yz = currentReflection->mtx.mf_raw.wz =
                                currentReflection->mtx.mf_raw.xw = currentReflection->mtx.mf_raw.yw =
                                    currentReflection->mtx.mf_raw.zw;

                if (Math3D_LineSegVsPlane(normalVec.x, normalVec.y, normalVec.z,
                                          currentReflection->reflectionPoly->dist, &sp10C, &sp100, &intersection, 1)) {
                    currentReflection->mtx.mf_raw.xx = intersection.x - sp118.x;
                    currentReflection->mtx.mf_raw.yx = intersection.y - sp118.y;
                    currentReflection->mtx.mf_raw.zx = intersection.z - sp118.z;
                }

                sp10C.x = (shieldMtx->mf_raw.xy * 100.0f) + vecB.x;
                sp10C.y = (shieldMtx->mf_raw.yy * 100.0f) + vecB.y;
                sp10C.z = (shieldMtx->mf_raw.zy * 100.0f) + vecB.z;

                sp100.x = (spE8.x * 4.0f) + sp10C.x;
                sp100.y = (spE8.y * 4.0f) + sp10C.y;
                sp100.z = (spE8.z * 4.0f) + sp10C.z;

                if (Math3D_LineSegVsPlane(normalVec.x, normalVec.y, normalVec.z,
                                          currentReflection->reflectionPoly->dist, &sp10C, &sp100, &intersection, 1)) {
                    currentReflection->mtx.mf_raw.xy = intersection.x - sp118.x;
                    currentReflection->mtx.mf_raw.yy = intersection.y - sp118.y;
                    currentReflection->mtx.mf_raw.zy = intersection.z - sp118.z;
                }
            } else {
                currentReflection->reflectionPoly = NULL;
            }
        }
    }
}

void MirRay_Draw(Actor* thisx, GlobalContext* globalCtx) {
    MirRay* thisv = (MirRay*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    MirRayShieldReflection reflection[6];
    s32 temp;

    thisv->reflectIntensity = 0.0f;
    if ((D_80B8E670 == 0) && !thisv->unLit && Player_HasMirrorShieldSetToDraw(globalCtx)) {
        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        MirRay_SetIntensity(thisv, globalCtx);
        if (!(thisv->reflectIntensity <= 0.0f)) {
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_mir_ray.c", 966);

            func_80093D84(globalCtx->state.gfxCtx);
            Matrix_Scale(1.0f, 1.0f, thisv->reflectIntensity * 5.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_mir_ray.c", 972),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 150, (s16)(temp = thisv->reflectIntensity * 100.0f));
            gSPDisplayList(POLY_XLU_DISP++, gShieldBeamGlowDL);
            MirRay_SetupReflectionPolys(thisv, globalCtx, reflection);
            MirRay_RemoveSimilarReflections(reflection);
            MirRay_ReflectedBeam(thisv, globalCtx, reflection);

            if (reflection[0].reflectionPoly == NULL) {
                reflection[0].opacity = 0;
            }
            for (i = 1; i < 6; i++) {
                if (reflection[i].reflectionPoly != NULL) {
                    if (reflection[0].opacity < reflection[i].opacity) {
                        reflection[0].opacity = reflection[i].opacity;
                    }
                }
            }
            for (i = 0; i < 6; i++) {
                if (reflection[i].reflectionPoly != NULL) {
                    Matrix_Translate(reflection[i].pos.x, reflection[i].pos.y, reflection[i].pos.z, MTXMODE_NEW);
                    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
                    Matrix_Mult(&reflection[i].mtx, MTXMODE_APPLY);
                    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_mir_ray.c", 1006),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_DECAL2);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 150, reflection[0].opacity);
                    gSPDisplayList(POLY_XLU_DISP++, gShieldBeamImageDL);
                }
            }

            D_80B8E670 = 1;

            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_mir_ray.c", 1027);
        }
    }
}

// Computes if the Point (pointx, pointy, pointz) lies within the right conical frustum with one end centred at vecA
// with radius radiusA, the other at vecB with radius radiusB
s32 MirRay_CheckInFrustum(Vec3f* vecA, Vec3f* vecB, f32 pointx, f32 pointy, f32 pointz, s16 radiusA, s16 radiusB) {
    f32 coneRadius;
    f32 closestPtx;
    f32 closestPty;
    f32 closestPtz;
    Vec3f vecdiff;
    f32 dist;
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;

    vecdiff.x = vecB->x - vecA->x;
    vecdiff.y = vecB->y - vecA->y;
    vecdiff.z = vecB->z - vecA->z;
    if (1) {}
    dist = SQ(vecdiff.x) + SQ(vecdiff.y) + SQ(vecdiff.z);

    if (dist == 0.0f) {
        return 0;
    }

    dist =
        (((pointx - vecA->x) * vecdiff.x) + ((pointy - vecA->y) * vecdiff.y) + ((pointz - vecA->z) * vecdiff.z)) / dist;

    // Closest point on line A-B to Point
    closestPtx = (vecdiff.x * dist) + vecA->x;
    closestPty = (vecdiff.y * dist) + vecA->y;
    closestPtz = (vecdiff.z * dist) + vecA->z;

    // Diameter of the double cone on the perpendicular plane through the closest point
    coneRadius = ((radiusB - radiusA) * dist) + radiusA;

    // If the Point is within the bounding double cone, check if it is in the frustum by checking whether it is between
    // the bounding planes
    if ((SQ(closestPtx - pointx) + SQ(closestPty - pointy) + SQ(closestPtz - pointz)) <= SQ(coneRadius)) {
        if (1) {}

        // Stores the vector difference again
        Math_Vec3f_Diff(vecB, vecA, &sp5C);

        sp50.x = pointx - vecA->x;
        sp50.y = pointy - vecA->y;
        sp50.z = pointz - vecA->z;

        if (Math3D_Cos(&sp5C, &sp50) < 0.0f) {
            return 0;
        }

        sp44.x = pointx - vecB->x;
        sp44.y = pointy - vecB->y;
        sp44.z = pointz - vecB->z;

        if (Math3D_Cos(&sp5C, &sp44) > 0.0f) {
            return 0;
        }
        return 1;
    }
    return 0;
}
