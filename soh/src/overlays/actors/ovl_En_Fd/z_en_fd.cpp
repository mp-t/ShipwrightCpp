/*
 * File: z_en_fd.c
 * Overlay: ovl_En_Fd
 * Description: Flare Dancer (enflamed form)
 */

#include "z_en_fd.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_fw/object_fw.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_9)

#define FLG_COREDEAD (0x4000)
#define FLG_COREDONE (0x8000)

void EnFd_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFd_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFd_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFd_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnFd_Run(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_SpinAndSpawnFire(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_Reappear(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_SpinAndGrow(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_JumpToGround(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_WaitForCore(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_UpdateFlames(EnFd* thisv);
void EnFd_UpdateDots(EnFd* thisv);
void EnFd_AddEffect(EnFd*, u8, Vec3f*, Vec3f*, Vec3f*, u8, f32, f32);
void EnFd_DrawDots(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_DrawFlames(EnFd* thisv, GlobalContext* globalCtx);
void EnFd_Land(EnFd* thisv, GlobalContext* globalCtx);

const ActorInit En_Fd_InitVars = {
    ACTOR_EN_FD,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_FW,
    sizeof(EnFd),
    (ActorFunc)EnFd_Init,
    (ActorFunc)EnFd_Destroy,
    (ActorFunc)EnFd_Update,
    (ActorFunc)EnFd_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[12] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040088, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 21, { { 1600, 0, 0 }, 5 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 12, { { 1600, 0, 0 }, 5 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 14, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 15, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 16, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 17, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 18, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 19, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 4, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 4, { { 5000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 8, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 8, { { 5000, 0, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    12,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInit = { 24, 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENFD_ANIM_0,
    /* 1 */ ENFD_ANIM_1,
    /* 2 */ ENFD_ANIM_2,
    /* 3 */ ENFD_ANIM_3,
    /* 4 */ ENFD_ANIM_4
} EnFdAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gFlareDancerCastingFireAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, 0.0f },
    { &gFlareDancerBackflipAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerGettingUpAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerChasingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gFlareDancerTwirlAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
};

s32 EnFd_SpawnCore(EnFd* thisv, GlobalContext* globalCtx) {
    if (thisv->invincibilityTimer != 0) {
        return false;
    }

    if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FW, thisv->corePos.x, thisv->corePos.y,
                           thisv->corePos.z, 0, thisv->actor.shape.rot.y, 0, thisv->runDir) == NULL) {
        return false;
    }

    thisv->actor.child->colChkInfo.health = thisv->actor.colChkInfo.health % 8;

    if (thisv->actor.child->colChkInfo.health == 0) {
        thisv->actor.child->colChkInfo.health = 8;
    }

    if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        func_8002DE04(globalCtx, &thisv->actor, thisv->actor.child);
    }

    thisv->coreActive = true;

    return true;
}

void EnFd_SpawnChildFire(EnFd* thisv, GlobalContext* globalCtx, s16 fireCnt, s16 color) {
    s32 i;

    for (i = 0; i < fireCnt; i++) {
        s16 angle = (s16)((((i * 360.0f) / fireCnt) * (0x10000 / 360.0f))) + thisv->actor.yawTowardsPlayer;
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FD_FIRE, thisv->actor.world.pos.x,
                           thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, angle, 0, (color << 0xF) | i);
    }
}

void EnFd_SpawnDot(EnFd* thisv, GlobalContext* globalCtx) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    if (thisv->actionFunc == EnFd_Run || thisv->actionFunc == EnFd_SpinAndSpawnFire) {
        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.floorHeight + 4.0f;
        pos.z = thisv->actor.world.pos.z;
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.y = ((Rand_ZeroOne() - 0.5f) * 0.2f) + 0.3f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        EnFd_AddEffect(thisv, FD_EFFECT_FLAME, &pos, &velocity, &accel, 8, 0.6f, 0.2f);
    }
}

/**
 * Checks to see if the hammer effect is active, and if it should be applied
 */
s32 EnFd_CheckHammer(EnFd* thisv, GlobalContext* globalCtx) {
    if (thisv->actionFunc == EnFd_Reappear || thisv->actionFunc == EnFd_SpinAndGrow ||
        thisv->actionFunc == EnFd_JumpToGround || thisv->actionFunc == EnFd_WaitForCore) {
        return false;
    } else if (globalCtx->actorCtx.unk_02 != 0 && thisv->actor.xzDistToPlayer < 300.0f &&
               thisv->actor.yDistToPlayer < 60.0f) {
        return true;
    } else {
        return false;
    }
}

s32 EnFd_ColliderCheck(EnFd* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    ColliderInfo* info;

    if (thisv->collider.base.acFlags & AC_HIT || EnFd_CheckHammer(thisv, globalCtx)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisv->invincibilityTimer != 0) {
            return false;
        }
        info = &thisv->collider.elements[0].info;
        if (info->acHitInfo != NULL && (info->acHitInfo->toucher.dmgFlags & 0x80)) {
            return false;
        }

        if (!EnFd_SpawnCore(thisv, globalCtx)) {
            return false;
        }
        thisv->invincibilityTimer = 30;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_DAMAGE);
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
        return true;
    } else if (DECR(thisv->attackTimer) == 0 && thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        if (thisv->invincibilityTimer != 0) {
            return false;
        }

        if (thisv->collider.base.atFlags & AT_BOUNCED) {
            return false;
        }
        thisv->attackTimer = 30;
        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        func_8002F71C(globalCtx, &thisv->actor, thisv->actor.speedXZ + 2.0f, thisv->actor.yawTowardsPlayer, 6.0f);
    }
    return false;
}

/**
 * Determines if `actor` is within an acceptable range for `thisv` to be able to "see" `actor`
 * `actor` must be within 400 units of `thisv`, `actor` must be within +/- 40 degrees facing angle
 * towards `actor`, and there must not be a collision poly between `thisv` and `actor`
 */
s32 EnFd_CanSeeActor(EnFd* thisv, Actor* actor, GlobalContext* globalCtx) {
    CollisionPoly* colPoly;
    s32 bgId;
    Vec3f colPoint;
    s16 angle;
    s32 pad;

    // Check to see if `actor` is within 400 units of `thisv`
    if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &actor->world.pos) > 400.0f) {
        return false;
    }

    // Check to see if the angle between thisv facing angle and `actor` is withing ~40 degrees
    angle = (f32)Math_Vec3f_Yaw(&thisv->actor.world.pos, &actor->world.pos) - thisv->actor.shape.rot.y;
    if (ABS(angle) > 0x1C70) {
        return false;
    }

    // check to see if the line between `thisv` and `actor` does not intersect a collision poly
    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &actor->world.pos, &colPoint, &colPoly,
                                true, false, false, true, &bgId)) {
        return false;
    }

    return true;
}

Actor* EnFd_FindBomb(EnFd* thisv, GlobalContext* globalCtx) {
    Actor* actor = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (actor != NULL) {
        if (actor->params != 0 || actor->parent != NULL) {
            actor = actor->next;
            continue;
        }

        if (actor->id != ACTOR_EN_BOM) {
            actor = actor->next;
            continue;
        }

        if (EnFd_CanSeeActor(thisv, actor, globalCtx) != 1) {
            actor = actor->next;
            continue;
        }

        return actor;
    }
    return NULL;
}

Actor* EnFd_FindPotentialTheat(EnFd* thisv, GlobalContext* globalCtx) {
    Player* player;
    Actor* bomb = EnFd_FindBomb(thisv, globalCtx);

    if (bomb != NULL) {
        return bomb;
    }

    if (thisv->attackTimer != 0) {
        return NULL;
    }

    player = GET_PLAYER(globalCtx);
    if (!EnFd_CanSeeActor(thisv, &player->actor, globalCtx)) {
        return NULL;
    }

    return &player->actor;
}

/**
 * Creates a delta in `dst` for the position from `thisv`'s current position to the next
 * position in a circle formed by `radius` with center at `thisv`'s initial position.
 */
Vec3f* EnFd_GetPosAdjAroundCircle(Vec3f* dst, EnFd* thisv, f32 radius, s16 dir) {
    s16 angle;
    Vec3f newPos;

    angle = Math_Vec3f_Yaw(&thisv->actor.home.pos, &thisv->actor.world.pos) + (dir * 0x1554); // ~30 degrees
    newPos.x = (Math_SinS(angle) * radius) + thisv->actor.home.pos.x;
    newPos.z = (Math_CosS(angle) * radius) + thisv->actor.home.pos.z;
    newPos.x -= thisv->actor.world.pos.x;
    newPos.z -= thisv->actor.world.pos.z;
    *dst = newPos;
    return dst;
}

s32 EnFd_ShouldStopRunning(EnFd* thisv, GlobalContext* globalCtx, f32 radius, s16* runDir) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f colPoint;
    Vec3f pos;

    // Check to see if the next position on the rotation around the circle
    // will result in a background collision
    EnFd_GetPosAdjAroundCircle(&pos, thisv, radius, *runDir);

    pos.x += thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y;
    pos.z += thisv->actor.world.pos.z;

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &pos, &colPoint, &poly, true, false, false,
                                true, &bgId)) {
        *runDir = -*runDir;
        return true;
    }

    if (thisv->circlesToComplete != 0 || DECR(thisv->spinTimer) != 0) {
        return false;
    }

    if (Rand_ZeroOne() > 0.5f) {
        *runDir = -*runDir;
    }
    return true;
}

