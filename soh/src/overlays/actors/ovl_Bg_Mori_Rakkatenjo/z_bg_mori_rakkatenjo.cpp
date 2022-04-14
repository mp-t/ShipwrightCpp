/*
 * File: z_bg_mori_rakkatenjo.c
 * Overlay: ovl_Bg_Mori_Rakkatenjo
 * Description: Falling ceiling in Forest Temple
 */

#include "z_bg_mori_rakkatenjo.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMoriRakkatenjo_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriRakkatenjo_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriRakkatenjo_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriRakkatenjo_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriRakkatenjo_SetupWaitForMoriTex(BgMoriRakkatenjo* thisv);
void BgMoriRakkatenjo_WaitForMoriTex(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx);
void BgMoriRakkatenjo_SetupWait(BgMoriRakkatenjo* thisv);
void BgMoriRakkatenjo_Wait(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx);
void BgMoriRakkatenjo_SetupFall(BgMoriRakkatenjo* thisv);
void BgMoriRakkatenjo_Fall(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx);
void BgMoriRakkatenjo_SetupRest(BgMoriRakkatenjo* thisv);
void BgMoriRakkatenjo_Rest(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx);
void BgMoriRakkatenjo_SetupRise(BgMoriRakkatenjo* thisv);
void BgMoriRakkatenjo_Rise(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx);

static s16 sCamSetting = 0;

const ActorInit Bg_Mori_Rakkatenjo_InitVars = {
    ACTOR_BG_MORI_RAKKATENJO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriRakkatenjo),
    (ActorFunc)BgMoriRakkatenjo_Init,
    (ActorFunc)BgMoriRakkatenjo_Destroy,
    (ActorFunc)BgMoriRakkatenjo_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -11, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgMoriRakkatenjo_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriRakkatenjo* thisv = (BgMoriRakkatenjo*)thisvx;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    // "Forest Temple obj. Falling Ceiling"
    osSyncPrintf("森の神殿 obj. 落下天井 (home posY %f)\n", thisv->dyna.actor.home.pos.y);
    if ((fabsf(1991.0f - thisv->dyna.actor.home.pos.x) > 0.001f) ||
        (fabsf(683.0f - thisv->dyna.actor.home.pos.y) > 0.001f) ||
        (fabsf(-2520.0f - thisv->dyna.actor.home.pos.z) > 0.001f)) {
        // "The set position has been changed. Let's fix the program."
        osSyncPrintf("Warning : セット位置が変更されています。プログラムを修正しましょう。\n");
    }
    if (thisv->dyna.actor.home.rot.y != 0x8000) {
        // "The set Angle has changed. Let's fix the program."
        osSyncPrintf("Warning : セット Angle が変更されています。プログラムを修正しましょう。\n");
    }
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        // "Forest Temple obj Falling Ceiling Bank Danger!"
        osSyncPrintf("Error : 森の神殿 obj 落下天井 バンク危険！(%s %d)\n", "../z_bg_mori_rakkatenjo.c", 205);
        Actor_Kill(&thisv->dyna.actor);
        return;
    }
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    CollisionHeader_GetVirtual(&gMoriRakkatenjoCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    BgMoriRakkatenjo_SetupWaitForMoriTex(thisv);
    sCamSetting = 0;
}

void BgMoriRakkatenjo_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriRakkatenjo* thisv = (BgMoriRakkatenjo*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

s32 BgMoriRakkatenjo_IsLinkUnder(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    Vec3f* pos = &GET_PLAYER(globalCtx)->actor.world.pos;

    return (-3300.0f < pos->z) && (pos->z < -1840.0f) && (1791.0f < pos->x) && (pos->x < 2191.0f);
}

s32 BgMoriRakkatenjo_IsLinkClose(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    Vec3f* pos = &GET_PLAYER(globalCtx)->actor.world.pos;

    return (-3360.0f < pos->z) && (pos->z < -1840.0f) && (1791.0f < pos->x) && (pos->x < 2191.0f);
}

void BgMoriRakkatenjo_SetupWaitForMoriTex(BgMoriRakkatenjo* thisv) {
    thisv->actionFunc = BgMoriRakkatenjo_WaitForMoriTex;
}

void BgMoriRakkatenjo_WaitForMoriTex(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        BgMoriRakkatenjo_SetupWait(thisv);
        thisv->dyna.actor.draw = BgMoriRakkatenjo_Draw;
    }
}

void BgMoriRakkatenjo_SetupWait(BgMoriRakkatenjo* thisv) {
    thisv->timer = (thisv->fallCount > 0) ? 100 : 21;
    thisv->dyna.actor.world.pos.y = 683.0f;
    thisv->actionFunc = BgMoriRakkatenjo_Wait;
}

