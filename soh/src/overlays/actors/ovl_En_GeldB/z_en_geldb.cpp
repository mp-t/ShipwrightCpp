/*
 * File: z_en_geldb.c
 * Overlay: ovl_En_GeldB
 * Description: Gerudo fighter
 */

#include "z_en_geldb.h"
#include "objects/object_geldb/object_geldb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

typedef enum {
    /*  0 */ GELDB_WAIT,
    /*  1 */ GELDB_DEFEAT,
    /*  2 */ GELDB_DAMAGED,
    /*  3 */ GELDB_JUMP,
    /*  4 */ GELDB_ROLL_BACK,
    /*  5 */ GELDB_READY,
    /*  6 */ GELDB_BLOCK,
    /*  7 */ GELDB_SLASH,
    /*  8 */ GELDB_ADVANCE,
    /*  9 */ GELDB_PIVOT,
    /* 10 */ GELDB_CIRCLE,
    /* 11 */ GELDB_UNUSED,
    /* 12 */ GELDB_SPIN_ATTACK,
    /* 13 */ GELDB_SIDESTEP,
    /* 14 */ GELDB_ROLL_FORWARD,
    /* 15 */ GELDB_STUNNED,
    /* 16 */ GELDB_SPIN_DODGE
} EnGeldBAction;

void EnGeldB_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGeldB_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGeldB_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGeldB_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 EnGeldB_DodgeRanged(GlobalContext* globalCtx, EnGeldB* thisv);

