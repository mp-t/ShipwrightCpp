#include "z_en_bom_bowl_man.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "objects/object_bg/object_bg.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_27)

typedef enum {
    /* 0 */ CHU_GIRL_EYES_ASLEEP,
    /* 1 */ CHU_GIRL_EYES_OPEN_SLOWLY,
    /* 2 */ CHU_GIRL_EYES_BLINK_RAPIDLY,
    /* 3 */ CHU_GIRL_EYES_AWAKE
} BombchuGirlEyeMode;

void EnBomBowlMan_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBomBowlMan_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBomBowlMan_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBomBowlMan_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBomBowMan_SetupWaitAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_WaitAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_TalkAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_WakeUp(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_BlinkAwake(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_CheckBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_WaitNotBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_TalkNotBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_SetupRunGame(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_RunGame(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowlMan_HandlePlayChoice(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void func_809C41FC(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_SetupChooseShowPrize(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowMan_ChooseShowPrize(EnBomBowlMan* thisv, GlobalContext* globalCtx);
void EnBomBowlMan_BeginPlayGame(EnBomBowlMan* thisv, GlobalContext* globalCtx);

ActorInit En_Bom_Bowl_Man_InitVars = {
    ACTOR_EN_BOM_BOWL_MAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BG,
    sizeof(EnBomBowlMan),
    (ActorFunc)EnBomBowlMan_Init,
    (ActorFunc)EnBomBowlMan_Destroy,
    (ActorFunc)EnBomBowlMan_Update,
    (ActorFunc)EnBomBowlMan_Draw,
    NULL,
};

void EnBomBowlMan_Init(Actor* thisx, GlobalContext* globalCtx2) {
    static f32 cuccoColliderDims[][3] = { { 16.0f, 46.0f, 0.0f }, { 36.0f, 56.0f, 0.0f } };
    static Vec3f cuccoSpawnPos[] = { { 60, -60, -430 }, { 0, -120, -620 } };
    static f32 cuccoScales[] = { 0.01f, 0.03f };
    EnBomBowlMan* thisv = (EnBomBowlMan*)thisx;
    EnSyatekiNiw* cucco;
    s32 i;
    GlobalContext* globalCtx = globalCtx2;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gChuGirlSkel, &gChuGirlNoddingOffAnim, thisv->jointTable,
                       thisv->morphTable, 11);
    // "☆ Man, my shoulders hurt~ ☆"
    osSyncPrintf(VT_FGCOL(GREEN) "☆ もー 肩こっちゃうよねぇ〜 \t\t ☆ \n" VT_RST);
    // "☆ Isn't there some sort of job that will pay better and be more relaxing? ☆ %d"
    osSyncPrintf(VT_FGCOL(GREEN) "☆ もっとラクしてもうかるバイトないかしら？ ☆ %d\n" VT_RST,
                 globalCtx->bombchuBowlingStatus);
    thisv->posCopy = thisv->actor.world.pos;
    thisv->actor.shape.yOffset = -60.0f;
    Actor_SetScale(&thisv->actor, 0.013f);

    for (i = 0; i < 2; i++) {
        cucco = (EnSyatekiNiw*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_SYATEKI_NIW, cuccoSpawnPos[i].x,
                                           cuccoSpawnPos[i].y, cuccoSpawnPos[i].z, 0, 0, 0, 1);

        if (cucco != NULL) {
            cucco->unk_2F4 = cuccoScales[i];
            cucco->collider.dim.radius = (s16)cuccoColliderDims[i][0];
            cucco->collider.dim.height = (s16)cuccoColliderDims[i][1];
        }
    }

    thisv->prizeSelect = (s16)Rand_ZeroFloat(4.99f);
    thisv->actor.targetMode = 1;
    thisv->actionFunc = EnBomBowMan_SetupWaitAsleep;
}

void EnBomBowlMan_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnBomBowMan_SetupWaitAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    thisv->frameCount = (f32)Animation_GetLastFrame(&gChuGirlNoddingOffAnim);
    Animation_Change(&thisv->skelAnime, &gChuGirlNoddingOffAnim, 1.0f, 0.0f, thisv->frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->actor.textId = 0xC0;
    thisv->dialogState = TEXT_STATE_EVENT;
    thisv->actionFunc = EnBomBowMan_WaitAsleep;
}

void EnBomBowMan_WaitAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;

    SkelAnime_Update(&thisv->skelAnime);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnBomBowMan_TalkAsleep;
    } else {
        yawDiff = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y));

        if (!(thisv->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
            func_8002F2CC(&thisv->actor, globalCtx, 120.0f);
        }
    }
}

void EnBomBowMan_TalkAsleep(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        thisv->actionFunc = EnBomBowMan_WakeUp;
    }
}

void EnBomBowMan_WakeUp(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    thisv->frameCount = (f32)Animation_GetLastFrame(&gChuGirlWakeUpAnim);
    Animation_Change(&thisv->skelAnime, &gChuGirlWakeUpAnim, 1.0f, 0.0f, thisv->frameCount, ANIMMODE_ONCE, -10.0f);
    thisv->eyeMode = CHU_GIRL_EYES_OPEN_SLOWLY;
    thisv->actionFunc = EnBomBowMan_BlinkAwake;
}

void EnBomBowMan_BlinkAwake(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    f32 frameCount = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (frameCount == 30.0f) {
        thisv->dialogState = TEXT_STATE_EVENT;

        // Check for beaten Dodongo's Cavern
        if ((gSaveContext.eventChkInf[2] & 0x20) || BREG(2)) {
            thisv->actor.textId = 0xBF;
        } else {
            thisv->actor.textId = 0x7058;
        }
    }
    Message_ContinueTextbox(globalCtx, thisv->actor.textId);

    if ((thisv->eyeTextureIndex == 0) && (thisv->eyeMode == CHU_GIRL_EYES_BLINK_RAPIDLY) && (thisv->blinkTimer == 0)) {
        // Blink twice, then move on
        thisv->eyeTextureIndex = 2;
        thisv->blinkCount++;
        if (thisv->blinkCount >= 3) {
            thisv->actionFunc = EnBomBowMan_CheckBeatenDC;
        }
    }
}

void EnBomBowMan_CheckBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->frameCount = (f32)Animation_GetLastFrame(&gChuGirlLeanOverCounterAnim);
        Animation_Change(&thisv->skelAnime, &gChuGirlLeanOverCounterAnim, 1.0f, 0.0f, thisv->frameCount, ANIMMODE_LOOP,
                         -10.0f);
        thisv->eyeMode = CHU_GIRL_EYES_AWAKE;
        thisv->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;

        // Check for beaten Dodongo's Cavern
        if (!((gSaveContext.eventChkInf[2] & 0x20) || BREG(2))) {
            thisv->actionFunc = EnBomBowMan_WaitNotBeatenDC;
        } else {
            thisv->actor.textId = 0x18;
            thisv->dialogState = TEXT_STATE_CHOICE;
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            thisv->actionFunc = EnBomBowlMan_HandlePlayChoice;
        }
    }
}

