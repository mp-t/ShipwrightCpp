#include "z_en_eiyer.h"
#include "objects/object_ei/object_ei.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnEiyer_Init(Actor* thisx, GlobalContext* globalCtx);
void EnEiyer_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnEiyer_Update(Actor* thisx, GlobalContext* globalCtx);
void EnEiyer_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnEiyer_SetupAppearFromGround(EnEiyer* thisv);
void EnEiyer_SetupUnderground(EnEiyer* thisv);
void EnEiyer_SetupInactive(EnEiyer* thisv);
void EnEiyer_SetupAmbush(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_SetupGlide(EnEiyer* thisv);
void EnEiyer_SetupStartAttack(EnEiyer* thisv);
void EnEiyer_SetupDiveAttack(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_SetupLand(EnEiyer* thisv);
void EnEiyer_SetupHurt(EnEiyer* thisv);
void EnEiyer_SetupDie(EnEiyer* thisv);
void EnEiyer_SetupDead(EnEiyer* thisv);
void EnEiyer_SetupStunned(EnEiyer* thisv);

void EnEiyer_AppearFromGround(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_WanderUnderground(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_CircleUnderground(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Inactive(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Ambush(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Glide(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_StartAttack(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_DiveAttack(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Land(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Hurt(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Die(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Dead(EnEiyer* thisv, GlobalContext* globalCtx);
void EnEiyer_Stunned(EnEiyer* thisv, GlobalContext* globalCtx);

ActorInit En_Eiyer_InitVars = {
    ACTOR_EN_EIYER,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_EI,
    sizeof(EnEiyer),
    (ActorFunc)EnEiyer_Init,
    (ActorFunc)EnEiyer_Destroy,
    (ActorFunc)EnEiyer_Update,
    (ActorFunc)EnEiyer_Draw,
    NULL,
};

static ColliderCylinderInit sColCylInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x04, 0x08 },
        { 0x00000019, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_ON,
        OCELEM_ON,
    },
    { 27, 17, -10, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 45, 15, 100 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x0),
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

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x19, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2500, ICHAIN_STOP),
};

/**
 * params    0: Spawn 3 clones and circle around spawn point
 * params  1-3: Clone, spawn another clone for the main Eiyer if params < 3
 * params   10: Normal Eiyer, wander around spawn point
 */
void EnEiyer_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnEiyer* thisv = (EnEiyer*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 600.0f, ActorShadow_DrawCircle, 65.0f);
    SkelAnime_Init(globalCtx, &thisv->skelanime, &gStingerSkel, &gStingerIdleAnim, thisv->jointTable, thisv->morphTable,
                   19);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sColCylInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (thisv->actor.params < 3) {
        // Each clone spawns another clone
        if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EIYER, thisv->actor.home.pos.x,
                               thisv->actor.home.pos.y, thisv->actor.home.pos.z, 0, thisv->actor.shape.rot.y + 0x4000, 0,
                               thisv->actor.params + 1) == NULL) {
            Actor_Kill(&thisv->actor);
            return;
        }

        if (thisv->actor.params == 0) {
            Actor* child = thisv->actor.child;
            s32 clonesSpawned;

            for (clonesSpawned = 0; clonesSpawned != 3; clonesSpawned++) {
                if (child == NULL) {
                    break;
                }
                child = child->child;
            }

            if (clonesSpawned != 3) {
                for (child = &thisv->actor; child != NULL; child = child->child) {
                    Actor_Kill(child);
                }
                return;
            } else {
                thisv->actor.child->parent = &thisv->actor;
                thisv->actor.child->child->parent = &thisv->actor;
                thisv->actor.child->child->child->parent = &thisv->actor;
            }
        }
    }

    if (thisv->actor.params == 0 || thisv->actor.params == 10) {
        EnEiyer_SetupAppearFromGround(thisv);
    } else {
        EnEiyer_SetupInactive(thisv);
    }
}

void EnEiyer_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnEiyer* thisv = (EnEiyer*)thisx;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnEiyer_RotateAroundHome(EnEiyer* thisv) {
    thisv->actor.world.pos.x = Math_SinS(thisv->actor.world.rot.y) * 80.0f + thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = Math_CosS(thisv->actor.world.rot.y) * 80.0f + thisv->actor.home.pos.z;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x4000;
}

void EnEiyer_SetupAppearFromGround(EnEiyer* thisv) {
    thisv->collider.info.bumper.dmgFlags = 0x19;
    Animation_PlayLoop(&thisv->skelanime, &gStingerIdleAnim);

    thisv->actor.world.pos.x = thisv->actor.home.pos.x;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y - 40.0f;
    thisv->actor.world.pos.z = thisv->actor.home.pos.z;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 0.0f;

    if (thisv->actor.params != 0xA) {
        if (thisv->actor.params == 0) {
            thisv->actor.world.rot.y = Rand_ZeroOne() * 0x10000;
        } else {
            thisv->actor.world.rot.y = thisv->actor.parent->world.rot.y + thisv->actor.params * 0x4000;
        }
        EnEiyer_RotateAroundHome(thisv);
    } else {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = Rand_ZeroOne() * 0x10000;
    }

    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_12);
    thisv->actor.shape.shadowScale = 0.0f;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actionFunc = EnEiyer_AppearFromGround;
}

void EnEiyer_SetupUnderground(EnEiyer* thisv) {
    if (thisv->actor.params == 0xA) {
        thisv->actor.speedXZ = -0.5f;
        thisv->actionFunc = EnEiyer_WanderUnderground;
    } else {
        thisv->actionFunc = EnEiyer_CircleUnderground;
    }

    thisv->collider.base.acFlags |= AC_ON;
    thisv->actor.flags &= ~ACTOR_FLAG_4;
    thisv->actor.flags |= ACTOR_FLAG_0;
}

void EnEiyer_SetupInactive(EnEiyer* thisv) {
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actionFunc = EnEiyer_Inactive;
}

void EnEiyer_SetupAmbush(EnEiyer* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;
    Animation_PlayOnce(&thisv->skelanime, &gStingerBackflipAnim);
    thisv->collider.info.bumper.dmgFlags = ~0x00300000;
    thisv->basePos = thisv->actor.world.pos;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.flags |= ACTOR_FLAG_12;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.shape.shadowScale = 65.0f;
    thisv->actor.shape.yOffset = 600.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_JUMP);
    EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 1, 700);
    thisv->actionFunc = EnEiyer_Ambush;
}

void EnEiyer_SetupGlide(EnEiyer* thisv) {
    thisv->targetYaw = thisv->actor.shape.rot.y;
    thisv->basePos.y = (cosf(-std::numbers::pi_v<float> / 8) * 5.0f) + thisv->actor.world.pos.y;
    Animation_MorphToLoop(&thisv->skelanime, &gStingerHitAnim, -5.0f);
    thisv->timer = 60;
    thisv->actionFunc = EnEiyer_Glide;
}

void EnEiyer_SetupStartAttack(EnEiyer* thisv) {
    thisv->actionFunc = EnEiyer_StartAttack;
}

void EnEiyer_SetupDiveAttack(EnEiyer* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->actor.velocity.y = 0.0f;
    thisv->basePos.y = player->actor.world.pos.y + 15.0f;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->collider.base.atFlags &= ~AT_HIT;
    thisv->actionFunc = EnEiyer_DiveAttack;
}

void EnEiyer_SetupLand(EnEiyer* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelanime, &gStingerDiveAnim, -3.0f);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->actor.flags |= ACTOR_FLAG_4;

    // Update BgCheck info, play sound, and spawn effect on the first frame of the land action
    thisv->timer = -1;
    thisv->actor.gravity = 0.0f;
    thisv->collider.dim.height = sColCylInit.dim.height;
    thisv->actionFunc = EnEiyer_Land;
}

void EnEiyer_SetupHurt(EnEiyer* thisv) {
    thisv->basePos.y = thisv->actor.world.pos.y;
    Animation_Change(&thisv->skelanime, &gStingerHitAnim, 2.0f, 0.0f, 0.0f, 0, -3.0f);
    thisv->timer = 40;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 200, 0, 40);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnEiyer_Hurt;
}

void EnEiyer_SetupDie(EnEiyer* thisv) {
    thisv->timer = 20;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 200, 0, 40);

    if (thisv->collider.info.bumper.dmgFlags != 0x19) {
        thisv->actor.speedXZ = 6.0f;
        Animation_MorphToLoop(&thisv->skelanime, &gStingerHitAnim, -3.0f);
    } else {
        thisv->actor.speedXZ -= 6.0f;
    }

    thisv->collider.info.bumper.dmgFlags = ~0x00300000;
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnEiyer_Die;
}

void EnEiyer_SetupDead(EnEiyer* thisv) {
    thisv->actor.colorFilterParams |= 0x2000;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actionFunc = EnEiyer_Dead;
}

void EnEiyer_SetupStunned(EnEiyer* thisv) {
    Animation_Change(&thisv->skelanime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, 0, -8.0f);
    thisv->timer = 80;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -1.0f;
    thisv->collider.dim.height = sColCylInit.dim.height + 8;
    Actor_SetColorFilter(&thisv->actor, 0, 200, 0, 80);
    thisv->collider.base.atFlags &= ~AT_ON;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->actionFunc = EnEiyer_Stunned;
}

void EnEiyer_AppearFromGround(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.75f)) {
        EnEiyer_SetupUnderground(thisv);
    }
}

