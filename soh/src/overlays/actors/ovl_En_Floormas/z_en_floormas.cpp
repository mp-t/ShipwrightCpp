/*
 * File: z_en_floormas
 * Overlay: En_Floormas
 * Description: Floormaster
 */

#include "z_en_floormas.h"
#include "objects/object_wallmaster/object_wallmaster.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_10)

#define SPAWN_INVISIBLE 0x8000
#define SPAWN_SMALL 0x10

#define MERGE_MASTER 0x40
#define MERGE_SLAVE 0x20

void EnFloormas_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFloormas_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFloormas_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFloormas_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFloormas_GrabLink(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Split(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Recover(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_DrawHighlighted(Actor* thisv, GlobalContext* globalCtx);
void EnFloormas_SmWait(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_SetupBigDecideAction(EnFloormas* thisv);
void EnFloormas_Freeze(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_TakeDamage(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Merge(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_JumpAtLink(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_SmSlaveJumpAtMaster(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_SmShrink(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_SmDecideAction(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_SmWalk(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Land(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Hover(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Turn(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Run(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_BigStopWalk(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_BigWalk(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Stand(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_BigDecideAction(EnFloormas* thisv, GlobalContext* globalCtx);
void EnFloormas_Charge(EnFloormas* thisv, GlobalContext* globalCtx);

const ActorInit En_Floormas_InitVars = {
    ACTOR_EN_FLOORMAS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_WALLMASTER,
    sizeof(EnFloormas),
    (ActorFunc)EnFloormas_Init,
    (ActorFunc)EnFloormas_Destroy,
    (ActorFunc)EnFloormas_Update,
    (ActorFunc)EnFloormas_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
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
        { 0xFFCFFFFF, 0x04, 0x10 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 30, 40, 150 };

static DamageTable sDamageTable = {
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
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(4, 0x4),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x4),
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
    ICHAIN_S8(naviEnemyId, 0x31, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

void EnFloormas_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnFloormas* thisv = (EnFloormas*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s32 invisble;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gWallmasterSkel, &gWallmasterWaitAnim, thisv->jointTable,
                       thisv->morphTable, 25);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->zOffset = -1600;
    invisble = thisv->actor.params & SPAWN_INVISIBLE;

    // s16 cast needed
    thisv->actor.params &= (s16) ~(SPAWN_INVISIBLE);
    if (invisble) {
        thisv->actor.flags |= ACTOR_FLAG_7;
        thisv->actor.draw = EnFloormas_DrawHighlighted;
    }

    if (thisv->actor.params == SPAWN_SMALL) {
        thisv->actor.draw = NULL;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->actionFunc = EnFloormas_SmWait;
    } else {
        // spawn first small floormaster
        thisv->actor.parent =
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_FLOORMAS, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, invisble + SPAWN_SMALL);
        if (thisv->actor.parent == NULL) {
            Actor_Kill(&thisv->actor);
            return;
        }
        // spawn 2nd small floormaster
        thisv->actor.child =
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_FLOORMAS, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, invisble + SPAWN_SMALL);
        if (thisv->actor.child == NULL) {
            Actor_Kill(thisv->actor.parent);
            Actor_Kill(&thisv->actor);
            return;
        }

        // link floormasters together
        thisv->actor.parent->child = &thisv->actor;
        thisv->actor.parent->parent = thisv->actor.child;
        thisv->actor.child->parent = &thisv->actor;
        thisv->actor.child->child = thisv->actor.parent;
        EnFloormas_SetupBigDecideAction(thisv);
    }
}

void EnFloormas_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFloormas* thisv = (EnFloormas*)thisx;
    ColliderCylinder* col = &thisv->collider;
    Collider_DestroyCylinder(globalCtx, col);
}

void EnFloormas_MakeInvulnerable(EnFloormas* thisv) {
    thisv->collider.base.colType = COLTYPE_HARD;
    thisv->collider.base.acFlags |= AC_HARD;
    thisv->actionTarget = 0x28;
}

void EnFloormas_MakeVulnerable(EnFloormas* thisv) {
    thisv->collider.base.colType = COLTYPE_HIT0;
    thisv->actionTarget = 0;
    thisv->collider.base.acFlags &= ~AC_HARD;
}

void EnFloormas_SetupBigDecideAction(EnFloormas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterWaitAnim);
    thisv->actionFunc = EnFloormas_BigDecideAction;
    thisv->actor.speedXZ = 0.0f;
}

void EnFloormas_SetupStand(EnFloormas* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWallmasterStandUpAnim, -3.0f);
    thisv->actionFunc = EnFloormas_Stand;
}

void EnFloormas_SetupBigWalk(EnFloormas* thisv) {
    if (thisv->actionFunc != EnFloormas_Run) {
        Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gWallmasterWalkAnim, 1.5f);
    } else {
        thisv->skelAnime.playSpeed = 1.5f;
    }

    thisv->actionTimer = Rand_S16Offset(2, 4);
    thisv->actionFunc = EnFloormas_BigWalk;
    thisv->actor.speedXZ = 1.5f;
}

void EnFloormas_SetupBigStopWalk(EnFloormas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterStopWalkAnim);
    thisv->actionFunc = EnFloormas_BigStopWalk;
    thisv->actor.speedXZ = 0.0f;
}

void EnFloormas_SetupRun(EnFloormas* thisv) {
    thisv->actionTimer = 0;
    thisv->actionFunc = EnFloormas_Run;
    thisv->actor.speedXZ = 5.0f;
    thisv->skelAnime.playSpeed = 3.0f;
}

void EnFloormas_SetupTurn(EnFloormas* thisv) {
    s16 rotDelta = thisv->actionTarget - thisv->actor.shape.rot.y;

    thisv->actor.speedXZ = 0.0f;
    if (rotDelta > 0) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gFloormasterTurnAnim, -3.0f);
    } else {
        Animation_Change(&thisv->skelAnime, &gFloormasterTurnAnim, -1.0f, Animation_GetLastFrame(&gFloormasterTurnAnim),
                         0.0f, ANIMMODE_ONCE, -3.0f);
    }

    if (thisv->actor.scale.x > 0.004f) {
        thisv->actionTarget = (rotDelta * (2.0f / 30.0f));
    } else {
        thisv->skelAnime.playSpeed = thisv->skelAnime.playSpeed + thisv->skelAnime.playSpeed;
        thisv->actionTarget = rotDelta * (2.0f / 15.0f);
    }
    thisv->actionFunc = EnFloormas_Turn;
}

void EnFloormas_SetupHover(EnFloormas* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gWallmasterHoverAnim, 3.0f, 0, Animation_GetLastFrame(&gWallmasterHoverAnim),
                     ANIMMODE_ONCE, -3.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = 0.0f;
    EnFloormas_MakeInvulnerable(thisv);
    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 15.0f, 6, 20.0f, 0x12C, 0x64, 1);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_ATTACK);
    thisv->actionFunc = EnFloormas_Hover;
}

void EnFloormas_SetupCharge(EnFloormas* thisv) {
    thisv->actionTimer = 25;
    thisv->actor.gravity = -0.15f;
    thisv->actionFunc = EnFloormas_Charge;
    thisv->actor.speedXZ = 0.5f;
}

void EnFloormas_SetupLand(EnFloormas* thisv) {
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, 42.0f, ANIMMODE_ONCE, 5.0f);
    if ((thisv->actor.speedXZ < 0.0f) || (thisv->actionFunc != EnFloormas_Charge)) {
        thisv->actionTimer = 30;
    } else {
        thisv->actionTimer = 45;
    }
    thisv->actor.gravity = -1.0f;
    thisv->actionFunc = EnFloormas_Land;
}

void EnFloormas_SetupSplit(EnFloormas* thisv) {

    Actor_SetScale(&thisv->actor, 0.004f);
    thisv->actor.flags |= ACTOR_FLAG_4;
    if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
        thisv->actor.draw = EnFloormas_DrawHighlighted;
    } else {
        thisv->actor.draw = EnFloormas_Draw;
    }
    thisv->actor.shape.rot.y = thisv->actor.parent->shape.rot.y + 0x5555;
    thisv->actor.world.pos = thisv->actor.parent->world.pos;
    thisv->actor.params = 0x10;
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, Animation_GetLastFrame(&gWallmasterJumpAnim),
                     ANIMMODE_ONCE, 0.0f);
    thisv->collider.dim.radius = sCylinderInit.dim.radius * 0.6f;
    thisv->collider.dim.height = sCylinderInit.dim.height * 0.6f;
    thisv->collider.info.bumperFlags &= ~BUMP_HOOKABLE;
    thisv->actor.speedXZ = 4.0f;
    thisv->actor.velocity.y = 7.0f;
    // using div creates a signed check.
    thisv->actor.colChkInfo.health = sColChkInfoInit.health >> 1;
    thisv->actionFunc = EnFloormas_Split;
}

void EnFloormas_SetupSmWalk(EnFloormas* thisv) {
    Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gWallmasterWalkAnim, 4.5f);
    thisv->actionFunc = EnFloormas_SmWalk;
    thisv->actor.speedXZ = 5.0f;
}

void EnFloormas_SetupSmDecideAction(EnFloormas* thisv) {
    if (thisv->actionFunc != EnFloormas_SmWalk) {
        Animation_PlayLoopSetSpeed(&thisv->skelAnime, &gWallmasterWalkAnim, 4.5f);
    }
    thisv->actionFunc = EnFloormas_SmDecideAction;
    thisv->actor.speedXZ = 5.0f;
}

void EnFloormas_SetupSmShrink(EnFloormas* thisv, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 15.0f;
    pos.z = thisv->actor.world.pos.z;
    EffectSsDeadDb_Spawn(globalCtx, &pos, &velocity, &accel, 150, -10, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
    thisv->actionFunc = EnFloormas_SmShrink;
}

void EnFloormas_SetupSlaveJumpAtMaster(EnFloormas* thisv) {
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = EnFloormas_SmSlaveJumpAtMaster;
    thisv->actor.speedXZ = 0.0f;
}

void EnFloormas_SetupJumpAtLink(EnFloormas* thisv) {
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = EnFloormas_JumpAtLink;
    thisv->actor.speedXZ = 0.0f;
}

void EnFloormas_SetupGrabLink(EnFloormas* thisv, Player* player) {
    f32 yDelta;
    f32 xzDelta;

    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 1.0f, 36.0f, 45.0f, ANIMMODE_ONCE, -3.0f);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    EnFloormas_MakeInvulnerable(thisv);
    if (!LINK_IS_ADULT) {
        yDelta = CLAMP(-thisv->actor.yDistToPlayer, 20.0f, 30.0f);
        xzDelta = -10.0f;
    } else {
        yDelta = CLAMP(-thisv->actor.yDistToPlayer, 25.0f, 45.0f);
        xzDelta = -70.0f;
    }
    thisv->actor.world.pos.y = player->actor.world.pos.y + yDelta;
    thisv->actor.world.pos.x = Math_SinS(thisv->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.x;
    thisv->actor.world.pos.z = Math_CosS(thisv->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.z;
    thisv->actor.shape.rot.x = -0x4CE0;
    thisv->actionFunc = EnFloormas_GrabLink;
}

void EnFloormas_SetupMerge(EnFloormas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterWaitAnim);
    thisv->actionTimer = 0;
    thisv->smActionTimer += 1500;
    EnFloormas_MakeInvulnerable(thisv);
    thisv->actionFunc = EnFloormas_Merge;
}

void EnFloormas_SetupSmWait(EnFloormas* thisv) {
    EnFloormas* parent = (EnFloormas*)thisv->actor.parent;
    EnFloormas* child = (EnFloormas*)thisv->actor.child;

    // if thisv is the last remaining small floor master, kill all.
    if ((parent->actionFunc == EnFloormas_SmWait) && (child->actionFunc == EnFloormas_SmWait)) {
        Actor_Kill(&parent->actor);
        Actor_Kill(&child->actor);
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->actor.draw = NULL;
    thisv->actionFunc = EnFloormas_SmWait;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_4);
}

void EnFloormas_SetupTakeDamage(EnFloormas* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWallmasterDamageAnim, -3.0f);
    if (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824) {
        thisv->actor.world.rot.y = thisv->collider.base.ac->world.rot.y;
    } else {
        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, thisv->collider.base.ac) + 0x8000;
    }
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0x14);
    thisv->actionFunc = EnFloormas_TakeDamage;
    thisv->actor.speedXZ = 5.0f;
    thisv->actor.velocity.y = 5.5f;
}

void EnFloormas_SetupRecover(EnFloormas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterRecoverFromDamageAnim);
    thisv->actor.velocity.y = thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actionFunc = EnFloormas_Recover;
}

void EnFloormas_SetupFreeze(EnFloormas* thisv) {
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 1.5f, 0, 20.0f, ANIMMODE_ONCE, -3.0f);
    thisv->actor.speedXZ = 0.0f;
    if (thisv->actor.colChkInfo.damageEffect == 4) {
        Actor_SetColorFilter(&thisv->actor, -0x8000, 0xFF, 0, 0x50);
    } else {
        Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 0x50);
        if (thisv->actor.scale.x > 0.004f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
        }
    }
    thisv->actionTimer = 80;
    thisv->actionFunc = EnFloormas_Freeze;
}

