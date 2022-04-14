/*
 * File: z_boss_sst.c
 * Overlay: ovl_Boss_Sst
 * Description: Bongo Bongo
 */

#include "z_boss_sst.h"
#include "objects/object_sst/object_sst.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_10)

#define vParity actionVar
#define vVanish actionVar

#define LEFT 0
#define RIGHT 1
#define OTHER_HAND(hand) ((BossSst*)hand->actor.child)
#define HAND_STATE(hand) sHandState[hand->actor.params]

#define ROOM_CENTER_X -50.0f
#define ROOM_CENTER_Y 0.0f
#define ROOM_CENTER_Z 0.0f

typedef enum {
    /*  0 */ HAND_WAIT,
    /*  1 */ HAND_BEAT,
    /*  2 */ HAND_RETREAT,
    /*  3 */ HAND_SLAM,
    /*  4 */ HAND_SWEEP,
    /*  5 */ HAND_PUNCH,
    /*  6 */ HAND_CLAP,
    /*  7 */ HAND_GRAB,
    /*  8 */ HAND_DAMAGED,
    /*  9 */ HAND_FROZEN,
    /* 10 */ HAND_BREAK_ICE,
    /* 11 */ HAND_DEATH
} BossSstHandState;

typedef enum {
    /* 0 */ BONGO_NULL,
    /* 1 */ BONGO_ICE,
    /* 2 */ BONGO_SHOCKWAVE,
    /* 3 */ BONGO_SHADOW
} BossSstEffectMode;

void BossSst_Init(Actor* thisx, GlobalContext* globalCtx);
void BossSst_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossSst_UpdateHand(Actor* thisx, GlobalContext* globalCtx);
void BossSst_UpdateHead(Actor* thisx, GlobalContext* globalCtx);
void BossSst_DrawHand(Actor* thisx, GlobalContext* globalCtx);
void BossSst_DrawHead(Actor* thisx, GlobalContext* globalCtx);
void BossSst_UpdateEffect(Actor* thisx, GlobalContext* globalCtx);
void BossSst_DrawEffect(Actor* thisx, GlobalContext* globalCtx);
void BossSst_Reset(void);


void BossSst_HeadSfx(BossSst* thisv, u16 sfxId);