void EnEiyer_CheckPlayerCollision(EnEiyer* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        thisv->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        EnEiyer_SetupAmbush(thisv, globalCtx);
    }
}

void EnEiyer_CircleUnderground(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    thisv->actor.world.rot.y += -0x60;
    EnEiyer_RotateAroundHome(thisv);
    EnEiyer_CheckPlayerCollision(thisv, globalCtx);

    // Clones disappear when the main Eiyer leaves the ground
    if (thisv->actor.params != 0 && ((EnEiyer*)thisv->actor.parent)->actionFunc != EnEiyer_CircleUnderground) {
        EnEiyer_SetupInactive(thisv);
    }
}

void EnEiyer_WanderUnderground(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) > 100.0f) {
        thisv->targetYaw = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos) + 0x8000;
    } else if (thisv->targetYaw == thisv->actor.world.rot.y && Rand_ZeroOne() > 0.99f) {
        thisv->targetYaw =
            thisv->actor.world.rot.y + (Rand_ZeroOne() < 0.5f ? -1 : 1) * (Rand_ZeroOne() * 0x2000 + 0x2000);
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->targetYaw, 0xB6);
    EnEiyer_CheckPlayerCollision(thisv, globalCtx);
}

void EnEiyer_Inactive(EnEiyer* thisv, GlobalContext* globalCtx) {
    EnEiyer* parent;

    if (thisv->actor.home.pos.y - 50.0f < thisv->actor.world.pos.y) {
        thisv->actor.world.pos.y -= 0.5f;
    }

    parent = (EnEiyer*)thisv->actor.parent;
    if (parent->actionFunc == EnEiyer_Dead) {
        Actor_Kill(&thisv->actor);
    } else if (parent->actionFunc == EnEiyer_AppearFromGround) {
        EnEiyer_SetupAppearFromGround(thisv);
    }
}

