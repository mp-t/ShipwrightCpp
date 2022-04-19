/*
 * File: z_en_nb.c
 * Overlay: ovl_En_Nb
 * Description: Nabooru
 */

#include "z_en_nb.h"
#include "vt.h"
#include "objects/object_nb/object_nb.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x00 */ NB_CHAMBER_INIT,
    /* 0x01 */ NB_CHAMBER_UNDERGROUND,
    /* 0x02 */ NB_CHAMBER_APPEAR,
    /* 0x03 */ NB_CHAMBER_IDLE,
    /* 0x04 */ NB_CHAMBER_RAISE_ARM,
    /* 0x05 */ NB_CHAMBER_RAISE_ARM_TRANSITION,
    /* 0x06 */ NB_GIVE_MEDALLION,
    /* 0x07 */ NB_ACTION_7,
    /* 0x08 */ NB_SEAL_HIDE,
    /* 0x09 */ NB_ACTION_9,
    /* 0x0A */ NB_KIDNAPPED,
    /* 0x0B */ NB_KIDNAPPED_LOOK_AROUND,
    /* 0x0C */ NB_PORTAL_FALLTHROUGH,
    /* 0x0D */ NB_IN_CONFRONTATION,
    /* 0x0E */ NB_ACTION_14,
    /* 0x0F */ NB_KNEEL,
    /* 0x10 */ NB_LOOK_RIGHT,
    /* 0x11 */ NB_LOOK_LEFT,
    /* 0x12 */ NB_RUN,
    /* 0x13 */ NB_CONFRONTATION_DESTROYED,
    /* 0x14 */ NB_CREDITS_INIT,
    /* 0x15 */ NB_CREDITS_FADEIN,
    /* 0x16 */ NB_CREDITS_SIT,
    /* 0x17 */ NB_CREDITS_HEAD_TURN,
    /* 0x18 */ NB_CROUCH_CRAWLSPACE,
    /* 0x19 */ NB_NOTICE_PLAYER,
    /* 0x1A */ NB_IDLE_CRAWLSPACE,
    /* 0x1B */ NB_IN_DIALOG,
    /* 0x1C */ NB_IN_PATH,
    /* 0x1D */ NB_IDLE_AFTER_TALK,
    /* 0x1E */ NB_ACTION_30
} EnNbAction;

typedef enum {
    /* 0x00 */ NB_DRAW_NOTHING,
    /* 0x01 */ NB_DRAW_DEFAULT,
    /* 0x02 */ NB_DRAW_HIDE,
    /* 0x03 */ NB_DRAW_KNEEL,
    /* 0x04 */ NB_DRAW_LOOK_DIRECTION
} EnNbDrawMode;

void EnNb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnNb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnNb_Update(Actor* thisx, GlobalContext* globalCtx);
void EnNb_Draw(Actor* thisx, GlobalContext* globalCtx);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static const void* sEyeTextures[] = {
    gNabooruEyeOpenTex,
    gNabooruEyeHalfTex,
    gNabooruEyeClosedTex,
};

static s32 D_80AB4318 = 0;

#include "z_en_nb_cutscene_data.cpp" EARLY

s32 EnNb_GetPath(EnNb* thisv) {
    s32 path = thisv->actor.params >> 8;

    return path & 0xFF;
}

s32 EnNb_GetType(EnNb* thisv) {
    s32 type = thisv->actor.params;

    return type & 0xFF;
}

