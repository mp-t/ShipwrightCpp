#include "z_en_ssh.h"
#include "objects/object_ssh/object_ssh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

#define SSH_STATE_STUNNED (1 << 0)
#define SSH_STATE_GROUND_START (1 << 2)
#define SSH_STATE_ATTACKED (1 << 3)
#define SSH_STATE_SPIN (1 << 4)

typedef enum {
    SSH_ANIM_UNK0, // Unused animation. Possibly being knocked back?
    SSH_ANIM_UP,
    SSH_ANIM_WAIT,
    SSH_ANIM_LAND,
    SSH_ANIM_DROP,
    SSH_ANIM_UNK5, // Slower version of ANIM_DROP
    SSH_ANIM_UNK6  // Faster repeating version of ANIM_UNK0
} EnSshAnimation;

void EnSsh_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSsh_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSsh_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSsh_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSsh_Idle(EnSsh* thisv, GlobalContext* globalCtx);
void EnSsh_Drop(EnSsh* thisv, GlobalContext* globalCtx);
void EnSsh_Return(EnSsh* thisv, GlobalContext* globalCtx);
void EnSsh_Start(EnSsh* thisv, GlobalContext* globalCtx);

#include "overlays/ovl_En_Ssh/ovl_En_Ssh.h"

const ActorInit En_Ssh_InitVars = {
    ACTOR_EN_SSH,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SSH,
    sizeof(EnSsh),
    (ActorFunc)EnSsh_Init,
    (ActorFunc)EnSsh_Destroy,
    (ActorFunc)EnSsh_Update,
    (ActorFunc)EnSsh_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 32, 50, -24, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 20, 60, -30, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, -240, 0 }, 28 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void EnSsh_SetupAction(EnSsh* thisv, EnSshActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnSsh_SpawnShockwave(EnSsh* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.floorHeight;
    pos.z = thisv->actor.world.pos.z;
    EffectSsBlast_SpawnWhiteCustomScale(globalCtx, &pos, &zeroVec, &zeroVec, 100, 220, 8);
}

s32 EnSsh_CreateBlureEffect(GlobalContext* globalCtx) {
    EffectBlureInit1 blureInit;
    u8 p1StartColor[] = { 255, 255, 255, 75 };
    u8 p2StartColor[] = { 255, 255, 255, 75 };
    u8 p1EndColor[] = { 255, 255, 255, 0 };
    u8 p2EndColor[] = { 255, 255, 255, 0 };
    s32 i;
    s32 blureIdx;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = p1StartColor[i];
        blureInit.p2StartColor[i] = p2StartColor[i];
        blureInit.p1EndColor[i] = p1EndColor[i];
        blureInit.p2EndColor[i] = p2EndColor[i];
    }

    blureInit.elemDuration = 6;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 3;

    Effect_Add(globalCtx, &blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    return blureIdx;
}

s32 EnSsh_CheckCeilingPos(EnSsh* thisv, GlobalContext* globalCtx) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f posB;

    posB.x = thisv->actor.world.pos.x;
    posB.y = thisv->actor.world.pos.y + 1000.0f;
    posB.z = thisv->actor.world.pos.z;
    if (!BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &posB, &thisv->ceilingPos, &poly, false,
                                 false, true, true, &bgId)) {
        return false;
    } else {
        return true;
    }
}

void EnSsh_AddBlureVertex(EnSsh* thisv) {
    Vec3f p1base = { 834.0f, 834.0f, 0.0f };
    Vec3f p2base = { 834.0f, -584.0f, 0.0f };
    Vec3f p1;
    Vec3f p2;

    p1base.x *= thisv->colliderScale;
    p1base.y *= thisv->colliderScale;
    p1base.z *= thisv->colliderScale;
    p2base.x *= thisv->colliderScale;
    p2base.y *= thisv->colliderScale;
    p2base.z *= thisv->colliderScale;
    Matrix_Push();
    Matrix_MultVec3f(&p1base, &p1);
    Matrix_MultVec3f(&p2base, &p2);
    Matrix_Pop();
    EffectBlure_AddVertex(Effect_GetByIndex(thisv->blureIdx), &p1, &p2);
}

void EnSsh_AddBlureSpace(EnSsh* thisv) {
    EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIdx));
}

