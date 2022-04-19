/*
 * File: z_en_fish.c
 * Overlay: ovl_En_Fish
 * Description: Fish
 */

#include "z_en_fish.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS 0

void EnFish_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFish_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFish_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFish_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFish_Respawning_SetupSlowDown(EnFish* thisv);
void EnFish_Respawning_SlowDown(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Respawning_SetupFollowChild(EnFish* thisv);
void EnFish_Respawning_FollowChild(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Respawning_SetupFleePlayer(EnFish* thisv);
void EnFish_Respawning_FleePlayer(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Respawning_SetupApproachPlayer(EnFish* thisv);
void EnFish_Respawning_ApproachPlayer(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Dropped_SetupFall(EnFish* thisv);
void EnFish_Dropped_Fall(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Dropped_SetupFlopOnGround(EnFish* thisv);
void EnFish_Dropped_FlopOnGround(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Dropped_SetupSwimAway(EnFish* thisv);
void EnFish_Dropped_SwimAway(EnFish* thisv, GlobalContext* globalCtx);
void EnFish_Unique_SetupSwimIdle(EnFish* thisv);
void EnFish_Unique_SwimIdle(EnFish* thisv, GlobalContext* globalCtx);

// Used in the cutscene functions
static Actor* D_80A17010 = NULL;
static f32 D_80A17014 = 0.0f;
static f32 D_80A17018 = 0.0f;

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

ActorInit En_Fish_InitVars = {
    ACTOR_EN_FISH,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnFish),
    (ActorFunc)EnFish_Init,
    (ActorFunc)EnFish_Destroy,
    (ActorFunc)EnFish_Update,
    (ActorFunc)EnFish_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 40, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 700, ICHAIN_STOP),
};

f32 EnFish_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void EnFish_SetInWaterAnimation(EnFish* thisv) {
    Animation_Change(&thisv->skelAnime, &gFishInWaterAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gFishInWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void EnFish_SetOutOfWaterAnimation(EnFish* thisv) {
    Animation_Change(&thisv->skelAnime, &gFishOutOfWaterAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gFishOutOfWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void EnFish_BeginRespawn(EnFish* thisv) {
    thisv->respawnTimer = 400;
    Actor_SetScale(&thisv->actor, 0.001f);
    thisv->actor.draw = NULL;
}

void EnFish_SetCutsceneData(EnFish* thisv) {
    Actor* thisx = &thisv->actor;

    if (D_80A17010 == NULL) {
        D_80A17010 = thisx;
        Actor_SetScale(thisx, 0.01f);
        thisx->draw = EnFish_Draw;
        thisx->shape.rot.x = 0;
        thisx->shape.rot.y = -0x6410;
        thisx->shape.rot.z = 0x4000;
        thisx->shape.yOffset = 600.0f;
        D_80A17014 = 10.0f;
        D_80A17018 = 0.0f;
        thisx->flags |= ACTOR_FLAG_4;
        EnFish_SetOutOfWaterAnimation(thisv);
    }
}

void EnFish_ClearCutsceneData(EnFish* thisv) {
    D_80A17010 = NULL;
    D_80A17014 = 0.0f;
    D_80A17018 = 0.0f;
}

void EnFish_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFish* thisv = (EnFish*)thisx;
    s16 params = thisv->actor.params;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gFishSkel, &gFishInWaterAnim, thisv->jointTable, thisv->morphTable,
                       7);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
    thisv->actor.colChkInfo.mass = 50;
    thisv->slowPhase = Rand_ZeroOne() * (0xFFFF + 0.5f);
    thisv->fastPhase = Rand_ZeroOne() * (0xFFFF + 0.5f);

    if (params == FISH_DROPPED) {
        thisv->actor.flags |= ACTOR_FLAG_4;
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 8.0f);
        EnFish_Dropped_SetupFall(thisv);
    } else if (params == FISH_SWIMMING_UNIQUE) {
        EnFish_Unique_SetupSwimIdle(thisv);
    } else {
        EnFish_Respawning_SetupSlowDown(thisv);
    }
}

void EnFish_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnFish* thisv = (EnFish*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnFish_SetYOffset(EnFish* thisv) {
    thisv->actor.shape.yOffset += (Math_SinS(thisv->slowPhase) * 10.0f + Math_SinS(thisv->fastPhase) * 5.0f);
    thisv->actor.shape.yOffset = CLAMP(thisv->actor.shape.yOffset, -200.0f, 200.0f);
}

s32 EnFish_InBottleRange(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f sp1C;

    if (thisv->actor.xzDistToPlayer < 32.0f) {
        sp1C.x = (Math_SinS(thisv->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.x;
        sp1C.y = player->actor.world.pos.y;
        sp1C.z = (Math_CosS(thisv->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.z;

        //! @bug: thisv check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnFish_XZDistanceSquared(&sp1C, &thisv->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

s32 EnFish_CheckXZDistanceToPlayer(EnFish* thisv, GlobalContext* globalCtx) {
    return (thisv->actor.xzDistToPlayer < 60.0f);
}

// Respawning type functions

void EnFish_Respawning_SetupSlowDown(EnFish* thisv) {
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    thisv->timer = Rand_S16Offset(5, 35);
    thisv->unk_250 = 0;
    EnFish_SetInWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Respawning_SlowDown;
}

void EnFish_Respawning_SlowDown(EnFish* thisv, GlobalContext* globalCtx) {
    EnFish_SetYOffset(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.05f, 0.3f, 0.0f);
    thisv->skelAnime.playSpeed = CLAMP_MAX(thisv->actor.speedXZ * 1.4f + 0.8f, 2.0f);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (thisv->timer <= 0) {
        EnFish_Respawning_SetupFollowChild(thisv);
    } else if (&thisv->actor == thisv->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(thisv);
    } else if (EnFish_CheckXZDistanceToPlayer(thisv, globalCtx)) {
        EnFish_Respawning_SetupFleePlayer(thisv);
    }
}

// The three following actionfunctions also turn the yaw to home if the fish is too far from it.

void EnFish_Respawning_SetupFollowChild(EnFish* thisv) {
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    thisv->timer = Rand_S16Offset(15, 45);
    thisv->unk_250 = 0;
    EnFish_SetInWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Respawning_FollowChild;
}

void EnFish_Respawning_FollowChild(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;

    EnFish_SetYOffset(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 1.8f, 0.08f, 0.4f, 0.0f);

    if ((EnFish_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > SQ(80.0f)) || (thisv->timer < 4)) {
        Math_StepToAngleS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos),
                          3000);
    } else if ((thisv->actor.child != NULL) && (&thisv->actor != thisv->actor.child)) {
        Math_StepToAngleS(&thisv->actor.world.rot.y,
                          Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.child->world.pos), 3000);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->skelAnime.playSpeed = CLAMP_MAX(thisv->actor.speedXZ * 1.5f + 0.8f, 4.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        EnFish_Respawning_SetupSlowDown(thisv);
    } else if (&thisv->actor == thisv->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(thisv);
    } else if (EnFish_CheckXZDistanceToPlayer(thisv, globalCtx)) {
        EnFish_Respawning_SetupFleePlayer(thisv);
    }
}

void EnFish_Respawning_SetupFleePlayer(EnFish* thisv) {
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    thisv->timer = Rand_S16Offset(10, 40);
    thisv->unk_250 = 0;
    EnFish_SetInWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Respawning_FleePlayer;
}

void EnFish_Respawning_FleePlayer(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 pad2;
    s16 frames;
    s16 yaw;
    s16 playerClose;

    EnFish_SetYOffset(thisv);
    playerClose = EnFish_CheckXZDistanceToPlayer(thisv, globalCtx);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 4.2f, 0.08f, 1.4f, 0.0f);

    if (EnFish_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > SQ(160.0f)) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        Math_StepToAngleS(&thisv->actor.world.rot.y, yaw, 3000);
    } else if ((thisv->actor.child != NULL) && (&thisv->actor != thisv->actor.child)) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.child->world.pos);
        Math_StepToAngleS(&thisv->actor.world.rot.y, yaw, 2000);
    } else if (playerClose) {
        yaw = thisv->actor.yawTowardsPlayer + 0x8000;
        frames = globalCtx->state.frames;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (globalCtx) {}
        Math_StepToAngleS(&thisv->actor.world.rot.y, yaw, 2000);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->skelAnime.playSpeed = CLAMP_MAX(thisv->actor.speedXZ * 1.5f + 0.8f, 4.0f);

    SkelAnime_Update(&thisv->skelAnime);

    if ((thisv->timer <= 0) || !playerClose) {
        EnFish_Respawning_SetupSlowDown(thisv);
    } else if (&thisv->actor == thisv->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(thisv);
    }
}

void EnFish_Respawning_SetupApproachPlayer(EnFish* thisv) {
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    EnFish_SetInWaterAnimation(thisv);
    thisv->timer = Rand_S16Offset(10, 40);
    thisv->unk_250 = 0;
    thisv->actionFunc = EnFish_Respawning_ApproachPlayer;
}

void EnFish_Respawning_ApproachPlayer(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad2;
    Vec3f sp38;
    s16 yaw;
    s16 temp_a0_2;

    EnFish_SetYOffset(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 1.8f, 0.1f, 0.5f, 0.0f);

    if (EnFish_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > SQ(80.0f)) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        Math_StepToAngleS(&thisv->actor.world.rot.y, yaw, 3000);
    } else {
        if ((s16)globalCtx->state.frames & 0x40) {
            temp_a0_2 = (thisv->actor.yawTowardsPlayer + 0x9000);
        } else {
            temp_a0_2 = (thisv->actor.yawTowardsPlayer + 0x7000);
        }

        sp38.x = player->actor.world.pos.x + (Math_SinS(temp_a0_2) * 20.0f);
        sp38.y = player->actor.world.pos.y;
        sp38.z = player->actor.world.pos.z + (Math_CosS(temp_a0_2) * 20.0f);

        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &sp38);
        Math_StepToAngleS(&thisv->actor.world.rot.y, yaw, 3000);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->skelAnime.playSpeed = CLAMP_MAX((thisv->actor.speedXZ * 1.5f) + 0.8f, 4.0f);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        EnFish_Respawning_SetupSlowDown(thisv);
    }
}

// Dropped type functions

void EnFish_Dropped_SetupFall(EnFish* thisv) {
    thisv->actor.gravity = -1.0f;
    thisv->actor.minVelocityY = -10.0f;
    thisv->actor.shape.yOffset = 0.0f;
    EnFish_SetOutOfWaterAnimation(thisv);
    thisv->unk_250 = 5;
    thisv->actionFunc = EnFish_Dropped_Fall;
    thisv->timer = 300;
}

void EnFish_Dropped_Fall(EnFish* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 0.1f, 0.0f);
    Math_StepToAngleS(&thisv->actor.world.rot.x, 0x4000, 100);
    Math_StepToAngleS(&thisv->actor.world.rot.z, -0x4000, 100);
    thisv->actor.shape.rot.x = thisv->actor.world.rot.x;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->actor.shape.rot.z = thisv->actor.world.rot.z;
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.bgCheckFlags & 1) { // On floor
        thisv->timer = 400;
        EnFish_Dropped_SetupFlopOnGround(thisv);
    } else if (thisv->actor.bgCheckFlags & 0x20) { // In water
        EnFish_Dropped_SetupSwimAway(thisv);
    } else if ((thisv->timer <= 0) && (thisv->actor.params == FISH_DROPPED) &&
               (thisv->actor.floorHeight < BGCHECK_Y_MIN + 10.0f)) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "BG missing? Running Actor_delete"
        osSyncPrintf("BG 抜け？ Actor_delete します(%s %d)\n", "../z_en_sakana.c", 822);
        osSyncPrintf(VT_RST);
        Actor_Kill(&thisv->actor);
    }
}

/**
 * If the fish is on a floor, thisv function is looped back to by EnFish_Dropped_FlopOnGround to set a new flopping
 * height and whether the sound should play again.
 */
void EnFish_Dropped_SetupFlopOnGround(EnFish* thisv) {
    s32 pad;
    f32 randomFloat;
    s32 playSound;

    thisv->actor.gravity = -1.0f;
    thisv->actor.minVelocityY = -10.0f;
    randomFloat = Rand_ZeroOne();

    if (randomFloat < 0.1f) {
        thisv->actor.velocity.y = (Rand_ZeroOne() * 3.0f) + 2.5f;
        playSound = true;
    } else if (randomFloat < 0.2f) {
        thisv->actor.velocity.y = (Rand_ZeroOne() * 1.2f) + 0.2f;
        playSound = true;
    } else {
        thisv->actor.velocity.y = 0.0f;

        if (Rand_ZeroOne() < 0.2f) {
            playSound = true;
        } else {
            playSound = false;
        }
    }

    thisv->actor.shape.yOffset = 300.0f;
    EnFish_SetOutOfWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Dropped_FlopOnGround;
    thisv->unk_250 = 5;

    if (playSound && (thisv->actor.draw != NULL)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FISH_LEAP);
    }
}

void EnFish_Dropped_FlopOnGround(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 frames = globalCtx->state.frames;
    s16 targetXRot;

    Math_SmoothStepToF(&thisv->actor.speedXZ, Rand_ZeroOne() * 0.2f, 0.1f, 0.1f, 0.0f);

    targetXRot = (s16)((((frames >> 5) & 2) | ((frames >> 2) & 1)) << 0xB) * 0.3f;

    if (frames & 4) {
        targetXRot = -targetXRot;
    }

    Math_StepToAngleS(&thisv->actor.world.rot.x, targetXRot, 4000);
    Math_StepToAngleS(&thisv->actor.world.rot.z, 0x4000, 1000);
    thisv->actor.world.rot.y +=
        (s16)(((Math_SinS(thisv->slowPhase) * 2000.0f) + (Math_SinS(thisv->fastPhase) * 1000.0f)) * Rand_ZeroOne());
    thisv->actor.shape.rot = thisv->actor.world.rot;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->timer <= 60) {
        // Blink when about to disappear
        if (frames & 4) {
            thisv->actor.draw = EnFish_Draw;
        } else {
            thisv->actor.draw = NULL;
        }
    } else if (thisv->actor.bgCheckFlags & 0x20) { // In water
        EnFish_Dropped_SetupSwimAway(thisv);
    } else if (thisv->actor.bgCheckFlags & 1) { // On floor
        EnFish_Dropped_SetupFlopOnGround(thisv);
    }
}

