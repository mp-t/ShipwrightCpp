/*
 * File: z_en_zl4.c
 * Overlay: ovl_En_Zl4
 * Description: Child Princess Zelda
 */

#include "z_en_zl4.h"
#include "objects/object_zl4/object_zl4.h"
#include "scenes/indoors/nakaniwa/nakaniwa_scene.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ ZL4_CS_WAIT,
    /* 1 */ ZL4_CS_START,
    /* 2 */ ZL4_CS_MEET,
    /* 3 */ ZL4_CS_STONE,
    /* 4 */ ZL4_CS_NAMES,
    /* 5 */ ZL4_CS_LEGEND,
    /* 6 */ ZL4_CS_WINDOW,
    /* 7 */ ZL4_CS_GANON,
    /* 8 */ ZL4_CS_PLAN
} EnZl4CutsceneState;

typedef enum {
    /* 0 */ ZL4_EYES_NEUTRAL,
    /* 1 */ ZL4_EYES_SHUT,
    /* 2 */ ZL4_EYES_LOOK_LEFT,
    /* 3 */ ZL4_EYES_LOOK_RIGHT,
    /* 4 */ ZL4_EYES_WIDE,
    /* 5 */ ZL4_EYES_SQUINT,
    /* 6 */ ZL4_EYES_OPEN
} EnZl4EyeExpression;

typedef enum {
    /* 0 */ ZL4_MOUTH_NEUTRAL,
    /* 1 */ ZL4_MOUTH_HAPPY,
    /* 2 */ ZL4_MOUTH_WORRIED,
    /* 3 */ ZL4_MOUTH_SURPRISED
} EnZl4MouthExpression;

typedef enum {
    /* 0 */ ZL4_EYE_OPEN,
    /* 1 */ ZL4_EYE_BLINK,
    /* 2 */ ZL4_EYE_SHUT,
    /* 3 */ ZL4_EYE_WIDE,
    /* 4 */ ZL4_EYE_SQUINT,
    /* 5 */ ZL4_EYE_LOOK_OUT,
    /* 6 */ ZL4_EYE_LOOK_IN
} EnZl4EyeState;

void EnZl4_Init(Actor* thisx, GlobalContext* globalCtx);
void EnZl4_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnZl4_Update(Actor* thisx, GlobalContext* globalCtx);
void EnZl4_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnZl4_Cutscene(EnZl4* thisv, GlobalContext* globalCtx);
void EnZl4_Idle(EnZl4* thisv, GlobalContext* globalCtx);
void EnZl4_TheEnd(EnZl4* thisv, GlobalContext* globalCtx);

const ActorInit En_Zl4_InitVars = {
    ACTOR_EN_ZL4,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZL4,
    sizeof(EnZl4),
    (ActorFunc)EnZl4_Init,
    (ActorFunc)EnZl4_Destroy,
    (ActorFunc)EnZl4_Update,
    (ActorFunc)EnZl4_Draw,
    NULL,
};

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
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 10, 44, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /*  0 */ ZL4_ANIM_0,
    /*  1 */ ZL4_ANIM_1,
    /*  2 */ ZL4_ANIM_2,
    /*  3 */ ZL4_ANIM_3,
    /*  4 */ ZL4_ANIM_4,
    /*  5 */ ZL4_ANIM_5,
    /*  6 */ ZL4_ANIM_6,
    /*  7 */ ZL4_ANIM_7,
    /*  8 */ ZL4_ANIM_8,
    /*  9 */ ZL4_ANIM_9,
    /* 10 */ ZL4_ANIM_10,
    /* 11 */ ZL4_ANIM_11,
    /* 12 */ ZL4_ANIM_12,
    /* 13 */ ZL4_ANIM_13,
    /* 14 */ ZL4_ANIM_14,
    /* 15 */ ZL4_ANIM_15,
    /* 16 */ ZL4_ANIM_16,
    /* 17 */ ZL4_ANIM_17,
    /* 18 */ ZL4_ANIM_18,
    /* 19 */ ZL4_ANIM_19,
    /* 20 */ ZL4_ANIM_20,
    /* 21 */ ZL4_ANIM_21,
    /* 22 */ ZL4_ANIM_22,
    /* 23 */ ZL4_ANIM_23,
    /* 24 */ ZL4_ANIM_24,
    /* 25 */ ZL4_ANIM_25,
    /* 26 */ ZL4_ANIM_26,
    /* 27 */ ZL4_ANIM_27,
    /* 28 */ ZL4_ANIM_28,
    /* 29 */ ZL4_ANIM_29,
    /* 30 */ ZL4_ANIM_30,
    /* 31 */ ZL4_ANIM_31,
    /* 32 */ ZL4_ANIM_32,
    /* 33 */ ZL4_ANIM_33
} EnZl4Animation;

