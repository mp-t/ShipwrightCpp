/*
 * File: z_bg_haka_meganebg.c
 * Overlay: ovl_Bg_Haka_MeganeBG
 * Description:
 */

#include "z_bg_haka_meganebg.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

void BgHakaMeganeBG_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaMeganeBG_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaMeganeBG_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaMeganeBG_Draw(Actor* thisvx, GlobalContext* globalCtx);

void func_8087DFF8(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E040(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E10C(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E1E0(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E258(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E288(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E2D8(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);
void func_8087E34C(BgHakaMeganeBG* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Haka_MeganeBG_InitVars = {
    ACTOR_BG_HAKA_MEGANEBG,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaMeganeBG),
    (ActorFunc)BgHakaMeganeBG_Init,
    (ActorFunc)BgHakaMeganeBG_Destroy,
    (ActorFunc)BgHakaMeganeBG_Update,
    (ActorFunc)BgHakaMeganeBG_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

// Unused
static u32 D_8087E3FC[] = {
    0x00000000, 0x00000000, 0x00000000, 0xC8C800FF, 0xFF0000FF,
};

static const Gfx* D_8087E410[] = {
    object_haka_objects_DL_008EB0,
    object_haka_objects_DL_00A1A0,
    object_haka_objects_DL_005000,
    object_haka_objects_DL_000040,
};

void BgHakaMeganeBG_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgHakaMeganeBG* thisv = (BgHakaMeganeBG*)thisvx;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->unk_168 = (thisvx->params >> 8) & 0xFF;
    thisvx->params &= 0xFF;

    if (thisvx->params == 2) {
        DynaPolyActor_Init(&thisv->dyna, DPM_UNK3);
        thisvx->flags |= ACTOR_FLAG_4;
        CollisionHeader_GetVirtual(&object_haka_objects_Col_005334, &colHeader);
        thisv->actionFunc = func_8087E258;
    } else {
        DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);

        if (thisvx->params == 0) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_009168, &colHeader);
            thisvx->flags |= ACTOR_FLAG_7;
            thisv->unk_16A = 20;
            thisv->actionFunc = func_8087DFF8;
        } else if (thisvx->params == 3) {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_000118, &colHeader);
            thisvx->home.pos.y += 100.0f;

            if (Flags_GetSwitch(globalCtx, thisv->unk_168)) {
                thisv->actionFunc = func_8087E34C;
                thisvx->world.pos.y = thisvx->home.pos.y;
            } else {
                thisvx->flags |= ACTOR_FLAG_4;
                thisv->actionFunc = func_8087E288;
            }
        } else {
            CollisionHeader_GetVirtual(&object_haka_objects_Col_00A7F4, &colHeader);
            thisv->unk_16A = 80;
            thisv->actionFunc = func_8087E10C;
            thisvx->uncullZoneScale = 3000.0f;
            thisvx->uncullZoneDownward = 3000.0f;
        }
    }

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void BgHakaMeganeBG_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaMeganeBG* thisv = (BgHakaMeganeBG*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_8087DFF8(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_16A != 0) {
        thisv->unk_16A--;
    }

    if (thisv->unk_16A == 0) {
        thisv->unk_16A = 40;
        thisv->dyna.actor.world.rot.y += 0x8000;
        thisv->actionFunc = func_8087E040;
    }
}

void func_8087E040(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    f32 xSub;

    if (thisv->unk_16A != 0) {
        thisv->unk_16A--;
    }

    xSub = (sinf(((thisv->unk_16A * 0.025f) + 0.5f) * std::numbers::pi_v<float>) + 1.0f) * 160.0f;

    if (thisv->dyna.actor.world.rot.y != thisv->dyna.actor.shape.rot.y) {
        xSub = 320.0f - xSub;
    }

    thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x - xSub;

    if (thisv->unk_16A == 0) {
        thisv->unk_16A = 20;
        thisv->actionFunc = func_8087DFF8;
    }
}

void func_8087E10C(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.velocity.y += 1.0f;

    if (thisv->dyna.actor.velocity.y > 20.0f) {
        thisv->dyna.actor.velocity.y = 20.0f;
    } else {
        thisv->dyna.actor.velocity.y = thisv->dyna.actor.velocity.y;
    }

    if (thisv->unk_16A != 0) {
        thisv->unk_16A--;
    }

    if (!Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y - 640.0f,
                      thisv->dyna.actor.velocity.y)) {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_CHINETRAP_DOWN - SFX_FLAG);
    }

    if (thisv->unk_16A == 0) {
        thisv->unk_16A = 120;
        thisv->actionFunc = func_8087E1E0;
        thisv->dyna.actor.velocity.y = 0.0f;
    }
}

void func_8087E1E0(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 16.0f / 3.0f);
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);

    if (thisv->unk_16A != 0) {
        thisv->unk_16A--;
    }

    if (thisv->unk_16A == 0) {
        thisv->unk_16A = 80;
        thisv->actionFunc = func_8087E10C;
    }
}

void func_8087E258(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.shape.rot.y += 0x180;
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void func_8087E288(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->unk_168)) {
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        thisv->actionFunc = func_8087E2D8;
    }
}

void func_8087E2D8(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->dyna.actor.speedXZ, 30.0f, 2.0f);

    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.speedXZ)) {
        Actor_SetFocus(&thisv->dyna.actor, 50.0f);
        thisv->actionFunc = func_8087E34C;
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
    }
}

void func_8087E34C(BgHakaMeganeBG* thisv, GlobalContext* globalCtx) {
}

void BgHakaMeganeBG_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaMeganeBG* thisv = (BgHakaMeganeBG*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgHakaMeganeBG_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaMeganeBG* thisv = (BgHakaMeganeBG*)thisvx;
    s16 params = thisv->dyna.actor.params;

    if (params == 0) {
        Gfx_DrawDListXlu(globalCtx, object_haka_objects_DL_008EB0);
    } else {
        Gfx_DrawDListOpa(globalCtx, D_8087E410[params]);
    }
}
