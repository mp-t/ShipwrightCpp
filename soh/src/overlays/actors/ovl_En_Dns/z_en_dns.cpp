/*
 * File: z_en_dns.c
 * Overlay: En_Dns
 * Description: Deku Salesman
 */

#include "z_en_dns.h"
#include "objects/object_shopnuts/object_shopnuts.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnDns_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDns_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDns_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDns_Draw(Actor* thisx, GlobalContext* globalCtx);

u32 func_809EF5A4(EnDns* thisv);
u32 func_809EF658(EnDns* thisv);
u32 func_809EF70C(EnDns* thisv);
u32 func_809EF73C(EnDns* thisv);
u32 func_809EF800(EnDns* thisv);
u32 func_809EF854(EnDns* thisv);
u32 func_809EF8F4(EnDns* thisv);
u32 func_809EF9A4(EnDns* thisv);

void func_809EF9F8(EnDns* thisv);
void func_809EFA28(EnDns* thisv);
void func_809EFA58(EnDns* thisv);
void func_809EFA9C(EnDns* thisv);
void func_809EFACC(EnDns* thisv);
void func_809EFAFC(EnDns* thisv);
void func_809EFB40(EnDns* thisv);

void EnDns_SetupWait(EnDns* thisv, GlobalContext* globalCtx);
void EnDns_Wait(EnDns* thisv, GlobalContext* globalCtx);
void EnDns_Talk(EnDns* thisv, GlobalContext* globalCtx);
void func_809EFDD0(EnDns* thisv, GlobalContext* globalCtx);
void func_809EFEE8(EnDns* thisv, GlobalContext* globalCtx);
void func_809EFF50(EnDns* thisv, GlobalContext* globalCtx);
void func_809EFF98(EnDns* thisv, GlobalContext* globalCtx);
void func_809F008C(EnDns* thisv, GlobalContext* globalCtx);
void EnDns_SetupBurrow(EnDns* thisv, GlobalContext* globalCtx);
void EnDns_Burrow(EnDns* thisv, GlobalContext* globalCtx);

const ActorInit En_Dns_InitVars = {
    ACTOR_EN_DNS,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SHOPNUTS,
    sizeof(EnDns),
    (ActorFunc)EnDns_Init,
    (ActorFunc)EnDns_Destroy,
    (ActorFunc)EnDns_Update,
    (ActorFunc)EnDns_Draw,
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
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 32, 0, { 0, 0, 0 } },
};

static u16 D_809F040C[] = {
    0x10A0, 0x10A1, 0x10A2, 0x10CA, 0x10CB, 0x10CC, 0x10CD, 0x10CE, 0x10CF, 0x10DC, 0x10DD,
};

// Debug text: "sells"  { "Deku Nuts",    "Deku Sticks",        "Piece of Heart",  "Deku Seeds",
//                        "Deku Shield",  "Bombs",              "Arrows",          "Red Potion",
//                        "Green Potion", "Deku Stick Upgrade", "Deku Nut Upgrade" }
static char* D_809F0424[] = {
    "デクの実売り            ", "デクの棒売り            ", "ハートの欠片売り        ", "デクの種売り            ",
    "デクの盾売り            ", "バクダン売り            ", "矢売り                  ", "赤のくすり売り          ",
    "緑のくすり売り          ", "デクの棒持てる数を増やす", "デクの実持てる数を増やす",
};

static DnsItemEntry D_809F0450 = { 20, 5, GI_NUTS_5_2, func_809EF5A4, func_809EFA28 };

static DnsItemEntry D_809F0460 = { 15, 1, GI_STICKS_1, func_809EF658, func_809EF9F8 };

static DnsItemEntry D_809F0470 = { 10, 1, GI_HEART_PIECE, func_809EF70C, func_809EFA58 };

static DnsItemEntry D_809F0480 = { 40, 30, GI_SEEDS_30, func_809EF73C, func_809EF9F8 };

static DnsItemEntry D_809F0490 = { 50, 1, GI_SHIELD_DEKU, func_809EF800, func_809EF9F8 };

static DnsItemEntry D_809F04A0 = { 40, 5, GI_BOMBS_5, func_809EF854, func_809EFA9C };

static DnsItemEntry D_809F04B0 = { 70, 20, GI_ARROWS_LARGE, func_809EF8F4, func_809EFACC };

static DnsItemEntry D_809F04C0 = { 40, 1, GI_POTION_RED, func_809EF9A4, func_809EF9F8 };

static DnsItemEntry D_809F04D0 = { 40, 1, GI_POTION_GREEN, func_809EF9A4, func_809EF9F8 };

static DnsItemEntry D_809F04E0 = { 40, 1, GI_STICK_UPGRADE_20, func_809EF70C, func_809EFAFC };

static DnsItemEntry D_809F04F0 = { 40, 1, GI_NUT_UPGRADE_30, func_809EF70C, func_809EFB40 };

