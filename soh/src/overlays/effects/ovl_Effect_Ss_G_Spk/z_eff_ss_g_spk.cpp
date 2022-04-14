/*
 * File: z_eff_ss_g_spk.c
 * Overlay: ovl_Effect_Ss_G_Spk
 * Description: Sparks
 */

#include "z_eff_ss_g_spk.h"
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
#define rScaleStep regs[10]

u32 EffectSsGSpk_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsGSpk_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGSpk_UpdateNoAccel(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsGSpk_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_G_Spk_InitVars = {
    EFFECT_SS_G_SPK,
    EffectSsGSpk_Init,
};

u32 EffectSsGSpk_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsGSpkInitParams* initParams = (EffectSsGSpkInitParams*)initParamsx;

    Math_Vec3f_Copy(&thisv->pos, &initParams->pos);
    Math_Vec3f_Copy(&thisv->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&thisv->accel, &initParams->accel);
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffSparkDL);

    if (initParams->updateMode == 0) {
        thisv->life = 10;
        thisv->vec.x = initParams->pos.x - initParams->actor->world.pos.x;
        thisv->vec.y = initParams->pos.y - initParams->actor->world.pos.y;
        thisv->vec.z = initParams->pos.z - initParams->actor->world.pos.z;
        thisv->update = EffectSsGSpk_Update;
    } else {
        thisv->life = 5;
        thisv->update = EffectSsGSpk_UpdateNoAccel;
    }

    thisv->draw = EffectSsGSpk_Draw;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;
    thisv->rEnvColorA = initParams->envColor.a;
    thisv->rTexIdx = 0;
    thisv->rScale = initParams->scale;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->actor = initParams->actor;

    return 1;
}

void EffectSsGSpk_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    static const void* sparkTextures[] = {
        gEffSpark1Tex,
        gEffSpark2Tex,
        gEffSpark3Tex,
        gEffSpark4Tex,
    };
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_g_spk.c", 208);

    scale = thisv->rScale * 0.0025f;
    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &globalCtx->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sparkTextures[thisv->rTexIdx]));
        func_80094BC4(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    if (1) {}
    if (1) {}

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_g_spk.c", 255);
}

void EffectSsGSpk_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {

    thisv->accel.x = (Rand_ZeroOne() - 0.5f) * 3.0f;
    thisv->accel.z = (Rand_ZeroOne() - 0.5f) * 3.0f;

    if (thisv->actor != NULL) {
        if ((thisv->actor->category == ACTORCAT_EXPLOSIVE) && (thisv->actor->update != NULL)) {
            thisv->pos.x = thisv->actor->world.pos.x + thisv->vec.x;
            thisv->pos.y = thisv->actor->world.pos.y + thisv->vec.y;
            thisv->pos.z = thisv->actor->world.pos.z + thisv->vec.z;
        }
    }

    thisv->vec.x += thisv->accel.x;
    thisv->vec.z += thisv->accel.z;

    thisv->rTexIdx++;
    thisv->rTexIdx &= 3;
    thisv->rScale += thisv->rScaleStep;
}

// thisv update mode is unused in the original game
// with thisv update mode, the sparks dont move randomly in the xz plane, appearing to be on top of each other
void EffectSsGSpk_UpdateNoAccel(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->actor != NULL) {
        if ((thisv->actor->category == ACTORCAT_EXPLOSIVE) && (thisv->actor->update != NULL)) {
            thisv->pos.x += (Math_SinS(thisv->actor->world.rot.y) * thisv->actor->speedXZ);
            thisv->pos.z += (Math_CosS(thisv->actor->world.rot.y) * thisv->actor->speedXZ);
        }
    }

    thisv->rTexIdx++;
    thisv->rTexIdx &= 3;
    thisv->rScale += thisv->rScaleStep;
}
