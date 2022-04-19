/*
 * File: z_en_ice_hono.c
 * Overlay: ovl_En_Ice_Hono
 * Description: The various types of Blue Fire
 */

#include "z_en_ice_hono.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void EnIceHono_Init(Actor* thisx, GlobalContext* globalCtx);
void EnIceHono_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnIceHono_Update(Actor* thisx, GlobalContext* globalCtx);
void EnIceHono_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnIceHono_CapturableFlame(EnIceHono* thisv, GlobalContext* globalCtx);
void EnIceHono_DropFlame(EnIceHono* thisv, GlobalContext* globalCtx);
void EnIceHono_SpreadFlames(EnIceHono* thisv, GlobalContext* globalCtx);
void EnIceHono_SmallFlameMove(EnIceHono* thisv, GlobalContext* globalCtx);

void EnIceHono_SetupActionCapturableFlame(EnIceHono* thisv);
void EnIceHono_SetupActionDroppedFlame(EnIceHono* thisv);
void EnIceHono_SetupActionSpreadFlames(EnIceHono* thisv);
void EnIceHono_SetupActionSmallFlame(EnIceHono* thisv);

ActorInit En_Ice_Hono_InitVars = {
    ACTOR_EN_ICE_HONO,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnIceHono),
    (ActorFunc)EnIceHono_Init,
    (ActorFunc)EnIceHono_Destroy,
    (ActorFunc)EnIceHono_Update,
    (ActorFunc)EnIceHono_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInitCapturableFlame = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 25, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitDroppedFlame = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChainCapturableFlame[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainDroppedFlame[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainSmallFlame[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

f32 EnIceHono_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void EnIceHono_InitCapturableFlame(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChainCapturableFlame);
    Actor_SetScale(&thisv->actor, 0.0074f);
    thisv->actor.flags |= ACTOR_FLAG_0;
    Actor_SetFocus(&thisv->actor, 10.0f);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitCapturableFlame);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    EnIceHono_SetupActionCapturableFlame(thisv);
}

void EnIceHono_InitDroppedFlame(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChainDroppedFlame);
    thisv->actor.scale.x = thisv->actor.scale.z = thisv->actor.scale.y = 0.00002f;
    thisv->actor.gravity = -0.3f;
    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.shape.rot.x = thisv->actor.shape.rot.y = thisv->actor.shape.rot.z = thisv->actor.world.rot.x =
        thisv->actor.world.rot.y = thisv->actor.world.rot.z = 0;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitDroppedFlame);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    thisv->collider.dim.radius = thisv->actor.scale.x * 4000.4f;
    thisv->collider.dim.height = thisv->actor.scale.y * 8000.2f;
    thisv->actor.colChkInfo.mass = 253;
    EnIceHono_SetupActionDroppedFlame(thisv);
}

void EnIceHono_InitSmallFlame(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChainSmallFlame);
    thisv->actor.scale.x = thisv->actor.scale.z = thisv->actor.scale.y = 0.0008f;
    thisv->actor.gravity = -0.3f;
    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.shape.yOffset = 0.0f;

    EnIceHono_SetupActionSmallFlame(thisv);
}

void EnIceHono_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;
    s16 params = thisv->actor.params;

    switch (thisv->actor.params) {
        case -1:
            EnIceHono_InitCapturableFlame(&thisv->actor, globalCtx);
            break;
        case 0:
            EnIceHono_InitDroppedFlame(&thisv->actor, globalCtx);
            break;
        case 1:
        case 2:
            EnIceHono_InitSmallFlame(&thisv->actor, globalCtx);
            break;
    }

    if ((thisv->actor.params == -1) || (thisv->actor.params == 0)) {
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, (s16)thisv->actor.world.pos.y + 10,
                                  thisv->actor.world.pos.z, 155, 210, 255, 0);
        thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
        thisv->unk_154 = Rand_ZeroOne() * (0x1FFFF / 2.0f);
        thisv->unk_156 = Rand_ZeroOne() * (0x1FFFF / 2.0f);
        osSyncPrintf("(ice 炎)(arg_data 0x%04x)\n", thisv->actor.params); // "(ice flame)"
    }
}

void EnIceHono_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;

    if ((thisv->actor.params == -1) || (thisv->actor.params == 0)) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

u32 EnIceHono_InBottleRange(EnIceHono* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->actor.xzDistToPlayer < 60.0f) {
        Vec3f tempPos;
        tempPos.x = Math_SinS(thisv->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.x;
        tempPos.y = player->actor.world.pos.y;
        tempPos.z = Math_CosS(thisv->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.z;

        //! @bug: thisv check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnIceHono_XZDistanceSquared(&tempPos, &thisv->actor.world.pos) <= SQ(40.0f)) {
            return true;
        }
    }
    return false;
}

void EnIceHono_SetupActionCapturableFlame(EnIceHono* thisv) {
    thisv->actionFunc = EnIceHono_CapturableFlame;
    thisv->alpha = 255;
    thisv->actor.shape.yOffset = -1000.0f;
}

void EnIceHono_CapturableFlame(EnIceHono* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
    } else if (EnIceHono_InBottleRange(thisv, globalCtx)) {
        // GI_MAX in thisv case allows the player to catch the actor in a bottle
        func_8002F434(&thisv->actor, globalCtx, GI_MAX, 60.0f, 100.0f);
    }

    if (thisv->actor.xzDistToPlayer < 200.0f) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    func_8002F8F0(&thisv->actor, NA_SE_EV_FIRE_PILLAR_S - SFX_FLAG);
}

void EnIceHono_SetupActionDroppedFlame(EnIceHono* thisv) {
    thisv->actionFunc = EnIceHono_DropFlame;
    thisv->timer = 200;
    thisv->alpha = 255;
}

