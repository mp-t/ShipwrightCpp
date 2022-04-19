/*
 * File: z_obj_tsubo.c
 * Overlay: ovl_Obj_Tsubo
 * Description: Breakable pot
 */

#include "z_obj_tsubo.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_tsubo/object_tsubo.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_23)

void ObjTsubo_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjTsubo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjTsubo_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjTsubo_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjTsubo_SpawnCollectible(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_ApplyGravity(ObjTsubo* thisv);
s32 ObjTsubo_SnapToFloor(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_InitCollider(Actor* thisx, GlobalContext* globalCtx);
void ObjTsubo_AirBreak(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_WaterBreak(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_SetupWaitForObject(ObjTsubo* thisv);
void ObjTsubo_WaitForObject(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_SetupIdle(ObjTsubo* thisv);
void ObjTsubo_Idle(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_SetupLiftedUp(ObjTsubo* thisv);
void ObjTsubo_LiftedUp(ObjTsubo* thisv, GlobalContext* globalCtx);
void ObjTsubo_SetupThrown(ObjTsubo* thisv);
void ObjTsubo_Thrown(ObjTsubo* thisv, GlobalContext* globalCtx);

static s16 D_80BA1B50 = 0;
static s16 D_80BA1B54 = 0;
static s16 D_80BA1B58 = 0;
static s16 D_80BA1B5C = 0;

ActorInit Obj_Tsubo_InitVars = {
    ACTOR_OBJ_TSUBO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjTsubo),
    (ActorFunc)ObjTsubo_Init,
    (ActorFunc)ObjTsubo_Destroy,
    (ActorFunc)ObjTsubo_Update,
    NULL,
    NULL,
};

static s16 sObjectIds[] = { OBJECT_GAMEPLAY_DANGEON_KEEP, OBJECT_TSUBO };

static const Gfx* D_80BA1B84[] = { gPotDL, object_tsubo_DL_0017C0 };

static const Gfx* D_80BA1B8C[] = { gPotFragmentDL, object_tsubo_DL_001960 };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HARD,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000002, 0x00, 0x01 },
        { 0x4FC1FFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 9, 26, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit[] = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE), ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 150, ICHAIN_CONTINUE),   ICHAIN_F32(uncullZoneForward, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 100, ICHAIN_CONTINUE),   ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

void ObjTsubo_SpawnCollectible(ObjTsubo* thisv, GlobalContext* globalCtx) {
    s16 dropParams = thisv->actor.params & 0x1F;

    if ((dropParams >= ITEM00_RUPEE_GREEN) && (dropParams <= ITEM00_BOMBS_SPECIAL)) {
        Item_DropCollectible(globalCtx, &thisv->actor.world.pos,
                             (dropParams | (((thisv->actor.params >> 9) & 0x3F) << 8)));
    }
}

void ObjTsubo_ApplyGravity(ObjTsubo* thisv) {
    thisv->actor.velocity.y += thisv->actor.gravity;
    if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
        thisv->actor.velocity.y = thisv->actor.minVelocityY;
    }
}

s32 ObjTsubo_SnapToFloor(ObjTsubo* thisv, GlobalContext* globalCtx) {
    CollisionPoly* floorPoly;
    Vec3f pos;
    s32 bgID;
    f32 floorY;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 20.0f;
    pos.z = thisv->actor.world.pos.z;
    floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &floorPoly, &bgID, &thisv->actor, &pos);
    if (floorY > BGCHECK_Y_MIN) {
        thisv->actor.world.pos.y = floorY;
        Math_Vec3f_Copy(&thisv->actor.home.pos, &thisv->actor.world.pos);
        return true;
    } else {
        osSyncPrintf("地面に付着失敗\n");
        return false;
    }
}

void ObjTsubo_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    ObjTsubo* thisv = (ObjTsubo*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
}

void ObjTsubo_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjTsubo* thisv = (ObjTsubo*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ObjTsubo_InitCollider(&thisv->actor, globalCtx);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, sColChkInfoInit);
    if (!ObjTsubo_SnapToFloor(thisv, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->objTsuboBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIds[(thisv->actor.params >> 8) & 1]);
    if (thisv->objTsuboBankIndex < 0) {
        osSyncPrintf("Error : バンク危険！ (arg_data 0x%04x)(%s %d)\n", thisv->actor.params, "../z_obj_tsubo.c", 410);
        Actor_Kill(&thisv->actor);
    } else {
        ObjTsubo_SetupWaitForObject(thisv);
        osSyncPrintf("(dungeon keep 壷)(arg_data 0x%04x)\n", thisv->actor.params);
    }
}

void ObjTsubo_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjTsubo* thisv = (ObjTsubo*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void ObjTsubo_AirBreak(ObjTsubo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 rand;
    s16 angle;
    Vec3f pos;
    Vec3f velocity;
    f32 sins;
    f32 coss;
    s32 arg5;
    s32 i;

    for (i = 0, angle = 0; i < 15; i++, angle += 0x4E20) {
        sins = Math_SinS(angle);
        coss = Math_CosS(angle);
        pos.x = sins * 8.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = coss * 8.0f;
        velocity.x = pos.x * 0.23f;
        velocity.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        velocity.z = pos.z * 0.23f;
        Math_Vec3f_Sum(&pos, &thisv->actor.world.pos, &pos);
        rand = Rand_ZeroOne();
        if (rand < 0.2f) {
            arg5 = 96;
        } else if (rand < 0.6f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &thisv->actor.world.pos, -240, arg5, 10, 10, 0,
                             (Rand_ZeroOne() * 95.0f) + 15.0f, 0, 32, 60, KAKERA_COLOR_NONE,
                             sObjectIds[(thisv->actor.params >> 8) & 1], D_80BA1B8C[(thisv->actor.params >> 8) & 1]);
    }
    func_80033480(globalCtx, &thisv->actor.world.pos, 30.0f, 4, 20, 50, 1);
}

void ObjTsubo_WaterBreak(ObjTsubo* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 angle;
    Vec3f pos = thisv->actor.world.pos;
    Vec3f velocity;
    s32 phi_s0;
    s32 i;

    pos.y += thisv->actor.yDistToWater;
    EffectSsGSplash_Spawn(globalCtx, &pos, NULL, NULL, 0, 400);
    for (i = 0, angle = 0; i < 15; i++, angle += 0x4E20) {
        f32 sins = Math_SinS(angle);
        f32 coss = Math_CosS(angle);

        pos.x = sins * 8.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = coss * 8.0f;
        velocity.x = pos.x * 0.2f;
        velocity.y = (Rand_ZeroOne() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;
        Math_Vec3f_Sum(&pos, &thisv->actor.world.pos, &pos);
        phi_s0 = (Rand_ZeroOne() < .2f) ? 64 : 32;
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &thisv->actor.world.pos, -180, phi_s0, 30, 30, 0,
                             (Rand_ZeroOne() * 95.0f) + 15.0f, 0, 32, 70, KAKERA_COLOR_NONE,
                             sObjectIds[(thisv->actor.params >> 8) & 1], D_80BA1B8C[(thisv->actor.params >> 8) & 1]);
    }
}

void ObjTsubo_SetupWaitForObject(ObjTsubo* thisv) {
    thisv->actionFunc = ObjTsubo_WaitForObject;
}

void ObjTsubo_WaitForObject(ObjTsubo* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objTsuboBankIndex)) {
        thisv->actor.draw = ObjTsubo_Draw;
        thisv->actor.objBankIndex = thisv->objTsuboBankIndex;
        ObjTsubo_SetupIdle(thisv);
        thisv->actor.flags &= ~ACTOR_FLAG_4;
    }
}