static AnimationInfo sAnimationInfo[] = {
    /*  0 */ /* standing idle */ { &gChildZeldaAnim_000654, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /*  1 */ /* moves to introduce herself */ { &gChildZeldaAnim_00E5C8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /*  2 */ /* introducing herself */ { &gChildZeldaAnim_00EBC4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /*  3 */ /* turns away from window surprised */
    { &gChildZeldaAnim_010DF8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /*  4 */ /* standing with hand in front of mouth */
    { &gChildZeldaAnim_011248, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /*  5 */ /* surprise, moves hand to mouth */ { &gChildZeldaAnim_011698, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /*  6 */ /* uncrosses arms, leans toward link with hands together */
    { &gChildZeldaAnim_011B34, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /*  7 */ /* turns to write letter */ { &gChildZeldaAnim_0125E4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    /*  8 */ /* writing letter */ { &gChildZeldaAnim_012E58, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /*  9 */ /* pulls back, looks askew */ { &gChildZeldaAnim_013280, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /* 10 */ /* looks askew at Link */ { &gChildZeldaAnim_013628, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 11 */ /* crosses arms, looks to the side */ { &gChildZeldaAnim_013A50, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /* 12 */ /* arms crossed, looking away */ { &gChildZeldaAnim_013EA0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 13 */ /* turns away, hands behind back, looks up */
    { &gChildZeldaAnim_015F14, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 14 */ /* turns back to link, hands on top of each other */
    { &gChildZeldaAnim_0169B4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 15 */ /* hands behind back looking up */ { &gChildZeldaAnim_016D08, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 16 */ /* leans toward link, looks askew */ { &gChildZeldaAnim_01726C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /* 17 */ /* leaning toward link, looking askew */
    { &gChildZeldaAnim_017818, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /* 18 */ /* neutral, looking at Link */ { &gChildZeldaAnim_01805C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 19 */ /* moves towards link, hands clasped */
    { &gChildZeldaAnim_018898, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /* 20 */ /* facing link, hands clasped */ { &gChildZeldaAnim_01910C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 21 */ /* look in window */ { &gChildZeldaAnim_019600, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 22 */ /* leans forward, hands together */ { &gChildZeldaAnim_01991C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    /* 23 */ /* turns to link, hands on top of each other */
    { &gChildZeldaAnim_01A2FC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 24 */ /* stands, hands on top of each other */
    { &gChildZeldaAnim_01AAE0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 25 */ /* leaning forward, hands together */ { &gChildZeldaAnim_01AE88, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 26 */ /* walks aside, points to window */ { &gChildZeldaAnim_01B874, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    /* 27 */ /* stands pointing at window */ { &gChildZeldaAnim_01BCF0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 28 */ /* laughs, hands together */ { &gChildZeldaAnim_01C494, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 29 */ /* happy, hands together */ { &gChildZeldaAnim_01C7B0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    /* 30 */ /* standing hands behind back looking down*/
    { &gChildZeldaAnim_01CE08, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 31 */ /* cocks head, hands clasped */ { &gChildZeldaAnim_00F0A4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    /* 32 */ /* happy, hands clasped */ { &gChildZeldaAnim_00F894, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* 33 */ /* transition to standing */ { &gChildZeldaAnim_000654, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

#include "z_en_zl4_cutscene_data.c"

void EnZl4_SetCsCameraAngle(GlobalContext* globalCtx, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(globalCtx);

    Camera_ChangeSetting(activeCam, CAM_SET_FREE0);
    activeCam->at = sCsCameraAngle[index].at;
    activeCam->eye = activeCam->eyeNext = sCsCameraAngle[index].eye;
    activeCam->roll = sCsCameraAngle[index].roll;
    activeCam->fov = sCsCameraAngle[index].fov;
}

void EnZl4_SetCsCameraMove(GlobalContext* globalCtx, s16 index) {
    Camera* activeCam = GET_ACTIVE_CAM(globalCtx);
    Player* player = GET_PLAYER(globalCtx);

    Camera_ChangeSetting(activeCam, CAM_SET_CS_0);
    Camera_ResetAnim(activeCam);
    Camera_SetCSParams(activeCam, sCsCameraMove[index].atPoints, sCsCameraMove[index].eyePoints, player,
                       sCsCameraMove[index].relativeToPlayer);
}

u16 EnZl4_GetText(GlobalContext* globalCtx, Actor* thisx) {
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 22);
    u16 stoneCount;
    s16 ret;

    if (faceReaction != 0) {
        return faceReaction;
    }

    stoneCount = 0;
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        stoneCount = 1;
    }
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        stoneCount++;
    }
    if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
        stoneCount++;
    }

    if (stoneCount > 1) {
        ret = 0x703D;
    } else {
        ret = 0x703C;
    }
    return ret;
}

s16 func_80B5B9B0(GlobalContext* globalCtx, Actor* thisx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        return false;
    }
    return true;
}

void EnZl4_UpdateFace(EnZl4* thisv) {
    if (thisv->blinkTimer > 0) {
        thisv->blinkTimer--;
    } else {
        thisv->blinkTimer = 0;
    }
    if (thisv->blinkTimer <= 2) {
        thisv->leftEyeState = thisv->rightEyeState = thisv->blinkTimer;
    }
    switch (thisv->eyeExpression) {
        case ZL4_EYES_NEUTRAL:
            if (thisv->blinkTimer == 0) {
                thisv->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;
        case ZL4_EYES_SHUT:
            if (thisv->blinkTimer == 0) {
                thisv->leftEyeState = thisv->rightEyeState = ZL4_EYE_SHUT;
            }
            break;
        case ZL4_EYES_LOOK_LEFT:
            if (thisv->blinkTimer == 0) {
                thisv->leftEyeState = ZL4_EYE_LOOK_OUT;
                thisv->rightEyeState = ZL4_EYE_LOOK_IN;
            }
            break;
        case ZL4_EYES_LOOK_RIGHT:
            if (thisv->blinkTimer == 0) {
                thisv->leftEyeState = ZL4_EYE_LOOK_IN;
                thisv->rightEyeState = ZL4_EYE_LOOK_OUT;
            }
            break;
        case ZL4_EYES_WIDE:
            if (thisv->blinkTimer == 0) {
                thisv->leftEyeState = thisv->rightEyeState = ZL4_EYE_WIDE;
            }
            break;
        case ZL4_EYES_SQUINT:
            if (thisv->blinkTimer == 0) {
                thisv->leftEyeState = thisv->rightEyeState = ZL4_EYE_SQUINT;
            }
            break;
        case ZL4_EYES_OPEN:
            if (thisv->blinkTimer >= 3) {
                thisv->blinkTimer = ZL4_EYE_OPEN;
            }
            break;
    }
    switch (thisv->mouthExpression) {
        case ZL4_MOUTH_HAPPY:
            thisv->mouthState = 1;
            break;
        case ZL4_MOUTH_WORRIED:
            thisv->mouthState = 2;
            break;
        case ZL4_MOUTH_SURPRISED:
            thisv->mouthState = 3;
            break;
        default:
            thisv->mouthState = 0;
            break;
    }
}

void EnZl4_SetMove(EnZl4* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80B5BB78(EnZl4* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_1E0.unk_18 = player->actor.world.pos;
    func_80034A14(&thisv->actor, &thisv->unk_1E0, 2, 2);
}

void EnZl4_GetActionStartPos(CsCmdActorAction* action, Vec3f* vec) {
    vec->x = action->startPos.x;
    vec->y = action->startPos.y;
    vec->z = action->startPos.z;
}

s32 EnZl4_SetupFromLegendCs(EnZl4* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* playerx = &GET_PLAYER(globalCtx)->actor;
    s16 rotY;

    func_8002DF54(globalCtx, &thisv->actor, 8);
    playerx->world.pos = thisv->actor.world.pos;
    rotY = thisv->actor.shape.rot.y;
    playerx->world.pos.x += 56.0f * Math_SinS(rotY);
    playerx->world.pos.z += 56.0f * Math_CosS(rotY);

    player->linearVelocity = playerx->speedXZ = 0.0f;

    EnZl4_SetCsCameraMove(globalCtx, 5);
    ShrinkWindow_SetVal(0x20);
    Interface_ChangeAlpha(2);
    thisv->talkTimer2 = 0;
    return true;
}

s32 EnZl4_InMovingAnim(EnZl4* thisv) {
    if ((thisv->skelAnime.animation == &gChildZeldaAnim_01B874) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01BCF0) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_0125E4) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_012E58) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_015F14) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_0169B4) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_016D08) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01805C) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01A2FC) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01AAE0) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01CE08) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_018898) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_01910C) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_00F0A4) ||
        (thisv->skelAnime.animation == &gChildZeldaAnim_00F894)) {
        return true;
    }
    return false;
}

void EnZl4_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZl4* thisv = (EnZl4*)thisx;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gChildZeldaSkel, NULL, thisv->jointTable, thisv->morphTable, 18);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_21);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.targetMode = 6;
    thisv->actor.textId = -1;
    thisv->eyeExpression = thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;

    if (gSaveContext.sceneSetupIndex >= 4) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
        thisv->actionFunc = EnZl4_TheEnd;
    } else if (gSaveContext.eventChkInf[4] & 1) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
        thisv->actionFunc = EnZl4_Idle;
    } else {
        if (gSaveContext.entranceIndex != 0x5F0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_21);
            thisv->csState = ZL4_CS_WAIT;
            thisv->talkState = 0;
        } else {
            EnZl4_SetupFromLegendCs(thisv, globalCtx);
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
            thisv->csState = ZL4_CS_LEGEND;
            thisv->talkState = 0;
        }
        thisv->actionFunc = EnZl4_Cutscene;
    }
}

void EnZl4_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZl4* thisv = (EnZl4*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnZl4_SetNextAnim(EnZl4* thisv, s32 nextAnim) {
    if (!Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        return false;
    }
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, nextAnim);
    return true;
}

void EnZl4_ReverseAnimation(EnZl4* thisv) {
    f32 tempFrame = thisv->skelAnime.startFrame;

    thisv->skelAnime.startFrame = thisv->skelAnime.endFrame;
    thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
    thisv->skelAnime.endFrame = tempFrame;
    thisv->skelAnime.playSpeed = -1.0f;
}

s32 EnZl4_CsWaitForPlayer(EnZl4* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* playerx = &GET_PLAYER(globalCtx)->actor;
    s16 rotY;
    s16 yawDiff;
    s16 absYawDiff;

    if (!Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        yawDiff = (f32)thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);
        if ((playerx->world.pos.y != thisv->actor.world.pos.y) || (absYawDiff >= 0x3FFC)) {
            return false;
        } else {
            func_8002F2CC(&thisv->actor, globalCtx, thisv->collider.dim.radius + 60.0f);
            return false;
        }
    }
    playerx->world.pos = thisv->actor.world.pos;
    rotY = thisv->actor.shape.rot.y;
    playerx->world.pos.x += 56.0f * Math_SinS(rotY);
    playerx->world.pos.z += 56.0f * Math_CosS(rotY);
    playerx->speedXZ = 0.0f;
    player->linearVelocity = 0.0f;
    return true;
}

s32 EnZl4_CsMeetPlayer(EnZl4* thisv, GlobalContext* globalCtx) {
    switch (thisv->talkState) {
        case 0:
            if (thisv->skelAnime.curFrame == 50.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_MEET);
            }
            if (!EnZl4_SetNextAnim(thisv, ZL4_ANIM_4)) {
                break;
            } else {
                Message_StartTextbox(globalCtx, 0x702E, NULL);
                thisv->talkState++;
            }
            break;
        case 1:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 1);
                Message_StartTextbox(globalCtx, 0x702F, NULL);
                thisv->talkTimer2 = 0;
                thisv->talkState++;
            }
            break;
        case 2:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardMeetCs);
                gSaveContext.cutsceneTrigger = 1;
                EnZl4_SetCsCameraMove(globalCtx, 0);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkTimer2 = 0;
                thisv->talkState++;
            }
            break;
        case 3:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 45) {
                Message_StartTextbox(globalCtx, 0x70F9, NULL);
                thisv->talkState++;
            }
            break;
        case 4:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraMove(globalCtx, 1);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkTimer2 = 0;
                thisv->talkState++;
            }
            break;
        case 5:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 10) {
                Message_StartTextbox(globalCtx, 0x70FA, NULL);
                thisv->talkState++;
            }
            break;
        case 6:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 2);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_22);
                thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                thisv->talkTimer2 = 0;
                thisv->talkState++;
                Message_StartTextbox(globalCtx, 0x70FB, NULL);
            }
            break;
    }
    return (thisv->talkState == 7) ? 1 : 0;
}

s32 EnZl4_CsAskStone(EnZl4* thisv, GlobalContext* globalCtx) {
    switch (thisv->talkState) {
        case 0:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_25)) {
                thisv->talkState++;
            }
        case 1:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 3);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkTimer1 = 40;
                thisv->talkState = 2;
            }
            break;
        case 2:
            if (DECR(thisv->talkTimer1) == 0) {
                Message_StartTextbox(globalCtx, 0x7030, NULL);
                thisv->talkState++;
            }
            break;
        case 3:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else if (globalCtx->msgCtx.choiceIndex == 0) {
                EnZl4_SetCsCameraAngle(globalCtx, 4);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_28);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_SQUINT;
                thisv->mouthExpression = ZL4_MOUTH_HAPPY;
                Message_StartTextbox(globalCtx, 0x7032, NULL);
                thisv->talkState = 7;
            } else {
                EnZl4_SetCsCameraAngle(globalCtx, 2);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_9);
                thisv->mouthExpression = ZL4_MOUTH_WORRIED;
                Message_StartTextbox(globalCtx, 0x7031, NULL);
                thisv->talkState++;
            }
            break;
        case 4:
            if (thisv->skelAnime.curFrame == 16.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_QUESTION);
            }
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_10)) {
                thisv->talkState++;
            }
        case 5:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_9);
                thisv->mouthExpression = ZL4_MOUTH_WORRIED;
                EnZl4_ReverseAnimation(thisv);
                thisv->talkState = 6;
            }
            break;
        case 6:
            thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
            EnZl4_SetCsCameraAngle(globalCtx, 3);
            Message_StartTextbox(globalCtx, 0x7030, NULL);
            thisv->talkState = 12;
            break;
        case 12:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_25)) {
                thisv->talkState = 13;
            }
        case 13:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else if (globalCtx->msgCtx.choiceIndex == 0) {
                EnZl4_SetCsCameraAngle(globalCtx, 4);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_28);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_SQUINT;
                thisv->mouthExpression = ZL4_MOUTH_HAPPY;
                Message_StartTextbox(globalCtx, 0x7032, NULL);
                thisv->talkState = 7;
            } else {
                EnZl4_SetCsCameraAngle(globalCtx, 2);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_9);
                thisv->mouthExpression = ZL4_MOUTH_WORRIED;
                Message_StartTextbox(globalCtx, 0x7031, NULL);
                thisv->talkState = 4;
            }
            break;
        case 7:
            if (thisv->skelAnime.curFrame == 17.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_SMILE_0);
            }
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_29)) {
                thisv->talkState++;
            }
        case 8:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraMove(globalCtx, 2);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                Message_StartTextbox(globalCtx, 0x70FC, NULL);
                thisv->talkState = 9;
            }
            break;
        case 9:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 5);
                Message_StartTextbox(globalCtx, 0x70FD, NULL);
                thisv->talkState++;
            }
            break;
        case 10:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_5);
                thisv->eyeExpression = ZL4_EYES_OPEN;
                thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
                Message_StartTextbox(globalCtx, 0x70FE, NULL);
                thisv->talkState++;
            }
            break;
    }
    return (thisv->talkState == 11) ? 1 : 0;
}

