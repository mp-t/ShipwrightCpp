/*
 * File: z_en_wf.c
 * Overlay: ovl_En_Wf
 * Description: Wolfos (Normal and White)
 */

#include "z_en_wf.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "objects/object_wf/object_wf.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnWf_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWf_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWf_Update(Actor* thisx, GlobalContext* globalCtx);
void EnWf_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnWf_SetupWaitToAppear(EnWf* thisv);
void EnWf_WaitToAppear(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupWait(EnWf* thisv);
void EnWf_Wait(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupRunAtPlayer(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_RunAtPlayer(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupSearchForPlayer(EnWf* thisv);
void EnWf_SearchForPlayer(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupRunAroundPlayer(EnWf* thisv);
void EnWf_RunAroundPlayer(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupSlash(EnWf* thisv);
void EnWf_Slash(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_RecoilFromBlockedSlash(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupBackflipAway(EnWf* thisv);
void EnWf_BackflipAway(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_Stunned(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_Damaged(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupSomersaultAndAttack(EnWf* thisv);
void EnWf_SomersaultAndAttack(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupBlocking(EnWf* thisv);
void EnWf_Blocking(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupSidestep(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_Sidestep(EnWf* thisv, GlobalContext* globalCtx);
void EnWf_SetupDie(EnWf* thisv);
void EnWf_Die(EnWf* thisv, GlobalContext* globalCtx);
s32 EnWf_DodgeRanged(GlobalContext* globalCtx, EnWf* thisv);

static ColliderJntSphElementInit sJntSphItemsInit[4] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { WOLFOS_LIMB_FRONT_RIGHT_CLAW, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { WOLFOS_LIMB_FRONT_LEFT_CLAW, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK1,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { WOLFOS_LIMB_HEAD, { { 800, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK1,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFC1FFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { WOLFOS_LIMB_THORAX, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphItemsInit),
    sJntSphItemsInit,
};

static ColliderCylinderInit sBodyCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sTailCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 20, -15, { 0, 0, 0 } },
};

typedef enum {
    /*  0 */ ENWF_DMGEFF_NONE,
    /*  1 */ ENWF_DMGEFF_STUN,
    /*  6 */ ENWF_DMGEFF_ICE_MAGIC = 6,
    /* 13 */ ENWF_DMGEFF_LIGHT_MAGIC = 13,
    /* 14 */ ENWF_DMGEFF_FIRE,
    /* 15 */ ENWF_DMGEFF_UNDEF // used like STUN in the code, but not in the table
} EnWfDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, ENWF_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, ENWF_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, ENWF_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, ENWF_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENWF_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Giant's Knife */ DMG_ENTRY(4, ENWF_DMGEFF_NONE),
    /* Fire arrow    */ DMG_ENTRY(4, ENWF_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, ENWF_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, ENWF_DMGEFF_ICE_MAGIC),
    /* Light magic   */ DMG_ENTRY(3, ENWF_DMGEFF_LIGHT_MAGIC),
    /* Shield        */ DMG_ENTRY(0, ENWF_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENWF_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENWF_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, ENWF_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, ENWF_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, ENWF_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, ENWF_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, ENWF_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ENWF_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, ENWF_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ENWF_DMGEFF_NONE),
};

ActorInit En_Wf_InitVars = {
    ACTOR_EN_WF,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_WF,
    sizeof(EnWf),
    (ActorFunc)EnWf_Init,
    (ActorFunc)EnWf_Destroy,
    (ActorFunc)EnWf_Update,
    (ActorFunc)EnWf_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3000, ICHAIN_STOP),
};

void EnWf_SetupAction(EnWf* thisv, EnWfActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnWf_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnWf* thisv = (EnWf*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    thisx->focus.pos = thisx->world.pos;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.health = 8;
    thisx->colChkInfo.cylRadius = 50;
    thisx->colChkInfo.cylHeight = 100;
    thisv->switchFlag = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    thisv->eyeIndex = 0;
    thisv->unk_2F4 = 10.0f; // Set and not used

    Collider_InitJntSph(globalCtx, &thisv->colliderSpheres);
    Collider_SetJntSph(globalCtx, &thisv->colliderSpheres, thisx, &sJntSphInit, thisv->colliderSpheresElements);
    Collider_InitCylinder(globalCtx, &thisv->colliderCylinderBody);
    Collider_SetCylinder(globalCtx, &thisv->colliderCylinderBody, thisx, &sBodyCylinderInit);
    Collider_InitCylinder(globalCtx, &thisv->colliderCylinderTail);
    Collider_SetCylinder(globalCtx, &thisv->colliderCylinderTail, thisx, &sTailCylinderInit);

    if (thisx->params == WOLFOS_NORMAL) {
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gWolfosNormalSkel, &gWolfosWaitingAnim, thisv->jointTable,
                           thisv->morphTable, WOLFOS_LIMB_MAX);
        Actor_SetScale(thisx, 0.0075f);
        thisx->naviEnemyId = 0x4C; // Wolfos
    } else {                       // WOLFOS_WHITE
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gWolfosWhiteSkel, &gWolfosWaitingAnim, thisv->jointTable,
                           thisv->morphTable, WOLFOS_LIMB_MAX);
        Actor_SetScale(thisx, 0.01f);
        thisv->colliderSpheres.elements[0].info.toucher.damage = thisv->colliderSpheres.elements[1].info.toucher.damage =
            8;
        thisx->naviEnemyId = 0x57; // White Wolfos
    }

    EnWf_SetupWaitToAppear(thisv);

    if ((thisv->switchFlag != 0xFF) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        Actor_Kill(thisx);
    }
}

void EnWf_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnWf* thisv = (EnWf*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->colliderSpheres);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinderBody);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinderTail);

    if ((thisv->actor.params != WOLFOS_NORMAL) && (thisv->switchFlag != 0xFF)) {
        func_800F5B58();
    }

    if (thisv->actor.parent != NULL) {
        EnEncount1* parent = (EnEncount1*)thisv->actor.parent;

        if (parent->actor.update != NULL) {
            if (parent->curNumSpawn > 0) {
                parent->curNumSpawn--;
            }

            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Number of concurrent events ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 同時発生数 ☆☆☆☆☆%d\n" VT_RST, parent->curNumSpawn);
            osSyncPrintf("\n\n");
        }
    }
}

s32 EnWf_ChangeAction(GlobalContext* globalCtx, EnWf* thisv, s16 mustChoose) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    s16 wallYawDiff;
    s16 playerYawDiff;
    Actor* explosive;

    wallYawDiff = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
    wallYawDiff = ABS(wallYawDiff);
    playerYawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    playerYawDiff = ABS(playerYawDiff);

    if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x2710, 0x2EE0, thisv->actor.shape.rot.y)) {
        if (player->swordAnimation == 0x11) {
            EnWf_SetupBlocking(thisv);
            return true;
        }

        if ((globalCtx->gameplayFrames % 2) != 0) {
            EnWf_SetupBlocking(thisv);
            return true;
        }
    }

    if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 0x5DC0, 0x2AA8, thisv->actor.shape.rot.y)) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

        if ((thisv->actor.bgCheckFlags & 8) && (ABS(wallYawDiff) < 0x2EE0) && (thisv->actor.xzDistToPlayer < 120.0f)) {
            EnWf_SetupSomersaultAndAttack(thisv);
            return true;
        } else if (player->swordAnimation == 0x11) {
            EnWf_SetupBlocking(thisv);
            return true;
        } else if ((thisv->actor.xzDistToPlayer < 80.0f) && (globalCtx->gameplayFrames % 2) != 0) {
            EnWf_SetupBlocking(thisv);
            return true;
        } else {
            EnWf_SetupBackflipAway(thisv);
            return true;
        }
    }

    explosive = Actor_FindNearby(globalCtx, &thisv->actor, -1, ACTORCAT_EXPLOSIVE, 80.0f);

    if (explosive != NULL) {
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

        if (((thisv->actor.bgCheckFlags & 8) && (wallYawDiff < 0x2EE0)) || (explosive->id == ACTOR_EN_BOM_CHU)) {
            if ((explosive->id == ACTOR_EN_BOM_CHU) && (Actor_WorldDistXYZToActor(&thisv->actor, explosive) < 80.0f) &&
                (s16)((thisv->actor.shape.rot.y - explosive->world.rot.y) + 0x8000) < 0x3E80) {
                EnWf_SetupSomersaultAndAttack(thisv);
                return true;
            } else {
                EnWf_SetupSidestep(thisv, globalCtx);
                return true;
            }
        } else {
            EnWf_SetupBackflipAway(thisv);
            return true;
        }
    }

    if (mustChoose) {
        s16 playerFacingAngleDiff;

        if (playerYawDiff >= 0x1B58) {
            EnWf_SetupSidestep(thisv, globalCtx);
            return true;
        }

        playerFacingAngleDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

        if ((thisv->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
            (((globalCtx->gameplayFrames % 8) != 0) || (ABS(playerFacingAngleDiff) < 0x38E0))) {
            EnWf_SetupSlash(thisv);
            return true;
        }

        EnWf_SetupRunAroundPlayer(thisv);
        return true;
    }
    return false;
}

void EnWf_SetupWaitToAppear(EnWf* thisv) {
    Animation_Change(&thisv->skelAnime, &gWolfosRearingUpFallingOverAnim, 0.5f, 0.0f, 7.0f, ANIMMODE_ONCE_INTERP, 0.0f);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y - 5.0f;
    thisv->actionTimer = 20;
    thisv->unk_300 = false;
    thisv->action = WOLFOS_ACTION_WAIT_TO_APPEAR;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.scale.y = 0.0f;
    thisv->actor.gravity = 0.0f;
    EnWf_SetupAction(thisv, EnWf_WaitToAppear);
}

void EnWf_WaitToAppear(EnWf* thisv, GlobalContext* globalCtx) {
    if (thisv->actionTimer >= 6) {
        thisv->actor.world.pos.y = thisv->actor.home.pos.y - 5.0f;

        if (thisv->actor.xzDistToPlayer < 240.0f) {
            thisv->actionTimer = 5;
            thisv->actor.flags |= ACTOR_FLAG_0;

            if ((thisv->actor.params != WOLFOS_NORMAL) && (thisv->switchFlag != 0xFF)) {
                func_800F5ACC(NA_BGM_MINI_BOSS);
            }
        }
    } else if (thisv->actionTimer != 0) {
        thisv->actor.scale.y += thisv->actor.scale.x * 0.2f;
        thisv->actor.world.pos.y += 0.5f;
        Math_SmoothStepToF(&thisv->actor.shape.shadowScale, 70.0f, 1.0f, 14.0f, 0.0f);
        thisv->actionTimer--;

        if (thisv->actionTimer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_APPEAR);
        }
    } else { // actionTimer == 0
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->actor.scale.y = thisv->actor.scale.x;
            thisv->actor.gravity = -2.0f;
            EnWf_SetupWait(thisv);
        }
    }
}

void EnWf_SetupWait(EnWf* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gWolfosWaitingAnim, -4.0f);
    thisv->action = WOLFOS_ACTION_WAIT;
    thisv->actionTimer = (Rand_ZeroOne() * 10.0f) + 2.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnWf_SetupAction(thisv, EnWf_Wait);
}

void EnWf_Wait(EnWf* thisv, GlobalContext* globalCtx) {
    Player* player;
    s32 pad;
    s16 angle;

    player = GET_PLAYER(globalCtx);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_2E2 != 0) {
        angle = (thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y) - thisv->unk_4D4.y;

        if (ABS(angle) > 0x2000) {
            thisv->unk_2E2--;
            return;
        }

        thisv->unk_2E2 = 0;
    }

    angle = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    angle = ABS(angle);

    if (!EnWf_DodgeRanged(globalCtx, thisv)) {
        // Only use of unk_2E0: never not zero, so thisv if block never runs
        if (thisv->unk_2E0 != 0) {
            thisv->unk_2E0--;

            if (angle >= 0x1FFE) {
                return;
            }
            thisv->unk_2E0 = 0;
        } else {
            if (EnWf_ChangeAction(globalCtx, thisv, false)) {
                return;
            }
        }

        angle = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        angle = ABS(angle);

        if ((thisv->actor.xzDistToPlayer < 80.0f) && (player->swordState != 0) && (angle >= 0x1F40)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
            EnWf_SetupRunAroundPlayer(thisv);
        } else {
            thisv->actionTimer--;

            if (thisv->actionTimer == 0) {
                if (Actor_IsFacingPlayer(&thisv->actor, 0x1555)) {
                    if (Rand_ZeroOne() > 0.3f) {
                        EnWf_SetupRunAtPlayer(thisv, globalCtx);
                    } else {
                        EnWf_SetupRunAroundPlayer(thisv);
                    }
                } else {
                    EnWf_SetupSearchForPlayer(thisv);
                }
                if ((globalCtx->gameplayFrames & 95) == 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
                }
            }
        }
    }
}

void EnWf_SetupRunAtPlayer(EnWf* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gWolfosRunningAnim);

    Animation_Change(&thisv->skelAnime, &gWolfosRunningAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -4.0f);
    thisv->action = WOLFOS_ACTION_RUN_AT_PLAYER;
    EnWf_SetupAction(thisv, EnWf_RunAtPlayer);
}

