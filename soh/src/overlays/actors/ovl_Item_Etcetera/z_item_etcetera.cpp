/*
 * File: z_item_etcetera.c
 * Overlay: ovl_Item_Etcetera
 * Description: Collectible Items
 */

#include "z_item_etcetera.h"

#define FLAGS ACTOR_FLAG_4

void ItemEtcetera_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemEtcetera_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemEtcetera_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemEtcetera_DrawThroughLens(Actor* thisx, GlobalContext* globalCtx);
void ItemEtcetera_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B857D0(ItemEtcetera* thisv, GlobalContext* globalCtx);
void func_80B85824(ItemEtcetera* thisv, GlobalContext* globalCtx);
void func_80B858B4(ItemEtcetera* thisv, GlobalContext* globalCtx);
void ItemEtcetera_SpawnSparkles(ItemEtcetera* thisv, GlobalContext* globalCtx);
void ItemEtcetera_MoveFireArrowDown(ItemEtcetera* thisv, GlobalContext* globalCtx);
void func_80B85B28(ItemEtcetera* thisv, GlobalContext* globalCtx);
void ItemEtcetera_UpdateFireArrow(ItemEtcetera* thisv, GlobalContext* globalCtx);

ActorInit Item_Etcetera_InitVars = {
    ACTOR_ITEM_ETCETERA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ItemEtcetera),
    (ActorFunc)ItemEtcetera_Init,
    (ActorFunc)ItemEtcetera_Destroy,
    (ActorFunc)ItemEtcetera_Update,
    NULL,
    NULL,
};

static s16 sObjectIds[] = {
    OBJECT_GI_BOTTLE, OBJECT_GI_BOTTLE_LETTER, OBJECT_GI_SHIELD_2, OBJECT_GI_ARROWCASE, OBJECT_GI_SCALE,
    OBJECT_GI_SCALE,  OBJECT_GI_KEY,           OBJECT_GI_M_ARROW,  OBJECT_GI_RUPY,      OBJECT_GI_RUPY,
    OBJECT_GI_RUPY,   OBJECT_GI_RUPY,          OBJECT_GI_HEARTS,   OBJECT_GI_KEY,
};

// Indexes passed to the item table in z_draw.c
static s16 sDrawItemIndexes[] = {
    GID_BOTTLE,       GID_LETTER_RUTO,  GID_SHIELD_HYLIAN, GID_QUIVER_40,   GID_SCALE_SILVER,
    GID_SCALE_GOLDEN, GID_KEY_SMALL,    GID_ARROW_FIRE,    GID_RUPEE_GREEN, GID_RUPEE_BLUE,
    GID_RUPEE_RED,    GID_RUPEE_PURPLE, GID_HEART_PIECE,   GID_KEY_SMALL,
};

static s16 sGetItemIds[] = {
    GI_BOTTLE,     GI_LETTER_RUTO, GI_SHIELD_HYLIAN, GI_QUIVER_40, GI_SCALE_SILVER, GI_SCALE_GOLD, GI_KEY_SMALL,
    GI_ARROW_FIRE, GI_NONE,        GI_NONE,          GI_NONE,      GI_NONE,         GI_NONE,       GI_NONE,
};

void ItemEtcetera_SetupAction(ItemEtcetera* thisv, ItemEtceteraActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ItemEtcetera_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemEtcetera* thisv = (ItemEtcetera*)thisx;
    s32 pad;
    s32 type;
    s32 objBankIndex;

    type = thisv->actor.params & 0xFF;
    osSyncPrintf("no = %d\n", type);
    objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIds[type]);
    osSyncPrintf("bank_ID = %d\n", objBankIndex);
    if (objBankIndex < 0) {
        ASSERT(0, "0", "../z_item_etcetera.c", 241);
    } else {
        thisv->objBankIndex = objBankIndex;
    }
    thisv->giDrawId = sDrawItemIndexes[type];
    thisv->getItemId = sGetItemIds[type];
    thisv->futureActionFunc = func_80B85824;
    thisv->drawFunc = ItemEtcetera_Draw;
    Actor_SetScale(&thisv->actor, 0.25f);
    ItemEtcetera_SetupAction(thisv, func_80B857D0);
    switch (type) {
        case ITEM_ETC_LETTER:
            Actor_SetScale(&thisv->actor, 0.5f);
            thisv->futureActionFunc = func_80B858B4;
            if (gSaveContext.eventChkInf[3] & 2) {
                Actor_Kill(&thisv->actor);
            }
            break;
        case ITEM_ETC_ARROW_FIRE:
            thisv->futureActionFunc = ItemEtcetera_UpdateFireArrow;
            Actor_SetScale(&thisv->actor, 0.5f);
            thisv->actor.draw = NULL;
            thisv->actor.shape.yOffset = 50.0f;
            break;
        case ITEM_ETC_RUPEE_GREEN_CHEST_GAME:
        case ITEM_ETC_RUPEE_BLUE_CHEST_GAME:
        case ITEM_ETC_RUPEE_RED_CHEST_GAME:
        case ITEM_ETC_RUPEE_PURPLE_CHEST_GAME:
        case ITEM_ETC_HEART_PIECE_CHEST_GAME:
        case ITEM_ETC_KEY_SMALL_CHEST_GAME:
            Actor_SetScale(&thisv->actor, 0.5f);
            thisv->futureActionFunc = func_80B85B28;
            thisv->drawFunc = ItemEtcetera_DrawThroughLens;
            thisv->actor.world.pos.y += 15.0f;
            break;
    }
}

