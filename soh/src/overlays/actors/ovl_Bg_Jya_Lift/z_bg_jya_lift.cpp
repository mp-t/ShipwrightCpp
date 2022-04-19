/*
 * File: z_bg_jya_lift.c
 * Overlay: ovl_Bg_Jya_Lift
 * Description: Chain Platform (Spirit Temple)
 */

#include "z_bg_jya_lift.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaLift_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaLift_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaLift_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaLift_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJyaLift_SetFinalPosY(BgJyaLift* thisv);
void BgJyaLift_SetInitPosY(BgJyaLift* thisv);
void BgJyaLift_DelayMove(BgJyaLift* thisv, GlobalContext* globalCtx);
void BgJyaLift_SetupMove(BgJyaLift* thisv);
void BgJyaLift_Move(BgJyaLift* thisv, GlobalContext* globalCtx);

static s16 sIsSpawned = false;

ActorInit Bg_Jya_Lift_InitVars = {
    ACTOR_BG_JYA_LIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaLift),
    (ActorFunc)BgJyaLift_Init,
    (ActorFunc)BgJyaLift_Destroy,
    (ActorFunc)BgJyaLift_Update,
    (ActorFunc)BgJyaLift_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2500, ICHAIN_STOP),
};

void BgJyaLift_InitDynapoly(BgJyaLift* thisv, GlobalContext* globalCtx, const CollisionHeader* collisionHeader, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collisionHeader, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void BgJyaLift_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaLift* thisv = (BgJyaLift*)thisx;

    thisv->isSpawned = false;
    if (sIsSpawned) {
        Actor_Kill(thisx);
        return;
    }

    // "Goddess lift CT"
    osSyncPrintf("女神リフト CT\n");
    BgJyaLift_InitDynapoly(thisv, globalCtx, &gLiftCol, DPM_UNK);
    Actor_ProcessInitChain(thisx, sInitChain);
    if (Flags_GetSwitch(globalCtx, (thisx->params & 0x3F))) {
        BgJyaLift_SetFinalPosY(thisv);
    } else {
        BgJyaLift_SetInitPosY(thisv);
    }
    thisx->room = -1;
    sIsSpawned = true;
    thisv->isSpawned = true;
}

void BgJyaLift_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaLift* thisv = (BgJyaLift*)thisx;

    if (thisv->isSpawned) {

        // "Goddess Lift DT"
        osSyncPrintf("女神リフト DT\n");
        sIsSpawned = false;
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void BgJyaLift_SetInitPosY(BgJyaLift* thisv) {
    thisv->actionFunc = BgJyaLift_DelayMove;
    thisv->dyna.actor.world.pos.y = 1613.0f;
    thisv->moveDelay = 0;
}

void BgJyaLift_DelayMove(BgJyaLift* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F) || (thisv->moveDelay > 0)) {
        thisv->moveDelay++;
        if (thisv->moveDelay >= 20) {
            OnePointCutscene_Init(globalCtx, 3430, -99, &thisv->dyna.actor, MAIN_CAM);
            BgJyaLift_SetupMove(thisv);
        }
    }
}

void BgJyaLift_SetupMove(BgJyaLift* thisv) {
    thisv->actionFunc = BgJyaLift_Move;
}

void BgJyaLift_Move(BgJyaLift* thisv, GlobalContext* globalCtx) {
    f32 distFromBottom;
    f32 tempVelocity;

    Math_SmoothStepToF(&thisv->dyna.actor.velocity.y, 4.0f, 0.1f, 1.0f, 0.0f);
    tempVelocity = (thisv->dyna.actor.velocity.y < 0.2f) ? 0.2f : thisv->dyna.actor.velocity.y;
    distFromBottom = Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, 973.0f, 0.1f, tempVelocity, 0.2f);
    if ((thisv->dyna.actor.world.pos.y < 1440.0f) && (1440.0f <= thisv->dyna.actor.prevPos.y)) {
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
    }
    if (fabsf(distFromBottom) < 0.001f) {
        BgJyaLift_SetFinalPosY(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_STOP);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_BRIDGE_OPEN - SFX_FLAG);
    }
}

void BgJyaLift_SetFinalPosY(BgJyaLift* thisv) {
    thisv->actionFunc = NULL;
    thisv->dyna.actor.world.pos.y = 973.0f;
}

void BgJyaLift_Update(Actor* thisx, GlobalContext* globalCtx2) {
    BgJyaLift* thisv = (BgJyaLift*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
    if ((thisv->dyna.unk_160 & 4) && ((thisv->unk_16B & 4) == 0)) {
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DIRECTED_YAW);
    } else if (((thisv->dyna.unk_160) & 4) == 0 && ((thisv->unk_16B & 4)) &&
               (globalCtx->cameraPtrs[MAIN_CAM]->setting == CAM_SET_DIRECTED_YAW)) {
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
    }
    thisv->unk_16B = thisv->dyna.unk_160;

    // Spirit Temple room 5 is the main room with the statue room 25 is directly above room 5
    if ((globalCtx->roomCtx.curRoom.num != 5) && (globalCtx->roomCtx.curRoom.num != 25)) {
        Actor_Kill(thisx);
    }
}

void BgJyaLift_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gLiftDL);
}
