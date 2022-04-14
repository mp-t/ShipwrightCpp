/*
 * File: z_en_tite.c
 * Overlay: ovl_En_Tite
 * Description: Tektite
 */

#include "z_en_tite.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"
#include "vt.h"
#include "objects/object_tite/object_tite.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

// EnTite_Idle
#define vIdleTimer actionVar1

// EnTite_Attack (vQueuedJumps also used by EnTite_MoveTowardPlayer)
#define vAttackState actionVar1
#define vQueuedJumps actionVar2

//  EnTite_FlipOnBack
#define vOnBackTimer actionVar1
#define vLegTwitchTimer actionVar2

typedef enum {
    /* 0x0 */ TEKTITE_DEATH_CRY,
    /* 0x1 */ TEKTITE_UNK_1,
    /* 0x2 */ TEKTITE_UNK_2,
    /* 0x3 */ TEKTITE_RECOIL,
    /* 0x4 */ TEKTITE_UNK_4,
    /* 0x5 */ TEKTITE_FALL_APART,
    /* 0x6 */ TEKTITE_IDLE,
    /* 0x7 */ TEKTITE_STUNNED,
    /* 0x8 */ TEKTITE_UNK_8,
    /* 0x9 */ TEKTITE_ATTACK,
    /* 0xA */ TEKTITE_TURN_TOWARD_PLAYER,
    /* 0xB */ TEKTITE_UNK9,
    /* 0xC */ TEKTITE_MOVE_TOWARD_PLAYER
} EnTiteAction;

typedef enum {
    /* 0x0 */ TEKTITE_BEGIN_LUNGE,
    /* 0x1 */ TEKTITE_MID_LUNGE,
    /* 0x2 */ TEKTITE_LANDED,
    /* 0x2 */ TEKTITE_SUBMERGED
} EnTiteAttackState;

typedef enum {
    /* 0x0 */ TEKTITE_INITIAL,
    /* 0x1 */ TEKTITE_UNFLIPPED,
    /* 0x2 */ TEKTITE_FLIPPED
} EnTiteFlipState;

void EnTite_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTite_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTite_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTite_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnTite_SetupIdle(EnTite* thisv);
void EnTite_SetupTurnTowardPlayer(EnTite* thisv);
void EnTite_SetupMoveTowardPlayer(EnTite* thisv);
void EnTite_SetupDeathCry(EnTite* thisv);
void EnTite_SetupFlipUpright(EnTite* thisv);

