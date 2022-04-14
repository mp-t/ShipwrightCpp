/*
 * File: z_en_rr.c
 * Overlay: ovl_En_Rr
 * Description: Like Like
 */

#include "z_en_rr.h"
#include "objects/object_rr/object_rr.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_10)

#define RR_MESSAGE_SHIELD (1 << 0)
#define RR_MESSAGE_TUNIC (1 << 1)
#define RR_MOUTH 4
#define RR_BASE 0

typedef enum {
    /* 0 */ REACH_NONE,
    /* 1 */ REACH_EXTEND,
    /* 2 */ REACH_STOP,
    /* 3 */ REACH_OPEN,
    /* 4 */ REACH_GAPE,
    /* 5 */ REACH_CLOSE
} EnRrReachState;

typedef enum {
    /* 0x0 */ RR_DMG_NONE,
    /* 0x1 */ RR_DMG_STUN,
    /* 0x2 */ RR_DMG_FIRE,
    /* 0x3 */ RR_DMG_ICE,
    /* 0x4 */ RR_DMG_LIGHT_MAGIC,
    /* 0xB */ RR_DMG_LIGHT_ARROW = 11,
    /* 0xC */ RR_DMG_SHDW_ARROW,
    /* 0xD */ RR_DMG_WIND_ARROW,
    /* 0xE */ RR_DMG_SPRT_ARROW,
    /* 0xF */ RR_DMG_NORMAL
} EnRrDamageEffect;

typedef enum {
    /* 0 */ RR_DROP_RANDOM_RUPEE,
    /* 1 */ RR_DROP_MAGIC,
    /* 2 */ RR_DROP_ARROW,
    /* 3 */ RR_DROP_FLEXIBLE,
    /* 4 */ RR_DROP_RUPEE_PURPLE,
    /* 5 */ RR_DROP_RUPEE_RED
} EnRrDropType;

void EnRr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnRr_InitBodySegments(EnRr* thisv, GlobalContext* globalCtx);

void EnRr_SetupDamage(EnRr* thisv);
void EnRr_SetupDeath(EnRr* thisv);

