/*
 * File: z_boss_fd.c
 * Overlay: ovl_Boss_Fd
 * Description: Volvagia, flying form
 */

#include "z_boss_fd.h"
#include "objects/object_fd/object_fd.h"
#include "overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.h"
#include "overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.h"
#include "overlays/actors/ovl_Boss_Fd2/z_boss_fd2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ INTRO_FLY_EMERGE,
    /* 1 */ INTRO_FLY_HOLE,
    /* 2 */ INTRO_FLY_CAMERA,
    /* 3 */ INTRO_FLY_RETRAT
} BossFdIntroFlyState;

typedef enum {
    /* 0 */ MANE_CENTER,
    /* 1 */ MANE_RIGHT,
    /* 2 */ MANE_LEFT
} BossFdManeIndex;

typedef enum {
    /* 0 */ EYE_OPEN,
    /* 1 */ EYE_HALF,
    /* 2 */ EYE_CLOSED
} BossFdEyeState;

void BossFd_Init(Actor* thisx, GlobalContext* globalCtx);
void BossFd_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossFd_Update(Actor* thisx, GlobalContext* globalCtx);
void BossFd_Draw(Actor* thisx, GlobalContext* globalCtx);

void BossFd_SetupFly(BossFd* thisv, GlobalContext* globalCtx);
void BossFd_Fly(BossFd* thisv, GlobalContext* globalCtx);
void BossFd_Wait(BossFd* thisv, GlobalContext* globalCtx);
void BossFd_UpdateEffects(BossFd* thisv, GlobalContext* globalCtx);
void BossFd_DrawBody(GlobalContext* globalCtx, BossFd* thisv);

ActorInit Boss_Fd_InitVars = {
    ACTOR_BOSS_FD,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FD,
    sizeof(BossFd),
    (ActorFunc)BossFd_Init,
    (ActorFunc)BossFd_Destroy,
    (ActorFunc)BossFd_Update,
    (ActorFunc)BossFd_Draw,
    NULL,
};

#include "z_boss_fd_colchk.cpp"

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x21, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void BossFd_SpawnEmber(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)Rand_ZeroFloat(10.0f);
            break;
        }
    }
}

void BossFd_SpawnDebris(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = Rand_ZeroFloat(100.0f);
            effect->vFdFxRotY = Rand_ZeroFloat(100.0f);
            break;
        }
    }
}

void BossFd_SpawnDust(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            break;
        }
    }
}

void BossFd_SpawnFireBreath(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale,
                            s16 alpha, s16 kbAngle) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_FIRE_BREATH;
            effect->timer1 = 0;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->pos.x -= effect->velocity.x;
            effect->pos.y -= effect->velocity.y;
            effect->pos.z -= effect->velocity.z;
            effect->vFdFxScaleMod = 0.0f;
            effect->alpha = alpha;
            effect->vFdFxYStop = Rand_ZeroFloat(10.0f);
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            effect->kbAngle = kbAngle;
            break;
        }
    }
}

void BossFd_SetCameraSpeed(BossFd* thisv, f32 speedMod) {
    thisv->camData.eyeVel.x = fabsf(thisv->camData.eye.x - thisv->camData.nextEye.x) * speedMod;
    thisv->camData.eyeVel.y = fabsf(thisv->camData.eye.y - thisv->camData.nextEye.y) * speedMod;
    thisv->camData.eyeVel.z = fabsf(thisv->camData.eye.z - thisv->camData.nextEye.z) * speedMod;
    thisv->camData.atVel.x = fabsf(thisv->camData.at.x - thisv->camData.nextAt.x) * speedMod;
    thisv->camData.atVel.y = fabsf(thisv->camData.at.y - thisv->camData.nextAt.y) * speedMod;
    thisv->camData.atVel.z = fabsf(thisv->camData.at.z - thisv->camData.nextAt.z) * speedMod;
}

void BossFd_UpdateCamera(BossFd* thisv, GlobalContext* globalCtx) {
    if (thisv->introCamera != SUBCAM_FREE) {
        Math_ApproachF(&thisv->camData.eye.x, thisv->camData.nextEye.x, thisv->camData.eyeMaxVel.x,
                       thisv->camData.eyeVel.x * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.eye.y, thisv->camData.nextEye.y, thisv->camData.eyeMaxVel.y,
                       thisv->camData.eyeVel.y * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.eye.z, thisv->camData.nextEye.z, thisv->camData.eyeMaxVel.z,
                       thisv->camData.eyeVel.z * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.at.x, thisv->camData.nextAt.x, thisv->camData.atMaxVel.x,
                       thisv->camData.atVel.x * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.at.y, thisv->camData.nextAt.y, thisv->camData.atMaxVel.y,
                       thisv->camData.atVel.y * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.at.z, thisv->camData.nextAt.z, thisv->camData.atMaxVel.z,
                       thisv->camData.atVel.z * thisv->camData.speedMod);
        Math_ApproachF(&thisv->camData.speedMod, 1.0f, 1.0f, thisv->camData.accel);
        thisv->camData.at.y += thisv->camData.yMod;
        Gameplay_CameraSetAtEye(globalCtx, thisv->introCamera, &thisv->camData.at, &thisv->camData.eye);
        Math_ApproachZeroF(&thisv->camData.yMod, 1.0f, 0.1f);
    }
}

void BossFd_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd* thisv = (BossFd*)thisx;
    s16 i;

    Flags_SetSwitch(globalCtx, 0x14);
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BG_VB_SIMA, 680.0f, -100.0f, 0.0f, 0, 0, 0,
                       100);
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&thisv->actor, 0.05f);
    SkelAnime_Init(globalCtx, &thisv->skelAnimeHead, &gVolvagiaHeadSkel, &gVolvagiaHeadEmergeAnim, NULL, NULL, 0);
    SkelAnime_Init(globalCtx, &thisv->skelAnimeRightArm, &gVolvagiaRightArmSkel, &gVolvagiaRightArmEmergeAnim, NULL,
                   NULL, 0);
    SkelAnime_Init(globalCtx, &thisv->skelAnimeLeftArm, &gVolvagiaLeftArmSkel, &gVolvagiaLeftArmEmergeAnim, NULL, NULL,
                   0);
    thisv->introState = BFD_CS_WAIT;
    if (thisv->introState == BFD_CS_NONE) {
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_FIRE_BOSS);
    }

    thisv->actor.world.pos.x = thisv->actor.world.pos.z = 0.0f;
    thisv->actor.world.pos.y = -200.0f;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->elements);

    for (i = 0; i < 100; i++) {
        thisv->bodySegsPos[i].x = thisv->actor.world.pos.x;
        thisv->bodySegsPos[i].y = thisv->actor.world.pos.y;
        thisv->bodySegsPos[i].z = thisv->actor.world.pos.z;
        if (i < 30) {
            thisv->centerMane.pos[i].x = thisv->actor.world.pos.x;
            thisv->centerMane.pos[i].y = thisv->actor.world.pos.y;
            thisv->centerMane.pos[i].z = thisv->actor.world.pos.z;
        }
    }

    thisv->actor.colChkInfo.health = 24;
    thisv->skinSegments = 18;
    if (thisv->introState == BFD_CS_NONE) {
        thisv->actionFunc = BossFd_Wait;
    } else {
        BossFd_SetupFly(thisv, globalCtx);
    }

    if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
        Actor_Kill(&thisv->actor);
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0,
                           WARP_DUNGEON_ADULT);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, 0.0f, 100.0f, 200.0f, 0, 0, 0, 0);
    } else {
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_FD2, thisv->actor.world.pos.x,
                           thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, thisv->introState);
    }
}

void BossFd_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd* thisv = (BossFd*)thisx;

    SkelAnime_Free(&thisv->skelAnimeHead, globalCtx);
    SkelAnime_Free(&thisv->skelAnimeRightArm, globalCtx);
    SkelAnime_Free(&thisv->skelAnimeLeftArm, globalCtx);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

s32 BossFd_IsFacingLink(BossFd* thisv) {
    return ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y)) < 0x2000;
}

void BossFd_SetupFly(BossFd* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnimeHead, &gVolvagiaHeadEmergeAnim);
    Animation_PlayOnce(&thisv->skelAnimeRightArm, &gVolvagiaRightArmEmergeAnim);
    Animation_PlayOnce(&thisv->skelAnimeLeftArm, &gVolvagiaLeftArmEmergeAnim);
    thisv->actionFunc = BossFd_Fly;
    thisv->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
}

static Vec3f sHoleLocations[] = {
    { 0.0f, 90.0f, -243.0f },    { 0.0f, 90.0f, 0.0f },    { 0.0f, 90.0f, 243.0f },
    { -243.0f, 90.0f, -243.0f }, { -243.0f, 90.0f, 0.0f }, { -243.0f, 90.0f, 243.0f },
    { 243.0f, 90.0f, -243.0f },  { 243.0f, 90.0f, 0.0f },  { 243.0f, 90.0f, 243.0f },
};

static Vec3f sCeilingTargets[] = {
    { 0.0f, 900.0f, -243.0f }, { 243.0, 900.0f, -100.0f },  { 243.0f, 900.0f, 100.0f },
    { 0.0f, 900.0f, 243.0f },  { -243.0f, 900.0f, 100.0f }, { -243.0, 900.0f, -100.0f },
};

