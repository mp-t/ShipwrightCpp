/*
 * File: z_bg_spot06_objects.c
 * Overlay: ovl_Bg_Spot06_Objects
 * Description: Lake Hylia Objects
 */

#include "z_bg_spot06_objects.h"
#include "objects/object_spot06_objects/object_spot06_objects.h"

#define FLAGS ACTOR_FLAG_9

typedef enum {
    /* 0x0 */ LHO_WATER_TEMPLE_ENTRACE_GATE,
    /* 0x1 */ LHO_WATER_TEMPLE_ENTRANCE_LOCK,
    /* 0x2 */ LHO_WATER_PLANE,
    /* 0x3 */ LHO_ICE_BLOCK
} LakeHyliaObjectsType;

typedef enum {
    /* 0x0 */ LHWB_GERUDO_VALLEY_RIVER_UPPER, // entrance from Gerudo Valley
    /* 0x1 */ LHWB_GERUDO_VALLEY_RIVER_LOWER, // river flowing from Gerudo Valley
    /* 0x2 */ LHWB_MAIN_1,                    // main water box
    /* 0x3 */ LHWB_MAIN_2                     // extension of main water box
} LakeHyliaWaterBoxIndices;

// Lake Hylia water plane levels
#define WATER_LEVEL_RAISED (-1313)
#define WATER_LEVEL_RIVER_RAISED (WATER_LEVEL_RAISED + 200)
#define WATER_LEVEL_LOWERED (WATER_LEVEL_RAISED - 680)
#define WATER_LEVEL_RIVER_LOWERED (WATER_LEVEL_RIVER_RAISED - 80)