void BossSst_HeadSetupLurk(BossSst* thisv);
void BossSst_HeadLurk(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupIntro(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadIntro(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupNeutral(BossSst* thisv);
void BossSst_HeadNeutral(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadWait(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HeadSetupDamagedHand(BossSst* thisv, s32 bothHands);
void BossSst_HeadDamagedHand(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupReadyCharge(BossSst* thisv);
void BossSst_HeadReadyCharge(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupCharge(BossSst* thisv);
void BossSst_HeadCharge(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupEndCharge(BossSst* thisv);
void BossSst_HeadEndCharge(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HeadSetupFrozenHand(BossSst* thisv);
void BossSst_HeadFrozenHand(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupUnfreezeHand(BossSst* thisv);
void BossSst_HeadUnfreezeHand(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HeadStunned(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupVulnerable(BossSst* thisv);
void BossSst_HeadVulnerable(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadDamage(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupRecover(BossSst* thisv);
void BossSst_HeadRecover(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HeadDeath(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupThrash(BossSst* thisv);
void BossSst_HeadThrash(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupDarken(BossSst* thisv);
void BossSst_HeadDarken(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupFall(BossSst* thisv);
void BossSst_HeadFall(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupMelt(BossSst* thisv);
void BossSst_HeadMelt(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HeadSetupFinish(BossSst* thisv);
void BossSst_HeadFinish(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandGrabPlayer(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandReleasePlayer(BossSst* thisv, GlobalContext* globalCtx, s32 dropPlayer);
void BossSst_HandSelectAttack(BossSst* thisv);
void BossSst_HandSetDamage(BossSst* thisv, s32 damage);
void BossSst_HandSetInvulnerable(BossSst* thisv, s32 isInv);

void BossSst_HandSetupWait(BossSst* thisv);
void BossSst_HandWait(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupDownbeat(BossSst* thisv);
void BossSst_HandDownbeat(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupOffbeat(BossSst* thisv);
void BossSst_HandOffbeat(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupDownbeatEnd(BossSst* thisv);
void BossSst_HandDownbeatEnd(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupOffbeatEnd(BossSst* thisv);
void BossSst_HandOffbeatEnd(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandReadySlam(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupSlam(BossSst* thisv);
void BossSst_HandSlam(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandEndSlam(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandReadySweep(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupSweep(BossSst* thisv);
void BossSst_HandSweep(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandReadyPunch(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupPunch(BossSst* thisv);
void BossSst_HandPunch(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandReadyClap(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupClap(BossSst* thisv);
void BossSst_HandClap(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupEndClap(BossSst* thisv);
void BossSst_HandEndClap(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandReadyGrab(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupGrab(BossSst* thisv);
void BossSst_HandGrab(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupCrush(BossSst* thisv);
void BossSst_HandCrush(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupEndCrush(BossSst* thisv);
void BossSst_HandEndCrush(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupSwing(BossSst* thisv);
void BossSst_HandSwing(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandSetupRetreat(BossSst* thisv);
void BossSst_HandRetreat(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandSetupReel(BossSst* thisv);
void BossSst_HandReel(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupReadyShake(BossSst* thisv);
void BossSst_HandReadyShake(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupShake(BossSst* thisv);
void BossSst_HandShake(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupReadyCharge(BossSst* thisv);
void BossSst_HandReadyCharge(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandSetupFrozen(BossSst* thisv);
void BossSst_HandFrozen(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupReadyBreakIce(BossSst* thisv);
void BossSst_HandReadyBreakIce(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupBreakIce(BossSst* thisv);
void BossSst_HandBreakIce(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandStunned(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandDamage(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupRecover(BossSst* thisv);
void BossSst_HandRecover(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_HandSetupThrash(BossSst* thisv);
void BossSst_HandThrash(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupDarken(BossSst* thisv);
void BossSst_HandDarken(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupFall(BossSst* thisv);
void BossSst_HandFall(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupMelt(BossSst* thisv);
void BossSst_HandMelt(BossSst* thisv, GlobalContext* globalCtx);
void BossSst_HandSetupFinish(BossSst* thisv);
void BossSst_HandFinish(BossSst* thisv, GlobalContext* globalCtx);

void BossSst_SpawnHeadShadow(BossSst* thisv);
void BossSst_SpawnHandShadow(BossSst* thisv);
void BossSst_SpawnShockwave(BossSst* thisv);
void BossSst_SpawnIceCrystal(BossSst* thisv, s32 index);
void BossSst_SpawnIceShard(BossSst* thisv);
void BossSst_IceShatter(BossSst* thisv);

#include "overlays/ovl_Boss_Sst/ovl_Boss_Sst.h"

static BossSst* sHead;
static BossSst* sHands[2];
static BgSstFloor* sFloor;

static Vec3f sRoomCenter = { ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z };
static Vec3f sHandOffsets[2];
static s16 sHandYawOffsets[2];

static s16 sCutsceneCamera;
static Vec3f sCameraAt = { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f sCameraEye = { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f sCameraAtVel = { 0.0f, 0.0f, 0.0f };
static Vec3f sCameraEyeVel = { 0.0f, 0.0f, 0.0f };

static Vec3f sCameraAtPoints[] = {
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 0.0f },
    { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 0.0f, ROOM_CENTER_Y + 600.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X - 60.0f, ROOM_CENTER_Y + 180.0f, ROOM_CENTER_Z + 730.0f },
};

static Vec3f sCameraEyePoints[] = {
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 800.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 700.0f, ROOM_CENTER_Z + 1400.0f },
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 750.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 350.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 1200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static u32 sBodyStatic = false;

// Unreferenced. Maybe two zero vectors?
static u32 sUnkValues[] = { 0, 0, 0, 0, 0, 0 };

static Color_RGBA8 sBodyColor = { 255, 255, 255, 255 };
static Color_RGBA8 sStaticColor = { 0, 0, 0, 255 };
static s32 sHandState[] = { HAND_WAIT, HAND_WAIT };

const ActorInit Boss_Sst_InitVars = {
    ACTOR_BOSS_SST,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_SST,
    sizeof(BossSst),
    (ActorFunc)BossSst_Init,
    (ActorFunc)BossSst_Destroy,
    (ActorFunc)BossSst_UpdateHand,
    (ActorFunc)BossSst_DrawHand,
    NULL,
};

#include "z_boss_sst_colchk.cpp"

static const AnimationHeader* sHandIdleAnims[] = { &gBongoLeftHandIdleAnim, &gBongoRightHandIdleAnim };
static const AnimationHeader* sHandFlatPoses[] = { &gBongoLeftHandFlatPoseAnim, &gBongoRightHandFlatPoseAnim };
static const AnimationHeader* sHandOpenPoses[] = { &gBongoLeftHandOpenPoseAnim, &gBongoRightHandOpenPoseAnim };
static const AnimationHeader* sHandFistPoses[] = { &gBongoLeftHandFistPoseAnim, &gBongoRightHandFistPoseAnim };
static const AnimationHeader* sHandClenchAnims[] = { &gBongoLeftHandClenchAnim, &gBongoRightHandClenchAnim };
static const AnimationHeader* sHandDamagePoses[] = { &gBongoLeftHandDamagePoseAnim, &gBongoRightHandDamagePoseAnim };
static const AnimationHeader* sHandPushoffPoses[] = { &gBongoLeftHandPushoffPoseAnim, &gBongoRightHandPushoffPoseAnim };
static const AnimationHeader* sHandHangPoses[] = { &gBongoLeftHandHangPoseAnim, &gBongoRightHandHangPoseAnim };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x29, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void BossSst_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossSst* thisv = (BossSst*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Collider_InitCylinder(globalCtx, &thisv->colliderCyl);
    Collider_InitJntSph(globalCtx, &thisv->colliderJntSph);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Flags_SetSwitch(globalCtx, 0x14);
    if (thisv->actor.params == BONGO_HEAD) {
        sFloor = (BgSstFloor*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_SST_FLOOR, sRoomCenter.x,
                                          sRoomCenter.y, sRoomCenter.z, 0, 0, 0, BONGOFLOOR_REST);
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBongoHeadSkel, &gBongoHeadEyeOpenIdleAnim, thisv->jointTable,
                           thisv->morphTable, 45);
        ActorShape_Init(&thisv->actor.shape, 70000.0f, ActorShadow_DrawCircle, 95.0f);
        Collider_SetJntSph(globalCtx, &thisv->colliderJntSph, &thisv->actor, &sJntSphInitHead, thisv->colliderItems);
        Collider_SetCylinder(globalCtx, &thisv->colliderCyl, &thisv->actor, &sCylinderInitHead);
        sHead = thisv;
        thisv->actor.world.pos.x = ROOM_CENTER_X + 50.0f;
        thisv->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
        thisv->actor.world.pos.z = ROOM_CENTER_Z - 650.0f;
        thisv->actor.home.pos = thisv->actor.world.pos;
        thisv->actor.shape.rot.y = 0;
        if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y,
                        ROOM_CENTER_Z + 400.0f, 0, 0, 0, WARP_DUNGEON_ADULT);
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, ROOM_CENTER_X, ROOM_CENTER_Y,
                        ROOM_CENTER_Z - 200.0f, 0, 0, 0, 0);
            Actor_Kill(&thisv->actor);
        } else {
            sHands[LEFT] =
                (BossSst*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BOSS_SST, thisv->actor.world.pos.x + 200.0f,
                                      thisv->actor.world.pos.y, thisv->actor.world.pos.z + 400.0f, 0,
                                      thisv->actor.shape.rot.y, 0, BONGO_LEFT_HAND);
            sHands[RIGHT] = (BossSst*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BOSS_SST,
                                                  thisv->actor.world.pos.x + (-200.0f), thisv->actor.world.pos.y,
                                                  thisv->actor.world.pos.z + 400.0f, 0, thisv->actor.shape.rot.y, 0,
                                                  BONGO_RIGHT_HAND);
            sHands[LEFT]->actor.child = &sHands[RIGHT]->actor;
            sHands[RIGHT]->actor.child = &sHands[LEFT]->actor;

            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->actor.update = BossSst_UpdateHead;
            thisv->actor.draw = BossSst_DrawHead;
            thisv->radius = -650.0f;
            thisv->actor.targetArrowOffset = 4000.0f;
            BossSst_HeadSetupLurk(thisv);
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_BOSS);
        }
    } else {
        Collider_SetJntSph(globalCtx, &thisv->colliderJntSph, &thisv->actor, &sJntSphInitHand, thisv->colliderItems);
        Collider_SetCylinder(globalCtx, &thisv->colliderCyl, &thisv->actor, &sCylinderInitHand);
        if (thisv->actor.params == BONGO_LEFT_HAND) {
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBongoLeftHandSkel, &gBongoLeftHandIdleAnim,
                               thisv->jointTable, thisv->morphTable, 27);
            thisv->vParity = -1;
            thisv->colliderJntSph.elements[0].dim.modelSphere.center.z *= -1;
        } else {
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gBongoRightHandSkel, &gBongoRightHandIdleAnim,
                               thisv->jointTable, thisv->morphTable, 27);
            thisv->vParity = 1;
        }

        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 95.0f);
        thisv->handZPosMod = -3500;
        thisv->actor.targetArrowOffset = 5000.0f;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        BossSst_HandSetupWait(thisv);
    }
}

void BossSst_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossSst* thisv = (BossSst*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->colliderJntSph);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderCyl);
    Audio_StopSfxByPos(&thisv->center);
}

void BossSst_HeadSetupLurk(BossSst* thisv) {
    thisv->actor.draw = NULL;
    sHands[LEFT]->actor.draw = NULL;
    sHands[RIGHT]->actor.draw = NULL;
    thisv->vVanish = false;
    thisv->actionFunc = BossSst_HeadLurk;
}

void BossSst_HeadLurk(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.yDistToPlayer < 1000.0f) {
        BossSst_HeadSetupIntro(thisv, globalCtx);
    }
}

void BossSst_HeadSetupIntro(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->timer = 611;
    thisv->ready = false;
    player->actor.world.pos.x = sRoomCenter.x;
    player->actor.world.pos.y = ROOM_CENTER_Y + 1000.0f;
    player->actor.world.pos.z = sRoomCenter.z;
    player->linearVelocity = player->actor.velocity.y = 0.0f;
    player->actor.shape.rot.y = -0x8000;
    player->targetYaw = -0x8000;
    player->currentYaw = -0x8000;
    player->fallStartHeight = 0;
    player->stateFlags1 |= 0x20;

    func_80064520(globalCtx, &globalCtx->csCtx);
    func_8002DF54(globalCtx, &thisv->actor, 8);
    sCutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, sCutsceneCamera, CAM_STAT_ACTIVE);
    Math_Vec3f_Copy(&sCameraAt, &player->actor.world.pos);
    if (gSaveContext.eventChkInf[7] & 0x80) {
        sCameraEye.z = ROOM_CENTER_Z - 100.0f;
    }

    Gameplay_CameraSetAtEye(globalCtx, sCutsceneCamera, &sCameraAt, &sCameraEye);
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    thisv->actionFunc = BossSst_HeadIntro;
}

void BossSst_HeadIntro(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 tempo;
    s32 introStateTimer;
    s32 revealStateTimer;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeCloseIdleAnim, -3.0f);
    }

    if (thisv->timer == 0) {
        sHands[RIGHT]->actor.flags |= ACTOR_FLAG_0;
        sHands[LEFT]->actor.flags |= ACTOR_FLAG_0;
        player->stateFlags1 &= ~0x20;
        func_80064534(globalCtx, &globalCtx->csCtx);
        func_8002DF54(globalCtx, &thisv->actor, 7);
        sCameraAt.y += 30.0f;
        sCameraAt.z += 300.0f;
        Gameplay_CameraSetAtEye(globalCtx, sCutsceneCamera, &sCameraAt, &sCameraEye);
        Gameplay_CopyCamera(globalCtx, MAIN_CAM, sCutsceneCamera);
        Gameplay_ChangeCameraStatus(globalCtx, sCutsceneCamera, CAM_STAT_WAIT);
        Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
        Gameplay_ClearCamera(globalCtx, sCutsceneCamera);
        gSaveContext.eventChkInf[7] |= 0x80;
        BossSst_HeadSetupNeutral(thisv);
        thisv->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sHands[LEFT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sHands[RIGHT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        thisv->timer = 112;
    } else if (thisv->timer >= 546) {
        if (player->actor.world.pos.y > 100.0f) {
            player->actor.world.pos.x = sRoomCenter.x;
            player->actor.world.pos.z = sRoomCenter.z;
            player->linearVelocity = 0;
            player->actor.shape.rot.y = -0x8000;
            player->targetYaw = -0x8000;
            player->currentYaw = -0x8000;
        }

        Math_Vec3f_Copy(&sCameraAt, &player->actor.world.pos);
        if (player->actor.bgCheckFlags & 2) {
            if (!thisv->ready) {
                sFloor->dyna.actor.params = BONGOFLOOR_HIT;
                thisv->ready = true;
                func_800AA000(thisv->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
                Audio_PlayActorSound2(&sFloor->dyna.actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            } else if (gSaveContext.eventChkInf[7] & 0x80) {
                sHands[RIGHT]->actor.draw = BossSst_DrawHand;
                sHands[LEFT]->actor.draw = BossSst_DrawHand;
                thisv->actor.draw = BossSst_DrawHead;
                thisv->timer = 178;
                sCameraAt.x = ROOM_CENTER_X - 23.0f;
                sCameraAt.y = ROOM_CENTER_Y + 0.0f;
                sCameraAt.z = ROOM_CENTER_Z + 0.0f;
            } else {
                thisv->timer = 546;
            }
        }
    } else if (thisv->timer >= 478) {
        sCameraEye.x += 10.0f;
        sCameraEye.y += 10.0f;
        sCameraEye.z -= 10.0f;
    } else if (thisv->timer >= 448) {
        if (thisv->timer == 460) {
            sHands[RIGHT]->actor.draw = BossSst_DrawHand;
            sHands[LEFT]->actor.draw = BossSst_DrawHand;
            thisv->actor.draw = BossSst_DrawHead;
            player->actor.world.pos.x = sRoomCenter.x;
            player->actor.world.pos.z = sRoomCenter.z;
            BossSst_HandSetupDownbeat(sHands[RIGHT]);
        }
        if (thisv->timer > 460) {
            sCameraEye.x -= 40.0f;
            sCameraEye.y -= 40.0f;
            sCameraEye.z += 20.0f;
        } else if (thisv->timer == 460) {
            sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 0.0f;
            sCameraAt.y = sHands[RIGHT]->actor.home.pos.y - 20.0f;
            sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 10.0f;
            sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 150.0f;
            sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 100.0f;
            sCameraEye.z = sHands[RIGHT]->actor.home.pos.z + 80.0f;
        }
    } else {
        if (thisv->timer >= 372) {
            introStateTimer = thisv->timer - 372;
            tempo = 6;
            if (thisv->timer == 447) {
                sCameraAt = player->actor.world.pos;
                sCameraEye.x = ROOM_CENTER_X - 200.0f;
                sCameraEye.y = ROOM_CENTER_Y + 160.0f;
                sCameraEye.z = ROOM_CENTER_Z - 190.0f;
            } else if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 30.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y + 0.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 20.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 100.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 10.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z - 210.0f;
            } else if (introStateTimer == 62) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x + 0.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y + 50.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 100.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x + 110.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 180.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z - 70.0f;
            }
        } else if (thisv->timer >= 304) {
            introStateTimer = thisv->timer - 304;
            tempo = 5;
            if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 40.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y - 90.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z - 40.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x - 20.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y + 210.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z + 170.0f;
            } else if (thisv->timer == 368) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 20.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y + 0.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 0.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x - 70.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 170.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z + 150.0f;
            }
        } else if (thisv->timer >= 244) {
            introStateTimer = thisv->timer - 244;
            tempo = 4;
            if (introStateTimer == 11) {
                sCameraAt.x = sHands[RIGHT]->actor.home.pos.x + 30.0f;
                sCameraAt.y = sHands[RIGHT]->actor.home.pos.y + 70.0f;
                sCameraAt.z = sHands[RIGHT]->actor.home.pos.z + 40.0f;
                sCameraEye.x = sHands[RIGHT]->actor.home.pos.x + 110.0f;
                sCameraEye.y = sHands[RIGHT]->actor.home.pos.y - 140.0f;
                sCameraEye.z = sHands[RIGHT]->actor.home.pos.z - 10.0f;
            } else if (thisv->timer == 300) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 20.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y - 80.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 320.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x - 130.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 130.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z - 150.0f;
            }
        } else if (thisv->timer >= 192) {
            introStateTimer = thisv->timer - 192;
            tempo = 3;
            if (thisv->timer == 240) {
                sCameraAt.x = sHands[LEFT]->actor.home.pos.x - 190.0f;
                sCameraAt.y = sHands[LEFT]->actor.home.pos.y - 110.0f;
                sCameraAt.z = sHands[LEFT]->actor.home.pos.z + 40.0f;
                sCameraEye.x = sHands[LEFT]->actor.home.pos.x + 120.0f;
                sCameraEye.y = sHands[LEFT]->actor.home.pos.y + 130.0f;
                sCameraEye.z = sHands[LEFT]->actor.home.pos.z + 50.0f;
            } else if (introStateTimer == 12) {
                sCameraAt.x = sRoomCenter.x + 50.0f;
                sCameraAt.y = sRoomCenter.y - 90.0f;
                sCameraAt.z = sRoomCenter.z - 200.0f;
                sCameraEye.x = sRoomCenter.x + 50.0f;
                sCameraEye.y = sRoomCenter.y + 350.0f;
                sCameraEye.z = sRoomCenter.z + 150.0f;
            }
        } else if (thisv->timer >= 148) {
            introStateTimer = thisv->timer - 148;
            tempo = 2;
        } else if (thisv->timer >= 112) {
            introStateTimer = thisv->timer - 112;
            tempo = 1;
        } else {
            introStateTimer = thisv->timer % 28;
            tempo = 0;
        }
        if (thisv->timer <= 198) {
            revealStateTimer = 198 - thisv->timer;
            if ((gSaveContext.eventChkInf[7] & 0x80) && (revealStateTimer <= 44)) {
                sCameraAt.x += 492.0f * 0.01f;
                sCameraAt.y += 200.0f * 0.01f;
                sCameraEye.x -= 80.0f * 0.01f;
                sCameraEye.y -= 360.0f * 0.01f;
                sCameraEye.z += 1000.0f * 0.01f;
            } else if (thisv->timer <= 20) {
                sCameraAt.y -= 700.0f * 0.01f;
                sCameraAt.z += 900.0f * 0.01f;
                sCameraEye.x += 650.0f * 0.01f;
                sCameraEye.y += 400.0f * 0.01f;
                sCameraEye.z += 1550.0f * 0.01f;
                thisv->vVanish = true;
                thisv->actor.flags |= ACTOR_FLAG_7;
            } else if (revealStateTimer < 40) {
                sCameraAt.x += 125.0f * 0.01f;
                sCameraAt.y += 350.0f * 0.01f;
                sCameraAt.z += 500.0f * 0.01f;
                sCameraEye.x += 200.0f * 0.01f;
                sCameraEye.y -= 850.0f * 0.01f;
            } else if (revealStateTimer >= 45) {
                if (revealStateTimer < 85) {
                    sCameraAt.x -= 250.0f * 0.01f;
                    sCameraAt.y += 425.0f * 0.01f;
                    sCameraAt.z -= 1200.0f * 0.01f;
                    sCameraEye.x -= 650.0f * 0.01f;
                    sCameraEye.y += 125.0f * 0.01f;
                    sCameraEye.z -= 350.0f * 0.01f;
                } else if (revealStateTimer == 85) {
                    if (!(gSaveContext.eventChkInf[7] & 0x80)) {
                        TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx,
                                               SEGMENTED_TO_VIRTUAL(gBongoTitleCardTex), 160, 180, 128, 40);
                    }
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
                    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
                    BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_DISAPPEAR);
                }
            }
        }
        if (introStateTimer == 12) {
            BossSst_HandSetupDownbeat(sHands[RIGHT]);
        }
        if ((introStateTimer != 5) && ((introStateTimer % ((tempo * 2) + 7)) == 5)) {
            BossSst_HandSetupOffbeat(sHands[LEFT]);
        }
    }

    if (thisv->actionFunc != BossSst_HeadNeutral) {
        Gameplay_CameraSetAtEye(globalCtx, sCutsceneCamera, &sCameraAt, &sCameraEye);
    }
}

void BossSst_HeadSetupWait(BossSst* thisv) {
    if (thisv->skelAnime.animation != &gBongoHeadEyeCloseIdleAnim) {
        Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeCloseIdleAnim, -5.0f);
    }
    thisv->actionFunc = BossSst_HeadWait;
}

void BossSst_HeadWait(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((HAND_STATE(sHands[LEFT]) == HAND_WAIT) && (HAND_STATE(sHands[RIGHT]) == HAND_WAIT)) {
        BossSst_HeadSetupNeutral(thisv);
    }
}

void BossSst_HeadSetupNeutral(BossSst* thisv) {
    thisv->timer = 127;
    thisv->ready = false;
    thisv->actionFunc = BossSst_HeadNeutral;
}

void BossSst_HeadNeutral(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (!thisv->ready && ((HAND_STATE(sHands[LEFT]) == HAND_BEAT) || (HAND_STATE(sHands[LEFT]) == HAND_WAIT)) &&
        ((HAND_STATE(sHands[RIGHT]) == HAND_BEAT) || (HAND_STATE(sHands[RIGHT]) == HAND_WAIT))) {
        thisv->ready = true;
    }

    if (thisv->ready) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
    }

    if (thisv->timer == 0) {
        if ((GET_PLAYER(globalCtx)->actor.world.pos.y > -50.0f) && !(GET_PLAYER(globalCtx)->stateFlags1 & 0x6080)) {
            sHands[Rand_ZeroOne() <= 0.5f]->ready = true;
            BossSst_HeadSetupWait(thisv);
        } else {
            thisv->timer = 28;
        }
    } else {
        Math_ApproachS(&thisv->actor.shape.rot.y,
                       Actor_WorldYawTowardPoint(&GET_PLAYER(globalCtx)->actor, &sRoomCenter) + 0x8000, 4, 0x400);
        if ((thisv->timer == 28) || (thisv->timer == 84)) {
            BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_PRAY);
        }
    }
}

void BossSst_HeadSetupDamagedHand(BossSst* thisv, s32 bothHands) {
    if (bothHands) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadEyeOpenAnim, -5.0f);
    } else {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadDamagedHandAnim, -5.0f);
    }
    thisv->actionFunc = BossSst_HeadDamagedHand;
}

void BossSst_HeadDamagedHand(BossSst* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if ((HAND_STATE(sHands[LEFT]) == HAND_DAMAGED) && (HAND_STATE(sHands[RIGHT]) == HAND_DAMAGED)) {
            BossSst_HeadSetupReadyCharge(thisv);
        } else if ((HAND_STATE(sHands[LEFT]) == HAND_FROZEN) || (HAND_STATE(sHands[RIGHT]) == HAND_FROZEN)) {
            BossSst_HeadSetupFrozenHand(thisv);
        } else if (thisv->skelAnime.animation == &gBongoHeadEyeOpenAnim) {
            BossSst_HeadSetupUnfreezeHand(thisv);
        } else {
            BossSst_HeadSetupWait(thisv);
        }
    }
}

void BossSst_HeadSetupReadyCharge(BossSst* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->colliderCyl.base.acFlags |= AC_ON;
    thisv->actionFunc = BossSst_HeadReadyCharge;
}

void BossSst_HeadReadyCharge(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (sHands[LEFT]->ready && (sHands[LEFT]->actionFunc == BossSst_HandReadyCharge) && sHands[RIGHT]->ready &&
        (sHands[RIGHT]->actionFunc == BossSst_HandReadyCharge)) {
        BossSst_HeadSetupCharge(thisv);
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 4, 0x800, 0x400);
    }
}

void BossSst_HeadSetupCharge(BossSst* thisv) {
    Animation_Change(&thisv->skelAnime, &gBongoHeadChargeAnim, 0.5f, 0.0f, Animation_GetLastFrame(&gBongoHeadChargeAnim),
                     ANIMMODE_ONCE_INTERP, -5.0f);
    BossSst_HandSetDamage(sHands[LEFT], 0x20);
    BossSst_HandSetDamage(sHands[RIGHT], 0x20);
    thisv->colliderJntSph.base.atFlags |= AT_ON;
    thisv->actor.speedXZ = 3.0f;
    thisv->radius = -650.0f;
    thisv->ready = false;
    thisv->actionFunc = BossSst_HeadCharge;
}

void BossSst_HeadCharge(BossSst* thisv, GlobalContext* globalCtx) {
    f32 chargeDist;
    s32 animFinish = SkelAnime_Update(&thisv->skelAnime);

    if (!thisv->ready && Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        thisv->ready = true;
        thisv->actor.speedXZ = 0.25f;
        thisv->skelAnime.playSpeed = 0.2f;
    }

    thisv->actor.speedXZ *= 1.25f;
    thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 45.0f);

    if (thisv->ready) {
        if (Math_SmoothStepToF(&thisv->radius, 650.0f, 0.4f, thisv->actor.speedXZ, 1.0f) < 10.0f) {
            thisv->radius = 650.0f;
            BossSst_HeadSetupEndCharge(thisv);
        } else {
            chargeDist = (650.0f - thisv->radius) * 3.0f;
            if (chargeDist > 180.0f) {
                chargeDist = 180.0f;
            }

            thisv->actor.world.pos.y = thisv->actor.home.pos.y - chargeDist;
        }

        if (!animFinish) {
            sHandOffsets[LEFT].z += 5.0f;
            sHandOffsets[RIGHT].z += 5.0f;
        }
    } else {
        Math_ApproachF(&thisv->radius, -700.0f, 0.4f, thisv->actor.speedXZ);
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 180.0f, 20.0f);
        sHandOffsets[LEFT].y += 5.0f;
        sHandOffsets[RIGHT].y += 5.0f;
    }

    if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
        thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHands[LEFT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHands[RIGHT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        func_8002F71C(globalCtx, &thisv->actor, 10.0f, thisv->actor.shape.rot.y, 5.0f);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
    }
}

void BossSst_HeadSetupEndCharge(BossSst* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeCloseIdleAnim, -20.0f);
    thisv->targetYaw = Actor_WorldYawTowardPoint(&thisv->actor, &sRoomCenter);
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->radius *= -1.0f;
    thisv->actionFunc = BossSst_HeadEndCharge;
}

void BossSst_HeadEndCharge(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 4, 0x800, 0x100) == 0) {
        BossSst_HandSetupRetreat(sHands[LEFT]);
        BossSst_HandSetupRetreat(sHands[RIGHT]);
        BossSst_HeadSetupNeutral(thisv);
    }
}

void BossSst_HeadSetupFrozenHand(BossSst* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    thisv->ready = false;
    thisv->colliderCyl.base.acFlags |= AC_ON;
    thisv->actionFunc = BossSst_HeadFrozenHand;
}

void BossSst_HeadFrozenHand(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->ready) {
        BossSst_HeadSetupUnfreezeHand(thisv);
    }
}

void BossSst_HeadSetupUnfreezeHand(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->actionFunc = BossSst_HeadUnfreezeHand;
}

void BossSst_HeadUnfreezeHand(BossSst* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        BossSst_HeadSetupWait(thisv);
    }
}

void BossSst_HeadSetupStunned(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadKnockoutAnim, -5.0f);
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadKnockoutAnim));
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->vVanish = false;
    thisv->actor.flags &= ~ACTOR_FLAG_7;
    BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_FREEZE);
    thisv->actionFunc = BossSst_HeadStunned;
}

void BossSst_HeadStunned(BossSst* thisv, GlobalContext* globalCtx) {
    f32 bounce;
    s32 animFinish;
    f32 currentFrame;

    Math_StepToF(&sHandOffsets[LEFT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[LEFT].x, 200.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].x, -200.0f, 20.0f);
    thisv->actor.velocity.y += thisv->actor.gravity;
    animFinish = SkelAnime_Update(&thisv->skelAnime);
    currentFrame = thisv->skelAnime.curFrame;
    if (currentFrame <= 6.0f) {
        bounce = (sinf((std::numbers::pi_v<float> / 11) * currentFrame) * 100.0f) + (thisv->actor.home.pos.y - 180.0f);
        if (thisv->actor.world.pos.y < bounce) {
            thisv->actor.world.pos.y = bounce;
        }
    } else if (currentFrame <= 11.0f) {
        thisv->actor.world.pos.y = (sinf((std::numbers::pi_v<float> / 11) * currentFrame) * 170.0f) + (thisv->actor.home.pos.y - 250.0f);
    } else {
        thisv->actor.world.pos.y =
            (sinf((currentFrame - 11.0f) * (std::numbers::pi_v<float> / 5)) * 50.0f) + (thisv->actor.home.pos.y - 250.0f);
    }

    if ((animFinish) || Animation_OnFrame(&thisv->skelAnime, 11.0f)) {
        BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_LAND);
    }

    if (thisv->radius < -500.0f) {
        Math_SmoothStepToF(&thisv->radius, -500.0f, 1.0f, 50.0f, 5.0f);
    } else {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.5f, 15.0f, 3.0f);
        thisv->radius += thisv->actor.speedXZ;
    }

    thisv->radius = CLAMP_MAX(thisv->radius, 400.0f);

    thisv->actor.world.pos.y += thisv->actor.velocity.y;
    if (animFinish) {
        BossSst_HeadSetupVulnerable(thisv);
    }
}

void BossSst_HeadSetupVulnerable(BossSst* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadStunnedAnim, -5.0f);
    thisv->colliderCyl.base.acFlags |= AC_ON;
    thisv->colliderCyl.info.bumper.dmgFlags = 0x0FC00702; // Sword-type damage
    thisv->actor.speedXZ = 0.0f;
    thisv->colliderJntSph.elements[10].info.bumperFlags |= (BUMP_ON | BUMP_HOOKABLE);
    thisv->colliderJntSph.elements[0].info.bumperFlags &= ~BUMP_ON;
    if (thisv->actionFunc != BossSst_HeadDamage) {
        thisv->timer = 50;
    }

    thisv->actionFunc = BossSst_HeadVulnerable;
}

void BossSst_HeadVulnerable(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&sHandOffsets[LEFT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].z, 600.0f, 20.0f);
    Math_StepToF(&sHandOffsets[LEFT].x, 200.0f, 20.0f);
    Math_StepToF(&sHandOffsets[RIGHT].x, -200.0f, 20.0f);
    if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        thisv->timer += 2;
        thisv->timer = CLAMP_MAX(thisv->timer, 50);
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            BossSst_HandSetupRecover(sHands[LEFT]);
            BossSst_HandSetupRecover(sHands[RIGHT]);
            BossSst_HeadSetupRecover(thisv);
        }
    }
}

void BossSst_HeadSetupDamage(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadDamageAnim, -3.0f);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    Actor_SetColorFilter(&sHands[LEFT]->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    Actor_SetColorFilter(&sHands[RIGHT]->actor, 0x4000, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadDamageAnim));
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_DAMAGE);
    thisv->actionFunc = BossSst_HeadDamage;
}

void BossSst_HeadDamage(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        BossSst_HeadSetupVulnerable(thisv);
    }
}

void BossSst_HeadSetupRecover(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadRecoverAnim, -5.0f);
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->colliderCyl.info.bumper.dmgFlags = 0xFFCFFFFF;
    thisv->colliderJntSph.elements[10].info.bumperFlags &= ~(BUMP_ON | BUMP_HOOKABLE);
    thisv->colliderJntSph.elements[0].info.bumperFlags |= BUMP_ON;
    thisv->vVanish = true;
    thisv->actor.speedXZ = 5.0f;
    thisv->actionFunc = BossSst_HeadRecover;
}

void BossSst_HeadRecover(BossSst* thisv, GlobalContext* globalCtx) {
    s32 animFinish;
    f32 currentFrame;
    f32 diff;

    animFinish = SkelAnime_Update(&thisv->skelAnime);
    currentFrame = thisv->skelAnime.curFrame;
    if (currentFrame < 10.0f) {
        thisv->actor.world.pos.y += 10.0f;
        sHandOffsets[LEFT].y -= 10.0f;
        sHandOffsets[RIGHT].y -= 10.0f;
        Math_SmoothStepToF(&thisv->radius, -750.0f, 1.0f, thisv->actor.speedXZ, 2.0f);
    } else {
        thisv->actor.speedXZ *= 1.25f;
        thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 50.0f);
        diff = Math_SmoothStepToF(&thisv->radius, -650.0f, 1.0f, thisv->actor.speedXZ, 2.0f);
        diff += Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 30.0f, 3.0f);
    }
    if (animFinish && (diff < 10.0f)) {
        thisv->actor.world.pos.y = thisv->actor.home.pos.y;
        thisv->radius = -650.0f;
        BossSst_HandSetupRetreat(sHands[LEFT]);
        BossSst_HandSetupRetreat(sHands[RIGHT]);
        BossSst_HeadSetupNeutral(thisv);
    }
}

void BossSst_SetCameraTargets(f32 cameraSpeedMod, s32 targetIndex) {
    Vec3f* nextAt = &sCameraAtPoints[targetIndex];
    Vec3f* nextEye = &sCameraEyePoints[targetIndex];

    if (targetIndex != 0) {
        Math_Vec3f_Copy(&sCameraAt, &sCameraAtPoints[targetIndex - 1]);
        Math_Vec3f_Copy(&sCameraEye, &sCameraEyePoints[targetIndex - 1]);
    }

    sCameraAtVel.x = (nextAt->x - sCameraAt.x) * cameraSpeedMod;
    sCameraAtVel.y = (nextAt->y - sCameraAt.y) * cameraSpeedMod;
    sCameraAtVel.z = (nextAt->z - sCameraAt.z) * cameraSpeedMod;

    sCameraEyeVel.x = (nextEye->x - sCameraEye.x) * cameraSpeedMod;
    sCameraEyeVel.y = (nextEye->y - sCameraEye.y) * cameraSpeedMod;
    sCameraEyeVel.z = (nextEye->z - sCameraEye.z) * cameraSpeedMod;
}

void BossSst_UpdateDeathCamera(BossSst* thisv, GlobalContext* globalCtx) {
    Vec3f cameraAt;
    Vec3f cameraEye;
    f32 sn;
    f32 cs;

    sCameraAt.x += sCameraAtVel.x;
    sCameraAt.y += sCameraAtVel.y;
    sCameraAt.z += sCameraAtVel.z;
    sCameraEye.x += sCameraEyeVel.x;
    sCameraEye.y += sCameraEyeVel.y;
    sCameraEye.z += sCameraEyeVel.z;

    sn = Math_SinS(thisv->actor.shape.rot.y);
    cs = Math_CosS(thisv->actor.shape.rot.y);
    cameraAt.x = thisv->actor.world.pos.x + (sCameraAt.z * sn) + (sCameraAt.x * cs);
    cameraAt.y = thisv->actor.home.pos.y - 140.0f + sCameraAt.y;
    cameraAt.z = thisv->actor.world.pos.z + (sCameraAt.z * cs) - (sCameraAt.x * sn);
    cameraEye.x = thisv->actor.world.pos.x + (sCameraEye.z * sn) + (sCameraEye.x * cs);
    cameraEye.y = thisv->actor.home.pos.y - 140.0f + sCameraEye.y;
    cameraEye.z = thisv->actor.world.pos.z + (sCameraEye.z * cs) - (sCameraEye.x * sn);
    Gameplay_CameraSetAtEye(globalCtx, sCutsceneCamera, &cameraAt, &cameraEye);
}

void BossSst_HeadSetupDeath(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Animation_MorphToLoop(&thisv->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_DEAD);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 60);
    Actor_SetColorFilter(&sHands[LEFT]->actor, 0x4000, 0xFF, 0, 60);
    Actor_SetColorFilter(&sHands[RIGHT]->actor, 0x4000, 0xFF, 0, 60);
    thisv->timer = 60;
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sHands[LEFT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sHands[RIGHT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    sCutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, sCutsceneCamera, CAM_STAT_ACTIVE);
    Gameplay_CopyCamera(globalCtx, sCutsceneCamera, MAIN_CAM);
    func_8002DF54(globalCtx, &player->actor, 8);
    func_80064520(globalCtx, &globalCtx->csCtx);
    Math_Vec3f_Copy(&sCameraEye, &GET_ACTIVE_CAM(globalCtx)->eye);
    thisv->actionFunc = BossSst_HeadDeath;
}

void BossSst_HeadDeath(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 140.0f, 20.0f);
    if (thisv->timer == 0) {
        BossSst_HandSetupThrash(sHands[LEFT]);
        BossSst_HandSetupThrash(sHands[RIGHT]);
        BossSst_HeadSetupThrash(thisv);
    } else if (thisv->timer > 48) {
        Gameplay_CameraSetAtEye(globalCtx, sCutsceneCamera, &thisv->actor.focus.pos, &sCameraEye);
        Math_StepToF(&thisv->radius, -350.0f, 10.0f);
    } else if (thisv->timer == 48) {
        Player* player = GET_PLAYER(globalCtx);

        player->actor.world.pos.x = sRoomCenter.x + (400.0f * Math_SinS(thisv->actor.shape.rot.y)) +
                                    (Math_CosS(thisv->actor.shape.rot.y) * -120.0f);
        player->actor.world.pos.z = sRoomCenter.z + (400.0f * Math_CosS(thisv->actor.shape.rot.y)) -
                                    (Math_SinS(thisv->actor.shape.rot.y) * -120.0f);
        player->actor.shape.rot.y = Actor_WorldYawTowardPoint(&player->actor, &sRoomCenter);
        func_8002DBD0(&thisv->actor, &sCameraEye, &GET_ACTIVE_CAM(globalCtx)->eye);
        func_8002DBD0(&thisv->actor, &sCameraAt, &GET_ACTIVE_CAM(globalCtx)->at);
        thisv->radius = -350.0f;
        thisv->actor.world.pos.x = sRoomCenter.x - (Math_SinS(thisv->actor.shape.rot.y) * 350.0f);
        thisv->actor.world.pos.z = sRoomCenter.z - (Math_CosS(thisv->actor.shape.rot.y) * 350.0f);
        BossSst_SetCameraTargets(1.0 / 48, 0);
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    } else {
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    }
}

void BossSst_HeadSetupThrash(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    thisv->timer = 160;
    thisv->targetYaw = thisv->actor.shape.rot.y;
    BossSst_SetCameraTargets(1.0 / 80, 1);
    thisv->actionFunc = BossSst_HeadThrash;
}

void BossSst_HeadThrash(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if ((thisv->timer == 0) && (thisv->actor.shape.rot.y == thisv->targetYaw)) {
        BossSst_HeadSetupDarken(thisv);
    } else if (thisv->timer >= 80) {
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    }
}

void BossSst_HeadSetupDarken(BossSst* thisv) {
    thisv->timer = 160;
    BossSst_SetCameraTargets(1.0 / 80, 2);
    thisv->actionFunc = BossSst_HeadDarken;
}

void BossSst_HeadDarken(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (1) {}

    if (thisv->timer >= 80) {
        if (thisv->timer == 80) {
            sBodyStatic = true;
        }
        BossSst_UpdateDeathCamera(thisv, globalCtx);
        sBodyColor.r = sBodyColor.g = sBodyColor.b = (thisv->timer * 3) - 240;
        if (thisv->timer == 80) {
            BossSst_SetCameraTargets(1.0 / 80, 3);
        }
    } else {
        sBodyColor.b = (80 - thisv->timer) / 1.0f;
        sBodyColor.r = sBodyColor.g = sStaticColor.r = sStaticColor.g = sStaticColor.b = (80 - thisv->timer) / 8.0f;
        BossSst_UpdateDeathCamera(thisv, globalCtx);
        if (thisv->timer == 0) {
            BossSst_HeadSetupFall(thisv);
        }
    }
}

void BossSst_HeadSetupFall(BossSst* thisv) {
    thisv->actor.speedXZ = 1.0f;
    Math_Vec3f_Copy(&sCameraAt, &sCameraAtPoints[3]);
    Math_Vec3f_Copy(&sCameraEye, &sCameraEyePoints[3]);
    sCameraAtVel.x = 0.0f;
    sCameraAtVel.z = 0.0f;
    sCameraAtVel.y = -50.0f;
    Math_Vec3f_Copy(&sCameraEyeVel, &sZeroVec);
    thisv->actionFunc = BossSst_HeadFall;
}

void BossSst_HeadFall(BossSst* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ *= 1.5f;
    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 230.0f, thisv->actor.speedXZ)) {
        BossSst_HeadSetupMelt(thisv);
    }

    if (sCameraAt.y > 200.0f) {
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    }
}

void BossSst_HeadSetupMelt(BossSst* thisv) {
    BossSst_SpawnHeadShadow(thisv);
    thisv->timer = 80;
    BossSst_SetCameraTargets(1.0 / 60, 5);
    thisv->actionFunc = BossSst_HeadMelt;
}

void BossSst_HeadMelt(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.scale.y -= 0.00025f;
    thisv->actor.scale.x += 0.000075f;
    thisv->actor.scale.z += 0.000075f;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y - 11500.0f * thisv->actor.scale.y;
    if (thisv->timer == 0) {
        BossSst_HeadSetupFinish(thisv);
    } else if (thisv->timer >= 20.0f) {
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    }
}

void BossSst_HeadSetupFinish(BossSst* thisv) {
    thisv->actor.draw = BossSst_DrawEffect;
    thisv->timer = 40;
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
    BossSst_SetCameraTargets(1.0 / 40, 6);
    thisv->actionFunc = BossSst_HeadFinish;
}

void BossSst_HeadFinish(BossSst* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 colorIndigo = { 80, 80, 150, 255 };
    static Color_RGBA8 colorDarkIndigo = { 40, 40, 80, 255 };
    static Color_RGBA8 colorUnused[2] = {
        { 0, 0, 0, 255 },
        { 100, 100, 100, 0 },
    };
    Vec3f spawnPos;
    s32 i;

    thisv->timer--;
    if (thisv->effectMode == BONGO_NULL) {
        if (thisv->timer < -170) {
            BossSst_UpdateDeathCamera(thisv, globalCtx);
            Gameplay_CopyCamera(globalCtx, MAIN_CAM, sCutsceneCamera);
            Gameplay_ChangeCameraStatus(globalCtx, sCutsceneCamera, CAM_STAT_WAIT);
            Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
            Gameplay_ClearCamera(globalCtx, sCutsceneCamera);
            func_8002DF54(globalCtx, &GET_PLAYER(globalCtx)->actor, 7);
            func_80064534(globalCtx, &globalCtx->csCtx);
            Actor_Kill(&thisv->actor);
            Actor_Kill(&sHands[LEFT]->actor);
            Actor_Kill(&sHands[RIGHT]->actor);
            Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
        }
    } else if (thisv->effects[0].alpha == 0) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z, 0,
                    0, 0, WARP_DUNGEON_ADULT);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART,
                    (Math_SinS(thisv->actor.shape.rot.y) * 200.0f) + ROOM_CENTER_X, ROOM_CENTER_Y,
                    Math_CosS(thisv->actor.shape.rot.y) * 200.0f + ROOM_CENTER_Z, 0, 0, 0, 0);
        BossSst_SetCameraTargets(1.0f, 7);
        thisv->effectMode = BONGO_NULL;
    } else if (thisv->timer == 0) {
        thisv->effects[0].status = 0;
        thisv->effects[1].status = -1;
        thisv->effects[2].status = -1;
    } else if (thisv->timer > 0) {
        thisv->effects[0].status += 5;
        BossSst_UpdateDeathCamera(thisv, globalCtx);
    }

    colorIndigo.a = thisv->effects[0].alpha;
    colorDarkIndigo.a = thisv->effects[0].alpha;

    for (i = 0; i < 5; i++) {
        spawnPos.x = sRoomCenter.x + 0.0f + Rand_CenteredFloat(800.0f);
        spawnPos.y = sRoomCenter.y + (-28.0f) + (Rand_ZeroOne() * 5.0f);
        spawnPos.z = sRoomCenter.z + 0.0f + Rand_CenteredFloat(800.0f);
        EffectSsGSplash_Spawn(globalCtx, &spawnPos, &colorIndigo, &colorDarkIndigo, 0, 0x3E8);
    }
}

void BossSst_HandSetupWait(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_WAIT;
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Animation_MorphToLoop(&thisv->skelAnime, sHandIdleAnims[thisv->actor.params], 5.0f);
    thisv->ready = false;
    thisv->timer = 20;
    thisv->actionFunc = BossSst_HandWait;
}

void BossSst_HandWait(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 20.0f);
    Math_StepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 1.0f);
    Math_StepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 1.0f);
    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        Player* player = GET_PLAYER(globalCtx);

        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if ((thisv->timer == 0) && (player->actor.world.pos.y > -50.0f) && !(player->stateFlags1 & 0x6080)) {
            BossSst_HandSelectAttack(thisv);
        }
    } else if (sHead->actionFunc == BossSst_HeadNeutral) {
        if ((thisv->actor.params == BONGO_RIGHT_HAND) && ((sHead->timer % 28) == 12)) {
            BossSst_HandSetupDownbeat(thisv);
        } else if ((thisv->actor.params == BONGO_LEFT_HAND) && ((sHead->timer % 7) == 5) && (sHead->timer < 112)) {
            BossSst_HandSetupOffbeat(thisv);
        }
    }
}

