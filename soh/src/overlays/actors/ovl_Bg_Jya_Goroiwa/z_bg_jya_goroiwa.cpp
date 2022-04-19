/*
 *  File: z_bg_jya_goroiwa.c
 *  Overlay: ovl_Bg_Jya_Goroiwa
 *  Description: Rolling Boulder
 *  moves very slowly in some cases
 */

#include "z_bg_jya_goroiwa.h"
#include "objects/object_goroiwa/object_goroiwa.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaGoroiwa_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaGoroiwa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaGoroiwa_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaGoroiwa_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgJyaGoroiwa_Wait(BgJyaGoroiwa* thisv, GlobalContext* globalCtx);
void BgJyaGoroiwa_Move(BgJyaGoroiwa* thisv, GlobalContext* globalCtx);

void BgJyaGoroiwa_SetupWait(BgJyaGoroiwa* thisv);
void BgJyaGoroiwa_SetupMove(BgJyaGoroiwa* thisv);
void BgJyaGoroiwa_UpdateRotation(BgJyaGoroiwa* thisv);
void BgJyaGoroiwa_UpdateCollider(BgJyaGoroiwa* thisv);

ActorInit Bg_Jya_Goroiwa_InitVars = {
    ACTOR_BG_JYA_GOROIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GOROIWA,
    sizeof(BgJyaGoroiwa),
    (ActorFunc)BgJyaGoroiwa_Init,
    (ActorFunc)BgJyaGoroiwa_Destroy,
    (ActorFunc)BgJyaGoroiwa_Update,
    (ActorFunc)BgJyaGoroiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 0, MASS_HEAVY };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaGoroiwa_UpdateCollider(BgJyaGoroiwa* thisv) {
    Sphere16* worldSphere = &thisv->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = thisv->actor.world.pos.x;
    worldSphere->center.y = thisv->actor.world.pos.y + 59.5f;
    worldSphere->center.z = thisv->actor.world.pos.z;
}

void BgJyaGoroiwa_InitCollider(BgJyaGoroiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, &thisv->colliderItem);
    BgJyaGoroiwa_UpdateCollider(thisv);
    thisv->collider.elements[0].dim.worldSphere.radius = 58;
}

void BgJyaGoroiwa_UpdateRotation(BgJyaGoroiwa* thisv) {
    f32 xDiff = thisv->actor.world.pos.x - thisv->actor.prevPos.x;

    thisv->actor.shape.rot.z -= 0x10000 / (119 * std::numbers::pi_v<float>) * xDiff;
}

void BgJyaGoroiwa_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaGoroiwa* thisv = (BgJyaGoroiwa*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    BgJyaGoroiwa_InitCollider(thisv, globalCtx);
    thisv->actor.shape.rot.x = thisv->actor.shape.rot.y = thisv->actor.shape.rot.z = 0;
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&thisv->actor.shape, 595.0f, ActorShadow_DrawCircle, 9.0f);
    thisv->actor.shape.shadowAlpha = 128;
    BgJyaGoroiwa_SetupMove(thisv);
}

void BgJyaGoroiwa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaGoroiwa* thisv = (BgJyaGoroiwa*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BgJyaGoroiwa_SetupMove(BgJyaGoroiwa* thisv) {
    thisv->actionFunc = BgJyaGoroiwa_Move;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->hasHit = false;
    thisv->speedFactor = 1.0f;
}

void BgJyaGoroiwa_Move(BgJyaGoroiwa* thisv, GlobalContext* globalCtx) {
    Actor* thisx = &thisv->actor;
    s16 relYawTowardsPlayer;
    f32 speedXZsqBase = (-100.0f - thisx->world.pos.y) * 2.5f;
    f32 posYfac;

    if (speedXZsqBase < 0.01f) {
        speedXZsqBase = 0.01f;
    }

    thisx->speedXZ = sqrtf(speedXZsqBase) * thisv->speedFactor;
    thisx->velocity.x = Math_SinS(thisx->world.rot.y) * thisx->speedXZ;
    thisx->velocity.z = Math_CosS(thisx->world.rot.y) * thisx->speedXZ;

    thisx->world.pos.x = thisx->world.pos.x + thisx->velocity.x;
    thisx->world.pos.z = thisx->world.pos.z + thisx->velocity.z;

    if ((thisx->world.pos.x > 1466.0f) && (thisx->world.pos.x < 1673.0f)) {
        thisx->world.pos.y = -129.5f;
    } else {
        posYfac = 1569.0f - thisx->world.pos.x;
        posYfac = fabsf(posYfac) - 103.0f;
        thisx->world.pos.y = ((35.0f / 92.0f) * posYfac) - 129.5f;
    }

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT & ~AT_ON;

        relYawTowardsPlayer = thisx->yawTowardsPlayer - thisx->world.rot.y;
        if ((relYawTowardsPlayer > -0x4000) && (relYawTowardsPlayer < 0x4000)) {
            thisx->world.rot.y += 0x8000;
        }

        func_8002F6D4(globalCtx, thisx, 2.0f, thisx->yawTowardsPlayer, 0.0f, 0);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_PL_BODY_HIT);

        thisv->yOffsetSpeed = 10.0f;
        thisv->speedFactor = 0.5f;
        thisv->hasHit = true;
    }

    if (thisv->hasHit) {
        thisv->yOffsetSpeed -= 1.5f;
        thisx->shape.yOffset += thisv->yOffsetSpeed * 10.0f;
        if (thisx->shape.yOffset < 595.0f) {
            thisx->shape.yOffset = 595.0f;
            BgJyaGoroiwa_SetupWait(thisv);
        }
    } else {
        Math_StepToF(&thisv->speedFactor, 1.0f, 0.04f);
    }

    if (thisx->world.pos.x > 1745.0f) {
        thisx->world.rot.y = -0x4000;
    } else if (thisx->world.pos.x < 1393.0f) {
        thisx->world.rot.y = 0x4000;
    }

    Audio_PlayActorSound2(thisx, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

void BgJyaGoroiwa_SetupWait(BgJyaGoroiwa* thisv) {
    thisv->actionFunc = BgJyaGoroiwa_Wait;
    thisv->waitTimer = 0;
}

void BgJyaGoroiwa_Wait(BgJyaGoroiwa* thisv, GlobalContext* globalCtx) {
    thisv->waitTimer++;
    if (thisv->waitTimer > 60) {
        BgJyaGoroiwa_SetupMove(thisv);
        thisv->speedFactor = 0.1f;
    }
}

void BgJyaGoroiwa_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgJyaGoroiwa* thisv = (BgJyaGoroiwa*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 bgId;
    Vec3f pos;

    if (!(player->stateFlags1 & 0x300000C0)) {
        thisv->actionFunc(thisv, globalCtx);
        BgJyaGoroiwa_UpdateRotation(thisv);
        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + 59.5f;
        pos.z = thisv->actor.world.pos.z;
        thisv->actor.floorHeight =
            BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId, &thisv->actor, &pos);
        BgJyaGoroiwa_UpdateCollider(thisv);
        if (thisv->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgJyaGoroiwa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gRollingRockDL);
}
