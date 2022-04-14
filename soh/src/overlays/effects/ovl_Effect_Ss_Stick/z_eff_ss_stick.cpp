/*
 * File: z_eff_ss_stick.c
 * Overlay: ovl_Effect_Ss_Stick
 * Description: Broken stick as child, broken sword as adult
 */

#include "z_eff_ss_stick.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"

#define rObjBankIdx regs[0]
#define rYaw regs[1]

u32 EffectSsStick_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsStick_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsStick_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Stick_InitVars = {
    EFFECT_SS_STICK,
    EffectSsStick_Init,
};

typedef struct {
    /* 0x00 */ s16 objectID;
    /* 0x04 */ const Gfx* displayList;
} StickDrawInfo;

u32 EffectSsStick_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    StickDrawInfo drawInfo[] = {
        { OBJECT_LINK_BOY, gLinkAdultBrokenGiantsKnifeBladeDL }, // adult, broken sword
        { OBJECT_LINK_CHILD, gLinkChildLinkDekuStickDL },        // child, broken stick
    };
    StickDrawInfo* ageInfoEntry = gSaveContext.linkAge + drawInfo;
    EffectSsStickInitParams* initParams = (EffectSsStickInitParams*)initParamsx;

    thisv->rObjBankIdx = Object_GetIndex(&globalCtx->objectCtx, ageInfoEntry->objectID);
    thisv->gfx = ageInfoEntry->displayList;
    thisv->vec = thisv->pos = initParams->pos;
    thisv->rYaw = initParams->yaw;
    thisv->velocity.x = Math_SinS(initParams->yaw) * 6.0f;
    thisv->velocity.z = Math_CosS(initParams->yaw) * 6.0f;
    thisv->life = 20;
    thisv->draw = EffectSsStick_Draw;
    thisv->update = EffectSsStick_Update;
    thisv->velocity.y = 26.0f;
    thisv->accel.y = -4.0f;

    return 1;
}

void EffectSsStick_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_stick.c", 153);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);

    if (!LINK_IS_ADULT) {
        Matrix_Scale(0.01f, 0.0025f, 0.01f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, thisv->rYaw, 0, MTXMODE_APPLY);
    } else {
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, thisv->rYaw, globalCtx->state.frames * 10000, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_stick.c", 176),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->rObjBankIdx].segment);
    gSPSegment(POLY_OPA_DISP++, 0x0C, gCullBackDList);
    gSPDisplayList(POLY_OPA_DISP++, thisv->gfx);

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_stick.c", 188);
}

void EffectSsStick_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
}