void EnGeldB_SetupWait(EnGeldB* thisv);
void EnGeldB_SetupReady(EnGeldB* thisv);
void EnGeldB_SetupAdvance(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_SetupPivot(EnGeldB* thisv);
void EnGeldB_SetupRollForward(EnGeldB* thisv);
void EnGeldB_SetupCircle(EnGeldB* thisv);
void EnGeldB_SetupSpinDodge(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_SetupSlash(EnGeldB* thisv);
void EnGeldB_SetupSpinAttack(EnGeldB* thisv);
void EnGeldB_SetupRollBack(EnGeldB* thisv);
void EnGeldB_SetupJump(EnGeldB* thisv);
void EnGeldB_SetupBlock(EnGeldB* thisv);
void EnGeldB_SetupSidestep(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_SetupDefeated(EnGeldB* thisv);

void EnGeldB_Wait(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Flee(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Ready(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Advance(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_RollForward(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Pivot(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Circle(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_SpinDodge(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Slash(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_SpinAttack(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_RollBack(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Stunned(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Damaged(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Jump(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Block(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Sidestep(EnGeldB* thisv, GlobalContext* globalCtx);
void EnGeldB_Defeated(EnGeldB* thisv, GlobalContext* globalCtx);

const ActorInit En_GeldB_InitVars = {
    ACTOR_EN_GELDB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GELDB,
    sizeof(EnGeldB),
    (ActorFunc)EnGeldB_Init,
    (ActorFunc)EnGeldB_Destroy,
    (ActorFunc)EnGeldB_Update,
    (ActorFunc)EnGeldB_Draw,
    NULL,
};

static ColliderCylinderInit sBodyCylInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit sBlockTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit sBlockTrisInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sBlockTrisElementsInit,
};

static ColliderQuadInit sSwordQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ GELDB_DMG_NORMAL,
    /* 0x1 */ GELDB_DMG_STUN,
    /* 0x6 */ GELDB_DMG_UNK_6 = 0x6,
    /* 0xD */ GELDB_DMG_UNK_D = 0xD,
    /* 0xE */ GELDB_DMG_UNK_E,
    /* 0xF */ GELDB_DMG_FREEZE
} EnGeldBDamageEffects;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Deku stick    */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, GELDB_DMG_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Ice arrow     */ DMG_ENTRY(2, GELDB_DMG_FREEZE),
    /* Light arrow   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 1   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 2   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Unk arrow 3   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Fire magic    */ DMG_ENTRY(4, GELDB_DMG_UNK_E),
    /* Ice magic     */ DMG_ENTRY(0, GELDB_DMG_UNK_6),
    /* Light magic   */ DMG_ENTRY(3, GELDB_DMG_UNK_D),
    /* Shield        */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Mirror Ray    */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Kokiri spin   */ DMG_ENTRY(1, GELDB_DMG_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, GELDB_DMG_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, GELDB_DMG_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unblockable   */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
    /* Hammer jump   */ DMG_ENTRY(4, GELDB_DMG_NORMAL),
    /* Unknown 2     */ DMG_ENTRY(0, GELDB_DMG_NORMAL),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3000, ICHAIN_STOP),
};

//static Vec3f sUnusedOffset = { 1100.0f, -700.0f, 0.0f };

void EnGeldB_SetupAction(EnGeldB* thisv, EnGeldBActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnGeldB_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EffectBlureInit1 blureInit;
    EnGeldB* thisv = (EnGeldB*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawFeet, 0.0f);
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.health = 20;
    thisx->colChkInfo.cylRadius = 50;
    thisx->colChkInfo.cylHeight = 100;
    thisx->naviEnemyId = 0x54;
    thisv->keyFlag = thisx->params & 0xFF00;
    thisx->params &= 0xFF;
    thisv->blinkState = 0;
    thisv->unkFloat = 10.0f;
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGerudoRedSkel, &gGerudoRedNeutralAnim, thisv->jointTable,
                       thisv->morphTable, GELDB_LIMB_MAX);
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, thisx, &sBodyCylInit);
    Collider_InitTris(globalCtx, &thisv->blockCollider);
    Collider_SetTris(globalCtx, &thisv->blockCollider, thisx, &sBlockTrisInit, thisv->blockElements);
    Collider_InitQuad(globalCtx, &thisv->swordCollider);
    Collider_SetQuad(globalCtx, &thisv->swordCollider, thisx, &sSwordQuadInit);
    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    Effect_Add(globalCtx, &thisv->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_SetScale(thisx, 0.012499999f);
    EnGeldB_SetupWait(thisv);
    if ((thisv->keyFlag != 0) && Flags_GetCollectible(globalCtx, thisv->keyFlag >> 8)) {
        Actor_Kill(thisx);
    }
}

void EnGeldB_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGeldB* thisv = (EnGeldB*)thisx;

    func_800F5B58();
    Effect_Delete(globalCtx, thisv->blureIndex);
    Collider_DestroyTris(globalCtx, &thisv->blockCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyQuad(globalCtx, &thisv->swordCollider);
}

s32 EnGeldB_ReactToPlayer(GlobalContext* globalCtx, EnGeldB* thisv, s16 arg2) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* thisx = &thisv->actor;
    s16 angleToWall;
    s16 angleToLink;
    Actor* bomb;

    angleToWall = thisx->wallYaw - thisx->shape.rot.y;
    angleToWall = ABS(angleToWall);
    angleToLink = thisx->yawTowardsPlayer - thisx->shape.rot.y;
    angleToLink = ABS(angleToLink);

    if (func_800354B4(globalCtx, thisx, 100.0f, 0x2710, 0x3E80, thisx->shape.rot.y)) {
        if (player->swordAnimation == 0x11) {
            EnGeldB_SetupSpinDodge(thisv, globalCtx);
            return true;
        } else if (globalCtx->gameplayFrames & 1) {
            EnGeldB_SetupBlock(thisv);
            return true;
        }
    }
    if (func_800354B4(globalCtx, thisx, 100.0f, 0x5DC0, 0x2AA8, thisx->shape.rot.y)) {
        thisx->shape.rot.y = thisx->world.rot.y = thisx->yawTowardsPlayer;
        if ((thisx->bgCheckFlags & 8) && (ABS(angleToWall) < 0x2EE0) && (thisx->xzDistToPlayer < 90.0f)) {
            EnGeldB_SetupJump(thisv);
            return true;
        } else if (player->swordAnimation == 0x11) {
            EnGeldB_SetupSpinDodge(thisv, globalCtx);
            return true;
        } else if ((thisx->xzDistToPlayer < 90.0f) && (globalCtx->gameplayFrames & 1)) {
            EnGeldB_SetupBlock(thisv);
            return true;
        } else {
            EnGeldB_SetupRollBack(thisv);
            return true;
        }
    } else if ((bomb = Actor_FindNearby(globalCtx, thisx, -1, ACTORCAT_EXPLOSIVE, 80.0f)) != NULL) {
        thisx->shape.rot.y = thisx->world.rot.y = thisx->yawTowardsPlayer;
        if (((thisx->bgCheckFlags & 8) && (angleToWall < 0x2EE0)) || (bomb->id == ACTOR_EN_BOM_CHU)) {
            if ((bomb->id == ACTOR_EN_BOM_CHU) && (Actor_WorldDistXYZToActor(thisx, bomb) < 80.0f) &&
                ((s16)(thisx->shape.rot.y - (bomb->world.rot.y - 0x8000)) < 0x3E80)) {
                EnGeldB_SetupJump(thisv);
                return true;
            } else {
                EnGeldB_SetupSidestep(thisv, globalCtx);
                return true;
            }
        } else {
            EnGeldB_SetupRollBack(thisv);
            return true;
        }
    } else if (arg2) {
        if (angleToLink >= 0x1B58) {
            EnGeldB_SetupSidestep(thisv, globalCtx);
            return true;
        } else {
            s16 angleToFacingLink = player->actor.shape.rot.y - thisx->shape.rot.y;

            if ((thisx->xzDistToPlayer <= 45.0f) && !Actor_OtherIsTargeted(globalCtx, thisx) &&
                ((globalCtx->gameplayFrames & 7) || (ABS(angleToFacingLink) < 0x38E0))) {
                EnGeldB_SetupSlash(thisv);
                return true;
            } else {
                EnGeldB_SetupCircle(thisv);
                return true;
            }
        }
    }
    return false;
}

void EnGeldB_SetupWait(EnGeldB* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gGerudoRedJumpAnim, 0.0f);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 120.0f;
    thisv->timer = 10;
    thisv->invisible = true;
    thisv->action = GELDB_WAIT;
    thisv->actor.bgCheckFlags &= ~3;
    thisv->actor.gravity = -2.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnGeldB_SetupAction(thisv, EnGeldB_Wait);
}

void EnGeldB_Wait(EnGeldB* thisv, GlobalContext* globalCtx) {
    if ((thisv->invisible && !Flags_GetSwitch(globalCtx, thisv->actor.home.rot.z)) ||
        thisv->actor.xzDistToPlayer > 300.0f) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.world.pos.y = thisv->actor.floorHeight + 120.0f;
    } else {
        thisv->invisible = false;
        thisv->actor.shape.shadowScale = 90.0f;
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (thisv->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        thisv->skelAnime.playSpeed = 1.0f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.focus.pos = thisv->actor.world.pos;
        thisv->actor.bgCheckFlags &= ~2;
        thisv->actor.velocity.y = 0.0f;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnGeldB_SetupReady(thisv);
    }
}

void EnGeldB_SetupFlee(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedJumpAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedJumpAnim, -2.0f, lastFrame, 0.0f, ANIMMODE_ONCE_INTERP, -4.0f);
    thisv->timer = 20;
    thisv->invisible = false;
    thisv->action = GELDB_WAIT;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    EnGeldB_SetupAction(thisv, EnGeldB_Flee);
}

void EnGeldB_Flee(EnGeldB* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame == 10.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    }
    if (thisv->skelAnime.curFrame == 2.0f) {
        thisv->actor.gravity = 0.0f;
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 300.0f, 1.0f, 20.5f, 0.0f);
        thisv->timer--;
        if (thisv->timer == 0) {
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnGeldB_SetupReady(EnGeldB* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gGerudoRedNeutralAnim, -4.0f);
    thisv->action = GELDB_READY;
    thisv->timer = Rand_ZeroOne() * 10.0f + 5.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnGeldB_SetupAction(thisv, EnGeldB_Ready);
}

void EnGeldB_Ready(EnGeldB* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s16 angleToLink;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->lookTimer != 0) {
        angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y - thisv->headRot.y;
        if (ABS(angleToLink) > 0x2000) {
            thisv->lookTimer--;
            return;
        }
        thisv->lookTimer = 0;
    }
    angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if (!EnGeldB_DodgeRanged(globalCtx, thisv)) {
        if (thisv->unkTimer != 0) {
            thisv->unkTimer--;

            if (ABS(angleToLink) >= 0x1FFE) {
                return;
            }
            thisv->unkTimer = 0;
        } else if (EnGeldB_ReactToPlayer(globalCtx, thisv, 0)) {
            return;
        }
        angleToLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        if ((thisv->actor.xzDistToPlayer < 100.0f) && (player->swordState != 0) && (ABS(angleToLink) >= 0x1F40)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            EnGeldB_SetupCircle(thisv);
        } else if (--thisv->timer == 0) {
            if (Actor_IsFacingPlayer(&thisv->actor, 30 * 0x10000 / 360)) {
                if ((210.0f > thisv->actor.xzDistToPlayer) && (thisv->actor.xzDistToPlayer > 150.0f) &&
                    (Rand_ZeroOne() < 0.3f)) {
                    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) || (Rand_ZeroOne() > 0.5f) ||
                        (ABS(angleToLink) < 0x38E0)) {
                        EnGeldB_SetupRollForward(thisv);
                    } else {
                        EnGeldB_SetupSpinAttack(thisv);
                    }
                } else if (Rand_ZeroOne() > 0.3f) {
                    EnGeldB_SetupAdvance(thisv, globalCtx);
                } else {
                    EnGeldB_SetupCircle(thisv);
                }
            } else {
                EnGeldB_SetupPivot(thisv);
            }
            if ((globalCtx->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
            }
        }
    }
}

void EnGeldB_SetupAdvance(EnGeldB* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedWalkAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedWalkAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -4.0f);
    thisv->action = GELDB_ADVANCE;
    EnGeldB_SetupAction(thisv, EnGeldB_Advance);
}

void EnGeldB_Advance(EnGeldB* thisv, GlobalContext* globalCtx) {
    s32 thisKeyFrame;
    s32 prevKeyFrame;
    s32 playSpeed;
    s16 facingAngletoLink;
    Player* player = GET_PLAYER(globalCtx);

    if (!EnGeldB_DodgeRanged(globalCtx, thisv)) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x2EE, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if (thisv->actor.xzDistToPlayer <= 40.0f) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, -8.0f, 1.0f, 1.5f, 0.0f);
        } else if (thisv->actor.xzDistToPlayer > 55.0f) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 8.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 6.65f, 0.0f);
        }
        thisv->skelAnime.playSpeed = thisv->actor.speedXZ / 8.0f;
        facingAngletoLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        facingAngletoLink = ABS(facingAngletoLink);
        if ((thisv->actor.xzDistToPlayer < 150.0f) && (player->swordState != 0) && (facingAngletoLink >= 0x1F40)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                EnGeldB_SetupCircle(thisv);
                return;
            }
        }
        thisKeyFrame = (s32)thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        prevKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        playSpeed = (f32)ABS(thisv->skelAnime.playSpeed);
        if (!Actor_IsFacingPlayer(&thisv->actor, 0x11C7)) {
            if (Rand_ZeroOne() > 0.5f) {
                EnGeldB_SetupCircle(thisv);
            } else {
                EnGeldB_SetupReady(thisv);
            }
        } else if (thisv->actor.xzDistToPlayer < 90.0f) {
            if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                (Rand_ZeroOne() > 0.03f || (thisv->actor.xzDistToPlayer <= 45.0f && facingAngletoLink < 0x38E0))) {
                EnGeldB_SetupSlash(thisv);
            } else if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (Rand_ZeroOne() > 0.5f)) {
                EnGeldB_SetupRollBack(thisv);
            } else {
                EnGeldB_SetupCircle(thisv);
            }
        }
        if (!EnGeldB_ReactToPlayer(globalCtx, thisv, 0)) {
            if ((210.0f > thisv->actor.xzDistToPlayer) && (thisv->actor.xzDistToPlayer > 150.0f) &&
                Actor_IsFacingPlayer(&thisv->actor, 0x71C)) {
                if (Actor_IsTargeted(globalCtx, &thisv->actor)) {
                    if (Rand_ZeroOne() > 0.5f) {
                        EnGeldB_SetupRollForward(thisv);
                    } else {
                        EnGeldB_SetupSpinAttack(thisv);
                    }
                } else {
                    EnGeldB_SetupCircle(thisv);
                    return;
                }
            }
            if ((globalCtx->gameplayFrames & 0x5F) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
            }
            if (thisKeyFrame != (s32)thisv->skelAnime.curFrame) {
                s32 temp = playSpeed + thisKeyFrame;

                if (((prevKeyFrame < 0) && (temp > 0)) || ((prevKeyFrame < 4) && (temp > 4))) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_LAND);
                }
            }
        }
    }
}

