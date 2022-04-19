#include "z_en_syateki_man.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.h"
#include "objects/object_ossan/object_ossan.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_27)

typedef enum {
    /* 0 */ SYATEKI_RESULT_NONE,
    /* 1 */ SYATEKI_RESULT_WINNER,
    /* 2 */ SYATEKI_RESULT_ALMOST,
    /* 3 */ SYATEKI_RESULT_FAILURE,
    /* 4 */ SYATEKI_RESULT_REFUSE
} EnSyatekiManGameResult;

typedef enum {
    /* 0 */ SYATEKI_TEXT_CHOICE,
    /* 1 */ SYATEKI_TEXT_START_GAME,
    /* 2 */ SYATEKI_TEXT_NO_RUPEES,
    /* 3 */ SYATEKI_TEXT_REFUSE
} EnSyatekiManTextIdx;

void EnSyatekiMan_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiMan_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiMan_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiMan_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSyatekiMan_Start(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_SetupIdle(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_Idle(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_Talk(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_StopTalk(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_StartGame(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_WaitForGame(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_EndGame(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_GivePrize(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_FinishPrize(EnSyatekiMan* thisv, GlobalContext* globalCtx);
void EnSyatekiMan_RestartGame(EnSyatekiMan* thisv, GlobalContext* globalCtx);

void EnSyatekiMan_BlinkWait(EnSyatekiMan* thisv);
void EnSyatekiMan_Blink(EnSyatekiMan* thisv);

void EnSyatekiMan_SetBgm(void);

ActorInit En_Syateki_Man_InitVars = {
    ACTOR_EN_SYATEKI_MAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OSSAN,
    sizeof(EnSyatekiMan),
    (ActorFunc)EnSyatekiMan_Init,
    (ActorFunc)EnSyatekiMan_Destroy,
    (ActorFunc)EnSyatekiMan_Update,
    (ActorFunc)EnSyatekiMan_Draw,
    NULL,
};

static u16 sBgmList[] = {
    NA_BGM_GENERAL_SFX,
    NA_BGM_NATURE_AMBIENCE,
    NA_BGM_FIELD_LOGIC,
    NA_BGM_DUNGEON,
    NA_BGM_KAKARIKO_ADULT,
    NA_BGM_FIELD_LOGIC,
    NA_BGM_KAKARIKO_ADULT,
    NA_BGM_ENEMY,
    NA_BGM_ENEMY,
    NA_BGM_ENEMY | 0x800,
    NA_BGM_BOSS,
    NA_BGM_INSIDE_DEKU_TREE,
    NA_BGM_MARKET,
    NA_BGM_TITLE,
    NA_BGM_LINK_HOUSE,
    NA_BGM_GAME_OVER,
    NA_BGM_BOSS_CLEAR,
    NA_BGM_ITEM_GET | 0x900,
    NA_BGM_OPENING_GANON,
    NA_BGM_HEART_GET | 0x900,
    NA_BGM_OCA_LIGHT,
    NA_BGM_JABU_JABU,
    NA_BGM_KAKARIKO_KID,
    NA_BGM_GREAT_FAIRY,
    NA_BGM_ZELDA_THEME,
    NA_BGM_FIRE_TEMPLE,
    NA_BGM_OPEN_TRE_BOX | 0x900,
    NA_BGM_FOREST_TEMPLE,
    NA_BGM_COURTYARD,
    NA_BGM_GANON_TOWER,
    NA_BGM_LONLON,
    NA_BGM_GORON_CITY,
    NA_BGM_SPIRITUAL_STONE,
    NA_BGM_OCA_BOLERO,
    NA_BGM_OCA_MINUET,
    NA_BGM_OCA_SERENADE,
    NA_BGM_OCA_REQUIEM,
    NA_BGM_OCA_NOCTURNE,
    NA_BGM_MINI_BOSS,
    NA_BGM_SMALL_ITEM_GET,
    NA_BGM_TEMPLE_OF_TIME,
    NA_BGM_EVENT_CLEAR,
    NA_BGM_KOKIRI,
    NA_BGM_OCA_FAIRY_GET,
    NA_BGM_SARIA_THEME,
    NA_BGM_SPIRIT_TEMPLE,
    NA_BGM_HORSE,
    NA_BGM_HORSE_GOAL,
    NA_BGM_INGO,
    NA_BGM_MEDALLION_GET,
    NA_BGM_OCA_SARIA,
    NA_BGM_OCA_EPONA,
    NA_BGM_OCA_ZELDA,
    NA_BGM_OCA_SUNS,
    NA_BGM_OCA_TIME,
    NA_BGM_OCA_STORM,
    NA_BGM_NAVI_OPENING,
    NA_BGM_DEKU_TREE_CS,
    NA_BGM_WINDMILL,
    NA_BGM_HYRULE_CS,
    NA_BGM_MINI_GAME,
    NA_BGM_SHEIK,
    NA_BGM_ZORA_DOMAIN,
    NA_BGM_APPEAR,
    NA_BGM_ADULT_LINK,
    NA_BGM_MASTER_SWORD,
    NA_BGM_INTRO_GANON,
    NA_BGM_SHOP,
    NA_BGM_CHAMBER_OF_SAGES,
    NA_BGM_FILE_SELECT,
    NA_BGM_ICE_CAVERN,
    NA_BGM_DOOR_OF_TIME,
    NA_BGM_OWL,
    NA_BGM_SHADOW_TEMPLE,
    NA_BGM_WATER_TEMPLE,
    NA_BGM_BRIDGE_TO_GANONS,
    NA_BGM_VARIOUS_SFX,
    NA_BGM_OCARINA_OF_TIME,
    NA_BGM_OCARINA_OF_TIME,
    NA_BGM_GERUDO_VALLEY,
    NA_BGM_POTION_SHOP,
    NA_BGM_KOTAKE_KOUME,
    NA_BGM_VARIOUS_SFX,
    NA_BGM_ESCAPE,
    NA_BGM_UNDERGROUND,
    NA_BGM_GANONDORF_BOSS,
    NA_BGM_GANON_BOSS,
    NA_BGM_END_DEMO,
};

static s16 sTextIds[] = { 0x2B, 0x2E, 0xC8, 0x2D };

static s16 sTextBoxCount[] = { TEXT_STATE_CHOICE, TEXT_STATE_EVENT, TEXT_STATE_EVENT, TEXT_STATE_EVENT };

void EnSyatekiMan_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSyatekiMan* thisv = (EnSyatekiMan*)thisx;

    osSyncPrintf("\n\n");
    // "Old man appeared!! Muhohohohohohohon"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 親父登場！！むほほほほほほほーん ☆☆☆☆☆ \n" VT_RST);
    thisv->actor.targetMode = 1;
    Actor_SetScale(&thisv->actor, 0.01f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gObjectOssanSkel, &gObjectOssanAnim_000338, thisv->jointTable,
                       thisv->morphTable, 9);
    if (!LINK_IS_ADULT) {
        thisv->headRot.z = 20;
    }
    thisv->blinkTimer = 20;
    thisv->eyeState = 0;
    thisv->blinkFunc = EnSyatekiMan_BlinkWait;
    thisv->actor.colChkInfo.cylRadius = 100;
    thisv->actionFunc = EnSyatekiMan_Start;
}

void EnSyatekiMan_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnSyatekiMan_Start(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gObjectOssanAnim_000338);

    Animation_Change(&thisv->skelAnime, &gObjectOssanAnim_000338, 1.0f, 0.0f, (s16)lastFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = EnSyatekiMan_SetupIdle;
}

void EnSyatekiMan_SetupIdle(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    if (thisv->gameResult == SYATEKI_RESULT_REFUSE) {
        thisv->textIdx = SYATEKI_TEXT_REFUSE;
    }

    thisv->actor.textId = sTextIds[thisv->textIdx];
    thisv->numTextBox = sTextBoxCount[thisv->textIdx];
    thisv->actionFunc = EnSyatekiMan_Idle;
}

void EnSyatekiMan_Idle(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnSyatekiMan_Talk;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
}

void EnSyatekiMan_Talk(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    s16 nextState = 0;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->cameraHold) {
        globalCtx->shootingGalleryStatus = -2;
    }
    if ((thisv->numTextBox == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->textIdx == SYATEKI_TEXT_CHOICE) {
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    if (gSaveContext.rupees >= 20) {
                        Rupees_ChangeBy(-20);
                        thisv->textIdx = SYATEKI_TEXT_START_GAME;
                        nextState = 1;
                    } else {
                        thisv->textIdx = SYATEKI_TEXT_NO_RUPEES;
                        nextState = 2;
                    }
                    thisv->actor.textId = sTextIds[thisv->textIdx];
                    thisv->numTextBox = sTextBoxCount[thisv->textIdx];
                    break;
                case 1:
                    thisv->actor.textId = sTextIds[SYATEKI_TEXT_REFUSE];
                    thisv->numTextBox = sTextBoxCount[SYATEKI_TEXT_REFUSE];
                    nextState = 2;
                    break;
            }
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        } else {
            Message_CloseTextbox(globalCtx);
        }
        switch (nextState) {
            case 0:
                thisv->actionFunc = EnSyatekiMan_SetupIdle;
                break;
            case 1:
                thisv->actionFunc = EnSyatekiMan_StartGame;
                break;
            case 2:
                thisv->actionFunc = EnSyatekiMan_StopTalk;
                break;
        }
    }
}

void EnSyatekiMan_StopTalk(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->cameraHold) {
        globalCtx->shootingGalleryStatus = -2;
    }
    if ((thisv->numTextBox == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->cameraHold) {
            OnePointCutscene_EndCutscene(globalCtx, thisv->csCam);
            thisv->csCam = SUBCAM_NONE;
            thisv->cameraHold = false;
        }
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnSyatekiMan_SetupIdle;
    }
}

void EnSyatekiMan_StartGame(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    EnSyatekiItm* gallery;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->cameraHold) {
        globalCtx->shootingGalleryStatus = -2;
    }
    if ((thisv->numTextBox == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->cameraHold) {
            OnePointCutscene_EndCutscene(globalCtx, thisv->csCam);
            thisv->csCam = SUBCAM_NONE;
            thisv->cameraHold = false;
        }
        Message_CloseTextbox(globalCtx);
        gallery = ((EnSyatekiItm*)thisv->actor.parent);
        if (gallery->actor.update != NULL) {
            gallery->signal = ENSYATEKI_START;
            thisv->actionFunc = EnSyatekiMan_WaitForGame;
        }
    }
}

void EnSyatekiMan_WaitForGame(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    EnSyatekiItm* gallery;

    SkelAnime_Update(&thisv->skelAnime);
    if (1) {}
    gallery = ((EnSyatekiItm*)thisv->actor.parent);
    if ((gallery->actor.update != NULL) && (gallery->signal == ENSYATEKI_END)) {
        thisv->csCam = OnePointCutscene_Init(globalCtx, 8002, -99, &thisv->actor, MAIN_CAM);
        switch (gallery->hitCount) {
            case 10:
                thisv->gameResult = SYATEKI_RESULT_WINNER;
                thisv->actor.textId = 0x71AF;
                break;
            case 8:
            case 9:
                thisv->gameResult = SYATEKI_RESULT_ALMOST;
                thisv->actor.textId = 0x71AE;
                break;
            default:
                thisv->gameResult = SYATEKI_RESULT_FAILURE;
                thisv->actor.textId = 0x71AD;
                if (globalCtx->shootingGalleryStatus == 15 + 1) {
                    thisv->gameResult = SYATEKI_RESULT_REFUSE;
                    thisv->actor.textId = 0x2D;
                }
                break;
        }
        globalCtx->shootingGalleryStatus = -2;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        thisv->actionFunc = EnSyatekiMan_EndGame;
    }
}

void EnSyatekiMan_EndGame(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    EnSyatekiItm* gallery;

    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->numTextBox == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->gameResult != SYATEKI_RESULT_FAILURE) {
            OnePointCutscene_EndCutscene(globalCtx, thisv->csCam);
            thisv->csCam = SUBCAM_NONE;
        }
        Message_CloseTextbox(globalCtx);
        gallery = ((EnSyatekiItm*)thisv->actor.parent);
        if (gallery->actor.update != NULL) {
            gallery->signal = ENSYATEKI_RESULTS;
            thisv->textIdx = 0;
            switch (thisv->gameResult) {
                case SYATEKI_RESULT_WINNER:
                    thisv->tempGallery = thisv->actor.parent;
                    thisv->actor.parent = NULL;
                    if (!LINK_IS_ADULT) {
                        if (!(gSaveContext.itemGetInf[0] & 0x2000)) {
                            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Equip_Pachinko ☆☆☆☆☆ %d\n" VT_RST,
                                         CUR_UPG_VALUE(UPG_BULLET_BAG));
                            if (CUR_UPG_VALUE(UPG_BULLET_BAG) == 1) {
                                thisv->getItemId = GI_BULLET_BAG_40;
                            } else {
                                thisv->getItemId = GI_BULLET_BAG_50;
                            }
                        } else {
                            thisv->getItemId = GI_RUPEE_PURPLE;
                        }
                    } else {
                        if (!(gSaveContext.itemGetInf[0] & 0x4000)) {
                            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Equip_Bow ☆☆☆☆☆ %d\n" VT_RST,
                                         CUR_UPG_VALUE(UPG_QUIVER));
                            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                                case 0:
                                    thisv->getItemId = GI_RUPEE_PURPLE;
                                    break;
                                case 1:
                                    thisv->getItemId = GI_QUIVER_40;
                                    break;
                                case 2:
                                    thisv->getItemId = GI_QUIVER_50;
                                    break;
                            }
                        } else {
                            thisv->getItemId = GI_RUPEE_PURPLE;
                        }
                    }
                    func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 2000.0f, 1000.0f);
                    thisv->actionFunc = EnSyatekiMan_GivePrize;
                    break;
                case SYATEKI_RESULT_ALMOST:
                    thisv->timer = 20;
                    func_8008EF44(globalCtx, 15);
                    thisv->actionFunc = EnSyatekiMan_RestartGame;
                    break;
                default:
                    if (thisv->gameResult == SYATEKI_RESULT_REFUSE) {
                        thisv->actionFunc = EnSyatekiMan_SetupIdle;
                    } else {
                        thisv->cameraHold = true;
                        thisv->actor.textId = sTextIds[thisv->textIdx];
                        thisv->numTextBox = sTextBoxCount[thisv->textIdx];
                        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
                        thisv->actionFunc = EnSyatekiMan_Talk;
                    }
                    break;
            }
        }
    }
}

