/*
 * File: z_eff_ss_dt_bubble.c
 * Overlay: ovl_Effect_Ss_Dt_Bubble
 * Description: Bubbles (a random mix of translucent and opaque)
 */

#include "z_eff_ss_dt_bubble.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rRandXZ regs[8]
#define rScale regs[9]
#define rLifespan regs[10]

u32 EffectSsDtBubble_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDtBubble_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDtBubble_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

static Color_RGBA8 sPrimColors[] = { { 255, 255, 100, 255 }, { 150, 255, 255, 255 }, { 100, 255, 255, 255 } };
static Color_RGBA8 sEnvColors[] = { { 170, 0, 0, 255 }, { 0, 100, 0, 255 }, { 0, 0, 255, 255 } };

EffectSsInit Effect_Ss_Dt_Bubble_InitVars = {
    EFFECT_SS_DT_BUBBLE,
    EffectSsDtBubble_Init,
};

u32 EffectSsDtBubble_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsDtBubbleInitParams* initParams = (EffectSsDtBubbleInitParams*)initParamsx;

    //! @bug Rand_ZeroOne in the macro means a random number is generated for both parts of the macro.
    // In the base game thisv works out because both addresses are segment 4, but it may break if
    // the addresses were changed to refer to different segments
    thisv->gfx = reinterpret_cast<const Gfx*>( SEGMENTED_TO_VIRTUAL(Rand_ZeroOne() < 0.5f ? gEffBubble1Tex : gEffBubble2Tex) );
    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->life = initParams->life;

    if (!initParams->customColor) {
        thisv->rPrimColorR = sPrimColors[initParams->colorProfile].r;
        thisv->rPrimColorG = sPrimColors[initParams->colorProfile].g;
        thisv->rPrimColorB = sPrimColors[initParams->colorProfile].b;
        thisv->rPrimColorA = sPrimColors[initParams->colorProfile].a;
        thisv->rEnvColorR = sEnvColors[initParams->colorProfile].r;
        thisv->rEnvColorG = sEnvColors[initParams->colorProfile].g;
        thisv->rEnvColorB = sEnvColors[initParams->colorProfile].b;
        thisv->rEnvColorA = sEnvColors[initParams->colorProfile].a;
    } else {
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorB = initParams->primColor.b;
        thisv->rPrimColorA = initParams->primColor.a;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorB = initParams->envColor.b;
        thisv->rEnvColorA = initParams->envColor.a;
    }

    thisv->rRandXZ = initParams->randXZ;
    thisv->rScale = initParams->scale;
    thisv->rLifespan = initParams->life;
    thisv->draw = EffectSsDtBubble_Draw;
    thisv->update = EffectSsDtBubble_Update;

    return 1;
}

void EffectSsDtBubble_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_dt_bubble.c", 201);

    scale = thisv->rScale * 0.004f;
    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_dt_bubble.c", 213),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093C14(gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                    (thisv->rPrimColorA * thisv->life) / thisv->rLifespan);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB,
                   (thisv->rEnvColorA * thisv->life) / thisv->rLifespan);
    gSPSegment(POLY_XLU_DISP++, 0x08, thisv->gfx);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_dt_bubble.c", 236);
}

void EffectSsDtBubble_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    f32 rand;

    if (thisv->rRandXZ == 1) {
        rand = Rand_ZeroOne();
        thisv->pos.x += (rand * 2.0f) - 1.0f;

        rand = Rand_ZeroOne();
        thisv->pos.z += (rand * 2.0f) - 1.0f;
    }
}
