/*
 * File: z_en_ge2.c
 * Overlay: ovl_En_Ge2
 * Description: Purple-clothed Gerudo
 */

#include "z_en_ge2.h"
#include "vt.h"
#include "objects/object_gla/object_gla.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

#define GE2_STATE_ANIMCOMPLETE (1 << 1)
#define GE2_STATE_KO (1 << 2)
#define GE2_STATE_CAPTURING (1 << 3)
#define GE2_STATE_TALKED (1 << 4)

typedef enum {
    /* 0 */ GE2_TYPE_PATROLLING,
    /* 1 */ GE2_TYPE_STATIONARY,
    /* 2 */ GE2_TYPE_GERUDO_CARD_GIVER
} EnGe2Type;

typedef enum {
    /* 0 */ GE2_ACTION_WALK,
    /* 1 */ GE2_ACTION_ABOUTTURN,
    /* 2 */ GE2_ACTION_TURNPLAYERSPOTTED,
    /* 3 */ GE2_ACTION_KNOCKEDOUT,
    /* 4 */ GE2_ACTION_CAPTURETURN,
    /* 5 */ GE2_ACTION_CAPTURECHARGE,
    /* 6 */ GE2_ACTION_CAPTURECLOSE,
    /* 7 */ GE2_ACTION_STAND,
    /* 8 */ GE2_ACTION_WAITLOOKATPLAYER
} EnGe2Action;

