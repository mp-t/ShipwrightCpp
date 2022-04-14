/*
 * File: z_en_ds.c
 * Overlay: ovl_En_Ds
 * Description: Potion Shop Granny
 */

#include "z_en_ds.h"
#include "objects/object_ds/object_ds.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnDs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDs_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDs_Wait(EnDs* thisv, GlobalContext* globalCtx);

const ActorInit En_Ds_InitVars = {
    ACTOR_EN_DS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DS,
    sizeof(EnDs),
    (ActorFunc)EnDs_Init,
    (ActorFunc)EnDs_Destroy,
    (ActorFunc)EnDs_Update,
    (ActorFunc)EnDs_Draw,
    NULL,
};

void EnDs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDs* thisv = (EnDs*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gPotionShopLadySkel, &gPotionShopLadyAnim, thisv->jointTable,
                       thisv->morphTable, 6);
    Animation_PlayOnce(&thisv->skelAnime, &gPotionShopLadyAnim);

    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;

    Actor_SetScale(&thisv->actor, 0.013f);

    thisv->actionFunc = EnDs_Wait;
    thisv->actor.targetMode = 1;
    thisv->unk_1E8 = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->unk_1E4 = 0.0f;
}

void EnDs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnDs_Talk(EnDs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDs_Wait;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    }
    thisv->unk_1E8 |= 1;
}

void EnDs_TalkNoEmptyBottle(EnDs* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnDs_Wait;
    }
    thisv->unk_1E8 |= 1;
}

void EnDs_TalkAfterGiveOddPotion(EnDs* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDs_Talk;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F2CC(&thisv->actor, globalCtx, 1000.0f);
    }
}

void EnDs_DisplayOddPotionText(EnDs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actor.textId = 0x504F;
        thisv->actionFunc = EnDs_TalkAfterGiveOddPotion;
        thisv->actor.flags &= ~ACTOR_FLAG_8;
        gSaveContext.itemGetInf[3] |= 1;
    }
}

void EnDs_GiveOddPotion(EnDs* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnDs_DisplayOddPotionText;
        gSaveContext.timer2State = 0;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_ODD_POTION, 10000.0f, 50.0f);
    }
}

void EnDs_TalkAfterBrewOddPotion(EnDs* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnDs_GiveOddPotion;
        func_8002F434(&thisv->actor, globalCtx, GI_ODD_POTION, 10000.0f, 50.0f);
    }
}

void EnDs_BrewOddPotion3(EnDs* thisv, GlobalContext* globalCtx) {
    if (thisv->brewTimer > 0) {
        thisv->brewTimer -= 1;
    } else {
        thisv->actionFunc = EnDs_TalkAfterBrewOddPotion;
        Message_ContinueTextbox(globalCtx, 0x504D);
    }

    Math_StepToF(&thisv->unk_1E4, 0, 0.03f);
    Environment_AdjustLights(globalCtx, thisv->unk_1E4 * (2.0f - thisv->unk_1E4), 0.0f, 0.1f, 1.0f);
}

void EnDs_BrewOddPotion2(EnDs* thisv, GlobalContext* globalCtx) {
    if (thisv->brewTimer > 0) {
        thisv->brewTimer -= 1;
    } else {
        thisv->actionFunc = EnDs_BrewOddPotion3;
        thisv->brewTimer = 60;
        Flags_UnsetSwitch(globalCtx, 0x3F);
    }
}

void EnDs_BrewOddPotion1(EnDs* thisv, GlobalContext* globalCtx) {
    if (thisv->brewTimer > 0) {
        thisv->brewTimer -= 1;
    } else {
        thisv->actionFunc = EnDs_BrewOddPotion2;
        thisv->brewTimer = 20;
    }

    Math_StepToF(&thisv->unk_1E4, 1.0f, 0.01f);
    Environment_AdjustLights(globalCtx, thisv->unk_1E4 * (2.0f - thisv->unk_1E4), 0.0f, 0.1f, 1.0f);
}

