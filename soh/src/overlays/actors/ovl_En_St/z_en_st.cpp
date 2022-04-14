/*
 * File: z_en_st.c
 * Overlay: ovl_En_St
 * Description: Skulltula (normal, big, invisible)
 */

#include "z_en_st.h"
#include "objects/object_st/object_st.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnSt_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSt_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSt_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSt_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnSt_ReturnToCeiling(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_MoveToGround(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_StartOnCeilingOrGround(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_WaitOnGround(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_Die(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_BounceAround(EnSt* thisv, GlobalContext* globalCtx);
void EnSt_FinishBouncing(EnSt* thisv, GlobalContext* globalCtx);

#include "overlays/ovl_En_St/ovl_En_St.h"

const ActorInit En_St_InitVars = {
    ACTOR_EN_ST,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSt),
    (ActorFunc)EnSt_Init,
    (ActorFunc)EnSt_Destroy,
    (ActorFunc)EnSt_Update,
    (ActorFunc)EnSt_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
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

static CollisionCheckInfoInit2 sColChkInit = { 2, 0, 0, 0, MASS_IMMOVABLE };

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
    1,
    sJntSphElementsInit,
};

typedef enum {
    /* 0 */ ENST_ANIM_0,
    /* 1 */ ENST_ANIM_1,
    /* 2 */ ENST_ANIM_2,
    /* 3 */ ENST_ANIM_3,
    /* 4 */ ENST_ANIM_4,
    /* 5 */ ENST_ANIM_5,
    /* 6 */ ENST_ANIM_6,
    /* 7 */ ENST_ANIM_7
} EnStAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, 0.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 4.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 8.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_000304, 6.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_005B98, 2.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
};

void EnSt_SetupAction(EnSt* thisv, EnStActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

/**
 * Spawns `dustCnt` dust particles in a random pattern around the Skulltula
 */
void EnSt_SpawnDust(EnSt* thisv, GlobalContext* globalCtx, s32 dustCnt) {
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 0 };
    Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.3f, 0.0f };
    Vec3f dustPos;
    s16 yAngle;
    s32 i;

    yAngle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    dustPos.y = thisv->actor.floorHeight;
    for (i = dustCnt; i >= 0; i--, yAngle += (s16)(0x10000 / dustCnt)) {
        dustAccel.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
        dustAccel.z = (Rand_ZeroOne() - 0.5f) * 4.0f;
        dustPos.x = thisv->actor.world.pos.x + (Math_SinS(yAngle) * 22.0f);
        dustPos.z = thisv->actor.world.pos.z + (Math_CosS(yAngle) * 22.0f);
        func_8002836C(globalCtx, &dustPos, &dustVel, &dustAccel, &primColor, &envColor, 120, 40, 10);
    }
}

void EnSt_SpawnBlastEffect(EnSt* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f blastPos;

    blastPos.x = thisv->actor.world.pos.x;
    blastPos.y = thisv->actor.floorHeight;
    blastPos.z = thisv->actor.world.pos.z;

    EffectSsBlast_SpawnWhiteCustomScale(globalCtx, &blastPos, &zeroVec, &zeroVec, 100, 220, 8);
}

void EnSt_SpawnDeadEffect(EnSt* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f firePos;

    firePos.x = thisv->actor.world.pos.x + ((Rand_ZeroOne() - 0.5f) * 60.0f);
    firePos.y = (thisv->actor.world.pos.y + 10.0f) + ((Rand_ZeroOne() - 0.5f) * 45.0f);
    firePos.z = thisv->actor.world.pos.z + ((Rand_ZeroOne() - 0.5f) * 60.0f);
    EffectSsDeadDb_Spawn(globalCtx, &firePos, &zeroVec, &zeroVec, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
}

s32 EnSt_CreateBlureEffect(GlobalContext* globalCtx) {
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

/**
 * Checks for the position of the ceiling above the Skulltula.
 * If no ceiling is found it is set to 1000 units above the Skulltula
 */
s32 EnSt_CheckCeilingPos(EnSt* thisv, GlobalContext* globalCtx) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f checkPos;

    checkPos.x = thisv->actor.world.pos.x;
    checkPos.y = thisv->actor.world.pos.y + 1000.0f;
    checkPos.z = thisv->actor.world.pos.z;
    if (!BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &checkPos, &thisv->ceilingPos, &poly, false,
                                 false, true, true, &bgId)) {
        return false;
    }
    thisv->unusedPos = thisv->actor.world.pos;
    thisv->unusedPos.y -= 100.0f;
    return true;
}

