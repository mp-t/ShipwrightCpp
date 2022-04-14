#include "z_en_syateki_itm.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    SYATEKI_ROUND_GREEN_APPEAR,
    SYATEKI_ROUND_BLUE_SEQUENTIAL,
    SYATEKI_ROUND_GREEN_THROW,
    SYATEKI_ROUND_BLUE_SIMUL,
    SYATEKI_ROUND_RED_LEFT,
    SYATEKI_ROUND_RED_RIGHT,
    SYATEKI_ROUND_MAX
} EnSyatekItemRound;

void EnSyatekiItm_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiItm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiItm_Update(Actor* thisx, GlobalContext* globalCtx);

void EnSyatekiItm_Idle(EnSyatekiItm* thisv, GlobalContext* globalCtx);
void EnSyatekiItm_StartRound(EnSyatekiItm* thisv, GlobalContext* globalCtx);
void EnSyatekiItm_SpawnTargets(EnSyatekiItm* thisv, GlobalContext* globalCtx);
void EnSyatekiItm_CheckTargets(EnSyatekiItm* thisv, GlobalContext* globalCtx);
void EnSyatekiItm_CleanupGame(EnSyatekiItm* thisv, GlobalContext* globalCtx);
void EnSyatekiItm_EndGame(EnSyatekiItm* thisv, GlobalContext* globalCtx);

const ActorInit En_Syateki_Itm_InitVars = {
    ACTOR_EN_SYATEKI_ITM,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnSyatekiItm),
    (ActorFunc)EnSyatekiItm_Init,
    (ActorFunc)EnSyatekiItm_Destroy,
    (ActorFunc)EnSyatekiItm_Update,
    NULL,
    NULL,
};

static Vec3f sGreenAppearHome = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSeqHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSeqHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sGreenThrowHome = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSimulHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSimulHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sRedLeftHome1 = { 260.0f, 100.0f, -320.0f };
static Vec3f sRedLeftHome2 = { 360.0f, 100.0f, -320.0f };
static Vec3f sRedRightHome1 = { -230.0f, 94.0f, -360.0f };
static Vec3f sRedRightHome2 = { -400.0f, 94.0f, -360.0f };
static Vec3f sGreenAppearFinal = { 0.0f, 53.0f, -270.0f };
static Vec3f sBlueSeqFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSeqFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sGreenThrowFinal = { 0.0f, 0.0f, 0.0f };
static Vec3f sBlueSimulFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSimulFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sRedLeftFinal1 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedLeftFinal2 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal1 = { 260.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal2 = { 260.0f, 0.0f, 0.0f };
static s16 sTargetColors[] = { 0, 1, 0, 1, 2, 2 };
static s16 sRupeeTypes[] = { 0, 1, 1, 0, 1, 1, 4, 4, 4, 4 };
static Vec3f sRupeePos[] = {
    { -40.0f, 0.0f, -90.0f }, { -20.0f, 0.0f, -90.0f }, { 0.0f, 0.0f, -90.0f },   { 20.0f, 0.0f, -90.0f },
    { 40.0f, 0.0f, -90.0f },  { -40.0f, 0.0f, -60.0f }, { -20.0f, 0.0f, -60.0f }, { 0.0f, 0.0f, -60.0f },
    { 20.0f, 0.0f, -60.0f },  { 40.0f, 0.0f, -60.0f },
};

void EnSyatekiItm_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnSyatekiItm* thisv = (EnSyatekiItm*)thisx;
    s32 i;

    thisv->man = (EnSyatekiMan*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_SYATEKI_MAN,
                                                  140.0f, 0.0f, 255.0f, 0, -0x4000, 0, 0);
    if (thisv->man == NULL) {
        // "Spawn error"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ エラー原 ☆☆☆☆ \n" VT_RST);
        Actor_Kill(&thisv->actor);
        return;
    }
    for (i = 0; i < 10; i++) {
        thisv->markers[i] =
            (EnExRuppy*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_RUPPY,
                                           sRupeePos[i].x, sRupeePos[i].y, sRupeePos[i].z, 0, 0, 0, 4);
        if (thisv->markers[i] == NULL) {
            // "Second spawn error"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ エラー原セカンド ☆☆☆☆ \n" VT_RST);
            Actor_Kill(&thisv->actor);
            return;
        }
        thisv->markers[i]->colorIdx = sRupeeTypes[i];
    }
    thisv->actionFunc = EnSyatekiItm_Idle;
}

