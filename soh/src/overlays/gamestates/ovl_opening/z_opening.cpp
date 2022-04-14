/*
 * File: z_opening.c
 * Overlay: ovl_opening
 * Description: Initializes the game into the title screen
 */

#include "global.h"

void Opening_SetupTitleScreen(OpeningContext* thisv) {
    gSaveContext.gameMode = 1;
    thisv->state.running = false;
    gSaveContext.linkAge = 0;
    Sram_InitDebugSave();
    gSaveContext.cutsceneIndex = 0xFFF3;
    gSaveContext.sceneSetupIndex = 7;
    SET_NEXT_GAMESTATE(&thisv->state, Gameplay_Init, GlobalContext);
}

void func_80803C5C(OpeningContext* thisv) {
}

void Opening_Main(GameState* thisx) {
    OpeningContext* thisv = (OpeningContext*)thisx;

    func_80095248(thisv->state.gfxCtx, 0, 0, 0);
    Opening_SetupTitleScreen(thisv);
    func_80803C5C(thisv);
}

void Opening_Destroy(GameState* thisx) {
}

void Opening_Init(GameState* thisx) {
    OpeningContext* thisv = (OpeningContext*)thisx;

    R_UPDATE_RATE = 1;
    Matrix_Init(&thisv->state);
    View_Init(&thisv->view, thisv->state.gfxCtx);
    thisv->state.main = Opening_Main;
    thisv->state.destroy = Opening_Destroy;
}