void BossFd_Fly(BossFd* thisv, GlobalContext* globalCtx) {
    u8 sp1CF = false;
    u8 temp_rand;
    s16 i1;
    s16 i2;
    s16 i3;
    f32 dx;
    f32 dy;
    f32 dz;
    Player* player = GET_PLAYER(globalCtx);
    f32 angleToTarget;
    f32 pitchToTarget;
    Vec3f* holePosition1;
    f32 temp_y;
    f32 temp_x;
    f32 temp_z;
    f32 temp;

    SkelAnime_Update(&thisv->skelAnimeHead);
    SkelAnime_Update(&thisv->skelAnimeRightArm);
    SkelAnime_Update(&thisv->skelAnimeLeftArm);
    dx = thisv->targetPosition.x - thisv->actor.world.pos.x;
    dy = thisv->targetPosition.y - thisv->actor.world.pos.y;
    dz = thisv->targetPosition.z - thisv->actor.world.pos.z;
    dx += Math_SinS((2096.0f + thisv->fwork[BFD_FLY_WOBBLE_RATE]) * thisv->work[BFD_MOVE_TIMER]) *
          thisv->fwork[BFD_FLY_WOBBLE_AMP];
    dy += Math_SinS((1096.0f + thisv->fwork[BFD_FLY_WOBBLE_RATE]) * thisv->work[BFD_MOVE_TIMER]) *
          thisv->fwork[BFD_FLY_WOBBLE_AMP];
    dz += Math_SinS((1796.0f + thisv->fwork[BFD_FLY_WOBBLE_RATE]) * thisv->work[BFD_MOVE_TIMER]) *
          thisv->fwork[BFD_FLY_WOBBLE_AMP];
    angleToTarget = (s16)(Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>));
    pitchToTarget = (s16)(Math_FAtan2F(dy, sqrtf(SQ(dx) + SQ(dz))) * (0x8000 / std::numbers::pi_v<float>));

    osSyncPrintf("MODE %d\n", thisv->work[BFD_ACTION_STATE]);

    Math_ApproachF(&thisv->fwork[BFD_BODY_PULSE], 0.1f, 1.0f, 0.02);

    //                                        Boss Intro Cutscene

    if (thisv->introState != BFD_CS_NONE) {
        Player* player2 = GET_PLAYER(globalCtx);
        Camera* mainCam = Gameplay_GetCamera(globalCtx, MAIN_CAM);

        switch (thisv->introState) {
            case BFD_CS_WAIT:
                thisv->fogMode = 3;
                thisv->targetPosition.x = 0.0f;
                thisv->targetPosition.y = -110.0f;
                thisv->targetPosition.z = 0.0;
                thisv->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                thisv->work[BFD_ACTION_STATE] = BOSSFD_WAIT_INTRO;
                if ((fabsf(player2->actor.world.pos.z) < 80.0f) &&
                    (fabsf(player2->actor.world.pos.x - 340.0f) < 60.0f)) {

                    thisv->introState = BFD_CS_START;
                    func_80064520(globalCtx, &globalCtx->csCtx);
                    func_8002DF54(globalCtx, &thisv->actor, 8);
                    thisv->introCamera = Gameplay_CreateSubCamera(globalCtx);
                    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
                    Gameplay_ChangeCameraStatus(globalCtx, thisv->introCamera, CAM_STAT_ACTIVE);
                    player2->actor.world.pos.x = 380.0f;
                    player2->actor.world.pos.y = 100.0f;
                    player2->actor.world.pos.z = 0.0f;
                    player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                    player2->actor.speedXZ = 0.0f;
                    thisv->camData.eye.x = player2->actor.world.pos.x - 70.0f;
                    thisv->camData.eye.y = player2->actor.world.pos.y + 40.0f;
                    thisv->camData.eye.z = player2->actor.world.pos.z + 70.0f;
                    thisv->camData.at.x = player2->actor.world.pos.x;
                    thisv->camData.at.y = player2->actor.world.pos.y + 30.0f;
                    thisv->camData.at.z = player2->actor.world.pos.z;
                    thisv->camData.nextEye.x = player2->actor.world.pos.x - 50.0f + 18.0f;
                    thisv->camData.nextEye.y = player2->actor.world.pos.y + 40;
                    thisv->camData.nextEye.z = player2->actor.world.pos.z + 50.0f - 18.0f;
                    thisv->camData.nextAt.x = player2->actor.world.pos.x;
                    thisv->camData.nextAt.y = player2->actor.world.pos.y + 50.0f;
                    thisv->camData.nextAt.z = player2->actor.world.pos.z;
                    BossFd_SetCameraSpeed(thisv, 1.0f);
                    thisv->camData.atMaxVel.x = thisv->camData.atMaxVel.y = thisv->camData.atMaxVel.z = 0.05f;
                    thisv->camData.eyeMaxVel.x = thisv->camData.eyeMaxVel.y = thisv->camData.eyeMaxVel.z = 0.05f;
                    thisv->timers[0] = 0;
                    thisv->camData.speedMod = 0.0f;
                    thisv->camData.accel = 0.0f;
                    if (gSaveContext.eventChkInf[7] & 8) {
                        thisv->introState = BFD_CS_EMERGE;
                        thisv->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                        thisv->camData.nextEye.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                        thisv->camData.nextEye.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                        thisv->camData.nextAt.x = 0.0f;
                        thisv->camData.nextAt.y = 120.0f;
                        thisv->camData.nextAt.z = 0.0f;
                        BossFd_SetCameraSpeed(thisv, 0.5f);
                        thisv->camData.eyeMaxVel.x = thisv->camData.eyeMaxVel.y = thisv->camData.eyeMaxVel.z = 0.1f;
                        thisv->camData.atMaxVel.x = thisv->camData.atMaxVel.y = thisv->camData.atMaxVel.z = 0.1f;
                        thisv->camData.accel = 0.005f;
                        thisv->timers[0] = 0;
                        thisv->holeIndex = 1;
                        thisv->targetPosition.x = sHoleLocations[thisv->holeIndex].x;
                        thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y - 200.0f;
                        thisv->targetPosition.z = sHoleLocations[thisv->holeIndex].z;
                        thisv->timers[0] = 50;
                        thisv->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                        thisv->actor.world.rot.x = 0x4000;
                        thisv->work[BFD_MOVE_TIMER] = 0;
                        thisv->timers[3] = 250;
                        thisv->timers[2] = 470;
                        thisv->fwork[BFD_FLY_SPEED] = 5.0f;
                    }
                }
                break;
            case BFD_CS_START:
                if (thisv->timers[0] == 0) {
                    thisv->camData.accel = 0.0010000002f;
                    thisv->timers[0] = 100;
                    thisv->introState = BFD_CS_LOOK_LINK;
                }
            case BFD_CS_LOOK_LINK:
                player2->actor.world.pos.x = 380.0f;
                player2->actor.world.pos.y = 100.0f;
                player2->actor.world.pos.z = 0.0f;
                player2->actor.speedXZ = 0.0f;
                player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                if (thisv->timers[0] == 50) {
                    thisv->fogMode = 1;
                }
                if (thisv->timers[0] < 50) {
                    Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                    thisv->camData.yMod = Math_CosS(thisv->work[BFD_MOVE_TIMER] * 0x8000) * thisv->camData.shake;
                    Math_ApproachF(&thisv->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                }
                if (thisv->timers[0] == 40) {
                    func_8002DF54(globalCtx, &thisv->actor, 0x13);
                }
                if (thisv->timers[0] == 0) {
                    thisv->introState = BFD_CS_LOOK_GROUND;
                    thisv->camData.nextAt.y = player2->actor.world.pos.y + 10.0f;
                    thisv->camData.atMaxVel.y = 0.2f;
                    thisv->camData.speedMod = 0.0f;
                    thisv->camData.accel = 0.02f;
                    thisv->timers[0] = 70;
                    thisv->work[BFD_MOVE_TIMER] = 0;
                }
                break;
            case BFD_CS_LOOK_GROUND:
                thisv->camData.yMod = Math_CosS(thisv->work[BFD_MOVE_TIMER] * 0x8000) * thisv->camData.shake;
                Math_ApproachF(&thisv->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
                if (thisv->timers[0] == 0) {
                    thisv->introState = BFD_CS_COLLAPSE;
                    thisv->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f;
                    thisv->camData.nextEye.y = player2->actor.world.pos.y + 100.0f;
                    thisv->camData.nextEye.z = player2->actor.world.pos.z + 200.0f;
                    thisv->camData.nextAt.x = player2->actor.world.pos.x;
                    thisv->camData.nextAt.y = player2->actor.world.pos.y - 150.0f;
                    thisv->camData.nextAt.z = player2->actor.world.pos.z - 50.0f;
                    BossFd_SetCameraSpeed(thisv, 0.1f);
                    thisv->timers[0] = 170;
                    thisv->camData.speedMod = 0.0f;
                    thisv->camData.accel = 0.0f;
                    func_8002DF54(globalCtx, &thisv->actor, 0x14);
                }
                break;
            case BFD_CS_COLLAPSE:
                thisv->camData.accel = 0.005f;
                thisv->camData.yMod = Math_CosS(thisv->work[BFD_MOVE_TIMER] * 0x8000) * thisv->camData.shake;
                Math_ApproachF(&thisv->camData.shake, 2.0f, 1.0f, 0.8 * 0.01f);
                Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
                if (thisv->timers[0] == 100) {
                    thisv->platformSignal = VBSIMA_COLLAPSE;
                }
                if (thisv->timers[0] == 0) {
                    thisv->introState = BFD_CS_EMERGE;
                    thisv->camData.speedMod = 0.0f;
                    thisv->camData.nextEye.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                    thisv->camData.nextEye.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                    thisv->camData.nextEye.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                    thisv->camData.nextAt.x = 0.0f;
                    thisv->camData.nextAt.y = 120.0f;
                    thisv->camData.nextAt.z = 0.0f;
                    BossFd_SetCameraSpeed(thisv, 0.5f);
                    thisv->camData.atMaxVel.x = thisv->camData.atMaxVel.y = thisv->camData.atMaxVel.z = 0.1f;
                    thisv->camData.eyeMaxVel.x = thisv->camData.eyeMaxVel.y = thisv->camData.eyeMaxVel.z = 0.1f;
                    thisv->camData.accel = 0.005f;
                    thisv->timers[0] = 0;
                    thisv->holeIndex = 1;
                    thisv->targetPosition.x = sHoleLocations[thisv->holeIndex].x;
                    thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y - 200.0f;
                    thisv->targetPosition.z = sHoleLocations[thisv->holeIndex].z;
                    thisv->timers[0] = 50;
                    thisv->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                    thisv->actor.world.rot.x = 0x4000;
                    thisv->work[BFD_MOVE_TIMER] = 0;
                    thisv->timers[3] = 250;
                    thisv->timers[2] = 470;
                    thisv->fwork[BFD_FLY_SPEED] = 5.0f;
                }
                break;
            case BFD_CS_EMERGE:
                osSyncPrintf("WAY_SPD X = %f\n", thisv->camData.atVel.x);
                osSyncPrintf("WAY_SPD Y = %f\n", thisv->camData.atVel.y);
                osSyncPrintf("WAY_SPD Z = %f\n", thisv->camData.atVel.z);
                if ((thisv->timers[3] > 190) && !(gSaveContext.eventChkInf[7] & 8)) {
                    Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                if (thisv->timers[3] == 190) {
                    thisv->camData.atMaxVel.x = thisv->camData.atMaxVel.y = thisv->camData.atMaxVel.z = 0.05f;
                    thisv->platformSignal = VBSIMA_KILL;
                    func_8002DF54(globalCtx, &thisv->actor, 1);
                }
                if (thisv->actor.world.pos.y > 120.0f) {
                    thisv->camData.nextAt = thisv->actor.world.pos;
                    thisv->camData.atVel.x = 190.0f;
                    thisv->camData.atVel.y = 85.56f;
                    thisv->camData.atVel.z = 25.0f;
                } else {
                    // the following `temp` stuff is probably fake but is required to match
                    // it's optimized to 1.0f because sp1CF is false at thisv point, but the 0.1f ends up in rodata
                    temp = 0.1f;
                    if (!sp1CF) {
                        temp = 1.0f;
                    }
                    Math_ApproachF(&thisv->camData.shake, 2.0f, temp, 0.1 * 0.08f);
                    thisv->camData.yMod = Math_CosS(thisv->work[BFD_MOVE_TIMER] * 0x8000) * thisv->camData.shake;
                }
                if (thisv->timers[3] == 160) {
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_FIRE_BOSS);
                }
                if ((thisv->timers[3] == 130) && !(gSaveContext.eventChkInf[7] & 8)) {
                    TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gVolvagiaBossTitleCardTex), 0xA0, 0xB4, 0x80, 0x28);
                }
                if (thisv->timers[3] <= 100) {
                    thisv->camData.eyeVel.x = thisv->camData.eyeVel.y = thisv->camData.eyeVel.z = 2.0f;
                    thisv->camData.nextEye.x = player2->actor.world.pos.x + 50.0f;
                    thisv->camData.nextEye.y = player2->actor.world.pos.y + 50.0f;
                    thisv->camData.nextEye.z = player2->actor.world.pos.z + 50.0f;
                }
                if (thisv->work[BFD_ACTION_STATE] == BOSSFD_FLY_HOLE) {
                    switch (thisv->introFlyState) {
                        case INTRO_FLY_EMERGE:
                            thisv->timers[5] = 100;
                            thisv->introFlyState = INTRO_FLY_HOLE;
                        case INTRO_FLY_HOLE:
                            if (thisv->timers[5] == 0) {
                                thisv->introFlyState = INTRO_FLY_CAMERA;
                                thisv->timers[5] = 75;
                            }
                            break;
                        case INTRO_FLY_CAMERA:
                            thisv->targetPosition = thisv->camData.eye;
                            if (thisv->timers[5] == 0) {
                                thisv->timers[0] = 0;
                                thisv->holeIndex = 7;
                                thisv->targetPosition.x = sHoleLocations[thisv->holeIndex].x;
                                thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y + 200.0f + 50.0f;
                                thisv->targetPosition.z = sHoleLocations[thisv->holeIndex].z;
                                thisv->introFlyState = INTRO_FLY_RETRAT;
                            }
                            if (thisv->timers[5] == 30) {
                                thisv->work[BFD_ROAR_TIMER] = 40;
                                thisv->fireBreathTimer = 20;
                            }
                        case INTRO_FLY_RETRAT:
                            break;
                    }
                }
                osSyncPrintf("thisv->timer[2] = %d\n", thisv->timers[2]);
                osSyncPrintf("thisv->timer[5] = %d\n", thisv->timers[5]);
                if (thisv->timers[2] == 0) {
                    mainCam->eye = thisv->camData.eye;
                    mainCam->eyeNext = thisv->camData.eye;
                    mainCam->at = thisv->camData.at;
                    func_800C08AC(globalCtx, thisv->introCamera, 0);
                    thisv->introState = thisv->introFlyState = thisv->introCamera = BFD_CS_NONE;
                    func_80064534(globalCtx, &globalCtx->csCtx);
                    func_8002DF54(globalCtx, &thisv->actor, 7);
                    thisv->actionFunc = BossFd_Wait;
                    thisv->handoffSignal = FD2_SIGNAL_GROUND;
                    gSaveContext.eventChkInf[7] |= 8;
                }
                break;
        }
        BossFd_UpdateCamera(thisv, globalCtx);
    } else {
        thisv->fwork[BFD_FLY_SPEED] = 5.0f;
    }

    //                             Attacks and Death Cutscene

    switch (thisv->work[BFD_ACTION_STATE]) {
        case BOSSFD_FLY_MAIN:
            sp1CF = true;
            if (thisv->timers[0] == 0) {
                if (thisv->actor.colChkInfo.health == 0) {
                    thisv->work[BFD_ACTION_STATE] = BOSSFD_DEATH_START;
                    thisv->timers[0] = 0;
                    thisv->timers[1] = 100;
                } else {
                    if (thisv->introState != BFD_CS_NONE) {
                        thisv->holeIndex = 6;
                    } else {
                        do {
                            temp_rand = Rand_ZeroFloat(8.9f);
                        } while (temp_rand == thisv->holeIndex);
                        thisv->holeIndex = temp_rand;
                    }
                    thisv->targetPosition.x = sHoleLocations[thisv->holeIndex].x;
                    thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y + 200.0f + 50.0f;
                    thisv->targetPosition.z = sHoleLocations[thisv->holeIndex].z;
                    thisv->fwork[BFD_TURN_RATE] = 0.0f;
                    thisv->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                    if (thisv->introState != BFD_CS_NONE) {
                        thisv->timers[0] = 10050;
                    } else {
                        thisv->timers[0] = 20;
                    }
                    thisv->fwork[BFD_FLY_WOBBLE_AMP] = 100.0f;
                    thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_HOLE;

                    if (thisv->work[BFD_START_ATTACK]) {
                        thisv->work[BFD_START_ATTACK] = false;
                        thisv->work[BFD_FLY_COUNT]++;
                        if (thisv->work[BFD_FLY_COUNT] & 1) {
                            thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_CHASE;
                            thisv->timers[0] = 300;
                            thisv->fwork[BFD_TURN_RATE_MAX] = 900.0f;
                            thisv->fwork[BFD_TARGET_Y_OFFSET] = 300.0f;
                            thisv->work[BFD_UNK_234] = thisv->work[BFD_UNK_236] = 0;
                        } else {
                            thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_CEILING;
                        }
                    }
                }
            }
            break;
        case BOSSFD_FLY_HOLE:
            if ((thisv->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_BURROW;
                thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y - 70.0f;
                thisv->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                thisv->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                thisv->timers[0] = 150;
                thisv->work[BFD_ROAR_TIMER] = 40;
                thisv->holePosition.x = thisv->targetPosition.x;
                thisv->holePosition.z = thisv->targetPosition.z;
            }
            break;
        case BOSSFD_BURROW:
            sp1CF = true;
            if (thisv->timers[0] == 0) {
                thisv->actionFunc = BossFd_Wait;
                thisv->handoffSignal = FD2_SIGNAL_GROUND;
            }
            break;
        case BOSSFD_EMERGE:
            if ((thisv->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                thisv->actor.world.pos = thisv->targetPosition;
                thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                thisv->actor.world.rot.x = 0x4000;
                thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y + 200.0f;
                thisv->timers[4] = 80;
                thisv->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                thisv->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                thisv->holePosition.x = thisv->targetPosition.x;
                thisv->holePosition.z = thisv->targetPosition.z;

                func_80033E1C(globalCtx, 1, 0x50, 0x5000);
                if (thisv->introState != BFD_CS_NONE) {
                    thisv->timers[0] = 50;
                } else {
                    thisv->timers[0] = 50;
                }
            }
            break;
        case BOSSFD_FLY_CEILING:
            thisv->fwork[BFD_FLY_SPEED] = 8;
            thisv->targetPosition.x = 0.0f;
            thisv->targetPosition.y = 700.0f;
            thisv->targetPosition.z = -300.0f;
            thisv->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            thisv->fwork[BFD_TURN_RATE_MAX] = 3000.0f;
            if (thisv->actor.world.pos.y > 700.0f) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_DROP_ROCKS;
                thisv->timers[0] = 25;
                thisv->timers[2] = 150;
                thisv->work[BFD_CEILING_TARGET] = 0;
            }
            break;
        case BOSSFD_DROP_ROCKS:
            thisv->fwork[BFD_FLY_SPEED] = 8;
            thisv->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            thisv->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            thisv->targetPosition.x = sCeilingTargets[thisv->work[BFD_CEILING_TARGET]].x;
            thisv->targetPosition.y = sCeilingTargets[thisv->work[BFD_CEILING_TARGET]].y + 900.0f;
            thisv->targetPosition.z = sCeilingTargets[thisv->work[BFD_CEILING_TARGET]].z;
            if (thisv->timers[0] == 0) {
                thisv->timers[0] = 25;
                thisv->work[BFD_CEILING_TARGET]++;
                if (thisv->work[BFD_CEILING_TARGET] >= 6) {
                    thisv->work[BFD_CEILING_TARGET] = 0;
                }
            }
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 2);
            if (thisv->timers[1] == 0) {
                osSyncPrintf("BGCHECKKKKKKKKKKKKKKKKKKKKKKK\n");
                if (thisv->actor.bgCheckFlags & 0x10) {
                    thisv->fwork[BFD_CEILING_BOUNCE] = -18384.0f;
                    thisv->timers[1] = 10;
                    Audio_PlaySoundGeneral(NA_SE_EV_EXPLOSION, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    func_80033E1C(globalCtx, 3, 0xA, 0x7530);
                    thisv->work[BFD_ROCK_TIMER] = 300;
                }
            } else {
                pitchToTarget = thisv->fwork[BFD_CEILING_BOUNCE];
                Math_ApproachZeroF(&thisv->fwork[BFD_CEILING_BOUNCE], 1.0f, 1000.0f);
            }
            if (thisv->timers[2] == 0) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                thisv->timers[0] = 0;
                thisv->work[BFD_START_ATTACK] = false;
            }
            break;
        case BOSSFD_FLY_CHASE:
            thisv->actor.flags |= ACTOR_FLAG_24;
            temp_y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 2396.0f) * 30.0f + thisv->fwork[BFD_TARGET_Y_OFFSET];
            thisv->targetPosition.x = player->actor.world.pos.x;
            thisv->targetPosition.y = player->actor.world.pos.y + temp_y + 30.0f;
            thisv->targetPosition.z = player->actor.world.pos.z;
            thisv->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
            if (((thisv->timers[0] % 64) == 0) && (thisv->timers[0] < 450)) {
                thisv->work[BFD_ROAR_TIMER] = 40;
                if (BossFd_IsFacingLink(thisv)) {
                    thisv->fireBreathTimer = 20;
                }
            }
            if ((thisv->work[BFD_DAMAGE_FLASH_TIMER] != 0) || (thisv->timers[0] == 0) ||
                (player->actor.world.pos.y < 70.0f)) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                thisv->timers[0] = 0;
                thisv->work[BFD_START_ATTACK] = false;
            } else {
                Math_ApproachF(&thisv->fwork[BFD_TARGET_Y_OFFSET], 50.0, 1.0f, 2.0f);
            }
            break;
        case BOSSFD_DEATH_START:
            if (sqrtf(SQ(dx) + SQ(dz)) < 50.0f) {
                thisv->timers[0] = 0;
            }
            if (thisv->timers[0] == 0) {
                thisv->timers[0] = (s16)Rand_ZeroFloat(10.0f) + 10;
                do {
                    thisv->targetPosition.x = Rand_CenteredFloat(200.0f);
                    thisv->targetPosition.y = 390.0f;
                    thisv->targetPosition.z = Rand_CenteredFloat(200.0f);
                    temp_x = thisv->targetPosition.x - thisv->actor.world.pos.x;
                    temp_z = thisv->targetPosition.z - thisv->actor.world.pos.z;
                } while (!(sqrtf(SQ(temp_x) + SQ(temp_z)) > 100.0f));
            }
            thisv->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            thisv->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            thisv->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            Math_ApproachF(&thisv->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if (thisv->timers[1] == 0) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_SKIN_BURN;
                thisv->timers[0] = 30;
            }
            break;
        case BOSSFD_SKIN_BURN:
            thisv->targetPosition.x = 0.0f;
            thisv->targetPosition.y = 390.0f;
            thisv->targetPosition.z = 0.0f;
            thisv->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            thisv->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            thisv->fwork[BFD_TURN_RATE_MAX] = 2000.0f;
            Math_ApproachF(&thisv->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if ((thisv->timers[0] == 0) && ((thisv->work[BFD_MOVE_TIMER] % 4) == 0)) {
                if (thisv->skinSegments != 0) {
                    thisv->skinSegments--;
                    if (thisv->skinSegments == 0) {
                        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                    }
                } else {
                    thisv->work[BFD_ACTION_STATE] = BOSSFD_BONES_FALL;
                    thisv->timers[0] = 30;
                }
            }
            if ((thisv->work[BFD_MOVE_TIMER] % 32) == 0) {
                thisv->work[BFD_ROAR_TIMER] = 40;
            }

            if (thisv->skinSegments != 0) {
                Vec3f sp188;
                Vec3f sp17C = { 0.0f, 0.0f, 0.0f };
                Vec3f sp170;
                Vec3f sp164 = { 0.0f, 0.03f, 0.0f };
                Vec3f sp158;
                f32 pad154;
                s16 temp_rand2;
                s16 sp150;

                if (thisv->fogMode == 0) {
                    globalCtx->envCtx.unk_D8 = 0;
                }
                thisv->fogMode = 0xA;

                sp150 = 1;
                if (thisv->work[BFD_MOVE_TIMER] & 0x1C) {
                    Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_BURN - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                for (i1 = 0; i1 < sp150; i1++) {
                    if (sp150) { // Needed for matching
                        temp_rand2 = Rand_ZeroFloat(99.9f);

                        sp188.x = thisv->bodySegsPos[temp_rand2].x;
                        sp188.y = thisv->bodySegsPos[temp_rand2].y - 10.0f;
                        sp188.z = thisv->bodySegsPos[temp_rand2].z;

                        sp164.y = 0.03f;

                        EffectSsKFire_Spawn(globalCtx, &sp188, &sp17C, &sp164, (s16)Rand_ZeroFloat(20.0f) + 40, 0x64);

                        for (i2 = 0; i2 < 15; i2++) {
                            sp170.x = Rand_CenteredFloat(20.0f);
                            sp170.y = Rand_CenteredFloat(20.0f);
                            sp170.z = Rand_CenteredFloat(20.0f);

                            sp158.y = 0.4f;
                            sp158.x = Rand_CenteredFloat(0.5f);
                            sp158.z = Rand_CenteredFloat(0.5f);

                            BossFd_SpawnEmber(thisv->effects, &sp188, &sp170, &sp158, (s16)Rand_ZeroFloat(3.0f) + 8);
                        }
                    }
                }
            }
            break;
        case BOSSFD_BONES_FALL:
            thisv->work[BFD_STOP_FLAG] = true;
            thisv->fogMode = 3;
            if (thisv->timers[0] < 18) {
                thisv->bodyFallApart[thisv->timers[0]] = 1;
            }
            if (thisv->timers[0] == 0) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_SKULL_PAUSE;
                thisv->timers[0] = 15;
                thisv->work[BFD_CEILING_TARGET] = 0;
                player->actor.world.pos.y = 90.0f;
                player->actor.world.pos.x = 40.0f;
                player->actor.world.pos.z = 150.0f;
            }
            break;
        case BOSSFD_SKULL_PAUSE:
            if (thisv->timers[0] == 0) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_SKULL_FALL;
                thisv->timers[0] = 20;
                thisv->work[BFD_STOP_FLAG] = false;
            }
            break;
        case BOSSFD_SKULL_FALL:
            thisv->fwork[BFD_TURN_RATE] = thisv->fwork[BFD_TURN_RATE_MAX] = thisv->actor.speedXZ =
                thisv->fwork[BFD_FLY_SPEED] = 0;

            if (thisv->timers[0] == 1) {
                thisv->actor.world.pos.x = 0;
                thisv->actor.world.pos.y = 900.0f;
                thisv->actor.world.pos.z = 150.0f;
                thisv->actor.world.rot.x = thisv->actor.world.rot.y = 0;
                thisv->actor.shape.rot.z = 0x1200;
                thisv->actor.velocity.x = 0;
                thisv->actor.velocity.z = 0;
            }
            if (thisv->timers[0] == 0) {
                if (thisv->actor.world.pos.y <= 110.0f) {
                    thisv->actor.world.pos.y = 110.0f;
                    thisv->actor.velocity.y = 0;
                    if (thisv->work[BFD_CEILING_TARGET] == 0) {
                        thisv->work[BFD_CEILING_TARGET]++;
                        thisv->timers[1] = 60;
                        thisv->work[BFD_CAM_SHAKE_TIMER] = 20;
                        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_8002DF54(globalCtx, &thisv->actor, 5);
                        for (i1 = 0; i1 < 15; i1++) {
                            Vec3f sp144 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp138 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp12C;

                            sp144.x = Rand_CenteredFloat(8.0f);
                            sp144.y = Rand_ZeroFloat(1.0f);
                            sp144.z = Rand_CenteredFloat(8.0f);

                            sp138.y = 0.3f;

                            sp12C.x = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.x;
                            sp12C.y = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y;
                            sp12C.z = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.z;
                            BossFd_SpawnDust(thisv->effects, &sp12C, &sp144, &sp138, Rand_ZeroFloat(100.0f) + 300);
                        }
                    }
                } else {
                    thisv->actor.velocity.y -= 1.0f;
                }
            } else {
                thisv->actor.velocity.y = 0;
            }
            if (thisv->timers[1] == 1) {
                thisv->work[BFD_ACTION_STATE] = BOSSFD_SKULL_BURN;
                thisv->timers[0] = 70;
            }
            break;
        case BOSSFD_SKULL_BURN:
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.world.pos.y = 110.0f;
            thisv->fwork[BFD_TURN_RATE] = thisv->fwork[BFD_TURN_RATE_MAX] = thisv->actor.speedXZ =
                thisv->fwork[BFD_FLY_SPEED] = 0.0f;

            if ((50 > thisv->timers[0]) && (thisv->timers[0] > 0)) {
                Vec3f sp120;
                Vec3f sp114 = { 0.0f, 0.0f, 0.0f };
                Vec3f sp108 = { 0.0f, 0.03f, 0.0f };

                Audio_PlaySoundGeneral(NA_SE_EN_GOMA_LAST - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);

                sp120.x = Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.x;
                sp120.y = (Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y) - 10.0f;
                sp120.z = (Rand_CenteredFloat(40.0f) + thisv->actor.world.pos.z) + 5.0f;

                sp108.y = 0.03f;

                EffectSsKFire_Spawn(globalCtx, &sp120, &sp114, &sp108, (s16)Rand_ZeroFloat(15.0f) + 30, 0);
            }
            if (thisv->timers[0] < 20) {
                Math_ApproachZeroF(&thisv->actor.scale.x, 1.0f, 0.0025f);
                Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
            }
            if (thisv->timers[0] == 0) {
                thisv->actionFunc = BossFd_Wait;
                thisv->actor.world.pos.y -= 1000.0f;
            }
            if (thisv->timers[0] == 7) {
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, thisv->actor.world.pos.x,
                            thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
            }
            break;
        case BOSSFD_WAIT_INTRO:
            break;
    }

    //                                 Update body segments and mane

    if (!thisv->work[BFD_STOP_FLAG]) {
        s16 i4;
        Vec3f spE0[3];
        Vec3f spBC[3];
        f32 phi_f20;
        f32 padB4;
        f32 padB0;
        f32 padAC;

        Math_ApproachS(&thisv->actor.world.rot.y, angleToTarget, 0xA, thisv->fwork[BFD_TURN_RATE]);

        if (((thisv->work[BFD_ACTION_STATE] == BOSSFD_FLY_CHASE) ||
             (thisv->work[BFD_ACTION_STATE] == BOSSFD_FLY_UNUSED)) &&
            (thisv->actor.world.pos.y < 110.0f) && (pitchToTarget < 0)) {
            pitchToTarget = 0;
            Math_ApproachF(&thisv->actor.world.pos.y, 110.0f, 1.0f, 5.0f);
        }

        Math_ApproachS(&thisv->actor.world.rot.x, pitchToTarget, 0xA, thisv->fwork[BFD_TURN_RATE]);
        Math_ApproachF(&thisv->fwork[BFD_TURN_RATE], thisv->fwork[BFD_TURN_RATE_MAX], 1.0f, 20000.0f);
        Math_ApproachF(&thisv->actor.speedXZ, thisv->fwork[BFD_FLY_SPEED], 1.0f, 0.1f);
        if (thisv->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            func_8002D908(&thisv->actor);
        }
        func_8002D7EC(&thisv->actor);

        thisv->work[BFD_LEAD_BODY_SEG]++;
        if (thisv->work[BFD_LEAD_BODY_SEG] >= 100) {
            thisv->work[BFD_LEAD_BODY_SEG] = 0;
        }
        i4 = thisv->work[BFD_LEAD_BODY_SEG];
        thisv->bodySegsPos[i4].x = thisv->actor.world.pos.x;
        thisv->bodySegsPos[i4].y = thisv->actor.world.pos.y;
        thisv->bodySegsPos[i4].z = thisv->actor.world.pos.z;
        thisv->bodySegsRot[i4].x = (thisv->actor.world.rot.x / (f32)0x8000) * std::numbers::pi_v<float>;
        thisv->bodySegsRot[i4].y = (thisv->actor.world.rot.y / (f32)0x8000) * std::numbers::pi_v<float>;
        thisv->bodySegsRot[i4].z = (thisv->actor.world.rot.z / (f32)0x8000) * std::numbers::pi_v<float>;

        thisv->work[BFD_LEAD_MANE_SEG]++;
        if (thisv->work[BFD_LEAD_MANE_SEG] >= 30) {
            thisv->work[BFD_LEAD_MANE_SEG] = 0;
        }
        i4 = thisv->work[BFD_LEAD_MANE_SEG];
        thisv->centerMane.scale[i4] = (Math_SinS(thisv->work[BFD_MOVE_TIMER] * 5596.0f) * 0.3f) + 1.0f;
        thisv->rightMane.scale[i4] = (Math_SinS(thisv->work[BFD_MOVE_TIMER] * 5496.0f) * 0.3f) + 1.0f;
        thisv->leftMane.scale[i4] = (Math_CosS(thisv->work[BFD_MOVE_TIMER] * 5696.0f) * 0.3f) + 1.0f;
        thisv->centerMane.pos[i4] = thisv->centerMane.head;
        thisv->fireManeRot[i4].x = (thisv->actor.world.rot.x / (f32)0x8000) * std::numbers::pi_v<float>;
        thisv->fireManeRot[i4].y = (thisv->actor.world.rot.y / (f32)0x8000) * std::numbers::pi_v<float>;
        thisv->fireManeRot[i4].z = (thisv->actor.world.rot.z / (f32)0x8000) * std::numbers::pi_v<float>;
        thisv->rightMane.pos[i4] = thisv->rightMane.head;
        thisv->leftMane.pos[i4] = thisv->leftMane.head;

        if ((0x3000 > thisv->actor.world.rot.x) && (thisv->actor.world.rot.x > -0x3000)) {
            Math_ApproachF(&thisv->flattenMane, 1.0f, 1.0f, 0.05f);
        } else {
            Math_ApproachF(&thisv->flattenMane, 0.5f, 1.0f, 0.05f);
        }

        if (thisv->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            if ((thisv->actor.prevPos.y < 90.0f) && (90.0f <= thisv->actor.world.pos.y)) {
                thisv->timers[4] = 80;
                func_80033E1C(globalCtx, 1, 80, 0x5000);
                thisv->work[BFD_ROAR_TIMER] = 40;
                thisv->work[BFD_MANE_EMBERS_TIMER] = 30;
                thisv->work[BFD_SPLASH_TIMER] = 10;
            }
            if ((thisv->actor.prevPos.y > 90.0f) && (90.0f >= thisv->actor.world.pos.y)) {
                thisv->timers[4] = 80;
                func_80033E1C(globalCtx, 1, 80, 0x5000);
                thisv->work[BFD_MANE_EMBERS_TIMER] = 30;
                thisv->work[BFD_SPLASH_TIMER] = 10;
            }
        }

        if (!sp1CF) {
            spE0[0].x = spE0[0].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1500.0f) * 3000.0f;
            spE0[1].x = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 2000.0f) * 4000.0f;
            spE0[1].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 2200.0f) * 4000.0f;
            spE0[2].x = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1700.0f) * 2000.0f;
            spE0[2].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1900.0f) * 2000.0f;
            spBC[0].x = spBC[0].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1500.0f) * -3000.0f;
            spBC[1].x = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 2200.0f) * -4000.0f;
            spBC[1].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 2000.0f) * -4000.0f;
            spBC[2].x = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1900.0f) * -2000.0f;
            spBC[2].y = Math_SinS(thisv->work[BFD_MOVE_TIMER] * 1700.0f) * -2000.0f;

            for (i3 = 0; i3 < 3; i3++) {
                Math_ApproachF(&thisv->rightArmRot[i3].x, spE0[i3].x, 1.0f, 1000.0f);
                Math_ApproachF(&thisv->rightArmRot[i3].y, spE0[i3].y, 1.0f, 1000.0f);
                Math_ApproachF(&thisv->leftArmRot[i3].x, spBC[i3].x, 1.0f, 1000.0f);
                Math_ApproachF(&thisv->leftArmRot[i3].y, spBC[i3].y, 1.0f, 1000.0f);
            }
        } else {
            for (i2 = 0; i2 < 3; i2++) {
                phi_f20 = 0.0f;
                Math_ApproachZeroF(&thisv->rightArmRot[i2].y, 0.1f, 100.0f);
                Math_ApproachZeroF(&thisv->leftArmRot[i2].y, 0.1f, 100.0f);
                if (i2 == 0) {
                    phi_f20 = -3000.0f;
                }
                Math_ApproachF(&thisv->rightArmRot[i2].x, phi_f20, 0.1f, 100.0f);
                Math_ApproachF(&thisv->leftArmRot[i2].x, -phi_f20, 0.1f, 100.0f);
            }
        }
    }
}

