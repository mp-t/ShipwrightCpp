/*
 * File: z_en_hintnuts.c
 * Overlay: ovl_En_Hintnuts
 * Description: Hint Deku Scrubs (Deku Tree)
 */

#include "z_en_hintnuts.h"
#include "objects/object_hintnuts/object_hintnuts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnHintnuts_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHintnuts_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHintnuts_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHintnuts_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnHintnuts_SetupWait(EnHintnuts* thisv);
void EnHintnuts_Wait(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_LookAround(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Stand(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_ThrowNut(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Burrow(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_BeginRun(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_BeginFreeze(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Run(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Talk(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Leave(EnHintnuts* thisv, GlobalContext* globalCtx);
void EnHintnuts_Freeze(EnHintnuts* thisv, GlobalContext* globalCtx);

const ActorInit En_Hintnuts_InitVars = {
    ACTOR_EN_HINTNUTS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_HINTNUTS,
    sizeof(EnHintnuts),
    (ActorFunc)EnHintnuts_Init,
    (ActorFunc)EnHintnuts_Destroy,
    (ActorFunc)EnHintnuts_Update,
    (ActorFunc)EnHintnuts_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
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
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 18, 32, MASS_HEAVY };

static s16 sPuzzleCounter = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x0A, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2600, ICHAIN_STOP),
};

void EnHintnuts_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHintnuts* thisv = (EnHintnuts*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    if (thisv->actor.params == 0xA) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
    } else {
        ActorShape_Init(&thisv->actor.shape, 0x0, ActorShadow_DrawCircle, 35.0f);
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gHintNutsSkel, &gHintNutsStandAnim, thisv->jointTable,
                       thisv->morphTable, 10);
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
        Actor_SetTextWithPrefix(globalCtx, &thisv->actor, (thisv->actor.params >> 8) & 0xFF);
        thisv->textIdCopy = thisv->actor.textId;
        thisv->actor.params &= 0xFF;
        sPuzzleCounter = 0;
        if (thisv->actor.textId == 0x109B) {
            if (Flags_GetClear(globalCtx, 0x9) != 0) {
                Actor_Kill(&thisv->actor);
                return;
            }
        }
        EnHintnuts_SetupWait(thisv);
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_HINTNUTS, thisv->actor.world.pos.x,
                           thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, thisv->actor.world.rot.y, 0, 0xA);
    }
}

void EnHintnuts_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHintnuts* thisv = (EnHintnuts*)thisx;

    if (thisv->actor.params != 0xA) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void EnHintnuts_HitByScrubProjectile1(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.textId != 0 && thisv->actor.category == ACTORCAT_ENEMY &&
        ((thisv->actor.params == 0) || (sPuzzleCounter == 2))) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_BG);
    }
}

void EnHintnuts_SetupWait(EnHintnuts* thisv) {
    Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gHintNutsUpAnim, 0.0f);
    thisv->animFlagAndTimer = Rand_S16Offset(100, 50);
    thisv->collider.dim.height = 5;
    thisv->actor.world.pos = thisv->actor.home.pos;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = EnHintnuts_Wait;
}

void EnHintnuts_SetupLookAround(EnHintnuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gHintNutsLookAroundAnim);
    thisv->animFlagAndTimer = 2;
    thisv->actionFunc = EnHintnuts_LookAround;
}

void EnHintnuts_SetupThrowScrubProjectile(EnHintnuts* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gHintNutsSpitAnim);
    thisv->actionFunc = EnHintnuts_ThrowNut;
}

void EnHintnuts_SetupStand(EnHintnuts* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gHintNutsStandAnim, -3.0f);
    if (thisv->actionFunc == EnHintnuts_ThrowNut) {
        thisv->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        thisv->animFlagAndTimer = 1;
    }
    thisv->actionFunc = EnHintnuts_Stand;
}

void EnHintnuts_SetupBurrow(EnHintnuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gHintNutsBurrowAnim, -5.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DOWN);
    thisv->actionFunc = EnHintnuts_Burrow;
}

void EnHintnuts_HitByScrubProjectile2(EnHintnuts* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gHintNutsUnburrowAnim, -3.0f);
    thisv->collider.dim.height = 37;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
    thisv->collider.base.acFlags &= ~AC_ON;

    if (thisv->actor.params > 0 && thisv->actor.params < 4 && thisv->actor.category == ACTORCAT_ENEMY) {
        if (sPuzzleCounter == -4) {
            sPuzzleCounter = 0;
        }
        if (thisv->actor.params == sPuzzleCounter + 1) {
            sPuzzleCounter++;
        } else {
            if (sPuzzleCounter > 0) {
                sPuzzleCounter = -sPuzzleCounter;
            }
            sPuzzleCounter--;
        }
        thisv->actor.flags |= ACTOR_FLAG_4;
        thisv->actionFunc = EnHintnuts_BeginFreeze;
    } else {
        thisv->actionFunc = EnHintnuts_BeginRun;
    }
}

