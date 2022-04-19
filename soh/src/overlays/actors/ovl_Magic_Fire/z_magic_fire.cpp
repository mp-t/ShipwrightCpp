/*
 * File: z_magic_fire.c
 * Overlay: ovl_Magic_Fire
 * Description: Din's Fire
 */

#include "z_magic_fire.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void MagicFire_Init(Actor* thisvx, GlobalContext* globalCtx);
void MagicFire_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void MagicFire_Update(Actor* thisvx, GlobalContext* globalCtx);
void MagicFire_Draw(Actor* thisvx, GlobalContext* globalCtx);

void MagicFire_UpdateBeforeCast(Actor* thisvx, GlobalContext* globalCtx);

typedef enum {
    /* 0x00 */ DF_ACTION_INITIALIZE,
    /* 0x01 */ DF_ACTION_EXPAND_SLOWLY,
    /* 0x02 */ DF_ACTION_STOP_EXPANDING,
    /* 0x03 */ DF_ACTION_EXPAND_QUICKLY
} MagicFireAction;

typedef enum {
    /* 0x00 */ DF_SCREEN_TINT_NONE,
    /* 0x01 */ DF_SCREEN_TINT_FADE_IN,
    /* 0x02 */ DF_SCREEN_TINT_MAINTAIN,
    /* 0x03 */ DF_SCREEN_TINT_FADE_OUT,
    /* 0x04 */ DF_SCREEN_TINT_FINISHED
} MagicFireScreenTint;

ActorInit Magic_Fire_InitVars = {
    ACTOR_MAGIC_FIRE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(MagicFire),
    (ActorFunc)MagicFire_Init,
    (ActorFunc)MagicFire_Destroy,
    (ActorFunc)MagicFire_Update,
    (ActorFunc)MagicFire_Draw,
    NULL,
};

#include "overlays/ovl_Magic_Fire/ovl_Magic_Fire.h"

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00020000, 0x00, 0x01 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 9, 9, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_STOP),
};

static u8 sVertexIndices[] = {
    3,  4,  5,  6,  7,  8,  9,  10, 16, 17, 18, 19, 25, 26, 27, 32, 35, 36, 37, 38,
    39, 45, 46, 47, 52, 53, 54, 59, 60, 61, 67, 68, 69, 70, 71, 72, 0,  1,  11, 12,
    14, 20, 21, 23, 28, 30, 33, 34, 40, 41, 43, 48, 50, 55, 57, 62, 64, 65, 73, 74,
};

void MagicFire_Init(Actor* thisvx, GlobalContext* globalCtx) {
    MagicFire* thisv = (MagicFire*)thisvx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->action = 0;
    thisv->screenTintBehaviour = 0;
    thisv->actionTimer = 0;
    thisv->alphaMultiplier = -3.0f;
    Actor_SetScale(&thisv->actor, 0.0f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    thisv->actor.update = MagicFire_UpdateBeforeCast;
    thisv->actionTimer = 20;
    thisv->actor.room = -1;
}

void MagicFire_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    func_800876C8(globalCtx);
}

void MagicFire_UpdateBeforeCast(Actor* thisvx, GlobalContext* globalCtx) {
    MagicFire* thisv = (MagicFire*)thisvx;
    Player* player = GET_PLAYER(globalCtx);

    if ((globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) ||
        (globalCtx->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->actionTimer > 0) {
        thisv->actionTimer--;
    } else {
        thisv->actor.update = MagicFire_Update;
        func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_FIRE);
    }
    thisv->actor.world.pos = player->actor.world.pos;
}

