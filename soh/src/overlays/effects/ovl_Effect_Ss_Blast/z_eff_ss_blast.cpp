/*
 * File: z_eff_ss_blast.c
 * Overlay: ovl_Effect_Ss_Blast
 * Description: Shockwave Effect
 */

#include "z_eff_ss_blast.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rAlphaTarget regs[8]
#define rScale regs[9]
#define rScaleStep regs[10]
#define rScaleStepDecay regs[11]

u32 EffectSsBlast_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsBlast_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsBlast_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Blast_InitVars = {
    EFFECT_SS_BLAST,
    EffectSsBlast_Init,
};

u32 EffectSsBlast_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsBlastParams* initParams = (EffectSsBlastParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->pos.y += 5.0f;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffShockwaveDL);
    thisv->life = initParams->life;
    thisv->draw = EffectSsBlast_Draw;
    thisv->update = EffectSsBlast_Update;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;
    thisv->rEnvColorA = initParams->envColor.a;
    thisv->rAlphaTarget = initParams->primColor.a / initParams->life;
    thisv->rScale = initParams->scale;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->rScaleStepDecay = initParams->sclaeStepDecay;
    return 1;
}

void EffectSsBlast_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mf;
    s32 pad;
    f32 radius;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_blast.c", 170);

    radius = thisv->rScale * 0.0025f;

    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
    func_800BFCB8(globalCtx, &mf, &thisv->pos);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, thisv->rPrimColorA);
    Matrix_Put(&mf);
    Matrix_Scale(radius, radius, radius, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_blast.c", 199),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_blast.c", 204);
}

void EffectSsBlast_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    Math_StepToS(&thisv->rPrimColorA, 0, thisv->rAlphaTarget);
    thisv->rScale += thisv->rScaleStep;

    if (thisv->rScaleStep != 0) {
        thisv->rScaleStep -= thisv->rScaleStepDecay;
    }
}
