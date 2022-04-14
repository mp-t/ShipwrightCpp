/*
 * File: z_en_wonder_talk.c
 * Overlay: ovl_En_Wonder_Talk
 * Description: Checkable spot (Green Navi)
 */

#include "z_en_wonder_talk.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_27)

void EnWonderTalk_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWonderTalk_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWonderTalk_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B391CC(EnWonderTalk* thisv, GlobalContext* globalCtx);
void func_80B395F0(EnWonderTalk* thisv, GlobalContext* globalCtx);
void func_80B3943C(EnWonderTalk* thisv, GlobalContext* globalCtx);

const ActorInit En_Wonder_Talk_InitVars = {
    ACTOR_EN_WONDER_TALK,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWonderTalk),
    (ActorFunc)EnWonderTalk_Init,
    (ActorFunc)EnWonderTalk_Destroy,
    (ActorFunc)EnWonderTalk_Update,
    NULL,
    NULL,
};

void EnWonderTalk_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnWonderTalk_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnWonderTalk* thisv = (EnWonderTalk*)thisx;

    osSyncPrintf("\n\n");
    // "Special conversation"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 特殊会話くん ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params);

    thisv->unk_150 = (thisv->actor.params >> 0xB) & 0x1F;
    thisv->unk_152 = (thisv->actor.params >> 6) & 0x1F;
    thisv->switchFlag = thisv->actor.params & 0x3F;
    if (thisv->switchFlag == 0x3F) {
        thisv->switchFlag = -1;
    }
    thisv->actor.targetMode = 1;
    if (thisv->switchFlag >= 0) {
        if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ Ｙｏｕ ａｒｅ Ｓｈｏｃｋ！  ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
            Actor_Kill(&thisv->actor);
            return;
        }
    }
    thisv->actionFunc = func_80B391CC;
    thisv->unk_15C = 40.0f;
}

void func_80B391CC(EnWonderTalk* thisv, GlobalContext* globalCtx) {
    if (thisv->switchFlag < 0 || !Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        switch (thisv->unk_150) {
            case 1:
                // "Slate GO!"
                osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 石板ＧＯ！ ☆☆☆☆☆ \n" VT_RST);
                thisv->height = 0.0f;
                thisv->unk_15C = 80.0f;
                // "Attention coordinates"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 注目座標\t       \t☆☆☆☆☆ %f\n" VT_RST, 0.0f);
                if (!LINK_IS_ADULT) {
                    thisv->actor.textId = 0x7040;
                    // "Children"
                    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ こども ☆☆☆☆☆ \n" VT_RST);
                } else {
                    // "Adult"
                    osSyncPrintf(VT_FGCOL(CYAN) " ☆☆☆☆☆ おとな ☆☆☆☆☆ \n" VT_RST);
                    thisv->actor.textId = 0x7088;
                }

                thisv->unk_156 = TEXT_STATE_EVENT;
                osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ thisv->actor.talk_message    ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.textId);
                break;
            case 2:
                // "Diary start!"
                osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 日記帳スタート！ ☆☆☆☆☆ \n" VT_RST);
                thisv->actor.textId = 0x5002;
                thisv->unk_156 = TEXT_STATE_CHOICE;
                thisv->height = 30.0f;
                thisv->unk_15C = 40.0f;
                // "Attention coordinates"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 注目座標\t       \t☆☆☆☆☆ %f\n" VT_RST, 30.0f);
                break;
            case 3:
                thisv->actor.textId = 0x501E;
                thisv->unk_156 = TEXT_STATE_EVENT;
                thisv->height = 0.0f;
                thisv->unk_15C = 110.0f;
                // "Attention coordinates"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 注目座標\t       \t☆☆☆☆☆ %f\n" VT_RST, 0.0f);
                break;
            case 4:
                thisv->actor.textId = 0x5020;
                thisv->unk_156 = TEXT_STATE_DONE;
                thisv->height = 0.0f;
                // "Attention coordinates"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 注目座標\t       \t☆☆☆☆☆ %f\n" VT_RST, 0.0f);
                thisv->unk_15C = 120.0f;
                if (gSaveContext.eventChkInf[1] & 0x2000) {
                    Actor_Kill(&thisv->actor);
                }
                break;
            case 5:
                thisv->actor.textId = 0x501F;
                thisv->unk_156 = TEXT_STATE_EVENT;
                thisv->height = 0.0f;
                thisv->unk_15C = 110.0f;
                // "Attention coordinates"
                osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 注目座標\t       \t☆☆☆☆☆ %f\n" VT_RST, 0.0f);
                break;
            default:
                thisv->actor.textId = 0x7072;
                thisv->unk_156 = TEXT_STATE_EVENT;
                break;
        }

        osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ thisv->actor.talk_message    ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.textId);
        thisv->actionFunc = func_80B3943C;
    }
}

