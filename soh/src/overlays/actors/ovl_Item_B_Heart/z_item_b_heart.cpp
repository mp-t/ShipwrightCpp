/*
 * File: z_item_b_heart.c
 * Overlay: ovl_Item_B_Heart
 * Description: Heart Container
 */

#include "z_item_b_heart.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"

#define FLAGS 0

void ItemBHeart_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemBHeart_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemBHeart_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemBHeart_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B85264(ItemBHeart* thisv, GlobalContext* globalCtx);

ActorInit Item_B_Heart_InitVars = {
    ACTOR_ITEM_B_HEART,
    ACTORCAT_MISC,
    FLAGS,
    OBJECT_GI_HEARTS,
    sizeof(ItemBHeart),
    (ActorFunc)ItemBHeart_Init,
    (ActorFunc)ItemBHeart_Destroy,
    (ActorFunc)ItemBHeart_Update,
    (ActorFunc)ItemBHeart_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

void ItemBHeart_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemBHeart* thisv = (ItemBHeart*)thisx;

    if (Flags_GetCollectible(globalCtx, 0x1F)) {
        Actor_Kill(&thisv->actor);
    } else {
        Actor_ProcessInitChain(&thisv->actor, sInitChain);
        ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.8f);
    }
}

void ItemBHeart_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ItemBHeart_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemBHeart* thisv = (ItemBHeart*)thisx;

    func_80B85264(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        Flags_SetCollectible(globalCtx, 0x1F);
        Actor_Kill(&thisv->actor);
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_HEART_CONTAINER_2, 30.0f, 40.0f);
    }
}

void func_80B85264(ItemBHeart* thisv, GlobalContext* globalCtx) {
    f32 yOffset;

    thisv->unk_164++;
    yOffset = (Math_SinS(thisv->unk_164 * 0x60C) * 5.0f) + 20.0f;
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + yOffset, 0.1f, thisv->unk_158);
    Math_ApproachF(&thisv->unk_158, 2.0f, 1.0f, 0.1f);
    thisv->actor.shape.rot.y += 0x400;

    Math_ApproachF(&thisv->actor.scale.x, 0.4f, 0.1f, 0.01f);
    thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;
}

void ItemBHeart_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemBHeart* thisv = (ItemBHeart*)thisx;
    Actor* actorIt;
    u8 flag = false;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_item_b_heart.c", 506);

    actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (actorIt != NULL) {
        if ((actorIt->id == ACTOR_DOOR_WARP1) && (actorIt->projectedPos.z > thisv->actor.projectedPos.z)) {
            flag = true;
            break;
        }
        actorIt = actorIt->next;
    }

    if (flag) {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_item_b_heart.c", 551),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGiHeartBorderDL);
        gSPDisplayList(POLY_XLU_DISP++, gGiHeartContainerDL);
    } else {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_item_b_heart.c", 557),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gGiHeartBorderDL);
        gSPDisplayList(POLY_OPA_DISP++, gGiHeartContainerDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_item_b_heart.c", 561);
}
