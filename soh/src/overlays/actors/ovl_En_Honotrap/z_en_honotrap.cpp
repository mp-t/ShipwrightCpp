/**
 * File: z_en_honotrap.c
 * Overlay: ovl_En_Honotrap
 * Description: Fake eye switches and Dampe flames
 */

#include "z_en_honotrap.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS ACTOR_FLAG_4

#define HONOTRAP_AT_ACTIVE (1 << 0)
#define HONOTRAP_AC_ACTIVE (1 << 1)
#define HONOTRAP_OC_ACTIVE (1 << 2)

typedef enum {
    /* 0 */ HONOTRAP_EYE_OPEN,
    /* 1 */ HONOTRAP_EYE_HALF,
    /* 2 */ HONOTRAP_EYE_CLOSE,
    /* 3 */ HONOTRAP_EYE_SHUT
} EnHonotrapEyeState;

void EnHonotrap_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHonotrap_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHonotrap_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHonotrap_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnHonotrap_SetupEyeIdle(EnHonotrap* thisv);
void EnHonotrap_EyeIdle(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupEyeOpen(EnHonotrap* thisv);
void EnHonotrap_EyeOpen(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupEyeAttack(EnHonotrap* thisv);
void EnHonotrap_EyeAttack(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupEyeClose(EnHonotrap* thisv);
void EnHonotrap_EyeClose(EnHonotrap* thisv, GlobalContext* globalCtx);

void EnHonotrap_SetupFlame(EnHonotrap* thisv);
void EnHonotrap_Flame(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupFlameDrop(EnHonotrap* thisv);
void EnHonotrap_FlameDrop(EnHonotrap* thisv, GlobalContext* globalCtx);

void EnHonotrap_SetupFlameMove(EnHonotrap* thisv);
void EnHonotrap_FlameMove(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupFlameChase(EnHonotrap* thisv);
void EnHonotrap_FlameChase(EnHonotrap* thisv, GlobalContext* globalCtx);
void EnHonotrap_SetupFlameVanish(EnHonotrap* thisv);
void EnHonotrap_FlameVanish(EnHonotrap* thisv, GlobalContext* globalCtx);

const ActorInit En_Honotrap_InitVars = {
    ACTOR_EN_HONOTRAP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(EnHonotrap),
    (ActorFunc)EnHonotrap_Init,
    (ActorFunc)EnHonotrap_Destroy,
    (ActorFunc)EnHonotrap_Update,
    (ActorFunc)EnHonotrap_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00100000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 10, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 9, 23, 1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void EnHonotrap_FlameCollisionCheck(EnHonotrap* thisv, GlobalContext* globalCtx) {
    s32 pad[3];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider.cyl);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.cyl.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.cyl.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.cyl.base);
    thisv->colChkFlags |= HONOTRAP_AT_ACTIVE;
    thisv->colChkFlags |= HONOTRAP_AC_ACTIVE;
    thisv->colChkFlags |= HONOTRAP_OC_ACTIVE;
}

void EnHonotrap_GetNormal(Vec3f* normal, Vec3f* vec) {
    f32 mag = Math3D_Vec3fMagnitude(vec);

    if (mag < 0.001f) {
        osSyncPrintf("Warning : vector size zero (%s %d)\n", "../z_en_honotrap.c", 328, normal);

        normal->x = normal->y = 0.0f;
        normal->z = 1.0f;
    } else {
        normal->x = vec->x * (1.0f / mag);
        normal->y = vec->y * (1.0f / mag);
        normal->z = vec->z * (1.0f / mag);
    }
}

void EnHonotrap_InitEye(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHonotrap* thisv = (EnHonotrap*)thisx;
    s32 i;
    s32 j;
    Vec3f* vtx;
    Vec3f triangle[3];
    f32 cos;
    f32 sin;

    Actor_SetScale(thisx, 0.1f);
    sin = Math_SinS(thisx->home.rot.y);
    cos = Math_CosS(thisx->home.rot.y);
    Collider_InitTris(globalCtx, &thisv->collider.tris);
    Collider_SetTris(globalCtx, &thisv->collider.tris, thisx, &sTrisInit, thisv->collider.elements);

    for (i = 0; i < 2; i++) {
        for (j = 0, vtx = triangle; j < 3; j++, vtx++) {
            Vec3f* baseVtx = &sTrisInit.elements[i].dim.vtx[j];

            vtx->x = baseVtx->z * sin + baseVtx->x * cos;
            vtx->y = baseVtx->y;
            vtx->z = baseVtx->z * cos - baseVtx->x * sin;
            Math_Vec3f_Sum(vtx, &thisx->world.pos, vtx);
        }
        Collider_SetTrisVertices(&thisv->collider.tris, i, &triangle[0], &triangle[1], &triangle[2]);
    }
    EnHonotrap_SetupEyeIdle(thisv);
    Actor_SetFocus(thisx, 0.0f);
}

void EnHonotrap_InitFlame(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHonotrap* thisv = (EnHonotrap*)thisx;

    Actor_SetScale(&thisv->actor, 0.0001f);
    Collider_InitCylinder(globalCtx, &thisv->collider.cyl);
    Collider_SetCylinder(globalCtx, &thisv->collider.cyl, &thisv->actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider.cyl);
    thisv->actor.minVelocityY = -1.0f;
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->actor.shape.shadowAlpha = 128;
    thisv->targetPos = GET_PLAYER(globalCtx)->actor.world.pos;
    thisv->targetPos.y += 10.0f;
    thisv->flameScroll = Rand_ZeroOne() * 511.0f;
    EnHonotrap_SetupFlame(thisv);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FLAME_IGNITION);
    if (thisv->actor.params == HONOTRAP_FLAME_DROP) {
        thisv->actor.room = -1;
        thisv->collider.cyl.dim.radius = 12;
        thisv->collider.cyl.dim.height = 30;
        thisv->actor.shape.yOffset = -1000.0f;
    }
}

void EnHonotrap_Init(Actor* thisx, GlobalContext* globalCtx) {
    Actor_ProcessInitChain(thisx, sInitChain);
    if (thisx->params == HONOTRAP_EYE) {
        EnHonotrap_InitEye(thisx, globalCtx);
    } else {
        EnHonotrap_InitFlame(thisx, globalCtx);
    }
}

void EnHonotrap_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHonotrap* thisv = (EnHonotrap*)thisx;

    if (thisv->actor.params == HONOTRAP_EYE) {
        Collider_DestroyTris(globalCtx, &thisv->collider.tris);
    } else {
        Collider_DestroyCylinder(globalCtx, &thisv->collider.cyl);
    }
}

void EnHonotrap_SetupEyeIdle(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_EyeIdle;
    thisv->eyeState = HONOTRAP_EYE_SHUT;
}

void EnHonotrap_EyeIdle(EnHonotrap* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.child != NULL) {
        thisv->timer = 200;
    } else if ((thisv->timer <= 0) && (thisv->actor.xzDistToPlayer < 750.0f) && (0.0f > thisv->actor.yDistToPlayer) &&
               (thisv->actor.yDistToPlayer > -700.0f) &&
               (-0x4000 < (thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) &&
               ((thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y) < 0x4000)) {
        EnHonotrap_SetupEyeOpen(thisv);
    }
}

void EnHonotrap_SetupEyeOpen(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_EyeOpen;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0x28);
    thisv->timer = 30;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_RED_EYE);
}