void EnEiyer_Ambush(EnEiyer* thisv, GlobalContext* globalCtx) {
    s32 animFinished;
    f32 curFrame;
    f32 xzOffset;
    s32 bgId;

    animFinished = SkelAnime_Update(&thisv->skelanime);
    curFrame = thisv->skelanime.curFrame;

    if (thisv->skelanime.curFrame < 12.0f) {
        thisv->actor.world.pos.y = ((1.0f - cosf((0.996f * std::numbers::pi_v<float> / 12.0f) * curFrame)) * 40.0f) + thisv->actor.home.pos.y;
        xzOffset = sinf((0.996f * std::numbers::pi_v<float> / 12.0f) * curFrame) * -40.0f;
        thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * xzOffset) + thisv->basePos.x;
        thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * xzOffset) + thisv->basePos.z;
    } else {
        Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 80.0f, 0.5f);
        thisv->actor.speedXZ = 0.8f;
    }

    if (animFinished) {
        thisv->collider.base.acFlags |= AC_ON;
        EnEiyer_SetupGlide(thisv);
    } else {
        thisv->actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                              &thisv->actor, &thisv->actor.world.pos);
    }
}

void EnEiyer_Glide(EnEiyer* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    s32 pad;
    s16 yawChange;

    SkelAnime_Update(&thisv->skelanime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    curFrame = thisv->skelanime.curFrame;
    Math_ApproachF(&thisv->basePos.y, thisv->actor.floorHeight + 80.0f + 5.0f, 0.3f, thisv->actor.speedXZ);
    thisv->actor.world.pos.y = thisv->basePos.y - cosf((curFrame - 5.0f) * (std::numbers::pi_v<float> / 40)) * 5.0f;

    if (curFrame <= 45.0f) {
        Math_StepToF(&thisv->actor.speedXZ, 1.0f, 0.03f);
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 1.5f, 0.03f);
    }

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->targetYaw = thisv->actor.wallYaw;
    }

    if (Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->targetYaw, 0xB6)) {
        if (thisv->timer != 0 || Rand_ZeroOne() > 0.05f) {
            thisv->actor.world.rot.y += 0x100;
        } else {
            yawChange = Rand_S16Offset(0x2000, 0x2000);
            thisv->targetYaw = (Rand_ZeroOne() < 0.5f ? -1 : 1) * yawChange + thisv->actor.world.rot.y;
        }
    }

    if (thisv->timer == 0 && thisv->actor.yDistToPlayer < 0.0f && thisv->actor.xzDistToPlayer < 120.0f) {
        EnEiyer_SetupStartAttack(thisv);
    }

    func_8002F974(&thisv->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_StartAttack(EnEiyer* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f focus;

    SkelAnime_Update(&thisv->skelanime);

    if (thisv->actor.shape.rot.x > 0 && thisv->actor.shape.rot.x < 0x8000) {
        focus.x = player->actor.world.pos.x;
        focus.y = player->actor.world.pos.y + 20.0f;
        focus.z = player->actor.world.pos.z;

        if (Math_ScaledStepToS(&thisv->actor.shape.rot.x, Actor_WorldPitchTowardPoint(&thisv->actor, &focus), 0x1000)) {
            EnEiyer_SetupDiveAttack(thisv, globalCtx);
        }
    } else {
        thisv->actor.shape.rot.x -= 0x1000;
    }

    thisv->actor.world.rot.x = -thisv->actor.shape.rot.x;
    Math_StepToF(&thisv->actor.speedXZ, 5.0f, 0.3f);
    Math_ApproachS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 2, 0x71C);
    func_8002F974(&thisv->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_DiveAttack(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    thisv->actor.speedXZ *= 1.1f;

    if (thisv->actor.bgCheckFlags & 8 || thisv->actor.bgCheckFlags & 1) {
        EnEiyer_SetupLand(thisv);
    }

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~(AT_ON | AT_HIT);
    }

    func_8002F974(&thisv->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void EnEiyer_Land(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);
    Math_ScaledStepToS(&thisv->actor.world.rot.x, -0x4000, 0x450);
    Math_StepToF(&thisv->actor.speedXZ, 7.0f, 1.0f);

    if (thisv->timer == -1) {
        if (thisv->actor.bgCheckFlags & 8 || thisv->actor.bgCheckFlags & 1) {
            thisv->timer = 10;
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 30, NA_SE_EN_OCTAROCK_SINK);

            if (thisv->actor.bgCheckFlags & 1) {
                EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 1, 700);
            }
        }
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            thisv->actor.shape.rot.x = 0;
            thisv->actor.world.rot.x = 0;
            EnEiyer_SetupAppearFromGround(thisv);
        }
    }
}

