#include "file_choose.h"
#include "textures/title_static/title_static.h"
#include "assets/overlays/ovl_File_Choose/ovl_file_choose.h"

static s16 D_808124C0[] = {
    0x0002, 0x0003, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001, 0x0002, 0x0000, 0x0001,
    0x0001, 0x0002, 0x0001, 0x0001, 0x0004, 0x0002, 0x0002, 0x0002, 0x0001, 0x0001, 0x0000, 0x0002, 0x0000, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0000, 0x0001, 0x0001, 0x0001, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0003,
    0x0002, 0x0002, 0x0004, 0x0003, 0x0002, 0x0004, 0x0001, 0x0002, 0x0002, 0x0001, 0x0001, 0x0002, 0x0002, 0x0003,
    0x0002, 0x0002, 0x0000, 0x0002, 0x0002, 0x0002, 0x0000, 0x0003, 0x0001, 0x0000,
};

static s16 D_80812544[] = {
    0x0001, 0x0002, 0x0000, 0x0001, 0x0001, 0x0002, 0x0001, 0x0001, 0x0004, 0x0002, 0x0002, 0x0002, 0x0001,
    0x0001, 0x0000, 0x0002, 0x0000, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0000, 0x0001, 0x0001, 0x0001,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0003, 0x0002, 0x0002, 0x0004, 0x0003, 0x0002, 0x0004, 0x0001,
    0x0002, 0x0002, 0x0001, 0x0001, 0x0002, 0x0002, 0x0003, 0x0002, 0x0002, 0x0000, 0x0002, 0x0002, 0x0002,
    0x0003, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001, 0x0003,
};

void FileChoose_DrawCharacter(GraphicsContext* gfxCtx, void* texture, s16 vtx) {
    OPEN_DISPS(gfxCtx, "../z_file_nameset_PAL.c", 110);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(gfxCtx, "../z_file_nameset_PAL.c", 119);
}

void FileChoose_SetKeyboardVtx(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 val;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t3;
    s16 phi_s1;
    s16 phi_t1;
    s16 phi_s2;

    thisv->keyboardVtx = static_cast<Vtx*>( Graph_Alloc(thisv->state.gfxCtx, sizeof(Vtx) * 4 * 5 * 13) );

    phi_s1 = 0x26;

    for (phi_t2 = 0, phi_s2 = 0, phi_t3 = 0; phi_s2 < 5; phi_s2++) {
        phi_t0 = -0x60;

        for (phi_t1 = 0; phi_t1 < 13; phi_t1++, phi_t3 += 4, phi_t2++) {
            thisv->keyboardVtx[phi_t3].v.ob[0] = thisv->keyboardVtx[phi_t3 + 2].v.ob[0] = D_80812544[phi_t2] + phi_t0;

            thisv->keyboardVtx[phi_t3 + 1].v.ob[0] = thisv->keyboardVtx[phi_t3 + 3].v.ob[0] =
                D_80812544[phi_t2] + phi_t0 + 12;

            thisv->keyboardVtx[phi_t3].v.ob[1] = thisv->keyboardVtx[phi_t3 + 1].v.ob[1] = phi_s1;

            thisv->keyboardVtx[phi_t3 + 2].v.ob[1] = thisv->keyboardVtx[phi_t3 + 3].v.ob[1] = phi_s1 - 12;

            thisv->keyboardVtx[phi_t3].v.ob[2] = thisv->keyboardVtx[phi_t3 + 1].v.ob[2] =
                thisv->keyboardVtx[phi_t3 + 2].v.ob[2] = thisv->keyboardVtx[phi_t3 + 3].v.ob[2] = 0;

            thisv->keyboardVtx[phi_t3].v.flag = thisv->keyboardVtx[phi_t3 + 1].v.flag =
                thisv->keyboardVtx[phi_t3 + 2].v.flag = thisv->keyboardVtx[phi_t3 + 3].v.flag = 0;

            thisv->keyboardVtx[phi_t3].v.tc[0] = thisv->keyboardVtx[phi_t3].v.tc[1] =
                thisv->keyboardVtx[phi_t3 + 1].v.tc[1] = thisv->keyboardVtx[phi_t3 + 2].v.tc[0] = 0;

            thisv->keyboardVtx[phi_t3 + 1].v.tc[0] = thisv->keyboardVtx[phi_t3 + 2].v.tc[1] =
                thisv->keyboardVtx[phi_t3 + 3].v.tc[0] = thisv->keyboardVtx[phi_t3 + 3].v.tc[1] = 0x200;

            thisv->keyboardVtx[phi_t3].v.cn[0] = thisv->keyboardVtx[phi_t3 + 1].v.cn[0] =
                thisv->keyboardVtx[phi_t3 + 2].v.cn[0] = thisv->keyboardVtx[phi_t3 + 3].v.cn[0] =
                    thisv->keyboardVtx[phi_t3].v.cn[1] = thisv->keyboardVtx[phi_t3 + 1].v.cn[1] =
                        thisv->keyboardVtx[phi_t3 + 2].v.cn[1] = thisv->keyboardVtx[phi_t3 + 3].v.cn[1] =
                            thisv->keyboardVtx[phi_t3].v.cn[2] = thisv->keyboardVtx[phi_t3 + 1].v.cn[2] =
                                thisv->keyboardVtx[phi_t3 + 2].v.cn[2] = thisv->keyboardVtx[phi_t3 + 3].v.cn[2] =
                                    thisv->keyboardVtx[phi_t3].v.cn[3] = thisv->keyboardVtx[phi_t3 + 1].v.cn[3] =
                                        thisv->keyboardVtx[phi_t3 + 2].v.cn[3] = thisv->keyboardVtx[phi_t3 + 3].v.cn[3] =
                                            255;

            phi_t0 += 0x10;
        }

        phi_s1 -= 0x10;
    }
}

