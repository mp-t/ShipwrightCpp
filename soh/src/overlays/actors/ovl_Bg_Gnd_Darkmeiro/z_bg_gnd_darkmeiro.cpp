/*
 * File: z_bg_gnd_darkmeiro.c
 * Overlay: ovl_Bg_Gnd_Darkmeiro
 * Description: Shadow trial actors (invisible path, clear block, and timer)
 */

#include "z_bg_gnd_darkmeiro.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgGndDarkmeiro_Init(Actor* thisx, GlobalContext* globalCtx);
void BgGndDarkmeiro_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgGndDarkmeiro_Update(Actor* thisx, GlobalContext* globalCtx);
void BgGndDarkmeiro_DrawInvisiblePath(Actor* thisx, GlobalContext* globalCtx);
void BgGndDarkmeiro_DrawSwitchBlock(Actor* thisx, GlobalContext* globalCtx);
void BgGndDarkmeiro_DrawStaticBlock(Actor* thisx, GlobalContext* globalCtx);

void BgGndDarkmeiro_Noop(BgGndDarkmeiro* thisv, GlobalContext* globalCtx);
void BgGndDarkmeiro_UpdateBlockTimer(BgGndDarkmeiro* thisv, GlobalContext* globalCtx);
void BgGndDarkmeiro_UpdateStaticBlock(BgGndDarkmeiro* thisv, GlobalContext* globalCtx);
void BgGndDarkmeiro_UpdateSwitchBlock(BgGndDarkmeiro* thisv, GlobalContext* globalCtx);

ActorInit Bg_Gnd_Darkmeiro_InitVars = {
    ACTOR_BG_GND_DARKMEIRO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndDarkmeiro),
    (ActorFunc)BgGndDarkmeiro_Init,
    (ActorFunc)BgGndDarkmeiro_Destroy,
    (ActorFunc)BgGndDarkmeiro_Update,
    NULL,
    NULL,
};

void BgGndDarkmeiro_ToggleBlock(BgGndDarkmeiro* thisv, GlobalContext* globalCtx) {
    if (thisv->actionFlags & 2) {
        if (thisv->timer1 == 0) {
            func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            thisv->actionFlags &= ~2;
        }
    } else if (thisv->timer1 != 0) {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        thisv->actionFlags |= 2;
    }
}

void BgGndDarkmeiro_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    const CollisionHeader* colHeader = NULL;
    BgGndDarkmeiro* thisv = (BgGndDarkmeiro*)thisx;

    thisv->updateFunc = BgGndDarkmeiro_Noop;
    Actor_SetScale(&thisv->dyna.actor, 0.1f);
    switch (thisv->dyna.actor.params & 0xFF) {
        case DARKMEIRO_INVISIBLE_PATH:
            thisv->dyna.actor.draw = BgGndDarkmeiro_DrawInvisiblePath;
            thisv->dyna.actor.flags |= ACTOR_FLAG_7;
            break;
        case DARKMEIRO_CLEAR_BLOCK:
            CollisionHeader_GetVirtual(&gClearBlockCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
            if (((thisv->dyna.actor.params >> 8) & 0x3F) == 0x3F) {
                thisv->updateFunc = BgGndDarkmeiro_UpdateStaticBlock;
                thisv->dyna.actor.draw = BgGndDarkmeiro_DrawStaticBlock;
            } else {
                thisv->actionFlags = thisv->timer1 = thisv->timer2 = 0;
                thisx->draw = BgGndDarkmeiro_DrawSwitchBlock;
                thisv->updateFunc = BgGndDarkmeiro_UpdateSwitchBlock;
                if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
                    func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
                } else {
                    thisv->timer1 = 64;
                    thisv->actionFlags |= 2;
                }
            }
            break;
        case DARKMEIRO_BLOCK_TIMER:
            thisv->actionFlags = thisv->timer1 = thisv->timer2 = 0;
            thisv->updateFunc = BgGndDarkmeiro_UpdateBlockTimer;
            thisx->draw = NULL;
            if (Flags_GetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 1)) {
                thisv->timer1 = 64;
                thisv->actionFlags |= 4;
            }
            if (Flags_GetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 2)) {
                thisv->timer2 = 64;
                thisv->actionFlags |= 8;
            }
            if ((thisv->timer1 != 0) || (thisv->timer2 != 0)) {
                Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
            } else {
                Flags_UnsetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
            }
            break;
    }
}