void EnRr_Approach(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_Reach(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_GrabPlayer(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_Damage(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_Death(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_Retreat(EnRr* thisv, GlobalContext* globalCtx);
void EnRr_Stunned(EnRr* thisv, GlobalContext* globalCtx);

const ActorInit En_Rr_InitVars = {
    ACTOR_EN_RR,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_RR,
    sizeof(EnRr),
    (ActorFunc)EnRr_Init,
    (ActorFunc)EnRr_Destroy,
    (ActorFunc)EnRr_Update,
    (ActorFunc)EnRr_Draw,
    NULL,
};

static char* sDropNames[] = {
    // "type 7", "small magic jar", "arrow", "fairy", "20 rupees", "50 rupees"
    "タイプ７  ", "魔法の壷小", "矢        ", "妖精      ", "20ルピー  ", "50ルピー  ",
};

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 55, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 20, -10, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Deku stick    */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, RR_DMG_ICE),
    /* Light arrow   */ DMG_ENTRY(15, RR_DMG_LIGHT_ARROW),
    /* Unk arrow 1   */ DMG_ENTRY(4, RR_DMG_WIND_ARROW),
    /* Unk arrow 2   */ DMG_ENTRY(15, RR_DMG_SHDW_ARROW),
    /* Unk arrow 3   */ DMG_ENTRY(15, RR_DMG_SPRT_ARROW),
    /* Fire magic    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice magic     */ DMG_ENTRY(3, RR_DMG_ICE),
    /* Light magic   */ DMG_ENTRY(10, RR_DMG_LIGHT_MAGIC),
    /* Shield        */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, RR_DMG_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(10, RR_DMG_SPRT_ARROW),
    /* Unblockable   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, RR_DMG_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x37, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnRr_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnRr* thisv = (EnRr*)thisx;
    s32 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    thisv->actor.colChkInfo.health = 4;
    Collider_InitCylinder(globalCtx, &thisv->collider1);
    Collider_SetCylinderType1(globalCtx, &thisv->collider1, &thisv->actor, &sCylinderInit1);
    Collider_InitCylinder(globalCtx, &thisv->collider2);
    Collider_SetCylinderType1(globalCtx, &thisv->collider2, &thisv->actor, &sCylinderInit2);
    Actor_SetFocus(&thisv->actor, 30.0f);
    thisv->actor.scale.y = 0.013f;
    thisv->actor.scale.x = thisv->actor.scale.z = 0.014f;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.velocity.y = thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = -0.4f;
    thisv->actionTimer = 0;
    thisv->eatenShield = 0;
    thisv->eatenTunic = 0;
    thisv->retreat = false;
    thisv->grabTimer = 0;
    thisv->invincibilityTimer = 0;
    thisv->effectTimer = 0;
    thisv->hasPlayer = false;
    thisv->stopScroll = false;
    thisv->ocTimer = 0;
    thisv->reachState = thisv->isDead = false;
    thisv->actionFunc = EnRr_Approach;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].height = thisv->bodySegs[i].heightTarget = thisv->bodySegs[i].scaleMod.x =
            thisv->bodySegs[i].scaleMod.y = thisv->bodySegs[i].scaleMod.z = 0.0f;
    }
    EnRr_InitBodySegments(thisv, globalCtx);
}

void EnRr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRr* thisv = (EnRr*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider1);
    Collider_DestroyCylinder(globalCtx, &thisv->collider2);
}

void EnRr_SetSpeed(EnRr* thisv, f32 speed) {
    thisv->actor.speedXZ = speed;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_WALK);
}

void EnRr_SetupReach(EnRr* thisv) {
    static f32 segmentHeights[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f };
    s32 i;

    thisv->reachState = 1;
    thisv->actionTimer = 20;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = segmentHeights[i];
        thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z = 0.8f;
        thisv->bodySegs[i].rotTarget.x = 6000.0f;
        thisv->bodySegs[i].rotTarget.z = 0.0f;
    }
    thisv->actionFunc = EnRr_Reach;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_UNARI);
}

void EnRr_SetupNeutral(EnRr* thisv) {
    s32 i;

    thisv->reachState = 0;
    thisv->segMoveRate = 0.0f;
    thisv->segPhaseVelTarget = 2500.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = 0.0f;
        thisv->bodySegs[i].rotTarget.x = thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    if (thisv->retreat) {
        thisv->actionTimer = 100;
        thisv->actionFunc = EnRr_Retreat;
    } else {
        thisv->actionTimer = 60;
        thisv->actionFunc = EnRr_Approach;
    }
}

void EnRr_SetupGrabPlayer(EnRr* thisv, Player* player) {
    s32 i;

    thisv->grabTimer = 100;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->ocTimer = 8;
    thisv->hasPlayer = true;
    thisv->reachState = 0;
    thisv->segMoveRate = thisv->swallowOffset = thisv->actor.speedXZ = 0.0f;
    thisv->pulseSizeTarget = 0.15f;
    thisv->segPhaseVelTarget = 5000.0f;
    thisv->wobbleSizeTarget = 512.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = 0.0f;
        thisv->bodySegs[i].rotTarget.x = thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    thisv->actionFunc = EnRr_GrabPlayer;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_DRINK);
}

u8 EnRr_GetMessage(u8 shield, u8 tunic) {
    u8 messageIndex = 0;

    if ((shield == 1 /* Deku shield */) || (shield == 2 /* Hylian shield */)) {
        messageIndex = RR_MESSAGE_SHIELD;
    }
    if ((tunic == 2 /* Goron tunic */) || (tunic == 3 /* Zora tunic */)) {
        messageIndex |= RR_MESSAGE_TUNIC;
    }

    return messageIndex;
}

void EnRr_SetupReleasePlayer(EnRr* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u8 shield;
    u8 tunic;

    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->hasPlayer = false;
    thisv->ocTimer = 110;
    thisv->segMoveRate = 0.0f;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->wobbleSizeTarget = 2048.0f;
    tunic = 0;
    shield = 0;
    if (CUR_EQUIP_VALUE(EQUIP_SHIELD) != 3 /* Mirror shield */) {
        shield = Inventory_DeleteEquipment(globalCtx, EQUIP_SHIELD);
        if (shield != 0) {
            thisv->eatenShield = shield;
            thisv->retreat = true;
        }
    }
    if (CUR_EQUIP_VALUE(EQUIP_TUNIC) != 1 /* Kokiri tunic */) {
        tunic = Inventory_DeleteEquipment(globalCtx, EQUIP_TUNIC);
        if (tunic != 0) {
            thisv->eatenTunic = tunic;
            thisv->retreat = true;
        }
    }
    player->actor.parent = NULL;
    switch (EnRr_GetMessage(shield, tunic)) {
        case RR_MESSAGE_SHIELD:
            Message_StartTextbox(globalCtx, 0x305F, NULL);
            break;
        case RR_MESSAGE_TUNIC:
            Message_StartTextbox(globalCtx, 0x3060, NULL);
            break;
        case RR_MESSAGE_TUNIC | RR_MESSAGE_SHIELD:
            Message_StartTextbox(globalCtx, 0x3061, NULL);
            break;
    }
    osSyncPrintf(VT_FGCOL(YELLOW) "%s[%d] : Rr_Catch_Cancel" VT_RST "\n", "../z_en_rr.c", 650);
    func_8002F6D4(globalCtx, &thisv->actor, 4.0f, thisv->actor.shape.rot.y, 12.0f, 8);
    if (thisv->actor.colorFilterTimer == 0) {
        thisv->actionFunc = EnRr_Approach;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_THROW);
    } else if (thisv->actor.colChkInfo.health != 0) {
        EnRr_SetupDamage(thisv);
    } else {
        EnRr_SetupDeath(thisv);
    }
}

