#include "file_choose.h"

#include <string.h>

#include "textures/title_static/title_static.h"
#include "textures/parameter_static/parameter_static.h"

static s16 sUnused = 106;

static s16 sScreenFillAlpha = 255;

static Gfx sScreenFillSetupDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

static s16 sFileInfoBoxPartWidths[] = { 36, 36, 36, 36, 24 };

static s16 sWindowContentColors[2][3] = {
    { 100, 150, 255 }, // blue
    { 100, 100, 100 }, // gray
};

void FileChoose_SetView(FileChooseContext* thisv, f32 eyeX, f32 eyeY, f32 eyeZ) {
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.x = up.z = 0.0f;

    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;

    up.y = 1.0f;

    func_800AA358(&thisv->view, &eye, &lookAt, &up);
    func_800AAA50(&thisv->view, 0x7F);
}

Gfx* FileChoose_QuadTextureIA8(Gfx* gfx, const char* texture, s16 width, s16 height, s16 point) 
{
    if (ResourceMgr_OTRSigCheck(texture))
        texture = ResourceMgr_LoadTexByName(texture);

    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

void FileChoose_InitModeUpdate(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    if (thisv->menuMode == FS_MENU_MODE_INIT) {
        thisv->menuMode = FS_MENU_MODE_CONFIG;
        thisv->configMode = CM_FADE_IN_START;
        thisv->nextTitleLabel = FS_TITLE_OPEN_FILE;
        osSyncPrintf("Ｓｒａｍ Ｓｔａｒｔ─Ｌｏａｄ  》》》》》  ");
        Sram_VerifyAndLoadAllSaves(thisv, &thisv->sramCtx);
        osSyncPrintf("終了！！！\n");
    }
}

void FileChoose_InitModeDraw(GameState* thisx) {
}

/**
 * Fade in the menu window and title label.
 * If a file is occupied fade in the name, name box, and connector.
 * Fade in the copy erase and options button according to the window alpha.
 */
void FileChoose_FadeInMenuElements(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;

    thisv->titleAlpha[0] += VREG(1);
    thisv->windowAlpha += VREG(2);

    for (i = 0; i < 3; i++) {
        thisv->fileButtonAlpha[i] = thisv->windowAlpha;

        if (SLOT_OCCUPIED(sramCtx, i)) {
            thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->windowAlpha;
            thisv->connectorAlpha[i] += VREG(1);
            if (thisv->connectorAlpha[i] >= 255) {
                thisv->connectorAlpha[i] = 255;
            }
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        thisv->optionButtonAlpha = thisv->windowAlpha;
}

/**
 * Converts a numerical value to ones-tens-hundreds digits
 */
void FileChoose_SplitNumber(u16 value, s16* hundreds, s16* tens, s16* ones) {
    *hundreds = 0;
    *tens = 0;
    *ones = value;

    while (true) {
        if ((*ones - 100) < 0) {
            break;
        }
        (*hundreds)++;
        *ones -= 100;
    }

    while (true) {
        if ((*ones - 10) < 0) {
            break;
        }
        (*tens)++;
        *ones -= 10;
    }
}

/**
 * Reduce the alpha of the black screen fill to create a fade in effect.
 * Additionally, slide the window from the right to the center of the screen.
 * Update function for `CM_FADE_IN_START`
 */
void FileChoose_StartFadeIn(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    FileChoose_FadeInMenuElements(&thisv->state);
    sScreenFillAlpha -= 40;
    thisv->windowPosX -= 20;

    if (thisv->windowPosX <= -94) {
        thisv->windowPosX = -94;
        thisv->configMode = CM_FADE_IN_END;
        sScreenFillAlpha = 0;
    }
}

/**
 * Finish fading in the remaining menu elements.
 * Fade in the controls text at the bottom of the screen.
 * Update function for `CM_FADE_IN_END`
 */
void FileChoose_FinishFadeIn(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->controlsAlpha += VREG(1);
    FileChoose_FadeInMenuElements(&thisv->state);

    if (thisv->titleAlpha[0] >= 255) {
        thisv->titleAlpha[0] = 255;
        thisv->controlsAlpha = 255;
        thisv->windowAlpha = 200;
        thisv->configMode = CM_MAIN_MENU;
    }
}

/**
 * Update the cursor and wait for the player to select a button to change menus accordingly.
 * If an empty file is selected, enter the name entry config mode.
 * If an occupied file is selected, enter the `Select` menu mode.
 * If copy, erase, or options is selected, set config mode accordingly.
 * Lastly, set any warning labels if appropriate.
 * Update function for `CM_MAIN_MENU`
 */
void FileChoose_UpdateMainMenu(GameState* thisx) {
    static u8 emptyName[] = { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        if (thisv->buttonIndex <= FS_BTN_MAIN_FILE_3) {
            osSyncPrintf("REGCK_ALL[%x]=%x,%x,%x,%x,%x,%x\n", thisv->buttonIndex,
                         GET_NEWF(sramCtx, thisv->buttonIndex, 0), GET_NEWF(sramCtx, thisv->buttonIndex, 1),
                         GET_NEWF(sramCtx, thisv->buttonIndex, 2), GET_NEWF(sramCtx, thisv->buttonIndex, 3),
                         GET_NEWF(sramCtx, thisv->buttonIndex, 4), GET_NEWF(sramCtx, thisv->buttonIndex, 5));

            if (!SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->configMode = CM_ROTATE_TO_NAME_ENTRY;
                thisv->kbdButton = FS_KBD_BTN_NONE;
                thisv->charPage = FS_CHAR_PAGE_ENG;
                thisv->kbdX = 0;
                thisv->kbdY = 0;
                thisv->charIndex = 0;
                thisv->charBgAlpha = 0;
                thisv->newFileNameCharCount = 0;
                thisv->nameEntryBoxPosX = 120;
                thisv->nameEntryBoxAlpha = 0;
                memcpy(&thisv->fileNames[thisv->buttonIndex][0], &emptyName, 8);
            } else if (thisv->n64ddFlags[thisv->buttonIndex] == thisv->n64ddFlag) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->actionTimer = 8;
                thisv->selectMode = SM_FADE_MAIN_TO_SELECT;
                thisv->selectedFileIndex = thisv->buttonIndex;
                thisv->menuMode = FS_MENU_MODE_SELECT;
                thisv->nextTitleLabel = FS_TITLE_OPEN_FILE;
            } else if (!thisv->n64ddFlags[thisv->buttonIndex]) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        } else {
            if (thisv->warningLabel == FS_WARNING_NONE) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->prevConfigMode = thisv->configMode;

                if (thisv->buttonIndex == FS_BTN_MAIN_COPY) {
                    thisv->configMode = CM_SETUP_COPY_SOURCE;
                    thisv->nextTitleLabel = FS_TITLE_COPY_FROM;
                } else if (thisv->buttonIndex == FS_BTN_MAIN_ERASE) {
                    thisv->configMode = CM_SETUP_ERASE_SELECT;
                    thisv->nextTitleLabel = FS_TITLE_ERASE_FILE;
                } else {
                    thisv->configMode = CM_MAIN_TO_OPTIONS;
                    thisv->kbdButton = 0;
                    thisv->kbdX = 0;
                    thisv->kbdY = 0;
                    thisv->charBgAlpha = 0;
                    thisv->newFileNameCharCount = 0;
                    thisv->nameEntryBoxPosX = 120;
                }

                thisv->actionTimer = 8;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    } else {
        if (ABS(thisv->stickRelY) > 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (thisv->stickRelY > 30) {
                thisv->buttonIndex--;
                if (thisv->buttonIndex < FS_BTN_MAIN_FILE_1) {
                    thisv->buttonIndex = FS_BTN_MAIN_OPTIONS;
                }
            } else {
                thisv->buttonIndex++;
                if (thisv->buttonIndex > FS_BTN_MAIN_OPTIONS) {
                    thisv->buttonIndex = FS_BTN_MAIN_FILE_1;
                }
            }
        }

        if (thisv->buttonIndex == FS_BTN_MAIN_COPY) {
            if (!SLOT_OCCUPIED(sramCtx, 0) && !SLOT_OCCUPIED(sramCtx, 1) && !SLOT_OCCUPIED(sramCtx, 2)) {
                thisv->warningButtonIndex = thisv->buttonIndex;
                thisv->warningLabel = FS_WARNING_NO_FILE_COPY;
                thisv->emptyFileTextAlpha = 255;
            } else if (SLOT_OCCUPIED(sramCtx, 0) && SLOT_OCCUPIED(sramCtx, 1) && SLOT_OCCUPIED(sramCtx, 2)) {
                thisv->warningButtonIndex = thisv->buttonIndex;
                thisv->warningLabel = FS_WARNING_NO_EMPTY_FILES;
                thisv->emptyFileTextAlpha = 255;
            } else {
                thisv->warningLabel = FS_WARNING_NONE;
            }
        } else if (thisv->buttonIndex == FS_BTN_MAIN_ERASE) {
            if (!SLOT_OCCUPIED(sramCtx, 0) && !SLOT_OCCUPIED(sramCtx, 1) && !SLOT_OCCUPIED(sramCtx, 2)) {
                thisv->warningButtonIndex = thisv->buttonIndex;
                thisv->warningLabel = FS_WARNING_NO_FILE_ERASE;
                thisv->emptyFileTextAlpha = 255;
            } else {
                thisv->warningLabel = FS_WARNING_NONE;
            }
        } else {
            thisv->warningLabel = FS_WARNING_NONE;
        }
    }
}

/**
 * Update function for `CM_UNUSED_31`
 */
void FileChoose_UnusedCM31(GameState* thisx) {
}

/**
 * Delay the next config mode from running until `XREG(73)` reaches 254.
 * Because the timer increments by 2, the delay is 127 frames (assuming the value was not changed by reg editor).
 * Unused in the final game, was possibly used for debugging.
 * Update function for `CM_UNUSED_DELAY`
 */
void FileChoose_UnusedCMDelay(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    XREG(73) += 2;

    if (XREG(73) == 254) {
        thisv->configMode = thisv->nextConfigMode;
        XREG(73) = 0;
    }
}

/**
 * Rotate the window from the main menu to the name entry menu.
 * Update function for `CM_ROTATE_TO_NAME_ENTRY`
 */
void FileChoose_RotateToNameEntry(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->windowRot += VREG(16);

    if (thisv->windowRot >= 314.0f) {
        thisv->windowRot = 314.0f;
        thisv->configMode = CM_START_NAME_ENTRY;
    }
}

/**
 * Rotate the window from the main menu to the options menu.
 * Update function for `CM_MAIN_TO_OPTIONS`
 */
void FileChoose_RotateToOptions(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->windowRot += VREG(16);

    if (thisv->windowRot >= 314.0f) {
        thisv->windowRot = 314.0f;
        thisv->configMode = CM_START_OPTIONS;
    }
}

/**
 * Rotate the window from the options menu to the main menu.
 * Update function for `CM_NAME_ENTRY_TO_MAIN` and `CM_OPTIONS_TO_MAIN`
 */
void FileChoose_RotateToMain(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->windowRot += VREG(16);

    if (thisv->windowRot >= 628.0f) {
        thisv->windowRot = 0.0f;
        thisv->configMode = CM_MAIN_MENU;
    }
}

static void (*gConfigModeUpdateFuncs[])(GameState*) = {
    FileChoose_StartFadeIn,        FileChoose_FinishFadeIn,
    FileChoose_UpdateMainMenu,     FileChoose_SetupCopySource,
    FileChoose_SelectCopySource,   FileChoose_SetupCopyDest1,
    FileChoose_SetupCopyDest2,     FileChoose_SelectCopyDest,
    FileChoose_ExitToCopySource1,  FileChoose_ExitToCopySource2,
    FileChoose_SetupCopyConfirm1,  FileChoose_SetupCopyConfirm2,
    FileChoose_CopyConfirm,        FileChoose_ReturnToCopyDest,
    FileChoose_CopyAnim1,          FileChoose_CopyAnim2,
    FileChoose_CopyAnim3,          FileChoose_CopyAnim4,
    FileChoose_CopyAnim5,          FileChoose_ExitCopyToMain,
    FileChoose_SetupEraseSelect,   FileChoose_EraseSelect,
    FileChoose_SetupEraseConfirm1, FileChoose_SetupEraseConfirm2,
    FileChoose_EraseConfirm,       FileChoose_ExitToEraseSelect1,
    FileChoose_ExitToEraseSelect2, FileChoose_EraseAnim1,
    FileChoose_EraseAnim2,         FileChoose_EraseAnim3,
    FileChoose_ExitEraseToMain,    FileChoose_UnusedCM31,
    FileChoose_RotateToNameEntry,  FileChoose_UpdateKeyboardCursor,
    FileChoose_StartNameEntry,     FileChoose_RotateToMain,
    FileChoose_RotateToOptions,    FileChoose_UpdateOptionsMenu,
    FileChoose_StartOptions,       FileChoose_RotateToMain,
    FileChoose_UnusedCMDelay,
};

/**
 * Updates the alpha of the cursor to make it pulsate.
 * On the debug rom, thisv function also handles switching languages with controller 3.
 */
void FileChoose_PulsateCursor(GameState* thisx) {
    static s16 cursorAlphaTargets[] = { 70, 200 };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 alphaStep;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* debugInput = &thisv->state.input[2];

    if (CHECK_BTN_ALL(debugInput->press.button, BTN_DLEFT)) {
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language = LANGUAGE_ENG;
        *((u8*)0x80000002) = LANGUAGE_ENG;

        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, 3, OS_WRITE);
        osSyncPrintf("1:read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);

        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);
        osSyncPrintf("read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);
    } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DUP)) {
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language = LANGUAGE_GER;
        *((u8*)0x80000002) = LANGUAGE_GER;

        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, 3, OS_WRITE);
        osSyncPrintf("1:read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);
        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);
        osSyncPrintf("read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);
    } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DRIGHT)) {
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language = LANGUAGE_FRA;
        *((u8*)0x80000002) = LANGUAGE_FRA;

        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, 3, OS_WRITE);
        osSyncPrintf("1:read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);

        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);
        osSyncPrintf("read_buff[]=%x, %x, %x, %x\n", sramCtx->readBuff[SRAM_HEADER_SOUND],
                     sramCtx->readBuff[SRAM_HEADER_ZTARGET], sramCtx->readBuff[SRAM_HEADER_LANGUAGE],
                     sramCtx->readBuff[SRAM_HEADER_MAGIC]);
    }

    alphaStep = ABS(thisv->highlightColor[3] - cursorAlphaTargets[thisv->highlightPulseDir]) / XREG(35);

    if (thisv->highlightColor[3] >= cursorAlphaTargets[thisv->highlightPulseDir]) {
        thisv->highlightColor[3] -= alphaStep;
    } else {
        thisv->highlightColor[3] += alphaStep;
    }

    XREG(35)--;

    if (XREG(35) == 0) {
        thisv->highlightColor[3] = cursorAlphaTargets[thisv->highlightPulseDir];
        XREG(35) = XREG(36 + thisv->highlightPulseDir);
        thisv->highlightPulseDir ^= 1;
    }
}

