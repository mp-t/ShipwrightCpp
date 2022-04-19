/*
 * File: z_en_heishi1.c
 * Overlay: ovl_En_Heishi1
 * Description: Courtyard Guards
 */

#include "z_en_heishi1.h"
#include "objects/object_sd/object_sd.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnHeishi1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi1_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnHeishi1_SetupWait(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_SetupWalk(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_SetupMoveToLink(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_SetupTurnTowardLink(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_SetupKick(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_SetupWaitNight(EnHeishi1* thisv, GlobalContext* globalCtx);

void EnHeishi1_Wait(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_Walk(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_MoveToLink(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_TurnTowardLink(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_Kick(EnHeishi1* thisv, GlobalContext* globalCtx);
void EnHeishi1_WaitNight(EnHeishi1* thisv, GlobalContext* globalCtx);

static s32 sPlayerIsCaught = false;

ActorInit En_Heishi1_InitVars = {
    0,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SD,
    sizeof(EnHeishi1),
    (ActorFunc)EnHeishi1_Init,
    (ActorFunc)EnHeishi1_Destroy,
    (ActorFunc)EnHeishi1_Update,
    (ActorFunc)EnHeishi1_Draw,
    NULL,
};

static f32 sAnimParamsInit[][8] = {
    { 1.0f, -10.0f, 3.0f, 0.5f, 1000.0f, 200.0f, 0.3f, 1000.0f },
    { 3.0f, -3.0f, 6.0f, 0.8f, 2000.0f, 400.0f, 0.5f, 2000.0f },
    { 1.0f, -10.0f, 3.0f, 0.5f, 1000.0f, 200.0f, 0.3f, 1000.0f },
    { 3.0f, -3.0f, 6.0f, 0.8f, 2000.0f, 400.0f, 0.5f, 2000.0f },
};

static s16 sBaseHeadTimers[] = { 20, 10, 20, 10, 13, 0 };

static Vec3f sRupeePositions[] = {
    { 0.0f, 0.0f, 90.0f },  { -55.0f, 0.0f, 90.0f }, { -55.0f, 0.0f, 30.0f }, { -55.0f, 0.0f, -30.0f },
    { 0.0f, 0.0f, -30.0f }, { 55.0f, 0.0f, -30.0f }, { 55.0f, 0.0f, 30.0f },  { 55.0f, 0.0f, 90.0f },
};

static s32 sCamDataIdxs[] = {
    7, 7, 2, 2, 2, 2, 3, 3, 4, 4, 5, 6, 4, 4, 5, 6,
};

static s16 sWaypoints[] = { 0, 4, 1, 5, 2, 6, 3, 7 };

void EnHeishi1_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHeishi1* thisv = (EnHeishi1*)thisx;
    Vec3f rupeePos;
    s32 i;

    Actor_SetScale(&thisv->actor, 0.01f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, thisv->jointTable, thisv->morphTable,
                   17);

    thisv->type = (thisv->actor.params >> 8) & 0xFF;
    thisv->path = thisv->actor.params & 0xFF;

    for (i = 0; i < ARRAY_COUNT(sAnimParamsInit[0]); i++) {
        thisv->animParams[i] = sAnimParamsInit[thisv->type][i];
    }

    // "type"
    osSyncPrintf(VT_FGCOL(GREEN) " 種類☆☆☆☆☆☆☆☆☆☆☆☆☆ %d\n" VT_RST, thisv->type);
    // "path data"
    osSyncPrintf(VT_FGCOL(YELLOW) " れえるでぇたぁ☆☆☆☆☆☆☆☆ %d\n" VT_RST, thisv->path);
    osSyncPrintf(VT_FGCOL(PURPLE) " anime_frame_speed ☆☆☆☆☆☆ %f\n" VT_RST, thisv->animSpeed);
    // "interpolation frame"
    osSyncPrintf(VT_FGCOL(PURPLE) " 補間フレーム☆☆☆☆☆☆☆☆☆ %f\n" VT_RST, thisv->transitionRate);
    // "targeted movement speed value between points"
    osSyncPrintf(VT_FGCOL(PURPLE) " point間の移動スピード目標値 ☆ %f\n" VT_RST, thisv->moveSpeedTarget);
    // "maximum movement speed value between points"
    osSyncPrintf(VT_FGCOL(PURPLE) " point間の移動スピード最大 ☆☆ %f\n" VT_RST, thisv->moveSpeedMax);
    // "(body) targeted turning angle speed value"
    osSyncPrintf(VT_FGCOL(PURPLE) " (体)反転アングルスピード目標値 %f\n" VT_RST, thisv->bodyTurnSpeedTarget);
    // "(body) maximum turning angle speed"
    osSyncPrintf(VT_FGCOL(PURPLE) " (体)反転アングルスピード最大☆ %f\n" VT_RST, thisv->bodyTurnSpeedMax);
    // "(head) targeted turning angle speed value"
    osSyncPrintf(VT_FGCOL(PURPLE) " (頭)反転アングルスピード加算値 %f\n" VT_RST, thisv->headTurnSpeedScale);
    // "(head) maximum turning angle speed"
    osSyncPrintf(VT_FGCOL(PURPLE) " (頭)反転アングルスピード最大☆ %f\n" VT_RST, thisv->headTurnSpeedMax);
    osSyncPrintf(VT_FGCOL(GREEN) " 今時間 %d\n" VT_RST, ((void)0, gSaveContext.dayTime)); // "current time"
    osSyncPrintf(VT_FGCOL(YELLOW) " チェック時間 %d\n" VT_RST, 0xBAAA);                   // "check time"
    osSyncPrintf("\n\n");

    if (thisv->path == 3) {
        for (i = 0; i < ARRAY_COUNT(sRupeePositions); i++) {
            rupeePos = sRupeePositions[i];
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_RUPPY, rupeePos.x, rupeePos.y,
                               rupeePos.z, 0, 0, 0, 3);
        }
    }

    if (thisv->type != 5) {
        if (((gSaveContext.dayTime < 0xB888) || IS_DAY) && !(gSaveContext.eventChkInf[8] & 1)) {
            thisv->actionFunc = EnHeishi1_SetupWalk;
        } else {
            Actor_Kill(&thisv->actor);
        }
    } else {
        if ((gSaveContext.dayTime >= 0xB889) || !IS_DAY || (gSaveContext.eventChkInf[8] & 1)) {
            thisv->actionFunc = EnHeishi1_SetupWaitNight;
        } else {
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnHeishi1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnHeishi1_SetupWalk(EnHeishi1* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiWalkAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiWalkAnim, thisv->animSpeed, 0.0f, (s16)frameCount, ANIMMODE_LOOP,
                     thisv->transitionRate);
    thisv->bodyTurnSpeed = 0.0f;
    thisv->moveSpeed = 0.0f;
    thisv->headDirection = Rand_ZeroFloat(1.99f);
    thisv->actionFunc = EnHeishi1_Walk;
}

void EnHeishi1_Walk(EnHeishi1* thisv, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* pointPos;
    f32 pathDiffX;
    f32 pathDiffZ;
    s16 randOffset;

    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 1.0f) || Animation_OnFrame(&thisv->skelAnime, 17.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KNIGHT_WALK);
    }

    if (!sPlayerIsCaught) {
        path = &globalCtx->setupPathList[thisv->path];
        pointPos = SEGMENTED_TO_VIRTUAL(path->points);
        pointPos += thisv->waypoint;

        Math_ApproachF(&thisv->actor.world.pos.x, pointPos->x, 1.0f, thisv->moveSpeed);
        Math_ApproachF(&thisv->actor.world.pos.z, pointPos->z, 1.0f, thisv->moveSpeed);

        Math_ApproachF(&thisv->moveSpeed, thisv->moveSpeedTarget, 1.0f, thisv->moveSpeedMax);

        pathDiffX = pointPos->x - thisv->actor.world.pos.x;
        pathDiffZ = pointPos->z - thisv->actor.world.pos.z;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, (Math_FAtan2F(pathDiffX, pathDiffZ) * (0x8000 / std::numbers::pi_v<float>)), 3,
                           thisv->bodyTurnSpeed, 0);

        Math_ApproachF(&thisv->bodyTurnSpeed, thisv->bodyTurnSpeedTarget, 1.0f, thisv->bodyTurnSpeedMax);

        if (thisv->headTimer == 0) {
            thisv->headDirection++;
            thisv->headAngleTarget = 0x2000;
            // if headDirection is odd, face 45 degrees left
            if ((thisv->headDirection & 1) != 0) {
                thisv->headAngleTarget *= -1;
            }
            randOffset = Rand_ZeroFloat(30.0f);
            thisv->headTimer = sBaseHeadTimers[thisv->type] + randOffset;
        }

        Math_ApproachF(&thisv->headAngle, thisv->headAngleTarget, thisv->headTurnSpeedScale, thisv->headTurnSpeedMax);

        if ((thisv->path == BREG(1)) && (BREG(0) != 0)) {
            osSyncPrintf(VT_FGCOL(RED) " 種類  %d\n" VT_RST, thisv->path);
            osSyncPrintf(VT_FGCOL(RED) " ぱす  %d\n" VT_RST, thisv->waypoint);
            osSyncPrintf(VT_FGCOL(RED) " 反転  %d\n" VT_RST, thisv->bodyTurnSpeed);
            osSyncPrintf(VT_FGCOL(RED) " 時間  %d\n" VT_RST, thisv->waypointTimer);
            osSyncPrintf(VT_FGCOL(RED) " 点座  %d\n" VT_RST, path->count);
            osSyncPrintf("\n\n");
        }

        // when 20 units away from a middle waypoint, decide whether or not to skip it
        if ((fabsf(pathDiffX) < 20.0f) && (fabsf(pathDiffZ) < 20.0f)) {
            if (thisv->waypointTimer == 0) {
                if (thisv->type >= 2) {
                    if ((thisv->waypoint >= 4) && (Rand_ZeroFloat(1.99f) > 1.0f)) {
                        if (thisv->waypoint == 7) {
                            thisv->waypoint = 0;
                        }
                        if (thisv->waypoint >= 4) {
                            thisv->waypoint -= 3;
                        }
                        thisv->waypointTimer = 5;
                        return;
                    }
                }
                thisv->actionFunc = EnHeishi1_SetupWait;
            }
        }
    }
}

void EnHeishi1_SetupMoveToLink(EnHeishi1* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiWalkAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiWalkAnim, 3.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -3.0f);
    thisv->bodyTurnSpeed = 0.0f;
    thisv->moveSpeed = 0.0f;
    Message_StartTextbox(globalCtx, 0x702D, &thisv->actor);
    Interface_SetDoAction(globalCtx, DO_ACTION_STOP);
    thisv->actionFunc = EnHeishi1_MoveToLink;
}

