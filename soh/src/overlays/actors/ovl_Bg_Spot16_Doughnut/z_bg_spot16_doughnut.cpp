/*
 * File: z_bg_spot16_doughnut.c
 * Overlay: ovl_Bg_Spot16_Doughnut
 * Description: Death Mountain cloud circle
 */

#include "z_bg_spot16_doughnut.h"
#include "objects/object_efc_doughnut/object_efc_doughnut.h"
#include "vt.h"

#define FLAGS 0

void BgSpot16Doughnut_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Doughnut_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Doughnut_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Doughnut_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgSpot16Doughnut_UpdateExpanding(Actor* thisx, GlobalContext* globalCtx);
void BgSpot16Doughnut_DrawExpanding(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Spot16_Doughnut_InitVars = {
    ACTOR_BG_SPOT16_DOUGHNUT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_EFC_DOUGHNUT,
    sizeof(BgSpot16Doughnut),
    (ActorFunc)BgSpot16Doughnut_Init,
    (ActorFunc)BgSpot16Doughnut_Destroy,
    (ActorFunc)BgSpot16Doughnut_Update,
    (ActorFunc)BgSpot16Doughnut_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 5000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 5000, ICHAIN_STOP),
};

static s16 sScales[] = {
    0, 0, 70, 210, 300,
};

void BgSpot16Doughnut_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Doughnut* thisv = (BgSpot16Doughnut*)thisx;
    s32 params;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.1f);
    thisv->fireFlag = 0;
    thisv->envColorAlpha = 255;
    params = thisv->actor.params;
    if (params == 1 || params == 2 || params == 3 || params == 4) {
        Actor_SetScale(&thisv->actor, sScales[thisv->actor.params] * 1.0e-4f);
        thisv->actor.draw = BgSpot16Doughnut_DrawExpanding;
        thisv->actor.update = BgSpot16Doughnut_UpdateExpanding;
    } else {
        // Scales thisv actor for scenes where it is featured in the background,
        // Death Mountain itself falls into the default case.
        switch (globalCtx->sceneNum) {
            case SCENE_SPOT01:
                Actor_SetScale(&thisv->actor, 0.04f);
                break;
            case SCENE_SHRINE:
            case SCENE_SHRINE_N:
            case SCENE_SHRINE_R:
                Actor_SetScale(&thisv->actor, 0.018f);
                break;
            default:
                Actor_SetScale(&thisv->actor, 0.1f);
                break;
        }
        osSyncPrintf(VT_FGCOL(CYAN) "%f" VT_RST "\n", thisv->actor.scale.x);
        if (!LINK_IS_ADULT || gSaveContext.eventChkInf[2] & 0x8000) {
            thisv->fireFlag &= ~1;
        } else {
            thisv->fireFlag |= 1;
        }
        osSyncPrintf("(ｓｐｏｔ１６ ドーナツ雲)(arg_data 0x%04x)\n", thisv->actor.params);
    }
}

void BgSpot16Doughnut_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void BgSpot16Doughnut_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Doughnut* thisv = (BgSpot16Doughnut*)thisx;

    if (!(thisv->fireFlag & 1)) {
        thisv->actor.shape.rot.y -= 0x20;
        if (thisv->envColorAlpha < 255) {
            thisv->envColorAlpha += 5;
        } else {
            thisv->envColorAlpha = 255;
        }
    } else if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[2] != NULL &&
               globalCtx->csCtx.npcActions[2]->action == 2) {
        if (thisv->envColorAlpha >= 6) {
            thisv->envColorAlpha -= 5;
        } else {
            thisv->envColorAlpha = 0;
            thisv->fireFlag &= ~1;
        }
    }
}

// Update function for outwardly expanding and dissipating
void BgSpot16Doughnut_UpdateExpanding(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Doughnut* thisv = (BgSpot16Doughnut*)thisx;

    if (thisv->envColorAlpha >= 6) {
        thisv->envColorAlpha -= 5;
    } else {
        Actor_Kill(&thisv->actor);
    }
    thisv->actor.shape.rot.y -= 0x20;
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x + 0.0019999998f);
}

void BgSpot16Doughnut_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Doughnut* thisv = (BgSpot16Doughnut*)thisx;
    u32 scroll = globalCtx->gameplayFrames & 0xFFFF;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 210);

    func_80093D84(globalCtx->state.gfxCtx);

    if (1) {}

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 213),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (thisv->fireFlag & 1) {
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, scroll * (-1), 0, 16, 32, 1, scroll, scroll * (-2), 16, 32));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, thisv->envColorAlpha);
        gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleFieryDL);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->envColorAlpha);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleNormalDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 238);
}

// Draw function for outwardly expanding and dissipating
void BgSpot16Doughnut_DrawExpanding(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot16Doughnut* thisv = (BgSpot16Doughnut*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 245);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 248),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->envColorAlpha);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_XLU_DISP++, gDeathMountainCloudCircleNormalDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_spot16_doughnut.c", 256);
}
