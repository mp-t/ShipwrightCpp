/*
 * File: z_en_diving_game.c
 * Overlay: ovl_En_Diving_Game
 * Description: Diving minigame
 */

#include "z_en_diving_game.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "objects/object_zo/object_zo.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnDivingGame_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDivingGame_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDivingGame_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDivingGame_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809EDCB0(EnDivingGame* thisv, GlobalContext* globalCtx);
void EnDivingGame_Talk(EnDivingGame* thisv, GlobalContext* globalCtx);
void EnDivingGame_HandlePlayChoice(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE048(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE0FC(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE194(EnDivingGame* thisv, GlobalContext* globalCtx);
void EnDivingGame_SetupRupeeThrow(EnDivingGame* thisv, GlobalContext* globalCtx);
void EnDivingGame_RupeeThrow(EnDivingGame* thisv, GlobalContext* globalCtx);
void EnDivingGame_SetupUnderwaterViewCs(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE780(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE800(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE8F0(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EE96C(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EEA00(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EEA90(EnDivingGame* thisv, GlobalContext* globalCtx);
void func_809EEAF8(EnDivingGame* thisv, GlobalContext* globalCtx);

ActorInit En_Diving_Game_InitVars = {
    ACTOR_EN_DIVING_GAME,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZO,
    sizeof(EnDivingGame),
    (ActorFunc)EnDivingGame_Init,
    (ActorFunc)EnDivingGame_Destroy,
    (ActorFunc)EnDivingGame_Update,
    (ActorFunc)EnDivingGame_Draw,
    NULL,
};

// used to ensure there's only one instance of thisv actor.
static u8 sHasSpawned = false;

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

static const void* sEyeTextures[] = {
    gZoraEyeOpenTex,
    gZoraEyeHalfTex,
    gZoraEyeClosedTex,
};

void EnDivingGame_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDivingGame* thisv = (EnDivingGame*)thisx;

    thisv->actor.gravity = -3.0f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gZoraSkel, &gZoraIdleAnim, thisv->jointTable, thisv->morphTable, 20);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 素もぐりＧＯ ☆☆☆☆☆ \n" VT_RST);
    thisv->actor.room = -1;
    thisv->actor.scale.x = 0.01f;
    thisv->actor.scale.y = 0.012999999f;
    thisv->actor.scale.z = 0.0139999995f;
    if (sHasSpawned) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST);
        thisv->unk_31F = 1;
        Actor_Kill(&thisv->actor);
    } else {
        sHasSpawned = true;
        thisv->actor.targetMode = 0;
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        thisv->actionFunc = func_809EDCB0;
    }
}

void EnDivingGame_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDivingGame* thisv = (EnDivingGame*)thisx;

    if (thisv->unk_31F == 0) {
        sHasSpawned = false;
        gSaveContext.timer1State = 0;
    }
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnDivingGame_SpawnRuppy(EnDivingGame* thisv, GlobalContext* globalCtx) {
    EnExRuppy* rupee;
    Vec3f rupeePos;

    rupeePos.x = (Rand_ZeroOne() - 0.5f) * 30.0f + thisv->actor.world.pos.x;
    rupeePos.y = (Rand_ZeroOne() - 0.5f) * 20.0f + (thisv->actor.world.pos.y + 30.0f);
    rupeePos.z = (Rand_ZeroOne() - 0.5f) * 20.0f + thisv->actor.world.pos.z;
    rupee = (EnExRuppy*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_RUPPY, rupeePos.x,
                                           rupeePos.y, rupeePos.z, 0, (s16)Rand_CenteredFloat(3500.0f) - 1000,
                                           thisv->rupeesLeftToThrow, 0);
    if (rupee != NULL) {
        rupee->actor.speedXZ = 12.0f;
        rupee->actor.velocity.y = 6.0f;
    }
}

s32 EnDivingGame_HasMinigameFinished(EnDivingGame* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.timer1State == 10 && !Gameplay_InCsMode(globalCtx)) {
        // Failed.
        gSaveContext.timer1State = 0;
        func_800F5B58();
        func_80078884(NA_SE_SY_FOUND);
        thisv->actor.textId = 0x71AD;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        thisv->unk_292 = TEXT_STATE_EVENT;
        thisv->allRupeesThrown = thisv->state = thisv->phase = thisv->unk_2A2 = thisv->grabbedRupeesCounter = 0;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = func_809EE048;
        return true;
    } else {
        s32 rupeesNeeded = 5;

        if (gSaveContext.eventChkInf[3] & 0x100) {
            rupeesNeeded = 10;
        }
        if (thisv->grabbedRupeesCounter >= rupeesNeeded) {
            // Won.
            gSaveContext.timer1State = 0;
            thisv->allRupeesThrown = thisv->state = thisv->phase = thisv->unk_2A2 = thisv->grabbedRupeesCounter = 0;
            if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                thisv->actor.textId = 0x4055;
            } else {
                thisv->actor.textId = 0x405D;
                if (thisv->extraWinCount < 100) {
                    thisv->extraWinCount++;
                }
            }
            Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
            thisv->unk_292 = TEXT_STATE_EVENT;
            func_800F5B58();
            Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
            func_8002DF54(globalCtx, NULL, 8);
            if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                thisv->actionFunc = func_809EE96C;
            } else {
                thisv->actionFunc = func_809EE048;
            }
            return true;
        }
    }
    return false;
}

// EnDivingGame_FinishMinigame ? // Reset probably
void func_809EDCB0(EnDivingGame* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gZoraIdleAnim);

    Animation_Change(&thisv->skelAnime, &gZoraIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->notPlayingMinigame = true;
    thisv->actionFunc = EnDivingGame_Talk;
}

void EnDivingGame_Talk(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->state != ENDIVINGGAME_STATE_PLAYING || !EnDivingGame_HasMinigameFinished(thisv, globalCtx)) {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            if (thisv->unk_292 != TEXT_STATE_DONE) {
                switch (thisv->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        func_8002DF54(globalCtx, NULL, 8);
                        thisv->actionFunc = EnDivingGame_HandlePlayChoice;
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        thisv->actionFunc = func_809EEA00;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        thisv->actionFunc = func_809EE8F0;
                        break;
                }
            }
        } else {
            if (Text_GetFaceReaction(globalCtx, 0x1D) != 0) {
                thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0x1D);
                thisv->unk_292 = TEXT_STATE_DONE;
            } else {
                switch (thisv->state) {
                    case ENDIVINGGAME_STATE_NOTPLAYING:
                        thisv->unk_292 = TEXT_STATE_CHOICE;
                        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                            thisv->actor.textId = 0x4053;
                            thisv->phase = ENDIVINGGAME_PHASE_1;
                        } else {
                            thisv->actor.textId = 0x405C;
                            thisv->phase = ENDIVINGGAME_PHASE_2;
                        }
                        break;
                    case ENDIVINGGAME_STATE_AWARDPRIZE:
                        thisv->actor.textId = 0x4056;
                        thisv->unk_292 = TEXT_STATE_EVENT;
                        break;
                    case ENDIVINGGAME_STATE_PLAYING:
                        thisv->actor.textId = 0x405B;
                        thisv->unk_292 = TEXT_STATE_EVENT;
                        break;
                }
            }
            func_8002F2CC(&thisv->actor, globalCtx, 80.0f);
        }
    }
}

