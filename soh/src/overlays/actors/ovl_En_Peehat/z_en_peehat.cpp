#include "z_en_peehat.h"
#include "objects/object_peehat/object_peehat.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_24)

#define GROUND_HOVER_HEIGHT 75.0f
#define MAX_LARVA 3

void EnPeehat_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPeehat_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPeehat_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPeehat_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnPeehat_Ground_SetStateGround(EnPeehat* thisv);
void EnPeehat_Flying_SetStateGround(EnPeehat* thisv);
void EnPeehat_Larva_SetStateSeekPlayer(EnPeehat* thisv);
void EnPeehat_Ground_StateGround(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_SetStateRise(EnPeehat* thisv);
void EnPeehat_Flying_StateGrounded(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Flying_SetStateRise(EnPeehat* thisv);
void EnPeehat_Flying_StateFly(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Flying_SetStateLanding(EnPeehat* thisv);
void EnPeehat_Ground_StateRise(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_SetStateHover(EnPeehat* thisv);
void EnPeehat_Flying_StateRise(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_StateSeekPlayer(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_SetStateReturnHome(EnPeehat* thisv);
void EnPeehat_Ground_SetStateLanding(EnPeehat* thisv);
void EnPeehat_Larva_StateSeekPlayer(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_SetStateAttackRecoil(EnPeehat* thisv);
void EnPeehat_Ground_StateLanding(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Flying_StateLanding(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_StateHover(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Ground_StateReturnHome(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_StateAttackRecoil(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_StateBoomerangStunned(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_Adult_StateDie(EnPeehat* thisv, GlobalContext* globalCtx);
void EnPeehat_SetStateExplode(EnPeehat* thisv);
void EnPeehat_StateExplode(EnPeehat* thisv, GlobalContext* globalCtx);

const ActorInit En_Peehat_InitVars = {
    ACTOR_EN_PEEHAT,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PEEHAT,
    sizeof(EnPeehat),
    (ActorFunc)EnPeehat_Init,
    (ActorFunc)EnPeehat_Destroy,
    (ActorFunc)EnPeehat_Update,
    (ActorFunc)EnPeehat_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_WOOD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 160, -70, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElemInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
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
    sJntSphElemInit,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 00 */ PEAHAT_DMG_EFF_ATTACK = 0,
    /* 06 */ PEAHAT_DMG_EFF_LIGHT_ICE_ARROW = 6,
    /* 12 */ PEAHAT_DMG_EFF_FIRE = 12,
    /* 13 */ PEAHAT_DMG_EFF_HOOKSHOT = 13,
    /* 14 */ PEAHAT_DMG_EFF_BOOMERANG = 14,
    /* 15 */ PEAHAT_DMG_EFF_NUT = 15
} DamageEffect;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, PEAHAT_DMG_EFF_NUT),
    /* Deku stick    */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Slingshot     */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Explosive     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Boomerang     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer swing  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hookshot      */ DMG_ENTRY(2, PEAHAT_DMG_EFF_HOOKSHOT),
    /* Kokiri sword  */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Master sword  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant's Knife */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Fire arrow    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Light arrow   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 1   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 2   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 3   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Fire magic    */ DMG_ENTRY(3, PEAHAT_DMG_EFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Light magic   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Shield        */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Mirror Ray    */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri spin   */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Giant spin    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Master spin   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri jump   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant jump    */ DMG_ENTRY(8, PEAHAT_DMG_EFF_ATTACK),
    /* Master jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 1     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Unblockable   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 2     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
};

typedef enum {
    /* 00 */ PEAHAT_STATE_DYING,
    /* 01 */ PEAHAT_STATE_EXPLODE,
    /* 03 */ PEAHAT_STATE_3 = 3,
    /* 04 */ PEAHAT_STATE_4,
    /* 05 */ PEAHAT_STATE_FLY,
    /* 07 */ PEAHAT_STATE_ATTACK_RECOIL = 7,
    /* 08 */ PEAHAT_STATE_8,
    /* 09 */ PEAHAT_STATE_9,
    /* 10 */ PEAHAT_STATE_LANDING,
    /* 12 */ PEAHAT_STATE_RETURN_HOME = 12,
    /* 13 */ PEAHAT_STATE_STUNNED,
    /* 14 */ PEAHAT_STATE_SEEK_PLAYER,
    /* 15 */ PEAHAT_STATE_15
} PeahatState;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 700, ICHAIN_STOP),
};

void EnPeehat_SetupAction(EnPeehat* thisv, EnPeehatActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnPeehat_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnPeehat* thisv = (EnPeehat*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 36.0f * 0.001f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPeehatSkel, &gPeehatRisingAnim, thisv->jointTable, thisv->morphTable,
                   24);
    ActorShape_Init(&thisv->actor.shape, 100.0f, ActorShadow_DrawCircle, 27.0f);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->unk2D4 = 0;
    thisv->actor.world.rot.y = 0;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actor.colChkInfo.health = 6;
    thisv->actor.colChkInfo.damageTable = &sDamageTable;
    thisv->actor.floorHeight = thisv->actor.world.pos.y;
    Collider_InitCylinder(globalCtx, &thisv->colCylinder);
    Collider_SetCylinder(globalCtx, &thisv->colCylinder, &thisv->actor, &sCylinderInit);
    Collider_InitQuad(globalCtx, &thisv->colQuad);
    Collider_SetQuad(globalCtx, &thisv->colQuad, &thisv->actor, &sQuadInit);
    Collider_InitJntSph(globalCtx, &thisv->colJntSph);
    Collider_SetJntSph(globalCtx, &thisv->colJntSph, &thisv->actor, &sJntSphInit, thisv->colJntSphItemList);

    thisv->actor.naviEnemyId = 0x48;
    thisv->xzDistToRise = 740.0f;
    thisv->xzDistMax = 1200.0f;
    thisv->actor.uncullZoneForward = 4000.0f;
    thisv->actor.uncullZoneScale = 800.0f;
    thisv->actor.uncullZoneDownward = 1800.0f;
    switch (thisv->actor.params) {
        case PEAHAT_TYPE_GROUNDED:
            EnPeehat_Ground_SetStateGround(thisv);
            break;
        case PEAHAT_TYPE_FLYING:
            thisv->actor.uncullZoneForward = 4200.0f;
            thisv->xzDistToRise = 2800.0f;
            thisv->xzDistMax = 1400.0f;
            EnPeehat_Flying_SetStateGround(thisv);
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            break;
        case PEAHAT_TYPE_LARVA:
            thisv->actor.scale.x = thisv->actor.scale.z = 0.006f;
            thisv->actor.scale.y = 0.003f;
            thisv->colCylinder.dim.radius = 25;
            thisv->colCylinder.dim.height = 15;
            thisv->colCylinder.dim.yShift = -5;
            thisv->colCylinder.info.bumper.dmgFlags = 0x1F824;
            thisv->colQuad.base.atFlags = AT_ON | AT_TYPE_ENEMY;
            thisv->colQuad.base.acFlags = AC_ON | AC_TYPE_PLAYER;
            thisv->actor.naviEnemyId = 0x49; // Larva
            EnPeehat_Larva_SetStateSeekPlayer(thisv);
            break;
    }
}

void EnPeehat_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPeehat* thisv = (EnPeehat*)thisx;
    EnPeehat* parent;

    Collider_DestroyCylinder(globalCtx, &thisv->colCylinder);
    Collider_DestroyJntSph(globalCtx, &thisv->colJntSph);

    // If PEAHAT_TYPE_LARVA, decrement total larva spawned
    if (thisv->actor.params > 0) {
        parent = (EnPeehat*)thisv->actor.parent;
        if (parent != NULL && parent->actor.update != NULL) {
            parent->unk2FA--;
        }
    }
}

void EnPeehat_SpawnDust(GlobalContext* globalCtx, EnPeehat* thisv, Vec3f* pos, f32 arg3, s32 arg4, f32 arg5, f32 arg6) {
    Vec3f dustPos;
    Vec3f dustVel = { 0.0f, 8.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, -1.5f, 0.0f };
    f32 rot; // radians
    s32 pScale;

    rot = (Rand_ZeroOne() - 0.5f) * 6.28f;
    dustPos.y = thisv->actor.floorHeight;
    dustPos.x = Math_SinF(rot) * arg3 + pos->x;
    dustPos.z = Math_CosF(rot) * arg3 + pos->z;
    dustAccel.x = (Rand_ZeroOne() - 0.5f) * arg5;
    dustAccel.z = (Rand_ZeroOne() - 0.5f) * arg5;
    dustVel.y += (Rand_ZeroOne() - 0.5f) * 4.0f;
    pScale = (Rand_ZeroOne() * 5 + 12) * arg6;
    EffectSsHahen_Spawn(globalCtx, &dustPos, &dustVel, &dustAccel, arg4, pScale, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

/**
 * Handles being hit when on the ground
 */
void EnPeehat_HitWhenGrounded(EnPeehat* thisv, GlobalContext* globalCtx) {
    thisv->colCylinder.base.acFlags &= ~AC_HIT;
    if ((globalCtx->gameplayFrames & 0xF) == 0) {
        Vec3f itemDropPos = thisv->actor.world.pos;

        itemDropPos.y += 70.0f;
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &itemDropPos, 0x40);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &itemDropPos, 0x40);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &itemDropPos, 0x40);
        thisv->unk2D4 = 240;
    } else {
        s32 i;

        thisv->colCylinder.base.acFlags &= ~AC_HIT;
        for (i = MAX_LARVA - thisv->unk2FA; i > 0; i--) {
            Actor* larva =
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_PEEHAT,
                                   Rand_CenteredFloat(25.0f) + thisv->actor.world.pos.x,
                                   Rand_CenteredFloat(25.0f) + (thisv->actor.world.pos.y + 50.0f),
                                   Rand_CenteredFloat(25.0f) + thisv->actor.world.pos.z, 0, 0, 0, PEAHAT_TYPE_LARVA);

            if (larva != NULL) {
                larva->velocity.y = 6.0f;
                larva->shape.rot.y = larva->world.rot.y = Rand_CenteredFloat(0xFFFF);
                thisv->unk2FA++;
            }
        }
        thisv->unk2D4 = 8;
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_DAMAGE);
}

void EnPeehat_Ground_SetStateGround(EnPeehat* thisv) {
    Animation_Change(&thisv->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Animation_GetLastFrame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    thisv->seekPlayerTimer = 600;
    thisv->unk2D4 = 0;
    thisv->unk2FA = 0;
    thisv->state = PEAHAT_STATE_3;
    thisv->colCylinder.base.acFlags &= ~AC_HIT;
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateGround);
}

void EnPeehat_Ground_StateGround(EnPeehat* thisv, GlobalContext* globalCtx) {
    if (IS_DAY) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        if (thisv->riseDelayTimer == 0) {
            if (thisv->actor.xzDistToPlayer < thisv->xzDistToRise) {
                EnPeehat_Ground_SetStateRise(thisv);
            }
        } else {
            Math_SmoothStepToF(&thisv->actor.shape.yOffset, -1000.0f, 1.0f, 10.0f, 0.0f);
            thisv->riseDelayTimer--;
        }
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        Math_SmoothStepToF(&thisv->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (thisv->unk2D4 != 0) {
            thisv->unk2D4--;
            if (thisv->unk2D4 & 4) {
                Math_SmoothStepToF(&thisv->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                Math_SmoothStepToF(&thisv->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (thisv->colCylinder.base.acFlags & AC_HIT) {
            EnPeehat_HitWhenGrounded(thisv, globalCtx);
        }
    }
}

void EnPeehat_Flying_SetStateGround(EnPeehat* thisv) {
    Animation_Change(&thisv->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Animation_GetLastFrame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    thisv->seekPlayerTimer = 400;
    thisv->unk2D4 = 0;
    thisv->unk2FA = 0; //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
    thisv->state = PEAHAT_STATE_4;
    EnPeehat_SetupAction(thisv, EnPeehat_Flying_StateGrounded);
}

void EnPeehat_Flying_StateGrounded(EnPeehat* thisv, GlobalContext* globalCtx) {
    if (IS_DAY) {
        if (thisv->actor.xzDistToPlayer < thisv->xzDistToRise) {
            EnPeehat_Flying_SetStateRise(thisv);
        }
    } else {
        Math_SmoothStepToF(&thisv->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (thisv->unk2D4 != 0) {
            thisv->unk2D4--;
            if (thisv->unk2D4 & 4) {
                Math_SmoothStepToF(&thisv->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                Math_SmoothStepToF(&thisv->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (thisv->colCylinder.base.acFlags & AC_HIT) {
            EnPeehat_HitWhenGrounded(thisv, globalCtx);
        }
    }
}

void EnPeehat_Flying_SetStateFly(EnPeehat* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPeehatFlyingAnim);
    thisv->state = PEAHAT_STATE_FLY;
    EnPeehat_SetupAction(thisv, EnPeehat_Flying_StateFly);
}

void EnPeehat_Flying_StateFly(EnPeehat* thisv, GlobalContext* globalCtx) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
    SkelAnime_Update(&thisv->skelAnime);
    if (!IS_DAY || thisv->xzDistToRise < thisv->actor.xzDistToPlayer) {
        EnPeehat_Flying_SetStateLanding(thisv);
    } else if (thisv->actor.xzDistToPlayer < thisv->xzDistMax) {
        if (thisv->unk2FA < MAX_LARVA && (globalCtx->gameplayFrames & 7) == 0) {
            Actor* larva = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_PEEHAT,
                                              Rand_CenteredFloat(25.0f) + thisv->actor.world.pos.x,
                                              Rand_CenteredFloat(5.0f) + thisv->actor.world.pos.y,
                                              Rand_CenteredFloat(25.0f) + thisv->actor.world.pos.z, 0, 0, 0, 1);
            if (larva != NULL) {
                larva->shape.rot.y = larva->world.rot.y = Rand_CenteredFloat(0xFFFF);
                thisv->unk2FA++;
            }
        }
    }
    thisv->bladeRot += thisv->bladeRotVel;
}

void EnPeehat_Ground_SetStateRise(EnPeehat* thisv) {
    f32 lastFrame = Animation_GetLastFrame(&gPeehatRisingAnim);

    if (thisv->state != PEAHAT_STATE_STUNNED) {
        Animation_Change(&thisv->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    thisv->state = PEAHAT_STATE_8;
    thisv->animTimer = lastFrame;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_UP);
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateRise);
}

void EnPeehat_Ground_StateRise(EnPeehat* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (thisv->animTimer != 0) {
            thisv->animTimer--;
            if (thisv->skelAnime.playSpeed == 0.0f) {
                if (thisv->animTimer == 0) {
                    thisv->animTimer = 40;
                    thisv->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (SkelAnime_Update(&thisv->skelAnime) || thisv->animTimer == 0) {
            EnPeehat_Ground_SetStateHover(thisv);
        } else {
            thisv->actor.world.pos.y += 6.5f;
        }
        if (thisv->actor.world.pos.y - thisv->actor.floorHeight < 80.0f) {
            Vec3f pos = thisv->actor.world.pos;
            pos.y = thisv->actor.floorHeight;
            func_80033480(globalCtx, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    EnPeehat_SpawnDust(globalCtx, thisv, &thisv->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    Math_SmoothStepToF(&thisv->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    thisv->bladeRot += thisv->bladeRotVel;
}

void EnPeehat_Flying_SetStateRise(EnPeehat* thisv) {
    f32 lastFrame;

    lastFrame = Animation_GetLastFrame(&gPeehatRisingAnim);
    if (thisv->state != PEAHAT_STATE_STUNNED) {
        Animation_Change(&thisv->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    thisv->state = PEAHAT_STATE_9;
    thisv->animTimer = lastFrame;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_UP);
    EnPeehat_SetupAction(thisv, EnPeehat_Flying_StateRise);
}

void EnPeehat_Flying_StateRise(EnPeehat* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (thisv->animTimer != 0) {
            thisv->animTimer--;
            if (thisv->skelAnime.playSpeed == 0.0f) {
                if (thisv->animTimer == 0) {
                    thisv->animTimer = 40;
                    thisv->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (SkelAnime_Update(&thisv->skelAnime) || thisv->animTimer == 0) {
            //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
            thisv->unk2FA = 0;
            EnPeehat_Flying_SetStateFly(thisv);
        } else {
            thisv->actor.world.pos.y += 18.0f;
        }
        if (thisv->actor.world.pos.y - thisv->actor.floorHeight < 80.0f) {
            Vec3f pos = thisv->actor.world.pos;
            pos.y = thisv->actor.floorHeight;
            func_80033480(globalCtx, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    EnPeehat_SpawnDust(globalCtx, thisv, &thisv->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    Math_SmoothStepToF(&thisv->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    thisv->bladeRot += thisv->bladeRotVel;
}

void EnPeehat_Ground_SetStateSeekPlayer(EnPeehat* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPeehatFlyingAnim);
    thisv->state = PEAHAT_STATE_SEEK_PLAYER;
    thisv->unk2E0 = 0.0f;
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateSeekPlayer);
}

void EnPeehat_Ground_StateSeekPlayer(EnPeehat* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.speedXZ, 3.0f, 1.0f, 0.25f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 80.0f, 1.0f, 3.0f, 0.0f);
    if (thisv->seekPlayerTimer <= 0) {
        EnPeehat_Ground_SetStateLanding(thisv);
        thisv->riseDelayTimer = 40;
    } else {
        thisv->seekPlayerTimer--;
    }
    if (IS_DAY && (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < thisv->xzDistMax)) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 1000, 0);
        if (thisv->unk2FA != 0) {
            thisv->actor.shape.rot.y += 0x1C2;
        } else {
            thisv->actor.shape.rot.y -= 0x1C2;
        }
    } else {
        EnPeehat_Ground_SetStateReturnHome(thisv);
    }
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 500, 0);
    thisv->bladeRot += thisv->bladeRotVel;
    Math_SmoothStepToF(&thisv->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_Larva_SetStateSeekPlayer(EnPeehat* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPeehatFlyingAnim);
    thisv->state = PEAHAT_STATE_SEEK_PLAYER;
    thisv->unk2D4 = 0;
    EnPeehat_SetupAction(thisv, EnPeehat_Larva_StateSeekPlayer);
}

void EnPeehat_Larva_StateSeekPlayer(EnPeehat* thisv, GlobalContext* globalCtx) {
    f32 speedXZ = 5.3f;

    if (thisv->actor.xzDistToPlayer <= 5.3f) {
        speedXZ = thisv->actor.xzDistToPlayer + 0.0005f;
    }
    if (thisv->actor.parent != NULL && thisv->actor.parent->update == NULL) {
        thisv->actor.parent = NULL;
    }
    thisv->actor.speedXZ = speedXZ;
    if (thisv->actor.world.pos.y - thisv->actor.floorHeight >= 70.0f) {
        Math_SmoothStepToF(&thisv->actor.velocity.y, -1.3f, 1.0f, 0.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->actor.velocity.y, -0.135f, 1.0f, 0.05f, 0.0f);
    }
    if (thisv->unk2D4 == 0) {
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1, 830, 0);
    } else {
        thisv->unk2D4--;
    }
    thisv->actor.shape.rot.y += 0x15E;
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 500, 0);
    thisv->bladeRot += thisv->bladeRotVel;
    Math_SmoothStepToF(&thisv->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_SM_FLY - SFX_FLAG);
    if (thisv->colQuad.base.atFlags & AT_BOUNCED) {
        thisv->actor.colChkInfo.health = 0;
        thisv->colQuad.base.acFlags = thisv->colQuad.base.acFlags & ~AC_BOUNCED;
        EnPeehat_SetStateAttackRecoil(thisv);
    } else if ((thisv->colQuad.base.atFlags & AT_HIT) || (thisv->colCylinder.base.acFlags & AC_HIT) ||
               (thisv->actor.bgCheckFlags & 1)) {
        Player* player = GET_PLAYER(globalCtx);
        thisv->colQuad.base.atFlags &= ~AT_HIT;
        if (!(thisv->colCylinder.base.acFlags & AC_HIT) && &player->actor == thisv->colQuad.base.at) {
            if (Rand_ZeroOne() > 0.5f) {
                thisv->actor.world.rot.y += 0x2000;
            } else {
                thisv->actor.world.rot.y -= 0x2000;
            }
            thisv->unk2D4 = 40;
        } else if (thisv->colCylinder.base.acFlags & AC_HIT || thisv->actor.bgCheckFlags & 1) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
                pos.y = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
                EffectSsDeadDb_Spawn(globalCtx, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                                     1);
            }
        }
        if (&player->actor != thisv->colQuad.base.at || thisv->colCylinder.base.acFlags & AC_HIT) {
            if (!(thisv->actor.bgCheckFlags & 1)) {
                EffectSsDeadSound_SpawnStationary(globalCtx, &thisv->actor.projectedPos, NA_SE_EN_PIHAT_SM_DEAD, 1, 1,
                                                  40);
            }
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x20);
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnPeehat_Ground_SetStateLanding(EnPeehat* thisv) {
    thisv->state = PEAHAT_STATE_LANDING;
    Animation_PlayOnce(&thisv->skelAnime, &gPeehatLandingAnim);
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateLanding);
}

void EnPeehat_Ground_StateLanding(EnPeehat* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0, 1, 50, 0);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnPeehat_Ground_SetStateGround(thisv);
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_LAND);
    } else if (thisv->actor.floorHeight < thisv->actor.world.pos.y) {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 0.3f, 3.5f, 0.25f);
        if (thisv->actor.world.pos.y - thisv->actor.floorHeight < 60.0f) {
            Vec3f pos = thisv->actor.world.pos;
            pos.y = thisv->actor.floorHeight;
            func_80033480(globalCtx, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(globalCtx, thisv, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    Math_SmoothStepToS(&thisv->bladeRotVel, 0, 1, 100, 0);
    thisv->bladeRot += thisv->bladeRotVel;
}

void EnPeehat_Flying_SetStateLanding(EnPeehat* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gPeehatLandingAnim);
    thisv->state = PEAHAT_STATE_LANDING;
    EnPeehat_SetupAction(thisv, EnPeehat_Flying_StateLanding);
}

void EnPeehat_Flying_StateLanding(EnPeehat* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0, 1, 50, 0);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnPeehat_Flying_SetStateGround(thisv);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_LAND);
        thisv->actor.world.pos.y = thisv->actor.floorHeight;
    } else if (thisv->actor.floorHeight < thisv->actor.world.pos.y) {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 0.3f, 13.5f, 0.25f);
        if (thisv->actor.world.pos.y - thisv->actor.floorHeight < 60.0f) {
            Vec3f pos = thisv->actor.world.pos;
            pos.y = thisv->actor.floorHeight;
            func_80033480(globalCtx, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(globalCtx, thisv, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    Math_SmoothStepToS(&thisv->bladeRotVel, 0, 1, 100, 0);
    thisv->bladeRot += thisv->bladeRotVel;
}

void EnPeehat_Ground_SetStateHover(EnPeehat* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPeehatFlyingAnim);
    thisv->actor.speedXZ = Rand_ZeroOne() * 0.5f + 2.5f;
    thisv->unk2D4 = Rand_ZeroOne() * 10 + 10;
    thisv->state = PEAHAT_STATE_15;
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateHover);
}

void EnPeehat_Ground_StateHover(EnPeehat* thisv, GlobalContext* globalCtx) {
    f32 cos;
    Player* player = GET_PLAYER(globalCtx);

    // hover but don't gain altitude
    if (thisv->actor.world.pos.y - thisv->actor.floorHeight > 75.0f) {
        thisv->actor.world.pos.y -= 1.0f;
    }
    thisv->actor.world.pos.y += Math_CosF(thisv->unk2E0) * 1.4f;
    cos = Math_CosF(thisv->unk2E0) * 0.18f;
    thisv->unk2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    thisv->unk2D4--;
    if (thisv->unk2D4 <= 0) {
        thisv->actor.speedXZ = Rand_ZeroOne() * 0.5f + 2.5f;
        thisv->unk2D4 = Rand_ZeroOne() * 10.0f + 10.0f;
        thisv->unk2F4 = (Rand_ZeroOne() - 0.5f) * 1000.0f;
    }
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.world.rot.y += thisv->unk2F4;
    if (thisv->seekPlayerTimer <= 0) {
        EnPeehat_Ground_SetStateLanding(thisv);
        thisv->riseDelayTimer = 40;
    } else {
        thisv->seekPlayerTimer--;
    }
    thisv->actor.shape.rot.y += 0x15E;
    // if daytime, and the player is close to the initial spawn position
    if (IS_DAY && Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < thisv->xzDistMax) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        EnPeehat_Ground_SetStateSeekPlayer(thisv);
        thisv->unk2FA = globalCtx->gameplayFrames & 1;
    } else {
        EnPeehat_Ground_SetStateReturnHome(thisv);
    }
    Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 500, 0);
    thisv->bladeRot += thisv->bladeRotVel;
    Math_SmoothStepToF(&thisv->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_Ground_SetStateReturnHome(EnPeehat* thisv) {
    thisv->state = PEAHAT_STATE_RETURN_HOME;
    thisv->actor.speedXZ = 2.5f;
    EnPeehat_SetupAction(thisv, EnPeehat_Ground_StateReturnHome);
}

void EnPeehat_Ground_StateReturnHome(EnPeehat* thisv, GlobalContext* globalCtx) {
    f32 cos;
    s16 yRot;
    Player* player;

    player = GET_PLAYER(globalCtx);
    if (thisv->actor.world.pos.y - thisv->actor.floorHeight > 75.0f) {
        thisv->actor.world.pos.y -= 1.0f;
    } else {
        thisv->actor.world.pos.y += 1.0f;
    }
    thisv->actor.world.pos.y += Math_CosF(thisv->unk2E0) * 1.4f;
    cos = Math_CosF(thisv->unk2E0) * 0.18f;
    thisv->unk2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    yRot = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, yRot, 1, 600, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, 4500, 1, 600, 0);
    thisv->actor.shape.rot.y += 0x15E;
    thisv->bladeRot += thisv->bladeRotVel;
    if (Math_Vec3f_DistXZ(&thisv->actor.world.pos, &thisv->actor.home.pos) < 2.0f) {
        EnPeehat_Ground_SetStateLanding(thisv);
        thisv->riseDelayTimer = 60;
    }
    if (IS_DAY && Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) < thisv->xzDistMax) {
        thisv->seekPlayerTimer = 400;
        EnPeehat_Ground_SetStateSeekPlayer(thisv);
        thisv->unk2FA = (globalCtx->gameplayFrames & 1);
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_SetStateAttackRecoil(EnPeehat* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPeehatRecoilAnim, -4.0f);
    thisv->state = PEAHAT_STATE_ATTACK_RECOIL;
    thisv->actor.speedXZ = -9.0f;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    EnPeehat_SetupAction(thisv, EnPeehat_StateAttackRecoil);
}

void EnPeehat_StateAttackRecoil(EnPeehat* thisv, GlobalContext* globalCtx) {
    thisv->bladeRot += thisv->bladeRotVel;
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.speedXZ += 0.5f;
    if (thisv->actor.speedXZ == 0.0f) {
        // Is PEAHAT_TYPE_LARVA
        if (thisv->actor.params > 0) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
                pos.y = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
                EffectSsDeadDb_Spawn(globalCtx, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                                     1);
            }
            Actor_Kill(&thisv->actor);
        } else {
            EnPeehat_Ground_SetStateSeekPlayer(thisv);
            // Is PEAHAT_TYPE_GROUNDED
            if (thisv->actor.params < 0) {
                thisv->unk2FA = (thisv->unk2FA != 0) ? 0 : 1;
            }
        }
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void EnPeehat_SetStateBoomerangStunned(EnPeehat* thisv) {
    thisv->state = PEAHAT_STATE_STUNNED;
    if (thisv->actor.floorHeight < thisv->actor.world.pos.y) {
        thisv->actor.speedXZ = -9.0f;
    }
    thisv->bladeRotVel = 0;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    Actor_SetColorFilter(&thisv->actor, 0, 200, 0, 80);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnPeehat_SetupAction(thisv, EnPeehat_StateBoomerangStunned);
}

void EnPeehat_StateBoomerangStunned(EnPeehat* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight, 1.0f, 8.0f, 0.0f);
    if (thisv->actor.colorFilterTimer == 0) {
        EnPeehat_Ground_SetStateRise(thisv);
    }
}

void EnPeehat_Adult_SetStateDie(EnPeehat* thisv) {
    thisv->bladeRotVel = 0;
    thisv->isStateDieFirstUpdate = 1;
    thisv->actor.speedXZ = 0.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 8);
    thisv->state = PEAHAT_STATE_DYING;
    thisv->scaleShift = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    EnPeehat_SetupAction(thisv, EnPeehat_Adult_StateDie);
}

void EnPeehat_Adult_StateDie(EnPeehat* thisv, GlobalContext* globalCtx) {
    if (thisv->isStateDieFirstUpdate) {
        thisv->unk2D4--;
        if (thisv->unk2D4 <= 0 || thisv->actor.colChkInfo.health == 0) {
            Animation_MorphToPlayOnce(&thisv->skelAnime, &gPeehatRecoilAnim, -4.0f);
            thisv->bladeRotVel = 4000;
            thisv->unk2D4 = 14;
            thisv->actor.speedXZ = 0;
            thisv->actor.velocity.y = 6;
            thisv->isStateDieFirstUpdate = 0;
            thisv->actor.shape.rot.z = thisv->actor.shape.rot.x = 0;
        } else if (thisv->actor.colorFilterTimer & 4) {
            Math_SmoothStepToF(&thisv->scaleShift, 0.205f, 1.0f, 0.235f, 0);
        } else {
            Math_SmoothStepToF(&thisv->scaleShift, 0, 1.0f, 0.005f, 0);
        }
    } else {
        SkelAnime_Update(&thisv->skelAnime);
        thisv->bladeRot += thisv->bladeRotVel;
        Math_SmoothStepToS(&thisv->bladeRotVel, 4000, 1, 250, 0);
        if (thisv->actor.colChkInfo.health == 0) {
            thisv->actor.scale.x -= 0.0015f;
            Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
        }
        if (Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.floorHeight + 88.5f, 1.0f, 3.0f, 0.0f) == 0.0f &&
            thisv->actor.world.pos.y - thisv->actor.floorHeight < 59.0f) {
            Vec3f pos = thisv->actor.world.pos;
            pos.y = thisv->actor.floorHeight;
            func_80033480(globalCtx, &pos, 80.0f, 1, 150, 100, 1);
            EnPeehat_SpawnDust(globalCtx, thisv, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
        if (thisv->actor.speedXZ < 0) {
            thisv->actor.speedXZ += 0.25f;
        }
        thisv->unk2D4--;
        if (thisv->unk2D4 <= 0) {
            if (thisv->actor.colChkInfo.health == 0) {
                EnPeehat_SetStateExplode(thisv);
                // if PEAHAT_TYPE_GROUNDED
            } else if (thisv->actor.params < 0) {
                EnPeehat_Ground_SetStateHover(thisv);
                thisv->riseDelayTimer = 60;
            } else {
                EnPeehat_Flying_SetStateFly(thisv);
            }
        }
    }
}

void EnPeehat_SetStateExplode(EnPeehat* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPeehatFlyingAnim);
    thisv->state = PEAHAT_STATE_EXPLODE;
    thisv->animTimer = 5;
    thisv->unk2E0 = 0.0f;
    EnPeehat_SetupAction(thisv, EnPeehat_StateExplode);
}

void EnPeehat_StateExplode(EnPeehat* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;
    s32 pad[2];

    if (thisv->animTimer == 5) {
        bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                   thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0x602, 0);
        if (bomb != NULL) {
            bomb->timer = 0;
        }
    }
    thisv->animTimer--;
    if (thisv->animTimer == 0) {
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x40);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x40);
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x40);
        Actor_Kill(&thisv->actor);
    }
}

void EnPeehat_Adult_CollisionCheck(EnPeehat* thisv, GlobalContext* globalCtx) {
    if ((thisv->colCylinder.base.acFlags & AC_BOUNCED) || (thisv->colQuad.base.acFlags & AC_BOUNCED)) {
        thisv->colQuad.base.acFlags &= ~AC_BOUNCED;
        thisv->colCylinder.base.acFlags &= ~AC_BOUNCED;
        thisv->colJntSph.base.acFlags &= ~AC_HIT;
    } else if (thisv->colJntSph.base.acFlags & AC_HIT) {
        thisv->colJntSph.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&thisv->actor, &thisv->colJntSph, 1);
        if (thisv->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_NUT ||
            thisv->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
            return;
        }
        if (thisv->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_HOOKSHOT) {
            thisv->actor.colChkInfo.health = 0;
        } else if (thisv->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_BOOMERANG) {
            if (thisv->state != PEAHAT_STATE_STUNNED) {
                EnPeehat_SetStateBoomerangStunned(thisv);
            }
            return;
        } else {
            Actor_ApplyDamage(&thisv->actor);
            Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 8);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PIHAT_DAMAGE);
        }

        if (thisv->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_FIRE) {
            Vec3f pos;
            s32 i;
            for (i = 4; i >= 0; i--) {
                pos.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
                pos.y = Rand_ZeroOne() * 25.0f + thisv->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
                EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &pos, 70, 0, 0, -1);
            }
            Actor_SetColorFilter(&thisv->actor, 0x4000, 200, 0, 100);
        }
        if (thisv->actor.colChkInfo.health == 0) {
            EnPeehat_Adult_SetStateDie(thisv);
        }
    }
}

void EnPeehat_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnPeehat* thisv = (EnPeehat*)thisx;
    s32 i;
    Player* player = GET_PLAYER(globalCtx);

    // If Adult Peahat
    if (thisx->params <= 0) {
        EnPeehat_Adult_CollisionCheck(thisv, globalCtx);
    }
    if (thisx->colChkInfo.damageEffect != PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
        if (thisx->speedXZ != 0.0f || thisx->velocity.y != 0.0f) {
            Actor_MoveForward(thisx);
            Actor_UpdateBgCheckInfo(globalCtx, thisx, 25.0f, 30.0f, 30.0f, 5);
        }

        thisv->actionFunc(thisv, globalCtx);
        if ((globalCtx->gameplayFrames & 0x7F) == 0) {
            thisv->jiggleRotInc = (Rand_ZeroOne() * 0.25f) + 0.5f;
        }
        thisv->jiggleRot += thisv->jiggleRotInc;
    }
    // if PEAHAT_TYPE_GROUNDED
    if (thisx->params < 0) {
        // Set the Z-Target point on the Peahat's weak point
        thisx->focus.pos.x = thisv->colJntSph.elements[0].dim.worldSphere.center.x;
        thisx->focus.pos.y = thisv->colJntSph.elements[0].dim.worldSphere.center.y;
        thisx->focus.pos.z = thisv->colJntSph.elements[0].dim.worldSphere.center.z;
        if (thisv->state == PEAHAT_STATE_SEEK_PLAYER) {
            Math_SmoothStepToS(&thisx->shape.rot.x, 6000, 1, 300, 0);
        } else {
            Math_SmoothStepToS(&thisx->shape.rot.x, 0, 1, 300, 0);
        }
    } else {
        thisx->focus.pos = thisx->world.pos;
    }
    Collider_UpdateCylinder(thisx, &thisv->colCylinder);
    if (thisx->colChkInfo.health > 0) {
        // If Adult Peahat
        if (thisx->params <= 0) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder.base);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colJntSph.base);
            if (thisx->colorFilterTimer == 0 || !(thisx->colorFilterParams & 0x4000)) {
                if (thisv->state != PEAHAT_STATE_EXPLODE) {
                    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colJntSph.base);
                }
            }
        }
        if (thisx->params != PEAHAT_TYPE_FLYING && thisv->colQuad.base.atFlags & AT_HIT) {
            thisv->colQuad.base.atFlags &= ~AT_HIT;
            if (&player->actor == thisv->colQuad.base.at) {
                EnPeehat_SetStateAttackRecoil(thisv);
            }
        }
    }
    if (thisv->state == PEAHAT_STATE_15 || thisv->state == PEAHAT_STATE_SEEK_PLAYER || thisv->state == PEAHAT_STATE_FLY ||
        thisv->state == PEAHAT_STATE_RETURN_HOME || thisv->state == PEAHAT_STATE_EXPLODE) {
        if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colQuad.base);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colQuad.base);
        }
        // if PEAHAT_TYPE_GROUNDED
        if (thisx->params < 0 && (thisx->flags & ACTOR_FLAG_6)) {
            for (i = 1; i >= 0; i--) {
                Vec3f posResult;
                CollisionPoly* poly = NULL;
                s32 bgId;
                Vec3f* posB = &thisv->bladeTip[i];

                if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisx->world.pos, posB, &posResult, &poly, true, true,
                                            false, true, &bgId) == true) {
                    func_80033480(globalCtx, &posResult, 0.0f, 1, 300, 150, 1);
                    EnPeehat_SpawnDust(globalCtx, thisv, &posResult, 0.0f, 3, 1.05f, 1.5f);
                }
            }
        } else if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder.base);
        }
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colCylinder.base);
    }
    Math_SmoothStepToF(&thisv->scaleShift, 0.0f, 1.0f, 0.001f, 0.0f);
}