void EnGeldB_SetupRollForward(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedFlipAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedFlipAnim, -1.0f, lastFrame, 0.0f, ANIMMODE_ONCE, -3.0f);
    thisv->timer = 0;
    thisv->invisible = true;
    thisv->action = GELDB_ROLL_FORWARD;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actor.speedXZ = 10.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    EnGeldB_SetupAction(thisv, EnGeldB_RollForward);
}

void EnGeldB_RollForward(EnGeldB* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 facingAngleToLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->invisible = false;
        thisv->actor.speedXZ = 0.0f;
        if (!Actor_IsFacingPlayer(&thisv->actor, 0x1554)) {
            EnGeldB_SetupReady(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            if (ABS(facingAngleToLink) < 0x38E0) {
                thisv->lookTimer = 20;
            }
        } else if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                   (Rand_ZeroOne() > 0.5f || (ABS(facingAngleToLink) < 0x3FFC))) {
            EnGeldB_SetupSlash(thisv);
        } else {
            EnGeldB_SetupAdvance(thisv, globalCtx);
        }
    }
    if ((globalCtx->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
}

void EnGeldB_SetupPivot(EnGeldB* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gGerudoRedSidestepAnim, -4.0f);
    thisv->action = GELDB_PIVOT;
    EnGeldB_SetupAction(thisv, EnGeldB_Pivot);
}

void EnGeldB_Pivot(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 angleToLink;
    s16 turnRate;
    f32 playSpeed;

    if (!EnGeldB_DodgeRanged(globalCtx, thisv) && !EnGeldB_ReactToPlayer(globalCtx, thisv, 0)) {
        angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        turnRate = (angleToLink > 0) ? ((angleToLink * 0.25f) + 2000.0f) : ((angleToLink * 0.25f) - 2000.0f);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y += turnRate;
        if (angleToLink > 0) {
            playSpeed = turnRate * 0.5f;
            playSpeed = CLAMP_MAX(playSpeed, 1.0f);
        } else {
            playSpeed = turnRate * 0.5f;
            playSpeed = CLAMP_MIN(playSpeed, -1.0f);
        }
        thisv->skelAnime.playSpeed = -playSpeed;
        SkelAnime_Update(&thisv->skelAnime);
        if (Actor_IsFacingPlayer(&thisv->actor, 30 * 0x10000 / 360)) {
            if (Rand_ZeroOne() > 0.8f) {
                EnGeldB_SetupCircle(thisv);
            } else {
                EnGeldB_SetupAdvance(thisv, globalCtx);
            }
        }
        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
    }
}

void EnGeldB_SetupCircle(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedSidestepAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    thisv->actor.speedXZ = Rand_CenteredFloat(12.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
    thisv->timer = Rand_ZeroOne() * 30.0f + 30.0f;
    thisv->action = GELDB_CIRCLE;
    thisv->approachRate = 0.0f;
    EnGeldB_SetupAction(thisv, EnGeldB_Circle);
}

void EnGeldB_Circle(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 angleBehindLink;
    s16 phi_v1;
    s32 nextKeyFrame;
    s32 thisKeyFrame;
    s32 pad;
    s32 prevKeyFrame;
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (!EnGeldB_DodgeRanged(globalCtx, thisv) && !EnGeldB_ReactToPlayer(globalCtx, thisv, 0)) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3A98;
        angleBehindLink = player->actor.shape.rot.y + 0x8000;
        if (Math_SinS(angleBehindLink - thisv->actor.shape.rot.y) >= 0.0f) {
            thisv->actor.speedXZ -= 0.25f;
            if (thisv->actor.speedXZ < -8.0f) {
                thisv->actor.speedXZ = -8.0f;
            }
        } else if (Math_SinS(angleBehindLink - thisv->actor.shape.rot.y) < 0.0f) {
            thisv->actor.speedXZ += 0.25f;
            if (thisv->actor.speedXZ > 8.0f) {
                thisv->actor.speedXZ = 8.0f;
            }
        }
        if ((thisv->actor.bgCheckFlags & 8) || !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ,
                                                                          thisv->actor.shape.rot.y + 0x3E80)) {
            if (thisv->actor.bgCheckFlags & 8) {
                if (thisv->actor.speedXZ >= 0.0f) {
                    phi_v1 = thisv->actor.shape.rot.y + 0x3E80;
                } else {
                    phi_v1 = thisv->actor.shape.rot.y - 0x3E80;
                }
                phi_v1 = thisv->actor.wallYaw - phi_v1;
            } else {
                thisv->actor.speedXZ *= -0.8f;
                phi_v1 = 0;
            }
            if (ABS(phi_v1) > 0x4000) {
                thisv->actor.speedXZ *= -0.8f;
                if (thisv->actor.speedXZ < 0.0f) {
                    thisv->actor.speedXZ -= 0.5f;
                } else {
                    thisv->actor.speedXZ += 0.5f;
                }
            }
        }
        if (thisv->actor.xzDistToPlayer <= 45.0f) {
            Math_SmoothStepToF(&thisv->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if (thisv->actor.xzDistToPlayer > 40.0f) {
            Math_SmoothStepToF(&thisv->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
        }
        if (thisv->approachRate != 0.0f) {
            thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->approachRate;
            thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->approachRate;
        }
        if (ABS(thisv->approachRate) < ABS(thisv->actor.speedXZ)) {
            thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
        } else {
            thisv->skelAnime.playSpeed = -thisv->approachRate * 0.5f;
        }
        thisv->skelAnime.playSpeed = CLAMP(thisv->skelAnime.playSpeed, -3.0f, 3.0f);

        thisKeyFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);

        prevKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        nextKeyFrame = (s32)ABS(thisv->skelAnime.playSpeed) + thisKeyFrame;
        if ((thisKeyFrame != (s32)thisv->skelAnime.curFrame) &&
            ((prevKeyFrame < 0 && 0 < nextKeyFrame) || (prevKeyFrame < 5 && 5 < nextKeyFrame))) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_LAND);
        }
        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
        if ((Math_CosS(angleBehindLink - thisv->actor.shape.rot.y) < -0.85f) &&
            !Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (thisv->actor.xzDistToPlayer <= 45.0f)) {
            EnGeldB_SetupSlash(thisv);
        } else if (--thisv->timer == 0) {
            if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (Rand_ZeroOne() > 0.5f)) {
                EnGeldB_SetupRollBack(thisv);
            } else {
                EnGeldB_SetupReady(thisv);
            }
        }
    }
}