void EnSyatekiMan_GivePrize(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnSyatekiMan_FinishPrize;
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 2000.0f, 1000.0f);
    }
}

void EnSyatekiMan_FinishPrize(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        // "Successful completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        if (!LINK_IS_ADULT) {
            gSaveContext.itemGetInf[0] |= 0x2000;
        } else if ((thisv->getItemId == GI_QUIVER_40) || (thisv->getItemId == GI_QUIVER_50)) {
            gSaveContext.itemGetInf[0] |= 0x4000;
        }
        thisv->gameResult = SYATEKI_RESULT_NONE;
        thisv->actor.parent = thisv->tempGallery;
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actionFunc = EnSyatekiMan_SetupIdle;
    }
}

void EnSyatekiMan_RestartGame(EnSyatekiMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer == 0) {
        EnSyatekiItm* gallery = ((EnSyatekiItm*)thisv->actor.parent);

        if (gallery->actor.update != NULL) {
            gallery->signal = ENSYATEKI_START;
            thisv->gameResult = SYATEKI_RESULT_NONE;
            thisv->actionFunc = EnSyatekiMan_WaitForGame;
            // "Let's try again! Baby!"
            osSyncPrintf(VT_FGCOL(BLUE) "再挑戦だぜ！ベイビー！" VT_RST "\n", thisv);
        }
    }
}