void EnWf_RunAtPlayer(EnWf* thisv, GlobalContext* globalCtx) {
    s32 animPrevFrame;
    s32 sp58;
    s32 pad;
    f32 baseRange = 0.0f;
    s16 playerFacingAngleDiff;
    Player* player = GET_PLAYER(globalCtx);
    s32 playSpeed;

    if (!EnWf_DodgeRanged(globalCtx, thisv)) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x2EE, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            baseRange = 150.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (50.0f + baseRange)) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, -8.0f, 1.0f, 1.5f, 0.0f);
        } else if ((65.0f + baseRange) < thisv->actor.xzDistToPlayer) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 8.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 6.65f, 0.0f);
        }

        thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.175f;
        playerFacingAngleDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        playerFacingAngleDiff = ABS(playerFacingAngleDiff);

        if ((thisv->actor.xzDistToPlayer < (150.0f + baseRange)) && (player->swordState != 0) &&
            (playerFacingAngleDiff >= 8000)) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                EnWf_SetupRunAroundPlayer(thisv);
                return;
            }
        }

        animPrevFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        sp58 = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        playSpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if (!Actor_IsFacingPlayer(&thisv->actor, 0x11C7)) {
            if (Rand_ZeroOne() > 0.5f) {
                EnWf_SetupRunAroundPlayer(thisv);
            } else {
                EnWf_SetupWait(thisv);
            }
        } else if (thisv->actor.xzDistToPlayer < (90.0f + baseRange)) {
            s16 temp_v1 = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

            if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                ((Rand_ZeroOne() > 0.03f) || ((thisv->actor.xzDistToPlayer <= 80.0f) && (ABS(temp_v1) < 0x38E0)))) {
                EnWf_SetupSlash(thisv);
            } else if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (Rand_ZeroOne() > 0.5f)) {
                EnWf_SetupBackflipAway(thisv);
            } else {
                EnWf_SetupRunAroundPlayer(thisv);
            }
        }

        if (!EnWf_ChangeAction(globalCtx, thisv, false)) {
            if ((globalCtx->gameplayFrames & 95) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
            }
            if ((animPrevFrame != (s32)thisv->skelAnime.curFrame) && (sp58 <= 0) && ((playSpeed + animPrevFrame) > 0)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_WALK);
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 20.0f, 3, 3.0f, 50, 50, 1);
            }
        }
    }
}