void FileChoose_ConfigModeUpdate(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    gConfigModeUpdateFuncs[thisv->configMode](&thisv->state);
}

void FileChoose_SetWindowVtx(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 i;
    s16 j;
    s16 x;
    s16 tmp;
    s16 tmp2;
    s16 tmp3;

    thisv->windowVtx = static_cast<Vtx*>( Graph_Alloc(thisv->state.gfxCtx, sizeof(Vtx) * 80) );
    tmp = thisv->windowPosX - 90;

    for (x = 0, i = 0; i < 4; i++) {
        tmp += 0x40;
        tmp2 = (i == 3) ? 0x30 : 0x40;

        for (j = 0, tmp3 = 0x50; j < 5; j++, x += 4, tmp3 -= 0x20) {
            thisv->windowVtx[x].v.ob[0] = thisv->windowVtx[x + 2].v.ob[0] = tmp;

            thisv->windowVtx[x + 1].v.ob[0] = thisv->windowVtx[x + 3].v.ob[0] = tmp + tmp2;

            thisv->windowVtx[x].v.ob[1] = thisv->windowVtx[x + 1].v.ob[1] = tmp3;

            thisv->windowVtx[x + 2].v.ob[1] = thisv->windowVtx[x + 3].v.ob[1] = tmp3 - 0x20;

            thisv->windowVtx[x].v.ob[2] = thisv->windowVtx[x + 1].v.ob[2] = thisv->windowVtx[x + 2].v.ob[2] =
                thisv->windowVtx[x + 3].v.ob[2] = 0;

            thisv->windowVtx[x].v.flag = thisv->windowVtx[x + 1].v.flag = thisv->windowVtx[x + 2].v.flag =
                thisv->windowVtx[x + 3].v.flag = 0;

            thisv->windowVtx[x].v.tc[0] = thisv->windowVtx[x].v.tc[1] = thisv->windowVtx[x + 1].v.tc[1] =
                thisv->windowVtx[x + 2].v.tc[0] = 0;

            thisv->windowVtx[x + 1].v.tc[0] = thisv->windowVtx[x + 3].v.tc[0] = tmp2 * 0x20;

            thisv->windowVtx[x + 2].v.tc[1] = thisv->windowVtx[x + 3].v.tc[1] = 0x400;

            thisv->windowVtx[x].v.cn[0] = thisv->windowVtx[x + 2].v.cn[0] = thisv->windowVtx[x].v.cn[1] =
                thisv->windowVtx[x + 2].v.cn[1] = thisv->windowVtx[x].v.cn[2] = thisv->windowVtx[x + 2].v.cn[2] =
                    thisv->windowVtx[x + 1].v.cn[0] = thisv->windowVtx[x + 3].v.cn[0] = thisv->windowVtx[x + 1].v.cn[1] =
                        thisv->windowVtx[x + 3].v.cn[1] = thisv->windowVtx[x + 1].v.cn[2] =
                            thisv->windowVtx[x + 3].v.cn[2] = thisv->windowVtx[x].v.cn[3] =
                                thisv->windowVtx[x + 2].v.cn[3] = thisv->windowVtx[x + 1].v.cn[3] =
                                    thisv->windowVtx[x + 3].v.cn[3] = 255;
        }
    }
}