void EnGeldB_SetupSpinDodge(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 sp3E;
    Player* player = GET_PLAYER(globalCtx);
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedSidestepAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    sp3E = player->actor.shape.rot.y;
    if (Math_SinS(sp3E - thisv->actor.shape.rot.y) > 0.0f) {
        thisv->actor.speedXZ = -10.0f;
    } else if (Math_SinS(sp3E - thisv->actor.shape.rot.y) < 0.0f) {
        thisv->actor.speedXZ = 10.0f;
    } else if (Rand_ZeroOne() > 0.5f) {
        thisv->actor.speedXZ = 10.0f;
    } else {
        thisv->actor.speedXZ = -10.0f;
    }
    thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->timer = 6;
    thisv->approachRate = 0.0f;
    thisv->unkFloat = 0.0f;
    thisv->action = GELDB_SPIN_DODGE;

    EnGeldB_SetupAction(thisv, EnGeldB_SpinDodge);
}

void EnGeldB_SpinDodge(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 phi_v1;
    s32 thisKeyFrame;
    s32 pad;
    s32 lastKeyFrame;
    s32 nextKeyFrame;

    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer + 0x3A98;
    if ((thisv->actor.bgCheckFlags & 8) ||
        !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.shape.rot.y + 0x3E80)) {
        if (thisv->actor.bgCheckFlags & 8) {
            if (thisv->actor.speedXZ >= 0.0f) {
                phi_v1 = thisv->actor.shape.rot.y + 0x3E80;
            } else {
                phi_v1 = thisv->actor.shape.rot.y - 0x3E80;
            }
            phi_v1 = thisv->actor.wallYaw - phi_v1;
        } else {
            thisv->actor.speedXZ *= -0.8f;
            phi_v1 = 0;
        }
        if (ABS(phi_v1) > 0x4000) {
            EnGeldB_SetupJump(thisv);
            return;
        }
    }
    if (thisv->actor.xzDistToPlayer <= 45.0f) {
        Math_SmoothStepToF(&thisv->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if (thisv->actor.xzDistToPlayer > 40.0f) {
        Math_SmoothStepToF(&thisv->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
    }
    if (thisv->approachRate != 0.0f) {
        thisv->actor.world.pos.x += Math_SinS(thisv->actor.yawTowardsPlayer) * thisv->approachRate;
        thisv->actor.world.pos.z += Math_CosS(thisv->actor.yawTowardsPlayer) * thisv->approachRate;
    }
    if (ABS(thisv->approachRate) < ABS(thisv->actor.speedXZ)) {
        thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
    } else {
        thisv->skelAnime.playSpeed = -thisv->approachRate * 0.5f;
    }
    thisv->skelAnime.playSpeed = CLAMP(thisv->skelAnime.playSpeed, -3.0f, 3.0f);
    thisKeyFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    lastKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
    nextKeyFrame = (s32)ABS(thisv->skelAnime.playSpeed) + thisKeyFrame;
    if ((thisKeyFrame != (s32)thisv->skelAnime.curFrame) &&
        ((lastKeyFrame < 0 && 0 < nextKeyFrame) || (lastKeyFrame < 5 && 5 < nextKeyFrame))) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_LAND);
    }
    if ((globalCtx->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
    thisv->timer--;
    if (thisv->timer == 0) {
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        if (!EnGeldB_DodgeRanged(globalCtx, thisv)) {
            if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (thisv->actor.xzDistToPlayer <= 70.0f)) {
                EnGeldB_SetupSlash(thisv);
            } else {
                EnGeldB_SetupRollBack(thisv);
            }
        }
    } else {
        if (thisv->actor.speedXZ >= 0.0f) {
            thisv->actor.shape.rot.y += 0x4000;
        } else {
            thisv->actor.shape.rot.y -= 0x4000;
        }
    }
}

void EnGeldB_SetupSlash(EnGeldB* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gGerudoRedSlashAnim);
    thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
    thisv->action = GELDB_SLASH;
    thisv->spinAttackState = 0;
    thisv->actor.speedXZ = 0.0f;
    Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_GERUDOFT_BREATH);
    EnGeldB_SetupAction(thisv, EnGeldB_Slash);
}

void EnGeldB_Slash(EnGeldB* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 angleFacingLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
    s16 angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    angleFacingLink = ABS(angleFacingLink);
    angleToLink = ABS(angleToLink);

    thisv->actor.speedXZ = 0.0f;
    if ((s32)thisv->skelAnime.curFrame == 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_ATTACK);
        thisv->swordState = 1;
    } else if ((s32)thisv->skelAnime.curFrame == 6) {
        thisv->swordState = -1;
    }
    if (thisv->swordCollider.base.atFlags & AT_BOUNCED) {
        thisv->swordState = -1;
        thisv->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        EnGeldB_SetupRollBack(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!Actor_IsFacingPlayer(&thisv->actor, 0x1554)) {
            EnGeldB_SetupReady(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            if (angleToLink > 0x4000) {
                thisv->lookTimer = 20;
            }
        } else if (Rand_ZeroOne() > 0.7f || (thisv->actor.xzDistToPlayer >= 120.0f)) {
            EnGeldB_SetupReady(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
        } else {
            thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                EnGeldB_SetupSidestep(thisv, globalCtx);
            } else if (angleFacingLink <= 0x2710) {
                if (angleToLink > 0x3E80) {
                    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                    EnGeldB_SetupCircle(thisv);
                } else {
                    EnGeldB_ReactToPlayer(globalCtx, thisv, 1);
                }
            } else {
                EnGeldB_SetupCircle(thisv);
            }
        }
    }
}