void EnRr_SetupDamage(EnRr* thisv) {
    s32 i;

    thisv->reachState = 0;
    thisv->actionTimer = 20;
    thisv->segMoveRate = 0.0f;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->pulseSizeTarget = 0.0f;
    thisv->wobbleSizeTarget = 0.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = 0.0f;
        thisv->bodySegs[i].rotTarget.x = thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    thisv->actionFunc = EnRr_Damage;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_DAMAGE);
}

void EnRr_SetupApproach(EnRr* thisv) {
    s32 i;

    thisv->segMoveRate = 0.0f;
    thisv->pulseSizeTarget = 0.15f;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = 0.0f;
        thisv->bodySegs[i].rotTarget.x = thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    thisv->actionFunc = EnRr_Approach;
}

void EnRr_SetupDeath(EnRr* thisv) {
    s32 i;

    thisv->isDead = true;
    thisv->frameCount = 0;
    thisv->shrinkRate = 0.0f;
    thisv->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].heightTarget = 0.0f;
        thisv->bodySegs[i].rotTarget.x = thisv->bodySegs[i].rotTarget.z = 0.0f;
    }
    thisv->actionFunc = EnRr_Death;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_DEAD);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void EnRr_SetupStunned(EnRr* thisv) {
    s32 i;

    thisv->stopScroll = true;
    thisv->segMovePhase = 0;
    thisv->segPhaseVel = 0.0f;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->segPulsePhaseDiff = 0.0f;
    thisv->segWobblePhaseDiffX = 0.0f;
    thisv->segWobbleXTarget = 3.0f;
    thisv->segWobblePhaseDiffZ = 0.0f;
    thisv->segWobbleZTarget = 1.0f;
    thisv->pulseSize = 0.0f;
    thisv->pulseSizeTarget = 0.15f;
    thisv->wobbleSize = 0.0f;
    thisv->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].scaleMod.y = 0.0f;
        thisv->bodySegs[i].rotTarget.x = 0.0f;
        thisv->bodySegs[i].rotTarget.y = 0.0f;
        thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scale.x = thisv->bodySegs[i].scale.y = thisv->bodySegs[i].scale.z =
            thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.y = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    thisv->actionFunc = EnRr_Stunned;
}