void EnFd_Fade(EnFd* thisv, GlobalContext* globalCtx) {
    if (thisv->invincibilityTimer != 0) {
        Math_SmoothStepToF(&thisv->fadeAlpha, 0.0f, 0.3f, 10.0f, 0.0f);
        thisv->actor.shape.shadowAlpha = thisv->fadeAlpha;
        if (!(thisv->fadeAlpha >= 0.9f)) {
            thisv->invincibilityTimer = 0;
            thisv->spinTimer = 0;
            thisv->actionFunc = EnFd_WaitForCore;
            thisv->actor.speedXZ = 0.0f;
        }
    }
}

void EnFd_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFd* thisv = (EnFd*)thisx;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gFlareDancerSkel, NULL, thisv->jointTable, thisv->morphTable, 27);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 32.0f);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colSphs);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(0xF), &sColChkInit);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.flags |= ACTOR_FLAG_24;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->firstUpdateFlag = true;
    thisv->actor.gravity = -1.0f;
    thisv->runDir = Rand_ZeroOne() < 0.5f ? -1 : 1;
    thisv->actor.naviEnemyId = 0x22;
    thisv->actionFunc = EnFd_Reappear;
}

void EnFd_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFd* thisv = (EnFd*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnFd_Reappear(EnFd* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.pos = thisv->actor.home.pos;
    thisv->actor.params = 0;
    thisv->actor.shape.shadowAlpha = 0xFF;
    thisv->coreActive = false;
    thisv->actor.scale.y = 0.0f;
    thisv->fadeAlpha = 255.0f;
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_0);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_LAUGH);
    thisv->actionFunc = EnFd_SpinAndGrow;
}

