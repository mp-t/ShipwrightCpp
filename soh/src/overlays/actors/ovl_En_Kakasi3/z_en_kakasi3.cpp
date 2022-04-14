/*
 * File: z_en_kakasi3.c
 * Overlay: ovl_En_Kakasi3
 * Description: Bonooru the Scarecrow
 */

#include "z_en_kakasi3.h"
#include "vt.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_25)

void EnKakasi3_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi3_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi3_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi3_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A911F0(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A91284(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A91348(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A915B8(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A91620(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A91760(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A917FC(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A9187C(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A918E4(EnKakasi3* thisv, GlobalContext* globalCtx);
void func_80A91A90(EnKakasi3* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

const ActorInit En_Kakasi3_InitVars = {
    ACTOR_EN_KAKASI3,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi3),
    (ActorFunc)EnKakasi3_Init,
    (ActorFunc)EnKakasi3_Destroy,
    (ActorFunc)EnKakasi3_Update,
    (ActorFunc)EnKakasi3_Draw,
    NULL,
};

void EnKakasi3_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi3* thisv = (EnKakasi3*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    SkelAnime_Free(&thisv->skelAnime, globalCtx); //OTR - Fixed thisv memory leak
    //! @bug SkelAnime_Free is not called
}

void EnKakasi3_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi3* thisv = (EnKakasi3*)thisx;

    osSyncPrintf("\n\n");
    // "Obonur" -- Related to the name of the scarecrow (Bonooru)
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ おーボヌール ☆☆☆☆☆ \n" VT_RST);
    thisv->actor.targetMode = 6;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
    thisv->actor.flags |= ACTOR_FLAG_10;
    thisv->rot = thisv->actor.world.rot;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actionFunc = func_80A911F0;
}

void func_80A90E28(EnKakasi3* thisv) {
    thisv->unk_1A4 = 0;
    thisv->skelAnime.playSpeed = 0.0f;
    thisv->unk_1AA = thisv->unk_1AE = 0x0;

    Math_ApproachZeroF(&thisv->skelAnime.curFrame, 0.5f, 1.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->rot.x, 5, 0x2710, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->rot.y, 5, 0x2710, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->rot.z, 5, 0x2710, 0);
}

void func_80A90EBC(EnKakasi3* thisv, GlobalContext* globalCtx, s32 arg) {
    s16 currentFrame;
    s16 ocarinaNote = globalCtx->msgCtx.lastOcaNoteIdx;

    if (arg != 0) {
        if (thisv->unk_19C[3] == 0) {
            thisv->unk_19C[3] = (s16)Rand_ZeroFloat(10.99f) + 30;
            thisv->unk_1A6 = (s16)Rand_ZeroFloat(4.99f);
        }

        thisv->unk_19A = (s16)Rand_ZeroFloat(2.99f) + 5;
        ocarinaNote = thisv->unk_1A6;
    }
    switch (ocarinaNote) {
        case OCARINA_NOTE_A:
            thisv->unk_19A++;
            if (thisv->unk_1A4 == 0) {
                thisv->unk_1A4 = 1;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KAKASHI_ROLL);
            }
            break;
        case OCARINA_NOTE_C_DOWN:
            thisv->unk_19A++;
            thisv->unk_1B8 = 1.0f;
            break;
        case OCARINA_NOTE_C_RIGHT:
            thisv->unk_19A++;
            if (thisv->unk_1AE == 0x0) {
                thisv->unk_1AE = 0x1388;
            }
            break;
        case OCARINA_NOTE_C_LEFT:
            thisv->unk_19A++;
            if (thisv->unk_1AA == 0x0) {
                thisv->unk_1AA = 0x1388;
            }
            break;
        case OCARINA_NOTE_C_UP:
            thisv->unk_19A++;
            thisv->unk_1B8 = 2.0f;
            break;
    }

    if (thisv->unk_19A > 8) {
        thisv->unk_19A = 8;
    }

    if (thisv->unk_19A != 0) {
        thisv->actor.gravity = -1.0f;
        if (thisv->unk_19A == 8 && (thisv->actor.bgCheckFlags & 1)) {
            thisv->actor.velocity.y = 3.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_KAKASHI_JUMP);
        }
        Math_ApproachF(&thisv->skelAnime.playSpeed, thisv->unk_1B8, 0.1f, 0.2f);
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->unk_1AA, 0x5, 0x3E8, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->unk_1AE, 0x5, 0x3E8, 0);

        if (thisv->unk_1AA != 0x0 && fabsf(thisv->actor.shape.rot.x - thisv->unk_1AA) < 50.0f) {
            thisv->unk_1AA *= -1.0f;
        }
        if (thisv->unk_1AE != 0x0 && fabsf(thisv->actor.shape.rot.z - thisv->unk_1AE) < 50.0f) {
            thisv->unk_1AE *= -1.0f;
        }

        if (thisv->unk_1A4 != 0) {
            thisv->actor.shape.rot.y += 0x1000;
            if (thisv->actor.shape.rot.y == 0) {
                thisv->unk_1A4 = 0;
            }
        }
        currentFrame = thisv->skelAnime.curFrame;
        if (currentFrame == 11 || currentFrame == 17) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KAKASHI_SWING);
        }
        SkelAnime_Update(&thisv->skelAnime);
    }
}

void func_80A911F0(EnKakasi3* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&thisv->skelAnime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = func_80A91284;
}

void func_80A91284(EnKakasi3* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    thisv->actor.textId = 0x40A1;
    thisv->dialogState = TEXT_STATE_DONE;
    thisv->unk_19A = 0;

    if (!LINK_IS_ADULT) {
        thisv->unk_194 = false;
        if (gSaveContext.scarecrowSpawnSongSet) {
            thisv->actor.textId = 0x40A0;
            thisv->dialogState = TEXT_STATE_EVENT;
            thisv->unk_1A8 = 1;
        }
    } else {
        thisv->unk_194 = true;
        if (gSaveContext.scarecrowSpawnSongSet) {
            if (thisv->unk_195) {
                thisv->actor.textId = 0x40A2;
            } else {
                thisv->actor.textId = 0x40A3;
            }
        }
    }
    thisv->actionFunc = func_80A91348;
}

void func_80A91348(EnKakasi3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    func_80A90E28(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->camId = SUBCAM_NONE;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (!thisv->unk_194) {
            if (thisv->unk_1A8 == 0) {
                thisv->actionFunc = func_80A91284;
            } else {
                thisv->actionFunc = func_80A91760;
            }
        } else {
            thisv->actionFunc = func_80A91284;
        }
    } else {
        s16 angleTowardsLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

        if (!(thisv->actor.xzDistToPlayer > 120.0f)) {
            s16 absAngleTowardsLink = ABS(angleTowardsLink);

            if (absAngleTowardsLink < 0x4300) {
                if (!thisv->unk_194) {

                    if (player->stateFlags2 & 0x1000000) {
                        thisv->camId = OnePointCutscene_Init(globalCtx, 2260, -99, &thisv->actor, MAIN_CAM);
                        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                        thisv->dialogState = TEXT_STATE_EVENT;
                        thisv->unk_1B8 = 0.0f;
                        Message_StartTextbox(globalCtx, 0x40A4, NULL);
                        player->stateFlags2 |= 0x800000;
                        thisv->actionFunc = func_80A915B8;
                        return;
                    }
                    if (thisv->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= 0x800000;
                    }
                } else if (gSaveContext.scarecrowSpawnSongSet && !thisv->unk_195) {

                    if (player->stateFlags2 & 0x1000000) {
                        thisv->camId = OnePointCutscene_Init(globalCtx, 2260, -99, &thisv->actor, MAIN_CAM);
                        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                        thisv->dialogState = TEXT_STATE_EVENT;
                        thisv->unk_1B8 = 0.0f;
                        Message_StartTextbox(globalCtx, 0x40A8, NULL);
                        player->stateFlags2 |= 0x800000;
                        thisv->actionFunc = func_80A9187C;
                        return;
                    }
                    if (thisv->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= 0x800000;
                    }
                }
                func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
            }
        }
    }
}

void func_80A915B8(EnKakasi3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_8010BD58(globalCtx, OCARINA_ACTION_SCARECROW_RECORDING);
        thisv->actionFunc = func_80A91620;
    }
}

void func_80A91620(EnKakasi3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04 ||
         (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05 && globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0B)) &&
        (globalCtx->msgCtx.msgMode == MSGMODE_NONE)) {

        OnePointCutscene_EndCutscene(globalCtx, thisv->camId);
        if (globalCtx->cameraPtrs[thisv->camId] == NULL) {
            thisv->camId = SUBCAM_NONE;
        }
        if (thisv->camId != SUBCAM_NONE) {
            func_8005B1A4(globalCtx->cameraPtrs[thisv->camId]);
        }
        thisv->actionFunc = func_80A911F0;
        return;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03 && globalCtx->msgCtx.msgMode == MSGMODE_NONE) {
        thisv->dialogState = TEXT_STATE_EVENT;
        Message_StartTextbox(globalCtx, 0x40A5, NULL);
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = func_80A91A90;
        return;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        func_80A90EBC(thisv, globalCtx, 0);
        player->stateFlags2 |= 0x800000;
    }
}

