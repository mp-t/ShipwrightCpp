/*
 * File: z_item_shield.c
 * Overlay: ovl_Item_Shield
 * Description: Deku Shield
 */

#include "vt.h"
#include "z_item_shield.h"
#include "objects/object_link_child/object_link_child.h"

#define FLAGS ACTOR_FLAG_4

void ItemShield_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemShield_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemShield_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemShield_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B86F68(ItemShield* thisv, GlobalContext* globalCtx);
void func_80B86BC8(ItemShield* thisv, GlobalContext* globalCtx);

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
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 15, 0, { 0, 0, 0 } },
};

const ActorInit Item_Shield_InitVars = {
    ACTOR_ITEM_SHIELD,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_LINK_CHILD,
    sizeof(ItemShield),
    (ActorFunc)ItemShield_Init,
    (ActorFunc)ItemShield_Destroy,
    (ActorFunc)ItemShield_Update,
    (ActorFunc)ItemShield_Draw,
    NULL,
};

static Color_RGBA8 unused = { 255, 255, 0, 255 };
static Color_RGBA8 unused2 = { 255, 0, 0, 255 };

void ItemShield_SetupAction(ItemShield* thisv, ItemShieldActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ItemShield_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemShield* thisv = (ItemShield*)thisx;
    s32 i;

    thisv->timer = 0;
    thisv->unk_19C = 0;

    switch (thisv->actor.params) {
        case 0:
            ActorShape_Init(&thisv->actor.shape, 1400.0f, NULL, 0.0f);
            thisv->actor.shape.rot.x = 0x4000;
            ItemShield_SetupAction(thisv, func_80B86BC8);
            break;

        case 1:
            ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
            ItemShield_SetupAction(thisv, func_80B86F68);
            thisv->unk_19C |= 2;
            for (i = 0; i < 8; i++) {
                thisv->unk_19E[i] = 1 + 2 * i;
                thisv->unk_1A8[i].x = Rand_CenteredFloat(10.0f);
                thisv->unk_1A8[i].y = Rand_CenteredFloat(10.0f);
                thisv->unk_1A8[i].z = Rand_CenteredFloat(10.0f);
            }
            break;
    }

    Actor_SetScale(&thisv->actor, 0.01f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    osSyncPrintf(VT_FGCOL(GREEN) "Item_Shild %d \n" VT_RST, thisv->actor.params);
}

void ItemShield_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ItemShield* thisv = (ItemShield*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B86AC8(ItemShield* thisv, GlobalContext* globalCtx) {
    Actor_MoveForward(&thisv->actor);
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    func_8002F434(&thisv->actor, globalCtx, GI_SHIELD_DEKU, 30.0f, 50.0f);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 0.0f, 5);
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->timer--;
        if (thisv->timer < 60) {
            if (thisv->timer & 1) {
                thisv->unk_19C |= 2;
            } else {
                thisv->unk_19C &= ~2;
            }
        }
        if (thisv->timer == 0) {
            Actor_Kill(&thisv->actor);
        }
    }
}

void func_80B86BC8(ItemShield* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    func_8002F434(&thisv->actor, globalCtx, GI_SHIELD_DEKU, 30.0f, 50.0f);
    if (thisv->collider.base.acFlags & AC_HIT) {
        ItemShield_SetupAction(thisv, func_80B86AC8);
        thisv->actor.velocity.y = 4.0f;
        thisv->actor.minVelocityY = -4.0f;
        thisv->actor.gravity = -0.8f;
        thisv->actor.speedXZ = 0.0f;
        thisv->timer = 160;
    } else {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void func_80B86CA8(ItemShield* thisv, GlobalContext* globalCtx) {
    static Vec3f D_80B871F4 = { 0.0f, 0.0f, 0.0f };
    static f32 D_80B87200[] = { 0.3f, 0.6f,  0.9f, 1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
                                1.0f, 0.85f, 0.7f, 0.55f, 0.4f, 0.25f, 0.1f, 0.0f };
    static f32 D_80B87240[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f,
                                0.6f, 0.4f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 temp;

    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 0.0f, 5);
    thisv->actor.shape.yOffset = ABS(Math_SinS(thisv->actor.shape.rot.x)) * 1500.0f;

    for (i = 0; i < 8; i++) {
        temp = 15 - thisv->unk_19E[i];
        D_80B871F4.x = thisv->unk_1A8[i].x;
        D_80B871F4.y = thisv->unk_1A8[i].y + (thisv->actor.shape.yOffset * 0.01f) + (D_80B87200[temp] * -10.0f * 0.2f);
        D_80B871F4.z = thisv->unk_1A8[i].z;
        EffectSsFireTail_SpawnFlame(globalCtx, &thisv->actor, &D_80B871F4, D_80B87200[temp] * 0.2f, -1,
                                    D_80B87240[temp]);
        if (thisv->unk_19E[i] != 0) {
            thisv->unk_19E[i]--;
        } else if (thisv->timer > 16) {
            thisv->unk_19E[i] = 15;
            thisv->unk_1A8[i].x = Rand_CenteredFloat(15.0f);
            thisv->unk_1A8[i].y = Rand_CenteredFloat(10.0f);
            thisv->unk_1A8[i].z = Rand_CenteredFloat(15.0f);
        }
    }
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->unk_198 -= thisv->actor.shape.rot.x >> 1;
        thisv->unk_198 -= thisv->unk_198 >> 2;
        thisv->actor.shape.rot.x += thisv->unk_198;
        if ((thisv->timer >= 8) && (thisv->timer < 24)) {
            Actor_SetScale(&thisv->actor, (thisv->timer - 8) * 0.000625f);
        }
        if (thisv->timer != 0) {
            thisv->timer--;
        } else {
            Actor_Kill(&thisv->actor);
        }
    }
}

void func_80B86F68(ItemShield* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    MtxF* shield = &player->shieldMf;

    thisv->actor.world.pos.x = shield->mf_raw.xw;
    thisv->actor.world.pos.y = shield->mf_raw.yw;
    thisv->actor.world.pos.z = shield->mf_raw.zw;
    thisv->unk_19C &= ~2;

    thisv->actor.shape.rot.y = Math_Atan2S(-shield->mf_raw.zz, -shield->mf_raw.xz);
    thisv->actor.shape.rot.x = Math_Atan2S(-shield->mf_raw.yz, sqrtf(shield->mf_raw.zz * shield->mf_raw.zz + shield->mf_raw.xz * shield->mf_raw.xz));

    if (ABS(thisv->actor.shape.rot.x) > 0x4000) {
        thisv->unk_19C |= 1;
    }

    ItemShield_SetupAction(thisv, func_80B86CA8);

    thisv->actor.velocity.y = 4.0;
    thisv->actor.minVelocityY = -4.0;
    thisv->actor.gravity = -0.8;
    thisv->unk_198 = 0;
    thisv->timer = 70;
    thisv->actor.speedXZ = 0;
}

void ItemShield_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemShield* thisv = (ItemShield*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void ItemShield_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemShield* thisv = (ItemShield*)thisx;

    if (!(thisv->unk_19C & 2)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_item_shield.c", 457);
        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_item_shield.c", 460),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gLinkChildDekuShieldDL));
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_item_shield.c", 465);
    }
}
