/*
 * File: z_demo_ext.c
 * Overlay: Demo_Ext
 * Description: Magic Vortex in Silver Gauntlets Cutscene
 */

#include "z_demo_ext.h"
#include "vt.h"
#include "objects/object_fhg/object_fhg.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x00 */ EXT_WAIT,
    /* 0x01 */ EXT_MAINTAIN,
    /* 0x02 */ EXT_DISPELL
} DemoExtAction;

typedef enum {
    /* 0x00 */ EXT_DRAW_NOTHING,
    /* 0x01 */ EXT_DRAW_VORTEX
} DemoExtDrawMode;

void DemoExt_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoExt_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoExt_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoExt_Draw(Actor* thisx, GlobalContext* globalCtx);

void DemoExt_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void DemoExt_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoExt* thisv = (DemoExt*)thisx;

    thisv->scrollIncr[0] = 25;
    thisv->scrollIncr[1] = 40;
    thisv->scrollIncr[2] = 5;
    thisv->scrollIncr[3] = 30;
    thisv->primAlpha = kREG(28) + 255;
    thisv->envAlpha = kREG(32) + 255;
    thisv->scale.x = kREG(19) + 400.0f;
    thisv->scale.y = kREG(20) + 100.0f;
    thisv->scale.z = kREG(21) + 400.0f;
}

void DemoExt_PlayVortexSFX(DemoExt* thisv) {
    if (thisv->alphaTimer <= (kREG(35) + 40.0f) - 15.0f) {
        Audio_PlaySoundGeneral(NA_SE_EV_FANTOM_WARP_L - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                               &D_801333E0, &D_801333E8);
    }
}

CsCmdActorAction* DemoExt_GetNpcAction(GlobalContext* globalCtx, s32 npcActionIndex) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[npcActionIndex];
    }
    return NULL;
}

void DemoExt_SetupWait(DemoExt* thisv) {
    thisv->action = EXT_WAIT;
    thisv->drawMode = EXT_DRAW_NOTHING;
}

void DemoExt_SetupMaintainVortex(DemoExt* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoExt_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
    thisv->action = EXT_MAINTAIN;
    thisv->drawMode = EXT_DRAW_VORTEX;
}

void DemoExt_SetupDispellVortex(DemoExt* thisv) {
    thisv->action = EXT_DISPELL;
    thisv->drawMode = EXT_DRAW_VORTEX;
}

void DemoExt_FinishClosing(DemoExt* thisv) {
    thisv->alphaTimer += 1.0f;
    if ((kREG(35) + 40.0f) <= thisv->alphaTimer) {
        Actor_Kill(&thisv->actor);
    }
}