static s16 D_80812818[] = { 0x001A, 0x000A, 0x000A, 0x000A };
static s16 D_80812820[] = { 0x0020, 0x000C, 0x000C, 0x000C };
static s16 D_80812828[] = { 0x0010, 0x000C, 0x000C, 0x000C };
static s16 D_80812830[] = { 0x0040, 0x0054, 0x0068, 0x0274, 0x0278, 0x027C };
static s16 D_8081283C[] = { 0x0040, 0x0054, 0x0068, 0x0278 };
static s16 D_80812844[] = { 0x0274, 0x0278 };
static s16 D_80812848[] = { 0x0274, 0x0278 };

void FileChoose_SetWindowContentVtx(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t5;
    s16 phi_a1;
    s16 phi_ra;
    s16 temp_t1;
    SramContext* sramCtx = &thisv->sramCtx;

    thisv->windowContentVtx = static_cast<Vtx*>( Graph_Alloc(thisv->state.gfxCtx, 0x288 * sizeof(Vtx)) );

    for (phi_t2 = 0; phi_t2 < 0x288; phi_t2 += 4) {
        thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = 0x12C;
        thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
            thisv->windowContentVtx[phi_t2].v.ob[0] + 0x10;

        thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] = 0;
        thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
            thisv->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2].v.ob[2] = thisv->windowContentVtx[phi_t2 + 1].v.ob[2] =
            thisv->windowContentVtx[phi_t2 + 2].v.ob[2] = thisv->windowContentVtx[phi_t2 + 3].v.ob[2] = 0;

        thisv->windowContentVtx[phi_t2].v.flag = thisv->windowContentVtx[phi_t2 + 1].v.flag =
            thisv->windowContentVtx[phi_t2 + 2].v.flag = thisv->windowContentVtx[phi_t2 + 3].v.flag = 0;

        thisv->windowContentVtx[phi_t2].v.tc[0] = thisv->windowContentVtx[phi_t2].v.tc[1] =
            thisv->windowContentVtx[phi_t2 + 1].v.tc[1] = thisv->windowContentVtx[phi_t2 + 2].v.tc[0] = 0;

        thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 2].v.tc[1] =
            thisv->windowContentVtx[phi_t2 + 3].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x200;

        thisv->windowContentVtx[phi_t2].v.cn[0] = thisv->windowContentVtx[phi_t2 + 1].v.cn[0] =
            thisv->windowContentVtx[phi_t2 + 2].v.cn[0] = thisv->windowContentVtx[phi_t2 + 3].v.cn[0] =
                thisv->windowContentVtx[phi_t2].v.cn[1] = thisv->windowContentVtx[phi_t2 + 1].v.cn[1] =
                    thisv->windowContentVtx[phi_t2 + 2].v.cn[1] = thisv->windowContentVtx[phi_t2 + 3].v.cn[1] =
                        thisv->windowContentVtx[phi_t2].v.cn[2] = thisv->windowContentVtx[phi_t2 + 1].v.cn[2] =
                            thisv->windowContentVtx[phi_t2 + 2].v.cn[2] = thisv->windowContentVtx[phi_t2 + 3].v.cn[2] =
                                thisv->windowContentVtx[phi_t2].v.cn[3] = thisv->windowContentVtx[phi_t2 + 1].v.cn[3] =
                                    thisv->windowContentVtx[phi_t2 + 2].v.cn[3] =
                                        thisv->windowContentVtx[phi_t2 + 3].v.cn[3] = 0xFF;
    }

    thisv->windowContentVtx[0].v.ob[0] = thisv->windowContentVtx[2].v.ob[0] = thisv->windowPosX;
    thisv->windowContentVtx[1].v.ob[0] = thisv->windowContentVtx[3].v.ob[0] = thisv->windowContentVtx[0].v.ob[0] + 0x80;
    thisv->windowContentVtx[0].v.ob[1] = thisv->windowContentVtx[1].v.ob[1] = 0x48;
    thisv->windowContentVtx[2].v.ob[1] = thisv->windowContentVtx[3].v.ob[1] = thisv->windowContentVtx[0].v.ob[1] - 0x10;
    thisv->windowContentVtx[1].v.tc[0] = thisv->windowContentVtx[3].v.tc[0] = 0x1000;

    for (phi_a1 = 0, phi_t2 = 4; phi_a1 < 3; phi_a1++) {
        phi_t0 = thisv->windowPosX - 6;

        for (phi_t5 = 0; phi_t5 < 5; phi_t5++, phi_t2 += 4) {
            thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
            thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
                thisv->windowContentVtx[phi_t2].v.ob[0] + sFileInfoBoxPartWidths[phi_t5];

            thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] =
                thisv->fileNamesY[phi_a1] + 0x2C;

            thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
                thisv->windowContentVtx[phi_t2].v.ob[1] - 0x38;

            thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[0] =
                sFileInfoBoxPartWidths[phi_t5] << 5;
            thisv->windowContentVtx[phi_t2 + 2].v.tc[1] = thisv->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x700;
            phi_t0 += sFileInfoBoxPartWidths[phi_t5];
        }
    }

    phi_t0 = thisv->windowPosX - 6;
    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_t2 += 20, phi_ra -= 0x10) {
        thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;

        thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
            thisv->windowContentVtx[phi_t2].v.ob[0] + 0x40;

        thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] =
            thisv->buttonYOffsets[phi_t5] + phi_ra;
        thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
            thisv->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

        thisv->windowContentVtx[phi_t2 + 4].v.ob[0] = thisv->windowContentVtx[phi_t2 + 6].v.ob[0] = phi_t0 + 0x40;

        thisv->windowContentVtx[phi_t2 + 5].v.ob[0] = thisv->windowContentVtx[phi_t2 + 7].v.ob[0] =
            thisv->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x6C;

        thisv->windowContentVtx[phi_t2 + 4].v.ob[1] = thisv->windowContentVtx[phi_t2 + 5].v.ob[1] =
            thisv->buttonYOffsets[phi_t5] + phi_ra;

        thisv->windowContentVtx[phi_t2 + 6].v.ob[1] = thisv->windowContentVtx[phi_t2 + 7].v.ob[1] =
            thisv->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2 + 5].v.tc[0] = thisv->windowContentVtx[phi_t2 + 7].v.tc[0] = 0xD80;

        if ((thisv->configMode == CM_COPY_ANIM_2) && (phi_t5 == thisv->copyDestFileIndex)) {
            temp_t1 = thisv->fileNamesY[phi_t5] + 0x2C;
        } else if (((thisv->configMode == CM_COPY_ANIM_3) || (thisv->configMode == CM_COPY_ANIM_4)) &&
                   (phi_t5 == thisv->copyDestFileIndex)) {
            temp_t1 = thisv->buttonYOffsets[phi_t5] + phi_ra;
        } else {
            temp_t1 = phi_ra + thisv->buttonYOffsets[phi_t5] + thisv->fileNamesY[phi_t5];
        }

        thisv->windowContentVtx[phi_t2 + 8].v.ob[0] = thisv->windowContentVtx[phi_t2 + 10].v.ob[0] = phi_t0 + 0xA8;

        thisv->windowContentVtx[phi_t2 + 9].v.ob[0] = thisv->windowContentVtx[phi_t2 + 11].v.ob[0] =
            thisv->windowContentVtx[phi_t2 + 8].v.ob[0] + 0x2C;

        thisv->windowContentVtx[phi_t2 + 8].v.ob[1] = thisv->windowContentVtx[phi_t2 + 9].v.ob[1] = temp_t1;
        thisv->windowContentVtx[phi_t2 + 10].v.ob[1] = thisv->windowContentVtx[phi_t2 + 11].v.ob[1] =
            thisv->windowContentVtx[phi_t2 + 8].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2 + 9].v.tc[0] = thisv->windowContentVtx[phi_t2 + 11].v.tc[0] = 0x580;
        thisv->windowContentVtx[phi_t2 + 12].v.ob[0] = thisv->windowContentVtx[phi_t2 + 14].v.ob[0] = phi_t0 + 0x34;
        thisv->windowContentVtx[phi_t2 + 13].v.ob[0] = thisv->windowContentVtx[phi_t2 + 15].v.ob[0] =
            thisv->windowContentVtx[phi_t2 + 12].v.ob[0] + 0x18;

        thisv->windowContentVtx[phi_t2 + 12].v.ob[1] = thisv->windowContentVtx[phi_t2 + 13].v.ob[1] =
            thisv->buttonYOffsets[phi_t5] + phi_ra;

        thisv->windowContentVtx[phi_t2 + 14].v.ob[1] = thisv->windowContentVtx[phi_t2 + 15].v.ob[1] =
            thisv->windowContentVtx[phi_t2 + 12].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2 + 13].v.tc[0] = thisv->windowContentVtx[phi_t2 + 15].v.tc[0] = 0x300;
        thisv->windowContentVtx[phi_t2 + 16].v.ob[0] = thisv->windowContentVtx[phi_t2 + 18].v.ob[0] = phi_t0 + 0x9C;
        thisv->windowContentVtx[phi_t2 + 17].v.ob[0] = thisv->windowContentVtx[phi_t2 + 19].v.ob[0] =
            thisv->windowContentVtx[phi_t2 + 16].v.ob[0] + 0x18;

        thisv->windowContentVtx[phi_t2 + 16].v.ob[1] = thisv->windowContentVtx[phi_t2 + 17].v.ob[1] =
            thisv->buttonYOffsets[phi_t5] + phi_ra;

        thisv->windowContentVtx[phi_t2 + 18].v.ob[1] = thisv->windowContentVtx[phi_t2 + 19].v.ob[1] =
            thisv->windowContentVtx[phi_t2 + 16].v.ob[1] - 0x10;

        thisv->windowContentVtx[phi_t2 + 17].v.tc[0] = thisv->windowContentVtx[phi_t2 + 19].v.tc[0] = 0x300;
    }

    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_ra -= WREG(38)) {
        if (SLOT_OCCUPIED(sramCtx, phi_t5)) {
            phi_t0 = thisv->windowPosX - WREG(39);

            if ((thisv->configMode == 0xF) && (phi_t5 == thisv->copyDestFileIndex)) {
                temp_t1 = thisv->fileNamesY[phi_t5] + 0x2C;
            } else if (((thisv->configMode == CM_COPY_ANIM_3) || (thisv->configMode == CM_COPY_ANIM_4)) &&
                       (phi_t5 == thisv->copyDestFileIndex)) {
                temp_t1 = thisv->buttonYOffsets[phi_t5] + phi_ra;
            } else {
                temp_t1 = phi_ra + thisv->buttonYOffsets[phi_t5] + thisv->fileNamesY[phi_t5];
            }

            temp_t1 += 2;

            for (phi_a1 = 0; phi_a1 < 8; phi_a1++, phi_t2 += 4, phi_t0 += WREG(40)) {
                thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] =
                    WREG(41) + phi_t0 + 0x40;
                thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    thisv->windowContentVtx[phi_t2].v.ob[0] + WREG(42);
                thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1 - 3;
                thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    thisv->windowContentVtx[phi_t2].v.ob[1] - WREG(43);
            }

            phi_t0 = thisv->windowPosX - 14;
            temp_t1 -= 0x16;

            for (phi_a1 = 0; phi_a1 < 4; phi_a1++, phi_t2 += 4) {
                thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    thisv->windowContentVtx[phi_t2].v.ob[0] + D_80812820[phi_a1];
                thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    thisv->windowContentVtx[phi_t2].v.ob[1] - D_80812828[phi_a1];
                phi_t0 += D_80812818[phi_a1];
            }

            thisv->windowContentVtx[phi_t2 - 15].v.tc[0] = thisv->windowContentVtx[phi_t2 - 13].v.tc[0] = 0x400;

            phi_t0 = thisv->windowPosX + 63;
            temp_t1 += 4;

            for (phi_a1 = 0; phi_a1 < 20; phi_a1++, phi_t2 += 4, phi_t0 += 9) {
                thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    thisv->windowContentVtx[phi_t2].v.ob[0] + 0xA;
                thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    thisv->windowContentVtx[phi_t2].v.ob[1] - 0xA;

                if (phi_a1 == 9) {
                    phi_t0 = thisv->windowPosX + 54;
                    temp_t1 -= 8;
                }
            }

            phi_t0 = thisv->windowPosX + 4;
            temp_t1 -= 0xA;

            for (phi_a1 = 0; phi_a1 < 10; phi_a1++, phi_t2 += 4, phi_t0 += 0x10) {
                thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    thisv->windowContentVtx[phi_t2].v.ob[0] + 0x10;
                thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    thisv->windowContentVtx[phi_t2].v.ob[1] - 0x10;
            }
        } else {
            phi_t2 += 0xA8;
        }
    }

    phi_t0 = thisv->windowPosX - 6;
    phi_ra = -0xC;

    for (phi_t5 = 0; phi_t5 < 2; phi_t5++, phi_t2 += 4, phi_ra -= 0x10) {
        thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
        thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
            thisv->windowContentVtx[phi_t2].v.ob[0] + 0x40;
        thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] =
            thisv->buttonYOffsets[phi_t5 + 3] + phi_ra;
        thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
            thisv->windowContentVtx[phi_t2].v.ob[1] - 0x10;
        thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;
    }

    thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
    thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
        thisv->windowContentVtx[phi_t2].v.ob[0] + 0x40;
    thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] =
        thisv->buttonYOffsets[5] - 0x34;
    thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
        thisv->windowContentVtx[phi_t2].v.ob[1] - 0x10;
    thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

    phi_t2 += 4;

    if (((thisv->menuMode == FS_MENU_MODE_CONFIG) && (thisv->configMode >= CM_MAIN_MENU)) ||
        ((thisv->menuMode == FS_MENU_MODE_SELECT) && (thisv->selectMode == SM_CONFIRM_FILE))) {
        if (thisv->menuMode == FS_MENU_MODE_CONFIG) {
            if ((thisv->configMode == CM_SELECT_COPY_SOURCE) || (thisv->configMode == CM_SELECT_COPY_DEST) ||
                (thisv->configMode == CM_ERASE_SELECT)) {
                phi_t5 = D_8081283C[thisv->buttonIndex];
            } else if ((thisv->configMode == CM_ERASE_CONFIRM) || (thisv->configMode == CM_COPY_CONFIRM)) {
                phi_t5 = D_80812844[thisv->buttonIndex];
            } else {
                phi_t5 = D_80812830[thisv->buttonIndex];
            }
        } else {
            phi_t5 = D_80812848[thisv->confirmButtonIndex];
        }

        thisv->windowContentVtx[phi_t2].v.ob[0] = thisv->windowContentVtx[phi_t2 + 2].v.ob[0] = thisv->windowPosX - 0xA;
        thisv->windowContentVtx[phi_t2 + 1].v.ob[0] = thisv->windowContentVtx[phi_t2 + 3].v.ob[0] =
            thisv->windowContentVtx[phi_t2].v.ob[0] + 0x48;
        thisv->windowContentVtx[phi_t2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 1].v.ob[1] =
            thisv->windowContentVtx[phi_t5].v.ob[1] + 4;
        thisv->windowContentVtx[phi_t2 + 2].v.ob[1] = thisv->windowContentVtx[phi_t2 + 3].v.ob[1] =
            thisv->windowContentVtx[phi_t2].v.ob[1] - 0x18;
        thisv->windowContentVtx[phi_t2 + 1].v.tc[0] = thisv->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x900;
        thisv->windowContentVtx[phi_t2 + 2].v.tc[1] = thisv->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x300;
    }

    thisv->windowContentVtx[phi_t2 + 4].v.ob[0] = thisv->windowContentVtx[phi_t2 + 6].v.ob[0] = thisv->windowPosX + 0x3A;
    thisv->windowContentVtx[phi_t2 + 5].v.ob[0] = thisv->windowContentVtx[phi_t2 + 7].v.ob[0] =
        thisv->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x80;
    thisv->windowContentVtx[phi_t2 + 4].v.ob[1] = thisv->windowContentVtx[phi_t2 + 5].v.ob[1] =
        thisv->windowContentVtx[D_80812830[thisv->warningButtonIndex]].v.ob[1];
    thisv->windowContentVtx[phi_t2 + 6].v.ob[1] = thisv->windowContentVtx[phi_t2 + 7].v.ob[1] =
        thisv->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;
    thisv->windowContentVtx[phi_t2 + 5].v.tc[0] = thisv->windowContentVtx[phi_t2 + 7].v.tc[0] = 0x1000;
}