void EnDs_OfferOddPotion(EnDs* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                thisv->actionFunc = EnDs_BrewOddPotion1;
                thisv->brewTimer = 60;
                Flags_SetSwitch(globalCtx, 0x3F);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                player->exchangeItemId = EXCH_ITEM_NONE;
                break;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x504C);
                thisv->actionFunc = EnDs_Talk;
        }
    }
}

s32 EnDs_CheckRupeesAndBottle() {
    if (gSaveContext.rupees < 100) {
        return 0;
    } else if (Inventory_HasEmptyBottle() == 0) {
        return 1;
    } else {
        return 2;
    }
}

void EnDs_GiveBluePotion(EnDs* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnDs_Talk;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_POTION_BLUE, 10000.0f, 50.0f);
    }
}

void EnDs_OfferBluePotion(EnDs* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                switch (EnDs_CheckRupeesAndBottle()) {
                    case 0: // have less than 100 rupees
                        Message_ContinueTextbox(globalCtx, 0x500E);
                        break;
                    case 1: // have 100 rupees but no empty bottle
                        Message_ContinueTextbox(globalCtx, 0x96);
                        thisv->actionFunc = EnDs_TalkNoEmptyBottle;
                        return;
                    case 2: // have 100 rupees and empty bottle
                        Rupees_ChangeBy(-100);
                        thisv->actor.flags &= ~ACTOR_FLAG_16;
                        func_8002F434(&thisv->actor, globalCtx, GI_POTION_BLUE, 10000.0f, 50.0f);
                        thisv->actionFunc = EnDs_GiveBluePotion;
                        return;
                }
                break;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x500D);
        }
        thisv->actionFunc = EnDs_Talk;
    }
}

void EnDs_Wait(EnDs* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (func_8002F368(globalCtx) == EXCH_ITEM_ODD_MUSHROOM) {
            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            player->actor.textId = 0x504A;
            thisv->actionFunc = EnDs_OfferOddPotion;
        } else if (gSaveContext.itemGetInf[3] & 1) {
            player->actor.textId = 0x500C;
            thisv->actionFunc = EnDs_OfferBluePotion;
        } else {
            if (INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_ODD_MUSHROOM) {
                player->actor.textId = 0x5049;
            } else {
                player->actor.textId = 0x5048;
            }
            thisv->actionFunc = EnDs_Talk;
        }
    } else {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        thisv->actor.textId = 0x5048;

        if ((ABS(yawDiff) < 0x2151) && (thisv->actor.xzDistToPlayer < 200.0f)) {
            func_8002F298(&thisv->actor, globalCtx, 100.0f, EXCH_ITEM_ODD_MUSHROOM);
            thisv->unk_1E8 |= 1;
        }
    }
}

void EnDs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDs* thisv = (EnDs*)thisx;

    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        thisv->skelAnime.curFrame = 0.0f;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->unk_1E8 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_1D8, &thisv->unk_1DE, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->unk_1D8.x, 0, 6, 0x1838, 100);
        Math_SmoothStepToS(&thisv->unk_1D8.y, 0, 6, 0x1838, 100);
        Math_SmoothStepToS(&thisv->unk_1DE.x, 0, 6, 0x1838, 100);
        Math_SmoothStepToS(&thisv->unk_1DE.y, 0, 6, 0x1838, 100);
    }
}

s32 EnDs_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDs* thisv = (EnDs*)thisx;

    if (limbIndex == 5) {
        rot->x += thisv->unk_1D8.y;
        rot->z += thisv->unk_1D8.x;
    }
    return false;
}

void EnDs_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f sMultVec = { 1100.0f, 500.0f, 0.0f };
    EnDs* thisv = (EnDs*)thisx;

    if (limbIndex == 5) {
        Matrix_MultVec3f(&sMultVec, &thisv->actor.focus.pos);
    }
}

void EnDs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDs* thisv = (EnDs*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDs_OverrideLimbDraw, EnDs_PostLimbDraw, thisv);
}