void BossSst_HandSetupDownbeat(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_BEAT;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 5.0f);
    thisv->actor.shape.rot.x = 0;
    thisv->timer = 12;
    thisv->actionFunc = BossSst_HandDownbeat;
}

void BossSst_HandDownbeat(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(thisv);
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer >= 3) {
            thisv->actor.shape.rot.x -= 0x100;
            Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 180.0f, 20.0f);
        } else {
            thisv->actor.shape.rot.x += 0x300;
            Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (thisv->timer == 0) {
            sFloor->dyna.actor.params = BONGOFLOOR_HIT;
            if (sHead->actionFunc == BossSst_HeadWait) {
                if (thisv->ready) {
                    BossSst_HandSelectAttack(thisv);
                } else {
                    BossSst_HandSetupWait(thisv);
                }
            } else {
                BossSst_HandSetupDownbeatEnd(thisv);
            }
            func_800AA000(thisv->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
    }
}

void BossSst_HandSetupDownbeatEnd(BossSst* thisv) {
    sFloor->dyna.actor.params = BONGOFLOOR_HIT;
    Animation_PlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params]);
    thisv->actionFunc = BossSst_HandDownbeatEnd;
}

void BossSst_HandDownbeatEnd(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(thisv);
    } else {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x800, 0x100);
        Math_StepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 1.0f);
        Math_StepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 1.0f);
        if ((sHead->actionFunc != BossSst_HeadIntro) && ((sHead->timer % 28) == 12)) {
            BossSst_HandSetupDownbeat(thisv);
        }
    }
}