void EnDivingGame_HandlePlayChoice(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) &&
        Message_ShouldAdvance(globalCtx)) { // Did player selected an answer?
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 20) {
                    Rupees_ChangeBy(-20);
                    thisv->actor.textId = 0x4054;
                } else {
                    thisv->actor.textId = 0x85;
                    thisv->allRupeesThrown = thisv->state = thisv->phase = thisv->unk_2A2 = thisv->grabbedRupeesCounter = 0;
                }
                break;
            case 1: // No
                thisv->actor.textId = 0x2D;
                thisv->allRupeesThrown = thisv->state = thisv->phase = thisv->unk_2A2 = thisv->grabbedRupeesCounter = 0;
                break;
        }
        if (!(gSaveContext.eventChkInf[3] & 0x100) || thisv->actor.textId == 0x85 || thisv->actor.textId == 0x2D) {
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            thisv->unk_292 = TEXT_STATE_EVENT;
            thisv->actionFunc = func_809EE048;
        } else {
            globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
            func_8002DF54(globalCtx, NULL, 8);
            thisv->actionFunc = func_809EE0FC;
        }
    }
}

// Waits for the message to close
void func_809EE048(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->phase == ENDIVINGGAME_PHASE_ENDED) {
            Message_CloseTextbox(globalCtx);
            func_8002DF54(globalCtx, NULL, 7);
            thisv->actionFunc = func_809EDCB0;
        } else {
            globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
            func_8002DF54(globalCtx, NULL, 8);
            thisv->actionFunc = func_809EE0FC;
        }
    }
}