void EnWf_SetupSearchForPlayer(EnWf* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gWolfosSidesteppingAnim, -4.0f);
    thisv->action = WOLFOS_ACTION_SEARCH_FOR_PLAYER;
    EnWf_SetupAction(thisv, EnWf_SearchForPlayer);
}

void EnWf_SearchForPlayer(EnWf* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    s16 phi_v1;
    f32 phi_f2;

    if (!EnWf_DodgeRanged(globalCtx, thisv) && !EnWf_ChangeAction(globalCtx, thisv, false)) {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        phi_v1 = (yawDiff > 0) ? (yawDiff * 0.25f) + 2000.0f : (yawDiff * 0.25f) - 2000.0f;
        thisv->actor.shape.rot.y += phi_v1;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

        if (yawDiff > 0) {
            phi_f2 = phi_v1 * 0.5f;
            phi_f2 = CLAMP_MAX(phi_f2, 1.0f);
        } else {
            phi_f2 = phi_v1 * 0.5f;
            phi_f2 = CLAMP_MIN(phi_f2, -1.0f);
        }

        thisv->skelAnime.playSpeed = -phi_f2;
        SkelAnime_Update(&thisv->skelAnime);

        if (Actor_IsFacingPlayer(&thisv->actor, 0x1555)) {
            if (Rand_ZeroOne() > 0.8f) {
                EnWf_SetupRunAroundPlayer(thisv);
            } else {
                EnWf_SetupRunAtPlayer(thisv, globalCtx);
            }
        }

        if ((globalCtx->gameplayFrames & 95) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
        }
    }
}

void EnWf_SetupRunAroundPlayer(EnWf* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gWolfosRunningAnim);

    Animation_Change(&thisv->skelAnime, &gWolfosRunningAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -4.0f);

    if (Rand_ZeroOne() > 0.5f) {
        thisv->runAngle = 16000;
    } else {
        thisv->runAngle = -16000;
    }

    thisv->skelAnime.playSpeed = thisv->actor.speedXZ = 6.0f;
    thisv->skelAnime.playSpeed *= 0.175f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actionTimer = (Rand_ZeroOne() * 30.0f) + 30.0f;
    thisv->action = WOLFOS_ACTION_RUN_AROUND_PLAYER;
    thisv->runSpeed = 0.0f;

    EnWf_SetupAction(thisv, EnWf_RunAroundPlayer);
}