void EnSyatekiItm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnSyatekiItm_Idle(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    s32 i;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->signal == ENSYATEKI_START) {
        player->actor.world.pos.x = -12.0f;
        player->actor.world.pos.y = 20.0f;
        player->actor.world.pos.z = 182.0f;
        player->currentYaw = player->actor.world.rot.y = player->actor.shape.rot.y = 0x7F03;
        player->actor.world.rot.x = player->actor.shape.rot.x = player->actor.world.rot.z = player->actor.shape.rot.z =
            0;
        func_8008EF44(globalCtx, 15);
        thisv->roundNum = thisv->hitCount = 0;
        for (i = 0; i < 6; i++) {
            thisv->roundFlags[i] = false;
        }
        for (i = 0; i < 10; i++) {
            thisv->markers[i]->galleryFlag = false;
        }
        thisv->actionFunc = EnSyatekiItm_StartRound;
    }
}

void EnSyatekiItm_StartRound(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    s32 i;
    s32 j;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->unkTimer == 0) {
        if (LINK_IS_ADULT) {
            for (i = 0, j = 0; i < SYATEKI_ROUND_MAX; i++) {
                if (thisv->roundFlags[i]) {
                    j++;
                }
            }
            if (j >= SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                thisv->signal = ENSYATEKI_END;
                thisv->actionFunc = EnSyatekiItm_CleanupGame;
                return;
            }
            i = Rand_ZeroFloat(5.99f);
            while (thisv->roundFlags[i]) {
                i = Rand_ZeroFloat(5.99f);
                if (1) {}
            }
            thisv->roundNum = i + 1;
            thisv->roundFlags[i] = true;
        } else {
            thisv->roundNum++;
            if (thisv->roundNum > SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                thisv->signal = ENSYATEKI_END;
                thisv->actionFunc = EnSyatekiItm_CleanupGame;
                return;
            }
        }

        thisv->timer = (thisv->roundNum == 1) ? 50 : 30;

        func_80078884(NA_SE_SY_FOUND);
        thisv->actionFunc = EnSyatekiItm_SpawnTargets;
    }
}

