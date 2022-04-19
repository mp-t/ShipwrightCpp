/*
 * File: z_bg_hidan_syoku.c
 * Overlay: ovl_Bg_Hidan_Syoku
 * Description: Stone Elevator in the Fire Temple
 */

#include "z_bg_hidan_syoku.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanSyoku_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanSyoku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanSyoku_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanSyoku_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8088F4B8(BgHidanSyoku* thisv, GlobalContext* globalCtx);
void func_8088F514(BgHidanSyoku* thisv, GlobalContext* globalCtx);
void func_8088F62C(BgHidanSyoku* thisv, GlobalContext* globalCtx);

ActorInit Bg_Hidan_Syoku_InitVars = {
    ACTOR_BG_HIDAN_SYOKU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanSyoku),
    (ActorFunc)BgHidanSyoku_Init,
    (ActorFunc)BgHidanSyoku_Destroy,
    (ActorFunc)BgHidanSyoku_Update,
    (ActorFunc)BgHidanSyoku_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHidanSyoku_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHidanSyoku* thisv = (BgHidanSyoku*)thisx;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gFireTempleFlareDancerPlatformCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->actionFunc = func_8088F4B8;
    thisv->dyna.actor.home.pos.y += 540.0f;
}

void BgHidanSyoku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanSyoku* thisv = (BgHidanSyoku*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_8088F47C(BgHidanSyoku* thisv) {
    thisv->timer = 60;
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    thisv->actionFunc = func_8088F62C;
}

void func_8088F4B8(BgHidanSyoku* thisv, GlobalContext* globalCtx) {
    if (Flags_GetClear(globalCtx, thisv->dyna.actor.room) && func_8004356C(&thisv->dyna)) {
        thisv->timer = 140;
        thisv->actionFunc = func_8088F514;
    }
}

void func_8088F514(BgHidanSyoku* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    thisv->dyna.actor.world.pos.y = (cosf(thisv->timer * (std::numbers::pi_v<float> / 140)) * 540.0f) + thisv->dyna.actor.home.pos.y;
    if (thisv->timer == 0) {
        func_8088F47C(thisv);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void func_8088F5A0(BgHidanSyoku* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - (cosf(thisv->timer * (std::numbers::pi_v<float> / 140)) * 540.0f);
    if (thisv->timer == 0) {
        func_8088F47C(thisv);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void func_8088F62C(BgHidanSyoku* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        thisv->timer = 140;
        if (thisv->dyna.actor.world.pos.y < thisv->dyna.actor.home.pos.y) {
            thisv->actionFunc = func_8088F514;
        } else {
            thisv->actionFunc = func_8088F5A0;
        }
    }
}

void BgHidanSyoku_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanSyoku* thisv = (BgHidanSyoku*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (func_8004356C(&thisv->dyna)) {
        if (thisv->unk_168 == 0) {
            thisv->unk_168 = 3;
        }
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
    } else if (!func_8004356C(&thisv->dyna)) {
        if (thisv->unk_168 != 0) {
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
        }
        thisv->unk_168 = 0;
    }
}

void BgHidanSyoku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gFireTempleFlareDancerPlatformDL);
}