void BossFd_Wait(BossFd* thisv, GlobalContext* globalCtx) {
    if (thisv->handoffSignal == FD2_SIGNAL_FLY) { // Set by BossFd2
        u8 temp_rand;

        thisv->handoffSignal = FD2_SIGNAL_NONE;
        BossFd_SetupFly(thisv, globalCtx);
        do {
            temp_rand = Rand_ZeroFloat(8.9f);
        } while (temp_rand == thisv->holeIndex);
        thisv->holeIndex = temp_rand;
        if (1) {} // Needed for matching
        thisv->targetPosition.x = sHoleLocations[thisv->holeIndex].x;
        thisv->targetPosition.y = sHoleLocations[thisv->holeIndex].y - 200.0f;
        thisv->targetPosition.z = sHoleLocations[thisv->holeIndex].z;
        thisv->actor.world.pos = thisv->targetPosition;

        thisv->timers[0] = 10;
        thisv->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
        thisv->work[BFD_START_ATTACK] = true;
    }
    if (thisv->handoffSignal == FD2_SIGNAL_DEATH) {
        thisv->handoffSignal = FD2_SIGNAL_NONE;
        BossFd_SetupFly(thisv, globalCtx);
        thisv->holeIndex = 1;
        thisv->targetPosition.x = sHoleLocations[1].x;
        thisv->targetPosition.y = sHoleLocations[1].y - 200.0f;
        thisv->targetPosition.z = sHoleLocations[1].z;
        thisv->actor.world.pos = thisv->targetPosition;
        thisv->timers[0] = 10;
        thisv->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
    }
}

