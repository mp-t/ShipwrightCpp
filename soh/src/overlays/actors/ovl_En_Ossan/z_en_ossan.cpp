#include "z_en_ossan.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_ossan/object_ossan.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "objects/object_os/object_os.h"
#include "objects/object_zo/object_zo.h"
#include "objects/object_rs/object_rs.h"
#include "objects/object_ds2/object_ds2.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_masterkokiri/object_masterkokiri.h"
#include "objects/object_km1/object_km1.h"
#include "objects/object_mastergolon/object_mastergolon.h"
#include "objects/object_masterzoora/object_masterzoora.h"
#include "objects/object_masterkokirihead/object_masterkokirihead.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnOssan_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_Update(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawKokiriShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawPotionShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawBombchuShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawBazaarShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawZoraShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawGoronShopkeeper(Actor* thisx, GlobalContext* globalCtx);
void EnOssan_DrawHappyMaskShopkeeper(Actor* thisx, GlobalContext* globalCtx);

void EnOssan_InitActionFunc(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_MainActionFunc(EnOssan* thisv, GlobalContext* globalCtx);

void EnOssan_TalkDefaultShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkKokiriShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkKakarikoPotionShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkBombchuShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkMarketPotionShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkBazaarShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkZoraShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkGoronShopkeeper(GlobalContext* globalCtx);
void EnOssan_TalkHappyMaskShopkeeper(GlobalContext* globalCtx);

s16 ShopItemDisp_Default(s16 v);
s16 ShopItemDisp_SpookyMask(s16 v);
s16 ShopItemDisp_SkullMask(s16 v);
s16 ShopItemDisp_BunnyHood(s16 v);
s16 ShopItemDisp_ZoraMask(s16 v);
s16 ShopItemDisp_GoronMask(s16 v);
s16 ShopItemDisp_GerudoMask(s16 v);

void EnOssan_InitKokiriShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitPotionShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitBombchuShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitBazaarShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitZoraShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitGoronShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);
void EnOssan_InitHappyMaskShopkeeper(EnOssan* thisv, GlobalContext* globalCtx);

