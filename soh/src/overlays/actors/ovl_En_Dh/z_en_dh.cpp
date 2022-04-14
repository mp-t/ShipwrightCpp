#include "z_en_dh.h"
#include "objects/object_dh/object_dh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_10)

typedef enum {
    /* 0 */ DH_WAIT,
    /* 1 */ DH_RETREAT,
    /* 2 */ DH_BURROW,
    /* 3 */ DH_WALK,
    /* 4 */ DH_ATTACK,
    /* 5 */ DH_DEATH,
    /* 6 */ DH_DAMAGE
} EnDhAction;

void EnDh_Init(Actor* thisv, GlobalContext* globalCtx);
void EnDh_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDh_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDh_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDh_SetupWait(EnDh* thisv);
void EnDh_SetupWalk(EnDh* thisv);
void EnDh_SetupAttack(EnDh* thisv);
void EnDh_SetupBurrow(EnDh* thisv);

void EnDh_Wait(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Walk(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Retreat(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Attack(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Burrow(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Damage(EnDh* thisv, GlobalContext* globalCtx);
void EnDh_Death(EnDh* thisv, GlobalContext* globalCtx);

const ActorInit En_Dh_InitVars = {
    ACTOR_EN_DH,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DH,
    sizeof(EnDh),
    (ActorFunc)EnDh_Init,
    (ActorFunc)EnDh_Destroy,
    (ActorFunc)EnDh_Update,
    (ActorFunc)EnDh_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 35, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static DamageTable D_809EC620 = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(2, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(2, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(4, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x2F, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

void EnDh_SetupAction(EnDh* thisv, EnDhActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnDh_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDh* thisv = (EnDh*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &D_809EC620;
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_dh_Skel_007E88, &object_dh_Anim_005880, thisv->jointTable,
                       thisv->limbRotTable, 16);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 64.0f);
    thisv->actor.params = ENDH_WAIT_UNDERGROUND;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.colChkInfo.health = LINK_IS_ADULT ? 14 : 20;
    thisv->alpha = thisv->unk_258 = 255;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    Collider_InitCylinder(globalCtx, &thisv->collider1);
    Collider_SetCylinder(globalCtx, &thisv->collider1, &thisv->actor, &sCylinderInit);
    Collider_InitJntSph(globalCtx, &thisv->collider2);
    Collider_SetJntSph(globalCtx, &thisv->collider2, &thisv->actor, &sJntSphInit, thisv->elements);
    EnDh_SetupWait(thisv);
}

void EnDh_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDh* thisv = (EnDh*)thisx;

    func_800F5B58();
    Collider_DestroyCylinder(globalCtx, &thisv->collider1);
    Collider_DestroyJntSph(globalCtx, &thisv->collider2);
}

void EnDh_SpawnDebris(GlobalContext* globalCtx, EnDh* thisv, Vec3f* spawnPos, f32 spread, s32 arg4, f32 accelXZ,
                      f32 scale) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 spreadAngle;
    f32 scaleMod;

    spreadAngle = (Rand_ZeroOne() - 0.5f) * 6.28f;
    pos.y = thisv->actor.floorHeight;
    pos.x = (Math_SinF(spreadAngle) * spread) + spawnPos->x;
    pos.z = (Math_CosF(spreadAngle) * spread) + spawnPos->z;
    accel.x = (Rand_ZeroOne() - 0.5f) * accelXZ;
    accel.z = (Rand_ZeroOne() - 0.5f) * accelXZ;
    vel.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    scaleMod = (Rand_ZeroOne() * 5.0f) + 12.0f;
    EffectSsHahen_Spawn(globalCtx, &pos, &vel, &accel, arg4, scaleMod * scale, -1, 10, NULL);
}

void EnDh_SetupWait(EnDh* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_dh_Anim_003A8C);
    thisv->curAction = DH_WAIT;
    thisv->actor.world.pos.x = Rand_CenteredFloat(600.0f) + thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = Rand_CenteredFloat(600.0f) + thisv->actor.home.pos.z;
    thisv->actor.shape.yOffset = -15000.0f;
    thisv->dirtWaveSpread = thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.flags |= ACTOR_FLAG_7;
    thisv->dirtWavePhase = thisv->actionState = thisv->actor.params = ENDH_WAIT_UNDERGROUND;
    EnDh_SetupAction(thisv, EnDh_Wait);
}

void EnDh_Wait(EnDh* thisv, GlobalContext* globalCtx) {
    if ((s32)thisv->skelAnime.curFrame == 5) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (Actor_GetCollidedExplosive(globalCtx, &thisv->collider1.base)) {
        thisv->actor.params = ENDH_START_ATTACK_BOMB;
    }
    if ((thisv->actor.params >= ENDH_START_ATTACK_GRAB) || (thisv->actor.params <= ENDH_HANDS_KILLED_4)) {
        switch (thisv->actionState) {
            case 0:
                thisv->actor.flags |= ACTOR_FLAG_0;
                thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
                thisv->actor.flags &= ~ACTOR_FLAG_7;
                thisv->actionState++;
                thisv->drawDirtWave++;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_HIDE);
            case 1:
                thisv->dirtWavePhase += 0x3A7;
                Math_SmoothStepToF(&thisv->dirtWaveSpread, 300.0f, 1.0f, 5.0f, 0.0f);
                thisv->dirtWaveHeight = Math_SinS(thisv->dirtWavePhase) * 55.0f;
                thisv->dirtWaveAlpha = (s16)(Math_SinS(thisv->dirtWavePhase) * 255.0f);
                EnDh_SpawnDebris(globalCtx, thisv, &thisv->actor.world.pos, thisv->dirtWaveSpread, 4, 2.05f, 1.2f);
                if (thisv->actor.shape.yOffset == 0.0f) {
                    thisv->drawDirtWave = false;
                    thisv->actionState++;
                } else if (thisv->dirtWavePhase > 0x12C0) {
                    thisv->actor.shape.yOffset += 500.0f;
                }
                break;
            case 2:
                EnDh_SetupWalk(thisv);
                break;
        }
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x7D0, 0);
        SkelAnime_Update(&thisv->skelAnime);
        if (thisv->actor.params != ENDH_START_ATTACK_BOMB) {
            func_8008EEAC(globalCtx, &thisv->actor);
        }
    }
}

void EnDh_SetupWalk(EnDh* thisv) {
    Animation_Change(&thisv->skelAnime, &object_dh_Anim_003A8C, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_dh_Anim_003A8C) - 3.0f, ANIMMODE_LOOP, -6.0f);
    thisv->curAction = DH_WALK;
    thisv->timer = 300;
    thisv->actor.speedXZ = 1.0f;
    EnDh_SetupAction(thisv, EnDh_Walk);
}

void EnDh_Walk(EnDh* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0xFA, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    SkelAnime_Update(&thisv->skelAnime);
    if (((s32)thisv->skelAnime.curFrame % 8) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_WALK);
    }
    if ((globalCtx->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_LAUGH);
    }
    if (thisv->actor.xzDistToPlayer <= 100.0f) {
        thisv->actor.speedXZ = 0.0f;
        if (Actor_IsFacingPlayer(&thisv->actor, 60 * 0x10000 / 360)) {
            EnDh_SetupAttack(thisv);
        }
    } else if (--thisv->timer == 0) {
        EnDh_SetupBurrow(thisv);
    }
}

void EnDh_SetupRetreat(EnDh* thisv, GlobalContext* globalCtx) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_dh_Anim_005880, -4.0f);
    thisv->curAction = DH_RETREAT;
    thisv->timer = 70;
    thisv->actor.speedXZ = 1.0f;
    EnDh_SetupAction(thisv, EnDh_Retreat);
}

