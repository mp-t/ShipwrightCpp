/*
 * File: z_en_tp.c
 * Overlay: ovl_En_Tp
 * Description: Electric Tailpasaran
 */

#include "z_en_tp.h"
#include "objects/object_tp/object_tp.h"

#define FLAGS 0

void EnTp_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTp_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTp_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTp_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnTp_Tail_SetupFollowHead(EnTp* thisv);
void EnTp_Tail_FollowHead(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_Head_SetupApproachPlayer(EnTp* thisv);
void EnTp_Head_ApproachPlayer(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_SetupDie(EnTp* thisv);
void EnTp_Die(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_Fragment_SetupFade(EnTp* thisv);
void EnTp_Fragment_Fade(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_Head_SetupTakeOff(EnTp* thisv);
void EnTp_Head_TakeOff(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_Head_SetupWait(EnTp* thisv);
void EnTp_Head_Wait(EnTp* thisv, GlobalContext* globalCtx);
void EnTp_Head_SetupBurrowReturnHome(EnTp* thisv);
void EnTp_Head_BurrowReturnHome(EnTp* thisv, GlobalContext* globalCtx);

typedef enum {
    /* 0 */ TAILPASARAN_ACTION_FRAGMENT_FADE,
    /* 1 */ TAILPASARAN_ACTION_DIE,
    /* 2 */ TAILPASARAN_ACTION_TAIL_FOLLOWHEAD,
    /* 4 */ TAILPASARAN_ACTION_HEAD_WAIT = 4,
    /* 7 */ TAILPASARAN_ACTION_HEAD_APPROACHPLAYER = 7,
    /* 8 */ TAILPASARAN_ACTION_HEAD_TAKEOFF,
    /* 9 */ TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME
} TailpasaranAction;

const ActorInit En_Tp_InitVars = {
    ACTOR_EN_TP,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_TP,
    sizeof(EnTp),
    (ActorFunc)EnTp_Init,
    (ActorFunc)EnTp_Destroy,
    (ActorFunc)EnTp_Update,
    (ActorFunc)EnTp_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x03, 0x08 },
            { 0xFFCFFFFF, 0x01, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 4 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT1,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

typedef enum {
    /* 00 */ TAILPASARAN_DMGEFF_NONE,
    /* 01 */ TAILPASARAN_DMGEFF_DEKUNUT,
    /* 14 */ TAILPASARAN_DMGEFF_SHOCKING = 14, // Kills the Tailpasaran but shocks Player
    /* 15 */ TAILPASARAN_DMGEFF_INSULATING     // Kills the Tailpasaran and does not shock Player
} TailpasaranDamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_INSULATING),
    /* Slingshot     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_INSULATING),
    /* Normal arrow  */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Hookshot      */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master sword  */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant's Knife */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Fire arrow    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice arrow     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Ice magic     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant spin    */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master spin   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Kokiri jump   */ DMG_ENTRY(2, TAILPASARAN_DMGEFF_SHOCKING),
    /* Giant jump    */ DMG_ENTRY(8, TAILPASARAN_DMGEFF_SHOCKING),
    /* Master jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 1     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, TAILPASARAN_DMGEFF_SHOCKING),
    /* Unknown 2     */ DMG_ENTRY(0, TAILPASARAN_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 10, ICHAIN_STOP),
};

void EnTp_SetupAction(EnTp* thisv, EnTpActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnTp_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnTp* thisv = (EnTp*)thisx;
    EnTp* now;
    EnTp* next;
    s32 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.targetMode = 3;
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.14f);
    thisv->unk_150 = 0;
    thisv->actor.colChkInfo.health = 1;
    now = thisv;
    thisv->alpha = 255;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);

    if (thisv->actor.params <= TAILPASARAN_HEAD) {
        thisv->actor.naviEnemyId = 0x06;
        thisv->timer = 0;
        thisv->collider.base.acFlags |= AC_HARD;
        thisv->collider.elements->dim.modelSphere.radius = thisv->collider.elements->dim.worldSphere.radius = 8;
        EnTp_Head_SetupWait(thisv);
        thisv->actor.focus.pos = thisv->actor.world.pos;
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4;
        Actor_SetScale(&thisv->actor, 1.5f);

        for (i = 0; i <= 6; i++) {
            next = (EnTp*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_TP, thisv->actor.world.pos.x,
                                      thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0 * i);

            if (0 * i) {} // Very fake, but needed to get the s registers right
            if (next != NULL) {
                now->actor.child = &next->actor;
                next->actor.parent = &now->actor;
                next->kiraSpawnTimer = i + 1;
                next->head = thisv;
                Actor_SetScale(&next->actor, 0.3f);

                if (i == 2) {
                    next->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4;
                    next->unk_150 = 1; // Why?
                }

                next->timer = next->unk_15C = i * -5;
                next->horizontalVariation = 6.0f - (i * 0.75f);
                now = next;
                if (0 * i) {}
            }
        }
    } else if (thisv->actor.params == TAILPASARAN_TAIL) {
        EnTp_Tail_SetupFollowHead(thisv);
    } else {
        EnTp_Fragment_SetupFade(thisv);
    }
}

void EnTp_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnTp* thisv = (EnTp*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnTp_Tail_SetupFollowHead(EnTp* thisv) {
    thisv->actionIndex = TAILPASARAN_ACTION_TAIL_FOLLOWHEAD;
    EnTp_SetupAction(thisv, EnTp_Tail_FollowHead);
}

void EnTp_Tail_FollowHead(EnTp* thisv, GlobalContext* globalCtx) {
    s16 angle;
    s16 phase;

    if (thisv->actor.params == TAILPASARAN_TAIL_DYING) {
        thisv->actionIndex = TAILPASARAN_ACTION_DIE;

        if (thisv->actor.parent == NULL) {
            EnTp_SetupDie(thisv);
        }
    } else {
        if (thisv->unk_150 != 0) {
            thisv->actor.flags |= ACTOR_FLAG_0;
        }

        if (thisv->head->unk_150 != 0) {
            thisv->actor.speedXZ = thisv->red = thisv->actor.velocity.y = thisv->heightPhase = 0.0f;
            if (thisv->actor.world.pos.y < thisv->head->actor.home.pos.y) {
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            }

            thisv->actor.world.pos = thisv->actor.parent->prevPos;
        } else {
            Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.parent->world.pos.y - 4.0f, 1.0f, 1.0f, 0.0f);
            angle = thisv->head->actor.shape.rot.y + 0x4000;
            phase = 2000 * (thisv->head->unk_15C + thisv->timer);
            thisv->actor.world.pos.x =
                thisv->actor.home.pos.x + Math_SinS(phase) * (Math_SinS(angle) * thisv->horizontalVariation);
            thisv->actor.world.pos.z =
                thisv->actor.home.pos.z + Math_SinS(phase) * (Math_CosS(angle) * thisv->horizontalVariation);
        }
    }
}

void EnTp_Head_SetupApproachPlayer(EnTp* thisv) {
    thisv->actionIndex = TAILPASARAN_ACTION_HEAD_APPROACHPLAYER;
    thisv->timer = 200;
    EnTp_SetupAction(thisv, EnTp_Head_ApproachPlayer);
}

void EnTp_Head_ApproachPlayer(EnTp* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.world.pos.y, player->actor.world.pos.y + 30.0f, 1.0f, 0.5f, 0.0f);
    Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == thisv->collider.base.at) {
            thisv->timer = 1;
        }
    }

    if (thisv->red < 255) {
        thisv->red += 15;
    }

    if (Math_CosF(thisv->heightPhase) == 0.0f) {
        thisv->extraHeightVariation = 2.0f * Rand_ZeroOne();
    }

    thisv->actor.world.pos.y += Math_CosF(thisv->heightPhase) * (2.0f + thisv->extraHeightVariation);
    thisv->heightPhase += 0.2f;
    Math_SmoothStepToF(&thisv->actor.speedXZ, 2.5f, 0.1f, 0.2f, 0.0f);
    thisv->timer--;

    if (thisv->timer != 0) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 750, 0);
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    } else {
        EnTp_Head_SetupBurrowReturnHome(thisv);
    }
}

