/*
 * File: z_bg_zg.c
 * Overlay: ovl_Bg_Zg
 * Description: Metal bars (Ganon's Castle)
 */

#include "z_bg_zg.h"
#include "objects/object_zg/object_zg.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgZg_Init(Actor* thisx, GlobalContext* globalCtx);
void BgZg_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgZg_Update(Actor* thisx, GlobalContext* globalCtx);
void BgZg_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808C0C50(BgZg* thisv);
s32 func_808C0C98(BgZg* thisv, GlobalContext* globalCtx);
s32 func_808C0CC8(BgZg* thisv);
void func_808C0CD4(BgZg* thisv, GlobalContext* globalCtx);
void func_808C0D08(BgZg* thisv, GlobalContext* globalCtx);
void func_808C0EEC(BgZg* thisv, GlobalContext* globalCtx);

static BgZgActionFunc sActionFuncs[] = {
    func_808C0CD4,
    func_808C0D08,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static BgZgDrawFunc sDrawFuncs[] = {
    func_808C0EEC,
};

ActorInit Bg_Zg_InitVars = {
    ACTOR_BG_ZG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZG,
    sizeof(BgZg),
    (ActorFunc)BgZg_Init,
    (ActorFunc)BgZg_Destroy,
    (ActorFunc)BgZg_Update,
    (ActorFunc)BgZg_Draw,
    NULL,
};

void BgZg_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgZg* thisv = (BgZg*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808C0C50(BgZg* thisv) {
    Audio_PlaySoundGeneral(NA_SE_EV_METALDOOR_OPEN, &thisv->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
}

s32 func_808C0C98(BgZg* thisv, GlobalContext* globalCtx) {
    s32 flag = (thisv->dyna.actor.params >> 8) & 0xFF;

    return Flags_GetSwitch(globalCtx, flag);
}

s32 func_808C0CC8(BgZg* thisv) {
    s32 flag = thisv->dyna.actor.params & 0xFF;

    return flag;
}

void func_808C0CD4(BgZg* thisv, GlobalContext* globalCtx) {
    if (func_808C0C98(thisv, globalCtx) != 0) {
        thisv->action = 1;
        func_808C0C50(thisv);
    }
}

void func_808C0D08(BgZg* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.world.pos.y += (kREG(16) + 20.0f) * 1.2f;
    if ((((kREG(17) + 200.0f) * 1.2f) + thisv->dyna.actor.home.pos.y) <= thisv->dyna.actor.world.pos.y) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgZg_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgZg* thisv = (BgZg*)thisx;
    s32 action = thisv->action;

    if (((action < 0) || (1 < action)) || (sActionFuncs[action] == NULL)) {
        // "Main Mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[action](thisv, globalCtx);
    }
}

void BgZg_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad[2];
    BgZg* thisv = (BgZg*)thisx;
    const CollisionHeader* colHeader;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    colHeader = NULL;
    CollisionHeader_GetVirtual(&gTowerCollapseBarsCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if ((func_808C0CC8(thisv) == 8) || (func_808C0CC8(thisv) == 9)) {
        thisv->dyna.actor.scale.x = thisv->dyna.actor.scale.x * 1.3f;
        thisv->dyna.actor.scale.z = thisv->dyna.actor.scale.z * 1.3f;
        thisv->dyna.actor.scale.y = thisv->dyna.actor.scale.y * 1.2f;
    }

    thisv->action = 0;
    thisv->drawConfig = 0;
    if (func_808C0C98(thisv, globalCtx)) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void func_808C0EEC(BgZg* thisv, GlobalContext* globalCtx) {
    GraphicsContext* localGfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(localGfxCtx, "../z_bg_zg.c", 311);

    func_80093D18(localGfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(localGfxCtx, "../z_bg_zg.c", 315),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseBarsDL);

    CLOSE_DISPS(localGfxCtx, "../z_bg_zg.c", 320);
}

void BgZg_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgZg* thisv = (BgZg*)thisx;
    s32 drawConfig = thisv->drawConfig;

    if (((drawConfig < 0) || (drawConfig > 0)) || sDrawFuncs[drawConfig] == NULL) {
        // "Drawing mode is wrong !!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sDrawFuncs[drawConfig](thisv, globalCtx);
    }
}