void EnFloormas_Die(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.scale.x > 0.004f) {
        // split
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer + 0x8000;
        EnFloormas_SetupSplit((EnFloormas*)thisv->actor.child);
        EnFloormas_SetupSplit((EnFloormas*)thisv->actor.parent);
        EnFloormas_SetupSplit(thisv);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SPLIT);
    } else {
        // Die
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x90);
        EnFloormas_SetupSmShrink(thisv, globalCtx);
    }
}

void EnFloormas_BigDecideAction(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        // within 400 units of link and within 90 degrees rotation of him
        if (thisv->actor.xzDistToPlayer < 400.0f && !Actor_IsFacingPlayer(&thisv->actor, 0x4000)) {
            thisv->actionTarget = thisv->actor.yawTowardsPlayer;
            EnFloormas_SetupTurn(thisv);
            // within 280 units of link and within 45 degrees rotation of him
        } else if (thisv->actor.xzDistToPlayer < 280.0f && Actor_IsFacingPlayer(&thisv->actor, 0x2000)) {
            EnFloormas_SetupHover(thisv, globalCtx);
        } else {
            EnFloormas_SetupStand(thisv);
        }
    }
}

void EnFloormas_Stand(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.scale.x > 0.004f) {
            EnFloormas_SetupBigWalk(thisv);
        } else if (thisv->actor.params == MERGE_SLAVE) {
            EnFloormas_SetupSmDecideAction(thisv);
        } else {
            EnFloormas_SetupSmWalk(thisv);
        }
    }
}

