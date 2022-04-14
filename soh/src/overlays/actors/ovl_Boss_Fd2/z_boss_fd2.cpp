/*
 * File: z_boss_fd2.c
 * Overlay: ovl_Boss_Fd2
 * Description: Volvagia, hole form
 */

#include "z_boss_fd2.h"
#include "objects/object_fd2/object_fd2.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ DEATH_START,
    /* 1 */ DEATH_RETREAT,
    /* 2 */ DEATH_HANDOFF,
    /* 3 */ DEATH_FD_BODY,
    /* 4 */ DEATH_FD_SKULL,
    /* 5 */ DEATH_FINISH
} BossFd2CutsceneState;

typedef enum {
    /* 0 */ EYE_OPEN,
    /* 1 */ EYE_HALF,
    /* 2 */ EYE_CLOSED
} BossFd2EyeState;

void BossFd2_Init(Actor* thisx, GlobalContext* globalCtx);
void BossFd2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossFd2_Update(Actor* thisx, GlobalContext* globalCtx);
void BossFd2_Draw(Actor* thisx, GlobalContext* globalCtx);

void BossFd2_SetupEmerge(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Emerge(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_SetupIdle(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Idle(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Burrow(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_SetupBreatheFire(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_BreatheFire(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_SetupClawSwipe(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_ClawSwipe(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Vulnerable(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Damaged(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Death(BossFd2* thisv, GlobalContext* globalCtx);
void BossFd2_Wait(BossFd2* thisv, GlobalContext* globalCtx);

const ActorInit Boss_Fd2_InitVars = {
    ACTOR_BOSS_FD2,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FD2,
    sizeof(BossFd2),
    (ActorFunc)BossFd2_Init,
    (ActorFunc)BossFd2_Destroy,
    (ActorFunc)BossFd2_Update,
    (ActorFunc)BossFd2_Draw,
    NULL,
};

#include "z_boss_fd2_colchk.cpp"

static Vec3f sHoleLocations[] = {
    { 0.0f, 90.0f, -243.0f },    { 0.0f, 90.0f, 0.0f },    { 0.0f, 90.0f, 243.0f },
    { -243.0f, 90.0f, -243.0f }, { -243.0f, 90.0f, 0.0f }, { -243.0f, 90.0f, 243.0f },
    { 243.0f, 90.0f, -243.0f },  { 243.0f, 90.0f, 0.0f },  { 243.0f, 90.0f, 243.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x21, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void BossFd2_SpawnDebris(GlobalContext* globalCtx, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                         Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
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

void BossFd2_SpawnFireBreath(GlobalContext* globalCtx, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                             Vec3f* acceleration, f32 scale, s16 alpha, s16 kbAngle) {
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

void BossFd2_SpawnEmber(GlobalContext* globalCtx, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                        Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == 0) {
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

void BossFd2_SpawnSkullPiece(GlobalContext* globalCtx, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                             Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_SKULL_PIECE;
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

void BossFd2_SpawnDust(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
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

void BossFd2_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd2* thisv = (BossFd2*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.0069999993f);
    thisv->actor.world.pos.y = -850.0f;
    ActorShape_Init(&thisv->actor.shape, -580.0f / thisv->actor.scale.y, NULL, 0.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gHoleVolvagiaSkel, &gHoleVolvagiaIdleAnim, NULL, NULL, 0);
    if (thisv->actor.params == BFD_CS_NONE) {
        BossFd2_SetupEmerge(thisv, globalCtx);
    } else {
        thisv->actionFunc = BossFd2_Wait;
    }
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->elements);
}

void BossFd2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BossFd2* thisv = (BossFd2*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BossFd2_SetupEmerge(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    s16 temp_rand;
    s8 health;

    osSyncPrintf("UP INIT 1\n");
    Animation_PlayOnce(&thisv->skelAnime, &gHoleVolvagiaEmergeAnim);
    thisv->actionFunc = BossFd2_Emerge;
    thisv->skelAnime.playSpeed = 0.0f;
    temp_rand = Rand_ZeroFloat(8.9f);
    thisv->actor.world.pos.x = sHoleLocations[temp_rand].x;
    thisv->actor.world.pos.z = sHoleLocations[temp_rand].z;
    thisv->work[FD2_ACTION_STATE] = 0;
    osSyncPrintf("UP INIT 2\n");
    thisv->timers[0] = 10;
    if (bossFd != NULL) {
        health = bossFd->actor.colChkInfo.health;
        if (health >= 18) {
            thisv->work[FD2_FAKEOUT_COUNT] = 0;
        } else if (health >= 12) {
            thisv->work[FD2_FAKEOUT_COUNT] = 1;
        } else if (health >= 6) {
            thisv->work[FD2_FAKEOUT_COUNT] = 2;
        } else {
            thisv->work[FD2_FAKEOUT_COUNT] = 3;
        }
    }
}

void BossFd2_Emerge(BossFd2* thisv, GlobalContext* globalCtx) {
    s8 health;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    Player* player = GET_PLAYER(globalCtx);
    s16 i;
    s16 holeTime;

    osSyncPrintf("UP 1    mode %d\n", thisv->work[FD2_ACTION_STATE]);
    SkelAnime_Update(&thisv->skelAnime);
    osSyncPrintf("UP 1.5 \n");
    switch (thisv->work[FD2_ACTION_STATE]) {
        case 0:
            osSyncPrintf("UP time %d \n", thisv->timers[0]);
            osSyncPrintf("PL time %x \n", player);
            osSyncPrintf("MT time %x \n", bossFd);
            if ((thisv->timers[0] == 0) && (player->actor.world.pos.y > 70.0f)) {
                osSyncPrintf("UP 1.6 \n");
                bossFd->faceExposed = 0;
                bossFd->holePosition.x = thisv->actor.world.pos.x;
                bossFd->holePosition.z = thisv->actor.world.pos.z;
                func_80033E1C(globalCtx, 1, 0x32, 0x5000);
                thisv->work[FD2_ACTION_STATE] = 1;
                thisv->work[FD2_HOLE_COUNTER]++;
                thisv->actor.world.pos.y = -200.0f;
                health = bossFd->actor.colChkInfo.health;
                if (health == 24) {
                    holeTime = 30;
                } else if (health >= 18) {
                    holeTime = 25;
                } else if (health >= 12) {
                    holeTime = 20;
                } else if (health >= 6) {
                    holeTime = 10;
                } else {
                    holeTime = 5;
                }
                thisv->timers[0] = holeTime;
                bossFd->timers[4] = thisv->timers[0] + 10;
                osSyncPrintf("UP 1.7 \n");
            }
            break;
        case 1:
            if (thisv->timers[0] == 0) {
                if (thisv->work[FD2_FAKEOUT_COUNT] != 0) {
                    thisv->work[FD2_FAKEOUT_COUNT]--;
                    i = Rand_ZeroFloat(8.9f);
                    thisv->actor.world.pos.x = sHoleLocations[i].x;
                    thisv->actor.world.pos.z = sHoleLocations[i].z;
                    thisv->work[FD2_ACTION_STATE] = 0;
                    thisv->timers[0] = 10;
                } else {
                    thisv->skelAnime.playSpeed = 1.0f;
                    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaEmergeAnim);
                    thisv->work[FD2_ACTION_STATE] = 2;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_ROAR);
                    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
                    thisv->timers[0] = 15;
                    thisv->actor.world.pos.y = 150.0f;
                    for (i = 0; i < 10; i++) {
                        thisv->rightMane.pos[i].x += Rand_CenteredFloat(100.0f);
                        thisv->rightMane.pos[i].z += Rand_CenteredFloat(100.0f);
                        thisv->leftMane.pos[i].x += Rand_CenteredFloat(100.0f);
                        thisv->leftMane.pos[i].z += Rand_CenteredFloat(100.0f);
                    }
                    bossFd->work[BFD_SPLASH_TIMER] = 5;
                }
            }
            break;
        case 2:
            Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x7D0);
            if ((thisv->timers[0] == 1) && (thisv->actor.xzDistToPlayer < 120.0f)) {
                func_8002F6D4(globalCtx, &thisv->actor, 3.0f, thisv->actor.yawTowardsPlayer, 2.0f, 0x20);
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            }
            if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
                BossFd2_SetupIdle(thisv, globalCtx);
            }
            break;
    }
    osSyncPrintf("UP 2\n");
}

void BossFd2_SetupIdle(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    s8 health;
    s16 idleTime;

    osSyncPrintf("UP INIT 1\n");
    Animation_PlayLoop(&thisv->skelAnime, &gHoleVolvagiaTurnAnim);
    thisv->actionFunc = BossFd2_Idle;
    health = bossFd->actor.colChkInfo.health;
    if (health == 24) {
        idleTime = 50;
    } else if (health >= 18) {
        idleTime = 40;
    } else if (health >= 12) {
        idleTime = 40;
    } else if (health >= 6) {
        idleTime = 30;
    } else {
        idleTime = 20;
    }
    thisv->timers[0] = idleTime;
}

void BossFd2_Idle(BossFd2* thisv, GlobalContext* globalCtx) {
    s16 prevToLink;

    SkelAnime_Update(&thisv->skelAnime);
    prevToLink = thisv->work[FD2_TURN_TO_LINK];
    thisv->work[FD2_TURN_TO_LINK] =
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x7D0, 0);
    osSyncPrintf("SW1 = %d\n", prevToLink);
    osSyncPrintf("SW2 = %d\n", thisv->work[FD2_TURN_TO_LINK]);
    if ((fabsf(prevToLink) <= 1000.0f) && (1000.0f < fabsf(thisv->work[FD2_TURN_TO_LINK]))) {
        Animation_MorphToLoop(&thisv->skelAnime, &gHoleVolvagiaTurnAnim, -5.0f);
    }
    if ((1000.0f < fabsf(prevToLink)) && (fabsf(thisv->work[FD2_TURN_TO_LINK]) <= 1000.0f)) {
        Animation_MorphToLoop(&thisv->skelAnime, &gHoleVolvagiaIdleAnim, -5.0f);
    }
    if (thisv->timers[0] == 0) {
        if (thisv->actor.xzDistToPlayer < 200.0f) {
            BossFd2_SetupClawSwipe(thisv, globalCtx);
        } else {
            BossFd2_SetupBreatheFire(thisv, globalCtx);
        }
    }
}

void BossFd2_SetupBurrow(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    Animation_MorphToPlayOnce(&thisv->skelAnime, &gHoleVolvagiaBurrowAnim, -5.0f);
    thisv->actionFunc = BossFd2_Burrow;
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaBurrowAnim);
    bossFd->timers[4] = 30;
    thisv->work[FD2_ACTION_STATE] = 0;
}

void BossFd2_Burrow(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    if (thisv->work[FD2_ACTION_STATE] == 0) {
        SkelAnime_Update(&thisv->skelAnime);
        if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
            thisv->work[FD2_ACTION_STATE] = 1;
            thisv->timers[0] = 25;
        }
    } else {
        Math_ApproachF(&thisv->actor.world.pos.y, -100.0f, 1.0f, 10.0f);
        if (thisv->timers[0] == 0) {
            if ((thisv->work[FD2_HOLE_COUNTER] >= 3) && ((s8)bossFd->actor.colChkInfo.health < 24)) {
                thisv->work[FD2_HOLE_COUNTER] = 0;
                thisv->actionFunc = BossFd2_Wait;
                bossFd->handoffSignal = FD2_SIGNAL_FLY;
            } else {
                BossFd2_SetupEmerge(thisv, globalCtx);
            }
        }
    }
}

void BossFd2_SetupBreatheFire(BossFd2* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gHoleVolvagiaBreatheFireAnim, -5.0f);
    thisv->actionFunc = BossFd2_BreatheFire;
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaBreatheFireAnim);
    thisv->work[FD2_ACTION_STATE] = 0;
}

static Vec3f sUnkVec = { 0.0f, 0.0f, 50.0f }; // Unused? BossFd uses a similar array for its fire breath sfx.

void BossFd2_BreatheFire(BossFd2* thisv, GlobalContext* globalCtx) {
    s16 i;
    Vec3f toLink;
    s16 angleX;
    s16 angleY;
    s16 breathOpacity = 0;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    Player* player = GET_PLAYER(globalCtx);
    f32 tempX;
    f32 tempY;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
        BossFd2_SetupBurrow(thisv, globalCtx);
    }
    if ((25.0f <= thisv->skelAnime.curFrame) && (thisv->skelAnime.curFrame < 70.0f)) {
        if (thisv->skelAnime.curFrame == 25.0f) {
            globalCtx->envCtx.unk_D8 = 0.0f;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_FIRE - SFX_FLAG);
        if (thisv->skelAnime.curFrame > 50) {
            breathOpacity = (70.0f - thisv->skelAnime.curFrame) * 12.0f;
        } else {
            breathOpacity = 255;
        }
        toLink.x = player->actor.world.pos.x - thisv->headPos.x;
        toLink.y = player->actor.world.pos.y - thisv->headPos.y;
        toLink.z = player->actor.world.pos.z - thisv->headPos.z;
        angleY = Math_Atan2S(toLink.z, toLink.x);
        angleX = -Math_Atan2S(sqrtf(SQ(toLink.x) + SQ(toLink.z)), toLink.y);
        angleY -= thisv->actor.shape.rot.y;
        if (angleY > 0x1F40) {
            angleY = 0x1F40;
        }
        if (angleY < -0x1F40) {
            angleY = -0x1F40;
        }
        angleX += (-0x1B58);
        if (angleX > 0x3E8) {
            angleX = 0x3E8;
        }
        if (angleX < -0xFA0) {
            angleX = -0xFA0;
        }
        Math_ApproachS(&thisv->headRot.y, angleY, 5, 0x7D0);
        Math_ApproachS(&thisv->headRot.x, angleX, 5, 0x7D0);
    } else {
        Math_ApproachS(&thisv->headRot.y, 0, 5, 0x7D0);
        Math_ApproachS(&thisv->headRot.x, 0, 5, 0x7D0);
    }
    if (breathOpacity != 0) {
        f32 breathScale;
        Vec3f spawnSpeed = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnVel;
        Vec3f spawnAccel = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnPos;

        bossFd->fogMode = 2;
        spawnSpeed.z = 30.0f;
        spawnPos = thisv->headPos;

        tempY = ((thisv->actor.shape.rot.y + thisv->headRot.y) / (f32)0x8000) * std::numbers::pi_v<float>;
        tempX = ((thisv->headRot.x / (f32)0x8000) * std::numbers::pi_v<float>) + 1.0f / 2;
        Matrix_RotateY(tempY, MTXMODE_NEW);
        Matrix_RotateX(tempX, MTXMODE_APPLY);
        Matrix_MultVec3f(&spawnSpeed, &spawnVel);

        breathScale = 300.0f + 50.0f * Math_SinS(thisv->work[FD2_VAR_TIMER] * 0x2000);
        BossFd2_SpawnFireBreath(globalCtx, bossFd->effects, &spawnPos, &spawnVel, &spawnAccel, breathScale,
                                breathOpacity, thisv->actor.shape.rot.y + thisv->headRot.y);

        spawnPos.x += spawnVel.x * 0.5f;
        spawnPos.y += spawnVel.y * 0.5f;
        spawnPos.z += spawnVel.z * 0.5f;

        breathScale = 300.0f + 50.0f * Math_SinS(thisv->work[FD2_VAR_TIMER] * 0x2000);
        BossFd2_SpawnFireBreath(globalCtx, bossFd->effects, &spawnPos, &spawnVel, &spawnAccel, breathScale,
                                breathOpacity, thisv->actor.shape.rot.y + thisv->headRot.y);

        spawnSpeed.x = 0.0f;
        spawnSpeed.y = 17.0f;
        spawnSpeed.z = 0.0f;

        for (i = 0; i < 6; i++) {
            tempY = Rand_ZeroFloat(2.0f * std::numbers::pi_v<float>);
            tempX = Rand_ZeroFloat(2.0f * std::numbers::pi_v<float>);
            Matrix_RotateY(tempY, MTXMODE_NEW);
            Matrix_RotateX(tempX, MTXMODE_APPLY);
            Matrix_MultVec3f(&spawnSpeed, &spawnVel);

            spawnAccel.x = (spawnVel.x * -10.0f) / 100.0f;
            spawnAccel.y = (spawnVel.y * -10.0f) / 100.0f;
            spawnAccel.z = (spawnVel.z * -10.0f) / 100.0f;

            BossFd2_SpawnEmber(globalCtx, bossFd->effects, &thisv->headPos, &spawnVel, &spawnAccel,
                               (s16)Rand_ZeroFloat(2.0f) + 8);
        }
    }
}

void BossFd2_SetupClawSwipe(BossFd2* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gHoleVolvagiaClawSwipeAnim, -5.0f);
    thisv->actionFunc = BossFd2_ClawSwipe;
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaClawSwipeAnim);
}

void BossFd2_ClawSwipe(BossFd2* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 5.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_ROAR);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_SW_NAIL);
    }
    if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
        BossFd2_SetupBurrow(thisv, globalCtx);
    }
}

void BossFd2_SetupVulnerable(BossFd2* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gHoleVolvagiaKnockoutAnim);
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaKnockoutAnim);
    thisv->actionFunc = BossFd2_Vulnerable;
    thisv->work[FD2_ACTION_STATE] = 0;
}

