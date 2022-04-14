/*
 * File: z_eff_ss_dead_dd.c
 * Overlay: ovl_Effect_Ss_Dead_Dd
 * Description:
 */

#include "z_eff_ss_dead_dd.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rAlpha regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rAlphaStep regs[10]
#define rAlphaMode regs[11] // if mode is 0 alpha decreases over time, otherwise it increases

u32 EffectSsDeadDd_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDeadDd_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDeadDd_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Dead_Dd_InitVars = {
    EFFECT_SS_DEAD_DD,
    EffectSsDeadDd_Init,
};

u32 EffectSsDeadDd_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    s32 i;
    EffectSsDeadDdInitParams* initParams = (EffectSsDeadDdInitParams*)initParamsx;

    if (initParams->type == 0) {
        thisv->pos = initParams->pos;
        thisv->velocity = initParams->velocity;
        thisv->accel = initParams->accel;
        thisv->life = initParams->life;
        thisv->rScaleStep = initParams->scaleStep;
        thisv->rAlphaMode = initParams->alphaStep;

        if (initParams->alphaStep != 0) {
            thisv->rAlphaStep = initParams->alphaStep;
        } else {
            thisv->rAlphaStep = initParams->alpha / initParams->life;
        }

        thisv->draw = EffectSsDeadDd_Draw;
        thisv->update = EffectSsDeadDd_Update;
        thisv->rScale = initParams->scale;
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorB = initParams->primColor.b;
        thisv->rAlpha = initParams->alpha;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorB = initParams->envColor.b;

    } else if (initParams->type == 1) {
        thisv->life = initParams->life;
        thisv->rScaleStep = initParams->scaleStep;
        thisv->rAlphaMode = 0;
        thisv->rAlphaStep = 155 / initParams->life;
        thisv->rScale = initParams->scale;
        thisv->rPrimColorR = 255;
        thisv->rPrimColorG = 255;
        thisv->rPrimColorB = 155;
        thisv->rAlpha = 155;
        thisv->rEnvColorR = 250;
        thisv->rEnvColorG = 180;
        thisv->rEnvColorB = 0;
        thisv->draw = EffectSsDeadDd_Draw;
        thisv->update = EffectSsDeadDd_Update;

        for (i = initParams->randIter; i > 0; i--) {
            thisv->pos.x = ((Rand_ZeroOne() - 0.5f) * initParams->randPosScale) + initParams->pos.x;
            thisv->pos.y = ((Rand_ZeroOne() - 0.5f) * initParams->randPosScale) + initParams->pos.y;
            thisv->pos.z = ((Rand_ZeroOne() - 0.5f) * initParams->randPosScale) + initParams->pos.z;
            thisv->accel.x = thisv->velocity.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
            thisv->accel.y = thisv->velocity.y = (Rand_ZeroOne() - 0.5f) * 2.0f;
            thisv->accel.z = thisv->velocity.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        }
    } else {
        osSyncPrintf("Effect_SS_Dd_disp_mode():mode_swが変です。\n");
        return 0;
    }

    return 1;
}

void EffectSsDeadDd_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    Mtx* mtx;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_dead_dd.c", 214);

    scale = thisv->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, scale);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        func_80094BC4(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, thisv->rAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rAlpha);
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);
        gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_dead_dd.c", 259);
}

void EffectSsDeadDd_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {

    thisv->rScale += thisv->rScaleStep;

    if (thisv->rScale < 0) {
        thisv->rScale = 0;
    }

    if (thisv->rAlphaMode != 0) {
        thisv->rAlpha += thisv->rAlphaStep;
        if (thisv->rAlpha > 255) {
            thisv->rAlpha = 255;
        }
    } else {
        if (thisv->rAlpha < thisv->rAlphaStep) {
            thisv->rAlpha = 0;
        } else {
            thisv->rAlpha -= thisv->rAlphaStep;
        }
    }
}