void EnWf_RunAroundPlayer(EnWf* thisv, GlobalContext* globalCtx) {
    s16 angle1;
    s16 angle2;
    s32 pad;
    f32 baseRange = 0.0f;
    s32 animPrevFrame;
    s32 animFrameSpeedDiff;
    s32 animSpeed;
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + thisv->runAngle, 1, 4000, 1);

    if (!EnWf_DodgeRanged(globalCtx, thisv) && !EnWf_ChangeAction(globalCtx, thisv, false)) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        angle1 = player->actor.shape.rot.y + thisv->runAngle + 0x8000;

        // Actor_TestFloorInDirection is useless here (see comment below)
        if ((thisv->actor.bgCheckFlags & 8) ||
            !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.shape.rot.y)) {
            angle2 = (thisv->actor.bgCheckFlags & 8)
                         ? (thisv->actor.wallYaw - thisv->actor.yawTowardsPlayer) - thisv->runAngle
                         : 0;

            // This is probably meant to reverse direction if the edge of a floor is encountered, but does nothing
            // unless bgCheckFlags & 8 anyway, since angle2 = 0 otherwise
            if (ABS(angle2) > 0x2EE0) {
                thisv->runAngle = -thisv->runAngle;
            }
        }

        if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            baseRange = 150.0f;
        }

        if (thisv->actor.xzDistToPlayer <= (60.0f + baseRange)) {
            Math_SmoothStepToF(&thisv->runSpeed, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if ((80.0f + baseRange) < thisv->actor.xzDistToPlayer) {
            Math_SmoothStepToF(&thisv->runSpeed, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->runSpeed, 0.0f, 1.0f, 6.65f, 0.0f);
        }

        if (thisv->runSpeed != 0.0f) {
            thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->runSpeed;
            thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->runSpeed;
        }

        if (ABS(thisv->runSpeed) < ABS(thisv->actor.speedXZ)) {
            thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.175f;
        } else {
            thisv->skelAnime.playSpeed = thisv->runSpeed * 0.175f;
        }

        thisv->skelAnime.playSpeed = CLAMP(thisv->skelAnime.playSpeed, -3.0f, 3.0f);
        animPrevFrame = thisv->skelAnime.curFrame;
        SkelAnime_Update(&thisv->skelAnime);
        animFrameSpeedDiff = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
        animSpeed = (f32)ABS(thisv->skelAnime.playSpeed);

        if ((animPrevFrame != (s32)thisv->skelAnime.curFrame) && (animFrameSpeedDiff <= 0) &&
            (animSpeed + animPrevFrame > 0)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_WALK);
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 20.0f, 3, 3.0f, 50, 50, 1);
        }

        if ((globalCtx->gameplayFrames & 95) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
        }

        if ((Math_CosS(angle1 - thisv->actor.shape.rot.y) < -0.85f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
            (thisv->actor.xzDistToPlayer <= 80.0f)) {
            EnWf_SetupSlash(thisv);
        } else {
            thisv->actionTimer--;

            if (thisv->actionTimer == 0) {
                if (Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (Rand_ZeroOne() > 0.5f)) {
                    EnWf_SetupBackflipAway(thisv);
                } else {
                    EnWf_SetupWait(thisv);
                    thisv->actionTimer = (Rand_ZeroOne() * 3.0f) + 1.0f;
                }
            }
        }
    }
}

void EnWf_SetupSlash(EnWf* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWolfosSlashingAnim);
    thisv->colliderSpheres.base.atFlags &= ~AT_BOUNCED;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->action = WOLFOS_ACTION_SLASH;
    thisv->unk_2FA = 0; // Set and not used
    thisv->actionTimer = 7;
    thisv->skelAnime.endFrame = 20.0f;
    thisv->actor.speedXZ = 0.0f;

    EnWf_SetupAction(thisv, EnWf_Slash);
}

void EnWf_Slash(EnWf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 shapeAngleDiff = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
    s16 yawAngleDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    s32 curFrame = thisv->skelAnime.curFrame;

    shapeAngleDiff = ABS(shapeAngleDiff);
    yawAngleDiff = ABS(yawAngleDiff);
    thisv->actor.speedXZ = 0.0f;

    if (((curFrame >= 9) && (curFrame <= 12)) || ((curFrame >= 17) && (curFrame <= 19))) {
        if (thisv->slashStatus == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_ATTACK);
        }

        thisv->slashStatus = 1;
    } else {
        thisv->slashStatus = 0;
    }

    if (((curFrame == 15) && !Actor_IsTargeted(globalCtx, &thisv->actor) &&
         (!Actor_IsFacingPlayer(&thisv->actor, 0x2000) || (thisv->actor.xzDistToPlayer >= 100.0f))) ||
        SkelAnime_Update(&thisv->skelAnime)) {
        if ((curFrame != 15) && (thisv->actionTimer != 0)) {
            thisv->actor.shape.rot.y += (s16)(3276.0f * (1.5f + (thisv->actionTimer - 4) * 0.4f));
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 15.0f, 1, 2.0f, 50, 50, 1);
            thisv->actionTimer--;
        } else if (!Actor_IsFacingPlayer(&thisv->actor, 0x1554) && (curFrame != 15)) {
            EnWf_SetupWait(thisv);
            thisv->actionTimer = (Rand_ZeroOne() * 5.0f) + 5.0f;

            if (yawAngleDiff > 13000) {
                thisv->unk_2E2 = 7;
            }
        } else if ((Rand_ZeroOne() > 0.7f) || (thisv->actor.xzDistToPlayer >= 120.0f)) {
            EnWf_SetupWait(thisv);
            thisv->actionTimer = (Rand_ZeroOne() * 5.0f) + 5.0f;
        } else {
            thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

            if (Rand_ZeroOne() > 0.7f) {
                EnWf_SetupSidestep(thisv, globalCtx);
            } else if (shapeAngleDiff <= 10000) {
                if (yawAngleDiff > 16000) {
                    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                    EnWf_SetupRunAroundPlayer(thisv);
                } else {
                    EnWf_ChangeAction(globalCtx, thisv, true);
                }
            } else {
                EnWf_SetupRunAroundPlayer(thisv);
            }
        }
    }
}

void EnWf_SetupRecoilFromBlockedSlash(EnWf* thisv) {
    f32 endFrame = 1.0f;

    if ((s32)thisv->skelAnime.curFrame >= 16) {
        endFrame = 15.0f;
    }

    Animation_Change(&thisv->skelAnime, &gWolfosSlashingAnim, -0.5f, thisv->skelAnime.curFrame - 1.0f, endFrame,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    thisv->action = WOLFOS_ACTION_RECOIL_FROM_BLOCKED_SLASH;
    thisv->slashStatus = 0;
    EnWf_SetupAction(thisv, EnWf_RecoilFromBlockedSlash);
}

void EnWf_RecoilFromBlockedSlash(EnWf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 angle1 = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
    s16 angle2 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    angle1 = ABS(angle1);
    angle2 = ABS(angle2);

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!Actor_IsFacingPlayer(&thisv->actor, 0x1554)) {
            EnWf_SetupWait(thisv);
            thisv->actionTimer = (Rand_ZeroOne() * 5.0f) + 5.0f;

            if (angle2 > 0x32C8) {
                thisv->unk_2E2 = 30;
            }
        } else {
            if ((Rand_ZeroOne() > 0.7f) || (thisv->actor.xzDistToPlayer >= 120.0f)) {
                EnWf_SetupWait(thisv);
                thisv->actionTimer = (Rand_ZeroOne() * 5.0f) + 5.0f;
            } else {
                thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

                if (Rand_ZeroOne() > 0.7f) {
                    EnWf_SetupSidestep(thisv, globalCtx);
                } else if (angle1 <= 0x2710) {
                    if (angle2 > 0x3E80) {
                        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
                        EnWf_SetupRunAroundPlayer(thisv);
                    } else {
                        EnWf_ChangeAction(globalCtx, thisv, true);
                    }
                } else {
                    EnWf_SetupRunAroundPlayer(thisv);
                }
            }
        }
    }
}