void EnSt_AddBlurVertex(EnSt* thisv) {
    Vec3f v1 = { 834.0f, 834.0f, 0.0f };
    Vec3f v2 = { 834.0f, -584.0f, 0.0f };
    Vec3f v1Pos;
    Vec3f v2Pos;

    v1.x *= thisv->colliderScale;
    v1.y *= thisv->colliderScale;
    v1.z *= thisv->colliderScale;

    v2.x *= thisv->colliderScale;
    v2.y *= thisv->colliderScale;
    v2.z *= thisv->colliderScale;

    Matrix_Push();
    Matrix_MultVec3f(&v1, &v1Pos);
    Matrix_MultVec3f(&v2, &v2Pos);
    Matrix_Pop();
    EffectBlure_AddVertex(Effect_GetByIndex(thisv->blureIdx), &v1Pos, &v2Pos);
}

void EnSt_AddBlurSpace(EnSt* thisv) {
    EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIdx));
}

void EnSt_SetWaitingAnimation(EnSt* thisv) {
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
}

void EnSt_SetReturnToCeilingAnimation(EnSt* thisv) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_UP);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_2);
}

void EnSt_SetLandAnimation(EnSt* thisv) {
    thisv->actor.world.pos.y = thisv->actor.floorHeight + thisv->floorHeightOffset;
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_4);
    thisv->sfxTimer = 0;
    thisv->animFrames = thisv->skelAnime.animLength;
}

void EnSt_SetDropAnimAndVel(EnSt* thisv) {
    if (thisv->takeDamageSpinTimer == 0) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_4);
        thisv->animFrames = thisv->skelAnime.animLength;
    }
    thisv->sfxTimer = 0;
    thisv->actor.velocity.y = -10.0f;
}

/**
 * Initalizes the Skulltula's 6 cylinders, and sphere collider.
 */
void EnSt_InitColliders(EnSt* thisv, GlobalContext* globalCtx) {
    ColliderCylinderInit* cylinders[6] = {
        &sCylinderInit, &sCylinderInit, &sCylinderInit, &sCylinderInit2, &sCylinderInit2, &sCylinderInit2,
    };

    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        Collider_InitCylinder(globalCtx, &thisv->colCylinder[i]);
        Collider_SetCylinder(globalCtx, &thisv->colCylinder[i], &thisv->actor, cylinders[i]);
    }

    thisv->colCylinder[0].info.bumper.dmgFlags = 0x0003F8F9;
    thisv->colCylinder[1].info.bumper.dmgFlags = 0xFFC00706;
    thisv->colCylinder[2].base.colType = COLTYPE_METAL;
    thisv->colCylinder[2].info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO;
    thisv->colCylinder[2].info.elemType = ELEMTYPE_UNK2;
    thisv->colCylinder[2].info.bumper.dmgFlags = 0xFFCC0706;

    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(2), &sColChkInit);

    Collider_InitJntSph(globalCtx, &thisv->colSph);
    Collider_SetJntSph(globalCtx, &thisv->colSph, &thisv->actor, &sJntSphInit, thisv->colSphItems);
}

void EnSt_CheckBodyStickHit(EnSt* thisv, GlobalContext* globalCtx) {
    ColliderInfo* body = &thisv->colCylinder[0].info;
    Player* player = GET_PLAYER(globalCtx);

    if (player->unk_860 != 0) {
        body->bumper.dmgFlags |= 2;
        thisv->colCylinder[1].info.bumper.dmgFlags &= ~2;
        thisv->colCylinder[2].info.bumper.dmgFlags &= ~2;
    } else {
        body->bumper.dmgFlags &= ~2;
        thisv->colCylinder[1].info.bumper.dmgFlags |= 2;
        thisv->colCylinder[2].info.bumper.dmgFlags |= 2;
    }
}

void EnSt_SetBodyCylinderAC(EnSt* thisv, GlobalContext* globalCtx) {
    Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[0]);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[0].base);
}