void EnTite_Idle(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_Attack(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_TurnTowardPlayer(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_MoveTowardPlayer(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_Recoil(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_Stunned(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_DeathCry(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_FallApart(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_FlipOnBack(EnTite* thisv, GlobalContext* globalCtx);
void EnTite_FlipUpright(EnTite* thisv, GlobalContext* globalCtx);

const ActorInit En_Tite_InitVars = {
    ACTOR_EN_TITE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_TITE,
    sizeof(EnTite),
    (ActorFunc)EnTite_Init,
    (ActorFunc)EnTite_Destroy,
    (ActorFunc)EnTite_Update,
    (ActorFunc)EnTite_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 0, { { 0, 1500, 0 }, 20 }, 100 },
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
    1,
    sJntSphElementsInit,
};

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xE),
    /* Ice magic     */ DMG_ENTRY(3, 0xF),
    /* Light magic   */ DMG_ENTRY(0, 0xE),
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
    ICHAIN_S8(naviEnemyId, 0x45, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -40, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

static AnimationHeader* D_80B1B634[] = {
    &object_tite_Anim_00083C, &object_tite_Anim_0004F8, &object_tite_Anim_00069C, NULL, NULL, NULL,
};

// Some kind of offset for the position of each tektite foot
static Vec3f sFootOffset = { 2800.0f, -200.0f, 0.0f };

// Relative positions to spawn ice chunks when tektite is frozen
static Vec3f sIceChunks[12] = {
    { 20.0f, 20.0f, 0.0f },   { 10.0f, 40.0f, 10.0f },   { -10.0f, 40.0f, 10.0f }, { -20.0f, 20.0f, 0.0f },
    { 10.0f, 40.0f, -10.0f }, { -10.0f, 40.0f, -10.0f }, { 0.0f, 20.0f, -20.0f },  { 10.0f, 0.0f, 10.0f },
    { 10.0f, 0.0f, -10.0f },  { 0.0f, 20.0f, 20.0f },    { -10.0f, 0.0f, 10.0f },  { -10.0f, 0.0f, -10.0f },
};

void EnTite_SetupAction(EnTite* thisv, EnTiteActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnTite_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnTite* thisv = (EnTite*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->targetMode = 3;
    Actor_SetScale(thisx, 0.01f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_tite_Skel_003A20, &object_tite_Anim_0012E4, thisv->jointTable,
                   thisv->morphTable, 25);
    ActorShape_Init(&thisx->shape, -200.0f, ActorShadow_DrawCircle, 70.0f);
    thisv->flipState = TEKTITE_INITIAL;
    thisx->colChkInfo.damageTable = sDamageTable;
    thisv->actionVar1 = 0;
    thisv->bodyBreak.val = BODYBREAK_STATUS_FINISHED;
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 20.0f;
    thisx->colChkInfo.health = 2;
    thisx->colChkInfo.mass = MASS_HEAVY;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, thisx, &sJntSphInit, &thisv->colliderItem);
    thisv->unk_2DC = 0x1D;
    if (thisv->actor.params == TEKTITE_BLUE) {
        thisv->unk_2DC |= 0x40; // Don't use the actor engine's ripple spawning code
        thisx->colChkInfo.health = 4;
        thisx->naviEnemyId += 1;
    }
    EnTite_SetupIdle(thisv);
}

void EnTite_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnTite* thisv = (EnTite*)thisx;
    EnEncount1* spawner;

    if (thisx->parent != NULL) {
        spawner = (EnEncount1*)thisx->parent;
        if (spawner->curNumSpawn > 0) {
            spawner->curNumSpawn--;
        }
        osSyncPrintf("\n\n");
        // "Number of simultaneous occurrences"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 同時発生数 ☆☆☆☆☆%d\n" VT_RST, spawner->curNumSpawn);
        osSyncPrintf("\n\n");
    }
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnTite_SetupIdle(EnTite* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_tite_Anim_0012E4, 4.0f);
    thisv->action = TEKTITE_IDLE;
    thisv->vIdleTimer = Rand_S16Offset(15, 30);
    thisv->actor.speedXZ = 0.0f;
    EnTite_SetupAction(thisv, EnTite_Idle);
}

void EnTite_Idle(EnTite* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (thisv->actor.params == TEKTITE_BLUE) {
        if (thisv->actor.bgCheckFlags & 0x20) {
            // Float on water surface
            thisv->actor.gravity = 0.0f;
            Math_SmoothStepToF(&thisv->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.world.pos.y + thisv->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
        } else {
            thisv->actor.gravity = -1.0f;
        }
    }
    if ((thisv->actor.bgCheckFlags & 3) && (thisv->actor.velocity.y <= 0.0f)) {
        thisv->actor.velocity.y = 0.0f;
    }
    if (thisv->vIdleTimer > 0) {
        thisv->vIdleTimer--;
    } else if ((thisv->actor.xzDistToPlayer < 300.0f) && (thisv->actor.yDistToPlayer <= 80.0f)) {
        EnTite_SetupTurnTowardPlayer(thisv);
    }
}

void EnTite_SetupAttack(EnTite* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &object_tite_Anim_00083C);
    thisv->action = TEKTITE_ATTACK;
    thisv->vAttackState = TEKTITE_BEGIN_LUNGE;
    thisv->vQueuedJumps = Rand_S16Offset(1, 3);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnTite_SetupAction(thisv, EnTite_Attack);
}

void EnTite_Attack(EnTite* thisv, GlobalContext* globalCtx) {
    s16 angleToPlayer;
    s32 attackState;
    Vec3f ripplePos;

    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        attackState = thisv->vAttackState; // for deciding whether to change animation
        switch (thisv->vAttackState) {
            case TEKTITE_BEGIN_LUNGE:
                // Snap to ground or water, then lunge into the air with some initial speed
                thisv->vAttackState = TEKTITE_MID_LUNGE;
                if ((thisv->actor.params != TEKTITE_BLUE) || !(thisv->actor.bgCheckFlags & 0x20)) {
                    if (thisv->actor.floorHeight > BGCHECK_Y_MIN) {
                        thisv->actor.world.pos.y = thisv->actor.floorHeight;
                    }
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
                } else {
                    thisv->actor.world.pos.y += thisv->actor.yDistToWater;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_JUMP_WATER);
                }
                thisv->actor.velocity.y = 8.0f;
                thisv->actor.gravity = -1.0f;
                thisv->actor.speedXZ = 4.0f;
                break;
            case TEKTITE_MID_LUNGE:
                // Continue trajectory until tektite has negative velocity and has landed on ground/water surface
                // Snap to ground/water surface, or if falling fast dip into the water and slow fall speed
                thisv->actor.flags |= ACTOR_FLAG_24;
                if ((thisv->actor.bgCheckFlags & 3) ||
                    ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20))) {
                    if (thisv->actor.velocity.y <= 0.0f) {
                        thisv->vAttackState = TEKTITE_LANDED;
                        if ((thisv->actor.params != TEKTITE_BLUE) || !(thisv->actor.bgCheckFlags & 0x20)) {
                            if (BGCHECK_Y_MIN < thisv->actor.floorHeight) {
                                thisv->actor.world.pos.y = thisv->actor.floorHeight;
                            }
                            thisv->actor.velocity.y = 0.0f;
                            thisv->actor.speedXZ = 0.0f;
                        } else {
                            thisv->actor.gravity = 0.0f;
                            if (thisv->actor.velocity.y < -8.0f) {
                                ripplePos = thisv->actor.world.pos;
                                ripplePos.y += thisv->actor.yDistToWater;
                                thisv->vAttackState++; // TEKTITE_SUBMERGED
                                thisv->actor.velocity.y *= 0.75f;
                                attackState = thisv->vAttackState;
                                EffectSsGRipple_Spawn(globalCtx, &ripplePos, 0, 500, 0);
                            } else {
                                thisv->actor.velocity.y = 0.0f;
                                thisv->actor.speedXZ = 0.0f;
                            }
                        }
                        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
                    }
                }
                break;
            case TEKTITE_LANDED:
                // Get ready to begin another lunge if more lunges are queued, otherwise start turning
                if (thisv->vQueuedJumps != 0) {
                    thisv->vQueuedJumps--;
                    thisv->vAttackState = TEKTITE_BEGIN_LUNGE;
                    thisv->collider.base.atFlags &= ~AT_HIT;
                } else {
                    EnTite_SetupTurnTowardPlayer(thisv);
                }
                break;
            case TEKTITE_SUBMERGED:
                // Check if floated to surface
                if (thisv->actor.yDistToWater == 0.0f) {
                    thisv->vAttackState = TEKTITE_LANDED;
                    attackState = thisv->vAttackState;
                }
                break;
        }
        // If switching attack state, change animation (unless tektite is switching between submerged and landed)
        if (attackState != thisv->vAttackState) {
            Animation_PlayOnce(&thisv->skelAnime, D_80B1B634[thisv->vAttackState]);
        }
    }

    switch (thisv->vAttackState) {
        case TEKTITE_BEGIN_LUNGE:
            // Slightly turn to player and switch to turning/idling action if the player is too far
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 1000, 0);
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
            angleToPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
            if ((thisv->actor.xzDistToPlayer > 300.0f) && (thisv->actor.yDistToPlayer > 80.0f)) {
                EnTite_SetupIdle(thisv);
            } else if (ABS(angleToPlayer) >= 9000) {
                EnTite_SetupTurnTowardPlayer(thisv);
            }
            break;
        case TEKTITE_MID_LUNGE:
            // Generate sparkles at feet upon landing, set jumping animation and hurtbox and check if hit player
            if (thisv->actor.velocity.y >= 5.0f) {
                if (thisv->actor.bgCheckFlags & 1) {
                    func_800355B8(globalCtx, &thisv->frontLeftFootPos);
                    func_800355B8(globalCtx, &thisv->frontRightFootPos);
                    func_800355B8(globalCtx, &thisv->backRightFootPos);
                    func_800355B8(globalCtx, &thisv->backLeftFootPos);
                }
            }
            if (!(thisv->collider.base.atFlags & AT_HIT) && (thisv->actor.flags & ACTOR_FLAG_6)) {
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            } else {
                Player* player = GET_PLAYER(globalCtx);
                thisv->collider.base.atFlags &= ~AT_HIT;
                Animation_MorphToLoop(&thisv->skelAnime, &object_tite_Anim_0012E4, 4.0f);
                thisv->actor.speedXZ = -6.0f;
                thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                if (&player->actor == thisv->collider.base.at) {
                    if (!(thisv->collider.base.atFlags & AT_BOUNCED)) {
                        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                    }
                }
                EnTite_SetupAction(thisv, EnTite_Recoil);
            }
            break;
        case TEKTITE_LANDED:
            // Slightly turn to player
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 1500, 0);
            break;
        case TEKTITE_SUBMERGED:
            // Float up to water surface
            Math_SmoothStepToF(&thisv->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.world.pos.y + thisv->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
            break;
    }
    // Create ripples on water surface where tektite feet landed
    if (thisv->actor.bgCheckFlags & 2) {
        if (!(thisv->actor.bgCheckFlags & 0x20)) {
            func_80033480(globalCtx, &thisv->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backLeftFootPos, 1.0f, 2, 80, 15, 1);
        }
    }
    // if landed, kill XZ speed and play appropriate sounds
    if (thisv->actor.params == TEKTITE_BLUE) {
        if (thisv->actor.bgCheckFlags & 0x40) {
            thisv->actor.speedXZ = 0.0f;
            if (thisv->vAttackState == TEKTITE_SUBMERGED) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_LAND_WATER);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_LAND_WATER2);
            }
            thisv->actor.bgCheckFlags &= ~0x40;
        } else if (thisv->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
    } else if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    }
}