void BossSst_HandSetupOffbeat(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_BEAT;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 5.0f);
    thisv->actor.shape.rot.x = 0;
    thisv->timer = 5;
    thisv->actionFunc = BossSst_HandOffbeat;
}

void BossSst_HandOffbeat(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(thisv);
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer != 0) {
            thisv->actor.shape.rot.x -= 0x140;
            Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 60.0f, 15.0f);
        } else {
            thisv->actor.shape.rot.x += 0x500;
            Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (thisv->timer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
            BossSst_HandSetupOffbeatEnd(thisv);
        }
    }
}

void BossSst_HandSetupOffbeatEnd(BossSst* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params]);
    thisv->actionFunc = BossSst_HandOffbeatEnd;
}

void BossSst_HandOffbeatEnd(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        BossSst_HandSetupWait(thisv);
    } else {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x400, 0xA0);
        Math_StepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 1.0f);
        Math_StepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 1.0f);
        if (sHead->actionFunc == BossSst_HeadWait) {
            if (thisv->ready) {
                BossSst_HandSelectAttack(thisv);
            } else {
                BossSst_HandSetupWait(thisv);
            }
        } else if ((sHead->actionFunc != BossSst_HeadIntro) && ((sHead->timer % 7) == 5) &&
                   ((sHead->timer % 28) != 5)) {
            BossSst_HandSetupOffbeat(thisv);
        }
    }
}

void BossSst_HandSetupEndSlam(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_RETREAT;
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandPushoffPoses[thisv->actor.params], 6.0f);
    thisv->actionFunc = BossSst_HandEndSlam;
}

void BossSst_HandEndSlam(BossSst* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        BossSst_HandSetupRetreat(thisv);
    }
}

void BossSst_HandSetupRetreat(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_RETREAT;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandHangPoses[thisv->actor.params], 10.0f);
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    thisv->colliderJntSph.base.acFlags |= AC_ON;
    thisv->actor.flags |= ACTOR_FLAG_0;
    BossSst_HandSetInvulnerable(thisv, false);
    thisv->timer = 0;
    thisv->actionFunc = BossSst_HandRetreat;
    thisv->actor.speedXZ = 3.0f;
}

void BossSst_HandRetreat(BossSst* thisv, GlobalContext* globalCtx) {
    f32 diff;
    s32 inPosition;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.speedXZ = thisv->actor.speedXZ * 1.2f;
    thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 50.0f);

    diff = Math_SmoothStepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 0.3f, thisv->actor.speedXZ, 1.0f);
    diff += Math_SmoothStepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 0.3f, thisv->actor.speedXZ, 1.0f);
    if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        thisv->actor.world.pos.y = (sinf((thisv->timer * std::numbers::pi_v<float>) / 16.0f) * 250.0f) + thisv->actor.home.pos.y;
        if (thisv->timer == 0) {
            BossSst_HandSetupWait(thisv);
        } else if (thisv->timer == 4) {
            Animation_MorphToLoop(&thisv->skelAnime, sHandIdleAnims[thisv->actor.params], 4.0f);
        }
    } else {
        inPosition = Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, 0x200);
        inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->actor.home.rot.z, 0x200);
        inPosition &= Math_ScaledStepToS(&thisv->handYRotMod, 0, 0x800);
        func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
        if ((Math_SmoothStepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f) < 1.0f) &&
            inPosition && (diff < 10.0f)) {
            thisv->timer = 8;
        }
    }
}

void BossSst_HandSetupReadySlam(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_SLAM;
    thisv->timer = 0;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 10.0f);
    thisv->actionFunc = BossSst_HandReadySlam;
}

void BossSst_HandReadySlam(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            BossSst_HandSetupSlam(thisv);
        }
    } else {
        Player* player = GET_PLAYER(globalCtx);

        if (Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 300.0f, 30.0f) &&
            (thisv->actor.xzDistToPlayer < 140.0f)) {
            thisv->timer = 20;
        }
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x1000, 0x100);
        Math_ApproachF(&thisv->actor.world.pos.x, player->actor.world.pos.x, 0.5f, 40.0f);
        Math_ApproachF(&thisv->actor.world.pos.z, player->actor.world.pos.z, 0.5f, 40.0f);
        func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupSlam(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_SLAM;
    thisv->actor.velocity.y = 1.0f;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 10.0f);
    BossSst_HandSetDamage(thisv, 0x20);
    thisv->ready = false;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    thisv->actionFunc = BossSst_HandSlam;
}

void BossSst_HandSlam(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToS(&thisv->handZPosMod, -0xDAC, 0x1F4);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x1000);
    Math_ScaledStepToS(&thisv->handYRotMod, 0, 0x1000);
    if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            if (thisv->colliderJntSph.base.acFlags & AC_ON) {
                BossSst_HandSetupEndSlam(thisv);
            } else {
                thisv->colliderJntSph.base.acFlags |= AC_ON;
                BossSst_HandSetupWait(thisv);
            }
        }
    } else {
        if (thisv->ready) {
            thisv->timer = 30;
            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            thisv->actor.velocity.y *= 1.5f;
            if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, thisv->actor.velocity.y)) {
                thisv->ready = true;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
                BossSst_SpawnShockwave(thisv);
                thisv->colliderCyl.base.atFlags |= AT_ON;
                Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCyl);
                thisv->colliderCyl.dim.radius = sCylinderInitHand.dim.radius;
            }
        }

        if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(globalCtx);

            player->actor.world.pos.x = (Math_SinS(thisv->actor.yawTowardsPlayer) * 100.0f) + thisv->actor.world.pos.x;
            player->actor.world.pos.z = (Math_CosS(thisv->actor.yawTowardsPlayer) * 100.0f) + thisv->actor.world.pos.z;

            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            func_8002F71C(globalCtx, &thisv->actor, 5.0f, thisv->actor.yawTowardsPlayer, 0.0f);
        }

        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x200);
    }
}

void BossSst_HandSetupReadySweep(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_SWEEP;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 10.0f);
    thisv->radius = Actor_WorldDistXZToPoint(&thisv->actor, &sHead->actor.world.pos);
    thisv->actor.world.rot.y = Actor_WorldYawTowardPoint(&sHead->actor, &thisv->actor.world.pos);
    thisv->targetYaw = thisv->actor.home.rot.y + (thisv->vParity * 0x2000);
    thisv->actionFunc = BossSst_HandReadySweep;
}

void BossSst_HandReadySweep(BossSst* thisv, GlobalContext* globalCtx) {
    s32 inPosition;

    SkelAnime_Update(&thisv->skelAnime);
    inPosition = Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 50.0f, 4.0f);
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 0x200);
    inPosition &= Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->targetYaw, 0x400);
    inPosition &= (Math_SmoothStepToF(&thisv->radius, sHead->actor.xzDistToPlayer, 0.5f, 60.0f, 1.0f) < 10.0f);

    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.world.rot.y) * thisv->radius) + sHead->actor.world.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.world.rot.y) * thisv->radius) + sHead->actor.world.pos.z;
    if (inPosition) {
        BossSst_HandSetupSweep(thisv);
    } else {
        func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupSweep(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 5.0f);
    BossSst_HandSetDamage(thisv, 0x10);
    thisv->targetYaw = thisv->actor.home.rot.y - (thisv->vParity * 0x2000);
    thisv->handMaxSpeed = 0x300;
    thisv->handAngSpeed = 0;
    thisv->ready = false;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    thisv->actionFunc = BossSst_HandSweep;
}

void BossSst_HandSweep(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 newTargetYaw;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->handAngSpeed += 0x60;
    thisv->handAngSpeed = CLAMP_MAX(thisv->handAngSpeed, thisv->handMaxSpeed);

    if (!Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 4, thisv->handAngSpeed, 0x10)) {
        thisv->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
        BossSst_HandSetupRetreat(thisv);
    } else if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
        thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        thisv->ready = true;
        func_8002F71C(globalCtx, &thisv->actor, 5.0f, thisv->actor.shape.rot.y - (thisv->vParity * 0x3800), 0.0f);
        func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
        newTargetYaw = thisv->actor.shape.rot.y - (thisv->vParity * 0x1400);
        if (((s16)(newTargetYaw - thisv->targetYaw) * thisv->vParity) > 0) {
            thisv->targetYaw = newTargetYaw;
        }
    }

    if (!thisv->ready && ((player->cylinder.dim.height > 40.0f) || (player->actor.world.pos.y > 1.0f))) {
        thisv->colliderJntSph.base.atFlags |= AT_ON;
        thisv->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
    } else {
        thisv->colliderJntSph.base.atFlags &= ~AT_ON;
        thisv->colliderJntSph.base.ocFlags1 |= OC1_NO_PUSH;
    }

    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.z;
}