void EnSyatekiItm_SpawnTargets(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 roundIdx;

    if (globalCtx->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        thisv->signal = ENSYATEKI_END;
        thisv->actionFunc = EnSyatekiItm_CleanupGame;
        return;
    }
    if (thisv->timer == 0) {
        for (i = 0; i < 2; i++) {
            Math_Vec3f_Copy(&thisv->targetHome[i], &zeroVec);
            Math_Vec3f_Copy(&thisv->targetFinal[i], &zeroVec);
            thisv->targets[i] = NULL;
        }
        thisv->numTargets = 2;
        thisv->curMarkers[0] = thisv->curMarkers[1] = NULL;
        roundIdx = thisv->roundNum - 1;

        switch (roundIdx) {
            case SYATEKI_ROUND_GREEN_APPEAR:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sGreenAppearHome);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sGreenAppearFinal);
                thisv->curMarkers[0] = thisv->markers[0];
                thisv->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sBlueSeqHome1);
                Math_Vec3f_Copy(&thisv->targetHome[1], &sBlueSeqHome2);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sBlueSeqFinal1);
                Math_Vec3f_Copy(&thisv->targetFinal[1], &sBlueSeqFinal2);
                thisv->curMarkers[0] = thisv->markers[1];
                thisv->curMarkers[1] = thisv->markers[2];
                break;
            case SYATEKI_ROUND_GREEN_THROW:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sGreenThrowHome);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sGreenThrowFinal);
                thisv->curMarkers[0] = thisv->markers[3];
                thisv->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SIMUL:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sBlueSimulHome1);
                Math_Vec3f_Copy(&thisv->targetHome[1], &sBlueSimulHome2);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sBlueSimulFinal1);
                Math_Vec3f_Copy(&thisv->targetFinal[1], &sBlueSimulFinal2);
                thisv->curMarkers[0] = thisv->markers[4];
                thisv->curMarkers[1] = thisv->markers[5];
                break;
            case SYATEKI_ROUND_RED_LEFT:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sRedLeftHome1);
                Math_Vec3f_Copy(&thisv->targetHome[1], &sRedLeftHome2);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sRedLeftFinal1);
                Math_Vec3f_Copy(&thisv->targetFinal[1], &sRedLeftFinal2);
                thisv->curMarkers[0] = thisv->markers[6];
                thisv->curMarkers[1] = thisv->markers[7];
                break;
            case SYATEKI_ROUND_RED_RIGHT:
                Math_Vec3f_Copy(&thisv->targetHome[0], &sRedRightHome1);
                Math_Vec3f_Copy(&thisv->targetHome[1], &sRedRightHome2);
                Math_Vec3f_Copy(&thisv->targetFinal[0], &sRedRightFinal1);
                Math_Vec3f_Copy(&thisv->targetFinal[1], &sRedRightFinal2);
                thisv->curMarkers[0] = thisv->markers[8];
                thisv->curMarkers[1] = thisv->markers[9];
                break;
        }

        for (i = 0; i < thisv->numTargets; i++) {
            thisv->targets[i] = (EnGSwitch*)Actor_SpawnAsChild(
                &globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_G_SWITCH, thisv->targetHome[i].x,
                thisv->targetHome[i].y, thisv->targetHome[i].z, 0, 0, 0, (ENGSWITCH_TARGET_RUPEE << 0xC) | 0x3F);
            if (thisv->targets[i] == NULL) {
                // "Rupee spawn error"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ルピーでエラー原 ☆☆☆☆ \n" VT_RST);
                Actor_Kill(&thisv->actor);
                return;
            }
            thisv->targets[i]->index = i;
            thisv->targets[i]->colorIdx = sTargetColors[roundIdx];
            Math_Vec3f_Copy(&thisv->targets[i]->targetPos, &thisv->targetFinal[i]);
            switch (roundIdx) {
                case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                    if (i == 1) {
                        thisv->targets[i]->delayTimer = 60;
                    }
                    break;
                case SYATEKI_ROUND_GREEN_THROW:
                    thisv->targets[i]->actor.velocity.y = 15.0f;
                    thisv->targets[i]->actor.gravity = -1.0f;
                    thisv->targets[i]->moveMode = GSWITCH_THROW;
                    break;
                case SYATEKI_ROUND_RED_LEFT:
                    thisv->targets[i]->actor.velocity.x = -5.0f;
                    thisv->targets[i]->moveMode = GSWITCH_LEFT;
                    break;
                case SYATEKI_ROUND_RED_RIGHT:
                    thisv->targets[i]->actor.velocity.x = 7.0f;
                    thisv->targets[i]->moveMode = GSWITCH_RIGHT;
                    break;
            }
        }
        thisv->targetState[0] = thisv->targetState[1] = ENSYATEKIHIT_NONE;
        thisv->actionFunc = EnSyatekiItm_CheckTargets;
    }
}

void EnSyatekiItm_CheckTargets(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    s16 j;

    if (globalCtx->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        thisv->signal = ENSYATEKI_END;
        thisv->actionFunc = EnSyatekiItm_CleanupGame;
    } else {
        for (i = 0, j = 0; i < 2; i++) {
            if (thisv->targetState[i] != ENSYATEKIHIT_NONE) {
                if (thisv->targetState[i] == ENSYATEKIHIT_HIT) {
                    thisv->curMarkers[i]->galleryFlag = true;
                }
                j++;
            }
        }
        if (j == thisv->numTargets) {
            thisv->actionFunc = EnSyatekiItm_StartRound;
        }
    }
}

void EnSyatekiItm_CleanupGame(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if ((thisv->targetState[i] == ENSYATEKIHIT_NONE) && (thisv->targets[i] != NULL)) {
            Actor_Kill(&thisv->targets[i]->actor);
        }
    }
    thisv->actionFunc = EnSyatekiItm_EndGame;
}

void EnSyatekiItm_EndGame(EnSyatekiItm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    player->actor.freezeTimer = 10;
    if (thisv->signal == ENSYATEKI_RESULTS) {
        thisv->signal = ENSYATEKI_NONE;
        thisv->actionFunc = EnSyatekiItm_Idle;
    }
    if (thisv->signal == ENSYATEKI_START) {
        // "1 frame attack and defense!"
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        thisv->signal = ENSYATEKI_NONE;
        thisv->actionFunc = EnSyatekiItm_Idle;
    }
}

void EnSyatekiItm_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSyatekiItm* thisv = (EnSyatekiItm*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->unkTimer != 0) {
        thisv->unkTimer--;
    }
    if (BREG(0)) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
    }
}
