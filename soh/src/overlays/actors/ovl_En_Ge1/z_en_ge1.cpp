/*
 * File: z_en_ge1.c
 * Overlay: ovl_En_Ge1
 * Description: White-clothed Gerudo
 */

#include "z_en_ge1.h"
#include "vt.h"
#include "objects/object_ge1/object_ge1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

#define GE1_STATE_TALKING (1 << 0)
#define GE1_STATE_GIVE_QUIVER (1 << 1)
#define GE1_STATE_IDLE_ANIM (1 << 2)
#define GE1_STATE_STOP_FIDGET (1 << 3)

typedef enum {
    /* 00 */ GE1_HAIR_BOB,
    /* 01 */ GE1_HAIR_STRAIGHT,
    /* 02 */ GE1_HAIR_SPIKY
} EnGe1Hairstyle;

void EnGe1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGe1_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 EnGe1_CheckCarpentersFreed(void);
void EnGe1_WatchForPlayerFrontOnly(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_SetNormalText(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_WatchForAndSensePlayer(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_GetReaction_ValleyFloor(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_CheckForCard_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_CheckGate_GateOp(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_GetReaction_GateGuard(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_TalkAfterGame_Archery(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_Wait_Archery(EnGe1* thisv, GlobalContext* globalCtx);
void EnGe1_CueUpAnimation(EnGe1* thisv);
void EnGe1_StopFidget(EnGe1* thisv);

ActorInit En_Ge1_InitVars = {
    ACTOR_EN_GE1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GE1,
    sizeof(EnGe1),
    (ActorFunc)EnGe1_Init,
    (ActorFunc)EnGe1_Destroy,
    (ActorFunc)EnGe1_Update,
    (ActorFunc)EnGe1_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000702, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static const Gfx* sHairstyleDLists[] = {
    gGerudoWhiteHairstyleBobDL,
    gGerudoWhiteHairstyleStraightFringeDL,
    gGerudoWhiteHairstyleSpikyDL,
};

static Vec3f D_80A327A8 = { 600.0f, 700.0f, 0.0f };

static const void* sEyeTextures[] = {
    gGerudoWhiteEyeOpenTex,
    gGerudoWhiteEyeHalfTex,
    gGerudoWhiteEyeClosedTex,
};

void EnGe1_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* thisv = (EnGe1*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGerudoWhiteSkel, &gGerudoWhiteIdleAnim, thisv->jointTable,
                       thisv->morphTable, GE1_LIMB_MAX);
    Animation_PlayOnce(&thisv->skelAnime, &gGerudoWhiteIdleAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->animation = &gGerudoWhiteIdleAnim;
    thisv->animFunc = EnGe1_CueUpAnimation;
    thisv->actor.targetMode = 6;
    Actor_SetScale(&thisv->actor, 0.01f);

    // In Gerudo Valley
    thisv->actor.uncullZoneForward = ((globalCtx->sceneNum == SCENE_SPOT09) ? 1000.0f : 1200.0f);

    switch (thisv->actor.params & 0xFF) {

        case GE1_TYPE_GATE_GUARD:
            thisv->hairstyle = GE1_HAIR_SPIKY;
            thisv->actionFunc = EnGe1_GetReaction_GateGuard;
            break;

        case GE1_TYPE_GATE_OPERATOR:
            thisv->hairstyle = GE1_HAIR_STRAIGHT;

            if (EnGe1_CheckCarpentersFreed()) {
                thisv->actionFunc = EnGe1_CheckGate_GateOp;
            } else {
                thisv->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;

        case GE1_TYPE_NORMAL:
            thisv->hairstyle = GE1_HAIR_STRAIGHT;

            if (EnGe1_CheckCarpentersFreed()) {
                thisv->actionFunc = EnGe1_SetNormalText;
            } else {
                thisv->actionFunc = EnGe1_WatchForAndSensePlayer;
            }
            break;

        case GE1_TYPE_VALLEY_FLOOR:
            if (LINK_IS_ADULT) {
                // "Valley floor Gerudo withdrawal"
                osSyncPrintf(VT_FGCOL(CYAN) "谷底 ゲルド 撤退 \n" VT_RST);
                Actor_Kill(&thisv->actor);
                return;
            }
            thisv->hairstyle = GE1_HAIR_BOB;
            thisv->actionFunc = EnGe1_GetReaction_ValleyFloor;
            break;

        case GE1_TYPE_HORSEBACK_ARCHERY:
            if (INV_CONTENT(SLOT_BOW) == ITEM_NONE) {
                Actor_Kill(&thisv->actor);
                return;
            }
            thisv->actor.targetMode = 3;
            thisv->hairstyle = GE1_HAIR_BOB;
            // "Horseback archery Gerudo EVENT_INF(0) ="
            osSyncPrintf(VT_FGCOL(CYAN) "やぶさめ ゲルド EVENT_INF(0) = %x\n" VT_RST, gSaveContext.eventInf[0]);

            if (gSaveContext.eventInf[0] & 0x100) {
                thisv->actionFunc = EnGe1_TalkAfterGame_Archery;
            } else if (EnGe1_CheckCarpentersFreed()) {
                thisv->actionFunc = EnGe1_Wait_Archery;
            } else {
                thisv->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;

        case GE1_TYPE_TRAINING_GROUNDS_GUARD:
            thisv->hairstyle = GE1_HAIR_STRAIGHT;

            if (EnGe1_CheckCarpentersFreed()) {
                thisv->actionFunc = EnGe1_CheckForCard_GTGGuard;
            } else {
                thisv->actionFunc = EnGe1_WatchForPlayerFrontOnly;
            }
            break;
    }

    thisv->stateFlags = 0;
}

void EnGe1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGe1* thisv = (EnGe1*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnGe1_SetTalkAction(EnGe1* thisv, GlobalContext* globalCtx, u16 textId, f32 arg3, EnGe1ActionFunc actionFunc) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = actionFunc;
        thisv->animFunc = EnGe1_StopFidget;
        thisv->stateFlags &= ~GE1_STATE_IDLE_ANIM;
        thisv->animation = &gGerudoWhiteIdleAnim;
        Animation_Change(&thisv->skelAnime, &gGerudoWhiteIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteIdleAnim), ANIMMODE_ONCE, -8.0f);
        return true;
    }

    thisv->actor.textId = textId;

    if (thisv->actor.xzDistToPlayer < arg3) {
        func_8002F2CC(&thisv->actor, globalCtx, arg3);
    }

    return false;
}

void EnGe1_SetAnimationIdle(EnGe1* thisv) {
    Animation_Change(&thisv->skelAnime, &gGerudoWhiteIdleAnim, -1.0f, Animation_GetLastFrame(&gGerudoWhiteIdleAnim),
                     0.0f, ANIMMODE_ONCE, 8.0f);
    thisv->animation = &gGerudoWhiteIdleAnim;
    thisv->animFunc = EnGe1_CueUpAnimation;
}

s32 EnGe1_CheckCarpentersFreed(void) {
    u16 carpenterFlags = gSaveContext.eventChkInf[9];

    if (!((carpenterFlags & 1) && (carpenterFlags & 2) && (carpenterFlags & 4) && (carpenterFlags & 8))) {
        return 0;
    }
    return 1;
}

/**
 * Sends player to different places depending on if has hookshot, and if thisv is the first time captured
 */
void EnGe1_KickPlayer(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;

    if (thisv->cutsceneTimer > 0) {
        thisv->cutsceneTimer--;
    } else {
        func_8006D074(globalCtx);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            globalCtx->nextEntranceIndex = 0x1A5;
        } else if (gSaveContext.eventChkInf[12] & 0x80) { // Caught previously
            globalCtx->nextEntranceIndex = 0x5F8;
        } else {
            globalCtx->nextEntranceIndex = 0x3B4;
        }

        globalCtx->fadeTransition = 0x26;
        globalCtx->sceneLoadFlag = 0x14;
    }
}

void EnGe1_SpotPlayer(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->cutsceneTimer = 30;
    thisv->actionFunc = EnGe1_KickPlayer;
    func_8002DF54(globalCtx, &thisv->actor, 0x5F);
    func_80078884(NA_SE_SY_FOUND);
    Message_StartTextbox(globalCtx, 0x6000, &thisv->actor);
}

void EnGe1_WatchForPlayerFrontOnly(EnGe1* thisv, GlobalContext* globalCtx) {
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        EnGe1_SpotPlayer(thisv, globalCtx);
    }

    if (thisv->collider.base.acFlags & AC_HIT) {
        EnGe1_SpotPlayer(thisv, globalCtx);
    }

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnGe1_ChooseActionFromTextId(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;

    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        switch (thisv->actor.textId) {
            case 0x6001:
                thisv->actionFunc = EnGe1_SetNormalText;
                break;

            case 0x601A:
            case 0x6019:
                thisv->actionFunc = EnGe1_GetReaction_ValleyFloor;
                break;

            case 0x6018:
                thisv->actionFunc = EnGe1_CheckGate_GateOp;
                break;

            default:
                thisv->actionFunc = EnGe1_GetReaction_ValleyFloor;
                break;
        }
    }
}

void EnGe1_SetNormalText(EnGe1* thisv, GlobalContext* globalCtx) {
    EnGe1_SetTalkAction(thisv, globalCtx, 0x6001, 100.0f, EnGe1_ChooseActionFromTextId);
}

void EnGe1_WatchForAndSensePlayer(EnGe1* thisv, GlobalContext* globalCtx) {
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((thisv->actor.xzDistToPlayer < 50.0f) || ((ABS(angleDiff) <= 0x4300) && (thisv->actor.xzDistToPlayer < 400.0f))) {
        EnGe1_SpotPlayer(thisv, globalCtx);
    }

    if (thisv->collider.base.acFlags & AC_HIT) {
        EnGe1_SpotPlayer(thisv, globalCtx);
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnGe1_GetReaction_ValleyFloor(EnGe1* thisv, GlobalContext* globalCtx) {
    u16 reactionText = Text_GetFaceReaction(globalCtx, 0x22);

    if (reactionText == 0) {
        reactionText = 0x6019;
    }

    EnGe1_SetTalkAction(thisv, globalCtx, reactionText, 100.0f, EnGe1_ChooseActionFromTextId);
}

// Gerudo Training Ground Guard functions

void EnGe1_WaitTillOpened_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    if (thisv->cutsceneTimer > 0) {
        thisv->cutsceneTimer--;
    } else {
        EnGe1_SetAnimationIdle(thisv);
        thisv->actionFunc = EnGe1_SetNormalText;
    }

    thisv->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void EnGe1_Open_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    if (thisv->stateFlags & GE1_STATE_IDLE_ANIM) {
        thisv->actionFunc = EnGe1_WaitTillOpened_GTGGuard;
        Flags_SetSwitch(globalCtx, (thisv->actor.params >> 8) & 0x3F);
        thisv->cutsceneTimer = 50;
        Message_CloseTextbox(globalCtx);
    } else if ((thisv->skelAnime.curFrame == 15.0f) || (thisv->skelAnime.curFrame == 19.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_HAND_CLAP);
    }
}

void EnGe1_SetupOpen_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = EnGe1_Open_GTGGuard;
        Animation_Change(&thisv->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        thisv->animation = &gGerudoWhiteClapAnim;
        thisv->animFunc = EnGe1_StopFidget;
        thisv->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void EnGe1_RefuseEntryTooPoor_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->actionFunc = EnGe1_CheckForCard_GTGGuard;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_OfferOpen_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);

        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < 10) {
                    Message_ContinueTextbox(globalCtx, 0x6016);
                    thisv->actionFunc = EnGe1_RefuseEntryTooPoor_GTGGuard;
                } else {
                    Rupees_ChangeBy(-10);
                    Message_ContinueTextbox(globalCtx, 0x6015);
                    thisv->actionFunc = EnGe1_SetupOpen_GTGGuard;
                }
                break;
            case 1:
                thisv->actionFunc = EnGe1_CheckForCard_GTGGuard;
                EnGe1_SetAnimationIdle(thisv);
                break;
        }
    }
}

void EnGe1_RefuseOpenNoCard_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_CheckForCard_GTGGuard;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_CheckForCard_GTGGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)) {
        EnGe1_SetTalkAction(thisv, globalCtx, 0x6014, 100.0f, EnGe1_OfferOpen_GTGGuard);
    } else {
        //! @bug This outcome is inaccessible in normal gameplay since thisv function it is unreachable without
        //! obtaining the card in the first place.
        EnGe1_SetTalkAction(thisv, globalCtx, 0x6013, 100.0f, EnGe1_RefuseOpenNoCard_GTGGuard);
    }
}

