#include "z_en_crow.h"
#include "objects/object_crow/object_crow.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_12 | ACTOR_FLAG_14)

void EnCrow_Init(Actor* thisx, GlobalContext* globalCtx);
void EnCrow_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnCrow_Update(Actor* thisx, GlobalContext* globalCtx);
void EnCrow_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnCrow_SetupFlyIdle(EnCrow* thisv);
void EnCrow_FlyIdle(EnCrow* thisv, GlobalContext* globalCtx);
void EnCrow_Respawn(EnCrow* thisv, GlobalContext* globalCtx);
void EnCrow_DiveAttack(EnCrow* thisv, GlobalContext* globalCtx);
void EnCrow_Die(EnCrow* thisv, GlobalContext* globalCtx);
void EnCrow_TurnAway(EnCrow* thisv, GlobalContext* globalCtx);
void EnCrow_Damaged(EnCrow* thisv, GlobalContext* globalCtx);

static Vec3f sZeroVecAccel = { 0.0f, 0.0f, 0.0f };

ActorInit En_Crow_InitVars = {
    ACTOR_EN_CROW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_CROW,
    sizeof(EnCrow),
    (ActorFunc)EnCrow_Init,
    (ActorFunc)EnCrow_Destroy,
    (ActorFunc)EnCrow_Update,
    (ActorFunc)EnCrow_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 30 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static u32 sDeathCount = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x58, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -200, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_STOP),
};

static Vec3f sHeadVec = { 2500.0f, 0.0f, 0.0f };

void EnCrow_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnCrow* thisv = (EnCrow*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGuaySkel, &gGuayFlyAnim, thisv->jointTable, thisv->morphTable, 9);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
    thisv->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    ActorShape_Init(&thisv->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);
    sDeathCount = 0;
    EnCrow_SetupFlyIdle(thisv);
}

void EnCrow_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnCrow* thisv = (EnCrow*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

// Setup Action functions

void EnCrow_SetupFlyIdle(EnCrow* thisv) {
    thisv->timer = 100;
    thisv->collider.base.acFlags |= AC_ON;
    thisv->actionFunc = EnCrow_FlyIdle;
    thisv->skelAnime.playSpeed = 1.0f;
}

void EnCrow_SetupDiveAttack(EnCrow* thisv) {
    thisv->timer = 300;
    thisv->actor.speedXZ = 4.0f;
    thisv->skelAnime.playSpeed = 2.0f;
    thisv->actionFunc = EnCrow_DiveAttack;
}

void EnCrow_SetupDamaged(EnCrow* thisv, GlobalContext* globalCtx) {
    s32 i;
    f32 scale;
    Vec3f iceParticlePos;

    thisv->actor.speedXZ *= Math_CosS(thisv->actor.world.rot.x);
    thisv->actor.velocity.y = 0.0f;
    Animation_Change(&thisv->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    scale = thisv->actor.scale.x * 100.0f;
    thisv->actor.world.pos.y += 20.0f * scale;
    thisv->actor.bgCheckFlags &= ~1;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.targetArrowOffset = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KAICHO_DEAD);

    if (thisv->actor.colChkInfo.damageEffect == 3) { // Ice arrows
        Actor_SetColorFilter(&thisv->actor, 0, 255, 0, 40);
        for (i = 0; i < 8; i++) {
            iceParticlePos.x = ((i & 1 ? 7.0f : -7.0f) * scale) + thisv->actor.world.pos.x;
            iceParticlePos.y = ((i & 2 ? 7.0f : -7.0f) * scale) + thisv->actor.world.pos.y;
            iceParticlePos.z = ((i & 4 ? 7.0f : -7.0f) * scale) + thisv->actor.world.pos.z;
            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &iceParticlePos, 150, 150, 150, 250, 235, 245, 255,
                                           ((Rand_ZeroOne() * 0.15f) + 0.85f) * scale);
        }
    } else if (thisv->actor.colChkInfo.damageEffect == 2) { // Fire arrows and Din's Fire
        Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 40);

        for (i = 0; i < 4; i++) {
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &thisv->actor.world.pos, 50.0f * scale, 0, 0, i);
        }
    } else {
        Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 40);
    }

    if (thisv->actor.flags & ACTOR_FLAG_15) {
        thisv->actor.speedXZ = 0.0f;
    }

    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.flags |= ACTOR_FLAG_4;

    thisv->actionFunc = EnCrow_Damaged;
}

void EnCrow_SetupDie(EnCrow* thisv) {
    thisv->actor.colorFilterTimer = 0;
    thisv->actionFunc = EnCrow_Die;
}

void EnCrow_SetupTurnAway(EnCrow* thisv) {
    thisv->timer = 100;
    thisv->actor.speedXZ = 3.5f;
    thisv->aimRotX = -0x1000;
    thisv->aimRotY = thisv->actor.yawTowardsPlayer + 0x8000;
    thisv->skelAnime.playSpeed = 2.0f;
    Actor_SetColorFilter(&thisv->actor, 0, 255, 0, 5);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->actionFunc = EnCrow_TurnAway;
}