void EnFd_SpinAndGrow(EnFd* thisv, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        thisv->actor.velocity.y = 6.0f;
        thisv->actor.scale.y = 0.01f;
        thisv->actor.world.rot.y ^= 0x8000;
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.speedXZ = 8.0f;
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_1);
        thisv->actionFunc = EnFd_JumpToGround;
    } else {
        thisv->actor.scale.y = thisv->skelAnime.curFrame * (0.01f / thisv->skelAnime.animLength);
        thisv->actor.shape.rot.y += 0x2000;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
}

void EnFd_JumpToGround(EnFd* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.bgCheckFlags & 1) && !(thisv->actor.velocity.y > 0.0f)) {
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_2);
        thisv->actionFunc = EnFd_Land;
    }
}

void EnFd_Land(EnFd* thisv, GlobalContext* globalCtx) {
    Vec3f adjPos;

    Math_SmoothStepToF(&thisv->skelAnime.playSpeed, 1.0f, 0.1f, 1.0f, 0.0f);
    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        thisv->spinTimer = Rand_S16Offset(60, 90);
        thisv->runRadius = Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->actor.home.pos);
        EnFd_GetPosAdjAroundCircle(&adjPos, thisv, thisv->runRadius, thisv->runDir);
        thisv->actor.world.rot.y = Math_FAtan2F(adjPos.x, adjPos.z) * (0x8000 / std::numbers::pi_v<float>);
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_4);
        thisv->actionFunc = EnFd_SpinAndSpawnFire;
    }
}

