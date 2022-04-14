/*
 * File: z_bg_jya_amishutter.c
 * Overlay: Bg_Jya_Amishutter
 * Description: Circular metal grate. Lifts up when you get close to it.
 */

#include "z_bg_jya_amishutter.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS 0

void BgJyaAmishutter_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaAmishutter_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaAmishutter_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaAmishutter_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJyaAmishutter_SetupWaitForPlayer(BgJyaAmishutter* thisv);
void BgJyaAmishutter_WaitForPlayer(BgJyaAmishutter* thisv);
void func_80893428(BgJyaAmishutter* thisv);
void func_80893438(BgJyaAmishutter* thisv);
void func_808934B0(BgJyaAmishutter* thisv);
void func_808934C0(BgJyaAmishutter* thisv);
void func_808934FC(BgJyaAmishutter* thisv);
void func_8089350C(BgJyaAmishutter* thisv);

const ActorInit Bg_Jya_Amishutter_InitVars = {
    ACTOR_BG_JYA_AMISHUTTER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaAmishutter),
    (ActorFunc)BgJyaAmishutter_Init,
    (ActorFunc)BgJyaAmishutter_Destroy,
    (ActorFunc)BgJyaAmishutter_Update,
    (ActorFunc)BgJyaAmishutter_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaAmishutter_InitDynaPoly(BgJyaAmishutter* thisv, GlobalContext* globalCtx, const CollisionHeader* collision,
                                  s32 flag) {
    s32 pad1;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_amishutter.c", 129,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgJyaAmishutter_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaAmishutter* thisv = (BgJyaAmishutter*)thisx;

    BgJyaAmishutter_InitDynaPoly(thisv, globalCtx, &gAmishutterCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    BgJyaAmishutter_SetupWaitForPlayer(thisv);
}

void BgJyaAmishutter_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaAmishutter* thisv = (BgJyaAmishutter*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgJyaAmishutter_SetupWaitForPlayer(BgJyaAmishutter* thisv) {
    thisv->actionFunc = BgJyaAmishutter_WaitForPlayer;
}

void BgJyaAmishutter_WaitForPlayer(BgJyaAmishutter* thisv) {
    if ((thisv->dyna.actor.xzDistToPlayer < 60.0f) && (fabsf(thisv->dyna.actor.yDistToPlayer) < 30.0f)) {
        func_80893428(thisv);
    }
}

void func_80893428(BgJyaAmishutter* thisv) {
    thisv->actionFunc = func_80893438;
}

void func_80893438(BgJyaAmishutter* thisv) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 100.0f, 3.0f)) {
        func_808934B0(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void func_808934B0(BgJyaAmishutter* thisv) {
    thisv->actionFunc = func_808934C0;
}

void func_808934C0(BgJyaAmishutter* thisv) {
    if (thisv->dyna.actor.xzDistToPlayer > 300.0f) {
        func_808934FC(thisv);
    }
}

void func_808934FC(BgJyaAmishutter* thisv) {
    thisv->actionFunc = func_8089350C;
}

void func_8089350C(BgJyaAmishutter* thisv) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 3.0f)) {
        BgJyaAmishutter_SetupWaitForPlayer(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void BgJyaAmishutter_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaAmishutter* thisv = (BgJyaAmishutter*)thisx;

    thisv->actionFunc(thisv);
}

void BgJyaAmishutter_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gAmishutterDL);
}