void BossFd2_Vulnerable(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    s16 i;

    thisv->disableAT = true;
    thisv->actor.flags |= ACTOR_FLAG_10;
    SkelAnime_Update(&thisv->skelAnime);
    switch (thisv->work[FD2_ACTION_STATE]) {
        case 0:
            if (Animation_OnFrame(&thisv->skelAnime, 13.0f)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_MAHI2);
            }
            if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME] - 3.0f)) {
                for (i = 0; i < 25; i++) {
                    Vec3f spawnVel;
                    Vec3f spawnAccel = { 0.0f, 0.0f, 0.0f };
                    Vec3f spawnPos;

                    spawnVel.x = Rand_CenteredFloat(8.0f);
                    spawnVel.y = Rand_ZeroFloat(1.0f);
                    spawnVel.z = Rand_CenteredFloat(8.0f);

                    spawnAccel.y = 0.5f;

                    spawnPos.x = Rand_CenteredFloat(10.0f) + thisv->actor.focus.pos.x;
                    spawnPos.y = Rand_CenteredFloat(10.0f) + thisv->actor.focus.pos.y;
                    spawnPos.z = Rand_CenteredFloat(10.0f) + thisv->actor.focus.pos.z;

                    BossFd2_SpawnDust(bossFd->effects, &spawnPos, &spawnVel, &spawnAccel,
                                      Rand_ZeroFloat(100.0f) + 300.0f);
                }
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_LAND);
            }
            if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
                Animation_MorphToLoop(&thisv->skelAnime, &gHoleVolvagiaVulnerableAnim, -5.0f);
                thisv->work[FD2_ACTION_STATE] = 1;
                thisv->timers[0] = 60;
            }
            break;
        case 1:
            if ((thisv->work[FD2_VAR_TIMER] & 0xF) == 0xF) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_KNOCKOUT);
            }
            if (thisv->timers[0] == 0) {
                BossFd2_SetupBurrow(thisv, globalCtx);
            }
            break;
    }
}

