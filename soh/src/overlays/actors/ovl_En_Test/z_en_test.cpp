/*
 * File: z_en_test.c
 * Overlay: ovl_En_Test
 * Description: Stalfos
 */

#include "z_en_test.h"
#include "objects/object_sk2/object_sk2.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnTest_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTest_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTest_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTest_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnTest_SetupWaitGround(EnTest* thisv);
void EnTest_SetupWaitAbove(EnTest* thisv);
void EnTest_SetupJumpBack(EnTest* thisv);
void EnTest_SetupSlashDownEnd(EnTest* thisv);
void EnTest_SetupSlashUp(EnTest* thisv);
void EnTest_SetupJumpslash(EnTest* thisv);
void EnTest_SetupWalkAndBlock(EnTest* thisv);
void func_80860EC0(EnTest* thisv);
void EnTest_SetupSlashDown(EnTest* thisv);
void func_80860BDC(EnTest* thisv);
void EnTest_SetupIdleFromBlock(EnTest* thisv);
void EnTest_SetupRecoil(EnTest* thisv);
void func_80862398(EnTest* thisv);
void func_80862154(EnTest* thisv);
void EnTest_SetupStopAndBlock(EnTest* thisv);
void func_808627C4(EnTest* thisv, GlobalContext* globalCtx);