void func_80A91760(EnKakasi3* thisv, GlobalContext* globalCtx) {

    func_80A90E28(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->dialogState == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        func_8010BD58(globalCtx, OCARINA_ACTION_SCARECROW_PLAYBACK);
        thisv->actionFunc = func_80A917FC;
        thisv->camId = OnePointCutscene_Init(globalCtx, 2280, -99, &thisv->actor, MAIN_CAM);
    }
}

void func_80A917FC(EnKakasi3* thisv, GlobalContext* globalCtx) {

    if (globalCtx->msgCtx.ocarinaMode != OCARINA_MODE_0F) {
        func_80A90EBC(thisv, globalCtx, 1);
    } else {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        Message_CloseTextbox(globalCtx);
        OnePointCutscene_EndCutscene(globalCtx, thisv->camId);
        thisv->actionFunc = func_80A911F0;
    }
}

void func_80A9187C(EnKakasi3* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_SCARECROW);
        thisv->actionFunc = func_80A918E4;
    }
}

void func_80A918E4(EnKakasi3* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (BREG(3) != 0) {
        // "No way!"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ まさか！ ☆☆☆☆☆ %d\n" VT_RST, globalCtx->msgCtx.ocarinaMode);
    }
    if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04 ||
         (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05 && globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0B)) &&
        globalCtx->msgCtx.msgMode == MSGMODE_NONE) {

        Message_StartTextbox(globalCtx, 0x40A6, NULL);
        thisv->dialogState = TEXT_STATE_EVENT;
        OnePointCutscene_EndCutscene(globalCtx, thisv->camId);
        thisv->camId = SUBCAM_NONE;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = func_80A91A90;
        return;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03 && globalCtx->msgCtx.msgMode == MSGMODE_NONE) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        if (BREG(3) != 0) {
            osSyncPrintf("\n\n");
            // "With thisv, other guys are OK! That's it!"
            osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ これで、他の奴もＯＫ！だ！ ☆☆☆☆☆ %d\n" VT_RST,
                         globalCtx->msgCtx.ocarinaMode);
        }
        thisv->unk_195 = true;
        Message_StartTextbox(globalCtx, 0x40A7, NULL);
        thisv->dialogState = TEXT_STATE_EVENT;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = func_80A91A90;
        return;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        func_80A90EBC(thisv, globalCtx, 0);
        player->stateFlags2 |= 0x800000;
    }
}