void EnDh_Retreat(EnDh* thisv, GlobalContext* globalCtx) {
    thisv->timer--;
    if (thisv->timer == 0) {
        thisv->retreat = false;
        EnDh_SetupBurrow(thisv);
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, (s16)(thisv->actor.yawTowardsPlayer + 0x8000), 1, 0xBB8, 0);
    }
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    SkelAnime_Update(&thisv->skelAnime);
}

void EnDh_SetupAttack(EnDh* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_dh_Anim_004658, -6.0f);
    thisv->timer = thisv->actionState = 0;
    thisv->curAction = DH_ATTACK;
    thisv->actor.speedXZ = 0.0f;
    EnDh_SetupAction(thisv, EnDh_Attack);
}

void EnDh_Attack(EnDh* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actionState++;
    } else if ((thisv->actor.xzDistToPlayer > 100.0f) || !Actor_IsFacingPlayer(&thisv->actor, 60 * 0x10000 / 360)) {
        Animation_Change(&thisv->skelAnime, &object_dh_Anim_004658, -1.0f, thisv->skelAnime.curFrame, 0.0f, ANIMMODE_ONCE,
                         -4.0f);
        thisv->actionState = 4;
        thisv->collider2.base.atFlags = thisv->collider2.elements[0].info.toucherFlags = AT_NONE; // also TOUCH_NONE
        thisv->collider2.elements[0].info.toucher.dmgFlags = thisv->collider2.elements[0].info.toucher.damage = 0;
    }
    switch (thisv->actionState) {
        case 1:
            Animation_PlayOnce(&thisv->skelAnime, &object_dh_Anim_001A3C);
            thisv->actionState++;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_BITE);
        case 0:
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, 0x5DC, 0);
            break;
        case 2:
            if (thisv->skelAnime.curFrame >= 4.0f) {
                thisv->collider2.base.atFlags = thisv->collider2.elements[0].info.toucherFlags =
                    AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
                thisv->collider2.elements[0].info.toucher.dmgFlags = 0xFFCFFFFF;
                thisv->collider2.elements[0].info.toucher.damage = 8;
            }
            if (thisv->collider2.base.atFlags & AT_BOUNCED) {
                thisv->collider2.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
                thisv->collider2.base.atFlags = thisv->collider2.elements[0].info.toucherFlags =
                    AT_NONE; // also TOUCH_NONE
                thisv->collider2.elements[0].info.toucher.dmgFlags = thisv->collider2.elements[0].info.toucher.damage = 0;
                thisv->actionState++;
            } else if (thisv->collider2.base.atFlags & AT_HIT) {
                thisv->collider2.base.atFlags &= ~AT_HIT;
                func_8002F71C(globalCtx, &thisv->actor, 8.0f, thisv->actor.shape.rot.y, 8.0f);
            }
            break;
        case 3:
            if ((thisv->actor.xzDistToPlayer <= 100.0f) && (Actor_IsFacingPlayer(&thisv->actor, 60 * 0x10000 / 360))) {
                Animation_Change(&thisv->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f,
                                 Animation_GetLastFrame(&object_dh_Anim_004658), ANIMMODE_ONCE, -6.0f);
                thisv->actionState = 0;
            } else {
                Animation_Change(&thisv->skelAnime, &object_dh_Anim_004658, -1.0f,
                                 Animation_GetLastFrame(&object_dh_Anim_004658), 0.0f, ANIMMODE_ONCE, -4.0f);
                thisv->actionState++;
                thisv->collider2.base.atFlags = thisv->collider2.elements[0].info.toucherFlags =
                    AT_NONE; // also TOUCH_NONE
                thisv->collider2.elements[0].info.toucher.dmgFlags = thisv->collider2.elements[0].info.toucher.damage = 0;
            }
            break;
        case 5:
            EnDh_SetupWalk(thisv);
            break;
        case 4:
            break;
    }
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
}