void EnTest_WaitGround(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_WaitAbove(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Fall(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Land(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Rise(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Idle(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_WalkAndBlock(EnTest* thisv, GlobalContext* globalCtx);
void func_80860C24(EnTest* thisv, GlobalContext* globalCtx);
void func_80860F84(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_SlashDown(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_SlashDownEnd(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_SlashUp(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_JumpBack(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Jumpslash(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_JumpUp(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_StopAndBlock(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_IdleFromBlock(EnTest* thisv, GlobalContext* globalCtx);
void func_808621D4(EnTest* thisv, GlobalContext* globalCtx);
void func_80862418(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Stunned(EnTest* thisv, GlobalContext* globalCtx);
void func_808628C8(EnTest* thisv, GlobalContext* globalCtx);
void func_80862E6C(EnTest* thisv, GlobalContext* globalCtx);
void func_80863044(EnTest* thisv, GlobalContext* globalCtx);
void func_8086318C(EnTest* thisv, GlobalContext* globalCtx);
void EnTest_Recoil(EnTest* thisv, GlobalContext* globalCtx);
void func_808633E8(EnTest* thisv, GlobalContext* globalCtx);
void func_80862FA8(EnTest* thisv, GlobalContext* globalCtx);

s32 EnTest_ReactToProjectile(GlobalContext* globalCtx, EnTest* thisv);

static u8 sJointCopyFlags[] = {
    false, // STALFOS_LIMB_NONE
    false, // STALFOS_LIMB_ROOT
    false, // STALFOS_LIMB_UPPERBODY_ROOT
    false, // STALFOS_LIMB_CORE_LOWER_ROOT
    true,  // STALFOS_LIMB_CORE_UPPER_ROOT
    true,  // STALFOS_LIMB_NECK_ROOT
    true,  // STALFOS_LIMB_HEAD_ROOT
    true,  // STALFOS_LIMB_7
    true,  // STALFOS_LIMB_8
    true,  // STALFOS_LIMB_JAW_ROOT
    true,  // STALFOS_LIMB_JAW
    true,  // STALFOS_LIMB_HEAD
    true,  // STALFOS_LIMB_NECK_UPPER
    true,  // STALFOS_LIMB_NECK_LOWER
    true,  // STALFOS_LIMB_CORE_UPPER
    true,  // STALFOS_LIMB_CHEST
    true,  // STALFOS_LIMB_SHOULDER_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_R
    true,  // STALFOS_LIMB_SHOULDER_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L_ROOT
    true,  // STALFOS_LIMB_SHOULDER_ARMOR_L
    true,  // STALFOS_LIMB_ARM_L_ROOT
    true,  // STALFOS_LIMB_UPPERARM_L_ROOT
    true,  // STALFOS_LIMB_FOREARM_L_ROOT
    true,  // STALFOS_LIMB_HAND_L_ROOT
    true,  // STALFOS_LIMB_HAND_L
    true,  // STALFOS_LIMB_SHIELD
    true,  // STALFOS_LIMB_FOREARM_L
    true,  // STALFOS_LIMB_UPPERARM_L
    true,  // STALFOS_LIMB_ARM_R_ROOT
    true,  // STALFOS_LIMB_UPPERARM_R_ROOT
    true,  // STALFOS_LIMB_FOREARM_R_ROOT
    true,  // STALFOS_LIMB_HAND_R_ROOT
    true,  // STALFOS_LIMB_SWORD
    true,  // STALFOS_LIMB_HAND_R
    true,  // STALFOS_LIMB_FOREARM_R
    true,  // STALFOS_LIMB_UPPERARM_R
    true,  // STALFOS_LIMB_CORE_LOWER
    false, // STALFOS_LIMB_LOWERBODY_ROOT
    false, // STALFOS_LIMB_WAIST_ROOT
    false, // STALFOS_LIMB_LEGS_ROOT
    false, // STALFOS_LIMB_LEG_L_ROOT
    false, // STALFOS_LIMB_THIGH_L_ROOT
    false, // STALFOS_LIMB_LOWERLEG_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L_ROOT
    false, // STALFOS_LIMB_ANKLE_L
    false, // STALFOS_LIMB_FOOT_L_ROOT
    false, // STALFOS_LIMB_FOOT_L
    false, // STALFOS_LIMB_LOWERLEG_L
    false, // STALFOS_LIMB_THIGH_L
    false, // STALFOS_LIMB_LEG_R_ROOT
    false, // STALFOS_LIMB_THIGH_R_ROOT
    false, // STALFOS_LIMB_LOWERLEG_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R_ROOT
    false, // STALFOS_LIMB_ANKLE_R
    false, // STALFOS_LIMB_FOOT_R_ROOT
    false, // STALFOS_LIMB_FOOT_R
    false, // STALFOS_LIMB_LOWERLEG_R
    false, // STALFOS_LIMB_THIGH_R
    false, // STALFOS_LIMB_WAIST
};

ActorInit En_Test_InitVars = {
    ACTOR_EN_TEST,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SK2,
    sizeof(EnTest),
    (ActorFunc)EnTest_Init,
    (ActorFunc)EnTest_Destroy,
    (ActorFunc)EnTest_Update,
    (ActorFunc)EnTest_Draw,
    NULL,
};

static ColliderCylinderInit sBodyColliderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sShieldColliderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFC1FFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 70, -50, { 0, 0, 0 } },
};

static ColliderQuadInit sSwordColliderInit = {
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
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ STALFOS_DMGEFF_NORMAL,
    /* 0x1 */ STALFOS_DMGEFF_STUN,
    /* 0x6 */ STALFOS_DMGEFF_FIREMAGIC = 6,
    /* 0xD */ STALFOS_DMGEFF_SLING = 0xD,
    /* 0xE */ STALFOS_DMGEFF_LIGHT,
    /* 0xF */ STALFOS_DMGEFF_FREEZE
} StalfosDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, STALFOS_DMGEFF_SLING),
    /* Explosive     */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, STALFOS_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Ice arrow     */ DMG_ENTRY(4, STALFOS_DMGEFF_FREEZE),
    /* Light arrow   */ DMG_ENTRY(2, STALFOS_DMGEFF_LIGHT),
    /* Unk arrow 1   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 2   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Unk arrow 3   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Fire magic    */ DMG_ENTRY(0, STALFOS_DMGEFF_FIREMAGIC),
    /* Ice magic     */ DMG_ENTRY(3, STALFOS_DMGEFF_FREEZE),
    /* Light magic   */ DMG_ENTRY(0, STALFOS_DMGEFF_LIGHT),
    /* Shield        */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Mirror Ray    */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Kokiri spin   */ DMG_ENTRY(1, STALFOS_DMGEFF_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, STALFOS_DMGEFF_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, STALFOS_DMGEFF_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Unblockable   */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
    /* Hammer jump   */ DMG_ENTRY(4, STALFOS_DMGEFF_NORMAL),
    /* Unknown 2     */ DMG_ENTRY(0, STALFOS_DMGEFF_NORMAL),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x1B, ICHAIN_CONTINUE),    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE), ICHAIN_F32(scale.y, 0, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1500, ICHAIN_STOP),
};

void EnTest_SetupAction(EnTest* thisv, EnTestActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnTest_Init(Actor* thisx, GlobalContext* globalCtx) {
    EffectBlureInit1 slashBlure;
    EnTest* thisv = (EnTest*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gStalfosSkel, &gStalfosMiddleGuardAnim, thisv->jointTable,
                   thisv->morphTable, STALFOS_LIMB_MAX);
    SkelAnime_Init(globalCtx, &thisv->upperSkelanime, &gStalfosSkel, &gStalfosMiddleGuardAnim, thisv->upperJointTable,
                   thisv->upperMorphTable, STALFOS_LIMB_MAX);

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);

    thisv->actor.colChkInfo.cylRadius = 40;
    thisv->actor.colChkInfo.cylHeight = 100;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 45.0f;
    thisv->actor.colChkInfo.damageTable = &sDamageTable;

    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, &thisv->actor, &sBodyColliderInit);

    Collider_InitCylinder(globalCtx, &thisv->shieldCollider);
    Collider_SetCylinder(globalCtx, &thisv->shieldCollider, &thisv->actor, &sShieldColliderInit);

    Collider_InitQuad(globalCtx, &thisv->swordCollider);
    Collider_SetQuad(globalCtx, &thisv->swordCollider, &thisv->actor, &sSwordColliderInit);

    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.colChkInfo.health = 10;

    slashBlure.p1StartColor[0] = slashBlure.p1StartColor[1] = slashBlure.p1StartColor[2] = slashBlure.p1StartColor[3] =
        slashBlure.p2StartColor[0] = slashBlure.p2StartColor[1] = slashBlure.p2StartColor[2] =
            slashBlure.p1EndColor[0] = slashBlure.p1EndColor[1] = slashBlure.p1EndColor[2] = slashBlure.p2EndColor[0] =
                slashBlure.p2EndColor[1] = slashBlure.p2EndColor[2] = 255;

    slashBlure.p1EndColor[3] = 0;
    slashBlure.p2EndColor[3] = 0;
    slashBlure.p2StartColor[3] = 64;

    slashBlure.elemDuration = 4;
    slashBlure.unkFlag = 0;
    slashBlure.calcMode = 2;

    Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE1, 0, 0, &slashBlure);

    if (thisv->actor.params != STALFOS_TYPE_CEILING) {
        EnTest_SetupWaitGround(thisv);
    } else {
        EnTest_SetupWaitAbove(thisv);
    }

    if (thisv->actor.params == STALFOS_TYPE_INVISIBLE) {
        thisv->actor.flags |= ACTOR_FLAG_7;
    }
}

void EnTest_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnTest* thisv = (EnTest*)thisx;

    if ((thisv->actor.params != STALFOS_TYPE_2) &&
        !Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
        func_800F5B58();
    }

    Effect_Delete(globalCtx, thisv->effectIndex);
    Collider_DestroyCylinder(globalCtx, &thisv->shieldCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyQuad(globalCtx, &thisv->swordCollider);
}

/**
 * If EnTest_ChooseAction failed to pick a new action, thisv function will unconditionally pick
 * a new action as a last resort
 */
void EnTest_ChooseRandomAction(EnTest* thisv, GlobalContext* globalCtx) {
    switch ((u32)(Rand_ZeroOne() * 10.0f)) {
        case 0:
        case 1:
        case 5:
        case 6:
            if ((thisv->actor.xzDistToPlayer < 220.0f) && (thisv->actor.xzDistToPlayer > 170.0f) &&
                Actor_IsFacingPlayer(&thisv->actor, 0x71C) && Actor_IsTargeted(globalCtx, &thisv->actor)) {
                EnTest_SetupJumpslash(thisv);
                break;
            }
            // fallthrough
        case 8:
            EnTest_SetupWalkAndBlock(thisv);
            break;

        case 3:
        case 4:
        case 7:
            func_808627C4(thisv, globalCtx);
            break;

        case 2:
        case 9:
        case 10:
            EnTest_SetupStopAndBlock(thisv);
            break;
    }
}

void EnTest_ChooseAction(EnTest* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

    yawDiff = ABS(yawDiff);

    if (yawDiff >= 0x61A8) {
        switch ((u32)(Rand_ZeroOne() * 10.0f)) {
            case 0:
            case 3:
            case 7:
                EnTest_SetupStopAndBlock(thisv);
                break;

            case 1:
            case 5:
            case 6:
            case 8:
                func_808627C4(thisv, globalCtx);
                break;

            case 2:
            case 4:
            case 9:
                if (thisv->actor.params != STALFOS_TYPE_CEILING) {
                    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                    EnTest_SetupJumpBack(thisv);
                }
                break;
        }
    } else if (yawDiff <= 0x3E80) {
        if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) > 0x3E80) {
            if (((globalCtx->gameplayFrames % 2) != 0) && (thisv->actor.params != STALFOS_TYPE_CEILING)) {
                thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                EnTest_SetupJumpBack(thisv);
            } else if ((thisv->actor.xzDistToPlayer < 220.0f) && (thisv->actor.xzDistToPlayer > 170.0f)) {
                if (Actor_IsFacingPlayer(&thisv->actor, 0x71C) && !Actor_IsTargeted(globalCtx, &thisv->actor)) {
                    EnTest_SetupJumpslash(thisv);
                }
            } else {
                EnTest_SetupWalkAndBlock(thisv);
            }
        } else {
            if (thisv->actor.xzDistToPlayer < 110.0f) {
                if (Rand_ZeroOne() > 0.2f) {
                    if (player->stateFlags1 & 0x10) {
                        if (thisv->actor.isTargeted) {
                            EnTest_SetupSlashDown(thisv);
                        } else {
                            func_808627C4(thisv, globalCtx);
                        }
                    } else {
                        EnTest_SetupSlashDown(thisv);
                    }
                }
            } else {
                EnTest_ChooseRandomAction(thisv, globalCtx);
            }
        }
    } else {
        EnTest_ChooseRandomAction(thisv, globalCtx);
    }
}

void EnTest_SetupWaitGround(EnTest* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosMiddleGuardAnim);
    thisv->unk_7C8 = 0;
    thisv->timer = 15;
    thisv->actor.scale.y = 0.0f;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y - 3.5f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnTest_SetupAction(thisv, EnTest_WaitGround);
}

void EnTest_WaitGround(EnTest* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((thisv->timer == 0) && (ABS(thisv->actor.yDistToPlayer) < 150.0f)) {
        thisv->unk_7C8 = 3;
        EnTest_SetupAction(thisv, EnTest_Rise);
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;

        if (thisv->actor.params != STALFOS_TYPE_2) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        thisv->actor.world.pos.y = thisv->actor.home.pos.y - 3.5f;
    }
}

void EnTest_SetupWaitAbove(EnTest* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosMiddleGuardAnim);
    thisv->unk_7C8 = 0;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 150.0f;
    Actor_SetScale(&thisv->actor, 0.0f);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnTest_SetupAction(thisv, EnTest_WaitAbove);
}

void EnTest_WaitAbove(EnTest* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 150.0f;

    if ((thisv->actor.xzDistToPlayer < 200.0f) && (ABS(thisv->actor.yDistToPlayer) < 450.0f)) {
        EnTest_SetupAction(thisv, EnTest_Fall);
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        Actor_SetScale(&thisv->actor, 0.015f);
    }
}

void EnTest_SetupIdle(EnTest* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosMiddleGuardAnim);
    thisv->unk_7C8 = 0xA;
    thisv->timer = (Rand_ZeroOne() * 10.0f) + 5.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnTest_SetupAction(thisv, EnTest_Idle);
}

void EnTest_Idle(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;

    SkelAnime_Update(&thisv->skelAnime);

    if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
        yawDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

        if (thisv->actor.xzDistToPlayer < 100.0f) {
            if ((player->swordState != 0) && (ABS(yawDiff) >= 0x1F40)) {
                thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

                if (Rand_ZeroOne() > 0.7f && player->swordAnimation != 0x11) {
                    EnTest_SetupJumpBack(thisv);
                } else {
                    func_808627C4(thisv, globalCtx);
                }
                return;
            }
        }

        if (thisv->timer != 0) {
            thisv->timer--;
        } else {
            if (Actor_IsFacingPlayer(&thisv->actor, 0x1555)) {
                if ((thisv->actor.xzDistToPlayer < 220.0f) && (thisv->actor.xzDistToPlayer > 160.0f) &&
                    (Rand_ZeroOne() < 0.3f)) {
                    if (Actor_IsTargeted(globalCtx, &thisv->actor)) {
                        EnTest_SetupJumpslash(thisv);
                    } else {
                        func_808627C4(thisv, globalCtx);
                    }
                } else {
                    if (Rand_ZeroOne() > 0.3f) {
                        EnTest_SetupWalkAndBlock(thisv);
                    } else {
                        func_808627C4(thisv, globalCtx);
                    }
                }
            } else {
                if (Rand_ZeroOne() > 0.7f) {
                    func_80860BDC(thisv);
                } else {
                    EnTest_ChooseAction(thisv, globalCtx);
                }
            }
        }
    }
}

void EnTest_Fall(EnTest* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gStalfosLandFromLeapAnim, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.world.pos.y <= thisv->actor.floorHeight) {
        thisv->skelAnime.playSpeed = 1.0f;
        thisv->unk_7C8 = 0xC;
        thisv->timer = thisv->unk_7E4 * 0.15f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        EnTest_SetupAction(thisv, EnTest_Land);
    }
}

void EnTest_Land(EnTest* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnTest_SetupIdle(thisv);
        thisv->timer = (Rand_ZeroOne() * 10.0f) + 5.0f;
    }
}