void EnFd_SpinAndSpawnFire(EnFd* thisv, GlobalContext* globalCtx) {
    f32 deceleration;
    f32 tgtSpeed;
    f32 rotSpeed;

    if ((thisv->spinTimer < 31) && (thisv->invincibilityTimer == 0)) {
        func_8002F974(&thisv->actor, NA_SE_EN_FLAME_FIRE_ATTACK - SFX_FLAG);
    } else {
        func_8002F974(&thisv->actor, NA_SE_EN_FLAME_ROLL - SFX_FLAG);
    }

    if (DECR(thisv->spinTimer) != 0) {
        thisv->actor.shape.rot.y += (thisv->runDir * 0x2000);
        if (thisv->spinTimer == 30 && thisv->invincibilityTimer == 0) {
            if (thisv->actor.xzDistToPlayer > 160.0f) {
                // orange flames
                EnFd_SpawnChildFire(thisv, globalCtx, 8, 0);
            } else {
                // blue flames
                EnFd_SpawnChildFire(thisv, globalCtx, 8, 1);
            }
        }
    } else {
        // slow shape rotation down to meet `thisv` rotation within ~1.66 degrees
        deceleration = thisv->actor.world.rot.y;
        deceleration -= thisv->actor.shape.rot.y;
        rotSpeed = 0.0f;
        tgtSpeed = fabsf(deceleration);
        deceleration /= tgtSpeed;
        Math_ApproachF(&rotSpeed, tgtSpeed, 0.6f, 0x2000);
        rotSpeed *= deceleration;
        thisv->actor.shape.rot.y += (s16)rotSpeed;
        rotSpeed = fabsf(rotSpeed);
        if ((s32)rotSpeed <= 300) {
            // ~1.6 degrees
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        }

        if (thisv->actor.shape.rot.y == thisv->actor.world.rot.y) {
            thisv->initYawToInitPos = Math_Vec3f_Yaw(&thisv->actor.home.pos, &thisv->actor.world.pos);
            thisv->curYawToInitPos = thisv->runDir < 0 ? 0xFFFF : 0;
            thisv->circlesToComplete = (globalCtx->state.frames & 7) + 2;
            thisv->spinTimer = Rand_S16Offset(30, 120);
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_3);
            thisv->actionFunc = EnFd_Run;
        }
    }
}

/**
 * Run around in a circle with the center being the initial position, and
 * the radius being the distance from the initial position to the nearest
 * threat (bomb or player).
 */
void EnFd_Run(EnFd* thisv, GlobalContext* globalCtx) {
    Actor* potentialThreat;
    s16 yawToYawTarget;
    f32 runRadiusTarget;
    Vec3f adjPos;

    if (EnFd_ShouldStopRunning(thisv, globalCtx, thisv->runRadius, &thisv->runDir)) {
        if (thisv->invincibilityTimer == 0) {
            thisv->actor.world.rot.y ^= 0x8000;
            thisv->actor.velocity.y = 6.0f;
            thisv->actor.speedXZ = 0.0f;
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFD_ANIM_1);
            thisv->actionFunc = EnFd_JumpToGround;
            return;
        }
    }

    yawToYawTarget = Math_Vec3f_Yaw(&thisv->actor.home.pos, &thisv->actor.world.pos) - thisv->initYawToInitPos;
    if (thisv->runDir > 0) {
        if ((u16)thisv->curYawToInitPos > (u16)(yawToYawTarget)) {
            thisv->circlesToComplete--;
        }
    } else if ((u16)thisv->curYawToInitPos < (u16)(yawToYawTarget)) {
        thisv->circlesToComplete--;
    }

    if (thisv->circlesToComplete < 0) {
        thisv->circlesToComplete = 0;
    }
    thisv->curYawToInitPos = yawToYawTarget;

    // If there is a bomb out, or if the player exists, set radius to
    // the distance to that threat, otherwise default to 200.
    potentialThreat = EnFd_FindPotentialTheat(thisv, globalCtx);
    if ((potentialThreat != NULL) && (thisv->invincibilityTimer == 0)) {
        runRadiusTarget = Math_Vec3f_DistXYZ(&thisv->actor.home.pos, &potentialThreat->world.pos);
    } else {
        runRadiusTarget = 200.0f;
    }
    Math_SmoothStepToF(&thisv->runRadius, runRadiusTarget, 0.3f, 100.0f, 0.0f);
    EnFd_GetPosAdjAroundCircle(&adjPos, thisv, thisv->runRadius, thisv->runDir);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(adjPos.x, adjPos.z) * (0x8000 / std::numbers::pi_v<float>), 4, 0xFA0, 1);
    thisv->actor.world.rot = thisv->actor.shape.rot;
    func_8002F974(&thisv->actor, NA_SE_EN_FLAME_RUN - SFX_FLAG);
    if (thisv->skelAnime.curFrame == 6.0f || thisv->skelAnime.curFrame == 13.0f || thisv->skelAnime.curFrame == 28.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_KICK);
    }
    Math_SmoothStepToF(&thisv->actor.speedXZ, 8.0f, 0.1f, 1.0f, 0.0f);
}

