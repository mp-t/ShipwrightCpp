#include "global.h"
#include <string.h>

Gfx D_8012AFB0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetColorImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 0x0F000000),
    gsDPSetFillColor((GPACK_RGBA5551(65, 65, 65, 1) << 16) | GPACK_RGBA5551(65, 65, 65, 1)),
    gsDPFillRectangle(0, 0, 319, 239),
    gsDPPipeSync(),
    gsDPSetFillColor((GPACK_RGBA5551(65, 65, 255, 1) << 16) | GPACK_RGBA5551(65, 65, 255, 1)),
    gsDPFillRectangle(20, 20, 300, 220),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx D_8012B000[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
    gsSPEndDisplayList(),
};

void TransitionUnk_InitGraphics(TransitionUnk* thisx) {
    s32 row2;
    s32 pad2;
    s32 pad3;
    Vtx_t* vtx2;
    s32 frame;
    s32 rowTex;
    s32 row;
    Gfx* gfx;
    Vtx* vtx;
    s32 col;
    s32 colTex;

    guMtxIdent(&thisx->modelView);
    guMtxIdent(&thisx->unk_98);
    guOrtho(&thisx->projection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1000.0f, 1000.0f, 1.0f);

    for (frame = 0; frame < 2; frame++) {
        thisx->frame = frame;
        vtx = (thisx->frame == 0) ? thisx->vtxFrame1 : thisx->vtxFrame2;
        for (colTex = 0, col = 0; col < thisx->col + 1; colTex += 0x20, col++) {
            for (rowTex = 0, row = 0; row < thisx->row + 1; row++) {
                vtx2 = &vtx->v;
                vtx++;

                vtx2->tc[0] = rowTex << 6;
                vtx2->ob[0] = row * 0x20;
                vtx2->ob[1] = col * 0x20;
                vtx2->ob[2] = -5;
                vtx2->flag = 0;
                vtx2->tc[1] = colTex << 6;
                vtx2->cn[0] = 0;
                vtx2->cn[1] = 0;
                vtx2->cn[2] = 120;
                vtx2->cn[3] = 255;
                rowTex += 0x20;
            }
        }
    }

    gfx = thisx->gfx;
    for (colTex = 0, col = 0; col < thisx->col; colTex += 0x20, col++) {

        gSPVertex(gfx++, SEGMENT_ADDR(0xA, (u32)col * (thisx->row + 1) * sizeof(Vtx)), 2 * (thisx->row + 1), 0);

        for (rowTex = 0, row = 0, row2 = 0; row < thisx->row;) {
            gDPPipeSync(gfx++);

            gDPLoadTextureTile(gfx++, reinterpret_cast<const void*>(SEGMENT_ADDR(0xB, 0)), G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, SCREEN_HEIGHT,
                               rowTex, colTex, rowTex + 0x20, colTex + 0x20, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(gfx++, row, row + 1, row2 + thisx->row + 2, thisx->row + row2 + 1, 0);

            rowTex += 0x20;
            row2++;
            row++;
        }
    }

    gDPPipeSync(gfx++);
    gSPEndDisplayList(gfx++);

    LOG_NUM("thisx->col * (1 + thisx->row * (1 + 7 + 1)) + 1 + 1", thisx->col * (1 + thisx->row * 9) + 2, "../z_fbdemo.c",
            144);
    LOG_NUM("gp - thisx->gfxtbl", gfx - thisx->gfx, "../z_fbdemo.c", 145);
}

void TransitionUnk_InitData(TransitionUnk* thisx) {
    s32 col;
    s32 row;

    for (col = 0; col < thisx->col + 1; col++) {
        for (row = 0; row < thisx->row + 1; row++) {
            (thisx->unk_0C + row + col * (thisx->row + 1))->unk_0 = row * 32;
            (thisx->unk_0C + row + col * (thisx->row + 1))->unk_4 = col * 32;
        }
    }
}

void TransitionUnk_Destroy(TransitionUnk* thisx) {
    osSyncPrintf("fbdemo_cleanup(%08x)\n", thisx);
    osSyncPrintf("msleep(100);\n");
    Sleep_Msec(100);

    if (thisx->unk_0C != NULL) {
        SystemArena_FreeDebug(thisx->unk_0C, "../z_fbdemo.c", 180);
        thisx->unk_0C = NULL;
    }
    if (thisx->vtxFrame1 != NULL) {
        SystemArena_FreeDebug(thisx->vtxFrame1, "../z_fbdemo.c", 181);
        thisx->vtxFrame1 = NULL;
    }
    if (thisx->vtxFrame2 != NULL) {
        SystemArena_FreeDebug(thisx->vtxFrame2, "../z_fbdemo.c", 182);
        thisx->vtxFrame2 = NULL;
    }
    if (thisx->gfx != NULL) {
        SystemArena_FreeDebug(thisx->gfx, "../z_fbdemo.c", 183);
        thisx->gfx = NULL;
    }
}

TransitionUnk* TransitionUnk_Init(TransitionUnk* thisx, s32 row, s32 col) {
    osSyncPrintf("fbdemo_init(%08x, %d, %d)\n", thisx, row, col);
    memset(thisx, 0, sizeof(*thisx));
    thisx->frame = 0;
    thisx->row = row;
    thisx->col = col;
    thisx->unk_0C = static_cast<TransitionUnkData*>( SystemArena_MallocDebug((row + 1) * sizeof(TransitionUnkData) * (col + 1), "../z_fbdemo.c", 195) );
    thisx->vtxFrame1 = static_cast<Vtx*>( SystemArena_MallocDebug((row + 1) * sizeof(Vtx) * (col + 1), "../z_fbdemo.c", 196) );
    thisx->vtxFrame2 = static_cast<Vtx*>( SystemArena_MallocDebug((row + 1) * sizeof(Vtx) * (col + 1), "../z_fbdemo.c", 197) );
    thisx->gfx = static_cast<Gfx*>( SystemArena_MallocDebug((thisx->col * (1 + thisx->row * 9) + 2) * sizeof(Gfx), "../z_fbdemo.c", 198) );

    if (thisx->unk_0C == NULL || thisx->vtxFrame1 == NULL || thisx->vtxFrame2 == NULL || thisx->gfx == NULL) {
        osSyncPrintf("fbdemo_init allocation error\n");
        if (thisx->unk_0C != NULL) {
            SystemArena_FreeDebug(thisx->unk_0C, "../z_fbdemo.c", 202);
            thisx->unk_0C = NULL;
        }
        if (thisx->vtxFrame1 != NULL) {
            SystemArena_FreeDebug(thisx->vtxFrame1, "../z_fbdemo.c", 203);
            thisx->vtxFrame1 = NULL;
        }
        if (thisx->vtxFrame2 != NULL) {
            SystemArena_FreeDebug(thisx->vtxFrame2, "../z_fbdemo.c", 204);
            thisx->vtxFrame2 = NULL;
        }
        if (thisx->gfx != NULL) {
            SystemArena_FreeDebug(thisx->gfx, "../z_fbdemo.c", 205);
            thisx->gfx = NULL;
        }
        return NULL;
    }
    TransitionUnk_InitGraphics(thisx);
    TransitionUnk_InitData(thisx);
    thisx->frame = 0;

    return thisx;
}

void TransitionUnk_SetData(TransitionUnk* thisx) {
    s32 col;
    Vtx* vtx;
    s32 row;

    for (col = 0; col < thisx->col + 1; col++) {
        for (row = 0; row < thisx->row + 1; row++) {
            vtx = (thisx->frame == 0) ? thisx->vtxFrame1 : thisx->vtxFrame2;
            (vtx + row + col * (thisx->row + 1))->v.ob[0] = (thisx->unk_0C + row + col * (thisx->row + 1))->unk_0;
            vtx = (thisx->frame == 0) ? thisx->vtxFrame1 : thisx->vtxFrame2;
            (vtx + row + col * (thisx->row + 1))->v.ob[1] = (thisx->unk_0C + row + col * (thisx->row + 1))->unk_4;
        }
    }
}

void TransitionUnk_Draw(TransitionUnk* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, D_8012B000);
    TransitionUnk_SetData(thisx);
    gSPMatrix(gfx++, &thisx->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &thisx->modelView, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(gfx++, 0xA, thisx->frame == 0 ? thisx->vtxFrame1 : thisx->vtxFrame2);
    gSPSegment(gfx++, 0xB, thisx->zBuffer);
    gSPDisplayList(gfx++, D_8012B000);
    gSPDisplayList(gfx++, thisx->gfx);
    gDPPipeSync(gfx++);
    thisx->frame ^= 1;
    *gfxP = gfx;
}

void TransitionUnk_Update(TransitionUnk* thisx) {
    f32 temp_f00;
    f32 temp_f12;
    s32 col;
    f32 phi_f14;
    s32 row;

    for (col = 0; col < thisx->col + 1; col++) {
        for (row = 0; row < thisx->row + 1; row++) {
            temp_f00 =
                (thisx->unk_0C + row + col * (thisx->row + 1))->unk_0 - (thisx->unk_0C + 5 + 4 * (thisx->row + 1))->unk_0;
            temp_f12 =
                (thisx->unk_0C + row + col * (thisx->row + 1))->unk_4 - (thisx->unk_0C + 5 + 4 * (thisx->row + 1))->unk_4;
            phi_f14 = (SQ(temp_f00) + SQ(temp_f12)) / 100.0f;
            if (phi_f14 != 0.0f) {
                if (phi_f14 < 1.0f) {
                    phi_f14 = 1.0f;
                }
                (thisx->unk_0C + row + col * (thisx->row + 1))->unk_0 -= temp_f00 / phi_f14;
                (thisx->unk_0C + row + col * (thisx->row + 1))->unk_4 -= temp_f12 / phi_f14;
            }
        }
    }
}

void func_800B23E8(TransitionUnk* thisx) {
}

s32 func_800B23F0(TransitionUnk* thisx) {
    return 0;
}