void EnTest_SetupWalkAndBlock(EnTest* thisv) {
    Animation_Change(&thisv->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosSlowAdvanceAnim);
    thisv->timer = (s16)(Rand_ZeroOne() * 5.0f);
    thisv->unk_7C8 = 0xD;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnTest_SetupAction(thisv, EnTest_WalkAndBlock);
}

void EnTest_WalkAndBlock(EnTest* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 checkDist = 0.0f;
    s32 pad1;
    s32 prevFrame;
    s32 temp_f16;
    f32 playSpeed;
    Player* player = GET_PLAYER(globalCtx);
    s32 absPlaySpeed;
    s16 yawDiff;

    if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
        thisv->timer++;

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            checkDist = 150.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, -5.0f, 1.0f, 0.8f, 0.0f);
        } else if (thisv->actor.xzDistToPlayer > (110.0f + checkDist)) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 5.0f, 1.0f, 0.8f, 0.0f);
        }

        if (thisv->actor.speedXZ >= 5.0f) {
            thisv->actor.speedXZ = 5.0f;
        } else if (thisv->actor.speedXZ < -5.0f) {
            thisv->actor.speedXZ = -5.0f;
        }

        if ((thisv->actor.params == STALFOS_TYPE_CEILING) &&
            !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.world.rot.y)) {
            thisv->actor.speedXZ *= -1.0f;
        }

        if (ABS(thisv->actor.speedXZ) < 3.0f) {
            Animation_Change(&thisv->skelAnime, &gStalfosSlowAdvanceAnim, 0.0f, thisv->skelAnime.curFrame,
                             Animation_GetLastFrame(&gStalfosSlowAdvanceAnim), 0, -6.0f);
            playSpeed = thisv->actor.speedXZ * 10.0f;
        } else {
            Animation_Change(&thisv->skelAnime, &gStalfosFastAdvanceAnim, 0.0f, thisv->skelAnime.curFrame,
                             Animation_GetLastFrame(&gStalfosFastAdvanceAnim), 0, -4.0f);
            playSpeed = thisv->actor.speedXZ * 10.0f * 0.02f;
        }

        if (thisv->actor.speedXZ >= 0.0f) {
            if (thisv->unk_7DE == 0) {
                thisv->unk_7DE++;
            }

            playSpeed = CLAMP_MAX(playSpeed, 2.5f);
            thisv->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = CLAMP_MIN(playSpeed, -2.5f);
            thisv->skelAnime.playSpeed = playSpeed;
        }

        yawDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

        if ((thisv->actor.xzDistToPlayer < 100.0f) && (player->swordState != 0)) {
            if (ABS(yawDiff) >= 0x1F40) {
                thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

                if ((Rand_ZeroOne() > 0.7f) && (player->swordAnimation != 0x11)) {
                    EnTest_SetupJumpBack(thisv);
                } else {
                    EnTest_SetupStopAndBlock(thisv);
                }

                return;
            }
        }

        prevFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);

        temp_f16 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if ((s32)thisv->skelAnime.curFrame != prevFrame) {
            s32 temp_v0_2 = absPlaySpeed + prevFrame;

            if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((thisv->timer % 32) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WARAU);
            thisv->timer += (s16)(Rand_ZeroOne() * 5.0f);
        }

        if ((thisv->actor.xzDistToPlayer < 220.0f) && (thisv->actor.xzDistToPlayer > 160.0f) &&
            (Actor_IsFacingPlayer(&thisv->actor, 0x71C))) {
            if (Actor_IsTargeted(globalCtx, &thisv->actor)) {
                if (Rand_ZeroOne() < 0.1f) {
                    EnTest_SetupJumpslash(thisv);
                    return;
                }
            } else if (player->heldItemActionParam != PLAYER_AP_NONE) {
                if (thisv->actor.isTargeted) {
                    if ((globalCtx->gameplayFrames % 2) != 0) {
                        func_808627C4(thisv, globalCtx);
                        return;
                    }

                    EnTest_ChooseAction(thisv, globalCtx);
                } else {
                    func_80860EC0(thisv);
                }
            }
        }

        if (Rand_ZeroOne() < 0.4f) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        }

        if (!Actor_IsFacingPlayer(&thisv->actor, 0x11C7)) {
            EnTest_SetupIdle(thisv);
            thisv->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
            return;
        }

        if (thisv->actor.xzDistToPlayer < 110.0f) {
            if (Rand_ZeroOne() > 0.2f) {
                if (player->stateFlags1 & 0x10) {
                    if (thisv->actor.isTargeted) {
                        EnTest_SetupSlashDown(thisv);
                    } else {
                        func_808627C4(thisv, globalCtx);
                    }
                } else {
                    EnTest_SetupSlashDown(thisv);
                }
            } else {
                EnTest_SetupStopAndBlock(thisv);
            }
        } else if (Rand_ZeroOne() < 0.1f) {
            thisv->actor.speedXZ = 5.0f;
        }
    }
}

// a variation of sidestep
void func_80860BDC(EnTest* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosSidestepAnim);
    thisv->unk_7C8 = 0xE;
    EnTest_SetupAction(thisv, func_80860C24);
}

// a variation of sidestep
void func_80860C24(EnTest* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    s16 yawChange;
    f32 playSpeed;
    s32 prevFrame;
    s32 temp1;
    s32 temp2;
    s32 absPlaySpeed;

    if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
        yawDiff = thisv->actor.yawTowardsPlayer;
        yawDiff -= thisv->actor.shape.rot.y;

        if (yawDiff > 0) {
            yawChange = (yawDiff / 42.0f) + 300.0f;
            thisv->actor.shape.rot.y += yawChange * 2;
        } else {
            yawChange = (yawDiff / 42.0f) - 300.0f;
            thisv->actor.shape.rot.y += yawChange * 2;
        }

        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

        if (yawDiff > 0) {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MAX(playSpeed, 1.0f);
            thisv->skelAnime.playSpeed = playSpeed;
        } else {
            playSpeed = yawChange * 0.02f;
            playSpeed = CLAMP_MIN(playSpeed, -1.0f);
            thisv->skelAnime.playSpeed = playSpeed;
        }

        prevFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        temp1 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if ((s32)thisv->skelAnime.curFrame != prevFrame) {
            temp2 = absPlaySpeed + prevFrame;

            if (((temp2 > 2) && (temp1 <= 0)) || ((temp1 < 7) && (temp2 >= 9))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if (Actor_IsFacingPlayer(&thisv->actor, 0x71C)) {
            if (Rand_ZeroOne() > 0.8f) {
                if ((Rand_ZeroOne() > 0.7f)) {
                    func_80860EC0(thisv);
                } else {
                    EnTest_ChooseAction(thisv, globalCtx);
                }
            } else {
                EnTest_SetupWalkAndBlock(thisv);
            }
        }
    }
}

// a variation of sidestep
void func_80860EC0(EnTest* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gStalfosSidestepAnim);
    thisv->unk_7C8 = 0xF;
    thisv->actor.speedXZ = (Rand_ZeroOne() > 0.5f) ? -0.5f : 0.5f;
    thisv->timer = (s16)((Rand_ZeroOne() * 15.0f) + 25.0f);
    thisv->unk_7EC = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnTest_SetupAction(thisv, func_80860F84);
}