void EnRr_CollisionCheck(EnRr* thisv, GlobalContext* globalCtx) {
    Vec3f hitPos;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->collider2.base.acFlags & AC_HIT) {
        thisv->collider2.base.acFlags &= ~AC_HIT;
        // "Kakin" (not sure what thisv means)
        osSyncPrintf(VT_FGCOL(GREEN) "カキン(%d)！！" VT_RST "\n", thisv->frameCount);
        hitPos.x = thisv->collider2.info.bumper.hitPos.x;
        hitPos.y = thisv->collider2.info.bumper.hitPos.y;
        hitPos.z = thisv->collider2.info.bumper.hitPos.z;
        CollisionCheck_SpawnShieldParticlesMetal2(globalCtx, &hitPos);
    } else {
        if (thisv->collider1.base.acFlags & AC_HIT) {
            u8 dropType = RR_DROP_RANDOM_RUPEE;

            thisv->collider1.base.acFlags &= ~AC_HIT;
            if (thisv->actor.colChkInfo.damageEffect != 0) {
                hitPos.x = thisv->collider1.info.bumper.hitPos.x;
                hitPos.y = thisv->collider1.info.bumper.hitPos.y;
                hitPos.z = thisv->collider1.info.bumper.hitPos.z;
                CollisionCheck_BlueBlood(globalCtx, NULL, &hitPos);
            }
            switch (thisv->actor.colChkInfo.damageEffect) {
                case RR_DMG_LIGHT_ARROW:
                    dropType++; // purple rupee
                case RR_DMG_SHDW_ARROW:
                    dropType++; // flexible
                case RR_DMG_WIND_ARROW:
                    dropType++; // arrow
                case RR_DMG_SPRT_ARROW:
                    dropType++; // magic jar
                case RR_DMG_NORMAL:
                    // "ouch"
                    osSyncPrintf(VT_FGCOL(RED) "いてっ( %d : LIFE %d : DAMAGE %d : %x )！！" VT_RST "\n",
                                 thisv->frameCount, thisv->actor.colChkInfo.health, thisv->actor.colChkInfo.damage,
                                 thisv->actor.colChkInfo.damageEffect);
                    thisv->stopScroll = false;
                    Actor_ApplyDamage(&thisv->actor);
                    thisv->invincibilityTimer = 40;
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, thisv->invincibilityTimer);
                    if (thisv->hasPlayer) {
                        EnRr_SetupReleasePlayer(thisv, globalCtx);
                    } else if (thisv->actor.colChkInfo.health != 0) {
                        EnRr_SetupDamage(thisv);
                    } else {
                        thisv->dropType = dropType;
                        EnRr_SetupDeath(thisv);
                    }
                    return;
                case RR_DMG_FIRE: // Fire Arrow and Din's Fire
                    Actor_ApplyDamage(&thisv->actor);
                    if (thisv->actor.colChkInfo.health == 0) {
                        thisv->dropType = RR_DROP_RANDOM_RUPEE;
                    }
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                    thisv->effectTimer = 20;
                    EnRr_SetupStunned(thisv);
                    return;
                case RR_DMG_ICE: // Ice Arrow and unused ice magic
                    Actor_ApplyDamage(&thisv->actor);
                    if (thisv->actor.colChkInfo.health == 0) {
                        thisv->dropType = RR_DROP_RANDOM_RUPEE;
                    }
                    if (thisv->actor.colorFilterTimer == 0) {
                        thisv->effectTimer = 20;
                        Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0x2000, 0x50);
                    }
                    EnRr_SetupStunned(thisv);
                    return;
                case RR_DMG_LIGHT_MAGIC: // Unused light magic
                    Actor_ApplyDamage(&thisv->actor);
                    if (thisv->actor.colChkInfo.health == 0) {
                        thisv->dropType = RR_DROP_RUPEE_RED;
                    }
                    Actor_SetColorFilter(&thisv->actor, -0x8000, 0xFF, 0x2000, 0x50);
                    EnRr_SetupStunned(thisv);
                    return;
                case RR_DMG_STUN: // Boomerang and Hookshot
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0x2000, 0x50);
                    EnRr_SetupStunned(thisv);
                    return;
            }
        }
        if ((thisv->ocTimer == 0) && (thisv->actor.colorFilterTimer == 0) && (player->invincibilityTimer == 0) &&
            !(player->stateFlags2 & 0x80) &&
            ((thisv->collider1.base.ocFlags1 & OC1_HIT) || (thisv->collider2.base.ocFlags1 & OC1_HIT))) {
            thisv->collider1.base.ocFlags1 &= ~OC1_HIT;
            thisv->collider2.base.ocFlags1 &= ~OC1_HIT;
            // "catch"
            osSyncPrintf(VT_FGCOL(GREEN) "キャッチ(%d)！！" VT_RST "\n", thisv->frameCount);
            if (globalCtx->grabPlayer(globalCtx, player)) {
                player->actor.parent = &thisv->actor;
                thisv->stopScroll = false;
                EnRr_SetupGrabPlayer(thisv, player);
            }
        }
    }
}