void BossFd2_SetupDamaged(BossFd2* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gHoleVolvagiaHitAnim);
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaHitAnim);
    thisv->actionFunc = BossFd2_Damaged;
    thisv->work[FD2_ACTION_STATE] = 0;
}

void BossFd2_Damaged(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->disableAT = true;
    if (thisv->work[FD2_ACTION_STATE] == 0) {
        if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
            Animation_PlayOnce(&thisv->skelAnime, &gHoleVolvagiaDamagedAnim);
            thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaDamagedAnim);
            thisv->work[FD2_ACTION_STATE] = 1;
        }
    } else if (thisv->work[FD2_ACTION_STATE] == 1) {
        if (Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_DAMAGE2);
        }
        if (Animation_OnFrame(&thisv->skelAnime, 20.0f)) {
            bossFd->timers[4] = 30;
        }
        if (Animation_OnFrame(&thisv->skelAnime, thisv->fwork[FD2_END_FRAME])) {
            thisv->work[FD2_ACTION_STATE] = 2;
            thisv->timers[0] = 25;
        }
    } else {
        Math_ApproachF(&thisv->actor.world.pos.y, -100.0f, 1.0f, 10.0f);
        if (thisv->timers[0] == 0) {
            thisv->actionFunc = BossFd2_Wait;
            bossFd->handoffSignal = FD2_SIGNAL_FLY;
        }
    }
}

