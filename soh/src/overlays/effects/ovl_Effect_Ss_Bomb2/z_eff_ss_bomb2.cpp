/*
 * File: z_eff_ss_bomb2.c
 * Overlay: ovl_Effect_Ss_Bomb2
 * Description: Bomb Blast
 */

#include "z_eff_ss_bomb2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIdx regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rDepth regs[10]

u32 EffectSsBomb2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsBomb2_DrawFade(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsBomb2_DrawLayered(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsBomb2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Bomb2_InitVars = {
    EFFECT_SS_BOMB2,
    EffectSsBomb2_Init,
};

static EffectSsDrawFunc sDrawFuncs[] = {
    EffectSsBomb2_DrawFade,
    EffectSsBomb2_DrawLayered,
};

u32 EffectSsBomb2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {

    EffectSsBomb2InitParams* initParams = (EffectSsBomb2InitParams*)initParamsx;

    Math_Vec3f_Copy(&thisv->pos, &initParams->pos);
    Math_Vec3f_Copy(&thisv->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&thisv->accel, &initParams->accel);
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    thisv->life = 24;
    thisv->update = EffectSsBomb2_Update;
    thisv->draw = sDrawFuncs[initParams->drawMode];
    thisv->rScale = initParams->scale;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->rPrimColorR = 255;
    thisv->rPrimColorG = 255;
    thisv->rPrimColorB = 255;
    thisv->rPrimColorA = 255;
    thisv->rEnvColorR = 0;
    thisv->rEnvColorG = 0;
    thisv->rEnvColorB = 200;

    return 1;
}

// unused in the original game. looks like EffectSsBomb but with color
void EffectSsBomb2_DrawFade(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* textures[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    s32 pad;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 298);

    scale = thisv->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094BC4(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                        thisv->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(textures[thisv->rTexIdx]));
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    if (1) {}
    if (1) {}

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 345);
}

void EffectSsBomb2_DrawLayered(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* textures[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    MtxF mtx2F;
    Mtx* mtx2;
    Mtx* mtx;
    s32 pad[3];
    f32 scale;
    f32 depth;
    f32 layer2Scale = 0.925f;
    s32 i;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 386);

    depth = thisv->rDepth;
    scale = thisv->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        mtx2 = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

        if (mtx2 != NULL) {
            func_80094BC4(gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                            thisv->rPrimColorA);
            gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, 0);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(textures[thisv->rTexIdx]));
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion2DL);
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);

            Matrix_MtxToMtxF(mtx2, &mtx2F);
            Matrix_Put(&mtx2F);

            for (i = 1; i >= 0; i--) {
                Matrix_Translate(0.0f, 0.0f, depth, MTXMODE_APPLY);
                Matrix_RotateZ((thisv->life * 0.02f) + 180.0f, MTXMODE_APPLY);
                Matrix_Scale(layer2Scale, layer2Scale, layer2Scale, MTXMODE_APPLY);
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_ss_bomb2.c", 448),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);
                layer2Scale -= 0.15f;
            }
        }
    }

    if (1) {}
    if (1) {}

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb2.c", 456);
}

void EffectSsBomb2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 divisor;

    thisv->rTexIdx = (23 - thisv->life) / 3;
    thisv->rScale += thisv->rScaleStep;

    if (thisv->rScaleStep == 30) {
        thisv->rDepth += 4.0f;
    } else {
        thisv->rDepth += 2.0f;
    }

    if ((thisv->life < 23) && (thisv->life > 13)) {
        divisor = thisv->life - 13;
        thisv->rPrimColorR = func_80027DD4(thisv->rPrimColorR, 255, divisor);
        thisv->rPrimColorG = func_80027DD4(thisv->rPrimColorG, 255, divisor);
        thisv->rPrimColorB = func_80027DD4(thisv->rPrimColorB, 150, divisor);
        thisv->rPrimColorA = func_80027DD4(thisv->rPrimColorA, 255, divisor);
        thisv->rEnvColorR = func_80027DD4(thisv->rEnvColorR, 150, divisor);
        thisv->rEnvColorG = func_80027DD4(thisv->rEnvColorG, 0, divisor);
        thisv->rEnvColorB = func_80027DD4(thisv->rEnvColorB, 0, divisor);
    } else if ((thisv->life < 14) && (thisv->life > -1)) {
        divisor = thisv->life + 1;
        thisv->rPrimColorR = func_80027DD4(thisv->rPrimColorR, 50, divisor);
        thisv->rPrimColorG = func_80027DD4(thisv->rPrimColorG, 50, divisor);
        thisv->rPrimColorB = func_80027DD4(thisv->rPrimColorB, 50, divisor);
        thisv->rPrimColorA = func_80027DD4(thisv->rPrimColorA, 150, divisor);
        thisv->rEnvColorR = func_80027DD4(thisv->rEnvColorR, 10, divisor);
        thisv->rEnvColorG = func_80027DD4(thisv->rEnvColorG, 10, divisor);
        thisv->rEnvColorB = func_80027DD4(thisv->rEnvColorB, 10, divisor);
    }
}