void EnFish_Dropped_SetupSwimAway(EnFish* thisv) {
    thisv->actor.home.pos = thisv->actor.world.pos;
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->timer = 200;
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    thisv->actor.shape.yOffset = 0.0f;
    EnFish_SetInWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Dropped_SwimAway;
    thisv->unk_250 = 5;
}

void EnFish_Dropped_SwimAway(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 2.8f, 0.1f, 0.4f, 0.0f);

    // If touching wall or not in water, turn back and slow down for one frame.
    if ((thisv->actor.bgCheckFlags & 8) || !(thisv->actor.bgCheckFlags & 0x20)) {
        thisv->actor.home.rot.y = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        thisv->actor.speedXZ *= 0.5f;
    }

    Math_StepToAngleS(&thisv->actor.world.rot.x, 0, 1500);
    Math_StepToAngleS(&thisv->actor.world.rot.y, thisv->actor.home.rot.y, 3000);
    Math_StepToAngleS(&thisv->actor.world.rot.z, 0, 1000);

    thisv->actor.shape.rot = thisv->actor.world.rot;

    // Raise if on a floor.
    if (thisv->actor.bgCheckFlags & 1) {
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 4.0f, 2.0f);
    } else {
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 10.0f, 2.0f);
    }

    // Shrink when close to disappearing.
    if (thisv->timer < 100) {
        Actor_SetScale(&thisv->actor, thisv->actor.scale.x * 0.982f);
    }

    thisv->skelAnime.playSpeed = CLAMP_MAX((thisv->actor.speedXZ * 1.5f) + 1.0f, 4.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

// Unique type functions

void EnFish_Unique_SetupSwimIdle(EnFish* thisv) {
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    thisv->timer = Rand_S16Offset(5, 35);
    thisv->unk_250 = 0;
    EnFish_SetInWaterAnimation(thisv);
    thisv->actionFunc = EnFish_Unique_SwimIdle;
}

void EnFish_Unique_SwimIdle(EnFish* thisv, GlobalContext* globalCtx) {
    static f32 speedStopping[] = { 0.0f, 0.04f, 0.09f };
    static f32 speedMoving[] = { 0.5f, 0.1f, 0.15f };
    f32 playSpeed;
    u32 frames = globalCtx->gameplayFrames;
    f32* speed;
    s32 pad2;
    f32 extraPlaySpeed;
    s32 pad3;

    if (thisv->actor.xzDistToPlayer < 60.0f) {
        if (thisv->timer < 12) {
            speed = speedMoving;
        } else {
            speed = speedStopping;
        }
    } else {
        if (thisv->timer < 4) {
            speed = speedMoving;
        } else {
            speed = speedStopping;
        }
    }

    EnFish_SetYOffset(thisv);
    Math_SmoothStepToF(&thisv->actor.speedXZ, speed[0], speed[1], speed[2], 0.0f);

    extraPlaySpeed = 0.0f;

    if ((EnFish_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > SQ(15.0f))) {
        if (!Math_ScaledStepToS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos),
                                200)) {
            extraPlaySpeed = 0.5f;
        }
    } else if ((thisv->timer < 4) && !Math_ScaledStepToS(&thisv->actor.world.rot.y, frames * 0x80, 100)) {
        extraPlaySpeed = 0.5f;
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    playSpeed = (thisv->actor.speedXZ * 1.2f) + 0.2f + extraPlaySpeed;
    thisv->skelAnime.playSpeed = CLAMP(playSpeed, 1.5f, 0.5);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timer <= 0) {
        thisv->timer = Rand_S16Offset(5, 80);
    }
}