static u16 D_8081284C[] = { 0x007C, 0x0124, 0x01CC };

static const char* sQuestItemTextures[] = {
    gFileSelKokiriEmeraldTex,   gFileSelGoronRubyTex,       gFileSelZoraSapphireTex,
    gFileSelForestMedallionTex, gFileSelFireMedallionTex,   gFileSelWaterMedallionTex,
    gFileSelSpiritMedallionTex, gFileSelShadowMedallionTex, gFileSelLightMedallionTex,
};

static s16 sQuestItemRed[] = { 255, 255, 255, 0, 255, 0, 255, 200, 200 };
static s16 sQuestItemGreen[] = { 255, 255, 255, 255, 60, 100, 130, 50, 200 };
static s16 sQuestItemBlue[] = { 255, 255, 255, 0, 0, 255, 0, 255, 0 };
static s16 sQuestItemFlags[] = { 0x0012, 0x0013, 0x0014, 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005 };
static s16 sNamePrimColors[2][3] = { { 255, 255, 255 }, { 100, 100, 100 } };
static const char* sHeartTextures[] = { gHeartFullTex, gDefenseHeartFullTex };
static s16 sHeartPrimColors[2][3] = { { 255, 70, 50 }, { 200, 0, 0 } };
static s16 sHeartEnvColors[2][3] = { { 50, 40, 60 }, { 255, 255, 255 } };

