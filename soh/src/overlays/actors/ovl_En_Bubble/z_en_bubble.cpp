#include "z_en_bubble.h"
#include "objects/object_bubble/object_bubble.h"

#define FLAGS ACTOR_FLAG_0

void EnBubble_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBubble_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBubble_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBubble_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBubble_Wait(EnBubble* thisv, GlobalContext* globalCtx);
void EnBubble_Pop(EnBubble* thisv, GlobalContext* globalCtx);
void EnBubble_Regrow(EnBubble* thisv, GlobalContext* globalCtx);

const ActorInit En_Bubble_InitVars = {
    ACTOR_EN_BUBBLE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BUBBLE,
    sizeof(EnBubble),
    (ActorFunc)EnBubble_Init,
    (ActorFunc)EnBubble_Destroy,
    (ActorFunc)EnBubble_Update,
    (ActorFunc)EnBubble_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x04 },
            { 0xFFCFD753, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    2,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    1, 2, 25, 25, MASS_IMMOVABLE,
};

static Vec3f sEffectAccel = { 0.0f, -0.5f, 0.0f };

static Color_RGBA8 sEffectPrimColor = { 255, 255, 255, 255 };

static Color_RGBA8 sEffectEnvColor = { 150, 150, 150, 0 };

void EnBubble_SetDimensions(EnBubble* thisv, f32 dim) {
    f32 a;
    f32 b;
    f32 c;
    f32 d;

    thisv->actor.flags |= ACTOR_FLAG_0;
    Actor_SetScale(&thisv->actor, 1.0f);
    thisv->actor.shape.yOffset = 16.0f;
    thisv->graphicRotSpeed = 16.0f;
    thisv->graphicEccentricity = 0.08f;
    thisv->expansionWidth = dim;
    thisv->expansionHeight = dim;
    a = Rand_ZeroOne();
    b = Rand_ZeroOne();
    c = Rand_ZeroOne();
    thisv->unk_218 = 1.0f;
    thisv->unk_21C = 1.0f;
    d = (a * a) + (b * b) + (c * c);
    thisv->unk_1FC.x = a / d;
    thisv->unk_1FC.y = b / d;
    thisv->unk_1FC.z = c / d;
}

u32 func_809CBCBC(EnBubble* thisv) {
    ColliderInfo* info = &thisv->colliderSphere.elements[0].info;

    info->toucher.dmgFlags = 0x8;
    info->toucher.effect = 0;
    info->toucher.damage = 4;
    info->toucherFlags = TOUCH_ON;
    thisv->actor.velocity.y = 0.0f;
    return 6;
}

// only called in an unused actionFunc
u32 func_809CBCEC(EnBubble* thisv) {
    EnBubble_SetDimensions(thisv, -1.0f);
    return 12;
}

void EnBubble_DamagePlayer(EnBubble* thisv, GlobalContext* globalCtx) {
    s32 damage = -thisv->colliderSphere.elements[0].info.toucher.damage;

    globalCtx->damagePlayer(globalCtx, damage);
    func_8002F7A0(globalCtx, &thisv->actor, 6.0f, thisv->actor.yawTowardsPlayer, 6.0f);
}

s32 EnBubble_Explosion(EnBubble* thisv, GlobalContext* globalCtx) {
    u32 i;
    Vec3f effectAccel;
    Vec3f effectVel;
    Vec3f effectPos;

    effectAccel = sEffectAccel;
    Math_SmoothStepToF(&thisv->expansionWidth, 4.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&thisv->expansionHeight, 4.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&thisv->graphicRotSpeed, 54.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&thisv->graphicEccentricity, 0.2f, 0.1f, 1000.0f, 0.0f);
    thisv->actor.shape.yOffset = ((thisv->expansionHeight + 1.0f) * 16.0f);

    if (DECR(thisv->explosionCountdown) != 0) {
        return -1;
    }
    effectPos.x = thisv->actor.world.pos.x;
    effectPos.y = thisv->actor.world.pos.y + thisv->actor.shape.yOffset;
    effectPos.z = thisv->actor.world.pos.z;
    for (i = 0; i < 20; i++) {
        effectVel.x = (Rand_ZeroOne() - 0.5f) * 7.0f;
        effectVel.y = Rand_ZeroOne() * 7.0f;
        effectVel.z = (Rand_ZeroOne() - 0.5f) * 7.0f;
        EffectSsDtBubble_SpawnCustomColor(globalCtx, &effectPos, &effectVel, &effectAccel, &sEffectPrimColor,
                                          &sEffectEnvColor, Rand_S16Offset(100, 50), 0x19, 0);
    }
    Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0x50);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    return Rand_S16Offset(90, 60);
}