void EnGeldB_SetupSpinAttack(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedSpinAttackAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedSpinAttackAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE_INTERP, 0.0f);
    thisv->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
    thisv->action = GELDB_SPIN_ATTACK;
    thisv->spinAttackState = 0;
    thisv->actor.speedXZ = 0.0f;
    EnGeldB_SetupAction(thisv, EnGeldB_SpinAttack);
}

void EnGeldB_SpinAttack(EnGeldB* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 angleFacingLink;
    s16 angleToLink;

    if (thisv->spinAttackState < 2) {
        if (thisv->swordCollider.base.atFlags & AT_BOUNCED) {
            thisv->swordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            thisv->spinAttackState = 1;
            thisv->skelAnime.playSpeed = 1.5f;
        } else if (thisv->swordCollider.base.atFlags & AT_HIT) {
            thisv->swordCollider.base.atFlags &= ~AT_HIT;
            if (&player->actor == thisv->swordCollider.base.at) {
                func_8002F71C(globalCtx, &thisv->actor, 6.0f, thisv->actor.yawTowardsPlayer, 6.0f);
                thisv->spinAttackState = 2;
                func_8002DF54(globalCtx, &thisv->actor, 0x18);
                Message_StartTextbox(globalCtx, 0x6003, &thisv->actor);
                thisv->timer = 30;
                thisv->actor.speedXZ = 0.0f;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_LAUGH);
                return;
            }
        }
    }
    if ((s32)thisv->skelAnime.curFrame < 9) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    } else if ((s32)thisv->skelAnime.curFrame == 13) {
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->leftFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->rightFootPos, 3.0f, 2, 2.0f, 0, 0, 0);
        thisv->swordState = 1;
        thisv->actor.speedXZ = 10.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_ATTACK);
    } else if ((s32)thisv->skelAnime.curFrame == 21) {
        thisv->actor.speedXZ = 0.0f;
    } else if ((s32)thisv->skelAnime.curFrame == 24) {
        thisv->swordState = -1;
    }
    if (SkelAnime_Update(&thisv->skelAnime) && (thisv->spinAttackState < 2)) {
        if (!Actor_IsFacingPlayer(&thisv->actor, 0x1554)) {
            EnGeldB_SetupReady(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            thisv->lookTimer = 46;
        } else if (thisv->spinAttackState != 0) {
            EnGeldB_SetupRollBack(thisv);
        } else if (Rand_ZeroOne() > 0.7f || (thisv->actor.xzDistToPlayer >= 120.0f)) {
            EnGeldB_SetupReady(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
        } else {
            thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                EnGeldB_SetupSidestep(thisv, globalCtx);
            } else {
                angleFacingLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
                angleFacingLink = ABS(angleFacingLink);
                if (angleFacingLink <= 0x2710) {
                    angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
                    angleToLink = ABS(angleToLink);
                    if (angleToLink > 0x3E80) {
                        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                        EnGeldB_SetupCircle(thisv);
                    } else {
                        EnGeldB_ReactToPlayer(globalCtx, thisv, 1);
                    }
                } else {
                    EnGeldB_SetupCircle(thisv);
                }
            }
        }
    }
}

void EnGeldB_SetupRollBack(EnGeldB* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gGerudoRedFlipAnim, -3.0f);
    thisv->timer = 0;
    thisv->invisible = true;
    thisv->action = GELDB_ROLL_BACK;
    thisv->actor.speedXZ = -8.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    EnGeldB_SetupAction(thisv, EnGeldB_RollBack);
}

void EnGeldB_RollBack(EnGeldB* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (thisv->actor.xzDistToPlayer < 170.0f) &&
            (thisv->actor.xzDistToPlayer > 140.0f) && (Rand_ZeroOne() < 0.2f)) {
            EnGeldB_SetupSpinAttack(thisv);
        } else if (globalCtx->gameplayFrames & 1) {
            EnGeldB_SetupSidestep(thisv, globalCtx);
        } else {
            EnGeldB_SetupReady(thisv);
        }
    }
    if ((globalCtx->state.frames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
    }
}

void EnGeldB_SetupStunned(EnGeldB* thisv) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = 0.0f;
    }
    if ((thisv->damageEffect != GELDB_DMG_FREEZE) || (thisv->action == GELDB_SPIN_ATTACK)) {
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gGerudoRedDamageAnim, 0.0f);
    }
    if (thisv->damageEffect == GELDB_DMG_FREEZE) {
        thisv->iceTimer = 36;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->action = GELDB_STUNNED;
    EnGeldB_SetupAction(thisv, EnGeldB_Stunned);
}

void EnGeldB_Stunned(EnGeldB* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }
        thisv->invisible = false;
    }
    if ((thisv->actor.colorFilterTimer == 0) && (thisv->actor.bgCheckFlags & 1)) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnGeldB_SetupDefeated(thisv);
        } else {
            EnGeldB_ReactToPlayer(globalCtx, thisv, 1);
        }
    }
}

void EnGeldB_SetupDamaged(EnGeldB* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gGerudoRedDamageAnim, -4.0f);
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->invisible = false;
        thisv->actor.speedXZ = -4.0f;
    } else {
        thisv->invisible = true;
    }
    thisv->lookTimer = 0;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_DAMAGE);
    thisv->action = GELDB_DAMAGED;
    EnGeldB_SetupAction(thisv, EnGeldB_Damaged);
}

void EnGeldB_Damaged(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 angleToWall;

    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }
        thisv->invisible = false;
    }
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x1194, 0);
    if (!EnGeldB_DodgeRanged(globalCtx, thisv) && !EnGeldB_ReactToPlayer(globalCtx, thisv, 0) &&
        SkelAnime_Update(&thisv->skelAnime) && (thisv->actor.bgCheckFlags & 1)) {
        angleToWall = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
        if ((thisv->actor.bgCheckFlags & 8) && (ABS(angleToWall) < 0x2EE0) && (thisv->actor.xzDistToPlayer < 90.0f)) {
            EnGeldB_SetupJump(thisv);
        } else if (!EnGeldB_DodgeRanged(globalCtx, thisv)) {
            if ((thisv->actor.xzDistToPlayer <= 45.0f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                (globalCtx->gameplayFrames & 7)) {
                EnGeldB_SetupSlash(thisv);
            } else {
                EnGeldB_SetupRollBack(thisv);
            }
        }
    }
}

void EnGeldB_SetupJump(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedFlipAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedFlipAnim, -1.0f, lastFrame, 0.0f, ANIMMODE_ONCE, -3.0f);
    thisv->timer = 0;
    thisv->invisible = false;
    thisv->action = GELDB_JUMP;
    thisv->actor.speedXZ = 6.5f;
    thisv->actor.velocity.y = 15.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnGeldB_SetupAction(thisv, EnGeldB_Jump);
}

void EnGeldB_Jump(EnGeldB* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    if (thisv->actor.velocity.y >= 5.0f) {
        func_800355B8(globalCtx, &thisv->leftFootPos);
        func_800355B8(globalCtx, &thisv->rightFootPos);
    }
    if (SkelAnime_Update(&thisv->skelAnime) && (thisv->actor.bgCheckFlags & 3)) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.shape.rot.x = 0;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            EnGeldB_SetupSlash(thisv);
        } else {
            EnGeldB_SetupReady(thisv);
        }
    }
}