s32 EnZl4_CsAskName(EnZl4* thisv, GlobalContext* globalCtx) {
    switch (thisv->talkState) {
        case 0:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_4)) {
                thisv->talkState++;
            }
            break;
        case 1:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 6);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_1);
                thisv->blinkTimer = 11;
                thisv->eyeExpression = ZL4_EYES_SQUINT;
                thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                Message_StartTextbox(globalCtx, 0x70FF, NULL);
                thisv->talkState++;
            }
            break;
        case 2:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_2)) {
                thisv->talkState++;
            }
        case 3:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_16);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState = 4;
            }
            break;
        case 4:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_17)) {
                Message_StartTextbox(globalCtx, 0x2073, NULL);
                thisv->talkState++;
            }
            break;
        case 5:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraMove(globalCtx, 3);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkTimer2 = 0;
                thisv->talkState = 6;
            }
            break;
        case 6:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 15) {
                Message_StartTextbox(globalCtx, 0x2074, NULL);
                thisv->talkState++;
            }
            break;
        case 7:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_6);
                thisv->mouthExpression = ZL4_MOUTH_HAPPY;
                Message_StartTextbox(globalCtx, 0x2075, NULL);
                thisv->talkState++;
            }
            break;
        case 8:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_25)) {
                thisv->talkState++;
            }
        case 9:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x7033, NULL);
                thisv->talkState = 10;
            }
            break;
        case 10:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                if (globalCtx->msgCtx.choiceIndex == 0) {
                    EnZl4_SetCsCameraMove(globalCtx, 4);
                    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_33);
                    thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                    thisv->talkTimer2 = 0;
                    thisv->talkState = 15;
                } else {
                    EnZl4_SetCsCameraAngle(globalCtx, 6);
                    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                    thisv->talkTimer1 = 20;
                    thisv->talkState++;
                    thisv->skelAnime.playSpeed = 0.0f;
                }
            }
            break;
        case 11:
            if (DECR(thisv->talkTimer1) == 0) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_11);
                thisv->blinkTimer = 11;
                thisv->eyeExpression = ZL4_EYES_LOOK_RIGHT;
                thisv->mouthExpression = ZL4_MOUTH_WORRIED;
                Message_StartTextbox(globalCtx, 0x7034, NULL);
                thisv->talkState++;
            }
            break;
        case 12:
            if (thisv->skelAnime.curFrame == 5.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_SIGH_0);
            }
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_12)) {
                thisv->talkState++;
            }
        case 13:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_6);
                thisv->blinkTimer = 3;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_HAPPY;
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState = 14;
            }
            break;
        case 14:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_25)) {
                Message_StartTextbox(globalCtx, 0x7033, NULL);
                thisv->talkState = 10;
            }
            break;
        case 15:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 30) {
                Message_StartTextbox(globalCtx, 0x7035, NULL);
                thisv->talkState++;
            }
            break;
        case 16:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
        case 17:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 == 130) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                globalCtx->nextEntranceIndex = 0xA0;
                gSaveContext.nextCutsceneIndex = 0xFFF7;
                globalCtx->sceneLoadFlag = 0x14;
                globalCtx->fadeTransition = 3;
            }
            break;
    }
    if ((thisv->talkTimer2 == 17) && (thisv->talkTimer2 > 130)) {
        return true;
    }
    return false;
}