void BossSst_HandSetupReadyPunch(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_PUNCH;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandPushoffPoses[thisv->actor.params], 10.0f);
    thisv->actionFunc = BossSst_HandReadyPunch;
}

void BossSst_HandReadyPunch(BossSst* thisv, GlobalContext* globalCtx) {
    s32 inPosition = Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0x400);

    if (SkelAnime_Update(&thisv->skelAnime) && inPosition) {
        BossSst_HandSetupPunch(thisv);
    }
}

void BossSst_HandSetupPunch(BossSst* thisv) {
    thisv->actor.speedXZ = 0.5f;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFistPoses[thisv->actor.params], 5.0f);
    BossSst_HandSetInvulnerable(thisv, true);
    thisv->targetRoll = thisv->vParity * 0x3F00;
    BossSst_HandSetDamage(thisv, 0x10);
    thisv->actionFunc = BossSst_HandPunch;
}

void BossSst_HandPunch(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 80.0f, 20.0f);
    if (Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->targetRoll, 0x400)) {
        thisv->targetRoll *= -1;
    }

    thisv->actor.speedXZ *= 1.25f;
    thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 50.0f);

    thisv->actor.world.pos.x += thisv->actor.speedXZ * Math_SinS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.z += thisv->actor.speedXZ * Math_CosS(thisv->actor.shape.rot.y);
    if (thisv->actor.bgCheckFlags & 8) {
        BossSst_HandSetupRetreat(thisv);
    } else if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
        func_8002F71C(globalCtx, &thisv->actor, 10.0f, thisv->actor.shape.rot.y, 5.0f);
        BossSst_HandSetupRetreat(thisv);
    }

    func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupReadyClap(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_CLAP;
    if (HAND_STATE(OTHER_HAND(thisv)) != HAND_CLAP) {
        BossSst_HandSetupReadyClap(OTHER_HAND(thisv));
    }

    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 10.0f);
    thisv->radius = Actor_WorldDistXZToPoint(&thisv->actor, &sHead->actor.world.pos);
    thisv->actor.world.rot.y = Actor_WorldYawTowardPoint(&sHead->actor, &thisv->actor.world.pos);
    thisv->targetYaw = thisv->actor.home.rot.y - (thisv->vParity * 0x1800);
    thisv->targetRoll = thisv->vParity * 0x4000;
    thisv->timer = 0;
    thisv->ready = false;
    OTHER_HAND(thisv)->ready = false;
    thisv->actionFunc = BossSst_HandReadyClap;
}

void BossSst_HandReadyClap(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            BossSst_HandSetupClap(thisv);
            BossSst_HandSetupClap(OTHER_HAND(thisv));
            OTHER_HAND(thisv)->radius = thisv->radius;
        }
    } else if (!thisv->ready) {
        thisv->ready = SkelAnime_Update(&thisv->skelAnime);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x600);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->targetRoll, 0x600);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 0x200);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->targetYaw, 0x400);
        thisv->ready &= Math_SmoothStepToF(&thisv->radius, sHead->actor.xzDistToPlayer, 0.5f, 50.0f, 1.0f) < 10.0f;
        thisv->ready &= Math_SmoothStepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 30.0f, 1.0f) < 1.0f;

        thisv->actor.world.pos.x = Math_SinS(thisv->actor.world.rot.y) * thisv->radius + sHead->actor.world.pos.x;
        thisv->actor.world.pos.z = Math_CosS(thisv->actor.world.rot.y) * thisv->radius + sHead->actor.world.pos.z;
    } else if (OTHER_HAND(thisv)->ready) {
        thisv->timer = 20;
    }
}

void BossSst_HandSetupClap(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 3.0f);
    thisv->timer = 0;
    thisv->handMaxSpeed = 0x240;
    thisv->handAngSpeed = 0;
    thisv->ready = false;
    BossSst_HandSetDamage(thisv, 0x20);
    thisv->actionFunc = BossSst_HandClap;
}

void BossSst_HandClap(BossSst* thisv, GlobalContext* globalCtx) {
    static s32 dropFlag = false;
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            if (dropFlag) {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos,
                                     (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                dropFlag = false;
            }

            BossSst_HandReleasePlayer(thisv, globalCtx, true);
            BossSst_HandSetupEndClap(thisv);
        }
    } else {
        if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            OTHER_HAND(thisv)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            BossSst_HandGrabPlayer(thisv, globalCtx);
        }

        if (thisv->ready) {
            thisv->timer = 30;
            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            if (!(player->stateFlags2 & 0x80)) {
                dropFlag = true;
            }
        } else {
            thisv->handAngSpeed += 0x40;
            thisv->handAngSpeed = CLAMP_MAX(thisv->handAngSpeed, thisv->handMaxSpeed);

            if (Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, thisv->handAngSpeed)) {
                if (thisv->actor.params == BONGO_LEFT_HAND) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_CLAP);
                }
                thisv->ready = true;
            } else {
                func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
            }

            thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.x;
            thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.z;
        }
    }

    if (player->actor.parent == &thisv->actor) {
        player->unk_850 = 0;
        player->actor.world.pos = thisv->actor.world.pos;
    }
}

void BossSst_HandSetupEndClap(BossSst* thisv) {
    thisv->targetYaw = thisv->actor.home.rot.y - (thisv->vParity * 0x1000);
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 10.0f);
    thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    thisv->actionFunc = BossSst_HandEndClap;
}

void BossSst_HandEndClap(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.z, 0, 0x200);
    if (Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 0x100)) {
        BossSst_HandSetupRetreat(thisv);
    }
    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * thisv->radius) + sHead->actor.world.pos.z;
}

void BossSst_HandSetupReadyGrab(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_GRAB;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 10.0f);
    thisv->targetYaw = thisv->vParity * -0x5000;
    thisv->targetRoll = thisv->vParity * 0x4000;
    thisv->actionFunc = BossSst_HandReadyGrab;
}

void BossSst_HandReadyGrab(BossSst* thisv, GlobalContext* globalCtx) {
    s32 inPosition;

    SkelAnime_Update(&thisv->skelAnime);
    inPosition = Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->targetRoll, 4, 0x800, 0x100) == 0;
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + thisv->targetYaw, 0xA00);
    Math_ApproachF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 20.0f);
    if (inPosition) {
        BossSst_HandSetupGrab(thisv);
    }
}

void BossSst_HandSetupGrab(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFistPoses[thisv->actor.params], 5.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + (thisv->vParity * 0x4000);
    thisv->targetYaw = thisv->actor.world.rot.y;
    thisv->timer = 30;
    thisv->actor.speedXZ = 0.5f;
    BossSst_HandSetDamage(thisv, 0x20);
    thisv->actionFunc = BossSst_HandGrab;
}

void BossSst_HandGrab(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.world.rot.y =
        ((1.0f - sinf(thisv->timer * (std::numbers::pi_v<float> / 60.0f))) * (thisv->vParity * 0x2000)) + thisv->targetYaw;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y - (thisv->vParity * 0x4000);
    if (thisv->timer < 5) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.5f, 25.0f, 5.0f);
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            thisv->actor.speedXZ = 0.0f;
            if (player->stateFlags2 & 0x80) {
                if (Rand_ZeroOne() < 0.5f) {
                    BossSst_HandSetupCrush(thisv);
                } else {
                    BossSst_HandSetupSwing(thisv);
                }
            } else {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos,
                                     (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                BossSst_HandSetupRetreat(thisv);
            }
        }
    } else {
        thisv->actor.speedXZ *= 1.26f;
        thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 70.0f);
        func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }

    if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
        thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_CATCH);
        BossSst_HandGrabPlayer(thisv, globalCtx);
        thisv->timer = CLAMP_MAX(thisv->timer, 5);
    }

    thisv->actor.world.pos.x += thisv->actor.speedXZ * Math_SinS(thisv->actor.world.rot.y);
    thisv->actor.world.pos.z += thisv->actor.speedXZ * Math_CosS(thisv->actor.world.rot.y);
    if (player->stateFlags2 & 0x80) {
        player->unk_850 = 0;
        player->actor.world.pos = thisv->actor.world.pos;
        player->actor.shape.rot.y = thisv->actor.shape.rot.y;
    }
}

void BossSst_HandSetupCrush(BossSst* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, sHandClenchAnims[thisv->actor.params], -10.0f);
    thisv->timer = 20;
    thisv->actionFunc = BossSst_HandCrush;
}

void BossSst_HandCrush(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (!(player->stateFlags2 & 0x80)) {
        BossSst_HandReleasePlayer(thisv, globalCtx, true);
        BossSst_HandSetupEndCrush(thisv);
    } else {
        player->actor.world.pos = thisv->actor.world.pos;
        if (thisv->timer == 0) {
            thisv->timer = 20;
            if (!LINK_IS_ADULT) {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S_KID);
            } else {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S);
            }

            globalCtx->damagePlayer(globalCtx, -8);
        }
        if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_CATCH);
        }
    }
}

void BossSst_HandSetupEndCrush(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 10.0f);
    thisv->actionFunc = BossSst_HandEndCrush;
}

void BossSst_HandEndCrush(BossSst* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        BossSst_HandSetupRetreat(thisv);
    }
}

void BossSst_HandSetupSwing(BossSst* thisv) {
    thisv->amplitude = -0x4000;
    thisv->timer = 1;
    thisv->center.x = thisv->actor.world.pos.x - (Math_SinS(thisv->actor.shape.rot.y) * 200.0f);
    thisv->center.y = thisv->actor.world.pos.y;
    thisv->center.z = thisv->actor.world.pos.z - (Math_CosS(thisv->actor.shape.rot.y) * 200.0f);
    thisv->actionFunc = BossSst_HandSwing;
}

void BossSst_HandSwing(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 offXZ;

    if (Math_ScaledStepToS(&thisv->actor.shape.rot.x, thisv->amplitude, thisv->timer * 0xE4 + 0x1C8)) {
        if (thisv->amplitude != 0) {
            thisv->amplitude = 0;
            if (thisv->timer == 4) {
                Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 4.0f);
            }
        } else {
            if (thisv->timer == 4) {
                player->actor.shape.rot.x = 0;
                player->actor.shape.rot.z = 0;
                BossSst_HandSetupRetreat(thisv);
                return;
            }
            thisv->amplitude = (thisv->timer == 3) ? -0x6000 : -0x4000;
            thisv->timer++;
        }
    }

    thisv->actor.world.pos.y = (Math_CosS(thisv->actor.shape.rot.x + 0x4000) * 200.0f) + thisv->center.y;
    offXZ = Math_SinS(thisv->actor.shape.rot.x + 0x4000) * 200.0f;
    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * offXZ) + thisv->center.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * offXZ) + thisv->center.z;
    if (thisv->timer != 4) {
        thisv->actor.shape.rot.z = (thisv->actor.shape.rot.x + 0x4000) * thisv->vParity;
    } else {
        Math_ScaledStepToS(&thisv->actor.shape.rot.z, 0, 0x800);
    }

    if (player->stateFlags2 & 0x80) {
        player->unk_850 = 0;
        Math_Vec3f_Copy(&player->actor.world.pos, &thisv->actor.world.pos);
        player->actor.shape.rot.x = thisv->actor.shape.rot.x;
        player->actor.shape.rot.z = (thisv->vParity * -0x4000) + thisv->actor.shape.rot.z;
    } else {
        Math_ScaledStepToS(&player->actor.shape.rot.x, 0, 0x600);
        Math_ScaledStepToS(&player->actor.shape.rot.z, 0, 0x600);
        player->actor.world.pos.x += 20.0f * Math_SinS(thisv->actor.shape.rot.y);
        player->actor.world.pos.z += 20.0f * Math_CosS(thisv->actor.shape.rot.y);
    }

    if ((thisv->timer == 4) && (thisv->amplitude == 0) && SkelAnime_Update(&thisv->skelAnime) &&
        (player->stateFlags2 & 0x80)) {
        BossSst_HandReleasePlayer(thisv, globalCtx, false);
        player->actor.world.pos.x += 70.0f * Math_SinS(thisv->actor.shape.rot.y);
        player->actor.world.pos.z += 70.0f * Math_CosS(thisv->actor.shape.rot.y);
        func_8002F71C(globalCtx, &thisv->actor, 15.0f, thisv->actor.shape.rot.y, 2.0f);
        func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
    }

    func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupReel(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_DAMAGED;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 4.0f);
    thisv->timer = 36;
    Math_Vec3f_Copy(&thisv->center, &thisv->actor.world.pos);
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 200);
    thisv->actionFunc = BossSst_HandReel;
}

void BossSst_HandReel(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (!(thisv->timer % 4)) {
        if (thisv->timer % 8) {
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 4.0f);
        } else {
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFistPoses[thisv->actor.params], 6.0f);
        }
    }

    thisv->actor.colorFilterTimer = 200;
    thisv->actor.world.pos.x += Rand_CenteredFloat(20.0f);
    thisv->actor.world.pos.y += Rand_CenteredFloat(20.0f);
    thisv->actor.world.pos.z += Rand_CenteredFloat(20.0f);

    if (thisv->actor.world.pos.y < (thisv->actor.floorHeight + 100.0f)) {
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 100.0f, 20.0f);
    }

    if (thisv->timer == 0) {
        BossSst_HandSetupReadyShake(thisv);
    }
}

void BossSst_HandSetupReadyShake(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandDamagePoses[thisv->actor.params], 8.0f);
    thisv->actionFunc = BossSst_HandReadyShake;
}

void BossSst_HandReadyShake(BossSst* thisv, GlobalContext* globalCtx) {
    f32 diff;
    s32 inPosition;

    diff = Math_SmoothStepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 0.5f, 25.0f, 1.0f);
    diff += Math_SmoothStepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 0.5f, 25.0f, 1.0f);
    diff += Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 200.0f, 0.2f, 30.0f, 1.0f);
    inPosition = Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x4000, 0x400);
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.z, 0, 0x1000);
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, 0x800);
    inPosition &= Math_StepToS(&thisv->handZPosMod, -0x5DC, 0x1F4);
    inPosition &= Math_ScaledStepToS(&thisv->handYRotMod, thisv->vParity * -0x2000, 0x800);
    thisv->actor.colorFilterTimer = 200;
    if ((diff < 30.0f) && inPosition) {
        BossSst_HandSetupShake(thisv);
    } else {
        func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void BossSst_HandSetupShake(BossSst* thisv) {
    thisv->timer = 200;
    thisv->actionFunc = BossSst_HandShake;
}

void BossSst_HandShake(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.shape.rot.x = 0x4000 + (sinf(thisv->timer * (std::numbers::pi_v<float> / 5)) * 0x2000);
    thisv->handYRotMod = (thisv->vParity * -0x2000) + (sinf(thisv->timer * (std::numbers::pi_v<float> / 4)) * 0x2800);

    if (!(thisv->timer % 8)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_SHAKEHAND);
    }

    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        if ((OTHER_HAND(thisv)->actionFunc == BossSst_HandShake) ||
            (OTHER_HAND(thisv)->actionFunc == BossSst_HandReadyCharge)) {
            BossSst_HandSetupReadyCharge(thisv);
        } else if (thisv->timer == 0) {
            thisv->timer = 80;
        }
    } else if (thisv->timer == 0) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        BossSst_HandSetupSlam(thisv);
    }
}