void EnWf_SetupBackflipAway(EnWf* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWolfosBackflippingAnim, -3.0f);
    thisv->actor.speedXZ = -6.0f;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actionTimer = 0;
    thisv->unk_300 = true;
    thisv->action = WOLFOS_ACTION_BACKFLIP_AWAY;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    EnWf_SetupAction(thisv, EnWf_BackflipAway);
}

void EnWf_BackflipAway(EnWf* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) && (thisv->actor.xzDistToPlayer < 170.0f) &&
            (thisv->actor.xzDistToPlayer > 140.0f) && (Rand_ZeroOne() < 0.2f)) {
            EnWf_SetupRunAtPlayer(thisv, globalCtx);
        } else if ((globalCtx->gameplayFrames % 2) != 0) {
            EnWf_SetupSidestep(thisv, globalCtx);
        } else {
            EnWf_SetupWait(thisv);
        }
    }
    if ((globalCtx->state.frames & 95) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
    }
}

void EnWf_SetupStunned(EnWf* thisv) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = 0.0f;
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gWolfosDamagedAnim, 0.0f);
    thisv->action = WOLFOS_ACTION_STUNNED;
    EnWf_SetupAction(thisv, EnWf_Stunned);
}

void EnWf_Stunned(EnWf* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }

        thisv->unk_300 = false;
    }

    if ((thisv->actor.colorFilterTimer == 0) && (thisv->actor.bgCheckFlags & 1)) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnWf_SetupDie(thisv);
        } else {
            EnWf_ChangeAction(globalCtx, thisv, true);
        }
    }
}

void EnWf_SetupDamaged(EnWf* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWolfosDamagedAnim, -4.0f);

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->unk_300 = false;
        thisv->actor.speedXZ = -4.0f;
    } else {
        thisv->unk_300 = true;
    }

    thisv->unk_2E2 = 0;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_DAMAGE);
    thisv->action = WOLFOS_ACTION_DAMAGED;
    EnWf_SetupAction(thisv, EnWf_Damaged);
}

void EnWf_Damaged(EnWf* thisv, GlobalContext* globalCtx) {
    s16 angleToWall;

    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->actor.speedXZ < 0.0f) {
            thisv->actor.speedXZ += 0.05f;
        }

        thisv->unk_300 = false;
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4500, 0);

    if (!EnWf_ChangeAction(globalCtx, thisv, false) && SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.bgCheckFlags & 1) {
            angleToWall = thisv->actor.wallYaw - thisv->actor.shape.rot.y;
            angleToWall = ABS(angleToWall);

            if ((thisv->actor.bgCheckFlags & 8) && (ABS(angleToWall) < 12000) && (thisv->actor.xzDistToPlayer < 120.0f)) {
                EnWf_SetupSomersaultAndAttack(thisv);
            } else if (!EnWf_DodgeRanged(globalCtx, thisv)) {
                if ((thisv->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                    ((globalCtx->gameplayFrames % 8) != 0)) {
                    EnWf_SetupSlash(thisv);
                } else if (Rand_ZeroOne() > 0.5f) {
                    EnWf_SetupWait(thisv);
                    thisv->actionTimer = (Rand_ZeroOne() * 5.0f) + 5.0f;
                    thisv->unk_2E2 = 30;
                } else {
                    EnWf_SetupBackflipAway(thisv);
                }
            }
        }
    }
}

void EnWf_SetupSomersaultAndAttack(EnWf* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gWolfosBackflippingAnim);

    Animation_Change(&thisv->skelAnime, &gWolfosBackflippingAnim, -1.0f, lastFrame, 0.0f, ANIMMODE_ONCE, -3.0f);
    thisv->actionTimer = 0;
    thisv->unk_300 = false;
    thisv->action = WOLFOS_ACTION_TURN_TOWARDS_PLAYER;
    thisv->actor.speedXZ = 6.5f;
    thisv->actor.velocity.y = 15.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    EnWf_SetupAction(thisv, EnWf_SomersaultAndAttack);
}

void EnWf_SomersaultAndAttack(EnWf* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 4000, 1);

    if (thisv->actor.velocity.y >= 5.0f) {
        //! @bug unk_4C8 and unk_4BC are used but not set (presumably intended to be feet positions like other actors)
        func_800355B8(globalCtx, &thisv->unk_4C8);
        func_800355B8(globalCtx, &thisv->unk_4BC);
    }

    if (SkelAnime_Update(&thisv->skelAnime) && (thisv->actor.bgCheckFlags & (1 | 2))) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.shape.rot.x = 0;
        thisv->actor.speedXZ = thisv->actor.velocity.y = 0.0f;
        thisv->actor.world.pos.y = thisv->actor.floorHeight;

        if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
            EnWf_SetupSlash(thisv);
        } else {
            EnWf_SetupWait(thisv);
        }
    }
}

void EnWf_SetupBlocking(EnWf* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gWolfosBlockingAnim);

    if (thisv->slashStatus != 0) {
        thisv->slashStatus = -1;
    }

    thisv->actor.speedXZ = 0.0f;
    thisv->action = WOLFOS_ACTION_BLOCKING;
    thisv->actionTimer = 10;

    Animation_Change(&thisv->skelAnime, &gWolfosBlockingAnim, 0.0f, 0.0f, lastFrame, ANIMMODE_ONCE_INTERP, -4.0f);
    EnWf_SetupAction(thisv, EnWf_Blocking);
}

