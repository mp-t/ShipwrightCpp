/*
 * File: z_bg_spot12_saku.c
 * Overlay: ovl_Bg_Spot12_Saku
 * Description:
 */

#include "z_bg_spot12_saku.h"
#include "objects/object_spot12_obj/object_spot12_obj.h"

#define FLAGS 0

void BgSpot12Saku_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B3550(BgSpot12Saku* thisv);
void func_808B357C(BgSpot12Saku* thisv, GlobalContext* globalCtx);
void func_808B35E4(BgSpot12Saku* thisv);
void func_808B3604(BgSpot12Saku* thisv, GlobalContext* globalCtx);
void func_808B3714(BgSpot12Saku* thisv);
void func_808B37AC(BgSpot12Saku* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot12_Saku_InitVars = {
    ACTOR_BG_SPOT12_SAKU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT12_OBJ,
    sizeof(BgSpot12Saku),
    (ActorFunc)BgSpot12Saku_Init,
    (ActorFunc)BgSpot12Saku_Destroy,
    (ActorFunc)BgSpot12Saku_Update,
    (ActorFunc)BgSpot12Saku_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void func_808B3420(BgSpot12Saku* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flags) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot12_saku.c", 140,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgSpot12Saku_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* thisv = (BgSpot12Saku*)thisx;

    func_808B3420(thisv, globalCtx, &gGerudoFortressGTGShutterCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        func_808B3714(thisv);
    } else {
        func_808B3550(thisv);
    }
}

void BgSpot12Saku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* thisv = (BgSpot12Saku*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808B3550(BgSpot12Saku* thisv) {
    thisv->actionFunc = func_808B357C;
    thisv->dyna.actor.scale.x = 0.1f;
    thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = thisv->dyna.actor.home.pos.z;
}

void func_808B357C(BgSpot12Saku* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        func_808B35E4(thisv);
        thisv->timer = 20;
        OnePointCutscene_Init(globalCtx, 4170, -99, &thisv->dyna.actor, MAIN_CAM);
    }
}

void func_808B35E4(BgSpot12Saku* thisv) {
    if (thisv->timer == 0) {
        thisv->actionFunc = func_808B3604;
    }
}

void func_808B3604(BgSpot12Saku* thisv, GlobalContext* globalCtx) {
    f32 temp_ret = Math_SmoothStepToF(&thisv->dyna.actor.scale.x, 0.001f / 0.14f, 0.16f, 0.0022f, 0.001f);
    f32 temp_f18 = ((0.1f - thisv->dyna.actor.scale.x) * 840.0f);

    thisv->dyna.actor.world.pos.x =
        thisv->dyna.actor.home.pos.x - (Math_SinS(thisv->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    thisv->dyna.actor.world.pos.z =
        thisv->dyna.actor.home.pos.z - (Math_CosS(thisv->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    if (fabsf(temp_ret) < 0.0001f) {
        func_808B3714(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    }
}

void func_808B3714(BgSpot12Saku* thisv) {
    thisv->actionFunc = func_808B37AC;
    thisv->dyna.actor.scale.x = 0.001f / 0.14f;
    thisv->dyna.actor.world.pos.x =
        thisv->dyna.actor.home.pos.x - (Math_SinS(thisv->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
    thisv->dyna.actor.world.pos.z =
        thisv->dyna.actor.home.pos.z - (Math_CosS(thisv->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
}

void func_808B37AC(BgSpot12Saku* thisv, GlobalContext* globalCtx) {
}

void BgSpot12Saku_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* thisv = (BgSpot12Saku*)thisx;

    if (thisv->timer > 0) {
        thisv->timer--;
    }
    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot12Saku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGerudoFortressGTGShutterDL);
}
