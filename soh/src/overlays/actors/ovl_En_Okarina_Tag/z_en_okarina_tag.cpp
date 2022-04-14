/*
 * File: z_en_okarina_tag.c
 * Overlay: ovl_En_Okarina_Tag
 * Description: Music Staff (Ocarina) spot
 */

#include "z_en_okarina_tag.h"
#include "scenes/misc/hakaana_ouke/hakaana_ouke_scene.h"
#include "scenes/overworld/spot02/spot02_scene.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnOkarinaTag_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaTag_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaTag_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80ABEF2C(EnOkarinaTag* thisv, GlobalContext* globalCtx);
void func_80ABF708(EnOkarinaTag* thisv, GlobalContext* globalCtx);
void func_80ABF28C(EnOkarinaTag* thisv, GlobalContext* globalCtx);
void func_80ABF0CC(EnOkarinaTag* thisv, GlobalContext* globalCtx);
void func_80ABF4C8(EnOkarinaTag* thisv, GlobalContext* globalCtx);
void func_80ABF7CC(EnOkarinaTag* thisv, GlobalContext* globalCtx);

const ActorInit En_Okarina_Tag_InitVars = {
    ACTOR_EN_OKARINA_TAG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnOkarinaTag),
    (ActorFunc)EnOkarinaTag_Init,
    (ActorFunc)EnOkarinaTag_Destroy,
    (ActorFunc)EnOkarinaTag_Update,
    NULL,
    NULL,
};

extern CutsceneData D_80ABF9D0[];
extern CutsceneData D_80ABFB40[];

void EnOkarinaTag_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnOkarinaTag_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaTag* thisv = (EnOkarinaTag*)thisx;

    osSyncPrintf("\n\n");
    // "Ocarina tag outbreak"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナタグ発生 ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->type = (thisv->actor.params >> 0xA) & 0x3F;
    thisv->ocarinaSong = (thisv->actor.params >> 6) & 0xF;
    thisv->switchFlag = thisv->actor.params & 0x3F;
    if (thisv->switchFlag == 0x3F) {
        thisv->switchFlag = -1;
    }
    if (thisv->ocarinaSong == 0xF) {
        thisv->ocarinaSong = 0;
        thisv->unk_158 = 1;
    }
    thisv->actor.targetMode = 1;
    if (thisv->actor.world.rot.z > 0) {
        thisv->interactRange = thisv->actor.world.rot.z * 40.0f;
    }

    // "Save information"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ セーブ情報\t ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
    // "Type index"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 種類インデックス ☆☆☆☆☆ %d\n" VT_RST, thisv->type);
    // "Correct answer information"
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 正解情報\t ☆☆☆☆☆ %d\n" VT_RST, thisv->ocarinaSong);
    // "Range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 範囲情報\t ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.world.rot.z);
    // "Processing range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 処理範囲情報\t ☆☆☆☆☆ %f\n" VT_RST, thisv->interactRange);
    // "Hit?"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 当り？\t\t ☆☆☆☆☆ %d\n" VT_RST, thisv->unk_158);
    osSyncPrintf("\n\n");

    if ((thisv->switchFlag >= 0) && (Flags_GetSwitch(globalCtx, thisv->switchFlag))) {
        Actor_Kill(&thisv->actor);
    } else {
        switch (thisv->type) {
            case 7:
                thisv->actionFunc = func_80ABEF2C;
                break;
            case 2:
                if (LINK_IS_ADULT) {
                    Actor_Kill(&thisv->actor);
                    break;
                }
            case 1:
            case 4:
            case 6:
                thisv->actionFunc = func_80ABF28C;
                break;
            case 5:
                thisv->actor.textId = 0x5021;
                thisv->actionFunc = func_80ABF708;
                break;
            default:
                Actor_Kill(&thisv->actor);
                break;
        }
    }
}

void func_80ABEF2C(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    Player* player;
    u16 ocarinaSong;

    player = GET_PLAYER(globalCtx);
    thisv->unk_15A++;
    if ((thisv->switchFlag >= 0) && (Flags_GetSwitch(globalCtx, thisv->switchFlag))) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        if ((thisv->ocarinaSong != 6) || (gSaveContext.scarecrowSpawnSongSet)) {
            if (player->stateFlags2 & 0x1000000) {
                // "North! ! ! ! !"
                osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ 北！！！！！ ☆☆☆☆☆ %f\n" VT_RST, thisv->actor.xzDistToPlayer);
            }
            if ((thisv->actor.xzDistToPlayer < (90.0f + thisv->interactRange)) &&
                (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 80.0f)) {
                if (player->stateFlags2 & 0x2000000) {
                    ocarinaSong = thisv->ocarinaSong;
                    if (ocarinaSong == 6) {
                        ocarinaSong = 0xA;
                    }
                    player->stateFlags2 |= 0x800000;
                    func_8010BD58(globalCtx, ocarinaSong + OCARINA_ACTION_CHECK_SARIA);
                    thisv->actionFunc = func_80ABF0CC;
                } else if ((thisv->actor.xzDistToPlayer < (50.0f + thisv->interactRange) &&
                            ((fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 40.0f)))) {
                    thisv->unk_15A = 0;
                    player->unk_6A8 = &thisv->actor;
                }
            }
        }
    }
}

