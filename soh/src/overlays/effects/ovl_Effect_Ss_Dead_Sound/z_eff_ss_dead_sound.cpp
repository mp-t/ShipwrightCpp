/*
 * File: z_eff_ss_dead_sound.c
 * Overlay: ovl_Effect_Ss_Dead_Sound
 * Description: Plays a sound effect
 */

#include "z_eff_ss_dead_sound.h"

#define rSfxId regs[10]
#define rRepeatMode regs[11] // sound is replayed every update. unused in the original game

u32 EffectSsDeadSound_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx);
void EffectSsDeadSound_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv);

EffectSsInit Effect_Ss_Dead_Sound_InitVars = {
    EFFECT_SS_DEAD_SOUND,
    EffectSsDeadSound_Init,
};

u32 EffectSsDeadSound_Init(GlobalContext* globalCtx, u32 index, EffectSs* thisv, void* initParamsx) {
    EffectSsDeadSoundInitParams* initParams = (EffectSsDeadSoundInitParams*)initParamsx;

    thisv->pos = initParams->pos;
    thisv->velocity = initParams->velocity;
    thisv->accel = initParams->accel;
    thisv->flags = 2;
    thisv->life = initParams->life;
    thisv->draw = NULL;
    thisv->update = EffectSsDeadSound_Update;
    thisv->rRepeatMode = initParams->repeatMode;
    thisv->rSfxId = initParams->sfxId;
    osSyncPrintf("コンストラクター3\n"); // "constructor 3"

    return 1;
}

void EffectSsDeadSound_Update(GlobalContext* globalCtx, u32 index, EffectSs* thisv) {
    switch (thisv->rRepeatMode) {
        case DEADSOUND_REPEAT_MODE_OFF:
            thisv->rRepeatMode--; // decrement to 0 so sound only plays once
            break;
        case DEADSOUND_REPEAT_MODE_ON:
            break;
        default:
            return;
    }

    Audio_PlaySoundGeneral(thisv->rSfxId, &thisv->pos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}