void BossFd2_SetupDeath(BossFd2* thisv, GlobalContext* globalCtx) {
    thisv->fwork[FD2_END_FRAME] = Animation_GetLastFrame(&gHoleVolvagiaDamagedAnim);
    Animation_Change(&thisv->skelAnime, &gHoleVolvagiaDamagedAnim, 1.0f, 0.0f, thisv->fwork[FD2_END_FRAME],
                     ANIMMODE_ONCE_INTERP, -3.0f);
    thisv->actionFunc = BossFd2_Death;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->deathState = DEATH_START;
}

void BossFd2_UpdateCamera(BossFd2* thisv, GlobalContext* globalCtx) {
    if (thisv->deathCamera != SUBCAM_FREE) {
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
        Gameplay_CameraSetAtEye(globalCtx, thisv->deathCamera, &thisv->camData.at, &thisv->camData.eye);
        Math_ApproachF(&thisv->camData.yMod, 0.0f, 1.0f, 0.1f);
    }
}

void BossFd2_Death(BossFd2* thisv, GlobalContext* globalCtx) {
    f32 retreatSpeed;
    Vec3f sp70;
    Vec3f sp64;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    Camera* mainCam = Gameplay_GetCamera(globalCtx, MAIN_CAM);
    f32 pad3;
    f32 pad2;
    f32 pad1;
    f32 cameraShake;
    SkelAnime* skelAnime = &thisv->skelAnime;

    SkelAnime_Update(skelAnime);
    switch (thisv->deathState) {
        case DEATH_START:
            thisv->deathState = DEATH_RETREAT;
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 1);
            thisv->deathCamera = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->deathCamera, CAM_STAT_ACTIVE);
            thisv->camData.eye = mainCam->eye;
            thisv->camData.at = mainCam->at;
            thisv->camData.eyeVel.x = 100.0f;
            thisv->camData.eyeVel.y = 100.0f;
            thisv->camData.eyeVel.z = 100.0f;
            thisv->camData.atVel.x = 100.0f;
            thisv->camData.atVel.y = 100.0f;
            thisv->camData.atVel.z = 100.0f;
            thisv->camData.accel = 0.02f;
            thisv->timers[0] = 0;
            thisv->work[FD2_HOLE_COUNTER] = 0;
            thisv->camData.eyeMaxVel.x = 0.1f;
            thisv->camData.eyeMaxVel.y = 0.1f;
            thisv->camData.eyeMaxVel.z = 0.1f;
            thisv->camData.atMaxVel.x = 0.1f;
            thisv->camData.atMaxVel.y = 0.1f;
            thisv->camData.atMaxVel.z = 0.1f;
        case DEATH_RETREAT:
            thisv->work[FD2_HOLE_COUNTER]++;
            if (thisv->work[FD2_HOLE_COUNTER] < 15) {
                retreatSpeed = 1.0f;
            } else if (thisv->work[FD2_HOLE_COUNTER] < 20) {
                retreatSpeed = 0.5f;
            } else {
                retreatSpeed = 0.25f;
            }
            if ((thisv->work[FD2_HOLE_COUNTER] == 1) || (thisv->work[FD2_HOLE_COUNTER] == 40)) {
                thisv->work[FD2_SCREAM_TIMER] = 20;
                if (thisv->work[FD2_HOLE_COUNTER] == 40) {
                    Audio_StopSfxById(NA_SE_EN_VALVAISA_DEAD);
                }

                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_DAMAGE2);
            }
            Math_ApproachF(&thisv->skelAnime.playSpeed, retreatSpeed, 1.0f, 1.0f);
            Matrix_RotateY(((thisv->actor.yawTowardsPlayer / (f32)0x8000) * std::numbers::pi_v<float>) + 0.2f, MTXMODE_NEW);
            sp70.x = 0.0f;
            sp70.y = 0.0f;
            sp70.z = 250.0f;
            Matrix_MultVec3f(&sp70, &sp64);
            thisv->camData.nextEye.x = thisv->actor.world.pos.x + sp64.x;
            thisv->camData.nextEye.y = 140.0f;
            thisv->camData.nextEye.z = thisv->actor.world.pos.z + sp64.z;
            if (thisv->actor.focus.pos.y >= 90.0f) {
                thisv->camData.nextAt.y = thisv->actor.focus.pos.y;
                thisv->camData.nextAt.x = thisv->actor.focus.pos.x;
                thisv->camData.nextAt.z = thisv->actor.focus.pos.z;
            }
            if (thisv->timers[0] == 0) {
                if (Animation_OnFrame(skelAnime, 20.0f)) {
                    bossFd->timers[4] = 60;
                }
                if (thisv->work[FD2_HOLE_COUNTER] >= 100) {
                    thisv->deathState = DEATH_HANDOFF;
                    thisv->timers[0] = 50;
                }
            } else if (Animation_OnFrame(skelAnime, 15.0f)) {
                Animation_MorphToPlayOnce(skelAnime, &gHoleVolvagiaDamagedAnim, -10.0f);
            }
            break;
        case DEATH_HANDOFF:
            if (thisv->timers[0] == 0) {
                thisv->actor.draw = NULL;
                thisv->deathState = DEATH_FD_BODY;
                bossFd->handoffSignal = FD2_SIGNAL_DEATH;
                thisv->work[FD2_ACTION_STATE] = 0;
                thisv->camData.speedMod = 0.0f;
            } else {
                Math_ApproachF(&thisv->actor.world.pos.y, -100.0f, 1.0f, 5.0f);
            }
            break;
        case DEATH_FD_BODY:
            if (bossFd->actor.world.pos.y < 80.0f) {
                if (bossFd->actor.world.rot.x > 0x3000) {
                    thisv->camData.nextAt = bossFd->actor.world.pos;
                    thisv->camData.nextAt.y = 80.0f;
                    thisv->camData.nextEye.x = bossFd->actor.world.pos.x;
                    thisv->camData.nextEye.y = 150.0f;
                    thisv->camData.nextEye.z = bossFd->actor.world.pos.z + 300.0f;
                }
            } else {
                thisv->camData.nextAt = bossFd->actor.world.pos;
                thisv->camData.nextEye.x = thisv->actor.world.pos.x;
                Math_ApproachF(&thisv->camData.nextEye.y, 200.0f, 1.0f, 2.0f);
                Math_ApproachF(&thisv->camData.nextEye.z, bossFd->actor.world.pos.z + 200.0f, 1.0f, 3.0f);
                if (thisv->work[FD2_ACTION_STATE] == 0) {
                    thisv->work[FD2_ACTION_STATE]++;
                    thisv->camData.speedMod = 0.0f;
                    thisv->camData.accel = 0.02f;
                    func_8002DF54(globalCtx, &bossFd->actor, 1);
                }
            }
            if ((bossFd->work[BFD_ACTION_STATE] == BOSSFD_BONES_FALL) && (bossFd->timers[0] == 5)) {
                thisv->deathState = DEATH_FD_SKULL;
                thisv->camData.speedMod = 0.0f;
                thisv->camData.accel = 0.02f;
                thisv->camData.nextEye.y = 150.0f;
                thisv->camData.nextEye.z = bossFd->actor.world.pos.z + 300.0f;
            }
            break;
        case DEATH_FD_SKULL:
            Math_ApproachF(&thisv->camData.nextAt.y, 100.0, 1.0f, 100.0f);
            thisv->camData.nextAt.x = 0.0f;
            thisv->camData.nextAt.z = 0.0f;
            thisv->camData.nextEye.x = 0.0f;
            thisv->camData.nextEye.y = 140.0f;
            Math_ApproachF(&thisv->camData.nextEye.z, 220.0f, 0.5f, 1.15f);
            if (bossFd->work[BFD_CAM_SHAKE_TIMER] != 0) {
                bossFd->work[BFD_CAM_SHAKE_TIMER]--;
                cameraShake = bossFd->work[BFD_CAM_SHAKE_TIMER] / 0.5f;
                if (cameraShake >= 20.0f) {
                    cameraShake = 20.0f;
                }
                thisv->camData.yMod = (bossFd->work[BFD_CAM_SHAKE_TIMER] & 1) ? cameraShake : -cameraShake;
            }
            if (bossFd->work[BFD_ACTION_STATE] == BOSSFD_SKULL_BURN) {
                thisv->deathState = DEATH_FINISH;
                mainCam->eye = thisv->camData.eye;
                mainCam->eyeNext = thisv->camData.eye;
                mainCam->at = thisv->camData.at;
                func_800C08AC(globalCtx, thisv->deathCamera, 0);
                thisv->deathCamera = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f,
                                   0, 0, 0, WARP_DUNGEON_ADULT);
                Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
            }
            break;
        case DEATH_FINISH:
            break;
    }
    BossFd2_UpdateCamera(thisv, globalCtx);
}