void func_80A91A90(EnKakasi3* thisv, GlobalContext* globalCtx) {
    func_80A90E28(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    func_8002DF54(globalCtx, NULL, 8);

    if (thisv->dialogState == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->unk_195) {
            if (!(gSaveContext.eventChkInf[9] & 0x1000)) {
                gSaveContext.eventChkInf[9] |= 0x1000;
            }
        }
        if (globalCtx->cameraPtrs[thisv->camId] == NULL) {
            thisv->camId = SUBCAM_NONE;
        }
        if (thisv->camId != SUBCAM_NONE) {
            func_8005B1A4(globalCtx->cameraPtrs[thisv->camId]);
        }
        Message_CloseTextbox(globalCtx);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actionFunc = func_80A911F0;
    }
}

void EnKakasi3_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi3* thisv = (EnKakasi3*)thisx;
    s32 pad;
    s32 i;

    if (BREG(2) != 0) {
        osSyncPrintf("\n\n");
        // "flag!"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ フラグ！ ☆☆☆☆☆ %d\n" VT_RST, gSaveContext.scarecrowSpawnSongSet);
    }

    thisv->unk_198++;
    thisv->actor.world.rot = thisv->actor.shape.rot;
    for (i = 0; i < ARRAY_COUNT(thisv->unk_19C); i++) {
        if (thisv->unk_19C[i] != 0) {
            thisv->unk_19C[i]--;
        }
    }

    Actor_SetFocus(&thisv->actor, 60.0f);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 28);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnKakasi3_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi3* thisv = (EnKakasi3*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
}
