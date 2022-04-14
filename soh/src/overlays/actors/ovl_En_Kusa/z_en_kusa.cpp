/*
 * File: z_en_kusa.c
 * Overlay: ovl_en_kusa
 * Description: Bush
 */

#include "z_en_kusa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_kusa/object_kusa.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_23)

void EnKusa_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKusa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKusa_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKusa_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnKusa_SetupLiftedUp(EnKusa* thisv);
void EnKusa_SetupWaitObject(EnKusa* thisv);
void EnKusa_SetupMain(EnKusa* thisv);
void EnKusa_SetupFall(EnKusa* thisv);
void EnKusa_SetupCut(EnKusa* thisv);
void EnKusa_SetupUprootedWaitRegrow(EnKusa* thisv);
void EnKusa_SetupRegrow(EnKusa* thisv);

void EnKusa_Fall(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_WaitObject(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_Main(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_LiftedUp(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_CutWaitRegrow(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_DoNothing(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_UprootedWaitRegrow(EnKusa* thisv, GlobalContext* globalCtx);
void EnKusa_Regrow(EnKusa* thisv, GlobalContext* globalCtx);

static s16 rotSpeedXtarget = 0;
static s16 rotSpeedX = 0;
static s16 rotSpeedYtarget = 0;
static s16 rotSpeedY = 0;

const ActorInit En_Kusa_InitVars = {
    ACTOR_EN_KUSA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnKusa),
    (ActorFunc)EnKusa_Init,
    (ActorFunc)EnKusa_Destroy,
    (ActorFunc)EnKusa_Update,
    NULL,
    NULL,
};

static s16 sObjectIds[] = { OBJECT_GAMEPLAY_FIELD_KEEP, OBJECT_KUSA, OBJECT_KUSA };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC00758, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 44, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 30, MASS_IMMOVABLE };

static Vec3f sUnitDirections[] = {
    { 0.0f, 0.7071f, 0.7071f },
    { 0.7071f, 0.7071f, 0.0f },
    { 0.0f, 0.7071f, -0.7071f },
    { -0.7071f, 0.7071f, 0.0f },
};

static s16 sFragmentScales[] = { 108, 102, 96, 84, 66, 55, 42, 38 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),         ICHAIN_F32_DIV1000(gravity, -3200, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(minVelocityY, -17000, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 100, ICHAIN_CONTINUE),         ICHAIN_F32(uncullZoneDownward, 120, ICHAIN_STOP),
};

void EnKusa_SetupAction(EnKusa* thisv, EnKusaActionFunc actionFunc) {
    thisv->timer = 0;
    thisv->actionFunc = actionFunc;
}

s32 EnKusa_SnapToFloor(EnKusa* thisv, GlobalContext* globalCtx, f32 yOffset) {
    s32 pad;
    CollisionPoly* poly;
    Vec3f pos;
    s32 bgId;
    f32 floorY;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 30.0f;
    pos.z = thisv->actor.world.pos.z;

    floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &poly, &bgId, &thisv->actor, &pos);

    if (floorY > BGCHECK_Y_MIN) {
        thisv->actor.world.pos.y = floorY + yOffset;
        Math_Vec3f_Copy(&thisv->actor.home.pos, &thisv->actor.world.pos);
        return true;
    } else {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Failure attaching to ground"
        osSyncPrintf("地面に付着失敗(%s %d)\n", "../z_en_kusa.c", 323);
        osSyncPrintf(VT_RST);
        return false;
    }
}

void EnKusa_DropCollectible(EnKusa* thisv, GlobalContext* globalCtx) {
    s16 dropParams;

    switch (thisv->actor.params & 3) {
        case ENKUSA_TYPE_0:
        case ENKUSA_TYPE_2:
            dropParams = (thisv->actor.params >> 8) & 0xF;

            if (dropParams >= 0xD) {
                dropParams = 0;
            }
            Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, dropParams << 4);
            break;
        case ENKUSA_TYPE_1:
            if (Rand_ZeroOne() < 0.5f) {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_SEEDS);
            } else {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_HEART);
            }
            break;
    }
}