void EnBomBowMan_WaitNotBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnBomBowMan_TalkNotBeatenDC;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 120.0f);
    }
}

void EnBomBowMan_TalkNotBeatenDC(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnBomBowMan_WaitNotBeatenDC;
    }
}

void EnBomBowMan_SetupRunGame(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->minigamePlayStatus == 0) {
        if (!thisv->startedPlaying) {
            thisv->actor.textId = 0x18;
        } else {
            thisv->actor.textId = 0x1A;
        }

        thisv->dialogState = TEXT_STATE_CHOICE;
    } else {
        thisv->actor.textId = 0x19;
        thisv->dialogState = TEXT_STATE_EVENT;
    }

    thisv->actionFunc = EnBomBowMan_RunGame;
}

void EnBomBowMan_RunGame(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;

    SkelAnime_Update(&thisv->skelAnime);

    if (BREG(3)) {
        osSyncPrintf(VT_FGCOL(RED) "☆ game_play->bomchu_game_flag ☆ %d\n" VT_RST, globalCtx->bombchuBowlingStatus);
        // "HOW'S THE FIRST WALL DOING?"
        osSyncPrintf(VT_FGCOL(RED) "☆ 壁１の状態どう？ ☆ %d\n" VT_RST, thisv->wallStatus[0]);
        // "HOW'S THE SECOND WALL DOING?"
        osSyncPrintf(VT_FGCOL(RED) "☆ 壁２の状態どう？ ☆ %d\n" VT_RST, thisv->wallStatus[1]);
        // "HOLE INFORMATION"
        osSyncPrintf(VT_FGCOL(RED) "☆ 穴情報\t     ☆ %d\n" VT_RST, thisv->bowlPit->status);
        osSyncPrintf("\n\n");
    }

    thisv->gameResult = 0;

    if (thisv->bowlPit != NULL) {
        if ((thisv->wallStatus[0] != 1) && (thisv->wallStatus[1] != 1) && (thisv->bowlPit->status == 2)) {
            thisv->gameResult = 1; // Won
            thisv->bowlPit->status = 0;
            // "Center HIT!"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 中央ＨＩＴ！！！！ ☆☆☆☆☆ \n" VT_RST);
        }

        if ((globalCtx->bombchuBowlingStatus == -1) &&
            (globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length == 0) && (thisv->bowlPit->status == 0) &&
            (thisv->wallStatus[0] != 1) && (thisv->wallStatus[1] != 1)) {
            thisv->gameResult = 2; // Lost
            // "Bombchu lost"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ ボムチュウ消化 ☆☆☆☆☆ \n" VT_RST);
        }
    }

    if (thisv->gameResult != 0) { // won or lost
        thisv->actor.textId = 0x1A;
        thisv->dialogState = TEXT_STATE_CHOICE;
        thisv->minigamePlayStatus = 0;

        if ((thisv->exItem != NULL) && (thisv->exItem->actor.update != NULL)) {
            thisv->exItem->killItem = true;
            thisv->exItem = NULL;
        }

        globalCtx->bombchuBowlingStatus = 0;
        thisv->playingAgain = true;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);

        if (thisv->gameResult == 2) {
            func_8002DF54(globalCtx, NULL, 8);
        }
        thisv->actionFunc = EnBomBowlMan_HandlePlayChoice;
    } else {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            if (thisv->minigamePlayStatus == 0) {
                thisv->actionFunc = EnBomBowlMan_HandlePlayChoice;
            } else {
                thisv->actionFunc = func_809C41FC;
            }
        } else {
            yawDiff = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y));

            if (!(thisv->actor.xzDistToPlayer > 120.0f) && (yawDiff < 0x4300)) {
                func_8002F2CC(&thisv->actor, globalCtx, 120.0f);
            }
        }
    }
}