void EnRr_InitBodySegments(EnRr* thisv, GlobalContext* globalCtx) {
    s32 i;

    thisv->segMovePhase = 0;
    thisv->segPhaseVel = 0.0f;
    thisv->segPhaseVelTarget = 2500.0f;
    thisv->segPulsePhaseDiff = 0.0f;
    thisv->segWobblePhaseDiffX = 0.0f;
    thisv->segWobbleXTarget = 3.0f;
    thisv->segWobblePhaseDiffZ = 0.0f;
    thisv->segWobbleZTarget = 1.0f;
    thisv->pulseSize = 0.0f;
    thisv->pulseSizeTarget = 0.15f;
    thisv->wobbleSize = 0.0f;
    thisv->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].scaleMod.y = 0.0f;
        thisv->bodySegs[i].rotTarget.x = 0.0f;
        thisv->bodySegs[i].rotTarget.y = 0.0f;
        thisv->bodySegs[i].rotTarget.z = 0.0f;
        thisv->bodySegs[i].scale.x = thisv->bodySegs[i].scale.y = thisv->bodySegs[i].scale.z =
            thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.y = thisv->bodySegs[i].scaleTarget.z = 1.0f;
    }
    for (i = 0; i < 5; i++) {
        thisv->bodySegs[i].scaleMod.x = thisv->bodySegs[i].scaleMod.z =
            Math_CosS(i * (u32)(s16)thisv->segPulsePhaseDiff * 0x1000) * thisv->pulseSize;
    }
    for (i = 1; i < 5; i++) {
        thisv->bodySegs[i].rotTarget.x = Math_CosS(i * (u32)(s16)thisv->segWobblePhaseDiffX * 0x1000) * thisv->wobbleSize;
        thisv->bodySegs[i].rotTarget.z = Math_SinS(i * (u32)(s16)thisv->segWobblePhaseDiffZ * 0x1000) * thisv->wobbleSize;
    }
}

void EnRr_UpdateBodySegments(EnRr* thisv, GlobalContext* globalCtx) {
    s32 i;
    s16 phase = thisv->segMovePhase;

    if (!thisv->isDead) {
        for (i = 0; i < 5; i++) {
            thisv->bodySegs[i].scaleMod.x = thisv->bodySegs[i].scaleMod.z =
                Math_CosS(phase + i * (s16)thisv->segPulsePhaseDiff * 0x1000) * thisv->pulseSize;
        }
        phase = thisv->segMovePhase;
        if (!thisv->isDead && (thisv->reachState == 0)) {
            for (i = 1; i < 5; i++) {
                thisv->bodySegs[i].rotTarget.x =
                    Math_CosS(phase + i * (s16)thisv->segWobblePhaseDiffX * 0x1000) * thisv->wobbleSize;
                thisv->bodySegs[i].rotTarget.z =
                    Math_SinS(phase + i * (s16)thisv->segWobblePhaseDiffZ * 0x1000) * thisv->wobbleSize;
            }
        }
    }
    if (!thisv->stopScroll) {
        thisv->segMovePhase += (s16)thisv->segPhaseVel;
    }
}

void EnRr_Approach(EnRr* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if ((thisv->actionTimer == 0) && (thisv->actor.xzDistToPlayer < 160.0f)) {
        EnRr_SetupReach(thisv);
    } else if ((thisv->actor.xzDistToPlayer < 400.0f) && (thisv->actor.speedXZ == 0.0f)) {
        EnRr_SetSpeed(thisv, 2.0f);
    }
}

void EnRr_Reach(EnRr* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    switch (thisv->reachState) {
        case REACH_EXTEND:
            if (thisv->actionTimer == 0) {
                thisv->reachState = REACH_STOP;
            }
            break;
        case REACH_STOP:
            if (thisv->actionTimer == 0) {
                thisv->actionTimer = 5;
                thisv->bodySegs[RR_MOUTH].scaleTarget.x = thisv->bodySegs[RR_MOUTH].scaleTarget.z = 1.5f;
                thisv->reachState = REACH_OPEN;
            }
            break;
        case REACH_OPEN:
            if (thisv->actionTimer == 0) {
                thisv->actionTimer = 2;
                thisv->bodySegs[RR_MOUTH].heightTarget = 2000.0f;
                thisv->reachState = REACH_GAPE;
            }
            break;
        case REACH_GAPE:
            if (thisv->actionTimer == 0) {
                thisv->actionTimer = 20;
                thisv->bodySegs[RR_MOUTH].scaleTarget.x = thisv->bodySegs[RR_MOUTH].scaleTarget.z = 0.8f;
                thisv->reachState = REACH_CLOSE;
            }
            break;
        case REACH_CLOSE:
            if (thisv->actionTimer == 0) {
                EnRr_SetupNeutral(thisv);
            }
            break;
    }
}