// only called in an unused actionFunc
u32 func_809CBFD4(EnBubble* thisv) {
    if (DECR(thisv->explosionCountdown) != 0) {
        return -1;
    }
    return func_809CBCEC(thisv);
}

// only called in an unused actionFunc
s32 func_809CC020(EnBubble* thisv) {
    thisv->expansionWidth += 1.0f / 12.0f;
    thisv->expansionHeight += 1.0f / 12.0f;

    if (DECR(thisv->explosionCountdown) != 0) {
        return false;
    }
    return true;
}

void EnBubble_Vec3fNormalizedRelfect(Vec3f* vec1, Vec3f* vec2, Vec3f* ret) {
    f32 norm;

    Math3D_Vec3fReflect(vec1, vec2, ret);
    norm = sqrtf((ret->x * ret->x) + (ret->y * ret->y) + (ret->z * ret->z));
    if (norm != 0.0f) {
        ret->x /= norm;
        ret->y /= norm;
        ret->z /= norm;
    } else {
        ret->x = ret->y = ret->z = 0.0f;
    }
}

void EnBubble_Vec3fNormalize(Vec3f* vec) {
    f32 norm = sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));

    if (norm != 0.0f) {
        vec->x /= norm;
        vec->y /= norm;
        vec->z /= norm;
    } else {
        vec->x = vec->y = vec->z = 0.0f;
    }
}

void EnBubble_Fly(EnBubble* thisv, GlobalContext* globalCtx) {
    CollisionPoly* sp94;
    Actor* bumpActor;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 bounceSpeed;
    s32 bgId;
    u8 bounceCount;

    if (thisv->colliderSphere.elements[1].info.bumperFlags & BUMP_HIT) {
        bumpActor = thisv->colliderSphere.base.ac;
        thisv->normalizedBumpVelocity = bumpActor->velocity;
        EnBubble_Vec3fNormalize(&thisv->normalizedBumpVelocity);
        thisv->velocityFromBump.x += (thisv->normalizedBumpVelocity.x * 3.0f);
        thisv->velocityFromBump.y += (thisv->normalizedBumpVelocity.y * 3.0f);
        thisv->velocityFromBump.z += (thisv->normalizedBumpVelocity.z * 3.0f);
    }
    thisv->sinkSpeed -= 0.1f;
    if (thisv->sinkSpeed < thisv->actor.minVelocityY) {
        thisv->sinkSpeed = thisv->actor.minVelocityY;
    }
    sp54.x = thisv->velocityFromBounce.x + thisv->velocityFromBump.x;
    sp54.y = thisv->velocityFromBounce.y + thisv->velocityFromBump.y + thisv->sinkSpeed;
    sp54.z = thisv->velocityFromBounce.z + thisv->velocityFromBump.z;
    EnBubble_Vec3fNormalize(&sp54);

    sp78.x = thisv->actor.world.pos.x;
    sp78.y = thisv->actor.world.pos.y + thisv->actor.shape.yOffset;
    sp78.z = thisv->actor.world.pos.z;
    sp6C = sp78;

    sp6C.x += (sp54.x * 24.0f);
    sp6C.y += (sp54.y * 24.0f);
    sp6C.z += (sp54.z * 24.0f);
    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp78, &sp6C, &sp84, &sp94, true, true, true, false, &bgId)) {
        sp60.x = COLPOLY_GET_NORMAL(sp94->normal.x);
        sp60.y = COLPOLY_GET_NORMAL(sp94->normal.y);
        sp60.z = COLPOLY_GET_NORMAL(sp94->normal.z);
        EnBubble_Vec3fNormalizedRelfect(&sp54, &sp60, &sp54);
        thisv->bounceDirection = sp54;
        bounceCount = thisv->bounceCount;
        thisv->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(Rand_ZeroOne() * 10.0f)) {
            thisv->bounceCount = 0;
        }
        bounceSpeed = (thisv->bounceCount == 0) ? 3.6000001f : 3.0f;
        thisv->velocityFromBump.x = thisv->velocityFromBump.y = thisv->velocityFromBump.z = 0.0f;
        thisv->velocityFromBounce.x = (thisv->bounceDirection.x * bounceSpeed);
        thisv->velocityFromBounce.y = (thisv->bounceDirection.y * bounceSpeed);
        thisv->velocityFromBounce.z = (thisv->bounceDirection.z * bounceSpeed);
        thisv->sinkSpeed = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_AWA_BOUND);
        thisv->graphicRotSpeed = 128.0f;
        thisv->graphicEccentricity = 0.48f;
    } else if (thisv->actor.bgCheckFlags & 0x20 && sp54.y < 0.0f) {
        sp60.x = sp60.z = 0.0f;
        sp60.y = 1.0f;
        EnBubble_Vec3fNormalizedRelfect(&sp54, &sp60, &sp54);
        thisv->bounceDirection = sp54;
        bounceCount = thisv->bounceCount;
        thisv->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(Rand_ZeroOne() * 10.0f)) {
            thisv->bounceCount = 0;
        }
        bounceSpeed = (thisv->bounceCount == 0) ? 3.6000001f : 3.0f;
        thisv->velocityFromBump.x = thisv->velocityFromBump.y = thisv->velocityFromBump.z = 0.0f;
        thisv->velocityFromBounce.x = (thisv->bounceDirection.x * bounceSpeed);
        thisv->velocityFromBounce.y = (thisv->bounceDirection.y * bounceSpeed);
        thisv->velocityFromBounce.z = (thisv->bounceDirection.z * bounceSpeed);
        thisv->sinkSpeed = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_AWA_BOUND);
        thisv->graphicRotSpeed = 128.0f;
        thisv->graphicEccentricity = 0.48f;
    }
    thisv->actor.velocity.x = thisv->velocityFromBounce.x + thisv->velocityFromBump.x;
    thisv->actor.velocity.y = thisv->velocityFromBounce.y + thisv->velocityFromBump.y + thisv->sinkSpeed;
    thisv->actor.velocity.z = thisv->velocityFromBounce.z + thisv->velocityFromBump.z;
    Math_ApproachF(&thisv->velocityFromBump.x, 0.0f, 0.3f, 0.1f);
    Math_ApproachF(&thisv->velocityFromBump.y, 0.0f, 0.3f, 0.1f);
    Math_ApproachF(&thisv->velocityFromBump.z, 0.0f, 0.3f, 0.1f);
}

