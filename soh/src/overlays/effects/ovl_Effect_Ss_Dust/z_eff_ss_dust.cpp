/*
 * File: z_eff_ss_dust.c
 * Overlay: ovl_Effect_Ss_Dust
 * Description: Dust Particle Effect
 */

#include "z_eff_ss_dust.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rTexIdx regs[8] // thisv reg is also used to set specific colors in the fire update function
#define rScale regs[9]
#define rScaleStep regs[10]
#define rDrawFlags regs[11]
#define rLifespan regs[12]

u32 EffectSsDust_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDust_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsBlast_UpdateFire(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDust_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Dust_InitVars = {
    EFFECT_SS_DUST,
    EffectSsDust_Init,
};

static EffectSsUpdateFunc sUpdateFuncs[] = {
    EffectSsDust_Update,
    EffectSsBlast_UpdateFire,
};

u32 EffectSsDust_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    s32 randColorOffset;
    EffectSsDustInitParams* initParams = (EffectSsDustInitParams*)initParamsx;

    Math_Vec3f_Copy(&thisv->pos, &initParams->pos);
    Math_Vec3f_Copy(&thisv->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&thisv->accel, &initParams->accel);
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffDustDL);
    thisv->life = initParams->life;
    thisv->update = sUpdateFuncs[initParams->updateMode];
    thisv->draw = EffectSsDust_Draw;

    if (initParams->drawFlags & 4) {
        randColorOffset = Rand_ZeroOne() * 20.0f - 10.0f;
        thisv->rPrimColorR = initParams->primColor.r + randColorOffset;
        thisv->rPrimColorG = initParams->primColor.g + randColorOffset;
        thisv->rPrimColorB = initParams->primColor.b + randColorOffset;
        thisv->rEnvColorR = initParams->envColor.r + randColorOffset;
        thisv->rEnvColorG = initParams->envColor.g + randColorOffset;
        thisv->rEnvColorB = initParams->envColor.b + randColorOffset;
    } else {
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorB = initParams->primColor.b;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorB = initParams->envColor.b;
    }

    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorA = initParams->envColor.a;
    thisv->rTexIdx = 0;
    thisv->rScale = initParams->scale;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->rLifespan = initParams->life;
    thisv->rDrawFlags = initParams->drawFlags;

    return 1;
}

void EffectSsDust_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* dustTextures[] = {
        gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    s32 pad;
    Mtx* mtx;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_dust.c", 321);

    scale = thisv->rScale * 0.0025f;
    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTextures[thisv->rTexIdx]));
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
        gDPPipeSync(POLY_XLU_DISP++);

        if (thisv->rDrawFlags & 1) {
            gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0,
                              COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_CLD_SURF2);
            gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
        } else if (thisv->rDrawFlags & 2) {
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
            gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
        } else {
            gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING);
        }

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_dust.c", 389);
}

void EffectSsDust_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    thisv->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;

    if ((thisv->life <= thisv->rLifespan) && (thisv->life >= (thisv->rLifespan - 7))) {
        if (thisv->rLifespan >= 5) {
            thisv->rTexIdx = thisv->rLifespan - thisv->life;
        } else {
            thisv->rTexIdx = ((thisv->rLifespan - thisv->life) * (8 / thisv->rLifespan));
        }
    } else {
        thisv->rTexIdx = 7;
    }

    thisv->rScale += thisv->rScaleStep;
}

// thisv update mode is unused in the original game
void EffectSsBlast_UpdateFire(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    thisv->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;

    switch (thisv->rTexIdx) {
        case 0:
            thisv->rPrimColorR = 255;
            thisv->rPrimColorG = 150;
            thisv->rPrimColorB = 0;
            thisv->rEnvColorR = 150;
            thisv->rEnvColorG = 50;
            thisv->rEnvColorB = 0;
            break;
        case 1:
            thisv->rPrimColorR = 200;
            thisv->rPrimColorG = 50;
            thisv->rPrimColorB = 0;
            thisv->rEnvColorR = 100;
            thisv->rEnvColorG = 0;
            thisv->rEnvColorB = 0;
            break;
        case 2:
            thisv->rPrimColorR = 50;
            thisv->rPrimColorG = 0;
            thisv->rPrimColorB = 0;
            thisv->rEnvColorR = 0;
            thisv->rEnvColorG = 0;
            thisv->rEnvColorB = 0;
            break;
        case 3:
            thisv->rPrimColorR = 50;
            thisv->rEnvColorR = thisv->rPrimColorG = thisv->rEnvColorG = thisv->rPrimColorB = thisv->rEnvColorB = 0;
            break;
    }

    if (thisv->rTexIdx < 7) {
        thisv->rTexIdx++;
    }

    thisv->rScale += thisv->rScaleStep;
}
