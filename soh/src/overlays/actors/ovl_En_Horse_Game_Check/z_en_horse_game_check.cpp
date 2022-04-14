/*
 * File: z_en_horse_game_check.c
 * Overlay: ovl_En_Horse_Game_Check
 * Description: Horseback Minigames
 */

#include "z_en_horse_game_check.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#define FLAGS ACTOR_FLAG_4

#define AT_FINISH_LINE(actor)                                                                                     \
    (Math3D_PointInSquare2D(sFinishLine[0], sFinishLine[1], sFinishLine[2], sFinishLine[3], (actor)->world.pos.x, \
                            (actor)->world.pos.z))
#define AT_RANCH_EXIT(actor)                                                                                  \
    (Math3D_PointInSquare2D(sRanchExit[0], sRanchExit[1], sRanchExit[2], sRanchExit[3], (actor)->world.pos.x, \
                            (actor)->world.pos.z))

#define INGORACE_PLAYER_MOVE (1 << 0)
#define INGORACE_SET_TIMER (1 << 1)
#define INGORACE_INGO_MOVE (1 << 2)

typedef enum {
    /* 0 */ INGORACE_NO_RESULT,
    /* 1 */ INGORACE_PLAYER_WIN,
    /* 2 */ INGORACE_INGO_WIN,
    /* 3 */ INGORACE_TIME_UP
} HorseGameIngoRaceResult;

#define MALONRACE_PLAYER_MOVE (1 << 0)
#define MALONRACE_SET_TIMER (1 << 1)
#define MALONRACE_SECOND_LAP (1 << 2)
#define MALONRACE_BROKE_RULE (1 << 3)
#define MALONRACE_START_SFX (1 << 4)
#define MALONRACE_PLAYER_START (1 << 5)
#define MALONRACE_PLAYER_ON_MARK (1 << 6)

typedef enum {
    /* 0 */ MALONRACE_NO_RESULT,
    /* 1 */ MALONRACE_SUCCESS,
    /* 2 */ MALONRACE_TIME_UP,
    /* 3 */ MALONRACE_UNUSED,
    /* 4 */ MALONRACE_FAILURE
} HorseGameMalonRaceResult;

void EnHorseGameCheck_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHorseGameCheck_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHorseGameCheck_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHorseGameCheck_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Horse_Game_Check_InitVars = {
    ACTOR_EN_HORSE_GAME_CHECK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnHorseGameCheck),
    (ActorFunc)EnHorseGameCheck_Init,
    (ActorFunc)EnHorseGameCheck_Destroy,
    (ActorFunc)EnHorseGameCheck_Update,
    (ActorFunc)EnHorseGameCheck_Draw,
    NULL,
};

static Vec3f sIngoRaceCheckpoints[] = {
    { 1700.0f, 1.0f, -540.0f },
    { 117.0f, 1.0f, 488.0f },
    { -1700.0f, 1.0f, -540.0f },
};

static f32 sFinishLine[4] = { -200.0f, 80.0f, -2300.0f, -1470.0f };

static f32 sRanchExit[4] = { 800.0f, 1000.0f, -2900.0f, -2700.0f };

static Vec3f sUnusedZeroVec = { 0.0f, 0.0f, 0.0f };

static Vec3f sFencePos[] = {
    { 820.0f, -44.0f, -1655.0f }, { 1497.0f, -21.0f, -1198.0f },  { 1655.0f, -44.0f, -396.0f },
    { 1291.0f, -44.0f, 205.0f },  { 379.0f, -21.0f, 455.0f },     { -95.0f, -21.0f, 455.0f },
    { -939.0f, 1.0f, 455.0f },    { -1644.0f, -21.0f, -1035.0f },
};