// Cutscene functions

void EnFish_Cutscene_FlopOnGround(EnFish* thisv, GlobalContext* globalCtx) {
    f32 sp24 = Math_SinS(thisv->slowPhase);
    f32 sp20 = Math_SinS(thisv->fastPhase);

    D_80A17014 += D_80A17018;

    if (D_80A17014 <= 1.0f) {
        D_80A17014 = 1.0f;

        if (Rand_ZeroOne() < 0.1f) {
            D_80A17018 = (Rand_ZeroOne() * 3.0f) + 2.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FISH_LEAP);
        } else {
            D_80A17018 = 0.0f;
        }
    } else {
        D_80A17018 -= 0.4f;
    }

    thisv->skelAnime.playSpeed = ((sp24 + sp20) * 0.5f) + 2.0f;
    SkelAnime_Update(&thisv->skelAnime);
}

void EnFish_Cutscene_WiggleFlyingThroughAir(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 sp28 = Math_SinS(thisv->slowPhase);
    f32 sp24 = Math_SinS(thisv->fastPhase);

    thisv->actor.shape.rot.x -= 500;
    thisv->actor.shape.rot.z += 100;
    Math_StepToF(&D_80A17014, 0.0f, 1.0f);
    thisv->skelAnime.playSpeed = ((sp28 + sp24) * 0.5f) + 2.0f;
    SkelAnime_Update(&thisv->skelAnime);
}

