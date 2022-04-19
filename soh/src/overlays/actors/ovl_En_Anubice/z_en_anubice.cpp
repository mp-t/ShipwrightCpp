/*
 * File: z_en_anubice.c
 * Overlay: ovl_En_Anubice
 * Description: Anubis Body
 */

#include "z_en_anubice.h"
#include "objects/object_anubice/object_anubice.h"
#include "overlays/actors/ovl_En_Anubice_Tag/z_en_anubice_tag.h"
#include "overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnAnubice_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAnubice_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAnubice_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAnubice_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnAnubice_FindFlameCircles(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_SetupIdle(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_Idle(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_GoToHome(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_SetupShootFireball(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_ShootFireball(EnAnubice* thisv, GlobalContext* globalCtx);
void EnAnubice_Die(EnAnubice* thisv, GlobalContext* globalCtx);

ActorInit En_Anubice_InitVars = {
    ACTOR_EN_ANUBICE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ANUBICE,
    sizeof(EnAnubice),
    (ActorFunc)EnAnubice_Init,
    (ActorFunc)EnAnubice_Destroy,
    (ActorFunc)EnAnubice_Update,
    (ActorFunc)EnAnubice_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 29, 103, 0, { 0, 0, 0 } },
};

typedef enum {
    /* 0x0 */ ANUBICE_DMGEFF_NONE,
    /* 0x2 */ ANUBICE_DMGEFF_FIRE = 2,
    /* 0xF */ ANUBICE_DMGEFF_0xF = 0xF // Treated the same as ANUBICE_DMGEFF_NONE in code
} AnubiceDamageEffect;

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Deku stick    */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Slingshot     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Explosive     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Boomerang     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Normal arrow  */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Hammer swing  */ DMG_ENTRY(1, ANUBICE_DMGEFF_0xF),
    /* Hookshot      */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Kokiri sword  */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Master sword  */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Giant's Knife */ DMG_ENTRY(6, ANUBICE_DMGEFF_0xF),
    /* Fire arrow    */ DMG_ENTRY(2, ANUBICE_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Light arrow   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 1   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 3   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Fire magic    */ DMG_ENTRY(3, ANUBICE_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Giant spin    */ DMG_ENTRY(6, ANUBICE_DMGEFF_0xF),
    /* Master spin   */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Giant jump    */ DMG_ENTRY(12, ANUBICE_DMGEFF_0xF),
    /* Master jump   */ DMG_ENTRY(4, ANUBICE_DMGEFF_0xF),
    /* Unknown 1     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
};

void EnAnubice_Hover(EnAnubice* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->hoverVelocityTimer += 1500.0f;
    thisv->targetHeight = player->actor.world.pos.y + thisv->playerHeightOffset;
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetHeight, 0.1f, 10.0f);
    Math_ApproachF(&thisv->playerHeightOffset, 10.0f, 0.1f, 0.5f);
    thisv->actor.velocity.y = Math_SinS(thisv->hoverVelocityTimer);
}

void EnAnubice_SetFireballRot(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 xzdist;
    f32 x;
    f32 y;
    f32 z;
    Player* player = GET_PLAYER(globalCtx);

    x = player->actor.world.pos.x - thisv->fireballPos.x;
    y = player->actor.world.pos.y + 10.0f - thisv->fireballPos.y;
    z = player->actor.world.pos.z - thisv->fireballPos.z;
    xzdist = sqrtf(SQ(x) + SQ(z));

    thisv->fireballRot.x = -RADF_TO_BINANG(Math_FAtan2F(y, xzdist));
    thisv->fireballRot.y = RADF_TO_BINANG(Math_FAtan2F(x, z));
}

void EnAnubice_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubice* thisv = (EnAnubice*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gAnubiceSkel, &gAnubiceIdleAnim, thisv->jointTable, thisv->morphTable,
                   ANUBICE_LIMB_MAX);

    osSyncPrintf("\n\n");
    // "☆☆☆☆☆ Anubis occurence ☆☆☆☆☆"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ アヌビス発生 ☆☆☆☆☆ \n" VT_RST);

    thisv->actor.naviEnemyId = 0x3A;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);

    Actor_SetScale(&thisv->actor, 0.015f);

    thisv->actor.colChkInfo.damageTable = sDamageTable;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.shape.yOffset = -4230.0f;
    thisv->focusHeightOffset = 0.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->home = thisv->actor.world.pos;
    thisv->actor.targetMode = 3;
    thisv->actionFunc = EnAnubice_FindFlameCircles;
}

void EnAnubice_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubice* thisv = (EnAnubice*)thisx;
    EnAnubiceTag* tag;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);

    if (thisv->actor.params != 0) {
        if (thisv->actor.parent) {}

        tag = (EnAnubiceTag*)thisv->actor.parent;
        if (tag != NULL && tag->actor.update != NULL) {
            tag->anubis = NULL;
        }
    }
}

void EnAnubice_FindFlameCircles(EnAnubice* thisv, GlobalContext* globalCtx) {
    Actor* currentProp;
    s32 flameCirclesFound;

    if (thisv->isMirroringLink) {
        if (!thisv->hasSearchedForFlameCircles) {
            flameCirclesFound = 0;
            currentProp = globalCtx->actorCtx.actorLists[ACTORCAT_PROP].head;
            while (currentProp != NULL) {
                if (currentProp->id != ACTOR_BG_HIDAN_CURTAIN) {
                    currentProp = currentProp->next;
                } else {
                    thisv->flameCircles[flameCirclesFound] = (BgHidanCurtain*)currentProp;
                    // "☆☆☆☆☆ How many fires? ☆☆☆☆☆"
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 火は幾つ？ ☆☆☆☆☆ %d\n" VT_RST, flameCirclesFound);
                    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 火は幾つ？ ☆☆☆☆☆ %x\n" VT_RST,
                                 thisv->flameCircles[flameCirclesFound]);
                    if (flameCirclesFound < ARRAY_COUNT(thisv->flameCircles) - 1) {
                        flameCirclesFound++;
                    }
                    currentProp = currentProp->next;
                }
            }
            thisv->hasSearchedForFlameCircles = true;
        }
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actionFunc = EnAnubice_SetupIdle;
    }
}

void EnAnubice_SetupIdle(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gAnubiceIdleAnim);

    Animation_Change(&thisv->skelAnime, &gAnubiceIdleAnim, 1.0f, 0.0f, (s16)lastFrame, ANIMMODE_LOOP, -10.0f);

    thisv->actionFunc = EnAnubice_Idle;
    thisv->actor.velocity.x = thisv->actor.velocity.z = thisv->actor.gravity = 0.0f;
}

void EnAnubice_Idle(EnAnubice* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachZeroF(&thisv->actor.shape.yOffset, 0.5f, 300.0f);
    Math_ApproachF(&thisv->focusHeightOffset, 70.0f, 0.5f, 5.0f);

    if (!thisv->isKnockedback) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    if (thisv->actor.shape.yOffset > -2.0f) {
        thisv->actor.shape.yOffset = 0.0f;

        if (player->swordState != 0) {
            thisv->actionFunc = EnAnubice_SetupShootFireball;
        } else if (thisv->isLinkOutOfRange) {
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.gravity = -1.0f;
            thisv->actionFunc = EnAnubice_GoToHome;
        }
    }
}

void EnAnubice_GoToHome(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 xzdist;
    f32 xRatio;
    f32 zRatio;
    f32 x;
    f32 z;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.shape.yOffset, -4230.0f, 0.5f, 300.0f);
    Math_ApproachZeroF(&thisv->focusHeightOffset, 0.5f, 5.0f);

    if (!thisv->isKnockedback) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    if ((fabsf(thisv->home.x - thisv->actor.world.pos.x) > 3.0f) &&
        (fabsf(thisv->home.z - thisv->actor.world.pos.z) > 3.0f)) {
        x = thisv->home.x - thisv->actor.world.pos.x;
        z = thisv->home.z - thisv->actor.world.pos.z;
        xzdist = sqrtf(SQ(x) + SQ(z));
        xRatio = x / xzdist;
        zRatio = z / xzdist;
        thisv->actor.world.pos.x += xRatio * 8;
        thisv->actor.world.pos.z += zRatio * 8.0f;
    } else if (thisv->actor.shape.yOffset < -4220.0f) {
        thisv->actor.shape.yOffset = -4230.0f;
        thisv->isMirroringLink = thisv->isLinkOutOfRange = false;
        thisv->actionFunc = EnAnubice_FindFlameCircles;
        thisv->actor.gravity = 0.0f;
    }
}

void EnAnubice_SetupShootFireball(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gAnubiceAttackingAnim);

    thisv->animLastFrame = lastFrame;
    Animation_Change(&thisv->skelAnime, &gAnubiceAttackingAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = EnAnubice_ShootFireball;
    thisv->actor.velocity.x = thisv->actor.velocity.z = 0.0f;
}

void EnAnubice_ShootFireball(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 curFrame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);

    if (!thisv->isKnockedback) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    EnAnubice_SetFireballRot(thisv, globalCtx);

    if (curFrame == 12.0f) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ANUBICE_FIRE, thisv->fireballPos.x,
                    thisv->fireballPos.y + 15.0f, thisv->fireballPos.z, thisv->fireballRot.x, thisv->fireballRot.y, 0, 0);
    }

    if (thisv->animLastFrame <= curFrame) {
        thisv->actionFunc = EnAnubice_SetupIdle;
    }
}

void EnAnubice_SetupDie(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(&gAnubiceFallDownAnim);

    thisv->animLastFrame = lastFrame;
    Animation_Change(&thisv->skelAnime, &gAnubiceFallDownAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -20.0f);

    thisv->isFallingOver = false;
    thisv->fallTargetPitch = 0;
    thisv->deathTimer = 20;
    thisv->actor.velocity.x = thisv->actor.velocity.z = 0.0f;
    thisv->actor.gravity = -1.0f;

    if (BgCheck_SphVsFirstPoly(&globalCtx->colCtx, &thisv->fireballPos, 70.0f)) {
        thisv->isFallingOver = true;
        thisv->fallTargetPitch = thisv->actor.shape.rot.x - 0x7F00;
    }

    thisv->actionFunc = EnAnubice_Die;
}

void EnAnubice_Die(EnAnubice* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    f32 rotX;
    Vec3f fireEffectInitialPos = { 0.0f, 0.0f, 0.0f };
    Vec3f fireEffectPos = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachZeroF(&thisv->actor.shape.shadowScale, 0.4f, 0.25f);

    if (thisv->isFallingOver) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->fallTargetPitch, 1, 10000, 0);
        if (fabsf(thisv->actor.shape.rot.y - thisv->fallTargetPitch) < 100.0f) {
            thisv->isFallingOver = false;
        }
    }

    curFrame = thisv->skelAnime.curFrame;
    rotX = curFrame * -3000.0f;
    rotX = CLAMP_MIN(rotX, -11000.0f);

    Matrix_RotateY(BINANG_TO_RAD(thisv->actor.shape.rot.y), MTXMODE_NEW);
    Matrix_RotateX(BINANG_TO_RAD(rotX), MTXMODE_APPLY);
    fireEffectInitialPos.y = Rand_CenteredFloat(10.0f) + 30.0f;
    Matrix_MultVec3f(&fireEffectInitialPos, &fireEffectPos);
    fireEffectPos.x += thisv->actor.world.pos.x + Rand_CenteredFloat(40.0f);
    fireEffectPos.y += thisv->actor.world.pos.y + Rand_CenteredFloat(40.0f);
    fireEffectPos.z += thisv->actor.world.pos.z + Rand_CenteredFloat(30.0f);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 128, 0, 8);
    EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &fireEffectPos, 100, 0, 0, -1);

    if ((thisv->animLastFrame <= curFrame) && (thisv->actor.bgCheckFlags & 1)) {
        Math_ApproachF(&thisv->actor.shape.yOffset, -4230.0f, 0.5f, 300.0f);
        if (thisv->actor.shape.yOffset < -2000.0f) {
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xC0);
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnAnubice_Update(Actor* thisx, GlobalContext* globalCtx) {
    f32 zero;
    BgHidanCurtain* flameCircle;
    s32 i;
    Vec3f sp48;
    Vec3f sp3C;
    EnAnubice* thisv = (EnAnubice*)thisx;

    if ((thisv->actionFunc != EnAnubice_SetupDie) && (thisv->actionFunc != EnAnubice_Die) &&
        (thisv->actor.shape.yOffset == 0.0f)) {
        EnAnubice_Hover(thisv, globalCtx);
        for (i = 0; i < ARRAY_COUNT(thisv->flameCircles); i++) {
            flameCircle = thisv->flameCircles[i];

            if ((flameCircle != NULL) && (fabsf(flameCircle->actor.world.pos.x - thisv->actor.world.pos.x) < 60.0f) &&
                (fabsf(thisv->flameCircles[i]->actor.world.pos.z - thisv->actor.world.pos.z) < 60.0f) &&
                (flameCircle->timer != 0)) {
                Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_ANUBIS_DEAD);
                thisv->actionFunc = EnAnubice_SetupDie;
                return;
            }
        }

        if (thisv->collider.base.acFlags & AC_HIT) {
            thisv->collider.base.acFlags &= ~AC_HIT;
            if (thisv->actor.colChkInfo.damageEffect == ANUBICE_DMGEFF_FIRE) {
                Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_ANUBIS_DEAD);
                thisv->actionFunc = EnAnubice_SetupDie;
                return;
            }

            if (!thisv->isKnockedback) {
                thisv->knockbackTimer = 10;
                thisv->isKnockedback = true;

                sp48.x = 0.0f;
                sp48.y = 0.0f;
                sp48.z = -10.0f;
                sp3C.x = 0.0f;
                sp3C.y = 0.0f;
                sp3C.z = 0.0f;

                Matrix_RotateY(BINANG_TO_RAD(thisv->actor.shape.rot.y), MTXMODE_NEW);
                Matrix_MultVec3f(&sp48, &sp3C);

                thisv->actor.velocity.x = sp3C.x;
                thisv->actor.velocity.z = sp3C.z;
                thisv->knockbackRecoveryVelocity.x = -sp3C.x;
                thisv->knockbackRecoveryVelocity.z = -sp3C.z;

                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_CUTBODY);
            }
        }

        if (thisv->isKnockedback) {
            thisv->actor.shape.rot.y += 6500;
            Math_ApproachF(&thisv->actor.velocity.x, thisv->knockbackRecoveryVelocity.x, 0.3f, 1.0f);
            Math_ApproachF(&thisv->actor.velocity.z, thisv->knockbackRecoveryVelocity.z, 0.3f, 1.0f);

            zero = 0.0f;
            if (zero) {}

            if (thisv->knockbackTimer == 0) {
                thisv->actor.velocity.x = thisv->actor.velocity.z = 0.0f;
                thisv->knockbackRecoveryVelocity.x = thisv->knockbackRecoveryVelocity.z = 0.0f;
                thisv->isKnockedback = false;
            }
        }
    }

    thisv->timeAlive++;

    if (thisv->knockbackTimer != 0) {
        thisv->knockbackTimer--;
    }

    if (thisv->deathTimer != 0) {
        thisv->deathTimer--;
    }

    thisv->actionFunc(thisv, globalCtx);

    thisv->actor.velocity.y += thisv->actor.gravity;
    func_8002D7EC(&thisv->actor);

    if (!thisv->isLinkOutOfRange) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 5.0f, 10.0f, 0x1D);
    } else {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 5.0f, 10.0f, 0x1C);
    }

    if ((thisv->actionFunc != EnAnubice_SetupDie) && (thisv->actionFunc != EnAnubice_Die)) {
        Actor_SetFocus(&thisv->actor, thisv->focusHeightOffset);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

        if (!thisv->isKnockedback && (thisv->actor.shape.yOffset == 0.0f)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

s32 EnAnubice_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnAnubice* thisv = (EnAnubice*)thisx;

    if (limbIndex == ANUBICE_LIMB_HEAD) {
        rot->z += thisv->headRot;
    }

    return false;
}

void EnAnubice_PostLimbDraw(struct GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnAnubice* thisv = (EnAnubice*)thisx;
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == ANUBICE_LIMB_HEAD) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_anubice.c", 853);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_anubice.c", 856),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gAnubiceEyesDL);
        Matrix_MultVec3f(&pos, &thisv->fireballPos);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_anubice.c", 868);
    }
}

void EnAnubice_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubice* thisv = (EnAnubice*)thisx;

    func_80093D84(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnAnubice_OverrideLimbDraw,
                      EnAnubice_PostLimbDraw, thisv);
}