static DnsItemEntry* sItemEntries[] = {
    &D_809F0450, &D_809F0460, &D_809F0470, &D_809F0480, &D_809F0490, &D_809F04A0,
    &D_809F04B0, &D_809F04C0, &D_809F04D0, &D_809F04E0, &D_809F04F0,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4E, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

typedef enum {
    /* 0 */ ENDNS_ANIM_0,
    /* 1 */ ENDNS_ANIM_1,
    /* 2 */ ENDNS_ANIM_2
} EnDnsAnimation;

static AnimationMinimalInfo sAnimationInfo[] = {
    { &gBusinessScrubNervousIdleAnim, ANIMMODE_LOOP, 0.0f },
    { &gBusinessScrubAnim_4404, ANIMMODE_ONCE, 0.0f },
    { &gBusinessScrubNervousTransitionAnim, ANIMMODE_ONCE, 0.0f },
};

void EnDns_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDns* thisv = (EnDns*)thisx;

    if (thisv->actor.params < 0) {
        // "Function Error (Deku Salesman)"
        osSyncPrintf(VT_FGCOL(RED) "引数エラー（売りナッツ）[ arg_data = %d ]" VT_RST "\n", thisv->actor.params);
        Actor_Kill(&thisv->actor);
        return;
    }
    // Sell Seeds instead of Arrows if Link is child
    if ((thisv->actor.params == 0x0006) && (LINK_AGE_IN_YEARS == YEARS_CHILD)) {
        thisv->actor.params = 0x0003;
    }
    // "Deku Salesman"
    osSyncPrintf(VT_FGCOL(GREEN) "◆◆◆ 売りナッツ『%s』 ◆◆◆" VT_RST "\n", D_809F0424[thisv->actor.params],
                 thisv->actor.params);
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBusinessScrubSkel, &gBusinessScrubNervousTransitionAnim,
                       thisv->jointTable, thisv->morphTable, 18);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    thisv->actor.textId = D_809F040C[thisv->actor.params];
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->maintainCollider = 1;
    thisv->standOnGround = 1;
    thisv->dropCollectible = 0;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -1.0f;
    thisv->dnsItemEntry = sItemEntries[thisv->actor.params];
    thisv->actionFunc = EnDns_SetupWait;
}