s32 EnZl4_CsTellLegend(EnZl4* thisv, GlobalContext* globalCtx) {
    Camera* activeCam = GET_ACTIVE_CAM(globalCtx);

    switch (thisv->talkState) {
        case 0:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 60) {
                Message_StartTextbox(globalCtx, 0x7037, NULL);
                thisv->talkState++;
            }
            break;
        case 1:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 7);
                Message_StartTextbox(globalCtx, 0x2076, NULL);
                thisv->talkState++;
            }
            break;
        case 2:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraMove(globalCtx, 6);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
            break;
        case 3:
            if (activeCam->animState == 2) {
                Message_StartTextbox(globalCtx, 0x2077, NULL);
                thisv->talkState++;
            }
            break;
        case 4:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else if (globalCtx->msgCtx.choiceIndex == 0) {
                EnZl4_SetCsCameraAngle(globalCtx, 8);
                Message_StartTextbox(globalCtx, 0x7005, NULL);
                thisv->talkState = 9;
            } else {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_5);
                thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
                Message_StartTextbox(globalCtx, 0x7038, NULL);
                thisv->talkState++;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_HURRY);
            }
            break;
        case 5:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_4)) {
                thisv->talkState++;
            }
        case 6:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_33);
                thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                Message_StartTextbox(globalCtx, 0x7037, NULL);
                thisv->talkState++;
            }
            break;
        case 7:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x2076, NULL);
                thisv->talkState++;
            }
            break;
        case 8:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x2077, NULL);
                thisv->talkState = 4;
            }
            break;
        case 9:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_26);
                Message_StartTextbox(globalCtx, 0x2078, NULL);
                thisv->talkState++;
            }
            break;
        case 10:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_27)) {
                thisv->talkState++;
            }
        case 11:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else if (globalCtx->msgCtx.choiceIndex == 0) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState = 13;
            } else {
                Message_StartTextbox(globalCtx, 0x700B, NULL);
                thisv->talkState = 12;
            }
            break;
        case 12:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState = 13;
            }
            break;
    }
    return (thisv->talkState == 13) ? 1 : 0;
}

