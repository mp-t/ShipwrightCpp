/*
 * File: z_en_sb.c
 * Overlay: ovl_En_Sb
 * Description: Shellblade
 */

#include "z_en_sb.h"
#include "vt.h"
#include "objects/object_sb/object_sb.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnSb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSb_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSb_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSb_SetupWaitClosed(EnSb* thisv);

void EnSb_WaitClosed(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_Open(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_WaitOpen(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_TurnAround(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_Lunge(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_Bounce(EnSb* thisv, GlobalContext* globalCtx);
void EnSb_Cooldown(EnSb* thisv, GlobalContext* globalCtx);

ActorInit En_Sb_InitVars = {
    ACTOR_EN_SB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SB,
    sizeof(EnSb),
    (ActorFunc)EnSb_Init,
    (ActorFunc)EnSb_Destroy,
    (ActorFunc)EnSb_Update,
    (ActorFunc)EnSb_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0xFFCFFFFF, 0x04, 0x08 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x01, 0x01, 0x01 },
    { 30, 40, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xD),
    /* Giant's Knife */ DMG_ENTRY(4, 0xD),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xD),
    /* Giant spin    */ DMG_ENTRY(4, 0xD),
    /* Master spin   */ DMG_ENTRY(2, 0xD),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xD),
    /* Giant jump    */ DMG_ENTRY(8, 0xD),
    /* Master jump   */ DMG_ENTRY(4, 0xD),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x27, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

static Vec3f sFlamePosOffsets[] = {
    { 5.0f, 0.0f, 0.0f },
    { -5.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 5.0f },
    { 0.0f, 0.0f, -5.0f },
};

typedef enum {
    /* 0x00 */ SHELLBLADE_OPEN,
    /* 0x01 */ SHELLBLADE_WAIT_CLOSED,
    /* 0x02 */ SHELLBLADE_WAIT_OPEN,
    /* 0x03 */ SHELLBLADE_LUNGE,
    /* 0x04 */ SHELLBLADE_BOUNCE
} ShellbladeBehavior;

void EnSb_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSb* thisv = (EnSb*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.colChkInfo.damageTable = sDamageTable;
    thisv->actor.colChkInfo.health = 2;
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_sb_Skel_002BF0, &object_sb_Anim_000194, NULL, NULL, 0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->isDead = false;
    thisv->actor.colChkInfo.mass = 0;
    Actor_SetScale(&thisv->actor, 0.006f);
    thisv->actor.shape.rot.y = 0;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = -0.35f;
    thisv->fire = 0;
    thisv->hitByWindArrow = false;
    thisv->actor.velocity.y = -1.0f;
    EnSb_SetupWaitClosed(thisv);
}

void EnSb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSb* thisv = (EnSb*)thisx;
    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnSb_SpawnBubbles(GlobalContext* globalCtx, EnSb* thisv) {
    s32 i;

    if (thisv->actor.yDistToWater > 0) {
        for (i = 0; i < 10; i++) {
            EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 10.0f, 10.0f, 30.0f, 0.25f);
        }
    }
}

void EnSb_SetupWaitClosed(EnSb* thisv) {
    Animation_Change(&thisv->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_00004C),
                     ANIMMODE_ONCE, 0.0f);
    thisv->behavior = SHELLBLADE_WAIT_CLOSED;
    thisv->actionFunc = EnSb_WaitClosed;
}

void EnSb_SetupOpen(EnSb* thisv) {
    Animation_Change(&thisv->skelAnime, &object_sb_Anim_000194, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_000194),
                     ANIMMODE_ONCE, 0.0f);
    thisv->behavior = SHELLBLADE_OPEN;
    thisv->actionFunc = EnSb_Open;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHELL_MOUTH);
}

void EnSb_SetupWaitOpen(EnSb* thisv) {
    Animation_Change(&thisv->skelAnime, &object_sb_Anim_002C8C, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_002C8C),
                     ANIMMODE_LOOP, 0.0f);
    thisv->behavior = SHELLBLADE_WAIT_OPEN;
    thisv->actionFunc = EnSb_WaitOpen;
}