void EnOssan_State_Idle(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_StartConversation(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_FacingShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_TalkingToShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_LookToLeftShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_LookToRightShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_BrowseLeftShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_BrowseRightShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_LookFromShelfToShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_ItemSelected(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_SelectMilkBottle(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_SelectWeirdEgg(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_SelectUnimplementedItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_SelectBombs(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_CantGetItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_GiveItemWithFanfare(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_ItemPurchased(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_ContinueShoppingPrompt(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_GiveLonLonMilk(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_DisplayOnlyBombDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_WaitForDisplayOnlyBombDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_21(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_22(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_QuickBuyDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_SelectMaskItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_LendMaskOfTruth(EnOssan* thisv, GlobalContext* globalCtx, Player* player);
void EnOssan_State_GiveDiscountDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player);

void EnOssan_Obj3ToSeg6(EnOssan* thisv, GlobalContext* globalCtx);

void EnOssan_StartShopping(GlobalContext* globalCtx, EnOssan* thisv);

void EnOssan_WaitForBlink(EnOssan* thisv);
void EnOssan_Blink(EnOssan* thisv);

u16 EnOssan_SetupHelloDialog(EnOssan* thisv);

s32 EnOssan_TakeItemOffShelf(EnOssan* thisv);
s32 EnOssan_ReturnItemToShelf(EnOssan* thisv);
void EnOssan_ResetItemPosition(EnOssan* thisv);
void EnOssan_SetStateGiveDiscountDialog(GlobalContext* globalCtx, EnOssan* thisv);

#define CURSOR_INVALID 0xFF

ActorInit En_Ossan_InitVars = {
    ACTOR_EN_OSSAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnOssan),
    (ActorFunc)EnOssan_Init,
    (ActorFunc)EnOssan_Destroy,
    (ActorFunc)EnOssan_Update,
    NULL,
    NULL,
};

// Unused collider
static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

// Rupees to pay back to Happy Mask Shop
static s16 sMaskPaymentPrice[] = { 10, 30, 20, 50 };

// item yaw offsets
static s16 sItemShelfRot[] = { 0xEAAC, 0xEAAC, 0xEAAC, 0xEAAC, 0x1554, 0x1554, 0x1554, 0x1554 };

// unused values?
static s16 D_80AC8904[] = { 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025 };

static const char* sShopkeeperPrintName[] = {
    "コキリの店  ", // "Kokiri Shop"
    "薬屋        ", // "Potion Shop"
    "夜の店      ", // "Night Shop"
    "路地裏の店  ", // "Back Alley Shop"
    "盾の店      ", // "Shield Shop"
    "大人の店    ", // "Adult Shop"
    "タロンの店  ", // "Talon Shop"
    "ゾーラの店  ", // "Zora Shop"
    "ゴロン夜の店", // "Goron Night Shop"
    "インゴーの店", // "Ingo Store"
    "お面屋      ", // "Mask Shop"
};

typedef struct {
    /* 0x00 */ s16 objId;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} ShopkeeperObjInfo;

static s16 sShopkeeperObjectIds[][3] = {
    { OBJECT_KM1, OBJECT_MASTERKOKIRIHEAD, OBJECT_MASTERKOKIRI },
    { OBJECT_DS2, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_RS, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_DS2, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_ZO, OBJECT_ID_MAX, OBJECT_MASTERZOORA },
    { OBJECT_OF1D_MAP, OBJECT_ID_MAX, OBJECT_MASTERGOLON },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OS, OBJECT_ID_MAX, OBJECT_ID_MAX },
};

static EnOssanTalkOwnerFunc sShopkeeperTalkOwner[] = {
    EnOssan_TalkKokiriShopkeeper,       EnOssan_TalkKakarikoPotionShopkeeper, EnOssan_TalkBombchuShopkeeper,
    EnOssan_TalkMarketPotionShopkeeper, EnOssan_TalkBazaarShopkeeper,         EnOssan_TalkDefaultShopkeeper,
    EnOssan_TalkDefaultShopkeeper,      EnOssan_TalkZoraShopkeeper,           EnOssan_TalkGoronShopkeeper,
    EnOssan_TalkDefaultShopkeeper,      EnOssan_TalkHappyMaskShopkeeper,
};

static f32 sShopkeeperScale[] = {
    0.01f, 0.011f, 0.0105f, 0.011f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f,
};

typedef struct {
    /* 0x00 */ s16 shopItemIndex;
    /* 0x02 */ s16 xOffset;
    /* 0x04 */ s16 yOffset;
    /* 0x06 */ s16 zOffset;
} ShopItem; // size 0x08

ShopItem sShopkeeperStores[][8] = {
    { { SI_DEKU_SHIELD, 50, 52, -20 },
      { SI_DEKU_NUTS_5, 50, 76, -20 },
      { SI_DEKU_NUTS_10, 80, 52, -3 },
      { SI_DEKU_STICK, 80, 76, -3 },
      { SI_DEKU_SEEDS_30, -50, 52, -20 },
      { SI_ARROWS_10, -50, 76, -20 },
      { SI_ARROWS_30, -80, 52, -3 },
      { SI_HEART, -80, 76, -3 } },

    { { SI_GREEN_POTION, 50, 52, -20 },
      { SI_BLUE_FIRE, 50, 76, -20 },
      { SI_RED_POTION_R30, 80, 52, -3 },
      { SI_FAIRY, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_BUGS, -50, 76, -20 },
      { SI_POE, -80, 52, -3 },
      { SI_FISH, -80, 76, -3 } },

    { { SI_BOMBCHU_10_2, 50, 52, -20 },
      { SI_BOMBCHU_10_4, 50, 76, -20 },
      { SI_BOMBCHU_10_3, 80, 52, -3 },
      { SI_BOMBCHU_10_1, 80, 76, -3 },
      { SI_BOMBCHU_20_3, -50, 52, -20 },
      { SI_BOMBCHU_20_1, -50, 76, -20 },
      { SI_BOMBCHU_20_4, -80, 52, -3 },
      { SI_BOMBCHU_20_2, -80, 76, -3 } },

    { { SI_GREEN_POTION, 50, 52, -20 },
      { SI_BLUE_FIRE, 50, 76, -20 },
      { SI_RED_POTION_R30, 80, 52, -3 },
      { SI_FAIRY, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_BUGS, -50, 76, -20 },
      { SI_POE, -80, 52, -3 },
      { SI_FISH, -80, 76, -3 } },

    { { SI_HYLIAN_SHIELD, 50, 52, -20 },
      { SI_BOMBS_5_R35, 50, 76, -20 },
      { SI_DEKU_NUTS_5, 80, 52, -3 },
      { SI_HEART, 80, 76, -3 },
      { SI_ARROWS_10, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_DEKU_STICK, -80, 52, -3 },
      { SI_ARROWS_30, -80, 76, -3 } },

    { { SI_HYLIAN_SHIELD, 50, 52, -20 },
      { SI_BOMBS_5_R25, 50, 76, -20 },
      { SI_DEKU_NUTS_5, 80, 52, -3 },
      { SI_HEART, 80, 76, -3 },
      { SI_ARROWS_10, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_DEKU_STICK, -80, 52, -3 },
      { SI_ARROWS_30, -80, 76, -3 } },

    { { SI_MILK_BOTTLE, 50, 52, -20 },
      { SI_DEKU_NUTS_5, 50, 76, -20 },
      { SI_DEKU_NUTS_10, 80, 52, -3 },
      { SI_HEART, 80, 76, -3 },
      { SI_WEIRD_EGG, -50, 52, -20 },
      { SI_DEKU_STICK, -50, 76, -20 },
      { SI_HEART, -80, 52, -3 },
      { SI_HEART, -80, 76, -3 } },

    { { SI_ZORA_TUNIC, 50, 52, -20 },
      { SI_ARROWS_10, 50, 76, -20 },
      { SI_HEART, 80, 52, -3 },
      { SI_ARROWS_30, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_FISH, -80, 52, -3 },
      { SI_RED_POTION_R50, -80, 76, -3 } },

    { { SI_BOMBS_5_R25, 50, 52, -20 },
      { SI_BOMBS_10, 50, 76, -20 },
      { SI_BOMBS_20, 80, 52, -3 },
      { SI_BOMBS_30, 80, 76, -3 },
      { SI_GORON_TUNIC, -50, 52, -20 },
      { SI_HEART, -50, 76, -20 },
      { SI_RED_POTION_R40, -80, 52, -3 },
      { SI_HEART, -80, 76, -3 } },

    { { SI_19, 50, 52, -20 },
      { SI_19, 50, 76, -20 },
      { SI_19, 80, 52, -3 },
      { SI_19, 80, 76, -3 },
      { SI_20, -50, 52, -20 },
      { SI_20, -50, 76, -20 },
      { SI_20, -80, 52, -3 },
      { SI_20, -80, 76, -3 } },

    { { SI_GERUDO_MASK, 50, 52, -20 },
      { SI_ZORA_MASK, 50, 76, -20 },
      { SI_MASK_OF_TRUTH, 80, 52, -3 },
      { SI_GORON_MASK, 80, 76, -3 },
      { SI_SKULL_MASK, -50, 52, -20 },
      { SI_KEATON_MASK, -50, 76, -20 },
      { SI_BUNNY_HOOD, -80, 52, -3 },
      { SI_SPOOKY_MASK, -80, 76, -3 } },
};
static EnOssanGetGirlAParamsFunc sShopItemReplaceFunc[] = {
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_SpookyMask,
    ShopItemDisp_SkullMask, ShopItemDisp_BunnyHood,  ShopItemDisp_Default, ShopItemDisp_ZoraMask,
    ShopItemDisp_GoronMask, ShopItemDisp_GerudoMask, ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,    ShopItemDisp_Default, ShopItemDisp_Default,
    ShopItemDisp_Default,   ShopItemDisp_Default,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

// When selecting an item to buy, thisv is the position the item moves to
static Vec3f sSelectedItemPosition[] = { { 17.0f, 58.0f, 30.0f }, { -17.0f, 58.0f, 30.0f } };

static EnOssanInitFunc sInitFuncs[] = {
    EnOssan_InitKokiriShopkeeper, EnOssan_InitPotionShopkeeper,    EnOssan_InitBombchuShopkeeper,
    EnOssan_InitPotionShopkeeper, EnOssan_InitBazaarShopkeeper,    EnOssan_InitBazaarShopkeeper,
    EnOssan_InitBazaarShopkeeper, EnOssan_InitZoraShopkeeper,      EnOssan_InitGoronShopkeeper,
    EnOssan_InitBazaarShopkeeper, EnOssan_InitHappyMaskShopkeeper,
};

static Vec3f sShopkeeperPositionOffsets[] = {
    { 0.0f, 0.0f, 33.0f }, { 0.0f, 0.0f, 31.0f }, { 0.0f, 0.0f, 31.0f }, { 0.0f, 0.0f, 31.0f },
    { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 36.0f },
    { 0.0f, 0.0f, 15.0f }, { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 26.0f },
};

static EnOssanStateFunc sStateFunc[] = {
    EnOssan_State_Idle,
    EnOssan_State_StartConversation,
    EnOssan_State_FacingShopkeeper,
    EnOssan_State_TalkingToShopkeeper,
    EnOssan_State_LookToLeftShelf,
    EnOssan_State_LookToRightShelf,
    EnOssan_State_BrowseLeftShelf,
    EnOssan_State_BrowseRightShelf,
    EnOssan_State_LookFromShelfToShopkeeper,
    EnOssan_State_ItemSelected,
    EnOssan_State_SelectMilkBottle,
    EnOssan_State_SelectWeirdEgg,
    EnOssan_State_SelectUnimplementedItem,
    EnOssan_State_SelectBombs,
    EnOssan_State_CantGetItem,
    EnOssan_State_GiveItemWithFanfare,
    EnOssan_State_ItemPurchased,
    EnOssan_State_ContinueShoppingPrompt,
    EnOssan_State_GiveLonLonMilk,
    EnOssan_State_DisplayOnlyBombDialog,
    EnOssan_State_WaitForDisplayOnlyBombDialog,
    EnOssan_State_21,
    EnOssan_State_22,
    EnOssan_State_QuickBuyDialog,
    EnOssan_State_SelectMaskItem,
    EnOssan_State_LendMaskOfTruth,
    EnOssan_State_GiveDiscountDialog,
};

void EnOssan_SetupAction(EnOssan* thisv, EnOssanActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

s16 ShopItemDisp_Default(s16 v) {
    return v;
}

s16 ShopItemDisp_SpookyMask(s16 v) {
    // Sold Skull Mask
    if (gSaveContext.itemGetInf[3] & 0x200) {
        return v;
    }
    return -1;
}

s16 ShopItemDisp_SkullMask(s16 v) {
    // Sold Keaton Mask
    if (gSaveContext.itemGetInf[3] & 0x100) {
        return v;
    }
    return -1;
}

s16 ShopItemDisp_BunnyHood(s16 v) {
    // Sold Spooky Mask
    if (gSaveContext.itemGetInf[3] & 0x400) {
        return v;
    }
    return -1;
}

s16 ShopItemDisp_ZoraMask(s16 v) {
    // Obtained Mask of Truth
    if (gSaveContext.itemGetInf[3] & 0x8000) {
        return v;
    }
    return -1;
}

s16 ShopItemDisp_GoronMask(s16 v) {
    // Obtained Mask of Truth
    if (gSaveContext.itemGetInf[3] & 0x8000) {
        return v;
    }
    return -1;
}

s16 ShopItemDisp_GerudoMask(s16 v) {
    // Obtained Mask of Truth
    if (gSaveContext.itemGetInf[3] & 0x8000) {
        return v;
    }
    return -1;
}

void EnOssan_SpawnItemsOnShelves(EnOssan* thisv, GlobalContext* globalCtx, ShopItem* shopItems) {
    EnTana* shelves;
    s16 itemParams;
    s32 i;

    for (i = 0; i < 8; i++, shopItems++) {
        if (shopItems->shopItemIndex < 0) {
            thisv->shelfSlots[i] = NULL;
        } else {
            itemParams = sShopItemReplaceFunc[shopItems->shopItemIndex](shopItems->shopItemIndex);

            if (itemParams < 0) {
                thisv->shelfSlots[i] = NULL;
            } else {
                shelves = thisv->shelves;
                thisv->shelfSlots[i] = (EnGirlA*)Actor_Spawn(
                    &globalCtx->actorCtx, globalCtx, ACTOR_EN_GIRLA, shelves->actor.world.pos.x + shopItems->xOffset,
                    shelves->actor.world.pos.y + shopItems->yOffset, shelves->actor.world.pos.z + shopItems->zOffset,
                    shelves->actor.shape.rot.x, shelves->actor.shape.rot.y + sItemShelfRot[i],
                    shelves->actor.shape.rot.z, itemParams);
            }
        }
    }
}

void EnOssan_UpdateShopOfferings(EnOssan* thisv, GlobalContext* globalCtx) {
    s32 i;
    ShopItem* storeItems;
    ShopItem* shopItem;

    if (thisv->actor.params == OSSAN_TYPE_MASK) {
        storeItems = sShopkeeperStores[thisv->actor.params];
        if (1) {}
        for (i = 0; i < 8; i++) {
            shopItem = &storeItems[i];
            if (shopItem->shopItemIndex >= 0 && thisv->shelfSlots[i] == NULL) {
                s16 params = sShopItemReplaceFunc[shopItem->shopItemIndex](shopItem->shopItemIndex);

                if (params >= 0) {
                    thisv->shelfSlots[i] = (EnGirlA*)Actor_Spawn(
                        &globalCtx->actorCtx, globalCtx, ACTOR_EN_GIRLA,
                        thisv->shelves->actor.world.pos.x + shopItem->xOffset,
                        thisv->shelves->actor.world.pos.y + shopItem->yOffset,
                        thisv->shelves->actor.world.pos.z + shopItem->zOffset, thisv->shelves->actor.shape.rot.x,
                        thisv->shelves->actor.shape.rot.y + sItemShelfRot[i], thisv->shelves->actor.shape.rot.z, params);
                }
            }
        }
    }
}

void EnOssan_TalkDefaultShopkeeper(GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x9E);
}

void EnOssan_TalkKakarikoPotionShopkeeper(GlobalContext* globalCtx) {
    if (globalCtx->curSpawn == 0) {
        Message_ContinueTextbox(globalCtx, 0x5046);
    } else {
        Message_ContinueTextbox(globalCtx, 0x504E);
    }
}

void EnOssan_TalkMarketPotionShopkeeper(GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x504E);
}

void EnOssan_TalkKokiriShopkeeper(GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x10BA);
}

void EnOssan_TalkBazaarShopkeeper(GlobalContext* globalCtx) {
    if (globalCtx->curSpawn == 0) {
        Message_ContinueTextbox(globalCtx, 0x9D);
    } else {
        Message_ContinueTextbox(globalCtx, 0x9C);
    }
}

void EnOssan_TalkBombchuShopkeeper(GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x7076);
}

void EnOssan_TalkZoraShopkeeper(GlobalContext* globalCtx) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        Message_ContinueTextbox(globalCtx, 0x403A);
    } else {
        Message_ContinueTextbox(globalCtx, 0x403B);
    }
}

// Goron City, Goron
void EnOssan_TalkGoronShopkeeper(GlobalContext* globalCtx) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        if (gSaveContext.eventChkInf[2] & 0x20) {
            Message_ContinueTextbox(globalCtx, 0x3028);
        } else if (CUR_UPG_VALUE(UPG_STRENGTH) != 0) {
            Message_ContinueTextbox(globalCtx, 0x302D);
        } else {
            Message_ContinueTextbox(globalCtx, 0x300F);
        }
    } else if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        Message_ContinueTextbox(globalCtx, 0x3057);
    } else {
        Message_ContinueTextbox(globalCtx, 0x305B);
    }
}

// Happy Mask Shop
void EnOssan_TalkHappyMaskShopkeeper(GlobalContext* globalCtx) {
    if ((gSaveContext.itemGetInf[3] & 0x100)       // Sold Keaton Mask
        && (gSaveContext.itemGetInf[3] & 0x200)    // Sold Skull Mask
        && (gSaveContext.itemGetInf[3] & 0x400)    // Sold Spooky Mask
        && (gSaveContext.itemGetInf[3] & 0x800)) { // Sold Bunny Hood
        Message_ContinueTextbox(globalCtx, 0x70AE);
    } else {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 1:
                Message_ContinueTextbox(globalCtx, 0x70A4);
                break;
            case 0:
                Message_ContinueTextbox(globalCtx, 0x70A3);
                break;
        }
    }
}

void EnOssan_UpdateCameraDirection(EnOssan* thisv, GlobalContext* globalCtx, f32 cameraFaceAngle) {
    thisv->cameraFaceAngle = cameraFaceAngle;
    Camera_SetCameraData(GET_ACTIVE_CAM(globalCtx), 0xC, NULL, NULL, cameraFaceAngle, 0, 0);
}

s32 EnOssan_TryGetObjBankIndexes(EnOssan* thisv, GlobalContext* globalCtx, s16* objectIds) {
    if (objectIds[1] != OBJECT_ID_MAX) {
        thisv->objBankIndex2 = Object_GetIndex(&globalCtx->objectCtx, objectIds[1]);
        if (thisv->objBankIndex2 < 0) {
            return false;
        }
    } else {
        thisv->objBankIndex2 = -1;
    }
    if (objectIds[2] != OBJECT_ID_MAX) {
        thisv->objBankIndex3 = Object_GetIndex(&globalCtx->objectCtx, objectIds[2]);
        if (thisv->objBankIndex3 < 0) {
            return false;
        }
    } else {
        thisv->objBankIndex3 = -1;
    }
    return true;
}

void EnOssan_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;
    s16* objectIds;

    if (thisv->actor.params == OSSAN_TYPE_TALON && (LINK_AGE_IN_YEARS != YEARS_CHILD)) {
        thisv->actor.params = OSSAN_TYPE_INGO;
    }

    //! @bug This check will always evaluate to false, it should be || not &&
    if (thisv->actor.params > OSSAN_TYPE_MASK && thisv->actor.params < OSSAN_TYPE_KOKIRI) {
        Actor_Kill(&thisv->actor);
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("引数がおかしいよ(arg_data=%d)！！\n", thisv->actor.params);
        osSyncPrintf(VT_RST);
        ASSERT(0, "0", "../z_en_oB1.c", 1246);
        return;
    }

    // If you've given Zelda's Letter to the Kakariko Guard
    if (thisv->actor.params == OSSAN_TYPE_MASK && !(gSaveContext.infTable[7] & 0x40)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->actor.params == OSSAN_TYPE_KAKARIKO_POTION && (LINK_AGE_IN_YEARS == YEARS_CHILD)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    // Completed Dodongo's Cavern
    if (thisv->actor.params == OSSAN_TYPE_BOMBCHUS && !(gSaveContext.eventChkInf[2] & 0x20)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    objectIds = sShopkeeperObjectIds[thisv->actor.params];
    thisv->objBankIndex1 = Object_GetIndex(&globalCtx->objectCtx, objectIds[0]);

    if (thisv->objBankIndex1 < 0) {
        Actor_Kill(&thisv->actor);
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("バンクが無いよ！！(%s)\n", sShopkeeperPrintName[thisv->actor.params]);
        osSyncPrintf(VT_RST);
        ASSERT(0, "0", "../z_en_oB1.c", 1284);
        return;
    }

    if (EnOssan_TryGetObjBankIndexes(thisv, globalCtx, objectIds) == 0) {
        Actor_Kill(&thisv->actor);
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("予備バンクが無いよ！！(%s)\n", sShopkeeperPrintName[thisv->actor.params]);
        osSyncPrintf(VT_RST);
        ASSERT(0, "0", "../z_en_oB1.c", 1295);
        return;
    }

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    EnOssan_SetupAction(thisv, EnOssan_InitActionFunc);
}

void EnOssan_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnOssan* thisv = (EnOssan*)thisx;
    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnOssan_UpdateCursorPos(GlobalContext* globalCtx, EnOssan* thisv) {
    s16 x;
    s16 y;

    Actor_GetScreenPos(globalCtx, &thisv->shelfSlots[thisv->cursorIndex]->actor, &x, &y);
    thisv->cursorX = x;
    thisv->cursorY = y;
}

void EnOssan_EndInteraction(GlobalContext* globalCtx, EnOssan* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    // "End of conversation!"
    osSyncPrintf(VT_FGCOL(YELLOW) "%s[%d]:★★★ 会話終了！！ ★★★" VT_RST "\n", "../z_en_oB1.c", 1337);
    YREG(31) = 0;
    Actor_ProcessTalkRequest(&thisv->actor, globalCtx);
    globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    globalCtx->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~0x20000000;
    func_800BC490(globalCtx, 1);
    Interface_ChangeAlpha(50);
    thisv->drawCursor = 0;
    thisv->stickLeftPrompt.isEnabled = false;
    thisv->stickRightPrompt.isEnabled = false;
    EnOssan_UpdateCameraDirection(thisv, globalCtx, 0.0f);
    thisv->actor.textId = EnOssan_SetupHelloDialog(thisv);
    thisv->stateFlag = OSSAN_STATE_IDLE;
}

s32 EnOssan_TestEndInteraction(EnOssan* thisv, GlobalContext* globalCtx, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        EnOssan_EndInteraction(globalCtx, thisv);
        return true;
    } else {
        return false;
    }
}

s32 EnOssan_TestCancelOption(EnOssan* thisv, GlobalContext* globalCtx, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        thisv->stateFlag = thisv->tempStateFlag;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
        return true;
    } else {
        return false;
    }
}

