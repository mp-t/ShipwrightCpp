#include "global.h"
#include <string.h>

#include "code/fbdemo_triforce/z_fbdemo_triforce.h"

void TransitionTriforce_Start(void* thisx) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    switch (thisv->state) {
        case 1:
        case 2:
            thisv->transPos = 1.0f;
            return;
    }
    thisv->transPos = 0.03f;
}

void* TransitionTriforce_Init(void* thisx) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    memset(thisv,0, sizeof(*thisv));
    guOrtho(&thisv->projection, -160.0f, 160.0f, -120.0f, 120.0f, -1000.0f, 1000.0f, 1.0f);
    thisv->transPos = 1.0f;
    thisv->state = 2;
    thisv->step = 0.015f;
    thisv->fadeDirection = 1;
    return thisv;
}

void TransitionTriforce_Destroy(void* thisx) {
}

void TransitionTriforce_Update(void* thisx, s32 updateRate) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;
    f32 temp_f0;
    s32 i;

    for (i = updateRate; i > 0; i--) {
        if (thisv->state == 1) {
            thisv->transPos = CLAMP_MIN(thisv->transPos * (1.0f - thisv->step), 0.03f);
        } else if (thisv->state == 2) {
            thisv->transPos = CLAMP_MIN(thisv->transPos - thisv->step, 0.03f);
        } else if (thisv->state == 3) {
            thisv->transPos = CLAMP_MAX(thisv->transPos / (1.0f - thisv->step), 1.0f);
        } else if (thisv->state == 4) {
            thisv->transPos = CLAMP_MAX(thisv->transPos + thisv->step, 1.0f);
        }
    }
}

void TransitionTriforce_SetColor(void* thisx, u32 color) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    thisv->color.rgba = color;
}

void TransitionTriforce_SetType(void* thisx, s32 type) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    thisv->fadeDirection = type;
}

// unused
void TransitionTriforce_SetState(void* thisx, s32 state) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    thisv->state = state;
}

void TransitionTriforce_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    f32 scale;
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;
    s32 pad;
    f32 rotation = thisv->transPos * 360.0f;

    modelView = thisv->modelView[thisv->frame];
    scale = thisv->transPos * 0.625f;
    thisv->frame ^= 1;
    osSyncPrintf("rate=%f tx=%f ty=%f rotate=%f\n", thisv->transPos, 0.0f, 0.0f, rotation);
    guScale(&modelView[0], scale, scale, 1.0f);
    guRotate(&modelView[1], rotation, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    gSPDisplayList(gfx++, sTriforceWipeDL);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, thisv->color.rgba);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPMatrix(gfx++, &thisv->projection, G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_LOAD);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_MUL);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_MUL);
    gSPVertex(gfx++, reinterpret_cast<std::uintptr_t>(sTriforceWipeVtx), 10, 0);
    if (!TransitionTriforce_IsDone(thisv)) {
        switch (thisv->fadeDirection) {
            case 1:
                gSP2Triangles(gfx++, 0, 4, 5, 0, 4, 1, 3, 0);
                gSP1Triangle(gfx++, 5, 3, 2, 0);
                break;
            case 2:
                gSP2Triangles(gfx++, 3, 4, 5, 0, 0, 2, 6, 0);
                gSP2Triangles(gfx++, 0, 6, 7, 0, 1, 0, 7, 0);
                gSP2Triangles(gfx++, 1, 7, 8, 0, 1, 8, 9, 0);
                gSP2Triangles(gfx++, 1, 9, 2, 0, 2, 9, 6, 0);
                break;
        }
    } else {
        switch (thisv->fadeDirection) {
            case 1:
                break;
            case 2:
                gSP1Quadrangle(gfx++, 6, 7, 8, 9, 0);
                break;
        }
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionTriforce_IsDone(void* thisx) {
    TransitionTriforce* thisv = (TransitionTriforce*)thisx;

    s32 ret = 0;

    if (thisv->state == 1 || thisv->state == 2) {
        return thisv->transPos <= 0.03f;

    } else if (thisv->state == 3 || thisv->state == 4) {
        return thisv->transPos >= 1.0f;
    }
    return ret;
}