void EnSt_SetLegsCylinderAC(EnSt* thisv, GlobalContext* globalCtx) {
    s16 angleTowardsLink = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y));

    if (angleTowardsLink < 0x3FFC) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[2]);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[2].base);
    } else {
        Collider_UpdateCylinder(&thisv->actor, &thisv->colCylinder[1]);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[1].base);
    }
}

s32 EnSt_SetCylinderOC(EnSt* thisv, GlobalContext* globalCtx) {
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
        Matrix_RotateY((thisv->initalYaw / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_MultVec3f(&cyloffsets[i], &cylPos);
        Matrix_Pop();
        thisv->colCylinder[i + 3].dim.pos.x = cylPos.x;
        thisv->colCylinder[i + 3].dim.pos.y = cylPos.y;
        thisv->colCylinder[i + 3].dim.pos.z = cylPos.z;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder[i + 3].base);
    }

    return true;
}

void EnSt_UpdateCylinders(EnSt* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.colChkInfo.health != 0) || (thisv->actionFunc == EnSt_FinishBouncing)) {
        if (DECR(thisv->gaveDamageSpinTimer) == 0) {
            EnSt_SetCylinderOC(thisv, globalCtx);
        }

        DECR(thisv->invulnerableTimer);
        DECR(thisv->takeDamageSpinTimer);

        if (thisv->invulnerableTimer == 0 && thisv->takeDamageSpinTimer == 0) {
            EnSt_SetBodyCylinderAC(thisv, globalCtx);
            EnSt_SetLegsCylinderAC(thisv, globalCtx);
        }
    }
}

s32 EnSt_CheckHitLink(EnSt* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 hit;
    s32 i;

    for (i = 0, hit = 0; i < 3; i++) {
        if (((thisv->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) != 0) == 0) {
            continue;
        }
        thisv->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
        hit = true;
    }

    if (!hit) {
        return false;
    }

    if (thisv->swayTimer == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_ROLL);
    }

    thisv->gaveDamageSpinTimer = 30;
    globalCtx->damagePlayer(globalCtx, -8);
    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
    func_8002F71C(globalCtx, &thisv->actor, 4.0f, thisv->actor.yawTowardsPlayer, 6.0f);
    return true;
}

s32 EnSt_CheckHitFrontside(EnSt* thisv) {
    u8 acFlags = thisv->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        // not hit
        return false;
    } else {
        thisv->colCylinder[2].base.acFlags &= ~AC_HIT;
        thisv->invulnerableTimer = 8;
        thisv->playSwayFlag = 0;
        thisv->swayTimer = 60;
        return true;
    }
}

s32 EnSt_CheckHitBackside(EnSt* thisv, GlobalContext* globalCtx) {
    ColliderCylinder* cyl = &thisv->colCylinder[0];
    s32 flags = 0; // ac hit flags from colliders 0 and 1
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->info.acHitInfo->toucher.dmgFlags;
    }

    cyl = &thisv->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->info.acHitInfo->toucher.dmgFlags;
    }

    if (!hit) {
        return false;
    }

    thisv->invulnerableTimer = 8;
    if (thisv->actor.colChkInfo.damageEffect == 1) {
        if (thisv->stunTimer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
            thisv->stunTimer = 120;
            Actor_SetColorFilter(&thisv->actor, 0, 0xC8, 0, thisv->stunTimer);
        }
        return false;
    }

    thisv->swayTimer = thisv->stunTimer = 0;
    thisv->gaveDamageSpinTimer = 1;
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
    thisv->takeDamageSpinTimer = thisv->skelAnime.animLength;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, thisv->takeDamageSpinTimer);
    if (Actor_ApplyDamage(&thisv->actor)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_DAMAGE);
        return false;
    }
    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->groundBounces = 3;
    thisv->deathTimer = 20;
    thisv->actor.gravity = -1.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALWALL_DEAD);

    if (flags & 0x1F820) {
        // arrow, fire arrow, ice arrow, light arrow,
        // and three unknows, unused arrows?
        EnSt_SetupAction(thisv, EnSt_Die);
        thisv->finishDeathTimer = 8;
    } else {
        EnSt_SetupAction(thisv, EnSt_BounceAround);
    }

    return true;
}