void MagicFire_Update(Actor* thisvx, GlobalContext* globalCtx) {
    MagicFire* thisv = (MagicFire*)thisvx;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    if (1) {}
    thisv->actor.world.pos = player->actor.world.pos;
    if ((globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) ||
        (globalCtx->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->action == DF_ACTION_EXPAND_SLOWLY) {
        thisv->collider.info.toucher.damage = thisv->actionTimer + 25;
    } else if (thisv->action == DF_ACTION_STOP_EXPANDING) {
        thisv->collider.info.toucher.damage = thisv->actionTimer;
    }
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    thisv->collider.dim.radius = (thisv->actor.scale.x * 325.0f);
    thisv->collider.dim.height = (thisv->actor.scale.y * 450.0f);
    thisv->collider.dim.yShift = (thisv->actor.scale.y * -225.0f);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    switch (thisv->action) {
        case DF_ACTION_INITIALIZE:
            thisv->actionTimer = 30;
            thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z = 0.0f;
            thisv->actor.world.rot.x = thisv->actor.world.rot.y = thisv->actor.world.rot.z = 0;
            thisv->actor.shape.rot.x = thisv->actor.shape.rot.y = thisv->actor.shape.rot.z = 0;
            thisv->alphaMultiplier = 0.0f;
            thisv->scalingSpeed = 0.08f;
            thisv->action++;
            break;
        case DF_ACTION_EXPAND_SLOWLY: // Fire sphere slowly expands out of player for 30 frames
            Math_StepToF(&thisv->alphaMultiplier, 1.0f, 1.0f / 30.0f);
            if (thisv->actionTimer > 0) {
                Math_SmoothStepToF(&thisv->actor.scale.x, 0.4f, thisv->scalingSpeed, 0.1f, 0.001f);
                thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;
            } else {
                thisv->actionTimer = 25;
                thisv->action++;
            }
            break;
        case DF_ACTION_STOP_EXPANDING: // Sphere stops expanding and maintains size for 25 frames
            if (thisv->actionTimer <= 0) {
                thisv->actionTimer = 15;
                thisv->action++;
                thisv->scalingSpeed = 0.05f;
            }
            break;
        case DF_ACTION_EXPAND_QUICKLY: // Sphere beings to grow again and quickly expands out until killed
            thisv->alphaMultiplier -= 8.0f / 119.00001f;
            thisv->actor.scale.x += thisv->scalingSpeed;
            thisv->actor.scale.y += thisv->scalingSpeed;
            thisv->actor.scale.z += thisv->scalingSpeed;
            if (thisv->alphaMultiplier <= 0.0f) {
                thisv->action = 0;
                Actor_Kill(&thisv->actor);
            }
            break;
    }
    switch (thisv->screenTintBehaviour) {
        case DF_SCREEN_TINT_NONE:
            if (thisv->screenTintBehaviourTimer <= 0) {
                thisv->screenTintBehaviourTimer = 20;
                thisv->screenTintBehaviour = DF_SCREEN_TINT_FADE_IN;
            }
            break;
        case DF_SCREEN_TINT_FADE_IN:
            thisv->screenTintIntensity = 1.0f - (thisv->screenTintBehaviourTimer / 20.0f);
            if (thisv->screenTintBehaviourTimer <= 0) {
                thisv->screenTintBehaviourTimer = 45;
                thisv->screenTintBehaviour = DF_SCREEN_TINT_MAINTAIN;
            }
            break;
        case DF_SCREEN_TINT_MAINTAIN:
            if (thisv->screenTintBehaviourTimer <= 0) {
                thisv->screenTintBehaviourTimer = 5;
                thisv->screenTintBehaviour = DF_SCREEN_TINT_FADE_OUT;
            }
            break;
        case DF_SCREEN_TINT_FADE_OUT:
            thisv->screenTintIntensity = (thisv->screenTintBehaviourTimer / 5.0f);
            if (thisv->screenTintBehaviourTimer <= 0) {
                thisv->screenTintBehaviour = DF_SCREEN_TINT_FINISHED;
            }
            break;
    }
    if (thisv->actionTimer > 0) {
        thisv->actionTimer--;
    }
    if (thisv->screenTintBehaviourTimer > 0) {
        thisv->screenTintBehaviourTimer--;
    }
}

void MagicFire_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    MagicFire* thisv = (MagicFire*)thisvx;
    s32 pad1;
    u32 gameplayFrames = globalCtx->gameplayFrames;
    s32 pad2;
    s32 i;
    u8 alpha;
    
    if (thisv->action > 0) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_magic_fire.c", 682);
        POLY_XLU_DISP = func_800937C0(POLY_XLU_DISP);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(s32)(60 * thisv->screenTintIntensity),
                        (u8)(s32)(20 * thisv->screenTintIntensity), (u8)(s32)(0 * thisv->screenTintIntensity),
                        (u8)(s32)(120 * thisv->screenTintIntensity));
        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
        gDPFillRectangle(POLY_XLU_DISP++, 0, 0, 319, 239);
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, 255, 200, 0, (u8)(thisv->alphaMultiplier * 255));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, (u8)(thisv->alphaMultiplier * 255));
        Matrix_Scale(0.15f, 0.15f, 0.15f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_magic_fire.c", 715),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        gDPLoadTextureBlock(POLY_XLU_DISP++, sTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, 6, 6, 15, G_TX_NOLOD);
        gDPSetTile(POLY_XLU_DISP++, G_IM_FMT_I, G_IM_SIZ_8b, 8, 0, 1, 0, G_TX_NOMIRROR | G_TX_WRAP, 6, 14,
                   G_TX_NOMIRROR | G_TX_WRAP, 6, 14);
        gDPSetTileSize(POLY_XLU_DISP++, 1, 0, 0, 252, 252);
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (gameplayFrames * 2) % 512,
                                        511 - ((gameplayFrames * 5) % 512), 64, 64, 1, (gameplayFrames * 2) % 256,
                                        255 - ((gameplayFrames * 20) % 256), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, sModelDL);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_magic_fire.c", 750);

        alpha = (s32)(thisv->alphaMultiplier * 255);
        Vtx* vertices = ResourceMgr_LoadVtxByName(sSphereVtx);
        for (i = 0; i < 36; i++) {
            vertices[sVertexIndices[i]].n.a = alpha;
        }

        alpha = (s32)(thisv->alphaMultiplier * 76);
        for (i = 36; i < 60; i++) {
            vertices[sVertexIndices[i]].n.a = alpha;
        }
    }
}