void EnGeldB_SetupBlock(EnGeldB* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedBlockAnim);

    if (thisv->swordState != 0) {
        thisv->swordState = -1;
    }
    thisv->actor.speedXZ = 0.0f;
    thisv->action = GELDB_BLOCK;
    thisv->timer = (s32)Rand_CenteredFloat(10.0f) + 10;
    Animation_Change(&thisv->skelAnime, &gGerudoRedBlockAnim, 0.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    EnGeldB_SetupAction(thisv, EnGeldB_Block);
}

void EnGeldB_Block(EnGeldB* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s16 angleToLink;
    s16 angleFacingLink;

    if (thisv->timer != 0) {
        thisv->timer--;
    } else {
        thisv->skelAnime.playSpeed = 1.0f;
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        angleToLink = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        if ((ABS(angleToLink) <= 0x4000) && (thisv->actor.xzDistToPlayer < 40.0f) &&
            (ABS(thisv->actor.yDistToPlayer) < 50.0f)) {
            if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x2710, 0x4000, thisv->actor.shape.rot.y)) {
                if (player->swordAnimation == 0x11) {
                    EnGeldB_SetupSpinDodge(thisv, globalCtx);
                } else if (globalCtx->gameplayFrames & 1) {
                    EnGeldB_SetupBlock(thisv);
                } else {
                    EnGeldB_SetupRollBack(thisv);
                }
            } else {
                angleFacingLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
                if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                    ((globalCtx->gameplayFrames & 1) || (ABS(angleFacingLink) < 0x38E0))) {
                    EnGeldB_SetupSlash(thisv);
                } else {
                    EnGeldB_SetupCircle(thisv);
                }
            }
        } else {
            EnGeldB_SetupCircle(thisv);
        }
    } else if ((thisv->timer == 0) &&
               func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x2710, 0x4000, thisv->actor.shape.rot.y)) {
        if (player->swordAnimation == 0x11) {
            EnGeldB_SetupSpinDodge(thisv, globalCtx);
        } else if (!EnGeldB_DodgeRanged(globalCtx, thisv)) {
            if ((globalCtx->gameplayFrames & 1)) {
                if ((thisv->actor.xzDistToPlayer < 100.0f) && (Rand_ZeroOne() > 0.7f)) {
                    EnGeldB_SetupJump(thisv);
                } else {
                    EnGeldB_SetupRollBack(thisv);
                }
            } else {
                EnGeldB_SetupBlock(thisv);
            }
        }
    }
}

void EnGeldB_SetupSidestep(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 linkAngle;
    Player* player;
    f32 lastFrame = Animation_GetLastFrame(&gGerudoRedSidestepAnim);

    Animation_Change(&thisv->skelAnime, &gGerudoRedSidestepAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, 0.0f);
    player = GET_PLAYER(globalCtx);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    linkAngle = player->actor.shape.rot.y;
    if (Math_SinS(linkAngle - thisv->actor.shape.rot.y) > 0.0f) {
        thisv->actor.speedXZ = -6.0f;
    } else if (Math_SinS(linkAngle - thisv->actor.shape.rot.y) < 0.0f) {
        thisv->actor.speedXZ = 6.0f;
    } else {
        thisv->actor.speedXZ = Rand_CenteredFloat(12.0f);
    }
    thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
    thisv->approachRate = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
    thisv->timer = Rand_ZeroOne() * 10.0f + 5.0f;
    thisv->action = GELDB_SIDESTEP;
    EnGeldB_SetupAction(thisv, EnGeldB_Sidestep);
}

void EnGeldB_Sidestep(EnGeldB* thisv, GlobalContext* globalCtx) {
    s16 behindLinkAngle;
    s16 phi_v1;
    Player* player = GET_PLAYER(globalCtx);
    s32 thisKeyFrame;
    s32 prevKeyFrame;
    f32 playSpeed;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xBB8, 1);
    behindLinkAngle = player->actor.shape.rot.y + 0x8000;
    if (Math_SinS(behindLinkAngle - thisv->actor.shape.rot.y) > 0.0f) {
        thisv->actor.speedXZ += 0.125f;
    } else if (Math_SinS(behindLinkAngle - thisv->actor.shape.rot.y) <= 0.0f) {
        thisv->actor.speedXZ -= 0.125f;
    }

    if ((thisv->actor.bgCheckFlags & 8) ||
        !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.shape.rot.y + 0x3E80)) {
        if (thisv->actor.bgCheckFlags & 8) {
            if (thisv->actor.speedXZ >= 0.0f) {
                phi_v1 = thisv->actor.shape.rot.y + 0x3E80;
            } else {
                phi_v1 = thisv->actor.shape.rot.y - 0x3E80;
            }
            phi_v1 = thisv->actor.wallYaw - phi_v1;
        } else {
            thisv->actor.speedXZ *= -0.8f;
            phi_v1 = 0;
        }
        if (ABS(phi_v1) > 0x4000) {
            thisv->actor.speedXZ *= -0.8f;
            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ -= 0.5f;
            } else {
                thisv->actor.speedXZ += 0.5f;
            }
        }
    }
    if (thisv->actor.speedXZ >= 0.0f) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3E80;
    } else {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y - 0x3E80;
    }
    if (thisv->actor.xzDistToPlayer <= 45.0f) {
        Math_SmoothStepToF(&thisv->approachRate, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if (thisv->actor.xzDistToPlayer > 40.0f) {
        Math_SmoothStepToF(&thisv->approachRate, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->approachRate, 0.0f, 1.0f, 6.65f, 0.0f);
    }
    if (thisv->approachRate != 0.0f) {
        thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->approachRate;
        thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->approachRate;
    }
    if (ABS(thisv->approachRate) < ABS(thisv->actor.speedXZ)) {
        thisv->skelAnime.playSpeed = -thisv->actor.speedXZ * 0.5f;
    } else {
        thisv->skelAnime.playSpeed = -thisv->approachRate * 0.5f;
    }
    thisv->skelAnime.playSpeed = CLAMP(thisv->skelAnime.playSpeed, -3.0f, 3.0f);
    thisKeyFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    prevKeyFrame = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);

    playSpeed = ((void)0, ABS(thisv->skelAnime.playSpeed)); // Needed to match for some reason

    if (!EnGeldB_DodgeRanged(globalCtx, thisv) && !EnGeldB_ReactToPlayer(globalCtx, thisv, 0)) {
        if (--thisv->timer == 0) {
            s16 angleFacingPlayer = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

            angleFacingPlayer = ABS(angleFacingPlayer);
            if (angleFacingPlayer >= 0x3A98) {
                EnGeldB_SetupReady(thisv);
                thisv->timer = (Rand_ZeroOne() * 5.0f) + 1.0f;
            } else {
                Player* player2 = GET_PLAYER(globalCtx);
                s16 angleFacingPlayer2 = player2->actor.shape.rot.y - thisv->actor.shape.rot.y;

                thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
                if ((thisv->actor.xzDistToPlayer <= 45.0f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                    (!(globalCtx->gameplayFrames & 3) || (ABS(angleFacingPlayer2) < 0x38E0))) {
                    EnGeldB_SetupSlash(thisv);
                } else if ((210.0f > thisv->actor.xzDistToPlayer) && (thisv->actor.xzDistToPlayer > 150.0f) &&
                           !(globalCtx->gameplayFrames & 1)) {
                    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) || (Rand_ZeroOne() > 0.5f) ||
                        (ABS(angleFacingPlayer2) < 0x38E0)) {
                        EnGeldB_SetupRollForward(thisv);
                    } else {
                        EnGeldB_SetupSpinAttack(thisv);
                    }
                } else {
                    EnGeldB_SetupAdvance(thisv, globalCtx);
                }
            }
        }
        if ((thisKeyFrame != (s32)thisv->skelAnime.curFrame) &&
            (((prevKeyFrame < 0) && (((s32)playSpeed + thisKeyFrame) > 0)) ||
             ((prevKeyFrame < 5) && (((s32)playSpeed + thisKeyFrame) > 5)))) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_LAND);
        }
        if ((globalCtx->gameplayFrames & 0x5F) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_BREATH);
        }
    }
}