/**
 * Checks if the Skulltula's colliders have been hit, returns true if the hit has dealt damage to the Skulltula
 */
s32 EnSt_CheckColliders(EnSt* thisv, GlobalContext* globalCtx) {
    if (EnSt_CheckHitFrontside(thisv)) {
        // player has hit the front shield area of the Skulltula
        return false;
    }

    if (globalCtx->actorCtx.unk_02 != 0) {
        return true;
    }

    if (EnSt_CheckHitBackside(thisv, globalCtx)) {
        // player has hit the backside of the Skulltula
        return true;
    }

    if (thisv->stunTimer == 0 && thisv->takeDamageSpinTimer == 0) {
        // check if the Skulltula has hit link.
        EnSt_CheckHitLink(thisv, globalCtx);
    }
    return false;
}

void EnSt_SetColliderScale(EnSt* thisv) {
    f32 scaleAmount = 1.0f;
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    if (thisv->actor.params == 1) {
        scaleAmount = 1.4f;
    }

    radius = thisv->colSph.elements[0].dim.modelSphere.radius;
    radius *= scaleAmount;
    thisv->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = thisv->colCylinder[i].dim.yShift;
        radius = thisv->colCylinder[i].dim.radius;
        height = thisv->colCylinder[i].dim.height;
        yShift *= scaleAmount;
        radius *= scaleAmount;
        height *= scaleAmount;

        thisv->colCylinder[i].dim.yShift = yShift;
        thisv->colCylinder[i].dim.radius = radius;
        thisv->colCylinder[i].dim.height = height;
    }
    Actor_SetScale(&thisv->actor, 0.04f * scaleAmount);
    thisv->colliderScale = scaleAmount;
    thisv->floorHeightOffset = 32.0f * scaleAmount;
}

s32 EnSt_SetTeethColor(EnSt* thisv, s16 redTarget, s16 greenTarget, s16 blueTarget, s16 minMaxStep) {
    s16 red = thisv->teethR;
    s16 green = thisv->teethG;
    s16 blue = thisv->teethB;

    minMaxStep = 255 / (s16)(0.6f * minMaxStep);
    if (minMaxStep <= 0) {
        minMaxStep = 1;
    }

    Math_SmoothStepToS(&red, redTarget, 1, minMaxStep, minMaxStep);
    Math_SmoothStepToS(&green, greenTarget, 1, minMaxStep, minMaxStep);
    Math_SmoothStepToS(&blue, blueTarget, 1, minMaxStep, minMaxStep);
    thisv->teethR = red;
    thisv->teethG = green;
    thisv->teethB = blue;
    return 1;
}

s32 EnSt_DecrStunTimer(EnSt* thisv) {
    if (thisv->stunTimer == 0) {
        return 0;
    }
    thisv->stunTimer--; //! @bug  no return but v0 ends up being stunTimer before decrement
}

/**
 * Updates the yaw of the Skulltula, used for the shaking animation right before
 * turning, and the actual turning to face away from the player, and then back to
 * face the player
 */
