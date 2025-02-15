/*
 * File: z_bg_spot18_futa.c
 * Overlay: ovl_Bg_Spot18_Futa
 * Description: The lid to the spinning goron vase.
 */

#include "z_bg_spot18_futa.h"
#include "objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS 0

void BgSpot18Futa_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Futa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Futa_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Futa_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit Bg_Spot18_Futa_InitVars = {
    ACTOR_BG_SPOT18_FUTA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SPOT18_OBJ,
    sizeof(BgSpot18Futa),
    (ActorFunc)BgSpot18Futa_Init,
    (ActorFunc)BgSpot18Futa_Destroy,
    (ActorFunc)BgSpot18Futa_Update,
    (ActorFunc)BgSpot18Futa_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgSpot18Futa_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Futa* thisv = (BgSpot18Futa*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gGoronCityVaseLidCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
}

void BgSpot18Futa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Futa* thisv = (BgSpot18Futa*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgSpot18Futa_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Futa* thisv = (BgSpot18Futa*)thisx;
    s32 iVar1;

    if (thisv->dyna.actor.parent == NULL) {
        iVar1 = Math_StepToF(&thisv->dyna.actor.scale.x, 0, 0.005);

        if (iVar1 != 0) {
            Actor_Kill(&thisv->dyna.actor);
        } else {
            thisv->dyna.actor.scale.z = thisv->dyna.actor.scale.x;
            thisv->dyna.actor.scale.y = thisv->dyna.actor.scale.x;
        }
    }
}

void BgSpot18Futa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGoronCityVaseLidDL);
}