void EnFish_UpdateCutscene(EnFish* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;
    const CsCmdActorAction* csAction = globalCtx->csCtx.npcActions[1];
    Vec3f startPos;
    Vec3f endPos;
    f32 progress;
    s32 bgId;

    if (csAction == NULL) {
        // "Warning : DEMO ended without dousa (action) 3 termination being called"
        osSyncPrintf("Warning : dousa 3 消滅 が呼ばれずにデモが終了した(%s %d)(arg_data 0x%04x)\n", "../z_en_sakana.c",
                     1169, thisv->actor.params);
        EnFish_ClearCutsceneData(thisv);
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->slowPhase += 0x111;
    thisv->fastPhase += 0x500;

    switch (csAction->action) {
        case 1:
            EnFish_Cutscene_FlopOnGround(thisv, globalCtx);
            break;
        case 2:
            EnFish_Cutscene_WiggleFlyingThroughAir(thisv, globalCtx);
            break;
        case 3:
            // "DEMO fish termination"
            osSyncPrintf("デモ魚消滅\n");
            EnFish_ClearCutsceneData(thisv);
            Actor_Kill(&thisv->actor);
            return;
        default:
            // "Improper DEMO action"
            osSyncPrintf("不正なデモ動作(%s %d)(arg_data 0x%04x)\n", "../z_en_sakana.c", 1200, thisv->actor.params);
            break;
    }

    startPos.x = csAction->startPos.x;
    startPos.y = csAction->startPos.y;
    startPos.z = csAction->startPos.z;
    endPos.x = csAction->endPos.x;
    endPos.y = csAction->endPos.y;
    endPos.z = csAction->endPos.z;

    progress = Environment_LerpWeight(csAction->endFrame, csAction->startFrame, globalCtx->csCtx.frames);

    thisv->actor.world.pos.x = (endPos.x - startPos.x) * progress + startPos.x;
    thisv->actor.world.pos.y = (endPos.y - startPos.y) * progress + startPos.y + D_80A17014;
    thisv->actor.world.pos.z = (endPos.z - startPos.z) * progress + startPos.z;

    thisv->actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                          &thisv->actor, &thisv->actor.world.pos);
}