// a variation of sidestep
void func_80860F84(EnTest* thisv, GlobalContext* globalCtx) {
    s16 playerYaw180;
    s32 pad;
    s32 prevFrame;
    s32 temp_f16;
    s16 yawDiff;
    Player* player = GET_PLAYER(globalCtx);
    f32 checkDist = 0.0f;
    s16 newYaw;
    s32 absPlaySpeed;

    if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3E80;
        playerYaw180 = player->actor.shape.rot.y + 0x8000;

        if (thisv->actor.speedXZ >= 0.0f) {
            if (thisv->actor.speedXZ < 6.0f) {
                thisv->actor.speedXZ += 0.5f;
            } else {
                thisv->actor.speedXZ = 6.0f;
            }
        } else {
            if (thisv->actor.speedXZ > -6.0f) {
                thisv->actor.speedXZ -= 0.5f;
            } else {
                thisv->actor.speedXZ = -6.0f;
            }
        }

        if ((thisv->actor.bgCheckFlags & 8) ||
            ((thisv->actor.params == STALFOS_TYPE_CEILING) &&
             !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.world.rot.y))) {
            if (thisv->actor.bgCheckFlags & 8) {
                if (thisv->actor.speedXZ >= 0.0f) {
                    newYaw = thisv->actor.shape.rot.y + 0x3FFF;
                } else {
                    newYaw = thisv->actor.shape.rot.y - 0x3FFF;
                }

                newYaw = thisv->actor.wallYaw - newYaw;
            } else {
                thisv->actor.speedXZ *= -0.8f;
                newYaw = 0;
            }

            if (ABS(newYaw) > 0x4000) {
                thisv->actor.speedXZ *= -0.8f;

                if (thisv->actor.speedXZ < 0.0f) {
                    thisv->actor.speedXZ -= 0.5f;
                } else {
                    thisv->actor.speedXZ += 0.5f;
                }
            }
        }

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            checkDist = 200.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (80.0f + checkDist)) {
            Math_SmoothStepToF(&thisv->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
        } else if (thisv->actor.xzDistToPlayer > (110.0f + checkDist)) {
            Math_SmoothStepToF(&thisv->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
        }

        if (thisv->unk_7EC != 0.0f) {
            thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->unk_7EC;
            thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->unk_7EC;
        }

        thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.5f;

        prevFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        temp_f16 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        absPlaySpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if ((s32)thisv->skelAnime.curFrame != prevFrame) {
            s32 temp_v0_2 = absPlaySpeed + prevFrame;

            if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WALK);
            }
        }

        if ((globalCtx->gameplayFrames & 95) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WARAU);
        }

        yawDiff = playerYaw180 - thisv->actor.shape.rot.y;
        yawDiff = ABS(yawDiff);

        if ((yawDiff > 0x6800) || (thisv->timer == 0)) {
            EnTest_ChooseAction(thisv, globalCtx);
        } else if (thisv->timer != 0) {
            thisv->timer--;
        }
    }
}

void EnTest_SetupSlashDown(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosDownSlashAnim);
    Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
    thisv->unk_7C8 = 0x10;
    thisv->actor.speedXZ = 0.0f;
    EnTest_SetupAction(thisv, EnTest_SlashDown);
    thisv->swordCollider.info.toucher.damage = 16;

    if (thisv->unk_7DE != 0) {
        thisv->unk_7DE = 3;
    }
}

void EnTest_SlashDown(EnTest* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;

    if ((s32)thisv->skelAnime.curFrame < 4) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }

    if ((s32)thisv->skelAnime.curFrame == 7) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((thisv->skelAnime.curFrame > 7.0f) && (thisv->skelAnime.curFrame < 11.0f)) {
        thisv->swordState = 1;
    } else {
        thisv->swordState = 0;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if ((globalCtx->gameplayFrames % 2) != 0) {
            EnTest_SetupSlashDownEnd(thisv);
        } else {
            EnTest_SetupSlashUp(thisv);
        }
    }
}

void EnTest_SetupSlashDownEnd(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosRecoverFromDownSlashAnim);
    thisv->unk_7C8 = 0x12;
    thisv->actor.speedXZ = 0.0f;
    EnTest_SetupAction(thisv, EnTest_SlashDownEnd);
}

void EnTest_SlashDownEnd(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->swordCollider.base.atFlags & AT_HIT) {
            thisv->swordCollider.base.atFlags &= ~AT_HIT;
            if (thisv->actor.params != STALFOS_TYPE_CEILING) {
                EnTest_SetupJumpBack(thisv);
                return;
            }
        }

        if (Rand_ZeroOne() > 0.7f) {
            EnTest_SetupIdle(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            return;
        }

        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, &player->actor);

        if (Rand_ZeroOne() > 0.7f) {
            if (thisv->actor.params != STALFOS_TYPE_CEILING) {
                EnTest_SetupJumpBack(thisv);
                return;
            }
        }

        yawDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x2710) {
            yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

            if ((ABS(yawDiff) > 0x3E80) && (thisv->actor.params != STALFOS_TYPE_CEILING)) {
                thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                EnTest_SetupJumpBack(thisv);
            } else if (player->stateFlags1 & 0x10) {
                if (thisv->actor.isTargeted) {
                    EnTest_SetupSlashDown(thisv);
                } else if ((globalCtx->gameplayFrames % 2) != 0) {
                    func_808627C4(thisv, globalCtx);
                } else {
                    EnTest_SetupJumpBack(thisv);
                }
            } else {
                EnTest_SetupSlashDown(thisv);
            }
        } else {
            func_808627C4(thisv, globalCtx);
        }
    }
}

void EnTest_SetupSlashUp(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosUpSlashAnim);
    thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
    thisv->unk_7C8 = 0x11;
    thisv->swordCollider.info.toucher.damage = 16;
    thisv->actor.speedXZ = 0.0f;
    EnTest_SetupAction(thisv, EnTest_SlashUp);

    if (thisv->unk_7DE != 0) {
        thisv->unk_7DE = 3;
    }
}

void EnTest_SlashUp(EnTest* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;

    if ((s32)thisv->skelAnime.curFrame == 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_SAKEBI);
    }

    if ((thisv->skelAnime.curFrame > 1.0f) && (thisv->skelAnime.curFrame < 8.0f)) {
        thisv->swordState = 1;
    } else {
        thisv->swordState = 0;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnTest_SetupSlashDown(thisv);
    }
}

void EnTest_SetupJumpBack(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosJumpBackwardsAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->unk_7C8 = 0x14;
    thisv->timer = 5;
    EnTest_SetupAction(thisv, EnTest_JumpBack);

    if (thisv->unk_7DE != 0) {
        thisv->unk_7DE = 3;
    }

    if (thisv->actor.params != STALFOS_TYPE_CEILING) {
        thisv->actor.speedXZ = -11.0f;
    } else {
        thisv->actor.speedXZ = -7.0f;
    }
}

void EnTest_JumpBack(EnTest* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xBB8, 1);

    if (thisv->timer == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WARAU);
    } else {
        thisv->timer--;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
            if (thisv->actor.xzDistToPlayer <= 100.0f) {
                if (Actor_IsFacingPlayer(&thisv->actor, 0x1555)) {
                    EnTest_SetupSlashDown(thisv);
                } else {
                    EnTest_SetupIdle(thisv);
                    thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
                }
            } else {
                if ((thisv->actor.xzDistToPlayer <= 220.0f) && Actor_IsFacingPlayer(&thisv->actor, 0xE38)) {
                    EnTest_SetupJumpslash(thisv);
                } else {
                    EnTest_SetupIdle(thisv);
                    thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
                }
            }
            thisv->actor.flags |= ACTOR_FLAG_0;
        }
    } else if (thisv->skelAnime.curFrame == (thisv->skelAnime.endFrame - 4.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    }
}