void EnNb_UpdatePath(EnNb* thisv, GlobalContext* globalCtx) {
    Vec3s* pointPos;
    Path* pathList;
    s32 pad;
    s32 path;

    pathList = globalCtx->setupPathList;

    if (pathList != NULL) {
        path = EnNb_GetPath(thisv);
        pathList += path;
        pointPos = SEGMENTED_TO_VIRTUAL(pathList->points);
        thisv->initialPos.x = pointPos[0].x;
        thisv->initialPos.y = pointPos[0].y;
        thisv->initialPos.z = pointPos[0].z;
        thisv->finalPos.x = pointPos[1].x;
        thisv->finalPos.y = pointPos[1].y;
        thisv->finalPos.z = pointPos[1].z;
        thisv->pathYaw = (Math_FAtan2F(thisv->finalPos.x - thisv->initialPos.x, thisv->finalPos.z - thisv->initialPos.z) *
                         (0x8000 / std::numbers::pi_v<float>));
        // "En_Nb_Get_path_info Rail Data Get! = %d!!!!!!!!!!!!!!"
        osSyncPrintf("En_Nb_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", path);
    } else {
        // "En_Nb_Get_path_info Rail Data Doesn't Exist!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf("En_Nb_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

void EnNb_SetupCollider(Actor* thisx, GlobalContext* globalCtx) {
    EnNb* thisv = (EnNb*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, thisx, &sCylinderInit);
}

void EnNb_UpdateCollider(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad[4];
    ColliderCylinder* collider = &thisv->collider;

    Collider_UpdateCylinder(&thisv->actor, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &collider->base);
}

void EnNb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnNb* thisv = (EnNb*)thisx;
    
    D_80AB4318 = 0;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80AB0FBC(EnNb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_300.unk_18 = player->actor.world.pos;
    thisv->unk_300.unk_14 = kREG(16) + 9.0f;
    func_80034A14(&thisv->actor, &thisv->unk_300, kREG(17) + 0xC, 2);
}

void func_80AB1040(EnNb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_300.unk_18 = player->actor.world.pos;
    thisv->unk_300.unk_14 = kREG(16) + 9.0f;
    func_80034A14(&thisv->actor, &thisv->unk_300, kREG(17) + 0xC, 4);
}

void func_80AB10C4(EnNb* thisv) {
    s32 pad2[2];
    Vec3s* tempPtr;
    Vec3s* tempPtr2;

    tempPtr = &thisv->unk_300.unk_08;
    Math_SmoothStepToS(&tempPtr->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&tempPtr->y, 0, 20, 6200, 100);
    tempPtr2 = &thisv->unk_300.unk_0E;
    Math_SmoothStepToS(&tempPtr2->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&tempPtr2->y, 0, 20, 6200, 100);
}

void EnNb_UpdateEyes(EnNb* thisv) {
    s32 pad[3];
    s16* blinkTimer = &thisv->blinkTimer;
    s16* eyeIdx = &thisv->eyeIdx;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIdx = *blinkTimer;
    if (*eyeIdx >= ARRAY_COUNT(sEyeTextures)) {
        *eyeIdx = 0;
    }
}

void func_80AB11EC(EnNb* thisv) {
    thisv->action = NB_ACTION_7;
    thisv->drawMode = NB_DRAW_NOTHING;
    thisv->alpha = 0;
    thisv->flag = 0;
    thisv->actor.shape.shadowAlpha = 0;
    thisv->alphaTimer = 0.0f;
}

void func_80AB1210(EnNb* thisv, GlobalContext* globalCtx) {
    s32 one; // required to match

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        if (D_80AB4318) {
            if (thisv->actor.params == NB_TYPE_DEMO02) {
                func_80AB11EC(thisv);
            }

            D_80AB4318 = 0;
        }
    } else {
        one = 1;
        if (!D_80AB4318) {
            D_80AB4318 = one;
        }
    }
}

void func_80AB1284(EnNb* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 4);
}

s32 EnNb_UpdateSkelAnime(EnNb* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

const CsCmdActorAction* EnNb_GetNpcCsAction(GlobalContext* globalCtx, s32 npcActionIdx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[npcActionIdx];
    }
    return NULL;
}

void EnNb_SetupCsPosRot(EnNb* thisv, GlobalContext* globalCtx, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, npcActionIdx);
    s16 newRotY;
    Actor* thisx = &thisv->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        thisx->world.rot.y = thisx->shape.rot.y = csCmdNPCAction->rot.y;
    }
}

s32 func_80AB1390(EnNb* thisv, GlobalContext* globalCtx, u16 arg2, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction;

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) &&
        (csCmdNPCAction = globalCtx->csCtx.npcActions[npcActionIdx], csCmdNPCAction != NULL) &&
        (csCmdNPCAction->action == arg2)) {
        return true;
    }
    return false;
}

s32 func_80AB13D8(EnNb* thisv, GlobalContext* globalCtx, u16 arg2, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction;

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) &&
        (csCmdNPCAction = globalCtx->csCtx.npcActions[npcActionIdx], csCmdNPCAction != NULL) &&
        (csCmdNPCAction->action != arg2)) {
        return true;
    }
    return false;
}

void EnNb_SetInitialCsPosRot(EnNb* thisv, GlobalContext* globalCtx, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, npcActionIdx);
    Actor* thisx = &thisv->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        thisx->world.rot.y = thisx->shape.rot.y = csCmdNPCAction->rot.y;
    }
}

void EnNb_SetCurrentAnim(EnNb* thisv, const AnimationHeader* animation, u8 mode, f32 transitionRate, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (!arg4) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animation, playbackSpeed, unk0, fc, mode, transitionRate);
}

void EnNb_SetChamberAnim(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetCurrentAnim(thisv, &gNabooruStandingHandsOnHipsChamberOfSagesAnim, 0, 0, 0);
    thisv->actor.shape.yOffset = -10000.0f;
}

void EnNb_SpawnBlueWarp(EnNb* thisv, GlobalContext* globalCtx) {
    f32 posX = thisv->actor.world.pos.x;
    f32 posY = thisv->actor.world.pos.y;
    f32 posZ = thisv->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void EnNb_GiveMedallion(EnNb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       0xC);
    Item_Give(globalCtx, ITEM_MEDALLION_SPIRIT);
}

void EnNb_ComeUpImpl(EnNb* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.yOffset += 250.0f / 3.0f;
}

void EnNb_SetupChamberCsImpl(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == 3) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(globalCtx);
        thisv->action = NB_CHAMBER_UNDERGROUND;
        globalCtx->csCtx.segment = &D_80AB431C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(globalCtx, ITEM_MEDALLION_SPIRIT);
        player->actor.world.rot.y = player->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
    }
}

void EnNb_SetupChamberWarpImpl(EnNb* thisv, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;
    const CsCmdActorAction* csCmdNPCAction;

    if (csCtx->state != CS_STATE_IDLE) {
        csCmdNPCAction = csCtx->npcActions[1];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 2) {
            thisv->action = NB_CHAMBER_APPEAR;
            thisv->drawMode = NB_DRAW_DEFAULT;
            EnNb_SpawnBlueWarp(thisv, globalCtx);
        }
    }
}