void EnSsh_InitColliders(EnSsh* thisv, GlobalContext* globalCtx) {
    ColliderCylinderInit* cylinders[6] = {
        &sCylinderInit1, &sCylinderInit1, &sCylinderInit1, &sCylinderInit2, &sCylinderInit2, &sCylinderInit2,
    };
    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        Collider_InitCylinder(globalCtx, &thisv->colCylinder[i]);
        Collider_SetCylinder(globalCtx, &thisv->colCylinder[i], &thisv->actor, cylinders[i]);
    }

    thisv->colCylinder[0].info.bumper.dmgFlags = 0x0003F8E9;
    thisv->colCylinder[1].info.bumper.dmgFlags = 0xFFC00716;
    thisv->colCylinder[2].base.colType = COLTYPE_METAL;
    thisv->colCylinder[2].info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO;
    thisv->colCylinder[2].info.elemType = ELEMTYPE_UNK2;
    thisv->colCylinder[2].info.bumper.dmgFlags = 0xFFCC0716;

    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(2), &sColChkInfoInit);

    Collider_InitJntSph(globalCtx, &thisv->colSph);
    Collider_SetJntSph(globalCtx, &thisv->colSph, &thisv->actor, &sJntSphInit, thisv->colSphElements);
}

f32 EnSsh_SetAnimation(EnSsh* thisv, s32 animIndex) {
    AnimationHeader* animation[] = {
        &object_ssh_Anim_005BE8, &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_0055F8,
        &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_005BE8,
    };
    f32 playbackSpeed[] = { 1.0f, 4.0f, 1.0f, 1.0f, 8.0f, 6.0f, 2.0f };
    u8 mode[] = { 3, 3, 1, 3, 1, 1, 1 };
    f32 frameCount = Animation_GetLastFrame(animation[animIndex]);
    s32 pad;

    Animation_Change(&thisv->skelAnime, animation[animIndex], playbackSpeed[animIndex], 0.0f, frameCount,
                     mode[animIndex], -6.0f);

    return frameCount;
}

void EnSsh_SetWaitAnimation(EnSsh* thisv) {
    EnSsh_SetAnimation(thisv, SSH_ANIM_WAIT);
}

void EnSsh_SetReturnAnimation(EnSsh* thisv) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_UP);
    EnSsh_SetAnimation(thisv, SSH_ANIM_UP);
}

void EnSsh_SetLandAnimation(EnSsh* thisv) {
    thisv->actor.world.pos.y = thisv->floorHeightOffset + thisv->actor.floorHeight;
    thisv->animTimer = EnSsh_SetAnimation(thisv, SSH_ANIM_LAND);
}

void EnSsh_SetDropAnimation(EnSsh* thisv) {
    if (thisv->unkTimer == 0) {
        thisv->animTimer = EnSsh_SetAnimation(thisv, SSH_ANIM_DROP);
    }
    thisv->actor.velocity.y = -10.0f;
}

void EnSsh_SetStunned(EnSsh* thisv) {
    if (thisv->stunTimer == 0) {
        thisv->stateFlags |= SSH_STATE_ATTACKED;
        thisv->stunTimer = 120;
        thisv->actor.colorFilterTimer = 0;
    }
}

void EnSsh_SetColliderScale(EnSsh* thisv, f32 scale, f32 radiusMod) {
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    radius = thisv->colSph.elements[0].dim.modelSphere.radius;
    radius *= scale;
    thisv->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = thisv->colCylinder[i].dim.yShift;
        radius = thisv->colCylinder[i].dim.radius;
        height = thisv->colCylinder[i].dim.height;
        yShift *= scale;
        radius *= scale * radiusMod;
        height *= scale;

        thisv->colCylinder[i].dim.yShift = yShift;
        thisv->colCylinder[i].dim.radius = radius;
        thisv->colCylinder[i].dim.height = height;
    }
    Actor_SetScale(&thisv->actor, 0.04f * scale);
    thisv->floorHeightOffset = 40.0f * scale;
    thisv->colliderScale = scale * 1.5f;
}