void EnTest_SetupJumpslash(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosJumpAnim);
    Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_STAL_WARAU);
    thisv->timer = 0;
    thisv->unk_7C8 = 0x17;
    thisv->actor.velocity.y = 10.0f;
    thisv->actor.speedXZ = 8.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
    EnTest_SetupAction(thisv, EnTest_Jumpslash);
    thisv->swordCollider.info.toucher.damage = 32;

    if (thisv->unk_7DE != 0) {
        thisv->unk_7DE = 3;
    }
}

void EnTest_Jumpslash(EnTest* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer == 0) {
            Animation_PlayOnce(&thisv->skelAnime, &gStalfosJumpslashAnim);
            thisv->timer = 1;
            thisv->swordState = 1;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_SAKEBI);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
        } else {
            thisv->actor.speedXZ = 0.0f;
            EnTest_SetupIdle(thisv);
        }
    }

    if ((thisv->timer != 0) && (thisv->skelAnime.curFrame >= 5.0f)) {
        thisv->swordState = 0;
    }

    if (thisv->actor.world.pos.y <= thisv->actor.floorHeight) {
        if (thisv->actor.speedXZ != 0.0f) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }

        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }
}

void EnTest_SetupJumpUp(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosJumpAnim);
    thisv->timer = 0;
    thisv->unk_7C8 = 4;
    thisv->actor.velocity.y = 14.0f;
    thisv->actor.speedXZ = 6.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnTest_SetupAction(thisv, EnTest_JumpUp);
}

void EnTest_JumpUp(EnTest* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.world.pos.y <= thisv->actor.floorHeight) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        thisv->unk_7E4 = -(s32)thisv->actor.velocity.y;

        if (thisv->unk_7E4 == 0) {
            thisv->unk_7E4 = 1;
        }

        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
        thisv->unk_7C8 = 0xC;
        thisv->timer = 4;
        Animation_Change(&thisv->skelAnime, &gStalfosLandFromLeapAnim, 0.0f, 0.0f, 0.0f, 2, 0.0f);
        EnTest_SetupAction(thisv, EnTest_Land);
    }
}

void EnTest_SetupStopAndBlock(EnTest* thisv) {
    Animation_Change(&thisv->skelAnime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
    thisv->unk_7C8 = 0x15;
    thisv->actor.speedXZ = 0.0f;
    thisv->timer = (Rand_ZeroOne() * 10.0f) + 11.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->unk_7DE = 5;
    EnTest_SetupAction(thisv, EnTest_StopAndBlock);
}

void EnTest_StopAndBlock(EnTest* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if ((ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) > 0x3E80) &&
        (thisv->actor.params != STALFOS_TYPE_CEILING) && ((globalCtx->gameplayFrames % 2) != 0)) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        EnTest_SetupJumpBack(thisv);
    }

    if (thisv->timer == 0) {
        EnTest_SetupIdleFromBlock(thisv);
    } else {
        thisv->timer--;
    }
}

void EnTest_SetupIdleFromBlock(EnTest* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gStalfosMiddleGuardAnim, -4.0f);
    thisv->unk_7C8 = 0x16;
    EnTest_SetupAction(thisv, EnTest_IdleFromBlock);
}

void EnTest_IdleFromBlock(EnTest* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.5f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->skelAnime.morphWeight == 0.0f) {
        thisv->actor.speedXZ = 0.0f;
        thisv->unk_7DE = 0;

        if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
            if (thisv->actor.xzDistToPlayer < 500.0f) {
                EnTest_ChooseAction(thisv, globalCtx);
            } else {
                func_808627C4(thisv, globalCtx);
            }
        }
    }
}

void func_80862154(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosFlinchFromHitFrontAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DAMAGE);
    thisv->unk_7C8 = 8;
    thisv->actor.speedXZ = -2.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
    EnTest_SetupAction(thisv, func_808621D4);
}

void func_808621D4(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.1f, 0.0f);

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.speedXZ = 0.0f;

        if ((thisv->actor.bgCheckFlags & 8) && ((ABS((s16)(thisv->actor.wallYaw - thisv->actor.shape.rot.y)) < 0x38A4) &&
                                               (thisv->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(thisv);
        } else if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
            EnTest_ChooseAction(thisv, globalCtx);
        } else {
            return;
        }
    }

    if (player->swordState != 0) {
        if ((thisv->actor.bgCheckFlags & 8) && ((ABS((s16)(thisv->actor.wallYaw - thisv->actor.shape.rot.y)) < 0x38A4) &&
                                               (thisv->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(thisv);
        } else if ((Rand_ZeroOne() > 0.7f) && (thisv->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->swordAnimation != 0x11)) {
            EnTest_SetupJumpBack(thisv);
        } else {
            EnTest_SetupStopAndBlock(thisv);
        }

        thisv->unk_7C8 = 8;
    }
}

void func_80862398(EnTest* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosFlinchFromHitBehindAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DAMAGE);
    thisv->unk_7C8 = 9;
    thisv->actor.speedXZ = -2.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
    EnTest_SetupAction(thisv, func_80862418);
}

void func_80862418(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.1f, 0.0f);

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.speedXZ = 0.0f;

        if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
            EnTest_ChooseAction(thisv, globalCtx);
        } else {
            return;
        }
    }

    if (player->swordState != 0) {
        if ((thisv->actor.bgCheckFlags & 8) && ((ABS((s16)(thisv->actor.wallYaw - thisv->actor.shape.rot.y)) < 0x38A4) &&
                                               (thisv->actor.xzDistToPlayer < 80.0f))) {
            EnTest_SetupJumpUp(thisv);
        } else if ((Rand_ZeroOne() > 0.7f) && (thisv->actor.params != STALFOS_TYPE_CEILING) &&
                   (player->swordAnimation != 0x11)) {
            EnTest_SetupJumpBack(thisv);
        } else {
            EnTest_SetupStopAndBlock(thisv);
        }

        thisv->unk_7C8 = 8;
    }
}

void EnTest_SetupStunned(EnTest* thisv) {
    thisv->unk_7C8 = 0xB;
    thisv->unk_7DE = 0;
    thisv->swordState = 0;
    thisv->skelAnime.playSpeed = 0.0f;
    thisv->actor.speedXZ = -4.0f;

    if (thisv->lastDamageEffect == STALFOS_DMGEFF_LIGHT) {
        Actor_SetColorFilter(&thisv->actor, -0x8000, 0x78, 0, 0x50);
    } else {
        Actor_SetColorFilter(&thisv->actor, 0, 0x78, 0, 0x50);

        if (thisv->lastDamageEffect == STALFOS_DMGEFF_FREEZE) {
            thisv->iceTimer = 36;
        } else {
            Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gStalfosFlinchFromHitFrontAnim, 0.0f);
        }
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnTest_SetupAction(thisv, EnTest_Stunned);
}

void EnTest_Stunned(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

    if (thisv->actor.colorFilterTimer == 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            func_80862FA8(thisv, globalCtx);
        } else if (player->swordState != 0) {
            if ((thisv->actor.bgCheckFlags & 8) &&
                ((ABS((s16)(thisv->actor.wallYaw - thisv->actor.shape.rot.y)) < 0x38A4) &&
                 (thisv->actor.xzDistToPlayer < 80.0f))) {
                EnTest_SetupJumpUp(thisv);
            } else if ((Rand_ZeroOne() > 0.7f) && (player->swordAnimation != 0x11)) {
                EnTest_SetupJumpBack(thisv);
            } else {
                EnTest_SetupStopAndBlock(thisv);
            }

            thisv->unk_7C8 = 8;
        } else {
            thisv->actor.speedXZ = 0.0f;
            if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
                EnTest_ChooseAction(thisv, globalCtx);
            }
        }
    }
}

