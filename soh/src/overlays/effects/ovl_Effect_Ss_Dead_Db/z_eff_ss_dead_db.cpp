/*
 * File: z_eff_ss_dead_db.c
 * Overlay: ovl_Effect_Ss_Dead_Db
 * Description: Flames and sound used when an enemy dies
 */

#include "z_eff_ss_dead_db.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTextIdx regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rPlaySound regs[10]
#define rReg11 regs[11]

u32 EffectSsDeadDb_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDeadDb_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDeadDb_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Dead_Db_InitVars = {
    EFFECT_SS_DEAD_DB,
    EffectSsDeadDb_Init,
};

u32 EffectSsDeadDb_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsDeadDbInitParams* initParams = (EffectSsDeadDbInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffEnemyDeathFlameDL);
    thisv->life = initParams->unk_34;
    thisv->flags = 4;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->rReg11 = initParams->unk_34;
    thisv->draw = EffectSsDeadDb_Draw;
    thisv->update = EffectSsDeadDb_Update;
    thisv->rScale = initParams->scale;
    thisv->rTextIdx = 0;
    thisv->rPlaySound = initParams->playSound;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rPrimColorA = initParams->primColor.a;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;

    return 1;
}

static const void* sTextures[] = {
    gEffEnemyDeathFlame1Tex, gEffEnemyDeathFlame2Tex,  gEffEnemyDeathFlame3Tex, gEffEnemyDeathFlame4Tex,
    gEffEnemyDeathFlame5Tex, gEffEnemyDeathFlame6Tex,  gEffEnemyDeathFlame7Tex, gEffEnemyDeathFlame8Tex,
    gEffEnemyDeathFlame9Tex, gEffEnemyDeathFlame10Tex,
};

void EffectSsDeadDb_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    Mtx* mtx;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_dead_db.c", 201);

    scale = thisv->rScale * 0.01f;

    SkinMatrix_SetTranslate(&mfTrans, thisv->pos.x, thisv->pos.y, thisv->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, scale);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094BC4(gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                        thisv->rPrimColorA);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[thisv->rTextIdx]));
        gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_dead_db.c", 247);
}

void EffectSsDeadDb_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    f32 w;
    f32 pad;

    thisv->rTextIdx = (f32)((thisv->rReg11 - thisv->life) * 9) / thisv->rReg11;
    thisv->rScale += thisv->rScaleStep;

    thisv->rPrimColorR -= 10;
    if (thisv->rPrimColorR < 0) {
        thisv->rPrimColorR = 0;
    }

    thisv->rPrimColorG -= 10;
    if (thisv->rPrimColorG < 0) {
        thisv->rPrimColorG = 0;
    }

    thisv->rPrimColorB -= 10;
    if (thisv->rPrimColorB < 0) {
        thisv->rPrimColorB = 0;
    }

    thisv->rEnvColorR -= 10;
    if (thisv->rEnvColorR < 0) {
        thisv->rEnvColorR = 0;
    }

    thisv->rEnvColorG -= 10;
    if (thisv->rEnvColorG < 0) {
        thisv->rEnvColorG = 0;
    }

    thisv->rEnvColorB -= 10;
    if (thisv->rEnvColorB < 0) {
        thisv->rEnvColorB = 0;
    }

    if (thisv->rPlaySound && (thisv->rTextIdx == 1)) {
        SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->pos, &thisv->vec, &w);
        Audio_PlaySoundGeneral(NA_SE_EN_EXTINCT, &thisv->vec, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}