s32 EnZl4_CsLookWindow(EnZl4* thisv, GlobalContext* globalCtx) {
    switch (thisv->talkState) {
        case 0:
            EnZl4_SetCsCameraMove(globalCtx, 7);
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardWindowCs);
            gSaveContext.cutsceneTrigger = 1;
            thisv->talkState++;
            break;
        case 1:
            if (globalCtx->csCtx.state != CS_STATE_IDLE) {
                if (globalCtx->csCtx.frames == 90) {
                    globalCtx->csCtx.state = CS_STATE_UNSKIPPABLE_INIT;
                }
            } else {
                globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardGanonCs);
                gSaveContext.cutsceneTrigger = 1;
                thisv->talkState++;
                func_8002DF54(globalCtx, &thisv->actor, 8);
            }
            break;
        case 2:
            if (globalCtx->csCtx.state != CS_STATE_IDLE) {
                if (globalCtx->csCtx.frames == 209) {
                    globalCtx->csCtx.state = CS_STATE_UNSKIPPABLE_INIT;
                }
            } else {
                func_800AA000(0.0f, 0xA0, 0xA, 0x28);
                func_8002DF54(globalCtx, &thisv->actor, 1);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_30);
                EnZl4_SetCsCameraAngle(globalCtx, 11);
                Message_StartTextbox(globalCtx, 0x7039, NULL);
                thisv->talkState++;
            }
            break;
        case 3:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
            break;
    }
    return (thisv->talkState == 4) ? 1 : 0;
}