void FileChoose_DrawFileInfo(GameState* thisx, s16 fileIndex, s16 isActive) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Font* sp54 = &thisv->font;
    s32 heartType;
    s16 i;
    s16 vtxOffset;
    s16 j;
    s16 deathCountSplit[3];

    if (1) {}

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 1709);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    // draw file name
    if (thisv->nameAlpha[fileIndex] != 0) {
        gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[D_8081284C[fileIndex]], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sNamePrimColors[isActive][0], sNamePrimColors[isActive][1],
                        sNamePrimColors[isActive][2], thisv->nameAlpha[fileIndex]);

        for (i = 0, vtxOffset = 0; vtxOffset < 0x20; i++, vtxOffset += 4) {
            FileChoose_DrawCharacter(thisv->state.gfxCtx,
                                     sp54->fontBuf + thisv->fileNames[fileIndex][i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    if ((fileIndex == thisv->selectedFileIndex) || (fileIndex == thisv->copyDestFileIndex)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, thisv->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[D_8081284C[fileIndex]] + 0x24, 12, 0);

        FileChoose_SplitNumber(thisv->deaths[fileIndex], &deathCountSplit[0], &deathCountSplit[1], &deathCountSplit[2]);

        // draw death count
        for (i = 0, vtxOffset = 0; i < 3; i++, vtxOffset += 4) {
            FileChoose_DrawCharacter(thisv->state.gfxCtx, sp54->fontBuf + deathCountSplit[i] * FONT_CHAR_TEX_SIZE,
                                     vtxOffset);
        }

        gDPPipeSync(POLY_OPA_DISP++);

        heartType = (thisv->defense[fileIndex] == 0) ? 0 : 1;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sHeartPrimColors[heartType][0], sHeartPrimColors[heartType][1],
                        sHeartPrimColors[heartType][2], thisv->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, sHeartEnvColors[heartType][0], sHeartEnvColors[heartType][1],
                       sHeartEnvColors[heartType][2], 255);

        i = thisv->healthCapacities[fileIndex] / 0x10;

        // draw hearts
        for (vtxOffset = 0, j = 0; j < i; j++, vtxOffset += 4) {
            gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x30, 4, 0);

            POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sHeartTextures[heartType], 0x10, 0x10, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);

        // draw quest items
        for (vtxOffset = 0, j = 0; j < 9; j++, vtxOffset += 4) {
            if (thisv->questItems[fileIndex] & gBitFlags[sQuestItemFlags[j]]) {
                gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x80, 4, 0);
                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sQuestItemRed[j], sQuestItemGreen[j], sQuestItemBlue[j],
                                thisv->fileInfoAlpha[fileIndex]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

                if (j < 3) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sQuestItemTextures[j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

                } else {
                    POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sQuestItemTextures[j], 0x10, 0x10, 0);
                }
            }
        }
    }

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 1797);
}

static const char* sFileInfoBoxTextures[] = {
    gFileSelFileInfoBox1Tex, gFileSelFileInfoBox2Tex, gFileSelFileInfoBox3Tex,
    gFileSelFileInfoBox4Tex, gFileSelFileInfoBox5Tex,
};

static const char* sTitleLabels[3][9] = {
    { gFileSelPleaseSelectAFileENGTex, gFileSelOpenThisFileENGTex, gFileSelCopyWhichFileENGTex,
      gFileSelCopyToWhichFileENGTex, gFileSelAreYouSureENGTex, gFileSelFileCopiedENGTex, gFileSelEraseWhichFileENGTex,
      gFileSelAreYouSure2ENGTex, gFileSelFileErasedENGTex },
    { gFileSelPleaseSelectAFileGERTex, gFileSelOpenThisFileGERTex, gFileSelWhichFile1GERTex,
      gFileSelCopyToWhichFileGERTex, gFileSelAreYouSureGERTex, gFileSelFileCopiedGERTex, gFileSelWhichFile2GERTex,
      gFileSelAreYouSure2GERTex, gFileSelFileErasedGERTex },
    { gFileSelPleaseSelectAFileFRATex, gFileSelOpenThisFileFRATex, gFileSelCopyWhichFileFRATex,
      gFileSelCopyToWhichFileFRATex, gFileSelAreYouSureFRATex, gFileSelFileCopiedFRATex, gFileSelEraseWhichFileFRATex,
      gFileSelAreYouSure2FRATex, gFileSelFileErasedFRATex }
};

static const char* sWarningLabels[3][5] = {
    { gFileSelNoFileToCopyENGTex, gFileSelNoFileToEraseENGTex, gFileSelNoEmptyFileENGTex, gFileSelFileEmptyENGTex,
      gFileSelFileInUseENGTex },
    { gFileSelNoFileToCopyGERTex, gFileSelNoFileToEraseGERTex, gFileSelNoEmptyFileGERTex, gFileSelFileEmptyGERTex,
      gFileSelFileInUseGERTex },
    { gFileSelNoFileToCopyFRATex, gFileSelNoFileToEraseFRATex, gFileSelNoEmptyFileFRATex, gFileSelFileEmptyFRATex,
      gFileSelFileInUseFRATex },
};

static const char* sFileButtonTextures[3][3] = {
    { gFileSelFile1ButtonENGTex, gFileSelFile2ButtonENGTex, gFileSelFile3ButtonENGTex },
    { gFileSelFile1ButtonGERTex, gFileSelFile2ButtonGERTex, gFileSelFile3ButtonGERTex },
    { gFileSelFile1ButtonFRATex, gFileSelFile2ButtonFRATex, gFileSelFile3ButtonFRATex },
};

static const char* sActionButtonTextures[3][4] = {
    { gFileSelCopyButtonENGTex, gFileSelEraseButtonENGTex, gFileSelYesButtonENGTex, gFileSelQuitButtonENGTex },
    { gFileSelCopyButtonGERTex, gFileSelEraseButtonGERTex, gFileSelYesButtonGERTex, gFileSelQuitButtonGERTex },
    { gFileSelCopyButtonFRATex, gFileSelEraseButtonFRATex, gFileSelYesButtonFRATex, gFileSelQuitButtonFRATex },
};

static const char* sOptionsButtonTextures[] = {
    gFileSelOptionsButtonENGTex,
    gFileSelOptionsButtonGERTex,
    gFileSelOptionsButtonENGTex,
};

/**
 * Draw most window contents including buttons, labels, and icons.
 * Does not include anything from the keyboard and settings windows.
 */
