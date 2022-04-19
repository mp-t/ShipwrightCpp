#include "z_en_bom_chu.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

#define BOMBCHU_SCALE 0.01f

void EnBomChu_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBomChu_WaitForRelease(EnBomChu* thisv, GlobalContext* globalCtx);
void EnBomChu_Move(EnBomChu* thisv, GlobalContext* globalCtx);
void EnBomChu_WaitForKill(EnBomChu* thisv, GlobalContext* globalCtx);

ActorInit En_Bom_Chu_InitVars = {
    ACTOR_EN_BOM_CHU,
    ACTORCAT_EXPLOSIVE,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBomChu),
    (ActorFunc)EnBomChu_Init,
    (ActorFunc)EnBomChu_Destroy,
    (ActorFunc)EnBomChu_Update,
    (ActorFunc)EnBomChu_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElemInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 12 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElemInit),
    sJntSphElemInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000 * BOMBCHU_SCALE, ICHAIN_STOP),
};

void EnBomChu_Init(Actor* thisx, GlobalContext* globalCtx) {
    static u8 p1StartColor[] = { 250, 0, 0, 250 };
    static u8 p2StartColor[] = { 200, 0, 0, 130 };
    static u8 p1EndColor[] = { 150, 0, 0, 100 };
    static u8 p2EndColor[] = { 100, 0, 0, 50 };
    EnBomChu* thisv = (EnBomChu*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderElements);

    thisv->collider.elements[0].dim.worldSphere.radius = thisv->collider.elements[0].dim.modelSphere.radius;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = p1StartColor[i];
        blureInit.p2StartColor[i] = p2StartColor[i];
        blureInit.p1EndColor[i] = p1EndColor[i];
        blureInit.p2EndColor[i] = p2EndColor[i];
    }

    blureInit.elemDuration = 16;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 0;

    Effect_Add(globalCtx, &thisv->blure1Index, EFFECT_BLURE1, 0, 0, &blureInit);
    Effect_Add(globalCtx, &thisv->blure2Index, EFFECT_BLURE1, 0, 0, &blureInit);

    thisv->actor.room = -1;
    thisv->timer = 120;
    thisv->actionFunc = EnBomChu_WaitForRelease;
}

void EnBomChu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBomChu* thisv = (EnBomChu*)thisx;

    Effect_Delete(globalCtx, thisv->blure1Index);
    Effect_Delete(globalCtx, thisv->blure2Index);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnBomChu_Explode(EnBomChu* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;
    s32 i;

    bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                               thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, BOMB_BODY);
    if (bomb != NULL) {
        bomb->timer = 0;
    }

    thisv->timer = 1;
    thisv->actor.speedXZ = 0.0f;

    if (thisv->actor.yDistToWater > 0.0f) {
        for (i = 0; i < 40; i++) {
            EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 1.0f, 5.0f, 30.0f, 0.25f);
        }
    }

    thisv->actionFunc = EnBomChu_WaitForKill;
}

void EnBomChu_CrossProduct(Vec3f* a, Vec3f* b, Vec3f* dest) {
    dest->x = (a->y * b->z) - (a->z * b->y);
    dest->y = (a->z * b->x) - (a->x * b->z);
    dest->z = (a->x * b->y) - (a->y * b->x);
}

void EnBomChu_UpdateFloorPoly(EnBomChu* thisv, CollisionPoly* floorPoly, GlobalContext* globalCtx) {
    Vec3f normal;
    Vec3f vec;
    f32 angle;
    f32 magnitude;
    f32 normDotUp;
    MtxF mf;

    thisv->actor.floorPoly = floorPoly;

    normal.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    normal.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    normal.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    normDotUp = DOTXYZ(normal, thisv->axisUp);

    if (!(fabsf(normDotUp) >= 1.0f)) {
        angle = Math_FAcosF(normDotUp);

        if (!(angle < 0.001f)) {
            EnBomChu_CrossProduct(&thisv->axisUp, &normal, &vec);
            //! @bug thisv function expects a unit vector but `vec` is not normalized
            Matrix_RotateAxis(angle, &vec, MTXMODE_NEW);

            Matrix_MultVec3f(&thisv->axisLeft, &vec);
            thisv->axisLeft = vec;

            EnBomChu_CrossProduct(&thisv->axisLeft, &normal, &thisv->axisForwards);

            magnitude = Math3D_Vec3fMagnitude(&thisv->axisForwards);

            if (magnitude < 0.001f) {
                EnBomChu_Explode(thisv, globalCtx);
                return;
            }

            thisv->axisForwards.x *= 1.0f / magnitude;
            thisv->axisForwards.y *= 1.0f / magnitude;
            thisv->axisForwards.z *= 1.0f / magnitude;

            thisv->axisUp = normal;

            if (1) {}

            // mf = (axisLeft | axisUp | axisForwards)

            mf.mf_raw.xx = thisv->axisLeft.x;
            mf.mf_raw.yx = thisv->axisLeft.y;
            mf.mf_raw.zx = thisv->axisLeft.z;

            mf.mf_raw.xy = normal.x;
            mf.mf_raw.yy = normal.y;
            mf.mf_raw.zy = normal.z;

            mf.mf_raw.xz = thisv->axisForwards.x;
            mf.mf_raw.yz = thisv->axisForwards.y;
            mf.mf_raw.zz = thisv->axisForwards.z;

            Matrix_MtxFToYXZRotS(&mf, &thisv->actor.world.rot, 0);

            // A hack for preventing bombchus from sticking to ledges.
            // The visual rotation reverts the sign inversion (shape.rot.x = -world.rot.x).
            // The better fix would be making func_8002D908 compute XYZ velocity better,
            // or not using it and make the bombchu compute its own velocity.
            thisv->actor.world.rot.x = -thisv->actor.world.rot.x;
        }
    }
}