s32 EnZl4_CsWarnAboutGanon(EnZl4* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 rotY;

    switch (thisv->talkState) {
        case 0:
            player->actor.world.pos = thisv->actor.world.pos;
            rotY = thisv->actor.shape.rot.y - 0x3FFC;
            player->actor.world.pos.x += 34.0f * Math_SinS(rotY);
            player->actor.world.pos.z += 34.0f * Math_CosS(rotY);
            EnZl4_SetCsCameraMove(globalCtx, 8);
            thisv->blinkTimer = 0;
            thisv->eyeExpression = ZL4_EYES_WIDE;
            thisv->mouthExpression = ZL4_MOUTH_WORRIED;
            thisv->talkTimer2 = 0;
            thisv->talkState++;
            Message_StartTextbox(globalCtx, 0x2079, NULL);
        case 1:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 20) {
                thisv->talkState++;
            }
            break;
        case 2:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraMove(globalCtx, 9);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkTimer2 = 0;
                thisv->talkState++;
            }
            break;
        case 3:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 20) {
                Message_StartTextbox(globalCtx, 0x207A, NULL);
                thisv->talkState++;
            }
            break;
        case 4:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 12);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_23);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
            break;
        case 5:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_24)) {
                Message_StartTextbox(globalCtx, 0x207B, NULL);
                thisv->talkState++;
            }
            break;
        case 6:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x703A, NULL);
                thisv->talkState++;
            }
            break;
        case 7:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else if (globalCtx->msgCtx.choiceIndex == 0) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_31);
                thisv->blinkTimer = 11;
                thisv->eyeExpression = ZL4_EYES_SQUINT;
                thisv->mouthExpression = ZL4_MOUTH_HAPPY;
                Message_StartTextbox(globalCtx, 0x703B, NULL);
                thisv->talkState = 11;
            } else {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_13);
                thisv->blinkTimer = 11;
                thisv->eyeExpression = ZL4_EYES_LOOK_LEFT;
                thisv->mouthExpression = ZL4_MOUTH_WORRIED;
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
            break;
        case 8:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_15)) {
                thisv->blinkTimer = 3;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
                Message_StartTextbox(globalCtx, 0x7073, NULL);
                thisv->talkState++;
            }
            break;
        case 9:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_14);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState++;
            }
            break;
        case 10:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_24)) {
                Message_StartTextbox(globalCtx, 0x703A, NULL);
                thisv->talkState = 7;
            }
            break;
        case 11:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_32)) {
                thisv->talkState++;
            }
        case 12:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                thisv->talkState = 13;
            }
            break;
    }
    return (thisv->talkState == 13) ? 1 : 0;
}