void FileChoose_DrawWindowContents(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 fileIndex;
    s16 temp;
    s16 i;
    s16 quadVtxIndex;
    s16 isActive;
    s16 pad;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 1940);

    // draw title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, thisv->windowContentVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[gSaveContext.language][thisv->titleLabel], G_IM_FMT_IA,
                        G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    // draw next title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->titleAlpha[1]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[gSaveContext.language][thisv->nextTitleLabel], G_IM_FMT_IA,
                        G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    temp = 4;

    gDPPipeSync(POLY_OPA_DISP++);

    // draw file info box (large box when a file is selected)
    for (fileIndex = 0; fileIndex < 3; fileIndex++, temp += 20) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[temp], 20, 0);

        for (quadVtxIndex = 0, i = 0; i < 5; i++, quadVtxIndex += 4) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, sFileInfoBoxTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b,
                                sFileInfoBoxPartWidths[i], 56, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
        }
    }

    for (i = 0; i < 3; i++, temp += 20) {
        // draw file button
        gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[temp], 20, 0);

        isActive = ((thisv->n64ddFlag == thisv->n64ddFlags[i]) || (thisv->nameBoxAlpha[i] == 0)) ? 0 : 1;

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                        sWindowContentColors[isActive][2], thisv->fileButtonAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sFileButtonTextures[gSaveContext.language][i], G_IM_FMT_IA, G_IM_SIZ_16b,
                            64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        // draw file name box
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                        sWindowContentColors[isActive][2], thisv->nameBoxAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

        // draw disk label for 64DD
        if (thisv->n64ddFlags[i]) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                            sWindowContentColors[isActive][2], thisv->nameAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelDISKButtonTex, G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
        }

        // draw connectors
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0], sWindowContentColors[isActive][1],
                        sWindowContentColors[isActive][2], thisv->connectorAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);

        if (thisv->n64ddFlags[i]) {
            gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
        }
    }

    // draw file info
    for (fileIndex = 0; fileIndex < 3; fileIndex++) {
        isActive = ((thisv->n64ddFlag == thisv->n64ddFlags[fileIndex]) || (thisv->nameBoxAlpha[fileIndex] == 0)) ? 0 : 1;
        FileChoose_DrawFileInfo(&thisv->state, fileIndex, isActive);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, &thisv->windowContentVtx[0x274], 20, 0);

    // draw primary action buttons (copy/erase)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->actionButtonAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][i], G_IM_FMT_IA, G_IM_SIZ_16b,
                            64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // draw confirm buttons (yes/quit)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        temp = thisv->confirmButtonTexIndices[i];

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->confirmButtonAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][temp], G_IM_FMT_IA,
                            G_IM_SIZ_16b, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    // draw options button
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                    thisv->optionButtonAlpha);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsButtonTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_16b, 64,
                        16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

    // draw highlight over currently selected button
    if (((thisv->menuMode == FS_MENU_MODE_CONFIG) &&
         ((thisv->configMode == CM_MAIN_MENU) || (thisv->configMode == CM_SELECT_COPY_SOURCE) ||
          (thisv->configMode == CM_SELECT_COPY_DEST) || (thisv->configMode == CM_COPY_CONFIRM) ||
          (thisv->configMode == CM_ERASE_SELECT) || (thisv->configMode == CM_ERASE_CONFIRM))) ||
        ((thisv->menuMode == FS_MENU_MODE_SELECT) && (thisv->selectMode == SM_CONFIRM_FILE))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->highlightColor[0], thisv->highlightColor[1],
                        thisv->highlightColor[2], thisv->highlightColor[3]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBigButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 72, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
    }

    // draw warning labels
    if (thisv->warningLabel > FS_WARNING_NONE) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, thisv->emptyFileTextAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sWarningLabels[gSaveContext.language][thisv->warningLabel], G_IM_FMT_IA,
                            G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2198);
}

void FileChoose_ConfigModeDraw(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2218);
    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(thisv, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&thisv->skyboxCtx, thisv->state.gfxCtx, 1, thisv->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &thisv->envCtx, &thisv->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    func_800949A8(thisv->state.gfxCtx);
    FileChoose_SetView(thisv, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&thisv->state);
    FileChoose_SetWindowContentVtx(&thisv->state);

    if ((thisv->configMode != CM_NAME_ENTRY) && (thisv->configMode != CM_START_NAME_ENTRY)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);

        if (thisv->windowRot != 0) {
            Matrix_RotateX(thisv->windowRot / 100.0f, MTXMODE_APPLY);
        }

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_choose.c", 2282),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawWindowContents(&thisv->state);
    }

    // draw name entry menu
    if ((thisv->configMode >= CM_ROTATE_TO_NAME_ENTRY) && (thisv->configMode <= CM_NAME_ENTRY_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((thisv->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_choose.c", 2316),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawNameEntry(&thisv->state);
    }

    // draw options menu
    if ((thisv->configMode >= CM_MAIN_TO_OPTIONS) && (thisv->configMode <= CM_OPTIONS_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                        thisv->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((thisv->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_choose.c", 2337),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawOptions(&thisv->state);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(thisv, 0.0f, 0.0f, 64.0f);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2352);
}

/**
 * Fade out the main menu elements to transition to select mode.
 * Update function for `SM_FADE_MAIN_TO_SELECT`
 */
void FileChoose_FadeMainToSelect(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;

    for (i = 0; i < 3; i++) {
        if (i != thisv->buttonIndex) {
            thisv->fileButtonAlpha[i] -= 25;
            thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                thisv->optionButtonAlpha = thisv->fileButtonAlpha[i];

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->nameAlpha[i] = thisv->nameBoxAlpha[i] = thisv->fileButtonAlpha[i];
                thisv->connectorAlpha[i] -= 31;
            }
        }
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 8;
        thisv->selectMode++;
        thisv->confirmButtonIndex = FS_BTN_CONFIRM_YES;
    }
}

/**
 * Moves the selected file to the top of the window.
 * Update function for `SM_MOVE_FILE_TO_TOP`
 */
void FileChoose_MoveSelectedFileToTop(GameState* thisx) {
    static s16 fileYOffsets[] = { 0, 16, 32 }; // amount to move by to reach the top of the screen
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 yStep;

    yStep = ABS(thisv->buttonYOffsets[thisv->buttonIndex] - fileYOffsets[thisv->buttonIndex]) / thisv->actionTimer;
    thisv->buttonYOffsets[thisv->buttonIndex] += yStep;
    thisv->actionTimer--;

    if ((thisv->actionTimer == 0) || (thisv->buttonYOffsets[thisv->buttonIndex] == fileYOffsets[thisv->buttonIndex])) {
        thisv->buttonYOffsets[FS_BTN_SELECT_YES] = thisv->buttonYOffsets[FS_BTN_SELECT_QUIT] = -24;
        thisv->actionTimer = 8;
        thisv->selectMode++;
    }
}

/**
 * Fade in the file info for the selected file.
 * Update function for `SM_FADE_IN_FILE_INFO`
 */
void FileChoose_FadeInFileInfo(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->fileInfoAlpha[thisv->buttonIndex] += 25;
    thisv->nameBoxAlpha[thisv->buttonIndex] -= 50;

    if (thisv->nameBoxAlpha[thisv->buttonIndex] <= 0) {
        thisv->nameBoxAlpha[thisv->buttonIndex] = 0;
    }

    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->fileInfoAlpha[thisv->buttonIndex] = 200;
        thisv->actionTimer = 8;
        thisv->selectMode++;
    }

    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] =
        thisv->fileInfoAlpha[thisv->buttonIndex];
}

/**
 * Update the cursor and handle the option that the player picks for confirming the selected file.
 * Update function for `SM_CONFIRM_FILE`
 */
void FileChoose_ConfirmFile(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Input* input = &thisv->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || (CHECK_BTN_ALL(input->press.button, BTN_A))) {
        if (thisv->confirmButtonIndex == FS_BTN_CONFIRM_YES) {
            func_800AA000(300.0f, 180, 20, 100);
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->selectMode = SM_FADE_OUT;
            func_800F6964(0xF);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            thisv->selectMode++;
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->selectMode++;
    } else if (ABS(thisv->stickRelY) >= 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->confirmButtonIndex ^= 1;
    }
}

/**
 * Fade out the file info for the selected file before returning to the main menu.
 * Update function for `SM_FADE_OUT_FILE_INFO`
 */
void FileChoose_FadeOutFileInfo(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->fileInfoAlpha[thisv->buttonIndex] -= 25;
    thisv->nameBoxAlpha[thisv->buttonIndex] += 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->buttonYOffsets[FS_BTN_SELECT_YES] = thisv->buttonYOffsets[FS_BTN_SELECT_QUIT] = 0;
        thisv->nameBoxAlpha[thisv->buttonIndex] = 200;
        thisv->fileInfoAlpha[thisv->buttonIndex] = 0;
        thisv->nextTitleLabel = FS_TITLE_SELECT_FILE;
        thisv->actionTimer = 8;
        thisv->selectMode++;
    }

    thisv->confirmButtonAlpha[0] = thisv->confirmButtonAlpha[1] = thisv->fileInfoAlpha[thisv->buttonIndex];
}

/**
 * Move the selected file back to the slot position then go to config mode for the main menu.
 * Update function for `SM_MOVE_FILE_TO_SLOT`
 */
void FileChoose_MoveSelectedFileToSlot(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 yStep;
    s16 i;

    yStep = ABS(thisv->buttonYOffsets[thisv->buttonIndex]) / thisv->actionTimer;
    thisv->buttonYOffsets[thisv->buttonIndex] -= yStep;

    if (thisv->buttonYOffsets[thisv->buttonIndex] <= 0) {
        thisv->buttonYOffsets[thisv->buttonIndex] = 0;
    }

    for (i = 0; i < 3; i++) {
        if (i != thisv->buttonIndex) {
            thisv->fileButtonAlpha[i] += 25;

            if (thisv->fileButtonAlpha[i] >= 200) {
                thisv->fileButtonAlpha[i] = 200;
            }

            thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                thisv->optionButtonAlpha = thisv->fileButtonAlpha[i];

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
                thisv->connectorAlpha[i] += 31;
            }
        }
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->actionTimer = 8;
        thisv->menuMode = FS_MENU_MODE_CONFIG;
        thisv->configMode = CM_MAIN_MENU;
        thisv->nextConfigMode = CM_MAIN_MENU;
        thisv->selectMode = SM_FADE_MAIN_TO_SELECT;
    }
}

/**
 * Fill the screen with black to fade out.
 * Update function for `SM_FADE_OUT`
 */
void FileChoose_FadeOut(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    sScreenFillAlpha += VREG(10);

    if (sScreenFillAlpha >= 255) {
        sScreenFillAlpha = 255;
        thisv->selectMode++;
    }
}

/**
 * Load the save for the appropriate file and start the game.
 * Note: On Debug ROM, File 1 will go to Map Select.
 * Update function for `SM_LOAD_GAME`
 */
void FileChoose_LoadGame(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    u16 swordEquipMask;
    s32 pad;

    if (thisv->buttonIndex == FS_BTN_SELECT_FILE_1 && CVar_GetS32("gDebugEnabled", 0)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        gSaveContext.fileNum = thisv->buttonIndex;
        Sram_OpenSave(&thisv->sramCtx);
        gSaveContext.gameMode = 0;
        SET_NEXT_GAMESTATE(&thisv->state, Select_Init, SelectContext);
        thisv->state.running = false;
    } else {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        gSaveContext.fileNum = thisv->buttonIndex;
        Sram_OpenSave(&thisv->sramCtx);
        gSaveContext.gameMode = 0;
        SET_NEXT_GAMESTATE(&thisv->state, Gameplay_Init, GlobalContext);
        thisv->state.running = false;
    }

    gSaveContext.respawn[0].entranceIndex = -1;
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gSaveContext.dogParams = 0;
    gSaveContext.timer1State = 0;
    gSaveContext.timer2State = 0;
    gSaveContext.eventInf[0] = 0;
    gSaveContext.eventInf[1] = 0;
    gSaveContext.eventInf[2] = 0;
    gSaveContext.eventInf[3] = 0;
    gSaveContext.unk_13EE = 0x32;
    gSaveContext.nayrusLoveTimer = 0;
    gSaveContext.healthAccumulator = 0;
    gSaveContext.unk_13F0 = 0;
    gSaveContext.unk_13F2 = 0;
    gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    gSaveContext.skyboxTime = 0;
    gSaveContext.nextTransition = 0xFF;
    gSaveContext.nextCutsceneIndex = 0xFFEF;
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = 0xFFFF;
    gSaveContext.unk_13C3 = 0;

    gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
        gSaveContext.buttonStatus[3] = gSaveContext.buttonStatus[4] = BTN_ENABLED;

    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC =
        gSaveContext.unk_13F4 = 0;

    gSaveContext.unk_13F6 = gSaveContext.magic;
    gSaveContext.magic = 0;
    gSaveContext.magicLevel = gSaveContext.magic;

    if (1) {}

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("Z_MAGIC_NOW_NOW=%d  MAGIC_NOW=%d\n", ((void)0, gSaveContext.unk_13F6), gSaveContext.magic);
    osSyncPrintf(VT_RST);

    gSaveContext.naviTimer = 0;

    // SWORDLESS LINK IS BACK BABY
    if (CVar_GetS32("gSwordlessLink", 0) != 0) 
    {
        if ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE)) {
            
            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            swordEquipMask = _byteswap_ushort(gEquipMasks[EQUIP_SWORD]) & gSaveContext.equips.equipment;
            gSaveContext.equips.equipment &= gEquipNegMasks[EQUIP_SWORD];
            gSaveContext.inventory.equipment ^= (gBitFlags[swordEquipMask - 1] << _byteswap_ushort(gEquipShifts[EQUIP_SWORD]));
        }
    }
}