void EnKusa_UpdateVelY(EnKusa* thisv) {
    thisv->actor.velocity.y += thisv->actor.gravity;

    if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
        thisv->actor.velocity.y = thisv->actor.minVelocityY;
    }
}

void EnKusa_RandScaleVecToZero(Vec3f* vec, f32 scale) {
    scale += ((Rand_ZeroOne() * 0.2f) - 0.1f) * scale;
    vec->x -= vec->x * scale;
    vec->y -= vec->y * scale;
    vec->z -= vec->z * scale;
}

void EnKusa_SetScaleSmall(EnKusa* thisv) {
    thisv->actor.scale.y = 0.16000001f;
    thisv->actor.scale.x = 0.120000005f;
    thisv->actor.scale.z = 0.120000005f;
}

void EnKusa_SpawnFragments(EnKusa* thisv, GlobalContext* globalCtx) {
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    s32 scaleIndex;
    Vec3f* dir;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(sUnitDirections); i++) {
        dir = &sUnitDirections[i];

        pos.x = thisv->actor.world.pos.x + (dir->x * thisv->actor.scale.x * 20.0f);
        pos.y = thisv->actor.world.pos.y + (dir->y * thisv->actor.scale.y * 20.0f) + 10.0f;
        pos.z = thisv->actor.world.pos.z + (dir->z * thisv->actor.scale.z * 20.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 8.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 8.0f;

        scaleIndex = (s32)(Rand_ZeroOne() * 111.1f) & 7;

        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, -100, 64, 40, 3, 0, sFragmentScales[scaleIndex], 0, 0,
                             80, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_KEEP, gCuttableShrubStalkDL);

        pos.x = thisv->actor.world.pos.x + (dir->x * thisv->actor.scale.x * 40.0f);
        pos.y = thisv->actor.world.pos.y + (dir->y * thisv->actor.scale.y * 40.0f) + 10.0f;
        pos.z = thisv->actor.world.pos.z + (dir->z * thisv->actor.scale.z * 40.0f);

        velocity.x = (Rand_ZeroOne() - 0.5f) * 6.0f;
        velocity.y = Rand_ZeroOne() * 10.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 6.0f;

        scaleIndex = (s32)(Rand_ZeroOne() * 111.1f) % 7;

        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, -100, 64, 40, 3, 0, sFragmentScales[scaleIndex], 0, 0,
                             80, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_KEEP, gCuttableShrubTipDL);
    }
}

void EnKusa_SpawnBugs(EnKusa* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 3; i++) {
        Actor* bug = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_INSECT, thisv->actor.world.pos.x,
                                 thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, Rand_ZeroOne() * 0xFFFF, 0, 1);

        if (bug == NULL) {
            break;
        }
    }
}

void EnKusa_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    EnKusa* thisv = (EnKusa*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
}

void EnKusa_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKusa* thisv = (EnKusa*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        thisv->actor.uncullZoneForward += 1000.0f;
    }

    EnKusa_InitCollider(thisx, globalCtx);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (thisv->actor.shape.rot.y == 0) {
        s16 rand = Rand_ZeroFloat(0x10000);

        thisv->actor.world.rot.y = rand;
        thisv->actor.home.rot.y = rand;
        thisv->actor.shape.rot.y = rand;
    }

    if (!EnKusa_SnapToFloor(thisv, globalCtx, 0.0f)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIds[thisx->params & 3]);

    if (thisv->objBankIndex < 0) {
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！ (arg_data 0x%04x)(%s %d)\n", thisx->params, "../z_en_kusa.c", 561);
        Actor_Kill(&thisv->actor);
        return;
    }

    EnKusa_SetupWaitObject(thisv);
}

void EnKusa_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnKusa* thisv = (EnKusa*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnKusa_SetupWaitObject(EnKusa* thisv) {
    EnKusa_SetupAction(thisv, EnKusa_WaitObject);
}

void EnKusa_WaitObject(EnKusa* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex)) {
        if (thisv->actor.flags & ACTOR_FLAG_ENKUSA_CUT) {
            EnKusa_SetupCut(thisv);
        } else {
            EnKusa_SetupMain(thisv);
        }

        thisv->actor.draw = EnKusa_Draw;
        thisv->actor.objBankIndex = thisv->objBankIndex;
        thisv->actor.flags &= ~ACTOR_FLAG_4;
    }
}