s32 EnZl4_CsMakePlan(EnZl4* thisv, GlobalContext* globalCtx) {
    switch (thisv->talkState) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_18);
            thisv->blinkTimer = 0;
            thisv->eyeExpression = ZL4_EYES_NEUTRAL;
            thisv->mouthExpression = ZL4_MOUTH_WORRIED;
            EnZl4_SetCsCameraMove(globalCtx, 10);
            thisv->talkTimer2 = 0;
            thisv->talkState++;
        case 1:
            thisv->talkTimer2++;
            if (thisv->talkTimer2 >= 10) {
                Message_StartTextbox(globalCtx, 0x7123, NULL);
                thisv->talkState++;
            }
            break;
        case 2:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                EnZl4_SetCsCameraAngle(globalCtx, 13);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_19);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
                Message_StartTextbox(globalCtx, 0x207C, NULL);
                thisv->talkState++;
            }
            break;
        case 3:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_20)) {
                thisv->talkState++;
            }
        case 4:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x207D, NULL);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_7);
                thisv->blinkTimer = 0;
                thisv->eyeExpression = ZL4_EYES_NEUTRAL;
                thisv->mouthExpression = ZL4_MOUTH_NEUTRAL;
                thisv->talkState = 5;
                thisv->unk_20F = thisv->lastAction = 0;
            }
            break;
        case 5:
            if (EnZl4_SetNextAnim(thisv, ZL4_ANIM_8)) {
                thisv->talkState++;
            }
        case 6:
            if (!((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx))) {
                break;
            } else {
                Camera_ChangeSetting(GET_ACTIVE_CAM(globalCtx), 1);
                thisv->talkState = 7;
                globalCtx->talkWithPlayer(globalCtx, &thisv->actor);
                func_8002F434(&thisv->actor, globalCtx, GI_LETTER_ZELDA, fabsf(thisv->actor.xzDistToPlayer) + 1.0f,
                              fabsf(thisv->actor.yDistToPlayer) + 1.0f);
                globalCtx->msgCtx.stateTimer = 4;
                globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            }
            break;
        case 7:
            if (Actor_HasParent(&thisv->actor, globalCtx)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_0);
                thisv->talkState++;
            } else {
                func_8002F434(&thisv->actor, globalCtx, GI_LETTER_ZELDA, fabsf(thisv->actor.xzDistToPlayer) + 1.0f,
                              fabsf(thisv->actor.yDistToPlayer) + 1.0f);
            }
            // no break here is required for matching
    }
    return (thisv->talkState == 8) ? 1 : 0;
}