void BossSst_HandSetupReadyCharge(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFistPoses[thisv->actor.params], 10.0f);
    thisv->ready = false;
    thisv->actionFunc = BossSst_HandReadyCharge;
}

void BossSst_HandReadyCharge(BossSst* thisv, GlobalContext* globalCtx) {
    if (!thisv->ready) {
        thisv->ready = SkelAnime_Update(&thisv->skelAnime);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x800);
        thisv->ready &=
            Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y + (thisv->vParity * 0x1000), 0x800);
        thisv->ready &= Math_ScaledStepToS(&thisv->handYRotMod, 0, 0x800);
        thisv->ready &= Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->vParity * 0x2800, 0x800);
        thisv->ready &= Math_StepToS(&thisv->handZPosMod, -0xDAC, 0x1F4);
        if (thisv->ready) {
            thisv->actor.colorFilterTimer = 0;
        }
    } else if (thisv->colliderJntSph.base.atFlags & AT_HIT) {
        thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        OTHER_HAND(thisv)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sHead->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        func_8002F71C(globalCtx, &thisv->actor, 10.0f, thisv->actor.shape.rot.y, 5.0f);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);
    }
}

void BossSst_HandSetupStunned(BossSst* hand) {
    Animation_MorphToPlayOnce(&hand->skelAnime, sHandIdleAnims[hand->actor.params], 10.0f);
    if (hand->actionFunc != BossSst_HandDamage) {
        hand->ready = false;
    }

    hand->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    hand->colliderJntSph.base.acFlags |= AC_ON;
    BossSst_HandSetInvulnerable(hand, true);
    Actor_SetColorFilter(&hand->actor, 0, 0xFF, 0, Animation_GetLastFrame(&gBongoHeadKnockoutAnim));
    hand->actionFunc = BossSst_HandStunned;
}

void BossSst_HandStunned(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.world.pos.z, (Math_CosS(sHead->actor.shape.rot.y) * 200.0f) + thisv->actor.home.pos.z,
                   0.5f, 25.0f);
    Math_ApproachF(&thisv->actor.world.pos.x, (Math_SinS(sHead->actor.shape.rot.y) * 200.0f) + thisv->actor.home.pos.x,
                   0.5f, 25.0f);
    if (!thisv->ready) {
        Math_ScaledStepToS(&thisv->handYRotMod, 0, 0x800);
        Math_StepToS(&thisv->handZPosMod, -0xDAC, 0x1F4);
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, thisv->actor.home.rot.x, 0x800);
        Math_ScaledStepToS(&thisv->actor.shape.rot.z, thisv->actor.home.rot.z, 0x800);
        Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y, 0x800);
        if (sHead->actionFunc == BossSst_HeadVulnerable) {
            thisv->ready = true;
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandDamagePoses[thisv->actor.params], 10.0f);
        }
    } else {
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 30.0f);
    }
}

void BossSst_HandSetupDamage(BossSst* hand) {
    hand->actor.shape.rot.x = 0;
    Animation_MorphToPlayOnce(&hand->skelAnime, sHandOpenPoses[hand->actor.params], 3.0f);
    hand->timer = 6;
    hand->actionFunc = BossSst_HandDamage;
}

void BossSst_HandDamage(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer >= 2) {
        thisv->actor.shape.rot.x -= 0x200;
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 200.0f, 50.0f);
    } else {
        thisv->actor.shape.rot.x += 0x400;
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 100.0f);
    }

    if (thisv->timer == 0) {
        if (thisv->actor.floorHeight >= 0.0f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
        BossSst_HandSetupStunned(thisv);
    }
}

void BossSst_HandSetupThrash(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_DEATH;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 2.0f);
    thisv->actor.shape.rot.x = 0;
    thisv->timer = 160;
    if (thisv->actor.params == BONGO_LEFT_HAND) {
        thisv->amplitude = -0x800;
    } else {
        thisv->amplitude = 0;
        thisv->actor.shape.rot.x = -0x800;
    }

    thisv->handAngSpeed = 0x180;
    thisv->actionFunc = BossSst_HandThrash;
}

void BossSst_HandThrash(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.world.pos.z, (Math_CosS(sHead->actor.shape.rot.y) * 200.0f) + thisv->actor.home.pos.z,
                   0.5f, 25.0f);
    Math_ApproachF(&thisv->actor.world.pos.x, (Math_SinS(sHead->actor.shape.rot.y) * 200.0f) + thisv->actor.home.pos.x,
                   0.5f, 25.0f);
    if (Math_ScaledStepToS(&thisv->actor.shape.rot.x, thisv->amplitude, thisv->handAngSpeed)) {
        if (thisv->amplitude != 0) {
            thisv->amplitude = 0;
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 5.0f);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            thisv->amplitude = -0x800;
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandOpenPoses[thisv->actor.params], 5.0f);
        }

        if (thisv->timer < 80.0f) {
            thisv->handAngSpeed -= 0x40;
            thisv->handAngSpeed = CLAMP_MIN(thisv->handAngSpeed, 0x40);
        }
    }

    thisv->actor.world.pos.y =
        (((thisv->handAngSpeed / 256.0f) + 0.5f) * 150.0f) * (-1.0f / 0x800) * thisv->actor.shape.rot.x;
    if (thisv->timer == 0) {
        BossSst_HandSetupDarken(thisv);
    }
}

void BossSst_HandSetupDarken(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 5.0f);
    thisv->actionFunc = BossSst_HandDarken;
}

void BossSst_HandDarken(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x800, thisv->handAngSpeed);
    Math_StepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 90.0f, 5.0f);
    if (sHead->actionFunc == BossSst_HeadFall) {
        BossSst_HandSetupFall(thisv);
    }
}

void BossSst_HandSetupFall(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFlatPoses[thisv->actor.params], 3.0f);
    thisv->actionFunc = BossSst_HandFall;
}

void BossSst_HandFall(BossSst* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x400);
    thisv->actor.world.pos.y = sHead->actor.world.pos.y + 230.0f;
    if (sHead->actionFunc == BossSst_HeadMelt) {
        BossSst_HandSetupMelt(thisv);
    }
}

void BossSst_HandSetupMelt(BossSst* thisv) {
    BossSst_SpawnHandShadow(thisv);
    thisv->actor.shape.shadowDraw = NULL;
    thisv->timer = 80;
    thisv->actionFunc = BossSst_HandMelt;
}

void BossSst_HandMelt(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.scale.y -= 0.00025f;
    thisv->actor.scale.x += 0.000025f;
    thisv->actor.scale.z += 0.000025f;
    thisv->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
    if (thisv->timer == 0) {
        BossSst_HandSetupFinish(thisv);
    }
}

void BossSst_HandSetupFinish(BossSst* thisv) {
    thisv->actor.draw = BossSst_DrawEffect;
    thisv->timer = 20;
    thisv->effects[0].status = 0;
    thisv->actionFunc = BossSst_HandFinish;
}

void BossSst_HandFinish(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        thisv->effectMode = BONGO_NULL;
    }
}

void BossSst_HandSetupRecover(BossSst* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandPushoffPoses[thisv->actor.params], 10.0f);
    thisv->ready = false;
    thisv->actionFunc = BossSst_HandRecover;
}

void BossSst_HandRecover(BossSst* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!thisv->ready) {
            Animation_MorphToPlayOnce(&thisv->skelAnime, sHandHangPoses[thisv->actor.params], 10.0f);
            thisv->ready = true;
        }
    }
    func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandSetupFrozen(BossSst* thisv) {
    s32 i;

    HAND_STATE(thisv) = HAND_FROZEN;
    Math_Vec3f_Copy(&thisv->center, &thisv->actor.world.pos);
    BossSst_HandSetupReadyBreakIce(OTHER_HAND(thisv));
    thisv->ready = false;
    thisv->effectMode = BONGO_ICE;
    thisv->timer = 35;
    for (i = 0; i < 18; i++) {
        thisv->effects[i].move = false;
    }

    BossSst_SpawnIceCrystal(thisv, 0);
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 0xA);
    thisv->handAngSpeed = 0;
    thisv->actionFunc = BossSst_HandFrozen;
}

void BossSst_HandFrozen(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if ((thisv->timer % 2) != 0) {
        BossSst_SpawnIceCrystal(thisv, (thisv->timer >> 1) + 1);
    }

    if (thisv->ready) {
        BossSst_IceShatter(thisv);
        BossSst_HandSetupRetreat(thisv);
        sHead->ready = true;
    } else {
        thisv->actor.colorFilterTimer = 10;
        if (thisv->handAngSpeed != 0) {
            f32 offY = Math_SinS(OTHER_HAND(thisv)->actor.shape.rot.x) * 5.0f;
            f32 offXZ = Math_CosS(OTHER_HAND(thisv)->actor.shape.rot.x) * 5.0f;

            if ((thisv->handAngSpeed % 2) != 0) {
                offY *= -1.0f;
                offXZ *= -1.0f;
            }

            thisv->actor.world.pos.x = thisv->center.x + (Math_CosS(OTHER_HAND(thisv)->actor.shape.rot.y) * offXZ);
            thisv->actor.world.pos.y = thisv->center.y + offY;
            thisv->actor.world.pos.z = thisv->center.z + (Math_SinS(OTHER_HAND(thisv)->actor.shape.rot.y) * offXZ);
            thisv->handAngSpeed--;
        }
    }
}

void BossSst_HandSetupReadyBreakIce(BossSst* thisv) {
    HAND_STATE(thisv) = HAND_BREAK_ICE;
    Animation_MorphToPlayOnce(&thisv->skelAnime, sHandFistPoses[thisv->actor.params], 5.0f);
    thisv->ready = false;
    thisv->actor.colorFilterTimer = 0;
    if (thisv->effectMode == BONGO_ICE) {
        thisv->effectMode = BONGO_NULL;
    }

    thisv->radius = Actor_WorldDistXZToPoint(&thisv->actor, &OTHER_HAND(thisv)->center);
    thisv->targetYaw = Actor_WorldYawTowardPoint(&thisv->actor, &OTHER_HAND(thisv)->center);
    BossSst_HandSetInvulnerable(thisv, true);
    thisv->actionFunc = BossSst_HandReadyBreakIce;
}

void BossSst_HandReadyBreakIce(BossSst* thisv, GlobalContext* globalCtx) {
    s32 inPosition;

    inPosition = Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->targetYaw, 0x400);
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x1000, 0x400);
    inPosition &= Math_ScaledStepToS(&thisv->actor.shape.rot.z, 0, 0x800);
    inPosition &= Math_ScaledStepToS(&thisv->handYRotMod, 0, 0x400);
    inPosition &= Math_StepToF(&thisv->actor.world.pos.y, OTHER_HAND(thisv)->center.y + 200.0f, 50.0f);
    inPosition &= Math_StepToF(&thisv->radius, 400.0f, 60.0f);
    thisv->actor.world.pos.x = OTHER_HAND(thisv)->center.x - (Math_SinS(thisv->targetYaw) * thisv->radius);
    thisv->actor.world.pos.z = OTHER_HAND(thisv)->center.z - (Math_CosS(thisv->targetYaw) * thisv->radius);
    if (SkelAnime_Update(&thisv->skelAnime) && inPosition) {
        BossSst_HandSetupBreakIce(thisv);
    }
}

void BossSst_HandSetupBreakIce(BossSst* thisv) {
    thisv->timer = 9;
    thisv->actionFunc = BossSst_HandBreakIce;
    thisv->actor.speedXZ = 0.5f;
}

void BossSst_HandBreakIce(BossSst* thisv, GlobalContext* globalCtx) {
    if ((thisv->timer % 2) != 0) {
        thisv->actor.speedXZ *= 1.5f;
        thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 60.0f);

        if (Math_StepToF(&thisv->radius, 100.0f, thisv->actor.speedXZ)) {
            BossSst_SpawnIceShard(thisv);
            if (thisv->timer != 0) {
                thisv->timer--;
            }

            if (thisv->timer != 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_ICE_BROKEN);
            }

            OTHER_HAND(thisv)->handAngSpeed = 5;
        }
    } else {
        thisv->actor.speedXZ *= 0.8f;
        Math_StepToF(&thisv->radius, 500.0f, thisv->actor.speedXZ);
        if (thisv->actor.speedXZ < 2.0f) {
            if (thisv->timer != 0) {
                thisv->timer--;
            }
        }
    }

    thisv->actor.world.pos.x = OTHER_HAND(thisv)->center.x - (Math_SinS(thisv->targetYaw) * thisv->radius);
    thisv->actor.world.pos.z = OTHER_HAND(thisv)->center.z - (Math_CosS(thisv->targetYaw) * thisv->radius);
    thisv->actor.world.pos.y = OTHER_HAND(thisv)->center.y + (thisv->radius * 0.4f);
    if (thisv->timer == 0) {
        OTHER_HAND(thisv)->ready = true;
        BossSst_HandSetupRetreat(thisv);
    }

    func_8002F974(&thisv->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void BossSst_HandGrabPlayer(BossSst* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->grabPlayer(globalCtx, player)) {
        player->actor.parent = &thisv->actor;
        if (player->actor.colChkInfo.health > 0) {
            thisv->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            if (HAND_STATE(thisv) == HAND_CLAP) {
                OTHER_HAND(thisv)->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            }
        }
    }
}

void BossSst_HandReleasePlayer(BossSst* thisv, GlobalContext* globalCtx, s32 dropPlayer) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->actor.parent == &thisv->actor) {
        player->actor.parent = NULL;
        player->unk_850 = 100;
        thisv->colliderJntSph.base.ocFlags1 |= OC1_ON;
        OTHER_HAND(thisv)->colliderJntSph.base.ocFlags1 |= OC1_ON;
        if (dropPlayer) {
            func_8002F71C(globalCtx, &thisv->actor, 0.0f, thisv->actor.shape.rot.y, 0.0f);
        }
    }
}

