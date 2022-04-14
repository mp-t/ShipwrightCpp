/*
 * File: z_eff_ss_solder_srch_ball.c
 * Overlay: ovl_Effect_Ss_Solder_Srch_Ball
 * Description: Vision sphere for courtyard guards
 */

#include "z_eff_ss_solder_srch_ball.h"

#define rUnused regs[1]

u32 EffectSsSolderSrchBall_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsSolderSrchBall_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Solder_Srch_Ball_InitVars = {
    EFFECT_SS_SOLDER_SRCH_BALL,
    EffectSsSolderSrchBall_Init,
};

u32 EffectSsSolderSrchBall_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsSolderSrchBallInitParams* initParams = (EffectSsSolderSrchBallInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->update = EffectSsSolderSrchBall_Update;
    thisv->life = 100;
    thisv->rUnused = initParams->unused;
    thisv->actor = reinterpret_cast<Actor*>(initParams->linkDetected); // actor field was incorrectly used as a pointer to something else
    return 1;
}

void EffectSsSolderSrchBall_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    s32 pad;
    f32 playerPosDiffX;
    f32 playerPosDiffY;
    f32 playerPosDiffZ;
    s16* linkDetected;
    Player* player = GET_PLAYER(globalCtx);

    linkDetected = reinterpret_cast<s16*>(thisv->actor);

    playerPosDiffX = player->actor.world.pos.x - thisv->pos.x;
    playerPosDiffY = player->actor.world.pos.y - thisv->pos.y;
    playerPosDiffZ = player->actor.world.pos.z - thisv->pos.z;

    if (!BgCheck_SphVsFirstPoly(&globalCtx->colCtx, &thisv->pos, 30.0f)) {
        if (sqrtf(SQ(playerPosDiffX) + SQ(playerPosDiffY) + SQ(playerPosDiffZ)) < 70.0f) {
            *linkDetected = true;
        }
    } else {
        if (thisv->life > 1) {
            thisv->life = 1;
        }
    }
}