void EnHintnuts_SetupRun(EnHintnuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gHintNutsRunAnim);
    thisv->animFlagAndTimer = 5;
    thisv->actionFunc = EnHintnuts_Run;
}

void EnHintnuts_SetupTalk(EnHintnuts* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gHintNutsTalkAnim, -5.0f);
    thisv->actionFunc = EnHintnuts_Talk;
    thisv->actor.speedXZ = 0.0f;
}

void EnHintnuts_SetupLeave(EnHintnuts* thisv, GlobalContext* globalCtx) {
    Animation_MorphToLoop(&thisv->skelAnime, &gHintNutsRunAnim, -5.0f);
    thisv->actor.speedXZ = 3.0f;
    thisv->animFlagAndTimer = 100;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->collider.base.ocFlags1 &= ~OC1_ON;
    thisv->actor.flags |= ACTOR_FLAG_4;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ITEM00, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                thisv->actor.world.pos.z, 0x0, 0x0, 0x0, 0x3); // recovery heart
    thisv->actionFunc = EnHintnuts_Leave;
}

void EnHintnuts_SetupFreeze(EnHintnuts* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gHintNutsFreezeAnim);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 100);
    thisv->actor.colorFilterTimer = 1;
    thisv->animFlagAndTimer = 0;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_FAINT);
    if (sPuzzleCounter == -3) {
        func_80078884(NA_SE_SY_ERROR);
        sPuzzleCounter = -4;
    }
    thisv->actionFunc = EnHintnuts_Freeze;
}

void EnHintnuts_Wait(EnHintnuts* thisv, GlobalContext* globalCtx) {
    s32 hasSlowPlaybackSpeed = false;

    if (thisv->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (thisv->animFlagAndTimer != 0)) {
        thisv->animFlagAndTimer--;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 9.0f)) {
        thisv->collider.base.acFlags |= AC_ON;
    } else if (Animation_OnFrame(&thisv->skelAnime, 8.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
    }

    thisv->collider.dim.height = 5.0f + ((CLAMP(thisv->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f);
    if (!hasSlowPlaybackSpeed && (thisv->actor.xzDistToPlayer < 120.0f)) {
        EnHintnuts_SetupBurrow(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.xzDistToPlayer < 120.0f) {
            EnHintnuts_SetupBurrow(thisv);
        } else if ((thisv->animFlagAndTimer == 0) && (thisv->actor.xzDistToPlayer > 320.0f)) {
            EnHintnuts_SetupLookAround(thisv);
        } else {
            EnHintnuts_SetupStand(thisv);
        }
    }
    if (hasSlowPlaybackSpeed && 160.0f < thisv->actor.xzDistToPlayer && fabsf(thisv->actor.yDistToPlayer) < 120.0f &&
        ((thisv->animFlagAndTimer == 0) || (thisv->actor.xzDistToPlayer < 480.0f))) {
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnHintnuts_LookAround(EnHintnuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->animFlagAndTimer != 0) {
        thisv->animFlagAndTimer--;
    }
    if ((thisv->actor.xzDistToPlayer < 120.0f) || (thisv->animFlagAndTimer == 0)) {
        EnHintnuts_SetupBurrow(thisv);
    }
}

void EnHintnuts_Stand(EnHintnuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->animFlagAndTimer != 0) {
        thisv->animFlagAndTimer--;
    }
    if (!(thisv->animFlagAndTimer & 0x1000)) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if (thisv->actor.xzDistToPlayer < 120.0f || thisv->animFlagAndTimer == 0x1000) {
        EnHintnuts_SetupBurrow(thisv);
    } else if (thisv->animFlagAndTimer == 0) {
        EnHintnuts_SetupThrowScrubProjectile(thisv);
    }
}

void EnHintnuts_ThrowNut(EnHintnuts* thisv, GlobalContext* globalCtx) {
    Vec3f nutPos;

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
    if (thisv->actor.xzDistToPlayer < 120.0f) {
        EnHintnuts_SetupBurrow(thisv);
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        EnHintnuts_SetupStand(thisv);
    } else if (Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        nutPos.x = thisv->actor.world.pos.x + (Math_SinS(thisv->actor.shape.rot.y) * 23.0f);
        nutPos.y = thisv->actor.world.pos.y + 12.0f;
        nutPos.z = thisv->actor.world.pos.z + (Math_CosS(thisv->actor.shape.rot.y) * 23.0f);
        if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_NUTSBALL, nutPos.x, nutPos.y, nutPos.z,
                        thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 1) != NULL) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
        }
    }
}