void EnSt_UpdateYaw(EnSt* thisv, GlobalContext* globalCtx) {
    u16 yawDir = 0;
    Vec3s rot;
    s16 yawDiff;
    s16 timer;
    s16 yawTarget;

    // Shake towards the end of the stun.
    if (thisv->stunTimer != 0) {
        if (thisv->stunTimer < 30) {
            if ((thisv->stunTimer % 2) != 0) {
                thisv->actor.shape.rot.y += 0x800;
            } else {
                thisv->actor.shape.rot.y -= 0x800;
            }
        }
        return;
    }

    if (thisv->swayTimer == 0 && thisv->deathTimer == 0 && thisv->finishDeathTimer == 0) {
        // not swaying or dying
        if (thisv->takeDamageSpinTimer != 0 || thisv->gaveDamageSpinTimer != 0) {
            // Skulltula is doing a spinning animation
            thisv->actor.shape.rot.y += 0x2000;
            return;
        }

        if (thisv->actionFunc != EnSt_WaitOnGround) {
            // set the timers to turn away or turn towards the player
            thisv->rotAwayTimer = 30;
            thisv->rotTowardsTimer = 0;
        }

        if (thisv->rotAwayTimer != 0) {
            // turn away from the player
            thisv->rotAwayTimer--;
            if (thisv->rotAwayTimer == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_ROLL);
                thisv->rotTowardsTimer = 30;
            }
        } else if (thisv->rotTowardsTimer != 0) {
            // turn towards the player
            thisv->rotTowardsTimer--;
            if (thisv->rotTowardsTimer == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_ROLL);
                thisv->rotAwayTimer = 30;
            }
            yawDir = 0x8000;
        }

        // calculate the new yaw to or away from the player.
        rot = thisv->actor.shape.rot;
        yawTarget = (thisv->actionFunc == EnSt_WaitOnGround ? thisv->actor.yawTowardsPlayer : thisv->initalYaw);
        yawDiff = rot.y - (yawTarget ^ yawDir);
        if (ABS(yawDiff) <= 0x4000) {
            Math_SmoothStepToS(&rot.y, yawTarget ^ yawDir, 4, 0x2000, 1);
        } else {
            rot.y += 0x2000;
        }

        thisv->actor.shape.rot = thisv->actor.world.rot = rot;

        // Do the shaking animation.
        if (yawDir == 0 && thisv->rotAwayTimer < 0xA) {
            timer = thisv->rotAwayTimer;
        } else if (yawDir == 0x8000 && thisv->rotTowardsTimer < 0xA) {
            timer = thisv->rotTowardsTimer;
        } else {
            return;
        }

        if ((timer % 2) != 0) {
            thisv->actor.shape.rot.y += 0x800;
        } else {
            thisv->actor.shape.rot.y -= 0x800;
        }
    }
}

/**
 * Checks to see if the Skulltula is done bouncing on the ground,
 * spawns dust particles as the Skulltula hits the ground
 */
s32 EnSt_IsDoneBouncing(EnSt* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.velocity.y > 0.0f || thisv->groundBounces == 0) {
        // the Skulltula is moving upwards or the groundBounces is 0
        return false;
    }

    if (!(thisv->actor.bgCheckFlags & 1)) {
        // the Skulltula is not on the ground.
        return false;
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    EnSt_SpawnDust(thisv, globalCtx, 10);
    // creates an elastic bouncing effect, boucing up less for each hit on the ground.
    thisv->actor.velocity.y = 6.0f / (4 - thisv->groundBounces);
    thisv->groundBounces--;
    if (thisv->groundBounces != 0) {
        return false;
    } else {
        // make sure the Skulltula stays on the ground.
        thisv->actor.velocity.y = 0.0f;
    }
    return true;
}

void EnSt_Bob(EnSt* thisv, GlobalContext* globalCtx) {
    f32 ySpeedTarget = 0.5f;

    if ((globalCtx->state.frames & 8) != 0) {
        ySpeedTarget *= -1.0f;
    }
    Math_SmoothStepToF(&thisv->actor.velocity.y, ySpeedTarget, 0.4f, 1000.0f, 0.0f);
}

s32 EnSt_IsCloseToPlayer(EnSt* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 yDist;

    if (thisv->takeDamageSpinTimer != 0) {
        // skull is spinning from damage.
        return false;
    } else if (thisv->actor.xzDistToPlayer > 160.0f) {
        // player is more than 160 xz units from the Skulltula
        return false;
    }

    yDist = thisv->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        // player is above the Skulltula or more than 400 units below
        // the Skulltula
        return false;
    }

    if (player->actor.world.pos.y < thisv->actor.floorHeight) {
        // player is below the Skulltula's ground position
        return false;
    }
    return true;
}

s32 EnSt_IsCloseToInitalPos(EnSt* thisv) {
    f32 velY = thisv->actor.velocity.y;
    f32 checkY = thisv->actor.world.pos.y + (velY * 2.0f);

    if (checkY >= thisv->actor.home.pos.y) {
        return true;
    }
    return false;
}

s32 EnSt_IsCloseToGround(EnSt* thisv) {
    f32 velY = thisv->actor.velocity.y;
    f32 checkY = thisv->actor.world.pos.y + (velY * 2.0f);

    if (checkY - thisv->actor.floorHeight <= thisv->floorHeightOffset) {
        return true;
    }
    return false;
}

/**
 * Does the animation of the Skulltula swaying back and forth after the Skulltula
 * has been hit in the front by a sword
 */
