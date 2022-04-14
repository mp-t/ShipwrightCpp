#include "file_choose.h"

// when choosing a file to copy or erase, the 6 main menu buttons are placed at these offsets
static s16 sChooseFileYOffsets[] = { -48, -48, -48, -24, -24, 0 };

static s16 D_8081248C[3][3] = {
    { 0, -48, -48 },
    { -64, 16, -48 },
    { -64, -64, 32 },
};

static s16 sEraseDelayTimer = 15;

/**
 * Move buttons into place for the select source screen and fade in the proper labels.
 * Update function for `CM_SETUP_COPY_SOURCE`
 */
void FileChoose_SetupCopySource(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    for (i = 0; i < 5; i++) {
        yStep = (ABS(thisv->buttonYOffsets[i] - sChooseFileYOffsets[i])) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 25;
    thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 25;
    thisv->optionButtonAlpha -= 25;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 25;
    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 8;

        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            thisv->optionButtonAlpha = 0;

        thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 200;
        thisv->titleLabel = thisv->nextTitleLabel;

        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->buttonIndex = FS_BTN_COPY_QUIT;
        thisv->configMode++;
    }
}

/**
 * Allow the player to select a file to copy or exit back to the main menu.
 * Update function for `CM_SELECT_COPY_SOURCE`
 */