void EnFloormas_BigWalk(EnFloormas* thisv, GlobalContext* globalCtx) {
    s32 animPastFrame;

    SkelAnime_Update(&thisv->skelAnime);
    animPastFrame = Animation_OnFrame(&thisv->skelAnime, 0.0f);
    if (animPastFrame) {
        if (thisv->actionTimer != 0) {
            thisv->actionTimer--;
        }
    }
    if (((animPastFrame || (Animation_OnFrame(&thisv->skelAnime, 12.0f))) ||
         (Animation_OnFrame(&thisv->skelAnime, 24.0f) != 0)) ||
        (Animation_OnFrame(&thisv->skelAnime, 36.0f) != 0)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_WALK);
    }

    if ((thisv->actor.xzDistToPlayer < 320.0f) && (Actor_IsFacingPlayer(&thisv->actor, 0x4000))) {
        EnFloormas_SetupRun(thisv);
    } else if (thisv->actor.bgCheckFlags & 8) {
        // set target rotation to the colliding wall's rotation
        thisv->actionTarget = thisv->actor.wallYaw;
        EnFloormas_SetupTurn(thisv);
    } else if ((thisv->actor.xzDistToPlayer < 400.0f) && !Actor_IsFacingPlayer(&thisv->actor, 0x4000)) {
        // set target rotation to link.
        thisv->actionTarget = thisv->actor.yawTowardsPlayer;
        EnFloormas_SetupTurn(thisv);
    } else if (thisv->actionTimer == 0) {
        EnFloormas_SetupBigStopWalk(thisv);
    }
}