u32 func_809CC648(EnBubble* thisv) {
    if (((thisv->colliderSphere.base.acFlags & AC_HIT) != 0) == false) {
        return false;
    }
    thisv->colliderSphere.base.acFlags &= ~AC_HIT;
    if (thisv->colliderSphere.elements[1].info.bumperFlags & BUMP_HIT) {
        thisv->unk_1F0.x = thisv->colliderSphere.base.ac->velocity.x / 10.0f;
        thisv->unk_1F0.y = thisv->colliderSphere.base.ac->velocity.y / 10.0f;
        thisv->unk_1F0.z = thisv->colliderSphere.base.ac->velocity.z / 10.0f;
        thisv->graphicRotSpeed = 128.0f;
        thisv->graphicEccentricity = 0.48f;
        return false;
    }
    thisv->unk_208 = 8;
    return true;
}

u32 EnBubble_DetectPop(EnBubble* thisv, GlobalContext* globalCtx) {
    if (DECR(thisv->unk_208) != 0 || thisv->actionFunc == EnBubble_Pop) {
        return false;
    }
    if (thisv->colliderSphere.base.ocFlags2 & OC2_HIT_PLAYER) {
        thisv->colliderSphere.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        EnBubble_DamagePlayer(thisv, globalCtx);
        thisv->unk_208 = 8;
        return true;
    }
    return func_809CC648(thisv);
}

void func_809CC774(EnBubble* thisv) {
    ColliderJntSphElementDim* dim;
    Vec3f src;
    Vec3f dest;

    dim = &thisv->colliderSphere.elements[0].dim;
    src.x = dim->modelSphere.center.x;
    src.y = dim->modelSphere.center.y;
    src.z = dim->modelSphere.center.z;

    Matrix_MultVec3f(&src, &dest);
    dim->worldSphere.center.x = dest.x;
    dim->worldSphere.center.y = dest.y;
    dim->worldSphere.center.z = dest.z;
    dim->worldSphere.radius = dim->modelSphere.radius * (1.0f + thisv->expansionWidth);
    thisv->colliderSphere.elements[1].dim = *dim;
}