void FileChoose_SelectCopySource(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];

    if (((thisv->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->actionTimer = 8;
        thisv->buttonIndex = FS_BTN_MAIN_COPY;
        thisv->nextTitleLabel = FS_TITLE_SELECT_FILE;
        thisv->configMode = CM_COPY_RETURN_MAIN;
        thisv->warningLabel = FS_WARNING_NONE;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
            thisv->actionTimer = 8;
            thisv->selectedFileIndex = thisv->buttonIndex;
            thisv->configMode = CM_SETUP_COPY_DEST_1;
            thisv->nextTitleLabel = FS_TITLE_COPY_TO;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {
        if (ABS(thisv->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (thisv->stickRelY >= 30) {
                thisv->buttonIndex--;

                if (thisv->buttonIndex < FS_BTN_COPY_FILE_1) {
                    thisv->buttonIndex = FS_BTN_COPY_QUIT;
                }
            } else {
                thisv->buttonIndex++;

                if (thisv->buttonIndex > FS_BTN_COPY_QUIT) {
                    thisv->buttonIndex = FS_BTN_COPY_FILE_1;
                }
            }
        }

        if (thisv->buttonIndex != FS_BTN_COPY_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
                thisv->warningLabel = FS_WARNING_FILE_EMPTY;
                thisv->warningButtonIndex = thisv->buttonIndex;
                thisv->emptyFileTextAlpha = 255;
            } else {
                thisv->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Move the menu buttons into place for the copy destination selection and switch titles.
 * Update function for `CM_SETUP_COPY_DEST_1`
 */
void FileChoose_SetupCopyDest1(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    for (i = 0; i < 3; i++) {
        yStep = ABS(thisv->buttonYOffsets[i] - D_8081248C[thisv->buttonIndex][i]) / thisv->actionTimer;

        if (D_8081248C[thisv->buttonIndex][i] >= thisv->buttonYOffsets[i]) {
            thisv->buttonYOffsets[i] += yStep;
        } else {
            thisv->buttonYOffsets[i] -= yStep;
        }
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->nameBoxAlpha[thisv->buttonIndex] -= 25;

    thisv->actionTimer--;
    if (thisv->actionTimer == 0) {
        thisv->buttonYOffsets[thisv->buttonIndex] = D_8081248C[thisv->buttonIndex][thisv->buttonIndex];
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->actionTimer = 8;
        thisv->configMode++;
    }
}

/**
 * Show the file info of the file selected to copy from.
 * Update function for `CM_SETUP_COPY_DEST_2`
 */
void FileChoose_SetupCopyDest2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->nameBoxAlpha[thisv->buttonIndex] -= 25;
    thisv->fileInfoAlpha[thisv->buttonIndex] += 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->nameBoxAlpha[thisv->buttonIndex] = 0;
        thisv->fileInfoAlpha[thisv->buttonIndex] = 200;
        thisv->buttonIndex = FS_BTN_COPY_QUIT;
        thisv->actionTimer = 8;
        thisv->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Allow the player to select a slot to copy to or exit to the copy select screen.
 * Update function for `CM_SELECT_COPY_DEST`
 */
void FileChoose_SelectCopyDest(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];

    if (((thisv->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->buttonIndex = thisv->selectedFileIndex;
        thisv->nextTitleLabel = FS_TITLE_COPY_FROM;
        thisv->actionTimer = 8;
        thisv->configMode = CM_EXIT_TO_COPY_SOURCE_1;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (!SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
            thisv->copyDestFileIndex = thisv->buttonIndex;
            thisv->nextTitleLabel = FS_TITLE_COPY_CONFIRM;
            thisv->actionTimer = 8;
            thisv->configMode = CM_SETUP_COPY_CONFIRM_1;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {

        if (ABS(thisv->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (thisv->stickRelY >= 30) {
                thisv->buttonIndex--;

                if ((thisv->buttonIndex == thisv->selectedFileIndex)) {
                    thisv->buttonIndex--;

                    if (thisv->buttonIndex < FS_BTN_COPY_FILE_1) {
                        thisv->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                } else {
                    if (thisv->buttonIndex < FS_BTN_COPY_FILE_1) {
                        thisv->buttonIndex = FS_BTN_COPY_QUIT;
                    }
                }
            } else {
                thisv->buttonIndex++;

                if (thisv->buttonIndex > FS_BTN_COPY_QUIT) {
                    thisv->buttonIndex = FS_BTN_COPY_FILE_1;
                }

                if (thisv->buttonIndex == thisv->selectedFileIndex) {
                    thisv->buttonIndex++;
                }
            }
        }

        if (thisv->buttonIndex != FS_BTN_COPY_QUIT) {
            if (SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
                thisv->warningLabel = FS_WARNING_FILE_IN_USE;
                thisv->warningButtonIndex = thisv->buttonIndex;
                thisv->emptyFileTextAlpha = 255;
            } else {
                thisv->warningLabel = FS_WARNING_NONE;
            }
        }
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to copy source screen.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_1`
 */
void FileChoose_ExitToCopySource1(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->fileInfoAlpha[thisv->buttonIndex] -= 25;
    thisv->nameBoxAlpha[thisv->buttonIndex] += 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->nextTitleLabel = FS_TITLE_COPY_FROM;
        thisv->nameBoxAlpha[thisv->buttonIndex] = 200;
        thisv->fileInfoAlpha[thisv->buttonIndex] = 0;
        thisv->actionTimer = 8;
        thisv->configMode++;
    }
}

/**
 * Move the buttons back into place and return to copy source select.
 * Update function for `CM_EXIT_TO_COPY_SOURCE_2`
 */
void FileChoose_ExitToCopySource2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 3; i++) {
        yStep = ABS(thisv->buttonYOffsets[i] - sChooseFileYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->buttonIndex = 3;
        thisv->configMode = CM_SELECT_COPY_SOURCE;
    }
}

/**
 * Rearrange buttons on the screen to prepare for copy confirmation.
 * Update function for `CM_SETUP_COPY_CONFIRM_1`
 */
void FileChoose_SetupCopyConfirm1(GameState* thisx) {
    static s16 D_808124A4[] = { -56, -40, -24, 0 };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;

    for (i = 0; i < 3; i++) {
        if ((i != thisv->copyDestFileIndex) && (i != thisv->selectedFileIndex)) {
            thisv->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->connectorAlpha[i] -= 31;
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
            }
        } else {
            if (thisv->copyDestFileIndex == i) {
                yStep = ABS(thisv->buttonYOffsets[i] - D_808124A4[i]) / thisv->actionTimer;
                thisv->buttonYOffsets[i] += yStep;

                if (thisv->buttonYOffsets[i] >= D_808124A4[i]) {
                    thisv->buttonYOffsets[i] = D_808124A4[i];
                }
            }
        }
    }

    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->actionTimer = 8;
        thisv->configMode++;
    }
}

/**
 * Fade in the 'Yes' button before allowing the player to decide.
 * Update function for `CM_SETUP_COPY_CONFIRM_2`
 */
void FileChoose_SetupCopyConfirm2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->configMode = CM_COPY_CONFIRM;
        thisv->buttonIndex = FS_BTN_CONFIRM_QUIT;
    }
}

/**
 * Allow the player to confirm the copy, or quit back to the destination select.
 * If yes is selected, the actual copy occurs in thisv function before moving on to the animation.
 * Update function for `CM_COPY_CONFIRM`
 */
void FileChoose_CopyConfirm(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];
    u16 dayTime;

    if (((thisv->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->actionTimer = 8;
        thisv->nextTitleLabel = FS_TITLE_COPY_TO;
        thisv->configMode = CM_RETURN_TO_COPY_DEST;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        dayTime = gSaveContext.dayTime;
        Sram_CopySave(thisv, sramCtx);
        gSaveContext.dayTime = dayTime;
        thisv->fileInfoAlpha[thisv->copyDestFileIndex] = thisv->nameAlpha[thisv->copyDestFileIndex] = 0;
        thisv->nextTitleLabel = FS_TITLE_COPY_COMPLETE;
        thisv->actionTimer = 8;
        thisv->configMode = CM_COPY_ANIM_1;
        func_800AA000(300.0f, 0xB4, 0x14, 0x64);
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (ABS(thisv->stickRelY) >= 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->buttonIndex ^= 1;
    }
}

/**
 * Move buttons back in place and return to copy destination select.
 * Update function for `CM_RETURN_TO_COPY_DEST`
 */
void FileChoose_ReturnToCopyDest(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;

    for (i = 0; i < 3; i++) {
        if ((i != thisv->copyDestFileIndex) && (i != thisv->selectedFileIndex)) {
            thisv->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
                thisv->connectorAlpha[i] += 31;
            }
        }

        yStep = ABS(thisv->buttonYOffsets[i] - D_8081248C[thisv->selectedFileIndex][i]) / thisv->actionTimer;

        if (D_8081248C[thisv->selectedFileIndex][i] >= thisv->buttonYOffsets[i]) {
            thisv->buttonYOffsets[i] += yStep;
        } else {
            thisv->buttonYOffsets[i] -= yStep;
        }
    }

    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->actionTimer = 8;
        thisv->buttonIndex = FS_BTN_COPY_QUIT;
        thisv->configMode = CM_SELECT_COPY_DEST;
    }
}

/**
 * Hide title
 * Update function for `CM_COPY_ANIM_1`
 */
void FileChoose_CopyAnim1(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->titleAlpha[0] -= 31;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->titleAlpha[0] = 0;
        thisv->actionTimer = 8;
        thisv->configMode++;
        osSyncPrintf("connect_alpha=%d  decision_alpha[%d]=%d\n", thisv->connectorAlpha[thisv->copyDestFileIndex],
                     thisv->copyDestFileIndex, thisv->fileInfoAlpha[thisv->copyDestFileIndex]);
    }
}

/**
 * Move a copy of the file window down and fade in the file info.
 * Update function for `CM_COPY_ANIM_2`
 */
void FileChoose_CopyAnim2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 yStep;

    thisv->fileInfoAlpha[thisv->copyDestFileIndex] += 25;
    thisv->nameAlpha[thisv->copyDestFileIndex] += 25;
    thisv->titleAlpha[1] += 31;
    yStep = ABS(thisv->fileNamesY[thisv->copyDestFileIndex] + 56) / thisv->actionTimer;
    thisv->fileNamesY[thisv->copyDestFileIndex] -= yStep;

    if (thisv->fileNamesY[thisv->copyDestFileIndex] <= -56) {
        thisv->fileNamesY[thisv->copyDestFileIndex] = -56;
    }

    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 90;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode++;
    }
}

/**
 * Play sound to indicate that the copy has completed. Wait for a timer or for
 * the player to press a button before moving on.
 * Update function for `CM_COPY_ANIM_3`
 */
void FileChoose_CopyAnim3(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Input* input = &thisv->state.input[0];

    if (thisv->actionTimer == 75) {
        thisv->connectorAlpha[thisv->copyDestFileIndex] = 255;
        Audio_PlaySoundGeneral(NA_SE_EV_DIAMOND_SWITCH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    thisv->actionTimer--;

    if (thisv->actionTimer < 74) {
        if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (thisv->actionTimer == 0)) {
            thisv->actionTimer = 8;
            thisv->nextTitleLabel = FS_TITLE_SELECT_FILE;
            thisv->configMode++;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

/**
 * Fade out the info boxes for both files and bring in their name boxes. Fade out title.
 * Update function for `CM_COPY_ANIM_4`
 */
void FileChoose_CopyAnim4(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->fileInfoAlpha[thisv->selectedFileIndex] -= 25;
    thisv->fileInfoAlpha[thisv->copyDestFileIndex] -= 25;
    thisv->nameBoxAlpha[thisv->selectedFileIndex] += 25;
    thisv->nameBoxAlpha[thisv->copyDestFileIndex] += 25;
    thisv->titleAlpha[0] -= 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->fileNamesY[thisv->copyDestFileIndex] = thisv->buttonYOffsets[3] = 0;
        thisv->actionTimer = 8;
        thisv->titleAlpha[0] = 0;
        thisv->configMode++;
    }
}

/**
 * Restore all buttons and labels back to their original place and go back to the main menu.
 * Update function for `CM_COPY_ANIM_5`
 */
void FileChoose_CopyAnim5(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(thisv->buttonYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= 0) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        if (i != thisv->buttonIndex) {
            thisv->fileButtonAlpha[i] += 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
                thisv->connectorAlpha[i] += 31;
            }
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    thisv->optionButtonAlpha += 25;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            thisv->connectorAlpha[i] = 0;
            thisv->fileButtonAlpha[i] = 200;
            thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->connectorAlpha[i];

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->connectorAlpha[i] = 255;
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
            }
        }

        thisv->fileNamesY[thisv->selectedFileIndex] = 0;
        thisv->highlightColor[3] = 70;
        thisv->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode = CM_MAIN_MENU;
    }
}

/**
 * Exit from the copy source screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_COPY_RETURN_MAIN`
 */
void FileChoose_ExitCopyToMain(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(thisv->buttonYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= 0) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode = CM_MAIN_MENU;
    }

    thisv->optionButtonAlpha = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Move buttons into place for the erase select screen and fade in the proper labels.
 * Update function for `CM_SETUP_ERASE_SELECT`
 */
void FileChoose_SetupEraseSelect(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(thisv->buttonYOffsets[i] - sChooseFileYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= sChooseFileYOffsets[i]) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] -= 50;
    thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] -= 50;
    thisv->optionButtonAlpha -= 50;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] += 25;

    if (thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] <= 0) {
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            thisv->optionButtonAlpha = 0;
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->highlightColor[3] = 70;
        thisv->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
            thisv->optionButtonAlpha = 0;
        thisv->confirmButtonAlpha[1] = 200;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->buttonIndex = FS_BTN_ERASE_QUIT;
        thisv->configMode++;
    }
}

/**
 * Allow the player to select a file to erase or exit back to the main menu.
 * Update function for `CM_ERASE_SELECT`
 */
void FileChoose_EraseSelect(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    Input* input = &thisv->state.input[0];

    if (((thisv->buttonIndex == FS_BTN_COPY_QUIT) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->buttonIndex = FS_BTN_MAIN_ERASE;
        thisv->actionTimer = 8;
        thisv->nextTitleLabel = FS_TITLE_SELECT_FILE;
        thisv->configMode = CM_EXIT_ERASE_TO_MAIN;
        thisv->warningLabel = FS_WARNING_NONE;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        if (SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
            thisv->actionTimer = 8;
            thisv->selectedFileIndex = thisv->buttonIndex;
            thisv->configMode = CM_SETUP_ERASE_CONFIRM_1;
            thisv->nextTitleLabel = FS_TITLE_ERASE_CONFIRM;
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else {
        if (ABS(thisv->stickRelY) >= 30) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if (thisv->stickRelY >= 30) {
                thisv->buttonIndex--;
                if (thisv->buttonIndex < FS_BTN_ERASE_FILE_1) {
                    thisv->buttonIndex = FS_BTN_ERASE_QUIT;
                }
            } else {
                thisv->buttonIndex++;
                if (thisv->buttonIndex > FS_BTN_ERASE_QUIT) {
                    thisv->buttonIndex = FS_BTN_ERASE_FILE_1;
                }
            }
        }

        if (thisv->buttonIndex != FS_BTN_ERASE_QUIT) {
            if (!SLOT_OCCUPIED(sramCtx, thisv->buttonIndex)) {
                thisv->warningLabel = FS_WARNING_FILE_EMPTY;
                thisv->warningButtonIndex = thisv->buttonIndex;
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
 * ...
 * Update function for `CM_SETUP_ERASE_CONFIRM_1`
 */
void FileChoose_SetupEraseConfirm1(GameState* thisx) {
    static s16 D_808124AC[] = { 0, 16, 32 };
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 3; i++) {
        if (i != thisv->buttonIndex) {
            thisv->fileButtonAlpha[i] -= 25;

            if (SLOT_OCCUPIED(sramCtx, i)) {
                thisv->connectorAlpha[i] -= 31;
                thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
            }
        } else {
            thisv->nameBoxAlpha[i] -= 25;
        }
    }
    thisv->titleAlpha[0] -= 15;
    thisv->titleAlpha[1] += 15;

    yStep = ABS(thisv->buttonYOffsets[thisv->buttonIndex] - D_808124AC[thisv->buttonIndex]) / thisv->actionTimer;

    if (thisv->buttonYOffsets[thisv->buttonIndex] >= D_808124AC[thisv->buttonIndex]) {
        thisv->buttonYOffsets[thisv->buttonIndex] -= yStep;
    } else {
        thisv->buttonYOffsets[thisv->buttonIndex] += yStep;
    }

    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        for (i = 0; i < 3; i++) {
            if (i != thisv->buttonIndex) {
                thisv->fileButtonAlpha[i] = 0;

                if (SLOT_OCCUPIED(sramCtx, i)) {
                    thisv->connectorAlpha[i] = 0;
                    thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i] = 0;
                }
            } else {
                thisv->nameBoxAlpha[i] = 0;
            }
        }

        thisv->buttonYOffsets[thisv->buttonIndex] = D_808124AC[thisv->buttonIndex];
        thisv->actionTimer = 8;
        thisv->configMode++;
    }
}

/**
 * Show the file info of the file selected to erase.
 * Update function for `CM_SETUP_ERASE_CONFIRM_2`
 */
void FileChoose_SetupEraseConfirm2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] += 25;
    thisv->titleAlpha[0] -= 15;
    thisv->titleAlpha[1] += 15;
    thisv->fileInfoAlpha[thisv->buttonIndex] += 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 8;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->fileInfoAlpha[thisv->buttonIndex] = thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = 200;
        thisv->buttonIndex = FS_BTN_ERASE_FILE_2;
        thisv->configMode = CM_ERASE_CONFIRM;
    }
}

/**
 * Allow the player to confirm their chioce to erase or return back to erase select.
 * Update function for `CM_ERASE_CONFIRM`
 */
void FileChoose_EraseConfirm(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Input* input = &thisv->state.input[0];

    if (((thisv->buttonIndex != FS_BTN_CONFIRM_YES) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) ||
        CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->buttonIndex = thisv->selectedFileIndex;
        thisv->nextTitleLabel = FS_TITLE_ERASE_FILE;
        thisv->configMode = CM_EXIT_TO_ERASE_SELECT_1;
        thisv->actionTimer = 8;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_START)) {
        thisv->n64ddFlags[thisv->selectedFileIndex] = thisv->connectorAlpha[thisv->selectedFileIndex] = 0;
        Audio_PlaySoundGeneral(NA_SE_EV_DIAMOND_SWITCH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->actionTimer = 8;
        thisv->configMode = CM_ERASE_ANIM_1;
        thisv->nextTitleLabel = FS_TITLE_ERASE_COMPLETE;
        func_800AA000(200.0f, 0xFF, 0x14, 0x96);
        sEraseDelayTimer = 15;
    } else if (ABS(thisv->stickRelY) >= 30) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->buttonIndex ^= 1;
    }
}

/**
 * Fade out file info, bring back the name box, and get ready to return to erase select screen.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_1`
 */
void FileChoose_ExitToEraseSelect1(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;

    thisv->fileInfoAlpha[thisv->buttonIndex] -= 25;
    thisv->nameBoxAlpha[thisv->buttonIndex] += 25;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 8;
        thisv->fileInfoAlpha[thisv->buttonIndex] = 0;
        thisv->configMode++;
    }
}

/**
 * Move the buttons back into place and return to erase select.
 * Update function for `CM_EXIT_TO_ERASE_SELECT_2`
 */
void FileChoose_ExitToEraseSelect2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    yStep = ABS(thisv->buttonYOffsets[thisv->buttonIndex] - sChooseFileYOffsets[thisv->buttonIndex]) / thisv->actionTimer;

    if (thisv->buttonYOffsets[thisv->buttonIndex] >= sChooseFileYOffsets[thisv->buttonIndex]) {
        thisv->buttonYOffsets[thisv->buttonIndex] -= yStep;
    } else {
        thisv->buttonYOffsets[thisv->buttonIndex] += yStep;
    }

    for (i = 0; i < 3; i++) {
        if (i != thisv->buttonIndex) {
            thisv->fileButtonAlpha[i] += 25;

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
        thisv->buttonYOffsets[thisv->buttonIndex] = sChooseFileYOffsets[thisv->buttonIndex];
        thisv->actionTimer = 8;
        thisv->buttonIndex = FS_BTN_ERASE_QUIT;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode = CM_ERASE_SELECT;
    }
}

/**
 * Wait for an initial delay, then start fading out the selected file.
 * The actual file deletion occurs in thisv function
 * Update function for `CM_ERASE_ANIM_1`
 */
void FileChoose_EraseAnim1(GameState* thisx) {
    static s16 D_80813800;
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;

    if (sEraseDelayTimer == 0) {
        if (thisv->actionTimer == 8) {
            D_80813800 = 1;
        }

        if (thisv->actionTimer != 0) {
            thisv->titleAlpha[0] -= 31;
            thisv->titleAlpha[1] += 31;
            thisv->fileInfoAlpha[thisv->selectedFileIndex] -= 25;
            thisv->confirmButtonAlpha[FS_BTN_CONFIRM_YES] -= 25;
            thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 25;
        }

        thisv->fileNamesY[thisv->selectedFileIndex] -= D_80813800;
        thisv->actionTimer--;
        D_80813800 += 2;

        if (thisv->actionTimer == 0) {
            Sram_EraseSave(thisv, sramCtx);
            thisv->titleLabel = thisv->nextTitleLabel;
            thisv->titleAlpha[0] = 255;
            thisv->titleAlpha[1] = thisv->connectorAlpha[thisv->selectedFileIndex] = 0;

            // probably a fake match, there should be a better chained assignment
            thisv->confirmButtonAlpha[0] = thisv->confirmButtonAlpha[1] = 0;
            if (1) {}
            thisv->fileInfoAlpha[thisv->selectedFileIndex] = thisv->nameBoxAlpha[thisv->selectedFileIndex] =
                thisv->confirmButtonAlpha[1];

            thisv->configMode++;
            thisv->actionTimer = 90;
        }
    } else {
        sEraseDelayTimer--;

        if (sEraseDelayTimer == 0) {
            Audio_PlaySoundGeneral(NA_SE_OC_ABYSS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
}

/**
 * Wait for a delay timer or for the palyer to press a button before returning to the main menu.
 * Update function for `CM_ERASE_ANIM_2`
 */
void FileChoose_EraseAnim2(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    Input* input = &thisv->state.input[0];

    if (CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B | BTN_START) || (--thisv->actionTimer == 0)) {
        thisv->buttonYOffsets[3] = 0;
        thisv->actionTimer = 8;
        thisv->nextTitleLabel = FS_TITLE_SELECT_FILE;
        thisv->configMode++;
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

/**
 * Exit from the erase animation to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_ERASE_ANIM_3`
 */
void FileChoose_EraseAnim3(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    SramContext* sramCtx = &thisv->sramCtx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(thisv->buttonYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= 0) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    for (i = 0; i < 3; i++) {
        thisv->fileButtonAlpha[i] += 25;

        if (SLOT_OCCUPIED(sramCtx, i)) {
            thisv->nameBoxAlpha[i] = thisv->nameAlpha[i] = thisv->fileButtonAlpha[i];
            thisv->connectorAlpha[i] += 31;
        }
    }

    if (thisv->fileButtonAlpha[thisv->selectedFileIndex] >= 200) {
        thisv->fileButtonAlpha[thisv->selectedFileIndex] = 200;
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->fileNamesY[thisv->selectedFileIndex] = 0;
        thisv->highlightColor[3] = 70;
        thisv->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        thisv->confirmButtonAlpha[0] = thisv->confirmButtonAlpha[1] = 0;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode = CM_MAIN_MENU;
    }

    thisv->optionButtonAlpha = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY];
}

/**
 * Exit from the erase select screen to the main menu. Return all buttons and labels to their original place.
 * Update function for `CM_EXIT_ERASE_TO_MAIN`
 */
void FileChoose_ExitEraseToMain(GameState* thisx) {
    FileChooseContext* thisv = (FileChooseContext*)thisx;
    s16 i;
    s16 yStep;

    for (i = 0; i < 5; i++) {
        yStep = ABS(thisv->buttonYOffsets[i]) / thisv->actionTimer;

        if (thisv->buttonYOffsets[i] >= 0) {
            thisv->buttonYOffsets[i] -= yStep;
        } else {
            thisv->buttonYOffsets[i] += yStep;
        }
    }

    thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] += 25;
    thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] += 25;
    thisv->optionButtonAlpha += 25;
    thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] -= 50;

    if (thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] <= 0) {
        thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
    }

    thisv->titleAlpha[0] -= 31;
    thisv->titleAlpha[1] += 31;
    thisv->actionTimer--;

    if (thisv->actionTimer == 0) {
        thisv->highlightColor[3] = 70;
        thisv->highlightPulseDir = 1;
        XREG(35) = XREG(36);
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY] = 200;
        thisv->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = 0;
        thisv->titleLabel = thisv->nextTitleLabel;
        thisv->titleAlpha[0] = 255;
        thisv->titleAlpha[1] = 0;
        thisv->configMode = CM_MAIN_MENU;
    }

    thisv->optionButtonAlpha = thisv->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        thisv->actionButtonAlpha[FS_BTN_ACTION_COPY];
}
