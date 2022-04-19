/*
 * File: z_en_ex_item.c
 * Overlay: ovl_En_Ex_Item
 * Description: Minigame prize items
 */

#include "z_en_ex_item.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnExItem_Init(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Update(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnExItem_DrawItems(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_DrawHeartPiece(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_DrawRupee(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_DrawKey(EnExItem* thisv, GlobalContext* globalCtx, s32 index);
void EnExItem_DrawMagic(EnExItem* thisv, GlobalContext* globalCtx, s16 magicIndex);

void EnExItem_WaitForObject(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_BowlPrize(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_SetupBowlCounter(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_BowlCounter(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_ExitChest(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_FairyMagic(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_TargetPrizeApproach(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_TargetPrizeGive(EnExItem* thisv, GlobalContext* globalCtx);
void EnExItem_TargetPrizeFinish(EnExItem* thisv, GlobalContext* globalCtx);

ActorInit En_Ex_Item_InitVars = {
    ACTOR_EN_EX_ITEM,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnExItem),
    (ActorFunc)EnExItem_Init,
    (ActorFunc)EnExItem_Destroy,
    (ActorFunc)EnExItem_Update,
    (ActorFunc)EnExItem_Draw,
    NULL,
};

void EnExItem_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnExItem_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* thisv = (EnExItem*)thisx;

    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->type = thisv->actor.params & 0xFF;
    thisv->unusedParam = (thisv->actor.params >> 8) & 0xFF;
    osSyncPrintf("\n\n");
    // "What will come out?"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, thisv->type);
    // "What will come out?"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, thisv->unusedParam);
    thisv->initPos = thisv->actor.world.pos;
    thisv->getItemObjId = -1;
    switch (thisv->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
            thisv->getItemObjId = OBJECT_GI_BOMBPOUCH;
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            thisv->getItemObjId = OBJECT_GI_HEARTS;
            break;
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBCHUS_COUNTER:
            thisv->getItemObjId = OBJECT_GI_BOMB_2;
            break;
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMBS_COUNTER:
            thisv->getItemObjId = OBJECT_GI_BOMB_1;
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            thisv->getItemObjId = OBJECT_GI_RUPY;
            break;
        case EXITEM_SMALL_KEY_CHEST:
            thisv->scale = 0.05f;
            thisv->actor.velocity.y = 10.0f;
            thisv->timer = 7;
            thisv->actionFunc = EnExItem_ExitChest;
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            thisv->getItemObjId = OBJECT_GI_GODDESS;
            break;
        case EXITEM_BULLET_BAG:
            thisv->getItemObjId = OBJECT_GI_DEKUPOUCH;
    }

    if (thisv->getItemObjId >= 0) {
        thisv->objectIdx = Object_GetIndex(&globalCtx->objectCtx, thisv->getItemObjId);
        thisv->actor.draw = NULL;
        if (thisv->objectIdx < 0) {
            Actor_Kill(&thisv->actor);
            // "What?"
            osSyncPrintf("なにみの？ %d\n", thisv->actor.params);
            // "bank is funny"
            osSyncPrintf(VT_FGCOL(PURPLE) " バンクおかしいしぞ！%d\n" VT_RST "\n", thisv->actor.params);
            return;
        }
        thisv->actionFunc = EnExItem_WaitForObject;
    }
}

void EnExItem_WaitForObject(EnExItem* thisv, GlobalContext* globalCtx) {
    s32 onCounter;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objectIdx)) {
        // "End of transfer"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params, thisv);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params, thisv);
        osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params, thisv);
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params, thisv);
        osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n\n" VT_RST, thisv->actor.params, thisv);
        thisv->actor.objBankIndex = thisv->objectIdx;
        thisv->actor.draw = EnExItem_Draw;
        thisv->stopRotate = false;
        onCounter = false;
        switch (thisv->type) {
            case EXITEM_BOMB_BAG_COUNTER:
                onCounter = true;
            case EXITEM_BOMB_BAG_BOWLING:
                thisv->unk_17C = func_8002EBCC;
                thisv->giDrawId = GID_BOMB_BAG_30;
                thisv->timer = 65;
                thisv->prizeRotateTimer = 35;
                thisv->scale = 0.5f;
                if (onCounter == 0) {
                    thisv->actionFunc = EnExItem_BowlPrize;
                } else {
                    thisv->actionFunc = EnExItem_SetupBowlCounter;
                    thisv->actor.shape.yOffset = -18.0f;
                }
                break;
            case EXITEM_HEART_PIECE_COUNTER:
                onCounter = true;
            case EXITEM_HEART_PIECE_BOWLING:
                thisv->unk_17C = func_8002ED80;
                thisv->timer = 65;
                thisv->prizeRotateTimer = 35;
                thisv->scale = 0.5f;
                if (!onCounter) {
                    func_80078884(NA_SE_SY_PIECE_OF_HEART);
                    thisv->actionFunc = EnExItem_BowlPrize;
                } else {
                    thisv->actionFunc = EnExItem_SetupBowlCounter;
                    thisv->actor.shape.yOffset = -10.0f;
                }
                break;
            case EXITEM_BOMBCHUS_COUNTER:
                onCounter = true;
            case EXITEM_BOMBCHUS_BOWLING:
                thisv->unk_17C = func_8002EBCC;
                thisv->giDrawId = GID_BOMBCHU;
                thisv->timer = 65;
                thisv->prizeRotateTimer = 35;
                thisv->scale = 0.5f;
                if (!onCounter) {
                    thisv->actionFunc = EnExItem_BowlPrize;
                } else {
                    thisv->actionFunc = EnExItem_SetupBowlCounter;
                }
                break;
            case EXITEM_BOMBS_BOWLING:
            case EXITEM_BOMBS_COUNTER:
                thisv->unk_17C = func_8002EBCC;
                thisv->giDrawId = GID_BOMB;
                thisv->timer = 65;
                thisv->prizeRotateTimer = 35;
                thisv->scale = 0.5f;
                thisv->unkFloat = 0.5f;
                if (thisv->type == EXITEM_BOMBS_BOWLING) {
                    thisv->actionFunc = EnExItem_BowlPrize;
                } else {
                    thisv->actionFunc = EnExItem_SetupBowlCounter;
                    thisv->actor.shape.yOffset = 10.0f;
                }
                break;
            case EXITEM_PURPLE_RUPEE_BOWLING:
            case EXITEM_PURPLE_RUPEE_COUNTER:
                thisv->unk_17C = func_8002EBCC;
                thisv->unk_180 = func_8002ED80;
                thisv->giDrawId = GID_RUPEE_PURPLE;
                thisv->timer = 65;
                thisv->prizeRotateTimer = 35;
                thisv->scale = 0.5f;
                thisv->unkFloat = 0.5f;
                if (thisv->type == EXITEM_PURPLE_RUPEE_BOWLING) {
                    thisv->actionFunc = EnExItem_BowlPrize;
                } else {
                    thisv->actionFunc = EnExItem_SetupBowlCounter;
                    thisv->actor.shape.yOffset = 40.0f;
                }
                break;
            case EXITEM_GREEN_RUPEE_CHEST:
            case EXITEM_BLUE_RUPEE_CHEST:
            case EXITEM_RED_RUPEE_CHEST:
            case EXITEM_13:
            case EXITEM_14:
                thisv->unk_17C = func_8002EBCC;
                thisv->unk_180 = func_8002ED80;
                thisv->timer = 7;
                thisv->scale = 0.5f;
                thisv->unkFloat = 0.5f;
                thisv->actor.velocity.y = 10.0f;
                switch (thisv->type) {
                    case EXITEM_GREEN_RUPEE_CHEST:
                        thisv->giDrawId = GID_RUPEE_GREEN;
                        break;
                    case EXITEM_BLUE_RUPEE_CHEST:
                        thisv->giDrawId = GID_RUPEE_BLUE;
                        break;
                    case EXITEM_RED_RUPEE_CHEST:
                        thisv->giDrawId = GID_RUPEE_RED;
                        break;
                    case EXITEM_14:
                        thisv->giDrawId = GID_RUPEE_PURPLE;
                        break;
                }
                thisv->actionFunc = EnExItem_ExitChest;
                break;
            case EXITEM_MAGIC_FIRE:
            case EXITEM_MAGIC_WIND:
            case EXITEM_MAGIC_DARK:
                thisv->scale = 0.35f;
                thisv->actionFunc = EnExItem_FairyMagic;
                break;
            case EXITEM_BULLET_BAG:
                thisv->unk_17C = func_8002EBCC;
                thisv->giDrawId = GID_BULLET_BAG;
                thisv->scale = 0.1f;
                thisv->timer = 80;
                thisv->prizeRotateTimer = 35;
                thisv->actionFunc = EnExItem_TargetPrizeApproach;
                break;
            case EXITEM_SMALL_KEY_CHEST:
                break;
        }
    }
}

void EnExItem_BowlPrize(EnExItem* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;
    f32 sp3C;

    if (!thisv->stopRotate) {
        thisv->actor.shape.rot.y += 0x1000;
        if ((thisv->prizeRotateTimer == 0) && ((thisv->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            thisv->stopRotate++;
        }
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, 0, 5, 0x1000, 0);
    }
    if (thisv->timer != 0) {
        if (thisv->prizeRotateTimer != 0) {
            sp3C = 250.0f;
            if (thisv->type == EXITEM_BOMBCHUS_BOWLING) {
                sp3C = 220.0f;
            }
            tmpf1 = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
            tmpf2 = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
            tmpf3 = globalCtx->view.lookAt.z + sp3C - globalCtx->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = globalCtx->view.eye.x + tmpf5 - thisv->actor.world.pos.x;
            tmpf2 = globalCtx->view.eye.y + tmpf6 - thisv->actor.world.pos.y;
            tmpf3 = globalCtx->view.eye.z + tmpf7 - thisv->actor.world.pos.z;

            thisv->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            thisv->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            thisv->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        // "parent"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 母親ー？     ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.parent);
        // "Can it move?"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 動いてねー？ ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.parent->update);
        if ((thisv->actor.parent != NULL) && (thisv->actor.parent->update != NULL)) {
            ((EnBomBowlPit*)thisv->actor.parent)->exItemDone = 1;
            // "It can't move!"
            osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ さぁきえるぞ！ ☆☆☆☆☆ \n" VT_RST);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnExItem_SetupBowlCounter(EnExItem* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = 0x4268;
    thisv->actionFunc = EnExItem_BowlCounter;
}

void EnExItem_BowlCounter(EnExItem* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += 0x800;
    if (thisv->killItem) {
        Actor_Kill(&thisv->actor);
    }
}

void EnExItem_ExitChest(EnExItem* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += 0x1000;
    if (thisv->timer != 0) {
        if (thisv->timer == 1) {
            thisv->chestKillTimer = 20;
        }
    } else {
        thisv->actor.velocity.y = 0.0f;
        if (thisv->chestKillTimer == 0) {
            Actor_Kill(&thisv->actor);
        }
    }
    Actor_MoveForward(&thisv->actor);
}

void EnExItem_FairyMagic(EnExItem* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += 0x800;
}

void EnExItem_TargetPrizeApproach(EnExItem* thisv, GlobalContext* globalCtx) {
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;

    Math_ApproachF(&thisv->scale, 0.8f, 0.1f, 0.02f);
    if (!thisv->stopRotate) {
        thisv->actor.shape.rot.y += 0x1000;
        if ((thisv->prizeRotateTimer == 0) && ((thisv->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            thisv->stopRotate++;
        }
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, -0x4000, 5, 0x1000, 0);
    }

    if (thisv->timer != 0) {
        if (thisv->prizeRotateTimer != 0) {
            tmpf1 = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
            tmpf2 = globalCtx->view.lookAt.y - 10.0f - globalCtx->view.eye.y;
            tmpf3 = globalCtx->view.lookAt.z + 10.0f - globalCtx->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = globalCtx->view.eye.x + tmpf5 - thisv->actor.world.pos.x;
            tmpf2 = globalCtx->view.eye.y - 10.0f + tmpf6 - thisv->actor.world.pos.y;
            tmpf3 = globalCtx->view.eye.z + 10.0f + tmpf7 - thisv->actor.world.pos.z;

            thisv->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            thisv->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            thisv->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        s32 getItemId;

        thisv->actor.draw = NULL;
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actor.parent = NULL;
        if (CUR_UPG_VALUE(UPG_BULLET_BAG) == 1) {
            getItemId = GI_BULLET_BAG_40;
        } else {
            getItemId = GI_BULLET_BAG_50;
        }
        func_8002F434(&thisv->actor, globalCtx, getItemId, 2000.0f, 1000.0f);
        thisv->actionFunc = EnExItem_TargetPrizeGive;
    }
}

void EnExItem_TargetPrizeGive(EnExItem* thisv, GlobalContext* globalCtx) {
    s32 getItemId;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnExItem_TargetPrizeFinish;
    } else {
        getItemId = (CUR_UPG_VALUE(UPG_BULLET_BAG) == 2) ? GI_BULLET_BAG_50 : GI_BULLET_BAG_40;

        func_8002F434(&thisv->actor, globalCtx, getItemId, 2000.0f, 1000.0f);
    }
}

void EnExItem_TargetPrizeFinish(EnExItem* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        // "Successful completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        gSaveContext.itemGetInf[1] |= 0x2000;
        Actor_Kill(&thisv->actor);
    }
}

void EnExItem_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* thisv = (EnExItem*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->chestKillTimer != 0) {
        thisv->chestKillTimer--;
    }
    if (thisv->prizeRotateTimer != 0) {
        thisv->prizeRotateTimer--;
    }
    thisv->actionFunc(thisv, globalCtx);
}

void EnExItem_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* thisv = (EnExItem*)thisx;
    s32 magicType;

    Actor_SetScale(&thisv->actor, thisv->scale);
    switch (thisv->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
        case EXITEM_BOMBCHUS_COUNTER:
        case EXITEM_BOMBS_COUNTER:
        case EXITEM_BULLET_BAG:
            EnExItem_DrawItems(thisv, globalCtx);
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            EnExItem_DrawHeartPiece(thisv, globalCtx);
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            EnExItem_DrawRupee(thisv, globalCtx);
            break;
        case EXITEM_SMALL_KEY_CHEST:
            EnExItem_DrawKey(thisv, globalCtx, 0);
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            magicType = thisv->type - EXITEM_MAGIC_FIRE;
            EnExItem_DrawMagic(thisv, globalCtx, magicType);
            break;
    }
}

void EnExItem_DrawItems(EnExItem* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_17C != NULL) {
        thisv->unk_17C(&thisv->actor, globalCtx, 0);
    }
    if (thisv) {}
    func_8002ED80(&thisv->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, thisv->giDrawId);
}

void EnExItem_DrawHeartPiece(EnExItem* thisv, GlobalContext* globalCtx) {
    func_8002ED80(&thisv->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, GID_HEART_PIECE);
}

void EnExItem_DrawMagic(EnExItem* thisv, GlobalContext* globalCtx, s16 magicIndex) {
    static s16 sgiDrawIds[] = { GID_DINS_FIRE, GID_FARORES_WIND, GID_NAYRUS_LOVE };

    func_8002ED80(&thisv->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, sgiDrawIds[magicIndex]);
}

void EnExItem_DrawKey(EnExItem* thisv, GlobalContext* globalCtx, s32 index) {
    static s32 keySegments[] = { 0x0403F140 };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ex_item.c", 880);

    func_8009460C(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ex_item.c", 887),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, reinterpret_cast<const void*>(SEGMENTED_TO_VIRTUAL(keySegments[index])));
    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ex_item.c", 893);
}

void EnExItem_DrawRupee(EnExItem* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_17C != NULL) {
        thisv->unk_17C(&thisv->actor, globalCtx, 0);
    }
    if (thisv->unk_180 != NULL) {
        thisv->unk_180(&thisv->actor, globalCtx, 0);
    }
    GetItem_Draw(globalCtx, thisv->giDrawId);
}
