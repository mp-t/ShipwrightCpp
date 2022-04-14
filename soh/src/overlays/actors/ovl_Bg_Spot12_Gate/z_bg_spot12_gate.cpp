/*
 * File: z_bg_spot12_gate.c
 * Overlay: ovl_Bg_Spot12_Gate
 * Description: Haunted Wasteland Gate
 */

#include "z_bg_spot12_gate.h"
#include "objects/object_spot12_obj/object_spot12_obj.h"

#define FLAGS 0

void BgSpot12Gate_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Gate_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Gate_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Gate_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B30C0(BgSpot12Gate* thisv);
void func_808B30D8(BgSpot12Gate* thisv, GlobalContext* globalCtx);
void func_808B3134(BgSpot12Gate* thisv);
void func_808B314C(BgSpot12Gate* thisv, GlobalContext* globalCtx);
void func_808B317C(BgSpot12Gate* thisv);
void func_808B318C(BgSpot12Gate* thisv, GlobalContext* globalCtx);
void func_808B3274(BgSpot12Gate* thisv);
void func_808B3298(BgSpot12Gate* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Spot12_Gate_InitVars = {
    ACTOR_BG_SPOT12_GATE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT12_OBJ,
    sizeof(BgSpot12Gate),
    (ActorFunc)BgSpot12Gate_Init,
    (ActorFunc)BgSpot12Gate_Destroy,
    (ActorFunc)BgSpot12Gate_Update,
    (ActorFunc)BgSpot12Gate_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

void BgSpot12Gate_InitDynaPoly(BgSpot12Gate* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flags) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot12_gate.c", 145,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgSpot12Gate_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Gate* thisv = (BgSpot12Gate*)thisx;

    BgSpot12Gate_InitDynaPoly(thisv, globalCtx, &gGerudoFortressWastelandGateCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        func_808B3274(thisv);
    } else {
        func_808B30C0(thisv);
    }
}

void BgSpot12Gate_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Gate* thisv = (BgSpot12Gate*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808B30C0(BgSpot12Gate* thisv) {
    thisv->actionFunc = func_808B30D8;
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y;
}

void func_808B30D8(BgSpot12Gate* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        func_808B3134(thisv);
        OnePointCutscene_Init(globalCtx, 4160, -99, &thisv->dyna.actor, MAIN_CAM);
    }
}

void func_808B3134(BgSpot12Gate* thisv) {
    thisv->actionFunc = func_808B314C;
    thisv->unk_168 = 40;
}

void func_808B314C(BgSpot12Gate* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_168 <= 0) {
        func_808B317C(thisv);
    }
}

void func_808B317C(BgSpot12Gate* thisv) {
    thisv->actionFunc = func_808B318C;
}

void func_808B318C(BgSpot12Gate* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 var;

    Math_StepToF(&thisv->dyna.actor.velocity.y, 1.6f, 0.03f);
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 200.0f,
                     thisv->dyna.actor.velocity.y)) {
        func_808B3274(thisv);
        var = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
        Quake_SetSpeed(var, -0x3CB0);
        Quake_SetQuakeValues(var, 3, 0, 0, 0);
        Quake_SetCountdown(var, 0xC);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    }
}

void func_808B3274(BgSpot12Gate* thisv) {
    thisv->actionFunc = func_808B3298;
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + 200.0f;
}

void func_808B3298(BgSpot12Gate* thisv, GlobalContext* globalCtx) {
}

void BgSpot12Gate_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Gate* thisv = (BgSpot12Gate*)thisx;

    if (thisv->unk_168 > 0) {
        thisv->unk_168--;
    }
    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot12Gate_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGerudoFortressWastelandGateDL);
}
