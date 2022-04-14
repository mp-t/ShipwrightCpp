/*
 * File: z_en_ishi.c
 * Overlay: ovl_En_Ishi
 * Description: Small and large gray rocks
 */

#include "z_en_ishi.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#include "vt.h"

#define FLAGS ACTOR_FLAG_23

void EnIshi_Init(Actor* thisx, GlobalContext* globalCtx);
void EnIshi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnIshi_Update(Actor* thisx, GlobalContext* globalCtx);
void EnIshi_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnIshi_Reset(void);

void EnIshi_SetupWait(EnIshi* thisv);
void EnIshi_Wait(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SetupLiftedUp(EnIshi* thisv);
void EnIshi_LiftedUp(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SetupFly(EnIshi* thisv);
void EnIshi_Fly(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SpawnFragmentsSmall(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SpawnFragmentsLarge(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SpawnDustSmall(EnIshi* thisv, GlobalContext* globalCtx);
void EnIshi_SpawnDustLarge(EnIshi* thisv, GlobalContext* globalCtx);

static s16 sRotSpeedX = 0;
static s16 sRotSpeedY = 0;

const ActorInit En_Ishi_InitVars = {
    ACTOR_EN_ISHI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(EnIshi),
    (ActorFunc)EnIshi_Init,
    (ActorFunc)EnIshi_Destroy,
    (ActorFunc)EnIshi_Update,
    (ActorFunc)EnIshi_Draw,
    (ActorResetFunc)EnIshi_Reset,
};

static f32 sRockScales[] = { 0.1f, 0.4f };
static f32 D_80A7FA20[] = { 58.0f, 80.0f };
static f32 D_80A7FA28[] = { 0.0f, 0.005f };

// the sizes of these arrays are very large and take up way more space than it needs to.
// coincidentally the sizes are the same as the ID for NA_SE_EV_ROCK_BROKEN, which may explain a mistake that could
// have been made here
static u16 sBreakSounds[] = { NA_SE_EV_ROCK_BROKEN, NA_SE_EV_WALL_BROKEN };

static u8 sBreakSoundDurations[] = { 20, 40 };

static EnIshiEffectSpawnFunc sFragmentSpawnFuncs[] = { EnIshi_SpawnFragmentsSmall, EnIshi_SpawnFragmentsLarge };

static EnIshiEffectSpawnFunc sDustSpawnFuncs[] = { EnIshi_SpawnDustSmall, EnIshi_SpawnDustLarge };

static ColliderCylinderInit sCylinderInits[] = {
    {
        {
            COLTYPE_HARD,
            AT_NONE,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 10, 18, -2, { 0, 0, 0 } },
    },
    {
        {
            COLTYPE_HARD,
            AT_NONE,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFF6, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 55, 70, 0, { 0, 0, 0 } },
    },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

void EnIshi_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    EnIshi* thisv = (EnIshi*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInits[thisv->actor.params & 1]);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
}

s32 EnIshi_SnapToFloor(EnIshi* thisv, GlobalContext* globalCtx, f32 arg2) {
    CollisionPoly* poly;
    Vec3f pos;
    s32 bgId;
    f32 floorY;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 30.0f;
    pos.z = thisv->actor.world.pos.z;
    floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &poly, &bgId, &thisv->actor, &pos);
    if (floorY > BGCHECK_Y_MIN) {
        thisv->actor.world.pos.y = floorY + arg2;
        Math_Vec3f_Copy(&thisv->actor.home.pos, &thisv->actor.world.pos);
        return true;
    } else {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Failure attaching to ground"
        osSyncPrintf("地面に付着失敗(%s %d)\n", "../z_en_ishi.c", 388);
        osSyncPrintf(VT_RST);
        return false;
    }
}

void EnIshi_SpawnFragmentsSmall(EnIshi* thisv, GlobalContext* globalCtx) {
    static s16 scales[] = { 16, 13, 11, 9, 7, 5 };
    s32 pad;
    Vec3f velocity;
    Vec3f pos;
    s16 phi_v0;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(scales); i++) {
        pos.x = thisv->actor.world.pos.x + (Rand_ZeroOne() - 0.5f) * 8.0f;
        pos.y = thisv->actor.world.pos.y + (Rand_ZeroOne() * 5.0f) + 5.0f;
        pos.z = thisv->actor.world.pos.z + (Rand_ZeroOne() - 0.5f) * 8.0f;
        Math_Vec3f_Copy(&velocity, &thisv->actor.velocity);
        if (thisv->actor.bgCheckFlags & 1) {
            velocity.x *= 0.8f;
            velocity.y *= -0.8f;
            velocity.z *= 0.8f;
        } else if (thisv->actor.bgCheckFlags & 8) {
            velocity.x *= -0.8f;
            velocity.y *= 0.8f;
            velocity.z *= -0.8f;
        }
        velocity.x += (Rand_ZeroOne() - 0.5f) * 11.0f;
        velocity.y += Rand_ZeroOne() * 6.0f;
        velocity.z += (Rand_ZeroOne() - 0.5f) * 11.0f;
        if (Rand_ZeroOne() < 0.5f) {
            phi_v0 = 65;
        } else {
            phi_v0 = 33;
        }
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, -420, phi_v0, 30, 5, 0, scales[i], 3, 10, 40,
                             KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_FIELD_KEEP, gFieldKakeraDL);
    }
}

void EnIshi_SpawnFragmentsLarge(EnIshi* thisv, GlobalContext* globalCtx) {
    static s16 scales[] = { 145, 135, 120, 100, 70, 50, 45, 40, 35 };
    Actor* thisx = &thisv->actor;
    Vec3f velocity;
    Vec3f pos;
    s16 angle = 0x1000;
    s32 i;
    f32 rand;
    s16 phi_v0;
    s16 phi_v1;

    for (i = 0; i < ARRAY_COUNT(scales); i++) {
        angle += 0x4E20;
        rand = Rand_ZeroOne() * 10.0f;
        pos.x = thisv->actor.world.pos.x + (Math_SinS(angle) * rand);
        pos.y = thisv->actor.world.pos.y + (Rand_ZeroOne() * 40.0f) + 5.0f;
        pos.z = thisv->actor.world.pos.z + (Math_CosS(angle) * rand);
        Math_Vec3f_Copy(&velocity, &thisx->velocity);
        if (thisx->bgCheckFlags & 1) {
            velocity.x *= 0.9f;
            velocity.y *= -0.8f;
            velocity.z *= 0.9f;
        } else if (thisx->bgCheckFlags & 8) {
            velocity.x *= -0.9f;
            velocity.y *= 0.8f;
            velocity.z *= -0.9f;
        }
        rand = Rand_ZeroOne() * 10.0f;
        velocity.x += rand * Math_SinS(angle);
        velocity.y += (Rand_ZeroOne() * 4.0f) + ((Rand_ZeroOne() * i) * 0.7f);
        velocity.z += rand * Math_CosS(angle);
        if (i == 0) {
            phi_v0 = 41;
            phi_v1 = -450;
        } else if (i < 4) {
            phi_v0 = 37;
            phi_v1 = -380;
        } else {
            phi_v0 = 69;
            phi_v1 = -320;
        }
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &thisv->actor.world.pos, phi_v1, phi_v0, 30, 5, 0, scales[i], 5,
                             2, 70, KAKERA_COLOR_WHITE, OBJECT_GAMEPLAY_FIELD_KEEP, gSilverRockFragmentsDL);
    }
}

void EnIshi_SpawnDustSmall(EnIshi* thisv, GlobalContext* globalCtx) {
    Vec3f pos;

    Math_Vec3f_Copy(&pos, &thisv->actor.world.pos);
    if (thisv->actor.bgCheckFlags & 1) {
        pos.x += 2.0f * thisv->actor.velocity.x;
        pos.y -= 2.0f * thisv->actor.velocity.y;
        pos.z += 2.0f * thisv->actor.velocity.z;
    } else if (thisv->actor.bgCheckFlags & 8) {
        pos.x -= 2.0f * thisv->actor.velocity.x;
        pos.y += 2.0f * thisv->actor.velocity.y;
        pos.z -= 2.0f * thisv->actor.velocity.z;
    }
    func_80033480(globalCtx, &pos, 60.0f, 3, 0x50, 0x3C, 1);
}

void EnIshi_SpawnDustLarge(EnIshi* thisv, GlobalContext* globalCtx) {
    Vec3f pos;

    Math_Vec3f_Copy(&pos, &thisv->actor.world.pos);
    if (thisv->actor.bgCheckFlags & 1) {
        pos.x += 2.0f * thisv->actor.velocity.x;
        pos.y -= 2.0f * thisv->actor.velocity.y;
        pos.z += 2.0f * thisv->actor.velocity.z;
    } else if (thisv->actor.bgCheckFlags & 8) {
        pos.x -= 2.0f * thisv->actor.velocity.x;
        pos.y += 2.0f * thisv->actor.velocity.y;
        pos.z -= 2.0f * thisv->actor.velocity.z;
    }
    func_80033480(globalCtx, &pos, 140.0f, 0xA, 0xB4, 0x5A, 1);
}

void EnIshi_DropCollectible(EnIshi* thisv, GlobalContext* globalCtx) {
    s16 dropParams;

    if ((thisv->actor.params & 1) == ROCK_SMALL) {
        dropParams = (thisv->actor.params >> 8) & 0xF;

        if (dropParams >= 0xD) {
            dropParams = 0;
        }

        Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, dropParams << 4);
    }
}