void EnOssan_SetStateStartShopping(GlobalContext* globalCtx, EnOssan* thisv, u8 skipHelloState) {
    YREG(31) = 1;
    thisv->headRot = thisv->headTargetRot = 0;
    Interface_SetDoAction(globalCtx, DO_ACTION_NEXT);
    EnOssan_UpdateCameraDirection(thisv, globalCtx, 0);

    if (!skipHelloState) {
        thisv->stateFlag = OSSAN_STATE_START_CONVERSATION;
    } else {
        EnOssan_StartShopping(globalCtx, thisv);
    }
}

void EnOssan_StartShopping(GlobalContext* globalCtx, EnOssan* thisv) {
    thisv->stateFlag = OSSAN_STATE_FACING_SHOPKEEPER;

    if (thisv->actor.params == OSSAN_TYPE_MASK) {
        // if all masks have been sold, give the option to ask about the mask of truth
        if ((gSaveContext.itemGetInf[3] & 0x100) && (gSaveContext.itemGetInf[3] & 0x200) &&
            (gSaveContext.itemGetInf[3] & 0x400) && (gSaveContext.itemGetInf[3] & 0x800)) {
            Message_ContinueTextbox(globalCtx, 0x70AD);
        } else {
            Message_ContinueTextbox(globalCtx, 0x70A2);
        }
    } else {
        Message_ContinueTextbox(globalCtx, 0x83);
    }

    Interface_SetDoAction(globalCtx, DO_ACTION_DECIDE);
    thisv->stickRightPrompt.isEnabled = true;
    thisv->stickLeftPrompt.isEnabled = true;
    EnOssan_UpdateCameraDirection(thisv, globalCtx, 0.0f);
}

void EnOssan_ChooseTalkToOwner(GlobalContext* globalCtx, EnOssan* thisv) {
    thisv->stateFlag = OSSAN_STATE_TALKING_TO_SHOPKEEPER;
    sShopkeeperTalkOwner[thisv->actor.params](globalCtx);
    Interface_SetDoAction(globalCtx, DO_ACTION_DECIDE);
    thisv->stickLeftPrompt.isEnabled = false;
    thisv->stickRightPrompt.isEnabled = false;
}

void EnOssan_SetLookToShopkeeperFromShelf(GlobalContext* globalCtx, EnOssan* thisv) {
    func_80078884(NA_SE_SY_CURSOR);
    thisv->drawCursor = 0;
    thisv->stateFlag = OSSAN_STATE_LOOK_SHOPKEEPER;
}

void EnOssan_State_Idle(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    thisv->headTargetRot = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        // "Start conversation!!"
        osSyncPrintf(VT_FGCOL(YELLOW) "★★★ 会話開始！！ ★★★" VT_RST "\n");
        player->stateFlags2 |= 0x20000000;
        func_800BC590(globalCtx);
        EnOssan_SetStateStartShopping(globalCtx, thisv, false);
    } else if (thisv->actor.xzDistToPlayer < 100.0f) {
        func_8002F2CC(&thisv->actor, globalCtx, 100);
    }
}

void EnOssan_UpdateJoystickInputState(GlobalContext* globalCtx, EnOssan* thisv) {
    Input* input = &globalCtx->state.input[0];
    s8 stickX = input->rel.stick_x;
    s8 stickY = input->rel.stick_y;

    thisv->moveHorizontal = thisv->moveVertical = false;

    if (thisv->stickAccumX == 0) {
        if (stickX > 30 || stickX < -30) {
            thisv->stickAccumX = stickX;
            thisv->moveHorizontal = true;
        }
    } else if (stickX <= 30 && stickX >= -30) {
        thisv->stickAccumX = 0;
    } else if (thisv->stickAccumX * stickX < 0) { // Stick has swapped directions
        thisv->stickAccumX = stickX;
        thisv->moveHorizontal = true;
    } else {
        thisv->stickAccumX += stickX;

        if (thisv->stickAccumX > 2000) {
            thisv->stickAccumX = 2000;
        } else if (thisv->stickAccumX < -2000) {
            thisv->stickAccumX = -2000;
        }
    }

    if (thisv->stickAccumY == 0) {
        if (stickY > 30 || stickY < -30) {
            thisv->stickAccumY = stickY;
            thisv->moveVertical = true;
        }
    } else if (stickY <= 30 && stickY >= -30) {
        thisv->stickAccumY = 0;
    } else if (thisv->stickAccumY * stickY < 0) { // Stick has swapped directions
        thisv->stickAccumY = stickY;
        thisv->moveVertical = true;
    } else {
        thisv->stickAccumY += stickY;

        if (thisv->stickAccumY > 2000) {
            thisv->stickAccumY = 2000;
        } else if (thisv->stickAccumY < -2000) {
            thisv->stickAccumY = -2000;
        }
    }
}

