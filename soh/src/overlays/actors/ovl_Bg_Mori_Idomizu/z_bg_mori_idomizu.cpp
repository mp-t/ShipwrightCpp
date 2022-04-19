/*
 * File: z_bg_mori_idomizu.c
 * Overlay: ovl_Bg_Mori_Idomizu
 * Description: Square of water in Forest Temple well
 */

#include "z_bg_mori_idomizu.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMoriIdomizu_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriIdomizu_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriIdomizu_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriIdomizu_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriIdomizu_SetupWaitForMoriTex(BgMoriIdomizu* thisv);
void BgMoriIdomizu_WaitForMoriTex(BgMoriIdomizu* thisv, GlobalContext* globalCtx);
void BgMoriIdomizu_SetupMain(BgMoriIdomizu* thisv);
void BgMoriIdomizu_Main(BgMoriIdomizu* thisv, GlobalContext* globalCtx);

static s16 sIsSpawned = false;

ActorInit Bg_Mori_Idomizu_InitVars = {
    ACTOR_BG_MORI_IDOMIZU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriIdomizu),
    (ActorFunc)BgMoriIdomizu_Init,
    (ActorFunc)BgMoriIdomizu_Destroy,
    (ActorFunc)BgMoriIdomizu_Update,
    NULL,
    NULL,
};

void BgMoriIdomizu_SetupAction(BgMoriIdomizu* thisv, BgMoriIdomizuActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgMoriIdomizu_SetWaterLevel(GlobalContext* globalCtx, s16 waterLevel) {
    WaterBox* waterBox = globalCtx->colCtx.colHeader->waterBoxes;

    waterBox[2].ySurface = waterLevel;
    waterBox[3].ySurface = waterLevel;
    waterBox[4].ySurface = waterLevel;
}

void BgMoriIdomizu_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* thisv = (BgMoriIdomizu*)thisvx;

    if (sIsSpawned) {
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->actor.scale.x = 1.1f;
    thisv->actor.scale.y = 1.0f;
    thisv->actor.scale.z = 1.0f;
    thisv->actor.world.pos.x = 119.0f;
    thisv->actor.world.pos.z = -1820.0f;
    thisv->prevSwitchFlagSet = Flags_GetSwitch(globalCtx, thisv->actor.params & 0x3F);
    if (thisv->prevSwitchFlagSet != 0) {
        thisv->actor.world.pos.y = -282.0f;
        BgMoriIdomizu_SetWaterLevel(globalCtx, -282);
    } else {
        thisv->actor.world.pos.y = 184.0f;
        BgMoriIdomizu_SetWaterLevel(globalCtx, 184);
    }
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        Actor_Kill(&thisv->actor);
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", thisv->actor.params, "../z_bg_mori_idomizu.c",
                     202);
        return;
    }
    BgMoriIdomizu_SetupWaitForMoriTex(thisv);
    sIsSpawned = true;
    thisv->isLoaded = true;
    thisv->actor.room = -1;
    // "Forest Temple well water"
    osSyncPrintf("(森の神殿 井戸水)(arg_data 0x%04x)\n", thisv->actor.params);
}

void BgMoriIdomizu_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* thisv = (BgMoriIdomizu*)thisvx;

    if (thisv->isLoaded) {
        sIsSpawned = false;
    }
}

void BgMoriIdomizu_SetupWaitForMoriTex(BgMoriIdomizu* thisv) {
    BgMoriIdomizu_SetupAction(thisv, BgMoriIdomizu_WaitForMoriTex);
}

void BgMoriIdomizu_WaitForMoriTex(BgMoriIdomizu* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        BgMoriIdomizu_SetupMain(thisv);
        thisv->actor.draw = BgMoriIdomizu_Draw;
    }
}

void BgMoriIdomizu_SetupMain(BgMoriIdomizu* thisv) {
    BgMoriIdomizu_SetupAction(thisv, BgMoriIdomizu_Main);
}

void BgMoriIdomizu_Main(BgMoriIdomizu* thisv, GlobalContext* globalCtx) {
    s8 roomNum;
    Actor* thisvx = &thisv->actor;
    s32 switchFlagSet;

    roomNum = globalCtx->roomCtx.curRoom.num;
    switchFlagSet = Flags_GetSwitch(globalCtx, thisvx->params & 0x3F);
    if (switchFlagSet) {
        thisv->targetWaterLevel = -282.0f;
    } else {
        thisv->targetWaterLevel = 184.0f;
    }
    if (switchFlagSet && !thisv->prevSwitchFlagSet) {
        OnePointCutscene_Init(globalCtx, 3240, 70, thisvx, MAIN_CAM);
        thisv->drainTimer = 90;
    } else if (!switchFlagSet && thisv->prevSwitchFlagSet) {
        OnePointCutscene_Init(globalCtx, 3240, 70, thisvx, MAIN_CAM);
        thisv->drainTimer = 90;
        thisvx->world.pos.y = 0.0f;
    }
    thisv->drainTimer--;
    if ((roomNum == 7) || (roomNum == 8) || (roomNum == 9)) {
        if (thisv->drainTimer < 70) {
            Math_StepToF(&thisvx->world.pos.y, thisv->targetWaterLevel, 3.5f);
            BgMoriIdomizu_SetWaterLevel(globalCtx, thisvx->world.pos.y);
            if (thisv->drainTimer > 0) {
                if (switchFlagSet) {
                    func_800788CC(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                } else {
                    func_800788CC(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                }
            }
        }
    } else {
        thisvx->world.pos.y = thisv->targetWaterLevel;
        BgMoriIdomizu_SetWaterLevel(globalCtx, thisvx->world.pos.y);
        Actor_Kill(thisvx);
        return;
    }
    thisv->prevSwitchFlagSet = switchFlagSet;
}

void BgMoriIdomizu_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* thisv = (BgMoriIdomizu*)thisvx;

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgMoriIdomizu_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriIdomizu* thisv = (BgMoriIdomizu*)thisvx;
    u32 gameplayFrames = globalCtx->gameplayFrames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 356);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 360),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPSegment(POLY_XLU_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 128);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0x7F - (gameplayFrames & 0x7F), gameplayFrames % 0x80, 0x20,
                                0x20, 1, gameplayFrames & 0x7F, gameplayFrames % 0x80, 0x20, 0x20));

    gSPDisplayList(POLY_XLU_DISP++, gMoriIdomizuWaterDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_idomizu.c", 382);
}