void BossFd2_Wait(BossFd2* thisv, GlobalContext* globalCtx) {
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    if (bossFd->handoffSignal == FD2_SIGNAL_GROUND) {
        bossFd->handoffSignal = FD2_SIGNAL_NONE;
        BossFd2_SetupEmerge(thisv, globalCtx);
        thisv->timers[0] = 20;
        thisv->work[FD2_HOLE_COUNTER] = 0;
    }
}

void BossFd2_CollisionCheck(BossFd2* thisv, GlobalContext* globalCtx) {
    s16 i;
    ColliderInfo* hurtbox;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    if (thisv->actionFunc == BossFd2_ClawSwipe) {
        Player* player = GET_PLAYER(globalCtx);

        for (i = 0; i < ARRAY_COUNT(thisv->elements); i++) {
            if (thisv->collider.elements[i].info.toucherFlags & TOUCH_HIT) {
                thisv->collider.elements[i].info.toucherFlags &= ~TOUCH_HIT;
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            }
        }
    }
    if (!bossFd->faceExposed) {
        thisv->collider.elements[0].info.elemType = ELEMTYPE_UNK2;
        thisv->collider.base.colType = COLTYPE_METAL;
    } else {
        thisv->collider.elements[0].info.elemType = ELEMTYPE_UNK3;
        thisv->collider.base.colType = COLTYPE_HIT3;
    }

    if (thisv->collider.elements[0].info.bumperFlags & BUMP_HIT) {
        thisv->collider.elements[0].info.bumperFlags &= ~BUMP_HIT;

        hurtbox = thisv->collider.elements[0].info.acHitInfo;
        if (!bossFd->faceExposed) {
            if (hurtbox->toucher.dmgFlags & 0x40000040) {
                bossFd->actor.colChkInfo.health -= 2;
                if ((s8)bossFd->actor.colChkInfo.health <= 2) {
                    bossFd->actor.colChkInfo.health = 1;
                }
                bossFd->faceExposed = true;
                BossFd2_SetupVulnerable(thisv, globalCtx);
                thisv->work[FD2_INVINC_TIMER] = 30;
                thisv->work[FD2_DAMAGE_FLASH_TIMER] = 5;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_MAHI1);
                for (i = 0; i < 30; i++) {
                    Vec3f debrisVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos;

                    debrisVel.x = Rand_CenteredFloat(10.0f);
                    debrisVel.y = Rand_ZeroFloat(5.0f) + 8.0f;
                    debrisVel.z = Rand_CenteredFloat(10.0f);

                    debrisPos.x = thisv->actor.focus.pos.x;
                    debrisPos.y = thisv->actor.focus.pos.y;
                    debrisPos.z = thisv->actor.focus.pos.z;

                    BossFd2_SpawnDebris(globalCtx, bossFd->effects, &debrisPos, &debrisVel, &debrisAccel,
                                        (s16)Rand_ZeroFloat(10.0) + 10);
                }
            }
        } else {
            u8 canKill = false;
            u8 damage;

            if ((damage = CollisionCheck_GetSwordDamage(hurtbox->toucher.dmgFlags)) == 0) {
                damage = (hurtbox->toucher.dmgFlags & 0x00001000) ? 4 : 2;
            } else {
                canKill = true;
            }
            if (hurtbox->toucher.dmgFlags & 0x80) {
                damage = 0;
            }
            if (((s8)bossFd->actor.colChkInfo.health > 2) || canKill) {
                bossFd->actor.colChkInfo.health -= damage;
                osSyncPrintf(VT_FGCOL(GREEN));
                osSyncPrintf("damage   %d\n", damage);
            }
            osSyncPrintf(VT_RST);
            osSyncPrintf("hp %d\n", bossFd->actor.colChkInfo.health);

            if ((s8)bossFd->actor.colChkInfo.health <= 0) {
                bossFd->actor.colChkInfo.health = 0;
                BossFd2_SetupDeath(thisv, globalCtx);
                thisv->work[FD2_DAMAGE_FLASH_TIMER] = 10;
                thisv->work[FD2_INVINC_TIMER] = 30000;
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_DEAD);
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            } else if (damage) {
                BossFd2_SetupDamaged(thisv, globalCtx);
                thisv->work[FD2_DAMAGE_FLASH_TIMER] = 10;
                thisv->work[FD2_INVINC_TIMER] = 100;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_VALVAISA_DAMAGE1);
            }
            if (damage) {
                for (i = 0; i < 30; i++) {
                    Vec3f pieceVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f pieceAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f piecePos;

                    pieceVel.x = Rand_CenteredFloat(6.0f);
                    pieceVel.y = Rand_ZeroFloat(4.0f) + 6.0f;
                    pieceVel.z = Rand_CenteredFloat(6.0f);

                    piecePos.x = thisv->actor.focus.pos.x;
                    piecePos.y = thisv->actor.focus.pos.y;
                    piecePos.z = thisv->actor.focus.pos.z;

                    BossFd2_SpawnSkullPiece(globalCtx, bossFd->effects, &piecePos, &pieceVel, &pieceAccel,
                                            (s16)Rand_ZeroFloat(6.0f) + 10);
                }
            }
        }
    }
}