u8 EnOssan_SetCursorIndexFromNeutral(EnOssan* thisv, u8 shelfOffset) {
    u8 i;

    // if cursor is on the top shelf
    if (thisv->cursorIndex & 1) {
        // scan top shelf for non-null item
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (thisv->shelfSlots[i] != NULL) {
                return i;
            }
        }
        // scan bottom shelf for non-null item
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (thisv->shelfSlots[i] != NULL) {
                return i;
            }
        }
    } else {
        // scan bottom shelf for non-null item
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (thisv->shelfSlots[i] != NULL) {
                return i;
            }
        }
        // scan top shelf for non-null item
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (thisv->shelfSlots[i] != NULL) {
                return i;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 EnOssan_CursorRight(EnOssan* thisv, u8 cursorIndex, u8 shelfSlotMin) {
    u8 c = shelfSlotMin + 4;

    while (cursorIndex >= shelfSlotMin && cursorIndex < c) {
        cursorIndex -= 2;
        if (cursorIndex >= shelfSlotMin && cursorIndex < c) {
            if (thisv->shelfSlots[cursorIndex] != NULL) {
                return cursorIndex;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 EnOssan_CursorLeft(EnOssan* thisv, u8 cursorIndex, u8 shelfSlotMax) {

    while (cursorIndex < shelfSlotMax) {
        cursorIndex += 2;
        if ((cursorIndex < shelfSlotMax) && thisv->shelfSlots[cursorIndex] != NULL) {
            return cursorIndex;
        }
    }
    return CURSOR_INVALID;
}

// pay salesman back
void EnOssan_TryPaybackMask(EnOssan* thisv, GlobalContext* globalCtx) {
    s16 price = sMaskPaymentPrice[thisv->happyMaskShopState];

    if (gSaveContext.rupees < price) {
        Message_ContinueTextbox(globalCtx, 0x70A8);
        thisv->happyMaskShopkeeperEyeIdx = 1;
        thisv->happyMaskShopState = OSSAN_HAPPY_STATE_ANGRY;
    } else {
        Rupees_ChangeBy(-price);

        if (thisv->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD) {
            gSaveContext.eventChkInf[8] |= 0x8000;
            Message_ContinueTextbox(globalCtx, 0x70A9);
            thisv->happyMaskShopState = OSSAN_HAPPY_STATE_ALL_MASKS_SOLD;
            return;
        }

        if (thisv->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK) {
            gSaveContext.eventChkInf[8] |= 0x1000;
        } else if (thisv->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK) {
            gSaveContext.eventChkInf[8] |= 0x4000;
        } else if (thisv->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK) {
            gSaveContext.eventChkInf[8] |= 0x2000;
        }

        Message_ContinueTextbox(globalCtx, 0x70A7);
        thisv->happyMaskShopState = OSSAN_HAPPY_STATE_NONE;
    }
    thisv->stateFlag = OSSAN_STATE_START_CONVERSATION;
}

void EnOssan_State_StartConversation(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    u8 dialogState = Message_GetState(&globalCtx->msgCtx);

    if (thisv->actor.params == OSSAN_TYPE_MASK && dialogState == TEXT_STATE_CHOICE) {
        if (!EnOssan_TestEndInteraction(thisv, globalCtx, &globalCtx->state.input[0]) &&
            Message_ShouldAdvance(globalCtx)) {
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    EnOssan_StartShopping(globalCtx, thisv);
                    break;
                case 1:
                    EnOssan_EndInteraction(globalCtx, thisv);
                    break;
            }
        }
    } else if (dialogState == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        func_80078884(NA_SE_SY_MESSAGE_PASS);

        switch (thisv->happyMaskShopState) {
            case OSSAN_HAPPY_STATE_ALL_MASKS_SOLD:
                Message_ContinueTextbox(globalCtx, 0x70AA);
                thisv->stateFlag = OSSAN_STATE_LEND_MASK_OF_TRUTH;
                return;
            case OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK:
                EnOssan_EndInteraction(globalCtx, thisv);
                return;
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD:
                EnOssan_TryPaybackMask(thisv, globalCtx);
                return;
            case OSSAN_HAPPY_STATE_ANGRY:
                globalCtx->nextEntranceIndex = 0x1D1;
                globalCtx->sceneLoadFlag = 0x14;
                globalCtx->fadeTransition = 0x2E;
                return;
        }

        if (!EnOssan_TestEndInteraction(thisv, globalCtx, &globalCtx->state.input[0])) {
            // "Shop around by moving the stick left and right"
            osSyncPrintf("「スティック左右で品物みてくれ！」\n");
            EnOssan_StartShopping(globalCtx, thisv);
        }
    }

    if (1) {}
}

s32 EnOssan_FacingShopkeeperDialogResult(EnOssan* thisv, GlobalContext* globalCtx) {
    switch (globalCtx->msgCtx.choiceIndex) {
        case 0:
            EnOssan_ChooseTalkToOwner(globalCtx, thisv);
            return true;
        case 1:
            EnOssan_EndInteraction(globalCtx, thisv);
            return true;
        default:
            return false;
    }
}

void EnOssan_State_FacingShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    u8 nextIndex;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) &&
        !EnOssan_TestEndInteraction(thisv, globalCtx, &globalCtx->state.input[0])) {
        if (Message_ShouldAdvance(globalCtx) && EnOssan_FacingShopkeeperDialogResult(thisv, globalCtx)) {
            func_80078884(NA_SE_SY_DECIDE);
            return;
        }
        // Stick Left
        if (thisv->stickAccumX < 0) {
            nextIndex = EnOssan_SetCursorIndexFromNeutral(thisv, 4);
            if (nextIndex != CURSOR_INVALID) {
                thisv->cursorIndex = nextIndex;
                thisv->stateFlag = OSSAN_STATE_LOOK_SHELF_LEFT;
                Interface_SetDoAction(globalCtx, DO_ACTION_DECIDE);
                thisv->stickLeftPrompt.isEnabled = false;
                func_80078884(NA_SE_SY_CURSOR);
            }
        } else if (thisv->stickAccumX > 0) {
            nextIndex = EnOssan_SetCursorIndexFromNeutral(thisv, 0);
            if (nextIndex != CURSOR_INVALID) {
                thisv->cursorIndex = nextIndex;
                thisv->stateFlag = OSSAN_STATE_LOOK_SHELF_RIGHT;
                Interface_SetDoAction(globalCtx, DO_ACTION_DECIDE);
                thisv->stickRightPrompt.isEnabled = false;
                func_80078884(NA_SE_SY_CURSOR);
            }
        }
    }
}

void EnOssan_State_TalkingToShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        EnOssan_StartShopping(globalCtx, thisv);
    }
}

void EnOssan_State_LookToLeftShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    Math_ApproachF(&thisv->cameraFaceAngle, 30.0f, 0.5f, 10.0f);

    if (thisv->cameraFaceAngle > 29.5f) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, 30.0f);
    }

    EnOssan_UpdateCameraDirection(thisv, globalCtx, thisv->cameraFaceAngle);

    if (thisv->cameraFaceAngle >= 30.0f) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, 30.0f);
        EnOssan_UpdateCursorPos(globalCtx, thisv);
        thisv->stateFlag = OSSAN_STATE_BROWSE_LEFT_SHELF;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
    } else {
        thisv->stickAccumX = 0;
    }
}

void EnOssan_State_LookToRightShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    Math_ApproachF(&thisv->cameraFaceAngle, -30.0f, 0.5f, 10.0f);

    if (thisv->cameraFaceAngle < -29.5f) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, -30.0f);
    }

    EnOssan_UpdateCameraDirection(thisv, globalCtx, thisv->cameraFaceAngle);

    if (thisv->cameraFaceAngle <= -30.0f) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, -30.0f);
        EnOssan_UpdateCursorPos(globalCtx, thisv);
        thisv->stateFlag = OSSAN_STATE_BROWSE_RIGHT_SHELF;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
    } else {
        thisv->stickAccumX = 0;
    }
}

void EnOssan_CursorUpDown(EnOssan* thisv) {
    u8 curTemp = thisv->cursorIndex;
    u8 curScanTemp;

    if (thisv->stickAccumY < 0) {
        curTemp &= 0xFE;
        if (thisv->shelfSlots[curTemp] != NULL) {
            thisv->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 0;
            }
            while (curScanTemp != curTemp) {
                if (thisv->shelfSlots[curScanTemp] != NULL) {
                    thisv->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 0;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 4;
            }
            while (curScanTemp != curTemp) {
                if (thisv->shelfSlots[curScanTemp] != NULL) {
                    thisv->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 4;
                }
            }
        }
    } else if (thisv->stickAccumY > 0) {
        curTemp |= 1;
        if (thisv->shelfSlots[curTemp] != NULL) {
            thisv->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 1;
            }
            while (curScanTemp != curTemp) {
                if (thisv->shelfSlots[curScanTemp] != NULL) {
                    thisv->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 1;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 5;
            }
            while (curScanTemp != curTemp) {
                if (thisv->shelfSlots[curScanTemp] != NULL) {
                    thisv->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 5;
                }
            }
        }
    }
}

s32 EnOssan_HasPlayerSelectedItem(GlobalContext* globalCtx, EnOssan* thisv, Input* input) {
    EnGirlA* selectedItem = thisv->shelfSlots[thisv->cursorIndex];

    if (EnOssan_TestEndInteraction(thisv, globalCtx, input)) {
        return true;
    }
    if (Message_ShouldAdvance(globalCtx)) {
        if (selectedItem->actor.params != SI_SOLD_OUT && selectedItem->isInvisible == 0) {
            thisv->tempStateFlag = thisv->stateFlag;
            Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->itemBuyPromptTextId);
            thisv->stickLeftPrompt.isEnabled = false;
            thisv->stickRightPrompt.isEnabled = false;
            switch (selectedItem->actor.params) {
                case SI_KEATON_MASK:
                case SI_SPOOKY_MASK:
                case SI_SKULL_MASK:
                case SI_BUNNY_HOOD:
                case SI_MASK_OF_TRUTH:
                case SI_ZORA_MASK:
                case SI_GORON_MASK:
                case SI_GERUDO_MASK:
                    func_80078884(NA_SE_SY_DECIDE);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM_MASK;
                    return true;
                case SI_MILK_BOTTLE:
                    func_80078884(NA_SE_SY_DECIDE);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM_MILK_BOTTLE;
                    return true;
                case SI_WEIRD_EGG:
                    func_80078884(NA_SE_SY_DECIDE);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM_WEIRD_EGG;
                    return true;
                case SI_19:
                case SI_20:
                    func_80078884(NA_SE_SY_ERROR);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM_UNIMPLEMENTED;
                    return true;
                case SI_BOMBS_5_R25:
                case SI_BOMBS_10:
                case SI_BOMBS_20:
                case SI_BOMBS_30:
                case SI_BOMBS_5_R35:
                    func_80078884(NA_SE_SY_DECIDE);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM_BOMBS;
                    return true;
                default:
                    func_80078884(NA_SE_SY_DECIDE);
                    thisv->drawCursor = 0;
                    thisv->stateFlag = OSSAN_STATE_SELECT_ITEM;
                    return true;
            }
        }
        func_80078884(NA_SE_SY_ERROR);
        return true;
    }
    return false;
}

void EnOssan_State_BrowseLeftShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    s32 a;
    s32 b;
    u8 prevIndex = thisv->cursorIndex;
    s32 c;
    s32 d;

    if (!EnOssan_ReturnItemToShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2152);
        thisv->delayTimer = 3;
        return;
    }
    if (thisv->delayTimer != 0) {
        thisv->delayTimer--;
        return;
    }
    thisv->drawCursor = 0xFF;
    thisv->stickRightPrompt.isEnabled = true;
    EnOssan_UpdateCursorPos(globalCtx, thisv);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) &&
        !EnOssan_HasPlayerSelectedItem(globalCtx, thisv, &globalCtx->state.input[0])) {
        if (thisv->moveHorizontal) {
            if (thisv->stickAccumX > 0) {
                a = EnOssan_CursorRight(thisv, thisv->cursorIndex, 4);
                if (a != CURSOR_INVALID) {
                    thisv->cursorIndex = a;
                } else {
                    EnOssan_SetLookToShopkeeperFromShelf(globalCtx, thisv);
                    return;
                }
            } else if (thisv->stickAccumX < 0) {
                b = EnOssan_CursorLeft(thisv, thisv->cursorIndex, 8);
                if (b != CURSOR_INVALID) {
                    thisv->cursorIndex = b;
                }
            }
        } else {
            if (thisv->stickAccumX > 0 && thisv->stickAccumX > 500) {
                c = EnOssan_CursorRight(thisv, thisv->cursorIndex, 4);
                if (c != CURSOR_INVALID) {
                    thisv->cursorIndex = c;
                } else {
                    EnOssan_SetLookToShopkeeperFromShelf(globalCtx, thisv);
                    return;
                }
            } else if (thisv->stickAccumX < 0 && thisv->stickAccumX < -500) {
                d = EnOssan_CursorLeft(thisv, thisv->cursorIndex, 8);
                if (d != CURSOR_INVALID) {
                    thisv->cursorIndex = d;
                }
            }
        }
        EnOssan_CursorUpDown(thisv);
        if (thisv->cursorIndex != prevIndex) {
            Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
            func_80078884(NA_SE_SY_CURSOR);
        }
    }
}

