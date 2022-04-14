#include "global.h"

// Note : This file is related to z_vismono, the original name was probably z_vis<something after "mono" alphabetically>

// z-buffer
extern u16 D_0E000000[];

// Init
void func_800AD920(struct_80166500* thisx) {
    thisx->useRgba = false;
    thisx->setScissor = false;
    thisx->primColor.r = 255;
    thisx->primColor.g = 255;
    thisx->primColor.b = 255;
    thisx->primColor.a = 255;
    thisx->envColor.a = 255;
    thisx->envColor.r = 0;
    thisx->envColor.g = 0;
    thisx->envColor.b = 0;
}

// Destroy
void func_800AD950(struct_80166500* thisx) {
}

// Draw
void func_800AD958(struct_80166500* thisx, Gfx** gfxp) {
    Gfx* gfx = *gfxp;

    // OTRTODO
#if 0
    u16* tex = D_0E000000;
    s32 fmt = thisx->useRgba == false ? G_IM_FMT_IA : G_IM_FMT_RGBA;
    s32 y;
    s32 height = 6;

    gDPPipeSync(gfx++);
    if (thisx->setScissor == true) {
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_OPA_SURF | G_RM_OPA_SURF2);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT);

    gDPSetColor(gfx++, G_SETPRIMCOLOR, thisx->primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, thisx->envColor.rgba);

    for (y = 0; y <= SCREEN_HEIGHT - height; y += height) {
        gDPLoadTextureBlock(gfx++, tex, fmt, G_IM_SIZ_16b, SCREEN_WIDTH, height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(gfx++, 0, (y) << 2, (SCREEN_WIDTH << 2), (y + height) << 2, G_TX_RENDERTILE, 0, 0,
                            (1 << 10), (1 << 10));
        tex += SCREEN_WIDTH * height;
    }

    gDPPipeSync(gfx++);
    *gfxp = gfx;
#endif
}
