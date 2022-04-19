/*
 * File: z_en_wallmas
 * Overlay: En_Wallmas
 * Description: Wallmaster (Ceiling monster)
 */

#include "z_en_wallmas.h"
#include "objects/object_wallmaster/object_wallmaster.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

#define TIMER_SCALE ((f32)OS_CLOCK_RATE / 10000000000)
#define DEGREE_60_RAD (60.0f * std::numbers::pi_v<float> / 180.0f)
#define DEGREE_15_RAD (15.0f * std::numbers::pi_v<float> / 180.0f)

#define DAMAGE_EFFECT_BURN 2
#define DAMAGE_EFFECT_STUN_WHITE 4
#define DAMAGE_EFFECT_STUN_BLUE 1

void EnWallmas_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWallmas_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWallmas_Update(Actor* thisx, GlobalContext* globalCtx);
void EnWallmas_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnWallmas_TimerInit(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_ProximityOrSwitchInit(EnWallmas* thisv);
void EnWallmas_WaitToDrop(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Drop(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Land(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Stand(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_JumpToCeiling(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_ReturnToCeiling(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_TakeDamage(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Cooldown(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Die(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_TakePlayer(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_WaitForProximity(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_WaitForSwitchFlag(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Stun(EnWallmas* thisv, GlobalContext* globalCtx);
void EnWallmas_Walk(EnWallmas* thisv, GlobalContext* globalCtx);

ActorInit En_Wallmas_InitVars = {
    ACTOR_EN_WALLMAS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_WALLMASTER,
    sizeof(EnWallmas),
    (ActorFunc)EnWallmas_Init,
    (ActorFunc)EnWallmas_Destroy,
    (ActorFunc)EnWallmas_Update,
    (ActorFunc)EnWallmas_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
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
    { 30, 40, 0, { 0 } },
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
    ICHAIN_S8(naviEnemyId, 0x30, 1),
    ICHAIN_F32(targetArrowOffset, 5500, 1),
    ICHAIN_F32_DIV1000(gravity, -1500, 0),
};

void EnWallmas_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnWallmas* thisv = (EnWallmas*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 0, NULL, 0.5f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gWallmasterSkel, &gWallmasterWaitAnim, thisv->jointTable,
                       thisv->morphTable, 25);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sCylinderInit);
    CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->switchFlag = (u8)(thisx->params >> 0x8);
    thisx->params = thisx->params & 0xFF;

    if (thisx->params == WMT_FLAG) {
        if (Flags_GetSwitch(globalCtx, thisv->switchFlag) != 0) {
            Actor_Kill(thisx);
            return;
        }

        EnWallmas_ProximityOrSwitchInit(thisv);
    } else if (thisx->params == WMT_PROXIMITY) {
        EnWallmas_ProximityOrSwitchInit(thisv);
    } else {
        EnWallmas_TimerInit(thisv, globalCtx);
    }
}

void EnWallmas_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnWallmas* thisv = (EnWallmas*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnWallmas_TimerInit(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.flags |= ACTOR_FLAG_5;
    thisv->timer = 0x82;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.world.pos.y = player->actor.world.pos.y;
    thisv->actor.floorHeight = player->actor.floorHeight;
    thisv->actor.draw = EnWallmas_Draw;
    thisv->actionFunc = EnWallmas_WaitToDrop;
}

void EnWallmas_SetupDrop(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    const AnimationHeader* objSegChangee = &gWallmasterLungeAnim;

    Animation_Change(&thisv->skelAnime, objSegChangee, 0.0f, 20.0f, Animation_GetLastFrame(&gWallmasterLungeAnim),
                     ANIMMODE_ONCE, 0.0f);

    thisv->yTarget = player->actor.world.pos.y;
    thisv->actor.world.pos.y = player->actor.world.pos.y + 300.0f;
    thisv->actor.world.rot.y = player->actor.shape.rot.y + 0x8000;
    thisv->actor.floorHeight = player->actor.floorHeight;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actor.flags &= ~ACTOR_FLAG_5;
    thisv->actionFunc = EnWallmas_Drop;
}

void EnWallmas_SetupLand(EnWallmas* thisv, GlobalContext* globalCtx) {
    const AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    const AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    Animation_Change(&thisv->skelAnime, objSegChangee, 1.0f, 41.0f, Animation_GetLastFrame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 15.0f, 6, 20.0f, 0x12C, 0x64, 1);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_LAND);
    thisv->actionFunc = EnWallmas_Land;
}

void EnWallmas_SetupStand(EnWallmas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterStandUpAnim);
    thisv->actionFunc = EnWallmas_Stand;
}

void EnWallmas_SetupWalk(EnWallmas* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gWallmasterWalkAnim, 3.0f);
    thisv->actionFunc = EnWallmas_Walk;
    thisv->actor.speedXZ = 3.0f;
}

void EnWallmas_SetupJumpToCeiling(EnWallmas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterStopWalkAnim);
    thisv->actionFunc = EnWallmas_JumpToCeiling;
    thisv->actor.speedXZ = 0.0f;
}
void EnWallmas_SetupReturnToCeiling(EnWallmas* thisv) {
    const AnimationHeader* objSegFrameCount = &gWallmasterJumpAnim;
    const AnimationHeader* objSegChangee = &gWallmasterJumpAnim;

    thisv->timer = 0;
    thisv->actor.speedXZ = 0.0f;

    Animation_Change(&thisv->skelAnime, objSegChangee, 3.0f, 0.0f, Animation_GetLastFrame(objSegFrameCount),
                     ANIMMODE_ONCE, -3.0f);

    thisv->actionFunc = EnWallmas_ReturnToCeiling;
}

void EnWallmas_SetupTakeDamage(EnWallmas* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWallmasterDamageAnim, -3.0f);
    if (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x0001F824) {
        thisv->actor.world.rot.y = thisv->collider.base.ac->world.rot.y;
    } else {
        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, thisv->collider.base.ac) + 0x8000;
    }

    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0x14);
    thisv->actionFunc = EnWallmas_TakeDamage;
    thisv->actor.speedXZ = 5.0f;
    thisv->actor.velocity.y = 10.0f;
}

void EnWallmas_SetupCooldown(EnWallmas* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gWallmasterRecoverFromDamageAnim);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actionFunc = EnWallmas_Cooldown;
}

void EnWallmas_SetupDie(EnWallmas* thisv, GlobalContext* globalCtx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;

    EffectSsDeadDb_Spawn(globalCtx, &thisv->actor.world.pos, &zeroVec, &zeroVec, 250, -10, 255, 255, 255, 255, 0, 0, 255,
                         1, 9, true);

    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xC0);
    thisv->actionFunc = EnWallmas_Die;
}

void EnWallmas_SetupTakePlayer(EnWallmas* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gWallmasterHoverAnim, -5.0f);
    thisv->timer = -0x1E;
    thisv->actionFunc = EnWallmas_TakePlayer;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;

    thisv->yTarget = thisv->actor.yDistToPlayer;
    func_8002DF38(globalCtx, &thisv->actor, 0x25);
    OnePointCutscene_Init(globalCtx, 9500, 9999, &thisv->actor, MAIN_CAM);
}

void EnWallmas_ProximityOrSwitchInit(EnWallmas* thisv) {
    thisv->timer = 0;
    thisv->actor.draw = NULL;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    if (thisv->actor.params == WMT_PROXIMITY) {
        thisv->actionFunc = EnWallmas_WaitForProximity;
    } else {
        thisv->actionFunc = EnWallmas_WaitForSwitchFlag;
    }
}

void EnWallmas_SetupStun(EnWallmas* thisv) {
    Animation_Change(&thisv->skelAnime, &gWallmasterJumpAnim, 1.5f, 0, 20.0f, ANIMMODE_ONCE, -3.0f);

    thisv->actor.speedXZ = 0.0f;
    if (thisv->actor.colChkInfo.damageEffect == 4) {
        Actor_SetColorFilter(&thisv->actor, -0x8000, 0xFF, 0, 0x50);
    } else {
        Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 0x50);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    }

    thisv->timer = 0x50;
    thisv->actionFunc = EnWallmas_Stun;
}

void EnWallmas_WaitToDrop(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f* playerPos = &player->actor.world.pos;

    thisv->actor.world.pos = *playerPos;
    thisv->actor.floorHeight = player->actor.floorHeight;
    thisv->actor.floorPoly = player->actor.floorPoly;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if ((player->stateFlags1 & 0x100000) || (player->stateFlags1 & 0x8000000) || !(player->actor.bgCheckFlags & 1) ||
        ((thisv->actor.params == 1) && (320.0f < Math_Vec3f_DistXZ(&thisv->actor.home.pos, playerPos)))) {
        Audio_StopSfxById(NA_SE_EN_FALL_AIM);
        thisv->timer = 0x82;
    }

    if (thisv->timer == 0x50) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_AIM);
    }

    if (thisv->timer == 0) {
        EnWallmas_SetupDrop(thisv, globalCtx);
    }
}