void EnRr_GrabPlayer(EnRr* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    func_800AA000(thisv->actor.xyzDistToPlayerSq, 120, 2, 120);
    if ((thisv->frameCount % 8) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_LIKE_EAT);
    }
    thisv->ocTimer = 8;
    if ((thisv->grabTimer == 0) || !(player->stateFlags2 & 0x80)) {
        EnRr_SetupReleasePlayer(thisv, globalCtx);
    } else {
        Math_ApproachF(&player->actor.world.pos.x, thisv->mouthPos.x, 1.0f, 30.0f);
        Math_ApproachF(&player->actor.world.pos.y, thisv->mouthPos.y + thisv->swallowOffset, 1.0f, 30.0f);
        Math_ApproachF(&player->actor.world.pos.z, thisv->mouthPos.z, 1.0f, 30.0f);
        Math_ApproachF(&thisv->swallowOffset, -55.0f, 1.0f, 5.0f);
    }
}

void EnRr_Damage(EnRr* thisv, GlobalContext* globalCtx) {
    s32 i;

    if (thisv->actor.colorFilterTimer == 0) {
        EnRr_SetupApproach(thisv);
    } else if ((thisv->actor.colorFilterTimer & 8) != 0) {
        for (i = 1; i < 5; i++) {
            thisv->bodySegs[i].rotTarget.z = 5000.0f;
        }
    } else {
        for (i = 1; i < 5; i++) {
            thisv->bodySegs[i].rotTarget.z = -5000.0f;
        }
    }
}

void EnRr_Death(EnRr* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 i;

    if (thisv->frameCount < 40) {
        for (i = 0; i < 5; i++) {
            Math_ApproachF(&thisv->bodySegs[i].heightTarget, i + 59 - (thisv->frameCount * 25.0f), 1.0f, 50.0f);
            thisv->bodySegs[i].scaleTarget.x = thisv->bodySegs[i].scaleTarget.z =
                (SQ(4 - i) * (f32)thisv->frameCount * 0.003f) + 1.0f;
        }
    } else if (thisv->frameCount >= 95) {
        Vec3f dropPos;

        dropPos.x = thisv->actor.world.pos.x;
        dropPos.y = thisv->actor.world.pos.y;
        dropPos.z = thisv->actor.world.pos.z;
        switch (thisv->eatenShield) {
            case 1:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_SHIELD_DEKU);
                break;
            case 2:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_SHIELD_HYLIAN);
                break;
        }
        switch (thisv->eatenTunic) {
            case 2:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_TUNIC_GORON);
                break;
            case 3:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_TUNIC_ZORA);
                break;
        }
        // "dropped"
        osSyncPrintf(VT_FGCOL(GREEN) "「%s」が出た！！" VT_RST "\n", sDropNames[thisv->dropType]);
        switch (thisv->dropType) {
            case RR_DROP_MAGIC:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_MAGIC_SMALL);
                break;
            case RR_DROP_ARROW:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_ARROWS_SINGLE);
                break;
            case RR_DROP_FLEXIBLE:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_FLEXIBLE);
                break;
            case RR_DROP_RUPEE_PURPLE:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_RUPEE_PURPLE);
                break;
            case RR_DROP_RUPEE_RED:
                Item_DropCollectible(globalCtx, &dropPos, ITEM00_RUPEE_RED);
                break;
            case RR_DROP_RANDOM_RUPEE:
            default:
                Item_DropCollectibleRandom(globalCtx, &thisv->actor, &dropPos, 12 << 4);
                break;
        }
        Actor_Kill(&thisv->actor);
    } else if (thisv->frameCount == 88) {
        Vec3f pos;
        Vec3f vel;
        Vec3f accel;

        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + 20.0f;
        pos.z = thisv->actor.world.pos.z;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        accel.x = 0.0f;
        accel.y = 0.0f;
        accel.z = 0.0f;

        EffectSsDeadDb_Spawn(globalCtx, &pos, &vel, &accel, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
    } else {
        Math_ApproachF(&thisv->actor.scale.x, 0.0f, 1.0f, thisv->shrinkRate);
        Math_ApproachF(&thisv->shrinkRate, 0.001f, 1.0f, 0.00001f);
        thisv->actor.scale.z = thisv->actor.scale.x;
    }
}