// a variation of sidestep
void func_808627C4(EnTest* thisv, GlobalContext* globalCtx) {
    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
        func_80860EC0(thisv);
        return;
    }

    Animation_MorphToLoop(&thisv->skelAnime, &gStalfosSidestepAnim, -2.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);
    thisv->actor.speedXZ = ((globalCtx->gameplayFrames % 2) != 0) ? -4.0f : 4.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
    thisv->timer = (Rand_ZeroOne() * 20.0f) + 20.0f;
    thisv->unk_7C8 = 0x18;
    EnTest_SetupAction(thisv, func_808628C8);
    thisv->unk_7EC = 0.0f;
}

// a variation of sidestep
void func_808628C8(EnTest* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad1;
    s32 prevFrame;
    s32 temp_f16;
    s32 pad2;
    f32 checkDist = 0.0f;
    s16 newYaw;
    f32 absPlaySpeed;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA0, 1);

    if (thisv->unk_7DE == 0) {
        thisv->unk_7DE++;
    }

    if (thisv->actor.speedXZ >= 0.0f) {
        if (thisv->actor.speedXZ < 6.0f) {
            thisv->actor.speedXZ += 0.125f;
        } else {
            thisv->actor.speedXZ = 6.0f;
        }
    } else {
        if (thisv->actor.speedXZ > -6.0f) {
            thisv->actor.speedXZ -= 0.125f;
        } else {
            thisv->actor.speedXZ = -6.0f;
        }
    }

    if ((thisv->actor.bgCheckFlags & 8) ||
        ((thisv->actor.params == STALFOS_TYPE_CEILING) &&
         !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.shape.rot.y + 0x3FFF))) {
        if (thisv->actor.bgCheckFlags & 8) {
            if (thisv->actor.speedXZ >= 0.0f) {
                newYaw = (thisv->actor.shape.rot.y + 0x3FFF);
            } else {
                newYaw = (thisv->actor.shape.rot.y - 0x3FFF);
            }

            newYaw = thisv->actor.wallYaw - newYaw;
        } else {
            thisv->actor.speedXZ *= -0.8f;
            newYaw = 0;
        }

        if (ABS(newYaw) > 0x4000) {
            thisv->actor.speedXZ *= -0.8f;

            if (thisv->actor.speedXZ < 0.0f) {
                thisv->actor.speedXZ -= 0.5f;
            } else {
                thisv->actor.speedXZ += 0.5f;
            }
        }
    }

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;

    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
        checkDist = 200.0f;
    }

    if (thisv->actor.xzDistToPlayer <= (80.0f + checkDist)) {
        Math_SmoothStepToF(&thisv->unk_7EC, -2.5f, 1.0f, 0.8f, 0.0f);
    } else if (thisv->actor.xzDistToPlayer > (110.0f + checkDist)) {
        Math_SmoothStepToF(&thisv->unk_7EC, 2.5f, 1.0f, 0.8f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->unk_7EC, 0.0f, 1.0f, 6.65f, 0.0f);
    }

    if (thisv->unk_7EC != 0.0f) {
        thisv->actor.world.pos.x += (Math_SinS(thisv->actor.shape.rot.y) * thisv->unk_7EC);
        thisv->actor.world.pos.z += (Math_CosS(thisv->actor.shape.rot.y) * thisv->unk_7EC);
    }

    thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.5f;
    prevFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);

    temp_f16 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
    absPlaySpeed = ABS(thisv->skelAnime.playSpeed);

    if ((thisv->timer % 32) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WARAU);
    }
    if ((s32)thisv->skelAnime.curFrame != prevFrame) {
        s32 temp_v0_2 = (s32)absPlaySpeed + prevFrame;

        if (((temp_v0_2 > 1) && (temp_f16 <= 0)) || ((temp_f16 < 7) && (temp_v0_2 >= 8))) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_WALK);
        }
    }

    if (thisv->timer == 0) {
        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            EnTest_SetupIdle(thisv);
        } else if (Actor_IsTargeted(globalCtx, &thisv->actor)) {
            if (!EnTest_ReactToProjectile(globalCtx, thisv)) {
                EnTest_ChooseAction(thisv, globalCtx);
            }
        } else if (player->heldItemActionParam != PLAYER_AP_NONE) {
            if ((globalCtx->gameplayFrames % 2) != 0) {
                EnTest_SetupIdle(thisv);
            } else {
                EnTest_SetupWalkAndBlock(thisv);
            }
        } else {
            EnTest_SetupWalkAndBlock(thisv);
        }

    } else {
        thisv->timer--;
    }
}

void func_80862DBC(EnTest* thisv, GlobalContext* globalCtx) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DAMAGE);
    thisv->unk_7C8 = 2;
    BodyBreak_Alloc(&thisv->bodyBreak, 60, globalCtx);
    thisv->actor.home.rot.x = 0;

    if (thisv->swordState >= 0) {
        EffectBlure_AddSpace(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->effectIndex)));
        thisv->swordState = -1;
    }

    thisv->actor.flags &= ~ACTOR_FLAG_0;

    if (thisv->actor.params == STALFOS_TYPE_5) {
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
    }

    EnTest_SetupAction(thisv, func_80862E6C);
}

void func_80862E6C(EnTest* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.child == NULL) {
        if (thisv->actor.home.rot.x == 0) {
            thisv->actor.home.rot.x = thisv->bodyBreak.count;
        }

        if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, thisv->actor.params + 8)) {
            thisv->actor.child = &thisv->actor;
        }
    } else {
        if (thisv->actor.home.rot.x == 0) {
            thisv->actor.colChkInfo.health = 10;

            if (thisv->actor.params == STALFOS_TYPE_4) {
                thisv->actor.params = -1;
            } else {
                Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
            }

            thisv->actor.child = NULL;
            thisv->actor.flags |= ACTOR_FLAG_0;
            EnTest_SetupJumpBack(thisv);
        } else if ((thisv->actor.params == STALFOS_TYPE_5) &&
                   !Actor_FindNearby(globalCtx, &thisv->actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 8000.0f)) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xD0);

            if (thisv->actor.parent != NULL) {
                thisv->actor.parent->home.rot.z--;
            }

            Actor_Kill(&thisv->actor);
        }
    }
}

void func_80862FA8(EnTest* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosFallOverBackwardsAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DEAD);
    thisv->unk_7DE = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.colorFilterTimer = 0;
    thisv->actor.speedXZ = 0.0f;

    if (thisv->actor.params <= STALFOS_TYPE_CEILING) {
        thisv->unk_7C8 = 5;
        EnTest_SetupAction(thisv, func_80863044);
    } else {
        func_80862DBC(thisv, globalCtx);
    }
}

void func_80863044(EnTest* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
        thisv->unk_7C8 = 7;
        EnTest_SetupAction(thisv, func_808633E8);
        BodyBreak_Alloc(&thisv->bodyBreak, 60, globalCtx);
    }

    if ((s32)thisv->skelAnime.curFrame == 15) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void func_808630F0(EnTest* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gStalfosFallOverForwardsAnim);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DEAD);
    thisv->unk_7C8 = 6;
    thisv->actor.colorFilterTimer = 0;
    thisv->unk_7DE = 0;
    thisv->actor.speedXZ = 0.0f;

    if (thisv->actor.params <= STALFOS_TYPE_CEILING) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        EnTest_SetupAction(thisv, func_8086318C);
    } else {
        func_80862DBC(thisv, globalCtx);
    }
}

void func_8086318C(EnTest* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->timer = (Rand_ZeroOne() * 10.0f) + 10.0f;
        thisv->unk_7C8 = 7;
        EnTest_SetupAction(thisv, func_808633E8);
        BodyBreak_Alloc(&thisv->bodyBreak, 60, globalCtx);
    }

    if (((s32)thisv->skelAnime.curFrame == 10) || ((s32)thisv->skelAnime.curFrame == 25)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void EnTest_SetupRecoil(EnTest* thisv) {
    thisv->swordState = 0;
    thisv->skelAnime.moveFlags = 2;
    thisv->unk_7C8 = 0x13;
    thisv->skelAnime.playSpeed = -1.0f;
    thisv->skelAnime.startFrame = thisv->skelAnime.curFrame;
    thisv->skelAnime.endFrame = 0.0f;
    EnTest_SetupAction(thisv, EnTest_Recoil);
}

void EnTest_Recoil(EnTest* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (Rand_ZeroOne() > 0.7f) {
            EnTest_SetupIdle(thisv);
            thisv->timer = (Rand_ZeroOne() * 5.0f) + 5.0f;
        } else if (((globalCtx->gameplayFrames % 2) != 0) && (thisv->actor.params != STALFOS_TYPE_CEILING)) {
            EnTest_SetupJumpBack(thisv);
        } else {
            func_808627C4(thisv, globalCtx);
        }
    }
}