void BossFd2_UpdateFace(BossFd2* thisv, GlobalContext* globalCtx) {
    f32 maxOpen;
    f32 openRate;
    s16 eyeStates[5] = { EYE_OPEN, EYE_HALF, EYE_CLOSED, EYE_CLOSED, EYE_HALF };

    if (((thisv->work[FD2_VAR_TIMER] % 8) == 0) && (Rand_ZeroOne() < 0.3f)) {
        thisv->work[FD2_BLINK_TIMER] = 4;
    }
    if ((thisv->actionFunc == BossFd2_Vulnerable) || (thisv->actionFunc == BossFd2_Damaged)) {
        if (thisv->work[FD2_VAR_TIMER] & 0x10) {
            thisv->eyeState = EYE_HALF;
        } else {
            thisv->eyeState = EYE_CLOSED;
        }
    } else {
        thisv->eyeState = eyeStates[thisv->work[FD2_BLINK_TIMER]];
    }

    if (thisv->work[FD2_BLINK_TIMER] != 0) {
        thisv->work[FD2_BLINK_TIMER]--;
    }

    if (thisv->work[FD2_SCREAM_TIMER] != 0) {
        maxOpen = 6000.0f;
        openRate = 1300.0f;
    } else {
        maxOpen = (thisv->work[FD2_VAR_TIMER] & 0x10) ? 1000.0f : 0.0f;
        openRate = 700.0f;
    }
    Math_ApproachF(&thisv->jawOpening, maxOpen, 0.3f, openRate);

    if (thisv->work[FD2_SCREAM_TIMER] != 0) {
        thisv->work[FD2_SCREAM_TIMER]--;
    }
}