void EnGe2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGe2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGe2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGe2_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 EnGe2_CheckCarpentersFreed(void);
void EnGe2_CaptureClose(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_CaptureCharge(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_CaptureTurn(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_KnockedOut(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_TurnPlayerSpotted(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_AboutTurn(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_Walk(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_Stand(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_WaitLookAtPlayer(EnGe2* thisv, GlobalContext* globalCtx);
void EnGe2_ForceTalk(EnGe2* thisv, GlobalContext* globalCtx);

// Update functions
void EnGe2_UpdateFriendly(Actor* thisx, GlobalContext* globalCtx);
void EnGe2_UpdateAfterTalk(Actor* thisx, GlobalContext* globalCtx);
void EnGe2_UpdateStunned(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Ge2_InitVars = {
    ACTOR_EN_GE2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GLA,
    sizeof(EnGe2),
    (ActorFunc)EnGe2_Init,
    (ActorFunc)EnGe2_Destroy,
    (ActorFunc)EnGe2_Update,
    (ActorFunc)EnGe2_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x000007A2, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static EnGe2ActionFunc sActionFuncs[] = {
    EnGe2_Walk,         EnGe2_AboutTurn,   EnGe2_TurnPlayerSpotted,
    EnGe2_KnockedOut,   EnGe2_CaptureTurn, EnGe2_CaptureCharge,
    EnGe2_CaptureClose, EnGe2_Stand,       EnGe2_WaitLookAtPlayer,
};

static const AnimationHeader* sAnimations[] = {
    &gGerudoPurpleWalkingAnim,         &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
    &gGerudoPurpleFallingToGroundAnim, &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleChargingAnim,
    &gGerudoPurpleLookingAboutAnim,    &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
};

static u8 sAnimModes[] = {
    ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP,
    ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_ONCE,
};

void EnGe2_ChangeAction(EnGe2* thisv, s32 i) {
    thisv->actionFunc = sActionFuncs[i];
    Animation_Change(&thisv->skelAnime, sAnimations[i], 1.0f, 0.0f, Animation_GetLastFrame(sAnimations[i]),
                     sAnimModes[i], -8.0f);
    thisv->stateFlags &= ~GE2_STATE_ANIMCOMPLETE;
}

void EnGe2_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe2* thisv = (EnGe2*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGerudoPurpleSkel, NULL, thisv->jointTable, thisv->morphTable, 22);
    Animation_PlayLoop(&thisv->skelAnime, &gGerudoPurpleWalkingAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);

    if (globalCtx->sceneNum == SCENE_SPOT09) {
        thisv->actor.uncullZoneForward = 1000.0f;
    } else {
        thisv->actor.uncullZoneForward = 1200.0f;
    }

    thisv->yDetectRange = (thisv->actor.world.rot.z + 1) * 40.0f;
    thisv->actor.world.rot.z = 0;
    thisv->actor.shape.rot.z = 0;

    switch (thisv->actor.params & 0xFF) {
        case GE2_TYPE_PATROLLING:
            EnGe2_ChangeAction(thisv, GE2_ACTION_WALK);
            if (EnGe2_CheckCarpentersFreed()) {
                thisv->actor.update = EnGe2_UpdateFriendly;
                thisv->actor.targetMode = 6;
            }
            break;
        case GE2_TYPE_STATIONARY:
            EnGe2_ChangeAction(thisv, GE2_ACTION_STAND);
            if (EnGe2_CheckCarpentersFreed()) {
                thisv->actor.update = EnGe2_UpdateFriendly;
                thisv->actor.targetMode = 6;
            }
            break;
        case GE2_TYPE_GERUDO_CARD_GIVER:
            EnGe2_ChangeAction(thisv, GE2_ACTION_WAITLOOKATPLAYER);
            thisv->actor.update = EnGe2_UpdateAfterTalk;
            thisv->actionFunc = EnGe2_ForceTalk;
            thisv->actor.targetMode = 6;
            break;
        default:
            ASSERT(0, "0", "../z_en_ge2.c", 418);
    }

    thisv->stateFlags = 0;
    thisv->unk_304 = 0; // Set and not used
    thisv->walkTimer = 0;
    thisv->playerSpottedParam = 0;
    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.gravity = -1.0f;
    thisv->walkDirection = thisv->actor.world.rot.y;
    thisv->walkDuration = ((thisv->actor.params & 0xFF00) >> 8) * 10;
}

void EnGe2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGe2* thisv = (EnGe2*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

// Detection/check functions

s32 Ge2_DetectPlayerInAction(GlobalContext* globalCtx, EnGe2* thisv) {
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < thisv->actor.xzDistToPlayer) {
        return 0;
    }

    if (thisv->actor.xzDistToPlayer < 50.0f) {
        return 2;
    }

    if (func_8002DDE4(globalCtx)) {
        return 1;
    }
    return 0;
}

s32 Ge2_DetectPlayerInUpdate(GlobalContext* globalCtx, EnGe2* thisv, Vec3f* pos, s16 yRot, f32 yDetectRange) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f posResult;
    CollisionPoly* outPoly;
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < thisv->actor.xzDistToPlayer) {
        return 0;
    }

    if (yDetectRange < ABS(thisv->actor.yDistToPlayer)) {
        return 0;
    }

    if (ABS((s16)(thisv->actor.yawTowardsPlayer - yRot)) > 0x2000) {
        return 0;
    }

    if (BgCheck_AnyLineTest1(&globalCtx->colCtx, pos, &player->bodyPartsPos[7], &posResult, &outPoly, 0)) {
        return 0;
    }
    return 1;
}

s32 EnGe2_CheckCarpentersFreed(void) {
    if ((u8)(gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        return 1;
    }
    return 0;
}

// Actions

void EnGe2_CaptureClose(EnGe2* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        func_8006D074(globalCtx);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            globalCtx->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) {
            globalCtx->nextEntranceIndex = 0x5F8;
        } else {
            globalCtx->nextEntranceIndex = 0x3B4;
        }

        globalCtx->fadeTransition = 0x26;
        globalCtx->sceneLoadFlag = 0x14;
    }
}

void EnGe2_CaptureCharge(EnGe2* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if (thisv->actor.xzDistToPlayer < 50.0f) {
        EnGe2_ChangeAction(thisv, GE2_ACTION_CAPTURECLOSE);
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        func_8006D074(globalCtx);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            globalCtx->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) {
            globalCtx->nextEntranceIndex = 0x5F8;
        } else {
            globalCtx->nextEntranceIndex = 0x3B4;
        }

        globalCtx->fadeTransition = 0x26;
        globalCtx->sceneLoadFlag = 0x14;
    }
}

void EnGe2_CaptureTurn(EnGe2* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (thisv->actor.world.rot.y == thisv->actor.yawTowardsPlayer) {
        EnGe2_ChangeAction(thisv, GE2_ACTION_CAPTURECHARGE);
        thisv->timer = 50;
        thisv->actor.speedXZ = 4.0f;
    }
}

void EnGe2_KnockedOut(EnGe2* thisv, GlobalContext* globalCtx) {
    static Vec3f effectVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f effectAccel = { 0.0f, -0.025f, 0.0f };
    static Color_RGBA8 effectPrimColor = { 255, 255, 255, 0 };
    static Color_RGBA8 effectEnvColor = { 255, 150, 0, 0 };
    s32 effectAngle;
    Vec3f effectPos;

    thisv->actor.flags &= ~ACTOR_FLAG_0;
    if (thisv->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        effectAngle = (globalCtx->state.frames) * 0x2800;
        effectPos.x = thisv->actor.focus.pos.x + (Math_CosS(effectAngle) * 5.0f);
        effectPos.y = thisv->actor.focus.pos.y + 10.0f;
        effectPos.z = thisv->actor.focus.pos.z + (Math_SinS(effectAngle) * 5.0f);
        EffectSsKiraKira_SpawnDispersed(globalCtx, &effectPos, &effectVelocity, &effectAccel, &effectPrimColor,
                                        &effectEnvColor, 1000, 16);
    }
}

void EnGe2_TurnPlayerSpotted(EnGe2* thisv, GlobalContext* globalCtx) {
    s32 playerSpotted;

    thisv->actor.speedXZ = 0.0f;

    if (thisv->stateFlags & GE2_STATE_TALKED) {
        thisv->stateFlags &= ~GE2_STATE_TALKED;
    } else {
        playerSpotted = Ge2_DetectPlayerInAction(globalCtx, thisv);

        if (playerSpotted != 0) {
            thisv->timer = 100;
            thisv->yawTowardsPlayer = thisv->actor.yawTowardsPlayer;

            if (thisv->playerSpottedParam < playerSpotted) {
                thisv->playerSpottedParam = playerSpotted;
            }
        } else if (thisv->actor.world.rot.y == thisv->yawTowardsPlayer) {
            thisv->playerSpottedParam = 0;
            EnGe2_ChangeAction(thisv, GE2_ACTION_ABOUTTURN);
            return;
        }
    }

    switch (thisv->playerSpottedParam) {
        case 1:
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->yawTowardsPlayer, 2, 0x200, 0x100);
            break;
        case 2:
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->yawTowardsPlayer, 2, 0x600, 0x180);
            break;
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnGe2_AboutTurn(EnGe2* thisv, GlobalContext* globalCtx) {
    s32 playerSpotted;

    thisv->actor.speedXZ = 0.0f;
    playerSpotted = Ge2_DetectPlayerInAction(globalCtx, thisv);

    if (playerSpotted != 0) {
        EnGe2_ChangeAction(thisv, GE2_ACTION_TURNPLAYERSPOTTED);
        thisv->timer = 100;
        thisv->playerSpottedParam = playerSpotted;
        thisv->yawTowardsPlayer = thisv->actor.yawTowardsPlayer;
    } else if (thisv->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->walkDirection, 2, 0x400, 0x200);
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }

    if (thisv->actor.shape.rot.y == thisv->walkDirection) {
        EnGe2_ChangeAction(thisv, GE2_ACTION_WALK);
    }
}

void EnGe2_Walk(EnGe2* thisv, GlobalContext* globalCtx) {
    u8 playerSpotted;

    playerSpotted = Ge2_DetectPlayerInAction(globalCtx, thisv);
    if (playerSpotted != 0) {
        thisv->actor.speedXZ = 0.0f;
        EnGe2_ChangeAction(thisv, GE2_ACTION_TURNPLAYERSPOTTED);
        thisv->timer = 100;
        thisv->playerSpottedParam = playerSpotted;
        thisv->yawTowardsPlayer = thisv->actor.yawTowardsPlayer;
    } else if (thisv->walkTimer >= thisv->walkDuration) {
        thisv->walkTimer = 0;
        thisv->walkDirection += 0x8000;
        EnGe2_ChangeAction(thisv, GE2_ACTION_ABOUTTURN);
        thisv->actor.speedXZ = 0.0f;
    } else {
        thisv->walkTimer++;
        thisv->actor.speedXZ = 2.0f;
    }
}

void EnGe2_Stand(EnGe2* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->walkDirection, 2, 0x400, 0x200);
}

void EnGe2_TurnToFacePlayer(EnGe2* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 6, 4000, 100);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2EE, thisv->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            Math_SmoothStepToS(&thisv->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            Math_SmoothStepToS(&thisv->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 12, 1000, 100);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
}

void EnGe2_LookAtPlayer(EnGe2* thisv, GlobalContext* globalCtx) {
    if ((ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4300) &&
        (thisv->actor.xzDistToPlayer < 200.0f)) {
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2EE, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->headRot.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2EE.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2EE.y, 0, 6, 6200, 100);
    }
}

void EnGe2_SetActionAfterTalk(EnGe2* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {

        switch (thisv->actor.params & 0xFF) {
            case GE2_TYPE_PATROLLING:
                EnGe2_ChangeAction(thisv, GE2_ACTION_ABOUTTURN);
                break;
            case GE2_TYPE_STATIONARY:
                EnGe2_ChangeAction(thisv, GE2_ACTION_STAND);
                break;
            case GE2_TYPE_GERUDO_CARD_GIVER:
                thisv->actionFunc = EnGe2_WaitLookAtPlayer;
                break;
        }
        thisv->actor.update = EnGe2_UpdateFriendly;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    }
    EnGe2_TurnToFacePlayer(thisv, globalCtx);
}

void EnGe2_WaitLookAtPlayer(EnGe2* thisv, GlobalContext* globalCtx) {
    EnGe2_LookAtPlayer(thisv, globalCtx);
}

void EnGe2_WaitTillCardGiven(EnGe2* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnGe2_SetActionAfterTalk;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
    }
}

void EnGe2_GiveCard(EnGe2* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = EnGe2_WaitTillCardGiven;
        func_8002F434(&thisv->actor, globalCtx, GI_GERUDO_CARD, 10000.0f, 50.0f);
    }
}

