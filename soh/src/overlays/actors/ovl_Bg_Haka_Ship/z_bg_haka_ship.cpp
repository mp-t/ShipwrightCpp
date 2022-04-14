/*
 * File: z_bg_haka_ship.c
 * Overlay: ovl_Bg_Haka_Ship
 * Description: Shadow Temple Ship
 */

#include "z_bg_haka_ship.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgHakaShip_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHakaShip_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHakaShip_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHakaShip_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgHakaShip_ChildUpdatePosition(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_WaitForSong(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_CutsceneStationary(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_Move(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_SetupCrash(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_CrashShake(BgHakaShip* thisv, GlobalContext* globalCtx);
void BgHakaShip_CrashFall(BgHakaShip* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Haka_Ship_InitVars = {
    ACTOR_BG_HAKA_SHIP,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKA_OBJECTS,
    sizeof(BgHakaShip),
    (ActorFunc)BgHakaShip_Init,
    (ActorFunc)BgHakaShip_Destroy,
    (ActorFunc)BgHakaShip_Update,
    (ActorFunc)BgHakaShip_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaShip_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaShip* thisv = (BgHakaShip*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, 1);
    thisv->switchFlag = (thisx->params >> 8) & 0xFF;
    thisv->dyna.actor.params &= 0xFF;

    if (thisv->dyna.actor.params == 0) {
        CollisionHeader_GetVirtual(&object_haka_objects_Col_00E408, &colHeader);
        thisv->counter = 8;
        thisv->actionFunc = BgHakaShip_WaitForSong;
    } else {
        CollisionHeader_GetVirtual(&object_haka_objects_Col_00ED7C, &colHeader);
        thisv->actionFunc = BgHakaShip_ChildUpdatePosition;
    }
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.shape.rot.y - 0x4000;
    thisv->yOffset = 0;
    if (thisv->dyna.actor.params == 0 &&
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_HAKA_SHIP,
                           thisv->dyna.actor.world.pos.x + -10.0f, thisv->dyna.actor.world.pos.y + 82.0f,
                           thisv->dyna.actor.world.pos.z, 0, 0, 0, 1) == NULL) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgHakaShip_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaShip* thisv = (BgHakaShip*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Audio_StopSfxByPos(&thisv->bellSoundPos);
}

void BgHakaShip_ChildUpdatePosition(BgHakaShip* thisv, GlobalContext* globalCtx) {
    Actor* parent = thisv->dyna.actor.parent;

    if (parent != NULL && parent->update != NULL) {
        thisv->dyna.actor.world.pos.x = parent->world.pos.x + -10.0f;
        thisv->dyna.actor.world.pos.y = parent->world.pos.y + 82.0f;
        thisv->dyna.actor.world.pos.z = parent->world.pos.z;
    } else {
        thisv->dyna.actor.parent = NULL;
    }
}

void BgHakaShip_WaitForSong(BgHakaShip* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if (thisv->counter) {
            thisv->counter--;
        }
        if (thisv->counter == 0) {
            thisv->counter = 130;
            thisv->actionFunc = BgHakaShip_CutsceneStationary;
            osSyncPrintf("シーン 外輪船 ...  アァクション！！\n");
            OnePointCutscene_Init(globalCtx, 3390, 999, &thisv->dyna.actor, MAIN_CAM);
        }
    }
}

void BgHakaShip_CutsceneStationary(BgHakaShip* thisv, GlobalContext* globalCtx) {
    if (thisv->counter) {
        thisv->counter--;
    }
    thisv->yOffset = sinf(thisv->counter * (std::numbers::pi_v<float> / 25)) * 6144.0f;
    if (thisv->counter == 0) {
        thisv->counter = 50;
        thisv->actionFunc = BgHakaShip_Move;
    }
}