void EnSb_SetupLunge(EnSb* thisv) {
    f32 frameCount = Animation_GetLastFrame(&object_sb_Anim_000124);
    f32 playbackSpeed = thisv->actor.yDistToWater > 0.0f ? 1.0f : 0.0f;

    Animation_Change(&thisv->skelAnime, &object_sb_Anim_000124, playbackSpeed, 0.0f, frameCount, ANIMMODE_ONCE, 0);
    thisv->behavior = SHELLBLADE_LUNGE;
    thisv->actionFunc = EnSb_Lunge;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_SHELL_MOUTH);
}

void EnSb_SetupBounce(EnSb* thisv) {
    Animation_Change(&thisv->skelAnime, &object_sb_Anim_0000B4, 1.0f, 0, Animation_GetLastFrame(&object_sb_Anim_0000B4),
                     ANIMMODE_ONCE, 0.0f);
    thisv->behavior = SHELLBLADE_BOUNCE;
    thisv->actionFunc = EnSb_Bounce;
}

void EnSb_SetupCooldown(EnSb* thisv, s32 changeSpeed) {
    f32 frameCount = Animation_GetLastFrame(&object_sb_Anim_00004C);

    if (thisv->behavior != SHELLBLADE_WAIT_CLOSED) {
        Animation_Change(&thisv->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
    }
    thisv->behavior = SHELLBLADE_WAIT_CLOSED;
    if (changeSpeed) {
        if (thisv->actor.yDistToWater > 0.0f) {
            thisv->actor.speedXZ = -5.0f;
            if (thisv->actor.velocity.y < 0.0f) {
                thisv->actor.velocity.y = 2.1f;
            }
        } else {
            thisv->actor.speedXZ = -6.0f;
            if (thisv->actor.velocity.y < 0.0f) {
                thisv->actor.velocity.y = 1.4f;
            }
        }
    }
    thisv->timer = 60;
    thisv->actionFunc = EnSb_Cooldown;
}

void EnSb_WaitClosed(EnSb* thisv, GlobalContext* globalCtx) {
    // always face toward link
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((thisv->actor.xzDistToPlayer <= 160.0f) && (thisv->actor.xzDistToPlayer > 40.0f)) {
        EnSb_SetupOpen(thisv);
    }
}

void EnSb_Open(EnSb* thisv, GlobalContext* globalCtx) {
    f32 currentFrame = thisv->skelAnime.curFrame;

    if (Animation_GetLastFrame(&object_sb_Anim_000194) <= currentFrame) {
        thisv->timer = 15;
        EnSb_SetupWaitOpen(thisv);
    } else {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);
        if ((thisv->actor.xzDistToPlayer > 160.0f) || (thisv->actor.xzDistToPlayer <= 40.0f)) {
            EnSb_SetupWaitClosed(thisv);
        }
    }
}

void EnSb_WaitOpen(EnSb* thisv, GlobalContext* globalCtx) {
    s16 timer = thisv->timer;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((thisv->actor.xzDistToPlayer > 160.0f) || (thisv->actor.xzDistToPlayer <= 40.0f)) {
        EnSb_SetupWaitClosed(thisv);
    }

    if (timer != 0) {
        thisv->timer = timer - 1;
    } else {
        thisv->timer = 0;
        thisv->attackYaw = thisv->actor.yawTowardsPlayer;
        thisv->actionFunc = EnSb_TurnAround;
    }
}

void EnSb_TurnAround(EnSb* thisv, GlobalContext* globalCtx) {
    s16 invertedYaw;

    invertedYaw = thisv->attackYaw + 0x8000;
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, invertedYaw, 0x1, 0x1F40, 0xA);

    if (thisv->actor.shape.rot.y == invertedYaw) {
        thisv->actor.world.rot.y = thisv->attackYaw;
        if (thisv->actor.yDistToWater > 0.0f) {
            thisv->actor.velocity.y = 3.0f;
            thisv->actor.speedXZ = 5.0f;
            thisv->actor.gravity = -0.35f;
        } else {
            thisv->actor.velocity.y = 2.0f;
            thisv->actor.speedXZ = 6.0f;
            thisv->actor.gravity = -2.0f;
        }
        EnSb_SpawnBubbles(globalCtx, thisv);
        thisv->bouncesLeft = 3;
        EnSb_SetupLunge(thisv);
        // "Attack!!"
        osSyncPrintf("アタァ〜ック！！\n");
    }
}

