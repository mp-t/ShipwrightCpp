/*
 * File: z_eff_ss_hitmark.c
 * Overlay: ovl_Effect_Ss_HitMark
 * Description: Hit Marks
 */

#include "z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rTexIdx regs[0]
#define rType regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rEnvColorR regs[5]
#define rEnvColorG regs[6]
#define rEnvColorB regs[7]
#define rScale regs[8]

u32 EffectSsHitMark_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsHitMark_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsHitMark_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

static Color_RGB8 sColors[] = {
    { 255, 255, 255 }, { 255, 255, 0 }, { 255, 255, 255 }, { 255, 0, 0 },   { 255, 200, 100 }, { 200, 150, 0 },
    { 150, 100, 0 },   { 100, 50, 0 },  { 255, 255, 255 }, { 255, 0, 0 },   { 255, 255, 0 },   { 255, 0, 0 },
    { 255, 255, 255 }, { 0, 255, 200 }, { 255, 255, 255 }, { 150, 0, 255 },
};

static const void* sTextures[] = {
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,  gEffHitMark9Tex,  gEffHitMark10Tex, gEffHitMark11Tex, gEffHitMark12Tex,
    gEffHitMark13Tex, gEffHitMark14Tex, gEffHitMark15Tex, gEffHitMark16Tex, gEffHitMark17Tex, gEffHitMark18Tex,
    gEffHitMark19Tex, gEffHitMark20Tex, gEffHitMark21Tex, gEffHitMark22Tex, gEffHitMark23Tex, gEffHitMark24Tex,
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,
};

EffectSsInit Effect_Ss_HitMark_InitVars = {
    EFFECT_SS_HITMARK,
    EffectSsHitMark_Init,
};

u32 EffectSsHitMark_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    s32 colorIdx;
    EffectSsHitMarkInitParams* initParams = (EffectSsHitMarkInitParams*)initParamsx;
    thisv->pos = initParams->pos;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffHitMarkDL);

    if (initParams->type == EFFECT_HITMARK_DUST) {
        thisv->life = 16;
    } else {
        thisv->life = 8;
    }

    thisv->draw = EffectSsHitMark_Draw;
    thisv->update = EffectSsHitMark_Update;
    colorIdx = initParams->type * 4;
    thisv->rTexIdx = 0;
    thisv->rType = initParams->type;
    thisv->rPrimColorR = sColors[colorIdx].r;
    thisv->rPrimColorG = sColors[colorIdx].g;
    thisv->rPrimColorB = sColors[colorIdx].b;
    thisv->rEnvColorR = sColors[colorIdx + 1].r;
    thisv->rEnvColorG = sColors[colorIdx + 1].g;
    thisv->rEnvColorB = sColors[colorIdx + 1].b;
    thisv->rScale = initParams->scale;

    return 1;
}

void EffectSsHitMark_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_hitmark.c", 297);

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    scale = thisv->rScale / 100.0f;
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[(thisv->rType * 8) + (thisv->rTexIdx)]));
        func_80094C50(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, 0);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }
    CLOSE_DISPS(gfxCtx, "../z_eff_ss_hitmark.c", 341);
}

void EffectSsHitMark_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 colorIdx;

    if (thisv->rType == EFFECT_HITMARK_DUST) {
        thisv->rTexIdx = (15 - thisv->life) / 2;
    } else {
        thisv->rTexIdx = 7 - thisv->life;
    }

    if (thisv->rTexIdx != 0) {
        colorIdx = thisv->rType * 4 + 2;
        thisv->rPrimColorR = func_80027DD4(thisv->rPrimColorR, sColors[colorIdx].r, thisv->life + 1);
        thisv->rPrimColorG = func_80027DD4(thisv->rPrimColorG, sColors[colorIdx].g, thisv->life + 1);
        thisv->rPrimColorB = func_80027DD4(thisv->rPrimColorB, sColors[colorIdx].b, thisv->life + 1);
        thisv->rEnvColorR = func_80027DD4(thisv->rEnvColorR, sColors[colorIdx + 1].r, thisv->life + 1);
        thisv->rEnvColorG = func_80027DD4(thisv->rEnvColorG, sColors[colorIdx + 1].g, thisv->life + 1);
        thisv->rEnvColorB = func_80027DD4(thisv->rEnvColorB, sColors[colorIdx + 1].b, thisv->life + 1);
    }
}
