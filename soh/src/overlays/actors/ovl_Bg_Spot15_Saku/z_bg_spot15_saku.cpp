/*
 * File: z_bg_spot15_saku.c
 * Overlay: ovl_Bg_Spot15_Saku
 * Description: Hyrule Castle Gate
 */

#include "z_bg_spot15_saku.h"
#include "objects/object_spot15_obj/object_spot15_obj.h"

#define FLAGS 0

void BgSpot15Saku_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B4930(BgSpot15Saku* thisv, GlobalContext* globalCtx);
void func_808B4978(BgSpot15Saku* thisv, GlobalContext* globalCtx);
void func_808B4A04(BgSpot15Saku* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot15_Saku_InitVars = {
    ACTOR_BG_SPOT15_SAKU,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_SPOT15_OBJ,
    sizeof(BgSpot15Saku),
    (ActorFunc)BgSpot15Saku_Init,
    (ActorFunc)BgSpot15Saku_Destroy,
    (ActorFunc)BgSpot15Saku_Update,
    (ActorFunc)BgSpot15Saku_Draw,
    NULL,
};

void BgSpot15Saku_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot15Saku* thisv = (BgSpot15Saku*)thisx;
    s32 pad2;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gLonLonCorralFenceCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->dyna.actor.scale.x = 0.1f;
    thisv->dyna.actor.scale.y = 0.1f;
    thisv->dyna.actor.scale.z = 0.1f;
    thisv->unk_170.x = thisv->dyna.actor.world.pos.x;
    thisv->unk_170.y = thisv->dyna.actor.world.pos.y;
    thisv->unk_170.z = thisv->dyna.actor.world.pos.z;
    if (gSaveContext.infTable[7] & 2) {
        thisv->dyna.actor.world.pos.z = 2659.0f;
    }
    thisv->actionFunc = func_808B4930;
}

void BgSpot15Saku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot15Saku* thisv = (BgSpot15Saku*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808B4930(BgSpot15Saku* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_168 && !(gSaveContext.infTable[7] & 2)) {
        thisv->timer = 2;
        thisv->actionFunc = func_808B4978;
    }
}

void func_808B4978(BgSpot15Saku* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        thisv->dyna.actor.world.pos.z -= 2.0f;
        if (thisv->dyna.actor.world.pos.z < 2660.0f) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
            thisv->timer = 30;
            thisv->actionFunc = func_808B4A04;
        }
    }
}

void func_808B4A04(BgSpot15Saku* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        thisv->unk_168 = 0;
        thisv->actionFunc = func_808B4930;
    }
}

void BgSpot15Saku_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot15Saku* thisv = (BgSpot15Saku*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot15Saku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot15_saku.c", 259);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot15_saku.c", 263),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gLonLonCorralFenceDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot15_saku.c", 268);
}