// another "start minigame" step
void func_809EE0FC(EnDivingGame* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gZoraThrowRupeesAnim);

    Animation_Change(&thisv->skelAnime, &gZoraThrowRupeesAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    thisv->notPlayingMinigame = false;
    thisv->actionFunc = func_809EE194;
}

// Wait a bit before start throwing the rupees.
void func_809EE194(EnDivingGame* thisv, GlobalContext* globalCtx) {
    f32 currentFrame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (currentFrame >= 15.0f) {
        thisv->actionFunc = EnDivingGame_SetupRupeeThrow;
    }
}

void EnDivingGame_SetupRupeeThrow(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    thisv->subCamId = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, 0, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->subCamId, CAM_STAT_ACTIVE);
    thisv->spawnRuppyTimer = 10;
    thisv->unk_2F4.x = -210.0f;
    thisv->unk_2F4.y = -80.0f;
    thisv->unk_2F4.z = -1020.0f;
    thisv->unk_2D0.x = -280.0f;
    thisv->unk_2D0.y = -20.0f;
    thisv->unk_2D0.z = -240.0f;
    if (!(gSaveContext.eventChkInf[3] & 0x100)) {
        thisv->rupeesLeftToThrow = 5;
    } else {
        thisv->rupeesLeftToThrow = 10;
    }
    thisv->unk_2DC.x = thisv->unk_2DC.y = thisv->unk_2DC.z = thisv->unk_300.x = thisv->unk_300.y = thisv->unk_300.z = 0.1f;
    thisv->camLookAt.x = globalCtx->view.lookAt.x;
    thisv->camLookAt.y = globalCtx->view.lookAt.y;
    thisv->camLookAt.z = globalCtx->view.lookAt.z;
    thisv->camEye.x = globalCtx->view.eye.x;
    thisv->camEye.y = globalCtx->view.eye.y + 80.0f;
    thisv->camEye.z = globalCtx->view.eye.z + 250.0f;
    thisv->unk_2E8.x = fabsf(thisv->camEye.x - thisv->unk_2D0.x) * 0.04f;
    thisv->unk_2E8.y = fabsf(thisv->camEye.y - thisv->unk_2D0.y) * 0.04f;
    thisv->unk_2E8.z = fabsf(thisv->camEye.z - thisv->unk_2D0.z) * 0.04f;
    thisv->unk_30C.x = fabsf(thisv->camLookAt.x - thisv->unk_2F4.x) * 0.04f;
    thisv->unk_30C.y = fabsf(thisv->camLookAt.y - thisv->unk_2F4.y) * 0.04f;
    thisv->unk_30C.z = fabsf(thisv->camLookAt.z - thisv->unk_2F4.z) * 0.04f;
    Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->camLookAt, &thisv->camEye);
    Gameplay_CameraSetFov(globalCtx, thisv->subCamId, globalCtx->mainCamera.fov);
    thisv->csCameraTimer = 60;
    thisv->actionFunc = EnDivingGame_RupeeThrow;
    thisv->unk_318 = 0.0f;
}

