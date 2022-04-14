/*
 * File: z_eff_ss_fire_tail.c
 * Overlay: ovl_Effect_Ss_Fire_Tail
 * Description: Fire (burned by something)
 */

#include "z_eff_ss_fire_tail.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rLifespan regs[1]
#define rReg2 regs[2]
#define rReg3 regs[3]
#define rPrimColorR regs[4]
#define rPrimColorG regs[5]
#define rPrimColorB regs[6]
#define rEnvColorR regs[7]
#define rEnvColorG regs[8]
#define rEnvColorB regs[9]
#define rReg10 regs[10]
#define rBodyPart regs[11]
#define rType regs[12]

u32 EffectSsFireTail_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsFireTail_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsFireTail_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Fire_Tail_InitVars = {
    EFFECT_SS_FIRE_TAIL,
    EffectSsFireTail_Init,
};

u32 EffectSsFireTail_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsFireTailInitParams* initParams = (EffectSsFireTailInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->vec = initParams->unk_14;
    thisv->velocity.x = 0.0f;
    thisv->velocity.y = 0.0f;
    thisv->velocity.z = 0.0f;
    thisv->accel.x = 0.0f;
    thisv->accel.y = 0.0f;
    thisv->accel.z = 0.0f;
    thisv->life = initParams->life;
    thisv->actor = initParams->actor;
    thisv->draw = EffectSsFireTail_Draw;
    thisv->update = EffectSsFireTail_Update;
    thisv->rScale = initParams->scale * 1000.0f;
    thisv->rLifespan = initParams->life;
    thisv->rReg2 = -0xA;
    thisv->rReg3 = -0xF;
    if (initParams->unk_20 == 0) {
        initParams->unk_20 = 1;
    }
    thisv->rReg10 = initParams->unk_20;
    thisv->rPrimColorR = initParams->primColor.r;
    thisv->rPrimColorG = initParams->primColor.g;
    thisv->rPrimColorB = initParams->primColor.b;
    thisv->rEnvColorR = initParams->envColor.r;
    thisv->rEnvColorG = initParams->envColor.g;
    thisv->rEnvColorB = initParams->envColor.b;
    thisv->rBodyPart = initParams->bodyPart;
    thisv->rType = initParams->type;

    return 1;
}

void EffectSsFireTail_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    s16 yaw;
    Vec3f scale;
    f32 temp1;
    f32 temp2;
    f32 dist;

    OPEN_DISPS(gfxCtx, "../z_eff_fire_tail.c", 182);

    scale.x = scale.y = scale.z = 0.0f;

    if (thisv->actor != NULL) {

        thisv->vec = thisv->actor->velocity;

        if (thisv->rBodyPart < 0) {
            Matrix_Translate(thisv->pos.x + thisv->actor->world.pos.x, thisv->pos.y + thisv->actor->world.pos.y,
                             thisv->pos.z + thisv->actor->world.pos.z, MTXMODE_NEW);
        } else {
            Player* player = GET_PLAYER(globalCtx);
            s16 bodyPart = thisv->rBodyPart;

            thisv->pos.x =
                player->bodyPartsPos[bodyPart].x - (Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx))) * 5.0f);
            thisv->pos.y = player->bodyPartsPos[bodyPart].y;
            thisv->pos.z =
                player->bodyPartsPos[bodyPart].z - (Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx))) * 5.0f);

            Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
        }
    } else {
        Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    }

    yaw = Math_Vec3f_Yaw(&scale, &thisv->vec) - Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx));
    temp1 = fabsf(Math_CosS(yaw));
    temp2 = Math_SinS(yaw);
    dist = Math_Vec3f_DistXZ(&scale, &thisv->vec) / (thisv->rReg10 * 0.1f);
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(temp2 * thisv->rReg2 * dist * (std::numbers::pi_v<float> / 180.0f), MTXMODE_APPLY);
    temp2 = 1.0f - ((f32)(thisv->life + 1) / thisv->rLifespan);
    temp2 = 1.0f - SQ(temp2);
    scale.x = scale.y = scale.z = temp2 * (thisv->rScale * 0.000010000001f);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    temp1 = (thisv->rReg3 * 0.01f * temp1 * dist) + 1.0f;

    if (temp1 < 0.1f) {
        temp1 = 0.1f;
    }

    Matrix_Scale(1.0f, temp1, 1.0f / temp1, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_eff_fire_tail.c", 238),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, 0);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                (globalCtx->state.frames * -0x14) & 0x1FF, 32, 128));

    if (thisv->rType != 0) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_fire_tail.c", 273);
}

void EffectSsFireTail_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    thisv->rScale *= 0.9f;
}
