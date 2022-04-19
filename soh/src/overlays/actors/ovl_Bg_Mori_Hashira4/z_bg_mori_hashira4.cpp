/*
 * File: z_bg_mori_hashira4
 * Overlay: ovl_Bg_Mori_Hashira4
 * Description: Forest Temple gates and rotating pillars
 */

#include "z_bg_mori_hashira4.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgMoriHashira4_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashira4_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashira4_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashira4_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriHashira4_SetupWaitForMoriTex(BgMoriHashira4* thisv);
void BgMoriHashira4_WaitForMoriTex(BgMoriHashira4* thisv, GlobalContext* globalCtx);
void BgMoriHashira4_SetupPillarsRotate(BgMoriHashira4* thisv);
void BgMoriHashira4_PillarsRotate(BgMoriHashira4* thisv, GlobalContext* globalCtx);
void BgMoriHashira4_GateWait(BgMoriHashira4* thisv, GlobalContext* globalCtx);
void BgMoriHashira4_GateOpen(BgMoriHashira4* thisv, GlobalContext* globalCtx);

ActorInit Bg_Mori_Hashira4_InitVars = {
    ACTOR_BG_MORI_HASHIRA4,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriHashira4),
    (ActorFunc)BgMoriHashira4_Init,
    (ActorFunc)BgMoriHashira4_Destroy,
    (ActorFunc)BgMoriHashira4_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static const Gfx* sDisplayLists[] = { gMoriHashiraPlatformsDL, gMoriHashiraGateDL };

static s16 sUnkTimer; // seems to be unused

void BgMoriHashira4_SetupAction(BgMoriHashira4* thisv, BgMoriHashira4ActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgMoriHashira4_InitDynaPoly(BgMoriHashira4* thisv, GlobalContext* globalCtx, const CollisionHeader* collision,
                                 s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader;
    s32 pad2;

    colHeader = NULL;
    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashira4.c", 155,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgMoriHashira4_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashira4* thisv = (BgMoriHashira4*)thisvx;

    thisv->switchFlag = (thisv->dyna.actor.params >> 8) & 0x3F;
    thisv->dyna.actor.params &= 0xFF;

    if (thisv->dyna.actor.params == 0) {
        BgMoriHashira4_InitDynaPoly(thisv, globalCtx, &gMoriHashira1Col, DPM_UNK3);
    } else {
        BgMoriHashira4_InitDynaPoly(thisv, globalCtx, &gMoriHashira2Col, DPM_UNK);
    }
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        Actor_Kill(&thisv->dyna.actor);
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", thisv->dyna.actor.params,
                     "../z_bg_mori_hashira4.c", 196);
        return;
    }
    if ((thisv->dyna.actor.params != 0) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }
    Actor_SetFocus(&thisv->dyna.actor, 50.0f);
    BgMoriHashira4_SetupWaitForMoriTex(thisv);
    // "(4 pillars of the Forest Temple) Bank danger"
    osSyncPrintf("(森の神殿 ４本柱)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
    sUnkTimer = 0;
}

void BgMoriHashira4_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashira4* thisv = (BgMoriHashira4*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgMoriHashira4_SetupWaitForMoriTex(BgMoriHashira4* thisv) {
    BgMoriHashira4_SetupAction(thisv, BgMoriHashira4_WaitForMoriTex);
}

void BgMoriHashira4_WaitForMoriTex(BgMoriHashira4* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        thisv->gateTimer = 0;
        if (thisv->dyna.actor.params == 0) {
            BgMoriHashira4_SetupPillarsRotate(thisv);
        } else {
            BgMoriHashira4_SetupAction(thisv, BgMoriHashira4_GateWait);
        }
        thisv->dyna.actor.draw = BgMoriHashira4_Draw;
    }
}

void BgMoriHashira4_SetupPillarsRotate(BgMoriHashira4* thisv) {
    BgMoriHashira4_SetupAction(thisv, BgMoriHashira4_PillarsRotate);
}

void BgMoriHashira4_PillarsRotate(BgMoriHashira4* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.world.rot.y += 0x96;
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
}

void BgMoriHashira4_GateWait(BgMoriHashira4* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag) || (thisv->gateTimer != 0)) {
        thisv->gateTimer++;
        if (thisv->gateTimer > 30) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
            BgMoriHashira4_SetupAction(thisv, BgMoriHashira4_GateOpen);
            OnePointCutscene_Init(globalCtx, 6010, 20, &thisv->dyna.actor, MAIN_CAM);
            sUnkTimer++;
        }
    }
}

void BgMoriHashira4_GateOpen(BgMoriHashira4* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 120.0f, 10.0f)) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgMoriHashira4_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashira4* thisv = (BgMoriHashira4*)thisvx;

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgMoriHashira4_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashira4* thisv = (BgMoriHashira4*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hashira4.c", 339);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hashira4.c", 344),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[thisv->dyna.actor.params]);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hashira4.c", 348);
}
