/*
 * File: z_bg_mori_bigst.c
 * Overlay: ovl_Bg_Mori_Bigst
 * Description: Forest Temple falling platform and Stalfos fight
 */

#include "z_bg_mori_bigst.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgMoriBigst_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriBigst_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriBigst_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriBigst_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriBigst_SetupWaitForMoriTex(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_WaitForMoriTex(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupNoop(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupStalfosFight(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_StalfosFight(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupFall(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_Fall(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupLanding(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_Landing(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupStalfosPairFight(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_StalfosPairFight(BgMoriBigst* thisv, GlobalContext* globalCtx);
void BgMoriBigst_SetupDone(BgMoriBigst* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Mori_Bigst_InitVars = {
    ACTOR_BG_MORI_BIGST,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriBigst),
    (ActorFunc)BgMoriBigst_Init,
    (ActorFunc)BgMoriBigst_Destroy,
    (ActorFunc)BgMoriBigst_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 3000, ICHAIN_CONTINUE),      ICHAIN_F32(uncullZoneScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 3000, ICHAIN_CONTINUE),     ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(minVelocityY, -12000, ICHAIN_CONTINUE), ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgMoriBigst_SetupAction(BgMoriBigst* thisv, BgMoriBigstActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgMoriBigst_InitDynapoly(BgMoriBigst* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_bigst.c", 190,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgMoriBigst_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriBigst* thisv = (BgMoriBigst*)thisvx;

    // "mori (bigST.keyceiling)"
    osSyncPrintf("mori (bigST.鍵型天井)(arg : %04x)(sw %d)(noE %d)(roomC %d)(playerPosY %f)\n", thisv->dyna.actor.params,
                 Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F),
                 Flags_GetTempClear(globalCtx, thisv->dyna.actor.room), Flags_GetClear(globalCtx, thisv->dyna.actor.room),
                 GET_PLAYER(globalCtx)->actor.world.pos.y);
    BgMoriBigst_InitDynapoly(thisv, globalCtx, &gMoriBigstCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        // "【Big Stalfos key ceiling】 bank danger!"
        osSyncPrintf("【ビッグスタルフォス鍵型天井】 バンク危険！\n");
        osSyncPrintf("%s %d\n", "../z_bg_mori_bigst.c", 234);
        Actor_Kill(&thisv->dyna.actor);
        return;
    }
    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y;
    } else {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + 270.0f;
    }
    Actor_SetFocus(&thisv->dyna.actor, 50.0f);
    BgMoriBigst_SetupWaitForMoriTex(thisv, globalCtx);
}

void BgMoriBigst_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriBigst* thisv = (BgMoriBigst*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgMoriBigst_SetupWaitForMoriTex(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    BgMoriBigst_SetupAction(thisv, BgMoriBigst_WaitForMoriTex);
}

void BgMoriBigst_WaitForMoriTex(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    Actor* thisvx = &thisv->dyna.actor;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        thisvx->draw = BgMoriBigst_Draw;
        if (Flags_GetClear(globalCtx, thisvx->room) && (GET_PLAYER(globalCtx)->actor.world.pos.y > 700.0f)) {
            if (Flags_GetSwitch(globalCtx, (thisvx->params >> 8) & 0x3F)) {
                BgMoriBigst_SetupDone(thisv, globalCtx);
            } else {
                BgMoriBigst_SetupStalfosFight(thisv, globalCtx);
            }
        } else {
            BgMoriBigst_SetupNoop(thisv, globalCtx);
        }
    }
}

void BgMoriBigst_SetupNoop(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    BgMoriBigst_SetupAction(thisv, NULL);
}

void BgMoriBigst_SetupStalfosFight(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    Actor* stalfos;

    BgMoriBigst_SetupAction(thisv, BgMoriBigst_StalfosFight);
    Flags_UnsetClear(globalCtx, thisv->dyna.actor.room);
    stalfos = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_TEST, 209.0f, 827.0f,
                                 -3320.0f, 0, 0, 0, 1);
    if (stalfos != NULL) {
        thisv->dyna.actor.child = NULL;
        thisv->dyna.actor.home.rot.z++;
    } else {
        // "Second Stalfos failure"
        osSyncPrintf("Warning : 第２スタルフォス発生失敗\n");
    }
    Flags_SetClear(globalCtx, thisv->dyna.actor.room);
}