void EnTp_SetupDie(EnTp* thisv) {
    Actor* now;

    thisv->timer = 2;

    if (thisv->actor.params <= TAILPASARAN_HEAD) {
        for (now = thisv->actor.child; now != NULL; now = now->child) {
            now->params = TAILPASARAN_TAIL_DYING;
            now->colChkInfo.health = 0;
        }

        thisv->timer = 13;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TAIL_DEAD);
    }
    thisv->actionIndex = TAILPASARAN_ACTION_DIE;
    EnTp_SetupAction(thisv, EnTp_Die);
}

/**
 * Spawns effects and smaller tail segment-like fragments
 */
void EnTp_Die(EnTp* thisv, GlobalContext* globalCtx) {
    EnTp* now;
    s16 i;
    s32 pad;
    Vec3f effectVelAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f effectPos = { 0.0f, 0.0f, 0.0f };

    thisv->timer--;

    if (thisv->timer <= 0) {
        if (thisv->actor.params == TAILPASARAN_HEAD_DYING) {
            effectPos.x = ((Rand_ZeroOne() - 0.5f) * 15.0f) + thisv->actor.world.pos.x;
            effectPos.z = ((Rand_ZeroOne() - 0.5f) * 15.0f) + thisv->actor.world.pos.z;
            effectPos.y = ((Rand_ZeroOne() - 0.5f) * 5.0f) + thisv->actor.world.pos.y;
            EffectSsDeadDb_Spawn(globalCtx, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0,
                                 0, 255, 1, 9, 1);

            effectPos.x = ((Rand_ZeroOne() - 0.5f) * 15.0f) + thisv->actor.world.pos.x;
            effectPos.z = ((Rand_ZeroOne() - 0.5f) * 15.0f) + thisv->actor.world.pos.z;
            effectPos.y = ((Rand_ZeroOne() - 0.5f) * 5.0f) + thisv->actor.world.pos.y;
            EffectSsDeadDb_Spawn(globalCtx, &effectPos, &effectVelAccel, &effectVelAccel, 100, 0, 255, 255, 255, 255, 0,
                                 0, 255, 1, 9, 1);
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x50);
        } else {
            for (i = 0; i < 1; i++) {
                now =
                    (EnTp*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_TP, thisv->actor.world.pos.x,
                                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, TAILPASARAN_FRAGMENT);

                if (now != NULL) {
                    Actor_SetScale(&now->actor, thisv->actor.scale.z * 0.5f);
                    now->red = thisv->red;
                }
            }
        }

        if (thisv->actor.child != NULL) {
            thisv->actor.child->parent = NULL;
            thisv->actor.child->params = TAILPASARAN_TAIL_DYING;
            thisv->actor.child->colChkInfo.health = 0;
        }

        thisv->unk_150 = 2;
        Actor_Kill(&thisv->actor);
    }
}