void EnHeishi1_MoveToLink(EnHeishi1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.world.pos.x, player->actor.world.pos.x, 1.0f, thisv->moveSpeed);
    Math_ApproachF(&thisv->actor.world.pos.z, player->actor.world.pos.z, 1.0f, thisv->moveSpeed);
    Math_ApproachF(&thisv->moveSpeed, 6.0f, 1.0f, 0.4f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, thisv->bodyTurnSpeed, 0);
    Math_ApproachF(&thisv->bodyTurnSpeed, 3000.0f, 1.0f, 300.0f);
    Math_ApproachZeroF(&thisv->headAngle, 0.5f, 2000.0f);

    if (thisv->actor.xzDistToPlayer < 70.0f) {
        thisv->actionFunc = EnHeishi1_SetupTurnTowardLink;
    }
}

void EnHeishi1_SetupWait(EnHeishi1* thisv, GlobalContext* globalCtx) {
    s16 rand;
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, thisv->animSpeed, 0.0f, (s16)frameCount, ANIMMODE_LOOP,
                     thisv->transitionRate);
    thisv->headBehaviorDecided = false;
    thisv->headDirection = Rand_ZeroFloat(1.99f);
    rand = Rand_ZeroFloat(50.0f);
    thisv->waitTimer = rand + 50;
    thisv->actionFunc = EnHeishi1_Wait;
}