// Update functions and Draw

void EnFish_OrdinaryUpdate(EnFish* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
    }

    thisv->slowPhase += 0x111;
    thisv->fastPhase += 0x500;

    if ((thisv->actor.child != NULL) && (thisv->actor.child->update == NULL) && (&thisv->actor != thisv->actor.child)) {
        thisv->actor.child = NULL;
    }

    if ((thisv->actionFunc == NULL) || (thisv->actionFunc(thisv, globalCtx), (thisv->actor.update != NULL))) {
        Actor_MoveForward(&thisv->actor);

        if (thisv->unk_250 != 0) {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 17.5f, 4.0f, 0.0f, thisv->unk_250);
        }

        if (thisv->actor.xzDistToPlayer < 70.0f) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        Actor_SetFocus(&thisv->actor, thisv->actor.shape.yOffset * 0.01f);

        if (Actor_HasParent(&thisv->actor, globalCtx)) {
            thisv->actor.parent = NULL;

            if (thisv->actor.params == FISH_DROPPED) {
                Actor_Kill(&thisv->actor);
                return;
            }

            EnFish_BeginRespawn(thisv);
        } else if (EnFish_InBottleRange(thisv, globalCtx)) {
            // GI_MAX in thisv case allows the player to catch the actor in a bottle
            func_8002F434(&thisv->actor, globalCtx, GI_MAX, 80.0f, 20.0f);
        }
    }
}