void EnFloormas_BigStopWalk(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnFloormas_SetupBigDecideAction(thisv);
    }
}

void EnFloormas_Run(EnFloormas* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f) ||
        Animation_OnFrame(&thisv->skelAnime, 24.0f) || Animation_OnFrame(&thisv->skelAnime, 36.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_WALK);
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x71C);

    if ((thisv->actor.xzDistToPlayer < 280.0f) && Actor_IsFacingPlayer(&thisv->actor, 0x2000) &&
        !(thisv->actor.bgCheckFlags & 8)) {
        EnFloormas_SetupHover(thisv, globalCtx);
    } else if (thisv->actor.xzDistToPlayer > 400.0f) {
        EnFloormas_SetupBigWalk(thisv);
    }
}

void EnFloormas_Turn(EnFloormas* thisv, GlobalContext* globalCtx) {
    char pad[4];
    f32 sp30;
    f32 sp2C;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnFloormas_SetupStand(thisv);
    }

    if (((thisv->skelAnime.playSpeed > 0.0f) && Animation_OnFrame(&thisv->skelAnime, 21.0f)) ||
        ((thisv->skelAnime.playSpeed < 0.0f) && Animation_OnFrame(&thisv->skelAnime, 6.0f))) {
        if (thisv->actor.scale.x > 0.004f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_WALK);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
        }
    }
    // Needed to match
    if (!thisv->skelAnime.curFrame) {}
    if (thisv->skelAnime.curFrame >= 7.0f && thisv->skelAnime.curFrame < 22.0f) {
        sp30 = Math_SinS(thisv->actor.shape.rot.y + 0x4268);
        sp2C = Math_CosS(thisv->actor.shape.rot.y + 0x4268);
        thisv->actor.shape.rot.y += thisv->actionTarget;
        thisv->actor.world.pos.x -=
            (thisv->actor.scale.x * 2700.0f) * (Math_SinS(thisv->actor.shape.rot.y + 0x4268) - sp30);
        thisv->actor.world.pos.z -=
            (thisv->actor.scale.x * 2700.0f) * (Math_CosS(thisv->actor.shape.rot.y + 0x4268) - sp2C);
    }
}

void EnFloormas_Hover(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnFloormas_SetupCharge(thisv);
    }
    thisv->actor.shape.rot.x += 0x140;
    thisv->actor.world.pos.y += 10.0f;
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 2730);
    Math_StepToS(&thisv->zOffset, 1200, 100);
}

