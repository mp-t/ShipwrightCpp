/*
 * File: z_eff_ss_g_magma2.c
 * Overlay: ovl_Effect_Ss_G_Magma2
 * Description:
 */

#include "z_eff_ss_g_magma2.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorA regs[2]
#define rEnvColorR regs[3]
#define rEnvColorG regs[4]
#define rEnvColorA regs[5]
#define rTexIdx regs[6]
#define rTimer regs[7]
#define rUpdateRate regs[8]
#define rDrawMode regs[9]
#define rObjBankIdx regs[10]
#define rScale regs[11]

u32 EffectSsGMagma2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsGMagma2_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGMagma2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

static const void* sTextures[] = {
    object_kingdodongo_Tex_02E4E0, object_kingdodongo_Tex_02E8E0, object_kingdodongo_Tex_02ECE0,
    object_kingdodongo_Tex_02F0E0, object_kingdodongo_Tex_02F4E0, object_kingdodongo_Tex_02F8E0,
    object_kingdodongo_Tex_02FCE0, object_kingdodongo_Tex_0300E0, object_kingdodongo_Tex_0304E0,
    object_kingdodongo_Tex_0308E0, object_kingdodongo_Tex_0308E0, object_kingdodongo_Tex_0308E0,
    object_kingdodongo_Tex_0308E0,
};

EffectSsInit Effect_Ss_G_Magma2_InitVars = {
    EFFECT_SS_G_MAGMA2,
    EffectSsGMagma2_Init,
};

u32 EffectSsGMagma2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    s32 objBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_KINGDODONGO);
    s32 pad;

    if ((objBankIndex >= 0) && Object_IsLoaded(&globalCtx->objectCtx, objBankIndex)) {
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        EffectSsGMagma2InitParams* initParams = (EffectSsGMagma2InitParams*)initParamsx;

        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[objBankIndex].segment);
        thisv->rObjBankIdx = objBankIndex;
        thisv->pos = initParams->pos;
        thisv->velocity = zeroVec;
        thisv->accel = zeroVec;
        thisv->life = 100;
        thisv->draw = EffectSsGMagma2_Draw;
        thisv->update = EffectSsGMagma2_Update;
        thisv->gfx = SEGMENTED_TO_VIRTUAL(object_kingdodongo_DL_025A90);
        thisv->rTexIdx = 0;
        thisv->rDrawMode = initParams->drawMode;
        thisv->rUpdateRate = initParams->updateRate;
        thisv->rScale = initParams->scale;
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorA = initParams->primColor.a;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorA = initParams->envColor.a;

        return 1;
    }

    return 0;
}

void EffectSsGMagma2_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 scale;
    void* object;

    scale = thisv->rScale / 100.0f;
    object = globalCtx->objectCtx.status[thisv->rObjBankIdx].segment;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_g_magma2.c", 261);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
    gSPSegment(POLY_XLU_DISP++, 0x06, object);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_g_magma2.c", 282),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisv->rDrawMode == 0) {
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x3D);
    } else {
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
    }

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, 0, thisv->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, 0, thisv->rEnvColorA);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[thisv->rTexIdx]));
    gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_g_magma2.c", 311);
}

void EffectSsGMagma2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rTimer += thisv->rUpdateRate;

    if (thisv->rTimer >= 10) {
        thisv->rTimer -= 10;
        thisv->rTexIdx++;

        if (thisv->rTexIdx >= 10) {
            thisv->life = 0;
        }

        if (thisv->rDrawMode == 0) {
            thisv->rPrimColorG -= 26;

            if (thisv->rPrimColorG <= 0) {
                thisv->rPrimColorG = 0;
            }

            thisv->rEnvColorR -= 26;

            if (thisv->rEnvColorR <= 0) {
                thisv->rEnvColorR = 0;
            }
        }
    }
}