void EnOssan_State_BrowseRightShelf(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    s32 pad[2];
    u8 prevIndex;
    u8 nextIndex;

    prevIndex = thisv->cursorIndex;
    if (!EnOssan_ReturnItemToShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2244);
        thisv->delayTimer = 3;
        return;
    }
    if (thisv->delayTimer != 0) {
        thisv->delayTimer--;
        return;
    }
    thisv->drawCursor = 0xFF;
    thisv->stickLeftPrompt.isEnabled = true;
    EnOssan_UpdateCursorPos(globalCtx, thisv);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) &&
        !EnOssan_HasPlayerSelectedItem(globalCtx, thisv, &globalCtx->state.input[0])) {
        if (thisv->moveHorizontal) {
            if (thisv->stickAccumX < 0) {
                nextIndex = EnOssan_CursorRight(thisv, thisv->cursorIndex, 0);
                if (nextIndex != CURSOR_INVALID) {
                    thisv->cursorIndex = nextIndex;
                } else {
                    EnOssan_SetLookToShopkeeperFromShelf(globalCtx, thisv);
                    return;
                }
            } else if (thisv->stickAccumX > 0) {
                nextIndex = EnOssan_CursorLeft(thisv, thisv->cursorIndex, 4);
                if (nextIndex != CURSOR_INVALID) {
                    thisv->cursorIndex = nextIndex;
                }
            }
        } else {
            if (thisv->stickAccumX < 0 && thisv->stickAccumX < -500) {
                nextIndex = EnOssan_CursorRight(thisv, thisv->cursorIndex, 0);
                if (nextIndex != CURSOR_INVALID) {
                    thisv->cursorIndex = nextIndex;
                } else {
                    EnOssan_SetLookToShopkeeperFromShelf(globalCtx, thisv);
                    return;
                }
            } else if (thisv->stickAccumX > 0 && thisv->stickAccumX > 500) {
                nextIndex = EnOssan_CursorLeft(thisv, thisv->cursorIndex, 4);
                if (nextIndex != CURSOR_INVALID) {
                    thisv->cursorIndex = nextIndex;
                }
            }
        }
        EnOssan_CursorUpDown(thisv);
        if (thisv->cursorIndex != prevIndex) {
            Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
            func_80078884(NA_SE_SY_CURSOR);
        }
    }
}

void EnOssan_State_LookFromShelfToShopkeeper(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    Math_ApproachF(&thisv->cameraFaceAngle, 0.0f, 0.5f, 10.0f);
    if ((thisv->cameraFaceAngle < 0.5f) && (thisv->cameraFaceAngle > -0.5f)) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, 0.0f);
    }
    EnOssan_UpdateCameraDirection(thisv, globalCtx, thisv->cameraFaceAngle);
    if (thisv->cameraFaceAngle == 0.0f) {
        EnOssan_StartShopping(globalCtx, thisv);
    }
}

void EnOssan_State_DisplayOnlyBombDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (!EnOssan_ReturnItemToShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2355);
        return;
    }
    Math_ApproachF(&thisv->cameraFaceAngle, 0.0f, 0.5f, 10.0f);
    if (thisv->cameraFaceAngle < 0.5f && thisv->cameraFaceAngle > -0.5f) {
        EnOssan_UpdateCameraDirection(thisv, globalCtx, 0.0f);
    }
    EnOssan_UpdateCameraDirection(thisv, globalCtx, thisv->cameraFaceAngle);
    if (thisv->cameraFaceAngle == 0.0f) {
        Message_ContinueTextbox(globalCtx, 0x3010);
        thisv->stateFlag = OSSAN_STATE_WAIT_FOR_DISPLAY_ONLY_BOMB_DIALOG;
    }
}

void EnOssan_GiveItemWithFanfare(GlobalContext* globalCtx, EnOssan* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    osSyncPrintf("\n" VT_FGCOL(YELLOW) "初めて手にいれた！！" VT_RST "\n\n");
    func_8002F434(&thisv->actor, globalCtx, thisv->shelfSlots[thisv->cursorIndex]->getItemId, 120.0f, 120.0f);
    globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    globalCtx->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~0x20000000;
    func_800BC490(globalCtx, 1);
    Interface_ChangeAlpha(50);
    thisv->drawCursor = 0;
    EnOssan_UpdateCameraDirection(thisv, globalCtx, 0.0f);
    thisv->stateFlag = OSSAN_STATE_GIVE_ITEM_FANFARE;
    osSyncPrintf(VT_FGCOL(YELLOW) "持ち上げ開始！！" VT_RST "\n\n");
}

void EnOssan_SetStateCantGetItem(GlobalContext* globalCtx, EnOssan* thisv, u16 textId) {
    Message_ContinueTextbox(globalCtx, textId);
    thisv->stateFlag = OSSAN_STATE_CANT_GET_ITEM;
}

void EnOssan_SetStateQuickBuyDialog(GlobalContext* globalCtx, EnOssan* thisv, u16 textId) {
    Message_ContinueTextbox(globalCtx, textId);
    thisv->stateFlag = OSSAN_STATE_QUICK_BUY;
}

void EnOssan_HandleCanBuyItem(GlobalContext* globalCtx, EnOssan* thisv) {
    EnGirlA* selectedItem = thisv->shelfSlots[thisv->cursorIndex];

    switch (selectedItem->canBuyFunc(globalCtx, selectedItem)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            if (selectedItem->actor.params == SI_HYLIAN_SHIELD && gSaveContext.infTable[7] & 0x40) {
                EnOssan_SetStateGiveDiscountDialog(globalCtx, thisv);
            } else {
                EnOssan_GiveItemWithFanfare(globalCtx, thisv);
                thisv->drawCursor = 0;
                thisv->shopItemSelectedTween = 0.0f;
                selectedItem->setOutOfStockFunc(globalCtx, selectedItem);
            }
            break;
        case CANBUY_RESULT_SUCCESS:
            selectedItem->itemGiveFunc(globalCtx, selectedItem);
            EnOssan_SetStateQuickBuyDialog(globalCtx, thisv, 0x84);
            thisv->drawCursor = 0;
            thisv->shopItemSelectedTween = 0.0f;
            selectedItem->setOutOfStockFunc(globalCtx, selectedItem);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x86);
            break;
        case CANBUY_RESULT_NEED_BOTTLE:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x96);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x85);
            break;
        case CANBUY_RESULT_CANT_GET_NOW_5:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x86);
            break;
    }
}

void EnOssan_HandleCanBuyLonLonMilk(GlobalContext* globalCtx, EnOssan* thisv) {
    EnGirlA* item = thisv->shelfSlots[thisv->cursorIndex];

    switch (item->canBuyFunc(globalCtx, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            Message_ContinueTextbox(globalCtx, 0x9C);
            thisv->stateFlag = OSSAN_STATE_GIVE_LON_LON_MILK;
            thisv->drawCursor = 0;
            break;
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(globalCtx, item);
            EnOssan_SetStateQuickBuyDialog(globalCtx, thisv, 0x98);
            thisv->drawCursor = 0;
            thisv->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(globalCtx, item);
            break;
        case CANBUY_RESULT_NEED_BOTTLE:
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x96);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x85);
            break;
    }
}

void EnOssan_HandleCanBuyWeirdEgg(GlobalContext* globalCtx, EnOssan* thisv) {
    EnGirlA* item = thisv->shelfSlots[thisv->cursorIndex];

    switch (item->canBuyFunc(globalCtx, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            EnOssan_GiveItemWithFanfare(globalCtx, thisv);
            thisv->drawCursor = 0;
            thisv->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(globalCtx, item);
            break;
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(globalCtx, item);
            EnOssan_SetStateQuickBuyDialog(globalCtx, thisv, 0x9A);
            thisv->drawCursor = 0;
            thisv->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(globalCtx, item);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x9D);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x85);
            break;
    }
}

void EnOssan_HandleCanBuyBombs(GlobalContext* globalCtx, EnOssan* thisv) {
    EnGirlA* item = thisv->shelfSlots[thisv->cursorIndex];

    switch (item->canBuyFunc(globalCtx, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(globalCtx, item);
            EnOssan_SetStateQuickBuyDialog(globalCtx, thisv, 0x84);
            thisv->drawCursor = 0;
            thisv->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(globalCtx, item);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x86);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            func_80078884(NA_SE_SY_ERROR);
            EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x85);
            break;
    }
}

void EnOssan_BuyGoronCityBombs(GlobalContext* globalCtx, EnOssan* thisv) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        if (!(gSaveContext.eventChkInf[2] & 0x20)) {
            if (gSaveContext.infTable[15] & 0x1000) {
                EnOssan_SetStateCantGetItem(globalCtx, thisv, 0x302E);
            } else {
                thisv->stickLeftPrompt.isEnabled = false;
                thisv->stickRightPrompt.isEnabled = false;
                thisv->drawCursor = 0;
                thisv->stateFlag = OSSAN_STATE_DISPLAY_ONLY_BOMB_DIALOG;
            }
        } else {
            EnOssan_HandleCanBuyBombs(globalCtx, thisv);
        }
    } else {
        EnOssan_HandleCanBuyBombs(globalCtx, thisv);
    }
}

void EnOssan_State_ItemSelected(EnOssan* thisv, GlobalContext* globalCtx2, Player* player) {
    GlobalContext* globalCtx = globalCtx2; // Necessary for OKs

    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2654);
        return;
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE &&
        !EnOssan_TestCancelOption(thisv, globalCtx, &globalCtx->state.input[0]) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                EnOssan_HandleCanBuyItem(globalCtx, thisv);
                break;
            case 1:
                thisv->stateFlag = thisv->tempStateFlag;
                Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
                break;
        }
    }
}

void EnOssan_State_SelectMilkBottle(EnOssan* thisv, GlobalContext* globalCtx2, Player* player) {
    GlobalContext* globalCtx = globalCtx2; // Need for OK

    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2693);
        return;
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE &&
        !EnOssan_TestCancelOption(thisv, globalCtx, &globalCtx->state.input[0]) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                EnOssan_HandleCanBuyLonLonMilk(globalCtx, thisv);
                break;
            case 1:
                thisv->stateFlag = thisv->tempStateFlag;
                Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
                break;
        }
    }
}

void EnOssan_State_SelectWeirdEgg(EnOssan* thisv, GlobalContext* globalCtx2, Player* player) {
    GlobalContext* globalCtx = globalCtx2; // Needed for OK

    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2732);
        return;
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE &&
        !EnOssan_TestCancelOption(thisv, globalCtx, &globalCtx->state.input[0]) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                EnOssan_HandleCanBuyWeirdEgg(globalCtx, thisv);
                break;
            case 1:
                thisv->stateFlag = thisv->tempStateFlag;
                Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
                break;
        }
    }
}

void EnOssan_State_SelectUnimplementedItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2771);
        return;
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        thisv->stateFlag = thisv->tempStateFlag;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
    }
}

void EnOssan_State_SelectBombs(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2798);
        return;
    }
    osSyncPrintf("店主の依頼 ( %d )\n", gSaveContext.infTable[15] & 0x1000);
    if (thisv->actor.params != OSSAN_TYPE_GORON) {
        EnOssan_State_ItemSelected(thisv, globalCtx, player);
        return;
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE &&
        !EnOssan_TestCancelOption(thisv, globalCtx, &globalCtx->state.input[0]) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                EnOssan_BuyGoronCityBombs(globalCtx, thisv);
                break;
            case 1:
                thisv->stateFlag = thisv->tempStateFlag;
                Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
                break;
        }
    }
}