// Gate Operator functions

void EnGe1_WaitGateOpen_GateOp(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnGe1_CheckGate_GateOp;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_WaitUntilGateOpened_GateOp(EnGe1* thisv, GlobalContext* globalCtx) {
    if (thisv->cutsceneTimer > 0) {
        thisv->cutsceneTimer--;
    } else {
        EnGe1_SetAnimationIdle(thisv);
        thisv->actionFunc = EnGe1_CheckGate_GateOp;
    }
    thisv->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void EnGe1_OpenGate_GateOp(EnGe1* thisv, GlobalContext* globalCtx) {
    if (thisv->stateFlags & GE1_STATE_IDLE_ANIM) {
        thisv->actionFunc = EnGe1_WaitUntilGateOpened_GateOp;
        Flags_SetSwitch(globalCtx, (thisv->actor.params >> 8) & 0x3F);
        thisv->cutsceneTimer = 50;
        Message_CloseTextbox(globalCtx);
    } else if ((thisv->skelAnime.curFrame == 15.0f) || (thisv->skelAnime.curFrame == 19.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_HAND_CLAP);
    }
}

void EnGe1_SetupOpenGate_GateOp(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = EnGe1_OpenGate_GateOp;
        Animation_Change(&thisv->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        thisv->animation = &gGerudoWhiteClapAnim;
        thisv->animFunc = EnGe1_StopFidget;
        thisv->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void EnGe1_CheckGate_GateOp(EnGe1* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (thisv->actor.params >> 8) & 0x3F)) {
        EnGe1_SetTalkAction(thisv, globalCtx, 0x6018, 100.0f, EnGe1_WaitGateOpen_GateOp);
    } else {
        EnGe1_SetTalkAction(thisv, globalCtx, 0x6017, 100.0f, EnGe1_SetupOpenGate_GateOp);
    }
}

// Gate guard functions

void EnGe1_Talk_GateGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;

    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_GetReaction_GateGuard;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_GetReaction_GateGuard(EnGe1* thisv, GlobalContext* globalCtx) {
    u16 reactionText;

    reactionText = Text_GetFaceReaction(globalCtx, 0x22);

    if (reactionText == 0) {
        reactionText = 0x6069;
    }

    if (EnGe1_SetTalkAction(thisv, globalCtx, reactionText, 100.0f, EnGe1_Talk_GateGuard)) {
        thisv->animFunc = EnGe1_CueUpAnimation;
        thisv->animation = &gGerudoWhiteDismissiveAnim;
        Animation_Change(&thisv->skelAnime, &gGerudoWhiteDismissiveAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoWhiteDismissiveAnim), ANIMMODE_ONCE, -8.0f);
    }
}

// Archery functions

void EnGe1_SetupWait_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_WaitTillItemGiven_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    s32 getItemId;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_SetupWait_Archery;
        if (thisv->stateFlags & GE1_STATE_GIVE_QUIVER) {
            gSaveContext.itemGetInf[0] |= 0x8000;
        } else {
            gSaveContext.infTable[25] |= 1;
        }
    } else {
        if (thisv->stateFlags & GE1_STATE_GIVE_QUIVER) {
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                //! @bug Asschest. See next function for details
                case 1:
                    getItemId = GI_QUIVER_40;
                    break;
                case 2:
                    getItemId = GI_QUIVER_50;
                    break;
            }
        } else {
            getItemId = GI_HEART_PIECE;
        }
        func_8002F434(&thisv->actor, globalCtx, getItemId, 10000.0f, 50.0f);
    }
}