void EnWf_Blocking(EnWf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    } else {
        thisv->skelAnime.playSpeed = 1.0f;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        s16 yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

        if ((ABS(yawDiff) <= 0x4000) && (thisv->actor.xzDistToPlayer < 60.0f) &&
            (ABS(thisv->actor.yDistToPlayer) < 50.0f)) {
            if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 10000, 0x4000, thisv->actor.shape.rot.y)) {
                if (player->swordAnimation == 0x11) {
                    EnWf_SetupBlocking(thisv);
                } else if ((globalCtx->gameplayFrames % 2) != 0) {
                    EnWf_SetupBlocking(thisv);
                } else {
                    EnWf_SetupBackflipAway(thisv);
                }

            } else {
                s16 angleFacingLink = player->actor.shape.rot.y - thisv->actor.shape.rot.y;

                if (!Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                    (((globalCtx->gameplayFrames % 2) != 0) || (ABS(angleFacingLink) < 0x38E0))) {
                    EnWf_SetupSlash(thisv);
                } else {
                    EnWf_SetupRunAroundPlayer(thisv);
                }
            }
        } else {
            EnWf_SetupRunAroundPlayer(thisv);
        }
    } else if (thisv->actionTimer == 0) {
        if (func_800354B4(globalCtx, &thisv->actor, 100.0f, 10000, 0x4000, thisv->actor.shape.rot.y)) {
            if (player->swordAnimation == 0x11) {
                EnWf_SetupBlocking(thisv);
            } else if ((globalCtx->gameplayFrames % 2) != 0) {
                EnWf_SetupBlocking(thisv);
            } else {
                EnWf_SetupBackflipAway(thisv);
            }
        }
    }
}

void EnWf_SetupSidestep(EnWf* thisv, GlobalContext* globalCtx) {
    s16 angle;
    Player* player;
    f32 lastFrame = Animation_GetLastFrame(&gWolfosRunningAnim);

    Animation_Change(&thisv->skelAnime, &gWolfosRunningAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -4.0f);

    player = GET_PLAYER(globalCtx);
    angle = player->actor.shape.rot.y + thisv->runAngle;

    if (Math_SinS(angle - thisv->actor.yawTowardsPlayer) > 0.0f) {
        thisv->runAngle = 16000;
    } else if (Math_SinS(angle - thisv->actor.yawTowardsPlayer) < 0.0f) {
        thisv->runAngle = -16000;
    } else if (Rand_ZeroOne() > 0.5f) {
        thisv->runAngle = 16000;
    } else {
        thisv->runAngle = -16000;
    }

    thisv->skelAnime.playSpeed = thisv->actor.speedXZ = 6.0f;
    thisv->skelAnime.playSpeed *= 0.175f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->runSpeed = 0.0f;
    thisv->actionTimer = (Rand_ZeroOne() * 10.0f) + 5.0f;
    thisv->action = WOLFOS_ACTION_SIDESTEP;

    EnWf_SetupAction(thisv, EnWf_Sidestep);
}

void EnWf_Sidestep(EnWf* thisv, GlobalContext* globalCtx) {
    s16 angleDiff1;
    Player* player = GET_PLAYER(globalCtx);
    s32 animPrevFrame;
    s32 animFrameSpeedDiff;
    s32 animSpeed;
    f32 baseRange = 0.0f;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + thisv->runAngle, 1, 3000, 1);

    // Actor_TestFloorInDirection is useless here (see comment below)
    if ((thisv->actor.bgCheckFlags & 8) ||
        !Actor_TestFloorInDirection(&thisv->actor, globalCtx, thisv->actor.speedXZ, thisv->actor.shape.rot.y)) {
        s16 angle =
            (thisv->actor.bgCheckFlags & 8) ? (thisv->actor.wallYaw - thisv->actor.yawTowardsPlayer) - thisv->runAngle : 0;

        // This is probably meant to reverse direction if the edge of a floor is encountered, but does nothing
        // unless bgCheckFlags & 8 anyway, since angle = 0 otherwise
        if (ABS(angle) > 0x2EE0) {
            thisv->runAngle = -thisv->runAngle;
        }
    }

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

    if (Actor_OtherIsTargeted(globalCtx, &thisv->actor)) {
        baseRange = 150.0f;
    }

    if (thisv->actor.xzDistToPlayer <= (60.0f + baseRange)) {
        Math_SmoothStepToF(&thisv->runSpeed, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if ((80.0f + baseRange) < thisv->actor.xzDistToPlayer) {
        Math_SmoothStepToF(&thisv->runSpeed, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->runSpeed, 0.0f, 1.0f, 6.65f, 0.0f);
    }

    if (thisv->runSpeed != 0.0f) {
        thisv->actor.world.pos.x += Math_SinS(thisv->actor.shape.rot.y) * thisv->runSpeed;
        thisv->actor.world.pos.z += Math_CosS(thisv->actor.shape.rot.y) * thisv->runSpeed;
    }

    if (ABS(thisv->runSpeed) < ABS(thisv->actor.speedXZ)) {
        thisv->skelAnime.playSpeed = thisv->actor.speedXZ * 0.175f;
    } else {
        thisv->skelAnime.playSpeed = thisv->runSpeed * 0.175f;
    }

    thisv->skelAnime.playSpeed = CLAMP(thisv->skelAnime.playSpeed, -3.0f, 3.0f);

    animPrevFrame = thisv->skelAnime.curFrame;
    SkelAnime_Update(&thisv->skelAnime);
    animFrameSpeedDiff = thisv->skelAnime.curFrame - ABS(thisv->skelAnime.playSpeed);
    animSpeed = (f32)ABS(thisv->skelAnime.playSpeed);

    if (!EnWf_ChangeAction(globalCtx, thisv, false)) {
        thisv->actionTimer--;

        if (thisv->actionTimer == 0) {
            angleDiff1 = player->actor.shape.rot.y - thisv->actor.yawTowardsPlayer;
            angleDiff1 = ABS(angleDiff1);

            if (angleDiff1 >= 0x3A98) {
                EnWf_SetupWait(thisv);
                thisv->actionTimer = (Rand_ZeroOne() * 3.0f) + 1.0f;
            } else {
                Player* player2 = GET_PLAYER(globalCtx);
                s16 angleDiff2 = player2->actor.shape.rot.y - thisv->actor.yawTowardsPlayer;

                thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

                if ((thisv->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(globalCtx, &thisv->actor) &&
                    (((globalCtx->gameplayFrames % 4) == 0) || (ABS(angleDiff2) < 0x38E0))) {
                    EnWf_SetupSlash(thisv);
                } else {
                    EnWf_SetupRunAtPlayer(thisv, globalCtx);
                }
            }
        }

        if ((animPrevFrame != (s32)thisv->skelAnime.curFrame) && (animFrameSpeedDiff <= 0) &&
            ((animSpeed + animPrevFrame) > 0)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_WALK);
            Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 20.0f, 3, 3.0f, 50, 50, 1);
        }

        if ((globalCtx->gameplayFrames & 95) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_CRY);
        }
    }
}

