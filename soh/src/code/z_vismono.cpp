#include "global.h"
#include <string.h>

// (Note: 80 = SCREEN_HEIGHT/3, see VisMono_DrawTexture)
// This may not have been kept up-to-date with the code, 1+1+1+80*(7+2+2+3)+1+1 makes more sense
#define DLSIZE (1 + 3 + 1 + 1 + 80 * (7 + 2 + 2 + 3) + 1)

// framebuffer
extern u16 D_0F000000[];

void VisMono_Init(VisMono* thisx) {
    memset(thisx, 0, sizeof(VisMono));
    thisx->unk_00 = 0;
    thisx->setScissor = false;
    thisx->primColor.r = 255;
    thisx->primColor.g = 255;
    thisx->primColor.b = 255;
    thisx->primColor.a = 255;
    thisx->envColor.r = 0;
    thisx->envColor.g = 0;
    thisx->envColor.b = 0;
    thisx->envColor.a = 0;
}

void VisMono_Destroy(VisMono* thisx) {
    SystemArena_FreeDebug(thisx->monoDl, "../z_vismono.c", 137);
}

void VisMono_UpdateTexture(VisMono* thisx, u16* tex) {
    s32 i;

    for (i = 0; i < 256; i++) {
        tex[i] = ((((i >> 3 & 0x1F) * 2 + (i << 2 & 0x1F) * 4) * 0xFF / 0xD9) << 8) |
                 (((i >> 6 & 0x1F) * 4 + (i >> 1 & 0x1F)) * 0xFF / 0xD9);
    }
}

Gfx* VisMono_DrawTexture(VisMono* thisx, Gfx* gfx) 
{
// OTRTODO
#if 1
    s32 y;
    s32 height = 3;
    //u16* tex = D_0F000000;
    u16* tex = reinterpret_cast<u16*>(0xFF000000);

    gDPPipeSync(gfx++);
    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) | G_RM_CLD_SURF2);
    gDPSetCombineLERP(gfx++, 1, 0, TEXEL1_ALPHA, TEXEL0, 0, 0, 0, 1, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0,
                      0, 0, PRIMITIVE);

    for (y = 0; y <= SCREEN_HEIGHT - height; y += height) {
        gDPLoadTextureBlock(gfx++, tex, G_IM_FMT_CI, G_IM_SIZ_8b, SCREEN_WIDTH * 2, height, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        gDPSetTile(gfx++, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 0x0, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, 0, 0,
                   G_TX_NOMIRROR | G_TX_CLAMP, 0, 0);
        gDPSetTileSize(gfx++, G_TX_RENDERTILE, (2 << 2), 0, ((SCREEN_WIDTH * 2 + 1) << 2), (2 << 2));

        gDPSetTile(gfx++, G_IM_FMT_CI, G_IM_SIZ_8b, 80, 0x0, 1, 1, G_TX_NOMIRROR | G_TX_CLAMP, 0, 0,
                   G_TX_NOMIRROR | G_TX_CLAMP, 0, 0);
        gDPSetTileSize(gfx++, 1, (1 << 2), 0, ((SCREEN_WIDTH * 2) << 2), (2 << 2));

        gSPTextureRectangle(gfx++, 0, y << 2, (SCREEN_WIDTH << 2), (y + height) << 2, G_TX_RENDERTILE, 2 << 5, 0,
                            2 << 10, 1 << 10);
        tex += SCREEN_WIDTH * height;
    }

    gDPPipeSync(gfx++);
    gSPEndDisplayList(gfx++);
#endif
    return gfx;
}

void VisMono_Draw(VisMono* thisx, Gfx** gfxp) {
    Gfx* gfx = *gfxp;
    u16* tlut;
    Gfx* monoDL;
    Gfx* glistpEnd;

    if (thisx->tlut) {
        tlut = thisx->tlut;
    } else {
        tlut = static_cast<u16*>(Graph_DlistAlloc(&gfx, 256 * sizeof(u16)));
        VisMono_UpdateTexture(thisx, tlut);
    }

    if (thisx->monoDl) {
        monoDL = thisx->monoDl;
    } else {
        monoDL = static_cast<Gfx*>(Graph_DlistAlloc(&gfx, DLSIZE * sizeof(Gfx)));
        glistpEnd = VisMono_DrawTexture(thisx, monoDL);

        if (!(glistpEnd <= monoDL + DLSIZE)) {
            LOG_ADDRESS("glistp_end", glistpEnd, "../z_vismono.c", 257);
            LOG_ADDRESS("mono_dl", monoDL, "../z_vismono.c", 258);
            LOG_ADDRESS("mono_dl + (1+3+1+1+80*(7+2+2+3)+1)", monoDL + DLSIZE, "../z_vismono.c", 259);
            LOG_ADDRESS("(1+3+1+1+80*(7+2+2+3)+1)", DLSIZE, "../z_vismono.c", 260);
        }
        ASSERT(glistpEnd <= monoDL + DLSIZE, "glistp_end <= mono_dl + DLSIZE", "../z_vismono.c", 262);
    }

    gDPPipeSync(gfx++);
    if (thisx->setScissor == true) {
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    gDPSetColor(gfx++, G_SETPRIMCOLOR, thisx->primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, thisx->envColor.rgba);

    gDPLoadTLUT_pal256(gfx++, tlut);

    gSPDisplayList(gfx++, monoDL);
    gDPPipeSync(gfx++);

    *gfxp = gfx;
}

void VisMono_DrawOld(VisMono* thisx) {
    Gfx* glistpEnd;

    if (!thisx->tlut) {
        thisx->tlut = static_cast<u16*>(SystemArena_MallocDebug(256 * sizeof(u16), "../z_vismono.c", 283));
        VisMono_UpdateTexture(thisx, thisx->tlut);
    }

    if (!thisx->monoDl) {
        thisx->monoDl = static_cast<Gfx*>(SystemArena_MallocDebug(DLSIZE * sizeof(Gfx), "../z_vismono.c", 289));
        glistpEnd = VisMono_DrawTexture(thisx, thisx->monoDl);
        ASSERT(glistpEnd <= thisx->monoDl + DLSIZE, "glistp_end <= thisx->mono_dl + DLSIZE", "../z_vismono.c", 292);
    }
}