void EnGeldB_SetupDefeated(EnGeldB* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gGerudoRedDefeatAnim, -4.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->invisible = false;
        thisv->actor.speedXZ = -6.0f;
    } else {
        thisv->invisible = true;
    }
    thisv->action = GELDB_DEFEAT;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GERUDOFT_DEAD);
    EnGeldB_SetupAction(thisv, EnGeldB_Defeated);
}

void EnGeldB_Defeated(EnGeldB* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
        thisv->invisible = false;
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnGeldB_SetupFlee(thisv);
    } else if ((s32)thisv->skelAnime.curFrame == 10) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        func_800F5B58();
    }
}

void EnGeldB_TurnHead(EnGeldB* thisv, GlobalContext* globalCtx) {
    if ((thisv->action == GELDB_READY) && (thisv->lookTimer != 0)) {
        thisv->headRot.y = Math_SinS(thisv->lookTimer * 0x1068) * 8920.0f;
    } else if (thisv->action != GELDB_STUNNED) {
        if ((thisv->action != GELDB_SLASH) && (thisv->action != GELDB_SPIN_ATTACK)) {
            Math_SmoothStepToS(&thisv->headRot.y, thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y, 1, 0x1F4, 0);
            thisv->headRot.y = CLAMP(thisv->headRot.y, -0x256F, 0x256F);
        } else {
            thisv->headRot.y = 0;
        }
    }
}

void EnGeldB_CollisionCheck(EnGeldB* thisv, GlobalContext* globalCtx) {
    s32 pad;
    EnItem00* key;

    if (thisv->blockCollider.base.acFlags & AC_BOUNCED) {
        thisv->blockCollider.base.acFlags &= ~AC_BOUNCED;
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;
    } else if ((thisv->bodyCollider.base.acFlags & AC_HIT) && (thisv->action >= GELDB_READY) &&
               (thisv->spinAttackState < 2)) {
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != GELDB_DMG_UNK_6) {
            thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
            Actor_SetDropFlag(&thisv->actor, &thisv->bodyCollider.info, 1);
            Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_GERUDOFT_BREATH);
            if ((thisv->actor.colChkInfo.damageEffect == GELDB_DMG_STUN) ||
                (thisv->actor.colChkInfo.damageEffect == GELDB_DMG_FREEZE)) {
                if (thisv->action != GELDB_STUNNED) {
                    Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);
                    Actor_ApplyDamage(&thisv->actor);
                    EnGeldB_SetupStunned(thisv);
                }
            } else {
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
                if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    if (thisv->keyFlag != 0) {
                        key = Item_DropCollectible(globalCtx, &thisv->actor.world.pos, thisv->keyFlag | ITEM00_SMALL_KEY);
                        if (key != NULL) {
                            key->actor.world.rot.y = Math_Vec3f_Yaw(&key->actor.world.pos, &thisv->actor.home.pos);
                            key->actor.speedXZ = 6.0f;
                            Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                    }
                    EnGeldB_SetupDefeated(thisv);
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                } else {
                    EnGeldB_SetupDamaged(thisv);
                }
            }
        }
    }
}

void EnGeldB_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnGeldB* thisv = (EnGeldB*)thisx;

    EnGeldB_CollisionCheck(thisv, globalCtx);
    if (thisv->actor.colChkInfo.damageEffect != GELDB_DMG_UNK_6) {
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 15.0f, 30.0f, 60.0f, 0x1D);
        thisv->actionFunc(thisv, globalCtx);
        thisv->actor.focus.pos = thisv->actor.world.pos;
        thisv->actor.focus.pos.y += 40.0f;
        EnGeldB_TurnHead(thisv, globalCtx);
    }
    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    if ((thisv->action >= GELDB_READY) && (thisv->spinAttackState < 2) &&
        ((thisv->actor.colorFilterTimer == 0) || !(thisv->actor.colorFilterParams & 0x4000))) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    }
    if ((thisv->action == GELDB_BLOCK) && (thisv->skelAnime.curFrame == 0.0f)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->blockCollider.base);
    }
    if (thisv->swordState > 0) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->swordCollider.base);
    }
    if (thisv->blinkState == 0) {
        if ((Rand_ZeroOne() < 0.1f) && ((globalCtx->gameplayFrames % 4) == 0)) {
            thisv->blinkState++;
        }
    } else {
        thisv->blinkState = (thisv->blinkState + 1) & 3;
    }
}

s32 EnGeldB_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnGeldB* thisv = (EnGeldB*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_geldB.c", 2507);
    if (limbIndex == GELDB_LIMB_NECK) {
        rot->z += thisv->headRot.x;
        rot->x += thisv->headRot.y;
        rot->y += thisv->headRot.z;
    } else if (limbIndex == GELDB_LIMB_HEAD) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
    } else if ((limbIndex == GELDB_LIMB_R_SWORD) || (limbIndex == GELDB_LIMB_L_SWORD)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    } else {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_geldB.c", 2529);
    return false;
}

