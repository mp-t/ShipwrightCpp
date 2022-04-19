/*
 * File: z_en_dog.c
 * Overlay: ovl_En_Dog
 * Description: Dog
 */

#include "z_en_dog.h"
#include "objects/object_dog/object_dog.h"

#define FLAGS 0

void EnDog_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDog_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDog_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDog_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDog_FollowPath(EnDog* thisv, GlobalContext* globalCtx);
void EnDog_ChooseMovement(EnDog* thisv, GlobalContext* globalCtx);
void EnDog_FollowPlayer(EnDog* thisv, GlobalContext* globalCtx);
void EnDog_RunAway(EnDog* thisv, GlobalContext* globalCtx);
void EnDog_FaceLink(EnDog* thisv, GlobalContext* globalCtx);
void EnDog_Wait(EnDog* thisv, GlobalContext* globalCtx);

ActorInit En_Dog_InitVars = {
    ACTOR_EN_DOG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DOG,
    sizeof(EnDog),
    (ActorFunc)EnDog_Init,
    (ActorFunc)EnDog_Destroy,
    (ActorFunc)EnDog_Update,
    (ActorFunc)EnDog_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 16, 20, 0, { 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, 50 };

typedef enum {
    /* 0 */ ENDOG_ANIM_0,
    /* 1 */ ENDOG_ANIM_1,
    /* 2 */ ENDOG_ANIM_2,
    /* 3 */ ENDOG_ANIM_3,
    /* 4 */ ENDOG_ANIM_4,
    /* 5 */ ENDOG_ANIM_5,
    /* 6 */ ENDOG_ANIM_6,
    /* 7 */ ENDOG_ANIM_7
} EnDogAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogRunAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogBarkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogSitAnim, 1.0f, 0.0f, 4.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogSitAnim, 1.0f, 5.0f, 25.0f, ANIMMODE_LOOP_PARTIAL, -6.0f },
    { &gDogBowAnim, 1.0f, 0.0f, 6.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogBow2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
};

typedef enum {
    /* 0x00 */ DOG_WALK,
    /* 0x01 */ DOG_RUN,
    /* 0x02 */ DOG_BARK,
    /* 0x03 */ DOG_SIT,
    /* 0x04 */ DOG_SIT_2,
    /* 0x05 */ DOG_BOW,
    /* 0x06 */ DOG_BOW_2
} DogBehavior;