void EnBomChu_WaitForRelease(EnBomChu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        EnBomChu_Explode(thisv, globalCtx);
        return;
    }

    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        thisv->actor.world.pos = player->actor.world.pos;
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
        thisv->actor.shape.rot.y = player->actor.shape.rot.y;

        // rot.y = 0 -> +z (forwards in model space)
        thisv->axisForwards.x = Math_SinS(thisv->actor.shape.rot.y);
        thisv->axisForwards.y = 0.0f;
        thisv->axisForwards.z = Math_CosS(thisv->actor.shape.rot.y);

        // +y (up in model space)
        thisv->axisUp.x = 0.0f;
        thisv->axisUp.y = 1.0f;
        thisv->axisUp.z = 0.0f;

        // rot.y = 0 -> +x (left in model space)
        thisv->axisLeft.x = Math_SinS(thisv->actor.shape.rot.y + 0x4000);
        thisv->axisLeft.y = 0;
        thisv->axisLeft.z = Math_CosS(thisv->actor.shape.rot.y + 0x4000);

        thisv->actor.speedXZ = 8.0f;
        //! @bug there is no NULL check on the floor poly.  If the player is out of bounds the floor poly will be NULL
        //! and will cause a crash inside thisv function.
        EnBomChu_UpdateFloorPoly(thisv, thisv->actor.floorPoly, globalCtx);
        thisv->actor.flags |= ACTOR_FLAG_0; // make chu targetable
        func_8002F850(globalCtx, &thisv->actor);
        thisv->actionFunc = EnBomChu_Move;
    }
}