void EnOssan_State_SelectMaskItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);
    EnGirlA* item = thisv->shelfSlots[thisv->cursorIndex];

    if (!EnOssan_TakeItemOffShelf(thisv)) {
        osSyncPrintf("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2845);
        return;
    }
    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(globalCtx)) {
            thisv->stateFlag = thisv->tempStateFlag;
            Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
        }
    } else if (talkState == TEXT_STATE_CHOICE &&
               !EnOssan_TestCancelOption(thisv, globalCtx, &globalCtx->state.input[0]) &&
               Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                switch (item->actor.params) {
                    case SI_KEATON_MASK:
                        gSaveContext.itemGetInf[2] |= 0x08;
                        break;
                    case SI_SPOOKY_MASK:
                        gSaveContext.itemGetInf[2] |= 0x20;
                        break;
                    case SI_SKULL_MASK:
                        gSaveContext.itemGetInf[2] |= 0x10;
                        break;
                    case SI_BUNNY_HOOD:
                        gSaveContext.itemGetInf[2] |= 0x40;
                        break;
                    case SI_MASK_OF_TRUTH:
                    case SI_ZORA_MASK:
                    case SI_GORON_MASK:
                    case SI_GERUDO_MASK:
                        break;
                }
                EnOssan_GiveItemWithFanfare(globalCtx, thisv);
                thisv->drawCursor = 0;
                thisv->shopItemSelectedTween = 0.0f;
                item->setOutOfStockFunc(globalCtx, item);
                break;
            case 1:
                thisv->stateFlag = thisv->tempStateFlag;
                Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
                break;
        }
    }
}

void EnOssan_State_CantGetItem(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        thisv->stateFlag = thisv->tempStateFlag;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
    }
}

void EnOssan_State_QuickBuyDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    EnGirlA* item;

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        thisv->shopItemSelectedTween = 0.0f;
        EnOssan_ResetItemPosition(thisv);
        item = thisv->shelfSlots[thisv->cursorIndex];
        item->updateStockedItemFunc(globalCtx, item);
        thisv->stateFlag = thisv->tempStateFlag;
        Message_ContinueTextbox(globalCtx, thisv->shelfSlots[thisv->cursorIndex]->actor.textId);
    }
}

void EnOssan_State_GiveItemWithFanfare(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    // The player sets itself as the parent actor to signal that it has obtained the give item request
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->stateFlag = OSSAN_STATE_ITEM_PURCHASED;
        return;
    }
    func_8002F434(&thisv->actor, globalCtx, thisv->shelfSlots[thisv->cursorIndex]->getItemId, 120.0f, 120.0f);
}

void EnOssan_State_ItemPurchased(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    EnGirlA* item;
    EnGirlA* itemTemp;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        if (thisv->actor.params == OSSAN_TYPE_MASK) {
            itemTemp = thisv->shelfSlots[thisv->cursorIndex];
            EnOssan_ResetItemPosition(thisv);
            item = thisv->shelfSlots[thisv->cursorIndex];
            item->updateStockedItemFunc(globalCtx, item);
            if (itemTemp->actor.params == SI_MASK_OF_TRUTH && !(gSaveContext.itemGetInf[3] & 0x8000)) {
                gSaveContext.itemGetInf[3] |= 0x8000;
                Message_ContinueTextbox(globalCtx, 0x70AB);
                thisv->happyMaskShopState = OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK;
                EnOssan_UpdateShopOfferings(thisv, globalCtx);
                thisv->stateFlag = OSSAN_STATE_START_CONVERSATION;
                return;
            } else {
                EnOssan_EndInteraction(globalCtx, thisv);
                return;
            }
        }
        item = thisv->shelfSlots[thisv->cursorIndex];
        item->buyEventFunc(globalCtx, item);
        thisv->stateFlag = OSSAN_STATE_CONTINUE_SHOPPING_PROMPT;
        Message_ContinueTextbox(globalCtx, 0x6B);
    }
}

void EnOssan_State_ContinueShoppingPrompt(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    EnGirlA* selectedItem;
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if (talkState == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(globalCtx)) {
            EnOssan_ResetItemPosition(thisv);
            selectedItem = thisv->shelfSlots[thisv->cursorIndex];
            selectedItem->updateStockedItemFunc(globalCtx, selectedItem);
            if (!EnOssan_TestEndInteraction(thisv, globalCtx, &globalCtx->state.input[0])) {
                switch (globalCtx->msgCtx.choiceIndex) {
                    case 0:
                        osSyncPrintf(VT_FGCOL(YELLOW) "★★★ 続けるよ！！ ★★★" VT_RST "\n");
                        player->actor.shape.rot.y += 0x8000;
                        player->stateFlags2 |= 0x20000000;
                        func_800BC490(globalCtx, 2);
                        Message_StartTextbox(globalCtx, thisv->actor.textId, &thisv->actor);
                        EnOssan_SetStateStartShopping(globalCtx, thisv, true);
                        func_8002F298(&thisv->actor, globalCtx, 100.0f, -1);
                        break;
                    case 1:
                    default:
                        osSyncPrintf(VT_FGCOL(YELLOW) "★★★ やめるよ！！ ★★★" VT_RST "\n");
                        EnOssan_EndInteraction(globalCtx, thisv);
                        break;
                }
            }
        }
    } else if (talkState == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        EnOssan_ResetItemPosition(thisv);
        selectedItem = thisv->shelfSlots[thisv->cursorIndex];
        selectedItem->updateStockedItemFunc(globalCtx, selectedItem);
        player->actor.shape.rot.y += 0x8000;
        player->stateFlags2 |= 0x20000000;
        func_800BC490(globalCtx, 2);
        Message_StartTextbox(globalCtx, thisv->actor.textId, &thisv->actor);
        EnOssan_SetStateStartShopping(globalCtx, thisv, true);
        func_8002F298(&thisv->actor, globalCtx, 100.0f, -1);
    }
}

void EnOssan_State_WaitForDisplayOnlyBombDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        gSaveContext.infTable[15] |= 0x1000;
        EnOssan_StartShopping(globalCtx, thisv);
    }
}

// Unreachable
void EnOssan_State_21(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE_HAS_NEXT && Message_ShouldAdvance(globalCtx)) {
        thisv->stateFlag = OSSAN_STATE_22;
        Message_ContinueTextbox(globalCtx, 0x3012);
        gSaveContext.infTable[15] |= 0x1000;
    }
}

// Unreachable
void EnOssan_State_22(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        EnOssan_StartShopping(globalCtx, thisv);
    }
}

void EnOssan_State_GiveLonLonMilk(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        EnOssan_GiveItemWithFanfare(globalCtx, thisv);
    }
}

// For giving Mask of Truth when you first sell all masks
void EnOssan_State_LendMaskOfTruth(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT && Message_ShouldAdvance(globalCtx)) {
        gSaveContext.itemGetInf[2] |= 0x400;
        thisv->cursorIndex = 2;
        EnOssan_GiveItemWithFanfare(globalCtx, thisv);
    }
}

// Hylian Shield discount dialog
void EnOssan_SetStateGiveDiscountDialog(GlobalContext* globalCtx, EnOssan* thisv) {
    Message_ContinueTextbox(globalCtx, 0x71B2);
    thisv->stateFlag = OSSAN_STATE_DISCOUNT_DIALOG;
}

void EnOssan_State_GiveDiscountDialog(EnOssan* thisv, GlobalContext* globalCtx, Player* player) {
    EnGirlA* selectedItem;

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        selectedItem = thisv->shelfSlots[thisv->cursorIndex];
        EnOssan_GiveItemWithFanfare(globalCtx, thisv);
        thisv->drawCursor = 0;
        thisv->shopItemSelectedTween = 0.0f;
        selectedItem->setOutOfStockFunc(globalCtx, selectedItem);
    }
}

void EnOssan_PositionSelectedItem(EnOssan* thisv) {
    EnGirlA* item;
    u8 i;
    u8 i2;
    ShopItem* shopItem;
    f32 tx;
    f32 ty;
    f32 tz;

    i = thisv->cursorIndex;
    shopItem = &sShopkeeperStores[thisv->actor.params][i];
    item = thisv->shelfSlots[i];

    i2 = i >> 2;
    tx = (sSelectedItemPosition[i2].x - shopItem->xOffset) * thisv->shopItemSelectedTween + shopItem->xOffset;
    ty = (sSelectedItemPosition[i2].y - shopItem->yOffset) * thisv->shopItemSelectedTween + shopItem->yOffset;
    tz = (sSelectedItemPosition[i2].z - shopItem->zOffset) * thisv->shopItemSelectedTween + shopItem->zOffset;

    item->actor.world.pos.x = thisv->shelves->actor.world.pos.x + tx;
    item->actor.world.pos.y = thisv->shelves->actor.world.pos.y + ty;
    item->actor.world.pos.z = thisv->shelves->actor.world.pos.z + tz;
}

void EnOssan_ResetItemPosition(EnOssan* thisv) {
    thisv->shopItemSelectedTween = 0.0f;
    EnOssan_PositionSelectedItem(thisv);
}

