/*
 * File: z_eff_ss_g_fire.c
 * Overlay: ovl_Effect_Ss_G_Fire
 * Description: Flame Footprints
 */

#include "z_eff_ss_g_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

u32 EffectSsGFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsGFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_G_Fire_InitVars = {
    EFFECT_SS_G_FIRE,
    EffectSsGFire_Init,
};

u32 EffectSsGFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsGFireInitParams* initParams = (EffectSsGFireInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    thisv->velocity = thisv->accel = zeroVec;
    thisv->pos = initParams->pos;
    thisv->draw = EffectSsGFire_Draw;
    thisv->update = EffectSsGFire_Update;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffFireFootprintDL);
    thisv->life = 8;
    thisv->flags = 0;
    thisv->rgScale = 200;
    thisv->rgTexIdx = 0;
    thisv->rgTexIdxStep = 50;
    thisv->rgPrimColorR = 255;
    thisv->rgPrimColorG = 220;
    thisv->rgPrimColorB = 80;
    thisv->rgPrimColorA = 255;
    thisv->rgEnvColorR = 130;
    thisv->rgEnvColorG = 30;
    thisv->rgEnvColorB = 0;
    thisv->rgEnvColorA = 0;

    return 1;
}

void EffectSsGFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    const void* fireFootprintTextures[] = {
        gEffFireFootprint1Tex, gEffFireFootprint2Tex, gEffFireFootprint3Tex, gEffFireFootprint4Tex,
        gEffFireFootprint5Tex, gEffFireFootprint6Tex, gEffFireFootprint7Tex, gEffFireFootprint8Tex,
    };
    s16 texIdx = (thisv->rgTexIdx / 100) % 7;

    EffectSs_DrawGEffect(globalCtx, thisv, fireFootprintTextures[texIdx]);
}

void EffectSsGFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rgTexIdx += thisv->rgTexIdxStep;
}