void EnKusa_SetupMain(EnKusa* thisv) {
    EnKusa_SetupAction(thisv, EnKusa_Main);
    thisv->actor.flags &= ~ACTOR_FLAG_4;
}

void EnKusa_Main(EnKusa* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        EnKusa_SetupLiftedUp(thisv);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_PL_PULL_UP_PLANT);
    } else if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        EnKusa_SpawnFragments(thisv, globalCtx);
        EnKusa_DropCollectible(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);

        if ((thisv->actor.params >> 4) & 1) {
            EnKusa_SpawnBugs(thisv, globalCtx);
        }

        if ((thisv->actor.params & 3) == ENKUSA_TYPE_0) {
            Actor_Kill(&thisv->actor);
            return;
        }

        EnKusa_SetupCut(thisv);
        thisv->actor.flags |= ACTOR_FLAG_ENKUSA_CUT;
    } else {
        if (!(thisv->collider.base.ocFlags1 & OC1_TYPE_PLAYER) && (thisv->actor.xzDistToPlayer > 12.0f)) {
            thisv->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        }

        if (thisv->actor.xzDistToPlayer < 600.0f) {
            Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

            if (thisv->actor.xzDistToPlayer < 400.0f) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
                if (thisv->actor.xzDistToPlayer < 100.0f) {
                    func_8002F580(&thisv->actor, globalCtx);
                }
            }
        }
    }
}

void EnKusa_SetupLiftedUp(EnKusa* thisv) {
    EnKusa_SetupAction(thisv, EnKusa_LiftedUp);
    thisv->actor.room = -1;
    thisv->actor.flags |= ACTOR_FLAG_4;
}

void EnKusa_LiftedUp(EnKusa* thisv, GlobalContext* globalCtx) {
    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        thisv->actor.room = globalCtx->roomCtx.curRoom.num;
        EnKusa_SetupFall(thisv);
        thisv->actor.velocity.x = thisv->actor.speedXZ * Math_SinS(thisv->actor.world.rot.y);
        thisv->actor.velocity.z = thisv->actor.speedXZ * Math_CosS(thisv->actor.world.rot.y);
        thisv->actor.colChkInfo.mass = 240;
        thisv->actor.gravity = -0.1f;
        EnKusa_UpdateVelY(thisv);
        EnKusa_RandScaleVecToZero(&thisv->actor.velocity, 0.005f);
        func_8002D7EC(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.5f, 35.0f, 0.0f, 0xC5);
        thisv->actor.gravity = -3.2f;
    }
}

void EnKusa_SetupFall(EnKusa* thisv) {
    EnKusa_SetupAction(thisv, EnKusa_Fall);
    rotSpeedXtarget = -0xBB8;
    rotSpeedYtarget = (Rand_ZeroOne() - 0.5f) * 1600.0f;
    rotSpeedX = 0;
    rotSpeedY = 0;
}