void ObjTsubo_SetupIdle(ObjTsubo* thisv) {
    thisv->actionFunc = ObjTsubo_Idle;
}

void ObjTsubo_Idle(ObjTsubo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 temp_v0;
    s32 phi_v1;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        ObjTsubo_SetupLiftedUp(thisv);
    } else if ((thisv->actor.bgCheckFlags & 0x20) && (thisv->actor.yDistToWater > 15.0f)) {
        ObjTsubo_WaterBreak(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        ObjTsubo_SpawnCollectible(thisv, globalCtx);
        Actor_Kill(&thisv->actor);
    } else if ((thisv->collider.base.acFlags & AC_HIT) &&
               (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x4FC1FFFC)) {
        ObjTsubo_AirBreak(thisv, globalCtx);
        ObjTsubo_SpawnCollectible(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_Kill(&thisv->actor);
    } else {
        if (thisv->actor.xzDistToPlayer < 600.0f) {
            Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
            thisv->collider.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            if (thisv->actor.xzDistToPlayer < 150.0f) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
        }
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            temp_v0 = thisv->actor.yawTowardsPlayer - GET_PLAYER(globalCtx)->actor.world.rot.y;
            phi_v1 = ABS(temp_v0);
            if (phi_v1 >= 0x5556) {
                // GI_NONE in thisv case allows the player to lift the actor
                func_8002F434(&thisv->actor, globalCtx, GI_NONE, 30.0f, 30.0f);
            }
        }
    }
}

