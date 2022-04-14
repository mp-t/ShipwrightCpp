/*
 * File: z_eff_ss_sibuki.c
 * Overlay: ovl_Effect_Ss_Sibuki
 * Description: Bubbles (only used by gohma and gohmalings in the original game)
 */

#include "z_eff_ss_sibuki.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rMoveDelay regs[8]
#define rDirection regs[9]
#define rScale regs[10]

u32 EffectSsSibuki_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsSibuki_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv);
void EffectSsSibuki_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Sibuki_InitVars = {
    EFFECT_SS_SIBUKI,
    EffectSsSibuki_Init,
};

u32 EffectSsSibuki_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsSibukiInitParams* initParams = (EffectSsSibukiInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;

    if (KREG(2) != 0) {
        thisv->gfx = reinterpret_cast<const Gfx*>( SEGMENTED_TO_VIRTUAL(gEffBubble2Tex) );
    } else {
        thisv->gfx = reinterpret_cast<const Gfx*>( SEGMENTED_TO_VIRTUAL(gEffBubble1Tex));
    }

    thisv->life = ((s16)((Rand_ZeroOne() * (500.0f + KREG(64))) * 0.01f)) + KREG(65) + 10;
    thisv->rMoveDelay = initParams->moveDelay + 1;
    thisv->draw = EffectSsSibuki_Draw;
    thisv->update = EffectSsSibuki_Update;
    thisv->rDirection = initParams->direction;
    thisv->rScale = initParams->scale;
    thisv->rPrimColorR = 100;
    thisv->rPrimColorG = 100;
    thisv->rPrimColorB = 100;
    thisv->rPrimColorA = 100;
    thisv->rEnvColorR = 255;
    thisv->rEnvColorG = 255;
    thisv->rEnvColorB = 255;
    thisv->rEnvColorA = 255;

    return 1;
}

void EffectSsSibuki_Draw(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    f32 scale = thisv->rScale / 100.0f;

    OPEN_DISPS(gfxCtx, "../z_eff_ss_sibuki.c", 163);

    Matrix_Translate(thisv->pos.x, thisv->pos.y, thisv->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ss_sibuki.c", 176),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D18(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->rPrimColorR, thisv->rPrimColorG, thisv->rPrimColorB, thisv->rPrimColorA);
    gDPSetEnvColor(POLY_OPA_DISP++, thisv->rEnvColorR, thisv->rEnvColorG, thisv->rEnvColorB, thisv->rEnvColorA);
    gSPSegment(POLY_OPA_DISP++, 0x08, thisv->gfx);
    gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gEffBubbleDL));

    CLOSE_DISPS(gfxCtx, "../z_eff_ss_sibuki.c", 198);
}

void EffectSsSibuki_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad[3];
    f32 xzVelScale;
    s16 yaw;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->pos.y <= player->actor.floorHeight) {
        thisv->life = 0;
    }

    if (thisv->rMoveDelay != 0) {
        thisv->rMoveDelay--;

        if (thisv->rMoveDelay == 0) {
            yaw = Camera_GetInputDirYaw(Gameplay_GetCamera(globalCtx, 0));
            xzVelScale = ((200.0f + KREG(20)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * (KREG(23) + 20.0f));

            if (thisv->rDirection != 0) {
                xzVelScale *= -1.0f;
            }

            thisv->velocity.x = Math_CosS(yaw) * xzVelScale;
            thisv->velocity.z = -Math_SinS(yaw) * xzVelScale;

            thisv->velocity.y = ((700.0f + KREG(21)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * (KREG(24) + 20.0f));
            thisv->accel.y = ((-100.0f + KREG(22)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * KREG(25));

            if (KREG(3) != 0) {
                thisv->velocity.x *= (KREG(3) * 0.01f);
                thisv->velocity.y *= (KREG(3) * 0.01f);
                thisv->velocity.z *= (KREG(3) * 0.01f);
                thisv->accel.y *= (KREG(4) * 0.01f);
            }
        }
    } else {
        if (thisv->rScale != 0) {
            thisv->rScale = (thisv->rScale - KREG(26)) - 3;
        }
    }
}