static const void* sNameLabelTextures[] = { gFileSelNameENGTex, gFileSelNameENGTex, gFileSelNameFRATex };

static const void* sBackspaceEndTextures[][2] = {
    { gFileSelBackspaceButtonTex, gFileSelENDButtonENGTex },
    { gFileSelBackspaceButtonTex, gFileSelENDButtonGERTex },
    { gFileSelBackspaceButtonTex, gFileSelENDButtonFRATex },
};

static u16 sBackspaceEndWidths[] = { 28, 44 };

static s16 D_808125EC[] = {
    0xFFE2, 0xFFF0, 0xFFFA, 0x0004, 0x000E, 0x0018, 0x0022, 0x002C, 0x0036, 0xFFF0, 0xFFF0,
};

static s16 D_80812604[] = {
    0x0048, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045,
};

/**
 * Set vertices used by all elements of the name entry screen that are NOT the keyboard.
 * This includes the cursor highlight, the name entry plate and characters, and the buttons.
 */
void FileChoose_SetNameEntryVtx(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Font* font = &thisv->font;
    s16 phi_s0;
    s16 phi_t1;
    u8 temp;
    s16 phi_v0;

    if (1) {}
    if (1) {}

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 205);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, D_80811BB0, 24, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sNameLabelTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_8b, 56, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);

    phi_s0 = 0x10;
    for (phi_t1 = 0; phi_t1 < 2; phi_t1++, phi_s0 += 4) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2], 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sBackspaceEndTextures[gSaveContext.language][phi_t1], G_IM_FMT_IA,
                            G_IM_SIZ_16b, sBackspaceEndWidths[phi_t1], 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, phi_s0, phi_s0 + 2, phi_s0 + 3, phi_s0 + 1, 0);
    }

    thisv->nameEntryVtx = static_cast<Vtx*>( Graph_Alloc(thisv->state.gfxCtx, 44 * sizeof(Vtx)) );

    for (phi_s0 = 0, phi_t1 = 0; phi_t1 < 44; phi_t1 += 4, phi_s0++) {
        if ((phi_s0 > 0) && (phi_s0 < 9)) {
            temp = thisv->fileNames[thisv->buttonIndex][phi_s0 - 1];

            thisv->nameEntryVtx[phi_t1].v.ob[0] = thisv->nameEntryVtx[phi_t1 + 2].v.ob[0] =
                D_808125EC[phi_s0] + thisv->nameEntryBoxPosX + D_808124C0[temp];

            thisv->nameEntryVtx[phi_t1 + 1].v.ob[0] = thisv->nameEntryVtx[phi_t1 + 3].v.ob[0] =
                thisv->nameEntryVtx[phi_t1].v.ob[0] + 0xA;
        } else {
            thisv->nameEntryVtx[phi_t1].v.ob[0] = thisv->nameEntryVtx[phi_t1 + 2].v.ob[0] =
                D_808125EC[phi_s0] + thisv->nameEntryBoxPosX;

            thisv->nameEntryVtx[phi_t1 + 1].v.ob[0] = thisv->nameEntryVtx[phi_t1 + 3].v.ob[0] =
                thisv->nameEntryVtx[phi_t1].v.ob[0] + 0xA;
        }

        thisv->nameEntryVtx[phi_t1].v.ob[1] = thisv->nameEntryVtx[phi_t1 + 1].v.ob[1] = D_80812604[phi_s0];

        thisv->nameEntryVtx[phi_t1 + 2].v.ob[1] = thisv->nameEntryVtx[phi_t1 + 3].v.ob[1] =
            thisv->nameEntryVtx[phi_t1].v.ob[1] - 0xA;

        thisv->nameEntryVtx[phi_t1].v.ob[2] = thisv->nameEntryVtx[phi_t1 + 1].v.ob[2] =
            thisv->nameEntryVtx[phi_t1 + 2].v.ob[2] = thisv->nameEntryVtx[phi_t1 + 3].v.ob[2] = 0;

        thisv->nameEntryVtx[phi_t1].v.flag = thisv->nameEntryVtx[phi_t1 + 1].v.flag =
            thisv->nameEntryVtx[phi_t1 + 2].v.flag = thisv->nameEntryVtx[phi_t1 + 3].v.flag = 0;

        thisv->nameEntryVtx[phi_t1].v.tc[0] = thisv->nameEntryVtx[phi_t1].v.tc[1] =
            thisv->nameEntryVtx[phi_t1 + 1].v.tc[1] = thisv->nameEntryVtx[phi_t1 + 2].v.tc[0] = 0;

        thisv->nameEntryVtx[phi_t1 + 1].v.tc[0] = thisv->nameEntryVtx[phi_t1 + 2].v.tc[1] =
            thisv->nameEntryVtx[phi_t1 + 3].v.tc[0] = thisv->nameEntryVtx[phi_t1 + 3].v.tc[1] = 0x200;

        thisv->nameEntryVtx[phi_t1].v.cn[0] = thisv->nameEntryVtx[phi_t1 + 1].v.cn[0] =
            thisv->nameEntryVtx[phi_t1 + 2].v.cn[0] = thisv->nameEntryVtx[phi_t1 + 3].v.cn[0] =
                thisv->nameEntryVtx[phi_t1].v.cn[1] = thisv->nameEntryVtx[phi_t1 + 1].v.cn[1] =
                    thisv->nameEntryVtx[phi_t1 + 2].v.cn[1] = thisv->nameEntryVtx[phi_t1 + 3].v.cn[1] =
                        thisv->nameEntryVtx[phi_t1].v.cn[2] = thisv->nameEntryVtx[phi_t1 + 1].v.cn[2] =
                            thisv->nameEntryVtx[phi_t1 + 2].v.cn[2] = thisv->nameEntryVtx[phi_t1 + 3].v.cn[2] =
                                thisv->nameEntryVtx[phi_t1].v.cn[3] = thisv->nameEntryVtx[phi_t1 + 1].v.cn[3] =
                                    thisv->nameEntryVtx[phi_t1 + 2].v.cn[3] = thisv->nameEntryVtx[phi_t1 + 3].v.cn[3] =
                                        0xFF;
    }

    thisv->nameEntryVtx[1].v.ob[0] = thisv->nameEntryVtx[3].v.ob[0] = thisv->nameEntryVtx[0].v.ob[0] + 0x6C;
    thisv->nameEntryVtx[2].v.ob[1] = thisv->nameEntryVtx[3].v.ob[1] = thisv->nameEntryVtx[0].v.ob[1] - 0x10;
    thisv->nameEntryVtx[1].v.tc[0] = thisv->nameEntryVtx[3].v.tc[0] = 0xD80;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                    thisv->nameEntryBoxAlpha);
    gSPVertex(POLY_OPA_DISP++, thisv->nameEntryVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPVertex(POLY_OPA_DISP++, thisv->nameEntryVtx + 4, 32, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->nameEntryBoxAlpha);

    for (phi_v0 = 0, phi_s0 = 0; phi_s0 < 0x20; phi_s0 += 4, phi_v0++) {
        FileChoose_DrawCharacter(thisv->state.gfxCtx,
                                 font->fontBuf + thisv->fileNames[thisv->buttonIndex][phi_v0] * FONT_CHAR_TEX_SIZE,
                                 phi_s0);
    }

    thisv->nameEntryVtx[0x25].v.tc[0] = thisv->nameEntryVtx[0x26].v.tc[1] = thisv->nameEntryVtx[0x27].v.tc[0] =
        thisv->nameEntryVtx[0x27].v.tc[1] = thisv->nameEntryVtx[0x29].v.tc[0] = thisv->nameEntryVtx[0x2A].v.tc[1] =
            thisv->nameEntryVtx[0x2B].v.tc[0] = thisv->nameEntryVtx[0x2B].v.tc[1] = 0x300;

    if ((thisv->kbdButton == 0) || (thisv->kbdButton == 1) || (thisv->kbdButton == 4)) {
        thisv->nameEntryVtx[0x29].v.tc[0] = thisv->nameEntryVtx[0x2B].v.tc[0] = 0x700;
    } else if ((thisv->kbdButton == 2) || (thisv->kbdButton == 3)) {
        thisv->nameEntryVtx[0x29].v.tc[0] = thisv->nameEntryVtx[0x2B].v.tc[0] = 0x500;
    }

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 307);
}