void EnBomBowlMan_HandlePlayChoice(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);

        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 30) {
                    Rupees_ChangeBy(-30);
                    thisv->minigamePlayStatus = 1;
                    thisv->wallStatus[0] = thisv->wallStatus[1] = 0;
                    globalCtx->bombchuBowlingStatus = 10;
                    Flags_SetSwitch(globalCtx, 0x38);

                    if (!thisv->startedPlaying && !thisv->playingAgain) {
                        thisv->actor.textId = 0x19;
                        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                        thisv->dialogState = TEXT_STATE_EVENT;
                        thisv->actionFunc = func_809C41FC;
                    } else {
                        thisv->actor.textId = 0x1B;
                        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                        thisv->dialogState = TEXT_STATE_EVENT;
                        OnePointCutscene_Init(globalCtx, 8010, -99, NULL, MAIN_CAM);
                        func_8002DF54(globalCtx, NULL, 8);
                        thisv->actionFunc = EnBomBowMan_SetupChooseShowPrize;
                    }
                } else {
                    thisv->playingAgain = false;
                    thisv->actor.textId = 0x85;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->dialogState = TEXT_STATE_EVENT;
                    thisv->actionFunc = func_809C41FC;
                }
                break;

            case 1: // No
                thisv->playingAgain = false;
                thisv->actor.textId = 0x2D;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->dialogState = TEXT_STATE_EVENT;
                thisv->actionFunc = func_809C41FC;
        }
    }
}

void func_809C41FC(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);

        if (((thisv->actor.textId == 0x2D) || (thisv->actor.textId == 0x85)) && Flags_GetSwitch(globalCtx, 0x38)) {
            Flags_UnsetSwitch(globalCtx, 0x38);
        }

        if (thisv->minigamePlayStatus == 1) {
            thisv->actor.textId = 0x1B;
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            thisv->dialogState = TEXT_STATE_EVENT;
            OnePointCutscene_Init(globalCtx, 8010, -99, NULL, MAIN_CAM);
            func_8002DF54(globalCtx, NULL, 8);
            thisv->actionFunc = EnBomBowMan_SetupChooseShowPrize;
        } else {
            if (thisv->gameResult == 2) {
                func_8002DF54(globalCtx, NULL, 7);
            }
            thisv->actionFunc = EnBomBowMan_SetupRunGame;
        }
    }
}

void EnBomBowMan_SetupChooseShowPrize(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    Vec3f accel = { 0.0f, 0.1f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        pos.x = 148.0f;
        pos.y = 40.0f;
        pos.z = 300.0f;
        EffectSsBomb2_SpawnLayered(globalCtx, &pos, &velocity, &accel, 50, 15);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_GOODS_APPEAR);
        thisv->prizeRevealTimer = 10;
        thisv->actionFunc = EnBomBowMan_ChooseShowPrize;
    }
}

static Vec3f sPrizePosOffset[] = {
    { 0.0f, 22.0f, 0.0f }, { 0.0f, 22.0f, 0.0f }, { 0.0f, 8.0f, 0.0f }, { 0.0f, 9.0f, 0.0f }, { 0.0f, -2.0f, 0.0f },
};

static s16 sPrizeRot[] = { 0x4268, 0x4268, -0x03E8, 0x0000, 0x4268, 0x0000 };