/**
 * En_Fw will set `thisv` params when it is done with its action.
 * It will set FLG_COREDONE when the core has returned to `thisv`'s initial
 * position, and FLG_COREDEAD when there is no health left
 */
void EnFd_WaitForCore(EnFd* thisv, GlobalContext* globalCtx) {
    if (thisv->spinTimer != 0) {
        thisv->spinTimer--;
        if (thisv->spinTimer == 0) {
            Actor_Kill(&thisv->actor);
        }
    } else if (thisv->actor.params & FLG_COREDONE) {
        thisv->actionFunc = EnFd_Reappear;
    } else if (thisv->actor.params & FLG_COREDEAD) {
        thisv->actor.params = 0;
        thisv->spinTimer = 30;
    }
}

void EnFd_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFd* thisv = (EnFd*)thisx;
    s32 pad;

    if (thisv->firstUpdateFlag) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
        thisv->firstUpdateFlag = false;
    }

    if (thisv->actionFunc != EnFd_Reappear) {
        SkelAnime_Update(&thisv->skelAnime);
        EnFd_SpawnDot(thisv, globalCtx);
    }

    if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        // has been hookshoted
        if (EnFd_SpawnCore(thisv, globalCtx)) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->invincibilityTimer = 30;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_DAMAGE);
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
        } else {
            thisv->actor.flags &= ~ACTOR_FLAG_13;
        }
    } else if (thisv->actionFunc != EnFd_WaitForCore) {
        EnFd_ColliderCheck(thisv, globalCtx);
    }
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    EnFd_Fade(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    EnFd_UpdateDots(thisv);
    EnFd_UpdateFlames(thisv);
    if (thisv->actionFunc != EnFd_Reappear && thisv->actionFunc != EnFd_SpinAndGrow &&
        thisv->actionFunc != EnFd_WaitForCore) {
        if (thisv->attackTimer == 0 && thisv->invincibilityTimer == 0) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        if ((thisv->actionFunc == EnFd_Run) || (thisv->actionFunc == EnFd_SpinAndSpawnFire)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

s32 EnFd_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfxP) {
    EnFd* thisv = (EnFd*)thisx;

    if (thisv->invincibilityTimer != 0) {
        switch (limbIndex) {
            case 13:
            case 21:
                *dList = NULL;
                break;
        }
    }

    return false;
}

void EnFd_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnFd* thisv = (EnFd*)thisx;
    Vec3f unused0 = { 6800.0f, 0.0f, 0.0f };
    Vec3f unused1 = { 6800.0f, 0.0f, 0.0f };
    Vec3f initialPos = { 0.0f, 0.0f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 i;

    if (limbIndex == 21) {
        Matrix_MultVec3f(&initialPos, &thisv->corePos);
    }

    if (limbIndex == 13) {
        Matrix_MultVec3f(&initialPos, &thisv->actor.focus.pos);
    }

    if (limbIndex == 3 || limbIndex == 6 || limbIndex == 7 || limbIndex == 10 || limbIndex == 14 || limbIndex == 15 ||
        limbIndex == 17 || limbIndex == 18 || limbIndex == 20 || limbIndex == 22 || limbIndex == 23 ||
        limbIndex == 24 || limbIndex == 25 || limbIndex == 26) {
        if ((globalCtx->state.frames % 2) != 0) {
            for (i = 0; i < 1; i++) {
                Matrix_MultVec3f(&initialPos, &pos);
                pos.x += (Rand_ZeroOne() - 0.5f) * 20.0f;
                pos.y += (Rand_ZeroOne() - 0.5f) * 40.0f;
                pos.z += (Rand_ZeroOne() - 0.5f) * 20.0f;
                accel.x = (Rand_ZeroOne() - 0.5f) * 0.4f;
                accel.y = ((Rand_ZeroOne() - 0.5f) * 0.2f) + 0.6f;
                accel.z = (Rand_ZeroOne() - 0.5f) * 0.4f;
                EnFd_AddEffect(thisv, FD_EFFECT_DOT, &pos, &velocity, &accel, 0, 0.006f, 0.0f);
            }
        }
    }

    Collider_UpdateSpheres(limbIndex, &thisv->collider);
}

void EnFd_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnFd* thisv = (EnFd*)thisx;
    s32 clampedHealth;
    Color_RGBA8 primColors[] = {
        { 255, 255, 200, 255 },
        { 200, 200, 200, 255 },
        { 255, 255, 0, 255 },
    };
    Color_RGBA8 envColors[] = {
        { 0, 255, 0, 255 },
        { 0, 0, 255, 255 },
        { 255, 0, 0, 255 },
    };
    u32 frames;
    s32 pad;

    frames = globalCtx->state.frames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 1751);

    Matrix_Push();
    EnFd_DrawDots(thisv, globalCtx);
    EnFd_DrawFlames(thisv, globalCtx);
    Matrix_Pop();
    if (thisv->actionFunc != EnFd_Reappear && !(thisv->fadeAlpha < 0.9f)) {
        if (1) {}
        func_80093D84(globalCtx->state.gfxCtx);
        clampedHealth = CLAMP(thisx->colChkInfo.health - 1, 0, 23);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, primColors[clampedHealth / 8].r, primColors[clampedHealth / 8].g,
                        primColors[clampedHealth / 8].b, (u8)thisv->fadeAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, envColors[clampedHealth / 8].r, envColors[clampedHealth / 8].g,
                       envColors[clampedHealth / 8].b, (u8)thisv->fadeAlpha);
        gSPSegment(
            POLY_XLU_DISP++, 0x8,
            Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, 0xFF - (u8)(frames * 6), 8, 0x40));
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x9, D_80116280);

        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, EnFd_OverrideLimbDraw, EnFd_PostLimbDraw, thisv,
                                           POLY_XLU_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 1822);
}

