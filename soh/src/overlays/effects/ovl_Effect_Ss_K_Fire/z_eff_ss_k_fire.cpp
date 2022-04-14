/*
 * File: z_eff_ss_k_fire.c
 * Overlay: ovl_Effect_Ss_K_Fire
 * Description:
 */

#include "z_eff_ss_k_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rAlpha regs[0]
#define rScroll regs[2]
#define rType regs[3]
#define rYScale regs[4]
#define rXZScale regs[5]
#define rScaleMax regs[6]

u32 EffectSsKFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsKFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsKFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_K_Fire_InitVars = {
    EFFECT_SS_K_FIRE,
    EffectSsKFire_Init,
};

u32 EffectSsKFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsKFireInitParams* initParams = (EffectSsKFireInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->life = 100;
    thisv->rScaleMax = initParams->scaleMax;
    thisv->rAlpha = 255;
    thisv->rScroll = (s16)Rand_ZeroFloat(5.0f) - 0x19;
    thisv->rType = initParams->type;
    thisv->draw = EffectSsKFire_Draw;
    thisv->update = EffectSsKFire_Update;

    return 1;
}

void EffectSsKFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 xzScale;
    f32 yScale;

    xzScale = thisv->rXZScale / 10000.0f;
    yScale = thisv->rYScale / 10000.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_k_fire.c", 152);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(xzScale, yScale, xzScale, MTXMODE_APPLY);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                globalCtx->state.frames * thisv->rScroll, 0x20, 0x80));

    if (thisv->rType >= 100) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, thisv->rAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 0);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, thisv->rAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
    }

    if (1) {}

    gDPPipeSync(POLY_XLU_DISP++);
    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

    if ((index & 1) != 0) {
        Matrix_RotateY(std::numbers::pi_v<float>, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_k_fire.c", 215),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(gfxCtx, "../z_eff_k_fire.c", 220);
}

void EffectSsKFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->rXZScale < thisv->rScaleMax) {
        thisv->rXZScale += 4;
        thisv->rYScale += 4;

        if (thisv->rXZScale > thisv->rScaleMax) {
            thisv->rXZScale = thisv->rScaleMax;

            if (thisv->rType != 3) {
                thisv->rYScale = thisv->rScaleMax;
            }
        }
    } else {
        if (thisv->rAlpha > 0) {
            thisv->rAlpha -= 10;
            if (thisv->rAlpha <= 0) {
                thisv->rAlpha = 0;
                thisv->life = 0;
            }
        }
    }

    if (thisv->rType == 3) {
        thisv->rYScale++;
    }
}
