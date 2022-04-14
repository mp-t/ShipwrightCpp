/*
 * File: z_eff_ss_kirakira.c
 * Overlay: ovl_Effect_Ss_KiraKira
 * Description: Sparkles
 */

#include "z_eff_ss_kirakira.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rRotSpeed regs[0]
#define rYaw regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rEnvColorA regs[9]
#define rAlphaStep regs[10]
#define rScale regs[11]
#define rLifespan regs[12]

u32 EffectSsKiraKira_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsKiraKira_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void func_809AABF0(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void func_809AACAC(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void func_809AAD6C(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_KiraKira_InitVars = {
    EFFECT_SS_KIRAKIRA,
    EffectSsKiraKira_Init,
};

u32 EffectSsKiraKira_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsKiraKiraInitParams* initParams = (EffectSsKiraKiraInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;

    if ((thisv->life = initParams->life) < 0) {
        thisv->life = -thisv->life;
        thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);
        thisv->update = func_809AAD6C;
        thisv->rEnvColorA = initParams->scale;
        thisv->rScale = 0;
    } else {
        thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffSparklesDL);

        if (initParams->updateMode == 0) {
            thisv->update = func_809AABF0;
        } else {
            thisv->update = func_809AACAC;
        }

        thisv->rEnvColorA = initParams->envColor.a;
        thisv->rScale = initParams->scale;
    }

    thisv->draw = EffectSsKiraKira_Draw;
    thisv->rRotSpeed = initParams->rotSpeed;
    thisv->rYaw = initParams->yaw;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;
    thisv->rAlphaStep = initParams->alphaStep;
    thisv->rLifespan = initParams->life;

    return 1;
}

void EffectSsKiraKira_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx;
    f32 scale;
    s32 pad;
    MtxF mfTrans;
    MtxF mfRotY;
    MtxF mfScale;
    MtxF mfTrans11DA0;
    MtxF mfTrans11DA0RotY;
    MtxF mfResult;
    Mtx* mtx;

    scale = thisv->rScale / 10000.0f;
    gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_kirakira.c", 257);

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetRotateZYX(&mfRotY, 0, 0, thisv->rYaw);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfRotY, &mfTrans11DA0RotY);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0RotY, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80093C14(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                        (((s8)((55.0f / thisv->rLifespan) * thisv->life) + 200)));
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_kirakira.c", 301);
}

void func_809AABF0(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    thisv->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;
    thisv->rEnvColorA += thisv->rAlphaStep;

    if (thisv->rEnvColorA < 0) {
        thisv->rEnvColorA = 0;
        thisv->rAlphaStep = -thisv->rAlphaStep;
    } else if (thisv->rEnvColorA > 255) {
        thisv->rEnvColorA = 255;
        thisv->rAlphaStep = -thisv->rAlphaStep;
    }

    thisv->rYaw += thisv->rRotSpeed;
}

void func_809AACAC(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->velocity.x *= 0.95f;
    thisv->velocity.z *= 0.95f;
    thisv->accel.x = Rand_CenteredFloat(0.2f);
    thisv->accel.z = Rand_CenteredFloat(0.2f);
    thisv->rEnvColorA += thisv->rAlphaStep;

    if (thisv->rEnvColorA < 0) {
        thisv->rEnvColorA = 0;
        thisv->rAlphaStep = -thisv->rAlphaStep;
    } else if (thisv->rEnvColorA > 255) {
        thisv->rEnvColorA = 255;
        thisv->rAlphaStep = -thisv->rAlphaStep;
    }

    thisv->rYaw += thisv->rRotSpeed;
}

void func_809AAD6C(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rScale = thisv->rEnvColorA * Math_SinS((32768.0f / thisv->rLifespan) * thisv->life);
}