void EnWf_SetupDie(EnWf* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWolfosRearingUpFallingOverAnim, -4.0f);
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->unk_300 = false;
        thisv->actor.speedXZ = -6.0f;
    } else {
        thisv->unk_300 = true;
    }

    thisv->action = WOLFOS_ACTION_DIE;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionTimer = thisv->skelAnime.animLength;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_WOLFOS_DEAD);
    EnWf_SetupAction(thisv, EnWf_Die);
}

void EnWf_Die(EnWf* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 2) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
        thisv->unk_300 = false;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xD0);

        if (thisv->switchFlag != 0xFF) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }

        Actor_Kill(&thisv->actor);
    } else {
        s32 i;
        Vec3f pos;
        Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };

        thisv->actionTimer--;

        for (i = ((s32)thisv->skelAnime.animLength - thisv->actionTimer) >> 1; i >= 0; i--) {
            pos.x = Rand_CenteredFloat(60.0f) + thisv->actor.world.pos.x;
            pos.z = Rand_CenteredFloat(60.0f) + thisv->actor.world.pos.z;
            pos.y = Rand_CenteredFloat(50.0f) + (thisv->actor.world.pos.y + 20.0f);
            EffectSsDeadDb_Spawn(globalCtx, &pos, &velAndAccel, &velAndAccel, 100, 0, 255, 255, 255, 255, 0, 0, 255, 1,
                                 9, true);
        }
    }
}

void func_80B36F40(EnWf* thisv, GlobalContext* globalCtx) {
    if ((thisv->action == WOLFOS_ACTION_WAIT) && (thisv->unk_2E2 != 0)) {
        thisv->unk_4D4.y = Math_SinS(thisv->unk_2E2 * 4200) * 8920.0f;
    } else if (thisv->action != WOLFOS_ACTION_STUNNED) {
        if (thisv->action != WOLFOS_ACTION_SLASH) {
            Math_SmoothStepToS(&thisv->unk_4D4.y, thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y, 1, 1500, 0);
            thisv->unk_4D4.y = CLAMP(thisv->unk_4D4.y, -0x3127, 0x3127);
        } else {
            thisv->unk_4D4.y = 0;
        }
    }
}

void EnWf_UpdateDamage(EnWf* thisv, GlobalContext* globalCtx) {
    if (thisv->colliderSpheres.base.acFlags & AC_BOUNCED) {
        thisv->colliderSpheres.base.acFlags &= ~(AC_HIT | AC_BOUNCED);
        thisv->colliderCylinderBody.base.acFlags &= ~AC_HIT;
        thisv->colliderCylinderTail.base.acFlags &= ~AC_HIT;
    } else if ((thisv->colliderCylinderBody.base.acFlags & AC_HIT) ||
               (thisv->colliderCylinderTail.base.acFlags & AC_HIT)) {
        if (thisv->action >= WOLFOS_ACTION_WAIT) {
            s16 yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

            if ((!(thisv->colliderCylinderBody.base.acFlags & AC_HIT) &&
                 (thisv->colliderCylinderTail.base.acFlags & AC_HIT)) ||
                (ABS(yawDiff) > 19000)) {
                thisv->actor.colChkInfo.damage *= 4;
            }

            thisv->colliderCylinderBody.base.acFlags &= ~AC_HIT;
            thisv->colliderCylinderTail.base.acFlags &= ~AC_HIT;

            if (thisv->actor.colChkInfo.damageEffect != ENWF_DMGEFF_ICE_MAGIC) {
                thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;
                Actor_SetDropFlag(&thisv->actor, &thisv->colliderCylinderBody.info, 1);
                thisv->slashStatus = 0;

                if ((thisv->actor.colChkInfo.damageEffect == ENWF_DMGEFF_STUN) ||
                    (thisv->actor.colChkInfo.damageEffect == ENWF_DMGEFF_UNDEF)) {
                    if (thisv->action != WOLFOS_ACTION_STUNNED) {
                        Actor_SetColorFilter(&thisv->actor, 0, 120, 0, 80);
                        Actor_ApplyDamage(&thisv->actor);
                        EnWf_SetupStunned(thisv);
                    }
                } else { // LIGHT_MAGIC, FIRE, NONE
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 8);

                    if (thisv->damageEffect == ENWF_DMGEFF_FIRE) {
                        thisv->fireTimer = 40;
                    }

                    if (Actor_ApplyDamage(&thisv->actor) == 0) {
                        EnWf_SetupDie(thisv);
                        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                    } else {
                        EnWf_SetupDamaged(thisv);
                    }
                }
            }
        }
    }
}

void EnWf_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnWf* thisv = (EnWf*)thisx;

    EnWf_UpdateDamage(thisv, globalCtx);

    if (thisv->actor.colChkInfo.damageEffect != ENWF_DMGEFF_ICE_MAGIC) {
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 32.0f, 30.0f, 60.0f, 0x1D);
        thisv->actionFunc(thisv, globalCtx);
        func_80B36F40(thisv, globalCtx);
    }

    if (thisv->actor.bgCheckFlags & (1 | 2)) {
        func_800359B8(&thisv->actor, thisv->actor.shape.rot.y, &thisv->actor.shape.rot);
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0, 1, 1000, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.z, 0, 1, 1000, 0);
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSpheres.base);

    if (thisv->action >= WOLFOS_ACTION_WAIT) {
        if ((thisv->actor.colorFilterTimer == 0) || !(thisv->actor.colorFilterParams & 0x4000)) {
            Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCylinderBody);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCylinderTail.base);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCylinderBody.base);
        }
    }

    if (thisv->action == WOLFOS_ACTION_BLOCKING) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSpheres.base);
    }

    if (thisv->slashStatus > 0) {
        if (!(thisv->colliderSpheres.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSpheres.base);
        } else {
            EnWf_SetupRecoilFromBlockedSlash(thisv);
        }
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 25.0f;

    if (thisv->eyeIndex == 0) {
        if ((Rand_ZeroOne() < 0.2f) && ((globalCtx->gameplayFrames % 4) == 0) && (thisv->actor.colorFilterTimer == 0)) {
            thisv->eyeIndex++;
        }
    } else {
        thisv->eyeIndex = (thisv->eyeIndex + 1) & 3;
    }
}