void EnHintnuts_Burrow(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnHintnuts_SetupWait(thisv);
    } else {
        thisv->collider.dim.height = 5.0f + ((3.0f - CLAMP(thisv->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 4.0f)) {
        thisv->collider.base.acFlags &= ~AC_ON;
    }

    Math_ApproachF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 0.5f, 3.0f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 0.5f, 3.0f);
}

void EnHintnuts_BeginRun(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_196 = thisv->actor.yawTowardsPlayer + 0x8000;
        EnHintnuts_SetupRun(thisv);
    }
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0xE38);
}

void EnHintnuts_BeginFreeze(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnHintnuts_SetupFreeze(thisv);
    }
}

void EnHintnuts_CheckProximity(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.category != ACTORCAT_ENEMY) {
        if ((thisv->collider.base.ocFlags1 & OC1_HIT) || thisv->actor.isTargeted) {
            thisv->actor.flags |= ACTOR_FLAG_16;
        } else {
            thisv->actor.flags &= ~ACTOR_FLAG_16;
        }
        if (thisv->actor.xzDistToPlayer < 130.0f) {
            thisv->actor.textId = thisv->textIdCopy;
            func_8002F2F4(&thisv->actor, globalCtx);
        }
    }
}

void EnHintnuts_Run(EnHintnuts* thisv, GlobalContext* globalCtx) {
    s32 temp_ret;
    s16 diffRotInit;
    s16 diffRot;
    f32 phi_f0;

    SkelAnime_Update(&thisv->skelAnime);
    temp_ret = Animation_OnFrame(&thisv->skelAnime, 0.0f);
    if (temp_ret != 0 && thisv->animFlagAndTimer != 0) {
        thisv->animFlagAndTimer--;
    }
    if ((temp_ret != 0) || (Animation_OnFrame(&thisv->skelAnime, 6.0f))) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }

    Math_StepToF(&thisv->actor.speedXZ, 7.5f, 1.0f);
    if (Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_196, 1, 0xE38, 0xB6) == 0) {
        if (thisv->actor.bgCheckFlags & 0x20) {
            thisv->unk_196 = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
        } else if (thisv->actor.bgCheckFlags & 8) {
            thisv->unk_196 = thisv->actor.wallYaw;
        } else if (thisv->animFlagAndTimer == 0) {
            diffRotInit = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
            diffRot = diffRotInit - thisv->actor.yawTowardsPlayer;
            if (ABS(diffRot) >= 0x2001) {
                thisv->unk_196 = diffRotInit;
            } else {
                phi_f0 = (0.0f <= (f32)diffRot) ? 1.0f : -1.0f;
                thisv->unk_196 = (s16)((phi_f0 * -8192.0f) + (f32)thisv->actor.yawTowardsPlayer);
            }
        } else {
            thisv->unk_196 = (s16)(thisv->actor.yawTowardsPlayer + 0x8000);
        }
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        EnHintnuts_SetupTalk(thisv);
    } else if (thisv->animFlagAndTimer == 0 && Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) < 20.0f &&
               fabsf(thisv->actor.world.pos.y - thisv->actor.home.pos.y) < 2.0f) {
        thisv->actor.speedXZ = 0.0f;
        if (thisv->actor.category == ACTORCAT_BG) {
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_16);
            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
        }
        EnHintnuts_SetupBurrow(thisv);
    } else {
        EnHintnuts_CheckProximity(thisv, globalCtx);
    }
}

void EnHintnuts_Talk(EnHintnuts* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0x3, 0x400, 0x100);
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) {
        EnHintnuts_SetupLeave(thisv, globalCtx);
    }
}