void EnGe2_ForceTalk(EnGe2* thisv, GlobalContext* globalCtx) {

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe2_GiveCard;
    } else {
        thisv->actor.textId = 0x6004;
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F1C4(&thisv->actor, globalCtx, 300.0f, 300.0f, 0);
    }
    EnGe2_LookAtPlayer(thisv, globalCtx);
}

void EnGe2_SetupCapturePlayer(EnGe2* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE2_STATE_CAPTURING;
    thisv->actor.speedXZ = 0.0f;
    EnGe2_ChangeAction(thisv, GE2_ACTION_CAPTURETURN);
    func_8002DF54(globalCtx, &thisv->actor, 95);
    func_80078884(NA_SE_SY_FOUND);
    Message_StartTextbox(globalCtx, 0x6000, &thisv->actor);
}

void EnGe2_MaintainColliderAndSetAnimState(EnGe2* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 40.0f, 25.0f, 40.0f, 5);

    if (!(thisv->stateFlags & GE2_STATE_ANIMCOMPLETE) && SkelAnime_Update(&thisv->skelAnime)) {
        thisv->stateFlags |= GE2_STATE_ANIMCOMPLETE;
    }
}

void EnGe2_MoveAndBlink(EnGe2* thisv, GlobalContext* globalCtx) {
    Actor_MoveForward(&thisv->actor);

    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }
    thisv->eyeIndex = thisv->blinkTimer;

    if (thisv->eyeIndex >= 3) {
        thisv->eyeIndex = 0;
    }
}