void EnZl4_Cutscene(EnZl4* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    switch (thisv->csState) {
        case ZL4_CS_WAIT:
            if (EnZl4_CsWaitForPlayer(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_START:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_3);
            thisv->blinkTimer = 0;
            thisv->eyeExpression = ZL4_EYES_NEUTRAL;
            thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
            Audio_PlayFanfare(NA_BGM_APPEAR);
            EnZl4_SetCsCameraAngle(globalCtx, 0);
            Interface_ChangeAlpha(2);
            ShrinkWindow_SetVal(0x20);
            thisv->talkState = 0;
            thisv->csState++;
            break;
        case ZL4_CS_MEET:
            if (EnZl4_CsMeetPlayer(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_STONE:
            if (EnZl4_CsAskStone(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_NAMES:
            if (EnZl4_CsAskName(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_LEGEND:
            if (EnZl4_CsTellLegend(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_WINDOW:
            if (EnZl4_CsLookWindow(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_GANON:
            if (EnZl4_CsWarnAboutGanon(thisv, globalCtx)) {
                thisv->talkState = 0;
                thisv->csState++;
            }
            break;
        case ZL4_CS_PLAN:
            if (EnZl4_CsMakePlan(thisv, globalCtx)) {
                func_8002DF54(globalCtx, &thisv->actor, 7);
                gSaveContext.unk_13EE = 0x32;
                gSaveContext.eventChkInf[4] |= 1;
                thisv->actionFunc = EnZl4_Idle;
            }
            break;
    }
    thisv->unk_1E0.unk_18 = player->actor.world.pos;
    func_80034A14(&thisv->actor, &thisv->unk_1E0, 2, (thisv->csState == ZL4_CS_WINDOW) ? 2 : 1);
    if (EnZl4_InMovingAnim(thisv)) {
        EnZl4_SetMove(thisv, globalCtx);
    }
}

void EnZl4_Idle(EnZl4* thisv, GlobalContext* globalCtx) {
    func_800343CC(globalCtx, &thisv->actor, &thisv->unk_1E0.unk_00, thisv->collider.dim.radius + 60.0f, EnZl4_GetText,
                  func_80B5B9B0);
    func_80B5BB78(thisv, globalCtx);
}

void EnZl4_TheEnd(EnZl4* thisv, GlobalContext* globalCtx) {
    s32 animIndex[] = { ZL4_ANIM_0, ZL4_ANIM_0, ZL4_ANIM_0,  ZL4_ANIM_0,  ZL4_ANIM_0,
                        ZL4_ANIM_0, ZL4_ANIM_0, ZL4_ANIM_26, ZL4_ANIM_21, ZL4_ANIM_3 };
    CsCmdActorAction* npcAction;
    Vec3f pos;

    if (SkelAnime_Update(&thisv->skelAnime) && (thisv->skelAnime.animation == &gChildZeldaAnim_010DF8)) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ZL4_ANIM_4);
    }
    if (EnZl4_InMovingAnim(thisv)) {
        EnZl4_SetMove(thisv, globalCtx);
    }
    if (globalCtx->csCtx.frames == 100) {
        thisv->eyeExpression = ZL4_EYES_LOOK_LEFT;
    }
    if (globalCtx->csCtx.frames == 450) {
        thisv->blinkTimer = 3;
        thisv->eyeExpression = ZL4_EYES_NEUTRAL;
        thisv->mouthExpression = ZL4_MOUTH_SURPRISED;
    }
    npcAction = globalCtx->csCtx.npcActions[0];
    if (npcAction != NULL) {
        EnZl4_GetActionStartPos(npcAction, &pos);
        if (thisv->lastAction == 0) {
            thisv->actor.world.pos = thisv->actor.home.pos = pos;
        }
        if (thisv->lastAction != npcAction->action) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animIndex[npcAction->action]);
            thisv->lastAction = npcAction->action;
        }
        thisv->actor.velocity.x = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.velocity.z = 0.0f;
    }
}

void EnZl4_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZl4* thisv = (EnZl4*)thisx;

    if (thisv->actionFunc != EnZl4_TheEnd) {
        SkelAnime_Update(&thisv->skelAnime);
    }
    EnZl4_UpdateFace(thisv);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    thisv->actionFunc(thisv, globalCtx);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnZl4_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnZl4* thisv = (EnZl4*)thisx;
    Vec3s sp1C;

    if (limbIndex == 17) {
        sp1C = thisv->unk_1E0.unk_08;
        Matrix_Translate(900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateX((sp1C.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((sp1C.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_Translate(-900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 10) {
        sp1C = thisv->unk_1E0.unk_0E;
        Matrix_RotateY((sp1C.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateX((sp1C.x / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
    }
    if ((limbIndex >= 3) && (limbIndex < 7)) {
        *dList = NULL;
    }
    return false;
}

void EnZl4_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnZl4* thisv = (EnZl4*)thisx;

    if (limbIndex == 17) {
        Matrix_MultVec3f(&zeroVec, &thisv->actor.focus.pos);
    }
}

void EnZl4_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnZl4* thisv = (EnZl4*)thisx;
    void* mouthTex[] = { gChildZeldaMouthNeutralTex, gChildZeldaMouthHappyTex, gChildZeldaMouthWorriedTex,
                         gChildZeldaMouthSurprisedTex };
    void* eyeTex[] = {
        gChildZeldaEyeOpenTex,   gChildZeldaEyeBlinkTex, gChildZeldaEyeShutTex, gChildZeldaEyeWideTex,
        gChildZeldaEyeSquintTex, gChildZeldaEyeOutTex,   gChildZeldaEyeInTex,
    };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zl4.c", 2012);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex[thisv->rightEyeState]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex[thisv->leftEyeState]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex[thisv->mouthState]));
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnZl4_OverrideLimbDraw, EnZl4_PostLimbDraw, thisv);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zl4.c", 2043);
}