s32 EnHorseGameCheck_InitIngoRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckIngoRace* thisv = (EnHorseGameCheckIngoRace*)base;
    s32 i;

    thisv->base.type = HORSEGAME_INGO_RACE;
    thisv->startFlags = 0;
    for (i = 0; i < 3; i++) {
        thisv->playerCheck[i] = 0;
    }
    thisv->ingoHorse =
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_HORSE, -250.0f, 1.0f, -1650.0f, 0, 0x4000, 0, 0x8003);

    if (thisv->ingoHorse == NULL) {
        LogUtils_HungupThread("../z_en_horse_game_check.c", 385);
    }
    thisv->startTimer = 0;
    thisv->finishTimer = 0;
    thisv->result = INGORACE_NO_RESULT;
    thisv->playerFinish = 0;
    thisv->ingoFinish = 0;

    return true;
}

s32 EnHorseGameCheck_DestroyIngoRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    return true;
}

void EnHorseGameCheck_FinishIngoRace(EnHorseGameCheckIngoRace* thisv, GlobalContext* globalCtx) {
    gSaveContext.cutsceneIndex = 0;
    if (thisv->result == INGORACE_PLAYER_WIN) {
        globalCtx->nextEntranceIndex = 0x4CE;
        if (gSaveContext.eventInf[0] & 0x40) {
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | 6;
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
            globalCtx->fadeTransition = 3;
            Environment_ForcePlaySequence(NA_BGM_INGO);
        } else {
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | 4;
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
            Environment_ForcePlaySequence(NA_BGM_INGO);
            globalCtx->fadeTransition = 0x2E;
        }
    } else {
        globalCtx->nextEntranceIndex = 0x558;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | 3;
        globalCtx->fadeTransition = 0x20;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
    }
    DREG(25) = 0;
    globalCtx->sceneLoadFlag = 0x14;
    gSaveContext.timer1State = 0;
}