void EnSt_Sway(EnSt* thisv) {
    Vec3f amtToTranslate;
    Vec3f translatedPos;
    f32 swayAmt;
    s16 rotAngle;

    if (thisv->swayTimer != 0) {

        thisv->swayAngle += 0xA28;
        thisv->swayTimer--;

        if (thisv->swayTimer == 0) {
            thisv->swayAngle = 0;
        }

        swayAmt = thisv->swayTimer * (7.0f / 15.0f);
        rotAngle = Math_SinS(thisv->swayAngle) * (swayAmt * (65536.0f / 360.0f));

        if (thisv->absPrevSwayAngle >= ABS(rotAngle) && thisv->playSwayFlag == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_WAVE);
            thisv->playSwayFlag = 1;
        }

        if (thisv->absPrevSwayAngle < ABS(rotAngle)) {
            thisv->playSwayFlag = 0;
        }

        thisv->absPrevSwayAngle = ABS(rotAngle);
        amtToTranslate.x = Math_SinS(rotAngle) * -200.0f;
        amtToTranslate.y = Math_CosS(rotAngle) * -200.0f;
        amtToTranslate.z = 0.0f;
        Matrix_Push();
        Matrix_Translate(thisv->ceilingPos.x, thisv->ceilingPos.y, thisv->ceilingPos.z, MTXMODE_NEW);
        Matrix_RotateY(thisv->actor.world.rot.y * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);
        Matrix_MultVec3f(&amtToTranslate, &translatedPos);
        Matrix_Pop();
        thisv->actor.shape.rot.z = -(rotAngle * 2);
        thisv->actor.world.pos.x = translatedPos.x;
        thisv->actor.world.pos.z = translatedPos.z;
    }
}

void EnSt_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSt* thisv = (EnSt*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 14.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_st_Skel_005298, NULL, thisv->jointTable, thisv->morphTable, 30);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_0);
    thisv->blureIdx = EnSt_CreateBlureEffect(globalCtx);
    EnSt_InitColliders(thisv, globalCtx);
    if (thisx->params == 2) {
        thisv->actor.flags |= ACTOR_FLAG_7;
    }
    if (thisv->actor.params == 1) {
        thisv->actor.naviEnemyId = 0x05;
    } else {
        thisv->actor.naviEnemyId = 0x04;
    }
    EnSt_CheckCeilingPos(thisv, globalCtx);
    thisv->actor.flags |= ACTOR_FLAG_14;
    thisv->actor.flags |= ACTOR_FLAG_24;
    EnSt_SetColliderScale(thisv);
    thisv->actor.gravity = 0.0f;
    thisv->initalYaw = thisv->actor.world.rot.y;
    EnSt_SetupAction(thisv, EnSt_StartOnCeilingOrGround);
}

void EnSt_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSt* thisv = (EnSt*)thisx;
    s32 i;

    Effect_Delete(globalCtx, thisv->blureIdx);
    for (i = 0; i < 6; i++) {
        Collider_DestroyCylinder(globalCtx, &thisv->colCylinder[i]);
    }
    Collider_DestroyJntSph(globalCtx, &thisv->colSph);
}

void EnSt_WaitOnCeiling(EnSt* thisv, GlobalContext* globalCtx) {
    if (EnSt_IsCloseToPlayer(thisv, globalCtx)) {
        EnSt_SetDropAnimAndVel(thisv);
        EnSt_SetupAction(thisv, EnSt_MoveToGround);
    } else {
        EnSt_Bob(thisv, globalCtx);
    }
}

/**
 * Skulltula is waiting on the ground for the player to move away, or for
 * a collider to have contact
 */