void EnNb_SetupDefaultChamberIdle(EnNb* thisv) {
    if (thisv->actor.shape.yOffset >= 0.0f) {
        thisv->action = NB_CHAMBER_IDLE;
        thisv->actor.shape.yOffset = 0.0f;
    }
}

void EnNb_SetupArmRaise(EnNb* thisv, GlobalContext* globalCtx) {
    const AnimationHeader* animation = &gNabooruRaisingArmsGivingMedallionAnim;
    const CsCmdActorAction* csCmdNPCAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = globalCtx->csCtx.npcActions[1];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 3) {
            Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            thisv->action = NB_CHAMBER_RAISE_ARM;
        }
    }
}

void EnNb_SetupRaisedArmTransition(EnNb* thisv, s32 animFinished) {
    const AnimationHeader* animation = &gNabooruArmsRaisedGivingMedallionAnim;

    if (animFinished) {
        Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        thisv->action = NB_CHAMBER_RAISE_ARM_TRANSITION;
    }
}

void EnNb_SetupMedallion(EnNb* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = globalCtx->csCtx.npcActions[6];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 2) {
            thisv->action = NB_GIVE_MEDALLION;
            EnNb_GiveMedallion(thisv, globalCtx);
        }
    }
}

// Action func is never explicitly set to thisv, but it runs when the memory gets zero cleared
void EnNb_SetupChamberCs(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetupChamberCsImpl(thisv, globalCtx);
}

void EnNb_SetupChamberWarp(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetupChamberWarpImpl(thisv, globalCtx);
}

void EnNb_ComeUp(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_ComeUpImpl(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupDefaultChamberIdle(thisv);
}

void func_80AB193C(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupArmRaise(thisv, globalCtx);
}

void EnNb_RaiseArm(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    func_80AB1284(thisv, globalCtx);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupRaisedArmTransition(thisv, animFinished);
}

void func_80AB19BC(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupMedallion(thisv, globalCtx);
}

void func_80AB19FC(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
}

void EnNb_SetupLightArrowOrSealingCs(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetCurrentAnim(thisv, &gNabooruPuttingHandsTogetherCastingMagicAnim, 2, 0.0f, 0);
    thisv->action = NB_ACTION_7;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnNb_PlaySealingSound(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void EnNb_InitializeDemo6K(EnNb* thisv, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_6K, thisv->actor.world.pos.x,
                       kREG(21) + 22.0f + thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 7);
}

void EnNb_SetupHide(EnNb* thisv, GlobalContext* globalCtx) {
    if (func_80AB1390(thisv, globalCtx, 4, 1)) {
        thisv->action = NB_SEAL_HIDE;
        thisv->drawMode = NB_DRAW_HIDE;
        thisv->alpha = 0;
        thisv->actor.shape.shadowAlpha = 0;
        thisv->alphaTimer = 0.0f;
        EnNb_PlaySealingSound();
    }
}

void EnNb_CheckToFade(EnNb* thisv, GlobalContext* globalCtx) {
    f32* alphaTimer = &thisv->alphaTimer;
    s32 alpha;

    if (func_80AB1390(thisv, globalCtx, 4, 1)) {
        *alphaTimer += 1.0f;
        if (*alphaTimer >= kREG(5) + 10.0f) {
            thisv->action = NB_ACTION_9;
            thisv->drawMode = NB_DRAW_DEFAULT;
            *alphaTimer = kREG(5) + 10.0f;
            thisv->alpha = 255;
            thisv->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *alphaTimer -= 1.0f;
        if (*alphaTimer <= 0.0f) {
            thisv->action = NB_ACTION_7;
            thisv->drawMode = NB_DRAW_NOTHING;
            *alphaTimer = 0.0f;
            thisv->alpha = 0;
            thisv->actor.shape.shadowAlpha = 0;
            return;
        }
    }

    alpha = (*alphaTimer / (kREG(5) + 10.0f)) * 255.0f;
    thisv->alpha = alpha;
    thisv->actor.shape.shadowAlpha = alpha;
}

void EnNb_SetupLightOrb(EnNb* thisv, GlobalContext* globalCtx) {
    if (func_80AB13D8(thisv, globalCtx, 4, 1)) {
        thisv->action = NB_SEAL_HIDE;
        thisv->drawMode = NB_DRAW_HIDE;
        thisv->alphaTimer = kREG(5) + 10.0f;
        thisv->alpha = 255;

        if (thisv->flag == 0) {
            EnNb_InitializeDemo6K(thisv, globalCtx);
            thisv->flag = 1;
        }

        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void EnNb_Hide(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetupHide(thisv, globalCtx);
    func_80AB1210(thisv, globalCtx);
}

void EnNb_Fade(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_CheckToFade(thisv, globalCtx);
    func_80AB1210(thisv, globalCtx);
}

void EnNb_CreateLightOrb(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupLightOrb(thisv, globalCtx);
    func_80AB1210(thisv, globalCtx);
}

void EnNb_DrawTransparency(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeSegIdx = thisv->eyeIdx;
    const void* eyeTex = sEyeTextures[eyeSegIdx];
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 263);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);
    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inKenjyanomaDemo02.c", 290);
}

void EnNb_InitKidnap(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetCurrentAnim(thisv, &gNabooruTrappedInVortexPushingGroundAnim, 0, 0.0f, 0);
    thisv->action = NB_KIDNAPPED;
    thisv->actor.shape.shadowAlpha = 0;
    gSaveContext.eventChkInf[9] |= 0x20;
}

void EnNb_PlayCrySFX(EnNb* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames == 3) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_NB_CRY_0);
    }
}

