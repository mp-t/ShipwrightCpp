/*
 * File: z_eff_ss_extra.c
 * Overlay: ovl_Effect_Ss_Extra
 * Description: Minigame Score Points
 */

#include "z_eff_ss_extra.h"
#include "objects/object_yabusame_point/object_yabusame_point.h"

#define rObjBankIdx regs[0]
#define rTimer regs[1]
#define rScoreIdx regs[2]
#define rScale regs[3]

u32 EffectSsExtra_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsExtra_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsExtra_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

static s16 sScores[] = { 30, 60, 100 };

EffectSsInit Effect_Ss_Extra_InitVars = {
    EFFECT_SS_EXTRA,
    EffectSsExtra_Init,
};

u32 EffectSsExtra_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsExtraInitParams* initParams = (EffectSsExtraInitParams*)initParamsx;
    s32 pad;
    s32 objBankIndex;
    u32 oldSeg6;

    objBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_YABUSAME_POINT);

    if ((objBankIndex >= 0) && Object_IsLoaded(&globalCtx->objectCtx, objBankIndex)) {
        oldSeg6 = gSegments[6];
        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[objBankIndex].segment);
        thisv->pos = initParams->pos;
        thisv->velocity = initParams->velocity;
        thisv->accel = initParams->accel;
        thisv->draw = EffectSsExtra_Draw;
        thisv->update = EffectSsExtra_Update;
        thisv->life = 50;
        thisv->rScoreIdx = initParams->scoreIdx;
        thisv->rScale = initParams->scale;
        thisv->rTimer = 5;
        thisv->rObjBankIdx = objBankIndex;
        gSegments[6] = oldSeg6;

        return 1;
    }

    return 0;
}

static const void* sTextures[] = {
    object_yabusame_point_Tex_000000,
    object_yabusame_point_Tex_000480,
    object_yabusame_point_Tex_000900,
};

void EffectSsExtra_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad;
    f32 scale = thisv->rScale / 100.0f;
    void* object = globalCtx->objectCtx.status[thisv->rObjBankIdx].segment;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_extra.c", 168);

    gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
    gSPSegment(POLY_XLU_DISP++, 0x06, object);
    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_ss_extra.c", 186),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[thisv->rScoreIdx]));
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_yabusame_point_DL_000DC0));

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_extra.c", 194);
}

void EffectSsExtra_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    if (thisv->rTimer != 0) {
        thisv->rTimer -= 1;
    } else {
        thisv->velocity.y = 0.0f;
    }

    if (thisv->rTimer == 1) {
        globalCtx->interfaceCtx.unk_23C = sScores[thisv->rScoreIdx];
    }
}
