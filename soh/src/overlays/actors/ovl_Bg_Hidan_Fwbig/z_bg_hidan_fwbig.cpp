/*
 * File: z_bg_hidan_fwbig.c
 * Overlay: ovl_Bg_Hidan_Fwbig
 * Description: Large fire walls at Fire Temple (flame wall before bombable door and the one that chases the player in
 * the lava room)
 */

#include "z_bg_hidan_fwbig.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0 */ FWBIG_MOVE,
    /* 1 */ FWBIG_RESET,
    /* 2 */ FWBIG_KILL
} HidanFwbigMoveState;

void BgHidanFwbig_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFwbig_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFwbig_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanFwbig_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHidanFwbig_UpdatePosition(BgHidanFwbig* thisv);

void BgHidanFwbig_WaitForSwitch(BgHidanFwbig* thisv, GlobalContext* globalCtx);
void BgHidanFwbig_WaitForCs(BgHidanFwbig* thisv, GlobalContext* globalCtx);
void BgHidanFwbig_Lower(BgHidanFwbig* thisv, GlobalContext* globalCtx);
void BgHidanFwbig_WaitForTimer(BgHidanFwbig* thisv, GlobalContext* globalCtx);
void BgHidanFwbig_WaitForPlayer(BgHidanFwbig* thisv, GlobalContext* globalCtx);
void BgHidanFwbig_Move(BgHidanFwbig* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Hidan_Fwbig_InitVars = {
    ACTOR_BG_HIDAN_FWBIG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanFwbig),
    (ActorFunc)BgHidanFwbig_Init,
    (ActorFunc)BgHidanFwbig_Destroy,
    (ActorFunc)BgHidanFwbig_Update,
    (ActorFunc)BgHidanFwbig_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 30, 130, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_STOP),
};

void BgHidanFwbig_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgHidanFwbig* thisv = (BgHidanFwbig*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->direction = (u16)(thisx->params >> 8);
    thisx->params &= 0xFF;
    if (thisv->direction != 0) {
        thisv->actor.home.pos.x = 1560.0f;
        thisv->actor.home.pos.z = 0.0f;
        if (player->actor.world.pos.z > 300.0f) {
            thisv->direction = -1;
            thisv->actor.home.rot.y = thisv->actor.shape.rot.y = -0x4E38;
        } else if (player->actor.world.pos.z < -300.0f) {
            thisv->direction = 1;
            thisv->actor.home.rot.y = thisv->actor.shape.rot.y = -0x31C8;
        } else {
            Actor_Kill(&thisv->actor);
            return;
        }
        BgHidanFwbig_UpdatePosition(thisv);
        Actor_SetScale(&thisv->actor, 0.15f);
        thisv->collider.dim.height = 230;
        thisv->actor.flags |= ACTOR_FLAG_4;
        thisv->moveState = FWBIG_MOVE;
        thisv->actionFunc = BgHidanFwbig_WaitForPlayer;
        thisv->actor.world.pos.y = thisv->actor.home.pos.y - (2400.0f * thisv->actor.scale.y);
    } else {
        Actor_SetScale(&thisv->actor, 0.1f);
        thisv->actionFunc = BgHidanFwbig_WaitForSwitch;
    }
}

void BgHidanFwbig_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHidanFwbig* thisv = (BgHidanFwbig*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void BgHidanFwbig_UpdatePosition(BgHidanFwbig* thisv) {
    s16 startAngle = thisv->actor.shape.rot.y + thisv->direction * -0x4000;

    thisv->actor.world.pos.x = (Math_SinS(startAngle) * 885.4f) + thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(startAngle) * 885.4f) + thisv->actor.home.pos.z;
}

void BgHidanFwbig_WaitForSwitch(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        thisv->actionFunc = BgHidanFwbig_WaitForCs;
        OnePointCutscene_Init(globalCtx, 3340, -99, &thisv->actor, MAIN_CAM);
        thisv->timer = 35;
    }
}

void BgHidanFwbig_WaitForCs(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (thisv->timer-- == 0) {
        thisv->actionFunc = BgHidanFwbig_Lower;
    }
}

void BgHidanFwbig_Rise(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 10.0f)) {
        if (thisv->direction == 0) {
            Flags_UnsetSwitch(globalCtx, thisv->actor.params);
            thisv->actionFunc = BgHidanFwbig_WaitForSwitch;
        } else {
            thisv->actionFunc = BgHidanFwbig_Move;
        }
    }
}

