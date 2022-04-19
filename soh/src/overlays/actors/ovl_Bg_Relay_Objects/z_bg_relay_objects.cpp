/*
 * File: z_bg_relay_objects.c
 * Overlay: ovl_Bg_Relay_Objects
 * Description: Windmill Setpieces
 */

#include "z_bg_relay_objects.h"
#include "objects/object_relay_objects/object_relay_objects.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0 */ WINDMILL_ROTATING_GEAR,
    /* 1 */ WINDMILL_DAMPE_STONE_DOOR
} WindmillSetpiecesMode;

void BgRelayObjects_Init(Actor* thisx, GlobalContext* globalCtx);
void BgRelayObjects_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgRelayObjects_Update(Actor* thisx, GlobalContext* globalCtx);
void BgRelayObjects_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgRelayObjects_Reset(void);

void func_808A90F4(BgRelayObjects* thisv, GlobalContext* globalCtx);
void func_808A91AC(BgRelayObjects* thisv, GlobalContext* globalCtx);
void func_808A9234(BgRelayObjects* thisv, GlobalContext* globalCtx);
void BgRelayObjects_DoNothing(BgRelayObjects* thisv, GlobalContext* globalCtx);
void func_808A932C(BgRelayObjects* thisv, GlobalContext* globalCtx);
void func_808A939C(BgRelayObjects* thisv, GlobalContext* globalCtx);

ActorInit Bg_Relay_Objects_InitVars = {
    ACTOR_BG_RELAY_OBJECTS,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_RELAY_OBJECTS,
    sizeof(BgRelayObjects),
    (ActorFunc)BgRelayObjects_Init,
    (ActorFunc)BgRelayObjects_Destroy,
    (ActorFunc)BgRelayObjects_Update,
    (ActorFunc)BgRelayObjects_Draw,
    BgRelayObjects_Reset,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(gravity, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static u32 D_808A9508 = 0;
void BgRelayObjects_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgRelayObjects* thisv = (BgRelayObjects*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisv->switchFlag = thisx->params & 0x3F;
    thisx->params = (thisx->params >> 8) & 0xFF;
    DynaPolyActor_Init(&thisv->dyna, 3);
    if (thisx->params == WINDMILL_ROTATING_GEAR) {
        CollisionHeader_GetVirtual(&gWindmillRotatingPlatformCol, &colHeader);
        if (gSaveContext.eventChkInf[6] & 0x20) {
            thisx->world.rot.y = 0x400;
        } else {
            thisx->world.rot.y = 0x80;
        }
        func_800F5718();
        thisx->room = -1;
        thisx->flags |= ACTOR_FLAG_5;
        if (D_808A9508 & 2) {
            thisx->params = 0xFF;
            Actor_Kill(thisx);
        } else {
            D_808A9508 |= 2;
            thisv->actionFunc = func_808A939C;
        }
    } else {
        CollisionHeader_GetVirtual(&gDampeRaceDoorCol, &colHeader);
        if (thisx->room == 0) {
            thisv->unk_169 = thisv->switchFlag - 0x33;
        } else {
            thisv->unk_169 = thisx->room + 1;
        }
        thisx->room = -1;
        thisv->timer = 1;
        if (thisv->unk_169 >= 6) {
            if (D_808A9508 & 1) {
                Actor_Kill(thisx);
            } else {
                D_808A9508 |= 1;
                thisv->actionFunc = BgRelayObjects_DoNothing;
            }
        } else if (thisv->unk_169 != 5) {
            Flags_UnsetSwitch(globalCtx, thisv->switchFlag);
            if (D_808A9508 & (1 << thisv->unk_169)) {
                Actor_Kill(thisx);
            } else {
                D_808A9508 |= (1 << thisv->unk_169);
                thisv->actionFunc = func_808A90F4;
            }
        } else {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
            thisv->actionFunc = func_808A91AC;
            thisx->world.pos.y += 120.0f;
            D_808A9508 |= 1;
        }
    }
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
}

void BgRelayObjects_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgRelayObjects* thisv = (BgRelayObjects*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    if ((thisv->dyna.actor.params == WINDMILL_ROTATING_GEAR) && (gSaveContext.cutsceneIndex < 0xFFF0)) {
        gSaveContext.eventChkInf[6] &= ~0x20;
    }
}

void func_808A90F4(BgRelayObjects* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if (thisv->timer != 0) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
            if (INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE) {
                thisv->timer = 120;
            } else {
                thisv->timer = 160;
            }
        }
        if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 120.0f, 12.0f)) {
            thisv->actionFunc = func_808A91AC;
        }
    }
}

void func_808A91AC(BgRelayObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_169 != 5) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
        func_8002F994(&thisv->dyna.actor, thisv->timer);
    }
    if ((thisv->timer == 0) || (thisv->unk_169 == globalCtx->roomCtx.curRoom.num)) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
        thisv->actionFunc = func_808A9234;
    }
}

void func_808A9234(BgRelayObjects* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.velocity.y += thisv->dyna.actor.gravity;
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.velocity.y)) {
        func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 180, 20, 100);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONE_BOUND);
        if (thisv->unk_169 != globalCtx->roomCtx.curRoom.num) {
            func_800788CC(NA_SE_EN_PO_LAUGH);
            thisv->timer = 5;
            thisv->actionFunc = func_808A932C;
            return;
        }
        Flags_UnsetSwitch(globalCtx, thisv->switchFlag);
        thisv->dyna.actor.flags &= ~ACTOR_FLAG_4;
        if (globalCtx->roomCtx.curRoom.num == 4) {
            gSaveContext.timer1State = 0xF;
        }
        thisv->actionFunc = BgRelayObjects_DoNothing;
    }
}

void BgRelayObjects_DoNothing(BgRelayObjects* thisv, GlobalContext* globalCtx) {
}

void func_808A932C(BgRelayObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        if (!Player_InCsMode(globalCtx)) {
            func_80078884(NA_SE_OC_ABYSS);
            Gameplay_TriggerRespawn(globalCtx);
            thisv->actionFunc = BgRelayObjects_DoNothing;
        }
    }
}

void func_808A939C(BgRelayObjects* thisv, GlobalContext* globalCtx) {
    if (Flags_GetEnv(globalCtx, 5)) {
        gSaveContext.eventChkInf[6] |= 0x20;
    }
    if (gSaveContext.eventChkInf[6] & 0x20) {
        Math_ScaledStepToS(&thisv->dyna.actor.world.rot.y, 0x400, 8);
    } else {
        Math_ScaledStepToS(&thisv->dyna.actor.world.rot.y, 0x80, 8);
    }
    thisv->dyna.actor.shape.rot.y += thisv->dyna.actor.world.rot.y;
    func_800F436C(&thisv->dyna.actor.projectedPos, NA_SE_EV_WOOD_GEAR - SFX_FLAG,
                  ((thisv->dyna.actor.world.rot.y - 0x80) * (1.0f / 0x380)) + 1.0f);
}

void BgRelayObjects_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgRelayObjects* thisv = (BgRelayObjects*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgRelayObjects_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgRelayObjects* thisv = (BgRelayObjects*)thisx;

    if (thisv->dyna.actor.params == WINDMILL_ROTATING_GEAR) {
        Gfx_DrawDListOpa(globalCtx, gWindmillRotatingPlatformDL);
    } else {
        Gfx_DrawDListOpa(globalCtx, gDampeRaceDoorDL);
    }
}

void BgRelayObjects_Reset(void) {
    D_808A9508 = 0;
}