s32 EnHorseGameCheck_UpdateIngoRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckIngoRace* thisv = (EnHorseGameCheckIngoRace*)base;
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    EnHorse* ingoHorse;
    EnHorse* horse;

    if ((thisv->startTimer > 50) && !(thisv->startFlags & INGORACE_SET_TIMER)) {
        thisv->startFlags |= INGORACE_SET_TIMER;
        func_80088B34(0);
    } else if ((thisv->startTimer > 80) && (player->rideActor != NULL) && !(thisv->startFlags & INGORACE_PLAYER_MOVE)) {
        thisv->startFlags |= INGORACE_PLAYER_MOVE;
        horse = (EnHorse*)player->rideActor;
        horse->inRace = 1;
    } else if ((thisv->startTimer > 81) && !(thisv->startFlags & INGORACE_INGO_MOVE)) {
        ingoHorse = (EnHorse*)thisv->ingoHorse;

        ingoHorse->inRace = 1;
        thisv->startFlags |= INGORACE_INGO_MOVE;
        Audio_PlaySoundGeneral(NA_SE_SY_START_SHOT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    thisv->startTimer++;

    for (i = 0; i < 3; i++) {
        if ((player->rideActor != NULL) &&
            (Math3D_Vec3f_DistXYZ(&sIngoRaceCheckpoints[i], &player->rideActor->world.pos) < 400.0f)) {
            if ((i > 0) && (thisv->playerCheck[i - 1] == 1)) {
                thisv->playerCheck[i] = 1;
            } else if (i == 0) {
                thisv->playerCheck[i] = 1;
            }
        }
        if (Math3D_Vec3f_DistXYZ(&sIngoRaceCheckpoints[i], &thisv->ingoHorse->world.pos) < 400.0f) {
            if ((i > 0) && (thisv->ingoCheck[i - 1] == 1)) {
                thisv->ingoCheck[i] = 1;
            } else if (i == 0) {
                thisv->ingoCheck[i] = 1;
            }
        }
    }

    if (thisv->result == INGORACE_NO_RESULT) {
        Player* player2 = player;

        if ((player2->rideActor != NULL) && (thisv->playerCheck[2] == 1) && AT_FINISH_LINE(player2->rideActor)) {
            thisv->playerFinish++;
            if (thisv->playerFinish > 0) {
                thisv->result = INGORACE_PLAYER_WIN;
                thisv->finishTimer = 55;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
                Audio_PlaySoundGeneral(NA_SE_SY_START_SHOT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            for (i = 0; i < 3; i++) {
                thisv->playerCheck[i] = 0;
            }
        }
        if ((thisv->ingoHorse != NULL) && (thisv->ingoCheck[2] == 1) && AT_FINISH_LINE(thisv->ingoHorse)) {
            thisv->ingoFinish++;
            if (thisv->ingoFinish > 0) {
                ingoHorse = (EnHorse*)thisv->ingoHorse;

                thisv->result = INGORACE_INGO_WIN;
                thisv->finishTimer = 70;
                ingoHorse->stateFlags |= ENHORSE_INGO_WON;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
                Audio_PlaySoundGeneral(NA_SE_SY_START_SHOT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            for (i = 0; i < 3; i++) {
                thisv->ingoCheck[i] = 0;
            }
        }
        if (((player2->rideActor != NULL) && AT_RANCH_EXIT(player2->rideActor)) || AT_RANCH_EXIT(&player2->actor)) {
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
            thisv->result = INGORACE_INGO_WIN;
            thisv->finishTimer = 20;
        }
        if ((gSaveContext.timer1Value >= 180) && (thisv->startFlags & 2)) {
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
            thisv->result = INGORACE_TIME_UP;
            thisv->finishTimer = 20;
        }
    } else {
        if (thisv->finishTimer > 0) {
            thisv->finishTimer--;
        } else {
            EnHorseGameCheck_FinishIngoRace(thisv, globalCtx);
        }
    }
    return true;
}

s32 EnHorseGameCheck_InitGerudoArchery(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckGerudoArchery* thisv = (EnHorseGameCheckGerudoArchery*)base;

    thisv->base.type = HORSEGAME_GERUDO_ARCHERY;
    thisv->unk_150 = 0;
    thisv->startTimer = 0;
    return true;
}

s32 EnHorseGameCheck_DestroyGerudoArchery(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    return true;
}

s32 EnHorseGameCheck_UpdateGerudoArchery(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckGerudoArchery* thisv = (EnHorseGameCheckGerudoArchery*)base;
    Player* player = GET_PLAYER(globalCtx);
    EnHorse* horse = (EnHorse*)player->rideActor;

    if (horse == NULL) {
        return true;
    }

    if (thisv->startTimer > 90) {
        if (globalCtx) {}
        horse->hbaStarted = 1;
    }
    thisv->startTimer++;
    return true;
}

s32 EnHorseGameCheck_InitType3(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheck3* thisv = (EnHorseGameCheck3*)base;

    thisv->base.type = HORSEGAME_TYPE3;
    thisv->unk_150 = 0;
    return true;
}

s32 EnHorseGameCheck_DestroyType3(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    return true;
}

s32 EnHorseGameCheck_UpdateType3(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    return true;
}

s32 EnHorseGameCheck_InitMalonRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckMalonRace* thisv = (EnHorseGameCheckMalonRace*)base;
    s32 i;

    thisv->base.type = HORSEGAME_MALON_RACE;
    thisv->raceFlags = 0;
    thisv->finishTimer = 0;
    thisv->result = MALONRACE_NO_RESULT;
    for (i = 0; i < 16; i++) {
        thisv->fenceCheck[i] = 0;
    }
    thisv->lapCount = 0;
    return true;
}

s32 EnHorseGameCheck_DestroyMalonRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    return true;
}

void EnHorseGameCheck_FinishMalonRace(EnHorseGameCheckMalonRace* thisv, GlobalContext* globalCtx) {
    if ((thisv->result == MALONRACE_SUCCESS) || (thisv->result == MALONRACE_TIME_UP)) {
        gSaveContext.cutsceneIndex = 0;
        globalCtx->nextEntranceIndex = 0x4CE;
        globalCtx->fadeTransition = 0x2E;
        globalCtx->sceneLoadFlag = 0x14;
    } else if (thisv->result == MALONRACE_FAILURE) {
        gSaveContext.timer1Value = 240;
        gSaveContext.timer1State = 0xF;
        gSaveContext.cutsceneIndex = 0;
        globalCtx->nextEntranceIndex = 0x4CE;
        globalCtx->fadeTransition = 0x2E;
        globalCtx->sceneLoadFlag = 0x14;
    } else {
        // "not supported"
        osSyncPrintf("En_HGC_Spot20_Ta_end():対応せず\n");
        gSaveContext.cutsceneIndex = 0;
        globalCtx->nextEntranceIndex = 0x157;
        globalCtx->fadeTransition = 0x2E;
        globalCtx->sceneLoadFlag = 0x14;
    }
}

s32 EnHorseGameCheck_UpdateMalonRace(EnHorseGameCheckBase* base, GlobalContext* globalCtx) {
    EnHorseGameCheckMalonRace* thisv = (EnHorseGameCheckMalonRace*)base;
    s32 i;
    Player* player = GET_PLAYER(globalCtx);
    EnHorse* horse;

    if (!(thisv->raceFlags & MALONRACE_PLAYER_ON_MARK) && AT_FINISH_LINE(player->rideActor)) {
        thisv->raceFlags |= MALONRACE_PLAYER_ON_MARK;
    } else if ((thisv->raceFlags & MALONRACE_PLAYER_ON_MARK) && !(thisv->raceFlags & MALONRACE_PLAYER_START) &&
               !AT_FINISH_LINE(player->rideActor)) {
        thisv->raceFlags |= MALONRACE_PLAYER_START;
    }
    if ((thisv->startTimer > 50) && !(thisv->raceFlags & MALONRACE_SET_TIMER)) {
        thisv->raceFlags |= MALONRACE_SET_TIMER;
        func_80088B34(0);
    } else if ((thisv->startTimer > 80) && (player->rideActor != NULL) && !(thisv->raceFlags & MALONRACE_PLAYER_MOVE)) {
        thisv->raceFlags |= MALONRACE_PLAYER_MOVE;
        horse = (EnHorse*)player->rideActor;

        horse->inRace = 1;
    } else if ((thisv->startTimer > 81) && !(thisv->raceFlags & MALONRACE_START_SFX)) {
        thisv->raceFlags |= MALONRACE_START_SFX;
        Audio_PlaySoundGeneral(NA_SE_SY_START_SHOT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    thisv->startTimer++;
    if (thisv->result == MALONRACE_NO_RESULT) {
        Player* player2 = player;
        f32 dist;

        for (i = 0; i < 16; i++) {
            if ((thisv->lapCount == 0) && (i >= 8)) {
                break;
            }
            dist = Math_Vec3f_DistXZ(&sFencePos[i % 8], &player2->rideActor->world.pos);
            if ((player->rideActor != NULL) && (dist < 250.0f)) {
                horse = (EnHorse*)player2->rideActor;

                if (horse->stateFlags & ENHORSE_JUMPING) {
                    if ((i > 0) && (thisv->fenceCheck[i - 1] == 1)) {
                        thisv->fenceCheck[i] = 1;
                    } else if (i == 0) {
                        thisv->fenceCheck[i] = 1;
                    }

                    if ((thisv->fenceCheck[i - 1] == 0) && !(thisv->raceFlags & MALONRACE_BROKE_RULE)) {
                        thisv->raceFlags |= MALONRACE_BROKE_RULE;
                        Message_StartTextbox(globalCtx, 0x208C, NULL);
                        thisv->result = 4;
                        thisv->finishTimer = 30;
                    }
                }
            }
        }
        if ((player2->rideActor != NULL) && (thisv->raceFlags & MALONRACE_PLAYER_START) &&
            AT_FINISH_LINE(player2->rideActor)) {
            if ((thisv->lapCount == 1) && (thisv->fenceCheck[15] == 0) && (player2->rideActor->prevPos.x < -200.0f)) {
                thisv->raceFlags |= MALONRACE_BROKE_RULE;
                Message_StartTextbox(globalCtx, 0x208C, NULL);
                thisv->result = MALONRACE_FAILURE;
                thisv->finishTimer = 30;
            } else if (thisv->fenceCheck[15] == 1) {
                thisv->lapCount = 2;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
                Audio_PlaySoundGeneral(NA_SE_SY_START_SHOT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->result = MALONRACE_SUCCESS;
                thisv->finishTimer = 70;
                gSaveContext.timer1State = 0xF;
            } else if ((thisv->fenceCheck[7] == 1) && !(thisv->raceFlags & MALONRACE_SECOND_LAP)) {
                thisv->lapCount = 1;
                thisv->raceFlags |= MALONRACE_SECOND_LAP;
                Message_StartTextbox(globalCtx, 0x208D, NULL);
            } else if (thisv->fenceCheck[7] == 0) {
                thisv->raceFlags |= MALONRACE_BROKE_RULE;
                Message_StartTextbox(globalCtx, 0x208C, NULL);
                thisv->result = MALONRACE_FAILURE;
                thisv->finishTimer = 30;
            } else if (player2->rideActor->prevPos.x > 80.0f) {
                thisv->raceFlags |= MALONRACE_BROKE_RULE;
                Message_StartTextbox(globalCtx, 0x208C, NULL);
                thisv->result = MALONRACE_FAILURE;
                thisv->finishTimer = 30;
            }
        }
        if ((gSaveContext.timer1Value >= 180) && (thisv->raceFlags & MALONRACE_SET_TIMER)) {
            gSaveContext.timer1Value = 240;
            thisv->result = MALONRACE_TIME_UP;
            thisv->finishTimer = 30;
            gSaveContext.timer1State = 0;
        }
    } else {
        if (thisv->finishTimer > 0) {
            thisv->finishTimer--;
        } else {
            EnHorseGameCheck_FinishMalonRace(thisv, globalCtx);
        }
    }
    return true;
}

static EnHorseGameCheckFunc sInitFuncs[] = {
    NULL,
    EnHorseGameCheck_InitIngoRace,
    EnHorseGameCheck_InitGerudoArchery,
    EnHorseGameCheck_InitType3,
    EnHorseGameCheck_InitMalonRace,
};

static EnHorseGameCheckFunc sDestroyFuncs[] = {
    NULL,
    EnHorseGameCheck_DestroyIngoRace,
    EnHorseGameCheck_DestroyGerudoArchery,
    EnHorseGameCheck_DestroyType3,
    EnHorseGameCheck_DestroyMalonRace,
};

static EnHorseGameCheckFunc sUpdateFuncs[] = {
    NULL,
    EnHorseGameCheck_UpdateIngoRace,
    EnHorseGameCheck_UpdateGerudoArchery,
    EnHorseGameCheck_UpdateType3,
    EnHorseGameCheck_UpdateMalonRace,
};

void EnHorseGameCheck_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHorseGameCheckBase* thisv = (EnHorseGameCheckBase*)thisx;

    if ((globalCtx->sceneNum == SCENE_SPOT20) && (Flags_GetEventChkInf(0x18) || DREG(1))) {
        thisv->actor.params = HORSEGAME_MALON_RACE;
    }
    if (sInitFuncs[thisv->actor.params] != NULL) {
        sInitFuncs[thisv->actor.params](thisv, globalCtx);
    }
}

void EnHorseGameCheck_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHorseGameCheckBase* thisv = (EnHorseGameCheckBase*)thisx;

    if (sDestroyFuncs[thisv->actor.params] != NULL) {
        sDestroyFuncs[thisv->actor.params](thisv, globalCtx);
    }
}

void EnHorseGameCheck_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHorseGameCheckBase* thisv = (EnHorseGameCheckBase*)thisx;

    if (sUpdateFuncs[thisv->type] != NULL) {
        sUpdateFuncs[thisv->type](thisv, globalCtx);
    }
}

void EnHorseGameCheck_Draw(Actor* thisx, GlobalContext* globalCtx) {
}
