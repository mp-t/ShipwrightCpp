/*
 * File: z_eff_ss_fcircle.c
 * Overlay: ovl_Effect_Ss_Fcircle
 * Description: Fire Circle
 */

#include "z_eff_ss_fcircle.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rUnused regs[3] // probably supposed to be an alpha
#define rRadius regs[8]
#define rHeight regs[9]
#define rYaw regs[10]
#define rScale regs[11]

u32 EffectSsFcircle_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsFcircle_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsFcircle_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Fcircle_InitVars = {
    EFFECT_SS_FCIRCLE,
    EffectSsFcircle_Init,
};

u32 EffectSsFcircle_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsFcircleInitParams* initParams = (EffectSsFcircleInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->actor = initParams->actor;
    thisv->vec.x = initParams->pos.x - initParams->actor->world.pos.x;
    thisv->vec.y = initParams->pos.y - initParams->actor->world.pos.y;
    thisv->vec.z = initParams->pos.z - initParams->actor->world.pos.z;
    thisv->gfx = gEffFireCircleDL;
    thisv->life = 20;
    thisv->draw = EffectSsFcircle_Draw;
    thisv->update = EffectSsFcircle_Update;
    thisv->rUnused = 255;
    thisv->rRadius = initParams->radius;
    thisv->rHeight = initParams->height;
    thisv->rYaw = initParams->actor->shape.rot.y;

    return 1;
}

void EffectSsFcircle_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 yScale;
    f32 xzScale;
    f32 scale;

    OPEN_DISPS(gfxCtx, "../z_eff_fcircle.c", 149);

    scale = (thisv->rScale * (0.5f + (thisv->life * 0.025f))) * 0.01f;
    yScale = (thisv->rHeight * 0.001f) * scale;
    xzScale = (thisv->rRadius * 0.001f) * scale;

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(xzScale, yScale, xzScale, MTXMODE_APPLY);
    Matrix_RotateY(thisv->rYaw * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_fcircle.c", 163),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128, 0, 32, 64, 1, 0,
                                ((globalCtx->gameplayFrames) * -0xF) % 256, 32, 64));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0, (thisv->life * 12.75f));
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    gSPDisplayList(POLY_XLU_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_fcircle.c", 186);
}

void EffectSsFcircle_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    Actor* actor = thisv->actor;

    if (actor != NULL) {
        if (actor->update != NULL) {
            thisv->pos.x = actor->world.pos.x + thisv->vec.x;
            thisv->pos.y = actor->world.pos.y + thisv->vec.y;
            thisv->pos.z = actor->world.pos.z + thisv->vec.z;
            thisv->rYaw = actor->shape.rot.y;

            if (actor->colorFilterTimer > 20) {
                thisv->life = 20;
            } else {
                thisv->life = actor->colorFilterTimer;
            }

            Math_StepToS(&thisv->rScale, 100, 20);
        } else {
            thisv->actor = NULL;
        }
    }
}