void EnDog_PlayWalkSFX(EnDog* thisv) {
    const AnimationHeader* walk = &gDogWalkAnim;

    if (thisv->skelAnime.animation == walk) {
        if ((thisv->skelAnime.curFrame == 1.0f) || (thisv->skelAnime.curFrame == 7.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void EnDog_PlayRunSFX(EnDog* thisv) {
    const AnimationHeader* run = &gDogRunAnim;

    if (thisv->skelAnime.animation == run) {
        if ((thisv->skelAnime.curFrame == 2.0f) || (thisv->skelAnime.curFrame == 4.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void EnDog_PlayBarkSFX(EnDog* thisv) {
    const AnimationHeader* bark = &gDogBarkAnim;

    if (thisv->skelAnime.animation == bark) {
        if ((thisv->skelAnime.curFrame == 13.0f) || (thisv->skelAnime.curFrame == 19.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_SMALL_DOG_BARK);
        }
    }
}

s32 EnDog_PlayAnimAndSFX(EnDog* thisv) {
    s32 animation;

    if (thisv->behavior != thisv->nextBehavior) {
        if (thisv->nextBehavior == DOG_SIT_2) {
            thisv->nextBehavior = DOG_SIT;
        }
        if (thisv->nextBehavior == DOG_BOW_2) {
            thisv->nextBehavior = DOG_BOW;
        }

        thisv->behavior = thisv->nextBehavior;
        switch (thisv->behavior) {
            case DOG_WALK:
                animation = ENDOG_ANIM_1;
                break;
            case DOG_RUN:
                animation = ENDOG_ANIM_2;
                break;
            case DOG_BARK:
                animation = ENDOG_ANIM_3;
                break;
            case DOG_SIT:
                animation = ENDOG_ANIM_4;
                break;
            case DOG_BOW:
                animation = ENDOG_ANIM_6;
                break;
        }
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animation);
    }

    switch (thisv->behavior) {
        case DOG_SIT:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDOG_ANIM_5);
                thisv->behavior = thisv->nextBehavior = DOG_SIT_2;
            }
            break;
        case DOG_BOW:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDOG_ANIM_7);
                thisv->behavior = thisv->nextBehavior = DOG_BOW_2;
            }
            break;
        case DOG_WALK:
            EnDog_PlayWalkSFX(thisv);
            break;
        case DOG_RUN:
            EnDog_PlayRunSFX(thisv);
            break;
        case DOG_BARK:
            EnDog_PlayBarkSFX(thisv);
            break;
    }
    return 0;
}

s8 EnDog_CanFollow(EnDog* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        return 2;
    }

    if (globalCtx->sceneNum == SCENE_MARKET_DAY) {
        return 0;
    }

    if (thisv->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        thisv->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        if (gSaveContext.dogParams != 0) {
            return 0;
        }
        gSaveContext.dogParams = (thisv->actor.params & 0x7FFF);
        return 1;
    }

    return 0;
}

s32 EnDog_UpdateWaypoint(EnDog* thisv, GlobalContext* globalCtx) {
    s32 change;

    if (thisv->path == NULL) {
        return 0;
    }

    if (thisv->reverse) {
        change = -1;
    } else {
        change = 1;
    }

    thisv->waypoint += change;

    if (thisv->reverse) {
        if (thisv->waypoint < 0) {
            thisv->waypoint = thisv->path->count - 1;
        }
    } else {
        if ((thisv->path->count - 1) < thisv->waypoint) {
            thisv->waypoint = 0;
        }
    }

    return 1;
}

s32 EnDog_Orient(EnDog* thisv, GlobalContext* globalCtx) {
    s16 targetYaw;
    f32 waypointDistSq;

    waypointDistSq = Path_OrientAndGetDistSq(&thisv->actor, thisv->path, thisv->waypoint, &targetYaw);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, targetYaw, 10, 1000, 1);

    if ((waypointDistSq > 0.0f) && (waypointDistSq < 1000.0f)) {
        return EnDog_UpdateWaypoint(thisv, globalCtx);
    } else {
        return 0;
    }
}

void EnDog_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDog* thisv = (EnDog*)thisx;
    s16 followingDog;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gDogSkel, NULL, thisv->jointTable, thisv->morphTable, 13);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDOG_ANIM_0);

    if ((thisv->actor.params & 0x8000) == 0) {
        thisv->actor.params = (thisv->actor.params & 0xF0FF) | ((((thisv->actor.params & 0x0F00) >> 8) + 1) << 8);
    }

    followingDog = ((gSaveContext.dogParams & 0x0F00) >> 8);
    if (followingDog == ((thisv->actor.params & 0x0F00) >> 8) && ((thisv->actor.params & 0x8000) == 0)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, 0, &sColChkInfoInit);
    Actor_SetScale(&thisv->actor, 0.0075f);
    thisv->waypoint = 0;
    thisv->actor.gravity = -1.0f;
    thisv->path = Path_GetByIndex(globalCtx, (thisv->actor.params & 0x00F0) >> 4, 0xF);

    switch (globalCtx->sceneNum) {
        case SCENE_MARKET_NIGHT:
            if ((!gSaveContext.dogIsLost) && (((thisv->actor.params & 0x0F00) >> 8) == 1)) {
                Actor_Kill(&thisv->actor);
            }
            break;
        case SCENE_IMPA: // Richard's Home
            if (!(thisv->actor.params & 0x8000)) {
                if (!gSaveContext.dogIsLost) {
                    thisv->nextBehavior = DOG_SIT;
                    thisv->actionFunc = EnDog_Wait;
                    thisv->actor.speedXZ = 0.0f;
                    return;
                } else {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            }
            break;
    }

    if (thisv->actor.params & 0x8000) {
        thisv->nextBehavior = DOG_WALK;
        thisv->actionFunc = EnDog_FollowPlayer;
    } else {
        thisv->nextBehavior = DOG_SIT;
        thisv->actionFunc = EnDog_ChooseMovement;
    }
}