void EnTite_SetupTurnTowardPlayer(EnTite* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_tite_Anim_000A14);
    thisv->action = TEKTITE_TURN_TOWARD_PLAYER;
    if ((thisv->actor.bgCheckFlags & 3) || ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20))) {
        if (thisv->actor.velocity.y <= 0.0f) {
            thisv->actor.gravity = 0.0f;
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.speedXZ = 0.0f;
        }
    }
    EnTite_SetupAction(thisv, EnTite_TurnTowardPlayer);
}

void EnTite_TurnTowardPlayer(EnTite* thisv, GlobalContext* globalCtx) {
    s16 angleToPlayer;
    s16 turnVelocity;

    if (((thisv->actor.bgCheckFlags & 3) ||
         ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20))) &&
        (thisv->actor.velocity.y <= 0.0f)) {
        thisv->actor.gravity = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }
    // Calculate turn velocity and animation speed based on angle to player
    if ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)) {
        thisv->actor.world.pos.y += thisv->actor.yDistToWater;
    }
    angleToPlayer = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;
    if (angleToPlayer > 0) {
        turnVelocity = (angleToPlayer / 42.0f) + 10.0f;
        thisv->actor.world.rot.y += (turnVelocity * 2);
    } else {
        turnVelocity = (angleToPlayer / 42.0f) - 10.0f;
        thisv->actor.world.rot.y += (turnVelocity * 2);
    }
    if (angleToPlayer > 0) {
        thisv->skelAnime.playSpeed = turnVelocity * 0.01f;
    } else {
        thisv->skelAnime.playSpeed = turnVelocity * 0.01f;
    }

    /**
     * Play sounds once every animation cycle
     */
    SkelAnime_Update(&thisv->skelAnime);
    if (((s16)thisv->skelAnime.curFrame & 7) == 0) {
        if ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_WALK_WATER);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_WALK);
        }
    }

    // Idle if player is far enough away from the tektite, move or attack if almost facing player
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if ((thisv->actor.xzDistToPlayer > 300.0f) && (thisv->actor.yDistToPlayer > 80.0f)) {
        EnTite_SetupIdle(thisv);
    } else if (Actor_IsFacingPlayer(&thisv->actor, 3640)) {
        if ((thisv->actor.xzDistToPlayer <= 180.0f) && (thisv->actor.yDistToPlayer <= 80.0f)) {
            EnTite_SetupAttack(thisv);
        } else {
            EnTite_SetupMoveTowardPlayer(thisv);
        }
    }
}

