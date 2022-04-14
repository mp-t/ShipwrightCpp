/*
 * File: z_en_fw.c
 * Overlay: ovl_En_Fw
 * Description: Flare Dancer Core
 */

#include "z_en_fw.h"
#include "objects/object_fw/object_fw.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_9)

void EnFw_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFw_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFw_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnFw_UpdateDust(EnFw* thisv);
void EnFw_DrawDust(EnFw* thisv, GlobalContext* globalCtx);
void EnFw_AddDust(EnFw* thisv, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, u8 initialTimer, f32 scale,
                  f32 scaleStep);
void EnFw_Bounce(EnFw* thisv, GlobalContext* globalCtx);
void EnFw_Run(EnFw* thisv, GlobalContext* globalCtx);
void EnFw_JumpToParentInitPos(EnFw* thisv, GlobalContext* globalCtx);
void EnFw_TurnToParentInitPos(EnFw* thisv, GlobalContext* globalCtx);

const ActorInit En_Fw_InitVars = {
    ACTOR_EN_FW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_FW,
    sizeof(EnFw),
    (ActorFunc)EnFw_Init,
    (ActorFunc)EnFw_Destroy,
    (ActorFunc)EnFw_Update,
    (ActorFunc)EnFw_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x04 },
            { 0xFFCFFFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 2, { { 1200, 0, 0 }, 16 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 D_80A1FB94 = { 8, 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENFW_ANIM_0,
    /* 1 */ ENFW_ANIM_1,
    /* 2 */ ENFW_ANIM_2
} EnFwAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gFlareDancerCoreInitRunCycleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, 0.0f },
    { &gFlareDancerCoreRunCycleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &gFlareDancerCoreEndRunCycleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
};

s32 EnFw_DoBounce(EnFw* thisv, s32 totalBounces, f32 yVelocity) {
    s16 temp_v1;

    if (!(thisv->actor.bgCheckFlags & 1) || (thisv->actor.velocity.y > 0.0f)) {
        // not on the ground or moving upwards.
        return false;
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
    thisv->bounceCnt--;
    if (thisv->bounceCnt <= 0) {
        if (thisv->bounceCnt == 0) {
            thisv->bounceCnt = 0;
            thisv->actor.velocity.y = 0.0f;
            return true;
        }
        thisv->bounceCnt = totalBounces;
    }
    thisv->actor.velocity.y = yVelocity;
    thisv->actor.velocity.y *= ((f32)thisv->bounceCnt / totalBounces);
    return 1;
}

s32 EnFw_PlayerInRange(EnFw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    CollisionPoly* poly;
    s32 bgId;
    Vec3f collisionPos;

    if (thisv->actor.xzDistToPlayer > 300.0f) {
        return false;
    }

    if (ABS((s16)((f32)thisv->actor.yawTowardsPlayer - (f32)thisv->actor.shape.rot.y)) > 0x1C70) {
        return false;
    }

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &player->actor.world.pos, &collisionPos,
                                &poly, true, false, false, true, &bgId)) {
        return false;
    }

    return true;
}

Vec3f* EnFw_GetPosAdjAroundCircle(Vec3f* dst, EnFw* thisv, f32 radius, s16 dir) {
    s16 angle;
    Vec3f posAdj;

    // increase rotation around circle ~30 degrees.
    angle = Math_Vec3f_Yaw(&thisv->actor.parent->home.pos, &thisv->actor.world.pos) + (dir * 0x1554);
    posAdj.x = (Math_SinS(angle) * radius) + thisv->actor.parent->home.pos.x;
    posAdj.z = (Math_CosS(angle) * radius) + thisv->actor.parent->home.pos.z;
    posAdj.x -= thisv->actor.world.pos.x;
    posAdj.z -= thisv->actor.world.pos.z;
    *dst = posAdj;
    return dst;
}

s32 EnFw_CheckCollider(EnFw* thisv, GlobalContext* globalCtx) {
    ColliderInfo* info;
    s32 phi_return;

    if (thisv->collider.base.acFlags & AC_HIT) {
        info = &thisv->collider.elements[0].info;
        if (info->acHitInfo->toucher.dmgFlags & 0x80) {
            thisv->lastDmgHook = true;
        } else {
            thisv->lastDmgHook = false;
        }
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (Actor_ApplyDamage(&thisv->actor) <= 0) {
            if (thisv->actor.parent->colChkInfo.health <= 8) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                thisv->actor.parent->colChkInfo.health = 0;
            } else {
                thisv->actor.parent->colChkInfo.health -= 8;
            }
            thisv->returnToParentTimer = 0;
        }
        return true;
    } else {
        return false;
    }
}