void EnNb_PlayAgonySFX(EnNb* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames == 420) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_NB_AGONY);
    }
}

void EnNb_SetPosInPortal(EnNb* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, 1);
    Vec3f* pos = &thisv->actor.world.pos;
    f32 f0;
    s32 pad;
    Vec3f startPos;
    Vec3f endPos;

    if (csCmdNPCAction != NULL) {
        f0 = Environment_LerpWeightAccelDecel(csCmdNPCAction->endFrame, csCmdNPCAction->startFrame,
                                              globalCtx->csCtx.frames, 4, 4);
        startPos.x = csCmdNPCAction->startPos.x;
        startPos.y = csCmdNPCAction->startPos.y;
        startPos.z = csCmdNPCAction->startPos.z;
        endPos.x = csCmdNPCAction->endPos.x;
        endPos.y = csCmdNPCAction->endPos.y;
        endPos.z = csCmdNPCAction->endPos.z;
        pos->x = ((endPos.x - startPos.x) * f0) + startPos.x;
        pos->y = ((endPos.y - startPos.y) * f0) + startPos.y;
        pos->z = ((endPos.z - startPos.z) * f0) + startPos.z;
    }
}

void EnNb_SetupCaptureCutsceneState(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetupCsPosRot(thisv, globalCtx, 1);
    thisv->action = NB_KIDNAPPED;
    thisv->drawMode = NB_DRAW_NOTHING;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnNb_SetRaisedArmCaptureAnim(EnNb* thisv, s32 animFinished) {
    const AnimationHeader* animation = &gNabooruSuckedByVortexAnim;

    if (animFinished) {
        Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupLookAroundInKidnap(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruTrappedInVortexPushingGroundAnim;

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP, -8.0f);
    thisv->action = NB_KIDNAPPED_LOOK_AROUND;
    thisv->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_SetupKidnap(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruTrappedInVortexRaisingArmAnim;

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, -8.0f);
    thisv->action = NB_PORTAL_FALLTHROUGH;
    thisv->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_CheckKidnapCsMode(EnNb* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, 1);
    s32 action;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        action = csCmdNPCAction->action;
        previousCsAction = thisv->previousCsAction;
        if (action != previousCsAction) {
            switch (action) {
                case 1:
                    EnNb_SetupCaptureCutsceneState(thisv, globalCtx);
                    break;
                case 7:
                    EnNb_SetupLookAroundInKidnap(thisv);
                    break;
                case 8:
                    EnNb_SetupKidnap(thisv);
                    break;
                case 9:
                    Actor_Kill(&thisv->actor);
                    break;
                default:
                    // "Operation Doesn't Exist!!!!!!!!"
                    osSyncPrintf("En_Nb_Kidnap_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            thisv->previousCsAction = action;
        }
    }
}

void func_80AB23A8(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_PlayCrySFX(thisv, globalCtx);
    EnNb_CheckKidnapCsMode(thisv, globalCtx);
}

void EnNb_MovingInPortal(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_PlayCrySFX(thisv, globalCtx);
    EnNb_PlayAgonySFX(thisv, globalCtx);
    EnNb_UpdateEyes(thisv);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_CheckKidnapCsMode(thisv, globalCtx);
}

void EnNb_SuckedInByPortal(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    EnNb_UpdateEyes(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_SetRaisedArmCaptureAnim(thisv, animFinished);
    EnNb_SetPosInPortal(thisv, globalCtx);
    EnNb_CheckKidnapCsMode(thisv, globalCtx);
}

void EnNb_SetupConfrontation(EnNb* thisv, GlobalContext* globalCtx) {
    const AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;

    EnNb_SetCurrentAnim(thisv, animation, 0, 0.0f, 0);
    thisv->action = NB_IN_CONFRONTATION;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnNb_PlayKnuckleDefeatSFX(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad[2];

    if (globalCtx->csCtx.frames == 548) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_NB_CRY_0);
        func_80078914(&thisv->actor.projectedPos, NA_SE_EN_FANTOM_HIT_THUNDER);
    }
}

void EnNb_PlayKneelingOnGroundSFX(EnNb* thisv) {
    s32 pad[2];

    if ((thisv->skelAnime.mode == 2) &&
        (Animation_OnFrame(&thisv->skelAnime, 18.0f) || Animation_OnFrame(&thisv->skelAnime, 25.0f))) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_EV_HUMAN_BOUND);
    }
}

void EnNb_PlayLookRightSFX(EnNb* thisv) {
    s32 pad[2];

    if ((thisv->skelAnime.mode == 2) && Animation_OnFrame(&thisv->skelAnime, 9.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_CONCRETE);
    }
}

void EnNb_PlayLookLeftSFX(EnNb* thisv) {
    s32 pad[2];

    if (Animation_OnFrame(&thisv->skelAnime, 9.0f) || Animation_OnFrame(&thisv->skelAnime, 13.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_CONCRETE);
    }
}

void EnNb_InitDemo6KInConfrontation(EnNb* thisv, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_6K, thisv->actor.world.pos.x,
                kREG(21) + 22.0f + thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0xB);
}