void EnTest_Rise(EnTest* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.scale.y < 0.015f) {
        thisv->actor.scale.y += 0.002f;
        thisv->actor.world.pos.y = thisv->actor.home.pos.y - 3.5f;
    } else {
        thisv->actor.world.pos.y = thisv->actor.home.pos.y;
        EnTest_SetupJumpBack(thisv);
    }
}

void func_808633E8(EnTest* thisv, GlobalContext* globalCtx) {
    thisv->actor.params = STALFOS_TYPE_1;

    if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, thisv->actor.params)) {
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xD0);

        if (thisv->actor.parent != NULL) {
            thisv->actor.parent->home.rot.z--;
        }

        Actor_Kill(&thisv->actor);
    }
}

void EnTest_UpdateHeadRot(EnTest* thisv, GlobalContext* globalCtx) {
    s16 lookAngle = thisv->actor.yawTowardsPlayer;

    lookAngle -= (s16)(thisv->headRot.y + thisv->actor.shape.rot.y);

    thisv->headRotOffset.y = CLAMP(lookAngle, -0x7D0, 0x7D0);
    thisv->headRot.y += thisv->headRotOffset.y;
    thisv->headRot.y = CLAMP(thisv->headRot.y, -0x382F, 0x382F);
}

void EnTest_UpdateDamage(EnTest* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->shieldCollider.base.acFlags & AC_BOUNCED) {
        thisv->shieldCollider.base.acFlags &= ~AC_BOUNCED;
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;

        if (thisv->unk_7C8 >= 0xA) {
            thisv->actor.speedXZ = -4.0f;
        }
    } else if (thisv->bodyCollider.base.acFlags & AC_HIT) {
        thisv->bodyCollider.base.acFlags &= ~AC_HIT;

        if ((thisv->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_SLING) &&
            (thisv->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC)) {
            thisv->lastDamageEffect = thisv->actor.colChkInfo.damageEffect;
            if (thisv->swordState >= 1) {
                thisv->swordState = 0;
            }
            thisv->unk_7DC = player->unk_845;
            thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            Actor_SetDropFlag(&thisv->actor, &thisv->bodyCollider.info, false);
            Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_STAL_WARAU);

            if ((thisv->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_STUN) ||
                (thisv->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_FREEZE) ||
                (thisv->actor.colChkInfo.damageEffect == STALFOS_DMGEFF_LIGHT)) {
                if (thisv->unk_7C8 != 0xB) {
                    Actor_ApplyDamage(&thisv->actor);
                    EnTest_SetupStunned(thisv);
                }
            } else {
                if (Actor_IsFacingPlayer(&thisv->actor, 0x4000)) {
                    if (Actor_ApplyDamage(&thisv->actor) == 0) {
                        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                        func_80862FA8(thisv, globalCtx);
                    } else {
                        func_80862154(thisv);
                    }
                } else if (Actor_ApplyDamage(&thisv->actor) == 0) {
                    func_808630F0(thisv, globalCtx);
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                } else {
                    func_80862398(thisv);
                }
            }
        }
    }
}

void EnTest_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnTest* thisv = (EnTest*)thisx;
    f32 oldWeight;
    u32 floorProperty;
    s32 pad;

    EnTest_UpdateDamage(thisv, globalCtx);

    if (thisv->actor.colChkInfo.damageEffect != STALFOS_DMGEFF_FIREMAGIC) {
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 0x1D);

        if (thisv->actor.params == STALFOS_TYPE_1) {
            if (thisv->actor.world.pos.y <= thisv->actor.home.pos.y) {
                thisv->actor.world.pos.y = thisv->actor.home.pos.y;
                thisv->actor.velocity.y = 0.0f;
            }

            if (thisv->actor.floorHeight <= thisv->actor.home.pos.y) {
                thisv->actor.floorHeight = thisv->actor.home.pos.y;
            }
        } else if (thisv->actor.bgCheckFlags & 2) {
            floorProperty = func_80041EA4(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);

            if ((floorProperty == 5) || (floorProperty == 0xC) ||
                func_80041D4C(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId) == 9) {
                Actor_Kill(&thisv->actor);
                return;
            }
        }

        thisv->actionFunc(thisv, globalCtx);

        switch (thisv->unk_7DE) {
            case 0:
                break;

            case 1:
                Animation_Change(&thisv->upperSkelanime, &gStalfosBlockWithShieldAnim, 2.0f, 0.0f,
                                 Animation_GetLastFrame(&gStalfosBlockWithShieldAnim), 2, 2.0f);
                AnimationContext_SetCopyTrue(globalCtx, thisv->skelAnime.limbCount, thisv->skelAnime.jointTable,
                                             thisv->upperSkelanime.jointTable, sJointCopyFlags);
                thisv->unk_7DE++;
                break;

            case 2:
                SkelAnime_Update(&thisv->upperSkelanime);
                SkelAnime_CopyFrameTableTrue(&thisv->skelAnime, thisv->skelAnime.jointTable,
                                             thisv->upperSkelanime.jointTable, sJointCopyFlags);
                break;

            case 3:
                thisv->unk_7DE++;
                thisv->upperSkelanime.morphWeight = 4.0f;
                // fallthrough
            case 4:
                oldWeight = thisv->upperSkelanime.morphWeight;
                thisv->upperSkelanime.morphWeight -= 1.0f;

                if (thisv->upperSkelanime.morphWeight <= 0.0f) {
                    thisv->unk_7DE = 0;
                }

                SkelAnime_InterpFrameTable(thisv->skelAnime.limbCount, thisv->upperSkelanime.jointTable,
                                           thisv->upperSkelanime.jointTable, thisv->skelAnime.jointTable,
                                           1.0f - (thisv->upperSkelanime.morphWeight / oldWeight));
                SkelAnime_CopyFrameTableTrue(&thisv->skelAnime, thisv->skelAnime.jointTable,
                                             thisv->upperSkelanime.jointTable, sJointCopyFlags);
                break;
        }

        if ((thisv->actor.colorFilterTimer == 0) && (thisv->actor.colChkInfo.health != 0)) {
            if ((thisv->unk_7C8 != 0x10) && (thisv->unk_7C8 != 0x17)) {
                EnTest_UpdateHeadRot(thisv, globalCtx);
            } else {
                Math_SmoothStepToS(&thisv->headRot.y, 0, 1, 0x3E8, 0);
            }
        }
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);

    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 45.0f;

    if ((thisv->actor.colChkInfo.health > 0) || (thisv->actor.colorFilterTimer != 0)) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);

        if ((thisv->unk_7C8 >= 0xA) &&
            ((thisv->actor.colorFilterTimer == 0) || (!(thisv->actor.colorFilterParams & 0x4000)))) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
        }

        if (thisv->unk_7DE != 0) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->shieldCollider.base);
        }
    }

    if (thisv->swordState >= 1) {
        if (!(thisv->swordCollider.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->swordCollider.base);
        } else {
            thisv->swordCollider.base.atFlags &= ~AT_BOUNCED;
            EnTest_SetupRecoil(thisv);
        }
    }

    if (thisv->actor.params == STALFOS_TYPE_INVISIBLE) {
        if (globalCtx->actorCtx.unk_03 != 0) {
            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_7;
            thisv->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        } else {
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_7);
            thisv->actor.shape.shadowDraw = NULL;
        }
    }
}

s32 EnTest_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTest* thisv = (EnTest*)thisx;
    s32 pad;

    if (limbIndex == STALFOS_LIMB_HEAD_ROOT) {
        rot->x += thisv->headRot.y;
        rot->y -= thisv->headRot.x;
        rot->z += thisv->headRot.z;
    } else if (limbIndex == STALFOS_LIMB_HEAD) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_test.c", 3582);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 80 + ABS((s16)(Math_SinS(globalCtx->gameplayFrames * 2000) * 175.0f)), 0, 0,
                       255);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_test.c", 3587);
    }

    if ((thisv->actor.params == STALFOS_TYPE_INVISIBLE) && !CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
        *dList = NULL;
    }

    return false;
}