void EnHeishi1_Wait(EnHeishi1* thisv, GlobalContext* globalCtx) {
    s16 randOffset;
    s32 i;

    SkelAnime_Update(&thisv->skelAnime);
    if (!sPlayerIsCaught) {
        switch (thisv->headBehaviorDecided) {
            case false:
                thisv->headDirection++;
                // if headDirection is odd, face 52 degrees left
                thisv->headAngleTarget = (thisv->headDirection & 1) ? 0x2500 : -0x2500;
                randOffset = Rand_ZeroFloat(30.0f);
                thisv->headTimer = sBaseHeadTimers[thisv->type] + randOffset;
                thisv->headBehaviorDecided = true;
                break;
            case true:
                if (thisv->headTimer == 0) {
                    if (thisv->waitTimer == 0) {
                        if ((thisv->type == 0) || (thisv->type == 1)) {
                            thisv->waypoint++;
                            if (thisv->waypoint >= 4) {
                                thisv->waypoint = 0;
                            }
                        } else {
                            // waypoints are defined with corners as 0-3 and middle points as 4-7
                            // to choose the next waypoint, the order "04152637" is hardcoded in an array
                            for (i = 0; i < ARRAY_COUNT(sWaypoints); i++) {
                                if (thisv->waypoint == sWaypoints[i]) {
                                    i++;
                                    if (i >= ARRAY_COUNT(sWaypoints)) {
                                        i = 0;
                                    }
                                    thisv->waypoint = sWaypoints[i];
                                    break;
                                }
                            }
                            thisv->waypointTimer = 5;
                        }
                        thisv->actionFunc = EnHeishi1_SetupWalk;
                    } else {
                        thisv->headBehaviorDecided = false;
                    }
                }
                break;
        }
        Math_ApproachF(&thisv->headAngle, thisv->headAngleTarget, thisv->headTurnSpeedScale,
                       thisv->headTurnSpeedMax + thisv->headTurnSpeedMax);

        if ((thisv->path == BREG(1)) && (BREG(0) != 0)) {
            osSyncPrintf(VT_FGCOL(GREEN) " 種類  %d\n" VT_RST, thisv->path);
            osSyncPrintf(VT_FGCOL(GREEN) " ぱす  %d\n" VT_RST, thisv->waypoint);
            osSyncPrintf(VT_FGCOL(GREEN) " 反転  %d\n" VT_RST, thisv->bodyTurnSpeed);
            osSyncPrintf(VT_FGCOL(GREEN) " 時間  %d\n" VT_RST, thisv->waypointTimer);
            osSyncPrintf("\n\n");
        }
    }
}