static Vec3f sFireAudioVec = { 0.0f, 0.0f, 50.0f };

void BossFd_Effects(BossFd* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 colorYellow = { 255, 255, 0, 255 };
    static Color_RGBA8 colorRed = { 255, 10, 0, 255 };
    s16 breathOpacity = 0;
    f32 jawAngle;
    f32 jawSpeed;
    f32 emberRate;
    f32 emberSpeed;
    s16 eyeStates[] = { EYE_OPEN, EYE_HALF, EYE_CLOSED, EYE_CLOSED, EYE_HALF };
    f32 temp_x;
    f32 temp_z;
    s16 i;

    if (1) {} // Needed for match

    if (thisv->fogMode == 0) {
        globalCtx->envCtx.unk_BF = 0;
        globalCtx->envCtx.unk_D8 = 0.5f + 0.5f * Math_SinS(thisv->work[BFD_VAR_TIMER] * 0x500);
        globalCtx->envCtx.unk_DC = 2;
        globalCtx->envCtx.unk_BD = 1;
        globalCtx->envCtx.unk_BE = 0;
    } else if (thisv->fogMode == 3) {
        globalCtx->envCtx.unk_BF = 0;
        globalCtx->envCtx.unk_DC = 2;
        globalCtx->envCtx.unk_BD = 2;
        globalCtx->envCtx.unk_BE = 0;
        Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
    } else if (thisv->fogMode == 2) {
        thisv->fogMode--;
        globalCtx->envCtx.unk_BF = 0;
        Math_ApproachF(&globalCtx->envCtx.unk_D8, 0.55f + 0.05f * Math_SinS(thisv->work[BFD_VAR_TIMER] * 0x3E00), 1.0f,
                       0.15f);
        globalCtx->envCtx.unk_DC = 2;
        globalCtx->envCtx.unk_BD = 3;
        globalCtx->envCtx.unk_BE = 0;
    } else if (thisv->fogMode == 10) {
        thisv->fogMode = 1;
        globalCtx->envCtx.unk_BF = 0;
        Math_ApproachF(&globalCtx->envCtx.unk_D8, 0.21f + 0.07f * Math_SinS(thisv->work[BFD_VAR_TIMER] * 0xC00), 1.0f,
                       0.05f);
        globalCtx->envCtx.unk_DC = 2;
        globalCtx->envCtx.unk_BD = 3;
        globalCtx->envCtx.unk_BE = 0;
    } else if (thisv->fogMode == 1) {
        Math_ApproachF(&globalCtx->envCtx.unk_D8, 0.0f, 1.0f, 0.03f);
        if (globalCtx->envCtx.unk_D8 <= 0.01f) {
            thisv->fogMode = 0;
        }
    }

    if (thisv->work[BFD_MANE_EMBERS_TIMER] != 0) {
        thisv->work[BFD_MANE_EMBERS_TIMER]--;
        emberSpeed = emberRate = 20.0f;
    } else {
        emberRate = 3.0f;
        emberSpeed = 5.0f;
    }
    Math_ApproachF(&thisv->fwork[BFD_MANE_EMBER_RATE], emberRate, 1.0f, 0.1f);
    Math_ApproachF(&thisv->fwork[BFD_MANE_EMBER_SPEED], emberSpeed, 1.0f, 0.5f);

    if (((thisv->work[BFD_VAR_TIMER] % 8) == 0) && (Rand_ZeroOne() < 0.3f)) {
        thisv->work[BFD_BLINK_TIMER] = 4;
    }
    thisv->eyeState = eyeStates[thisv->work[BFD_BLINK_TIMER]];

    if (thisv->work[BFD_BLINK_TIMER] != 0) {
        thisv->work[BFD_BLINK_TIMER]--;
    }

    if (thisv->work[BFD_ROAR_TIMER] != 0) {
        if (thisv->work[BFD_ROAR_TIMER] == 37) {
            Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_ROAR, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
        jawAngle = 6000.0f;
        jawSpeed = 1300.0f;
    } else {
        jawAngle = (thisv->work[BFD_VAR_TIMER] & 0x10) ? 0.0f : 1000.0f;
        jawSpeed = 500.0f;
    }
    Math_ApproachF(&thisv->jawOpening, jawAngle, 0.3f, jawSpeed);

    if (thisv->work[BFD_ROAR_TIMER] != 0) {
        thisv->work[BFD_ROAR_TIMER]--;
    }

    if (thisv->timers[4] != 0) {
        Vec3f spawnVel1;
        Vec3f spawnAccel1;
        Vec3f spawnPos1;
        s32 pad;

        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_APPEAR - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0,
                               &D_801333E0, &D_801333E8);
        if (thisv->work[BFD_SPLASH_TIMER] != 0) {
            thisv->work[BFD_SPLASH_TIMER]--;
            if ((thisv->actor.colChkInfo.health == 0) ||
                ((thisv->introState == BFD_CS_EMERGE) && (thisv->actor.world.rot.x > 0x3000))) {
                if ((u8)thisv->fogMode == 0) {
                    globalCtx->envCtx.unk_D8 = 0.0f;
                }
                thisv->fogMode = 2;
            }
            for (i = 0; i < 5; i++) {
                spawnVel1.x = Rand_CenteredFloat(20.0f);
                spawnVel1.y = Rand_ZeroFloat(5.0f) + 4.0f;
                spawnVel1.z = Rand_CenteredFloat(20.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;

                temp_x = (spawnVel1.x * 20) / 10.0f;
                temp_z = (spawnVel1.z * 20) / 10.0f;
                spawnPos1.x = temp_x + thisv->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + thisv->holePosition.z;

                func_8002836C(globalCtx, &spawnPos1, &spawnVel1, &spawnAccel1, &colorYellow, &colorRed,
                              (s16)Rand_ZeroFloat(150.0f) + 800, 10, (s16)Rand_ZeroFloat(5.0f) + 17);
            }
        } else {
            for (i = 0; i < 2; i++) {
                spawnVel1.x = Rand_CenteredFloat(10.0f);
                spawnVel1.y = Rand_ZeroFloat(3.0f) + 3.0f;
                spawnVel1.z = Rand_CenteredFloat(10.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;
                temp_x = (spawnVel1.x * 50) / 10.0f;
                temp_z = (spawnVel1.z * 50) / 10.0f;

                spawnPos1.x = temp_x + thisv->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + thisv->holePosition.z;

                func_8002836C(globalCtx, &spawnPos1, &spawnVel1, &spawnAccel1, &colorYellow, &colorRed, 500, 10, 20);
            }
        }

        for (i = 0; i < 8; i++) {
            spawnVel1.x = Rand_CenteredFloat(20.0f);
            spawnVel1.y = Rand_ZeroFloat(10.0f);
            spawnVel1.z = Rand_CenteredFloat(20.0f);

            spawnAccel1.y = 0.4f;
            spawnAccel1.x = Rand_CenteredFloat(0.5f);
            spawnAccel1.z = Rand_CenteredFloat(0.5f);

            spawnPos1.x = Rand_CenteredFloat(60.0) + thisv->holePosition.x;
            spawnPos1.y = Rand_ZeroFloat(40.0f) + 100.0f;
            spawnPos1.z = Rand_CenteredFloat(60.0) + thisv->holePosition.z;

            BossFd_SpawnEmber(thisv->effects, &spawnPos1, &spawnVel1, &spawnAccel1, (s16)Rand_ZeroFloat(1.5f) + 6);
        }
    }

    if ((thisv->fireBreathTimer != 0) && (thisv->fireBreathTimer < 17)) {
        breathOpacity = (thisv->fireBreathTimer >= 6) ? 255 : thisv->fireBreathTimer * 50;
    }
    if (breathOpacity != 0) {
        f32 spawnAngleX;
        f32 spawnAngleY;
        Vec3f spawnSpeed2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnVel2;
        Vec3f spawnAccel2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnPos2;

        thisv->fogMode = 2;
        spawnSpeed2.z = 30.0f;

        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_FIRE - SFX_FLAG, &sFireAudioVec, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        spawnPos2 = thisv->headPos;

        spawnAngleY = (thisv->actor.world.rot.y / (f32)0x8000) * std::numbers::pi_v<float>;
        spawnAngleX = (((-thisv->actor.world.rot.x) / (f32)0x8000) * std::numbers::pi_v<float>) + 0.3f;
        Matrix_RotateY(spawnAngleY, MTXMODE_NEW);
        Matrix_RotateX(spawnAngleX, MTXMODE_APPLY);
        Matrix_MultVec3f(&spawnSpeed2, &spawnVel2);

        BossFd_SpawnFireBreath(thisv->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * Math_SinS(thisv->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               thisv->actor.world.rot.y);

        spawnPos2.x += spawnVel2.x * 0.5f;
        spawnPos2.y += spawnVel2.y * 0.5f;
        spawnPos2.z += spawnVel2.z * 0.5f;

        BossFd_SpawnFireBreath(thisv->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * Math_SinS(thisv->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               thisv->actor.world.rot.y);
        spawnSpeed2.x = 0.0f;
        spawnSpeed2.y = 17.0f;
        spawnSpeed2.z = 0.0f;

        for (i = 0; i < 6; i++) {
            spawnAngleY = Rand_ZeroFloat(2.0f * std::numbers::pi_v<float>);
            spawnAngleX = Rand_ZeroFloat(2.0f * std::numbers::pi_v<float>);
            Matrix_RotateY(spawnAngleY, MTXMODE_NEW);
            Matrix_RotateX(spawnAngleX, MTXMODE_APPLY);
            Matrix_MultVec3f(&spawnSpeed2, &spawnVel2);

            spawnAccel2.x = (spawnVel2.x * -10) / 100;
            spawnAccel2.y = (spawnVel2.y * -10) / 100;
            spawnAccel2.z = (spawnVel2.z * -10) / 100;

            BossFd_SpawnEmber(thisv->effects, &thisv->headPos, &spawnVel2, &spawnAccel2, (s16)Rand_ZeroFloat(2.0f) + 8);
        }
    }

    if ((thisv->actor.world.pos.y < 90.0f) || (700.0f < thisv->actor.world.pos.y) || (thisv->actionFunc == BossFd_Wait)) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0;
    }
}

void BossFd_CollisionCheck(BossFd* thisv, GlobalContext* globalCtx) {
    ColliderJntSphElement* headCollider = &thisv->collider.elements[0];
    ColliderInfo* hurtbox;

    if (headCollider->info.bumperFlags & BUMP_HIT) {
        headCollider->info.bumperFlags &= ~BUMP_HIT;
        hurtbox = headCollider->info.acHitInfo;
        thisv->actor.colChkInfo.health -= 2;
        if (hurtbox->toucher.dmgFlags & 0x1000) {
            thisv->actor.colChkInfo.health -= 2;
        }
        if ((s8)thisv->actor.colChkInfo.health <= 2) {
            thisv->actor.colChkInfo.health = 2;
        }
        thisv->work[BFD_DAMAGE_FLASH_TIMER] = 10;
        thisv->work[BFD_INVINC_TIMER] = 20;
        Audio_PlaySoundGeneral(NA_SE_EN_VALVAISA_DAMAGE1, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void BossFd_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd* thisv = (BossFd*)thisx;
    f32 headGlow;
    f32 rManeGlow;
    f32 lManeGlow;
    s16 i;

    osSyncPrintf("FD MOVE START \n");
    thisv->work[BFD_VAR_TIMER]++;
    thisv->work[BFD_MOVE_TIMER]++;
    thisv->actionFunc(thisv, globalCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->timers); i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }
    if (thisv->fireBreathTimer != 0) {
        thisv->fireBreathTimer--;
    }
    if (thisv->work[BFD_DAMAGE_FLASH_TIMER] != 0) {
        thisv->work[BFD_DAMAGE_FLASH_TIMER]--;
    }
    if (thisv->work[BFD_INVINC_TIMER] != 0) {
        thisv->work[BFD_INVINC_TIMER]--;
    }
    if (thisv->work[BFD_ACTION_STATE] < BOSSFD_DEATH_START) {
        if (thisv->work[BFD_INVINC_TIMER] == 0) {
            BossFd_CollisionCheck(thisv, globalCtx);
        }
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    BossFd_Effects(thisv, globalCtx);
    thisv->fwork[BFD_TEX1_SCROLL_X] += 4.0f;
    thisv->fwork[BFD_TEX1_SCROLL_Y] = 120.0f;
    thisv->fwork[BFD_TEX2_SCROLL_X] += 3.0f;
    thisv->fwork[BFD_TEX2_SCROLL_Y] -= 2.0f;

    Math_ApproachF(&thisv->fwork[BFD_BODY_TEX2_ALPHA], (thisv->work[BFD_VAR_TIMER] & 0x10) ? 30.0f : 158.0f, 1.0f, 8.0f);
    if (thisv->skinSegments == 0) {
        thisv->fwork[BFD_HEAD_TEX2_ALPHA] = thisv->fwork[BFD_BODY_TEX2_ALPHA];
    } else {
        headGlow = (thisv->work[BFD_VAR_TIMER] & 4) ? 0.0f : 255.0f;
        Math_ApproachF(&thisv->fwork[BFD_HEAD_TEX2_ALPHA], headGlow, 1.0f, 64.0f);
    }

    headGlow = (thisv->work[BFD_VAR_TIMER] & 8) ? 128.0f : 255.0f;
    rManeGlow = ((thisv->work[BFD_VAR_TIMER] + 3) & 8) ? 128.0f : 255.0f;
    lManeGlow = ((thisv->work[BFD_VAR_TIMER] + 6) & 8) ? 128.0f : 255.0f;

    Math_ApproachF(&thisv->fwork[BFD_MANE_COLOR_CENTER], headGlow, 1.0f, 16.0f);
    Math_ApproachF(&thisv->fwork[BFD_MANE_COLOR_RIGHT], rManeGlow, 1.0f, 16.0f);
    Math_ApproachF(&thisv->fwork[BFD_MANE_COLOR_LEFT], lManeGlow, 1.0f, 16.0f);

    if (thisv->work[BFD_ROCK_TIMER] != 0) {
        thisv->work[BFD_ROCK_TIMER]--;
        if ((thisv->work[BFD_ROCK_TIMER] % 16) == 0) {
            EnVbBall* bossFdRock = (EnVbBall*)Actor_SpawnAsChild(
                &globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_VB_BALL, thisv->actor.world.pos.x, 1000.0f,
                thisv->actor.world.pos.z, 0, 0, (s16)Rand_ZeroFloat(50.0f) + 130, 100);

            if (bossFdRock != NULL) {
                for (i = 0; i < 10; i++) {
                    Vec3f debrisVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos;

                    debrisPos.x = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.x;
                    debrisPos.y = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.y;
                    debrisPos.z = Rand_CenteredFloat(300.0f) + bossFdRock->actor.world.pos.z;

                    BossFd_SpawnDebris(thisv->effects, &debrisPos, &debrisVel, &debrisAccel,
                                       (s16)Rand_ZeroFloat(15.0f) + 20);
                }
            }
        }
    }

    if (1) { // Needed for matching, and also to define new variables
        Vec3f emberVel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberAccel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberPos;
        s16 temp_rand;

        for (i = 0; i < 6; i++) {
            emberAccel.y = 0.4f;
            emberAccel.x = Rand_CenteredFloat(0.5f);
            emberAccel.z = Rand_CenteredFloat(0.5f);

            temp_rand = Rand_ZeroFloat(8.9f);

            emberPos.x = sHoleLocations[temp_rand].x + Rand_CenteredFloat(60.0f);
            emberPos.y = (sHoleLocations[temp_rand].y + 10.0f) + Rand_ZeroFloat(40.0f);
            emberPos.z = sHoleLocations[temp_rand].z + Rand_CenteredFloat(60.0f);

            BossFd_SpawnEmber(thisv->effects, &emberPos, &emberVel, &emberAccel, (s16)Rand_ZeroFloat(2.0f) + 6);
        }

        if (thisv->skinSegments != 0) {
            for (i = 0; i < (s16)thisv->fwork[BFD_MANE_EMBER_RATE]; i++) {
                temp_rand = Rand_ZeroFloat(29.9f);
                emberPos.y = thisv->centerMane.pos[temp_rand].y + Rand_CenteredFloat(20.0f);

                if (emberPos.y >= 90.0f) {
                    emberPos.x = thisv->centerMane.pos[temp_rand].x + Rand_CenteredFloat(20.0f);
                    emberPos.z = thisv->centerMane.pos[temp_rand].z + Rand_CenteredFloat(20.0f);

                    emberVel.x = Rand_CenteredFloat(thisv->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.y = Rand_CenteredFloat(thisv->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.z = Rand_CenteredFloat(thisv->fwork[BFD_MANE_EMBER_SPEED]);

                    emberAccel.y = 0.4f;
                    emberAccel.x = Rand_CenteredFloat(0.5f);
                    emberAccel.z = Rand_CenteredFloat(0.5f);

                    BossFd_SpawnEmber(thisv->effects, &emberPos, &emberVel, &emberAccel, (s16)Rand_ZeroFloat(2.0f) + 8);
                }
            }
        }
    }
    osSyncPrintf("FD MOVE END 1\n");
    BossFd_UpdateEffects(thisv, globalCtx);
    osSyncPrintf("FD MOVE END 2\n");
}

void BossFd_UpdateEffects(BossFd* thisv, GlobalContext* globalCtx) {
    BossFdEffect* effect = thisv->effects;
    Player* player = GET_PLAYER(globalCtx);
    Color_RGB8 colors[4] = { { 255, 128, 0 }, { 255, 0, 0 }, { 255, 255, 0 }, { 255, 0, 0 } };
    Vec3f diff;
    s16 i1;
    s16 i2;

    for (i1 = 0; i1 < 180; i1++, effect++) {
        if (effect->type != BFD_FX_NONE) {
            effect->timer1++;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == BFD_FX_EMBER) {
                s16 cInd = effect->timer1 % 4;

                effect->color.r = colors[cInd].r;
                effect->color.g = colors[cInd].g;
                effect->color.b = colors[cInd].b;
                effect->alpha -= 20;
                if (effect->alpha <= 0) {
                    effect->alpha = 0;
                    effect->type = 0;
                }
            } else if ((effect->type == BFD_FX_DEBRIS) || (effect->type == BFD_FX_SKULL_PIECE)) {
                effect->vFdFxRotX += 0.55f;
                effect->vFdFxRotY += 0.1f;
                if (effect->pos.y <= 100.0f) {
                    effect->type = 0;
                }
            } else if (effect->type == BFD_FX_DUST) {
                if (effect->timer2 >= 8) {
                    effect->timer2 = 8;
                    effect->type = 0;
                } else if (((effect->timer1 % 2) != 0) || (Rand_ZeroOne() < 0.3f)) {
                    effect->timer2++;
                }
            } else if (effect->type == BFD_FX_FIRE_BREATH) {
                diff.x = player->actor.world.pos.x - effect->pos.x;
                diff.y = player->actor.world.pos.y + 30.0f - effect->pos.y;
                diff.z = player->actor.world.pos.z - effect->pos.z;
                if ((thisv->timers[3] == 0) && (sqrtf(SQ(diff.x) + SQ(diff.y) + SQ(diff.z)) < 20.0f)) {
                    thisv->timers[3] = 50;
                    func_8002F6D4(globalCtx, NULL, 5.0f, effect->kbAngle, 0.0f, 0x30);
                    if (player->isBurning == false) {
                        for (i2 = 0; i2 < ARRAY_COUNT(player->flameTimers); i2++) {
                            player->flameTimers[i2] = Rand_S16Offset(0, 200);
                        }
                        player->isBurning = true;
                    }
                }
                if (effect->timer2 == 0) {
                    if (effect->scale < 2.5f) {
                        effect->scale += effect->vFdFxScaleMod;
                        effect->vFdFxScaleMod += 0.08f;
                    }
                    if ((effect->pos.y <= (effect->vFdFxYStop + 130.0f)) || (effect->timer1 >= 10)) {
                        effect->accel.y = 5.0f;
                        effect->timer2++;
                        effect->velocity.y = 0.0f;
                        effect->accel.x = (effect->velocity.x * -25.0f) / 100.0f;
                        effect->accel.z = (effect->velocity.z * -25.0f) / 100.0f;
                    }
                } else {
                    if (effect->scale < 2.5f) {
                        Math_ApproachF(&effect->scale, 2.5f, 0.5f, 0.5f);
                    }
                    effect->timer2++;
                    if (effect->timer2 >= 9) {
                        effect->type = 0;
                    }
                }
            }
        }
    }
}

void BossFd_DrawEffects(BossFdEffect* effect, GlobalContext* globalCtx) {
    static const void* dustTex[] = {
        gDust1Tex, gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
    };
    u8 flag = false;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s16 i;
    BossFdEffect* firstEffect = effect;

    OPEN_DISPS(gfxCtx, "../z_boss_fd.c", 4023);

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_EMBER) {
            if (!flag) {
                func_80093D84(globalCtx->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberMaterialDL);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->color.r, effect->color.g, effect->color.b, effect->alpha);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_fd.c", 4046),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberModelDL);
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_DEBRIS) {
            if (!flag) {
                func_80093D18(globalCtx->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisMaterialDL);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_RotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_fd.c", 4068),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisModelDL);
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_DUST) {
            if (!flag) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 90, 30, 0, 255);
                gDPSetEnvColor(POLY_XLU_DISP++, 90, 30, 0, 0);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_fd.c", 4104),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTex[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_FIRE_BREATH) {
            if (!flag) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 255);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, effect->alpha);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_fd.c", 4154),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTex[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
        }
    }

    effect = firstEffect;
    flag = false;
    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_SKULL_PIECE) {
            if (!flag) {
                func_80093D84(globalCtx->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceMaterialDL);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_RotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_fd.c", 4192),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceModelDL);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_fd.c", 4198);
}

void BossFd_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd* thisv = (BossFd*)thisx;

    osSyncPrintf("FD DRAW START\n");
    if (thisv->actionFunc != BossFd_Wait) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4217);
        func_80093D18(globalCtx->state.gfxCtx);
        if (thisv->work[BFD_DAMAGE_FLASH_TIMER] & 2) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
        }

        BossFd_DrawBody(globalCtx, thisv);
        POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4243);
    }

    osSyncPrintf("FD DRAW END\n");
    BossFd_DrawEffects(thisv->effects, globalCtx);
    osSyncPrintf("FD DRAW END2\n");
}