void DemoExt_CheckCsMode(DemoExt* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* csCmdNPCAction = DemoExt_GetNpcAction(globalCtx, 5);
    s32 csAction;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        csAction = csCmdNPCAction->action;
        previousCsAction = thisv->previousCsAction;

        if (csAction != previousCsAction) {
            switch (csAction) {
                case 1:
                    DemoExt_SetupWait(thisv);
                    break;
                case 2:
                    DemoExt_SetupMaintainVortex(thisv, globalCtx);
                    break;
                case 3:
                    DemoExt_SetupDispellVortex(thisv);
                    break;
                default:
                    // "Demo_Ext_Check_DemoMode: there is no such action!"
                    osSyncPrintf("Demo_Ext_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            thisv->previousCsAction = csAction;
        }
    }
}

void DemoExt_SetScrollAndRotation(DemoExt* thisv) {
    s16* scrollIncr = thisv->scrollIncr;
    s16* curScroll = thisv->curScroll;
    s32 i;

    for (i = 3; i != 0; i--) {
        curScroll[i] += scrollIncr[i];
    }
    thisv->rotationPitch += (s16)(kREG(34) + 1000);
}

void DemoExt_SetColorsAndScales(DemoExt* thisv) {
    Vec3f* scale = &thisv->scale;
    f32 shrinkFactor;

    shrinkFactor = ((kREG(35) + 40.0f) - thisv->alphaTimer) / (kREG(35) + 40.0f);
    if (shrinkFactor < 0.0f) {
        shrinkFactor = 0.0f;
    }

    thisv->primAlpha = (u32)(kREG(28) + 255) * shrinkFactor;
    thisv->envAlpha = (u32)(kREG(32) + 255) * shrinkFactor;
    scale->x = (kREG(19) + 400.0f) * shrinkFactor;
    scale->y = (kREG(20) + 100.0f) * shrinkFactor;
    scale->z = (kREG(21) + 400.0f) * shrinkFactor;
}

void DemoExt_Wait(DemoExt* thisv, GlobalContext* globalCtx) {
    DemoExt_CheckCsMode(thisv, globalCtx);
}

void DemoExt_MaintainVortex(DemoExt* thisv, GlobalContext* globalCtx) {
    DemoExt_PlayVortexSFX(thisv);
    DemoExt_SetScrollAndRotation(thisv);
    DemoExt_CheckCsMode(thisv, globalCtx);
}

void DemoExt_DispellVortex(DemoExt* thisv, GlobalContext* globalCtx) {
    DemoExt_PlayVortexSFX(thisv);
    DemoExt_SetScrollAndRotation(thisv);
    DemoExt_SetColorsAndScales(thisv);
    DemoExt_FinishClosing(thisv);
}

static DemoExtActionFunc sActionFuncs[] = {
    DemoExt_Wait,
    DemoExt_MaintainVortex,
    DemoExt_DispellVortex,
};

void DemoExt_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoExt* thisv = (DemoExt*)thisx;

    if ((thisv->action < EXT_WAIT) || (thisv->action > EXT_DISPELL) || sActionFuncs[thisv->action] == NULL) {
        // "Main mode is abnormal!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[thisv->action](thisv, globalCtx);
    }
}

void DemoExt_DrawNothing(Actor* thisx, GlobalContext* globalCtx) {
}

void DemoExt_DrawVortex(Actor* thisx, GlobalContext* globalCtx) {
    DemoExt* thisv = (DemoExt*)thisx;
    Mtx* mtx;
    GraphicsContext* gfxCtx;
    s16* curScroll;
    Vec3f* scale;

    scale = &thisv->scale;
    gfxCtx = globalCtx->state.gfxCtx;
    mtx = Graph_Alloc(gfxCtx, sizeof(Mtx));

    OPEN_DISPS(gfxCtx, "../z_demo_ext.c", 460);
    Matrix_Push();
    Matrix_Scale(scale->x, scale->y, scale->z, MTXMODE_APPLY);
    Matrix_RotateZYX((s16)(kREG(16) + 0x4000), thisv->rotationPitch, kREG(18), MTXMODE_APPLY);
    Matrix_Translate(kREG(22), kREG(23), kREG(24), MTXMODE_APPLY);
    Matrix_ToMtx(mtx, "../z_demo_ext.c", 476);
    Matrix_Pop();
    func_80093D84(gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, kREG(33) + 128, kREG(25) + 140, kREG(26) + 80, kREG(27) + 140, thisv->primAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, kREG(29) + 90, kREG(30) + 50, kREG(31) + 95, thisv->envAlpha);

    curScroll = thisv->curScroll;
    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        Gfx_TwoTexScroll(gfxCtx, 0, curScroll[0], curScroll[1], 0x40, 0x40, 1, curScroll[2], curScroll[3], 0x40, 0x40));

    gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gPhantomWarpDL);
    gSPPopMatrix(POLY_XLU_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_ext.c", 512);
}

static DemoExtDrawFunc sDrawFuncs[] = {
    DemoExt_DrawNothing,
    DemoExt_DrawVortex,
};

void DemoExt_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoExt* thisv = (DemoExt*)thisx;

    if ((thisv->drawMode < EXT_DRAW_NOTHING) || (thisv->drawMode > EXT_DRAW_VORTEX) ||
        sDrawFuncs[thisv->drawMode] == NULL) {
        // "Draw mode is abnormal!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sDrawFuncs[thisv->drawMode](thisx, globalCtx);
    }
}

const ActorInit Demo_Ext_InitVars = {
    ACTOR_DEMO_EXT,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FHG,
    sizeof(DemoExt),
    (ActorFunc)DemoExt_Init,
    (ActorFunc)DemoExt_Destroy,
    (ActorFunc)DemoExt_Update,
    (ActorFunc)DemoExt_Draw,
    NULL,
};