void EnIceHono_DropFlame(EnIceHono* thisv, GlobalContext* globalCtx) {
    u32 bgFlag = thisv->actor.bgCheckFlags & 1;

    Math_StepToF(&thisv->actor.scale.x, 0.0017f, 0.00008f);
    thisv->actor.scale.z = thisv->actor.scale.x;
    Math_StepToF(&thisv->actor.scale.y, 0.0017f, 0.00008f);

    if (bgFlag != 0) {
        s32 i;
        for (i = 0; i < 8; i++) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ICE_HONO, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0,
                        ((s32)(Rand_ZeroOne() * 1000.0f) + i * 0x2000) - 0x1F4, 0, 1);
        }
        EnIceHono_SetupActionSpreadFlames(thisv);
    }
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, thisv->actor.scale.x * 3500.0f, 0.0f, 5);

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    thisv->collider.dim.radius = thisv->actor.scale.x * 4000.0f;
    thisv->collider.dim.height = thisv->actor.scale.y * 8000.0f;
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnIceHono_SetupActionSpreadFlames(EnIceHono* thisv) {
    thisv->actionFunc = EnIceHono_SpreadFlames;
    thisv->timer = 60;
    thisv->alpha = 255;
}

void EnIceHono_SpreadFlames(EnIceHono* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 20) {
        Math_StepToF(&thisv->actor.scale.x, 0.011f, 0.00014f);
        Math_StepToF(&thisv->actor.scale.y, 0.006f, 0.00012f);
    } else {
        Math_StepToF(&thisv->actor.scale.x, 0.0001f, 0.00015f);
        Math_StepToF(&thisv->actor.scale.y, 0.0001f, 0.00015f);
    }
    thisv->actor.scale.z = thisv->actor.scale.x;
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, thisv->actor.scale.x * 3500.0f, 0.0f, 4);
    if (thisv->timer < 25) {
        thisv->alpha -= 10;
        thisv->alpha = CLAMP(thisv->alpha, 0, 255);
    }

    if ((thisv->alpha > 100) && (thisv->timer < 40)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        thisv->collider.dim.radius = thisv->actor.scale.x * 6000.0f;
        thisv->collider.dim.height = thisv->actor.scale.y * 8000.0f;
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    if (thisv->timer == 46) {
        s32 i;
        for (i = 0; i < 10; i++) {
            s32 rot = i * 0x1999;
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ICE_HONO, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0,
                        ((s32)(Rand_ZeroOne() * 1000.0f) + rot) - 0x1F4, 0, 2);
        }
    }

    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnIceHono_SetupActionSmallFlame(EnIceHono* thisv) {
    thisv->actionFunc = EnIceHono_SmallFlameMove;
    thisv->timer = 44;
    thisv->alpha = 255;
    if (thisv->actor.params == 1) {
        thisv->smallFlameTargetYScale = (Rand_ZeroOne() * 0.005f) + 0.004f;
        thisv->actor.speedXZ = (Rand_ZeroOne() * 1.6f) + 0.5f;
    } else {
        thisv->smallFlameTargetYScale = (Rand_ZeroOne() * 0.005f) + 0.003f;
        thisv->actor.speedXZ = (Rand_ZeroOne() * 2.0f) + 0.5f;
    }
}

void EnIceHono_SmallFlameMove(EnIceHono* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 20) {
        Math_StepToF(&thisv->actor.scale.x, 0.006f, 0.00016f);
        Math_StepToF(&thisv->actor.scale.y, thisv->smallFlameTargetYScale * 0.667f, 0.00014f);
    } else {
        Math_StepToF(&thisv->actor.scale.x, 0.0001f, 0.00015f);
        Math_StepToF(&thisv->actor.scale.y, 0.0001f, 0.00015f);
    }
    thisv->actor.scale.z = thisv->actor.scale.x;
    Math_StepToF(&thisv->actor.speedXZ, 0, 0.06f);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 0.0f, 5);

    if (thisv->timer < 25) {
        thisv->alpha -= 10;
        thisv->alpha = CLAMP(thisv->alpha, 0, 255);
    }
    if (thisv->timer <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnIceHono_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;
    s32 pad1;
    f32 intensity;
    s32 pad2;
    f32 sin154;
    f32 sin156;

    if (thisv->timer > 0) {
        thisv->timer--;
    }
    if (thisv->actor.params == 0) {
        func_8002F8F0(&thisv->actor, NA_SE_IT_FLAME - SFX_FLAG);
    }
    if ((thisv->actor.params == -1) || (thisv->actor.params == 0)) {
        thisv->unk_154 += 0x1111;
        thisv->unk_156 += 0x4000;
        sin156 = Math_SinS(thisv->unk_156);
        sin154 = Math_SinS(thisv->unk_154);
        intensity = (Rand_ZeroOne() * 0.05f) + ((sin154 * 0.125f) + (sin156 * 0.1f)) + 0.425f;
        if ((intensity > 0.7f) || (intensity < 0.2f)) {
            osSyncPrintf("ありえない値(ratio = %f)\n", intensity); // "impossible value(ratio = %f)"
        }
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, (s16)thisv->actor.world.pos.y + 10,
                                  thisv->actor.world.pos.z, (s32)(155.0f * intensity), (s32)(210.0f * intensity),
                                  (s32)(255.0f * intensity), 1400);
    }

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void EnIceHono_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnIceHono* thisv = (EnIceHono*)thisx;
    u32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ice_hono.c", 695);
    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (globalCtx->state.frames * -20) % 512,
                                32, 128));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, thisv->alpha);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 0);

    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y + 0x8000) *
                       (std::numbers::pi_v<float> / 0x8000),
                   MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ice_hono.c", 718),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ice_hono.c", 722);
}