void BossSst_MoveAround(BossSst* thisv) {
    BossSst* hand;
    Vec3f* vec;
    f32 sn;
    f32 cs;
    s32 i;

    sn = Math_SinS(thisv->actor.shape.rot.y);
    cs = Math_CosS(thisv->actor.shape.rot.y);
    if (thisv->actionFunc != BossSst_HeadEndCharge) {
        thisv->actor.world.pos.x = sRoomCenter.x + (thisv->radius * sn);
        thisv->actor.world.pos.z = sRoomCenter.z + (thisv->radius * cs);
    }

    for (i = 0; i < 2; i++) {
        hand = sHands[i];
        vec = &sHandOffsets[i];

        hand->actor.world.pos.x = thisv->actor.world.pos.x + (vec->z * sn) + (vec->x * cs);
        hand->actor.world.pos.y = thisv->actor.world.pos.y + vec->y;
        hand->actor.world.pos.z = thisv->actor.world.pos.z + (vec->z * cs) - (vec->x * sn);

        hand->actor.home.pos.x = thisv->actor.world.pos.x + (400.0f * sn) + (-200.0f * hand->vParity * cs);
        hand->actor.home.pos.y = thisv->actor.world.pos.y;
        hand->actor.home.pos.z = thisv->actor.world.pos.z + (400.0f * cs) - (-200.0f * hand->vParity * sn);

        hand->actor.home.rot.y = thisv->actor.shape.rot.y;
        hand->actor.shape.rot.y = sHandYawOffsets[i] + thisv->actor.shape.rot.y;

        if (hand->actor.world.pos.y < hand->actor.floorHeight) {
            hand->actor.world.pos.y = hand->actor.floorHeight;
        }
    }
}

void BossSst_HandSelectAttack(BossSst* thisv) {
    f32 rand = Rand_ZeroOne() * 6.0f;
    s32 randInt;

    if (HAND_STATE(OTHER_HAND(thisv)) == HAND_DAMAGED) {
        rand *= 5.0f / 6;
        if (rand > 4.0f) {
            rand = 4.0f;
        }
    }

    randInt = rand;
    if (randInt == 0) {
        BossSst_HandSetupReadySlam(thisv);
    } else if (randInt == 1) {
        BossSst_HandSetupReadySweep(thisv);
    } else if (randInt == 2) {
        BossSst_HandSetupReadyPunch(thisv);
    } else if (randInt == 5) {
        BossSst_HandSetupReadyClap(thisv);
    } else { // randInt == 3 || randInt == 4
        BossSst_HandSetupReadyGrab(thisv);
    }
}

void BossSst_HandSetDamage(BossSst* thisv, s32 damage) {
    s32 i;

    thisv->colliderJntSph.base.atFlags |= AT_ON;
    for (i = 0; i < 11; i++) {
        thisv->colliderJntSph.elements[i].info.toucher.damage = damage;
    }
}

void BossSst_HandSetInvulnerable(BossSst* thisv, s32 isInv) {
    thisv->colliderJntSph.base.acFlags &= ~AC_HIT;
    if (isInv) {
        thisv->colliderJntSph.base.colType = COLTYPE_HARD;
        thisv->colliderJntSph.base.acFlags |= AC_HARD;
    } else {
        thisv->colliderJntSph.base.colType = COLTYPE_HIT0;
        thisv->colliderJntSph.base.acFlags &= ~AC_HARD;
    }
}

void BossSst_HeadSfx(BossSst* thisv, u16 sfxId) {
    func_80078914(&thisv->center, sfxId);
}

void BossSst_HandCollisionCheck(BossSst* thisv, GlobalContext* globalCtx) {
    if ((thisv->colliderJntSph.base.acFlags & AC_HIT) && (thisv->colliderJntSph.base.colType != COLTYPE_HARD)) {
        s32 bothHands = true;

        thisv->colliderJntSph.base.acFlags &= ~AC_HIT;
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            thisv->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            thisv->colliderJntSph.base.acFlags &= ~AC_ON;
            thisv->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
            BossSst_HandReleasePlayer(thisv, globalCtx, true);
            if (HAND_STATE(OTHER_HAND(thisv)) == HAND_CLAP) {
                BossSst_HandReleasePlayer(OTHER_HAND(thisv), globalCtx, true);
                BossSst_HandSetupRetreat(OTHER_HAND(thisv));
            }

            thisv->actor.flags &= ~ACTOR_FLAG_0;
            if (thisv->actor.colChkInfo.damageEffect == 3) {
                BossSst_HandSetupFrozen(thisv);
            } else {
                BossSst_HandSetupReel(thisv);
                if (HAND_STATE(OTHER_HAND(thisv)) != HAND_DAMAGED) {
                    bothHands = false;
                }
            }

            BossSst_HeadSetupDamagedHand(sHead, bothHands);
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos,
                                 (Rand_ZeroOne() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_DAMAGE_HAND);
        }
    }
}

void BossSst_HeadCollisionCheck(BossSst* thisv, GlobalContext* globalCtx) {
    if (thisv->colliderCyl.base.acFlags & AC_HIT) {
        thisv->colliderCyl.base.acFlags &= ~AC_HIT;
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (thisv->actionFunc == BossSst_HeadVulnerable) {
                if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                    BossSst_HeadSetupDeath(thisv, globalCtx);
                } else {
                    BossSst_HeadSetupDamage(thisv);
                }

                BossSst_HandSetupDamage(sHands[LEFT]);
                BossSst_HandSetupDamage(sHands[RIGHT]);
            } else {
                BossSst_HeadSetupStunned(thisv);
                if (HAND_STATE(sHands[RIGHT]) == HAND_FROZEN) {
                    BossSst_IceShatter(sHands[RIGHT]);
                } else if (HAND_STATE(sHands[LEFT]) == HAND_FROZEN) {
                    BossSst_IceShatter(sHands[LEFT]);
                }

                BossSst_HandSetupStunned(sHands[RIGHT]);
                BossSst_HandSetupStunned(sHands[LEFT]);
            }
        }
    }
}

void BossSst_UpdateHand(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossSst* thisv = (BossSst*)thisx;
    BossSstHandTrail* trail;

    if (thisv->colliderCyl.base.atFlags & AT_ON) {
        if ((thisv->effects[0].move < 5) ||
            (thisv->actor.xzDistToPlayer < ((thisv->effects[2].scale * 0.01f) * sCylinderInitHand.dim.radius)) ||
            (thisv->colliderCyl.base.atFlags & AT_HIT)) {
            thisv->colliderCyl.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            thisv->colliderCyl.dim.radius = (thisv->effects[0].scale * 0.01f) * sCylinderInitHand.dim.radius;
        }
    }

    BossSst_HandCollisionCheck(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 130.0f, 0.0f, 5);
    Actor_SetFocus(&thisv->actor, 0.0f);
    if (thisv->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if ((sHead->actionFunc != BossSst_HeadLurk) && (sHead->actionFunc != BossSst_HeadIntro) &&
        (thisv->colliderJntSph.base.acFlags & AC_ON)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if (thisv->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if (thisv->colliderCyl.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCyl.base);
    }

    if ((HAND_STATE(thisv) != HAND_DEATH) && (HAND_STATE(thisv) != HAND_WAIT) && (HAND_STATE(thisv) != HAND_BEAT) &&
        (HAND_STATE(thisv) != HAND_FROZEN)) {
        thisv->trailCount++;
        thisv->trailCount = CLAMP_MAX(thisv->trailCount, 7);
    } else {
        thisv->trailCount--;
        thisv->trailCount = CLAMP_MIN(thisv->trailCount, 0);
    }

    trail = &thisv->handTrails[thisv->trailIndex];
    Math_Vec3f_Copy(&trail->world.pos, &thisv->actor.world.pos);
    trail->world.rot = thisv->actor.shape.rot;
    trail->zPosMod = thisv->handZPosMod;
    trail->yRotMod = thisv->handYRotMod;

    thisv->trailIndex = (thisv->trailIndex + 1) % 7;
    BossSst_UpdateEffect(&thisv->actor, globalCtx);
}

void BossSst_UpdateHead(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossSst* thisv = (BossSst*)thisx;

    func_8002DBD0(&thisv->actor, &sHandOffsets[RIGHT], &sHands[RIGHT]->actor.world.pos);
    func_8002DBD0(&thisv->actor, &sHandOffsets[LEFT], &sHands[LEFT]->actor.world.pos);

    sHandYawOffsets[LEFT] = sHands[LEFT]->actor.shape.rot.y - thisx->shape.rot.y;
    sHandYawOffsets[RIGHT] = sHands[RIGHT]->actor.shape.rot.y - thisx->shape.rot.y;

    BossSst_HeadCollisionCheck(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->vVanish) {
        if ((globalCtx->actorCtx.unk_03 == 0) || (thisx->colorFilterTimer != 0)) {
            thisv->actor.flags &= ~ACTOR_FLAG_7;
        } else {
            thisv->actor.flags |= ACTOR_FLAG_7;
        }
    }

    if (thisv->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if ((thisv->actionFunc != BossSst_HeadLurk) && (thisv->actionFunc != BossSst_HeadIntro)) {
        if (thisv->colliderCyl.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCyl.base);
        }
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    if (thisv->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
    }

    BossSst_MoveAround(thisv);
    if ((!thisv->vVanish || CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) &&
        ((thisv->actionFunc == BossSst_HeadReadyCharge) || (thisv->actionFunc == BossSst_HeadCharge) ||
         (thisv->actionFunc == BossSst_HeadFrozenHand) || (thisv->actionFunc == BossSst_HeadStunned) ||
         (thisv->actionFunc == BossSst_HeadVulnerable) || (thisv->actionFunc == BossSst_HeadDamage))) {
        thisv->actor.flags |= ACTOR_FLAG_0;
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }

    if (thisv->actionFunc == BossSst_HeadCharge) {
        BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_MOVE - SFX_FLAG);
    }

    BossSst_UpdateEffect(&thisv->actor, globalCtx);
}

s32 BossSst_OverrideHandDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    BossSst* thisv = (BossSst*)thisx;

    if (limbIndex == 1) {
        pos->z += thisv->handZPosMod;
        rot->y += thisv->handYRotMod;
    }
    return false;
}

void BossSst_PostHandDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    BossSst* thisv = (BossSst*)thisx;

    Collider_UpdateSpheres(limbIndex, &thisv->colliderJntSph);
}

s32 BossSst_OverrideHandTrailDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* data, Gfx** gfx) {
    BossSstHandTrail* trail = (BossSstHandTrail*)data;

    if (limbIndex == 1) {
        pos->z += trail->zPosMod;
        rot->y += trail->yRotMod;
    }
    return false;
}

void BossSst_DrawHand(Actor* thisx, GlobalContext* globalCtx) {
    BossSst* thisv = (BossSst*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 6563);

    func_80093D18(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, sBodyColor.r, sBodyColor.g, sBodyColor.b, 255);

    if (!sBodyStatic) {
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, sStaticColor.r, sStaticColor.g, sStaticColor.b, 0);
        gSPSegment(POLY_OPA_DISP++, 0x08, sBodyStaticDList);
    }

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          BossSst_OverrideHandDraw, BossSst_PostHandDraw, thisv);
    if (thisv->trailCount >= 2) {
        BossSstHandTrail* trail;
        BossSstHandTrail* trail2;
        s32 i;
        s32 idx;
        s32 end;
        s32 pad;

        func_80093D84(globalCtx->state.gfxCtx);

        end = thisv->trailCount >> 1;
        idx = (thisv->trailIndex + 4) % 7;
        trail = &thisv->handTrails[idx];
        trail2 = &thisv->handTrails[(idx + 2) % 7];

        for (i = 0; i < end; i++) {
            if (Math3D_Vec3fDistSq(&trail2->world.pos, &trail->world.pos) > 900.0f) {
                Matrix_SetTranslateRotateYXZ(trail->world.pos.x, trail->world.pos.y, trail->world.pos.z,
                                             &trail->world.rot);
                Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);

                gSPSegment(POLY_XLU_DISP++, 0x08, sHandTrailDList);
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, ((3 - i) * 10) + 20, 0, ((3 - i) * 20) + 50,
                                ((3 - i) * 30) + 70);

                POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                                   thisv->skelAnime.dListCount, BossSst_OverrideHandTrailDraw, NULL,
                                                   trail, POLY_XLU_DISP);
            }
            idx = (idx + 5) % 7;
            trail2 = trail;
            trail = &thisv->handTrails[idx];
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 6654);

    BossSst_DrawEffect(&thisv->actor, globalCtx);
}

s32 BossSst_OverrideHeadDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    BossSst* thisv = (BossSst*)thisx;
    s32 shakeAmp;
    s32 pad;
    s32 timer12;
    f32 shakeMod;

    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7) && thisv->vVanish) {
        *dList = NULL;
    } else if (thisv->actionFunc == BossSst_HeadThrash) { // Animation modifications for death cutscene
        shakeAmp = (thisv->timer / 10) + 1;
        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {

            shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
            rot->x += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf((thisv->timer % 5) * (std::numbers::pi_v<float> / 5));
            rot->z -= ((0x800 * Rand_ZeroOne() + 0x1000) / 0x10) * shakeAmp * shakeMod + 0x1000;

            if (limbIndex == 3) {

                shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
                rot->y += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;
            }
        } else if ((limbIndex == 5) || (limbIndex == 6)) {

            shakeMod = sinf((thisv->timer % 5) * (std::numbers::pi_v<float> / 5));
            rot->z -= ((0x280 * Rand_ZeroOne() + 0x500) / 0x10) * shakeAmp * shakeMod + 0x500;

            if (limbIndex == 5) {

                shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
                rot->x += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;

                shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
                rot->y += ((0x500 * Rand_ZeroOne() + 0xA00) / 0x10) * shakeAmp * shakeMod;
            }
        } else if (limbIndex == 2) {
            shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
            rot->x += ((0x200 * Rand_ZeroOne() + 0x400) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf(thisv->timer * (std::numbers::pi_v<float> / 5));
            rot->y += ((0x200 * Rand_ZeroOne() + 0x400) / 0x10) * shakeAmp * shakeMod;

            shakeMod = sinf((thisv->timer % 5) * (std::numbers::pi_v<float> / 5));
            rot->z -= ((0x100 * Rand_ZeroOne() + 0x200) / 0x10) * shakeAmp * shakeMod + 0x200;
        }
    } else if (thisv->actionFunc == BossSst_HeadDeath) {
        if (thisv->timer > 48) {
            timer12 = thisv->timer - 36;
        } else {
            pad = ((thisv->timer > 6) ? 6 : thisv->timer);
            timer12 = pad * 2;
        }

        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x2000 * sinf(timer12 * (std::numbers::pi_v<float> / 24));
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0xA00 * sinf(timer12 * (std::numbers::pi_v<float> / 24));
        } else if (limbIndex == 2) {
            rot->z -= 0x400 * sinf(timer12 * (std::numbers::pi_v<float> / 24));
        }
    } else if ((thisv->actionFunc == BossSst_HeadDarken) || (thisv->actionFunc == BossSst_HeadFall) ||
               (thisv->actionFunc == BossSst_HeadMelt)) {
        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x1000;
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0x500;
        } else if (limbIndex == 2) {
            rot->z -= 0x200;
        }
    }
    return false;
}

