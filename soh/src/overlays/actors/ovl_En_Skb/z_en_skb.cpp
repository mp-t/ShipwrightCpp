#include "z_en_skb.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "objects/object_skb/object_skb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnSkb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSkb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSkb_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSkb_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AFCD60(EnSkb* thisv);
void func_80AFCDF8(EnSkb* thisv);
void func_80AFCE5C(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFCF48(EnSkb* thisv);
void func_80AFCFF0(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD0A4(EnSkb* thisv);
void EnSkb_Advance(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD33C(EnSkb* thisv);
void EnSkb_SetupAttack(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD47C(EnSkb* thisv);
void func_80AFD508(EnSkb* thisv, GlobalContext* globalCtx);
void EnSkb_SetupStunned(EnSkb* thisv);
void func_80AFD59C(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD6CC(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD7B4(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD880(EnSkb* thisv, GlobalContext* globalCtx);
void func_80AFD968(EnSkb* thisv, GlobalContext* globalCtx);

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 15, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
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

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(1, 0xF),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x7),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x7),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(3, 0xD),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xD),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

ActorInit En_Skb_InitVars = {
    ACTOR_EN_SKB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SKB,
    sizeof(EnSkb),
    (ActorFunc)EnSkb_Init,
    (ActorFunc)EnSkb_Destroy,
    (ActorFunc)EnSkb_Update,
    (ActorFunc)EnSkb_Draw,
    NULL,
};

void EnSkb_SetupAction(EnSkb* thisv, EnSkbActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnSkb_SpawnDebris(GlobalContext* globalCtx, EnSkb* thisv, Vec3f* spawnPos) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 spreadAngle;
    f32 scale;

    spreadAngle = (Rand_ZeroOne() - 0.5f) * 6.28f;
    pos.y = thisv->actor.floorHeight;
    pos.x = (Math_SinF(spreadAngle) * 15.0f) + spawnPos->x;
    pos.z = (Math_CosF(spreadAngle) * 15.0f) + spawnPos->z;
    accel.x = Rand_CenteredFloat(1.0f);
    accel.z = Rand_CenteredFloat(1.0f);
    vel.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    scale = (Rand_ZeroOne() * 5.0f) + 12.0f;
    EffectSsHahen_Spawn(globalCtx, &pos, &vel, &accel, 2, scale * 0.8f, -1, 10, 0);
    func_80033480(globalCtx, &pos, 10.0f, 1, 150, 0, 1);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnSkb_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSkb* thisv = (EnSkb*)thisx;
    s16 paramOffsetBody;
    s16 paramOffsetArm;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.colChkInfo.mass = 0xFE;
    thisv->actor.colChkInfo.health = 2;
    thisv->actor.shape.yOffset = -8000.0f;
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gStalchildSkel, &gStalchildUncurlingAnim, thisv->jointTable,
                   thisv->morphTable, 20);
    thisv->actor.naviEnemyId = 0x55;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItem);
    Actor_SetScale(&thisv->actor, ((thisv->actor.params * 0.1f) + 1.0f) * 0.01f);

    paramOffsetBody = thisv->actor.params + 0xA;
    thisv->collider.elements[0].dim.worldSphere.radius = paramOffsetBody;
    thisv->collider.elements[0].dim.modelSphere.radius = paramOffsetBody;
    if (1) {};
    paramOffsetArm = (thisv->actor.params * 2) + 0x14;
    thisv->collider.elements[1].dim.worldSphere.radius = paramOffsetArm;
    thisv->collider.elements[1].dim.modelSphere.radius = paramOffsetArm;
    thisv->actor.home.pos = thisv->actor.world.pos;
    thisv->actor.floorHeight = thisv->actor.world.pos.y;
    func_80AFCDF8(thisv);
}

void EnSkb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSkb* thisv = (EnSkb*)thisx;

    if (thisv->actor.parent != NULL) {
        EnEncount1* spawner = (EnEncount1*)thisv->actor.parent;

        if (spawner->actor.update != NULL) {
            if (spawner->curNumSpawn > 0) {
                spawner->curNumSpawn--;
            }
        }
    }
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void func_80AFCD60(EnSkb* thisv) {
    if (IS_DAY) {
        func_80AFCF48(thisv);
    } else if (Actor_IsFacingPlayer(&thisv->actor, 0x11C7) &&
               (thisv->actor.xzDistToPlayer < (60.0f + (thisv->actor.params * 6.0f)))) {
        func_80AFD33C(thisv);
    } else {
        func_80AFD0A4(thisv);
    }
}

void func_80AFCDF8(EnSkb* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gStalchildUncurlingAnim, 1.0f);
    thisv->unk_280 = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIVA_APPEAR);
    EnSkb_SetupAction(thisv, func_80AFCE5C);
}