static void (*gSelectModeUpdateFuncs[])(GameState*) = {
    FileChoose_FadeMainToSelect, FileChoose_MoveSelectedFileToTop,  FileChoose_FadeInFileInfo, FileChoose_ConfirmFile,
    FileChoose_FadeOutFileInfo,  FileChoose_MoveSelectedFileToSlot, FileChoose_FadeOut,        FileChoose_LoadGame,
};

void FileChoose_SelectModeUpdate(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    gSelectModeUpdateFuncs[thisv->selectMode](&thisv->state);
}

void FileChoose_SelectModeDraw(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2753);

    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(thisv, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&thisv->skyboxCtx, thisv->state.gfxCtx, 1, thisv->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &thisv->envCtx, &thisv->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    func_800949A8(thisv->state.gfxCtx);
    FileChoose_SetView(thisv, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&thisv->state);
    FileChoose_SetWindowContentVtx(&thisv->state);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, thisv->windowColor[0], thisv->windowColor[1], thisv->windowColor[2],
                    thisv->windowAlpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
    Matrix_RotateX(thisv->windowRot / 100.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(thisv->state.gfxCtx, "../z_file_choose.c", 2810),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[0], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

    gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[32], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

    gSPVertex(POLY_OPA_DISP++, &thisv->windowVtx[64], 16, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

    FileChoose_DrawWindowContents(&thisv->state);
    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(thisv, 0.0f, 0.0f, 64.0f);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2834);
}

static void (*gFileSelectDrawFuncs[])(GameState*) = {
    FileChoose_InitModeDraw,
    FileChoose_ConfigModeDraw,
    FileChoose_SelectModeDraw,
};

static void (*gFileSelectUpdateFuncs[])(GameState*) = {
    FileChoose_InitModeUpdate,
    FileChoose_ConfigModeUpdate,
    FileChoose_SelectModeUpdate,
};

void FileChoose_Main(GameState* thisx) {
    static const char* controlsTextures[] = {
        gFileSelControlsENGTex,
        gFileSelControlsGERTex,
        gFileSelControlsFRATex,
    };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Input* input = &thisv->state.input[0];

    OPEN_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 2898);

    thisv->n64ddFlag = 0;

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_OPA_DISP++, 0x01, thisv->staticSegment);
    gSPSegment(POLY_OPA_DISP++, 0x02, thisv->parameterSegment);

    func_80095248(thisv->state.gfxCtx, 0, 0, 0);

    thisv->stickRelX = input->rel.stick_x;
    thisv->stickRelY = input->rel.stick_y;

    if (thisv->stickRelX < -30) {
        if (thisv->stickXDir == -1) {
            thisv->inputTimerX--;
            if (thisv->inputTimerX < 0) {
                thisv->inputTimerX = 2;
            } else {
                thisv->stickRelX = 0;
            }
        } else {
            thisv->inputTimerX = 10;
            thisv->stickXDir = -1;
        }
    } else if (thisv->stickRelX > 30) {
        if (thisv->stickXDir == 1) {
            thisv->inputTimerX--;
            if (thisv->inputTimerX < 0) {
                thisv->inputTimerX = 2;
            } else {
                thisv->stickRelX = 0;
            }
        } else {
            thisv->inputTimerX = 10;
            thisv->stickXDir = 1;
        }
    } else {
        thisv->stickXDir = 0;
    }

    if (thisv->stickRelY < -30) {
        if (thisv->stickYDir == -1) {
            thisv->inputTimerY -= 1;
            if (thisv->inputTimerY < 0) {
                thisv->inputTimerY = 2;
            } else {
                thisv->stickRelY = 0;
            }
        } else {
            thisv->inputTimerY = 10;
            thisv->stickYDir = -1;
        }
    } else if (thisv->stickRelY > 30) {
        if (thisv->stickYDir == 1) {
            thisv->inputTimerY -= 1;
            if (thisv->inputTimerY < 0) {
                thisv->inputTimerY = 2;
            } else {
                thisv->stickRelY = 0;
            }
        } else {
            thisv->inputTimerY = 10;
            thisv->stickYDir = 1;
        }
    } else {
        thisv->stickYDir = 0;
    }

    thisv->emptyFileTextAlpha = 0;

    FileChoose_PulsateCursor(&thisv->state);
    gFileSelectUpdateFuncs[thisv->menuMode](&thisv->state);
    gFileSelectDrawFuncs[thisv->menuMode](&thisv->state);

    // do not draw controls text in the options menu
    if ((thisv->configMode <= CM_NAME_ENTRY_TO_MAIN) || (thisv->configMode >= CM_UNUSED_DELAY)) {
        func_800944C4(thisv->state.gfxCtx);

        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 255, 255, thisv->controlsAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, controlsTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_8b, 144, 16,
                            0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(POLY_OPA_DISP++, 0x0168, 0x0330, 0x03A8, 0x0370, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gSPDisplayList(POLY_OPA_DISP++, sScreenFillSetupDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, sScreenFillAlpha);
    gDPFillRectangle(POLY_OPA_DISP++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);

    CLOSE_DISPS(thisv->state.gfxCtx, "../z_file_choose.c", 3035);
}

void FileChoose_InitContext(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    EnvironmentContext* envCtx = &thisv->envCtx;
    SramContext* sramCtx = &thisv->sramCtx;

    Sram_Alloc(&thisv->state, sramCtx);

    ZREG(7) = 32;
    ZREG(8) = 22;
    ZREG(9) = 20;
    ZREG(10) = -10;
    ZREG(11) = 0;
    ZREG(12) = 1000;
    ZREG(13) = -700;
    ZREG(14) = 164;
    ZREG(15) = 104;
    ZREG(16) = 160;
    ZREG(17) = 100;
    ZREG(18) = 162;
    ZREG(19) = 152;
    ZREG(20) = 214;

    XREG(13) = 580;
    XREG(14) = 400;
    XREG(35) = 20;
    XREG(36) = 20;
    XREG(37) = 20;
    XREG(43) = 8;
    XREG(44) = -78;
    XREG(45) = 0;
    XREG(46) = 0;
    XREG(47) = 0;
    XREG(48) = 0;
    XREG(49) = 3;
    XREG(50) = 8;
    XREG(51) = 8;
    XREG(52) = 10;
    XREG(73) = 0;

    VREG(0) = 14;
    VREG(1) = 5;
    VREG(2) = 4;
    VREG(4) = 1;
    VREG(5) = 6;
    VREG(6) = 2;
    VREG(7) = 6;
    VREG(8) = 80;

    sScreenFillAlpha = 255;

    VREG(10) = 10;
    VREG(11) = 30;
    VREG(12) = -100;
    VREG(13) = -85;
    VREG(14) = 4;
    VREG(16) = 25;
    VREG(17) = 1;
    VREG(18) = 1;
    VREG(20) = 92;
    VREG(21) = 171;
    VREG(22) = 11;
    VREG(23) = 10;
    VREG(24) = 26;
    VREG(25) = 2;
    VREG(26) = 1;
    VREG(27) = 0;
    VREG(28) = 0;
    VREG(29) = 160;
    VREG(30) = 64;
    VREG(31) = 154;
    VREG(32) = 152;
    VREG(33) = 106;

    WREG(38) = 16;
    WREG(39) = 9;
    WREG(40) = 10;
    WREG(41) = 14;
    WREG(42) = 11;
    WREG(43) = 12;

    thisv->menuMode = FS_MENU_MODE_INIT;

    thisv->buttonIndex = thisv->selectMode = thisv->selectedFileIndex = thisv->copyDestFileIndex =
        thisv->confirmButtonIndex = 0;

    thisv->confirmButtonTexIndices[0] = 2;
    thisv->confirmButtonTexIndices[1] = 3;
    thisv->titleLabel = FS_TITLE_SELECT_FILE;
    thisv->nextTitleLabel = FS_TITLE_OPEN_FILE;
    thisv->highlightPulseDir = 1;
    thisv->unk_1CAAC = 0xC;
    thisv->highlightColor[0] = 155;
    thisv->highlightColor[1] = 255;
    thisv->highlightColor[2] = 255;
    thisv->highlightColor[3] = 70;
    thisv->configMode = CM_FADE_IN_START;
    thisv->windowRot = 0.0f;
    thisv->stickXDir = thisv->inputTimerX = 0;
    thisv->stickYDir = thisv->inputTimerY = 0;
    thisv->kbdX = thisv->kbdY = thisv->charIndex = 0;
    thisv->kbdButton = FS_KBD_BTN_NONE;

    thisv->windowColor[0] = 100;
    thisv->windowColor[1] = 150;
    thisv->windowColor[2] = 255;

    thisv->windowAlpha = thisv->titleAlpha[0] = thisv->titleAlpha[1] = thisv->fileButtonAlpha[0] =
        thisv->fileButtonAlpha[1] = thisv->fileButtonAlpha[2] = thisv->nameBoxAlpha[0] = thisv->nameBoxAlpha[1] =
            thisv->nameBoxAlpha[2] = thisv->nameAlpha[0] = thisv->nameAlpha[1] = thisv->nameAlpha[2] =
                thisv->connectorAlpha[0] = thisv->connectorAlpha[1] = thisv->connectorAlpha[2] = thisv->fileInfoAlpha[0] =
                    thisv->fileInfoAlpha[1] = thisv->fileInfoAlpha[2] = thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] =
                        thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] = thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] =
                            thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = thisv->optionButtonAlpha =
                                thisv->nameEntryBoxAlpha = thisv->controlsAlpha = thisv->emptyFileTextAlpha = 0;

    thisv->windowPosX = 6;
    thisv->actionTimer = 8;
    thisv->warningLabel = FS_WARNING_NONE;

    thisv->warningButtonIndex = thisv->buttonYOffsets[0] = thisv->buttonYOffsets[1] = thisv->buttonYOffsets[2] =
        thisv->buttonYOffsets[3] = thisv->buttonYOffsets[4] = thisv->buttonYOffsets[5] = thisv->fileNamesY[0] =
            thisv->fileNamesY[1] = thisv->fileNamesY[2] = 0;

    thisv->unk_1CAD6[0] = 0;
    thisv->unk_1CAD6[1] = 3;
    thisv->unk_1CAD6[2] = 6;
    thisv->unk_1CAD6[3] = 8;
    thisv->unk_1CAD6[4] = 10;

    ShrinkWindow_SetVal(0);

    gSaveContext.skyboxTime = 0;
    gSaveContext.dayTime = 0;

    Skybox_Init(&thisv->state, &thisv->skyboxCtx, SKYBOX_NORMAL_SKY);

    gTimeIncrement = 10;

    envCtx->unk_19 = 0;
    envCtx->unk_1A = 0;
    envCtx->unk_21 = 0;
    envCtx->unk_22 = 0;
    envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;
    envCtx->unk_1F = 0;
    envCtx->unk_20 = 0;
    envCtx->unk_BD = 0;
    envCtx->unk_17 = 2;
    envCtx->skyboxDisabled = 0;
    envCtx->skyboxBlend = 0;
    envCtx->unk_84 = 0.0f;
    envCtx->unk_88 = 0.0f;

    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &thisv->envCtx, &thisv->skyboxCtx);

    gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
        gSaveContext.buttonStatus[3] = gSaveContext.buttonStatus[4] = BTN_ENABLED;

    thisv->n64ddFlags[0] = thisv->n64ddFlags[1] = thisv->n64ddFlags[2] = thisv->defense[0] = thisv->defense[1] =
        thisv->defense[2] = 0;

    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);

    gSaveContext.language = sramCtx->readBuff[SRAM_HEADER_LANGUAGE];

    if (gSaveContext.language >= LANGUAGE_MAX) {
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language = LANGUAGE_ENG;
    }
}