void ItemEtcetera_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_80B857D0(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex)) {
        thisv->actor.objBankIndex = thisv->objBankIndex;
        thisv->actor.draw = thisv->drawFunc;
        thisv->actionFunc = thisv->futureActionFunc;
    }
}

void func_80B85824(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        if ((thisv->actor.params & 0xFF) == 1) {
            gSaveContext.eventChkInf[3] |= 2;
            Flags_SetSwitch(globalCtx, 0xB);
        }
        Actor_Kill(&thisv->actor);
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 30.0f, 50.0f);
    }
}

void func_80B858B4(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        if ((thisv->actor.params & 0xFF) == 1) {
            gSaveContext.eventChkInf[3] |= 2;
            Flags_SetSwitch(globalCtx, 0xB);
        }
        Actor_Kill(&thisv->actor);
    } else {
        if (0) {} // Necessary to match
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 30.0f, 50.0f);
        if ((globalCtx->gameplayFrames & 0xD) == 0) {
            EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemEtcetera_SpawnSparkles(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.2f, 0.0f };
    static Vec3f accel = { 0.0f, 0.05f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 255, 50, 50, 0 };
    Vec3f pos;

    velocity.x = Rand_CenteredFloat(3.0f);
    velocity.z = Rand_CenteredFloat(3.0f);
    velocity.y = -0.05f;
    accel.y = -0.025f;
    pos.x = Rand_CenteredFloat(12.0f) + thisv->actor.world.pos.x;
    pos.y = (Rand_ZeroOne() * 6.0f) + thisv->actor.world.pos.y;
    pos.z = Rand_CenteredFloat(12.0f) + thisv->actor.world.pos.z;
    EffectSsKiraKira_SpawnDispersed(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 5000, 16);
}

void ItemEtcetera_MoveFireArrowDown(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 0.0f, 5);
    Actor_MoveForward(&thisv->actor);
    if (!(thisv->actor.bgCheckFlags & 1)) {
        ItemEtcetera_SpawnSparkles(thisv, globalCtx);
    }
    thisv->actor.shape.rot.y += 0x400;
    func_80B85824(thisv, globalCtx);
}

void func_80B85B28(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    if (Flags_GetTreasure(globalCtx, (thisv->actor.params >> 8) & 0x1F)) {
        Actor_Kill(&thisv->actor);
    }
}

void ItemEtcetera_UpdateFireArrow(ItemEtcetera* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[0] != NULL)) {
        LOG_NUM("(game_play->demo_play.npcdemopnt[0]->dousa)", globalCtx->csCtx.npcActions[0]->action,
                "../z_item_etcetera.c", 441);
        if (globalCtx->csCtx.npcActions[0]->action == 2) {
            thisv->actor.draw = ItemEtcetera_Draw;
            thisv->actor.gravity = -0.1f;
            thisv->actor.minVelocityY = -4.0f;
            thisv->actionFunc = ItemEtcetera_MoveFireArrowDown;
        }
    } else {
        thisv->actor.gravity = -0.1f;
        thisv->actor.minVelocityY = -4.0f;
        thisv->actionFunc = ItemEtcetera_MoveFireArrowDown;
    }
}

void ItemEtcetera_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemEtcetera* thisv = (ItemEtcetera*)thisx;
    thisv->actionFunc(thisv, globalCtx);
}

void ItemEtcetera_DrawThroughLens(Actor* thisx, GlobalContext* globalCtx) {
    ItemEtcetera* thisv = (ItemEtcetera*)thisx;
    if (globalCtx->actorCtx.unk_03 != 0) {
        func_8002EBCC(&thisv->actor, globalCtx, 0);
        func_8002ED80(&thisv->actor, globalCtx, 0);
        GetItem_Draw(globalCtx, thisv->giDrawId);
    }
}

void ItemEtcetera_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemEtcetera* thisv = (ItemEtcetera*)thisx;

    func_8002EBCC(&thisv->actor, globalCtx, 0);
    func_8002ED80(&thisv->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, thisv->giDrawId);
}