void BossSst_PostHeadDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Vec3f headVec = { 1000.0f, 0.0f, 0.0f };
    BossSst* thisv = (BossSst*)thisx;
    Vec3f headPos;

    if (limbIndex == 8) {
        Matrix_MultVec3f(&zeroVec, &thisv->actor.focus.pos);
        Matrix_MultVec3f(&headVec, &headPos);
        thisv->colliderCyl.dim.pos.x = headPos.x;
        thisv->colliderCyl.dim.pos.y = headPos.y;
        thisv->colliderCyl.dim.pos.z = headPos.z;
    }

    Collider_UpdateSpheres(limbIndex, &thisv->colliderJntSph);
}

void BossSst_DrawHead(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossSst* thisv = (BossSst*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 6810);

    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
        func_80093D18(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, sBodyColor.r, sBodyColor.g, sBodyColor.b, 255);
        if (!sBodyStatic) {
            gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, sStaticColor.r, sStaticColor.g, sStaticColor.b, 0);
            gSPSegment(POLY_OPA_DISP++, 0x08, sBodyStaticDList);
        }
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, 255);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[2]);
    }

    if (thisv->actionFunc == BossSst_HeadThrash) {
        f32 randPitch = Rand_ZeroOne() * (2 * std::numbers::pi_v<float>);
        f32 randYaw = Rand_ZeroOne() * (2 * std::numbers::pi_v<float>);

        Matrix_RotateY(randYaw, MTXMODE_APPLY);
        Matrix_RotateX(randPitch, MTXMODE_APPLY);
        Matrix_Scale((thisv->timer * 0.000375f) + 1.0f, 1.0f - (thisv->timer * 0.00075f),
                     (thisv->timer * 0.000375f) + 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(-randPitch, MTXMODE_APPLY);
        Matrix_RotateY(-randYaw, MTXMODE_APPLY);
    }

    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
        POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, BossSst_OverrideHeadDraw, BossSst_PostHeadDraw,
                                           thisv, POLY_OPA_DISP);
    } else {
        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, BossSst_OverrideHeadDraw, BossSst_PostHeadDraw,
                                           thisv, POLY_XLU_DISP);
    }

    if ((thisv->actionFunc == BossSst_HeadIntro) && (113 >= thisv->timer) && (thisv->timer > 20)) {
        s32 yOffset;
        Vec3f vanishMaskPos;
        Vec3f vanishMaskOffset;

        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 18, 255);

        yOffset = 113 * 8 - thisv->timer * 8;
        vanishMaskPos.x = ROOM_CENTER_X + 85.0f;
        vanishMaskPos.y = ROOM_CENTER_Y - 250.0f + yOffset;
        vanishMaskPos.z = ROOM_CENTER_Z + 190.0f;
        if (vanishMaskPos.y > 450.0f) {
            vanishMaskPos.y = 450.0f;
        }

        Matrix_MultVec3fExt(&vanishMaskPos, &vanishMaskOffset, &globalCtx->billboardMtxF);
        Matrix_Translate(thisv->actor.world.pos.x + vanishMaskOffset.x, thisv->actor.world.pos.y + vanishMaskOffset.y,
                         thisv->actor.world.pos.z + vanishMaskOffset.z, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_sst.c", 6934),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, sIntroVanishDList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 6941);

    SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &thisv->actor.focus.pos, &thisv->center);
    BossSst_DrawEffect(&thisv->actor, globalCtx);
}

void BossSst_SpawnHeadShadow(BossSst* thisv) {
    static Vec3f shadowOffset[] = {
        { 0.0f, 0.0f, 340.0f },
        { -160.0f, 0.0f, 250.0f },
        { 160.0f, 0.0f, 250.0f },
    };
    s32 pad;
    s32 i;
    f32 sn;
    f32 cs;

    thisv->effectMode = BONGO_SHADOW;
    sn = Math_SinS(thisv->actor.shape.rot.y);
    cs = Math_CosS(thisv->actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        BossSstEffect* shadow = &thisv->effects[i];
        Vec3f* offset = &shadowOffset[i];

        shadow->pos.x = thisv->actor.world.pos.x + (sn * offset->z) + (cs * offset->x);
        shadow->pos.y = 0.0f;
        shadow->pos.z = thisv->actor.world.pos.z + (cs * offset->z) - (sn * offset->x);

        shadow->scale = 1450;
        shadow->alpha = 254;
        shadow->status = 65;
    }

    thisv->effects[3].status = -1;
}

void BossSst_SpawnHandShadow(BossSst* thisv) {
    thisv->effectMode = BONGO_SHADOW;
    thisv->effects[0].pos.x = thisv->actor.world.pos.x + (Math_CosS(thisv->actor.shape.rot.y) * 30.0f * thisv->vParity);
    thisv->effects[0].pos.z = thisv->actor.world.pos.z - (Math_SinS(thisv->actor.shape.rot.y) * 30.0f * thisv->vParity);
    thisv->effects[0].pos.y = thisv->actor.world.pos.y;
    thisv->effects[0].scale = 2300;
    thisv->effects[0].alpha = 254;
    thisv->effects[0].status = 5;
    thisv->effects[1].status = -1;
}

void BossSst_SpawnShockwave(BossSst* thisv) {
    s32 i;
    s32 scale = 120;
    s32 alpha = 250;

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHADEST_HAND_WAVE);
    thisv->effectMode = BONGO_SHOCKWAVE;

    for (i = 0; i < 3; i++) {
        BossSstEffect* shockwave = &thisv->effects[i];

        Math_Vec3f_Copy(&shockwave->pos, &thisv->actor.world.pos);
        shockwave->move = (i + 9) * 2;
        shockwave->scale = scale;
        shockwave->alpha = alpha / shockwave->move;
        scale -= 50;
        alpha -= 25;
    }
}

void BossSst_SpawnIceCrystal(BossSst* thisv, s32 index) {
    BossSstEffect* ice = &thisv->effects[index];
    Sphere16* sphere;

    if (index < 11) {
        sphere = &thisv->colliderJntSph.elements[index].dim.worldSphere;

        ice->pos.x = sphere->center.x;
        ice->pos.y = sphere->center.y;
        ice->pos.z = sphere->center.z;
        if (index == 0) {
            ice->pos.x -= 25.0f;
            ice->pos.y -= 25.0f;
            ice->pos.z -= 25.0f;
        }
    } else {
        sphere = &thisv->colliderJntSph.elements[0].dim.worldSphere;

        ice->pos.x = ((((index - 11) & 1) ? 1 : -1) * 25.0f) + sphere->center.x;
        ice->pos.y = ((((index - 11) & 2) ? 1 : -1) * 25.0f) + sphere->center.y;
        ice->pos.z = ((((index - 11) & 4) ? 1 : -1) * 25.0f) + sphere->center.z;
    }

    ice->pos.x -= thisv->actor.world.pos.x;
    ice->pos.y -= thisv->actor.world.pos.y;
    ice->pos.z -= thisv->actor.world.pos.z;

    ice->status = 0;

    ice->rot.x = Rand_ZeroOne() * 0x10000;
    ice->rot.y = Rand_ZeroOne() * 0x10000;
    ice->rot.z = Rand_ZeroOne() * 0x10000;

    ice->alpha = 120;
    ice->move = true;

    ice->vel.x = (Rand_ZeroOne() * 0.06f + 0.12f) * ice->pos.x;
    ice->vel.y = (Rand_ZeroOne() * 15.0f + 5.0f);
    ice->vel.z = (Rand_ZeroOne() * 0.06f + 0.12f) * ice->pos.z;
    ice->scale = 4000;

    if ((index % 2) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_FREEZE_S);
    }
}

void BossSst_SpawnIceShard(BossSst* thisv) {
    s32 i;
    Vec3f spawnPos;
    f32 offXZ;

    thisv->effectMode = BONGO_ICE;
    offXZ = Math_CosS(thisv->actor.shape.rot.x) * 50.0f;
    spawnPos.x = Math_CosS(thisv->actor.shape.rot.y) * offXZ + thisv->actor.world.pos.x;
    spawnPos.y = Math_SinS(thisv->actor.shape.rot.x) * 50.0f + thisv->actor.world.pos.y - 10.0f;
    spawnPos.z = Math_SinS(thisv->actor.shape.rot.y) * offXZ + thisv->actor.world.pos.z;

    for (i = 0; i < 18; i++) {
        BossSstEffect* ice = &thisv->effects[i];

        Math_Vec3f_Copy(&ice->pos, &spawnPos);
        ice->status = 1;
        ice->rot.x = Rand_ZeroOne() * 0x10000;
        ice->rot.y = Rand_ZeroOne() * 0x10000;
        ice->rot.z = Rand_ZeroOne() * 0x10000;

        ice->alpha = 120;
        ice->move = true;

        ice->vel.x = Rand_CenteredFloat(20.0f);
        ice->vel.y = Rand_ZeroOne() * 10.0f + 3.0f;
        ice->vel.z = Rand_CenteredFloat(20.0f);

        ice->scale = Rand_ZeroOne() * 200.0f + 400.0f;
    }
}

void BossSst_IceShatter(BossSst* thisv) {
    s32 i;

    thisv->effects[0].status = 1;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_ICE_BROKEN);

    for (i = 0; i < 18; i++) {
        BossSstEffect* ice = &thisv->effects[i];

        if (ice->move) {
            ice->pos.x += thisv->actor.world.pos.x;
            ice->pos.y += thisv->actor.world.pos.y;
            ice->pos.z += thisv->actor.world.pos.z;
        }
    }
}

void BossSst_UpdateEffect(Actor* thisx, GlobalContext* globalCtx) {
    BossSst* thisv = (BossSst*)thisx;
    BossSstEffect* effect;
    s32 i;

    if (thisv->effectMode != BONGO_NULL) {
        if (thisv->effectMode == BONGO_ICE) {
            if (thisv->effects[0].status) {
                for (i = 0; i < 18; i++) {
                    effect = &thisv->effects[i];

                    if (effect->move) {
                        effect->pos.x += effect->vel.x;
                        effect->pos.y += effect->vel.y;
                        effect->pos.z += effect->vel.z;
                        effect->alpha -= 3;
                        effect->vel.y -= 1.0f;
                        effect->rot.x += 0xD00;
                        effect->rot.y += 0x1100;
                        effect->rot.z += 0x1500;
                    }
                }
            }
            if (thisv->effects[0].alpha == 0) {
                thisv->effectMode = BONGO_NULL;
            }
        } else if (thisv->effectMode == BONGO_SHOCKWAVE) {
            for (i = 0; i < 3; i++) {
                BossSstEffect* effect2 = &thisv->effects[i];
                s32 scale = effect2->move * 2;

                effect2->scale += CLAMP_MAX(scale, 20) + i;
                if (effect2->move != 0) {
                    effect2->move--;
                }
            }

            if (thisv->effects[0].move == 0) {
                thisv->effectMode = BONGO_NULL;
            }
        } else if (thisv->effectMode == BONGO_SHADOW) {
            effect = &thisv->effects[0];

            if (thisv->actor.params == BONGO_HEAD) {
                SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &thisv->actor.focus.pos, &thisv->center);
                BossSst_HeadSfx(thisv, NA_SE_EN_SHADEST_LAST - SFX_FLAG);
            }
            while (effect->status != -1) {
                if (effect->status == 0) {
                    effect->alpha -= 2;
                } else {
                    effect->scale += effect->status;
                }

                effect->scale = CLAMP_MAX(effect->scale, 10000);
                effect++;
            }
        }
    }
}

void BossSst_DrawEffect(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossSst* thisv = (BossSst*)thisx;
    s32 i;
    BossSstEffect* effect;

    if (thisv->effectMode != BONGO_NULL) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 7302);

        func_80093D84(globalCtx->state.gfxCtx);
        if (thisv->effectMode == BONGO_ICE) {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, globalCtx->gameplayFrames % 256, 0x20, 0x10, 1,
                                        0, (globalCtx->gameplayFrames * 2) % 256, 0x40, 0x20));
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, thisv->effects[0].alpha);
            gSPDisplayList(POLY_XLU_DISP++, gBongoIceCrystalDL);

            for (i = 0; i < 18; i++) {
                effect = &thisv->effects[i];
                if (effect->move) {
                    func_8003435C(&effect->pos, globalCtx);
                    if (thisv->effects[0].status != 0) {
                        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    } else {
                        Matrix_Translate(effect->pos.x + thisv->actor.world.pos.x,
                                         effect->pos.y + thisv->actor.world.pos.y,
                                         effect->pos.z + thisv->actor.world.pos.z, MTXMODE_NEW);
                    }

                    Matrix_RotateZYX(effect->rot.x, effect->rot.y, effect->rot.z, MTXMODE_APPLY);
                    Matrix_Scale(effect->scale * 0.001f, effect->scale * 0.001f, effect->scale * 0.001f, MTXMODE_APPLY);

                    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_sst.c", 7350),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(POLY_XLU_DISP++, gBongoIceShardDL);
                }
            }
        } else if (thisv->effectMode == BONGO_SHOCKWAVE) {
            f32 scaleY = 0.005f;

            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128, 0, 0x20, 0x40, 1,
                                        0, (globalCtx->gameplayFrames * -15) % 256, 0x20, 0x40));

            for (i = 0; i < 3; i++, scaleY -= 0.001f) {
                effect = &thisv->effects[i];

                if (effect->move != 0) {
                    Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    Matrix_Scale(effect->scale * 0.001f, scaleY, effect->scale * 0.001f, MTXMODE_APPLY);

                    gDPPipeSync(POLY_XLU_DISP++);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 30, 0, 30, effect->alpha * effect->move);
                    gDPSetEnvColor(POLY_XLU_DISP++, 30, 0, 30, 0);
                    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_sst.c", 7396),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);
                }
            }
        } else if (thisv->effectMode == BONGO_SHADOW) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 10, 10, 80, 0);
            gDPSetEnvColor(POLY_XLU_DISP++, 10, 10, 10, thisv->effects[0].alpha);

            effect = &thisv->effects[0];
            while (effect->status != -1) {
                Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                Matrix_Scale(effect->scale * 0.001f, 1.0f, effect->scale * 0.001f, MTXMODE_APPLY);

                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_sst.c", 7423),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, sShadowDList);
                effect++;
            }
        }

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_sst.c", 7433);
    }
}

void BossSst_Reset(void) {
    sHead = NULL;
    sHands[0] = NULL;
    sHands[1] = NULL;
    sFloor = NULL;
    sHandOffsets[0].x = 0.0f;
    sHandOffsets[0].y = 0.0f;
    sHandOffsets[0].z = 0.0f;
    
    sHandOffsets[1].x = 0.0f;
    sHandOffsets[1].y = 0.0f;
    sHandOffsets[1].z = 0.0f;

    sHandYawOffsets[0] = 0;
    sHandYawOffsets[1] = 0;

    sCutsceneCamera= 0;
    sBodyStatic = false;
}