void EnTite_SetupMoveTowardPlayer(EnTite* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_tite_Anim_000C70);
    thisv->action = TEKTITE_MOVE_TOWARD_PLAYER;
    thisv->actor.velocity.y = 10.0f;
    thisv->actor.gravity = -1.0f;
    thisv->actor.speedXZ = 4.0f;
    thisv->vQueuedJumps = Rand_S16Offset(1, 3);
    if ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_JUMP_WATER);
    } else {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    }
    EnTite_SetupAction(thisv, EnTite_MoveTowardPlayer);
}

/**
 *  Jumping toward player as a method of travel (different from attacking, has no hitbox)
 */
void EnTite_MoveTowardPlayer(EnTite* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 1.0f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.bgCheckFlags & 0x42) {
        if (!(thisv->actor.bgCheckFlags & 0x40)) {
            func_80033480(globalCtx, &thisv->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_LAND_WATER);
        }
    }

    if ((thisv->actor.bgCheckFlags & 2) || ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x40))) {
        if (thisv->vQueuedJumps != 0) {
            thisv->vQueuedJumps--;
        } else {
            EnTite_SetupIdle(thisv);
        }
    }

    if (((thisv->actor.bgCheckFlags & 3) || (thisv->actor.params == TEKTITE_BLUE && (thisv->actor.bgCheckFlags & 0x60))) &&
        (thisv->actor.velocity.y <= 0.0f)) {
        // slightly turn toward player upon landing and snap to ground or water.
        thisv->actor.speedXZ = 0.0f;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4000, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if ((thisv->actor.params != TEKTITE_BLUE) || !(thisv->actor.bgCheckFlags & 0x20)) {
            if (thisv->actor.floorHeight > BGCHECK_Y_MIN) {
                thisv->actor.world.pos.y = thisv->actor.floorHeight;
            }
        } else if (thisv->actor.bgCheckFlags & 0x40) {
            Vec3f ripplePos = thisv->actor.world.pos;
            thisv->actor.bgCheckFlags &= ~0x40;
            ripplePos.y += thisv->actor.yDistToWater;
            thisv->actor.gravity = 0.0f;
            thisv->actor.velocity.y *= 0.75f;
            EffectSsGRipple_Spawn(globalCtx, &ripplePos, 0, 500, 0);
            return;
        } else {
            // If submerged, float to surface
            Math_SmoothStepToF(&thisv->actor.velocity.y, 0.0f, 1.0f, 2.0f, 0.0f);
            Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.world.pos.y + thisv->actor.yDistToWater, 1.0f, 2.0f,
                               0.0f);
            if (thisv->actor.yDistToWater != 0.0f) {
                // Do not change state until tekite has floated to surface
                return;
            }
        }

        // Idle or turn if player is too far away, otherwise keep jumping
        if (((thisv->actor.xzDistToPlayer > 300.0f) && (thisv->actor.yDistToPlayer > 80.0f))) {
            EnTite_SetupIdle(thisv);
        } else if (((thisv->actor.xzDistToPlayer <= 180.0f)) && ((thisv->actor.yDistToPlayer <= 80.0f))) {
            if (thisv->vQueuedJumps <= 0) {
                EnTite_SetupTurnTowardPlayer(thisv);
            } else {
                thisv->actor.velocity.y = 10.0f;
                thisv->actor.speedXZ = 4.0f;
                thisv->actor.flags |= ACTOR_FLAG_24;
                thisv->actor.gravity = -1.0f;
                if ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_JUMP_WATER);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
                }
            }
        } else {
            thisv->actor.velocity.y = 10.0f;
            thisv->actor.speedXZ = 4.0f;
            thisv->actor.flags |= ACTOR_FLAG_24;
            thisv->actor.gravity = -1.0f;
            if ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_JUMP_WATER);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
            }
        }
        // If in midair:
    } else {
        // Turn slowly toward player
        thisv->actor.flags |= ACTOR_FLAG_24;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 1000, 0);
        if (thisv->actor.velocity.y >= 6.0f) {
            if (thisv->actor.bgCheckFlags & 1) {
                func_800355B8(globalCtx, &thisv->frontLeftFootPos);
                func_800355B8(globalCtx, &thisv->frontRightFootPos);
                func_800355B8(globalCtx, &thisv->backRightFootPos);
                func_800355B8(globalCtx, &thisv->backLeftFootPos);
            }
        }
    }
}