void ObjTsubo_SetupLiftedUp(ObjTsubo* thisv) {
    thisv->actionFunc = ObjTsubo_LiftedUp;
    thisv->actor.room = -1;
    func_8002F7DC(&thisv->actor, NA_SE_PL_PULL_UP_POT);
    thisv->actor.flags |= ACTOR_FLAG_4;
}

void ObjTsubo_LiftedUp(ObjTsubo* thisv, GlobalContext* globalCtx) {
    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        thisv->actor.room = globalCtx->roomCtx.curRoom.num;
        ObjTsubo_SetupThrown(thisv);
        ObjTsubo_ApplyGravity(thisv);
        func_8002D7EC(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 15.0f, 0.0f, 0x85);
    }
}

void ObjTsubo_SetupThrown(ObjTsubo* thisv) {
    thisv->actor.velocity.x = Math_SinS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
    thisv->actor.velocity.z = Math_CosS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
    thisv->actor.colChkInfo.mass = 240;
    D_80BA1B50 = (Rand_ZeroOne() - 0.7f) * 2800.0f;
    D_80BA1B58 = (Rand_ZeroOne() - 0.5f) * 2000.0f;
    D_80BA1B54 = 0;
    D_80BA1B5C = 0;
    thisv->actionFunc = ObjTsubo_Thrown;
}

void ObjTsubo_Thrown(ObjTsubo* thisv, GlobalContext* globalCtx) {
    s32 pad[2];

    if ((thisv->actor.bgCheckFlags & 0xB) || (thisv->collider.base.atFlags & AT_HIT)) {
        ObjTsubo_AirBreak(thisv, globalCtx);
        ObjTsubo_SpawnCollectible(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_Kill(&thisv->actor);
    } else if (thisv->actor.bgCheckFlags & 0x40) {
        ObjTsubo_WaterBreak(thisv, globalCtx);
        ObjTsubo_SpawnCollectible(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_Kill(&thisv->actor);
    } else {
        ObjTsubo_ApplyGravity(thisv);
        func_8002D7EC(&thisv->actor);
        Math_StepToS(&D_80BA1B54, D_80BA1B50, 0x64);
        Math_StepToS(&D_80BA1B5C, D_80BA1B58, 0x64);
        thisv->actor.shape.rot.x += D_80BA1B54;
        thisv->actor.shape.rot.y += D_80BA1B5C;
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 15.0f, 0.0f, 0x85);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void ObjTsubo_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjTsubo* thisv = (ObjTsubo*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void ObjTsubo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, D_80BA1B84[(thisx->params >> 8) & 1]);
}