void EnDog_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDog* thisv = (EnDog*)thisx;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnDog_FollowPath(EnDog* thisv, GlobalContext* globalCtx) {
    s32 behaviors[] = { DOG_SIT, DOG_BOW, DOG_BARK };
    s32 unused[] = { 40, 80, 20 };
    f32 speed;
    s32 frame;

    if (EnDog_CanFollow(thisv, globalCtx) == 1) {
        thisv->actionFunc = EnDog_FollowPlayer;
    }

    if (DECR(thisv->behaviorTimer) != 0) {
        if (thisv->nextBehavior == DOG_WALK) {
            speed = 1.0f;
        } else {
            speed = 4.0f;
        }
        Math_SmoothStepToF(&thisv->actor.speedXZ, speed, 0.4f, 1.0f, 0.0f);
        EnDog_Orient(thisv, globalCtx);
        thisv->actor.shape.rot = thisv->actor.world.rot;

        // Used to change between two text boxes for Richard's owner in the Market Day scene
        // depending on where he is on his path. En_Hy checks these event flags.
        if (thisv->waypoint < 9) {
            // Richard is close to her, text says something about his coat
            gSaveContext.eventInf[3] |= 1;
        } else {
            // Richard is far, text says something about running fast
            gSaveContext.eventInf[3] &= ~1;
        }
    } else {
        frame = globalCtx->state.frames % 3;
        thisv->nextBehavior = behaviors[frame];
        // no clue why they're using the behavior id to calculate timer. possibly meant to use the unused array?
        thisv->behaviorTimer = Rand_S16Offset(60, behaviors[frame]);
        thisv->actionFunc = EnDog_ChooseMovement;
    }
}

void EnDog_ChooseMovement(EnDog* thisv, GlobalContext* globalCtx) {
    if (EnDog_CanFollow(thisv, globalCtx) == 1) {
        thisv->actionFunc = EnDog_FollowPlayer;
    }

    if (DECR(thisv->behaviorTimer) == 0) {
        thisv->behaviorTimer = Rand_S16Offset(200, 100);
        if (globalCtx->state.frames % 2) {
            thisv->nextBehavior = DOG_WALK;
        } else {
            thisv->nextBehavior = DOG_RUN;
        }

        if (thisv->nextBehavior == DOG_RUN) {
            thisv->behaviorTimer /= 2;
        }
        thisv->actionFunc = EnDog_FollowPath;
    }
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.4f, 1.0f, 0.0f);
}

void EnDog_FollowPlayer(EnDog* thisv, GlobalContext* globalCtx) {
    f32 speed;

    if (gSaveContext.dogParams == 0) {
        thisv->nextBehavior = DOG_SIT;
        thisv->actionFunc = EnDog_Wait;
        thisv->actor.speedXZ = 0.0f;
        return;
    }

    if (thisv->actor.xzDistToPlayer > 400.0f) {
        if (thisv->nextBehavior != DOG_SIT && thisv->nextBehavior != DOG_SIT_2) {
            thisv->nextBehavior = DOG_BOW;
        }
        gSaveContext.dogParams = 0;
        speed = 0.0f;
    } else if (thisv->actor.xzDistToPlayer > 100.0f) {
        thisv->nextBehavior = DOG_RUN;
        speed = 4.0f;
    } else if (thisv->actor.xzDistToPlayer < 40.0f) {
        if (thisv->nextBehavior != DOG_BOW && thisv->nextBehavior != DOG_BOW_2) {
            thisv->nextBehavior = DOG_BOW;
        }
        speed = 0.0f;
    } else {
        thisv->nextBehavior = DOG_WALK;
        speed = 1.0f;
    }

    Math_ApproachF(&thisv->actor.speedXZ, speed, 0.6f, 1.0f);

    if (!(thisv->actor.xzDistToPlayer > 400.0f)) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 10, 1000, 1);
        thisv->actor.shape.rot = thisv->actor.world.rot;
    }
}

