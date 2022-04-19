/*
 * File: z_bg_jya_1flift.c
 * Overlay: ovl_Bg_Jya_1flift
 * Description: Shortcut Elevator used in the vanilla version of the Spirit Temple
 */

#include "z_bg_jya_1flift.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS ACTOR_FLAG_4

void BgJya1flift_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJya1flift_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJya1flift_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJya1flift_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJya1flift_SetupWaitForSwitch(BgJya1flift* thisv);
void BgJya1flift_WaitForSwitch(BgJya1flift* thisv, GlobalContext* globalCtx);
void BgJya1flift_DoNothing(BgJya1flift* thisv, GlobalContext* globalCtx);
void BgJya1flift_ChangeDirection(BgJya1flift* thisv);
void BgJya1flift_Move(BgJya1flift* thisv, GlobalContext* globalCtx);
void BgJya1flift_SetupDoNothing(BgJya1flift* thisv);
void BgJya1flift_ResetMoveDelay(BgJya1flift* thisv);
void BgJya1flift_DelayMove(BgJya1flift* thisv, GlobalContext* globalCtx);

static u8 sIsSpawned = false;

ActorInit Bg_Jya_1flift_InitVars = {
    ACTOR_BG_JYA_1FLIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJya1flift),
    (ActorFunc)BgJya1flift_Init,
    (ActorFunc)BgJya1flift_Destroy,
    (ActorFunc)BgJya1flift_Update,
    (ActorFunc)BgJya1flift_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 70, 80, -82, { 0, 0, 0 } },
};

static f32 sFinalPositions[] = { 443.0f, -50.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

void BgJya1flift_InitDynapoly(BgJya1flift* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_1flift.c", 179,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgJya1flift_InitCollision(Actor* thisx, GlobalContext* globalCtx) {
    BgJya1flift* thisv = (BgJya1flift*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->dyna.actor, &sCylinderInit);
    thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void BgJya1flift_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJya1flift* thisv = (BgJya1flift*)thisx;
    // "1 F lift"
    osSyncPrintf("(１Ｆリフト)(flag %d)(room %d)\n", sIsSpawned, globalCtx->roomCtx.curRoom.num);
    thisv->hasInitialized = false;
    if (sIsSpawned) {
        Actor_Kill(thisx);
        return;
    }
    BgJya1flift_InitDynapoly(thisv, globalCtx, &g1fliftCol, 0);
    Actor_ProcessInitChain(thisx, sInitChain);
    BgJya1flift_InitCollision(thisx, globalCtx);
    if (Flags_GetSwitch(globalCtx, (thisx->params & 0x3F))) {
        LINK_AGE_IN_YEARS == YEARS_ADULT ? BgJya1flift_ChangeDirection(thisv) : BgJya1flift_SetupDoNothing(thisv);
    } else {
        BgJya1flift_SetupWaitForSwitch(thisv);
    }
    thisx->room = -1;
    sIsSpawned = true;
    thisv->hasInitialized = true;
}

void BgJya1flift_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJya1flift* thisv = (BgJya1flift*)thisx;

    if (thisv->hasInitialized) {
        sIsSpawned = false;
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void BgJya1flift_SetupWaitForSwitch(BgJya1flift* thisv) {
    thisv->actionFunc = BgJya1flift_WaitForSwitch;
    thisv->dyna.actor.world.pos.y = sFinalPositions[0];
}

void BgJya1flift_WaitForSwitch(BgJya1flift* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params & 0x3F))) {
        BgJya1flift_ChangeDirection(thisv);
    }
}

void BgJya1flift_SetupDoNothing(BgJya1flift* thisv) {
    thisv->actionFunc = BgJya1flift_DoNothing;
    thisv->dyna.actor.world.pos.y = sFinalPositions[0];
}

void BgJya1flift_DoNothing(BgJya1flift* thisv, GlobalContext* globalCtx) {
}

void BgJya1flift_ChangeDirection(BgJya1flift* thisv) {
    thisv->actionFunc = BgJya1flift_Move;
    thisv->isMovingDown ^= true;
    thisv->dyna.actor.velocity.y = 0.0f;
}

void BgJya1flift_Move(BgJya1flift* thisv, GlobalContext* globalCtx) {
    f32 tempVelocity;

    Math_StepToF(&thisv->dyna.actor.velocity.y, 6.0f, 0.4f);
    if (thisv->dyna.actor.velocity.y < 1.0f) {
        tempVelocity = 1.0f;
    } else {
        tempVelocity = thisv->dyna.actor.velocity.y;
    }
    if (fabsf(Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, (sFinalPositions[thisv->isMovingDown]), 0.5f,
                                 tempVelocity, 1.0f)) < 0.001f) {
        thisv->dyna.actor.world.pos.y = sFinalPositions[thisv->isMovingDown];
        BgJya1flift_ResetMoveDelay(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void BgJya1flift_ResetMoveDelay(BgJya1flift* thisv) {
    thisv->actionFunc = BgJya1flift_DelayMove;
    thisv->moveDelay = 0;
}

void BgJya1flift_DelayMove(BgJya1flift* thisv, GlobalContext* globalCtx) {
    thisv->moveDelay++;
    if (thisv->moveDelay >= 21) {
        BgJya1flift_ChangeDirection(thisv);
    }
}

void BgJya1flift_Update(Actor* thisx, GlobalContext* globalCtx2) {
    BgJya1flift* thisv = (BgJya1flift*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s32 tempIsRiding;

    // Room 0 is the first room and 6 is the room that the lift starts on
    if (globalCtx->roomCtx.curRoom.num == 6 || globalCtx->roomCtx.curRoom.num == 0) {
        thisv->actionFunc(thisv, globalCtx);
        tempIsRiding = func_8004356C(&thisv->dyna) ? true : false;
        if ((thisv->actionFunc == BgJya1flift_Move) || (thisv->actionFunc == BgJya1flift_DelayMove)) {
            if (tempIsRiding) {
                Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
            } else if (!tempIsRiding && thisv->isLinkRiding) {
                Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            }
        }
        thisv->isLinkRiding = tempIsRiding;
        Collider_UpdateCylinder(thisx, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    } else {
        Actor_Kill(thisx);
    }
}

void BgJya1flift_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, g1fliftDL);
}
