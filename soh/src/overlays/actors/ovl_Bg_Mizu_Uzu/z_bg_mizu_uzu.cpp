/*
 * File: z_bg_mizu_uzu.c
 * Overlay: ovl_Bg_Mizu_Uzu
 * Description: Water Noise
 */

#include "z_bg_mizu_uzu.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS 0

void BgMizuUzu_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuUzu_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuUzu_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuUzu_Draw(Actor* thisvx, GlobalContext* globalCtx);

void func_8089F788(BgMizuUzu* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Mizu_Uzu_InitVars = {
    ACTOR_BG_MIZU_UZU,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuUzu),
    (ActorFunc)BgMizuUzu_Init,
    (ActorFunc)BgMizuUzu_Destroy,
    (ActorFunc)BgMizuUzu_Update,
    (ActorFunc)BgMizuUzu_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMizuUzu_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMizuUzu* thisv = (BgMizuUzu*)thisvx;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gObjectMizuObjectsUzuCol_0074EC, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->actionFunc = func_8089F788;
}

void BgMizuUzu_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgMizuUzu* thisv = (BgMizuUzu*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_8089F788(BgMizuUzu* thisv, GlobalContext* globalCtx) {
    Actor* thisvx = &thisv->dyna.actor;

    if (GET_PLAYER(globalCtx)->currentBoots == PLAYER_BOOTS_IRON) {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    } else {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
    Audio_PlayActorSound2(thisvx, NA_SE_EV_WATER_CONVECTION - SFX_FLAG);
    thisvx->shape.rot.y += 0x1C0;
}

void BgMizuUzu_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgMizuUzu* thisv = (BgMizuUzu*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgMizuUzu_Draw(Actor* thisvx, GlobalContext* globalCtx) {
}