void EnCrow_SetupRespawn(EnCrow* thisv) {
    if (sDeathCount == 10) {
        thisv->actor.params = 1;
        sDeathCount = 0;
        thisv->collider.elements[0].dim.worldSphere.radius =
            sJntSphInit.elements[0].dim.modelSphere.radius * 0.03f * 100.0f;
    } else {
        thisv->actor.params = 0;
        thisv->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    }

    Animation_PlayLoop(&thisv->skelAnime, &gGuayFlyAnim);
    Math_Vec3f_Copy(&thisv->actor.world.pos, &thisv->actor.home.pos);
    thisv->actor.shape.rot.x = 0;
    thisv->actor.shape.rot.z = 0;
    thisv->timer = 300;
    thisv->actor.shape.yOffset = 2000;
    thisv->actor.targetArrowOffset = 2000.0f;
    thisv->actor.draw = NULL;
    thisv->actionFunc = EnCrow_Respawn;
}

// Action functions

void EnCrow_FlyIdle(EnCrow* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 skelanimeUpdated;
    s16 var;

    SkelAnime_Update(&thisv->skelAnime);
    skelanimeUpdated = Animation_OnFrame(&thisv->skelAnime, 0.0f);
    thisv->actor.speedXZ = (Rand_ZeroOne() * 1.5f) + 3.0f;

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->aimRotY = thisv->actor.wallYaw;
    } else if (Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) > 300.0f) {
        thisv->aimRotY = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
    }

    if ((Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->aimRotY, 5, 0x300, 0x10) == 0) && skelanimeUpdated &&
        (Rand_ZeroOne() < 0.1f)) {
        var = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos) - thisv->actor.shape.rot.y;
        if (var > 0) {
            thisv->aimRotY += 0x1000 + (0x1000 * Rand_ZeroOne());
        } else {
            thisv->aimRotY -= 0x1000 + (0x1000 * Rand_ZeroOne());
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KAICHO_CRY);
    }

    if (thisv->actor.yDistToWater > -40.0f) {
        thisv->aimRotX = -0x1000;
    } else if (thisv->actor.world.pos.y < (thisv->actor.home.pos.y - 50.0f)) {
        thisv->aimRotX = -0x800 - (Rand_ZeroOne() * 0x800);
    } else if (thisv->actor.world.pos.y > (thisv->actor.home.pos.y + 50.0f)) {
        thisv->aimRotX = 0x800 + (Rand_ZeroOne() * 0x800);
    }

    if ((Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->aimRotX, 10, 0x100, 8) == 0) && (skelanimeUpdated) &&
        (Rand_ZeroOne() < 0.1f)) {
        if (thisv->actor.home.pos.y < thisv->actor.world.pos.y) {
            thisv->aimRotX -= (0x400 * Rand_ZeroOne()) + 0x400;
        } else {
            thisv->aimRotX += (0x400 * Rand_ZeroOne()) + 0x400;
        }
        thisv->aimRotX = CLAMP(thisv->aimRotX, -0x1000, 0x1000);
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x100, 0x400);
    }

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if ((thisv->timer == 0) && (thisv->actor.xzDistToPlayer < 300.0f) && !(player->stateFlags1 & 0x00800000) &&
        (thisv->actor.yDistToWater < -40.0f) && (Player_GetMask(globalCtx) != PLAYER_MASK_SKULL)) {
        EnCrow_SetupDiveAttack(thisv);
    }
}

void EnCrow_DiveAttack(EnCrow* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 facingPlayer;
    Vec3f pos;
    s16 target;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    facingPlayer = Actor_IsFacingPlayer(&thisv->actor, 0x2800);

    if (facingPlayer) {
        pos.x = player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y + 20.0f;
        pos.z = player->actor.world.pos.z;
        target = Actor_WorldPitchTowardPoint(&thisv->actor, &pos);
        if (target > 0x3000) {
            target = 0x3000;
        }
        Math_ApproachS(&thisv->actor.shape.rot.x, target, 2, 0x400);
    } else {
        Math_ApproachS(&thisv->actor.shape.rot.x, -0x1000, 2, 0x100);
    }

    if (facingPlayer || (thisv->actor.xzDistToPlayer > 80.0f)) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 4, 0xC00);
    }

    if ((thisv->timer == 0) || (Player_GetMask(globalCtx) == PLAYER_MASK_SKULL) ||
        (thisv->collider.base.atFlags & AT_HIT) || (thisv->actor.bgCheckFlags & 9) ||
        (player->stateFlags1 & 0x00800000) || (thisv->actor.yDistToWater > -40.0f)) {
        if (thisv->collider.base.atFlags & AT_HIT) {
            thisv->collider.base.atFlags &= ~AT_HIT;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KAICHO_ATTACK);
        }

        EnCrow_SetupFlyIdle(thisv);
    }
}

