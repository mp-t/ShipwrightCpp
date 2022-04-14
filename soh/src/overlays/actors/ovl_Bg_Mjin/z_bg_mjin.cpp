/*
 * File: z_bg_mjin.c
 * Overlay: ovl_Bg_Mjin
 * Description: Warp Pad
 */

#include "z_bg_mjin.h"
#include "objects/object_mjin/object_mjin.h"
#include "objects/object_mjin_wind/object_mjin_wind.h"
#include "objects/object_mjin_soul/object_mjin_soul.h"
#include "objects/object_mjin_dark/object_mjin_dark.h"
#include "objects/object_mjin_ice/object_mjin_ice.h"
#include "objects/object_mjin_flame/object_mjin_flame.h"
#include "objects/object_mjin_flash/object_mjin_flash.h"
#include "objects/object_mjin_oka/object_mjin_oka.h"

#define FLAGS ACTOR_FLAG_4

void BgMjin_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMjin_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMjin_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMjin_Draw(Actor* thisvx, GlobalContext* globalCtx);

void func_808A0850(BgMjin* thisv, GlobalContext* globalCtx);
void BgMjin_DoNothing(BgMjin* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Mjin_InitVars = {
    ACTOR_BG_MJIN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgMjin),
    (ActorFunc)BgMjin_Init,
    (ActorFunc)BgMjin_Destroy,
    (ActorFunc)BgMjin_Update,
    NULL,
    NULL,
};

extern UNK_TYPE D_06000000;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

static s16 sObjectIDs[] = { OBJECT_MJIN_FLASH, OBJECT_MJIN_DARK, OBJECT_MJIN_FLAME,
                            OBJECT_MJIN_ICE,   OBJECT_MJIN_SOUL, OBJECT_MJIN_WIND };

const void* gPedestalEmblems[] = { gLightMedallionPlatformTex, gShadowMedallionPlatformTex, gFireMedallionPlatformTex,
                             gWaterMedallionPlatformTex, gSpiritMedallionPlatformTex, gForestMedallionPlatformTex };

void BgMjin_SetupAction(BgMjin* thisv, BgMjinActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgMjin_Init(Actor* thisvx, GlobalContext* globalCtx) {
    BgMjin* thisv = (BgMjin*)thisvx;
    s8 objBankIndex;

    Actor_ProcessInitChain(thisvx, sInitChain);
    objBankIndex = Object_GetIndex(&globalCtx->objectCtx, (thisvx->params != 0 ? OBJECT_MJIN : OBJECT_MJIN_OKA));
    thisv->objBankIndex = objBankIndex;
    if (objBankIndex < 0) {
        Actor_Kill(thisvx);
    } else {
        BgMjin_SetupAction(thisv, func_808A0850);
    }
}

void BgMjin_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgMjin* thisv = (BgMjin*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808A0850(BgMjin* thisv, GlobalContext* globalCtx) {
    const CollisionHeader* colHeader;
    const CollisionHeader* collision;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex)) {
        colHeader = NULL;
        thisv->dyna.actor.flags &= ~ACTOR_FLAG_4;
        thisv->dyna.actor.objBankIndex = thisv->objBankIndex;
        Actor_SetObjectDependency(globalCtx, &thisv->dyna.actor);
        DynaPolyActor_Init(&thisv->dyna, 0);
        collision = thisv->dyna.actor.params != 0 ? &gWarpPadCol : &gOcarinaWarpPadCol;
        CollisionHeader_GetVirtual(collision, &colHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
        BgMjin_SetupAction(thisv, BgMjin_DoNothing);
        thisv->dyna.actor.draw = BgMjin_Draw;
    }
}

void BgMjin_DoNothing(BgMjin* thisv, GlobalContext* globalCtx) {
}

void BgMjin_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgMjin* thisv = (BgMjin*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgMjin_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    BgMjin* thisv = (BgMjin*)thisvx;
    const Gfx* dlist;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mjin.c", 250);

    if (thisvx->params != 0) {
        s32 objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIDs[thisvx->params - 1]);

        if (objBankIndex >= 0) {
            gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[objBankIndex].segment);
        }

        gSPSegment(POLY_OPA_DISP++, 0x08, gPedestalEmblems[thisvx->params - 1]);
        dlist = gWarpPadBaseDL;
    } else {
        dlist = gOcarinaWarpPadDL;
    }

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mjin.c", 285),
              G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mjin.c", 288);
}
