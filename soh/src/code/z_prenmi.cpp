#include "global.h"
#include "vt.h"

void func_80092320(PreNMIContext* thisv) {
    thisv->state.running = false;
    thisv->state.init = NULL;
    thisv->state.size = 0;
}

void PreNMI_Update(PreNMIContext* thisv) {
    osSyncPrintf(VT_COL(YELLOW, BLACK) "prenmi_move\n" VT_RST);

    // Strings existing only in rodata
    if (0) {
        osSyncPrintf("../z_prenmi.c");
        osSyncPrintf("(int)volume = %d\n");
    }

    if (thisv->timer == 0) {
        ViConfig_UpdateVi(1);
        func_80092320(thisv);
        return;
    }

    thisv->timer--;
}

void PreNMI_Draw(PreNMIContext* thisv) {
    GraphicsContext* gfxCtx = thisv->state.gfxCtx;

    osSyncPrintf(VT_COL(YELLOW, BLACK) "prenmi_draw\n" VT_RST);

    OPEN_DISPS(gfxCtx, "../z_prenmi.c", 96);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    func_80095248(gfxCtx, 0, 0, 0);
    func_800940B0(gfxCtx);
    gDPSetFillColor(POLY_OPA_DISP++, (GPACK_RGBA5551(255, 255, 255, 1) << 16) | GPACK_RGBA5551(255, 255, 255, 1));
    gDPFillRectangle(POLY_OPA_DISP++, 0, thisv->timer + 100, SCREEN_WIDTH - 1, thisv->timer + 100);

    CLOSE_DISPS(gfxCtx, "../z_prenmi.c", 112);
}

void PreNMI_Main(GameState* thisvx) {
    PreNMIContext* thisv = (PreNMIContext*)thisvx;

    PreNMI_Update(thisv);
    PreNMI_Draw(thisv);

    thisv->state.unk_A0 = 1;
}

void PreNMI_Destroy(GameState* thisvx) {
}

void PreNMI_Init(GameState* thisvx) {
    PreNMIContext* thisv = (PreNMIContext*)thisvx;

    thisv->state.main = PreNMI_Main;
    thisv->state.destroy = PreNMI_Destroy;
    thisv->timer = 30;
    thisv->unk_A8 = 10;

    R_UPDATE_RATE = 1;
}