void FileChoose_Destroy(GameState* thisx) {
}

void FileChoose_Init(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    size_t size = (u32)_title_staticSegmentRomEnd - (u32)_title_staticSegmentRomStart;
    s32 pad;

    SREG(30) = 1;
    osSyncPrintf("SIZE=%x\n", size);

    thisv->staticSegment = static_cast<u8*>( GameState_Alloc(&thisv->state, size, "../z_file_choose.c", 3392) );
    ASSERT(thisv->staticSegment != NULL, "thisv->staticSegment != NULL", "../z_file_choose.c", 3393);
    DmaMgr_SendRequest1(thisv->staticSegment, (u32)_title_staticSegmentRomStart, size, "../z_file_choose.c", 3394);

    size = (u32)_parameter_staticSegmentRomEnd - (u32)_parameter_staticSegmentRomStart;
    thisv->parameterSegment = static_cast<u8*>( GameState_Alloc(&thisv->state, size, "../z_file_choose.c", 3398) );
    ASSERT(thisv->parameterSegment != NULL, "thisv->parameterSegment != NULL", "../z_file_choose.c", 3399);
    DmaMgr_SendRequest1(thisv->parameterSegment, (u32)_parameter_staticSegmentRomStart, size, "../z_file_choose.c",
                        3400);

    Matrix_Init(&thisv->state);
    View_Init(&thisv->view, thisv->state.gfxCtx);
    thisv->state.main = FileChoose_Main;
    thisv->state.destroy = FileChoose_Destroy;
    FileChoose_InitContext(&thisv->state);
    Font_LoadOrderedFont(&thisv->font);
    Audio_QueueSeqCmd(0xF << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA);
    func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
}