void EnDh_SetupBurrow(EnDh* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_dh_Anim_002148, -6.0f);
    thisv->curAction = DH_BURROW;
    thisv->dirtWaveSpread = thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->dirtWavePhase = 0;
    thisv->actionState = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_HIDE);
    EnDh_SetupAction(thisv, EnDh_Burrow);
}

void EnDh_Burrow(EnDh* thisv, GlobalContext* globalCtx) {
    switch (thisv->actionState) {
        case 0:
            thisv->actionState++;
            thisv->drawDirtWave++;
            thisv->collider1.base.atFlags = thisv->collider1.info.toucherFlags =
                AT_ON | AT_TYPE_ENEMY; // also TOUCH_ON | TOUCH_SFX_WOOD
            thisv->collider1.info.toucher.dmgFlags = 0xFFCFFFFF;
            thisv->collider1.info.toucher.damage = 4;
        case 1:
            thisv->dirtWavePhase += 0x47E;
            Math_SmoothStepToF(&thisv->dirtWaveSpread, 300.0f, 1.0f, 8.0f, 0.0f);
            thisv->dirtWaveHeight = Math_SinS(thisv->dirtWavePhase) * 55.0f;
            thisv->dirtWaveAlpha = (s16)(Math_SinS(thisv->dirtWavePhase) * 255.0f);
            EnDh_SpawnDebris(globalCtx, thisv, &thisv->actor.world.pos, thisv->dirtWaveSpread, 4, 2.05f, 1.2f);
            thisv->collider1.dim.radius = thisv->dirtWaveSpread * 0.6f;
            if (SkelAnime_Update(&thisv->skelAnime)) {
                thisv->actionState++;
            }
            break;
        case 2:
            thisv->drawDirtWave = false;
            thisv->collider1.dim.radius = 35;
            thisv->collider1.base.atFlags = thisv->collider1.info.toucherFlags = AT_NONE; // Also TOUCH_NONE
            thisv->collider1.info.toucher.dmgFlags = thisv->collider1.info.toucher.damage = 0;
            EnDh_SetupWait(thisv);
            break;
    }
}