s32 BossFd_OverrideRightArmDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    BossFd* thisv = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += 4000.0f + thisv->rightArmRot[0].x;
            break;
        case 2:
            rot->y += thisv->rightArmRot[1].x;
            rot->z += thisv->rightArmRot[1].y;
            break;
        case 3:
            rot->y += thisv->rightArmRot[2].x;
            rot->z += thisv->rightArmRot[2].y;
            break;
    }
    if (thisv->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

s32 BossFd_OverrideLeftArmDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    BossFd* thisv = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += -4000.0f + thisv->leftArmRot[0].x;
            break;
        case 2:
            rot->y += thisv->leftArmRot[1].x;
            rot->z += thisv->leftArmRot[1].y;
            break;
        case 3:
            rot->y += thisv->leftArmRot[2].x;
            rot->z += thisv->leftArmRot[2].y;
            break;
    }
    if (thisv->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

static s16 sBodyIndex[] = { 0, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5 };
static s16 sManeIndex[] = { 0, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10 }; // Unused

void BossFd_DrawMane(GlobalContext* globalCtx, BossFd* thisv, Vec3f* manePos, Vec3f* maneRot, f32* maneScale, u8 mode) {
    f32 sp140[] = { 0.0f, 10.0f, 17.0f, 20.0f, 19.5f, 18.0f, 17.0f, 15.0f, 15.0f, 15.0f };
    f32 sp118[] = { 0.0f, 10.0f, 17.0f, 20.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f };
    f32 spF0[] = { 0.4636457f, 0.3366129f, 0.14879614f, 0.04995025f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // arctan of {0.5, 0.35, 0.15, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    f32 spC8[] = { -0.4636457f, -0.3366129f, -0.14879614f, 0.024927188f, 0.07478157f,
                   0.04995025f, 0.09961288f, 0.0f,         0.0f,         0.0f };
    // arctan of {-0.5, -0.35, -0.15, 0.025, 0.075, 0.05, 0.1, 0.0, 0.0}
    s16 maneIndex;
    s16 i;
    s16 maneLength;
    Vec3f spB4;
    Vec3f spA8;
    f32 phi_f20;
    f32 phi_f22;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4419);

    maneLength = thisv->skinSegments;
    maneLength = CLAMP_MAX(maneLength, 10);

    for (i = 0; i < maneLength; i++) {
        maneIndex = (thisv->work[BFD_LEAD_MANE_SEG] - (i * 2) + 30) % 30;

        if (mode == 0) {
            spB4.x = spB4.z = 0.0f;
            spB4.y = ((sp140[i] * 0.1f) * 10.0f) * thisv->flattenMane;
            phi_f20 = 0.0f;
            phi_f22 = spC8[i] * thisv->flattenMane;
        } else if (mode == 1) {
            phi_f22 = (spC8[i] * thisv->flattenMane) * 0.7f;
            phi_f20 = spF0[i] * thisv->flattenMane;

            spB4.y = (sp140[i] * thisv->flattenMane) * 0.7f;
            spB4.x = -sp118[i] * thisv->flattenMane;
            spB4.z = 0.0f;
        } else {
            phi_f22 = (spC8[i] * thisv->flattenMane) * 0.7f;
            phi_f20 = -spF0[i] * thisv->flattenMane;

            spB4.y = (sp140[i] * thisv->flattenMane) * 0.7f;
            spB4.x = sp118[i] * thisv->flattenMane;
            spB4.z = 0.0f;
        }

        Matrix_RotateY((maneRot + maneIndex)->y, MTXMODE_NEW);
        Matrix_RotateX(-(maneRot + maneIndex)->x, MTXMODE_APPLY);

        Matrix_MultVec3f(&spB4, &spA8);

        Matrix_Translate((manePos + maneIndex)->x + spA8.x, (manePos + maneIndex)->y + spA8.y,
                         (manePos + maneIndex)->z + spA8.z, MTXMODE_NEW);
        Matrix_RotateY((maneRot + maneIndex)->y + phi_f20, MTXMODE_APPLY);
        Matrix_RotateX(-((maneRot + maneIndex)->x + phi_f22), MTXMODE_APPLY);
        Matrix_Scale(maneScale[maneIndex] * (0.01f - (i * 0.0008f)), maneScale[maneIndex] * (0.01f - (i * 0.0008f)),
                     0.01f, MTXMODE_APPLY);
        Matrix_RotateX(-std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4480),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeModelDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4483);
}

