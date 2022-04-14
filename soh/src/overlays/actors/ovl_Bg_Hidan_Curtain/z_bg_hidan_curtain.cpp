/*
 * File: z_bg_hidan_curtain.c
 * Overlay: ovl_Bg_Hidan_Curtain
 * Description: Flame circle
 */

#include "z_bg_hidan_curtain.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanCurtain_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanCurtain_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanCurtain_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanCurtain_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHidanCurtain_WaitForSwitchOn(BgHidanCurtain* thisv, GlobalContext* globalCtx);
void BgHidanCurtain_WaitForCutscene(BgHidanCurtain* thisv, GlobalContext* globalCtx);
void BgHidanCurtain_WaitForClear(BgHidanCurtain* thisv, GlobalContext* globalCtx);
void BgHidanCurtain_TurnOn(BgHidanCurtain* thisv, GlobalContext* globalCtx);
void BgHidanCurtain_TurnOff(BgHidanCurtain* thisv, GlobalContext* globalCtx);
void BgHidanCurtain_WaitForTimer(BgHidanCurtain* thisv, GlobalContext* globalCtx);

typedef struct {
    /* 0x00 */ s16 radius;
    /* 0x02 */ s16 height;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 riseDist;
    /* 0x0C */ f32 riseSpeed;
} BgHidanCurtainParams; // size = 0x10

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
    { 81, 144, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sCcInfoInit = { 1, 80, 100, MASS_IMMOVABLE };

static BgHidanCurtainParams sHCParams[] = { { 81, 144, 0.090f, 144.0f, 5.0f }, { 46, 88, 0.055f, 88.0f, 3.0f } };

const ActorInit Bg_Hidan_Curtain_InitVars = {
    ACTOR_BG_HIDAN_CURTAIN,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHidanCurtain),
    (ActorFunc)BgHidanCurtain_Init,
    (ActorFunc)BgHidanCurtain_Destroy,
    (ActorFunc)BgHidanCurtain_Update,
    (ActorFunc)BgHidanCurtain_Draw,
    NULL,
};

void BgHidanCurtain_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHidanCurtain* thisv = (BgHidanCurtain*)thisx;
    BgHidanCurtainParams* hcParams;

    osSyncPrintf("Curtain (arg_data 0x%04x)\n", thisv->actor.params);
    Actor_SetFocus(&thisv->actor, 20.0f);
    thisv->type = (thisx->params >> 0xC) & 0xF;
    if (thisv->type > 6) {
        // "Type is not set"
        osSyncPrintf("Error : object のタイプが設定されていない(%s %d)(arg_data 0x%04x)\n", "../z_bg_hidan_curtain.c",
                     352, thisv->actor.params);
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->size = ((thisv->type == 2) || (thisv->type == 4)) ? 1 : 0;
    hcParams = &sHCParams[thisv->size];
    thisv->treasureFlag = (thisx->params >> 6) & 0x3F;
    thisx->params &= 0x3F;

    if ((thisv->actor.params < 0) || (thisv->actor.params > 0x3F)) {
        // "Save bit is not set"
        osSyncPrintf("Warning : object のセーブビットが設定されていない(%s %d)(arg_data 0x%04x)\n",
                     "../z_bg_hidan_curtain.c", 373, thisv->actor.params);
    }
    Actor_SetScale(&thisv->actor, hcParams->scale);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->collider.dim.pos.x = thisv->actor.world.pos.x;
    thisv->collider.dim.pos.y = thisv->actor.world.pos.y;
    thisv->collider.dim.pos.z = thisv->actor.world.pos.z;
    thisv->collider.dim.radius = hcParams->radius;
    thisv->collider.dim.height = hcParams->height;
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sCcInfoInit);
    if (thisv->type == 0) {
        thisv->actionFunc = BgHidanCurtain_WaitForClear;
    } else {
        thisv->actionFunc = BgHidanCurtain_WaitForSwitchOn;
        if ((thisv->type == 4) || (thisv->type == 5)) {
            thisv->actor.world.pos.y = thisv->actor.home.pos.y - hcParams->riseDist;
        }
    }
    if (((thisv->type == 1) && Flags_GetTreasure(globalCtx, thisv->treasureFlag)) ||
        (((thisv->type == 0) || (thisv->type == 6)) && Flags_GetClear(globalCtx, thisv->actor.room))) {
        Actor_Kill(&thisv->actor);
    }
    thisv->texScroll = Rand_ZeroOne() * 15.0f;
}

void BgHidanCurtain_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHidanCurtain* thisv = (BgHidanCurtain*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void BgHidanCurtain_WaitForSwitchOn(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        if (thisv->type == 1) {
            thisv->actionFunc = BgHidanCurtain_WaitForCutscene;
            OnePointCutscene_Init(globalCtx, 3350, -99, &thisv->actor, MAIN_CAM);
            thisv->timer = 50;
        } else if (thisv->type == 3) {
            thisv->actionFunc = BgHidanCurtain_WaitForCutscene;
            OnePointCutscene_Init(globalCtx, 3360, 60, &thisv->actor, MAIN_CAM);
            thisv->timer = 30;
        } else {
            thisv->actionFunc = BgHidanCurtain_TurnOff;
        }
    }
}