void EnGe1_BeginGiveItem_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    s32 getItemId;

    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = EnGe1_WaitTillItemGiven_Archery;
    }

    if (thisv->stateFlags & GE1_STATE_GIVE_QUIVER) {
        switch (CUR_UPG_VALUE(UPG_QUIVER)) {
            //! @bug Asschest: the compiler inserts a default assigning *(sp+0x24) to getItemId, which is junk data left
            //! over from the previous function run in EnGe1_Update, namely EnGe1_CueUpAnimation. The top stack variable
            //! in that function is &thisv->skelAnime = thisx + 198, and depending on where thisv loads in memory, the
            //! getItemId changes.
            case 1:
                getItemId = GI_QUIVER_40;
                break;
            case 2:
                getItemId = GI_QUIVER_50;
                break;
        }
    } else {
        getItemId = GI_HEART_PIECE;
    }

    func_8002F434(&thisv->actor, globalCtx, getItemId, 10000.0f, 50.0f);
}

void EnGe1_TalkWinPrize_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_BeginGiveItem_Archery;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 200.0f);
    }
}

void EnGe1_TalkTooPoor_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_WaitDoNothing(EnGe1* thisv, GlobalContext* globalCtx) {
}

void EnGe1_BeginGame_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* horse;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;

        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < 20) {
                    Message_ContinueTextbox(globalCtx, 0x85);
                    thisv->actionFunc = EnGe1_TalkTooPoor_Archery;
                } else {
                    Rupees_ChangeBy(-20);
                    globalCtx->nextEntranceIndex = 0x129;
                    gSaveContext.nextCutsceneIndex = 0xFFF0;
                    globalCtx->fadeTransition = 0x26;
                    globalCtx->sceneLoadFlag = 0x14;
                    gSaveContext.eventInf[0] |= 0x100;
                    gSaveContext.eventChkInf[6] |= 0x100;

                    if (!(player->stateFlags1 & 0x800000)) {
                        func_8002DF54(globalCtx, &thisv->actor, 1);
                    } else {
                        horse = Actor_FindNearby(globalCtx, &player->actor, ACTOR_EN_HORSE, ACTORCAT_BG, 1200.0f);
                        player->actor.freezeTimer = 1200;

                        if (horse != NULL) {
                            horse->freezeTimer = 1200;
                        }
                    }

                    thisv->actionFunc = EnGe1_WaitDoNothing;
                }
                break;

            case 1:
                thisv->actionFunc = EnGe1_Wait_Archery;
                Message_CloseTextbox(globalCtx);
                break;
        }
    }
}