void EnIshi_Fall(EnIshi* thisv) {
    thisv->actor.velocity.y += thisv->actor.gravity;

    if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
        thisv->actor.velocity.y = thisv->actor.minVelocityY;
    }
}

void func_80A7ED94(Vec3f* arg0, f32 arg1) {
    arg1 += ((Rand_ZeroOne() * 0.2f) - 0.1f) * arg1;
    arg0->x -= arg0->x * arg1;
    arg0->y -= arg0->y * arg1;
    arg0->z -= arg0->z * arg1;
}

void EnIshi_SpawnBugs(EnIshi* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 3; i++) {
        Actor* bug = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_INSECT, thisv->actor.world.pos.x,
                                 thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, Rand_ZeroOne() * 0xFFFF, 0, 1);

        if (bug == NULL) {
            break;
        }
    }
}

static InitChainEntry sInitChains[][5] = {
    {
        ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneScale, 150, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
    },
    {
        ICHAIN_F32_DIV1000(gravity, -2500, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneScale, 250, ICHAIN_CONTINUE),
        ICHAIN_F32(uncullZoneDownward, 500, ICHAIN_STOP),
    },
};

void EnIshi_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnIshi* thisv = (EnIshi*)thisx;
    s16 type = thisv->actor.params & 1;

    Actor_ProcessInitChain(&thisv->actor, sInitChains[type]);
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        thisv->actor.uncullZoneForward += 1000.0f;
    }
    if (thisv->actor.shape.rot.y == 0) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = Rand_ZeroFloat(0x10000);
    }
    Actor_SetScale(&thisv->actor, sRockScales[type]);
    EnIshi_InitCollider(&thisv->actor, globalCtx);
    if ((type == ROCK_LARGE) &&
        Flags_GetSwitch(globalCtx, ((thisv->actor.params >> 0xA) & 0x3C) | ((thisv->actor.params >> 6) & 3))) {
        Actor_Kill(&thisv->actor);
        return;
    }
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    thisv->actor.shape.yOffset = D_80A7FA20[type];
    if (!((thisv->actor.params >> 5) & 1) && !EnIshi_SnapToFloor(thisv, globalCtx, 0.0f)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    EnIshi_SetupWait(thisv);
}

