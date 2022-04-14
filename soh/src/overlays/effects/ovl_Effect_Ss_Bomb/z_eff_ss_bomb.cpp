/*
 * File: z_eff_ss_bomb.c
 * Overlay: ovl_Effect_Ss_Bomb
 * Description: Bomb Blast. Unused in the orignal game.
 */

#include "z_eff_ss_bomb.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIdx regs[1]

u32 EffectSsBomb_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsBomb_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsBomb_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Bomb_InitVars = {
    EFFECT_SS_BOMB,
    EffectSsBomb_Init,
};

u32 EffectSsBomb_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsBombInitParams* initParams = (EffectSsBombInitParams*)initParamsx;

    Math_Vec3f_Copy(&thisv->pos, &initParams->pos);
    Math_Vec3f_Copy(&thisv->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&thisv->accel, &initParams->accel);
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    thisv->life = 20;
    thisv->draw = EffectSsBomb_Draw;
    thisv->update = EffectSsBomb_Update;
    thisv->rScale = 100;
    thisv->rTexIdx = 0;

    return 1;
}

void EffectSsBomb_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* explosionTextures[] = {
        gEffBombExplosion1Tex,
        gEffBombExplosion2Tex,
        gEffBombExplosion3Tex,
        gEffBombExplosion4Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    s32 pad;
    f32 scale;
    s16 color;

    if (1) {}

    OPEN_DISPS(gfxCtx, "../z_eff_ss_bomb.c", 168);

    scale = thisv->rScale / 100.0f;

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(explosionTextures[thisv->rTexIdx]));
        gDPPipeSync(POLY_XLU_DISP++);
        func_80094C50(gfxCtx);
        color = thisv->life * 12.75f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color, color, color, color);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
        gDPPipeSync(POLY_XLU_DISP++);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_bomb.c", 214);
}

void EffectSsBomb_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if ((thisv->life < 21) && (thisv->life > 16)) {
        thisv->rTexIdx = (20 - thisv->life);
    } else {
        thisv->rScale += 0;
        thisv->rTexIdx = 3;
    }

    thisv->accel.x = ((Rand_ZeroOne() * 0.4f) - 0.2f);
    thisv->accel.z = ((Rand_ZeroOne() * 0.4f) - 0.2f);
}
