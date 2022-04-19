/*
 * File: z_bg_spot01_idomizu.c
 * Overlay: ovl_Bg_Spot01_Idomizu
 * Description: Kakariko Village Well Water
 */

#include "z_bg_spot01_idomizu.h"
#include "objects/object_spot01_objects/object_spot01_objects.h"

#define FLAGS ACTOR_FLAG_5

void BgSpot01Idomizu_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idomizu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idomizu_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idomizu_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808ABB84(BgSpot01Idomizu* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot01_Idomizu_InitVars = {
    ACTOR_BG_SPOT01_IDOMIZU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT01_OBJECTS,
    sizeof(BgSpot01Idomizu),
    (ActorFunc)BgSpot01Idomizu_Init,
    (ActorFunc)BgSpot01Idomizu_Destroy,
    (ActorFunc)BgSpot01Idomizu_Update,
    (ActorFunc)BgSpot01Idomizu_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpot01Idomizu_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot01Idomizu* thisv = (BgSpot01Idomizu*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    if (gSaveContext.eventChkInf[6] & 0x80 || LINK_AGE_IN_YEARS == YEARS_ADULT) {
        thisv->waterHeight = -550.0f;
    } else {
        thisv->waterHeight = 52.0f;
    }
    thisv->actionFunc = func_808ABB84;
    thisv->actor.world.pos.y = thisv->waterHeight;
}

void BgSpot01Idomizu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_808ABB84(BgSpot01Idomizu* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[6] & 0x80) {
        thisv->waterHeight = -550.0f;
    }
    globalCtx->colCtx.colHeader->waterBoxes[0].ySurface = thisv->actor.world.pos.y;
    if (thisv->waterHeight < thisv->actor.world.pos.y) {
        Audio_PlaySoundGeneral(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->waterHeight, 1.0f, 2.0f);
}

void BgSpot01Idomizu_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot01Idomizu* thisv = (BgSpot01Idomizu*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot01Idomizu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    u32 frames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot01_idomizu.c", 228);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot01_idomizu.c", 232),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    frames = globalCtx->state.frames;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 127 - frames % 128, frames & 0x7F, 32, 32, 1, frames % 128,
                                frames & 0x7F, 32, 32));

    gSPDisplayList(POLY_XLU_DISP++, gKakarikoWellWaterDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot01_idomizu.c", 244);
}