s32 EnWf_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnWf* thisv = (EnWf*)thisx;

    if ((limbIndex == WOLFOS_LIMB_HEAD) || (limbIndex == WOLFOS_LIMB_EYES)) {
        rot->y -= thisv->unk_4D4.y;
    }

    return false;
}

void EnWf_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f colliderVec = { 1200.0f, 0.0f, 0.0f };
    static Vec3f bodyPartVec = { 0.0f, 0.0f, 0.0f };
    EnWf* thisv = (EnWf*)thisx;
    s32 bodyPartIndex = -1;

    Collider_UpdateSpheres(limbIndex, &thisv->colliderSpheres);

    if (limbIndex == WOLFOS_LIMB_TAIL) {
        Vec3f colliderPos;

        bodyPartIndex = -1;
        Matrix_MultVec3f(&colliderVec, &colliderPos);
        thisv->colliderCylinderTail.dim.pos.x = colliderPos.x;
        thisv->colliderCylinderTail.dim.pos.y = colliderPos.y;
        thisv->colliderCylinderTail.dim.pos.z = colliderPos.z;
    }

    if ((thisv->fireTimer != 0) || ((thisv->actor.colorFilterTimer != 0) && (thisv->actor.colorFilterParams & 0x4000))) {
        switch (limbIndex) {
            case WOLFOS_LIMB_EYES:
                bodyPartIndex = 0;
                break;
            case WOLFOS_LIMB_FRONT_RIGHT_LOWER_LEG:
                bodyPartIndex = 1;
                break;
            case WOLFOS_LIMB_FRONT_LEFT_LOWER_LEG:
                bodyPartIndex = 2;
                break;
            case WOLFOS_LIMB_THORAX:
                bodyPartIndex = 3;
                break;
            case WOLFOS_LIMB_ABDOMEN:
                bodyPartIndex = 4;
                break;
            case WOLFOS_LIMB_TAIL:
                bodyPartIndex = 5;
                break;
            case WOLFOS_LIMB_BACK_RIGHT_SHIN:
                bodyPartIndex = 6;
                break;
            case 37:
                //! @bug There is no limb with index thisv large, so bodyPartsPos[7] is uninitialised. Thus a flame will
                //! be drawn at 0,0,0 when the Wolfos is on fire.
                bodyPartIndex = 7;
                break;
            case WOLFOS_LIMB_BACK_RIGHT_PASTERN:
                bodyPartIndex = 8;
                break;
            case WOLFOS_LIMB_BACK_LEFT_PAW:
                bodyPartIndex = 9;
                break;
        }

        if (bodyPartIndex >= 0) {
            Vec3f bodyPartPos;

            Matrix_MultVec3f(&bodyPartVec, &bodyPartPos);
            thisv->bodyPartsPos[bodyPartIndex].x = bodyPartPos.x;
            thisv->bodyPartsPos[bodyPartIndex].y = bodyPartPos.y;
            thisv->bodyPartsPos[bodyPartIndex].z = bodyPartPos.z;
        }
    }
}

static const void* sWolfosNormalEyeTextures[] = { gWolfosNormalEyeOpenTex, gWolfosNormalEyeHalfTex, gWolfosNormalEyeNarrowTex,
                                            gWolfosNormalEyeHalfTex };
static const void* sWolfosWhiteEyeTextures[] = { gWolfosWhiteEyeOpenTex, gWolfosWhiteEyeHalfTex, gWolfosWhiteEyeNarrowTex,
                                           gWolfosWhiteEyeHalfTex };

void EnWf_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnWf* thisv = (EnWf*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_wf.c", 2157);

    // This conditional will always evaluate to true, since unk_300 is false whenever action is
    // WOLFOS_ACTION_WAIT_TO_APPEAR.
    if ((thisv->action != WOLFOS_ACTION_WAIT_TO_APPEAR) || !thisv->unk_300) {
        func_80093D18(globalCtx->state.gfxCtx);

        if (thisv->actor.params == WOLFOS_NORMAL) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sWolfosNormalEyeTextures[thisv->eyeIndex]));
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sWolfosWhiteEyeTextures[thisv->eyeIndex]));
        }

        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnWf_OverrideLimbDraw, EnWf_PostLimbDraw, &thisv->actor);

        if (thisv->fireTimer != 0) {
            thisv->actor.colorFilterTimer++;
            if (1) {}
            thisv->fireTimer--;

            if ((thisv->fireTimer % 4) == 0) {
                s32 fireIndex = thisv->fireTimer >> 2;

                EffectSsEnFire_SpawnVec3s(globalCtx, &thisv->actor, &thisv->bodyPartsPos[fireIndex], 75, 0, 0, fireIndex);
            }
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_wf.c", 2190);
}

s32 EnWf_DodgeRanged(GlobalContext* globalCtx, EnWf* thisv) {
    Actor* actor = Actor_GetProjectileActor(globalCtx, &thisv->actor, 600.0f);

    if (actor != NULL) {
        s16 angleToFacing;
        s16 pad;
        f32 dist;

        angleToFacing = Actor_WorldYawTowardActor(&thisv->actor, actor) - thisv->actor.shape.rot.y;
        thisv->actor.world.rot.y = (u16)thisv->actor.shape.rot.y & 0xFFFF;
        dist = Actor_WorldDistXYZToPoint(&thisv->actor, &actor->world.pos);

        if ((ABS(angleToFacing) < 0x2EE0) && (sqrt(dist) < 400.0)) {
            EnWf_SetupBlocking(thisv);
        } else {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x3FFF;
            if ((ABS(angleToFacing) < 0x2000) || (ABS(angleToFacing) > 0x5FFF)) {
                EnWf_SetupSidestep(thisv, globalCtx);
                thisv->actor.speedXZ *= 2.0f;
            } else if (ABS(angleToFacing) < 0x5FFF) {
                EnWf_SetupBackflipAway(thisv);
            }
        }
        return true;
    }

    return false;
}