void EnTp_Fragment_SetupFade(EnTp* thisv) {
    thisv->actionIndex = TAILPASARAN_ACTION_FRAGMENT_FADE;
    thisv->actor.world.pos.x += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    thisv->actor.world.pos.y += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    thisv->actor.world.pos.z += ((Rand_ZeroOne() - 0.5f) * 5.0f);
    thisv->actor.velocity.x = (Rand_ZeroOne() - 0.5f) * 1.5f;
    thisv->actor.velocity.y = (Rand_ZeroOne() - 0.5f) * 1.5f;
    thisv->actor.velocity.z = (Rand_ZeroOne() - 0.5f) * 1.5f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnTp_SetupAction(thisv, EnTp_Fragment_Fade);
}

void EnTp_Fragment_Fade(EnTp* thisv, GlobalContext* globalCtx) {
    func_8002D7EC(&thisv->actor);
    thisv->alpha -= 20;

    if (thisv->alpha < 20) {
        thisv->alpha = 0;
        Actor_Kill(&thisv->actor);
    }
}

void EnTp_Head_SetupTakeOff(EnTp* thisv) {
    thisv->timer = (Rand_ZeroOne() * 15.0f) + 40.0f;
    thisv->actionIndex = TAILPASARAN_ACTION_HEAD_TAKEOFF;
    EnTp_SetupAction(thisv, EnTp_Head_TakeOff);
}

/**
 * Flies up and loops around until it makes for Player
 */