void func_80AB2688(EnNb* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80AB26C8(EnNb* thisv) {
    thisv->action = NB_IN_CONFRONTATION;
    thisv->drawMode = NB_DRAW_NOTHING;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_80AB26DC(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    const AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    EnNb_SetupCsPosRot(thisv, globalCtx, 1);
    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    thisv->action = NB_ACTION_14;
    thisv->drawMode = NB_DRAW_KNEEL;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupKneel(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    thisv->action = NB_KNEEL;
    thisv->drawMode = NB_DRAW_KNEEL;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfKneeling(EnNb* thisv, s32 animFinished) {
    const AnimationHeader* animation = &gNabooruOnAllFoursAnim;

    if (animFinished) {
        Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        thisv->drawMode = NB_DRAW_KNEEL;
    }
}

void EnNb_SetupLookRight(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruOnAllFoursToOnOneKneeLookingRightTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    thisv->action = NB_LOOK_RIGHT;
    thisv->drawMode = NB_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookingRight(EnNb* thisv, s32 animFinished) {
    const AnimationHeader* animation = &gNabooruOnOneKneeLookingRightAnim;

    if (animFinished) {
        Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        thisv->drawMode = NB_DRAW_LOOK_DIRECTION;
    }
}

void EnNb_SetupLookLeft(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruOnOneKneeTurningHeadRightToLeftTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    thisv->action = NB_LOOK_LEFT;
    thisv->drawMode = NB_DRAW_LOOK_DIRECTION;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookLeft(EnNb* thisv, s32 animFinished) {
    const AnimationHeader* animation = &gNabooruOnOneKneeLookingLeftAnim;

    if (animFinished) {
        Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupDemo6KInConfrontation(EnNb* thisv, GlobalContext* globalCtx, s32 animFinished) {
    if (!thisv->flag && animFinished) {
        EnNb_InitDemo6KInConfrontation(thisv, globalCtx);
        thisv->flag = 1;
    }
}

void EnNb_SetupRun(EnNb* thisv) {
    const AnimationHeader* animation = &gNabooruKneeingToRunningToHitAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&thisv->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    thisv->action = NB_RUN;
    thisv->drawMode = NB_DRAW_LOOK_DIRECTION;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupConfrontationDestroy(EnNb* thisv) {
    thisv->action = NB_CONFRONTATION_DESTROYED;
    thisv->drawMode = NB_DRAW_NOTHING;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnNb_CheckConfrontationCsMode(EnNb* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction;
    s32 csAction;
    s32 previousCsAction;

    csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, 1);
    if (csCmdNPCAction != NULL) {
        csAction = csCmdNPCAction->action;
        previousCsAction = thisv->previousCsAction;

        if (csAction != previousCsAction) {
            switch (csAction) {
                case 1:
                    func_80AB26C8(thisv);
                    break;
                case 10:
                    func_80AB26DC(thisv, globalCtx);
                    break;
                case 11:
                    EnNb_SetupKneel(thisv);
                    break;
                case 12:
                    EnNb_SetupLookRight(thisv);
                    break;
                case 13:
                    EnNb_SetupLookLeft(thisv);
                    break;
                case 14:
                    EnNb_SetupRun(thisv);
                    break;
                case 9:
                    EnNb_SetupConfrontationDestroy(thisv);
                    break;
                default:
                    // "En_Nb_Confrontion_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    osSyncPrintf("En_Nb_Confrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            thisv->previousCsAction = csAction;
        }
    }
}

void EnNb_CheckConfrontationCsModeWrapper(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void func_80AB2C18(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_UpdateEyes(thisv);
    func_80AB2688(thisv, globalCtx);
    func_80AB1284(thisv, globalCtx);
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void EnNb_Kneel(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    EnNb_UpdateEyes(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_CheckIfKneeling(thisv, animFinished);
    EnNb_PlayKneelingOnGroundSFX(thisv);
    func_80AB2688(thisv, globalCtx);
    func_80AB1284(thisv, globalCtx);
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void EnNb_LookRight(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    EnNb_UpdateEyes(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_CheckIfLookingRight(thisv, animFinished);
    EnNb_PlayLookRightSFX(thisv);
    func_80AB2688(thisv, globalCtx);
    func_80AB1284(thisv, globalCtx);
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void EnNb_LookLeft(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    EnNb_UpdateEyes(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_CheckIfLookLeft(thisv, animFinished);
    func_80AB2688(thisv, globalCtx);
    func_80AB1284(thisv, globalCtx);
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void EnNb_Run(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    EnNb_PlayKnuckleDefeatSFX(thisv, globalCtx);
    EnNb_UpdateEyes(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_PlayLookLeftSFX(thisv);
    func_80AB2688(thisv, globalCtx);
    func_80AB1284(thisv, globalCtx);
    EnNb_SetupDemo6KInConfrontation(thisv, globalCtx, animFinished);
    EnNb_CheckConfrontationCsMode(thisv, globalCtx);
}

void EnNb_ConfrontationDestroy(EnNb* thisv, GlobalContext* globalCtx) {
    thisv->timer++;

    if (thisv->timer > 60.0f) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AB2E70(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inConfrontion.c", 572);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inConfrontion.c", 593);
}

s32 func_80AB2FC0(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* thisv = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        *dList = gNabooruHeadMouthOpenDL;
    }

    return 0;
}

void func_80AB2FE4(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 eyeIdx = thisv->eyeIdx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inConfrontion.c", 623);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, func_80AB2FC0,
                          NULL, &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_nb_inConfrontion.c", 644);
}

void EnNb_SetupCreditsSpawn(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetCurrentAnim(thisv, &gNabooruSittingCrossLeggedAnim, 0, 0.0f, 0);
    thisv->action = NB_CREDITS_INIT;
    thisv->drawMode = NB_DRAW_NOTHING;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnNb_SetAlphaInCredits(EnNb* thisv) {
    f32* alphaTimer = &thisv->alphaTimer;
    s32 alpha;

    thisv->alphaTimer++;

    if ((kREG(17) + 10.0f) <= thisv->alphaTimer) {
        thisv->alpha = 255;
        thisv->actor.shape.shadowAlpha = 255;
    } else {
        alpha = (*alphaTimer / (kREG(17) + 10.0f)) * 255.0f;
        thisv->alpha = alpha;
        thisv->actor.shape.shadowAlpha = alpha;
    }
}

void EnNb_SetupCreditsFadeIn(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetInitialCsPosRot(thisv, globalCtx, 1);
    thisv->action = NB_CREDITS_FADEIN;
    thisv->drawMode = NB_DRAW_HIDE;
}

void EnNb_SetupCreditsSit(EnNb* thisv) {
    if (thisv->alphaTimer >= kREG(17) + 10.0f) {
        thisv->action = NB_CREDITS_SIT;
        thisv->drawMode = NB_DRAW_DEFAULT;
    }
}

void EnNb_SetupCreditsHeadTurn(EnNb* thisv) {
    EnNb_SetCurrentAnim(thisv, &gNabooruSittingCrossLeggedTurningToLookUpRightTransitionAnim, 2, -8.0f, 0);
    thisv->action = NB_CREDITS_HEAD_TURN;
}

void EnNb_CheckIfLookingUp(EnNb* thisv, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(thisv, &gNabooruSittingCrossLeggedLookingUpRightAnim, 0, 0.0f, 0);
    }
}

void EnNb_CheckCreditsCsModeImpl(EnNb* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(globalCtx, 1);
    s32 action;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        action = csCmdNPCAction->action;
        previousCsAction = thisv->previousCsAction;
        if (action != previousCsAction) {
            switch (action) {
                case 15:
                    EnNb_SetupCreditsFadeIn(thisv, globalCtx);
                    break;
                case 16:
                    EnNb_SetupCreditsHeadTurn(thisv);
                    break;
                default:
                    // "En_Nb_inEnding_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    osSyncPrintf("En_Nb_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            thisv->previousCsAction = action;
        }
    }
}

void EnNb_CheckCreditsCsMode(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_CheckCreditsCsModeImpl(thisv, globalCtx);
}

void EnNb_CreditsFade(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetAlphaInCredits(thisv);
    EnNb_SetupCreditsSit(thisv);
}

void func_80AB3428(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_CheckCreditsCsModeImpl(thisv, globalCtx);
}

void EnNb_LookUp(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    func_80AB1284(thisv, globalCtx);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_CheckIfLookingUp(thisv, animFinished);
}

void EnNb_CrawlspaceSpawnCheck(EnNb* thisv, GlobalContext* globalCtx) {
    if (!(gSaveContext.eventChkInf[9] & 0x20) && LINK_IS_CHILD) {
        EnNb_UpdatePath(thisv, globalCtx);

        // looking into crawlspace
        if (!(gSaveContext.eventChkInf[9] & 0x10)) {
            EnNb_SetCurrentAnim(thisv, &gNabooruKneeingAtCrawlspaceAnim, 0, 0.0f, 0);
            thisv->action = NB_CROUCH_CRAWLSPACE;
            thisv->drawMode = NB_DRAW_DEFAULT;
        } else {
            EnNb_SetCurrentAnim(thisv, &gNabooruStandingHandsOnHipsAnim, 0, 0.0f, 0);
            thisv->headTurnFlag = 1;
            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
            thisv->actor.world.pos = thisv->finalPos;
            thisv->action = NB_IDLE_AFTER_TALK;
            thisv->drawMode = NB_DRAW_DEFAULT;
        }
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AB359C(EnNb* thisv) {
    PosRot* world = &thisv->actor.world;
    Vec3f* initialPos = &thisv->initialPos;
    Vec3f* finalPos = &thisv->finalPos;
    f32 f0;
    u16 temp_t1;
    s16 temp_2;

    thisv->movementTimer++;
    temp_2 = kREG(17);
    temp_t1 = temp_2;
    temp_t1 += 25;

    if (temp_t1 >= thisv->movementTimer) {
        f0 = Environment_LerpWeightAccelDecel(temp_t1, 0, thisv->movementTimer, 3, 3);
        world->pos.x = initialPos->x + (f0 * (finalPos->x - initialPos->x));
        world->pos.y = initialPos->y + (f0 * (finalPos->y - initialPos->y));
        world->pos.z = initialPos->z + (f0 * (finalPos->z - initialPos->z));
    }
}

void EnNb_SetNoticeSFX(EnNb* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_NB_NOTICE);
}

s32 EnNb_GetNoticedStatus(EnNb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerX = player->actor.world.pos.x;
    f32 playerZ = player->actor.world.pos.z;
    f32 thisX = thisv->actor.world.pos.x;
    f32 thisZ = thisv->actor.world.pos.z;

    if (SQ(playerX - thisX) + SQ(playerZ - thisZ) < SQ(80.0f)) {
        return true;
    } else {
        return false;
    }
}

void func_80AB36DC(EnNb* thisv, GlobalContext* globalCtx) {
    u16 moveTime = thisv->movementTimer;

    if ((((u16)((u16)(kREG(17) + 25) - 4))) > moveTime) {
        s16 invScale = 4 - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->pathYaw, invScale, 6200, 100);
        }
    } else {
        s16 invScale = (u16)(kREG(17) + 25) - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, invScale, 6200, 100);
        }
    }
}

void EnNb_CheckNoticed(EnNb* thisv, GlobalContext* globalCtx) {
    if (EnNb_GetNoticedStatus(thisv, globalCtx)) {
        EnNb_SetCurrentAnim(thisv, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
        thisv->action = NB_NOTICE_PLAYER;
        EnNb_SetNoticeSFX(thisv);
    }
}

void EnNb_SetupIdleCrawlspace(EnNb* thisv, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(thisv, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        thisv->headTurnFlag = 1;
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->action = NB_IDLE_CRAWLSPACE;
    }
}

void func_80AB3838(EnNb* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = NB_IN_DIALOG;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;

        if (!(gSaveContext.infTable[22] & 0x1000)) {
            thisv->actor.textId = 0x601D;
        } else {
            thisv->actor.textId = 0x6024;
        }

        func_8002F2F4(&thisv->actor, globalCtx);
    }
}

void EnNb_SetupPathMovement(EnNb* thisv, GlobalContext* globalCtx) {
    EnNb_SetCurrentAnim(thisv, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
    gSaveContext.eventChkInf[9] |= 0x10;
    thisv->action = NB_IN_PATH;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
}

void EnNb_SetTextIdAsChild(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    u8 choiceIndex;
    s32 pad1;
    u16 textId;

    textId = thisv->actor.textId;

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (textId == 0x6025) {
            EnNb_SetupPathMovement(thisv, globalCtx);
        } else {
            if (textId == 0x6027) {
                gSaveContext.infTable[22] |= 0x1000;
            }
            thisv->action = NB_IDLE_CRAWLSPACE;
        }
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    } else if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        choiceIndex = globalCtx->msgCtx.choiceIndex;

        if (textId == 0x601D) {
            switch (choiceIndex) {
                case 0:
                    thisv->actor.textId = 0x601E;
                    break;
                case 1:
                    thisv->actor.textId = 0x601F;
                    break;
                default:
                    thisv->actor.textId = 0x6020;
            }
        } else if (textId == 0x6020) {
            switch (choiceIndex) {
                case 0:
                    thisv->actor.textId = 0x6021;
                    break;
                default:
                    thisv->actor.textId = 0x6022;
                    break;
            }
        } else {
            switch (choiceIndex) {
                case 0:
                    thisv->actor.textId = 0x6025;
                    break;
                default:
                    thisv->actor.textId = 0x6027;
                    break;
            }
        }

        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
    }
}

void func_80AB3A7C(EnNb* thisv, GlobalContext* globalCtx, s32 animFinished) {
    u16 movementTimer = thisv->movementTimer;

    if ((u16)(kREG(17) + 25) > movementTimer) {
        if (animFinished) {
            EnNb_SetCurrentAnim(thisv, &gNabooruWalkingAnim, 0, 0.0f, 0);
        }
    } else {
        EnNb_SetCurrentAnim(thisv, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        thisv->action = NB_IDLE_AFTER_TALK;
    }
}

void func_80AB3B04(EnNb* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = NB_ACTION_30;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0x23);

        if ((thisv->actor.textId) == 0) {
            thisv->actor.textId = 0x6026;
        }

        func_8002F2F4(&thisv->actor, globalCtx);
    }
}

void func_80AB3B7C(EnNb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->action = NB_IDLE_AFTER_TALK;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    }
}

void EnNb_WaitForNotice(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_CheckNoticed(thisv, globalCtx);
}

void EnNb_StandUpAfterNotice(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetupIdleCrawlspace(thisv, animFinished);
}

void EnNb_BlockCrawlspace(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    func_80AB0FBC(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    func_80AB3838(thisv, globalCtx);
}

void EnNb_InitCrawlspaceDialogue(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    func_80AB0FBC(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    EnNb_SetTextIdAsChild(thisv, globalCtx);
}

void EnNb_FollowPath(EnNb* thisv, GlobalContext* globalCtx) {
    s32 animFinished;

    func_80AB359C(thisv);
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    func_80AB36DC(thisv, globalCtx);
    func_80AB10C4(thisv);
    animFinished = EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    func_80AB3A7C(thisv, globalCtx, animFinished);
}

void func_80AB3DB0(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    func_80AB0FBC(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    func_80AB3B04(thisv, globalCtx);
}

void func_80AB3E10(EnNb* thisv, GlobalContext* globalCtx) {
    func_80AB1284(thisv, globalCtx);
    EnNb_UpdateCollider(thisv, globalCtx);
    func_80AB1040(thisv, globalCtx);
    EnNb_UpdateSkelAnime(thisv);
    EnNb_UpdateEyes(thisv);
    func_80AB3B7C(thisv, globalCtx);
}

static EnNbActionFunc sActionFuncs[] = {
    EnNb_SetupChamberCs,
    EnNb_SetupChamberWarp,
    EnNb_ComeUp,
    func_80AB193C,
    EnNb_RaiseArm,
    func_80AB19BC,
    func_80AB19FC,
    EnNb_Hide,
    EnNb_Fade,
    EnNb_CreateLightOrb,
    func_80AB23A8,
    EnNb_MovingInPortal,
    EnNb_SuckedInByPortal,
    EnNb_CheckConfrontationCsModeWrapper,
    func_80AB2C18,
    EnNb_Kneel,
    EnNb_LookRight,
    EnNb_LookLeft,
    EnNb_Run,
    EnNb_ConfrontationDestroy,
    EnNb_CheckCreditsCsMode,
    EnNb_CreditsFade,
    func_80AB3428,
    EnNb_LookUp,
    EnNb_WaitForNotice,
    EnNb_StandUpAfterNotice,
    EnNb_BlockCrawlspace,
    EnNb_InitCrawlspaceDialogue,
    EnNb_FollowPath,
    func_80AB3DB0,
    func_80AB3E10,
};

void EnNb_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnNb* thisv = (EnNb*)thisx;

    if (thisv->action < 0 || thisv->action > 30 || sActionFuncs[thisv->action] == NULL) {
        // "Main mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sActionFuncs[thisv->action](thisv, globalCtx);
}

void EnNb_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnNb* thisv = (EnNb*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnNb_SetupCollider(thisx, globalCtx);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gNabooruSkel, NULL, thisv->jointTable, thisv->morphTable,
                       NB_LIMB_MAX);

    switch (EnNb_GetType(thisv)) {
        case NB_TYPE_DEMO02:
            EnNb_SetupLightArrowOrSealingCs(thisv, globalCtx);
            break;
        case NB_TYPE_KIDNAPPED:
            EnNb_InitKidnap(thisv, globalCtx);
            break;
        case NB_TYPE_KNUCKLE:
            EnNb_SetupConfrontation(thisv, globalCtx);
            break;
        case NB_TYPE_CREDITS:
            EnNb_SetupCreditsSpawn(thisv, globalCtx);
            break;
        case NB_TYPE_CRAWLSPACE:
            EnNb_CrawlspaceSpawnCheck(thisv, globalCtx);
            break;
        default: // giving medallion
            EnNb_SetChamberAnim(thisv, globalCtx);
            break;
    }
}

s32 EnNb_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* thisv = (EnNb*)thisx;
    struct_80034A14_arg1* unk_300 = &thisv->unk_300;
    s32 ret = false;

    if (thisv->headTurnFlag != 0) {
        if (limbIndex == NB_LIMB_TORSO) {
            rot->x += unk_300->unk_0E.y;
            rot->y -= unk_300->unk_0E.x;
            ret = false;
        } else if (limbIndex == NB_LIMB_HEAD) {
            rot->x += unk_300->unk_08.y;
            rot->z += unk_300->unk_08.x;
            ret = false;
        }
    }

    return ret;
}

void EnNb_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnNb* thisv = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        Vec3f vec1 = { 0.0f, 10.0f, 0.0f };
        Vec3f vec2;

        Matrix_MultVec3f(&vec1, &vec2);
        thisv->actor.focus.pos.x = vec2.x;
        thisv->actor.focus.pos.y = vec2.y;
        thisv->actor.focus.pos.z = vec2.z;
        thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
        thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
        thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    }
}

void EnNb_DrawNothing(EnNb* thisv, GlobalContext* globalCtx) {
}

void EnNb_DrawDefault(EnNb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 eyeIdx = thisv->eyeIdx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_nb.c", 992);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnNb_OverrideLimbDraw, EnNb_PostLimbDraw, &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_nb.c", 1013);
}

static EnNbDrawFunc sDrawFuncs[] = {
    EnNb_DrawNothing, EnNb_DrawDefault, EnNb_DrawTransparency, func_80AB2E70, func_80AB2FE4,
};

void EnNb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnNb* thisv = (EnNb*)thisx;

    if (thisv->drawMode < 0 || thisv->drawMode >= 5 || sDrawFuncs[thisv->drawMode] == NULL) {
        // "Draw mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sDrawFuncs[thisv->drawMode](thisv, globalCtx);
}

ActorInit En_Nb_InitVars = {
    ACTOR_EN_NB,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_NB,
    sizeof(EnNb),
    (ActorFunc)EnNb_Init,
    (ActorFunc)EnNb_Destroy,
    (ActorFunc)EnNb_Update,
    (ActorFunc)EnNb_Draw,
    NULL,
};