void EnFish_RespawningUpdate(EnFish* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == FISH_SWIMMING_UNIQUE) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if ((thisv->actor.child != NULL) && (thisv->actor.child->update == NULL) && (&thisv->actor != thisv->actor.child)) {
        thisv->actor.child = NULL;
    }

    if ((thisv->actionFunc == NULL) || (thisv->actionFunc(thisv, globalCtx), (thisv->actor.update != NULL))) {
        Actor_MoveForward(&thisv->actor);

        if (thisv->respawnTimer == 20) {
            thisv->actor.draw = EnFish_Draw;
        } else if (thisv->respawnTimer == 0) {
            Actor_SetScale(&thisv->actor, 0.01f);
        } else if (thisv->respawnTimer < 20) {
            Actor_SetScale(&thisv->actor, CLAMP_MAX(thisv->actor.scale.x + 0.001f, 0.01f));
        }
    }
}

void EnFish_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFish* thisv = (EnFish*)thisx;

    if ((D_80A17010 == NULL) && (thisv->actor.params == FISH_DROPPED) && (globalCtx->csCtx.state != 0) &&
        (globalCtx->csCtx.npcActions[1] != NULL)) {
        EnFish_SetCutsceneData(thisv);
    }

    if ((D_80A17010 != NULL) && (&thisv->actor == D_80A17010)) {
        EnFish_UpdateCutscene(thisv, globalCtx);
    } else if (thisv->respawnTimer > 0) {
        thisv->respawnTimer--;
        EnFish_RespawningUpdate(thisv, globalCtx);
    } else {
        EnFish_OrdinaryUpdate(thisv, globalCtx);
    }
}

void EnFish_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnFish* thisv = (EnFish*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, NULL);
    Collider_UpdateSpheres(0, &thisv->collider);
}