void EnTp_Head_TakeOff(EnTp* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 2.5f, 0.1f, 0.2f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.y, player->actor.world.pos.y + 85.0f + thisv->horizontalVariation, 1.0f,
                       thisv->actor.speedXZ * 0.25f, 0.0f);
    Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        if (&player->actor == thisv->collider.base.at) {
            thisv->unk_15C = 1;
        }
    }

    if (thisv->red != 0) {
        thisv->red -= 15;
    }

    if (Math_CosF(thisv->heightPhase) == 0.0f) {
        thisv->extraHeightVariation = Rand_ZeroOne() * 4.0f;
    }

    thisv->actor.world.pos.y +=
        Math_CosF(thisv->heightPhase) * ((thisv->actor.speedXZ * 0.25f) + thisv->extraHeightVariation);
    thisv->actor.world.rot.y += thisv->unk_164;
    thisv->heightPhase += 0.2f;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos), 1, 750,
                       0);

    if (thisv->timer == 0) {
        EnTp_Head_SetupApproachPlayer(thisv);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnTp_Head_SetupWait(EnTp* thisv) {
    thisv->actionIndex = TAILPASARAN_ACTION_HEAD_WAIT;
    thisv->unk_150 = 0;
    thisv->actor.shape.rot.x = -0x4000;
    thisv->timer = 60;
    thisv->unk_15C = 0;
    thisv->actor.speedXZ = 0.0f;
    EnTp_SetupAction(thisv, EnTp_Head_Wait);
}

/**
 * Awaken and rise from the ground when Player is closer than 200
 */
void EnTp_Head_Wait(EnTp* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yaw;

    thisv->unk_15C--;

    if (thisv->actor.xzDistToPlayer < 200.0f) {
        if (thisv->collider.base.atFlags & AT_HIT) {
            thisv->collider.base.atFlags &= ~AT_HIT;
            if (&player->actor == thisv->collider.base.at) {
                thisv->timer = 0;
            }
        }

        if (thisv->timer != 0) {
            thisv->timer--;

            Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0, 1, 500, 0);
            Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 1500, 0);

            yaw = Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos) + 0x4000;
            Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 30.0f, 0.3f, 1.0f, 0.3f);
            thisv->actor.world.pos.x = thisv->actor.home.pos.x +
                                      (Math_SinS(2000 * thisv->unk_15C) * (Math_SinS(yaw) * thisv->horizontalVariation));
            thisv->actor.world.pos.z = thisv->actor.home.pos.z +
                                      (Math_SinS(2000 * thisv->unk_15C) * (Math_CosS(yaw) * thisv->horizontalVariation));
        } else {
            thisv->actor.shape.rot.x = 0;
            thisv->unk_150 = 1;
            EnTp_Head_SetupTakeOff(thisv);
        }
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, -0x4000, 1, 500, 0);

        if (Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.3f, 1.5f, 0.3f) == 0.0f) {
            thisv->timer = 60;
        } else {
            yaw = Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos);
            thisv->actor.world.pos.x =
                thisv->actor.home.pos.x + (Math_SinS(2000 * thisv->unk_15C) * (Math_SinS(yaw) * 6.0f));
            thisv->actor.world.pos.z =
                thisv->actor.home.pos.z + (Math_SinS(2000 * thisv->unk_15C) * (Math_CosS(yaw) * 6.0f));
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (thisv->actor.world.pos.y != thisv->actor.home.pos.y) {
        Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnTp_Head_SetupBurrowReturnHome(EnTp* thisv) {
    thisv->actionIndex = TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME;
    thisv->timer = 0;
    EnTp_SetupAction(thisv, EnTp_Head_BurrowReturnHome);
}

void EnTp_Head_BurrowReturnHome(EnTp* thisv, GlobalContext* globalCtx) {
    static Vec3f bubbleAccel = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 bubblePrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 bubbleEnvColor = { 150, 150, 150, 0 };
    Vec3f bubbleVelocity;
    Vec3f bubblePos;
    s32 closeToFloor;
    EnTp* now;
    s16 temp_v0; // Required to match, usage can maybe be improved

    closeToFloor = false;
    temp_v0 = thisv->timer;
    thisv->unk_15C--;

    if ((temp_v0 != 0) || ((thisv->actor.home.pos.y - thisv->actor.world.pos.y) > 60.0f)) {
        thisv->timer = temp_v0 - 1;
        temp_v0 = thisv->timer;

        if (temp_v0 == 0) {
            EnTp_Head_SetupWait(thisv);

            for (now = (EnTp*)thisv->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                now->unk_15C = now->timer;
            }
        } else {
            if (thisv->actor.shape.rot.x != -0x4000) {
                thisv->timer = 80;
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.speedXZ = 0.0f;
                thisv->actor.world.pos = thisv->actor.home.pos;
                thisv->actor.shape.rot.x = -0x4000;

                for (now = (EnTp*)thisv->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->actor.velocity.y = 0.0f;
                    now->actor.speedXZ = 0.0f;
                    now->actor.world.pos = thisv->actor.home.pos;
                    now->actor.world.pos.y = thisv->actor.home.pos.y - 80.0f;
                }
            }

            thisv->actor.world.pos.y = thisv->actor.home.pos.y - thisv->timer;
        }
    } else {
        if (thisv->actor.shape.rot.x != 0x4000) {
            thisv->actor.shape.rot.x -= 0x400;
        }

        if (thisv->red != 0) {
            thisv->red -= 15;
        }

        thisv->actor.speedXZ = 2.0f * Math_CosS(thisv->actor.shape.rot.x);
        thisv->actor.velocity.y = Math_SinS(thisv->actor.shape.rot.x) * -2.0f;

        if ((thisv->actor.world.pos.y - thisv->actor.floorHeight) < 20.0f) {
            closeToFloor = true;
        }

        if (thisv->actor.world.pos.y != thisv->actor.home.pos.y) {
            Audio_PlaySoundGeneral(NA_SE_EN_TAIL_FLY - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }

        if (closeToFloor && ((globalCtx->gameplayFrames & 1) != 0)) {
            bubblePos = thisv->actor.world.pos;
            bubblePos.y = thisv->actor.floorHeight;

            bubbleVelocity.x = Rand_CenteredFloat(5.0f);
            bubbleVelocity.y = (Rand_ZeroOne() * 3.5f) + 1.5f;
            bubbleVelocity.z = Rand_CenteredFloat(5.0f);

            EffectSsDtBubble_SpawnCustomColor(globalCtx, &bubblePos, &bubbleVelocity, &bubbleAccel, &bubblePrimColor,
                                              &bubbleEnvColor, Rand_S16Offset(100, 50), 20, 0);
        }
    }
}

void EnTp_UpdateDamage(EnTp* thisv, GlobalContext* globalCtx) {
    s32 phi_s2;
    s32 phi_s4;
    EnTp* head; // Can eliminate thisv and just use now, but they're used differently
    EnTp* now;

    if ((thisv->collider.base.acFlags & AC_HIT) && (thisv->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD)) {
        phi_s4 = phi_s2 = 0;

        if (thisv->actor.params <= TAILPASARAN_HEAD) {
            phi_s2 = 1;
        }

        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&thisv->actor, &thisv->collider, 1);
        thisv->damageEffect = thisv->actor.colChkInfo.damageEffect;

        if (thisv->actor.colChkInfo.damageEffect != TAILPASARAN_DMGEFF_NONE) {
            if (thisv->actor.colChkInfo.damageEffect == TAILPASARAN_DMGEFF_DEKUNUT) {
                phi_s4 = 1;
            }

            // Head is invincible
            if (phi_s2 == 0) {
                Actor_ApplyDamage(&thisv->actor);
            }

            if (thisv->actor.colChkInfo.health == 0) {
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                head = thisv->head;

                if (head->actor.params <= TAILPASARAN_HEAD) {
                    EnTp_SetupDie(head);
                    head->damageEffect = thisv->actor.colChkInfo.damageEffect;
                    head->actor.params = TAILPASARAN_HEAD_DYING;
                }
            } else {
                if (phi_s4 != 0) {
                    thisv->actor.freezeTimer = 80;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    if (phi_s2 != 0) {
                        Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 0x50);
                    } else {
                        Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0x2000, 0x50);
                    }
                }

                for (now = (EnTp*)thisv->actor.parent; now != NULL; now = (EnTp*)now->actor.parent) {
                    now->collider.base.acFlags &= ~AC_HIT;

                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);

                        if (phi_s2 != 0) {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0, 0x50);
                        } else {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0x2000, 0x50);
                        }
                    }
                }

                for (now = (EnTp*)thisv->actor.child; now != NULL; now = (EnTp*)now->actor.child) {
                    now->collider.base.acFlags &= ~AC_HIT;
                    if (phi_s4 != 0) {
                        now->actor.freezeTimer = 80;

                        if (phi_s2 != 0) {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0, 0x50);
                        } else {
                            Actor_SetColorFilter(&now->actor, 0, 0xFF, 0x2000, 0x50);
                        }
                    }
                }
            }
        }
    }
}