void BossFd2_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossFd2* thisv = (BossFd2*)thisx;
    s16 i;

    osSyncPrintf("FD2 move start \n");
    thisv->disableAT = false;
    thisv->actor.flags &= ~ACTOR_FLAG_10;
    thisv->work[FD2_VAR_TIMER]++;
    thisv->work[FD2_UNK_TIMER]++;

    thisv->actionFunc(thisv, globalCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->timers); i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }
    if (thisv->work[FD2_DAMAGE_FLASH_TIMER] != 0) {
        thisv->work[FD2_DAMAGE_FLASH_TIMER]--;
    }
    if (thisv->work[FD2_INVINC_TIMER] != 0) {
        thisv->work[FD2_INVINC_TIMER]--;
    }

    if (thisv->deathState == DEATH_START) {
        if (thisv->work[FD2_INVINC_TIMER] == 0) {
            BossFd2_CollisionCheck(thisv, globalCtx);
        }
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        if (!thisv->disableAT) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    BossFd2_UpdateFace(thisv, globalCtx);
    thisv->fwork[FD2_TEX1_SCROLL_X] += 4.0f;
    thisv->fwork[FD2_TEX1_SCROLL_Y] = 120.0f;
    thisv->fwork[FD2_TEX2_SCROLL_X] += 3.0f;
    thisv->fwork[FD2_TEX2_SCROLL_Y] -= 2.0f;
    if (thisv->actor.focus.pos.y < 90.0f) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0;
    }
}

s32 BossFd2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    BossFd2* thisv = (BossFd2*)thisx;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;

    if (limbIndex == 31) {
        rot->y -= (f32)thisv->headRot.y;
        rot->z += (f32)thisv->headRot.x;
    }
    switch (limbIndex) {
        case 35:
        case 36:
            rot->z -= thisv->jawOpening * 0.1f;
            break;
        case 32:
            rot->z += thisv->jawOpening;
            break;
    }
    if ((bossFd->faceExposed == 1) && (limbIndex == 35)) {
        *dList = gHoleVolvagiaBrokenFaceDL;
    }

    if ((limbIndex == 32) || (limbIndex == 35) || (limbIndex == 36)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2165);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)bossFd->fwork[BFD_HEAD_TEX2_ALPHA]);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2172);
    } else {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2174);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)bossFd->fwork[BFD_BODY_TEX2_ALPHA]);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2181);
    }
    if ((0 < limbIndex) && (limbIndex < 16)) {
        *dList = NULL;
    }
    return false;
}

void BossFd2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f targetMod = { 4500.0f, 0.0f, 0.0f };
    static Vec3f headMod = { 4000.0f, 0.0f, 0.0f };
    static Vec3f centerManeMod = { 4000.0f, -2900.0, 2000.0f };
    static Vec3f rightManeMod = { 4000.0f, -1600.0, 0.0f };
    static Vec3f leftManeMod = { 4000.0f, -1600.0, -2000.0f };
    BossFd2* thisv = (BossFd2*)thisx;

    if (limbIndex == 35) {
        Matrix_MultVec3f(&targetMod, &thisv->actor.focus.pos);
        Matrix_MultVec3f(&headMod, &thisv->headPos);
        Matrix_MultVec3f(&centerManeMod, &thisv->centerMane.head);
        Matrix_MultVec3f(&rightManeMod, &thisv->rightMane.head);
        Matrix_MultVec3f(&leftManeMod, &thisv->leftMane.head);
    }
    Collider_UpdateSpheres(limbIndex, &thisv->collider);
}