void EnEiyer_Hurt(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_ApproachF(&thisv->basePos.y, thisv->actor.floorHeight + 80.0f + 5.0f, 0.5f, thisv->actor.speedXZ);
    thisv->actor.world.pos.y = thisv->basePos.y - 5.0f;

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->targetYaw = thisv->actor.wallYaw;
    } else {
        thisv->targetYaw = thisv->actor.yawTowardsPlayer + 0x8000;
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->targetYaw, 0x38E);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x200);
    thisv->actor.shape.rot.z = sinf(thisv->timer * (std::numbers::pi_v<float> / 5)) * 5120.0f;

    if (thisv->timer == 0) {
        thisv->actor.shape.rot.x = 0;
        thisv->actor.shape.rot.z = 0;
        thisv->collider.base.acFlags |= AC_ON;
        EnEiyer_SetupGlide(thisv);
    }
    thisv->actor.world.rot.x = -thisv->actor.shape.rot.x;
}

void EnEiyer_Die(EnEiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->actor.speedXZ > 0.0f) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x4000, 0x400);
    } else {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x4000, 0x400);
    }

    thisv->actor.shape.rot.z += 0x1000;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.world.rot.x = -thisv->actor.shape.rot.x;

    if (thisv->timer == 0 || thisv->actor.bgCheckFlags & 0x10) {
        EnEiyer_SetupDead(thisv);
    }
}