void EnFloormas_Slide(EnFloormas* thisv, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f velocity;

    pos.x = thisv->actor.world.pos.x;
    pos.z = thisv->actor.world.pos.z;
    pos.y = thisv->actor.floorHeight;

    velocity.y = 2.0f;
    velocity.x = Math_SinS(thisv->actor.shape.rot.y + 0x6000) * 7.0f;
    velocity.z = Math_CosS(thisv->actor.shape.rot.y + 0x6000) * 7.0f;

    func_800286CC(globalCtx, &pos, &velocity, &accel, 450, 100);

    velocity.x = Math_SinS(thisv->actor.shape.rot.y - 0x6000) * 7.0f;
    velocity.z = Math_CosS(thisv->actor.shape.rot.y - 0x6000) * 7.0f;

    func_800286CC(globalCtx, &pos, &velocity, &accel, 450, 100);

    func_8002F974(&thisv->actor, NA_SE_EN_FLOORMASTER_SLIDING);
}

void EnFloormas_Charge(EnFloormas* thisv, GlobalContext* globalCtx) {
    f32 distFromGround;

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }

    Math_StepToF(&thisv->actor.speedXZ, 15.0f, SQ(thisv->actor.speedXZ) * (1.0f / 3.0f));
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x1680, 0x140);

    distFromGround = thisv->actor.world.pos.y - thisv->actor.floorHeight;
    if (distFromGround < 10.0f) {
        thisv->actor.world.pos.y = thisv->actor.floorHeight + 10.0f;
        thisv->actor.gravity = 0.0f;
        thisv->actor.velocity.y = 0.0f;
    }

    if (distFromGround < 12.0f) {
        EnFloormas_Slide(thisv, globalCtx);
    }

    if ((thisv->actor.bgCheckFlags & 8) || (thisv->actionTimer == 0)) {
        EnFloormas_SetupLand(thisv);
    }
}

void EnFloormas_Land(EnFloormas* thisv, GlobalContext* globalCtx) {
    s32 isOnGround;

    isOnGround = thisv->actor.bgCheckFlags & 1;
    if (thisv->actor.bgCheckFlags & 2) {
        if (thisv->actor.params != MERGE_MASTER) {
            EnFloormas_MakeVulnerable(thisv);
        }

        if (thisv->actor.velocity.y < -4.0f) {
            if (thisv->actor.scale.x > 0.004f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_LAND);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
            }
        }
    }
    if (thisv->actor.bgCheckFlags & 8) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (isOnGround) {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 2.0f);
    }

    if ((thisv->actor.speedXZ > 0.0f) && ((thisv->actor.world.pos.y - thisv->actor.floorHeight) < 12.0f)) {
        EnFloormas_Slide(thisv, globalCtx);
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actionTimer != 0) {
            thisv->actionTimer--;
        }

        if (thisv->actionTimer == 0 && isOnGround) {
            if (thisv->skelAnime.endFrame < 45.0f) {
                thisv->skelAnime.endFrame = Animation_GetLastFrame(&gWallmasterJumpAnim);
            } else if (thisv->actor.params == MERGE_MASTER) {
                EnFloormas_SetupMerge(thisv);
            } else {
                EnFloormas_SetupStand(thisv);
                thisv->smActionTimer = 50;
            }
        }
    }

    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x140);
    Math_StepToS(&thisv->zOffset, -1600, 100);
}

void EnFloormas_Split(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->smActionTimer = 50;
            EnFloormas_SetupStand(thisv);
        }
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 1.0f);
    }

    if (thisv->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
    }
}

void EnFloormas_SmWalk(EnFloormas* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    DECR(thisv->smActionTimer);

    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 18.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }

    if (thisv->smActionTimer == 0) {
        EnFloormas_SetupSmDecideAction(thisv);
    } else if (thisv->actor.bgCheckFlags & 8) {
        thisv->actionTarget = thisv->actor.wallYaw;
        EnFloormas_SetupTurn(thisv);
    } else if (thisv->actor.xzDistToPlayer < 120.0f) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 0x38E);
    }
}

void EnFloormas_SmDecideAction(EnFloormas* thisv, GlobalContext* globalCtx) {
    Actor* primaryFloormas;
    s32 isAgainstWall;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 18.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }
    isAgainstWall = thisv->actor.bgCheckFlags & 8;
    if (isAgainstWall) {
        thisv->actionTarget = thisv->actor.wallYaw;
        EnFloormas_SetupTurn(thisv);
        return;
    }

    if (thisv->actor.params == MERGE_SLAVE) {
        if (thisv->actor.parent->params == MERGE_MASTER) {
            primaryFloormas = thisv->actor.parent;
        } else if (thisv->actor.child->params == MERGE_MASTER) {
            primaryFloormas = thisv->actor.child;
        } else {
            thisv->actor.params = 0x10;
            return;
        }

        Math_ScaledStepToS(&thisv->actor.shape.rot.y, Actor_WorldYawTowardActor(&thisv->actor, primaryFloormas), 0x38E);
        if (Actor_WorldDistXZToActor(&thisv->actor, primaryFloormas) < 80.0f) {
            EnFloormas_SetupSlaveJumpAtMaster(thisv);
        }
    } else {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x71C);
        if (thisv->actor.xzDistToPlayer < 80.0f) {
            EnFloormas_SetupJumpAtLink(thisv);
        }
    }
}