void EnFd_AddEffect(EnFd* thisv, u8 type, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 timer, f32 scale,
                    f32 scaleStep) {
    EnFdEffect* eff = thisv->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type != FD_EFFECT_NONE) {
            continue;
        }
        eff->scale = scale;
        eff->scaleStep = scaleStep;
        eff->initialTimer = eff->timer = timer;
        eff->type = type;
        eff->pos = *pos;
        eff->accel = *accel;
        eff->velocity = *velocity;
        if (eff->type == FD_EFFECT_DOT) {
            eff->color.a = 255;
            eff->timer = (s16)(Rand_ZeroOne() * 10.0f);
        }
        return;
    }
}

void EnFd_UpdateFlames(EnFd* thisv) {
    s16 i;
    EnFdEffect* eff = thisv->effects;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_FLAME) {
            eff->timer--;
            if (eff->timer == 0) {
                eff->type = FD_EFFECT_NONE;
            }
            eff->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;
            eff->scale += eff->scaleStep;
        }
    }
}

void EnFd_UpdateDots(EnFd* thisv) {
    EnFdEffect* eff = thisv->effects;
    s16 i;
    Color_RGBA8 dotColors[] = {
        { 255, 128, 0, 0 },
        { 255, 0, 0, 0 },
        { 255, 255, 0, 0 },
        { 255, 0, 0, 0 },
    };

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_DOT) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->timer++;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            eff->color.r = dotColors[eff->timer % 4].r;
            eff->color.g = dotColors[eff->timer % 4].g;
            eff->color.b = dotColors[eff->timer % 4].b;
            if (eff->color.a > 30) {
                eff->color.a -= 30;
            } else {
                eff->color.a = 0;
                eff->type = FD_EFFECT_NONE;
            }
        }
    }
}

void EnFd_DrawFlames(EnFd* thisv, GlobalContext* globalCtx) {
    static void* dustTextures[] = {
        gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
    };
    s32 firstDone;
    s16 i;
    s16 idx;
    EnFdEffect* eff = thisv->effects;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 1969);
    firstDone = false;
    if (1) {}
    func_80093D84(globalCtx->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_FLAME) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_7928);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, (u8)((thisv->fadeAlpha / 255.0f) * 255));
                firstDone = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (u8)((thisv->fadeAlpha / 255.0f) * 255));
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fd.c", 2006),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            idx = eff->timer * (8.0f / eff->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(dustTextures[idx]));
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerSquareParticleDL);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 2020);
}

void EnFd_DrawDots(EnFd* thisv, GlobalContext* globalCtx) {
    s16 i;
    s16 firstDone;
    EnFdEffect* eff = thisv->effects;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 2034);

    firstDone = false;
    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_DOT) {
            if (!firstDone) {
                func_80093D84(globalCtx->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_79F8);
                firstDone = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b,
                            (u8)(eff->color.a * (thisv->fadeAlpha / 255.0f)));
            gDPPipeSync(POLY_XLU_DISP++);
            if (1) {}
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fd.c", 2064),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerTriangleParticleDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fd.c", 2071);
}