void EnIshi_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnIshi* thisv = (EnIshi*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnIshi_SetupWait(EnIshi* thisv) {
    thisv->actionFunc = EnIshi_Wait;
}

void EnIshi_Wait(EnIshi* thisv, GlobalContext* globalCtx) {
    static u16 liftSounds[] = { NA_SE_PL_PULL_UP_ROCK, NA_SE_PL_PULL_UP_BIGROCK };
    s32 pad;
    s16 type = thisv->actor.params & 1;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        EnIshi_SetupLiftedUp(thisv);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, liftSounds[type]);
        if ((thisv->actor.params >> 4) & 1) {
            EnIshi_SpawnBugs(thisv, globalCtx);
        }
    } else if ((thisv->collider.base.acFlags & AC_HIT) && (type == ROCK_SMALL) &&
               thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x40000048) {
        EnIshi_DropCollectible(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, sBreakSoundDurations[type],
                                           sBreakSounds[type]);
        sFragmentSpawnFuncs[type](thisv, globalCtx);
        sDustSpawnFuncs[type](thisv, globalCtx);
        Actor_Kill(&thisv->actor);
    } else if (thisv->actor.xzDistToPlayer < 600.0f) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        thisv->collider.base.acFlags &= ~AC_HIT;
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        if (thisv->actor.xzDistToPlayer < 400.0f) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            if (thisv->actor.xzDistToPlayer < 90.0f) {
                // GI_NONE in these cases allows the player to lift the actor
                if (type == ROCK_LARGE) {
                    func_8002F434(&thisv->actor, globalCtx, GI_NONE, 80.0f, 20.0f);
                } else {
                    func_8002F434(&thisv->actor, globalCtx, GI_NONE, 50.0f, 10.0f);
                }
            }
        }
    }
}

void EnIshi_SetupLiftedUp(EnIshi* thisv) {
    thisv->actionFunc = EnIshi_LiftedUp;
    thisv->actor.room = -1;
    thisv->actor.flags |= ACTOR_FLAG_4;
}