void BgHidanFwbig_Lower(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - (2400.0f * thisv->actor.scale.y), 10.0f)) {
        if (thisv->direction == 0) {
            thisv->actionFunc = BgHidanFwbig_WaitForTimer;
            thisv->timer = 150;
        } else if (thisv->moveState == FWBIG_KILL) {
            Actor_Kill(&thisv->actor);
        } else {
            if (thisv->moveState == FWBIG_MOVE) {
                thisv->actor.shape.rot.y -= (thisv->direction * 0x1800);
            } else {
                thisv->moveState = FWBIG_MOVE;
                thisv->actor.shape.rot.y = thisv->actor.home.rot.y;
            }
            BgHidanFwbig_UpdatePosition(thisv);
            thisv->actionFunc = BgHidanFwbig_Rise;
        }
    }
}

void BgHidanFwbig_WaitForTimer(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        thisv->actionFunc = BgHidanFwbig_Rise;
    }
    func_8002F994(&thisv->actor, thisv->timer);
}

void BgHidanFwbig_WaitForPlayer(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->actor.world.pos.x < 1150.0f) {
        thisv->actionFunc = BgHidanFwbig_Rise;
        OnePointCutscene_Init(globalCtx, 3290, -99, &thisv->actor, MAIN_CAM);
    }
}

void BgHidanFwbig_Move(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    if (!Player_InCsMode(globalCtx)) {
        if (Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y + (thisv->direction * 0x6390), 0x20)) {
            thisv->moveState = FWBIG_RESET;
            thisv->actionFunc = BgHidanFwbig_Lower;
        } else {
            BgHidanFwbig_UpdatePosition(thisv);
        }
    }
}

void BgHidanFwbig_MoveCollider(BgHidanFwbig* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f projPos;
    f32 cs;
    f32 sn;

    func_8002DBD0(&thisv->actor, &projPos, &player->actor.world.pos);
    projPos.z = ((projPos.z >= 0.0f) ? 1.0f : -1.0f) * 25.0f * -1.0f;
    if (thisv->direction == 0) {
        projPos.x = CLAMP(projPos.x, -360.0f, 360.0f);
    } else {
        projPos.x = CLAMP(projPos.x, -500.0f, 500.0f);
    }

    sn = Math_SinS(thisv->actor.shape.rot.y);
    cs = Math_CosS(thisv->actor.shape.rot.y);
    thisv->collider.dim.pos.x = thisv->actor.world.pos.x + (projPos.x * cs) + (projPos.z * sn);
    thisv->collider.dim.pos.z = thisv->actor.world.pos.z - (projPos.x * sn) + (projPos.z * cs);
    thisv->collider.dim.pos.y = thisv->actor.world.pos.y;

    thisv->actor.world.rot.y = (projPos.z < 0.0f) ? thisv->actor.shape.rot.y : thisv->actor.shape.rot.y + 0x8000;
}

void BgHidanFwbig_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHidanFwbig* thisv = (BgHidanFwbig*)thisx;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        func_8002F71C(globalCtx, &thisv->actor, 5.0f, thisv->actor.world.rot.y, 1.0f);
        if (thisv->direction != 0) {
            thisv->actionFunc = BgHidanFwbig_Lower;
        }
    }
    if ((thisv->direction != 0) && (globalCtx->roomCtx.prevRoom.num == thisv->actor.room)) {
        thisv->moveState = FWBIG_KILL;
        thisv->actionFunc = BgHidanFwbig_Lower;
    }

    thisv->actionFunc(thisv, globalCtx);

    if ((thisv->actor.home.pos.y - 200.0f) < thisv->actor.world.pos.y) {
        if (gSaveContext.sceneSetupIndex < 4) {
            func_8002F974(&thisv->actor, NA_SE_EV_BURNING - SFX_FLAG);
        } else if ((s16)thisv->actor.world.pos.x == -513) {
            func_8002F974(&thisv->actor, NA_SE_EV_FLAME_OF_FIRE - SFX_FLAG);
        }
        BgHidanFwbig_MoveCollider(thisv, globalCtx);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgHidanFwbig_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    f32 height;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_fwbig.c", 630);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gEffUnknown4Tex));

    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gEffUnknown5Tex));

    height = thisx->scale.y * 2400.0f;
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0,
                    ((height - (thisx->home.pos.y - thisx->world.pos.y)) * 255.0f) / height);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 0x80, 0, 0x20, 0x40, 1, 0,
                                (u8)(globalCtx->gameplayFrames * -15), 0x20, 0x40));

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_hidan_fwbig.c", 660),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gFireTempleBigFireWallDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_fwbig.c", 664);
}
