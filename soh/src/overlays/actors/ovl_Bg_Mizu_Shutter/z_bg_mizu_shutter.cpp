#include "z_bg_mizu_shutter.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_4

#define SIZE_PARAM (((u16)thisv->dyna.actor.params >> 0xC) & 0xF)
#define TIMER_PARAM (((u16)thisv->dyna.actor.params >> 6) & 0x3F)

void BgMizuShutter_Init(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_Destroy(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_Update(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_Draw(BgMizuShutter* thisv, GlobalContext* globalCtx);

void BgMizuShutter_WaitForTimer(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_WaitForSwitch(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_Move(BgMizuShutter* thisv, GlobalContext* globalCtx);
void BgMizuShutter_WaitForCutscene(BgMizuShutter* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Mizu_Shutter_InitVars = {
    ACTOR_BG_MIZU_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuShutter),
    (ActorFunc)BgMizuShutter_Init,
    (ActorFunc)BgMizuShutter_Destroy,
    (ActorFunc)BgMizuShutter_Update,
    (ActorFunc)BgMizuShutter_Draw,
    NULL,
};

static const Gfx* sDisplayLists[] = { gObjectMizuObjectsShutterDL_007130, gObjectMizuObjectsShutterDL_0072D0 };

static const CollisionHeader* sCollisionHeaders[] = {
    &gObjectMizuObjectsShutterCol_007250,
    &gObjectMizuObjectsShutterCol_0073F0,
};

static Vec3f sDisplacements[] = {
    { 0.0f, 100.0f, 0.0f },
    { 0.0f, 140.0f, 0.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMizuShutter_Init(BgMizuShutter* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMizuShutter* thisv = (BgMizuShutter*)thisvx;
    s32 pad2;
    const CollisionHeader* sp30 = NULL;
    s32 pad3;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->displayList = sDisplayLists[SIZE_PARAM];
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(sCollisionHeaders[SIZE_PARAM], &sp30);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, sp30);
    if ((SIZE_PARAM == BGMIZUSHUTTER_SMALL) || (SIZE_PARAM == BGMIZUSHUTTER_LARGE)) {
        thisv->closedPos = thisv->dyna.actor.world.pos;
        thisv->timer = 0;
        thisv->timerMax = TIMER_PARAM * 20;
        Matrix_RotateY(thisv->dyna.actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
        Matrix_RotateX(thisv->dyna.actor.world.rot.x * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_RotateZ(thisv->dyna.actor.world.rot.z * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_MultVec3f(&sDisplacements[SIZE_PARAM], &thisv->openPos);
        thisv->openPos.x += thisv->dyna.actor.world.pos.x;
        thisv->openPos.y += thisv->dyna.actor.world.pos.y;
        thisv->openPos.z += thisv->dyna.actor.world.pos.z;
        if (thisv->timerMax != 0x3F * 20) {
            Flags_UnsetSwitch(globalCtx, (u16)thisv->dyna.actor.params & 0x3F);
            thisv->dyna.actor.world.pos = thisv->closedPos;
        }
        if (Flags_GetSwitch(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
            thisv->dyna.actor.world.pos = thisv->openPos;
            thisv->actionFunc = BgMizuShutter_WaitForTimer;
        } else {
            thisv->actionFunc = BgMizuShutter_WaitForSwitch;
        }
    }
}

void BgMizuShutter_Destroy(BgMizuShutter* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMizuShutter* thisv = (BgMizuShutter*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgMizuShutter_WaitForSwitch(BgMizuShutter* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
        if (ABS(thisv->dyna.actor.world.rot.x) > 0x2C60) {
            OnePointCutscene_Init(globalCtx, 4510, -99, &thisv->dyna.actor, MAIN_CAM);
        } else {
            OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        }
        thisv->actionFunc = BgMizuShutter_WaitForCutscene;
        thisv->timer = 30;
    }
}

void BgMizuShutter_WaitForCutscene(BgMizuShutter* thisv, GlobalContext* globalCtx) {
    if (thisv->timer-- == 0) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
        thisv->actionFunc = BgMizuShutter_Move;
    }
}

void BgMizuShutter_Move(BgMizuShutter* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.x, thisv->openPos.x, 1.0f, 4.0f, 0.1f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->openPos.y, 1.0f, 4.0f, 0.1f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.z, thisv->openPos.z, 1.0f, 4.0f, 0.1f);
        if ((thisv->dyna.actor.world.pos.x == thisv->openPos.x) && (thisv->dyna.actor.world.pos.y == thisv->openPos.y) &&
            (thisv->dyna.actor.world.pos.z == thisv->openPos.z)) {
            thisv->timer = thisv->timerMax;
            thisv->actionFunc = BgMizuShutter_WaitForTimer;
        }
    } else {
        Math_SmoothStepToF(&thisv->maxSpeed, 20.0f, 1.0f, 3.0f, 0.1f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.x, thisv->closedPos.x, 1.0f, thisv->maxSpeed, 0.1f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->closedPos.y, 1.0f, thisv->maxSpeed, 0.1f);
        Math_SmoothStepToF(&thisv->dyna.actor.world.pos.z, thisv->closedPos.z, 1.0f, thisv->maxSpeed, 0.1f);
        if ((thisv->dyna.actor.world.pos.x == thisv->closedPos.x) &&
            (thisv->dyna.actor.world.pos.y == thisv->closedPos.y) &&
            (thisv->dyna.actor.world.pos.z == thisv->closedPos.z)) {
            func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONE_BOUND);
            thisv->actionFunc = BgMizuShutter_WaitForSwitch;
        }
    }
}

void BgMizuShutter_WaitForTimer(BgMizuShutter* thisv, GlobalContext* globalCtx) {
    if (thisv->timerMax != 0x3F * 20) {
        thisv->timer--;
        func_8002F994(&thisv->dyna.actor, thisv->timer);
        if (thisv->timer == 0) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
            Flags_UnsetSwitch(globalCtx, (u16)thisv->dyna.actor.params & 0x3F);
            thisv->actionFunc = BgMizuShutter_Move;
        }
    }
}

void BgMizuShutter_Update(BgMizuShutter* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMizuShutter* thisv = (BgMizuShutter*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgMizuShutter_Draw(BgMizuShutter* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMizuShutter* thisv = (BgMizuShutter*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mizu_shutter.c", 410);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mizu_shutter.c", 415),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisv->displayList != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, thisv->displayList);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mizu_shutter.c", 422);
}