void BgMoriRakkatenjo_Wait(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    if (thisv->fallCount == 0) {
        if (BgMoriRakkatenjo_IsLinkClose(thisv, globalCtx) || (thisv->timer < 20)) {
            if (thisv->timer <= 0) {
                BgMoriRakkatenjo_SetupFall(thisv);
            }
        } else {
            thisv->timer = 21;
        }
    } else {
        if (BgMoriRakkatenjo_IsLinkUnder(thisv, globalCtx) || (thisv->timer < 20)) {
            if (thisv->timer <= 0) {
                BgMoriRakkatenjo_SetupFall(thisv);
            }
        } else {
            thisv->timer = 100;
        }
    }
    if (thisv->timer < 20) {
        func_800788CC(NA_SE_EV_BLOCKSINK - SFX_FLAG);
    }
}

void BgMoriRakkatenjo_SetupFall(BgMoriRakkatenjo* thisv) {
    thisv->actionFunc = BgMoriRakkatenjo_Fall;
    thisv->bounceCount = 0;
    thisv->dyna.actor.velocity.y = 0.0f;
}

void BgMoriRakkatenjo_Fall(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    static f32 bounceVel[] = { 4.0f, 1.5f, 0.4f, 0.1f };
    s32 pad;
    Actor* thisvx = &thisv->dyna.actor;
    s32 quake;

    Actor_MoveForward(thisvx);
    if ((thisvx->velocity.y < 0.0f) && (thisvx->world.pos.y <= 403.0f)) {
        if (thisv->bounceCount >= ARRAY_COUNT(bounceVel)) {
            BgMoriRakkatenjo_SetupRest(thisv);
        } else {
            if (thisv->bounceCount == 0) {
                thisv->fallCount++;
                func_800788CC(NA_SE_EV_STONE_BOUND);
                func_800AA000(SQ(thisvx->yDistToPlayer), 0xFF, 0x14, 0x96);
            }
            thisvx->world.pos.y =
                403.0f - (thisvx->world.pos.y - 403.0f) * bounceVel[thisv->bounceCount] / fabsf(thisvx->velocity.y);
            thisvx->velocity.y = bounceVel[thisv->bounceCount];
            thisv->bounceCount++;
            quake = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
            Quake_SetSpeed(quake, 50000);
            Quake_SetQuakeValues(quake, 5, 0, 0, 0);
            Quake_SetCountdown(quake, 5);
        }
    }
}

void BgMoriRakkatenjo_SetupRest(BgMoriRakkatenjo* thisv) {
    thisv->actionFunc = BgMoriRakkatenjo_Rest;
    thisv->dyna.actor.world.pos.y = 403.0f;
    thisv->timer = 20;
}

void BgMoriRakkatenjo_Rest(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    if (thisv->timer <= 0) {
        BgMoriRakkatenjo_SetupRise(thisv);
    }
}

void BgMoriRakkatenjo_SetupRise(BgMoriRakkatenjo* thisv) {
    thisv->actionFunc = BgMoriRakkatenjo_Rise;
    thisv->dyna.actor.velocity.y = -0.1f;
}

void BgMoriRakkatenjo_Rise(BgMoriRakkatenjo* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->dyna.actor.velocity.y, 5.0f, 0.06f, 0.1f, 0.0f);
    thisv->dyna.actor.world.pos.y += thisv->dyna.actor.velocity.y;
    if (thisv->dyna.actor.world.pos.y >= 683.0f) {
        BgMoriRakkatenjo_SetupWait(thisv);
    }
}

void BgMoriRakkatenjo_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriRakkatenjo* thisv = (BgMoriRakkatenjo*)thisvx;

    if (thisv->timer > 0) {
        thisv->timer--;
    }
    thisv->actionFunc(thisv, globalCtx);
    if (BgMoriRakkatenjo_IsLinkUnder(thisv, globalCtx)) {
        if (sCamSetting == CAM_SET_NONE) {
            osSyncPrintf("camera changed (mori rakka tenjyo) ... \n");
            sCamSetting = globalCtx->cameraPtrs[MAIN_CAM]->setting;
            Camera_SetCameraData(globalCtx->cameraPtrs[MAIN_CAM], 1, &thisv->dyna.actor, NULL, 0, 0, 0);
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FOREST_BIRDS_EYE);
        }
    } else if (sCamSetting != CAM_SET_NONE) {
        osSyncPrintf("camera changed (previous) ... \n");
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON1);
        sCamSetting = 0;
    }
}

void BgMoriRakkatenjo_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriRakkatenjo* thisv = (BgMoriRakkatenjo*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 497);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 502),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gMoriRakkatenjoDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 506);
}
