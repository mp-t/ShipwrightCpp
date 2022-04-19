/*
 * File: z_bg_menkuri_kaiten.c
 * Overlay: Bg_Menkuri_Kaiten
 * Description: Large rotating stone ring used in Gerudo Training Grounds and Forest Temple.
 */

#include "z_bg_menkuri_kaiten.h"
#include "objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMenkuriKaiten_Init(Actor* thisx, GlobalContext* globalCtx);
void BgMenkuriKaiten_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgMenkuriKaiten_Update(Actor* thisx, GlobalContext* globalCtx);
void BgMenkuriKaiten_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit Bg_Menkuri_Kaiten_InitVars = {
    ACTOR_BG_MENKURI_KAITEN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MENKURI_OBJECTS,
    sizeof(BgMenkuriKaiten),
    (ActorFunc)BgMenkuriKaiten_Init,
    (ActorFunc)BgMenkuriKaiten_Destroy,
    (ActorFunc)BgMenkuriKaiten_Update,
    (ActorFunc)BgMenkuriKaiten_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMenkuriKaiten_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgMenkuriKaiten* thisv = (BgMenkuriKaiten*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK3);
    CollisionHeader_GetVirtual(&gGTGRotatingRingPlatformCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void BgMenkuriKaiten_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgMenkuriKaiten* thisv = (BgMenkuriKaiten*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgMenkuriKaiten_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgMenkuriKaiten* thisv = (BgMenkuriKaiten*)thisx;

    if (!Flags_GetSwitch(globalCtx, thisv->dyna.actor.params) && func_80043590(&thisv->dyna)) {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
        thisv->dyna.actor.shape.rot.y += 0x80;
    }
}

void BgMenkuriKaiten_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGTGRotatingRingPlatformDL);
}
