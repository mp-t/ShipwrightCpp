/*
 * File: z_item_inbox.c
 * Overlay: ovl_Item_Inbox
 * Description: Zelda's magic effect when opening gates in castle collapse
 */

#include "z_item_inbox.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void ItemInbox_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Draw(Actor* thisx, GlobalContext* globalCtx);

void ItemInbox_Wait(ItemInbox* thisv, GlobalContext* globalCtx);

ActorInit Item_Inbox_InitVars = {
    ACTOR_ITEM_INBOX,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ItemInbox),
    (ActorFunc)ItemInbox_Init,
    (ActorFunc)ItemInbox_Destroy,
    (ActorFunc)ItemInbox_Update,
    (ActorFunc)ItemInbox_Draw,
    NULL,
};

void ItemInbox_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* thisv = (ItemInbox*)thisx;

    thisv->actionFunc = ItemInbox_Wait;
    Actor_SetScale(&thisv->actor, 0.2);
}

void ItemInbox_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ItemInbox_Wait(ItemInbox* thisv, GlobalContext* globalCtx) {
    if (Flags_GetTreasure(globalCtx, (thisv->actor.params >> 8) & 0x1F)) {
        Actor_Kill(&thisv->actor);
    }
}

void ItemInbox_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* thisv = (ItemInbox*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void ItemInbox_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* thisv = (ItemInbox*)thisx;

    func_8002EBCC(&thisv->actor, globalCtx, 0);
    func_8002ED80(&thisv->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, thisv->actor.params & 0xFF);
}