void BgMoriBigst_StalfosFight(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->dyna.actor.home.rot.z == 0) &&
        ((thisv->dyna.actor.home.pos.y - 5.0f) <= GET_PLAYER(globalCtx)->actor.world.pos.y)) {
        BgMoriBigst_SetupFall(thisv, globalCtx);
        OnePointCutscene_Init(globalCtx, 3220, 72, &thisv->dyna.actor, MAIN_CAM);
    }
}

void BgMoriBigst_SetupFall(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    BgMoriBigst_SetupAction(thisv, BgMoriBigst_Fall);
}

void BgMoriBigst_Fall(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    Actor_MoveForward(&thisv->dyna.actor);
    if (thisv->dyna.actor.world.pos.y <= thisv->dyna.actor.home.pos.y) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y;
        BgMoriBigst_SetupLanding(thisv, globalCtx);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONE_BOUND);
        OnePointCutscene_Init(globalCtx, 1020, 8, &thisv->dyna.actor, MAIN_CAM);
        func_8002DF38(globalCtx, NULL, 0x3C);
    }
}

void BgMoriBigst_SetupLanding(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 quake;

    BgMoriBigst_SetupAction(thisv, BgMoriBigst_Landing);
    thisv->waitTimer = 18;
    quake = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
    Quake_SetSpeed(quake, 25000);
    Quake_SetQuakeValues(quake, 5, 0, 0, 0);
    Quake_SetCountdown(quake, 16);
}

void BgMoriBigst_Landing(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    if (thisv->waitTimer <= 0) {
        BgMoriBigst_SetupStalfosPairFight(thisv, globalCtx);
    }
}

void BgMoriBigst_SetupStalfosPairFight(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    Actor* stalfos1;
    Actor* stalfos2;

    BgMoriBigst_SetupAction(thisv, BgMoriBigst_StalfosPairFight);
    Flags_UnsetClear(globalCtx, thisv->dyna.actor.room);
    stalfos1 = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_TEST, 70.0f, 827.0f,
                                  -3383.0f, 0, 0, 0, 5);
    if (stalfos1 != NULL) {
        thisv->dyna.actor.child = NULL;
        thisv->dyna.actor.home.rot.z++;
    } else {
        // "Warning: 3-1 Stalfos failure"
        osSyncPrintf("Warning : 第３-1スタルフォス発生失敗\n");
    }
    stalfos2 = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_TEST, 170.0f, 827.0f,
                                  -3260.0f, 0, 0, 0, 5);
    if (stalfos2 != NULL) {
        thisv->dyna.actor.child = NULL;
        thisv->dyna.actor.home.rot.z++;
    } else {
        // "Warning: 3-2 Stalfos failure"
        osSyncPrintf("Warning : 第３-2スタルフォス発生失敗\n");
    }
    Flags_SetClear(globalCtx, thisv->dyna.actor.room);
}

void BgMoriBigst_StalfosPairFight(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    if ((thisv->dyna.actor.home.rot.z == 0) && !Player_InCsMode(globalCtx)) {
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
        BgMoriBigst_SetupDone(thisv, globalCtx);
    }
}

void BgMoriBigst_SetupDone(BgMoriBigst* thisv, GlobalContext* globalCtx) {
    BgMoriBigst_SetupAction(thisv, NULL);
}

void BgMoriBigst_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriBigst* thisv = (BgMoriBigst*)thisvx;

    Actor_SetFocus(&thisv->dyna.actor, 50.0f);
    if (thisv->waitTimer > 0) {
        thisv->waitTimer--;
    }
    if (func_80043590(&thisv->dyna)) {
        func_80074CE8(globalCtx, 6);
    }
    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgMoriBigst_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriBigst* thisv = (BgMoriBigst*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_bigst.c", 541);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_bigst.c", 548),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gMoriBigstDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_bigst.c", 553);
}