void EnTite_SetupRecoil(EnTite* thisv) {
    thisv->action = TEKTITE_RECOIL;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tite_Anim_0012E4, 4.0f);
    thisv->actor.speedXZ = -6.0f;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actor.gravity = -1.0f;
    EnTite_SetupAction(thisv, EnTite_Recoil);
}

/**
 * After tektite hits or gets hit, recoils backwards and slides a bit upon landing
 */
void EnTite_Recoil(EnTite* thisv, GlobalContext* globalCtx) {
    s16 angleToPlayer;

    // Snap to ground or water surface upon landing
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (((thisv->actor.bgCheckFlags & 3) || (thisv->actor.params == TEKTITE_BLUE && (thisv->actor.bgCheckFlags & 0x20))) &&
        (thisv->actor.velocity.y <= 0.0f)) {
        if ((thisv->actor.params != TEKTITE_BLUE) || !(thisv->actor.bgCheckFlags & 0x20)) {
            if (thisv->actor.floorHeight > BGCHECK_Y_MIN) {
                thisv->actor.world.pos.y = thisv->actor.floorHeight;
            }
        } else {
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.gravity = 0.0f;
            thisv->actor.world.pos.y += thisv->actor.yDistToWater;
        }
    }

    // play sound and generate ripples
    if (thisv->actor.bgCheckFlags & 0x42) {
        if (!(thisv->actor.bgCheckFlags & 0x40)) {
            func_80033480(globalCtx, &thisv->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        } else {
            thisv->actor.bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_LAND_WATER2);
        }
    }

    // If player is far away, idle. Otherwise attack or move
    angleToPlayer = (thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y);
    if ((thisv->actor.speedXZ == 0.0f) && ((thisv->actor.bgCheckFlags & 1) || ((thisv->actor.params == TEKTITE_BLUE) &&
                                                                             (thisv->actor.bgCheckFlags & 0x20)))) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        thisv->collider.base.atFlags &= ~AT_HIT;
        if ((thisv->actor.xzDistToPlayer > 300.0f) && (thisv->actor.yDistToPlayer > 80.0f) &&
            (ABS(thisv->actor.shape.rot.x) < 4000) && (ABS(thisv->actor.shape.rot.z) < 4000) &&
            ((thisv->actor.bgCheckFlags & 1) ||
             ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)))) {
            EnTite_SetupIdle(thisv);
        } else if ((thisv->actor.xzDistToPlayer < 180.0f) && (thisv->actor.yDistToPlayer <= 80.0f) &&
                   (ABS(angleToPlayer) <= 6000)) {
            EnTite_SetupAttack(thisv);
        } else {
            EnTite_SetupMoveTowardPlayer(thisv);
        }
    }
    SkelAnime_Update(&thisv->skelAnime);
}

void EnTite_SetupStunned(EnTite* thisv) {
    Animation_Change(&thisv->skelAnime, &object_tite_Anim_0012E4, 0.0f, 0.0f,
                     (f32)Animation_GetLastFrame(&object_tite_Anim_0012E4), ANIMMODE_LOOP, 4.0f);
    thisv->action = TEKTITE_STUNNED;
    thisv->actor.speedXZ = -6.0f;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->damageEffect == 0xF) {
        thisv->spawnIceTimer = 48;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnTite_SetupAction(thisv, EnTite_Stunned);
}