s32 EnFw_SpawnDust(EnFw* thisv, u8 timer, f32 scale, f32 scaleStep, s32 dustCnt, f32 radius, f32 xzAccel, f32 yAccel) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s16 angle;
    s32 i;

    pos = thisv->actor.world.pos;
    pos.y = thisv->actor.floorHeight + 2.0f;
    angle = ((Rand_ZeroOne() - 0.5f) * 0x10000);
    i = dustCnt;
    while (i >= 0) {
        accel.x = (Rand_ZeroOne() - 0.5f) * xzAccel;
        accel.y = yAccel;
        accel.z = (Rand_ZeroOne() - 0.5f) * xzAccel;
        pos.x = (Math_SinS(angle) * radius) + thisv->actor.world.pos.x;
        pos.z = (Math_CosS(angle) * radius) + thisv->actor.world.pos.z;
        EnFw_AddDust(thisv, &pos, &velocity, &accel, timer, scale, scaleStep);
        angle += (s16)(0x10000 / dustCnt);
        i--;
    }
    return 0;
}

void EnFw_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFw* thisv = (EnFw*)thisx;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gFlareDancerCoreSkel, NULL, thisv->jointTable, thisv->morphTable,
                       11);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFW_ANIM_0);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->sphs);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(0x10), &D_80A1FB94);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->runDirection = -thisv->actor.params;
    thisv->actionFunc = EnFw_Bounce;
    thisv->actor.gravity = -1.0f;
}

void EnFw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFw* thisv = (EnFw*)thisx;
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnFw_Bounce(EnFw* thisv, GlobalContext* globalCtx) {
    if (EnFw_DoBounce(thisv, 3, 8.0f) && thisv->bounceCnt == 0) {
        thisv->returnToParentTimer = Rand_S16Offset(300, 150);
        thisv->actionFunc = EnFw_Run;
    }
}

void EnFw_Run(EnFw* thisv, GlobalContext* globalCtx) {
    f32 tmpAngle;
    s16 phi_v0;
    f32 facingDir;
    EnBom* bomb;
    Actor* flareDancer;

    Math_SmoothStepToF(&thisv->skelAnime.playSpeed, 1.0f, 0.1f, 1.0f, 0.0f);
    if (thisv->skelAnime.animation == &gFlareDancerCoreInitRunCycleAnim) {
        if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame) == 0) {
            thisv->runRadius = Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->actor.parent->world.pos);
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFW_ANIM_2);
        }
        return;
    }

    if (thisv->damageTimer == 0 && thisv->explosionTimer == 0 && EnFw_CheckCollider(thisv, globalCtx)) {
        if (thisv->actor.parent->colChkInfo.health > 0) {
            if (!thisv->lastDmgHook) {
                thisv->actor.velocity.y = 6.0f;
            }
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_MAN_DAMAGE);
            thisv->damageTimer = 20;
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_MAN_DAMAGE);
            thisv->explosionTimer = 6;
        }
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->explosionTimer != 0) {
        thisv->skelAnime.playSpeed = 0.0f;
        Math_SmoothStepToF(&thisv->actor.scale.x, 0.024999999f, 0.08f, 0.6f, 0.0f);
        Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
        if (thisv->actor.colorFilterTimer == 0) {
            Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, thisv->explosionTimer);
            thisv->explosionTimer--;
        }

        if (thisv->explosionTimer == 0) {
            bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->bompPos.x, thisv->bompPos.y,
                                       thisv->bompPos.z, 0, 0, 0x600, 0);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
            flareDancer = thisv->actor.parent;
            flareDancer->params |= 0x4000;
            Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0xA0);
            Actor_Kill(&thisv->actor);
            return;
        }
    } else {
        if (!(thisv->actor.bgCheckFlags & 1) || thisv->actor.velocity.y > 0.0f) {
            Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, thisv->damageTimer);
            return;
        }
        DECR(thisv->damageTimer);
        if ((200.0f - thisv->runRadius) < 0.9f) {
            if (DECR(thisv->returnToParentTimer) == 0) {
                thisv->actor.speedXZ = 0.0f;
                thisv->actionFunc = EnFw_TurnToParentInitPos;
                return;
            }
        }

        // Run outwards until the radius of the run circle is 200
        Math_SmoothStepToF(&thisv->runRadius, 200.0f, 0.3f, 100.0f, 0.0f);

        if (thisv->turnAround) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 1.0f, 0.0f);
            tmpAngle = (s16)(thisv->actor.world.rot.y ^ 0x8000);
            facingDir = thisv->actor.shape.rot.y;
            tmpAngle = Math_SmoothStepToF(&facingDir, tmpAngle, 0.1f, 10000.0f, 0.0f);
            thisv->actor.shape.rot.y = facingDir;
            if (tmpAngle > 0x1554) {
                return;
            }
            thisv->turnAround = false;
        } else {
            Vec3f sp48;
            EnFw_GetPosAdjAroundCircle(&sp48, thisv, thisv->runRadius, thisv->runDirection);
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, (Math_FAtan2F(sp48.x, sp48.z) * (0x8000 / std::numbers::pi_v<float>)), 4, 0xFA0, 1);
        }

        thisv->actor.world.rot = thisv->actor.shape.rot;

        if (thisv->slideTimer == 0 && EnFw_PlayerInRange(thisv, globalCtx)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_MAN_SURP);
            thisv->slideSfxTimer = 8;
            thisv->slideTimer = 8;
        }

        if (thisv->slideTimer != 0) {
            if (DECR(thisv->slideSfxTimer) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_MAN_SLIDE);
                thisv->slideSfxTimer = 4;
            }
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 1.0f, 0.0f);
            thisv->skelAnime.playSpeed = 0.0f;
            EnFw_SpawnDust(thisv, 8, 0.16f, 0.2f, 3, 8.0f, 20.0f, ((Rand_ZeroOne() - 0.5f) * 0.2f) + 0.3f);
            thisv->slideTimer--;
            if (thisv->slideTimer == 0) {
                thisv->turnAround = true;
                thisv->runDirection = -thisv->runDirection;
            }
        } else {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 6.0f, 0.1f, 1.0f, 0.0f);
            phi_v0 = thisv->skelAnime.curFrame;
            if (phi_v0 == 1 || phi_v0 == 4) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FLAME_MAN_RUN);
                EnFw_SpawnDust(thisv, 8, 0.16f, 0.1f, 1, 0.0f, 20.0f, 0.0f);
            }
        }
    }
}