// returns true if animation has completed
s32 EnOssan_TakeItemOffShelf(EnOssan* thisv) {
    Math_ApproachF(&thisv->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (thisv->shopItemSelectedTween >= 0.85f) {
        thisv->shopItemSelectedTween = 1.0f;
    }
    EnOssan_PositionSelectedItem(thisv);
    if (thisv->shopItemSelectedTween == 1.0f) {
        return true;
    } else {
        return false;
    }
}

// returns true if animation has completed
s32 EnOssan_ReturnItemToShelf(EnOssan* thisv) {
    Math_ApproachF(&thisv->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (thisv->shopItemSelectedTween <= 0.15f) {
        thisv->shopItemSelectedTween = 0.0f;
    }
    EnOssan_PositionSelectedItem(thisv);
    if (thisv->shopItemSelectedTween == 0.0f) {
        return true;
    } else {
        return false;
    }
}

void EnOssan_UpdateItemSelectedProperty(EnOssan* thisv) {
    EnGirlA** temp_a1 = thisv->shelfSlots;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (temp_a1[0] != NULL) {
            if (thisv->stateFlag != OSSAN_STATE_SELECT_ITEM && thisv->stateFlag != OSSAN_STATE_SELECT_ITEM_MILK_BOTTLE &&
                thisv->stateFlag != OSSAN_STATE_SELECT_ITEM_WEIRD_EGG &&
                thisv->stateFlag != OSSAN_STATE_SELECT_ITEM_UNIMPLEMENTED &&
                thisv->stateFlag != OSSAN_STATE_SELECT_ITEM_BOMBS && thisv->stateFlag != OSSAN_STATE_SELECT_ITEM_MASK &&
                thisv->stateFlag != OSSAN_STATE_CANT_GET_ITEM && thisv->drawCursor == 0) {
                temp_a1[0]->isSelected = false;
            } else {
                if (thisv->cursorIndex == i) {
                    temp_a1[0]->isSelected = true;
                } else {
                    temp_a1[0]->isSelected = false;
                }
            }
        }
        temp_a1++;
    }
}

void EnOssan_UpdateCursorAnim(EnOssan* thisv) {
    f32 t;

    t = thisv->cursorAnimTween;
    if (thisv->cursorAnimState == 0) {
        t += 0.05f;
        if (t >= 1.0f) {
            t = 1.0f;
            thisv->cursorAnimState = 1;
        }
    } else {
        t -= 0.05f;
        if (t <= 0.0f) {
            t = 0.0f;
            thisv->cursorAnimState = 0;
        }
    }
    thisv->cursorColorR = ColChanMix(0, 0.0f, t);
    thisv->cursorColorG = ColChanMix(255, 80.0f, t);
    thisv->cursorColorB = ColChanMix(80, 0.0f, t);
    thisv->cursorColorA = ColChanMix(255, 0.0f, t);
    thisv->cursorAnimTween = t;
}

void EnOssan_UpdateStickDirectionPromptAnim(EnOssan* thisv) {
    f32 arrowAnimTween;
    f32 new_var3;       // likely fake temp
    s32 new_var2 = 255; // likely fake temp
    f32 stickAnimTween;

    arrowAnimTween = thisv->arrowAnimTween;
    stickAnimTween = thisv->stickAnimTween;
    if (thisv->arrowAnimState == 0) {
        arrowAnimTween += 0.05f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            thisv->arrowAnimState = 1;
        }

    } else {
        arrowAnimTween -= 0.05f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            thisv->arrowAnimState = 0;
        }
    }

    thisv->arrowAnimTween = arrowAnimTween;
    if (thisv->stickAnimState == 0) {
        stickAnimTween += 0.1f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            thisv->stickAnimState = 1;
        }

    } else {
        stickAnimTween = 0.0f;
        thisv->stickAnimState = 0;
    }

    thisv->stickAnimTween = stickAnimTween;
    thisv->stickLeftPrompt.arrowColorR = (u8)(255 - ((s32)(155.0f * arrowAnimTween)));
    thisv->stickLeftPrompt.arrowColorG = (u8)(new_var2 - (s32)(155.0f * arrowAnimTween));
    new_var3 = (155.0f * arrowAnimTween);
    thisv->stickLeftPrompt.arrowColorB = (u8)(0 - ((s32)((-100.0f) * arrowAnimTween)));
    thisv->stickLeftPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    thisv->stickRightPrompt.arrowColorR = (u8)(new_var2 - (s32)new_var3);
    thisv->stickRightPrompt.arrowColorG = (u8)(255 - (s32)new_var3);
    thisv->stickRightPrompt.arrowColorB = (u8)(0 - ((s32)((-100.0f) * arrowAnimTween)));
    thisv->stickRightPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    thisv->stickRightPrompt.arrowTexX = 290.0f;
    thisv->stickLeftPrompt.arrowTexX = 33.0f;
    thisv->stickRightPrompt.stickTexX = 274.0f;
    thisv->stickLeftPrompt.stickTexX = 49.0f;
    thisv->stickRightPrompt.stickTexX += (8.0f * stickAnimTween);
    thisv->stickLeftPrompt.stickTexX -= (8.0f * stickAnimTween);
    thisv->stickLeftPrompt.arrowTexY = thisv->stickRightPrompt.arrowTexY = 91.0f;
    thisv->stickLeftPrompt.stickTexY = thisv->stickRightPrompt.stickTexY = 95.0f;
}

void EnOssan_WaitForBlink(EnOssan* thisv) {
    s16 decr = thisv->blinkTimer - 1;

    if (decr != 0) {
        thisv->blinkTimer = decr;
    } else {
        thisv->blinkFunc = EnOssan_Blink;
    }
}

void EnOssan_Blink(EnOssan* thisv) {
    s16 decr;
    s16 eyeTextureIdxTemp;

    decr = thisv->blinkTimer - 1;
    if (decr != 0) {
        thisv->blinkTimer = decr;
        return;
    }
    eyeTextureIdxTemp = thisv->eyeTextureIdx + 1;
    if (eyeTextureIdxTemp > 2) {
        thisv->eyeTextureIdx = 0;
        thisv->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
        thisv->blinkFunc = EnOssan_WaitForBlink;
    } else {
        thisv->eyeTextureIdx = eyeTextureIdxTemp;
        thisv->blinkTimer = 1;
    }
}

s32 EnOssan_AreShopkeeperObjectsLoaded(EnOssan* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex1)) {
        if (thisv->objBankIndex2 >= 0 && !Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex2)) {
            return false;
        }
        if (thisv->objBankIndex3 >= 0 && !Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex3)) {
            return false;
        }
        return true;
    }
    return false;
}

void EnOssan_InitBazaarShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gObjectOssanSkel, &gObjectOssanAnim_000338, NULL, NULL, 0);
    thisv->actor.draw = EnOssan_DrawBazaarShopkeeper;
    thisv->obj3ToSeg6Func = NULL;
}

void EnOssan_InitKokiriShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gKm1Skel, NULL, NULL, NULL, 0);
    gSegments[6] = reinterpret_cast<std::uintptr_t>(PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objBankIndex3].segment));
    Animation_Change(&thisv->skelAnime, &object_masterkokiri_Anim_0004A8, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_masterkokiri_Anim_0004A8), 0, 0.0f);
    thisv->actor.draw = EnOssan_DrawKokiriShopkeeper;
    thisv->obj3ToSeg6Func = EnOssan_Obj3ToSeg6;
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_ELF, thisv->actor.world.pos.x,
                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);
}

void EnOssan_InitGoronShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGoronSkel, NULL, NULL, NULL, 0);
    gSegments[6] = reinterpret_cast<std::uintptr_t>(PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objBankIndex3].segment));
    Animation_Change(&thisv->skelAnime, &gGoronShopkeeperAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gGoronShopkeeperAnim),
                     0, 0.0f);
    thisv->actor.draw = EnOssan_DrawGoronShopkeeper;
    thisv->obj3ToSeg6Func = EnOssan_Obj3ToSeg6;
}

void EnOssan_InitZoraShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gZoraSkel, NULL, NULL, NULL, 0);
    gSegments[6] = reinterpret_cast<std::uintptr_t>(PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objBankIndex3].segment));
    Animation_Change(&thisv->skelAnime, &gZoraShopkeeperAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gZoraShopkeeperAnim),
                     0, 0.0f);
    thisv->actor.draw = EnOssan_DrawZoraShopkeeper;
    thisv->obj3ToSeg6Func = EnOssan_Obj3ToSeg6;
}

void EnOssan_InitPotionShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ds2_Skel_004258, &object_ds2_Anim_0002E4, 0, 0, 0);
    thisv->actor.draw = EnOssan_DrawPotionShopkeeper;
    thisv->obj3ToSeg6Func = NULL;
}

void EnOssan_InitHappyMaskShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_os_Skel_004658, &object_os_Anim_0002E4, NULL, NULL, 0);
    thisv->actor.draw = EnOssan_DrawHappyMaskShopkeeper;
    thisv->obj3ToSeg6Func = NULL;
}

void EnOssan_InitBombchuShopkeeper(EnOssan* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_rs_Skel_004868, &object_rs_Anim_00065C, 0, 0, 0);
    thisv->actor.draw = EnOssan_DrawBombchuShopkeeper;
    thisv->obj3ToSeg6Func = NULL;
}

u16 EnOssan_SetupHelloDialog(EnOssan* thisv) {
    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_NONE;
    // mask shop messages
    if (thisv->actor.params == OSSAN_TYPE_MASK) {
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_SOLD_OUT) {
            if (gSaveContext.itemGetInf[3] & 0x800) {
                if (!(gSaveContext.eventChkInf[8] & 0x8000)) {
                    // Pay back Bunny Hood
                    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD;
                    return 0x70C6;
                } else {
                    return 0x70AC;
                }
            }
            if (gSaveContext.itemGetInf[3] & 0x400) {
                if (!(gSaveContext.eventChkInf[8] & 0x4000)) {
                    // Pay back Spooky Mask
                    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK;
                    return 0x70C5;
                } else {
                    return 0x70AC;
                }
            }
            if (gSaveContext.itemGetInf[3] & 0x200) {
                if (!(gSaveContext.eventChkInf[8] & 0x2000)) {
                    // Pay back Skull Mask
                    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK;
                    return 0x70C4;
                } else {
                    return 0x70AC;
                }
            }
            if (gSaveContext.itemGetInf[3] & 0x100) {
                if (!(gSaveContext.eventChkInf[8] & 0x1000)) {
                    // Pay back Keaton Mask
                    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK;
                    return 0x70A5;
                } else {
                    return 0x70AC;
                }
            }
        } else {
            if (gSaveContext.itemGetInf[3] & 0x800) {
                return 0x70AC;
            } else if (!(gSaveContext.itemGetInf[3] & 0x400) && !(gSaveContext.itemGetInf[2] & 0x10) &&
                       !(gSaveContext.itemGetInf[3] & 0x100)) {
                // Haven't borrowed the Keaton Mask
                if (!(gSaveContext.itemGetInf[2] & 0x8)) {
                    return 0x70A1;
                } else {
                    // Haven't sold the Keaton Mask
                    thisv->happyMaskShopState = OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK;
                    return 0x70A6;
                }
            } else {
                return 0x70C7;
            }
        }
    }

    return 0x9E;
}