void EnHeishi1_SetupTurnTowardLink(EnHeishi1* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->kickTimer = 30;
    thisv->actionFunc = EnHeishi1_TurnTowardLink;
}

void EnHeishi1_TurnTowardLink(EnHeishi1* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->type != 5) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, thisv->bodyTurnSpeed, 0);
        Math_ApproachF(&thisv->bodyTurnSpeed, 3000.0f, 1.0f, 300.0f);
        Math_ApproachZeroF(&thisv->headAngle, 0.5f, 2000.0f);
    }

    if (thisv->kickTimer == 0) {
        thisv->actionFunc = EnHeishi1_SetupKick;
    }
}

void EnHeishi1_SetupKick(EnHeishi1* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = EnHeishi1_Kick;
}

void EnHeishi1_Kick(EnHeishi1* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (!thisv->loadStarted) {
        // if dialog state is 5 and textbox has been advanced, kick player out
        if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
            Message_CloseTextbox(globalCtx);
            if (!thisv->loadStarted) {
                gSaveContext.eventChkInf[4] |= 0x4000;
                globalCtx->nextEntranceIndex = 0x4FA;
                globalCtx->sceneLoadFlag = 0x14;
                thisv->loadStarted = true;
                sPlayerIsCaught = false;
                globalCtx->fadeTransition = 0x2E;
                gSaveContext.nextTransition = 0x2E;
            }
        }
    }
}

void EnHeishi1_SetupWaitNight(EnHeishi1* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = EnHeishi1_WaitNight;
}

void EnHeishi1_WaitNight(EnHeishi1* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.xzDistToPlayer < 100.0f) {
        Message_StartTextbox(globalCtx, 0x702D, &thisv->actor);
        func_80078884(NA_SE_SY_FOUND);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発見！ ☆☆☆☆☆ \n" VT_RST); // "Discovered!"
        func_8002DF54(globalCtx, &thisv->actor, 1);
        thisv->actionFunc = EnHeishi1_SetupKick;
    }
}