void EnDns_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDns* thisv = (EnDns*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnDns_ChangeAnim(EnDns* thisv, u8 index) {
    s16 frameCount;

    frameCount = Animation_GetLastFrame(sAnimationInfo[index].animation);
    thisv->unk_2BA = index; // Not used anywhere else?
    Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, (f32)frameCount,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

/* Item give checking functions */

u32 func_809EF5A4(EnDns* thisv) {
    if ((CUR_CAPACITY(UPG_NUTS) != 0) && (AMMO(ITEM_NUT) >= CUR_CAPACITY(UPG_NUTS))) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_NUT) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF658(EnDns* thisv) {
    if ((CUR_CAPACITY(UPG_STICKS) != 0) && (AMMO(ITEM_STICK) >= CUR_CAPACITY(UPG_STICKS))) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_STICK) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF70C(EnDns* thisv) {
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF73C(EnDns* thisv) {
    if (INV_CONTENT(ITEM_SLINGSHOT) == ITEM_NONE) {
        return 3;
    }
    if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_SEEDS) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF800(EnDns* thisv) {
    if (gBitFlags[4] & gSaveContext.inventory.equipment) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF854(EnDns* thisv) {
    if (!CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 3;
    }
    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF8F4(EnDns* thisv) {
    if (Item_CheckObtainability(ITEM_BOW) == ITEM_NONE) {
        return 3;
    }
    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF9A4(EnDns* thisv) {
    if (!Inventory_HasEmptyBottle()) {
        return 1;
    }
    if (gSaveContext.rupees < thisv->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

/* Paying and flagging functions */

void func_809EF9F8(EnDns* thisv) {
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFA28(EnDns* thisv) {
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFA58(EnDns* thisv) {
    gSaveContext.itemGetInf[0] |= 0x800;
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFA9C(EnDns* thisv) {
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFACC(EnDns* thisv) {
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFAFC(EnDns* thisv) {
    gSaveContext.infTable[25] |= 0x4;
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void func_809EFB40(EnDns* thisv) {
    gSaveContext.infTable[25] |= 0x8;
    Rupees_ChangeBy(-thisv->dnsItemEntry->itemPrice);
}

void EnDns_SetupWait(EnDns* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame == thisv->skelAnime.endFrame) {
        thisv->actionFunc = EnDns_Wait;
        EnDns_ChangeAnim(thisv, ENDNS_ANIM_0);
    }
}

void EnDns_Wait(EnDns* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 2000, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDns_Talk;
    } else {
        if ((thisv->collider.base.ocFlags1 & OC1_HIT) || thisv->actor.isTargeted) {
            thisv->actor.flags |= ACTOR_FLAG_16;
        } else {
            thisv->actor.flags &= ~ACTOR_FLAG_16;
        }
        if (thisv->actor.xzDistToPlayer < 130.0f) {
            func_8002F2F4(&thisv->actor, globalCtx);
        }
    }
}

void EnDns_Talk(EnDns* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // OK
                switch (thisv->dnsItemEntry->purchaseableCheck(thisv)) {
                    case 0:
                        Message_ContinueTextbox(globalCtx, 0x10A5);
                        thisv->actionFunc = func_809F008C;
                        break;
                    case 1:
                        Message_ContinueTextbox(globalCtx, 0x10A6);
                        thisv->actionFunc = func_809F008C;
                        break;
                    case 3:
                        Message_ContinueTextbox(globalCtx, 0x10DE);
                        thisv->actionFunc = func_809F008C;
                        break;
                    case 2:
                    case 4:
                        Message_ContinueTextbox(globalCtx, 0x10A7);
                        thisv->actionFunc = func_809EFEE8;
                        break;
                }
                break;
            case 1: // No way
                Message_ContinueTextbox(globalCtx, 0x10A4);
                thisv->actionFunc = func_809F008C;
        }
    }
}

void func_809EFDD0(EnDns* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == 0x9) {
        if (CUR_UPG_VALUE(UPG_STICKS) < 2) {
            func_8002F434(&thisv->actor, globalCtx, GI_STICK_UPGRADE_20, 130.0f, 100.0f);
        } else {
            func_8002F434(&thisv->actor, globalCtx, GI_STICK_UPGRADE_30, 130.0f, 100.0f);
        }
    } else if (thisv->actor.params == 0xA) {
        if (CUR_UPG_VALUE(UPG_NUTS) < 2) {
            func_8002F434(&thisv->actor, globalCtx, GI_NUT_UPGRADE_30, 130.0f, 100.0f);
        } else {
            func_8002F434(&thisv->actor, globalCtx, GI_NUT_UPGRADE_40, 130.0f, 100.0f);
        }
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->dnsItemEntry->getItemId, 130.0f, 100.0f);
    }
}

void func_809EFEE8(EnDns* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_809EFDD0(thisv, globalCtx);
        thisv->actionFunc = func_809EFF50;
    }
}

void func_809EFF50(EnDns* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_809EFF98;
    } else {
        func_809EFDD0(thisv, globalCtx);
    }
}

void func_809EFF98(EnDns* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags1 & 0x400) {
        if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
            thisv->dnsItemEntry->setRupeesAndFlags(thisv);
            thisv->dropCollectible = 1;
            thisv->maintainCollider = 0;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            EnDns_ChangeAnim(thisv, ENDNS_ANIM_1);
            thisv->actionFunc = EnDns_SetupBurrow;
        }
    } else {
        thisv->dnsItemEntry->setRupeesAndFlags(thisv);
        thisv->dropCollectible = 1;
        thisv->maintainCollider = 0;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        EnDns_ChangeAnim(thisv, ENDNS_ANIM_1);
        thisv->actionFunc = EnDns_SetupBurrow;
    }
}

void func_809F008C(EnDns* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        thisv->maintainCollider = 0;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        EnDns_ChangeAnim(thisv, ENDNS_ANIM_1);
        thisv->actionFunc = EnDns_SetupBurrow;
    }
}

void EnDns_SetupBurrow(EnDns* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gBusinessScrubAnim_4404);

    if (thisv->skelAnime.curFrame == frameCount) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_AKINDONUTS_HIDE);
        thisv->actionFunc = EnDns_Burrow;
        thisv->standOnGround = 0;
        thisv->yInitPos = thisv->actor.world.pos.y;
    }
}

void EnDns_Burrow(EnDns* thisv, GlobalContext* globalCtx) {
    f32 depth;
    Vec3f initPos;
    s32 i;

    depth = thisv->yInitPos - thisv->actor.world.pos.y;
    if ((thisv->dustTimer & 3) == 0) {
        initPos.x = thisv->actor.world.pos.x;
        initPos.y = thisv->yInitPos;
        initPos.z = thisv->actor.world.pos.z;
        func_80028990(globalCtx, 20.0f, &initPos);
    }
    thisv->actor.shape.rot.y += 0x2000;
    // Drops only if you bought its item
    if (depth > 400.0f) {
        if (thisv->dropCollectible) {
            initPos.x = thisv->actor.world.pos.x;
            initPos.y = thisv->yInitPos;
            initPos.z = thisv->actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_DropCollectible(globalCtx, &initPos, ITEM00_HEART);
            }
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnDns_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDns* thisv = (EnDns*)thisx;
    s16 pad;

    thisv->dustTimer++;
    thisv->actor.textId = D_809F040C[thisv->actor.params];
    Actor_SetFocus(&thisv->actor, 60.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    SkelAnime_Update(&thisv->skelAnime);
    Actor_MoveForward(&thisv->actor);
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->standOnGround) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 20.0f, 4);
    }
    if (thisv->maintainCollider) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void EnDns_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDns* thisv = (EnDns*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, &thisv->actor);
}