void EnRr_Retreat(EnRr* thisv, GlobalContext* globalCtx) {
    if (thisv->actionTimer == 0) {
        thisv->retreat = false;
        thisv->actionFunc = EnRr_Approach;
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 0xA, 0x3E8, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if (thisv->actor.speedXZ == 0.0f) {
            EnRr_SetSpeed(thisv, 2.0f);
        }
    }
}

void EnRr_Stunned(EnRr* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.colorFilterTimer == 0) {
        thisv->stopScroll = false;
        if (thisv->hasPlayer) {
            EnRr_SetupReleasePlayer(thisv, globalCtx);
        } else if (thisv->actor.colChkInfo.health != 0) {
            thisv->actionFunc = EnRr_Approach;
        } else {
            EnRr_SetupDeath(thisv);
        }
    }
}

void EnRr_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRr* thisv = (EnRr*)thisx;
    s32 i;

    thisv->frameCount++;
    if (!thisv->stopScroll) {
        thisv->scrollTimer++;
    }
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->grabTimer != 0) {
        thisv->grabTimer--;
    }
    if (thisv->ocTimer != 0) {
        thisv->ocTimer--;
    }
    if (thisv->invincibilityTimer != 0) {
        thisv->invincibilityTimer--;
    }
    if (thisv->effectTimer != 0) {
        thisv->effectTimer--;
    }

    Actor_SetFocus(&thisv->actor, 30.0f);
    EnRr_UpdateBodySegments(thisv, globalCtx);
    if (!thisv->isDead && ((thisv->actor.colorFilterTimer == 0) || !(thisv->actor.colorFilterParams & 0x4000))) {
        EnRr_CollisionCheck(thisv, globalCtx);
    }

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->hasPlayer == 0x3F80) { // checks if 1.0f has been stored to hasPlayer's address
        ASSERT(0, "0", "../z_en_rr.c", 1355);
    }

    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.1f);
    Actor_MoveForward(&thisv->actor);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider1);
    thisv->collider2.dim.pos.x = thisv->mouthPos.x;
    thisv->collider2.dim.pos.y = thisv->mouthPos.y;
    thisv->collider2.dim.pos.z = thisv->mouthPos.z;
    if (!thisv->isDead && (thisv->invincibilityTimer == 0)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
        if (thisv->ocTimer == 0) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider1.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
    } else {
        thisv->collider2.base.ocFlags1 &= ~OC1_HIT;
        thisv->collider2.base.acFlags &= ~AC_HIT;
        thisv->collider1.base.ocFlags1 &= ~OC1_HIT;
        thisv->collider1.base.acFlags &= ~AC_HIT;
    }
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 30.0f, 20.0f, 7);
    if (!thisv->stopScroll) {
        Math_ApproachF(&thisv->segPhaseVel, thisv->segPhaseVelTarget, 1.0f, 50.0f);
        Math_ApproachF(&thisv->segPulsePhaseDiff, 4.0f, 1.0f, 5.0f);
        Math_ApproachF(&thisv->segWobblePhaseDiffX, thisv->segWobbleXTarget, 1.0f, 0.04f);
        Math_ApproachF(&thisv->segWobblePhaseDiffZ, thisv->segWobbleZTarget, 1.0f, 0.01f);
        Math_ApproachF(&thisv->pulseSize, thisv->pulseSizeTarget, 1.0f, 0.0015f);
        Math_ApproachF(&thisv->wobbleSize, thisv->wobbleSizeTarget, 1.0f, 20.0f);
        for (i = 0; i < 5; i++) {
            Math_SmoothStepToS(&thisv->bodySegs[i].rot.x, thisv->bodySegs[i].rotTarget.x, 5, thisv->segMoveRate * 1000.0f,
                               0);
            Math_SmoothStepToS(&thisv->bodySegs[i].rot.z, thisv->bodySegs[i].rotTarget.z, 5, thisv->segMoveRate * 1000.0f,
                               0);
            Math_ApproachF(&thisv->bodySegs[i].scale.x, thisv->bodySegs[i].scaleTarget.x, 1.0f, thisv->segMoveRate * 0.2f);
            thisv->bodySegs[i].scale.z = thisv->bodySegs[i].scale.x;
            Math_ApproachF(&thisv->bodySegs[i].height, thisv->bodySegs[i].heightTarget, 1.0f, thisv->segMoveRate * 300.0f);
        }
        Math_ApproachF(&thisv->segMoveRate, 1.0f, 1.0f, 0.2f);
    }
}