void EnHonotrap_EyeOpen(EnHonotrap* thisv, GlobalContext* globalCtx) {
    f32 cos;
    f32 sin;

    thisv->eyeState--;
    if (thisv->eyeState <= HONOTRAP_EYE_OPEN) {
        EnHonotrap_SetupEyeAttack(thisv);
        sin = Math_SinS(thisv->actor.shape.rot.y);
        cos = Math_CosS(thisv->actor.shape.rot.y);
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_HONOTRAP,
                           (sin * 12.0f) + thisv->actor.home.pos.x, thisv->actor.home.pos.y - 10.0f,
                           (cos * 12.0f) + thisv->actor.home.pos.z, thisv->actor.home.rot.x, thisv->actor.home.rot.y,
                           thisv->actor.home.rot.z, HONOTRAP_FLAME_MOVE);
    }
}

void EnHonotrap_SetupEyeAttack(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_EyeAttack;
    thisv->eyeState = HONOTRAP_EYE_OPEN;
}

void EnHonotrap_EyeAttack(EnHonotrap* thisv, GlobalContext* globalCtx) {
    if (thisv->timer <= 0) {
        EnHonotrap_SetupEyeClose(thisv);
    }
}

void EnHonotrap_SetupEyeClose(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_EyeClose;
}

void EnHonotrap_EyeClose(EnHonotrap* thisv, GlobalContext* globalCtx) {
    thisv->eyeState++;
    if (thisv->eyeState >= HONOTRAP_EYE_SHUT) {
        EnHonotrap_SetupEyeIdle(thisv);
        thisv->timer = 200;
    }
}

void EnHonotrap_SetupFlame(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_Flame;
}

void EnHonotrap_Flame(EnHonotrap* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 ready =
        Math_StepToF(&thisv->actor.scale.x, (thisv->actor.params == HONOTRAP_FLAME_MOVE) ? 0.004f : 0.0048f, 0.0006f);

    thisv->actor.scale.z = thisv->actor.scale.y = thisv->actor.scale.x;
    if (ready) {
        if (thisv->actor.params == HONOTRAP_FLAME_MOVE) {
            EnHonotrap_SetupFlameMove(thisv);
        } else {
            EnHonotrap_SetupFlameDrop(thisv);
        }
    }
}