/**
 * stunned or frozen
 */
void EnTite_Stunned(EnTite* thisv, GlobalContext* globalCtx) {
    s16 angleToPlayer;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    // Snap to ground or water
    if (((thisv->actor.bgCheckFlags & 3) ||
         ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20))) &&
        (thisv->actor.velocity.y <= 0.0f)) {
        if (((thisv->actor.params != TEKTITE_BLUE) || !(thisv->actor.bgCheckFlags & 0x20))) {
            if (thisv->actor.floorHeight > BGCHECK_Y_MIN) {
                thisv->actor.world.pos.y = thisv->actor.floorHeight;
            }
        } else {
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.gravity = 0.0f;
            thisv->actor.world.pos.y += thisv->actor.yDistToWater;
        }
    }
    // Play sounds and spawn dirt effects upon landing
    if (thisv->actor.bgCheckFlags & 0x42) {
        if (!(thisv->actor.bgCheckFlags & 0x40)) {
            func_80033480(globalCtx, &thisv->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->frontRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backRightFootPos, 1.0f, 2, 80, 15, 1);
            func_80033480(globalCtx, &thisv->backLeftFootPos, 1.0f, 2, 80, 15, 1);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        } else {
            thisv->actor.bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_LAND_WATER2);
        }
    }
    // Decide on next action based on health, flip state and player distance
    angleToPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if (((thisv->actor.colorFilterTimer == 0) && (thisv->actor.speedXZ == 0.0f)) &&
        ((thisv->actor.bgCheckFlags & 1) ||
         ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)))) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if (thisv->actor.colChkInfo.health == 0) {
            EnTite_SetupDeathCry(thisv);
        } else if (thisv->flipState == TEKTITE_FLIPPED) {
            EnTite_SetupFlipUpright(thisv);
        } else if (((thisv->actor.xzDistToPlayer > 300.0f) && (thisv->actor.yDistToPlayer > 80.0f) &&
                    (ABS(thisv->actor.shape.rot.x) < 4000) && (ABS(thisv->actor.shape.rot.z) < 4000)) &&
                   ((thisv->actor.bgCheckFlags & 1) ||
                    ((thisv->actor.params == TEKTITE_BLUE) && (thisv->actor.bgCheckFlags & 0x20)))) {
            EnTite_SetupIdle(thisv);
        } else if ((thisv->actor.xzDistToPlayer < 180.0f) && (thisv->actor.yDistToPlayer <= 80.0f) &&
                   (ABS(angleToPlayer) <= 6000)) {
            EnTite_SetupAttack(thisv);
        } else {
            EnTite_SetupMoveTowardPlayer(thisv);
        }
    }
    SkelAnime_Update(&thisv->skelAnime);
}

void EnTite_SetupDeathCry(EnTite* thisv) {
    thisv->action = TEKTITE_DEATH_CRY;
    thisv->actor.colorFilterTimer = 0;
    thisv->actor.speedXZ = 0.0f;
    EnTite_SetupAction(thisv, EnTite_DeathCry);
}

/**
 * First frame of death. Scream in pain and allocate memory for EnPart data
 */
void EnTite_DeathCry(EnTite* thisv, GlobalContext* globalCtx) {
    EffectSsDeadSound_SpawnStationary(globalCtx, &thisv->actor.projectedPos, NA_SE_EN_TEKU_DEAD, true,
                                      DEADSOUND_REPEAT_MODE_OFF, 40);
    thisv->action = TEKTITE_FALL_APART;
    EnTite_SetupAction(thisv, EnTite_FallApart);
    BodyBreak_Alloc(&thisv->bodyBreak, 24, globalCtx);
}

/**
 * Spawn EnPart and drop items
 */
void EnTite_FallApart(EnTite* thisv, GlobalContext* globalCtx) {
    if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, thisv->actor.params + 0xB)) {
        if (thisv->actor.params == TEKTITE_BLUE) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xE0);
        } else {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x40);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnTite_SetupFlipOnBack(EnTite* thisv) {

    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &object_tite_Anim_000A14, 1.5f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_REVERSE);
    thisv->flipState = TEKTITE_FLIPPED;
    thisv->vOnBackTimer = 500;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = -1.0f;
    thisv->vLegTwitchTimer = (Rand_ZeroOne() * 50.0f);
    thisv->actor.velocity.y = 11.0f;
    EnTite_SetupAction(thisv, EnTite_FlipOnBack);
}

/**
 * During the flip animation and also while idling on back
 */