s32 EnSsh_Damaged(EnSsh* thisv) {
    if ((thisv->stunTimer == 120) && (thisv->stateFlags & SSH_STATE_STUNNED)) {
        Actor_SetColorFilter(&thisv->actor, 0, 0xC8, 0, thisv->stunTimer);
    }
    if (DECR(thisv->stunTimer) != 0) {
        Math_SmoothStepToS(&thisv->maxTurnRate, 0x2710, 0xA, 0x3E8, 1);
        return false;
    } else {
        thisv->stunTimer = 0;
        thisv->stateFlags &= ~SSH_STATE_STUNNED;
        thisv->spinTimer = 0;
        if (thisv->swayTimer == 0) {
            thisv->spinTimer = 30;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_ROLL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_ST_ATTACK);
        return true;
    }
}

void EnSsh_Turn(EnSsh* thisv, GlobalContext* globalCtx) {
    if (thisv->hitTimer != 0) {
        thisv->hitTimer--;
    }
    if (DECR(thisv->spinTimer) != 0) {
        thisv->actor.world.rot.y += 10000.0f * (thisv->spinTimer / 30.0f);
    } else if ((thisv->swayTimer == 0) && (thisv->stunTimer == 0)) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 4, 0x2710, 1);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnSsh_Stunned(EnSsh* thisv, GlobalContext* globalCtx) {
    if ((thisv->swayTimer == 0) && (thisv->stunTimer == 0)) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer ^ 0x8000, 4, thisv->maxTurnRate, 1);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if (thisv->stunTimer < 30) {
        if (thisv->stunTimer & 1) {
            thisv->actor.shape.rot.y += 0x7D0;
        } else {
            thisv->actor.shape.rot.y -= 0x7D0;
        }
    }
}

void EnSsh_UpdateYaw(EnSsh* thisv, GlobalContext* globalCtx) {
    if (thisv->stunTimer != 0) {
        EnSsh_Stunned(thisv, globalCtx);
    } else {
        EnSsh_Turn(thisv, globalCtx);
    }
}

void EnSsh_Bob(EnSsh* thisv, GlobalContext* globalCtx) {
    f32 bobVel = 0.5f;

    if ((globalCtx->state.frames & 8) != 0) {
        bobVel *= -1.0f;
    }
    Math_SmoothStepToF(&thisv->actor.velocity.y, bobVel, 0.4f, 1000.0f, 0.0f);
}

s32 EnSsh_IsCloseToLink(EnSsh* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 yDist;

    if (thisv->stateFlags & SSH_STATE_GROUND_START) {
        return true;
    }
    if (thisv->unkTimer != 0) {
        return true;
    }
    if (thisv->swayTimer != 0) {
        return true;
    }
    if (thisv->animTimer != 0) {
        return true;
    }

    if (thisv->actor.xzDistToPlayer > 160.0f) {
        return false;
    }

    yDist = thisv->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        return false;
    }

    if (player->actor.world.pos.y < thisv->actor.floorHeight) {
        return false;
    }
    return true;
}

s32 EnSsh_IsCloseToHome(EnSsh* thisv) {
    f32 vel = thisv->actor.velocity.y;
    f32 nextY = thisv->actor.world.pos.y + 2.0f * thisv->actor.velocity.y;

    if (nextY >= thisv->actor.home.pos.y) {
        return 1;
    }
    return 0;
}

s32 EnSsh_IsCloseToGround(EnSsh* thisv) {
    f32 vel = thisv->actor.velocity.y;
    f32 nextY = thisv->actor.world.pos.y + 2.0f * thisv->actor.velocity.y;

    if ((nextY - thisv->actor.floorHeight) <= thisv->floorHeightOffset) {
        return 1;
    }
    return 0;
}

