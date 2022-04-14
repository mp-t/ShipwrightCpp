/*
 * File: z_eff_ss_lightning.c
 * Overlay: ovl_Effect_Ss_Lightning
 * Description: Lightning
 */

#include "z_eff_ss_lightning.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rNumBolts regs[8]
#define rScale regs[9]
#define rYaw regs[10]
#define rLifespan regs[11]

u32 EffectSsLightning_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsLightning_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsLightning_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Lightning_InitVars = {
    EFFECT_SS_LIGHTNING,
    EffectSsLightning_Init,
};

u32 EffectSsLightning_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsLightningInitParams* initParams = (EffectSsLightningInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffLightningDL);
    thisv->life = initParams->life;
    thisv->draw = EffectSsLightning_Draw;
    thisv->update = EffectSsLightning_Update;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;
    thisv->rEnvColorA = initParams->envColor.a;
    thisv->rNumBolts = initParams->numBolts;
    thisv->rScale = initParams->scale;
    thisv->rYaw = initParams->yaw;
    thisv->rLifespan = initParams->life;

    return 1;
}

void EffectSsLightning_NewLightning(GlobalContext* globalCtx, Vec3f* pos, s16 yaw, EffectSs* thisv) {
    EffectSs newLightning;

    EffectSs_Delete(&newLightning);
    newLightning = *thisv;
    newLightning.pos = *pos;
    newLightning.rNumBolts--;
    newLightning.rYaw = yaw;
    newLightning.life = newLightning.rLifespan;

    EffectSs_Insert(globalCtx, &newLightning);
}

void EffectSsLightning_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* lightningTextures[] = {
        gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
        gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfResult;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfRotate;
    MtxF mfTrans11DA0;
    MtxF mfTrans11DA0Rotate;
    Mtx* mtx;
    f32 yScale;
    s16 texIdx;
    f32 xzScale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_lightning.c", 233);

    yScale = thisv->rScale * 0.01f;
    texIdx = thisv->rLifespan - thisv->life;

    if (texIdx > 7) {
        texIdx = 7;
    }

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    xzScale = yScale * 0.6f;
    SkinMatrix_SetScale(&mfScale, xzScale, yScale, xzScale);
    SkinMatrix_SetRotateZYX(&mfRotate, thisv->vec.x, thisv->vec.y, thisv->rYaw);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfRotate, &mfTrans11DA0Rotate);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0Rotate, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094C50(gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(lightningTextures[texIdx]));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                        thisv->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_lightning.c", 281);
}

void EffectSsLightning_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad;
    Vec3f pos;
    s16 yaw;
    f32 scale;

    if ((thisv->rNumBolts != 0) && ((thisv->life + 1) == thisv->rLifespan)) {

        yaw = thisv->rYaw + (((Rand_ZeroOne() < 0.5f) ? -1 : 1) * ((s16)((Rand_ZeroOne() * 3640.0f)) + 0xE38));

        scale = (thisv->rScale * 0.01f) * 80.0f;
        pos.y = thisv->pos.y + (Math_SinS(thisv->rYaw - 0x4000) * scale);

        scale = Math_CosS(thisv->rYaw - 0x4000) * scale;
        pos.x = thisv->pos.x - (Math_CosS(Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx))) * scale);
        pos.z = thisv->pos.z + (Math_SinS(Camera_GetInputDirYaw(GET_ACTIVE_CAM(globalCtx))) * scale);

        EffectSsLightning_NewLightning(globalCtx, &pos, yaw, thisv);

        if (Rand_ZeroOne() < 0.1f) {
            EffectSsLightning_NewLightning(globalCtx, &pos, (thisv->rYaw * 2) - yaw, thisv);
        }
    }
}
