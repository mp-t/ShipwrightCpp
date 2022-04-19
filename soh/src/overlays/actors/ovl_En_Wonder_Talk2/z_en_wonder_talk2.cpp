/*
 * File: z_en_wonder_talk2.c
 * Overlay: ovl_En_Wonder_Talk2
 * Description: Dialog spot
 */

#include "z_en_wonder_talk2.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_27)

void EnWonderTalk2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWonderTalk2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWonderTalk2_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B3A10C(EnWonderTalk2* thisv, GlobalContext* globalCtx);
void func_80B3A4F8(EnWonderTalk2* thisv, GlobalContext* globalCtx);
void func_80B3A15C(EnWonderTalk2* thisv, GlobalContext* globalCtx);
void func_80B3A3D4(EnWonderTalk2* thisv, GlobalContext* globalCtx);
void EnWonderTalk2_DoNothing(EnWonderTalk2* thisv, GlobalContext* globalCtx);

ActorInit En_Wonder_Talk2_InitVars = {
    ACTOR_EN_WONDER_TALK2,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWonderTalk2),
    (ActorFunc)EnWonderTalk2_Init,
    (ActorFunc)EnWonderTalk2_Destroy,
    (ActorFunc)EnWonderTalk2_Update,
    NULL,
    NULL,
};

static s16 D_80B3A8E0[] = { 6, 0, 1, 2, 3, 4, 5 };

void EnWonderTalk2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnWonderTalk2_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnWonderTalk2* thisv = (EnWonderTalk2*)thisx;

    osSyncPrintf("\n\n");
    // "Transparent message"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君 ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params);
    thisv->baseMsgId = (thisv->actor.params >> 6) & 0xFF;
    if (thisv->actor.world.rot.z > 0) {
        s32 rangeIndex = 0;
        s16 rotZmod10 = thisv->actor.world.rot.z;

        while (rotZmod10 > 10) {
            rotZmod10 -= 10;
            rangeIndex++;
        }
        // rangeIndex = rot.z/10 here
        thisv->triggerRange = rotZmod10 * 40.0f;
        if (rangeIndex > 6) {
            rangeIndex = 0;
        }

        thisv->actor.targetMode = D_80B3A8E0[rangeIndex];

        osSyncPrintf("\n\n");
        // "originally?"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元は？       ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.world.rot.z);
        // "The range is?"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ レンジは？   ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.targetMode);
        // "Is the range?"
        osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ は、範囲わ？ ☆☆☆☆☆ %f\n" VT_RST, thisv->triggerRange);
        osSyncPrintf("\n\n");
        osSyncPrintf("\n\n");
        osSyncPrintf("\n\n");
    }

    thisv->initPos = thisv->actor.world.pos;
    thisv->switchFlag = (thisv->actor.params & 0x3F);
    thisv->talkMode = ((thisv->actor.params >> 0xE) & 3);

    if (thisv->switchFlag == 0x3F) {
        thisv->switchFlag = -1;
    }
    if (thisv->switchFlag >= 0 && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
        Actor_Kill(&thisv->actor);
        return;
    }
    if ((thisv->talkMode == 1) && (globalCtx->sceneNum == SCENE_MEN) && (thisv->switchFlag != 0x08) &&
        (thisv->switchFlag != 0x16) && (thisv->switchFlag != 0x2F)) {

        thisv->unk_15A = false;
        thisv->talkMode = 4;
    }
    if (thisv->talkMode == 3) {
        thisv->actor.flags &= ~ACTOR_FLAG_27;
        thisv->actionFunc = EnWonderTalk2_DoNothing;
    } else {
        thisv->actionFunc = func_80B3A10C;
    }
}

void func_80B3A10C(EnWonderTalk2* thisv, GlobalContext* globalCtx) {
    thisv->actor.textId = 0x200;
    thisv->actor.textId |= thisv->baseMsgId;
    if (thisv->talkMode == 1 || thisv->talkMode == 4) {
        thisv->actionFunc = func_80B3A4F8;
    } else {
        thisv->actionFunc = func_80B3A15C;
    }
}

