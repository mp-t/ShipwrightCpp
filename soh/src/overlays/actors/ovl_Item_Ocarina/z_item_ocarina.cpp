/*
 * File: z_item_ocarina.c
 * Overlay: ovl_Item_Ocarina
 * Description: Ocarina of Time
 */

#include "z_item_ocarina.h"
#include "scenes/overworld/spot00/spot00_scene.h"

#define FLAGS ACTOR_FLAG_4

void ItemOcarina_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Draw(Actor* thisx, GlobalContext* globalCtx);

void ItemOcarina_GetThrown(ItemOcarina* thisv, GlobalContext* globalCtx);
void ItemOcarina_Fly(ItemOcarina* thisv, GlobalContext* globalCtx);
void ItemOcarina_WaitInWater(ItemOcarina* thisv, GlobalContext* globalCtx);
void ItemOcarina_StartSoTCutscene(ItemOcarina* thisv, GlobalContext* globalCtx);
void func_80B864EC(ItemOcarina* thisv, GlobalContext* globalCtx);
void func_80B865E0(ItemOcarina* thisv, GlobalContext* globalCtx);
void ItemOcarina_DoNothing(ItemOcarina* thisv, GlobalContext* globalCtx);

ActorInit Item_Ocarina_InitVars = {
    ACTOR_ITEM_OCARINA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GI_OCARINA,
    sizeof(ItemOcarina),
    (ActorFunc)ItemOcarina_Init,
    (ActorFunc)ItemOcarina_Destroy,
    (ActorFunc)ItemOcarina_Update,
    (ActorFunc)ItemOcarina_Draw,
    NULL,
};

void ItemOcarina_SetupAction(ItemOcarina* thisv, ItemOcarinaActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ItemOcarina_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemOcarina* thisv = (ItemOcarina*)thisx;
    s32 params = thisx->params;

    ActorShape_Init(&thisv->actor.shape, 0, 0, 0);
    Actor_SetScale(&thisv->actor, 0.1f);

    switch (params) {
        case 0:
            ItemOcarina_SetupAction(thisv, ItemOcarina_GetThrown);
            break;
        case 1:
            ItemOcarina_SetupAction(thisv, func_80B865E0);
            break;
        case 2:
            ItemOcarina_SetupAction(thisv, ItemOcarina_DoNothing);
            break;
        case 3:
            ItemOcarina_SetupAction(thisv, ItemOcarina_WaitInWater);
            if (!(gSaveContext.eventChkInf[8] & 1) || (gSaveContext.eventChkInf[4] & 8)) {
                Actor_Kill(thisx);
                return;
            }
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ELF_MSG2, 299.0f, -140.0f, 884.0f, 0, 4, 1, 0x3800);
            Actor_SetScale(thisx, 0.2f);
            break;
        default:
            Actor_Kill(thisx);
            return;
    }

    LOG_NUM("no", params, "../z_item_ocarina.c", 210);
    thisv->spinRotOffset = 0x400;
}

void ItemOcarina_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ItemOcarina_Fly(ItemOcarina* thisv, GlobalContext* globalCtx) {
    Vec3f ripplePos;

    func_8002D7EC(&thisv->actor);
    thisv->actor.shape.rot.x += thisv->spinRotOffset * 2;
    thisv->actor.shape.rot.y += thisv->spinRotOffset * 3;

    if (thisv->actor.minVelocityY < thisv->actor.velocity.y) {
        thisv->actor.velocity.y += thisv->actor.gravity;
        if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
            thisv->actor.velocity.y = thisv->actor.minVelocityY;
        }
    }

    if (globalCtx->csCtx.frames == 881) {
        thisv->actor.world.pos.x = 250.0f;
        thisv->actor.world.pos.y = 60.0f;
        thisv->actor.world.pos.z = 1075.0f;
        thisv->actor.velocity.x = 1.0f;
        thisv->actor.velocity.y = -5.0f;
        thisv->actor.velocity.z = -7.0f;
    }

    if (globalCtx->csCtx.frames == 897) {
        EffectSsGRipple_Spawn(globalCtx, &thisv->actor.world.pos, 100, 500, 0);
        EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, 0, 0, 1, 0);
        thisv->actor.velocity.x = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.velocity.z = 0.0f;
        thisv->actor.gravity = -0.1f;
        thisv->actor.minVelocityY = -0.5f;
        thisv->spinRotOffset = 0;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BOMB_DROP_WATER);
    }

    // landed in water
    if (globalCtx->csCtx.frames == 906) {
        ripplePos.x = 274.0f;
        ripplePos.y = -60.0f;
        ripplePos.z = 907.0f;
        EffectSsGRipple_Spawn(globalCtx, &ripplePos, 100, 500, 0);
    }
}

void ItemOcarina_GetThrown(ItemOcarina* thisv, GlobalContext* globalCtx) {
    thisv->actor.gravity = -0.3f;
    thisv->actor.minVelocityY = -5.0f;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.y = 6.0f;
    thisv->actor.velocity.z = 0.0f;
    ItemOcarina_SetupAction(thisv, ItemOcarina_Fly);
}

void func_80B864EC(ItemOcarina* thisv, GlobalContext* globalCtx) {
    func_8002D7EC(&thisv->actor);
    thisv->actor.shape.rot.x += thisv->spinRotOffset * 2;
    thisv->actor.shape.rot.y += thisv->spinRotOffset * 3;

    if (thisv->actor.minVelocityY < thisv->actor.velocity.y) {
        thisv->actor.velocity.y += thisv->actor.gravity;

        if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
            thisv->actor.velocity.y = thisv->actor.minVelocityY;
        }
    }

    if (globalCtx->csCtx.frames == 220) {
        thisv->actor.world.pos.x = 144.0f;
        thisv->actor.world.pos.y = 80.0f;
        thisv->actor.world.pos.z = 1686.0f;
        thisv->actor.velocity.x = 1.0f;
        thisv->actor.velocity.y = 2.0f;
        thisv->actor.velocity.z = -7.0f;
        thisv->actor.gravity = -0.15f;
        thisv->actor.minVelocityY = -5.0f;
    }
}

void func_80B865E0(ItemOcarina* thisv, GlobalContext* globalCtx) {
    thisv->actor.gravity = -0.3f;
    thisv->actor.minVelocityY = -5.0f;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.y = 4.0f;
    thisv->actor.velocity.z = 6.0f;
    ItemOcarina_SetupAction(thisv, func_80B864EC);
}

void ItemOcarina_DoNothing(ItemOcarina* thisv, GlobalContext* globalCtx) {
}

void ItemOcarina_StartSoTCutscene(ItemOcarina* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gHyruleFieldZeldaSongOfTimeCs);
        gSaveContext.cutsceneTrigger = 1;
    }
}

void ItemOcarina_WaitInWater(ItemOcarina* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        gSaveContext.eventChkInf[4] |= 8;
        Flags_SetSwitch(globalCtx, 3);
        thisv->actionFunc = ItemOcarina_StartSoTCutscene;
        thisv->actor.draw = NULL;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_OCARINA_OOT, 30.0f, 50.0f);

        if ((globalCtx->gameplayFrames & 13) == 0) {
            EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemOcarina_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemOcarina* thisv = (ItemOcarina*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void ItemOcarina_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemOcarina* thisv = (ItemOcarina*)thisx;

    func_8002EBCC(thisx, globalCtx, 0);
    func_8002ED80(thisx, globalCtx, 0);
    GetItem_Draw(globalCtx, GID_OCARINA_TIME);
}