s32 BossFd_OverrideHeadDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd* thisv = (BossFd*)thisx;

    switch (limbIndex) {
        case 5:
        case 6:
            rot->z -= thisv->jawOpening * 0.1f;
            break;
        case 2:
            rot->z += thisv->jawOpening;
            break;
    }
    if ((thisv->faceExposed == true) && (limbIndex == 5)) {
        *dList = gVolvagiaBrokenFaceDL;
    }
    if (thisv->skinSegments == 0) {
        if (limbIndex == 6) {
            *dList = gVolvagiaSkullDL;
        } else if (limbIndex == 2) {
            *dList = gVolvagiaJawboneDL;
        } else {
            *dList = NULL;
        }
    }
    return false;
}

void BossFd_PostHeadDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f targetMod = { 4500.0f, 0.0f, 0.0f };
    static Vec3f headMod = { 4000.0f, 0.0f, 0.0f };
    BossFd* thisv = (BossFd*)thisx;

    if (limbIndex == 5) {
        Matrix_MultVec3f(&targetMod, &thisv->actor.focus.pos);
        Matrix_MultVec3f(&headMod, &thisv->headPos);
    }
}

static const void* sEyeTextures[] = {
    gVolvagiaEyeOpenTex,
    gVolvagiaEyeHalfTex,
    gVolvagiaEyeClosedTex,
};