void BossFd2_UpdateMane(BossFd2* thisv, GlobalContext* globalCtx, Vec3f* head, Vec3f* pos, Vec3f* rot, Vec3f* pull,
                        f32* scale) {
    f32 sp138[10] = { 0.0f, 100.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    f32 sp110[10] = { 0.0f, 5.0f, -10.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f };
    f32 spE8[10] = { 0.4f, 0.6f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    s16 i;
    Vec3f temp_vec;
    f32 temp_f2;
    f32 phi_f0;
    f32 temp_angleX;
    f32 temp_angleY;
    Vec3f spBC;
    Vec3f spB0;
    f32 xyScale;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2389);
    Matrix_Push();
    gDPPipeSync(POLY_OPA_DISP++);

    for (i = 0; i < 10; i++) {
        if (i == 0) {
            (pos + i)->x = head->x;
            (pos + i)->y = head->y;
            (pos + i)->z = head->z;
        } else {
            Math_ApproachF(&(pull + i)->x, 0.0f, 1.0f, 1.0f);
            Math_ApproachF(&(pull + i)->y, 0.0f, 1.0f, 1.0f);
            Math_ApproachF(&(pull + i)->z, 0.0f, 1.0f, 1.0f);
        }
    }

    for (i = 1; i < 10; i++) {
        temp_vec.x = (pos + i)->x + (pull + i)->x - (pos + i - 1)->x;

        phi_f0 = (pos + i)->y + (pull + i)->y - 2.0f + sp138[i];
        temp_f2 = (pos + i - 1)->y + sp110[i];
        if (phi_f0 > temp_f2) {
            phi_f0 = temp_f2;
        }
        if ((head->y >= -910.0f) && (phi_f0 < 110.0f)) {
            phi_f0 = 110.0f;
        }
        temp_vec.y = phi_f0 - (pos + i - 1)->y;

        temp_vec.z = (pos + i)->z + (pull + i)->z - (pos + i - 1)->z;
        temp_angleY = Math_Atan2F(temp_vec.z, temp_vec.x);
        temp_angleX = -Math_Atan2F(sqrtf(SQ(temp_vec.x) + SQ(temp_vec.z)), temp_vec.y);
        (rot + i - 1)->y = temp_angleY;
        (rot + i - 1)->x = temp_angleX;
        spBC.x = 0.0f;
        spBC.y = 0.0f;
        spBC.z = spE8[i] * 25.0f;
        Matrix_RotateY(temp_angleY, MTXMODE_NEW);
        Matrix_RotateX(temp_angleX, MTXMODE_APPLY);
        Matrix_MultVec3f(&spBC, &spB0);
        temp_vec.x = (pos + i)->x;
        temp_vec.y = (pos + i)->y;
        temp_vec.z = (pos + i)->z;
        (pos + i)->x = (pos + i - 1)->x + spB0.x;
        (pos + i)->y = (pos + i - 1)->y + spB0.y;
        (pos + i)->z = (pos + i - 1)->z + spB0.z;
        (pull + i)->x = (((pos + i)->x - temp_vec.x) * 88.0f) / 100.0f;
        (pull + i)->y = (((pos + i)->y - temp_vec.y) * 88.0f) / 100.0f;
        (pull + i)->z = (((pos + i)->z - temp_vec.z) * 88.0f) / 100.0f;
        if ((pull + i)->x > 30.0f) {
            (pull + i)->x = 30.0f;
        }
        if ((pull + i)->x < -30.0f) {
            (pull + i)->x = -30.0f;
        }
        if ((pull + i)->y > 30.0f) {
            (pull + i)->y = 30.0f;
        }
        if ((pull + i)->y < -30.0f) {
            (pull + i)->y = -30.0f;
        }
        if ((pull + i)->z > 30.0f) {
            (pull + i)->z = 30.0f;
        }
        if ((pull + i)->z < -30.0f) {
            (pull + i)->z = -30.0f;
        }
    }

    for (i = 0; i < 9; i++) {
        Matrix_Translate((pos + i)->x, (pos + i)->y, (pos + i)->z, MTXMODE_NEW);
        Matrix_RotateY((rot + i)->y, MTXMODE_APPLY);
        Matrix_RotateX((rot + i)->x, MTXMODE_APPLY);
        xyScale = (0.01f - (i * 0.0009f)) * spE8[i] * scale[i];
        Matrix_Scale(xyScale, xyScale, 0.01f * spE8[i], MTXMODE_APPLY);
        Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2498),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gHoleVolvagiaManeModelDL);
    }
    Matrix_Pop();
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2503);
}

void BossFd2_DrawMane(BossFd2* thisv, GlobalContext* globalCtx) {
    s32 pad;
    BossFd* bossFd = (BossFd*)thisv->actor.parent;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2515);
    if (1) {}
    for (i = 0; i < 10; i++) {
        thisv->centerMane.scale[i] = 1.5f + 0.3f * Math_SinS(5596.0f * thisv->work[FD2_VAR_TIMER] + i * 0x3200);
        thisv->rightMane.scale[i] = 1.5f + 0.3f * Math_SinS(5496.0f * thisv->work[FD2_VAR_TIMER] + i * 0x3200);
        thisv->leftMane.scale[i] = 1.5f + 0.3f * Math_CosS(5696.0f * thisv->work[FD2_VAR_TIMER] + i * 0x3200);
    }

    func_80093D84(globalCtx->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gHoleVolvagiaManeMaterialDL);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_CENTER], 0, 255);
    BossFd2_UpdateMane(thisv, globalCtx, &thisv->centerMane.head, thisv->centerMane.pos, thisv->centerMane.rot,
                       thisv->centerMane.pull, thisv->centerMane.scale);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_RIGHT], 0, 255);
    BossFd2_UpdateMane(thisv, globalCtx, &thisv->rightMane.head, thisv->rightMane.pos, thisv->rightMane.rot,
                       thisv->rightMane.pull, thisv->rightMane.scale);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_LEFT], 0, 255);
    BossFd2_UpdateMane(thisv, globalCtx, &thisv->leftMane.head, thisv->leftMane.pos, thisv->leftMane.rot,
                       thisv->leftMane.pull, thisv->leftMane.scale);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2601);
}

void BossFd2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = { gHoleVolvagiaEyeOpenTex, gHoleVolvagiaEyeHalfTex, gHoleVolvagiaEyeClosedTex };
    s32 pad;
    BossFd2* thisv = (BossFd2*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2617);
    osSyncPrintf("FD2 draw start \n");
    if (thisv->actionFunc != BossFd2_Wait) {
        func_80093D18(globalCtx->state.gfxCtx);
        if (thisv->work[FD2_DAMAGE_FLASH_TIMER] & 2) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
        }
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeState]));

        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (s16)thisv->fwork[FD2_TEX1_SCROLL_X],
                                    (s16)thisv->fwork[FD2_TEX1_SCROLL_Y], 0x20, 0x20, 1,
                                    (s16)thisv->fwork[FD2_TEX2_SCROLL_X], (s16)thisv->fwork[FD2_TEX2_SCROLL_Y], 0x20,
                                    0x20));
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 128);

        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, BossFd2_OverrideLimbDraw, BossFd2_PostLimbDraw, &thisv->actor);
        BossFd2_DrawMane(thisv, globalCtx);
        POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_fd2.c", 2688);
}
