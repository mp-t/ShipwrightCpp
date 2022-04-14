/*
 * File: z_eff_ss_dead_ds.c
 * Overlay: ovl_Effect_Ss_Dead_Ds
 * Description: Burn mark on the floor
 */

#include "z_eff_ss_dead_ds.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTimer regs[1]
#define rRoll regs[2]
#define rPitch regs[3]
#define rYaw regs[4]
#define rAlpha regs[5]
#define rScaleStep regs[9]
#define rAlphaStep regs[10]
#define rHalfOfLife regs[11]

u32 EffectSsDeadDs_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDeadDs_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsDeadDs_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Dead_Ds_InitVars = {
    EFFECT_SS_DEAD_DS,
    EffectSsDeadDs_Init,
};

u32 EffectSsDeadDs_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsDeadDsInitParams* initParams = (EffectSsDeadDsInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->life = initParams->life;
    thisv->rScaleStep = initParams->scaleStep;
    thisv->rHalfOfLife = initParams->life / 2;
    thisv->rAlphaStep = initParams->alpha / thisv->rHalfOfLife;
    thisv->draw = EffectSsDeadDs_Draw;
    thisv->update = EffectSsDeadDs_Update;
    thisv->rScale = initParams->scale;
    thisv->rAlpha = initParams->alpha;
    thisv->rTimer = 0;

    return 1;
}

void EffectSsDeadDs_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad;
    f32 scale;
    s32 pad1;
    s32 pad2;
    MtxF mf;
    f32 temp;
    Vec3f pos;
    CollisionPoly* floorPoly;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_dead_ds.c", 157);

    scale = thisv->rScale * 0.01f;
    func_80094BC4(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, thisv->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    pos = thisv->pos;

    if (thisv->rTimer == 0) {
        Vec3s rpy;
        Vec3f sp44;

        sp44.x = pos.x - thisv->velocity.x;
        sp44.y = pos.y - thisv->velocity.y;
        sp44.z = pos.z - thisv->velocity.z;

        if (BgCheck_EntitySphVsWall1(&globalCtx->colCtx, &thisv->pos, &pos, &sp44, 1.5f, &floorPoly, 1.0f)) {
            func_80038A28(floorPoly, thisv->pos.x, thisv->pos.y, thisv->pos.z, &mf);
            Matrix_Put(&mf);
        } else {
            pos.y++;
            temp = BgCheck_EntityRaycastFloor1(&globalCtx->colCtx, &floorPoly, &pos);

            if (floorPoly != NULL) {
                func_80038A28(floorPoly, thisv->pos.x, temp + 1.5f, thisv->pos.z, &mf);
                Matrix_Put(&mf);
            } else {
                Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
                Matrix_Get(&mf);
            }
        }

        Matrix_MtxFToZYXRotS(&mf, &rpy, 0);
        thisv->rRoll = rpy.x;
        thisv->rPitch = rpy.y;
        thisv->rYaw = rpy.z;
        thisv->pos.y = mf.mf_raw.yw;
        thisv->rTimer++;
    }

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->rRoll, thisv->rPitch, thisv->rYaw, MTXMODE_APPLY);
    Matrix_RotateX(1.57f, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_ss_dead_ds.c", 246),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_dead_ds.c", 255);
}

void EffectSsDeadDs_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->life < thisv->rHalfOfLife) {

        thisv->rScale += thisv->rScaleStep;
        if (thisv->rScale < 0) {
            thisv->rScale = 0;
        }

        thisv->rAlpha -= thisv->rAlphaStep;
        if (thisv->rAlpha < 0) {
            thisv->rAlpha = 0;
        }
    }
}