// Update functions

void EnGe2_UpdateFriendly(Actor* thisx, GlobalContext* globalCtx) {
    EnGe2* thisv = (EnGe2*)thisx;

    EnGe2_MaintainColliderAndSetAnimState(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if ((thisv->actor.params & 0xFF) == GE2_TYPE_PATROLLING) {
            thisv->actor.speedXZ = 0.0f;
            EnGe2_ChangeAction(thisv, GE2_ACTION_WAITLOOKATPLAYER);
        }
        thisv->actionFunc = EnGe2_SetActionAfterTalk;
        thisv->actor.update = EnGe2_UpdateAfterTalk;
    } else {
        thisv->actor.textId = 0x6005;

        if (thisv->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
    EnGe2_MoveAndBlink(thisv, globalCtx);
}

void EnGe2_UpdateAfterTalk(Actor* thisx, GlobalContext* globalCtx) {
    EnGe2* thisv = (EnGe2*)thisx;

    thisv->stateFlags |= GE2_STATE_TALKED;
    EnGe2_MaintainColliderAndSetAnimState(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    EnGe2_MoveAndBlink(thisv, globalCtx);
}

void EnGe2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGe2* thisv = (EnGe2*)thisx;
    s32 paramsType;

    EnGe2_MaintainColliderAndSetAnimState(thisv, globalCtx);

    if ((thisv->stateFlags & GE2_STATE_KO) || (thisv->stateFlags & GE2_STATE_CAPTURING)) {
        thisv->actionFunc(thisv, globalCtx);
    } else if (thisv->collider.base.acFlags & 2) {
        if ((thisv->collider.info.acHitInfo != NULL) && (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x80)) {
            Actor_SetColorFilter(&thisv->actor, 0, 120, 0, 400);
            thisv->actor.update = EnGe2_UpdateStunned;
            return;
        }

        EnGe2_ChangeAction(thisv, GE2_ACTION_KNOCKEDOUT);
        thisv->timer = 100;
        thisv->stateFlags |= GE2_STATE_KO;
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_SK_CRASH);
    } else {
        thisv->actionFunc(thisv, globalCtx);

        if (Ge2_DetectPlayerInUpdate(globalCtx, thisv, &thisv->actor.focus.pos, thisv->actor.shape.rot.y,
                                     thisv->yDetectRange)) {
            // "Discovered!"
            osSyncPrintf(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            EnGe2_SetupCapturePlayer(thisv, globalCtx);
        }

        if (((thisv->actor.params & 0xFF) == GE2_TYPE_STATIONARY) && (thisv->actor.xzDistToPlayer < 100.0f)) {
            // "Discovered!"
            osSyncPrintf(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            EnGe2_SetupCapturePlayer(thisv, globalCtx);
        }
    }

    if (!(thisv->stateFlags & GE2_STATE_KO)) {
        paramsType = thisv->actor.params & 0xFF; // Not necessary, but looks a bit nicer
        if ((paramsType == GE2_TYPE_PATROLLING) || (paramsType == GE2_TYPE_STATIONARY)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
    EnGe2_MoveAndBlink(thisv, globalCtx);

    if (EnGe2_CheckCarpentersFreed() && !(thisv->stateFlags & GE2_STATE_KO)) {
        thisv->actor.update = EnGe2_UpdateFriendly;
        thisv->actor.targetMode = 6;
    }
}

void EnGe2_UpdateStunned(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnGe2* thisv = (EnGe2*)thisx;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 40.0f, 25.0f, 40.0f, 5);

    if ((thisv->collider.base.acFlags & 2) &&
        ((thisv->collider.info.acHitInfo == NULL) || !(thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x80))) {
        thisv->actor.colorFilterTimer = 0;
        EnGe2_ChangeAction(thisv, GE2_ACTION_KNOCKEDOUT);
        thisv->timer = 100;
        thisv->stateFlags |= GE2_STATE_KO;
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_SK_CRASH);
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    if (EnGe2_CheckCarpentersFreed()) {
        thisv->actor.update = EnGe2_UpdateFriendly;
        thisv->actor.targetMode = 6;
        thisv->actor.colorFilterTimer = 0;
    } else if (thisv->actor.colorFilterTimer == 0) {
        thisv->actor.update = EnGe2_Update;
    }
}

s32 EnGe2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe2* thisv = (EnGe2*)thisx;

    if (limbIndex == 3) {
        rot->x += thisv->headRot.y;
        rot->z += thisv->headRot.x;
    }
    return 0;
}

void EnGe2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A343B0 = { 600.0f, 700.0f, 0.0f };
    EnGe2* thisv = (EnGe2*)thisx;

    if (limbIndex == 6) {
        Matrix_MultVec3f(&D_80A343B0, &thisv->actor.focus.pos);
    }
}

void EnGe2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = { gGerudoPurpleEyeOpenTex, gGerudoPurpleEyeHalfTex, gGerudoPurpleEyeClosedTex };
    s32 pad;
    EnGe2* thisv = (EnGe2*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ge2.c", 1274);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
    func_8002EBCC(&thisv->actor, globalCtx, 0);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnGe2_OverrideLimbDraw, EnGe2_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ge2.c", 1291);
}