void BgSpot06Objects_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot06Objects_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot06Objects_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot06Objects_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgSpot06Objects_GateWaitForSwitch(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_GateWaitToOpen(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_GateOpen(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_DoNothing(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_LockWait(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_LockPullOutward(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_LockSwimToSurface(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_LockFloat(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_WaterPlaneCutsceneWait(BgSpot06Objects* thisv, GlobalContext* globalCtx);
void BgSpot06Objects_WaterPlaneCutsceneRise(BgSpot06Objects* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot06_Objects_InitVars = {
    ACTOR_BG_SPOT06_OBJECTS,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SPOT06_OBJECTS,
    sizeof(BgSpot06Objects),
    (ActorFunc)BgSpot06Objects_Init,
    (ActorFunc)BgSpot06Objects_Destroy,
    (ActorFunc)BgSpot06Objects_Update,
    (ActorFunc)BgSpot06Objects_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000080, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, -160 }, 18 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphItemsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static InitChainEntry sInitChainWaterPlane[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgSpot06Objects_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot06Objects* thisv = (BgSpot06Objects*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    thisv->switchFlag = thisx->params & 0xFF;
    thisx->params = (thisx->params >> 8) & 0xFF;

    osSyncPrintf("spot06 obj nthisx->arg_data=[%d]", thisx->params);

    switch (thisx->params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
            Actor_ProcessInitChain(thisx, sInitChain);
            DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
            CollisionHeader_GetVirtual(&gLakeHyliaWaterTempleGateCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);

            if (LINK_IS_ADULT && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
                thisx->world.pos.y = thisx->home.pos.y + 120.0f;
                thisv->actionFunc = BgSpot06Objects_DoNothing;

            } else {
                thisv->actionFunc = BgSpot06Objects_GateWaitForSwitch;
            }

            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            Actor_ProcessInitChain(thisx, sInitChain);
            Collider_InitJntSph(globalCtx, &thisv->collider);
            Collider_SetJntSph(globalCtx, &thisv->collider, thisx, &sJntSphInit, thisv->colliderItem);

            if (LINK_IS_ADULT && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
                if (!(gSaveContext.eventChkInf[6] & 0x200)) {
                    thisx->home.pos.y = thisx->world.pos.y = WATER_LEVEL_LOWERED;
                } else {
                    thisx->home.pos.y = thisx->world.pos.y = WATER_LEVEL_RAISED;
                }

                thisv->actionFunc = BgSpot06Objects_LockFloat;
                thisx->world.pos.z -= 100.0f;
                thisx->home.pos.z = thisx->world.pos.z + 16.0f;
                thisv->collider.elements[0].dim.worldSphere.radius =
                    thisv->collider.elements[0].dim.modelSphere.radius * 2;
                thisv->collider.elements[0].dim.worldSphere.center.z = thisx->world.pos.z + 16.0f;
            } else {
                thisv->actionFunc = BgSpot06Objects_LockWait;
                thisv->collider.elements[0].dim.worldSphere.radius = thisv->collider.elements[0].dim.modelSphere.radius;
                thisv->collider.elements[0].dim.worldSphere.center.z = thisx->world.pos.z;
            }

            thisv->collider.elements[0].dim.worldSphere.center.x = thisx->world.pos.x;
            thisv->collider.elements[0].dim.worldSphere.center.y = thisx->world.pos.y;
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
            break;
        case LHO_WATER_PLANE:
            Actor_ProcessInitChain(thisx, sInitChainWaterPlane);
            thisx->flags = ACTOR_FLAG_4 | ACTOR_FLAG_5;

            if (LINK_IS_ADULT && !(gSaveContext.eventChkInf[6] & 0x200)) {
                if (gSaveContext.sceneSetupIndex < 4) {
                    thisv->lakeHyliaWaterLevel = -681.0f;
                    globalCtx->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].ySurface =
                        WATER_LEVEL_RIVER_LOWERED;
                    globalCtx->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].zMin -= 50;
                    globalCtx->colCtx.colHeader->waterBoxes[LHWB_MAIN_1].ySurface = WATER_LEVEL_LOWERED;
                    globalCtx->colCtx.colHeader->waterBoxes[LHWB_MAIN_2].ySurface = WATER_LEVEL_LOWERED;
                    thisv->actionFunc = BgSpot06Objects_DoNothing;
                } else {
                    thisx->world.pos.y = thisv->lakeHyliaWaterLevel = -681.0f;
                    thisx->world.pos.y += WATER_LEVEL_RAISED;
                    thisv->actionFunc = BgSpot06Objects_WaterPlaneCutsceneWait;
                }
            } else {
                thisv->lakeHyliaWaterLevel = 0.0f;
                thisv->actionFunc = BgSpot06Objects_DoNothing;
            }
            break;
        case LHO_ICE_BLOCK:
            Actor_ProcessInitChain(thisx, sInitChain);
            DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
            CollisionHeader_GetVirtual(&gLakeHyliaZoraShortcutIceblockCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
            thisv->actionFunc = BgSpot06Objects_DoNothing;

            if (!LINK_IS_ADULT) {
                Actor_Kill(thisx);
            }
            break;
    }
}

void BgSpot06Objects_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot06Objects* thisv = (BgSpot06Objects*)thisx;

    switch (thisv->dyna.actor.params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
        case LHO_ICE_BLOCK:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            Collider_DestroyJntSph(globalCtx, &thisv->collider);
            break;
        case LHO_WATER_PLANE:
            break;
    }
}

/**
 * Water Temple entrance gate effect functions
 */
void BgSpot06Objects_GateSpawnBubbles(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    Vec3f sp34;
    f32 tmp;

    if ((globalCtx->gameplayFrames % 3) == 0) {
        tmp = Rand_CenteredFloat(160.0f);
        sp34.x = (Math_SinS(thisv->dyna.actor.shape.rot.y + 0x4000) * tmp) + thisv->dyna.actor.world.pos.x;
        sp34.y = thisv->dyna.actor.world.pos.y;
        sp34.z = (Math_CosS(thisv->dyna.actor.shape.rot.y + 0x4000) * tmp) + thisv->dyna.actor.world.pos.z;
        EffectSsBubble_Spawn(globalCtx, &sp34, 50.0f, 70.0f, 10.0f, (Rand_ZeroOne() * 0.05f) + 0.175f);
    }
}

/**
 * This is where the gate waits for the switch to be set by the fish shaped lock.
 */
void BgSpot06Objects_GateWaitForSwitch(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    s32 i;

    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        thisv->timer = 100;
        thisv->dyna.actor.world.pos.y += 3.0f;
        thisv->actionFunc = BgSpot06Objects_GateWaitToOpen;

        for (i = 0; i < 15; i++) {
            BgSpot06Objects_GateSpawnBubbles(thisv, globalCtx);
        }
    }
}

/**
 * This is where the gate waits a few frames before rising after the switch is set.
 */
void BgSpot06Objects_GateWaitToOpen(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        thisv->actionFunc = BgSpot06Objects_GateOpen;
    }
}

/**
 * This is where the gate finally rises upward.
 */
void BgSpot06Objects_GateOpen(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    BgSpot06Objects_GateSpawnBubbles(thisv, globalCtx);

    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 120.0f, 0.6f)) {
        thisv->actionFunc = BgSpot06Objects_DoNothing;
        thisv->timer = 0;
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void BgSpot06Objects_DoNothing(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
}

/**
 * Fish shaped lock effect functions
 */
void BgSpot06Objects_LockSpawnWaterRipples(BgSpot06Objects* thisv, GlobalContext* globalCtx, s32 flag) {
    if (flag || !(globalCtx->gameplayFrames % 7)) {
        EffectSsGRipple_Spawn(globalCtx, &thisv->dyna.actor.home.pos, 300, 700, 0);
    }
}

void BgSpot06Objects_LockSpawnBubbles(BgSpot06Objects* thisv, GlobalContext* globalCtx, s32 flag) {
    if (!(globalCtx->gameplayFrames % 7) || flag) {
        EffectSsBubble_Spawn(globalCtx, &thisv->dyna.actor.world.pos, 0.0f, 40.0f, 30.0f,
                             (Rand_ZeroOne() * 0.05f) + 0.175f);
    }
}

/**
 * This is where the fish shaped lock waits to be pulled out by the hookshot. Once it does it will spawn bubbles.
 */
void BgSpot06Objects_LockWait(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 i;
    s32 pad2;
    Vec3f effectPos;
    f32 sin;
    f32 cos;

    if (thisv->collider.base.acFlags & 2) {
        thisv->timer = 130;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
        sin = Math_SinS(thisv->dyna.actor.world.rot.y);
        cos = Math_CosS(thisv->dyna.actor.world.rot.y);
        thisv->dyna.actor.world.pos.x += (3.0f * sin);
        thisv->dyna.actor.world.pos.z += (3.0f * cos);

        for (i = 0; i < 20; i++) {
            BgSpot06Objects_LockSpawnBubbles(thisv, globalCtx, 1);
        }

        effectPos.x = thisv->dyna.actor.world.pos.x + (5.0f * sin);
        effectPos.y = thisv->dyna.actor.world.pos.y;
        effectPos.z = thisv->dyna.actor.world.pos.z + (5.0f * cos);

        for (i = 0; i < 3; i++) {
            EffectSsBubble_Spawn(globalCtx, &effectPos, 0.0f, 20.0f, 20.0f, (Rand_ZeroOne() * 0.1f) + 0.7f);
        }

        EffectSsGSplash_Spawn(globalCtx, &thisv->dyna.actor.world.pos, NULL, NULL, 1, 700);
        thisv->collider.elements->dim.worldSphere.radius = 45;
        thisv->actionFunc = BgSpot06Objects_LockPullOutward;
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        OnePointCutscene_Init(globalCtx, 4120, 170, &thisv->dyna.actor, MAIN_CAM);
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

/**
 * Once the fish shaped lock is pulled out from the Hookshot it will move outward.
 */
void BgSpot06Objects_LockPullOutward(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->dyna.actor.world.pos.x += (0.3f * Math_SinS(thisv->dyna.actor.world.rot.y));
    thisv->dyna.actor.world.pos.z += (0.3f * Math_CosS(thisv->dyna.actor.world.rot.y));
    BgSpot06Objects_LockSpawnBubbles(thisv, globalCtx, 0);

    if (thisv->timer == 0) {
        thisv->dyna.actor.velocity.y = 0.5f;
        thisv->dyna.actor.flags &= ~ACTOR_FLAG_13;

        thisv->actionFunc = BgSpot06Objects_LockSwimToSurface;
    }
}

/**
 * After being pulled all the way out the fish shaped lock will rise to the surface, creating bubbles in the water as it
 * does so.
 */
void BgSpot06Objects_LockSwimToSurface(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    f32 cos;
    f32 pad;

    thisv->dyna.actor.world.pos.y += thisv->dyna.actor.velocity.y;

    if (thisv->dyna.actor.velocity.y <= 0.0f) {
        cos = Math_CosS(thisv->dyna.actor.shape.rot.x) * 4.3f;
        thisv->dyna.actor.world.pos.x += (cos * Math_SinS(thisv->dyna.actor.shape.rot.y));
        thisv->dyna.actor.world.pos.z += (cos * Math_CosS(thisv->dyna.actor.shape.rot.y));
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.world.pos.y - 1.3f;
        BgSpot06Objects_LockSpawnWaterRipples(thisv, globalCtx, 0);

        if (Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.x, 0, 0x260) != 0) {
            thisv->dyna.actor.home.pos.x =
                thisv->dyna.actor.world.pos.x - (Math_SinS(thisv->dyna.actor.shape.rot.y) * 16.0f);
            thisv->dyna.actor.home.pos.z =
                thisv->dyna.actor.world.pos.z - (Math_CosS(thisv->dyna.actor.shape.rot.y) * 16.0f);
            thisv->dyna.actor.world.pos.y = -1993.0f;
            thisv->timer = 32;
            thisv->dyna.actor.flags &= ~ACTOR_FLAG_4;
            thisv->collider.elements[0].dim.worldSphere.radius = thisv->collider.elements[0].dim.modelSphere.radius * 2;
            thisv->actionFunc = BgSpot06Objects_LockFloat;
        }
    } else {
        if (thisv->dyna.actor.world.pos.y >= -1973.0f) {
            thisv->dyna.actor.velocity.y = 0.0f;
            BgSpot06Objects_LockSpawnWaterRipples(thisv, globalCtx, 1);
            EffectSsGSplash_Spawn(globalCtx, &thisv->dyna.actor.home.pos, NULL, NULL, 1, 700);
        } else if (thisv->dyna.actor.shape.rot.x == -0x4000) {
            thisv->dyna.actor.velocity.y += 0.02f;
            thisv->dyna.actor.world.pos.x = Rand_CenteredFloat(1.0f) + thisv->dyna.actor.home.pos.x;
            thisv->dyna.actor.world.pos.z = Rand_CenteredFloat(1.0f) + thisv->dyna.actor.home.pos.z;
            thisv->dyna.actor.velocity.y =
                (thisv->dyna.actor.velocity.y > 10.0f) ? (10.0f) : (thisv->dyna.actor.velocity.y);
            BgSpot06Objects_LockSpawnBubbles(thisv, globalCtx, 0);
        } else {
            BgSpot06Objects_LockSpawnBubbles(thisv, globalCtx, 0);

            if (Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.x, -0x4000, 0x30)) {
                thisv->dyna.actor.home.pos.x = thisv->dyna.actor.world.pos.x;
                thisv->dyna.actor.home.pos.y = -1993.0f;
                thisv->dyna.actor.home.pos.z = thisv->dyna.actor.world.pos.z;
            }
        }
    }
}

/**
 * Once the fish shaped lock finishes rising to the surface it will float and create ripples in the water every few
 * frames.
 */
void BgSpot06Objects_LockFloat(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    BgSpot06Objects_LockSpawnWaterRipples(thisv, globalCtx, 0);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->dyna.actor.world.pos.y = (2.0f * sinf(thisv->timer * (std::numbers::pi_v<float> / 16.0f))) + thisv->dyna.actor.home.pos.y;

    if (thisv->timer == 0) {
        thisv->timer = 32;
    }
}

void BgSpot06Objects_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot06Objects* thisv = (BgSpot06Objects*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    if (thisx->params == LHO_WATER_TEMPLE_ENTRANCE_LOCK) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

/**
 * Draw the Lake Hylia water plane, and scroll its texture
 */
void BgSpot06Objects_DrawLakeHyliaWater(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 gameplayFrames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot06_objects.c", 844);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot06_objects.c", 850),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gameplayFrames = globalCtx->state.frames;

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -gameplayFrames, gameplayFrames, 32, 32, 1, gameplayFrames,
                                gameplayFrames, 32, 32));
    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -gameplayFrames, gameplayFrames * 6, 32, 32, 1,
                                gameplayFrames, gameplayFrames * 6, 32, 32));

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);

    if ((thisv->lakeHyliaWaterLevel < -680.0f) && (gSaveContext.sceneSetupIndex < 4)) {
        gSPDisplayList(POLY_XLU_DISP++, gLakeHyliaLowWaterDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gLakeHyliaHighWaterDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot06_objects.c", 879);
}

void BgSpot06Objects_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot06Objects* thisv = (BgSpot06Objects*)thisx;

    switch (thisv->dyna.actor.params) {
        case LHO_WATER_TEMPLE_ENTRACE_GATE:
            Gfx_DrawDListOpa(globalCtx, gLakeHyliaWaterTempleGateDL);
            break;
        case LHO_WATER_TEMPLE_ENTRANCE_LOCK:
            Gfx_DrawDListOpa(globalCtx, gLakeHyliaWaterTempleKeyDL);

            if (thisv->actionFunc == BgSpot06Objects_LockSwimToSurface) {
                Collider_UpdateSpheres(1, &thisv->collider);
            }
            break;
        case LHO_WATER_PLANE:
            BgSpot06Objects_DrawLakeHyliaWater(thisv, globalCtx);
            break;
        case LHO_ICE_BLOCK:
            Gfx_DrawDListOpa(globalCtx, gLakeHyliaZoraShortcutIceblockDL);
            break;
    }
}

/**
 * This is where the Lake Hylia water plane waits for the cutscene to set the water risen flag after the Water Temple is
 * cleared.
 */
void BgSpot06Objects_WaterPlaneCutsceneWait(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[6] & 0x200) {
        thisv->actionFunc = BgSpot06Objects_WaterPlaneCutsceneRise;
    }
}

/**
 * This is where the Lake Hylia water plane rises in the cutscene after the Water Temple is cleared.
 */
void BgSpot06Objects_WaterPlaneCutsceneRise(BgSpot06Objects* thisv, GlobalContext* globalCtx) {
    s32 pad;

    thisv->dyna.actor.world.pos.y = thisv->lakeHyliaWaterLevel + WATER_LEVEL_RAISED;

    if (thisv->lakeHyliaWaterLevel >= 0.0001f) {
        thisv->dyna.actor.world.pos.y = WATER_LEVEL_RAISED;
        thisv->actionFunc = BgSpot06Objects_DoNothing;
    } else {
        Math_SmoothStepToF(&thisv->lakeHyliaWaterLevel, 1.0f, 0.1f, 1.0f, 0.001f);
        globalCtx->colCtx.colHeader->waterBoxes[LHWB_GERUDO_VALLEY_RIVER_LOWER].ySurface = WATER_LEVEL_RIVER_LOWERED;
        globalCtx->colCtx.colHeader->waterBoxes[LHWB_MAIN_1].ySurface = thisv->dyna.actor.world.pos.y;
        globalCtx->colCtx.colHeader->waterBoxes[LHWB_MAIN_2].ySurface = thisv->dyna.actor.world.pos.y;
    }

    func_8002F948(&thisv->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
}