void BgHidanCurtain_WaitForCutscene(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    if (thisv->timer-- == 0) {
        thisv->actionFunc = BgHidanCurtain_TurnOff;
    }
}

void BgHidanCurtain_WaitForClear(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    if (Flags_GetClear(globalCtx, thisv->actor.room)) {
        thisv->actionFunc = BgHidanCurtain_TurnOff;
    }
}

void BgHidanCurtain_WaitForSwitchOff(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    if (!Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        thisv->actionFunc = BgHidanCurtain_TurnOn;
    }
}

void BgHidanCurtain_TurnOn(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    f32 riseSpeed = sHCParams[thisv->size].riseSpeed;

    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, riseSpeed)) {
        Flags_UnsetSwitch(globalCtx, thisv->actor.params);
        thisv->actionFunc = BgHidanCurtain_WaitForSwitchOn;
    }
}

void BgHidanCurtain_TurnOff(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    BgHidanCurtainParams* hcParams = &sHCParams[thisv->size];

    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - hcParams->riseDist, hcParams->riseSpeed)) {
        if ((thisv->type == 0) || (thisv->type == 6)) {
            Actor_Kill(&thisv->actor);
        } else if (thisv->type == 5) {
            thisv->actionFunc = BgHidanCurtain_WaitForSwitchOff;
        } else {
            if (thisv->type == 2) {
                thisv->timer = 400;
            } else if (thisv->type == 4) {
                thisv->timer = 200;
            } else if (thisv->type == 3) {
                thisv->timer = 160;
            } else { // thisv->type == 1
                thisv->timer = 300;
            }
            thisv->actionFunc = BgHidanCurtain_WaitForTimer;
        }
    }
}

void BgHidanCurtain_WaitForTimer(BgHidanCurtain* thisv, GlobalContext* globalCtx) {
    DECR(thisv->timer);
    if (thisv->timer == 0) {
        thisv->actionFunc = BgHidanCurtain_TurnOn;
    }
    if ((thisv->type == 1) || (thisv->type == 3)) {
        func_8002F994(&thisv->actor, thisv->timer);
    }
}

void BgHidanCurtain_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgHidanCurtain* thisv = (BgHidanCurtain*)thisx;
    BgHidanCurtainParams* hcParams = &sHCParams[thisv->size];
    f32 riseProgress;

    if ((globalCtx->cameraPtrs[MAIN_CAM]->setting == CAM_SET_SLOW_CHEST_CS) ||
        (globalCtx->cameraPtrs[MAIN_CAM]->setting == CAM_SET_TURN_AROUND)) {
        thisv->collider.base.atFlags &= ~AT_HIT;
    } else {
        if (thisv->collider.base.atFlags & AT_HIT) {
            thisv->collider.base.atFlags &= ~AT_HIT;
            func_8002F71C(globalCtx, &thisv->actor, 5.0f, thisv->actor.yawTowardsPlayer, 1.0f);
        }
        if ((thisv->type == 4) || (thisv->type == 5)) {
            thisv->actor.world.pos.y = (2.0f * thisv->actor.home.pos.y) - hcParams->riseDist - thisv->actor.world.pos.y;
        }

        thisv->actionFunc(thisv, globalCtx);

        if ((thisv->type == 4) || (thisv->type == 5)) {
            thisv->actor.world.pos.y = (2.0f * thisv->actor.home.pos.y) - hcParams->riseDist - thisv->actor.world.pos.y;
        }
        riseProgress = (hcParams->riseDist - (thisv->actor.home.pos.y - thisv->actor.world.pos.y)) / hcParams->riseDist;
        thisv->alpha = 255.0f * riseProgress;
        if (thisv->alpha > 50) {
            thisv->collider.dim.height = hcParams->height * riseProgress;
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            if (gSaveContext.sceneSetupIndex <= 3) {
                func_8002F974(&thisv->actor, NA_SE_EV_FIRE_PILLAR_S - SFX_FLAG);
            }
        } else if ((thisv->type == 1) && Flags_GetTreasure(globalCtx, thisv->treasureFlag)) {
            Actor_Kill(&thisv->actor);
        }
        thisv->texScroll++;
    }
}

void BgHidanCurtain_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanCurtain* thisv = (BgHidanCurtain*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_curtain.c", 685);
    func_80093D84(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0, thisv->alpha);

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->texScroll & 0x7F, 0, 0x20, 0x40, 1, 0,
                                (thisv->texScroll * -0xF) & 0xFF, 0x20, 0x40));

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_hidan_curtain.c", 698),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_curtain.c", 702);
}