void EnFloormas_SmShrink(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->actor.scale.x, 0.0f, 0.0015f)) {
        EnFloormas_SetupSmWait(thisv);
    }
    thisv->actor.scale.z = thisv->actor.scale.x;
    thisv->actor.scale.y = thisv->actor.scale.x;
}

void EnFloormas_JumpAtLink(EnFloormas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->skelAnime.curFrame < 20.0f) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    } else if (Animation_OnFrame(&thisv->skelAnime, 20.0f)) {
        thisv->actor.speedXZ = 5.0f;
        thisv->actor.velocity.y = 7.0f;
    } else if (thisv->actor.bgCheckFlags & 2) {
        thisv->actionTimer = 0x32;
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        EnFloormas_SetupLand(thisv);
    } else if ((thisv->actor.yDistToPlayer < -10.0f) && (thisv->collider.base.ocFlags1 & OC1_HIT) &&
               (&player->actor == thisv->collider.base.oc)) {
        globalCtx->grabPlayer(globalCtx, player);
        EnFloormas_SetupGrabLink(thisv, player);
    }
}

void EnFloormas_GrabLink(EnFloormas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    EnFloormas* parent;
    EnFloormas* child;
    f32 yDelta;
    f32 xzDelta;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->skelAnime.playSpeed > 0.0f) {
            thisv->skelAnime.playSpeed = -1.0f;
            thisv->skelAnime.endFrame = 36.0f;
            thisv->skelAnime.startFrame = 45.0f;
        } else {
            thisv->skelAnime.playSpeed = 1.0f;
            thisv->skelAnime.endFrame = 45.0f;
            thisv->skelAnime.startFrame = 36.0f;
        }
    }

    if (!LINK_IS_ADULT) {
        yDelta = CLAMP(-thisv->actor.yDistToPlayer, 20.0f, 30.0f);
        xzDelta = -10.0f;
    } else {
        yDelta = CLAMP(-thisv->actor.yDistToPlayer, 25.0f, 45.0f);
        xzDelta = -30.0f;
    }

    thisv->actor.world.pos.y = player->actor.world.pos.y + yDelta;
    thisv->actor.world.pos.x = Math_SinS(thisv->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.x;
    thisv->actor.world.pos.z = Math_CosS(thisv->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.z;

    // let go
    if (!(player->stateFlags2 & 0x80) || (player->invincibilityTimer < 0)) {
        parent = (EnFloormas*)thisv->actor.parent;
        child = (EnFloormas*)thisv->actor.child;

        if (((parent->actionFunc == EnFloormas_GrabLink) || parent->actionFunc == EnFloormas_SmWait) &&
            (child->actionFunc == EnFloormas_GrabLink || child->actionFunc == EnFloormas_SmWait)) {

            parent->actor.params = MERGE_SLAVE;
            child->actor.params = MERGE_SLAVE;
            thisv->actor.params = MERGE_MASTER;
        }

        thisv->actor.shape.rot.x = 0;
        thisv->actor.velocity.y = 6.0f;
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.speedXZ = -3.0f;
        EnFloormas_SetupLand(thisv);
    } else {
        // Damage link every 20 frames
        if ((thisv->actionTarget % 20) == 0) {
            if (!LINK_IS_ADULT) {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S_KID);
            } else {
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S);
            }
            globalCtx->damagePlayer(globalCtx, -8);
        }
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_STICK);
}

void EnFloormas_SmSlaveJumpAtMaster(EnFloormas* thisv, GlobalContext* globalCtx) {
    Actor* primFloormas;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.parent->params == MERGE_MASTER) {
        primFloormas = thisv->actor.parent;
    } else if (thisv->actor.child->params == MERGE_MASTER) {
        primFloormas = thisv->actor.child;
    } else {
        if (thisv->actor.bgCheckFlags & 2) {
            thisv->actor.params = 0x10;
            EnFloormas_SetupLand(thisv);
        }
        return;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 20.0f)) {
        thisv->actor.speedXZ = 5.0f;
        thisv->actor.velocity.y = 7.0f;
    } else if (thisv->skelAnime.curFrame < 20.0f) {
        Math_ApproachS(&thisv->actor.shape.rot.y, Actor_WorldYawTowardActor(&thisv->actor, primFloormas), 2, 0xE38);
    } else if ((((primFloormas->world.pos.y - thisv->actor.world.pos.y) < -10.0f) &&
                (fabsf(thisv->actor.world.pos.x - primFloormas->world.pos.x) < 10.0f)) &&
               (fabsf(thisv->actor.world.pos.z - primFloormas->world.pos.z) < 10.0f)) {
        EnFloormas_SetupSmWait(thisv);
        thisv->collider.base.ocFlags1 |= OC1_ON;
    } else if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        EnFloormas_SetupLand(thisv);
    }

    if (fabsf(thisv->actor.world.pos.x - primFloormas->world.pos.x) < 5.0f &&
        fabsf(thisv->actor.world.pos.z - primFloormas->world.pos.z) < 5.0f) {
        Math_StepToF(&thisv->actor.speedXZ, 0, 2.0f);
    }
}

