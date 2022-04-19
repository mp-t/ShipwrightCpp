/*
 * File: z_eff_ss_stone1.c
 * Overlay: ovl_Effect_Ss_Stone1
 * Description:
 */

#include "z_eff_ss_stone1.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rReg0 regs[0]

u32 EffectSsStone1_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsStone1_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsStone1_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Stone1_InitVars = {
    EFFECT_SS_STONE1,
    EffectSsStone1_Init,
};

typedef struct {
    /* 0x00 */ const void* texture;
    /* 0x04 */ Color_RGBA8 primColor;
    /* 0x08 */ Color_RGBA8 envColor;
} EffStoneDrawInfo;

static EffStoneDrawInfo sDrawInfo[] = {
    { gUnknownEffStone8Tex, { 200, 0, 0, 255 }, { 0, 0, 0, 255 } },
    { gUnknownEffStone7Tex, { 255, 100, 0, 255 }, { 100, 0, 0, 255 } },
    { gUnknownEffStone6Tex, { 255, 200, 0, 255 }, { 200, 0, 0, 255 } },
    { gUnknownEffStone5Tex, { 255, 255, 0, 255 }, { 255, 0, 0, 255 } },
    { gUnknownEffStone4Tex, { 255, 255, 150, 255 }, { 255, 150, 0, 255 } },
    { gUnknownEffStone3Tex, { 255, 255, 255, 255 }, { 255, 255, 0, 255 } },
    { gUnknownEffStone2Tex, { 255, 255, 255, 255 }, { 0, 255, 0, 255 } },
    { gUnknownEffStone1Tex, { 255, 255, 255, 255 }, { 0, 255, 255, 255 } },
};

u32 EffectSsStone1_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsStone1InitParams* initParams = (EffectSsStone1InitParams*)initParamsx;
    Vec3f pos = initParams->pos;

    thisv->pos = pos;
    thisv->vec = pos;
    thisv->life = 8;
    thisv->rReg0 = initParams->unk_C;
    thisv->draw = EffectSsStone1_Draw;
    thisv->update = EffectSsStone1_Update;

    return 1;
}

void EffectSsStone1_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    EffStoneDrawInfo* drawParams = &sDrawInfo[thisv->life];
    Vec3f mfVec;
    f32 mfW;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_stone1.c", 154);

    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->pos, &mfVec, &mfW);
    scale = (mfW < 1500.0f) ? 3.0f : (mfW / 1500.0f) * 3.0f;
    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_stone1.c", 168),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80094C50(gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(drawParams->texture));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, drawParams->primColor.r, drawParams->primColor.g, drawParams->primColor.b,
                    255);
    gDPSetEnvColor(POLY_XLU_DISP++, drawParams->envColor.r, drawParams->envColor.g, drawParams->envColor.b, 255);
    gSPDisplayList(POLY_XLU_DISP++, gUnknownEffStoneDL);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_stone1.c", 183);
}

void EffectSsStone1_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if ((thisv->life == 6) && (thisv->rReg0 != 0)) {
        iREG(50) = 0;
    }
}