s32 EnPeehat_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnPeehat* thisv = (EnPeehat*)thisx;

    if (limbIndex == 4) {
        rot->x = -thisv->bladeRot;
    }
    if (limbIndex == 3 || (limbIndex == 23 && (thisv->state == PEAHAT_STATE_DYING || thisv->state == PEAHAT_STATE_3 ||
                                               thisv->state == PEAHAT_STATE_4))) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1946);
        Matrix_Push();
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(thisv->jiggleRot * 0.115f, MTXMODE_APPLY);
        Matrix_RotateY(thisv->jiggleRot * 0.13f, MTXMODE_APPLY);
        Matrix_RotateZ(thisv->jiggleRot * 0.1f, MTXMODE_APPLY);
        Matrix_Scale(1.0f - thisv->scaleShift, thisv->scaleShift + 1.0f, 1.0f - thisv->scaleShift, MTXMODE_APPLY);
        Matrix_RotateZ(-(thisv->jiggleRot * 0.1f), MTXMODE_APPLY);
        Matrix_RotateY(-(thisv->jiggleRot * 0.13f), MTXMODE_APPLY);
        Matrix_RotateX(-(thisv->jiggleRot * 0.115f), MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1959),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1963);
        return true;
    }
    return false;
}

void EnPeehat_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f peahatBladeTip[] = { { 0.0f, 0.0f, 5500.0f }, { 0.0f, 0.0f, -5500.0f } };

    EnPeehat* thisv = (EnPeehat*)thisx;
    f32 damageYRot;

    if (limbIndex == 4) {
        Matrix_MultVec3f(&peahatBladeTip[0], &thisv->bladeTip[0]);
        Matrix_MultVec3f(&peahatBladeTip[1], &thisv->bladeTip[1]);
        return;
    }
    // is Adult Peahat
    if (limbIndex == 3 && thisv->actor.params <= 0) {
        damageYRot = 0.0f;
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1981);
        Matrix_Push();
        Matrix_Translate(-1000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Collider_UpdateSpheres(0, &thisv->colJntSph);
        Matrix_Translate(500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        if (thisv->actor.colorFilterTimer != 0 && (thisv->actor.colorFilterParams & 0x4000)) {
            damageYRot = Math_SinS(thisv->actor.colorFilterTimer * 0x4E20) * 0.35f;
        }
        Matrix_RotateY(3.2f + damageYRot, MTXMODE_APPLY);
        Matrix_Scale(0.3f, 0.2f, 0.2f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1990),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_peehat.c", 1994);
    }
}

void EnPeehat_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f D_80AD285C[] = {
        { 0.0f, 0.0f, -4500.0f }, { -4500.0f, 0.0f, 0.0f }, { 4500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 4500.0f }
    };
    EnPeehat* thisv = (EnPeehat*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnPeehat_OverrideLimbDraw,
                      EnPeehat_PostLimbDraw, thisv);
    if (thisv->actor.speedXZ != 0.0f || thisv->actor.velocity.y != 0.0f) {
        Matrix_MultVec3f(&D_80AD285C[0], &thisv->colQuad.dim.quad[1]);
        Matrix_MultVec3f(&D_80AD285C[1], &thisv->colQuad.dim.quad[0]);
        Matrix_MultVec3f(&D_80AD285C[2], &thisv->colQuad.dim.quad[3]);
        Matrix_MultVec3f(&D_80AD285C[3], &thisv->colQuad.dim.quad[2]);
        Collider_SetQuadVertices(&thisv->colQuad, &thisv->colQuad.dim.quad[0], &thisv->colQuad.dim.quad[1],
                                 &thisv->colQuad.dim.quad[2], &thisv->colQuad.dim.quad[3]);
    }
}