void EnHeishi1_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHeishi1* thisv = (EnHeishi1*)thisx;
    s16 path;
    u8 i;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad2;
    Camera* activeCam;

    thisv->activeTimer++;

    for (i = 0; i < ARRAY_COUNT(thisv->timers); i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }

    if (thisv->waypointTimer != 0) {
        thisv->waypointTimer--;
    }

    activeCam = GET_ACTIVE_CAM(globalCtx);

    if (player->actor.freezeTimer == 0) {

        thisv->actionFunc(thisv, globalCtx);

        thisv->actor.uncullZoneForward = 550.0f;
        thisv->actor.uncullZoneScale = 350.0f;
        thisv->actor.uncullZoneDownward = 700.0f;

        if (thisv->type != 5) {
            path = thisv->path * 2;
            if ((sCamDataIdxs[path] == activeCam->camDataIdx) || (sCamDataIdxs[path + 1] == activeCam->camDataIdx)) {
                if (!sPlayerIsCaught) {
                    if ((thisv->actionFunc == EnHeishi1_Walk) || (thisv->actionFunc == EnHeishi1_Wait)) {
                        Vec3f searchBallVel;
                        Vec3f searchBallAccel = { 0.0f, 0.0f, 0.0f };
                        Vec3f searchBallMult = { 0.0f, 0.0f, 20.0f };
                        Vec3f searchBallPos;

                        searchBallPos.x = thisv->actor.world.pos.x;
                        searchBallPos.y = thisv->actor.world.pos.y + 60.0f;
                        searchBallPos.z = thisv->actor.world.pos.z;

                        Matrix_Push();
                        Matrix_RotateY(((thisv->actor.shape.rot.y + thisv->headAngle) / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
                        searchBallMult.z = 30.0f;
                        Matrix_MultVec3f(&searchBallMult, &searchBallVel);
                        Matrix_Pop();

                        EffectSsSolderSrchBall_Spawn(globalCtx, &searchBallPos, &searchBallVel, &searchBallAccel, 2,
                                                     &thisv->linkDetected);

                        if (thisv->actor.xzDistToPlayer < 60.0f) {
                            thisv->linkDetected = true;
                        } else if (thisv->actor.xzDistToPlayer < 70.0f) {
                            // thisv case probably exists to detect link making a jump sound
                            // from slightly further away than the previous 60 unit check
                            if (player->actor.velocity.y > -4.0f) {
                                thisv->linkDetected = true;
                            }
                        }

                        if (thisv->linkDetected) {
                            //! @bug This appears to be a check to make sure that link is standing on the ground
                            // before getting caught. However thisv is an issue for two reasons:
                            // 1: When doing a backflip or falling from the upper path, links y velocity will reach
                            // less than -4.0 before even touching the ground.
                            // 2: There is one frame when landing from a sidehop where you can sidehop again without
                            // letting y velocity reach -4.0 or less. This enables the player to do frame perfect
                            // sidehops onto the next screen and prevent getting caught.
                            if (!(player->actor.velocity.y > -3.9f)) {
                                thisv->linkDetected = false;
                                // thisv 60 unit height check is so the player doesnt get caught when on the upper path
                                if (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < 60.0f) {
                                    func_80078884(NA_SE_SY_FOUND);
                                    // "Discovered!"
                                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発見！ ☆☆☆☆☆ \n" VT_RST);
                                    func_8002DF54(globalCtx, &thisv->actor, 1);
                                    sPlayerIsCaught = true;
                                    thisv->actionFunc = EnHeishi1_SetupMoveToLink;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

s32 EnHeishi1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnHeishi1* thisv = (EnHeishi1*)thisx;

    // turn the guards head to match the direction he is looking
    if (limbIndex == 16) {
        rot->x += (s16)thisv->headAngle;
    }

    return false;
}

void EnHeishi1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHeishi1* thisv = (EnHeishi1*)thisx;
    Vec3f matrixScale = { 0.3f, 0.3f, 0.3f };

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnHeishi1_OverrideLimbDraw, NULL,
                      thisv);
    func_80033C30(&thisv->actor.world.pos, &matrixScale, 0xFF, globalCtx);

    if ((thisv->path == BREG(1)) && (BREG(0) != 0)) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y + 100.0f, thisv->actor.world.pos.z,
                               17000, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f, 1.0f, 255, 0, 0,
                               255, 4, globalCtx->state.gfxCtx);
    }
}