void EnTite_FlipOnBack(EnTite* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, 0x7FFF, 1, 4000, 0);
    // randomly reset the leg wiggling animation whenever timer reaches 0 to give illusion of twitching legs
    thisv->vLegTwitchTimer--;
    if (thisv->vLegTwitchTimer == 0) {
        thisv->vLegTwitchTimer = Rand_ZeroOne() * 30.0f;
        thisv->skelAnime.curFrame = Rand_ZeroOne() * 5.0f;
    }
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.bgCheckFlags & 3) {
        // Upon landing, spawn dust and make noise
        if (thisv->actor.bgCheckFlags & 2) {
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 20.0f, 0xB, 4.0f, 0, 0, 0);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
        thisv->vOnBackTimer--;
        if (thisv->vOnBackTimer == 0) {
            EnTite_SetupFlipUpright(thisv);
        }
    } else {
        // Gradually increase y offset during flip so that the actor position is at tektite's back instead of feet
        if (thisv->actor.shape.yOffset < 2800.0f) {
            thisv->actor.shape.yOffset += 400.0f;
        }
    }
}

void EnTite_SetupFlipUpright(EnTite* thisv) {
    thisv->flipState = TEKTITE_UNFLIPPED;
    thisv->actionVar1 = 1000; // value unused here and overwritten in SetupIdle
    //! @bug flying tektite: water sets gravity to 0 so y velocity will never decrease from 13
    thisv->actor.velocity.y = 13.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TEKU_REVERSE);
    EnTite_SetupAction(thisv, EnTite_FlipUpright);
}

void EnTite_FlipUpright(EnTite* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, 0, 1, 0xFA0, 0);
    SkelAnime_Update(&thisv->skelAnime);
    //! @bug flying tektite: the following condition is never met and tektite stays stuck in thisv action forever
    if (thisv->actor.bgCheckFlags & 2) {
        func_80033480(globalCtx, &thisv->frontLeftFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(globalCtx, &thisv->frontRightFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(globalCtx, &thisv->backRightFootPos, 1.0f, 2, 80, 15, 1);
        func_80033480(globalCtx, &thisv->backLeftFootPos, 1.0f, 2, 80, 15, 1);
        thisv->actor.shape.yOffset = 0.0f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        EnTite_SetupIdle(thisv);
    }
}

void EnTite_CheckDamage(Actor* thisx, GlobalContext* globalCtx) {
    EnTite* thisv = (EnTite*)thisx;

    if ((thisv->collider.base.acFlags & AC_HIT) && (thisv->action >= TEKTITE_IDLE)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisx->colChkInfo.damageEffect != 0xE) { // Immune to fire magic
            thisv->damageEffect = thisx->colChkInfo.damageEffect;
            Actor_SetDropFlag(thisx, &thisv->collider.elements[0].info, 0);
            // Stun if Tektite hit by nut, boomerang, hookshot, ice arrow or ice magic
            if ((thisx->colChkInfo.damageEffect == 1) || (thisx->colChkInfo.damageEffect == 0xF)) {
                if (thisv->action != TEKTITE_STUNNED) {
                    Actor_SetColorFilter(thisx, 0, 0x78, 0, 0x50);
                    Actor_ApplyDamage(thisx);
                    EnTite_SetupStunned(thisv);
                }
                // Otherwise apply damage and handle death where necessary
            } else {
                if ((thisx->colorFilterTimer == 0) || ((thisx->colorFilterParams & 0x4000) == 0)) {
                    Actor_SetColorFilter(thisx, 0x4000, 0xFF, 0, 8);
                    Actor_ApplyDamage(thisx);
                }
                if (thisx->colChkInfo.health == 0) {
                    EnTite_SetupDeathCry(thisv);
                } else {
                    // Flip tektite back up if it's on its back
                    Audio_PlayActorSound2(thisx, NA_SE_EN_TEKU_DAMAGE);
                    if (thisv->flipState != TEKTITE_FLIPPED) {
                        EnTite_SetupRecoil(thisv);
                    } else {
                        EnTite_SetupFlipUpright(thisv);
                    }
                }
            }
        }
        // If hammer has recently hit the floor and player is close to tektite, flip over
    } else if ((thisx->colChkInfo.health != 0) && (globalCtx->actorCtx.unk_02 != 0) &&
               (thisx->xzDistToPlayer <= 400.0f) && (thisx->bgCheckFlags & 1)) {
        if (thisv->flipState == TEKTITE_FLIPPED) {
            EnTite_SetupFlipUpright(thisv);
        } else if ((thisv->action >= TEKTITE_IDLE) || (thisv->action >= TEKTITE_IDLE)) {
            if (1) {}
            EnTite_SetupFlipOnBack(thisv);
        }
    }
}