void EnEiyer_Dead(EnEiyer* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.shadowAlpha = CLAMP_MIN((s16)(thisv->actor.shape.shadowAlpha - 5), 0);
    thisv->actor.world.pos.y -= 2.0f;

    if (thisv->actor.shape.shadowAlpha == 0) {
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 80);
        Actor_Kill(&thisv->actor);
    }
}

void EnEiyer_Stunned(EnEiyer* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x200);
    SkelAnime_Update(&thisv->skelanime);

    if (Animation_OnFrame(&thisv->skelanime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_FLUTTER);
    }

    if (thisv->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    }

    if (thisv->timer == 0) {
        thisv->actor.gravity = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->collider.dim.height = sColCylInit.dim.height;
        EnEiyer_SetupGlide(thisv);
    }
}

void EnEiyer_UpdateDamage(EnEiyer* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);

        if (thisv->actor.colChkInfo.damageEffect != 0 || thisv->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_DEAD);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            }

            // If underground, one hit kill
            if (thisv->collider.info.bumper.dmgFlags == 0x19) {
                if (thisv->actor.colChkInfo.damage == 0) {
                    EnEiyer_SetupAmbush(thisv, globalCtx);
                } else {
                    EnEiyer_SetupDie(thisv);
                }
            } else if (thisv->actor.colChkInfo.damageEffect == 1) {
                if (thisv->actionFunc != EnEiyer_Stunned) {
                    EnEiyer_SetupStunned(thisv);
                }
            } else if (thisv->actor.colChkInfo.health != 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_DAMAGE);
                EnEiyer_SetupHurt(thisv);
            } else {
                thisv->collider.dim.height = sColCylInit.dim.height;
                EnEiyer_SetupDie(thisv);
            }
        }
    }
}

void EnEiyer_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnEiyer* thisv = (EnEiyer*)thisx;
    s32 pad;

    EnEiyer_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.world.rot.x == 0 || thisv->actionFunc == EnEiyer_Stunned) {
        Actor_MoveForward(&thisv->actor);
    } else {
        func_8002D97C(&thisv->actor);
    }

    if (thisv->actionFunc == EnEiyer_Glide || thisv->actionFunc == EnEiyer_DiveAttack ||
        thisv->actionFunc == EnEiyer_Stunned || thisv->actionFunc == EnEiyer_Die || thisv->actionFunc == EnEiyer_Hurt ||
        (thisv->actionFunc == EnEiyer_Land && thisv->timer == -1)) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 27.0f, 30.0f, 7);
    }

    if (thisv->actor.params == 0xA ||
        (thisv->actionFunc != EnEiyer_AppearFromGround && thisv->actionFunc != EnEiyer_CircleUnderground)) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    }

    // only the main Eiyer can ambush the player
    if (thisv->actor.params == 0 || thisv->actor.params == 0xA) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (thisv->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->actionFunc != EnEiyer_Ambush) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    if (thisv->actor.flags & ACTOR_FLAG_0) {
        thisv->actor.focus.pos.x = thisv->actor.world.pos.x + Math_SinS(thisv->actor.shape.rot.y) * 12.5f;
        thisv->actor.focus.pos.z = thisv->actor.world.pos.z + Math_CosS(thisv->actor.shape.rot.y) * 12.5f;
        thisv->actor.focus.pos.y = thisv->actor.world.pos.y;
    }
}

s32 EnEiyer_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    EnEiyer* thisv = (EnEiyer*)thisx;

    if (limbIndex == 1) {
        pos->z += 2500.0f;
    }

    if (thisv->collider.info.bumper.dmgFlags == 0x19 && limbIndex != 9 && limbIndex != 10) {
        *dList = NULL;
    }
    return 0;
}

void EnEiyer_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnEiyer* thisv = (EnEiyer*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_eiyer.c", 1494);
    if (thisv->actionFunc != EnEiyer_Dead) {
        func_80093D18(globalCtx->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable,
                                       EnEiyer_OverrideLimbDraw, NULL, thisv, POLY_OPA_DISP);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, D_80116280);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->actor.shape.shadowAlpha);

        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable,
                                       EnEiyer_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_eiyer.c", 1541);
}