void EnSsh_Sway(EnSsh* thisv) {
    Vec3f swayVecBase;
    Vec3f swayVec;
    f32 temp;
    s16 swayAngle;

    if (thisv->swayTimer != 0) {
        thisv->swayAngle += 0x640;
        thisv->swayTimer--;
        if (thisv->swayTimer == 0) {
            thisv->swayAngle = 0;
        }
        temp = thisv->swayTimer * (1.0f / 6);
        swayAngle = temp * (0x10000 / 360.0f) * Math_SinS(thisv->swayAngle);
        temp = thisv->actor.world.pos.y - thisv->ceilingPos.y;
        swayVecBase.x = Math_SinS(swayAngle) * temp;
        swayVecBase.y = Math_CosS(swayAngle) * temp;
        swayVecBase.z = 0.0f;
        Matrix_Push();
        Matrix_Translate(thisv->ceilingPos.x, thisv->ceilingPos.y, thisv->ceilingPos.z, MTXMODE_NEW);
        Matrix_RotateY(thisv->actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_MultVec3f(&swayVecBase, &swayVec);
        Matrix_Pop();
        thisv->actor.shape.rot.z = -(swayAngle * 2);
        thisv->actor.world.pos.x = swayVec.x;
        thisv->actor.world.pos.z = swayVec.z;
    }
}

void EnSsh_CheckBodyStickHit(EnSsh* thisv, GlobalContext* globalCtx) {
    ColliderInfo* info = &thisv->colCylinder[0].info;
    Player* player = GET_PLAYER(globalCtx);

    if (player->unk_860 != 0) {
        info->bumper.dmgFlags |= 2;
        thisv->colCylinder[1].info.bumper.dmgFlags &= ~2;
        thisv->colCylinder[2].info.bumper.dmgFlags &= ~2;
    } else {
        info->bumper.dmgFlags &= ~2;
        thisv->colCylinder[1].info.bumper.dmgFlags |= 2;
        thisv->colCylinder[2].info.bumper.dmgFlags |= 2;
    }
}

s32 EnSsh_CheckHitPlayer(EnSsh* thisv, GlobalContext* globalCtx) {
    s32 i;
    s32 hit = false;

    if ((thisv->hitCount == 0) && (thisv->spinTimer == 0)) {
        return false;
    }
    for (i = 0; i < 3; i++) {
        if (thisv->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) {
            thisv->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
            hit = true;
        }
    }
    if (!hit) {
        return false;
    }
    thisv->hitTimer = 30;
    if (thisv->swayTimer == 0) {
        thisv->spinTimer = thisv->hitTimer;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_ROLL);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_ST_ATTACK);
    globalCtx->damagePlayer(globalCtx, -8);
    func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.yawTowardsPlayer, 6.0f);
    thisv->hitCount--;
    return true;
}

s32 EnSsh_CheckHitFront(EnSsh* thisv) {
    u32 acFlags;

    if (thisv->colCylinder[2].base.acFlags) {} // Needed for matching
    acFlags = thisv->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        return 0;
    } else {
        thisv->colCylinder[2].base.acFlags &= ~AC_HIT;
        thisv->invincibilityTimer = 8;
        if ((thisv->swayTimer == 0) && (thisv->hitTimer == 0) && (thisv->stunTimer == 0)) {
            thisv->swayTimer = 60;
        }
        return 1;
    }
}

s32 EnSsh_CheckHitBack(EnSsh* thisv, GlobalContext* globalCtx) {
    ColliderCylinder* cyl = &thisv->colCylinder[0];
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    cyl = &thisv->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    if (!hit) {
        return false;
    }
    thisv->invincibilityTimer = 8;
    if (thisv->hitCount <= 0) {
        thisv->hitCount++;
    }
    if (thisv->stunTimer == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_ST_DAMAGE);
    }
    EnSsh_SetStunned(thisv);
    thisv->stateFlags |= SSH_STATE_STUNNED;
    return false;
}

s32 EnSsh_CollisionCheck(EnSsh* thisv, GlobalContext* globalCtx) {
    if (thisv->stunTimer == 0) {
        EnSsh_CheckHitPlayer(thisv, globalCtx);
    }
    if (EnSsh_CheckHitFront(thisv)) {
        return false;
    } else if (globalCtx->actorCtx.unk_02 != 0) {
        thisv->invincibilityTimer = 8;
        if (thisv->stunTimer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_ST_DAMAGE);
        }
        EnSsh_SetStunned(thisv);
        thisv->stateFlags |= SSH_STATE_STUNNED;
        return false;
    } else {
        return EnSsh_CheckHitBack(thisv, globalCtx);
        // Always returns false
    }
}

void EnSsh_SetBodyCylinderAC(EnSsh* thisv, GlobalContext* globalCtx) {
    Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[0]);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[0].base);
}

