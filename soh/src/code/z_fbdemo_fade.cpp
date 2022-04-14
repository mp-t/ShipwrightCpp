#include "global.h"
#include "vt.h"

#include <string.h>

static Gfx sRCPSetupFade[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE),
    gsSPEndDisplayList(),
};

void TransitionFade_Start(void* thisx) {
    TransitionFade* thisv = (TransitionFade*)thisx;

    switch (thisv->fadeType) {
        case 0:
            break;
        case 1:
            thisv->fadeTimer = 0;
            thisv->fadeColor.a = thisv->fadeDirection != 0 ? 0xFF : 0;
            break;
        case 2:
            thisv->fadeColor.a = 0;
            break;
    }
    thisv->isDone = 0;
}

void* TransitionFade_Init(void* thisx) {
    TransitionFade* thisv = (TransitionFade*)thisx;

    memset(thisv, 0, sizeof(*thisv));
    return thisv;
}

void TransitionFade_Destroy(void* thisx) {
}

void TransitionFade_Update(void* thisx, s32 updateRate) {
    s32 alpha;
    s16 newAlpha;
    TransitionFade* thisv = (TransitionFade*)thisx;

    switch (thisv->fadeType) {
        case 0:
            break;
        case 1:
            thisv->fadeTimer += updateRate;
            if (thisv->fadeTimer >= gSaveContext.fadeDuration) {
                thisv->fadeTimer = gSaveContext.fadeDuration;
                thisv->isDone = 1;
            }
            if (!gSaveContext.fadeDuration) {
                // "Divide by 0! Zero is included in ZCommonGet fade_speed"
                osSyncPrintf(VT_COL(RED, WHITE) "０除算! ZCommonGet fade_speed に０がはいってる" VT_RST);
            }

            alpha = (255.0f * thisv->fadeTimer) / ((void)0, gSaveContext.fadeDuration);
            thisv->fadeColor.a = (thisv->fadeDirection != 0) ? 255 - alpha : alpha;
            break;
        case 2:
            newAlpha = thisv->fadeColor.a;
            if (iREG(50) != 0) {
                if (iREG(50) < 0) {
                    if (Math_StepToS(&newAlpha, 255, 255)) {
                        iREG(50) = 150;
                    }
                } else {
                    Math_StepToS(&iREG(50), 20, 60);
                    if (Math_StepToS(&newAlpha, 0, iREG(50))) {
                        iREG(50) = 0;
                        thisv->isDone = 1;
                    }
                }
            }
            thisv->fadeColor.a = newAlpha;
            break;
    }
}

void TransitionFade_Draw(void* thisx, Gfx** gfxP) {
    TransitionFade* thisv = (TransitionFade*)thisx;
    Gfx* gfx;
    Color_RGBA8_u32* color = &thisv->fadeColor;

    if (color->a > 0) {
        gfx = *gfxP;
        gSPDisplayList(gfx++, sRCPSetupFade);
        gDPSetPrimColor(gfx++, 0, 0, color->r, color->g, color->b, color->a);
        gDPFillRectangle(gfx++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
        gDPPipeSync(gfx++);
        *gfxP = gfx;
    }
}

s32 TransitionFade_IsDone(void* thisx) {
    TransitionFade* thisv = (TransitionFade*)thisx;

    return thisv->isDone;
}

void TransitionFade_SetColor(void* thisx, u32 color) {
    TransitionFade* thisv = (TransitionFade*)thisx;

    thisv->fadeColor.rgba = color;
}

void TransitionFade_SetType(void* thisx, s32 type) {
    TransitionFade* thisv = (TransitionFade*)thisx;

    if (type == 1) {
        thisv->fadeType = 1;
        thisv->fadeDirection = 1;
    } else if (type == 2) {
        thisv->fadeType = 1;
        thisv->fadeDirection = 0;
    } else if (type == 3) {
        thisv->fadeType = 2;
    } else {
        thisv->fadeType = 0;
    }
}
