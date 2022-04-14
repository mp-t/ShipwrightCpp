/*
 * File: z_eff_ss_g_splash.c
 * Overlay: ovl_Effect_Ss_G_Splash
 * Description: Splash
 */

#include "z_eff_ss_g_splash.h"
#include "objects/gameplay_keep/gameplay_keep.h"

//! @bug the reuse of regs[11] means that EffectSs_DrawGEffect will treat the type as an object bank index
// thisv ends up having no effect because the texture provided does not use segment 6
#define rType regs[11]

u32 EffectSsGSplash_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParams);
void EffectSsGSplash_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGSplash_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_G_Splash_InitVars = {
    EFFECT_SS_G_SPLASH,
    EffectSsGSplash_Init,
};

u32 EffectSsGSplash_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsGSplashInitParams* initParams = (EffectSsGSplashInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    thisv->velocity = thisv->accel = zeroVec;
    thisv->pos = initParams->pos;
    thisv->draw = EffectSsGSplash_Draw;
    thisv->update = EffectSsGSplash_Update;

    if (initParams->scale == 0) {
        initParams->scale = 600;
    }

    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffWaterSplashDL);
    thisv->life = 8;
    thisv->rgScale = initParams->scale;
    thisv->rgTexIdx = 0;
    thisv->rgTexIdxStep = 100;

    if (initParams->customColor) {
        thisv->rgPrimColorR = initParams->primColor.r;
        thisv->rgPrimColorG = initParams->primColor.g;
        thisv->rgPrimColorB = initParams->primColor.b;
        thisv->rgPrimColorA = initParams->primColor.a;
        thisv->rgEnvColorR = initParams->envColor.r;
        thisv->rgEnvColorG = initParams->envColor.g;
        thisv->rgEnvColorB = initParams->envColor.b;
        thisv->rgEnvColorA = initParams->envColor.a;
        thisv->rType = initParams->type;
    } else {
        switch (initParams->type) {
            case 0:
                thisv->rgPrimColorR = 255;
                thisv->rgPrimColorG = 255;
                thisv->rgPrimColorB = 255;
                thisv->rgPrimColorA = 200;
                thisv->rgEnvColorR = 255;
                thisv->rgEnvColorG = 255;
                thisv->rgEnvColorB = 255;
                thisv->rgEnvColorA = 200;
                thisv->rType = 0;
                break;
            case 1:
                thisv->rgPrimColorR = 255;
                thisv->rgPrimColorG = 255;
                thisv->rgPrimColorB = 255;
                thisv->rgPrimColorA = 255;
                thisv->rgEnvColorR = 255;
                thisv->rgEnvColorG = 255;
                thisv->rgEnvColorB = 255;
                thisv->rgEnvColorA = 255;
                thisv->rType = 1;
                break;
            case 2:
                thisv->rgPrimColorR = 255;
                thisv->rgPrimColorG = 255;
                thisv->rgPrimColorB = 255;
                thisv->rgPrimColorA = 200;
                thisv->rgEnvColorR = 255;
                thisv->rgEnvColorG = 255;
                thisv->rgEnvColorB = 255;
                thisv->rgEnvColorA = 200;
                thisv->rType = 2;
                break;
        }
    }
    return 1;
}

void EffectSsGSplash_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* waterSplashTextures[] = {
        gEffWaterSplash1Tex, gEffWaterSplash2Tex, gEffWaterSplash3Tex, gEffWaterSplash4Tex,
        gEffWaterSplash5Tex, gEffWaterSplash6Tex, gEffWaterSplash7Tex, gEffWaterSplash8Tex,
    };
    s16 texIdx;

    switch (thisv->rType) {
        case 0:
            texIdx = thisv->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(globalCtx, thisv, waterSplashTextures[texIdx]);
            break;

        case 1:
            texIdx = thisv->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(globalCtx, thisv, waterSplashTextures[texIdx]);
            break;

        case 2:
            texIdx = thisv->rgTexIdx / 100;
            if (texIdx > 7) {
                texIdx = 7;
            }
            EffectSs_DrawGEffect(globalCtx, thisv, waterSplashTextures[texIdx]);
            break;

        default:
            break;
    }
}

void EffectSsGSplash_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    Vec3f newSplashPos;

    if ((thisv->rType == 1) && (thisv->life == 5)) {
        newSplashPos = thisv->pos;
        newSplashPos.y += ((thisv->rgScale * 20) * 0.002f);
        EffectSsGSplash_Spawn(globalCtx, &newSplashPos, 0, 0, 2, thisv->rgScale / 2);
    }

    thisv->rgTexIdx += thisv->rgTexIdxStep;
}
