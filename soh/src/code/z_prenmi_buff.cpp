#include "global.h"

#define COLD_RESET 0
#define NMI 1

void PreNmiBuff_Init(PreNmiBuff* thisv) {
    thisv->resetting = false;

    if (osResetType == COLD_RESET) {
        thisv->resetCount = 0;
        thisv->duration = 0;
    } else {
        thisv->resetCount++;
        thisv->duration += thisv->resetTime;
    }

    thisv->resetTime = 0;
}

void PreNmiBuff_SetReset(PreNmiBuff* thisv) {
    thisv->resetting = true;
    thisv->resetTime = osGetTime();
}

u32 PreNmiBuff_IsResetting(PreNmiBuff* thisv) {
    return thisv->resetting;
}