void EnDh_SetupDamage(EnDh* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_dh_Anim_003D6C, -6.0f);
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = -1.0f;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_DAMAGE);
    thisv->curAction = DH_DAMAGE;
    EnDh_SetupAction(thisv, EnDh_Damage);
}

void EnDh_Damage(EnDh* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.speedXZ < 0.0f) {
        thisv->actor.speedXZ += 0.15f;
    }
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if (thisv->retreat) {
            EnDh_SetupRetreat(thisv, globalCtx);
        } else if ((thisv->actor.xzDistToPlayer <= 105.0f) && Actor_IsFacingPlayer(&thisv->actor, 60 * 0x10000 / 360)) {
            f32 frames = Animation_GetLastFrame(&object_dh_Anim_004658);

            EnDh_SetupAttack(thisv);
            Animation_Change(&thisv->skelAnime, &object_dh_Anim_004658, 1.0f, 20.0f, frames, ANIMMODE_ONCE, -6.0f);
        } else {
            EnDh_SetupWalk(thisv);
        }
        thisv->unk_258 = 255;
    }
}

void EnDh_SetupDeath(EnDh* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_dh_Anim_0032BC, -1.0f);
    thisv->curAction = DH_DEATH;
    thisv->timer = 300;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    func_800F5B58();
    thisv->actor.params = ENDH_DEATH;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEADHAND_DEAD);
    EnDh_SetupAction(thisv, EnDh_Death);
}

void EnDh_Death(EnDh* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) || (thisv->timer != 300)) {
        if (thisv->timer == 300) {
            Animation_PlayLoop(&thisv->skelAnime, &object_dh_Anim_00375C);
        }
        thisv->timer--;
        if (thisv->timer < 150) {
            if (thisv->alpha != 0) {
                thisv->actor.scale.y -= 0.000075f;
                thisv->actor.shape.shadowAlpha = thisv->alpha -= 5;
            } else {
                Actor_Kill(&thisv->actor);
                return;
            }
        }
    } else {
        if (((s32)thisv->skelAnime.curFrame == 53) || ((s32)thisv->skelAnime.curFrame == 56) ||
            ((s32)thisv->skelAnime.curFrame == 61)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_RIZA_DOWN);
        }
        if ((s32)thisv->skelAnime.curFrame == 61) {
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
        }
    }
}