void EnGeldB_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f footOffset = { 300.0f, 0.0f, 0.0f };
    static Vec3f swordTipOffset = { 0.0f, -3000.0f, 0.0f };
    static Vec3f swordHiltOffset = { 400.0f, 0.0f, 0.0f };
    static Vec3f swordQuadOffset1 = { 1600.0f, -4000.0f, 0.0f };
    static Vec3f swordQuadOffset0 = { -3000.0f, -2000.0f, 1300.0f };
    static Vec3f swordQuadOffset3 = { -3000.0f, -2000.0f, -1300.0f };
    static Vec3f swordQuadOffset2 = { 1000.0f, 1000.0f, 0.0f };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f swordTip;
    Vec3f swordHilt;
    EnGeldB* thisv = (EnGeldB*)thisx;
    s32 bodyPart = -1;

    if (limbIndex == GELDB_LIMB_R_SWORD) {
        Matrix_MultVec3f(&swordQuadOffset1, &thisv->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&swordQuadOffset0, &thisv->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&swordQuadOffset3, &thisv->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&swordQuadOffset2, &thisv->swordCollider.dim.quad[2]);
        Collider_SetQuadVertices(&thisv->swordCollider, &thisv->swordCollider.dim.quad[0],
                                 &thisv->swordCollider.dim.quad[1], &thisv->swordCollider.dim.quad[2],
                                 &thisv->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&swordTipOffset, &swordTip);
        Matrix_MultVec3f(&swordHiltOffset, &swordHilt);

        if ((thisv->swordState < 0) || ((thisv->action != GELDB_SLASH) && (thisv->action != GELDB_SPIN_ATTACK))) {
            EffectBlure_AddSpace(Effect_GetByIndex(thisv->blureIndex));
            thisv->swordState = 0;
        } else if (thisv->swordState > 0) {
            EffectBlure_AddVertex(Effect_GetByIndex(thisv->blureIndex), &swordTip, &swordHilt);
        }
    } else {
        Actor_SetFeetPos(&thisv->actor, limbIndex, GELDB_LIMB_L_FOOT, &footOffset, GELDB_LIMB_R_FOOT, &footOffset);
    }

    if (limbIndex == GELDB_LIMB_L_FOOT) {
        Matrix_MultVec3f(&footOffset, &thisv->leftFootPos);
    } else if (limbIndex == GELDB_LIMB_R_FOOT) {
        Matrix_MultVec3f(&footOffset, &thisv->rightFootPos);
    }

    if (thisv->iceTimer != 0) {
        switch (limbIndex) {
            case GELDB_LIMB_NECK:
                bodyPart = 0;
                break;
            case GELDB_LIMB_L_SWORD:
                bodyPart = 1;
                break;
            case GELDB_LIMB_R_SWORD:
                bodyPart = 2;
                break;
            case GELDB_LIMB_L_UPPER_ARM:
                bodyPart = 3;
                break;
            case GELDB_LIMB_R_UPPER_ARM:
                bodyPart = 4;
                break;
            case GELDB_LIMB_TORSO:
                bodyPart = 5;
                break;
            case GELDB_LIMB_WAIST:
                bodyPart = 6;
                break;
            case GELDB_LIMB_L_FOOT:
                bodyPart = 7;
                break;
            case GELDB_LIMB_R_FOOT:
                bodyPart = 8;
                break;
            default:
                break;
        }

        if (bodyPart >= 0) {
            Vec3f limbPos;

            Matrix_MultVec3f(&zeroVec, &limbPos);
            thisv->bodyPartsPos[bodyPart].x = limbPos.x;
            thisv->bodyPartsPos[bodyPart].y = limbPos.y;
            thisv->bodyPartsPos[bodyPart].z = limbPos.z;
        }
    }
}

void EnGeldB_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f blockTrisOffsets0[3] = {
        { -3000.0f, 6000.0f, 1600.0f },
        { -3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 6000.0f, 1600.0f },
    };
    static Vec3f blockTrisOffsets1[3] = {
        { -3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 0.0f, 1600.0f },
        { 3000.0f, 6000.0f, 1600.0f },
    };
    static void* eyeTextures[] = { gGerudoRedEyeOpenTex, gGerudoRedEyeHalfTex, gGerudoRedEyeShutTex,
                                   gGerudoRedEyeHalfTex };
    s32 pad;
    EnGeldB* thisv = (EnGeldB*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_geldB.c", 2672);
    if (1) {}

    if ((thisv->spinAttackState >= 2) && SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->spinAttackState == 2) {
            Animation_Change(&thisv->skelAnime, &gGerudoRedSpinAttackAnim, 0.5f, 0.0f, 12.0f, ANIMMODE_ONCE_INTERP,
                             4.0f);
            thisv->spinAttackState++;
            thisx->world.rot.y = thisx->shape.rot.y = thisx->yawTowardsPlayer;
        } else {
            thisv->timer--;
            if (thisv->timer == 0) {
                if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
                    globalCtx->nextEntranceIndex = 0x1A5;
                } else if (gSaveContext.eventChkInf[12] & 0x80) {
                    globalCtx->nextEntranceIndex = 0x5F8;
                } else {
                    globalCtx->nextEntranceIndex = 0x3B4;
                }
                globalCtx->fadeTransition = 0x26;
                globalCtx->sceneLoadFlag = 0x14;
            }
        }
    }

    if ((thisv->action != GELDB_WAIT) || !thisv->invisible) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->blinkState]));
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnGeldB_OverrideLimbDraw, EnGeldB_PostLimbDraw, thisv);
        if (thisv->action == GELDB_BLOCK) {
            s32 i;
            Vec3f blockTrisVtx0[3];
            Vec3f blockTrisVtx1[3];

            for (i = 0; i < 3; i++) {
                Matrix_MultVec3f(&blockTrisOffsets0[i], &blockTrisVtx0[i]);
                Matrix_MultVec3f(&blockTrisOffsets1[i], &blockTrisVtx1[i]);
            }
            Collider_SetTrisVertices(&thisv->blockCollider, 0, &blockTrisVtx0[0], &blockTrisVtx0[1], &blockTrisVtx0[2]);
            Collider_SetTrisVertices(&thisv->blockCollider, 1, &blockTrisVtx1[0], &blockTrisVtx1[1], &blockTrisVtx1[2]);
        }

        if (thisv->iceTimer != 0) {
            thisx->colorFilterTimer++;
            thisv->iceTimer--;
            if ((thisv->iceTimer % 4) == 0) {
                s32 iceIndex = thisv->iceTimer >> 2;

                EffectSsEnIce_SpawnFlyingVec3s(globalCtx, thisx, &thisv->bodyPartsPos[iceIndex], 150, 150, 150, 250, 235,
                                               245, 255, 1.5f);
            }
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_geldB.c", 2744);
}

s32 EnGeldB_DodgeRanged(GlobalContext* globalCtx, EnGeldB* thisv) {
    Actor* actor = Actor_GetProjectileActor(globalCtx, &thisv->actor, 800.0f);

    if (actor != NULL) {
        s16 angleToFacing;
        s16 pad18;
        f32 dist;

        angleToFacing = Actor_WorldYawTowardActor(&thisv->actor, actor) - thisv->actor.shape.rot.y;
        thisv->actor.world.rot.y = (u16)thisv->actor.shape.rot.y & 0xFFFF;
        dist = Actor_WorldDistXYZToPoint(&thisv->actor, &actor->world.pos);
        //! @bug
        // Actor_WorldDistXYZToPoint already sqrtfs the distance, so thisv actually checks for a
        // distance of 360000. Also it's a double calculation because no f on sqrt.
        if ((ABS(angleToFacing) < 0x2EE0) && (sqrt(dist) < 600.0)) {
            if (actor->id == ACTOR_ARMS_HOOK) {
                EnGeldB_SetupJump(thisv);
            } else {
                EnGeldB_SetupBlock(thisv);
            }
        } else {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
            if ((ABS(angleToFacing) < 0x2000) || (ABS(angleToFacing) > 0x5FFF)) {
                EnGeldB_SetupSidestep(thisv, globalCtx);
                thisv->actor.speedXZ *= 3.0f;
            } else if (ABS(angleToFacing) < 0x5FFF) {
                EnGeldB_SetupRollBack(thisv);
            }
        }
        return true;
    }
    return false;
}