// Throws rupee when thisv->spawnRuppyTimer == 0
void EnDivingGame_RupeeThrow(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (func_800C0DB4(globalCtx, &thisv->actor.projectedPos)) {
        Audio_SetExtraFilter(0);
    }
    if (thisv->subCamId != 0) {
        Math_ApproachF(&thisv->camEye.x, thisv->unk_2D0.x, thisv->unk_2DC.x, thisv->unk_2E8.x * thisv->unk_318);
        Math_ApproachF(&thisv->camEye.z, thisv->unk_2D0.z, thisv->unk_2DC.z, thisv->unk_2E8.z * thisv->unk_318);
        Math_ApproachF(&thisv->camLookAt.x, thisv->unk_2F4.x, thisv->unk_300.x, thisv->unk_30C.x * thisv->unk_318);
        Math_ApproachF(&thisv->camLookAt.y, thisv->unk_2F4.y, thisv->unk_300.y, thisv->unk_30C.y * thisv->unk_318);
        Math_ApproachF(&thisv->camLookAt.z, thisv->unk_2F4.z, thisv->unk_300.z, thisv->unk_30C.z * thisv->unk_318);
        Math_ApproachF(&thisv->unk_318, 1.0f, 1.0f, 0.02f);
    }
    Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->camLookAt, &thisv->camEye);
    if (!thisv->allRupeesThrown && thisv->spawnRuppyTimer == 0) {
        thisv->spawnRuppyTimer = 5;
        EnDivingGame_SpawnRuppy(thisv, globalCtx);
        thisv->rupeesLeftToThrow--;
        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
            thisv->unk_296 = 30;
        } else {
            thisv->unk_296 = 5;
        }
        if (thisv->rupeesLeftToThrow <= 0) {
            thisv->rupeesLeftToThrow = 0;
            thisv->allRupeesThrown = true;
        }
    }
    if (thisv->csCameraTimer == 0 ||
        ((fabsf(thisv->camEye.x - thisv->unk_2D0.x) < 2.0f) && (fabsf(thisv->camEye.y - thisv->unk_2D0.y) < 2.0f) &&
         (fabsf(thisv->camEye.z - thisv->unk_2D0.z) < 2.0f) && (fabsf(thisv->camLookAt.x - thisv->unk_2F4.x) < 2.0f) &&
         (fabsf(thisv->camLookAt.y - thisv->unk_2F4.y) < 2.0f) && (fabsf(thisv->camLookAt.z - thisv->unk_2F4.z) < 2.0f))) {
        if (thisv->unk_2A2 != 0) {
            thisv->csCameraTimer = 70;
            thisv->unk_2A2 = 2;
            thisv->actionFunc = func_809EE780;
        } else {
            thisv->actionFunc = EnDivingGame_SetupUnderwaterViewCs;
        }
    }
}

// Called just before changing the camera to focus the underwater rupees.
void EnDivingGame_SetupUnderwaterViewCs(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_296 == 0) {
        thisv->unk_2A2 = 1;
        thisv->csCameraTimer = 100;
        thisv->actionFunc = EnDivingGame_RupeeThrow;
        thisv->camLookAt.x = thisv->unk_2F4.x = -210.0f;
        thisv->camLookAt.y = thisv->unk_2F4.y = -80.0f;
        thisv->camLookAt.z = thisv->unk_2F4.z = -1020.0f;
        thisv->camEye.x = thisv->unk_2D0.x = -280.0f;
        thisv->camEye.y = thisv->unk_2D0.y = -20.0f;
        thisv->camEye.z = thisv->unk_2D0.z = -240.0f;
    }
}

// EnDivingGame_SayStartAndWait ?
void func_809EE780(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->csCameraTimer == 0) {
        Gameplay_ClearCamera(globalCtx, thisv->subCamId);
        Gameplay_ChangeCameraStatus(globalCtx, 0, CAM_STAT_ACTIVE);
        thisv->actor.textId = 0x405A;
        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        thisv->unk_292 = TEXT_STATE_EVENT;
        thisv->actionFunc = func_809EE800;
    }
}

// EnDivingGame_TalkDuringMinigame
void func_809EE800(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
            func_80088B34(BREG(2) + 50);
        } else {
            func_80088B34(BREG(2) + 50);
        }
        func_800F5ACC(NA_BGM_TIMED_MINI_GAME);
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actor.textId = 0x405B;
        thisv->unk_292 = TEXT_STATE_EVENT;
        thisv->state = ENDIVINGGAME_STATE_PLAYING;
        thisv->actionFunc = EnDivingGame_Talk;
    }
}