void EnSb_Lunge(EnSb* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.2f);
    if ((thisv->actor.velocity.y <= -0.1f) || ((thisv->actor.bgCheckFlags & 2))) {
        if (!(thisv->actor.yDistToWater > 0.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
        thisv->actor.bgCheckFlags = thisv->actor.bgCheckFlags & ~2;
        EnSb_SetupBounce(thisv);
    }
}

void EnSb_Bounce(EnSb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 currentFrame;
    f32 frameCount;

    currentFrame = thisv->skelAnime.curFrame;
    frameCount = Animation_GetLastFrame(&object_sb_Anim_0000B4);
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.2f);

    if (currentFrame == frameCount) {
        if (thisv->bouncesLeft != 0) {
            thisv->bouncesLeft--;
            thisv->timer = 1;
            if (thisv->actor.yDistToWater > 0.0f) {
                thisv->actor.velocity.y = 3.0f;
                thisv->actor.speedXZ = 5.0f;
                thisv->actor.gravity = -0.35f;
            } else {
                thisv->actor.velocity.y = 2.0f;
                thisv->actor.speedXZ = 6.0f;
                thisv->actor.gravity = -2.0f;
            }
            EnSb_SpawnBubbles(globalCtx, thisv);
            EnSb_SetupLunge(thisv);
        } else if (thisv->actor.bgCheckFlags & 1) {
            thisv->actor.bgCheckFlags &= ~2;
            thisv->actor.speedXZ = 0.0f;
            thisv->timer = 1;
            EnSb_SetupWaitClosed(thisv);
            osSyncPrintf(VT_FGCOL(RED) "攻撃終了！！" VT_RST "\n"); // "Attack Complete!"
        }
    }
}

void EnSb_Cooldown(EnSb* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
        if (thisv->actor.bgCheckFlags & 1) {
            thisv->actor.bgCheckFlags &= ~1;
            thisv->actor.speedXZ = 0.0f;
        }
    } else {
        if (thisv->actor.bgCheckFlags & 1) {
            thisv->actor.bgCheckFlags &= ~1;
            thisv->actionFunc = EnSb_WaitClosed;
            thisv->actor.speedXZ = 0.0f;
        }
    }
}

