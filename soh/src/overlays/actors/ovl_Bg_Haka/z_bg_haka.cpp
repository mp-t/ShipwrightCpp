/*
 * File: z_bg_haka.c
 * Overlay: ovl_Bg_Haka
 * Description: Gravestone
 */

#include "z_bg_haka.h"
#include "objects/object_haka/object_haka.h"

#define FLAGS 0

void BgHaka_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHaka_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHaka_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHaka_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8087B758(BgHaka* thisv, Player* player);
void func_8087B7E8(BgHaka* thisv, GlobalContext* globalCtx);
void func_8087B938(BgHaka* thisv, GlobalContext* globalCtx);
void func_8087BAAC(BgHaka* thisv, GlobalContext* globalCtx);
void func_8087BAE4(BgHaka* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Haka_InitVars = {
    ACTOR_BG_HAKA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA,
    sizeof(BgHaka),
    (ActorFunc)BgHaka_Init,
    (ActorFunc)BgHaka_Destroy,
    (ActorFunc)BgHaka_Update,
    (ActorFunc)BgHaka_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(minVelocityY, 0, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHaka_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHaka* thisv = (BgHaka*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gGravestoneCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->actionFunc = func_8087B7E8;
}

void BgHaka_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHaka* thisv = (BgHaka*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_8087B758(BgHaka* thisv, Player* player) {
    Vec3f sp1C;

    func_8002DBD0(&thisv->dyna.actor, &sp1C, &player->actor.world.pos);
    if (fabsf(sp1C.x) < 34.6f && sp1C.z > -112.8f && sp1C.z < -36.0f) {
        player->stateFlags2 |= 0x200;
    }
}

void func_8087B7E8(BgHaka* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 != 0.0f) {
        if (globalCtx->sceneNum == SCENE_SPOT02 && !LINK_IS_ADULT && IS_DAY) {
            thisv->dyna.unk_150 = 0.0f;
            player->stateFlags2 &= ~0x10;
            if (!Gameplay_InCsMode(globalCtx)) {
                Message_StartTextbox(globalCtx, 0x5073, NULL);
                thisv->dyna.actor.params = 100;
                thisv->actionFunc = func_8087BAE4;
            }
        } else if (0.0f < thisv->dyna.unk_150 ||
                   (globalCtx->sceneNum == SCENE_SPOT06 && !LINK_IS_ADULT && !Flags_GetSwitch(globalCtx, 0x23))) {
            thisv->dyna.unk_150 = 0.0f;
            player->stateFlags2 &= ~0x10;
        } else {
            thisv->dyna.actor.world.rot.y = thisv->dyna.actor.shape.rot.y + 0x8000;
            thisv->actionFunc = func_8087B938;
        }
    }
    func_8087B758(thisv, player);
}

void func_8087B938(BgHaka* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 sp38;

    thisv->dyna.actor.speedXZ += 0.05f;
    thisv->dyna.actor.speedXZ = CLAMP_MAX(thisv->dyna.actor.speedXZ, 1.5f);
    sp38 = Math_StepToF(&thisv->dyna.actor.minVelocityY, 60.0f, thisv->dyna.actor.speedXZ);
    thisv->dyna.actor.world.pos.x =
        Math_SinS(thisv->dyna.actor.world.rot.y) * thisv->dyna.actor.minVelocityY + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z =
        Math_CosS(thisv->dyna.actor.world.rot.y) * thisv->dyna.actor.minVelocityY + thisv->dyna.actor.home.pos.z;
    if (sp38 != 0) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
        if (thisv->dyna.actor.params == 1) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        } else if (!IS_DAY && globalCtx->sceneNum == SCENE_SPOT02) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_POH, thisv->dyna.actor.home.pos.x,
                        thisv->dyna.actor.home.pos.y, thisv->dyna.actor.home.pos.z, 0, thisv->dyna.actor.shape.rot.y, 0,
                        1);
        }
        thisv->actionFunc = func_8087BAAC;
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void func_8087BAAC(BgHaka* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 != 0.0f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
}

void func_8087BAE4(BgHaka* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    if (thisv->dyna.actor.params != 0) {
        thisv->dyna.actor.params -= 1;
    }
    if (thisv->dyna.unk_150 != 0.0f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
    if (thisv->dyna.actor.params == 0) {
        thisv->actionFunc = func_8087B7E8;
    }
    func_8087B758(thisv, player);
}

void BgHaka_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHaka* thisv = (BgHaka*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgHaka_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka.c", 401);

    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka.c", 406),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGravestoneStoneDL);
    Matrix_Translate(0.0f, 0.0f, thisx->minVelocityY * 10.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka.c", 416),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGravestoneEarthDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka.c", 421);
}