void EnIshi_LiftedUp(EnIshi* thisv, GlobalContext* globalCtx) {
    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        thisv->actor.room = globalCtx->roomCtx.curRoom.num;
        if ((thisv->actor.params & 1) == ROCK_LARGE) {
            Flags_SetSwitch(globalCtx, ((thisv->actor.params >> 0xA) & 0x3C) | ((thisv->actor.params >> 6) & 3));
        }
        EnIshi_SetupFly(thisv);
        EnIshi_Fall(thisv);
        func_80A7ED94(&thisv->actor.velocity, D_80A7FA28[thisv->actor.params & 1]);
        func_8002D7EC(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.5f, 35.0f, 0.0f, 0xC5);
    }
}

void EnIshi_SetupFly(EnIshi* thisv) {
    thisv->actor.velocity.x = Math_SinS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
    thisv->actor.velocity.z = Math_CosS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
    if ((thisv->actor.params & 1) == ROCK_SMALL) {
        sRotSpeedX = (Rand_ZeroOne() - 0.5f) * 16000.0f;
        sRotSpeedY = (Rand_ZeroOne() - 0.5f) * 2400.0f;
    } else {
        sRotSpeedX = (Rand_ZeroOne() - 0.5f) * 8000.0f;
        sRotSpeedY = (Rand_ZeroOne() - 0.5f) * 1600.0f;
    }
    thisv->actor.colChkInfo.mass = 240;
    thisv->actionFunc = EnIshi_Fly;
}

void EnIshi_Fly(EnIshi* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 type = thisv->actor.params & 1;
    s32 pad2;
    s32 quakeIdx;
    Vec3f contactPos;

    if (thisv->actor.bgCheckFlags & 9) {
        EnIshi_DropCollectible(thisv, globalCtx);
        sFragmentSpawnFuncs[type](thisv, globalCtx);
        if (!(thisv->actor.bgCheckFlags & 0x20)) {
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, sBreakSoundDurations[type],
                                               sBreakSounds[type]);
            sDustSpawnFuncs[type](thisv, globalCtx);
        }
        if (type == ROCK_LARGE) {
            quakeIdx = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
            Quake_SetSpeed(quakeIdx, -0x3CB0);
            Quake_SetQuakeValues(quakeIdx, 3, 0, 0, 0);
            Quake_SetCountdown(quakeIdx, 7);
            func_800AA000(thisv->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
        }
        Actor_Kill(&thisv->actor);
        return;
    }
    if (thisv->actor.bgCheckFlags & 0x40) {
        contactPos.x = thisv->actor.world.pos.x;
        contactPos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
        contactPos.z = thisv->actor.world.pos.z;
        EffectSsGSplash_Spawn(globalCtx, &contactPos, 0, 0, 0, 350);
        if (type == ROCK_SMALL) {
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 150, 650, 0);
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 400, 800, 4);
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 500, 1100, 8);
        } else {
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 300, 700, 0);
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 500, 900, 4);
            EffectSsGRipple_Spawn(globalCtx, &contactPos, 500, 1300, 8);
        }
        thisv->actor.minVelocityY = -6.0f;
        sRotSpeedX >>= 2;
        sRotSpeedY >>= 2;
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
        thisv->actor.bgCheckFlags &= ~0x40;
    }
    Math_StepToF(&thisv->actor.shape.yOffset, 0.0f, 2.0f);
    EnIshi_Fall(thisv);
    func_80A7ED94(&thisv->actor.velocity, D_80A7FA28[type]);
    func_8002D7EC(&thisv->actor);
    thisv->actor.shape.rot.x += sRotSpeedX;
    thisv->actor.shape.rot.y += sRotSpeedY;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 7.5f, 35.0f, 0.0f, 0xC5);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnIshi_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnIshi* thisv = (EnIshi*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void EnIshi_DrawSmall(EnIshi* thisv, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gFieldKakeraDL);
}

void EnIshi_DrawLarge(EnIshi* thisv, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ishi.c", 1050);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ishi.c", 1055),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSilverRockDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ishi.c", 1062);
}

static EnIshiDrawFunc sDrawFuncs[] = { EnIshi_DrawSmall, EnIshi_DrawLarge };

void EnIshi_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnIshi* thisv = (EnIshi*)thisx;

    sDrawFuncs[thisv->actor.params & 1](thisv, globalCtx);
}

void EnIshi_Reset(void) {
    sRotSpeedX = 0;
    sRotSpeedY = 0;
}