void EnSt_WaitOnGround(EnSt* thisv, GlobalContext* globalCtx) {
    if (thisv->takeDamageSpinTimer != 0) {
        thisv->takeDamageSpinTimer--;
        if (thisv->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (thisv->animFrames != 0) {
        thisv->animFrames--;
        if (thisv->animFrames == 0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (!EnSt_IsCloseToPlayer(thisv, globalCtx)) {
        // Player is no longer within range, return to ceiling.
        EnSt_SetReturnToCeilingAnimation(thisv);
        EnSt_SetupAction(thisv, EnSt_ReturnToCeiling);
        return;
    }

    if (DECR(thisv->sfxTimer) == 0) {
        // play the "laugh" sfx every 64 frames.
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_LAUGH);
        thisv->sfxTimer = 64;
    }

    // simply bob up and down.
    EnSt_Bob(thisv, globalCtx);
}

void EnSt_LandOnGround(EnSt* thisv, GlobalContext* globalCtx) {
    if (thisv->animFrames != 0) {
        thisv->animFrames--;
        if (thisv->animFrames == 0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (thisv->takeDamageSpinTimer != 0) {
        thisv->takeDamageSpinTimer--;
        if (thisv->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    thisv->sfxTimer++;
    if (thisv->sfxTimer == 14) {
        // play the sound effect of the Skulltula hitting the ground.
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_DOWN_SET);
    }

    if ((thisv->actor.floorHeight + thisv->floorHeightOffset) < thisv->actor.world.pos.y) {
        // the Skulltula has hit the ground.
        thisv->sfxTimer = 0;
        EnSt_SetupAction(thisv, EnSt_WaitOnGround);
    } else {
        Math_SmoothStepToF(&thisv->actor.velocity.y, 2.0f, 0.3f, 1.0f, 0.0f);
    }
}

void EnSt_MoveToGround(EnSt* thisv, GlobalContext* globalCtx) {
    if (thisv->takeDamageSpinTimer != 0) {
        thisv->takeDamageSpinTimer--;
        if (thisv->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENST_ANIM_5);
        }
    }

    if (!EnSt_IsCloseToPlayer(thisv, globalCtx)) {
        // the player moved out of range, return to the ceiling.
        EnSt_SetReturnToCeilingAnimation(thisv);
        EnSt_SetupAction(thisv, EnSt_ReturnToCeiling);
    } else if (EnSt_IsCloseToGround(thisv)) {
        // The Skulltula has become close to the ground.
        EnSt_SpawnBlastEffect(thisv, globalCtx);
        EnSt_SetLandAnimation(thisv);
        EnSt_SetupAction(thisv, EnSt_LandOnGround);
    } else if (DECR(thisv->sfxTimer) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_DOWN);
        thisv->sfxTimer = 3;
    }
}

void EnSt_ReturnToCeiling(EnSt* thisv, GlobalContext* globalCtx) {
    f32 animPctDone = thisv->skelAnime.curFrame / (thisv->skelAnime.animLength - 1.0f);

    if (animPctDone == 1.0f) {
        EnSt_SetReturnToCeilingAnimation(thisv);
    }

    if (EnSt_IsCloseToPlayer(thisv, globalCtx)) {
        // player came back into range
        EnSt_SetDropAnimAndVel(thisv);
        EnSt_SetupAction(thisv, EnSt_MoveToGround);
    } else if (EnSt_IsCloseToInitalPos(thisv)) {
        // the Skulltula is close to the initial postion.
        EnSt_SetWaitingAnimation(thisv);
        EnSt_SetupAction(thisv, EnSt_WaitOnCeiling);
    } else {
        // accelerate based on the current animation frame.
        thisv->actor.velocity.y = 4.0f * animPctDone;
    }
}

/**
 * The Skulltula has been killed, bounce around
 */
void EnSt_BounceAround(EnSt* thisv, GlobalContext* globalCtx) {
    thisv->actor.colorFilterTimer = thisv->deathTimer;
    func_8002D868(&thisv->actor);
    thisv->actor.world.rot.x += 0x800;
    thisv->actor.world.rot.z -= 0x800;
    thisv->actor.shape.rot = thisv->actor.world.rot;
    if (EnSt_IsDoneBouncing(thisv, globalCtx)) {
        thisv->actor.shape.yOffset = 400.0f;
        thisv->actor.speedXZ = 1.0f;
        thisv->actor.gravity = -2.0f;
        EnSt_SetupAction(thisv, EnSt_FinishBouncing);
    } else {
        Math_SmoothStepToF(&thisv->actor.shape.yOffset, 400.0f, 0.4f, 10000.0f, 0.0f);
    }
}

/**
 * Finish up the bouncing animation, and rotate towards the final position
 */
void EnSt_FinishBouncing(EnSt* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (DECR(thisv->deathTimer) == 0) {
        thisv->actor.velocity = zeroVec;
        thisv->finishDeathTimer = 8;
        EnSt_SetupAction(thisv, EnSt_Die);
        return;
    }

    if (DECR(thisv->setTargetYawTimer) == 0) {
        thisv->deathYawTarget = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        thisv->setTargetYawTimer = 8;
    }

    Math_SmoothStepToS(&thisv->actor.world.rot.x, 0x3FFC, 4, 0x2710, 1);
    Math_SmoothStepToS(&thisv->actor.world.rot.z, 0, 4, 0x2710, 1);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->deathYawTarget, 0xA, 0x2710, 1);

    thisv->actor.shape.rot = thisv->actor.world.rot;

    func_8002D868(&thisv->actor);
    thisv->groundBounces = 2;
    EnSt_IsDoneBouncing(thisv, globalCtx);
}

/**
 * Spawn the enemy dying effects, and drop a random item
 */
void EnSt_Die(EnSt* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->finishDeathTimer) != 0) {
        EnSt_SpawnDeadEffect(thisv, globalCtx);
    } else {
        Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0xE0);
        Actor_Kill(&thisv->actor);
    }
}