void EnGe1_TalkOfferPlay_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x6041);
        thisv->actionFunc = EnGe1_BeginGame_Archery;
    }
}

void EnGe1_TalkNoPrize_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_TalkOfferPlay_Archery;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 300.0f);
    }
}

void EnGe1_TalkAfterGame_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    gSaveContext.eventInf[0] &= ~0x100;
    LOG_NUM("z_common_data.yabusame_total", gSaveContext.minigameScore, "../z_en_ge1.c", 1110);
    LOG_NUM("z_common_data.memory.information.room_inf[127][ 0 ]", HIGH_SCORE(HS_HBA), "../z_en_ge1.c", 1111);
    thisv->actor.flags |= ACTOR_FLAG_16;

    if (HIGH_SCORE(HS_HBA) < gSaveContext.minigameScore) {
        HIGH_SCORE(HS_HBA) = gSaveContext.minigameScore;
    }

    if (gSaveContext.minigameScore < 1000) {
        thisv->actor.textId = 0x6045;
        thisv->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else if (!(gSaveContext.infTable[25] & 1)) {
        thisv->actor.textId = 0x6046;
        thisv->actionFunc = EnGe1_TalkWinPrize_Archery;
        thisv->stateFlags &= ~GE1_STATE_GIVE_QUIVER;
    } else if (gSaveContext.minigameScore < 1500) {
        thisv->actor.textId = 0x6047;
        thisv->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else if (gSaveContext.itemGetInf[0] & 0x8000) {
        thisv->actor.textId = 0x6047;
        thisv->actionFunc = EnGe1_TalkNoPrize_Archery;
    } else {
        thisv->actor.textId = 0x6044;
        thisv->actionFunc = EnGe1_TalkWinPrize_Archery;
        thisv->stateFlags |= GE1_STATE_GIVE_QUIVER;
    }
}

void EnGe1_TalkNoHorse_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= GE1_STATE_TALKING;
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnGe1_Wait_Archery;
        EnGe1_SetAnimationIdle(thisv);
    }
}