static Vec3f sEffectOffsets[] = {
    { 25.0f, 0.0f, 0.0f },
    { -25.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 25.0f },
    { 0.0f, 0.0f, -25.0f },
};

void EnRr_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f zeroVec;
    EnRr* thisv = (EnRr*)thisx;
    s32 i;
    Mtx* segMtx = Graph_Alloc(globalCtx->state.gfxCtx, 4 * sizeof(Mtx));

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_rr.c", 1478);
    if (1) {}
    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x0C, segMtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (thisv->scrollTimer * 0) & 0x7F,
                                (thisv->scrollTimer * 0) & 0x3F, 32, 16, 1, (thisv->scrollTimer * 0) & 0x3F,
                                (thisv->scrollTimer * -6) & 0x7F, 32, 16));
    Matrix_Push();

    Matrix_Scale((1.0f + thisv->bodySegs[RR_BASE].scaleMod.x) * thisv->bodySegs[RR_BASE].scale.x,
                 (1.0f + thisv->bodySegs[RR_BASE].scaleMod.y) * thisv->bodySegs[RR_BASE].scale.y,
                 (1.0f + thisv->bodySegs[RR_BASE].scaleMod.z) * thisv->bodySegs[RR_BASE].scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_rr.c", 1501),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Matrix_Pop();
    zeroVec.x = 0.0f;
    zeroVec.y = 0.0f;
    zeroVec.z = 0.0f;
    for (i = 1; i < 5; i++) {
        Matrix_Translate(0.0f, thisv->bodySegs[i].height + 1000.0f, 0.0f, MTXMODE_APPLY);

        Matrix_RotateZYX(thisv->bodySegs[i].rot.x, thisv->bodySegs[i].rot.y, thisv->bodySegs[i].rot.z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Scale((1.0f + thisv->bodySegs[i].scaleMod.x) * thisv->bodySegs[i].scale.x,
                     (1.0f + thisv->bodySegs[i].scaleMod.y) * thisv->bodySegs[i].scale.y,
                     (1.0f + thisv->bodySegs[i].scaleMod.z) * thisv->bodySegs[i].scale.z, MTXMODE_APPLY);
        Matrix_ToMtx(segMtx, "../z_en_rr.c", 1527);
        Matrix_Pop();
        segMtx++;
        Matrix_MultVec3f(&zeroVec, &thisv->effectPos[i]);
    }
    thisv->effectPos[0] = thisv->actor.world.pos;
    Matrix_MultVec3f(&zeroVec, &thisv->mouthPos);
    gSPDisplayList(POLY_XLU_DISP++, gLikeLikeDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_rr.c", 1551);
    if (thisv->effectTimer != 0) {
        Vec3f effectPos;
        s16 effectTimer = thisv->effectTimer - 1;

        thisv->actor.colorFilterTimer++;
        if ((effectTimer & 1) == 0) {
            s32 segIndex = 4 - (effectTimer >> 2);
            s32 offIndex = (effectTimer >> 1) & 3;

            effectPos.x = thisv->effectPos[segIndex].x + sEffectOffsets[offIndex].x + Rand_CenteredFloat(10.0f);
            effectPos.y = thisv->effectPos[segIndex].y + sEffectOffsets[offIndex].y + Rand_CenteredFloat(10.0f);
            effectPos.z = thisv->effectPos[segIndex].z + sEffectOffsets[offIndex].z + Rand_CenteredFloat(10.0f);
            if (thisv->actor.colorFilterParams & 0x4000) {
                EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &effectPos, 100, 0, 0, -1);
            } else {
                EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255,
                                               3.0f);
            }
        }
    }
}