void func_80AFCE5C(EnSkb* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame < 4.0f) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0;
    }
    Math_SmoothStepToF(&thisv->actor.shape.yOffset, 0.0f, 1.0f, 800.0f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.shape.shadowScale, 25.0f, 1.0f, 2.5f, 0.0f);
    if ((globalCtx->gameplayFrames & 1) != 0) {
        EnSkb_SpawnDebris(globalCtx, thisv, &thisv->actor.world.pos);
    }
    if ((SkelAnime_Update(&thisv->skelAnime) != 0) && (0.0f == thisv->actor.shape.yOffset)) {
        func_80AFCD60(thisv);
    }
}

void func_80AFCF48(EnSkb* thisv) {
    Animation_Change(&thisv->skelAnime, &gStalchildUncurlingAnim, -1.0f,
                     Animation_GetLastFrame(&gStalchildUncurlingAnim), 0.0f, ANIMMODE_ONCE, -4.0f);
    thisv->unk_280 = 0;
    thisv->unk_281 = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_AKINDONUTS_HIDE);
    EnSkb_SetupAction(thisv, func_80AFCFF0);
}

void func_80AFCFF0(EnSkb* thisv, GlobalContext* globalCtx) {
    if ((Math_SmoothStepToF(&thisv->actor.shape.yOffset, -8000.0f, 1.0f, 500.0f, 0.0f) != 0.0f) &&
        (globalCtx->gameplayFrames & 1)) {
        EnSkb_SpawnDebris(globalCtx, thisv, &thisv->actor.world.pos);
    }
    Math_SmoothStepToF(&thisv->actor.shape.shadowScale, 0.0f, 1.0f, 2.5f, 0.0f);
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AFD0A4(EnSkb* thisv) {
    Animation_Change(&thisv->skelAnime, &gStalchildWalkingAnim, 0.96000004f, 0.0f,
                     Animation_GetLastFrame(&gStalchildWalkingAnim), ANIMMODE_LOOP, -4.0f);
    thisv->unk_280 = 4;
    thisv->unk_288 = 0;
    thisv->actor.speedXZ = thisv->actor.scale.y * 160.0f;
    EnSkb_SetupAction(thisv, EnSkb_Advance);
}

void EnSkb_Advance(EnSkb* thisv, GlobalContext* globalCtx) {
    s32 thisKeyFrame;
    s32 prevKeyFrame;
    f32 playSpeed;
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->unk_283 != 0) && ((globalCtx->gameplayFrames & 0xF) == 0)) {
        thisv->unk_288 = Rand_CenteredFloat(50000.0f);
    }
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, (thisv->actor.yawTowardsPlayer + thisv->unk_288), 1, 0x2EE, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisKeyFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->skelAnime.playSpeed >= 0.0f) {
        playSpeed = thisv->skelAnime.playSpeed;
    } else {
        playSpeed = -thisv->skelAnime.playSpeed;
    }
    prevKeyFrame = (thisv->skelAnime.curFrame - playSpeed);
    if (thisv->skelAnime.playSpeed >= 0.0f) {
        playSpeed = thisv->skelAnime.playSpeed;
    } else {
        playSpeed = -thisv->skelAnime.playSpeed;
    }
    if (thisKeyFrame != (s32)thisv->skelAnime.curFrame) {
        if (((prevKeyFrame < 9) && (((s32)playSpeed + thisKeyFrame) >= 8)) ||
            !((prevKeyFrame >= 16) || (((s32)playSpeed + thisKeyFrame) < 15))) {

            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALKID_WALK);
        }
    }
    if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) > 800.0f || IS_DAY) {
        func_80AFCF48(thisv);
    } else if (Actor_IsFacingPlayer(&thisv->actor, 0x11C7) &&
               (thisv->actor.xzDistToPlayer < (60.0f + (thisv->actor.params * 6.0f)))) {
        func_80AFD33C(thisv);
    }
}

