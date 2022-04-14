/*
 * File: z_eff_ss_ice_smoke.c
 * Overlay: ovl_Effect_Ss_Ice_Smoke
 * Description: Ice Smoke
 */

#include "z_eff_ss_ice_smoke.h"
#include "objects/object_fz/object_fz.h"

#define rObjBankIdx regs[0]
#define rAlpha regs[1]
#define rScale regs[2]

u32 EffectSsIceSmoke_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsIceSmoke_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsIceSmoke_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Ice_Smoke_InitVars = {
    EFFECT_SS_ICE_SMOKE,
    EffectSsIceSmoke_Init,
};

u32 EffectSsIceSmoke_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsIceSmokeInitParams* initParams = (EffectSsIceSmokeInitParams*)initParamsx;
    s32 pad;
    s32 objBankIdx;
    std::uintptr_t oldSeg6;

    objBankIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_FZ);

    if ((objBankIdx > -1) && Object_IsLoaded(&globalCtx->objectCtx, objBankIdx)) {
        oldSeg6 = gSegments[6];
        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[objBankIdx].segment);
        Math_Vec3f_Copy(&thisv->pos, &initParams->pos);
        Math_Vec3f_Copy(&thisv->velocity, &initParams->velocity);
        Math_Vec3f_Copy(&thisv->accel, &initParams->accel);
        thisv->rObjBankIdx = objBankIdx;
        thisv->rAlpha = 0;
        thisv->rScale = initParams->scale;
        thisv->life = 50;
        thisv->draw = EffectSsIceSmoke_Draw;
        thisv->update = EffectSsIceSmoke_Update;
        gSegments[6] = oldSeg6;

        return 1;
    }

    osSyncPrintf("Effect_SS_Ice_Smoke_ct():バンク Object_Bank_Fzが有りません。\n");

    return 0;
}

void EffectSsIceSmoke_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad;
    void* object;
    Mtx* mtx;
    f32 scale;
    s32 objBankIdx;

    object = globalCtx->objectCtx.status[thisv->rObjBankIdx].segment;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 155);

    objBankIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_FZ);

    if ((objBankIdx > -1) && Object_IsLoaded(&globalCtx->objectCtx, objBankIdx)) {
        gDPPipeSync(POLY_XLU_DISP++);
        func_80093D84(globalCtx->state.gfxCtx);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
        gSPSegment(POLY_XLU_DISP++, 0x06, object);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamStartDL));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 235, 235, thisv->rAlpha);
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->life * 3, thisv->life * 15, 32, 64, 1, 0, 0, 32, 32));
        Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
        scale = thisv->rScale * 0.0001f;
        Matrix_Scale(scale, scale, 1.0f, MTXMODE_APPLY);

        mtx = Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 196);

        if (mtx != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamDL));
        }
    } else {
        thisv->life = -1;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_eff_ss_ice_smoke.c", 210);
}

void EffectSsIceSmoke_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 objBankIdx;

    objBankIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_FZ);

    if ((objBankIdx > -1) && Object_IsLoaded(&globalCtx->objectCtx, objBankIdx)) {
        if (thisv->rAlpha < 100) {
            thisv->rAlpha += 10;
        }
    } else {
        thisv->life = -1;
    }
}