void EnKusa_Fall(EnKusa* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f contactPos;

    if (thisv->actor.bgCheckFlags & 0xB) {
        if (!(thisv->actor.bgCheckFlags & 0x20)) {
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);
        }
        EnKusa_SpawnFragments(thisv, globalCtx);
        EnKusa_DropCollectible(thisv, globalCtx);
        switch (thisv->actor.params & 3) {
            case ENKUSA_TYPE_0:
            case ENKUSA_TYPE_2:
                Actor_Kill(&thisv->actor);
                break;

            case ENKUSA_TYPE_1:
                EnKusa_SetupUprootedWaitRegrow(thisv);
                break;
        }
        return;
    }

    if (thisv->actor.bgCheckFlags & 0x40) {
        contactPos.x = thisv->actor.world.pos.x;
        contactPos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
        contactPos.z = thisv->actor.world.pos.z;
        EffectSsGSplash_Spawn(globalCtx, &contactPos, NULL, NULL, 0, 400);
        EffectSsGRipple_Spawn(globalCtx, &contactPos, 150, 650, 0);
        EffectSsGRipple_Spawn(globalCtx, &contactPos, 400, 800, 4);
        EffectSsGRipple_Spawn(globalCtx, &contactPos, 500, 1100, 8);
        thisv->actor.minVelocityY = -3.0f;
        rotSpeedX >>= 1;
        rotSpeedXtarget >>= 1;
        rotSpeedY >>= 1;
        rotSpeedYtarget >>= 1;
        thisv->actor.bgCheckFlags &= ~0x40;
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
    }

    EnKusa_UpdateVelY(thisv);
    Math_StepToS(&rotSpeedX, rotSpeedXtarget, 0x1F4);
    Math_StepToS(&rotSpeedY, rotSpeedYtarget, 0xAA);
    thisv->actor.shape.rot.x += rotSpeedX;
    thisv->actor.shape.rot.y += rotSpeedY;
    EnKusa_RandScaleVecToZero(&thisv->actor.velocity, 0.05f);
    func_8002D7EC(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.5f, 35.0f, 0.0f, 0xC5);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnKusa_SetupCut(EnKusa* thisv) {
    switch (thisv->actor.params & 3) {
        case ENKUSA_TYPE_2:
            EnKusa_SetupAction(thisv, EnKusa_DoNothing);
            break;
        case ENKUSA_TYPE_1:
            EnKusa_SetupAction(thisv, EnKusa_CutWaitRegrow);
            break;
    }
}

void EnKusa_CutWaitRegrow(EnKusa* thisv, GlobalContext* globalCtx) {
    if (thisv->timer >= 120) {
        EnKusa_SetupRegrow(thisv);
    }
}

void EnKusa_DoNothing(EnKusa* thisv, GlobalContext* globalCtx) {
}

void EnKusa_SetupUprootedWaitRegrow(EnKusa* thisv) {
    thisv->actor.world.pos.x = thisv->actor.home.pos.x;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y - 9.0f;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z;
    EnKusa_SetScaleSmall(thisv);
    thisv->actor.shape.rot = thisv->actor.home.rot;
    EnKusa_SetupAction(thisv, EnKusa_UprootedWaitRegrow);
}

void EnKusa_UprootedWaitRegrow(EnKusa* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 120) {
        if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.6f)) {
            if (thisv->timer >= 170) {
                EnKusa_SetupRegrow(thisv);
            }
        }
    }
}

void EnKusa_SetupRegrow(EnKusa* thisv) {
    EnKusa_SetupAction(thisv, EnKusa_Regrow);
    EnKusa_SetScaleSmall(thisv);
    thisv->actor.shape.rot = thisv->actor.home.rot;
    thisv->actor.flags &= ~ACTOR_FLAG_ENKUSA_CUT;
}

void EnKusa_Regrow(EnKusa* thisv, GlobalContext* globalCtx) {
    s32 isFullyGrown = true;

    isFullyGrown &= Math_StepToF(&thisv->actor.scale.y, 0.4f, 0.014f);
    isFullyGrown &= Math_StepToF(&thisv->actor.scale.x, 0.4f, 0.011f);
    thisv->actor.scale.z = thisv->actor.scale.x;

    if (isFullyGrown) {
        Actor_SetScale(&thisv->actor, 0.4f);
        EnKusa_SetupMain(thisv);
        thisv->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
    }
}

void EnKusa_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnKusa* thisv = (EnKusa*)thisx;

    thisv->timer++;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.flags & ACTOR_FLAG_ENKUSA_CUT) {
        thisv->actor.shape.yOffset = -6.25f;
    } else {
        thisv->actor.shape.yOffset = 0.0f;
    }
}

void EnKusa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Gfx* dLists[] = { gFieldBushDL, object_kusa_DL_000140, object_kusa_DL_000140 };
    EnKusa* thisv = (EnKusa*)thisx;

    if (thisv->actor.flags & ACTOR_FLAG_ENKUSA_CUT) {
        Gfx_DrawDListOpa(globalCtx, object_kusa_DL_0002E0);
    } else {
        Gfx_DrawDListOpa(globalCtx, dLists[thisx->params & 3]);
    }
}