void EnTest_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f unused1 = { 1100.0f, -700.0f, 0.0f };
    static Vec3f D_80864658 = { 300.0f, 0.0f, 0.0f };
    static Vec3f D_80864664 = { 3400.0f, 0.0f, 0.0f };
    static Vec3f D_80864670 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_8086467C = { 7000.0f, 1000.0f, 0.0f };
    static Vec3f D_80864688 = { 3000.0f, -2000.0f, -1000.0f };
    static Vec3f D_80864694 = { 3000.0f, -2000.0f, 1000.0f };
    static Vec3f D_808646A0 = { -1300.0f, 1100.0f, 0.0f };
    static Vec3f unused2 = { -3000.0f, 1900.0f, 800.0f };
    static Vec3f unused3 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused4 = { 1900.0f, 1900.0f, 800.0f };
    static Vec3f unused5 = { -3000.0f, -1100.0f, 800.0f };
    static Vec3f unused6 = { 1900.0f, -1100.0f, 800.0f };
    static Vec3f unused7 = { 1900.0f, 1900.0f, 800.0f };
    s32 bodyPart = -1;
    Vec3f sp70;
    Vec3f sp64;
    EnTest* thisv = (EnTest*)thisx;
    s32 pad;
    Vec3f sp50;

    BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 0, 60, 60, dList, BODYBREAK_OBJECT_DEFAULT);

    if (limbIndex == STALFOS_LIMB_SWORD) {
        Matrix_MultVec3f(&D_8086467C, &thisv->swordCollider.dim.quad[1]);
        Matrix_MultVec3f(&D_80864688, &thisv->swordCollider.dim.quad[0]);
        Matrix_MultVec3f(&D_80864694, &thisv->swordCollider.dim.quad[3]);
        Matrix_MultVec3f(&D_808646A0, &thisv->swordCollider.dim.quad[2]);

        Collider_SetQuadVertices(&thisv->swordCollider, &thisv->swordCollider.dim.quad[0],
                                 &thisv->swordCollider.dim.quad[1], &thisv->swordCollider.dim.quad[2],
                                 &thisv->swordCollider.dim.quad[3]);

        Matrix_MultVec3f(&D_80864664, &sp70);
        Matrix_MultVec3f(&D_80864670, &sp64);

        if ((thisv->swordState >= 1) &&
            ((thisv->actor.params != STALFOS_TYPE_INVISIBLE) || (globalCtx->actorCtx.unk_03 != 0))) {
            EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->effectIndex)), &sp70, &sp64);
        } else if (thisv->swordState >= 0) {
            EffectBlure_AddSpace(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->effectIndex)));
            thisv->swordState = -1;
        }

    } else if ((limbIndex == STALFOS_LIMB_SHIELD) && (thisv->unk_7DE != 0)) {
        Matrix_MultVec3f(&D_80864670, &sp64);

        thisv->shieldCollider.dim.pos.x = sp64.x;
        thisv->shieldCollider.dim.pos.y = sp64.y;
        thisv->shieldCollider.dim.pos.z = sp64.z;
    } else {
        Actor_SetFeetPos(&thisv->actor, limbIndex, STALFOS_LIMB_FOOT_L, &D_80864658, STALFOS_LIMB_ANKLE_R, &D_80864658);

        if ((limbIndex == STALFOS_LIMB_FOOT_L) || (limbIndex == STALFOS_LIMB_ANKLE_R)) {
            if ((thisv->unk_7C8 == 0x15) || (thisv->unk_7C8 == 0x16)) {
                if (thisv->actor.speedXZ != 0.0f) {
                    Matrix_MultVec3f(&D_80864658, &sp64);
                    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &sp64, 10.0f, 1, 8.0f, 0x64, 0xF, 0);
                }
            }
        }
    }

    if (thisv->iceTimer != 0) {
        switch (limbIndex) {
            case STALFOS_LIMB_HEAD:
                bodyPart = 0;
                break;
            case STALFOS_LIMB_CHEST:
                bodyPart = 1;
                break;
            case STALFOS_LIMB_SWORD:
                bodyPart = 2;
                break;
            case STALFOS_LIMB_SHIELD:
                bodyPart = 3;
                break;
            case STALFOS_LIMB_UPPERARM_R:
                bodyPart = 4;
                break;
            case STALFOS_LIMB_UPPERARM_L:
                bodyPart = 5;
                break;
            case STALFOS_LIMB_WAIST:
                bodyPart = 6;
                break;
            case STALFOS_LIMB_FOOT_L:
                bodyPart = 7;
                break;
            case STALFOS_LIMB_FOOT_R:
                bodyPart = 8;
                break;
        }

        if (bodyPart >= 0) {
            Matrix_MultVec3f(&D_80864670, &sp50);

            thisv->bodyPartsPos[bodyPart].x = sp50.x;
            thisv->bodyPartsPos[bodyPart].y = sp50.y;
            thisv->bodyPartsPos[bodyPart].z = sp50.z;
        }
    }
}

void EnTest_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnTest* thisv = (EnTest*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    func_8002EBCC(&thisv->actor, globalCtx, 1);

    if ((thisx->params <= STALFOS_TYPE_CEILING) || (thisx->child == NULL)) {
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnTest_OverrideLimbDraw,
                          EnTest_PostLimbDraw, thisv);
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

// a variation of sidestep
void func_80864158(EnTest* thisv, f32 xzSpeed) {
    Animation_MorphToLoop(&thisv->skelAnime, &gStalfosSidestepAnim, -2.0f);
    thisv->actor.speedXZ = xzSpeed;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
    thisv->timer = (Rand_ZeroOne() * 20.0f) + 15.0f;
    thisv->unk_7C8 = 0x18;
    EnTest_SetupAction(thisv, func_808628C8);
}

/**
 * Check if a projectile actor is within 300 units and react accordingly.
 * Returns true if the projectile test passes and a new action is performed.
 */
s32 EnTest_ReactToProjectile(GlobalContext* globalCtx, EnTest* thisv) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 wallYawDiff;
    s16 touchingWall;
    s16 directionFlag;

    projectileActor = Actor_GetProjectileActor(globalCtx, &thisv->actor, 300.0f);

    if (projectileActor != NULL) {
        yawToProjectile = Actor_WorldYawTowardActor(&thisv->actor, projectileActor) - (u16)thisv->actor.shape.rot.y;

        if ((u8)(thisv->actor.bgCheckFlags & 8)) {
            wallYawDiff = ((u16)thisv->actor.wallYaw - (u16)thisv->actor.shape.rot.y);
            touchingWall = true;
        } else {
            touchingWall = false;
        }

        if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &projectileActor->world.pos) < 200.0f) {
            if (Actor_IsTargeted(globalCtx, &thisv->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
                EnTest_SetupJumpUp(thisv);
            } else if (ABS(yawToProjectile) < 0x2000) {
                EnTest_SetupStopAndBlock(thisv);
            } else if (ABS(yawToProjectile) < 0x6000) {
                EnTest_SetupJumpBack(thisv);
            } else {
                EnTest_SetupJumpUp(thisv);
            }

            return true;
        }

        if (Actor_IsTargeted(globalCtx, &thisv->actor) && (projectileActor->id == ACTOR_ARMS_HOOK)) {
            EnTest_SetupJumpUp(thisv);
            return true;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) > 0x6000)) {
            directionFlag = globalCtx->gameplayFrames % 2;

            if (touchingWall && (wallYawDiff > 0x2000) && (wallYawDiff < 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (wallYawDiff < -0x2000) && (wallYawDiff > -0x6000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                func_80864158(thisv, 4.0f);
            } else {
                func_80864158(thisv, -4.0f);
            }
        } else if (ABS(yawToProjectile) < 0x6000) {
            directionFlag = globalCtx->gameplayFrames % 2;

            if (touchingWall && (ABS(wallYawDiff) > 0x6000)) {
                directionFlag = false;
            } else if (touchingWall && (ABS(wallYawDiff) < 0x2000)) {
                directionFlag = true;
            }

            if (directionFlag) {
                EnTest_SetupJumpBack(thisv);
            } else {
                EnTest_SetupJumpUp(thisv);
            }
        }

        return true;
    }

    return false;
}