void EnBomChu_Move(EnBomChu* thisv, GlobalContext* globalCtx) {
    CollisionPoly* polySide;
    CollisionPoly* polyUpDown;
    s32 bgIdSide;
    s32 bgIdUpDown;
    s32 i;
    f32 lineLength;
    Vec3f posA;
    Vec3f posB;
    Vec3f posSide;
    Vec3f posUpDown;

    thisv->actor.speedXZ = 8.0f;
    lineLength = thisv->actor.speedXZ * 2.0f;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if ((thisv->timer == 0) || (thisv->collider.base.acFlags & AC_HIT) ||
        ((thisv->collider.base.ocFlags1 & OC1_HIT) && (thisv->collider.base.oc->category != ACTORCAT_PLAYER))) {
        EnBomChu_Explode(thisv, globalCtx);
        return;
    }

    posA.x = thisv->actor.world.pos.x + (thisv->axisUp.x * 2.0f);
    posA.y = thisv->actor.world.pos.y + (thisv->axisUp.y * 2.0f);
    posA.z = thisv->actor.world.pos.z + (thisv->axisUp.z * 2.0f);

    posB.x = thisv->actor.world.pos.x - (thisv->axisUp.x * 4.0f);
    posB.y = thisv->actor.world.pos.y - (thisv->axisUp.y * 4.0f);
    posB.z = thisv->actor.world.pos.z - (thisv->axisUp.z * 4.0f);

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &posA, &posB, &posUpDown, &polyUpDown, true, true, true, true,
                                &bgIdUpDown) &&
        !(func_80041DB8(&globalCtx->colCtx, polyUpDown, bgIdUpDown) & 0x30) && // && not crawl space?
        !SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, polyUpDown, bgIdUpDown)) {
        // forwards
        posB.x = (thisv->axisForwards.x * lineLength) + posA.x;
        posB.y = (thisv->axisForwards.y * lineLength) + posA.y;
        posB.z = (thisv->axisForwards.z * lineLength) + posA.z;

        if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                    &bgIdSide) &&
            !(func_80041DB8(&globalCtx->colCtx, polySide, bgIdSide) & 0x30) &&
            !SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, polySide, bgIdSide)) {
            EnBomChu_UpdateFloorPoly(thisv, polySide, globalCtx);
            thisv->actor.world.pos = posSide;
            thisv->actor.floorBgId = bgIdSide;
            thisv->actor.speedXZ = 0.0f;
        } else {
            if (thisv->actor.floorPoly != polyUpDown) {
                EnBomChu_UpdateFloorPoly(thisv, polyUpDown, globalCtx);
            }

            thisv->actor.world.pos = posUpDown;
            thisv->actor.floorBgId = bgIdUpDown;
        }
    } else {
        thisv->actor.speedXZ = 0.0f;
        lineLength *= 3.0f;
        posA = posB;

        for (i = 0; i < 3; i++) {
            if (i == 0) {
                // backwards
                posB.x = posA.x - (thisv->axisForwards.x * lineLength);
                posB.y = posA.y - (thisv->axisForwards.y * lineLength);
                posB.z = posA.z - (thisv->axisForwards.z * lineLength);
            } else if (i == 1) {
                // left
                posB.x = posA.x + (thisv->axisLeft.x * lineLength);
                posB.y = posA.y + (thisv->axisLeft.y * lineLength);
                posB.z = posA.z + (thisv->axisLeft.z * lineLength);
            } else {
                // right
                posB.x = posA.x - (thisv->axisLeft.x * lineLength);
                posB.y = posA.y - (thisv->axisLeft.y * lineLength);
                posB.z = posA.z - (thisv->axisLeft.z * lineLength);
            }

            if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &posA, &posB, &posSide, &polySide, true, true, true, true,
                                        &bgIdSide) &&
                !(func_80041DB8(&globalCtx->colCtx, polySide, bgIdSide) & 0x30) &&
                !SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, polySide, bgIdSide)) {
                EnBomChu_UpdateFloorPoly(thisv, polySide, globalCtx);
                thisv->actor.world.pos = posSide;
                thisv->actor.floorBgId = bgIdSide;
                break;
            }
        }

        if (i == 3) {
            // no collision nearby
            EnBomChu_Explode(thisv, globalCtx);
        }
    }

    Math_ScaledStepToS(&thisv->actor.shape.rot.x, -thisv->actor.world.rot.x, 0x800);
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 0x800);
    Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->actor.world.rot.z, 0x800);

    func_8002F8F0(&thisv->actor, NA_SE_IT_BOMBCHU_MOVE - SFX_FLAG);
}

void EnBomChu_WaitForKill(EnBomChu* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        Actor_Kill(&thisv->actor);
    }
}

/**
 * Transform coordinates from model space to world space, according to current orientation.
 * `posModel` is expected to already be at world scale (1/100 compared to model scale)
 */
void EnBomChu_ModelToWorld(EnBomChu* thisv, Vec3f* posModel, Vec3f* dest) {
    f32 x = posModel->x + thisv->visualJitter;

    dest->x = thisv->actor.world.pos.x + (thisv->axisLeft.x * x) + (thisv->axisUp.x * posModel->y) +
              (thisv->axisForwards.x * posModel->z);
    dest->y = thisv->actor.world.pos.y + (thisv->axisLeft.y * x) + (thisv->axisUp.y * posModel->y) +
              (thisv->axisForwards.y * posModel->z);
    dest->z = thisv->actor.world.pos.z + (thisv->axisLeft.z * x) + (thisv->axisUp.z * posModel->y) +
              (thisv->axisForwards.z * posModel->z);
}

void EnBomChu_SpawnRipples(EnBomChu* thisv, GlobalContext* globalCtx, f32 y) {
    Vec3f pos;

    pos.x = thisv->actor.world.pos.x;
    pos.y = y;
    pos.z = thisv->actor.world.pos.z;

    EffectSsGRipple_Spawn(globalCtx, &pos, 70, 500, 0);
    EffectSsGRipple_Spawn(globalCtx, &pos, 70, 500, 4);
    EffectSsGRipple_Spawn(globalCtx, &pos, 70, 500, 8);
}