void func_80AFD33C(EnSkb* thisv) {
    Animation_Change(&thisv->skelAnime, &gStalchildAttackingAnim, 0.6f, 0.0f,
                     Animation_GetLastFrame(&gStalchildAttackingAnim), ANIMMODE_ONCE_INTERP, 4.0f);
    thisv->collider.base.atFlags &= ~4;
    thisv->unk_280 = 3;
    thisv->actor.speedXZ = 0.0f;
    EnSkb_SetupAction(thisv, EnSkb_SetupAttack);
}

void EnSkb_SetupAttack(EnSkb* thisv, GlobalContext* globalCtx) {
    s32 frameData;

    frameData = thisv->skelAnime.curFrame;
    if (frameData == 3) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALKID_ATTACK);
        thisv->unk_281 = 1;
    } else if (frameData == 6) {
        thisv->unk_281 = 0;
    }
    if (thisv->collider.base.atFlags & 4) {
        thisv->collider.base.atFlags &= ~6;
        func_80AFD47C(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        func_80AFCD60(thisv);
    }
}

void func_80AFD47C(EnSkb* thisv) {
    Animation_Change(&thisv->skelAnime, &gStalchildAttackingAnim, -0.4f, thisv->skelAnime.curFrame - 1.0f, 0.0f,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    thisv->collider.base.atFlags &= ~4;
    thisv->unk_280 = 5;
    thisv->unk_281 = 0;
    EnSkb_SetupAction(thisv, func_80AFD508);
}

void func_80AFD508(EnSkb* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        func_80AFCD60(thisv);
    }
}

void EnSkb_SetupStunned(EnSkb* thisv) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = 0.0f;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->unk_281 = 0;
    thisv->unk_280 = 6;
    EnSkb_SetupAction(thisv, func_80AFD59C);
}

void func_80AFD59C(EnSkb* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }
    }
    if ((thisv->actor.colorFilterTimer == 0) && (thisv->actor.bgCheckFlags & 1)) {
        if (thisv->actor.colChkInfo.health == 0) {
            func_80AFD7B4(thisv, globalCtx);
        } else {
            func_80AFCD60(thisv);
        }
    }
}

void func_80AFD644(EnSkb* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gStalchildDamagedAnim, -4.0f);
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = -4.0f;
    }
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALKID_DAMAGE);
    thisv->unk_280 = 2;
    EnSkb_SetupAction(thisv, func_80AFD6CC);
}

void func_80AFD6CC(EnSkb* thisv, GlobalContext* globalCtx) {
    // thisv cast is likely not real, but allows for a match
    u8* new_var;

    new_var = &thisv->unk_283;
    if ((thisv->unk_283 != 1) || BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, 1)) {
        if ((*new_var) != 0) {
            thisv->unk_283 = (*new_var) | 2;
        }
        if (thisv->actor.bgCheckFlags & 2) {
            thisv->actor.speedXZ = 0;
        }
        if (thisv->actor.bgCheckFlags & 1) {
            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ += 0.05f;
            }
        }

        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x1194, 0);
        if (SkelAnime_Update(&thisv->skelAnime) && (thisv->actor.bgCheckFlags & 1)) {
            func_80AFCD60(thisv);
        }
    }
}

void func_80AFD7B4(EnSkb* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gStalchildDyingAnim, -4.0f);
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = -6.0f;
    }
    thisv->unk_280 = 1;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    BodyBreak_Alloc(&thisv->bodyBreak, 18, globalCtx);
    thisv->unk_283 |= 4;
    EffectSsDeadSound_SpawnStationary(globalCtx, &thisv->actor.projectedPos, NA_SE_EN_STALKID_DEAD, 1, 1, 0x28);
    EnSkb_SetupAction(thisv, func_80AFD880);
}

void func_80AFD880(EnSkb* thisv, GlobalContext* globalCtx) {
    if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, 1)) {
        if (thisv->actor.scale.x == 0.01f) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x10);
        } else if (thisv->actor.scale.x <= 0.015f) {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_RUPEE_BLUE);
        } else {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_RUPEE_RED);
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_RUPEE_RED);
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, ITEM00_RUPEE_RED);
        }

        thisv->unk_283 |= 8;
        Actor_Kill(&thisv->actor);
    }
}

