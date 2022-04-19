/*
 * File: z_en_ms.c
 * Overlay: ovl_En_Ms
 * Description: Magic Bean Salesman
 */

#include "z_en_ms.h"
#include "objects/object_ms/object_ms.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnMs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMs_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnMs_SetOfferText(EnMs* thisv, GlobalContext* globalCtx);
void EnMs_Wait(EnMs* thisv, GlobalContext* globalCtx);
void EnMs_Talk(EnMs* thisv, GlobalContext* globalCtx);
void EnMs_Sell(EnMs* thisv, GlobalContext* globalCtx);
void EnMs_TalkAfterPurchase(EnMs* thisv, GlobalContext* globalCtx);

ActorInit En_Ms_InitVars = {
    ACTOR_EN_MS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MS,
    sizeof(EnMs),
    (ActorFunc)EnMs_Init,
    (ActorFunc)EnMs_Destroy,
    (ActorFunc)EnMs_Update,
    (ActorFunc)EnMs_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x00, 0x01, 0x01 },
    { 22, 37, 0, { 0 } },
};

static s16 sPrices[] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
};

static u16 sOfferTextIDs[] = {
    0x405E, 0x405F, 0x4060, 0x4061, 0x4062, 0x4063, 0x4064, 0x4065, 0x4066, 0x4067,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

void EnMs_SetOfferText(EnMs* thisv, GlobalContext* globalCtx) {
    thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0x1B);
    if (thisv->actor.textId == 0) {
        if (BEANS_BOUGHT >= 10) {
            thisv->actor.textId = 0x406B;
        } else {
            thisv->actor.textId = sOfferTextIDs[BEANS_BOUGHT];
        }
    }
}

void EnMs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* thisv = (EnMs*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBeanSalesmanSkel, &gBeanSalesmanEatingAnim, thisv->jointTable,
                       thisv->morphTable, 9);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&thisv->actor, 0.015f);

    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -1.0f;

    EnMs_SetOfferText(thisv, globalCtx);

    thisv->actionFunc = EnMs_Wait;
}

void EnMs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* thisv = (EnMs*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnMs_Wait(EnMs* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    EnMs_SetOfferText(thisv, globalCtx);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) { // if talk is initiated
        thisv->actionFunc = EnMs_Talk;
    } else if ((thisv->actor.xzDistToPlayer < 90.0f) && (ABS(yawDiff) < 0x2000)) { // talk range
        func_8002F2CC(&thisv->actor, globalCtx, 90.0f);
    }
}

void EnMs_Talk(EnMs* thisv, GlobalContext* globalCtx) {
    u8 dialogState;

    dialogState = Message_GetState(&globalCtx->msgCtx);
    if (dialogState != TEXT_STATE_CHOICE) {
        if ((dialogState == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) { // advanced final textbox
            thisv->actionFunc = EnMs_Wait;
        }
    } else if (Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < sPrices[BEANS_BOUGHT]) {
                    Message_ContinueTextbox(globalCtx, 0x4069); // not enough rupees text
                    return;
                }
                func_8002F434(&thisv->actor, globalCtx, GI_BEAN, 90.0f, 10.0f);
                thisv->actionFunc = EnMs_Sell;
                return;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x4068);
            default:
                return;
        }
    }
}

void EnMs_Sell(EnMs* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        Rupees_ChangeBy(-sPrices[BEANS_BOUGHT]);
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnMs_TalkAfterPurchase;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_BEAN, 90.0f, 10.0f);
    }
}

void EnMs_TalkAfterPurchase(EnMs* thisv, GlobalContext* globalCtx) {
    // if dialog state is 6 and player responded to textbox
    if ((Message_GetState(&globalCtx->msgCtx)) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x406C);
        thisv->actionFunc = EnMs_Talk;
    }
}

void EnMs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* thisv = (EnMs*)thisx;
    s32 pad;

    thisv->activeTimer += 1;
    Actor_SetFocus(&thisv->actor, 20.0f);
    thisv->actor.targetArrowOffset = 500.0f;
    Actor_SetScale(&thisv->actor, 0.015f);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actionFunc(thisv, globalCtx);

    if (gSaveContext.entranceIndex == 0x157 && gSaveContext.sceneSetupIndex == 8) { // ride carpet if in credits
        Actor_MoveForward(&thisv->actor);
        osSyncPrintf("OOOHHHHHH %f\n", thisv->actor.velocity.y);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    }
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnMs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnMs* thisv = (EnMs*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
}