void func_80B3A15C(EnWonderTalk2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_158++;
    if ((thisv->switchFlag >= 0) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if (!thisv->unk_15A) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->unk_15A = true;
        }
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if ((thisv->switchFlag >= 0) && (thisv->talkMode != 2)) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
            // "I saved it! All of it!"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ セーブしたよ！おもいっきり！ %x\n" VT_RST, thisv->switchFlag);
        }

        thisv->actionFunc = func_80B3A10C;
    } else {
        s16 yawDiff = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y));

        if (!((thisv->actor.xzDistToPlayer > 40.0f + thisv->triggerRange) ||
              (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) > 100.0f) || (yawDiff >= 0x4000))) {
            if (thisv->unk_158 >= 2) {
                osSyncPrintf("\n\n");
                // "Transparent Message Kimi Set"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君せっと %x\n" VT_RST, thisv->actor.params);
                // "Save Information"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ情報 \t           %x\n" VT_RST, thisv->switchFlag);
                // "Specified message type"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 指定メッセージ種類     %x\n" VT_RST, thisv->baseMsgId);
                // "Actual message type"
                osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 実質メッセージ種類     %x\n" VT_RST, thisv->actor.textId);
                // "Specified range"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 指定範囲               %d\n" VT_RST, thisv->actor.world.rot.z);
                // "Processing range"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 処理範囲               %f\n" VT_RST, thisv->triggerRange);
                switch (thisv->talkMode) {
                    case 0:
                        // "Normal"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ 通常 ☆☆ \n" VT_RST);
                        break;
                    case 2:
                        // "Check only"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ チェックのみ ☆☆ \n" VT_RST);
                        break;
                    case 3:
                        // "Lock only"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ ロックのみ ☆☆ \n" VT_RST);
                        break;
                }
            }

            thisv->unk_158 = 0;
            func_8002F1C4(&thisv->actor, globalCtx, thisv->triggerRange + 50.0f, 100.0f, EXCH_ITEM_NONE);
        }
    }
}

void func_80B3A3D4(EnWonderTalk2* thisv, GlobalContext* globalCtx) {
    if (BREG(2) != 0) {
        // "Oh"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ わー %d\n" VT_RST, Message_GetState(&globalCtx->msgCtx));
    }

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) {
                    Message_CloseTextbox(globalCtx);
                }
            } else {
                break;
            }
        case TEXT_STATE_NONE:
            if ((thisv->switchFlag >= 0) && (thisv->talkMode != 4)) {
                Flags_SetSwitch(globalCtx, thisv->switchFlag);
                // "(Forced) I saved it! All of it!"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ (強制)セーブしたよ！おもいっきり！ %x\n" VT_RST, thisv->switchFlag);
            }

            if (thisv->talkMode == 4) {
                thisv->unk_15A = true;
            }
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_4);
            func_8002DF54(globalCtx, NULL, 7);
            thisv->unk_156 = true;
            thisv->actionFunc = func_80B3A4F8;
            break;
    }
}

void func_80B3A4F8(EnWonderTalk2* thisv, GlobalContext* globalCtx) {
    Player* player;

    player = GET_PLAYER(globalCtx);
    thisv->unk_158++;
    if (thisv->switchFlag >= 0 && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if (!thisv->unk_15A) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->unk_15A = true;
        }
    } else if ((thisv->talkMode != 4) || !thisv->unk_15A) {
        if (BREG(2) != 0) {
            // "distance"
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ きょり %f\n" VT_RST, thisv->actor.xzDistToPlayer);
        }
        if (((thisv->actor.xzDistToPlayer < (40.0f + thisv->triggerRange)) &&
             (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 100.0f)) &&
            !Gameplay_InCsMode(globalCtx)) {
            if (thisv->unk_158 >= 2) {
                osSyncPrintf("\n\n");
                // "Transparent Message Kimi Seto"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 透明メッセージ君せっと %x\n" VT_RST, thisv->actor.params);
                // "Save Information"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ情報 \t           %x\n" VT_RST, thisv->switchFlag);
                // "Specified message type"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 指定メッセージ種類     %x\n" VT_RST, thisv->baseMsgId);
                // "Real message type"
                osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 実質メッセージ種類     %x\n" VT_RST, thisv->actor.textId);
                // "Specified range"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 指定範囲               %d\n" VT_RST, thisv->actor.world.rot.z);
                // "Processing range"
                osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 処理範囲               %f\n" VT_RST, thisv->triggerRange);
                // "What is your range?"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ レンジは？ \t\t   %d\n" VT_RST, thisv->actor.targetMode);
                osSyncPrintf("\n\n");
                osSyncPrintf("\n\n");
                switch (thisv->talkMode) {
                    case 1:
                        // "Compulsion"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆ 強制 ☆☆ \n" VT_RST);
                        break;
                    case 4:
                        // "Gerudo Training Grounds Forced Check Only"
                        osSyncPrintf(VT_FGCOL(RED) " ☆☆ ゲルドの修練場強制チェックのみ ☆☆ \n" VT_RST);
                        break;
                }

                osSyncPrintf("\n\n");
            }
            thisv->unk_158 = 0;
            if (!thisv->unk_156) {
                Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
                func_8002DF54(globalCtx, NULL, 8);
                thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_4;
                thisv->actionFunc = func_80B3A3D4;
            }

        } else {
            thisv->unk_156 = false;
        }
    }
}

void EnWonderTalk2_DoNothing(EnWonderTalk2* thisv, GlobalContext* globalCtx) {
}

void EnWonderTalk2_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnWonderTalk2* thisv = (EnWonderTalk2*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.world.pos.y = thisv->initPos.y;

    Actor_SetFocus(&thisv->actor, thisv->height);

    if (BREG(0) != 0) {
        if (thisv->unk_158 != 0) {
            if ((thisv->unk_158 & 1) == 0) {
                DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 70, 70, 70, 255, 4, globalCtx->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                   thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 0, 0, 255, 255, 4, globalCtx->state.gfxCtx);
        }
    }
}