void func_80AFD968(EnSkb* thisv, GlobalContext* globalCtx) {
    s16 pad;
    s32 i;
    Vec3f flamePos;
    s16 scale;
    s16 phi_v1;
    Player* player;

    if ((thisv->unk_280 != 1) && (thisv->actor.bgCheckFlags & 0x60) && (thisv->actor.yDistToWater >= 40.0f)) {
        thisv->actor.colChkInfo.health = 0;
        thisv->unk_281 = 0;
        func_80AFD7B4(thisv, globalCtx);
    } else if (thisv->unk_280 >= 3) {
        if ((thisv->collider.base.acFlags & 2) != 0) {
            thisv->collider.base.acFlags &= ~2;
            if (thisv->actor.colChkInfo.damageEffect != 6) {
                thisv->unk_282 = thisv->actor.colChkInfo.damageEffect;
                Actor_SetDropFlag(&thisv->actor, &thisv->collider.elements[1].info, 1);
                thisv->unk_281 = 0;
                if (thisv->actor.colChkInfo.damageEffect == 1) {
                    if (thisv->unk_280 != 6) {
                        Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);
                        Actor_ApplyDamage(&thisv->actor);
                        EnSkb_SetupStunned(thisv);
                    }
                } else {
                    phi_v1 = 8;
                    if (thisv->actor.colChkInfo.damageEffect == 7) {
                        scale = thisv->actor.scale.y * 7500.0f;
                        for (i = 4; i >= 0; i--) {
                            flamePos = thisv->actor.world.pos;
                            flamePos.x += Rand_CenteredFloat(20.0f);
                            flamePos.z += Rand_CenteredFloat(20.0f);
                            flamePos.y += (Rand_ZeroOne() * 25.0f);
                            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &flamePos, scale, 0, 0, -1);
                        }
                        phi_v1 = 25;
                    }
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, phi_v1);
                    if (!Actor_ApplyDamage(&thisv->actor)) {
                        func_80AFD7B4(thisv, globalCtx);
                        return;
                    }
                    player = GET_PLAYER(globalCtx);
                    if (thisv->unk_283 == 0) {
                        if ((thisv->actor.colChkInfo.damageEffect == 0xD) ||
                            ((thisv->actor.colChkInfo.damageEffect == 0xE) &&
                             ((player->swordAnimation >= 4 && player->swordAnimation <= 11) ||
                              (player->swordAnimation == 20 || player->swordAnimation == 21)))) {
                            BodyBreak_Alloc(&thisv->bodyBreak, 2, globalCtx);
                            thisv->unk_283 = 1;
                        }
                    }
                    func_80AFD644(thisv);
                }
            }
        }
    }
}

void EnSkb_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSkb* thisv = (EnSkb*)thisx;
    s32 pad;

    func_80AFD968(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 15.0f, 30.0f, 60.0f, 0x1D);
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += (3000.0f * thisv->actor.scale.y);
    if (thisv->unk_281 != 0) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->unk_280 >= 3) {
        if ((thisv->actor.colorFilterTimer == 0) || ((thisv->actor.colorFilterParams & 0x4000) == 0)) {

            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnSkb_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSkb* thisv = (EnSkb*)thisx;
    s16 color;
    s16 pad[2];

    if (limbIndex == 11) {
        if ((thisv->unk_283 & 2) == 0) {
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_skb.c", 972);
            color = ABS((s16)(Math_SinS((globalCtx->gameplayFrames * 0x1770)) * 95.0f)) + 160;
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, color, color, color, 255);
            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_skb.c", 978);
        } else {
            *dList = NULL;
        }
    } else if ((limbIndex == 12) && ((thisv->unk_283 & 2) != 0)) {
        *dList = NULL;
    }
    return 0;
}

void EnSkb_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnSkb* thisv = (EnSkb*)thisx;

    Collider_UpdateSpheres(limbIndex, &thisv->collider);

    if ((thisv->unk_283 ^ 1) == 0) {
        BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 11, 12, 18, dList, BODYBREAK_OBJECT_DEFAULT);
    } else if ((thisv->unk_283 ^ (thisv->unk_283 | 4)) == 0) {
        BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 0, 18, 18, dList, BODYBREAK_OBJECT_DEFAULT);
    }
}

void EnSkb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSkb* thisv = (EnSkb*)thisx;
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnSkb_OverrideLimbDraw,
                      EnSkb_PostLimbDraw, &thisv->actor);
}