void BgGndDarkmeiro_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgGndDarkmeiro* thisv = (BgGndDarkmeiro*)thisx;

    if ((thisv->dyna.actor.params & 0xFF) == 1) {
        if (1) {}
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void BgGndDarkmeiro_Noop(BgGndDarkmeiro* thisv, GlobalContext* globalCtx) {
}

void BgGndDarkmeiro_UpdateBlockTimer(BgGndDarkmeiro* thisv, GlobalContext* globalCtx) {
    s16 timeLeft;

    if (Flags_GetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 1)) {
        if (thisv->actionFlags & 4) {
            if (thisv->timer1 > 0) {
                thisv->timer1--;
            } else {
                Flags_UnsetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 1);
                thisv->actionFlags &= ~4;
            }
        } else {
            thisv->actionFlags |= 4;
            thisv->timer1 = 304;
            Audio_PlaySoundGeneral(NA_SE_EV_RED_EYE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (Flags_GetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 2)) {
        if (thisv->actionFlags & 8) {
            if (thisv->timer2 > 0) {
                thisv->timer2--;
            } else {
                Flags_UnsetSwitch(globalCtx, ((thisv->dyna.actor.params >> 8) & 0x3F) + 2);
                thisv->actionFlags &= ~8;
            }
        } else {
            thisv->actionFlags |= 8;
            thisv->timer2 = 304;
            Audio_PlaySoundGeneral(NA_SE_EV_RED_EYE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    timeLeft = CLAMP_MIN(thisv->timer1, thisv->timer2);
    if (timeLeft > 0) {
        func_8002F994(&thisv->dyna.actor, timeLeft);
    }
    if ((thisv->timer1 >= 64) || (thisv->timer2 >= 64)) {
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
    } else {
        Flags_UnsetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
    }
}

void BgGndDarkmeiro_UpdateStaticBlock(BgGndDarkmeiro* thisv, GlobalContext* globalCtx) {
}

void BgGndDarkmeiro_UpdateSwitchBlock(BgGndDarkmeiro* thisv, GlobalContext* globalCtx) {
    if (thisv->timer1 > 0) {
        thisv->timer1--;
    }

    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        thisv->timer1 = 64;
    }

    BgGndDarkmeiro_ToggleBlock(thisv, globalCtx);
}

void BgGndDarkmeiro_Update(Actor* thisx, GlobalContext* globalCtx2) {
    BgGndDarkmeiro* thisv = (BgGndDarkmeiro*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    thisv->updateFunc(thisv, globalCtx2);
}

void BgGndDarkmeiro_DrawInvisiblePath(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListXlu(globalCtx, gShadowTrialPathDL);
}

void BgGndDarkmeiro_DrawSwitchBlock(Actor* thisx, GlobalContext* globalCtx) {
    BgGndDarkmeiro* thisv = (BgGndDarkmeiro*)thisx;
    s16 vanishTimer;

    vanishTimer = thisv->timer1;
    if (vanishTimer != 0) {
        if (vanishTimer > 64) {
            thisv->timer2 = (thisv->timer2 < 120) ? thisv->timer2 + 8 : 127;
        } else if (vanishTimer > 16) {
            thisv->timer2 = (Math_CosS((u16)thisv->timer1 * 0x1000) * 64.0f) + 127.0f;
            if (thisv->timer2 > 127) {
                thisv->timer2 = 127;
            }
        } else {
            thisv->timer2 = vanishTimer * 8;
        }

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 378);
        //! @bug Due to a bug in the display list, the transparency data is not used.
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, thisv->timer2);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 380);

        Gfx_DrawDListXlu(globalCtx, gClearBlockDL);
    }
}

void BgGndDarkmeiro_DrawStaticBlock(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 391);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 198, 202, 208, 255);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_darkmeiro.c", 393);

    Gfx_DrawDListXlu(globalCtx, gClearBlockDL);
}