void EnFloormas_Merge(EnFloormas* thisv, GlobalContext* globalCtx) {
    EnFloormas* parent;
    EnFloormas* child;
    s32 mergeCnt;
    f32 prevScale;
    f32 curScale;

    mergeCnt = 0;

    DECR(thisv->smActionTimer);

    parent = (EnFloormas*)thisv->actor.parent;
    child = (EnFloormas*)thisv->actor.child;

    if (thisv->smActionTimer == 0) {
        if (parent->actionFunc != EnFloormas_SmWait) {
            EnFloormas_SetupSmShrink(parent, globalCtx);
        }

        if (child->actionFunc != EnFloormas_SmWait) {
            EnFloormas_SetupSmShrink(child, globalCtx);
        }
    } else {
        if ((parent->actionFunc != EnFloormas_SmWait) && (parent->actionFunc != EnFloormas_SmShrink)) {
            mergeCnt++;
        }

        if ((child->actionFunc != EnFloormas_SmWait) && (child->actionFunc != EnFloormas_SmShrink)) {
            mergeCnt++;
        }
    }

    prevScale = thisv->actor.scale.x;

    if (mergeCnt == 1) {
        Math_StepToF(&thisv->actor.scale.x, 0.007f, 0.0005f);
    } else if (mergeCnt == 0) {
        Math_StepToF(&thisv->actor.scale.x, 0.01f, 0.0005f);
    }

    curScale = thisv->actor.scale.x;
    thisv->actor.scale.y = thisv->actor.scale.z = curScale;

    if (((prevScale == 0.007f) || (prevScale == 0.004f)) && (prevScale != thisv->actor.scale.x)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_EXPAND);
    }

    thisv->collider.dim.radius = (sCylinderInit.dim.radius * 100.0f) * thisv->actor.scale.x;
    thisv->collider.dim.height = (sCylinderInit.dim.height * 100.0f) * thisv->actor.scale.x;

    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->actor.scale.x >= 0.01f) {
            thisv->actor.flags &= ~ACTOR_FLAG_4;
            EnFloormas_MakeVulnerable(thisv);
            thisv->actor.params = 0;
            thisv->collider.info.bumperFlags |= BUMP_HOOKABLE;
            thisv->actor.colChkInfo.health = sColChkInfoInit.health;
            EnFloormas_SetupStand(thisv);
        } else {
            if (thisv->actionTimer == 0) {
                Animation_PlayOnce(&thisv->skelAnime, &gFloormasterTapFingerAnim);
                thisv->actionTimer = 1;
            } else {
                Animation_PlayOnce(&thisv->skelAnime, &gWallmasterWaitAnim);
                thisv->actionTimer = 0;
            }
        }
    }
    func_8002F974(&thisv->actor, NA_SE_EN_FLOORMASTER_RESTORE - SFX_FLAG);
}

void EnFloormas_SmWait(EnFloormas* thisv, GlobalContext* globalCtx) {
}

void EnFloormas_TakeDamage(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnFloormas_Die(thisv, globalCtx);
        } else {
            EnFloormas_SetupRecover(thisv);
        }
    }

    if (Animation_OnFrame(&thisv->skelAnime, 13.0f)) {
        if (thisv->actor.scale.x > 0.004f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
    }
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.2f);
}

void EnFloormas_Recover(EnFloormas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnFloormas_SetupStand(thisv);
    }
}

void EnFloormas_Freeze(EnFloormas* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->actionTimer == 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnFloormas_Die(thisv, globalCtx);
            return;
        }
        EnFloormas_SetupStand(thisv);
    }
}

void EnFloormas_ColliderCheck(EnFloormas* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 isSmall;

    if ((thisv->collider.base.acFlags & AC_HIT) != 0) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (thisv->collider.base.colType != COLTYPE_HARD) {
                isSmall = 0;
                if (thisv->actor.scale.x < 0.01f) {
                    isSmall = 1;
                }
                if (isSmall && thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x80) {
                    thisv->actor.colChkInfo.damage = 2;
                    thisv->actor.colChkInfo.damageEffect = 0;
                }
                if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    if (isSmall) {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLOORMASTER_SM_DEAD);
                    } else {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_DEAD);
                    }
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                } else if (thisv->actor.colChkInfo.damage != 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_DAMAGE);
                }

                if ((thisv->actor.colChkInfo.damageEffect == 4) || (thisv->actor.colChkInfo.damageEffect == 1)) {
                    if (thisv->actionFunc != EnFloormas_Freeze) {
                        EnFloormas_SetupFreeze(thisv);
                    }
                } else {
                    if (thisv->actor.colChkInfo.damageEffect == 2) {
                        EffectSsFCircle_Spawn(globalCtx, &thisv->actor, &thisv->actor.world.pos,
                                              thisv->actor.scale.x * 4000.0f, thisv->actor.scale.x * 4000.0f);
                    }
                    EnFloormas_SetupTakeDamage(thisv);
                }
            }
        }
    }
}