void EnHonotrap_SetupFlameDrop(EnHonotrap* thisv) {
    thisv->timer = 40;
    thisv->actor.velocity.y = 1.0f;
    thisv->actor.velocity.x = 2.0f * Math_SinS(thisv->actor.world.rot.y);
    thisv->actor.velocity.z = 2.0f * Math_CosS(thisv->actor.world.rot.y);
    thisv->actionFunc = EnHonotrap_FlameDrop;
}

void EnHonotrap_FlameDrop(EnHonotrap* thisv, GlobalContext* globalCtx) {
    if ((thisv->collider.cyl.base.atFlags & AT_HIT) || (thisv->timer <= 0)) {
        if ((thisv->collider.cyl.base.atFlags & AT_HIT) && !(thisv->collider.cyl.base.atFlags & AT_BOUNCED)) {
            func_8002F71C(globalCtx, &thisv->actor, 5.0f, thisv->actor.yawTowardsPlayer, 0.0f);
        }
        thisv->actor.velocity.x = thisv->actor.velocity.y = thisv->actor.velocity.z = 0.0f;
        EnHonotrap_SetupFlameVanish(thisv);
    } else {
        if (thisv->actor.velocity.y > 0.0f) {
            thisv->actor.world.pos.x += thisv->actor.velocity.x;
            thisv->actor.world.pos.z += thisv->actor.velocity.z;
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.0f, 12.0f, 0.0f, 5);
        }
        if (!Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 1.0f, thisv->actor.velocity.y)) {
            thisv->actor.velocity.y += 1.0f;
        } else {
            thisv->actor.velocity.y = 0.0f;
        }
        EnHonotrap_FlameCollisionCheck(thisv, globalCtx);
    }
}

void EnHonotrap_SetupFlameMove(EnHonotrap* thisv) {
    f32 distFrac;

    thisv->actionFunc = EnHonotrap_FlameMove;

    distFrac = 1.0f / (Actor_WorldDistXYZToPoint(&thisv->actor, &thisv->targetPos) + 1.0f);
    thisv->actor.velocity.x = (thisv->targetPos.x - thisv->actor.world.pos.x) * distFrac;
    thisv->actor.velocity.y = (thisv->targetPos.y - thisv->actor.world.pos.y) * distFrac;
    thisv->actor.velocity.z = (thisv->targetPos.z - thisv->actor.world.pos.z) * distFrac;
    thisv->speedMod = 0.0f;

    thisv->timer = 160;
}

void EnHonotrap_FlameMove(EnHonotrap* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f speed;
    s32 ready;

    Math_StepToF(&thisv->speedMod, 13.0f, 0.5f);
    speed.x = fabsf(thisv->speedMod * thisv->actor.velocity.x);
    speed.y = fabsf(thisv->speedMod * thisv->actor.velocity.y);
    speed.z = fabsf(thisv->speedMod * thisv->actor.velocity.z);
    ready = true;
    ready &= Math_StepToF(&thisv->actor.world.pos.x, thisv->targetPos.x, speed.x);
    ready &= Math_StepToF(&thisv->actor.world.pos.y, thisv->targetPos.y, speed.y);
    ready &= Math_StepToF(&thisv->actor.world.pos.z, thisv->targetPos.z, speed.z);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.0f, 10.0f, 0.0f, 0x1D);

    if (thisv->collider.tris.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(globalCtx);
        Vec3f shieldNorm;
        Vec3f tempVel;
        Vec3f shieldVec;

        shieldVec.x = -player->shieldMf.xz;
        shieldVec.y = -player->shieldMf.yz;
        shieldVec.z = -player->shieldMf.zz;
        EnHonotrap_GetNormal(&shieldNorm, &shieldVec);

        tempVel = thisv->actor.velocity;
        Math3D_Vec3fReflect(&tempVel, &shieldNorm, &thisv->actor.velocity);
        thisv->actor.speedXZ = thisv->speedMod * 0.5f;
        thisv->actor.world.rot.y = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
        EnHonotrap_SetupFlameVanish(thisv);
    } else if (thisv->collider.tris.base.atFlags & AT_HIT) {
        thisv->actor.velocity.y = thisv->actor.speedXZ = 0.0f;
        EnHonotrap_SetupFlameVanish(thisv);
    } else if (thisv->timer <= 0) {
        EnHonotrap_SetupFlameVanish(thisv);
    } else {
        EnHonotrap_FlameCollisionCheck(thisv, globalCtx);
        if (ready) {
            EnHonotrap_SetupFlameChase(thisv);
        }
    }
}

