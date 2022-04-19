/*
 * File: z_bg_jya_kanaami.c
 * Overlay: ovl_Bg_Jya_Kanaami
 * Description: Climbable grating/bridge (Spirit Temple)
 */

#include "z_bg_jya_kanaami.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS 0

void BgJyaKanaami_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaKanaami_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaKanaami_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaKanaami_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80899880(BgJyaKanaami* thisv);
void func_80899894(BgJyaKanaami* thisv, GlobalContext* globalCtx);
void func_8089993C(BgJyaKanaami* thisv);
void func_80899950(BgJyaKanaami* thisv, GlobalContext* globalCtx);
void func_80899A08(BgJyaKanaami* thisv);

ActorInit Bg_Jya_Kanaami_InitVars = {
    ACTOR_BG_JYA_KANAAMI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaKanaami),
    (ActorFunc)BgJyaKanaami_Init,
    (ActorFunc)BgJyaKanaami_Destroy,
    (ActorFunc)BgJyaKanaami_Update,
    (ActorFunc)BgJyaKanaami_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaKanaami_InitDynaPoly(BgJyaKanaami* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_kanaami.c", 145,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgJyaKanaami_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaKanaami* thisv = (BgJyaKanaami*)thisx;

    BgJyaKanaami_InitDynaPoly(thisv, globalCtx, &gKanaamiCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        func_80899A08(thisv);
    } else {
        func_80899880(thisv);
    }
    osSyncPrintf("(jya 金網)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void BgJyaKanaami_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaKanaami* thisv = (BgJyaKanaami*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80899880(BgJyaKanaami* thisv) {
    thisv->actionFunc = func_80899894;
    thisv->unk_16A = 0;
}

void func_80899894(BgJyaKanaami* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F) || thisv->unk_16A > 0) {
        if (thisv->dyna.actor.world.pos.x > -1000.0f && thisv->unk_16A == 0) {
            OnePointCutscene_Init(globalCtx, 3450, -99, &thisv->dyna.actor, MAIN_CAM);
        }
        thisv->unk_16A += 1;
        if (thisv->unk_16A >= 0xA) {
            func_8089993C(thisv);
        }
    }
}

void func_8089993C(BgJyaKanaami* thisv) {
    thisv->actionFunc = func_80899950;
    thisv->unk_168 = 0;
}

void func_80899950(BgJyaKanaami* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s32 quakeId;

    thisv->unk_168 += 0x20;
    if (Math_ScaledStepToS(&thisv->dyna.actor.world.rot.x, 0x4000, thisv->unk_168)) {
        func_80899A08(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_TRAP_BOUND);
        quakeId = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
        Quake_SetSpeed(quakeId, 25000);
        Quake_SetQuakeValues(quakeId, 2, 0, 0, 0);
        Quake_SetCountdown(quakeId, 16);
    }
}

void func_80899A08(BgJyaKanaami* thisv) {
    thisv->actionFunc = 0;
    thisv->dyna.actor.world.rot.x = 0x4000;
}

void BgJyaKanaami_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaKanaami* thisv = (BgJyaKanaami*)thisx;

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
    thisv->dyna.actor.shape.rot.x = thisv->dyna.actor.world.rot.x;
}

void BgJyaKanaami_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gKanaamiDL);
}