void EnOssan_InitActionFunc(EnOssan* thisv, GlobalContext* globalCtx) {
    ShopItem* items;

    if (EnOssan_AreShopkeeperObjectsLoaded(thisv, globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        thisv->actor.objBankIndex = thisv->objBankIndex1;
        Actor_SetObjectDependency(globalCtx, &thisv->actor);

        thisv->shelves = (EnTana*)Actor_Find(&globalCtx->actorCtx, ACTOR_EN_TANA, ACTORCAT_PROP);

        if (thisv->shelves == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "Warning!! There are no shelves!!"
            osSyncPrintf("★★★ 警告！！ 棚がないよ！！ ★★★\n");
            osSyncPrintf(VT_RST);
            return;
        }

        // "Shopkeeper (params) init"
        osSyncPrintf(VT_FGCOL(YELLOW) "◇◇◇ 店のおやじ( %d ) 初期設定 ◇◇◇" VT_RST "\n", thisv->actor.params);

        thisv->actor.world.pos.x += sShopkeeperPositionOffsets[thisv->actor.params].x;
        thisv->actor.world.pos.y += sShopkeeperPositionOffsets[thisv->actor.params].y;
        thisv->actor.world.pos.z += sShopkeeperPositionOffsets[thisv->actor.params].z;

        items = sShopkeeperStores[thisv->actor.params];

        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
        sInitFuncs[thisv->actor.params](thisv, globalCtx);
        thisv->actor.textId = EnOssan_SetupHelloDialog(thisv);
        thisv->cursorY = thisv->cursorX = 100.0f;
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        thisv->actor.colChkInfo.cylRadius = 50;
        thisv->stateFlag = OSSAN_STATE_IDLE;
        thisv->stickAccumX = thisv->stickAccumY = 0;

        thisv->cursorIndex = 0;
        thisv->cursorZ = 1.5f;
        thisv->cursorColorR = 0;
        thisv->cursorColorG = 255;
        thisv->cursorColorB = 80;
        thisv->cursorColorA = 255;
        thisv->cursorAnimTween = 0;

        thisv->cursorAnimState = 0;
        thisv->drawCursor = 0;
        thisv->happyMaskShopkeeperEyeIdx = 0;

        thisv->stickLeftPrompt.stickColorR = 200;
        thisv->stickLeftPrompt.stickColorG = 200;
        thisv->stickLeftPrompt.stickColorB = 200;
        thisv->stickLeftPrompt.stickColorA = 180;
        thisv->stickLeftPrompt.stickTexX = 49;
        thisv->stickLeftPrompt.stickTexY = 95;
        thisv->stickLeftPrompt.arrowColorR = 255;
        thisv->stickLeftPrompt.arrowColorG = 255;
        thisv->stickLeftPrompt.arrowColorB = 0;
        thisv->stickLeftPrompt.arrowColorA = 200;
        thisv->stickLeftPrompt.arrowTexX = 33;
        thisv->stickLeftPrompt.arrowTexY = 91;
        thisv->stickLeftPrompt.z = 1;
        thisv->stickLeftPrompt.isEnabled = false;

        thisv->stickRightPrompt.stickColorR = 200;
        thisv->stickRightPrompt.stickColorG = 200;
        thisv->stickRightPrompt.stickColorB = 200;
        thisv->stickRightPrompt.stickColorA = 180;
        thisv->stickRightPrompt.stickTexX = 274;
        thisv->stickRightPrompt.stickTexY = 95;
        thisv->stickRightPrompt.arrowColorR = 255;
        thisv->stickRightPrompt.arrowColorG = 255;
        thisv->stickRightPrompt.arrowColorB = 0;
        thisv->stickRightPrompt.arrowColorA = 200;
        thisv->stickRightPrompt.arrowTexX = 290;
        thisv->stickRightPrompt.arrowTexY = 91;
        thisv->stickRightPrompt.z = 1;
        thisv->stickRightPrompt.isEnabled = false;

        thisv->arrowAnimState = 0;
        thisv->stickAnimState = 0;
        thisv->arrowAnimTween = 0;
        thisv->stickAnimTween = 0;
        thisv->shopItemSelectedTween = 0;
        Actor_SetScale(&thisv->actor, sShopkeeperScale[thisv->actor.params]);
        EnOssan_SpawnItemsOnShelves(thisv, globalCtx, items);
        thisv->headRot = thisv->headTargetRot = 0;
        thisv->blinkTimer = 20;
        thisv->eyeTextureIdx = 0;
        thisv->blinkFunc = EnOssan_WaitForBlink;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        EnOssan_SetupAction(thisv, EnOssan_MainActionFunc);
    }
}

void EnOssan_Obj3ToSeg6(EnOssan* thisv, GlobalContext* globalCtx) {
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->objBankIndex3].segment);
}

void EnOssan_MainActionFunc(EnOssan* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->blinkFunc(thisv);
    EnOssan_UpdateJoystickInputState(globalCtx, thisv);
    EnOssan_UpdateItemSelectedProperty(thisv);
    EnOssan_UpdateStickDirectionPromptAnim(thisv);
    EnOssan_UpdateCursorAnim(thisv);
    Math_StepToS(&thisv->headRot, thisv->headTargetRot, 0x190);

    if (player != NULL) {
        sStateFunc[thisv->stateFlag](thisv, globalCtx, player);
    }

    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 26.0f, 10.0f, 0.0f, 5);
    Actor_SetFocus(&thisv->actor, 90.0f);
    Actor_SetScale(&thisv->actor, sShopkeeperScale[thisv->actor.params]);

    // use animation object if needed
    if (thisv->obj3ToSeg6Func != NULL) {
        thisv->obj3ToSeg6Func(thisv, globalCtx);
    }

    SkelAnime_Update(&thisv->skelAnime);
}

void EnOssan_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnOssan* thisv = (EnOssan*)thisx;

    thisv->timer++;
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnOssan_OverrideLimbDrawDefaultShopkeeper(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos,
                                              Vec3s* rot, void* thisx) {
    EnOssan* thisv = (EnOssan*)thisx;

    if (limbIndex == 8) {
        rot->x += thisv->headRot;
    }
    return 0;
}

void EnOssan_DrawCursor(GlobalContext* globalCtx, EnOssan* thisv, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx, uly, lrx, lry;
    f32 w;
    s32 dsdx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4192);
    if (drawCursor != 0) {
        func_80094520(globalCtx->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, thisv->cursorColorR, thisv->cursorColorG, thisv->cursorColorB,
                        thisv->cursorColorA);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, gSelectionCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4215);
}

void EnOssan_DrawTextRec(GlobalContext* globalCtx, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t,
                         f32 dx, f32 dy) {
    f32 unk;
    s32 ulx, uly, lrx, lry;
    f32 w, h;
    s32 dsdx, dtdy;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4228);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    h = 12.0f * z;
    unk = (1.0f / z) * 1024;
    dsdx = unk * dx;
    dtdy = dy * unk;

    ulx = (x - w) * 4.0f;
    uly = (y - h) * 4.0f;
    lrx = (x + w) * 4.0f;
    lry = (y + h) * 4.0f;
    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4242);
}

void EnOssan_DrawStickDirectionPrompts(GlobalContext* globalCtx, EnOssan* thisv) {
    s32 drawStickLeftPrompt = thisv->stickLeftPrompt.isEnabled;
    s32 drawStickRightPrompt = thisv->stickRightPrompt.isEnabled;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4252);
    if (drawStickLeftPrompt || drawStickRightPrompt) {
        func_80094520(globalCtx->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickLeftPrompt) {
            EnOssan_DrawTextRec(globalCtx, thisv->stickLeftPrompt.arrowColorR, thisv->stickLeftPrompt.arrowColorG,
                                thisv->stickLeftPrompt.arrowColorB, thisv->stickLeftPrompt.arrowColorA,
                                thisv->stickLeftPrompt.arrowTexX, thisv->stickLeftPrompt.arrowTexY,
                                thisv->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickRightPrompt) {
            EnOssan_DrawTextRec(globalCtx, thisv->stickRightPrompt.arrowColorR, thisv->stickRightPrompt.arrowColorG,
                                thisv->stickRightPrompt.arrowColorB, thisv->stickRightPrompt.arrowColorA,
                                thisv->stickRightPrompt.arrowTexX, thisv->stickRightPrompt.arrowTexY,
                                thisv->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickLeftPrompt) {
            EnOssan_DrawTextRec(globalCtx, thisv->stickLeftPrompt.stickColorR, thisv->stickLeftPrompt.stickColorG,
                                thisv->stickLeftPrompt.stickColorB, thisv->stickLeftPrompt.stickColorA,
                                thisv->stickLeftPrompt.stickTexX, thisv->stickLeftPrompt.stickTexY,
                                thisv->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickRightPrompt) {
            EnOssan_DrawTextRec(globalCtx, thisv->stickRightPrompt.stickColorR, thisv->stickRightPrompt.stickColorG,
                                thisv->stickRightPrompt.stickColorB, thisv->stickRightPrompt.stickColorA,
                                thisv->stickRightPrompt.stickTexX, thisv->stickRightPrompt.stickTexY,
                                thisv->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4300);
}

void EnOssan_DrawBazaarShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sBazaarShopkeeperEyeTextures[] = { gOssanEyeOpenTex, gOssanEyeHalfTex, gOssanEyeClosedTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4320);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sBazaarShopkeeperEyeTextures[thisv->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnOssan_OverrideLimbDrawDefaultShopkeeper, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4340);
}

s32 EnOssan_OverrideLimbDrawKokiriShopkeeper(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos,
                                             Vec3s* rot, void* thisx) {
    static const void* sKokiriShopkeeperEyeTextures[] = {
        gKokiriShopkeeperEyeDefaultTex,
        gKokiriShopkeeperEyeHalfTex,
        gKokiriShopkeeperEyeOpenTex,
    };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4354);

    if (limbIndex == 15) {
        gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->objBankIndex2].segment);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->objBankIndex2].segment);
        *dList = gKokiriShopkeeperHeadDL;
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sKokiriShopkeeperEyeTextures[thisv->eyeTextureIdx]));
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4374);

    return 0;
}

Gfx* EnOssan_EndDList(GraphicsContext* gfxCtx) {
    Gfx* disp = static_cast<Gfx*>(Graph_Alloc(gfxCtx, sizeof(Gfx)));

    gSPEndDisplayList(disp);
    return disp;
}

Gfx* EnOssan_SetEnvColor(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* disp = static_cast<Gfx*>(Graph_Alloc(gfxCtx, sizeof(Gfx) * 2));

    gDPSetEnvColor(disp, r, g, b, a);
    gSPEndDisplayList(disp + 1);
    return disp;
}

void EnOssan_DrawKokiriShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4409);

    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, EnOssan_SetEnvColor(globalCtx->state.gfxCtx, 0, 130, 70, 255));
    gSPSegment(POLY_OPA_DISP++, 0x09, EnOssan_SetEnvColor(globalCtx->state.gfxCtx, 110, 170, 20, 255));
    gSPSegment(POLY_OPA_DISP++, 0x0C, EnOssan_EndDList(globalCtx->state.gfxCtx));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnOssan_OverrideLimbDrawKokiriShopkeeper, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4434);
}

void EnOssan_DrawGoronShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sGoronShopkeeperEyeTextures[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4455);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sGoronShopkeeperEyeTextures[thisv->eyeTextureIdx]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4476);
}

s32 EnOssan_OverrideLimbDrawZoraShopkeeper(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           void* thisx) {
    EnOssan* thisv = (EnOssan*)thisx;

    if (limbIndex == 15) {
        rot->x += thisv->headRot;
    }
    return 0;
}

void EnOssan_DrawZoraShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sZoraShopkeeperEyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4506);

    func_80093D18(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, EnOssan_EndDList(globalCtx->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sZoraShopkeeperEyeTextures[thisv->eyeTextureIdx]));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnOssan_OverrideLimbDrawZoraShopkeeper, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4531);
}

void EnOssan_DrawPotionShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sPotionShopkeeperEyeTextures[] = { gPotionShopkeeperEyeOpenTex, gPotionShopkeeperEyeHalfTex,
                                                    gPotionShopkeeperEyeClosedTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4544);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sPotionShopkeeperEyeTextures[thisv->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4564);
}

void EnOssan_DrawHappyMaskShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sHappyMaskShopkeeperEyeTextures[] = { gOsEyeClosedTex, gOsEyeOpenTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4578);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(sHappyMaskShopkeeperEyeTextures[thisv->happyMaskShopkeeperEyeIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4598);
}

void EnOssan_DrawBombchuShopkeeper(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sBombchuShopkeeperEyeTextures[] = { gBombchuShopkeeperEyeOpenTex, gBombchuShopkeeperEyeHalfTex,
                                                     gBombchuShopkeeperEyeClosedTex };
    EnOssan* thisv = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4611);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sBombchuShopkeeperEyeTextures[thisv->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
    EnOssan_DrawCursor(globalCtx, thisv, thisv->cursorX, thisv->cursorY, thisv->cursorZ, thisv->drawCursor);
    EnOssan_DrawStickDirectionPrompts(globalCtx, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_oB1.c", 4631);
}