void EnCrow_Damaged(EnCrow* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.5f);
    thisv->actor.colorFilterTimer = 40;

    if (!(thisv->actor.flags & ACTOR_FLAG_15)) {
        if (thisv->actor.colorFilterParams & 0x4000) {
            Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x4000, 0x200);
            thisv->actor.shape.rot.z += 0x1780;
        }
        if ((thisv->actor.bgCheckFlags & 1) || (thisv->actor.floorHeight == BGCHECK_Y_MIN)) {
            EffectSsDeadDb_Spawn(globalCtx, &thisv->actor.world.pos, &sZeroVecAccel, &sZeroVecAccel,
                                 thisv->actor.scale.x * 10000.0f, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
            EnCrow_SetupDie(thisv);
        }
    }
}

void EnCrow_Die(EnCrow* thisv, GlobalContext* globalCtx) {
    f32 step;

    if (thisv->actor.params != 0) {
        step = 0.006f;
    } else {
        step = 0.002f;
    }

    if (Math_StepToF(&thisv->actor.scale.x, 0.0f, step)) {
        if (thisv->actor.params == 0) {
            sDeathCount++;
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0);
        } else {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_RUPEE_RED);
        }
        EnCrow_SetupRespawn(thisv);
    }

    thisv->actor.scale.z = thisv->actor.scale.y = thisv->actor.scale.x;
}

void EnCrow_TurnAway(EnCrow* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->aimRotY = thisv->actor.wallYaw;
    } else {
        thisv->aimRotY = thisv->actor.yawTowardsPlayer + 0x8000;
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->aimRotY, 3, 0xC00);
    Math_ApproachS(&thisv->actor.shape.rot.x, thisv->aimRotX, 5, 0x100);

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        EnCrow_SetupFlyIdle(thisv);
    }
}

void EnCrow_Respawn(EnCrow* thisv, GlobalContext* globalCtx) {
    f32 target;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        SkelAnime_Update(&thisv->skelAnime);
        thisv->actor.draw = EnCrow_Draw;
        if (thisv->actor.params != 0) {
            target = 0.03f;
        } else {
            target = 0.01f;
        }
        if (Math_StepToF(&thisv->actor.scale.x, target, target * 0.1f)) {
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->actor.flags &= ~ACTOR_FLAG_4;
            thisv->actor.colChkInfo.health = 1;
            EnCrow_SetupFlyIdle(thisv);
        }
        thisv->actor.scale.z = thisv->actor.scale.y = thisv->actor.scale.x;
    }
}

void EnCrow_UpdateDamage(EnCrow* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.elements[0].info, 1);
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (thisv->actor.colChkInfo.damageEffect == 1) { // Deku Nuts
                EnCrow_SetupTurnAway(thisv);
            } else {
                Actor_ApplyDamage(&thisv->actor);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                EnCrow_SetupDamaged(thisv, globalCtx);
            }
        }
    }
}

void EnCrow_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnCrow* thisv = (EnCrow*)thisx;
    f32 pad;
    f32 height;
    f32 scale;

    EnCrow_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    scale = thisv->actor.scale.x * 100.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.world.rot.x = -thisv->actor.shape.rot.x;

    if (thisv->actionFunc != EnCrow_Respawn) {
        if (thisv->actor.colChkInfo.health != 0) {
            height = 20.0f * scale;
            func_8002D97C(&thisv->actor);
        } else {
            height = 0.0f;
            Actor_MoveForward(&thisv->actor);
        }
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 12.0f * scale, 25.0f * scale, 50.0f * scale, 7);
    } else {
        height = 0.0f;
    }

    thisv->collider.elements[0].dim.worldSphere.center.x = thisv->actor.world.pos.x;
    thisv->collider.elements[0].dim.worldSphere.center.y = thisv->actor.world.pos.y + height;
    thisv->collider.elements[0].dim.worldSphere.center.z = thisv->actor.world.pos.z;

    if (thisv->actionFunc == EnCrow_DiveAttack) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->actionFunc != EnCrow_Respawn) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    Actor_SetFocus(&thisv->actor, height);

    if (thisv->actor.colChkInfo.health != 0 && Animation_OnFrame(&thisv->skelAnime, 3.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KAICHO_FLUTTER);
    }
}

s32 EnCrow_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCrow* thisv = (EnCrow*)thisx;

    if (thisv->actor.colChkInfo.health != 0) {
        if (limbIndex == 7) {
            rot->y += 0xC00 * sinf(thisv->skelAnime.curFrame * (std::numbers::pi_v<float> / 4));
        } else if (limbIndex == 8) {
            rot->y += 0x1400 * sinf((thisv->skelAnime.curFrame + 2.5f) * (std::numbers::pi_v<float> / 4));
        }
    }
    return false;
}

void EnCrow_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnCrow* thisv = (EnCrow*)thisx;
    Vec3f* vec;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&sHeadVec, &thisv->bodyPartsPos[0]);
        thisv->bodyPartsPos[0].y -= 20.0f;
    } else if ((limbIndex == 4) || (limbIndex == 6) || (limbIndex == 8)) {
        vec = &thisv->bodyPartsPos[(limbIndex >> 1) - 1];
        Matrix_MultVec3f(&sZeroVecAccel, vec);
        vec->y -= 20.0f;
    }
}

void EnCrow_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnCrow* thisv = (EnCrow*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnCrow_OverrideLimbDraw, EnCrow_PostLimbDraw, thisv);
}