void FileChoose_DrawKeyboard(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Font* font = &thisv->font;
    s16 i = 0;
    s16 tmp;
    s16 vtx = 0;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 324);

    func_800949A8(thisv->state.gfxCtx);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, COMBINED, 0,
                      0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, thisv->charBgAlpha, 255, 255, 255, 255);

    while (vtx < 0x100) {
        gSPVertex(POLY_OPA_DISP++, &thisv->keyboardVtx[vtx], 32, 0);

        for (tmp = 0; tmp < 32; i++, tmp += 4) {
            FileChoose_DrawCharacter(thisv->state.gfxCtx, font->fontBuf + D_808123F0[i] * FONT_CHAR_TEX_SIZE, tmp);
        }

        vtx += 32;
    }

    gSPVertex(POLY_OPA_DISP++, &thisv->keyboardVtx[0x100], 4, 0);
    FileChoose_DrawCharacter(thisv->state.gfxCtx, font->fontBuf + D_808123F0[i] * FONT_CHAR_TEX_SIZE, 0);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 347);
}

void FileChoose_DrawNameEntry(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Font* font = &thisv->font;
    Input* input = &thisv->state.input[0];
    s16 i;
    s16 tmp;
    u16 dayTime;
    s16 validName;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 368);

    FileChoose_SetKeyboardVtx(&thisv->state);
    FileChoose_SetNameEntryVtx(&thisv->state);
    FileChoose_PulsateCursor(&thisv->state);

    tmp = (thisv->newFileNameCharCount * 4) + 4;
    thisv->nameEntryVtx[36].v.ob[0] = thisv->nameEntryVtx[38].v.ob[0] = thisv->nameEntryVtx[tmp].v.ob[0] - 6;
    thisv->nameEntryVtx[37].v.ob[0] = thisv->nameEntryVtx[39].v.ob[0] = thisv->nameEntryVtx[36].v.ob[0] + 24;
    thisv->nameEntryVtx[36].v.ob[1] = thisv->nameEntryVtx[37].v.ob[1] = thisv->nameEntryVtx[tmp].v.ob[1] + 7;
    thisv->nameEntryVtx[38].v.ob[1] = thisv->nameEntryVtx[39].v.ob[1] = thisv->nameEntryVtx[36].v.ob[1] - 24;

    Vtx* vertices = ResourceMgr_LoadVtxByName(D_80811BB0);

    if ((thisv->kbdButton == FS_KBD_BTN_HIRA) || (thisv->kbdButton == FS_KBD_BTN_KATA) ||
        (thisv->kbdButton == FS_KBD_BTN_END)) {
        if (thisv->kbdX != thisv->kbdButton) {
            osSyncPrintf("014 xpos=%d  contents=%d\n", thisv->kbdX, thisv->kbdButton);
        }

        thisv->nameEntryVtx[40].v.ob[0] = thisv->nameEntryVtx[42].v.ob[0] = vertices[(thisv->kbdX + 1) * 4].v.ob[0] - 4;
        thisv->nameEntryVtx[41].v.ob[0] = thisv->nameEntryVtx[43].v.ob[0] = thisv->nameEntryVtx[40].v.ob[0] + 52;
        thisv->nameEntryVtx[40].v.ob[1] = thisv->nameEntryVtx[41].v.ob[1] = vertices[(thisv->kbdX + 1) * 4].v.ob[1] + 4;

    } else if ((thisv->kbdButton == FS_KBD_BTN_ENG) || (thisv->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        if (thisv->kbdX != thisv->kbdButton) {
            osSyncPrintf("23 xpos=%d  contents=%d\n", thisv->kbdX, thisv->kbdButton);
        }
        thisv->nameEntryVtx[40].v.ob[0] = thisv->nameEntryVtx[42].v.ob[0] = vertices[(thisv->kbdX + 1) * 4].v.ob[0] - 4;
        thisv->nameEntryVtx[41].v.ob[0] = thisv->nameEntryVtx[43].v.ob[0] = thisv->nameEntryVtx[40].v.ob[0] + 40;
        thisv->nameEntryVtx[40].v.ob[1] = thisv->nameEntryVtx[41].v.ob[1] = vertices[(thisv->kbdX + 1) * 4].v.ob[1] + 4;
    } else {
        if (thisv->charIndex >= 65) {
            osSyncPrintf("mjp=%d  xpos=%d  ypos=%d  name_contents=%d\n", thisv->charIndex, thisv->kbdX, thisv->kbdY,
                         thisv->kbdButton);
        }

        thisv->nameEntryVtx[40].v.ob[0] = thisv->nameEntryVtx[42].v.ob[0] =
            thisv->keyboardVtx[thisv->charIndex * 4].v.ob[0] - D_80812544[thisv->charIndex] - 6;
        thisv->nameEntryVtx[41].v.ob[0] = thisv->nameEntryVtx[43].v.ob[0] = thisv->nameEntryVtx[40].v.ob[0] + 24;
        thisv->nameEntryVtx[40].v.ob[1] = thisv->nameEntryVtx[41].v.ob[1] =
            thisv->keyboardVtx[thisv->charIndex * 4].v.ob[1] + 6;
    }

    thisv->nameEntryVtx[42].v.ob[1] = thisv->nameEntryVtx[43].v.ob[1] = thisv->nameEntryVtx[40].v.ob[1] - 24;

    gSPVertex(POLY_OPA_DISP++, &thisv->nameEntryVtx[36], 8, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->highlightColor[0], thisv->highlightColor[1], thisv->highlightColor[2],
                    thisv->highlightColor[3]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelCharHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 24, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    if ((thisv->kbdButton == FS_KBD_BTN_HIRA) || (thisv->kbdButton == FS_KBD_BTN_KATA) ||
        (thisv->kbdButton == FS_KBD_BTN_END)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelMediumButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 56, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

    } else if ((thisv->kbdButton == FS_KBD_BTN_ENG) || (thisv->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelSmallButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 40, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    }

    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

    FileChoose_DrawKeyboard(&thisv->state);
    gDPPipeSync(POLY_OPA_DISP++);
    func_800949A8(thisv->state.gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    if (thisv->configMode == CM_NAME_ENTRY) {
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            // place cursor on END button
            thisv->kbdY = 5;
            thisv->kbdX = 4;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if ((thisv->newFileNameCharCount == 7) && (thisv->fileNames[thisv->buttonIndex][7] != 0x3E)) {
                for (i = thisv->newFileNameCharCount; i < 7; i++) {
                    thisv->fileNames[thisv->buttonIndex][i] = thisv->fileNames[thisv->buttonIndex][i + 1];
                }

                thisv->fileNames[thisv->buttonIndex][i] = 0x3E;
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                thisv->newFileNameCharCount--;

                if (thisv->newFileNameCharCount < 0) {
                    thisv->newFileNameCharCount = 0;
                    thisv->configMode = CM_NAME_ENTRY_TO_MAIN;
                } else {
                    for (i = thisv->newFileNameCharCount; i < 7; i++) {
                        thisv->fileNames[thisv->buttonIndex][i] = thisv->fileNames[thisv->buttonIndex][i + 1];
                    }

                    thisv->fileNames[thisv->buttonIndex][i] = 0x3E;
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                }
            }
        } else {
            if (thisv->charPage <= FS_CHAR_PAGE_ENG) {
                if (thisv->kbdY != 5) {
                    // draw the character the cursor is hovering over in yellow
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);
                    gSPVertex(POLY_OPA_DISP++, &thisv->keyboardVtx[thisv->charIndex * 4], 4, 0);

                    FileChoose_DrawCharacter(thisv->state.gfxCtx,
                                             font->fontBuf + D_808123F0[thisv->charIndex] * FONT_CHAR_TEX_SIZE, 0);

                    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        thisv->fileNames[thisv->buttonIndex][thisv->newFileNameCharCount] = D_808123F0[thisv->charIndex];
                        thisv->newFileNameCharCount++;

                        if (thisv->newFileNameCharCount > 7) {
                            thisv->newFileNameCharCount = 7;
                        }
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A) && (thisv->charPage != thisv->kbdButton)) {
                    if (thisv->kbdButton == FS_KBD_BTN_BACKSPACE) {
                        if ((thisv->newFileNameCharCount == 7) && (thisv->fileNames[thisv->buttonIndex][7] != 0x3E)) {
                            for (i = thisv->newFileNameCharCount; i < 7; i++) {
                                thisv->fileNames[thisv->buttonIndex][i] = thisv->fileNames[thisv->buttonIndex][i + 1];
                            }

                            thisv->fileNames[thisv->buttonIndex][i] = 0x3E;
                            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        } else {
                            thisv->newFileNameCharCount--;

                            if (thisv->newFileNameCharCount < 0) {
                                thisv->newFileNameCharCount = 0;
                            }

                            for (i = thisv->newFileNameCharCount; i < 7; i++) {
                                thisv->fileNames[thisv->buttonIndex][i] = thisv->fileNames[thisv->buttonIndex][i + 1];
                            }

                            thisv->fileNames[thisv->buttonIndex][i] = 0x3E;
                            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_S, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                    } else if (thisv->kbdButton == FS_KBD_BTN_END) {
                        validName = false;

                        for (i = 0; i < 8; i++) {
                            if (thisv->fileNames[thisv->buttonIndex][i] != 0x3E) {
                                validName = true;
                                break;
                            }
                        }

                        if (validName) {
                            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            gSaveContext.fileNum = thisv->buttonIndex;
                            dayTime = ((void)0, gSaveContext.dayTime);
                            Sram_InitSave(thisv, &thisv->sramCtx);
                            gSaveContext.dayTime = dayTime;
                            thisv->configMode = CM_NAME_ENTRY_TO_MAIN;
                            thisv->nameBoxAlpha[thisv->buttonIndex] = thisv->nameAlpha[thisv->buttonIndex] = 200;
                            thisv->connectorAlpha[thisv->buttonIndex] = 255;
                            func_800AA000(300.0f, 0xB4, 0x14, 0x64);
                        } else {
                            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                    }
                }

                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    thisv->newFileNameCharCount++;

                    if (thisv->newFileNameCharCount > 7) {
                        thisv->newFileNameCharCount = 7;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    thisv->newFileNameCharCount--;

                    if (thisv->newFileNameCharCount < 0) {
                        thisv->newFileNameCharCount = 0;
                    }
                }
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 550);
}

/**
 * Fade in the name entry box and slide it to the center of the screen from the right side.
 * After the name entry box is in place, init the keyboard/cursor and change modes.
 * Update function for `CM_START_NAME_ENTRY`
 */
void FileChoose_StartNameEntry(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->nameEntryBoxAlpha += 25;

    if (thisv->nameEntryBoxAlpha >= 255) {
        thisv->nameEntryBoxAlpha = 255;
    }

    thisv->nameEntryBoxPosX -= 30;

    if (thisv->nameEntryBoxPosX <= 0) {
        thisv->nameEntryBoxPosX = 0;
        thisv->nameEntryBoxAlpha = 255;
        thisv->kbdX = 0;
        thisv->kbdY = 0;
        thisv->kbdButton = 99;
        thisv->configMode = CM_NAME_ENTRY;
    }
}

/**
 * Update the keyboard cursor and play sounds at the appropriate times.
 * There are many special cases for warping the cursor depending on where
 * the cursor currently is.
 * Update function for `CM_NAME_ENTRY`
 */
void FileChoose_UpdateKeyboardCursor(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 prevKbdX;

    thisv->kbdButton = 99;

    if (thisv->kbdY != 5) {
        if (thisv->stickRelX < -30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->charIndex--;
            thisv->kbdX--;
            if (thisv->kbdX < 0) {
                thisv->kbdX = 12;
                thisv->charIndex = (thisv->kbdY * 13) + thisv->kbdX;
            }
        } else if (thisv->stickRelX > 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->charIndex++;
            thisv->kbdX++;
            if (thisv->kbdX > 12) {
                thisv->kbdX = 0;
                thisv->charIndex = (thisv->kbdY * 13) + thisv->kbdX;
            }
        }
    } else {
        if (thisv->stickRelX < -30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->kbdX--;
            if (thisv->kbdX < 3) {
                thisv->kbdX = 4;
            }
        } else if (thisv->stickRelX > 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->kbdX++;
            if (thisv->kbdX > 4) {
                thisv->kbdX = 3;
            }
        }
    }

    if (thisv->stickRelY > 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->kbdY--;

        if (thisv->kbdY < 0) {
            // dont go to bottom row
            if (thisv->kbdX < 8) {
                thisv->kbdY = 4;
                thisv->charIndex = (s32)(thisv->kbdX + 52);
            } else {
                thisv->kbdY = 5;
                thisv->charIndex += 52;
                prevKbdX = thisv->kbdX;

                if (thisv->kbdX < 10) {
                    thisv->kbdX = 3;
                } else if (thisv->kbdX < 13) {
                    thisv->kbdX = 4;
                }

                thisv->unk_1CAD6[thisv->kbdX] = prevKbdX;
            }
        } else {
            thisv->charIndex -= 13;

            if (thisv->kbdY == 4) {
                thisv->charIndex = 52;
                thisv->kbdX = thisv->unk_1CAD6[thisv->kbdX];
                thisv->charIndex += thisv->kbdX;
            }
        }
    } else if (thisv->stickRelY < -30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->kbdY++;

        if (thisv->kbdY > 5) {
            thisv->kbdY = 0;
            thisv->kbdX = thisv->unk_1CAD6[thisv->kbdX];
            thisv->charIndex = thisv->kbdX;
        } else {
            thisv->charIndex += 13;

            if (thisv->kbdY == 5) {
                if (thisv->kbdX < 8) {
                    thisv->kbdY = 0;
                    thisv->charIndex = thisv->kbdX;
                } else {
                    prevKbdX = thisv->kbdX;

                    if (thisv->kbdX < 3) {
                        thisv->kbdX = 0;
                    } else if (thisv->kbdX < 6) {
                        thisv->kbdX = 1;
                    } else if (thisv->kbdX < 8) {
                        thisv->kbdX = 2;
                    } else if (thisv->kbdX < 10) {
                        thisv->kbdX = 3;
                    } else if (thisv->kbdX < 13) {
                        thisv->kbdX = 4;
                    }

                    thisv->unk_1CAD6[thisv->kbdX] = prevKbdX;
                }
            }
        }
    }

    if (thisv->kbdY == 5) {
        thisv->kbdButton = thisv->kbdX;
    }
}

/**
 * This function is mostly a copy paste of `FileChoose_StartNameEntry`.
 * The name entry box fades and slides in even though it is not visible.
 * After thisv is complete, change to the options config mode.
 * Update function for `CM_START_OPTIONS`
 */
void FileChoose_StartOptions(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->nameEntryBoxAlpha += 25;

    if (thisv->nameEntryBoxAlpha >= 255) {
        thisv->nameEntryBoxAlpha = 255;
    }

    thisv->nameEntryBoxPosX -= 30;

    if (thisv->nameEntryBoxPosX <= 0) {
        thisv->nameEntryBoxPosX = 0;
        thisv->nameEntryBoxAlpha = 255;
        thisv->configMode = CM_OPTIONS_MENU;
    }
}

static u8 sSelectedSetting;

/**
 * Update the cursor and appropriate settings for the options menu.
 * If the player presses B, write the selected options to the SRAM header
 * and set config mode to rotate back to the main menu.
 * Update function for `CM_OPTIONS_MENU`
 */
void FileChoose_UpdateOptionsMenu(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->configMode = CM_OPTIONS_TO_MAIN;
        sramCtx->readBuff[0] = gSaveContext.audioSetting;
        sramCtx->readBuff[1] = gSaveContext.zTargetSetting;
        osSyncPrintf("ＳＡＶＥ");
        Sram_WriteSramHeader(sramCtx);
        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("sram->read_buff[2] = J_N = %x\n", sramCtx->readBuff[2]);
        osSyncPrintf("sram->read_buff[2] = J_N = %x\n", &sramCtx->readBuff[2]);
        osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
        osSyncPrintf(VT_RST);
        func_800F6700(gSaveContext.audioSetting);
        osSyncPrintf("終了\n");
        return;
    }

    if (thisv->stickRelX < -30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

        if (sSelectedSetting == FS_SETTING_AUDIO) {
            gSaveContext.audioSetting--;

            // because audio setting is unsigned, can't check for < 0
            if (gSaveContext.audioSetting > 0xF0) {
                gSaveContext.audioSetting = FS_AUDIO_SURROUND;
            }
        } else {
            gSaveContext.zTargetSetting ^= 1;
        }
    } else if (thisv->stickRelX > 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

        if (sSelectedSetting == FS_SETTING_AUDIO) {
            gSaveContext.audioSetting++;

            if (gSaveContext.audioSetting > FS_AUDIO_SURROUND) {
                gSaveContext.audioSetting = FS_AUDIO_STEREO;
            }
        } else {
            gSaveContext.zTargetSetting ^= 1;
        }
    }

    if ((thisv->stickRelY < -30) || (thisv->stickRelY > 30)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        sSelectedSetting ^= 1;
    } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        sSelectedSetting ^= 1;
    }
}

typedef struct {
    /* 0x00 */ const void* texture[3];
    /* 0x0C */ u16 width[3];
    /* 0x12 */ u16 height;
} OptionsMenuTextureInfo; // size = 0x14

static OptionsMenuTextureInfo gOptionsMenuHeaders[] = {
    {
        { gFileSelOptionsENGTex, gFileSelOptionsGERTex, gFileSelOptionsENGTex },
        { 128, 128, 128 },
        16,
    },
    {
        { gFileSelSOUNDENGTex, gFileSelSOUNDENGTex, gFileSelSOUNDFRATex },
        { 64, 64, 64 },
        16,
    },
    {
        { gFileSelLTargetingENGTex, gFileSelLTargetingGERTex, gFileSelLTargetingFRATex },
        { 64, 144, 64 },
        16,
    },
    {
        { gFileSelCheckBrightnessENGTex, gFileSelCheckBrightnessGERTex, gFileSelCheckBrightnessFRATex },
        { 128, 128, 128 },
        16,
    },
};

static OptionsMenuTextureInfo gOptionsMenuSettings[] = {
    {
        { gFileSelStereoENGTex, gFileSelStereoENGTex, gFileSelStereoFRATex },
        { 48, 48, 48 },
        16,
    },
    {
        { gFileSelMonoENGTex, gFileSelMonoENGTex, gFileSelMonoENGTex },
        { 48, 48, 48 },
        16,
    },
    {
        { gFileSelHeadsetENGTex, gFileSelHeadsetGERTex, gFileSelHeadsetFRATex },
        { 48, 48, 48 },
        16,
    },
    {
        { gFileSelSurroundENGTex, gFileSelSurroundENGTex, gFileSelSurroundENGTex },
        { 48, 48, 48 },
        16,
    },
    {
        { gFileSelSwitchENGTex, gFileSelSwitchGERTex, gFileSelSwitchFRATex },
        { 48, 80, 48 },
        16,
    },
    {
        { gFileSelHoldENGTex, gFileSelHoldGERTex, gFileSelHoldFRATex },
        { 48, 80, 48 },
        16,
    },
};

void FileChoose_DrawOptionsImpl(GameState* thisx) {
    static s16 cursorPrimRed = 255;
    static s16 cursorPrimGreen = 255;
    static s16 cursorPrimBlue = 255;
    static s16 cursorEnvRed = 0;
    static s16 cursorEnvGreen = 0;
    static s16 cursorEnvBlue = 0;
    static s16 cursorPulseDir = 1;
    static s16 cursorFlashTimer = 20;
    static s16 cursorPrimColors[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 cursorEnvColors[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 cursorRed;
    s16 cursorGreen;
    s16 cursorBlue;
    s16 i;
    s16 j;
    s16 vtx;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 848);

    cursorRed = ABS(cursorPrimRed - cursorPrimColors[cursorPulseDir][0]) / cursorFlashTimer;
    cursorGreen = ABS(cursorPrimGreen - cursorPrimColors[cursorPulseDir][1]) / cursorFlashTimer;
    cursorBlue = ABS(cursorPrimBlue - cursorPrimColors[cursorPulseDir][2]) / cursorFlashTimer;

    if (cursorPrimRed >= cursorPrimColors[cursorPulseDir][0]) {
        cursorPrimRed -= cursorRed;
    } else {
        cursorPrimRed += cursorRed;
    }

    if (cursorPrimGreen >= cursorPrimColors[cursorPulseDir][1]) {
        cursorPrimGreen -= cursorGreen;
    } else {
        cursorPrimGreen += cursorGreen;
    }

    if (cursorPrimBlue >= cursorPrimColors[cursorPulseDir][2]) {
        cursorPrimBlue -= cursorBlue;
    } else {
        cursorPrimBlue += cursorBlue;
    }

    cursorRed = ABS(cursorEnvRed - cursorEnvColors[cursorPulseDir][0]) / cursorFlashTimer;
    cursorGreen = ABS(cursorEnvGreen - cursorEnvColors[cursorPulseDir][1]) / cursorFlashTimer;
    cursorBlue = ABS(cursorEnvBlue - cursorEnvColors[cursorPulseDir][2]) / cursorFlashTimer;

    if (cursorEnvRed >= cursorEnvColors[cursorPulseDir][0]) {
        cursorEnvRed -= cursorRed;
    } else {
        cursorEnvRed += cursorRed;
    }

    if (cursorEnvGreen >= cursorEnvColors[cursorPulseDir][1]) {
        cursorEnvGreen -= cursorGreen;
    } else {
        cursorEnvGreen += cursorGreen;
    }

    if (cursorEnvBlue >= cursorEnvColors[cursorPulseDir][2]) {
        cursorEnvBlue -= cursorBlue;
    } else {
        cursorEnvBlue += cursorBlue;
    }

    if (--cursorFlashTimer == 0) {
        cursorPrimRed = cursorPrimColors[cursorPulseDir][0];
        cursorPrimGreen = cursorPrimColors[cursorPulseDir][1];
        cursorPrimBlue = cursorPrimColors[cursorPulseDir][2];

        cursorEnvRed = cursorEnvColors[cursorPulseDir][0];
        cursorEnvGreen = cursorEnvColors[cursorPulseDir][1];
        cursorEnvBlue = cursorEnvColors[cursorPulseDir][2];

        cursorFlashTimer = 20;

        if (++cursorPulseDir > 1) {
            cursorPulseDir = 0;
        }
    }

    if (gSaveContext.language == LANGUAGE_GER) {
        gSPVertex(POLY_OPA_DISP++, D_80811E30, 32, 0);
    } else {
        gSPVertex(POLY_OPA_DISP++, D_80811D30, 32, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuHeaders[i].texture[gSaveContext.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, gOptionsMenuHeaders[i].width[gSaveContext.language],
                            gOptionsMenuHeaders[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    if (gSaveContext.language == LANGUAGE_GER) {
        gSPVertex(POLY_OPA_DISP++, D_80812130, 32, 0);
    } else {
        gSPVertex(POLY_OPA_DISP++, D_80811F30, 32, 0);
    }

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == gSaveContext.audioSetting) {
            if (sSelectedSetting == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, cursorPrimRed, cursorPrimGreen, cursorPrimBlue,
                                thisv->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, cursorEnvRed, cursorEnvGreen, cursorEnvBlue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, thisv->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuSettings[i].texture[gSaveContext.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, gOptionsMenuSettings[i].width[gSaveContext.language],
                            gOptionsMenuHeaders[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (gSaveContext.zTargetSetting + 4)) {
            if (sSelectedSetting != FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, cursorPrimRed, cursorPrimGreen, cursorPrimBlue,
                                thisv->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, cursorEnvRed, cursorEnvGreen, cursorEnvBlue, 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, thisv->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, gOptionsMenuSettings[i].texture[gSaveContext.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, gOptionsMenuSettings[i].width[gSaveContext.language],
                            gOptionsMenuHeaders[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // check brightness bars
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    Matrix_Push();
    Matrix_Translate(0.0f, 0.1f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 1009),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerTopVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.2f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 1021),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPVertex(POLY_OPA_DISP++, gOptionsDividerMiddleVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    Matrix_Push();
    Matrix_Translate(0.0f, 0.4f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 1033),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerBottomVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_Pop();

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_nameset_PAL.c", 1040);
}

void FileChoose_DrawOptions(GameState* thisx) {
    FileChoose_DrawOptionsImpl(thisx);
}