void func_80ABF0CC(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        thisv->actionFunc = func_80ABEF2C;
    } else {
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (thisv->switchFlag >= 0) {
                Flags_SetSwitch(globalCtx, thisv->switchFlag);
            }
            if (globalCtx->sceneNum == SCENE_MIZUSIN) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
            }
            if ((globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) && (globalCtx->sceneNum != SCENE_YOUSEI_IZUMI_YOKO)) {
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            }
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            thisv->actionFunc = func_80ABEF2C;
            return;
        }
        if (thisv->unk_158 != 0) {
            if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_05) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_06) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_07) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_08) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_09) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0A) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0D)) {
                if (thisv->switchFlag >= 0) {
                    Flags_SetSwitch(globalCtx, thisv->switchFlag);
                }
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                thisv->actionFunc = func_80ABEF2C;
                return;
            }
        }
        if ((globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05) && (globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0E)) {
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            thisv->actionFunc = func_80ABEF2C;
        } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80ABF28C(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_15A++;
    if ((thisv->ocarinaSong != 6) || (gSaveContext.scarecrowSpawnSongSet)) {
        if ((thisv->switchFlag >= 0) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
        } else if (((thisv->type != 4) || !(gSaveContext.eventChkInf[4] & 0x800)) &&
                   ((thisv->type != 6) || !(gSaveContext.eventChkInf[1] & 0x2000)) &&
                   (thisv->actor.xzDistToPlayer < (90.0f + thisv->interactRange)) &&
                   (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 80.0f)) {
            if (player->stateFlags2 & 0x1000000) {
                switch (thisv->type) {
                    case 1:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    case 2:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_STORMS);
                        break;
                    case 4:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_TIME);
                        break;
                    case 6:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    default:
                        // "Ocarina Invisible-kun demo start check error source"
                        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナ透明君デモ開始チェックエラー原 ☆☆☆☆☆ %d\n" VT_RST,
                                     thisv->type);
                        Actor_Kill(&thisv->actor);
                        break;
                }
                player->stateFlags2 |= 0x800000;
                thisv->actionFunc = func_80ABF4C8;
            } else if ((thisv->actor.xzDistToPlayer < (50.0f + thisv->interactRange)) &&
                       (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 40.0f)) {
                thisv->unk_15A = 0;
                player->stateFlags2 |= 0x800000;
            }
        }
    }
}

void func_80ABF4C8(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        thisv->actionFunc = func_80ABF28C;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        if (thisv->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }
        switch (thisv->type) {
            case 1:
                Flags_SetSwitch(globalCtx, thisv->switchFlag);
                gSaveContext.eventChkInf[3] |= 0x200;
                break;
            case 2:
                globalCtx->csCtx.segment = D_80ABF9D0;
                gSaveContext.cutsceneTrigger = 1;
                func_800F574C(1.18921f, 0x5A);
                break;
            case 4:
                globalCtx->csCtx.segment = D_80ABFB40;
                gSaveContext.cutsceneTrigger = 1;
                break;
            case 6:
                globalCtx->csCtx.segment = LINK_IS_ADULT ? SEGMENTED_TO_VIRTUAL(&spot02_scene_Cs_003C80)
                                                         : SEGMENTED_TO_VIRTUAL(&spot02_scene_Cs_005020);
                gSaveContext.cutsceneTrigger = 1;
                gSaveContext.eventChkInf[1] |= 0x2000;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                break;
            default:
                break;
        }
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        thisv->actionFunc = func_80ABF28C;
    } else {
        if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
            if (globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0E) {
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                thisv->actionFunc = func_80ABF28C;
                return;
            }
        }
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80ABF708(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    s16 yawDiffNew;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80ABF7CC;
    } else {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y;
        thisv->unk_15A++;
        if (!(thisv->actor.xzDistToPlayer > 120.0f)) {
            if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
                thisv->actor.textId = 0x5021;
            }
            yawDiffNew = ABS(yawDiff);
            if (yawDiffNew < 0x4300) {
                thisv->unk_15A = 0;
                func_8002F2CC(&thisv->actor, globalCtx, 70.0f);
            }
        }
    }
}

void func_80ABF7CC(EnOkarinaTag* thisv, GlobalContext* globalCtx) {
    // "Open sesame sesame!"
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 開けゴマゴマゴマ！ ☆☆☆☆☆ %d\n" VT_RST, Message_GetState(&globalCtx->msgCtx));

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        if (!CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(&gSunSongGraveSunSongTeachCs);
            gSaveContext.cutsceneTrigger = 1;
        }
        thisv->actionFunc = func_80ABF708;
    }
}

void EnOkarinaTag_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaTag* thisv = (EnOkarinaTag*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (BREG(0) != 0) {
        if (thisv->unk_15A != 0) {
            if (!(thisv->unk_15A & 1)) {
                DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 120, 255, 4, globalCtx->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                   thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
        }
    }
}