static const Gfx* sBodyDLists[] = {
    gVolvagiaBodySeg1DL,  gVolvagiaBodySeg2DL,  gVolvagiaBodySeg3DL,  gVolvagiaBodySeg4DL,  gVolvagiaBodySeg5DL,
    gVolvagiaBodySeg6DL,  gVolvagiaBodySeg7DL,  gVolvagiaBodySeg8DL,  gVolvagiaBodySeg9DL,  gVolvagiaBodySeg10DL,
    gVolvagiaBodySeg11DL, gVolvagiaBodySeg12DL, gVolvagiaBodySeg13DL, gVolvagiaBodySeg14DL, gVolvagiaBodySeg15DL,
    gVolvagiaBodySeg16DL, gVolvagiaBodySeg17DL, gVolvagiaBodySeg18DL,
};

void BossFd_DrawBody(GlobalContext* globalCtx, BossFd* thisv) {
    s16 segIndex;
    s16 i;
    f32 temp_float;
    Mtx* tempMat = static_cast<Mtx*>( Graph_Alloc(globalCtx->state.gfxCtx, 18 * sizeof(Mtx)) );

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4589);
    if (thisv->skinSegments != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeState]));
    }
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (s16)thisv->fwork[BFD_TEX1_SCROLL_X],
                                (s16)thisv->fwork[BFD_TEX1_SCROLL_Y], 0x20, 0x20, 1, (s16)thisv->fwork[BFD_TEX2_SCROLL_X],
                                (s16)thisv->fwork[BFD_TEX2_SCROLL_Y], 0x20, 0x20));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)thisv->fwork[BFD_BODY_TEX2_ALPHA]);

    osSyncPrintf("LH\n");
    Matrix_Push();
    segIndex = (thisv->work[BFD_LEAD_BODY_SEG] + sBodyIndex[2]) % 100;
    Matrix_Translate(thisv->bodySegsPos[segIndex].x, thisv->bodySegsPos[segIndex].y, thisv->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-thisv->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_Translate(-13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_Scale(thisv->actor.scale.x * 0.1f, thisv->actor.scale.y * 0.1f, thisv->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnimeRightArm.skeleton, thisv->skelAnimeRightArm.jointTable,
                      BossFd_OverrideRightArmDraw, NULL, thisv);
    Matrix_Pop();
    osSyncPrintf("RH\n");
    Matrix_Push();
    segIndex = (thisv->work[BFD_LEAD_BODY_SEG] + sBodyIndex[2]) % 100;
    Matrix_Translate(thisv->bodySegsPos[segIndex].x, thisv->bodySegsPos[segIndex].y, thisv->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-thisv->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_Translate(13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_Scale(thisv->actor.scale.x * 0.1f, thisv->actor.scale.y * 0.1f, thisv->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnimeLeftArm.skeleton, thisv->skelAnimeLeftArm.jointTable,
                      BossFd_OverrideLeftArmDraw, NULL, thisv);
    Matrix_Pop();
    osSyncPrintf("BD\n");
    gSPSegment(POLY_OPA_DISP++, 0x0D, tempMat);

    Matrix_Push();
    for (i = 0; i < 18; i++, tempMat++) {
        segIndex = (thisv->work[BFD_LEAD_BODY_SEG] + sBodyIndex[i + 1]) % 100;
        Matrix_Translate(thisv->bodySegsPos[segIndex].x, thisv->bodySegsPos[segIndex].y, thisv->bodySegsPos[segIndex].z,
                         MTXMODE_NEW);
        Matrix_RotateY(thisv->bodySegsRot[segIndex].y, MTXMODE_APPLY);
        Matrix_RotateX(-thisv->bodySegsRot[segIndex].x, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, 35.0f, MTXMODE_APPLY);
        Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
        if (i < thisv->skinSegments) {
            Matrix_Scale(1.0f + (Math_SinS((thisv->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 thisv->fwork[BFD_BODY_PULSE]),
                         1.0f + (Math_SinS((thisv->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 thisv->fwork[BFD_BODY_PULSE]),
                         1.0f, MTXMODE_APPLY);
            Matrix_RotateY(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
            Matrix_ToMtx(tempMat, "../z_boss_fd.c", 4719);
            gSPMatrix(POLY_OPA_DISP++, tempMat, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, sBodyDLists[i]);
        } else {
            MtxF spFC;
            Vec3f spF0 = { 0.0f, 0.0f, 0.0f };
            Vec3f spE4;
            Vec3s spDC;
            f32 padD8;

            if (thisv->bodyFallApart[i] < 2) {
                f32 spD4 = 0.1f;

                temp_float = 0.1f;
                Matrix_Translate(0.0f, 0.0f, -1100.0f, MTXMODE_APPLY);
                Matrix_RotateY(-std::numbers::pi_v<float>, MTXMODE_APPLY);
                if (i >= 14) {
                    f32 sp84 = 1.0f - ((i - 14) * 0.2f);

                    Matrix_Scale(sp84, sp84, 1.0f, MTXMODE_APPLY);
                    spD4 = 0.1f * sp84;
                    temp_float = 0.1f * sp84;
                }
                Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4768),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaRibsDL);

                if (thisv->bodyFallApart[i] == 1) {
                    EnVbBall* bones;

                    thisv->bodyFallApart[i] = 2;
                    Matrix_MultVec3f(&spF0, &spE4);
                    Matrix_Get(&spFC);
                    Matrix_MtxFToYXZRotS(&spFC, &spDC, 0);
                    bones =
                        (EnVbBall*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_VB_BALL,
                                                      spE4.x, spE4.y, spE4.z, spDC.x, spDC.y, spDC.z, i + 200);

                    bones->actor.scale.x = thisv->actor.scale.x * temp_float;
                    bones->actor.scale.y = thisv->actor.scale.y * spD4;
                    bones->actor.scale.z = thisv->actor.scale.z * 0.1f;
                }
            }
        }
        if (i > 0) {
            Collider_UpdateSpheres(i + 1, &thisv->collider);
        }
    }
    Matrix_Pop();
    osSyncPrintf("BH\n");

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)thisv->fwork[BFD_HEAD_TEX2_ALPHA]);
    Matrix_Push();
    temp_float =
        (thisv->work[BFD_ACTION_STATE] >= BOSSFD_SKULL_FALL) ? -20.0f : -10.0f - ((thisv->actor.speedXZ - 5.0f) * 10.0f);
    segIndex = (thisv->work[BFD_LEAD_BODY_SEG] + sBodyIndex[0]) % 100;
    Matrix_Translate(thisv->bodySegsPos[segIndex].x, thisv->bodySegsPos[segIndex].y, thisv->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_RotateY(thisv->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_RotateX(-thisv->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_RotateZ((thisv->actor.shape.rot.z / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, temp_float, MTXMODE_APPLY);
    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 25.0f, MTXMODE_APPLY);
    osSyncPrintf("BHC\n");
    Collider_UpdateSpheres(0, &thisv->collider);
    Matrix_Pop();
    osSyncPrintf("BHCE\n");
    Matrix_Scale(thisv->actor.scale.x * 0.1f, thisv->actor.scale.y * 0.1f, thisv->actor.scale.z * 0.1f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnimeHead.skeleton, thisv->skelAnimeHead.jointTable, BossFd_OverrideHeadDraw,
                      BossFd_PostHeadDraw, &thisv->actor);
    osSyncPrintf("SK\n");
    {
        Vec3f spB0 = { 0.0f, 1700.0f, 7000.0f };
        Vec3f spA4 = { -1000.0f, 700.0f, 7000.0f };

        func_80093D84(globalCtx->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeMaterialDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, thisv->fwork[BFD_MANE_COLOR_CENTER], 0, 255);
        Matrix_Push();
        Matrix_MultVec3f(&spB0, &thisv->centerMane.head);
        BossFd_DrawMane(globalCtx, thisv, thisv->centerMane.pos, thisv->fireManeRot, thisv->centerMane.scale, MANE_CENTER);
        Matrix_Pop();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, thisv->fwork[BFD_MANE_COLOR_RIGHT], 0, 255);
        Matrix_Push();
        Matrix_MultVec3f(&spA4, &thisv->rightMane.head);
        BossFd_DrawMane(globalCtx, thisv, thisv->rightMane.pos, thisv->fireManeRot, thisv->rightMane.scale, MANE_RIGHT);
        Matrix_Pop();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, thisv->fwork[BFD_MANE_COLOR_LEFT], 0, 255);
        Matrix_Push();
        spA4.x *= -1.0f;
        Matrix_MultVec3f(&spA4, &thisv->leftMane.head);
        BossFd_DrawMane(globalCtx, thisv, thisv->leftMane.pos, thisv->fireManeRot, thisv->leftMane.scale, MANE_LEFT);
        Matrix_Pop();
    }

    Matrix_Pop();
    osSyncPrintf("END\n");
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd.c", 4987);
}
