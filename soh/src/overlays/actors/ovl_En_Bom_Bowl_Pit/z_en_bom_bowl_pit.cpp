#include "z_en_bom_bowl_pit.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"

#define FLAGS ACTOR_FLAG_4

void EnBomBowlPit_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBomBowlPit_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBomBowlPit_Update(Actor* thisx, GlobalContext* globalCtx);

void EnBomBowlPit_SetupDetectHit(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_DetectHit(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_CameraDollyIn(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_SpawnPrize(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_SetupGivePrize(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_GivePrize(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_WaitTillPrizeGiven(EnBomBowlPit* thisv, GlobalContext* globalCtx);
void EnBomBowlPit_Reset(EnBomBowlPit* thisv, GlobalContext* globalCtx);

static s32 sGetItemIds[] = { GI_BOMB_BAG_30, GI_HEART_PIECE, GI_BOMBCHUS_10, GI_BOMBS_1, GI_RUPEE_PURPLE };

const ActorInit En_Bom_Bowl_Pit_InitVars = {
    ACTOR_EN_BOM_BOWL_PIT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBomBowlPit),
    (ActorFunc)EnBomBowlPit_Init,
    (ActorFunc)EnBomBowlPit_Destroy,
    (ActorFunc)EnBomBowlPit_Update,
    NULL,
    NULL,
};

void EnBomBowlPit_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBomBowlPit* thisv = (EnBomBowlPit*)thisx;

    thisv->actionFunc = EnBomBowlPit_SetupDetectHit;
}

void EnBomBowlPit_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnBomBowlPit_SetupDetectHit(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if (thisv->start != 0) {
        thisv->start = thisv->status = 0;
        thisv->actionFunc = EnBomBowlPit_DetectHit;
    }
}

void EnBomBowlPit_DetectHit(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    EnBomChu* chu;
    Vec3f chuPosDiff;

    if (globalCtx->cameraPtrs[MAIN_CAM]->setting == CAM_SET_CHU_BOWLING) {
        chu = (EnBomChu*)globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

        while (chu != NULL) {
            if ((&chu->actor == &thisv->actor) || (chu->actor.id != ACTOR_EN_BOM_CHU)) {
                chu = (EnBomChu*)chu->actor.next;
                continue;
            }

            chuPosDiff.x = chu->actor.world.pos.x - thisv->actor.world.pos.x;
            chuPosDiff.y = chu->actor.world.pos.y - thisv->actor.world.pos.y;
            chuPosDiff.z = chu->actor.world.pos.z - thisv->actor.world.pos.z;

            if (((fabsf(chuPosDiff.x) < 40.0f) || (BREG(2))) && ((fabsf(chuPosDiff.y) < 40.0f) || (BREG(2))) &&
                ((fabsf(chuPosDiff.z) < 40.0f) || (BREG(2)))) {
                func_8002DF54(globalCtx, NULL, 8);
                chu->timer = 1;

                thisv->camId = Gameplay_CreateSubCamera(globalCtx);
                Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
                Gameplay_ChangeCameraStatus(globalCtx, thisv->camId, CAM_STAT_ACTIVE);

                thisv->unk_1C8.x = thisv->unk_1C8.y = thisv->unk_1C8.z = 0.1f;
                thisv->unk_1A4.x = thisv->unk_1A4.y = thisv->unk_1A4.z = 0.1f;

                thisv->unk_180.x = thisv->unk_168.x = globalCtx->view.lookAt.x;
                thisv->unk_180.y = thisv->unk_168.y = globalCtx->view.lookAt.y;
                thisv->unk_180.z = thisv->unk_168.z = globalCtx->view.lookAt.z;

                thisv->unk_18C.x = thisv->unk_174.x = globalCtx->view.eye.x;
                thisv->unk_18C.y = thisv->unk_174.y = globalCtx->view.eye.y;
                thisv->unk_18C.z = thisv->unk_174.z = globalCtx->view.eye.z;

                thisv->unk_1BC.x = 20.0f;
                thisv->unk_1BC.y = 100.0f;
                thisv->unk_1BC.z = -800.0f;

                thisv->unk_198.x = 20.0f;
                thisv->unk_198.y = 50.0f;
                thisv->unk_198.z = -485.0f;

                thisv->unk_1B0.x = fabsf(thisv->unk_18C.x - thisv->unk_198.x) * 0.02f;
                thisv->unk_1B0.y = fabsf(thisv->unk_18C.y - thisv->unk_198.y) * 0.02f;
                thisv->unk_1B0.z = fabsf(thisv->unk_18C.z - thisv->unk_198.z) * 0.02f;

                thisv->unk_1D4.x = fabsf(thisv->unk_180.x - thisv->unk_1BC.x) * 0.02f;
                thisv->unk_1D4.y = fabsf(thisv->unk_180.y - thisv->unk_1BC.y) * 0.02f;
                thisv->unk_1D4.z = fabsf(thisv->unk_180.z - thisv->unk_1BC.z) * 0.02f;

                Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &thisv->unk_180, &thisv->unk_18C);
                thisv->actor.textId = 0xF;
                Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
                thisv->unk_154 = TEXT_STATE_EVENT;
                func_80078884(NA_SE_EV_HIT_SOUND);
                func_8002DF54(globalCtx, NULL, 8);
                thisv->status = 1;
                thisv->actionFunc = EnBomBowlPit_CameraDollyIn;
                break;
            } else {
                chu = (EnBomChu*)chu->actor.next;
            }
        }
    }
}

void EnBomBowlPit_CameraDollyIn(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if (thisv->camId != SUBCAM_FREE) {
        Math_ApproachF(&thisv->unk_180.x, thisv->unk_1BC.x, thisv->unk_1C8.x, thisv->unk_1D4.x);
        Math_ApproachF(&thisv->unk_180.y, thisv->unk_1BC.y, thisv->unk_1C8.y, thisv->unk_1D4.y);
        Math_ApproachF(&thisv->unk_180.z, thisv->unk_1BC.z, thisv->unk_1C8.z, thisv->unk_1D4.z);
        Math_ApproachF(&thisv->unk_18C.x, thisv->unk_198.x, thisv->unk_1A4.x, thisv->unk_1B0.x);
        Math_ApproachF(&thisv->unk_18C.y, thisv->unk_198.y, thisv->unk_1A4.y, thisv->unk_1B0.y);
        Math_ApproachF(&thisv->unk_18C.z, thisv->unk_198.z, thisv->unk_1A4.z, thisv->unk_1B0.z);
    }

    Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &thisv->unk_180, &thisv->unk_18C);

    if ((thisv->unk_154 == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
    }

    if ((fabsf(thisv->unk_18C.x - thisv->unk_198.x) < 5.0f) && (fabsf(thisv->unk_18C.y - thisv->unk_198.y) < 5.0f) &&
        (fabsf(thisv->unk_18C.z - thisv->unk_198.z) < 5.0f) && (fabsf(thisv->unk_180.x - thisv->unk_1BC.x) < 5.0f) &&
        (fabsf(thisv->unk_180.y - thisv->unk_1BC.y) < 5.0f) && (fabsf(thisv->unk_180.z - thisv->unk_1BC.z) < 5.0f)) {
        Message_CloseTextbox(globalCtx);
        thisv->timer = 30;
        thisv->actionFunc = EnBomBowlPit_SpawnPrize;
    }
}

void EnBomBowlPit_SpawnPrize(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        thisv->exItem = (EnExItem*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_ITEM,
                                                     thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                                     thisv->actor.world.pos.z - 70.0f, 0, 0, 0, thisv->prizeIndex);
        if (thisv->exItem != NULL) {
            thisv->actionFunc = EnBomBowlPit_SetupGivePrize;
        }
    }
}

void EnBomBowlPit_SetupGivePrize(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if (thisv->exItemDone != 0) {
        switch (thisv->prizeIndex) {
            case EXITEM_BOMB_BAG_BOWLING:
                gSaveContext.itemGetInf[1] |= 2;
                break;
            case EXITEM_HEART_PIECE_BOWLING:
                gSaveContext.itemGetInf[1] |= 4;
                break;
        }

        Gameplay_ClearCamera(globalCtx, thisv->camId);
        Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = EnBomBowlPit_GivePrize;
    }
}

void EnBomBowlPit_GivePrize(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    func_8002DF54(globalCtx, NULL, 7);
    thisv->getItemId = sGetItemIds[thisv->prizeIndex];

    if ((thisv->getItemId == GI_BOMB_BAG_30) && (CUR_CAPACITY(UPG_BOMB_BAG) == 30)) {
        thisv->getItemId = GI_BOMB_BAG_40;
    }

    player->stateFlags1 &= ~0x20000000;
    thisv->actor.parent = NULL;
    func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 2000.0f, 1000.0f);
    player->stateFlags1 |= 0x20000000;
    thisv->actionFunc = EnBomBowlPit_WaitTillPrizeGiven;
}

void EnBomBowlPit_WaitTillPrizeGiven(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnBomBowlPit_Reset;
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 2000.0f, 1000.0f);
    }
}

void EnBomBowlPit_Reset(EnBomBowlPit* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        // "Normal termination"/"completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        if (thisv->getItemId == GI_HEART_PIECE) {
            gSaveContext.healthAccumulator = 0x140;
            // "Ah recovery!" (?)
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ あぁ回復！ ☆☆☆☆☆ \n" VT_RST);
        }
        thisv->exItemDone = 0;
        thisv->status = 2;
        thisv->actionFunc = EnBomBowlPit_SetupDetectHit;
    }
}

void EnBomBowlPit_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBomBowlPit* thisv = (EnBomBowlPit*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->timer != 0) {
        thisv->timer--;
    }
}