void EnHonotrap_SetupFlameChase(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_FlameChase;

    thisv->actor.velocity.x = thisv->actor.velocity.y = thisv->actor.velocity.z = thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.x = thisv->actor.world.rot.y = thisv->actor.world.rot.z = 0;

    thisv->timer = 100;
}

void EnHonotrap_FlameChase(EnHonotrap* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0x300);
    Math_StepToF(&thisv->actor.speedXZ, 3.0f, 0.1f);
    thisv->actor.gravity = (-thisv->actor.yDistToPlayer < 10.0f) ? 0.08f : -0.08f;
    func_8002D868(&thisv->actor);
    if (thisv->actor.velocity.y > 1.0f) {
        thisv->actor.velocity.y = 1.0f;
    }
    thisv->actor.velocity.y *= 0.95f;
    func_8002D7EC(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.0f, 10.0f, 0.0f, 0x1D);
    if (thisv->collider.cyl.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(globalCtx);
        Vec3s shieldRot;

        Matrix_MtxFToYXZRotS(&player->shieldMf, &shieldRot, false);
        thisv->actor.world.rot.y = ((shieldRot.y * 2) - thisv->actor.world.rot.y) + 0x8000;
        EnHonotrap_SetupFlameVanish(thisv);
    } else if (thisv->collider.cyl.base.atFlags & AT_HIT) {
        thisv->actor.speedXZ *= 0.1f;
        thisv->actor.velocity.y *= 0.1f;
        EnHonotrap_SetupFlameVanish(thisv);
    } else if ((thisv->actor.bgCheckFlags & 8) || (thisv->timer <= 0)) {
        EnHonotrap_SetupFlameVanish(thisv);
    } else {
        EnHonotrap_FlameCollisionCheck(thisv, globalCtx);
    }
}

void EnHonotrap_SetupFlameVanish(EnHonotrap* thisv) {
    thisv->actionFunc = EnHonotrap_FlameVanish;
}

void EnHonotrap_FlameVanish(EnHonotrap* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 ready = Math_StepToF(&thisv->actor.scale.x, 0.0001f, 0.00015f);

    thisv->actor.scale.z = thisv->actor.scale.y = thisv->actor.scale.x;
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.0f, 10.0f, 0.0f, 0x1D);
    if (ready) {
        Actor_Kill(&thisv->actor);
    }
}

void EnHonotrap_Update(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.1f, 0.0f };
    s32 pad;
    EnHonotrap* thisv = (EnHonotrap*)thisx;

    if (thisv->timer > 0) {
        thisv->timer--;
    }
    if (thisv->actor.params == HONOTRAP_EYE) {
        if ((thisv->actor.child != NULL) && (thisv->actor.child->update == NULL)) {
            thisv->actor.child = NULL;
        }
    } else {
        thisv->colChkFlags = 0;
        thisv->bobPhase += 0x640;
        thisv->actor.shape.yOffset = (Math_SinS(thisv->bobPhase) * 1000.0f) + 600.0f;
        Actor_SetFocus(&thisv->actor, 5.0f);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actor.params == HONOTRAP_EYE) {
        if (thisv->collider.tris.base.acFlags & AC_HIT) {
            EffectSsBomb2_SpawnLayered(globalCtx, &thisv->actor.world.pos, &velocity, &accel, 15, 8);
            Actor_Kill(&thisv->actor);
        } else if (thisv->eyeState < HONOTRAP_EYE_SHUT) {
            thisv->collider.tris.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.tris.base);
        }
    }
}

void EnHonotrap_DrawEye(Actor* thisx, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gEyeSwitchSilverOpenTex,
        gEyeSwitchSilverHalfTex,
        gEyeSwitchSilverClosedTex,
        gEyeSwitchSilverClosedTex,
    };
    EnHonotrap* thisv = (EnHonotrap*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 982);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeState]));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 987),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gEyeSwitch2DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 991);
}

void EnHonotrap_DrawFlame(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnHonotrap* thisv = (EnHonotrap*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 1000);

    func_80093D84(globalCtx->state.gfxCtx);
    thisv->flameScroll -= 20;
    thisv->flameScroll &= 0x1FF;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, thisv->flameScroll, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y + 0x8000) *
                       (std::numbers::pi_v<float> / 0x8000),
                   MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 1024),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_honotrap.c", 1028);
}

void EnHonotrap_Draw(Actor* thisx, GlobalContext* globalCtx) {
    switch (thisx->params) {
        case HONOTRAP_EYE:
            EnHonotrap_DrawEye(thisx, globalCtx);
            break;
        case HONOTRAP_FLAME_MOVE:
        case HONOTRAP_FLAME_DROP:
            EnHonotrap_DrawFlame(thisx, globalCtx);
            break;
    }
}