s32 EnSb_IsVulnerable(EnSb* thisv) {
    switch (thisv->behavior) {
        case SHELLBLADE_OPEN:
            if ((thisv->skelAnime.curFrame >= 2.0f) && (thisv->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_CLOSED:
            if ((thisv->skelAnime.curFrame >= 0.0f) && (thisv->skelAnime.curFrame <= 1.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_OPEN:
            if ((thisv->skelAnime.curFrame >= 0.0f) && (thisv->skelAnime.curFrame <= 19.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_LUNGE:
            if (thisv->skelAnime.curFrame == 0.0f) {
                return true;
            }
            break;
        case SHELLBLADE_BOUNCE:
            if ((thisv->skelAnime.curFrame >= 3.0f) && (thisv->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
    }
    return false;
}

s32 EnSb_UpdateDamage(EnSb* thisv, GlobalContext* globalCtx) {
    Vec3f hitPoint;
    f32 hitY;
    s16 yawDiff;
    s32 tookDamage;
    u8 hitByWindArrow;

    // hit box collided, switch to cool down
    if ((thisv->collider.base.atFlags & AT_HIT)) {
        EnSb_SetupCooldown(thisv, 1);
        return 1;
    }

    // hurt box collided, take damage if appropriate
    if ((thisv->collider.base.acFlags & AC_HIT)) {
        hitByWindArrow = false;
        tookDamage = false;
        thisv->collider.base.acFlags &= ~AC_HIT;

        switch (thisv->actor.colChkInfo.damageEffect) {
            case 14: // wind arrow
                hitByWindArrow = true;
            case 15: // explosions, arrow, hammer, ice arrow, light arrow, spirit arrow, shadow arrow
                if (EnSb_IsVulnerable(thisv)) {
                    hitY = thisv->collider.info.bumper.hitPos.y - thisv->actor.world.pos.y;
                    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        Actor_ApplyDamage(&thisv->actor);
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        tookDamage = true;
                    }
                }
                break;
            case 2: // fire arrow, dins fire
                thisv->fire = 4;
                Actor_ApplyDamage(&thisv->actor);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                tookDamage = true;
                break;
            case 1:  // hookshot/longshot
            case 13: // all sword damage
                if (EnSb_IsVulnerable(thisv)) {
                    hitY = thisv->collider.info.bumper.hitPos.y - thisv->actor.world.pos.y;
                    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        Actor_ApplyDamage(&thisv->actor);
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0x2000, 0x50);
                        tookDamage = true;
                        EnSb_SetupCooldown(thisv, 0);
                    }
                }
                break;
            default:
                break;
        }
        if (thisv->actor.colChkInfo.health == 0) {
            thisv->hitByWindArrow = hitByWindArrow;
            BodyBreak_Alloc(&thisv->bodyBreak, 8, globalCtx);
            thisv->isDead = true;
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_SHELL_DEAD);
            return 1;
        }

        // if player attack didn't do damage, play recoil sound and spawn sparks
        if (!tookDamage) {
            hitPoint.x = thisv->collider.info.bumper.hitPos.x;
            hitPoint.y = thisv->collider.info.bumper.hitPos.y;
            hitPoint.z = thisv->collider.info.bumper.hitPos.z;
            CollisionCheck_SpawnShieldParticlesMetal2(globalCtx, &hitPoint);
        }
    }

    return 0;
}

void EnSb_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSb* thisv = (EnSb*)thisx;
    s32 pad;

    if (thisv->isDead) {
        if (thisv->actor.yDistToWater > 0.0f) {
            thisv->actor.params = 4;
        } else {
            thisv->actor.params = 1;
        }
        if (BodyBreak_SpawnParts(&thisv->actor, &thisv->bodyBreak, globalCtx, thisv->actor.params)) {
            if (!thisv->hitByWindArrow) {
                Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x80);
            } else {
                Item_DropCollectible(globalCtx, &thisv->actor.world.pos, 8);
            }
            Actor_Kill(&thisv->actor);
        }
    } else {
        Actor_SetFocus(&thisv->actor, 20.0f);
        Actor_SetScale(&thisv->actor, 0.006f);
        Actor_MoveForward(&thisv->actor);
        thisv->actionFunc(thisv, globalCtx);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 20.0f, 5);
        EnSb_UpdateDamage(thisv, globalCtx);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        SkelAnime_Update(&thisv->skelAnime);
    }
}

void EnSb_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnSb* thisv = (EnSb*)thisx;

    BodyBreak_SetInfo(&thisv->bodyBreak, limbIndex, 0, 6, 8, dList, BODYBREAK_OBJECT_DEFAULT);
}

void EnSb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSb* thisv = (EnSb*)thisx;
    Vec3f flamePos;
    Vec3f* offset;
    s16 fireDecr;

    func_8002EBCC(&thisv->actor, globalCtx, 1);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, EnSb_PostLimbDraw, thisv);
    if (thisv->fire != 0) {
        thisv->actor.colorFilterTimer++;
        fireDecr = thisv->fire - 1;
        // thisv is intended to draw flames after being burned, but the condition is never met to run thisv code
        // fire gets set to 4 when burned, decrements to 3 and fails the "& 1" check and never stores the decrement
        if ((fireDecr & 1) == 0) {
            offset = &sFlamePosOffsets[(fireDecr & 3)];
            flamePos.x = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.x + offset->x);
            flamePos.y = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.y + offset->y);
            flamePos.z = Rand_CenteredFloat(5.0f) + (thisv->actor.world.pos.z + offset->z);
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &flamePos, 100, 0, 0, -1);
        }
    }
}