void BgHakaShip_Move(BgHakaShip* thisv, GlobalContext* globalCtx) {
    f32 distanceFromHome;
    Actor* child;

    if (thisv->counter) {
        thisv->counter--;
    }
    if (thisv->counter == 0) {
        thisv->counter = 50;
    }
    thisv->dyna.actor.world.pos.y = (sinf(thisv->counter * (std::numbers::pi_v<float> / 25)) * 50.0f) + thisv->dyna.actor.home.pos.y;

    distanceFromHome = thisv->dyna.actor.home.pos.x - thisv->dyna.actor.world.pos.x;
    if (distanceFromHome > 7650.0f) {
        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x - 7650.0f;
        thisv->dyna.actor.speedXZ = 0.0f;
    }
    if (distanceFromHome > 7600.0f && !Gameplay_InCsMode(globalCtx)) {
        thisv->counter = 40;
        thisv->dyna.actor.speedXZ = 0.0f;
        Message_StartTextbox(globalCtx, 0x5071, NULL);
        thisv->actionFunc = BgHakaShip_SetupCrash;
    } else {
        Math_StepToF(&thisv->dyna.actor.speedXZ, 4.0f, 0.2f);
    }
    child = thisv->dyna.actor.child;
    if (child != NULL && child->update != NULL) {
        child->shape.rot.z += ((655.0f / 13.0f) * thisv->dyna.actor.speedXZ);
    } else {
        thisv->dyna.actor.child = NULL;
    }
    thisv->yOffset = sinf(thisv->counter * (std::numbers::pi_v<float> / 25)) * 6144.0f;
}

void BgHakaShip_SetupCrash(BgHakaShip* thisv, GlobalContext* globalCtx) {
    if (thisv->counter) {
        thisv->counter--;
    }
    if (thisv->counter == 0) {
        thisv->counter = 40;
        thisv->actionFunc = BgHakaShip_CrashShake;
    }
    Math_ScaledStepToS(&thisv->yOffset, 0, 128);
}

void BgHakaShip_CrashShake(BgHakaShip* thisv, GlobalContext* globalCtx) {
    if (thisv->counter != 0) {
        thisv->counter--;
    }
    thisv->dyna.actor.world.pos.y = thisv->counter % 4 * 3 - 6 + thisv->dyna.actor.home.pos.y;
    if (!thisv->counter) {
        thisv->dyna.actor.gravity = -1.0f;
        thisv->actionFunc = BgHakaShip_CrashFall;
    }
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_BLOCKSINK - SFX_FLAG);
}

void BgHakaShip_CrashFall(BgHakaShip* thisv, GlobalContext* globalCtx) {
    Actor* child;

    if (thisv->dyna.actor.home.pos.y - thisv->dyna.actor.world.pos.y > 2000.0f) {
        Actor_Kill(&thisv->dyna.actor);
        child = thisv->dyna.actor.child;
        if (child != NULL && child->update != NULL) {
            Actor_Kill(child);
        }
    } else {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCKSINK - SFX_FLAG);
        if ((thisv->dyna.actor.home.pos.y - thisv->dyna.actor.world.pos.y > 500.0f) && func_8004356C(&thisv->dyna)) {
            Gameplay_TriggerVoidOut(globalCtx);
        }
    }
}

void BgHakaShip_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaShip* thisv = (BgHakaShip*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->dyna.actor.params == 0) {
        Actor_MoveForward(&thisv->dyna.actor);
    }
}

void BgHakaShip_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaShip* thisv = (BgHakaShip*)thisx;
    f32 angleTemp;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 528);
    func_80093D18(globalCtx->state.gfxCtx);
    if (thisv->dyna.actor.params == 0) {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 534),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_00D330);
        angleTemp = thisv->yOffset * (std::numbers::pi_v<float> / 0x8000);
        Matrix_Translate(-3670.0f, 620.0f, 1150.0f, MTXMODE_APPLY);
        Matrix_RotateZ(angleTemp, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 547),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_005A70);
        Matrix_Translate(0.0f, 0.0f, -2300.0f, MTXMODE_APPLY);
        Matrix_RotateZ(-(2.0f * angleTemp), MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 556),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_005A70);
    } else {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 562),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_haka_objects_DL_00E910);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_haka_ship.c", 568);
    if (thisv->actionFunc == BgHakaShip_CutsceneStationary || thisv->actionFunc == BgHakaShip_Move) {
        s32 pad;
        Vec3f sp2C;

        sp2C.x = thisv->dyna.actor.world.pos.x + -367.0f;
        sp2C.y = thisv->dyna.actor.world.pos.y + 62.0f;
        sp2C.z = thisv->dyna.actor.world.pos.z;

        SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &sp2C, &thisv->bellSoundPos);
        func_80078914(&thisv->bellSoundPos, NA_SE_EV_SHIP_BELL - SFX_FLAG);
    }
}