void EnSt_StartOnCeilingOrGround(EnSt* thisv, GlobalContext* globalCtx) {
    if (!EnSt_IsCloseToGround(thisv)) {
        thisv->rotAwayTimer = 60;
        EnSt_SetupAction(thisv, EnSt_WaitOnCeiling);
        EnSt_WaitOnCeiling(thisv, globalCtx);
    } else {
        EnSt_SetLandAnimation(thisv);
        EnSt_SetupAction(thisv, EnSt_LandOnGround);
        EnSt_LandOnGround(thisv, globalCtx);
    }
}

void EnSt_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSt* thisv = (EnSt*)thisx;
    s32 pad;
    Color_RGBA8 color = { 0, 0, 0, 0 };

    if (thisv->actor.flags & ACTOR_FLAG_15) {
        SkelAnime_Update(&thisv->skelAnime);
    } else if (!EnSt_CheckColliders(thisv, globalCtx)) {
        // no collision has been detected.

        if (thisv->stunTimer == 0) {
            SkelAnime_Update(&thisv->skelAnime);
        }

        if (thisv->swayTimer == 0 && thisv->stunTimer == 0) {
            func_8002D7EC(&thisv->actor);
        }

        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

        if ((thisv->stunTimer == 0) && (thisv->swayTimer == 0)) {
            // run the current action if the Skulltula isn't stunned
            // or swaying.
            thisv->actionFunc(thisv, globalCtx);
        } else if (thisv->stunTimer != 0) {
            // decrement the stun timer.
            EnSt_DecrStunTimer(thisv);
        } else {
            // sway the Skulltula.
            EnSt_Sway(thisv);
        }

        EnSt_UpdateYaw(thisv, globalCtx);

        if (thisv->actionFunc == EnSt_WaitOnGround) {
            if ((globalCtx->state.frames & 0x10) != 0) {
                color.r = 255;
            }
        }

        EnSt_SetTeethColor(thisv, color.r, color.g, color.b, 8);
        EnSt_UpdateCylinders(thisv, globalCtx);
        Actor_SetFocus(&thisv->actor, 0.0f);
    }
}

s32 EnSt_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dListP, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSt* thisv = (EnSt*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_st.c", 2260);
    switch (limbIndex) {
        case 1:
            if (thisv->gaveDamageSpinTimer != 0 && thisv->swayTimer == 0) {
                if (thisv->gaveDamageSpinTimer >= 2) {
                    EnSt_AddBlurVertex(thisv);
                } else {
                    EnSt_AddBlurSpace(thisv);
                }
            }
            break;
        case 4:
            // teeth
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, thisv->teethR, thisv->teethG, thisv->teethB, 0);
            break;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_st.c", 2295);
    return false;
}

void EnSt_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dListP, Vec3s* rot, void* thisx) {
    EnSt* thisv = (EnSt*)thisx;

    Collider_UpdateSpheres(limbIndex, &thisv->colSph);
}

void EnSt_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSt* thisv = (EnSt*)thisx;

    EnSt_CheckBodyStickHit(thisv, globalCtx);
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnSt_OverrideLimbDraw,
                      EnSt_PostLimbDraw, thisv);
}
