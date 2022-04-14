/*
 * File: z_eff_ss_sibuki2.c
 * Overlay: ovl_Effect_Ss_Sibuki2
 * Description: Unfinished and unused bubble effect
 */

#include "z_eff_ss_sibuki2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rTexIdx regs[8]
#define rScale regs[9]

u32 EffectSsSibuki2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsSibuki2_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsSibuki2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Sibuki2_InitVars = {
    EFFECT_SS_SIBUKI2,
    EffectSsSibuki2_Init,
};

u32 EffectSsSibuki2_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsSibuki2InitParams* initParams = (EffectSsSibuki2InitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->life = 10;
    thisv->draw = EffectSsSibuki2_Draw;
    thisv->update = EffectSsSibuki2_Update;
    thisv->rScale = initParams->scale;
    thisv->rPrimColorR = 255;
    thisv->rPrimColorG = 255;
    thisv->rPrimColorB = 255;
    thisv->rPrimColorA = 255;
    thisv->rEnvColorR = 100;
    thisv->rEnvColorG = 100;
    thisv->rEnvColorB = 100;
    thisv->rEnvColorA = 255;
    thisv->rTexIdx = 0;

    return 1;
}

void EffectSsSibuki2_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* bubbleTextures[] = {
        gEffUnusedBubbles1Tex, gEffUnusedBubbles1Tex, gEffUnusedBubbles2Tex,
        gEffUnusedBubbles3Tex, gEffUnusedBubbles4Tex, gEffUnusedBubbles5Tex,
        gEffUnusedBubbles6Tex, gEffUnusedBubbles7Tex, gEffUnusedBubbles8Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    f32 scale = thisv->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_sibuki2.c", 158);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_sibuki2.c", 171),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, thisv->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bubbleTextures[thisv->rTexIdx]));
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffUnusedBubblesDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_sibuki2.c", 198);
}

void EffectSsSibuki2_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->rTexIdx < 8) {
        thisv->rTexIdx++;
    }
}