void EnBomBowMan_ChooseShowPrize(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    s16 prizeTemp;
    s32 pad;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->prizeRevealTimer == 0) {
        switch (thisv->prizeSelect) {
            case 0:
                prizeTemp = EXITEM_BOMB_BAG_BOWLING;
                if (gSaveContext.itemGetInf[1] & 2) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 1:
                prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                break;
            case 2:
                prizeTemp = EXITEM_BOMBCHUS_BOWLING;
                break;
            case 3:
                prizeTemp = EXITEM_HEART_PIECE_BOWLING;
                if (gSaveContext.itemGetInf[1] & 4) {
                    prizeTemp = EXITEM_PURPLE_RUPEE_BOWLING;
                }
                break;
            case 4:
                prizeTemp = EXITEM_BOMBS_BOWLING;
                break;
        }

        thisv->prizeIndex = prizeTemp;

        if (BREG(7)) {
            thisv->prizeIndex = BREG(7) - 1;
        }

        thisv->exItem = (EnExItem*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_ITEM,
                                                     sPrizePosOffset[thisv->prizeIndex].x + 148.0f,
                                                     sPrizePosOffset[thisv->prizeIndex].y + 40.0f,
                                                     sPrizePosOffset[thisv->prizeIndex].z + 300.0f, 0,
                                                     sPrizeRot[thisv->prizeIndex], 0, thisv->prizeIndex + EXITEM_COUNTER);

        if (!thisv->startedPlaying) {
            thisv->bowlPit = (EnBomBowlPit*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx,
                                                              ACTOR_EN_BOM_BOWL_PIT, 0.0f, 90.0f, -860.0f, 0, 0, 0, 0);
            if (thisv->bowlPit != NULL) {
                thisv->bowlPit->prizeIndex = thisv->prizeIndex;
            }
        } else {
            thisv->bowlPit->prizeIndex = thisv->prizeIndex;
        }

        thisv->bowlPit->start = 1;
        thisv->minigamePlayStatus = 2;
        thisv->actor.textId = 0x405A;
        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        thisv->dialogState = TEXT_STATE_EVENT;

        // Cycle through prizes in order
        thisv->prizeSelect++;
        if (thisv->prizeSelect >= 5) {
            thisv->prizeSelect = 0;
        }
        thisv->actionFunc = EnBomBowlMan_BeginPlayGame;
    }
}

void EnBomBowlMan_BeginPlayGame(EnBomBowlMan* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((Message_GetState(&globalCtx->msgCtx) == thisv->dialogState) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        thisv->startedPlaying = true;

        if (BREG(2)) {
            BREG(2) = 0;
        }

        // "Wow"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆ わー ☆ %d\n" VT_RST, globalCtx->bombchuBowlingStatus);
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actionFunc = EnBomBowMan_SetupRunGame;
    }
}

void EnBomBowlMan_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBomBowlMan* thisv = (EnBomBowlMan*)thisx;

    thisv->timer++;
    thisv->actor.focus.pos.y = 60.0f;
    Actor_SetFocus(&thisv->actor, 60.0f);

    switch (thisv->eyeMode) {
        case CHU_GIRL_EYES_ASLEEP:
            thisv->eyeTextureIndex = 2;
            break;
        case CHU_GIRL_EYES_OPEN_SLOWLY:
            if (thisv->eyeTextureIndex > 0) {
                thisv->eyeTextureIndex--;
            } else {
                thisv->blinkTimer = 30;
                thisv->eyeMode = CHU_GIRL_EYES_BLINK_RAPIDLY;
            }
            break;
        case CHU_GIRL_EYES_BLINK_RAPIDLY:
            if ((thisv->blinkTimer == 0) && (thisv->eyeTextureIndex > 0)) {
                thisv->eyeTextureIndex--;
            }
            break;
        default:
            if (thisv->blinkTimer == 0) {
                thisv->eyeTextureIndex++;
                if (thisv->eyeTextureIndex >= 3) {
                    thisv->eyeTextureIndex = 0;
                    thisv->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
                }
            }

            func_80038290(globalCtx, &thisv->actor, &thisv->unk_218, &thisv->unk_224, thisv->actor.focus.pos);
            break;
    }

    DECR(thisv->prizeRevealTimer);
    DECR(thisv->blinkTimer);

    thisv->actionFunc(thisv, globalCtx);
}

s32 EnBomBowlMan_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    EnBomBowlMan* thisv = (EnBomBowlMan*)thisx;

    if (limbIndex == 4) { // head
        rot->x += thisv->unk_218.y;
        rot->z += thisv->unk_218.z;
    }

    return false;
}

void EnBomBowlMan_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = { gChuGirlEyeOpenTex, gChuGirlEyeHalfTex, gChuGirlEyeClosedTex };
    EnBomBowlMan* thisv = (EnBomBowlMan*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bom_bowl_man.c", 907);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTextureIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnBomBowlMan_OverrideLimbDraw, NULL, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bom_bowl_man.c", 923);
}