void EnWallmas_Drop(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!Player_InCsMode(globalCtx) && !(player->stateFlags2 & 0x10) && (player->invincibilityTimer >= 0) &&
        (thisv->actor.xzDistToPlayer < 30.0f) && (thisv->actor.yDistToPlayer < -5.0f) &&
        (-(f32)(player->cylinder.dim.height + 10) < thisv->actor.yDistToPlayer)) {
        EnWallmas_SetupTakePlayer(thisv, globalCtx);
    }
}

void EnWallmas_Land(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnWallmas_SetupStand(thisv);
    }
}

void EnWallmas_Stand(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnWallmas_SetupWalk(thisv);
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 0xB6);
}

void EnWallmas_Walk(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnWallmas_SetupJumpToCeiling(thisv);
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, (s16)((s32)thisv->actor.yawTowardsPlayer + 0x8000), 0xB6);

    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f) ||
        Animation_OnFrame(&thisv->skelAnime, 24.0f) || Animation_OnFrame(&thisv->skelAnime, 36.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_WALK);
    }
}

void EnWallmas_JumpToCeiling(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnWallmas_SetupReturnToCeiling(thisv);
    }
}

void EnWallmas_ReturnToCeiling(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->skelAnime.curFrame > 20.0f) {
        thisv->timer += 9;
        thisv->actor.world.pos.y = thisv->actor.world.pos.y + 30.0f;
    }

    if (Animation_OnFrame(&thisv->skelAnime, 20.0f) != 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_UP);
    }

    if (thisv->actor.yDistToPlayer < -900.0f) {
        if (thisv->actor.params == WMT_FLAG) {
            Actor_Kill(&thisv->actor);
            return;
        }

        if (thisv->actor.params == WMT_TIMER ||
            Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < 200.0f) {
            EnWallmas_TimerInit(thisv, globalCtx);
        } else {
            EnWallmas_ProximityOrSwitchInit(thisv);
        }
    }
}