void EnGe1_Wait_Archery(EnGe1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u16 textId;

    if (!(player->stateFlags1 & 0x800000)) {
        EnGe1_SetTalkAction(thisv, globalCtx, 0x603F, 100.0f, EnGe1_TalkNoHorse_Archery);
    } else {
        if (gSaveContext.eventChkInf[6] & 0x100) {
            if (gSaveContext.infTable[25] & 1) {
                textId = 0x6042;
            } else {
                textId = 0x6043;
            }
        } else {
            textId = 0x6040;
        }
        EnGe1_SetTalkAction(thisv, globalCtx, textId, 200.0f, EnGe1_TalkOfferPlay_Archery);
    }
}

// General functions

void EnGe1_TurnToFacePlayer(EnGe1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 6, 4000, 100);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2A2, thisv->actor.focus.pos);
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

void EnGe1_LookAtPlayer(EnGe1* thisv, GlobalContext* globalCtx) {
    s16 angleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2A2, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->headRot.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->headRot.y, 0, 6, 6200, 100);
    }
}

void EnGe1_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* thisv = (EnGe1*)thisx;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 40.0f, 25.0f, 40.0f, 5);
    thisv->animFunc(thisv);
    thisv->actionFunc(thisv, globalCtx);

    if (thisv->stateFlags & GE1_STATE_TALKING) {
        EnGe1_TurnToFacePlayer(thisv, globalCtx);
        thisv->stateFlags &= ~GE1_STATE_TALKING;
    } else {
        EnGe1_LookAtPlayer(thisv, globalCtx);
    }
    thisv->unk_2A2.x = thisv->unk_2A2.y = thisv->unk_2A2.z = 0;

    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }
    thisv->eyeIndex = thisv->blinkTimer;

    if (thisv->eyeIndex >= 3) {
        thisv->eyeIndex = 0;
    }
}

