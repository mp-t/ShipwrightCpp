/*
 * File: z_eff_ss_g_magma.c
 * Overlay: ovl_Effect_Ss_G_Magma
 * Description: Magma Bubbles
 */

#include "z_eff_ss_g_magma.h"
#include "objects/gameplay_keep/gameplay_keep.h"

u32 EffectSsGMagma_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsGMagma_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGMagma_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_G_Magma_InitVars = {
    EFFECT_SS_G_MAGMA,
    EffectSsGMagma_Init,
};

u32 EffectSsGMagma_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsGMagmaInitParams* initParams = (EffectSsGMagmaInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    thisv->velocity = thisv->accel = zeroVec;
    thisv->pos = initParams->pos;
    thisv->draw = EffectSsGMagma_Draw;
    thisv->update = EffectSsGMagma_Update;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffMagmaBubbleDL);
    thisv->life = 16;
    thisv->rgScale = (s16)(Rand_ZeroOne() * 100.0f) + 200;
    thisv->rgTexIdx = 0;
    thisv->rgTexIdxStep = 50;
    thisv->rgPrimColorR = 255;
    thisv->rgPrimColorG = 255;
    thisv->rgPrimColorB = 0;
    thisv->rgPrimColorA = 255;
    thisv->rgEnvColorR = 255;
    thisv->rgEnvColorG = 0;
    thisv->rgEnvColorB = 0;
    thisv->rgEnvColorA = 0;

    return 1;
}

static const void* sTextures[] = {
    gEffMagmaBubble1Tex, gEffMagmaBubble2Tex, gEffMagmaBubble3Tex, gEffMagmaBubble4Tex,
    gEffMagmaBubble5Tex, gEffMagmaBubble6Tex, gEffMagmaBubble7Tex, gEffMagmaBubble8Tex,
};

void EffectSsGMagma_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s16 texIdx = thisv->rgTexIdx / 100;

    if (texIdx > 7) {
        texIdx = 7;
    }

    EffectSs_DrawGEffect(globalCtx, thisv, sTextures[texIdx]);
}

void EffectSsGMagma_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rgTexIdx += thisv->rgTexIdxStep;
}