void func_809EE8F0(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx))) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnDivingGame_Talk;
    } else {
        EnDivingGame_HasMinigameFinished(thisv, globalCtx);
    }
}

// EnDivingGame_SayCongratsAndWait ? // EnDivingGame_PlayerWonPhase1
void func_809EE96C(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx))) {
        Message_CloseTextbox(globalCtx);
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actor.textId = 0x4056;
        thisv->unk_292 = TEXT_STATE_EVENT;
        thisv->state = ENDIVINGGAME_STATE_AWARDPRIZE;
        thisv->actionFunc = EnDivingGame_Talk;
    }
}

void func_809EEA00(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->unk_292 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx))) {
        Message_CloseTextbox(globalCtx);
        thisv->actor.parent = NULL;
        func_8002F434(&thisv->actor, globalCtx, GI_SCALE_SILVER, 90.0f, 10.0f);
        thisv->actionFunc = func_809EEA90;
    }
}

void func_809EEA90(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_809EEAF8;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_SCALE_SILVER, 90.0f, 10.0f);
    }
}

// Award the scale?
void func_809EEAF8(EnDivingGame* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        // "Successful completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        thisv->allRupeesThrown = thisv->state = thisv->phase = thisv->unk_2A2 = thisv->grabbedRupeesCounter = 0;
        gSaveContext.eventChkInf[3] |= 0x100;
        thisv->actionFunc = func_809EDCB0;
    }
}

void EnDivingGame_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnDivingGame* thisv = (EnDivingGame*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f pos;

    if (thisv->csCameraTimer != 0) {
        thisv->csCameraTimer--;
    }
    if (thisv->unk_296 != 0) {
        thisv->unk_296--;
    }
    if (thisv->eyeTimer != 0) {
        thisv->eyeTimer--;
    }
    if (thisv->spawnRuppyTimer != 0) {
        thisv->spawnRuppyTimer--;
    }

    if (1) {}

    if (gSaveContext.timer1Value == 10) {
        func_800F5918();
    }
    if (thisv->eyeTimer == 0) {
        thisv->eyeTimer = 2;
        thisv->eyeTexIndex++;
        if (thisv->eyeTexIndex >= 3) {
            thisv->eyeTexIndex = 0;
            thisv->eyeTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 80.0f);
    thisv->unk_324.unk_18 = player->actor.world.pos;
    thisv->unk_324.unk_18.y = player->actor.world.pos.y;
    func_80034A14(&thisv->actor, &thisv->unk_324, 2, 4);
    thisv->vec_284 = thisv->unk_324.unk_08;
    thisv->vec_28A = thisv->unk_324.unk_0E;
    if ((globalCtx->gameplayFrames % 16) == 0) {
        pos = thisv->actor.world.pos;
        pos.y += 20.0f;
        EffectSsGRipple_Spawn(globalCtx, &pos, 100, 500, 30);
    }
    thisv->unk_290++;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 29);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

Gfx* EnDivingGame_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* displayList = static_cast<Gfx*>(Graph_Alloc(gfxCtx, sizeof(Gfx)));

    gSPEndDisplayList(displayList);
    return displayList;
}

s32 EnDivingGame_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    EnDivingGame* thisv = (EnDivingGame*)thisx;
    s32 pad;

    if (limbIndex == 6) {
        rot->x += thisv->vec_28A.y;
    }

    if (limbIndex == 15) {
        rot->x += thisv->vec_284.y;
        rot->z += thisv->vec_284.z;
    }

    if (thisv->notPlayingMinigame && (limbIndex == 8 || limbIndex == 9 || limbIndex == 12)) {
        rot->y += Math_SinS((globalCtx->state.frames * (limbIndex * 50 + 0x814))) * 200.0f;
        rot->z += Math_CosS((globalCtx->state.frames * (limbIndex * 50 + 0x940))) * 200.0f;
    }

    return 0;
}

void EnDivingGame_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDivingGame* thisv = (EnDivingGame*)thisx;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_diving_game.c", 1212);
    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, EnDivingGame_EmptyDList(globalCtx->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeTexIndex]));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDivingGame_OverrideLimbDraw, NULL, thisv);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_diving_game.c", 1232);
}
