/*
 * File: z_eff_ss_hahen.c
 * Overlay: ovl_Effect_Ss_Hahen
 * Description: Fragments
 */

#include "z_eff_ss_hahen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPitch regs[0]
#define rYaw regs[1]
#define rUnused regs[2]
#define rScale regs[3]
#define rObjId regs[4]
#define rObjBankIdx regs[5]
#define rMinLife regs[6]

u32 EffectSsHahen_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsHahen_DrawGray(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsHahen_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsHahen_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Hahen_InitVars = {
    EFFECT_SS_HAHEN,
    EffectSsHahen_Init,
};

void EffectSsHahen_CheckForObject(EffectSs* thisv, GlobalContext* globalCtx) {
    if (((thisv->rObjBankIdx = Object_GetIndex(&globalCtx->objectCtx, thisv->rObjId)) < 0) ||
        !Object_IsLoaded(&globalCtx->objectCtx, thisv->rObjBankIdx)) {
        thisv->life = -1;
        thisv->draw = NULL;
    }
}

u32 EffectSsHahen_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsHahenInitParams* initParams = (EffectSsHahenInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->life = 200;

    if (initParams->dList != NULL) {
        thisv->gfx = initParams->dList;
        thisv->rObjId = initParams->objId;
        EffectSsHahen_CheckForObject(thisv, globalCtx);
    } else {
        thisv->gfx = SEGMENTED_TO_VIRTUAL(gEffFragments1DL);
        thisv->rObjId = -1;
    }

    if ((thisv->rObjId == OBJECT_HAKA_OBJECTS) && (thisv->gfx == gEffFragments2DL)) {
        thisv->draw = EffectSsHahen_DrawGray;
    } else {
        thisv->draw = EffectSsHahen_Draw;
    }

    thisv->update = EffectSsHahen_Update;
    thisv->rUnused = initParams->unused;
    thisv->rScale = initParams->scale;
    thisv->rPitch = Rand_ZeroOne() * 314.0f;
    thisv->rYaw = Rand_ZeroOne() * 314.0f;
    thisv->rMinLife = 200 - initParams->life;

    return 1;
}

void EffectSsHahen_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 scale = thisv->rScale * 0.001f;

    OPEN_DISPS(gfxCtx, "../z_eff_hahen.c", 208);

    if (thisv->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->rObjBankIdx].segment);
    }

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_RotateY(thisv->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_RotateX(thisv->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_hahen.c", 228),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_hahen.c", 236);
}

// in the original game thisv function is hardcoded to be used only by the skull pots in Shadow Temple
void EffectSsHahen_DrawGray(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 scale = thisv->rScale * 0.001f;

    OPEN_DISPS(gfxCtx, "../z_eff_hahen.c", 253);

    if (thisv->rObjId != -1) {
        gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->rObjBankIdx].segment);
    }

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_RotateY(thisv->rYaw * 0.01f, MTXMODE_APPLY);
    Matrix_RotateX(thisv->rPitch * 0.01f, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_hahen.c", 271),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 100, 100, 120, 255);
    gSPDisplayList(POLY_OPA_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_hahen.c", 288);
}

void EffectSsHahen_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->rPitch += 55;
    thisv->rYaw += 10;

    if ((thisv->pos.y <= player->actor.floorHeight) && (thisv->life < thisv->rMinLife)) {
        thisv->life = 0;
    }

    if (thisv->rObjId != -1) {
        EffectSsHahen_CheckForObject(thisv, globalCtx);
    }
}