void EnSsh_SetLegsCylinderAC(EnSsh* thisv, GlobalContext* globalCtx) {
    s16 angleTowardsLink = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y));

    if (angleTowardsLink < 90 * (0x10000 / 360)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[2]);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[2].base);
    } else {
        Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[1]);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[1].base);
    }
}

s32 EnSsh_SetCylinderOC(EnSsh* thisv, GlobalContext* globalCtx) {
    Vec3f cyloffsets[] = {
        { 40.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { -40.0f, 0.0f, 0.0f },
    };
    Vec3f cylPos;
    s32 i;

    for (i = 0; i < 3; i++) {
        cylPos = thisv->actor.world.pos;
        cyloffsets[i].x *= thisv->colliderScale;
        cyloffsets[i].y *= thisv->colliderScale;
        cyloffsets[i].z *= thisv->colliderScale;
        Matrix_Push();
        Matrix_Translate(cylPos.x, cylPos.y, cylPos.z, MTXMODE_NEW);
        Matrix_RotateY((thisv->initialYaw / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_MultVec3f(&cyloffsets[i], &cylPos);
        Matrix_Pop();
        thisv->colCylinder[i + 3].dim.pos.x = cylPos.x;
        thisv->colCylinder[i + 3].dim.pos.y = cylPos.y;
        thisv->colCylinder[i + 3].dim.pos.z = cylPos.z;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[i + 3].base);
    }
    return 1;
}

void EnSsh_SetColliders(EnSsh* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.colChkInfo.health == 0) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colSph.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colSph.base);
    } else {
        if (thisv->hitTimer == 0) {
            EnSsh_SetCylinderOC(thisv, globalCtx);
        }
        if (DECR(thisv->invincibilityTimer) == 0) {
            EnSsh_SetBodyCylinderAC(thisv, globalCtx);
            EnSsh_SetLegsCylinderAC(thisv, globalCtx);
        }
    }
}

void EnSsh_Init(Actor* thisx, GlobalContext* globalCtx) {
    f32 frameCount;
    s32 pad;
    EnSsh* thisv = (EnSsh*)thisx;

    frameCount = Animation_GetLastFrame(&object_ssh_Anim_000304);
    if (thisv->actor.params == ENSSH_FATHER) {
        if (gSaveContext.inventory.gsTokens >= 100) {
            Actor_Kill(&thisv->actor);
            return;
        }
    } else if (gSaveContext.inventory.gsTokens >= (thisv->actor.params * 10)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_ssh_Skel_0052E0, NULL, thisv->jointTable, thisv->morphTable, 30);
    Animation_Change(&thisv->skelAnime, &object_ssh_Anim_000304, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP_INTERP, 0.0f);
    thisv->blureIdx = EnSsh_CreateBlureEffect(globalCtx);
    EnSsh_InitColliders(thisv, globalCtx);
    thisv->stateFlags = 0;
    thisv->hitCount = 0;
    EnSsh_CheckCeilingPos(thisv, globalCtx);
    if (thisv->actor.params != ENSSH_FATHER) {
        EnSsh_SetColliderScale(thisv, 0.5f, 1.0f);
    } else {
        EnSsh_SetColliderScale(thisv, 0.75f, 1.0f);
    }
    thisv->actor.gravity = 0.0f;
    thisv->initialYaw = thisv->actor.world.rot.y;
    EnSsh_SetupAction(thisv, EnSsh_Start);
}

void EnSsh_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSsh* thisv = (EnSsh*)thisx;
    s32 i;

    Effect_Delete(globalCtx, thisv->blureIdx);
    for (i = 0; i < 6; i++) {
        Collider_DestroyCylinder(globalCtx, &thisv->colCylinder[i]);
    }
    Collider_DestroyJntSph(globalCtx, &thisv->colSph);
}

void EnSsh_Wait(EnSsh* thisv, GlobalContext* globalCtx) {
    if (EnSsh_IsCloseToLink(thisv, globalCtx)) {
        EnSsh_SetDropAnimation(thisv);
        EnSsh_SetupAction(thisv, EnSsh_Drop);
    } else {
        EnSsh_Bob(thisv, globalCtx);
    }
}

void EnSsh_Talk(EnSsh* thisv, GlobalContext* globalCtx) {
    EnSsh_Bob(thisv, globalCtx);
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnSsh_Idle;
    }
}