void EnTp_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnTp* thisv = (EnTp*)thisx;
    Vec3f kiraVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f kiraAccel = { 0.0f, -0.6f, 0.0f };
    Vec3f kiraPos;
    Color_RGBA8 kiraPrimColor = { 0, 0, 255, 255 };
    Color_RGBA8 kiraEnvColor = { 0, 0, 0, 0 };
    Player* player = GET_PLAYER(globalCtx);
    s16 yawToWall;

    if (player->stateFlags1 & 0x4000000) { // Shielding
        thisv->damageEffect = TAILPASARAN_DMGEFF_NONE;
    }

    if (thisv->actor.colChkInfo.health != 0) {
        EnTp_UpdateDamage(thisv, globalCtx);
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.params <= TAILPASARAN_HEAD) {
        Actor_MoveForward(&thisv->actor);

        if (thisv->actionIndex != TAILPASARAN_ACTION_HEAD_BURROWRETURNHOME) {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 15.0f, 10.0f, 5);
        }

        // Turn away from wall
        if ((thisv->actor.speedXZ != 0.0f) && (thisv->actor.bgCheckFlags & 8)) {
            yawToWall = thisv->actor.wallYaw - thisv->actor.world.rot.y;

            if (ABS(yawToWall) > 0x4000) {
                if (yawToWall >= 0) {
                    thisv->actor.world.rot.y -= 500;
                } else {
                    thisv->actor.world.rot.y += 500;
                }

                thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
            }
        }

        thisv->actor.shape.rot.z += 0x800;

        if (thisv->actor.shape.rot.z == 0) {
            Audio_PlaySoundGeneral(NA_SE_EN_TAIL_CRY, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }

        if (thisv->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    if (thisv->actor.params != TAILPASARAN_TAIL_DYING) {
        thisv->kiraSpawnTimer--;
        thisv->kiraSpawnTimer &= 7;
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;

    if (thisv->damageEffect == TAILPASARAN_DMGEFF_SHOCKING) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if ((thisv->kiraSpawnTimer & 7) == 0) {
        kiraPrimColor.r = thisv->red;
        kiraAccel.x = -thisv->actor.velocity.x * 0.25f;
        kiraAccel.y = -thisv->actor.velocity.y * 0.25f;
        kiraAccel.z = -thisv->actor.velocity.z * 0.25f;
        kiraPos.x = ((Rand_ZeroOne() - 0.5f) * 25.0f) + thisv->actor.world.pos.x;
        kiraPos.y = ((Rand_ZeroOne() - 0.5f) * 20.0f) + thisv->actor.world.pos.y;
        kiraPos.z = ((Rand_ZeroOne() - 0.5f) * 25.0f) + thisv->actor.world.pos.z;
        EffectSsKiraKira_SpawnSmall(globalCtx, &kiraPos, &kiraVelocity, &kiraAccel, &kiraPrimColor, &kiraEnvColor);
    }

    if ((thisv->actionIndex >= TAILPASARAN_ACTION_TAIL_FOLLOWHEAD) && (thisv->actor.colChkInfo.health != 0)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void EnTp_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnTp* thisv = (EnTp*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_tp.c", 1451);

    if (thisv->unk_150 != 2) {
        if ((thisx->params <= TAILPASARAN_HEAD) || (thisx->params == TAILPASARAN_HEAD_DYING)) {
            func_80093D18(globalCtx->state.gfxCtx);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_tp.c", 1459),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gTailpasaranHeadDL);

            Matrix_Translate(0.0f, 0.0f, 8.0f, MTXMODE_APPLY);
        } else {
            func_80093D84(globalCtx->state.gfxCtx);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, thisv->red, 0, 255, thisv->alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT,
                              TEXEL0, ENVIRONMENT);
            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gTailpasaranTailSegmentTex));
            gDPPipeSync(POLY_XLU_DISP++);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_tp.c", 1480),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gTailpasaranTailSegmentDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_tp.c", 1495);

    if ((thisx->params <= TAILPASARAN_TAIL) || (thisx->params == TAILPASARAN_TAIL_DYING)) {
        Collider_UpdateSpheres(0, &thisv->collider);
    }
}