void EnWallmas_TakeDamage(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnWallmas_SetupDie(thisv, globalCtx);
        } else {
            EnWallmas_SetupCooldown(thisv);
        }
    }
    if (Animation_OnFrame(&thisv->skelAnime, 13.0f) != 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    }

    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.2f);
}

void EnWallmas_Cooldown(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        EnWallmas_SetupReturnToCeiling(thisv);
    }
}

void EnWallmas_Die(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->actor.scale.x, 0.0f, 0.0015) != 0) {
        Actor_SetScale(&thisv->actor, 0.01f);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xC0);
        Actor_Kill(&thisv->actor);
    }
    thisv->actor.scale.z = thisv->actor.scale.x;
    thisv->actor.scale.y = thisv->actor.scale.x;
}

void EnWallmas_TakePlayer(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (Animation_OnFrame(&thisv->skelAnime, 1.0f) != 0) {
        if (!LINK_IS_ADULT) {
            func_8002F7DC(&thisv->actor, NA_SE_VO_LI_DAMAGE_S_KID);
        } else {
            func_8002F7DC(&thisv->actor, NA_SE_VO_LI_DAMAGE_S);
        }

        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_CATCH);
    }
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        player->actor.world.pos.x = thisv->actor.world.pos.x;
        player->actor.world.pos.z = thisv->actor.world.pos.z;

        if (thisv->timer < 0) {
            thisv->actor.world.pos.y = thisv->actor.world.pos.y + 2.0f;
        } else {
            thisv->actor.world.pos.y = thisv->actor.world.pos.y + 10.0f;
        }

        if (!LINK_IS_ADULT) {
            player->actor.world.pos.y = thisv->actor.world.pos.y - 30.0f;
        } else {
            player->actor.world.pos.y = thisv->actor.world.pos.y - 50.0f;
        }

        if (thisv->timer == -0x1E) {
            if (!LINK_IS_ADULT) {
                func_8002F7DC(&thisv->actor, NA_SE_VO_LI_TAKEN_AWAY_KID);
            } else {
                func_8002F7DC(&thisv->actor, NA_SE_VO_LI_TAKEN_AWAY);
            }
        }
        if (thisv->timer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_UP);
        }

        thisv->timer = thisv->timer + 2;
    } else {
        Math_StepToF(&thisv->actor.world.pos.y, player->actor.world.pos.y + (!LINK_IS_ADULT ? 30.0f : 50.0f), 5.0f);
    }

    Math_StepToF(&thisv->actor.world.pos.x, player->actor.world.pos.x, 3.0f);
    Math_StepToF(&thisv->actor.world.pos.z, player->actor.world.pos.z, 3.0f);

    if (thisv->timer == 0x1E) {
        func_80078884(NA_SE_OC_ABYSS);
        Gameplay_TriggerRespawn(globalCtx);
    }
}

void EnWallmas_WaitForProximity(EnWallmas* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < 200.0f) {
        EnWallmas_TimerInit(thisv, globalCtx);
    }
}

void EnWallmas_WaitForSwitchFlag(EnWallmas* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag) != 0) {
        EnWallmas_TimerInit(thisv, globalCtx);
        thisv->timer = 0x51;
    }
}

void EnWallmas_Stun(EnWallmas* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnWallmas_SetupDie(thisv, globalCtx);
        } else {
            EnWallmas_SetupReturnToCeiling(thisv);
        }
    }
}