void EnSsh_Idle(EnSsh* thisv, GlobalContext* globalCtx) {
    if (1) {}
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnSsh_Talk;
        if (thisv->actor.params == ENSSH_FATHER) {
            gSaveContext.eventChkInf[9] |= 0x40;
        }
        if ((thisv->actor.textId == 0x26) || (thisv->actor.textId == 0x27)) {
            gSaveContext.infTable[25] |= 0x40;
        }
        if ((thisv->actor.textId == 0x24) || (thisv->actor.textId == 0x25)) {
            gSaveContext.infTable[25] |= 0x80;
        }
    } else {
        if ((thisv->unkTimer != 0) && (DECR(thisv->unkTimer) == 0)) {
            EnSsh_SetAnimation(thisv, SSH_ANIM_WAIT);
        }
        if ((thisv->animTimer != 0) && (DECR(thisv->animTimer) == 0)) {
            EnSsh_SetAnimation(thisv, SSH_ANIM_WAIT);
        }
        if (!EnSsh_IsCloseToLink(thisv, globalCtx)) {
            EnSsh_SetReturnAnimation(thisv);
            EnSsh_SetupAction(thisv, EnSsh_Return);
        } else {
            if (DECR(thisv->sfxTimer) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_LAUGH);
                thisv->sfxTimer = 64;
            }
            EnSsh_Bob(thisv, globalCtx);
            if ((thisv->unkTimer == 0) && (thisv->animTimer == 0)) {
                thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0xD);
                if (thisv->actor.textId == 0) {
                    if (thisv->actor.params == ENSSH_FATHER) {
                        if (gSaveContext.inventory.gsTokens >= 50) {
                            thisv->actor.textId = 0x29;
                        } else if (gSaveContext.inventory.gsTokens >= 10) {
                            if (gSaveContext.infTable[25] & 0x80) {
                                thisv->actor.textId = 0x24;
                            } else {
                                thisv->actor.textId = 0x25;
                            }
                        } else {
                            if (gSaveContext.infTable[25] & 0x40) {
                                thisv->actor.textId = 0x27;
                            } else {
                                thisv->actor.textId = 0x26;
                            }
                        }
                    } else {
                        thisv->actor.textId = 0x22;
                    }
                }
                func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
            }
        }
    }
}

void EnSsh_Land(EnSsh* thisv, GlobalContext* globalCtx) {
    if ((thisv->unkTimer != 0) && (DECR(thisv->unkTimer) == 0)) {
        EnSsh_SetAnimation(thisv, SSH_ANIM_WAIT);
    }
    if ((thisv->animTimer != 0) && (DECR(thisv->animTimer) == 0)) {
        EnSsh_SetAnimation(thisv, SSH_ANIM_WAIT);
    }
    if ((thisv->actor.floorHeight + thisv->floorHeightOffset) <= thisv->actor.world.pos.y) {
        EnSsh_SetupAction(thisv, EnSsh_Idle);
    } else {
        Math_SmoothStepToF(&thisv->actor.velocity.y, 2.0f, 0.6f, 1000.0f, 0.0f);
    }
}

void EnSsh_Drop(EnSsh* thisv, GlobalContext* globalCtx) {
    if ((thisv->unkTimer != 0) && (DECR(thisv->unkTimer) == 0)) {
        EnSsh_SetAnimation(thisv, SSH_ANIM_DROP);
    }
    if (!EnSsh_IsCloseToLink(thisv, globalCtx)) {
        EnSsh_SetReturnAnimation(thisv);
        EnSsh_SetupAction(thisv, EnSsh_Return);
    } else if (EnSsh_IsCloseToGround(thisv)) {
        EnSsh_SpawnShockwave(thisv, globalCtx);
        EnSsh_SetLandAnimation(thisv);
        EnSsh_SetupAction(thisv, EnSsh_Land);
    } else if (DECR(thisv->sfxTimer) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_DOWN);
        thisv->sfxTimer = 3;
    }
}