void EnBomChu_Update(Actor* thisx, GlobalContext* globalCtx2) {
    static Vec3f blureP1Model = { 0.0f, 7.0f, -6.0f };
    static Vec3f blureP2LeftModel = { 12.0f, 0.0f, -5.0f };
    static Vec3f blureP2RightModel = { -12.0f, 0.0f, -5.0f };
    GlobalContext* globalCtx = globalCtx2;
    EnBomChu* thisv = (EnBomChu*)thisx;
    s16 yaw;
    f32 sin;
    f32 cos;
    f32 tempX;
    Vec3f blureP1;
    Vec3f blureP2;
    WaterBox* waterBox;
    f32 waterY;

    if (thisv->actor.floorBgId != BGCHECK_SCENE) {
        yaw = thisv->actor.shape.rot.y;
        func_800433A4(&globalCtx->colCtx, thisv->actor.floorBgId, &thisv->actor);

        if (yaw != thisv->actor.shape.rot.y) {
            yaw = thisv->actor.shape.rot.y - yaw;

            sin = Math_SinS(yaw);
            cos = Math_CosS(yaw);

            tempX = thisv->axisForwards.x;
            thisv->axisForwards.x = (thisv->axisForwards.z * sin) + (cos * tempX);
            thisv->axisForwards.z = (thisv->axisForwards.z * cos) - (sin * tempX);

            tempX = thisv->axisUp.x;
            thisv->axisUp.x = (thisv->axisUp.z * sin) + (cos * tempX);
            thisv->axisUp.z = (thisv->axisUp.z * cos) - (sin * tempX);

            tempX = thisv->axisLeft.x;
            thisv->axisLeft.x = (thisv->axisLeft.z * sin) + (cos * tempX);
            thisv->axisLeft.z = (thisv->axisLeft.z * cos) - (sin * tempX);
        }
    }

    thisv->actionFunc(thisv, globalCtx);
    func_8002D97C(&thisv->actor);

    thisv->collider.elements[0].dim.worldSphere.center.x = thisv->actor.world.pos.x;
    thisv->collider.elements[0].dim.worldSphere.center.y = thisv->actor.world.pos.y;
    thisv->collider.elements[0].dim.worldSphere.center.z = thisv->actor.world.pos.z;

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    if (thisv->actionFunc != EnBomChu_WaitForRelease) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    Actor_SetFocus(&thisv->actor, 0.0f);

    if (thisv->actionFunc == EnBomChu_Move) {
        thisv->visualJitter =
            (5.0f + (Rand_ZeroOne() * 3.0f)) * Math_SinS(((Rand_ZeroOne() * (f32)0x200) + (f32)0x3000) * thisv->timer);

        EnBomChu_ModelToWorld(thisv, &blureP1Model, &blureP1);

        EnBomChu_ModelToWorld(thisv, &blureP2LeftModel, &blureP2);
        EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blure1Index)), &blureP1, &blureP2);

        EnBomChu_ModelToWorld(thisv, &blureP2RightModel, &blureP2);
        EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blure2Index)), &blureP1, &blureP2);

        waterY = thisv->actor.world.pos.y;

        if (WaterBox_GetSurface1(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                 &waterY, &waterBox)) {
            thisv->actor.yDistToWater = waterY - thisv->actor.world.pos.y;

            if (thisv->actor.yDistToWater < 0.0f) {
                if (thisv->actor.bgCheckFlags & 0x20) {
                    EnBomChu_SpawnRipples(thisv, globalCtx, waterY);
                }

                thisv->actor.bgCheckFlags &= ~0x20;
            } else {
                if (!(thisv->actor.bgCheckFlags & 0x20) && (thisv->timer != 120)) {
                    EnBomChu_SpawnRipples(thisv, globalCtx, waterY);
                } else {
                    EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 0.0f, 3.0f, 15.0f, 0.25f);
                }

                thisv->actor.bgCheckFlags |= 0x20;
            }
        } else {
            thisv->actor.bgCheckFlags &= ~0x20;
            thisv->actor.yDistToWater = BGCHECK_Y_MIN;
        }
    }
}

void EnBomChu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBomChu* thisv = (EnBomChu*)thisx;
    f32 colorIntensity;
    s32 blinkHalfPeriod;
    s32 blinkTime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bom_chu.c", 921);

    func_80093D18(globalCtx->state.gfxCtx);
    func_8002EBCC(&thisv->actor, globalCtx, 0);

    if (thisv->timer >= 40) {
        blinkTime = thisv->timer % 20;
        blinkHalfPeriod = 10;
    } else if (thisv->timer >= 10) {
        blinkTime = thisv->timer % 10;
        blinkHalfPeriod = 5;
    } else {
        blinkTime = thisv->timer & 1;
        blinkHalfPeriod = 1;
    }

    if (blinkTime > blinkHalfPeriod) {
        blinkTime = 2 * blinkHalfPeriod - blinkTime;
    }

    colorIntensity = blinkTime / (f32)blinkHalfPeriod;

    gDPSetEnvColor(POLY_OPA_DISP++, 9.0f + (colorIntensity * 209.0f), 9.0f + (colorIntensity * 34.0f),
                   35.0f + (colorIntensity * -35.0f), 255);
    Matrix_Translate(thisv->visualJitter * (1.0f / BOMBCHU_SCALE), 0.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bom_chu.c", 956),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBombchuDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bom_chu.c", 961);
}
