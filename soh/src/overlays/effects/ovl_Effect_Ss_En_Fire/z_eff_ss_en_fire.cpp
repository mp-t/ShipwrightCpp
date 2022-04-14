/*
 * File: z_eff_ss_en_fire.c
 * Overlay: ovl_Effect_Ss_En_Fire
 * Description:
 */

#include "z_eff_ss_en_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScaleMax regs[0]
#define rScale regs[1]
#define rLifespan regs[2]
#define rUnused regs[3]
#define rPitch regs[4]
#define rYaw regs[5]
#define rReg6 regs[6]
#define rBodyPart regs[7]
#define rFlags regs[8]
#define rScroll regs[9]

u32 EffectSsEnFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsEnFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsEnFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_En_Fire_InitVars = {
    EFFECT_SS_EN_FIRE,
    EffectSsEnFire_Init,
};

u32 EffectSsEnFire_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsEnFireInitParams* initParams = (EffectSsEnFireInitParams*)initParamsx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    thisv->pos = initParams->pos;
    thisv->velocity = thisv->accel = zeroVec;
    thisv->life = 20;
    thisv->rLifespan = thisv->life;
    thisv->actor = initParams->actor;
    thisv->rScroll = Rand_ZeroOne() * 20.0f;
    thisv->draw = EffectSsEnFire_Draw;
    thisv->update = EffectSsEnFire_Update;
    thisv->rUnused = -15;

    if (initParams->bodyPart < 0) {
        thisv->rYaw = Math_Vec3f_Yaw(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.y;
        thisv->rPitch =
            Math_Vec3f_Pitch(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.x;
        thisv->vec.z = Math_Vec3f_DistXYZ(&initParams->pos, &initParams->actor->world.pos);
    }

    thisv->rScaleMax = initParams->scale;

    if ((initParams->unk_12 & 0x8000) != 0) {
        thisv->rScale = initParams->scale;
    } else {
        thisv->rScale = 0;
    }

    thisv->rReg6 = initParams->unk_12 & 0x7FFF;
    thisv->rBodyPart = initParams->bodyPart;
    thisv->rFlags = initParams->flags;

    return 1;
}

void EffectSsEnFire_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    f32 scale;
    s16 camYaw;
    s32 pad[3];
    s16 redGreen;

    OPEN_DISPS(gfxCtx, "../z_eff_en_fire.c", 169);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    camYaw = (Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000);
    Matrix_RotateY(camYaw * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);

    scale = Math_SinS(thisv->life * 0x333) * (thisv->rScale * 0.00005f);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_en_fire.c", 180),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    redGreen = thisv->life - 5;

    if (redGreen < 0) {
        redGreen = 0;
    }

    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, redGreen * 12.7f, 0, 0, 0);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x0, 0x80, redGreen * 12.7f, redGreen * 12.7f, 0, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (thisv->rScroll * -0x14) & 0x1FF,
                                0x20, 0x80));

    if (((thisv->rFlags & 0x7FFF) != 0) || (thisv->life < 18)) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_en_fire.c", 213);
}

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3f firePos[10];
} FireActorF;

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3s firePos[10];
} FireActorS;

void EffectSsEnFire_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {

    thisv->rScroll++;

    if (thisv->actor != NULL) {
        if (thisv->actor->colorFilterTimer >= 22) {
            thisv->life++;
        }
        if (thisv->actor->update != NULL) {
            Math_SmoothStepToS(&thisv->rScale, thisv->rScaleMax, 1, thisv->rScaleMax >> 3, 0);

            if (thisv->rBodyPart < 0) {
                Matrix_Translate(thisv->actor->world.pos.x, thisv->actor->world.pos.y, thisv->actor->world.pos.z,
                                 MTXMODE_NEW);
                Matrix_RotateY((thisv->rYaw + thisv->actor->shape.rot.y) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
                Matrix_RotateX((thisv->rPitch + thisv->actor->shape.rot.x) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
                Matrix_MultVec3f(&thisv->vec, &thisv->pos);
            } else {
                if ((thisv->rFlags & 0x8000)) {
                    thisv->pos.x = ((FireActorS*)thisv->actor)->firePos[thisv->rBodyPart].x;
                    thisv->pos.y = ((FireActorS*)thisv->actor)->firePos[thisv->rBodyPart].y;
                    thisv->pos.z = ((FireActorS*)thisv->actor)->firePos[thisv->rBodyPart].z;
                } else {
                    thisv->pos.x = ((FireActorF*)thisv->actor)->firePos[thisv->rBodyPart].x;
                    thisv->pos.y = ((FireActorF*)thisv->actor)->firePos[thisv->rBodyPart].y;
                    thisv->pos.z = ((FireActorF*)thisv->actor)->firePos[thisv->rBodyPart].z;
                }
            }
        } else if (thisv->rReg6 != 0) {
            thisv->life = 0;
        } else {
            thisv->actor = NULL;
        }
    }
}
