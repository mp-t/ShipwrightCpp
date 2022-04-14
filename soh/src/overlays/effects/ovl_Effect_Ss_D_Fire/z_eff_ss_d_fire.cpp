/*
 * File: z_eff_ss_d_fire.c
 * Overlay: ovl_Effect_Ss_D_Fire
 * Description: Dodongo Fire
 */

#include "z_eff_ss_d_fire.h"
#include "objects/object_dodongo/object_dodongo.h"

#define rScale regs[0]
#define rTexIdx regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rFadeDelay regs[6]
#define rScaleStep regs[9]
#define rObjBankIdx regs[10]
#define rYAccelStep regs[11] // has no effect due to how it's implemented

u32 EffectSsDFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_D_Fire_InitVars = {
    EFFECT_SS_D_FIRE,
    EffectSsDFire_Init,
};

u32 EffectSsDFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsDFireInitParams* initParams = (EffectSsDFireInitParams*)initParamsx;
    s32 objBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_DODONGO);

    if (objBankIndex >= 0) {
        thisv->pos = initParams->pos;
        thisv->velocity = initParams->velocity;
        thisv->accel = initParams->accel;
        thisv->gfx = SEGMENTED_TO_VIRTUAL(gDodongoFireDL);
        thisv->life = initParams->life;
        thisv->rScale = initParams->scale;
        thisv->rScaleStep = initParams->scaleStep;
        thisv->rYAccelStep = 0;
        thisv->rObjBankIdx = objBankIndex;
        thisv->draw = EffectSsDFire_Draw;
        thisv->update = EffectSsDFire_Update;
        thisv->rTexIdx = ((s16)(globalCtx->state.frames % 4) ^ 3);
        thisv->rPrimColorR = 255;
        thisv->rPrimColorG = 255;
        thisv->rPrimColorB = 50;
        thisv->rPrimColorA = initParams->alpha;
        thisv->rFadeDelay = initParams->fadeDelay;

        return 1;
    }

    return 0;
}

static const void* sTextures[] = { gDodongoFire0Tex, gDodongoFire1Tex, gDodongoFire2Tex, gDodongoFire3Tex };

void EffectSsDFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    s32 pad;
    void* object;
    Mtx* mtx;
    f32 scale;

    object = globalCtx->objectCtx.status[thisv->rObjBankIdx].segment;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_d_fire.c", 276);

    if (Object_GetIndex(&globalCtx->objectCtx, OBJECT_DODONGO) > -1) {
        gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
        gSPSegment(POLY_XLU_DISP++, 0x06, object);
        scale = thisv->rScale / 100.0f;
        SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
        SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
        SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
        SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

        mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

        if (mtx != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            func_80094BC4(gfxCtx);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                            thisv->rPrimColorA);
            gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[thisv->rTexIdx]));
            gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_d_fire.c", 330);
}

void EffectSsDFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rTexIdx++;
    thisv->rTexIdx &= 3;
    thisv->rScale += thisv->rScaleStep;

    if (thisv->rFadeDelay >= thisv->life) {
        thisv->rPrimColorA -= 5;
        if (thisv->rPrimColorA < 0) {
            thisv->rPrimColorA = 0;
        }
    } else {
        thisv->rPrimColorA += 15;
        if (thisv->rPrimColorA > 255) {
            thisv->rPrimColorA = 255;
        }
    }

    if (thisv->accel.y < 0.0f) {
        thisv->accel.y += thisv->rYAccelStep * 0.01f;
    }

    if (thisv->life <= 0) {
        thisv->rYAccelStep += 0;
    }
}