void EnDog_RunAway(EnDog* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer < 200.0f) {
        Math_ApproachF(&thisv->actor.speedXZ, 4.0f, 0.6f, 1.0f);
        Math_SmoothStepToS(&thisv->actor.world.rot.y, (thisv->actor.yawTowardsPlayer ^ 0x8000), 10, 1000, 1);
    } else {
        thisv->actionFunc = EnDog_FaceLink;
    }
    thisv->actor.shape.rot = thisv->actor.world.rot;
}

void EnDog_FaceLink(EnDog* thisv, GlobalContext* globalCtx) {
    s16 rotTowardLink;
    s16 prevRotY;
    f32 absAngleDiff;

    // if the dog is more than 200 units away from Link, turn to face him then wait
    if (200.0f <= thisv->actor.xzDistToPlayer) {
        thisv->nextBehavior = DOG_WALK;

        Math_ApproachF(&thisv->actor.speedXZ, 1.0f, 0.6f, 1.0f);

        rotTowardLink = thisv->actor.yawTowardsPlayer;
        prevRotY = thisv->actor.world.rot.y;
        Math_SmoothStepToS(&thisv->actor.world.rot.y, rotTowardLink, 10, 1000, 1);

        absAngleDiff = thisv->actor.world.rot.y;
        absAngleDiff -= prevRotY;
        absAngleDiff = fabsf(absAngleDiff);
        if (absAngleDiff < 200.0f) {
            thisv->nextBehavior = DOG_SIT;
            thisv->actionFunc = EnDog_Wait;
            thisv->actor.speedXZ = 0.0f;
        }
    } else {
        thisv->nextBehavior = DOG_RUN;
        thisv->actionFunc = EnDog_RunAway;
    }
    thisv->actor.shape.rot = thisv->actor.world.rot;
}

void EnDog_Wait(EnDog* thisv, GlobalContext* globalCtx) {
    thisv->unusedAngle = (thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y);

    // If another dog is following Link and he gets within 200 units of waiting dog, run away
    if ((gSaveContext.dogParams != 0) && (thisv->actor.xzDistToPlayer < 200.0f)) {
        thisv->nextBehavior = DOG_RUN;
        thisv->actionFunc = EnDog_RunAway;
    }
}

void EnDog_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDog* thisv = (EnDog*)thisx;
    s32 pad;

    EnDog_PlayAnimAndSFX(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, thisv->collider.dim.radius, thisv->collider.dim.height * 0.5f, 0.0f,
                            5);
    Actor_MoveForward(&thisv->actor);
    thisv->actionFunc(thisv, globalCtx);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnDog_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return false;
}

void EnDog_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
}

void EnDog_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDog* thisv = (EnDog*)thisx;
    Color_RGBA8 colors[] = { { 255, 255, 200, 0 }, { 150, 100, 50, 0 } };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dog.c", 972);

    func_80093D18(globalCtx->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, colors[thisv->actor.params & 0xF].r, colors[thisv->actor.params & 0xF].g,
                   colors[thisv->actor.params & 0xF].b, colors[thisv->actor.params & 0xF].a);

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDog_OverrideLimbDraw, EnDog_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dog.c", 994);
}
