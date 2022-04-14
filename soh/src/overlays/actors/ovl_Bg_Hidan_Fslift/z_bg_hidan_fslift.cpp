/*
 * File: z_bg_hidan_fslift.c
 * Overlay: ovl_Bg_Hidan_Fslift
 * Description: Hookshot Elevator
 */

#include "z_bg_hidan_fslift.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanFslift_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFslift_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFslift_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFslift_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80886FCC(BgHidanFslift* thisv, GlobalContext* globalCtx);
void func_8088706C(BgHidanFslift* thisv, GlobalContext* globalCtx);
void func_808870D8(BgHidanFslift* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Hidan_Fslift_InitVars = {
    ACTOR_BG_HIDAN_FSLIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanFslift),
    (ActorFunc)BgHidanFslift_Init,
    (ActorFunc)BgHidanFslift_Destroy,
    (ActorFunc)BgHidanFslift_Update,
    (ActorFunc)BgHidanFslift_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_STOP),
};

void BgHidanFslift_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad1;
    BgHidanFslift* thisv = (BgHidanFslift*)thisx;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gFireTempleHookshotElevatorCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_OBJ_HSBLOCK,
                           thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y + 40.0f,
                           thisv->dyna.actor.world.pos.z + -28.0f, 0, 0, 0, 2) == NULL) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }
    thisv->actionFunc = func_80886FCC;
}

void func_80886F24(BgHidanFslift* thisv) {
    if (thisv->dyna.actor.child != NULL && thisv->dyna.actor.child->update != NULL) {
        thisv->dyna.actor.child->world.pos.x = thisv->dyna.actor.world.pos.x;
        thisv->dyna.actor.child->world.pos.y = thisv->dyna.actor.world.pos.y + 40.0f;
        thisv->dyna.actor.child->world.pos.z = thisv->dyna.actor.world.pos.z + -28.0f;
    } else {
        thisv->dyna.actor.child = NULL;
    }
}

void BgHidanFslift_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFslift* thisv = (BgHidanFslift*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80886FB4(BgHidanFslift* thisv) {
    thisv->timer = 40;
    thisv->actionFunc = func_80886FCC;
}

void func_80886FCC(BgHidanFslift* thisv, GlobalContext* globalCtx) {
    s32 heightBool;

    if (thisv->timer) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        heightBool = false;
        if ((thisv->dyna.actor.world.pos.y - thisv->dyna.actor.home.pos.y) < 0.5f) {
            heightBool = true;
        }
        if (func_80043590(&thisv->dyna) && (heightBool)) {
            thisv->actionFunc = func_808870D8;
        } else if (!heightBool) {
            thisv->actionFunc = func_8088706C;
        }
    }
}

void func_8088706C(BgHidanFslift* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 4.0f)) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        func_80886FB4(thisv);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
    func_80886F24(thisv);
}

void func_808870D8(BgHidanFslift* thisv, GlobalContext* globalCtx) {
    if (func_80043590(&thisv->dyna)) {
        if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 790.0f, 4.0f)) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            func_80886FB4(thisv);
        } else {
            func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
        }
    } else {
        func_80886FB4(thisv);
    }
    func_80886F24(thisv);
}

void BgHidanFslift_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanFslift* thisv = (BgHidanFslift*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (func_8004356C(&thisv->dyna)) {
        if (thisv->unk_16A == 0) {
            thisv->unk_16A = 3;
        }
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
    } else if (!func_8004356C(&thisv->dyna)) {
        if (thisv->unk_16A != 0) {
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
        }
        thisv->unk_16A = 0;
    }
}

void BgHidanFslift_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gFireTempleHookshotElevatorDL);
}