void EnWallmas_ColUpdate(EnWallmas* thisv, GlobalContext* globalCtx) {
    if ((thisv->collider.base.acFlags & AC_HIT) != 0) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_DEAD);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            } else {
                if (thisv->actor.colChkInfo.damage != 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FALL_DAMAGE);
                }
            }

            if ((thisv->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_WHITE) ||
                (thisv->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_STUN_BLUE)) {
                if (thisv->actionFunc != EnWallmas_Stun) {
                    EnWallmas_SetupStun(thisv);
                }
            } else {
                if (thisv->actor.colChkInfo.damageEffect == DAMAGE_EFFECT_BURN) {
                    EffectSsFCircle_Spawn(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40, 40);
                }

                EnWallmas_SetupTakeDamage(thisv);
            }
        }
    }
}

void EnWallmas_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnWallmas* thisv = (EnWallmas*)thisx;
    char pad[4];

    EnWallmas_ColUpdate(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);

    if ((thisv->actionFunc == EnWallmas_WaitToDrop) || (thisv->actionFunc == EnWallmas_WaitForProximity) ||
        (thisv->actionFunc == EnWallmas_TakePlayer) || (thisv->actionFunc == EnWallmas_WaitForSwitchFlag)) {
        return;
    }

    if ((thisv->actionFunc != EnWallmas_ReturnToCeiling) && (thisv->actionFunc != EnWallmas_TakePlayer)) {
        Actor_MoveForward(&thisv->actor);
    }

    if (thisv->actionFunc != EnWallmas_Drop) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 25.0f, 0.0f, 0x1D);
    } else if (thisv->actor.world.pos.y <= thisv->yTarget) {
        thisv->actor.world.pos.y = thisv->yTarget;
        thisv->actor.velocity.y = 0.0f;
        EnWallmas_SetupLand(thisv, globalCtx);
    }

    if ((thisv->actionFunc != EnWallmas_Die) && (thisv->actionFunc != EnWallmas_Drop)) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

        if ((thisv->actionFunc != EnWallmas_TakeDamage) && (thisv->actor.bgCheckFlags & 1) != 0 &&
            (thisv->actor.freezeTimer == 0)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    Actor_SetFocus(&thisv->actor, 25.0f);

    if (thisv->actionFunc == EnWallmas_TakeDamage) {
        return;
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnWallmas_DrawXlu(EnWallmas* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 xzScale;
    MtxF mf;

    if ((thisv->actor.floorPoly == NULL) || ((thisv->timer >= 0x51) && (thisv->actionFunc != EnWallmas_Stun))) {
        return;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1386);

    func_80094044(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_80038A28(thisv->actor.floorPoly, thisv->actor.world.pos.x, thisv->actor.floorHeight, thisv->actor.world.pos.z,
                  &mf);
    Matrix_Mult(&mf, MTXMODE_NEW);

    if ((thisv->actionFunc != EnWallmas_WaitToDrop) && (thisv->actionFunc != EnWallmas_ReturnToCeiling) &&
        (thisv->actionFunc != EnWallmas_TakePlayer) && (thisv->actionFunc != EnWallmas_WaitForSwitchFlag)) {
        xzScale = thisv->actor.scale.x * 50.0f;
    } else {
        xzScale = ((0x50 - thisv->timer) >= 0x51 ? 0x50 : (0x50 - thisv->timer)) * TIMER_SCALE;
    }

    Matrix_Scale(xzScale, 1.0f, xzScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1421), G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1426);
}

s32 EnWallMas_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnWallmas* thisv = (EnWallmas*)thisx;

    if (limbIndex == 1) {
        if (thisv->actionFunc != EnWallmas_TakePlayer) {
            pos->z -= 1600.0f;
        } else {
            pos->z -= ((1600.0f * (thisv->skelAnime.endFrame - thisv->skelAnime.curFrame)) / thisv->skelAnime.endFrame);
        }
    }
    return false;
}

void EnWallMas_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 2) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1478);

        Matrix_Push();
        Matrix_Translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_RotateY(DEGREE_60_RAD, MTXMODE_APPLY);
        Matrix_RotateZ(DEGREE_15_RAD, MTXMODE_APPLY);
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1489), G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, gWallmasterFingerDL);

        Matrix_Pop();

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_wallmas.c", 1495);
    }
}

void EnWallmas_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnWallmas* thisv = (EnWallmas*)thisx;

    if (thisv->actionFunc != EnWallmas_WaitToDrop) {
        func_80093D18(globalCtx->state.gfxCtx);
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnWallMas_OverrideLimbDraw, EnWallMas_PostLimbDraw, thisv);
    }

    EnWallmas_DrawXlu(thisv, globalCtx);
}