// Animation functions

void EnGe1_CueUpAnimation(EnGe1* thisv) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_PlayOnce(&thisv->skelAnime, thisv->animation);
    }
}

void EnGe1_StopFidget(EnGe1* thisv) {
    if (!(thisv->stateFlags & GE1_STATE_IDLE_ANIM)) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->stateFlags |= GE1_STATE_IDLE_ANIM;
        }
        thisv->stateFlags |= GE1_STATE_STOP_FIDGET;
    }
}

s32 EnGe1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    s32 pad;
    EnGe1* thisv = (EnGe1*)thisx;

    if (limbIndex == GE1_LIMB_HEAD) {
        rot->x += thisv->headRot.y;
        rot->z += thisv->headRot.x;
    }

    if (thisv->stateFlags & GE1_STATE_STOP_FIDGET) {
        thisv->stateFlags &= ~GE1_STATE_STOP_FIDGET;
        return 0;
    }

    // The purpose of the state flag GE1_STATE_STOP_FIDGET is to skip thisv code, which thisv actor has in lieu of an idle
    // animation.
    if ((limbIndex == GE1_LIMB_TORSO) || (limbIndex == GE1_LIMB_L_FOREARM) || (limbIndex == GE1_LIMB_R_FOREARM)) {
        rot->y += Math_SinS(globalCtx->state.frames * (limbIndex * 50 + 0x814)) * 200.0f;
        rot->z += Math_CosS(globalCtx->state.frames * (limbIndex * 50 + 0x940)) * 200.0f;
    }
    return 0;
}

void EnGe1_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe1* thisv = (EnGe1*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ge1.c", 1419);

    if (limbIndex == GE1_LIMB_HEAD) {
        gSPDisplayList(POLY_OPA_DISP++, sHairstyleDLists[thisv->hairstyle]);
        Matrix_MultVec3f(&D_80A327A8, &thisv->actor.focus.pos);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ge1.c", 1427);
}

void EnGe1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGe1* thisv = (EnGe1*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ge1.c", 1442);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnGe1_OverrideLimbDraw, EnGe1_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ge1.c", 1459);
}
