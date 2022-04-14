/*
 * File: z_eff_ss_g_ripple.c
 * Overlay: ovl_Effect_Ss_G_Ripple
 * Description: Water Ripple
 */

#include "z_eff_ss_g_ripple.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rWaterBoxNum regs[0]
#define rRadius regs[1]
#define rRadiusMax regs[2]
#define rPrimColorR regs[3]
#define rPrimColorG regs[4]
#define rPrimColorB regs[5]
#define rPrimColorA regs[6]
#define rEnvColorR regs[7]
#define rEnvColorG regs[8]
#define rEnvColorB regs[9]
#define rEnvColorA regs[10]
#define rLifespan regs[11]

u32 EffectSsGRipple_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsGRipple_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGRipple_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_G_Ripple_InitVars = {
    EFFECT_SS_G_RIPPLE,
    EffectSsGRipple_Init,
};

u32 EffectSsGRipple_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    s32 pad;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    WaterBox* waterBox;
    EffectSsGRippleInitParams* initParams = (EffectSsGRippleInitParams*)initParamsx;

    waterBox = NULL;
    thisv->velocity = thisv->accel = zeroVec;
    thisv->pos = initParams->pos;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffWaterRippleDL);
    thisv->life = initParams->life + 20;
    thisv->flags = 0;
    thisv->draw = EffectSsGRipple_Draw;
    thisv->update = EffectSsGRipple_Update;
    thisv->rRadius = initParams->radius;
    thisv->rRadiusMax = initParams->radiusMax;
    thisv->rLifespan = initParams->life;
    thisv->rPrimColorR = 255;
    thisv->rPrimColorG = 255;
    thisv->rPrimColorB = 255;
    thisv->rPrimColorA = 255;
    thisv->rEnvColorR = 255;
    thisv->rEnvColorG = 255;
    thisv->rEnvColorB = 255;
    thisv->rEnvColorA = 255;
    thisv->rWaterBoxNum = WaterBox_GetSurface2(globalCtx, &globalCtx->colCtx, &initParams->pos, 3.0f, &waterBox);

    return 1;
}

void EffectSsGRipple_DrawRipple(GlobalContext* globalCtx, EffectSs* thisv, const void* segment) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    f32 radius;
    s32 pad;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    Mtx* mtx;
    f32 yPos;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_g_ripple.c", 199);

    if (globalCtx) {}

    radius = thisv->rRadius * 0.0025f;

    if ((thisv->rWaterBoxNum != -1) && (thisv->rWaterBoxNum < globalCtx->colCtx.colHeader->numWaterBoxes)) {
        yPos = (thisv->rWaterBoxNum + globalCtx->colCtx.colHeader->waterBoxes)->ySurface;
    } else {
        yPos = thisv->pos.y;
    }

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, yPos, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, radius, radius, radius);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094BC4(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                        thisv->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_g_ripple.c", 247);
}

void EffectSsGRipple_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->rLifespan == 0) {
        EffectSsGRipple_DrawRipple(globalCtx, thisv, gEffWaterRippleTex);
    }
}

void EffectSsGRipple_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    f32 radius;
    f32 primAlpha;
    f32 envAlpha;

    if (DECR(thisv->rLifespan) == 0) {
        radius = thisv->rRadius;
        Math_SmoothStepToF(&radius, thisv->rRadiusMax, 0.2f, 30.0f, 1.0f);
        thisv->rRadius = radius;

        primAlpha = thisv->rPrimColorA;
        envAlpha = thisv->rEnvColorA;

        Math_SmoothStepToF(&primAlpha, 0.0f, 0.2f, 15.0f, 7.0f);
        Math_SmoothStepToF(&envAlpha, 0.0f, 0.2f, 15.0f, 7.0f);

        thisv->rPrimColorA = primAlpha;
        thisv->rEnvColorA = envAlpha;
    }
}