void EnSsh_Return(EnSsh* thisv, GlobalContext* globalCtx) {
    f32 frameRatio = thisv->skelAnime.curFrame / (thisv->skelAnime.animLength - 1.0f);

    if (frameRatio == 1.0f) {
        EnSsh_SetReturnAnimation(thisv);
    }
    if (EnSsh_IsCloseToLink(thisv, globalCtx)) {
        EnSsh_SetDropAnimation(thisv);
        EnSsh_SetupAction(thisv, EnSsh_Drop);
    } else if (EnSsh_IsCloseToHome(thisv)) {
        EnSsh_SetWaitAnimation(thisv);
        EnSsh_SetupAction(thisv, EnSsh_Wait);
    } else {
        thisv->actor.velocity.y = 4.0f * frameRatio;
    }
}

void EnSsh_UpdateColliderScale(EnSsh* thisv) {
    if (thisv->stateFlags & SSH_STATE_SPIN) {
        if (thisv->spinTimer == 0) {
            thisv->stateFlags &= ~SSH_STATE_SPIN;
            if (thisv->actor.params != ENSSH_FATHER) {
                EnSsh_SetColliderScale(thisv, 0.5f, 1.0f);
            } else {
                EnSsh_SetColliderScale(thisv, 0.75f, 1.0f);
            }
        }
    } else {
        if (thisv->spinTimer != 0) {
            thisv->stateFlags |= SSH_STATE_SPIN;
            if (thisv->actor.params != ENSSH_FATHER) {
                EnSsh_SetColliderScale(thisv, 0.5f, 2.0f);
            } else {
                EnSsh_SetColliderScale(thisv, 0.75f, 2.0f);
            }
        }
    }
}

void EnSsh_Start(EnSsh* thisv, GlobalContext* globalCtx) {
    if (!EnSsh_IsCloseToGround(thisv)) {
        EnSsh_SetupAction(thisv, EnSsh_Wait);
        EnSsh_Wait(thisv, globalCtx);
    } else {
        EnSsh_SetLandAnimation(thisv);
        thisv->stateFlags |= 4;
        EnSsh_SetupAction(thisv, EnSsh_Land);
        EnSsh_Land(thisv, globalCtx);
    }
}

void EnSsh_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSsh* thisv = (EnSsh*)thisx;

    EnSsh_UpdateColliderScale(thisv);
    if (EnSsh_CollisionCheck(thisv, globalCtx)) {
        return; // EnSsh_CollisionCheck always returns false, so thisv never happens
    }
    if (thisv->stunTimer != 0) {
        EnSsh_Damaged(thisv);
    } else {
        SkelAnime_Update(&thisv->skelAnime);
        func_8002D7EC(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
        thisv->actionFunc(thisv, globalCtx);
    }
    EnSsh_UpdateYaw(thisv, globalCtx);
    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }
    thisv->blinkState = thisv->blinkTimer;
    if (thisv->blinkState >= 3) {
        thisv->blinkState = 0;
    }
    EnSsh_SetColliders(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 0.0f);
}

s32 EnSsh_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSsh* thisv = (EnSsh*)thisx;

    switch (limbIndex) {
        case 1:
            if ((thisv->spinTimer != 0) && (thisv->swayTimer == 0)) {
                if (thisv->spinTimer >= 2) {
                    EnSsh_AddBlureVertex(thisv);
                } else {
                    EnSsh_AddBlureSpace(thisv);
                }
            }
            break;
        case 4:
            if (thisv->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_0046C0;
            }
            break;
        case 5:
            if (thisv->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004080;
            }
            break;
        case 8:
            if (thisv->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004DE8;
            }
            break;
    }
    return false;
}

void EnSsh_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSsh* thisv = (EnSsh*)thisx;

    Collider_UpdateSpheres(limbIndex, &thisv->colSph);
}

void EnSsh_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* blinkTex[] = {
        object_ssh_Tex_0007E0,
        object_ssh_Tex_000C60,
        object_ssh_Tex_001060,
    };
    s32 pad;
    EnSsh* thisv = (EnSsh*)thisx;

    EnSsh_CheckBodyStickHit(thisv, globalCtx);
    EnSsh_Sway(thisv);
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ssh.c", 2333);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(blinkTex[thisv->blinkState]));
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ssh.c", 2336);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnSsh_OverrideLimbDraw,
                      EnSsh_PostLimbDraw, &thisv->actor);
}