void EnFloormas_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFloormas* thisv = (EnFloormas*)thisx;
    s32 pad;

    if (thisv->actionFunc != EnFloormas_SmWait) {
        if (thisv->collider.base.atFlags & AT_HIT) {
            thisv->collider.base.atFlags &= ~AT_HIT;
            thisv->actor.speedXZ *= -0.5f;

            if (-5.0f < thisv->actor.speedXZ) {
                thisv->actor.speedXZ = -5.0f;
            } else {
                thisv->actor.speedXZ = thisv->actor.speedXZ;
            }

            thisv->actor.velocity.y = 5.0f;

            EnFloormas_SetupLand(thisv);
        }
        EnFloormas_ColliderCheck(thisv, globalCtx);
        thisv->actionFunc(thisv, globalCtx);

        if (thisv->actionFunc != EnFloormas_TakeDamage) {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        }

        if (thisv->actionFunc != EnFloormas_GrabLink) {
            Actor_MoveForward(&thisv->actor);
        }

        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, thisv->actor.scale.x * 3000.0f, 0.0f, 0x1D);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (thisv->actionFunc == EnFloormas_Charge) {
            thisv->actor.flags |= ACTOR_FLAG_24;
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->actionFunc != EnFloormas_GrabLink) {
            if (thisv->actionFunc != EnFloormas_Split && thisv->actionFunc != EnFloormas_TakeDamage &&
                thisv->actor.freezeTimer == 0) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }

            if ((thisv->actionFunc != EnFloormas_SmSlaveJumpAtMaster) || (thisv->skelAnime.curFrame < 20.0f)) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
        }

        Actor_SetFocus(&thisv->actor, thisv->actor.scale.x * 2500.0f);

        if (thisv->collider.base.colType == COLTYPE_HARD) {
            if (thisv->actionTarget != 0) {
                thisv->actionTarget--;
            }

            if (thisv->actionTarget == 0) {
                thisv->actionTarget = 0x28;
            }
        }
    }
}

s32 EnFloormas_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx, Gfx** gfx) {
    EnFloormas* thisv = (EnFloormas*)thisx;

    if (limbIndex == 1) {
        pos->z += thisv->zOffset;
    }
    return false;
}

void EnFloormas_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    if (limbIndex == 2) {
        Matrix_Push();
        Matrix_Translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_RotateY(DEGTORAD(60.0f), MTXMODE_APPLY);
        Matrix_RotateZ(DEGTORAD(15.0f), MTXMODE_APPLY);
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_floormas.c", 2299), G_MTX_LOAD);
        gSPDisplayList((*gfx)++, gWallmasterFingerDL);
        Matrix_Pop();
    }
}

static Color_RGBA8 sMergeColor = { 0, 255, 0, 0 };

void EnFloormas_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnFloormas* thisv = (EnFloormas*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_floormas.c", 2318);

    func_80093D18(globalCtx->state.gfxCtx);
    if (thisv->collider.base.colType == COLTYPE_HARD) {
        func_80026230(globalCtx, &sMergeColor, thisv->actionTarget % 0x28, 0x28);
    }

    POLY_OPA_DISP =
        SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                           EnFloormas_OverrideLimbDraw, EnFloormas_PostLimbDraw, thisv, POLY_OPA_DISP);
    if (thisv->collider.base.colType == COLTYPE_HARD) {
        func_80026608(globalCtx);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_floormas.c", 2340);
}

void EnFloormas_DrawHighlighted(Actor* thisx, GlobalContext* globalCtx) {
    EnFloormas* thisv = (EnFloormas*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_floormas.c", 2352);

    func_80093D84(globalCtx->state.gfxCtx);
    if (thisv->collider.base.colType == COLTYPE_HARD) {
        func_80026690(globalCtx, &sMergeColor, thisv->actionTarget % 0x28, 0x28);
    }
    POLY_XLU_DISP =
        SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                           EnFloormas_OverrideLimbDraw, EnFloormas_PostLimbDraw, thisv, POLY_XLU_DISP);
    if (thisv->collider.base.colType == COLTYPE_HARD) {
        func_80026A6C(globalCtx);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_floormas.c", 2374);
}