void EnHintnuts_Leave(EnHintnuts* thisv, GlobalContext* globalCtx) {
    s16 temp_a1;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->animFlagAndTimer != 0) {
        thisv->animFlagAndTimer--;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if (thisv->actor.bgCheckFlags & 8) {
        temp_a1 = thisv->actor.wallYaw;
    } else {
        temp_a1 = thisv->actor.yawTowardsPlayer - Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - 0x8000;
        if (ABS(temp_a1) >= 0x4001) {
            temp_a1 = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000;
        } else {
            temp_a1 = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - (temp_a1 >> 1) + 0x8000;
        }
    }
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, temp_a1, 0x800);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if ((thisv->animFlagAndTimer == 0) || (thisv->actor.projectedPos.z < 0.0f)) {
        Message_CloseTextbox(globalCtx);
        if (thisv->actor.params == 3) {
            Flags_SetClear(globalCtx, thisv->actor.room);
            sPuzzleCounter = 3;
        }
        if (thisv->actor.child != NULL) {
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisv->actor.child, ACTORCAT_PROP);
        }
        Actor_Kill(&thisv->actor);
    }
}

void EnHintnuts_Freeze(EnHintnuts* thisv, GlobalContext* globalCtx) {
    thisv->actor.colorFilterTimer = 1;
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_FAINT);
    }
    if (thisv->animFlagAndTimer == 0) {
        if (sPuzzleCounter == 3) {
            if (thisv->actor.child != NULL) {
                Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisv->actor.child, ACTORCAT_PROP);
            }
            thisv->animFlagAndTimer = 1;
        } else if (sPuzzleCounter == -4) {
            thisv->animFlagAndTimer = 2;
        }
    } else if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y - 35.0f, 7.0f) != 0) {
        if (thisv->animFlagAndTimer == 1) {
            Actor_Kill(&thisv->actor);
        } else {
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->actor.flags &= ~ACTOR_FLAG_4;
            thisv->actor.colChkInfo.health = sColChkInfoInit.health;
            thisv->actor.colorFilterTimer = 0;
            EnHintnuts_SetupWait(thisv);
        }
    }
}

void EnHintnuts_ColliderCheck(EnHintnuts* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if (thisv->collider.base.ac->id != ACTOR_EN_NUTSBALL) {
            EnHintnuts_SetupBurrow(thisv);
        } else {
            EnHintnuts_HitByScrubProjectile1(thisv, globalCtx);
            EnHintnuts_HitByScrubProjectile2(thisv);
        }
    } else if (globalCtx->actorCtx.unk_02 != 0) {
        EnHintnuts_HitByScrubProjectile1(thisv, globalCtx);
        EnHintnuts_HitByScrubProjectile2(thisv);
    }
}

void EnHintnuts_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHintnuts* thisv = (EnHintnuts*)thisx;
    s32 pad;

    if (thisv->actor.params != 0xA) {
        EnHintnuts_ColliderCheck(thisv, globalCtx);
        thisv->actionFunc(thisv, globalCtx);
        if (thisv->actionFunc != EnHintnuts_Freeze && thisv->actionFunc != EnHintnuts_BeginFreeze) {
            Actor_MoveForward(&thisv->actor);
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, thisv->collider.dim.radius,
                                    thisv->collider.dim.height, 0x1D);
        }
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        if (thisv->actionFunc == EnHintnuts_Wait) {
            Actor_SetFocus(&thisv->actor, thisv->skelAnime.curFrame);
        } else if (thisv->actionFunc == EnHintnuts_Burrow) {
            Actor_SetFocus(&thisv->actor,
                           20.0f - ((thisv->skelAnime.curFrame * 20.0f) / Animation_GetLastFrame(&gHintNutsBurrowAnim)));
        } else {
            Actor_SetFocus(&thisv->actor, 20.0f);
        }
    }
}

s32 EnHintnuts_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    Vec3f vec;
    f32 curFrame;
    EnHintnuts* thisv = (EnHintnuts*)thisx;

    if (limbIndex == 5 && thisv->actionFunc == EnHintnuts_ThrowNut) {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            vec.y = 1.0f - (curFrame * 0.0833f);
            vec.z = 1.0f + (curFrame * 0.1167f);
            vec.x = 1.0f + (curFrame * 0.1167f);
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            vec.y = 0.5f + curFrame;
            vec.z = 1.7f - (curFrame * 0.7f);
            vec.x = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            vec.y = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            vec.z = 1.0f;
            vec.x = 1.0f;
        } else {
            return false;
        }
        Matrix_Scale(vec.x, vec.y, vec.z, MTXMODE_APPLY);
    }
    return false;
}

void EnHintnuts_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHintnuts* thisv = (EnHintnuts*)thisx;

    if (thisv->actor.params == 0xA) {
        Gfx_DrawDListOpa(globalCtx, gHintNutsFlowerDL);
    } else {
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnHintnuts_OverrideLimbDraw,
                          NULL, thisv);
    }
}