void EnDh_CollisionCheck(EnDh* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 lastHealth;

    if ((thisv->collider2.base.acFlags & AC_HIT) && !thisv->retreat) {
        thisv->collider2.base.acFlags &= ~AC_HIT;
        if ((thisv->actor.colChkInfo.damageEffect != 0) && (thisv->actor.colChkInfo.damageEffect != 6)) {
            thisv->collider2.base.atFlags = thisv->collider2.elements[0].info.toucherFlags = AT_NONE; // also TOUCH_NONE
            thisv->collider2.elements[0].info.toucher.dmgFlags = thisv->collider2.elements[0].info.toucher.damage = 0;
            if (player->unk_844 != 0) {
                thisv->unk_258 = player->unk_845;
            }
            Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
            lastHealth = thisv->actor.colChkInfo.health;
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                EnDh_SetupDeath(thisv);
                Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x90);
            } else {
                if (((lastHealth >= 15) && (thisv->actor.colChkInfo.health < 15)) ||
                    ((lastHealth >= 9) && (thisv->actor.colChkInfo.health < 9)) ||
                    ((lastHealth >= 3) && (thisv->actor.colChkInfo.health < 3))) {

                    thisv->retreat++;
                }
                EnDh_SetupDamage(thisv);
            }
        }
    }
}

void EnDh_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDh* thisv = (EnDh*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad40;

    EnDh_CollisionCheck(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 45.0f, 45.0f, 0x1D);
    thisv->actor.focus.pos = thisv->headPos;
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider1);
    if (thisv->actor.colChkInfo.health > 0) {
        if (thisv->curAction == DH_WAIT) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        } else {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        }
        if (((thisv->curAction != DH_DAMAGE) && (thisv->actor.shape.yOffset == 0.0f)) ||
            ((player->unk_844 != 0) && (player->unk_845 != thisv->unk_258))) {

            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        }
    } else {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
    }
}

void EnDh_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Vec3f headOffset = { 2000.0f, 1000.0f, 0.0f };
    EnDh* thisv = (EnDh*)thisx;

    if (limbIndex == 13) {
        Matrix_MultVec3f(&headOffset, &thisv->headPos);
        Matrix_Push();
        Matrix_Translate(headOffset.x, headOffset.y, headOffset.z, MTXMODE_APPLY);
        Collider_UpdateSpheres(1, &thisv->collider2);
        Matrix_Pop();
    }
}

void EnDh_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDh* thisv = (EnDh*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dh.c", 1099);
    if (thisv->alpha == 255) {
        func_80093D18(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, thisv->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        POLY_OPA_DISP =
            SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                               thisv->skelAnime.dListCount, NULL, EnDh_PostLimbDraw, &thisv->actor, POLY_OPA_DISP);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, NULL, NULL, &thisv->actor, POLY_XLU_DISP);
    }
    if (thisv->drawDirtWave) {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 85, 55, 0, 130);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->state.frames * -3) % 0x80, 0, 0x20, 0x40, 1,
                                    (globalCtx->state.frames * -10) % 0x80, (globalCtx->state.frames * -20) % 0x100,
                                    0x20, 0x40));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, thisv->dirtWaveAlpha);

        Matrix_Translate(0.0f, -thisv->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(thisv->dirtWaveSpread * 0.01f, thisv->dirtWaveHeight * 0.01f, thisv->dirtWaveSpread * 0.01f,
                     MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dh.c", 1160),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, object_dh_DL_007FC0);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dh.c", 1166);
}