void EnTite_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnTite* thisv = (EnTite*)thisx;
    char pad[0x4];
    CollisionPoly* floorPoly;
    WaterBox* waterBox;
    f32 waterSurfaceY;

    EnTite_CheckDamage(thisx, globalCtx);
    // Stay still if hit by immunity damage type thisv frame
    if (thisx->colChkInfo.damageEffect != 0xE) {
        thisv->actionFunc(thisv, globalCtx);
        Actor_MoveForward(thisx);
        Actor_UpdateBgCheckInfo(globalCtx, thisx, 25.0f, 40.0f, 20.0f, thisv->unk_2DC);
        // If on water, snap feet to surface and spawn ripples
        if ((thisv->actor.params == TEKTITE_BLUE) && (thisx->bgCheckFlags & 0x20)) {
            floorPoly = thisx->floorPoly;
            if ((((globalCtx->gameplayFrames % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->backRightFootPos.x,
                                         thisv->backRightFootPos.z, &waterSurfaceY, &waterBox)) &&
                (thisv->backRightFootPos.y <= waterSurfaceY)) {
                thisv->backRightFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(globalCtx, &thisv->backRightFootPos, 0, 220, 0);
            }
            if (((((globalCtx->gameplayFrames + 2) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->backLeftFootPos.x,
                                         thisv->backLeftFootPos.z, &waterSurfaceY, &waterBox)) &&
                (thisv->backLeftFootPos.y <= waterSurfaceY)) {
                thisv->backLeftFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(globalCtx, &thisv->backLeftFootPos, 0, 220, 0);
            }
            if (((((globalCtx->gameplayFrames + 4) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->frontLeftFootPos.x,
                                         thisv->frontLeftFootPos.z, &waterSurfaceY, &waterBox)) &&
                (thisv->frontLeftFootPos.y <= waterSurfaceY)) {
                thisv->frontLeftFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(globalCtx, &thisv->frontLeftFootPos, 0, 220, 0);
            }
            if (((((globalCtx->gameplayFrames + 1) % 8) == 0) || (thisx->velocity.y < 0.0f)) &&
                (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->frontRightFootPos.x,
                                         thisv->frontRightFootPos.z, &waterSurfaceY, &waterBox)) &&
                (thisv->frontRightFootPos.y <= waterSurfaceY)) {
                thisv->frontRightFootPos.y = waterSurfaceY;
                EffectSsGRipple_Spawn(globalCtx, &thisv->frontRightFootPos, 0, 220, 0);
            }
            thisx->floorPoly = floorPoly;
        }

        // If on ground and currently flipped over, set tektite to be fully upside-down
        if (thisx->bgCheckFlags & 3) {
            func_800359B8(thisx, thisx->shape.rot.y, &thisx->shape.rot);
            if (thisv->flipState >= TEKTITE_FLIPPED) {
                thisx->shape.rot.z += 0x7FFF;
            }
            // Otherwise ensure the tektite is rotating back upright
        } else {
            Math_SmoothStepToS(&thisx->shape.rot.x, 0, 1, 1000, 0);
            if (thisv->flipState <= TEKTITE_UNFLIPPED) {
                Math_SmoothStepToS(&thisx->shape.rot.z, 0, 1, 1000, 0);
                if (thisx->shape.yOffset > 0) {
                    thisx->shape.yOffset -= 400.0f;
                }
            }
        }
    }
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 20.0f;

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnTite_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** limbDList, Vec3s* rot, void* thisx) {
    EnTite* thisv = (EnTite*)thisx;

    switch (limbIndex) {
        case 8:
            Matrix_MultVec3f(&sFootOffset, &thisv->backRightFootPos);
            break;
        case 13:
            Matrix_MultVec3f(&sFootOffset, &thisv->frontRightFootPos);
            break;
        case 18:
            Matrix_MultVec3f(&sFootOffset, &thisv->backLeftFootPos);
            break;
        case 23:
            Matrix_MultVec3f(&sFootOffset, &thisv->frontLeftFootPos);
            break;
    }

    BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 0, 24, 24, limbDList, BODYBREAK_OBJECT_DEFAULT);
}

void EnTite_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnTite* thisv = (EnTite*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_tite.c", 1704);
    func_80093D18(globalCtx->state.gfxCtx);
    Collider_UpdateSpheres(0, &thisv->collider);
    if (thisv->actor.params == TEKTITE_BLUE) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001300));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001700));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001900));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001B00));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001F00));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_002100));
    }
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, EnTite_PostLimbDraw,
                      thisx);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_tite.c", 1735);

    if (thisv->spawnIceTimer != 0) {
        // Spawn chunks of ice all over the tektite's body
        thisx->colorFilterTimer++;
        thisv->spawnIceTimer--;
        if ((thisv->spawnIceTimer & 3) == 0) {
            Vec3f iceChunk;
            s32 idx = thisv->spawnIceTimer >> 2;

            iceChunk.x = thisx->world.pos.x + sIceChunks[idx].x;
            iceChunk.y = thisx->world.pos.y + sIceChunks[idx].y;
            iceChunk.z = thisx->world.pos.z + sIceChunks[idx].z;
            EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &iceChunk, 150, 150, 150, 250, 235, 245, 255, 1.0f);
        }
    }
}