void EnFw_TurnToParentInitPos(EnFw* thisv, GlobalContext* globalCtx) {
    s16 angleToParentInit;

    angleToParentInit = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.parent->home.pos);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, angleToParentInit, 4, 0xFA0, 1);
    if (ABS(angleToParentInit - thisv->actor.shape.rot.y) < 0x65) {
        // angle to parent init pos is ~0.5 degrees
        thisv->actor.world.rot = thisv->actor.shape.rot;
        thisv->actor.velocity.y = 14.0f;
        thisv->actor.home.pos = thisv->actor.world.pos;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENFW_ANIM_1);
        thisv->actionFunc = EnFw_JumpToParentInitPos;
    }
}

void EnFw_JumpToParentInitPos(EnFw* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1 && thisv->actor.velocity.y <= 0.0f) {
        thisv->actor.parent->params |= 0x8000;
        Actor_Kill(&thisv->actor);
    } else {
        Math_SmoothStepToF(&thisv->actor.world.pos.x, thisv->actor.parent->home.pos.x, 0.6f, 8.0f, 0.0f);
        Math_SmoothStepToF(&thisv->actor.world.pos.z, thisv->actor.parent->home.pos.z, 0.6f, 8.0f, 0.0f);
    }
}

void EnFw_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFw* thisv = (EnFw*)thisx;
    SkelAnime_Update(&thisv->skelAnime);
    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        // not attached to hookshot.
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 20.0f, 0.0f, 5);
        thisv->actionFunc(thisv, globalCtx);
        if (thisv->damageTimer == 0 && thisv->explosionTimer == 0 && thisv->actionFunc == EnFw_Run) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

s32 EnFw_OverrideLimbDraw(GlobalContext* globalContext, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                          void* thisx) {
    return false;
}

void EnFw_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFw* thisv = (EnFw*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 2) {
        // body
        Matrix_MultVec3f(&zeroVec, &thisv->bompPos);
    }

    if (limbIndex == 3) {
        // head
        Matrix_MultVec3f(&zeroVec, &thisv->actor.focus.pos);
    }

    Collider_UpdateSpheres(limbIndex, &thisv->collider);
}

void EnFw_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnFw* thisv = (EnFw*)thisx;

    EnFw_UpdateDust(thisv);
    Matrix_Push();
    EnFw_DrawDust(thisv, globalCtx);
    Matrix_Pop();
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnFw_OverrideLimbDraw, EnFw_PostLimbDraw, thisv);
}

void EnFw_AddDust(EnFw* thisv, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, u8 initialTimer, f32 scale,
                  f32 scaleStep) {
    EnFwEffect* eff = thisv->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type != 1) {
            eff->scale = scale;
            eff->scaleStep = scaleStep;
            eff->initialTimer = eff->timer = initialTimer;
            eff->type = 1;
            eff->pos = *initialPos;
            eff->accel = *accel;
            eff->velocity = *initialSpeed;
            return;
        }
    }
}

void EnFw_UpdateDust(EnFw* thisv) {
    EnFwEffect* eff = thisv->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type != 0) {
            if ((--eff->timer) == 0) {
                eff->type = 0;
            }
            eff->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;
            eff->scale += eff->scaleStep;
        }
    }
}

void EnFw_DrawDust(EnFw* thisv, GlobalContext* globalCtx) {
    static void* dustTextures[] = {
        gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
    };
    EnFwEffect* eff = thisv->effects;
    s16 firstDone;
    s16 alpha;
    s16 i;
    s16 idx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fw.c", 1191);

    firstDone = false;
    func_80093D84(globalCtx->state.gfxCtx);
    if (1) {}

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++, eff++) {
        if (eff->type != 0) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0U);
                gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_7928);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
                firstDone = true;
            }

            alpha = eff->timer * (255.0f / eff->initialTimer);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fw.c", 1229),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            idx = eff->timer * (8.0f / eff->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(dustTextures[idx]));
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerSquareParticleDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fw.c", 1243);
}
