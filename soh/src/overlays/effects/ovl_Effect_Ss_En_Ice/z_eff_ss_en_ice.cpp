/*
 * File: z_eff_ss_en_ice.c
 * Overlay: ovl_Effect_Ss_En_Ice
 * Description: Ice clumps
 */

#include "z_eff_ss_en_ice.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rLifespan regs[0]
#define rYaw regs[1]
#define rPitch regs[2]
#define rRotSpeed regs[3]
#define rPrimColorR regs[4]
#define rPrimColorG regs[5]
#define rPrimColorB regs[6]
#define rPrimColorA regs[7]
#define rEnvColorR regs[8]
#define rEnvColorG regs[9]
#define rEnvColorB regs[10]
#define rAlphaMode regs[11]
#define rScale regs[12]

u32 EffectSsEnIce_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsEnIce_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsEnIce_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsEnIce_UpdateFlying(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_En_Ice_InitVars = {
    EFFECT_SS_EN_ICE,
    EffectSsEnIce_Init,
};

u32 EffectSsEnIce_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsEnIceInitParams* initParams = (EffectSsEnIceInitParams*)initParamsx;

    if (initParams->type == 0) {
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

        thisv->pos = initParams->pos;
        thisv->vec.x = thisv->pos.x - initParams->actor->world.pos.x;
        thisv->vec.y = thisv->pos.y - initParams->actor->world.pos.y;
        thisv->vec.z = thisv->pos.z - initParams->actor->world.pos.z;
        thisv->velocity = zeroVec;
        thisv->accel = zeroVec;
        thisv->life = 10;
        thisv->actor = initParams->actor;
        thisv->draw = EffectSsEnIce_Draw;
        thisv->update = EffectSsEnIce_UpdateFlying;
        thisv->rScale = initParams->scale * 100.0f;
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorB = initParams->primColor.b;
        thisv->rPrimColorA = initParams->primColor.a;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorB = initParams->envColor.b;
        thisv->rAlphaMode = 1;
        thisv->rPitch = Rand_CenteredFloat(65536.0f);
    } else if (initParams->type == 1) {
        thisv->pos = initParams->pos;
        thisv->vec = initParams->pos;
        thisv->velocity = initParams->velocity;
        thisv->accel = initParams->accel;
        thisv->life = initParams->life;
        thisv->draw = EffectSsEnIce_Draw;
        thisv->update = EffectSsEnIce_Update;
        thisv->rLifespan = initParams->life;
        thisv->rScale = initParams->scale * 100.0f;
        thisv->rYaw = Math_Atan2S(initParams->velocity.z, initParams->velocity.x);
        thisv->rPitch = 0;
        thisv->rPrimColorR = initParams->primColor.r;
        thisv->rPrimColorG = initParams->primColor.g;
        thisv->rPrimColorB = initParams->primColor.b;
        thisv->rPrimColorA = initParams->primColor.a;
        thisv->rEnvColorR = initParams->envColor.r;
        thisv->rEnvColorG = initParams->envColor.g;
        thisv->rEnvColorB = initParams->envColor.b;
        thisv->rAlphaMode = 0;
    } else {
        osSyncPrintf("Effect_Ss_En_Ice_ct():pid->mode_swがエラーです。\n");
        return 0;
    }

    return 1;
}

void EffectSsEnIce_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 scale;
    Vec3f hiliteLightDir;
    u32 gameplayFrames;
    f32 alpha;

    scale = thisv->rScale * 0.01f;
    gameplayFrames = globalCtx->gameplayFrames;

    OPEN_DISPS(gfxCtx, "../z_eff_en_ice.c", 235);

    if (thisv->rAlphaMode != 0) {
        alpha = thisv->life * 12;
    } else {
        if ((thisv->rLifespan > 0) && (thisv->life < (thisv->rLifespan >> 1))) {
            alpha = ((thisv->life * 2.0f) / thisv->rLifespan);
            alpha *= 255.0f;
        } else {
            alpha = 255.0f;
        }
    }

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateY(thisv->rYaw * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateX(thisv->rPitch * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_en_ice.c", 261),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    hiliteLightDir.x = 89.8f;
    hiliteLightDir.y = 0.0f;
    hiliteLightDir.z = 89.8f;

    func_80093D84(globalCtx->state.gfxCtx);
    func_8002EB44(&thisv->pos, &globalCtx->view.eye, &hiliteLightDir, globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, gameplayFrames & 0xFF, 0x20, 0x10, 1, 0,
                                (gameplayFrames * 2) & 0xFF, 0x40, 0x20));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB,
                    thisv->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, (u32)alpha);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment2DL);

    CLOSE_DISPS(gfxCtx, "../z_eff_en_ice.c", 294);
}

void EffectSsEnIce_UpdateFlying(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s16 rand;

    if ((thisv->actor != NULL) && (thisv->actor->update != NULL)) {
        if ((thisv->life >= 9) && (thisv->actor->colorFilterTimer != 0) && (!(thisv->actor->colorFilterParams & 0xC000))) {
            thisv->pos.x = thisv->actor->world.pos.x + thisv->vec.x;
            thisv->pos.y = thisv->actor->world.pos.y + thisv->vec.y;
            thisv->pos.z = thisv->actor->world.pos.z + thisv->vec.z;
            thisv->life++;
        } else if (thisv->life == 9) {
            thisv->accel.x = Math_SinS(Math_Vec3f_Yaw(&thisv->actor->world.pos, &thisv->pos)) * (Rand_ZeroOne() + 1.0f);
            thisv->accel.z = Math_CosS(Math_Vec3f_Yaw(&thisv->actor->world.pos, &thisv->pos)) * (Rand_ZeroOne() + 1.0f);
            thisv->accel.y = -1.5f;
            thisv->velocity.y = 5.0f;
        }
    } else {
        if (thisv->life >= 9) {
            rand = Rand_CenteredFloat(65535.0f);
            thisv->accel.x = Math_SinS(rand) * (Rand_ZeroOne() + 1.0f);
            thisv->accel.z = Math_CosS(rand) * (Rand_ZeroOne() + 1.0f);
            thisv->life = 8;
            thisv->accel.y = -1.5f;
            thisv->velocity.y = 5.0f;
        }
    }
}

void EffectSsEnIce_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rPitch += thisv->rRotSpeed; // rRotSpeed is not initialized so thisv does nothing
}
