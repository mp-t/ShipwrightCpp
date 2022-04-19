/*
 * File: z_magic_wind.c
 * Overlay: ovl_Magic_Wind
 * Description: Farore's Wind
 */

#include "z_magic_wind.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void MagicWind_Init(Actor* thisvx, GlobalContext* globalCtx);
void MagicWind_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void MagicWind_Update(Actor* thisvx, GlobalContext* globalCtx);
void MagicWind_Draw(Actor* thisvx, GlobalContext* globalCtx);

void MagicWind_Shrink(MagicWind* thisv, GlobalContext* globalCtx);
void MagicWind_WaitForTimer(MagicWind* thisv, GlobalContext* globalCtx);
void MagicWind_FadeOut(MagicWind* thisv, GlobalContext* globalCtx);
void MagicWind_WaitAtFullSize(MagicWind* thisv, GlobalContext* globalCtx);
void MagicWind_Grow(MagicWind* thisv, GlobalContext* globalCtx);

ActorInit Magic_Wind_InitVars = {
    ACTOR_MAGIC_WIND,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(MagicWind),
    (ActorFunc)MagicWind_Init,
    (ActorFunc)MagicWind_Destroy,
    (ActorFunc)MagicWind_Update,
    (ActorFunc)MagicWind_Draw,
    NULL,
};

#include "overlays/ovl_Magic_Wind/ovl_Magic_Wind.h"

static u8 sAlphaUpdVals[] = {
    0x00, 0x03, 0x04, 0x07, 0x09, 0x0A, 0x0D, 0x0F, 0x11, 0x12, 0x15, 0x16, 0x19, 0x1B, 0x1C, 0x1F, 0x21, 0x23,
};

void MagicWind_SetupAction(MagicWind* thisv, MagicWindFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void MagicWind_Init(Actor* thisvx, GlobalContext* globalCtx) {
    MagicWind* thisv = (MagicWind*)thisvx;
    Player* player = GET_PLAYER(globalCtx);

    if (SkelCurve_Init(globalCtx, &thisv->skelCurve, &sSkel, &sAnim) == 0) {
        // "Magic_Wind_Actor_ct (): Construct failed"
        osSyncPrintf("Magic_Wind_Actor_ct():コンストラクト失敗\n");
    }
    thisv->actor.room = -1;
    switch (thisv->actor.params) {
        case 0:
            SkelCurve_SetAnim(&thisv->skelCurve, &sAnim, 0.0f, 60.0f, 0.0f, 1.0f);
            thisv->timer = 29;
            MagicWind_SetupAction(thisv, MagicWind_WaitForTimer);
            break;
        case 1:
            SkelCurve_SetAnim(&thisv->skelCurve, &sAnim, 60.0f, 0.0f, 60.0f, -1.0f);
            MagicWind_SetupAction(thisv, MagicWind_Shrink);
            // "Means start"
            LOG_STRING("表示開始", "../z_magic_wind.c", 486);
            func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_WIND_WARP);
            break;
    }
}

void MagicWind_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    MagicWind* thisv = (MagicWind*)thisvx;
    SkelCurve_Destroy(globalCtx, &thisv->skelCurve);
    func_800876C8(globalCtx);
    // "wipe out"
    LOG_STRING("消滅", "../z_magic_wind.c", 505);
}

void MagicWind_UpdateAlpha(f32 alpha) {
    s32 i;

    Vtx* vtx = ResourceMgr_LoadVtxByName(sCylinderVtx);

    for (i = 0; i < ARRAY_COUNT(sAlphaUpdVals); i++) {
        vtx[sAlphaUpdVals[i]].n.a = alpha * 255.0f;
    }
}

void MagicWind_WaitForTimer(MagicWind* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->timer > 0) {
        thisv->timer--;
        return;
    }

    // "Means start"
    LOG_STRING("表示開始", "../z_magic_wind.c", 539);
    func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_WIND_NORMAL);
    MagicWind_UpdateAlpha(1.0f);
    MagicWind_SetupAction(thisv, MagicWind_Grow);
    SkelCurve_Update(globalCtx, &thisv->skelCurve);
}

void MagicWind_Grow(MagicWind* thisv, GlobalContext* globalCtx) {
    if (SkelCurve_Update(globalCtx, &thisv->skelCurve)) {
        MagicWind_SetupAction(thisv, MagicWind_WaitAtFullSize);
        thisv->timer = 50;
    }
}

void MagicWind_WaitAtFullSize(MagicWind* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        MagicWind_SetupAction(thisv, MagicWind_FadeOut);
        thisv->timer = 30;
    }
}

void MagicWind_FadeOut(MagicWind* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        MagicWind_UpdateAlpha((f32)thisv->timer * (1.0f / 30.0f));
        thisv->timer--;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void MagicWind_Shrink(MagicWind* thisv, GlobalContext* globalCtx) {
    if (SkelCurve_Update(globalCtx, &thisv->skelCurve)) {
        Actor_Kill(&thisv->actor);
    }
}

void MagicWind_Update(Actor* thisvx, GlobalContext* globalCtx) {
    MagicWind* thisv = (MagicWind*)thisvx;
    if (globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK ||
        globalCtx->msgCtx.msgMode == MSGMODE_SONG_PLAYED) {
        Actor_Kill(thisvx);
        return;
    }

    thisv->actionFunc(thisv, globalCtx);
}

s32 MagicWind_OverrideLimbDraw(GlobalContext* globalCtx, SkelAnimeCurve* skelCurve, s32 limbIndex, void* thisvx) {
    MagicWind* thisv = (MagicWind*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_magic_wind.c", 615);

    if (limbIndex == 1) {
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->state.frames * 9) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0xF) & 0xFF), 0x40, 0x40, 1,
                                    (globalCtx->state.frames * 0xF) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0x1E) & 0xFF), 0x40, 0x40));

    } else if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 9,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->state.frames * 3) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 5) & 0xFF), 0x40, 0x40, 1,
                                    (globalCtx->state.frames * 6) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0xA) & 0xFF), 0x40, 0x40));
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_magic_wind.c", 646);

    return true;
}

void MagicWind_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MagicWind* thisv = (MagicWind*)thisvx;

    OPEN_DISPS(gfxCtx, "../z_magic_wind.c", 661);

    if (thisv->actionFunc != MagicWind_WaitForTimer) {
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 25);
        SkelCurve_Draw(thisvx, globalCtx, &thisv->skelCurve, MagicWind_OverrideLimbDraw, NULL, 1, NULL);
    }

    CLOSE_DISPS(gfxCtx, "../z_magic_wind.c", 673);
}