void EnSyatekiMan_BlinkWait(EnSyatekiMan* thisv) {
    s16 decrBlinkTimer = thisv->blinkTimer - 1;

    if (decrBlinkTimer != 0) {
        thisv->blinkTimer = decrBlinkTimer;
    } else {
        thisv->blinkFunc = EnSyatekiMan_Blink;
    }
}

void EnSyatekiMan_Blink(EnSyatekiMan* thisv) {
    s16 decrBlinkTimer = thisv->blinkTimer - 1;

    if (decrBlinkTimer != 0) {
        thisv->blinkTimer = decrBlinkTimer;
    } else {
        s16 nextEyeState = thisv->eyeState + 1;

        if (nextEyeState >= 3) {
            thisv->eyeState = 0;
            thisv->blinkTimer = 20 + (s32)(Rand_ZeroOne() * 60.0f);
            thisv->blinkFunc = EnSyatekiMan_BlinkWait;
        } else {
            thisv->eyeState = nextEyeState;
            thisv->blinkTimer = 1;
        }
    }
}

void EnSyatekiMan_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSyatekiMan* thisv = (EnSyatekiMan*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    thisv->actionFunc(thisv, globalCtx);
    EnSyatekiMan_SetBgm();
    thisv->blinkFunc(thisv);
    thisv->actor.focus.pos.y = 70.0f;
    Actor_SetFocus(&thisv->actor, 70.0f);
    func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->bodyRot, thisv->actor.focus.pos);
}

s32 EnSyatekiMan_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    EnSyatekiMan* thisv = (EnSyatekiMan*)thisx;
    s32 turnDirection;

    if (limbIndex == 1) {
        rot->x += thisv->bodyRot.y;
    }
    if (limbIndex == 8) {
        *dList = gObjectOssanEnSyatekiManDL_007E28;
        turnDirection = 1;
        if (thisv->gameResult == SYATEKI_RESULT_REFUSE) {
            turnDirection = -1;
        }
        rot->x += thisv->headRot.y * turnDirection;
        rot->z += thisv->headRot.z;
    }
    return 0;
}

void EnSyatekiMan_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSyatekiMan* thisv = (EnSyatekiMan*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnSyatekiMan_OverrideLimbDraw, NULL, thisv);
}

void EnSyatekiMan_SetBgm(void) {
    if (BREG(80)) {
        BREG(80) = false;
        Audio_QueueSeqCmd(sBgmList[BREG(81)]);
    }
}