void func_80B3943C(EnWonderTalk* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    s16 yawDiffTemp;

    thisv->unk_15A++;
    if (thisv->unk_150 == 4 && (gSaveContext.eventChkInf[1] & 0x2000)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->switchFlag < 0 || !Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if ((Actor_ProcessTalkRequest(&thisv->actor, globalCtx))) {
            if (thisv->unk_156 != TEXT_STATE_DONE) {
                thisv->actionFunc = func_80B395F0;
            } else {
                if (thisv->switchFlag >= 0) {
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                    Flags_SetSwitch(globalCtx, thisv->switchFlag);
                }
                thisv->actionFunc = func_80B391CC;
            }
        } else if (!(thisv->unk_15C < thisv->actor.xzDistToPlayer)) {
            yawDiffTemp = (thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y);
            yawDiff = ABS(yawDiffTemp);

            if (yawDiff < 0x4000) {
                if (thisv->unk_15A >= 2) {
                    osSyncPrintf("\n\n");
                    // "Save information"
                    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ セーブ情報\t\t☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
                    // "Type index"
                    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 種類インデックス\t☆☆☆☆☆ %d\n" VT_RST, thisv->unk_150);
                    // "Actual message type"
                    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 実質メッセージ種類     %x\n" VT_RST, thisv->actor.textId);
                    // "Specified range"
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 指定範囲               %d\n" VT_RST, thisv->actor.world.rot.z);
                    osSyncPrintf("\n\n");
                }
                thisv->unk_15A = 0;
                func_8002F2CC(&thisv->actor, globalCtx, thisv->unk_15C);
            }
        }
    }
}

void func_80B395F0(EnWonderTalk* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_156 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->switchFlag >= 0) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }
        switch (thisv->unk_150) {
            case 1:
                Message_CloseTextbox(globalCtx);
                thisv->actionFunc = func_80B391CC;
                break;
            case 2:
                switch (globalCtx->msgCtx.choiceIndex) {
                    case 0:
                        if (!LINK_IS_ADULT) {
                            // "I'm still a child!"
                            osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ まだコドモなの！ ☆☆☆☆☆ \n" VT_RST);
                            thisv->actor.textId = 0x5001;
                        } else {
                            // "I'm an adult. .. .."
                            osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ アダルトなの。。。 ☆☆☆☆☆ \n" VT_RST);
                            thisv->actor.textId = 0x5003;
                        }
                        break;
                    case 1:
                        // "Out!"
                        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ はずれ！ ☆☆☆☆☆ \n" VT_RST);
                        thisv->actor.textId = 0x5004;
                        break;
                }

                thisv->unk_156 = TEXT_STATE_DONE;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->actionFunc = func_80B391CC;
                break;
            case 3:
                Message_CloseTextbox(globalCtx);
                if (thisv->unk_164 == 0) {
                    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_POH, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 2);
                    thisv->unk_164 = 1;
                }

                thisv->actionFunc = func_80B391CC;
                break;
            case 5:
                Message_CloseTextbox(globalCtx);
                if (thisv->unk_164 == 0) {
                    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_POH, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 3);
                    thisv->unk_164 = 1;
                }
                thisv->actionFunc = func_80B391CC;
                break;
        }
    }
}

void EnWonderTalk_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnWonderTalk* thisv = (EnWonderTalk*)thisx;

    if (thisv->unk_158 != 0) {
        thisv->unk_158--;
    }
    thisv->actionFunc(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, thisv->height);

    if (BREG(0) != 0) {
        if (thisv->unk_15A != 0) {
            if ((thisv->unk_15A & 1) == 0) {
                DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 10, 10, 10, 255, 4, globalCtx->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                   thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 0, 255, 0, 255, 4, globalCtx->state.gfxCtx);
        }
    }
}