void EnBubble_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBubble* thisv = (EnBubble*)thisx;
    u32 pad;

    ActorShape_Init(&thisv->actor.shape, 16.0f, ActorShadow_DrawCircle, 0.2f);
    Collider_InitJntSph(globalCtx, &thisv->colliderSphere);
    Collider_SetJntSph(globalCtx, &thisv->colliderSphere, &thisv->actor, &sJntSphInit, thisv->colliderSphereItems);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(9), &sColChkInfoInit2);
    thisv->actor.naviEnemyId = 0x16;
    thisv->bounceDirection.x = Rand_ZeroOne();
    thisv->bounceDirection.y = Rand_ZeroOne();
    thisv->bounceDirection.z = Rand_ZeroOne();
    EnBubble_Vec3fNormalize(&thisv->bounceDirection);
    thisv->velocityFromBounce.x = thisv->bounceDirection.x * 3.0f;
    thisv->velocityFromBounce.y = thisv->bounceDirection.y * 3.0f;
    thisv->velocityFromBounce.z = thisv->bounceDirection.z * 3.0f;
    EnBubble_SetDimensions(thisv, 0.0f);
    thisv->actionFunc = EnBubble_Wait;
}

void EnBubble_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBubble* thisv = (EnBubble*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->colliderSphere);
}

void EnBubble_Wait(EnBubble* thisv, GlobalContext* globalCtx) {
    if (EnBubble_DetectPop(thisv, globalCtx)) {
        thisv->explosionCountdown = func_809CBCBC(thisv);
        thisv->actionFunc = EnBubble_Pop;
    } else {
        EnBubble_Fly(thisv, globalCtx);
        thisv->actor.shape.yOffset = ((thisv->expansionHeight + 1.0f) * 16.0f);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSphere.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSphere.base);
    }
}

void EnBubble_Pop(EnBubble* thisv, GlobalContext* globalCtx) {
    if (EnBubble_Explosion(thisv, globalCtx) >= 0) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 60, NA_SE_EN_AWA_BREAK);
        Actor_Kill(&thisv->actor);
    }
}

// unused
void EnBubble_Disappear(EnBubble* thisv, GlobalContext* globalCtx) {
    s32 temp_v0;

    temp_v0 = func_809CBFD4(thisv);
    if (temp_v0 >= 0) {
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        thisv->explosionCountdown = temp_v0;
        thisv->actionFunc = EnBubble_Regrow;
    }
}

// unused
void EnBubble_Regrow(EnBubble* thisv, GlobalContext* globalCtx) {
    if (func_809CC020(thisv)) {
        thisv->actionFunc = EnBubble_Wait;
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSphere.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSphere.base);
}

void EnBubble_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBubble* thisv = (EnBubble*)thisx;

    func_8002D7EC(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 16.0f, 16.0f, 0.0f, 7);
    thisv->actionFunc(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, thisv->actor.shape.yOffset);
}

void EnBubble_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBubble* thisv = (EnBubble*)thisx;
    u32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bubble.c", 1175);

    if (thisv->actionFunc != EnBubble_Disappear) {
        func_80093D84(globalCtx->state.gfxCtx);
        Math_SmoothStepToF(&thisv->graphicRotSpeed, 16.0f, 0.2f, 1000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->graphicEccentricity, 0.08f, 0.2f, 1000.0f, 0.0f);
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

        Matrix_Scale(thisv->expansionWidth + 1.0f, thisv->expansionHeight + 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZ(((f32)globalCtx->state.frames * (std::numbers::pi_v<float> / 180.0f)) * thisv->graphicRotSpeed, MTXMODE_APPLY);
        Matrix_Scale(thisv->graphicEccentricity + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZ((-(f32)globalCtx->state.frames * (std::numbers::pi_v<float> / 180.0f)) * thisv->graphicRotSpeed, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bubble.c", 1220),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gBubbleDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bubble.c", 1226);

    if (thisv->actionFunc != EnBubble_Disappear) {
        thisv->actor.shape.shadowScale = (f32)((thisv->expansionWidth + 1.0f) * 0.2f);
        func_809CC774(thisv);
    }
}
