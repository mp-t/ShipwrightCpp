/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Running Man (adult)
 */

#include "z_en_mm2.h"
#include "vt.h"
#include "objects/object_mm/object_mm.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ RM2_ANIM_RUN,
    /* 1 */ RM2_ANIM_SIT,
    /* 2 */ RM2_ANIM_SIT_WAIT,
    /* 3 */ RM2_ANIM_STAND,
    /* 4 */ RM2_ANIM_SPRINT,
    /* 5 */ RM2_ANIM_EXCITED, // plays when talking to him with bunny hood on
    /* 6 */ RM2_ANIM_HAPPY    // plays when you sell him the bunny hood
} RunningManAnimIndex;

typedef enum {
    /* 0 */ RM2_MOUTH_CLOSED,
    /* 1 */ RM2_MOUTH_OPEN
} RunningManMouthTex;

void EnMm2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMm2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMm2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMm2_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_80AAF3C0(EnMm2* thisv, GlobalContext* globalCtx);
void func_80AAF57C(EnMm2* thisv, GlobalContext* globalCtx);
void func_80AAF668(EnMm2* thisv, GlobalContext* globalCtx);
s32 EnMm2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnMm2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

const ActorInit En_Mm2_InitVars = {
    ACTOR_EN_MM2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MM,
    sizeof(EnMm2),
    (ActorFunc)EnMm2_Init,
    (ActorFunc)EnMm2_Destroy,
    (ActorFunc)EnMm2_Update,
    (ActorFunc)EnMm2_Draw,
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
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 63, 0, { 0, 0, 0 } },
};

static AnimationSpeedInfo sAnimationInfo[] = {
    { &gRunningManRunAnim, 1.0f, ANIMMODE_LOOP, -7.0f },     { &gRunningManSitStandAnim, -1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSitWaitAnim, 1.0f, ANIMMODE_LOOP, -7.0f }, { &gRunningManSitStandAnim, 1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSprintAnim, 1.0f, ANIMMODE_LOOP, -7.0f },  { &gRunningManExcitedAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
    { &gRunningManHappyAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_STOP),
};

void EnMm2_ChangeAnim(EnMm2* thisv, s32 index, s32* currentIndex) {
    f32 phi_f0;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        phi_f0 = 0.0f;
    } else {
        phi_f0 = sAnimationInfo[index].morphFrames;
    }

    if (sAnimationInfo[index].playSpeed >= 0.0f) {
        Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed, 0.0f,
                         (f32)Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode,
                         phi_f0);
    } else {
        Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed,
                         (f32)Animation_GetLastFrame(sAnimationInfo[index].animation), 0.0f, sAnimationInfo[index].mode,
                         phi_f0);
    }
    *currentIndex = index;
}

void func_80AAEF70(EnMm2* thisv, GlobalContext* globalCtx) {
    if ((gSaveContext.eventChkInf[9] & 0xF) != 0xF) {
        thisv->actor.textId = 0x6086;
    } else if (gSaveContext.infTable[23] & 0x8000) {
        if (gSaveContext.eventInf[1] & 1) {
            thisv->actor.textId = 0x6082;
        } else if (gSaveContext.timer2State != 0) {
            thisv->actor.textId = 0x6076;
        } else if (HIGH_SCORE(HS_MARATHON) == 158) {
            thisv->actor.textId = 0x607E;
        } else {
            thisv->actor.textId = 0x6081;
        }
    } else if (gSaveContext.timer2State) {
        thisv->actor.textId = 0x6076;
    } else {
        thisv->actor.textId = 0x607D;
        gSaveContext.eventInf[1] &= ~1;
        HIGH_SCORE(HS_MARATHON) = 158;
    }
}

void EnMm2_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnMm2* thisv = (EnMm2*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 21.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gRunningManSkel, NULL, thisv->jointTable, thisv->morphTable, 16);
    Animation_Change(&thisv->skelAnime, sAnimationInfo[RM2_ANIM_SIT_WAIT].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[RM2_ANIM_SIT_WAIT].animation),
                     sAnimationInfo[RM2_ANIM_SIT_WAIT].mode, sAnimationInfo[RM2_ANIM_SIT_WAIT].morphFrames);
    thisv->previousAnimation = RM2_ANIM_SIT_WAIT;
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->mouthTexIndex = RM2_MOUTH_CLOSED;
    thisv->actor.targetMode = 6;
    thisv->unk_1F4 |= 1;
    thisv->actor.gravity = -1.0f;
    if (thisv->actor.params == 1) {
        thisv->actionFunc = func_80AAF668;
    } else {
        func_80AAEF70(thisv, globalCtx);
        thisv->actionFunc = func_80AAF57C;
    }
    if (!LINK_IS_ADULT) {
        Actor_Kill(&thisv->actor);
    }
    if (thisv->actor.params == 1) {
        if (!(gSaveContext.infTable[23] & 0x8000) || !(gSaveContext.eventInf[1] & 1)) {
            osSyncPrintf(VT_FGCOL(CYAN) " マラソン 開始されていない \n" VT_RST "\n");
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnMm2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMm2* thisv = (EnMm2*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80AAF224(EnMm2* thisv, GlobalContext* globalCtx, EnMm2ActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = actionFunc;
        return 1;
    }
    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if ((ABS(yawDiff) <= 0x4300) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
    return 0;
}

void func_80AAF2BC(EnMm2* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1F6 > 60) {
        Actor_Kill(&thisv->actor);
    }
    SkelAnime_Update(&thisv->skelAnime);
    thisv->unk_1F6++;
    Math_SmoothStepToF(&thisv->actor.speedXZ, 10.0f, 0.6f, 2.0f, 0.0f);
}

void func_80AAF330(EnMm2* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actionFunc = func_80AAF2BC;
        EnMm2_ChangeAnim(thisv, RM2_ANIM_RUN, &thisv->previousAnimation);
        thisv->mouthTexIndex = RM2_MOUTH_OPEN;
        if (!(thisv->unk_1F4 & 2)) {
            Message_CloseTextbox(globalCtx);
        }
        gSaveContext.timer2State = 0;
        gSaveContext.eventInf[1] &= ~1;
    }
}

void func_80AAF3C0(EnMm2* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    switch (thisv->actor.textId) {
        case 0x607D:
        case 0x607E:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                switch (globalCtx->msgCtx.choiceIndex) {
                    case 0:
                        Message_ContinueTextbox(globalCtx, 0x607F);
                        thisv->actor.textId = 0x607F;
                        gSaveContext.eventInf[1] |= 1;
                        break;
                    case 1:
                        Message_ContinueTextbox(globalCtx, 0x6080);
                        thisv->actor.textId = 0x6080;
                        break;
                };
                if (thisv->unk_1F4 & 4) {
                    if (1) {}
                    thisv->unk_1F4 &= ~4;
                    HIGH_SCORE(HS_MARATHON) += 1;
                }
            }
            return;
        case 0x6081:
            if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                thisv->unk_1F4 |= 4;
                HIGH_SCORE(HS_MARATHON) -= 1;
                Message_ContinueTextbox(globalCtx, 0x607E);
                thisv->actor.textId = 0x607E;
            }
            return;
    }

    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        if (thisv->actor.textId == 0x607F) {
            func_80088AA0(0);
            thisv->actionFunc = func_80AAF57C;
        } else {
            thisv->actionFunc = func_80AAF57C;
        }
        thisv->actionFunc = func_80AAF57C;
        func_80AAEF70(thisv, globalCtx);
    }
}

void func_80AAF57C(EnMm2* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    func_80AAEF70(thisv, globalCtx);
    if ((func_80AAF224(thisv, globalCtx, func_80AAF3C0)) && (thisv->actor.textId == 0x607D)) {
        gSaveContext.infTable[23] |= 0x8000;
    }
}

void func_80AAF5EC(EnMm2* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->unk_1F4 &= ~1;
        EnMm2_ChangeAnim(thisv, RM2_ANIM_STAND, &thisv->previousAnimation);
        thisv->actionFunc = func_80AAF330;
    }
}

void func_80AAF668(EnMm2* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y = -0x3E80;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    SkelAnime_Update(&thisv->skelAnime);
    if (((void)0, gSaveContext.timer2Value) < HIGH_SCORE(HS_MARATHON)) {
        thisv->actor.textId = 0x6085;
    } else {
        thisv->actor.textId = 0x6084;
    }
    if (func_80AAF224(thisv, globalCtx, func_80AAF5EC)) {
        thisv->unk_1F6 = 0;
        if (((void)0, gSaveContext.timer2Value) < HIGH_SCORE(HS_MARATHON)) {
            HIGH_SCORE(HS_MARATHON) = gSaveContext.timer2Value;
        }
    } else {
        LOG_HEX("((z_common_data.event_inf[1]) & (0x0001))", gSaveContext.eventInf[1] & 1, "../z_en_mm2.c", 541);
        if (!(gSaveContext.eventInf[1] & 1)) {
            thisv->unk_1F4 |= 2;
            thisv->unk_1F4 &= ~1;
            EnMm2_ChangeAnim(thisv, RM2_ANIM_STAND, &thisv->previousAnimation);
            thisv->actionFunc = func_80AAF330;
        }
    }
}

void EnMm2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMm2* thisv = (EnMm2*)thisx;
    s32 pad;

    if (thisv->unk_1F4 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_1E8, &thisv->unk_1EE, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->unk_1E8.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_1E8.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_1EE.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_1EE.y, 0, 6, 6200, 100);
    }
    thisv->actionFunc(thisv, globalCtx);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
}

void EnMm2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* mouthTextures[] = { gRunningManMouthOpenTex, gRunningManMouthClosedTex };
    EnMm2* thisv = (EnMm2*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_mm2.c", 634);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthTexIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnMm2_OverrideLimbDraw, EnMm2_PostLimbDraw, thisv);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_mm2.c", 654);
}

s32 EnMm2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMm2* thisv = (EnMm2*)thisx;

    switch (limbIndex) {
        case 8:
            rot->x += thisv->unk_1EE.y;
            rot->y -= thisv->unk_1EE.x;
            break;
        case 15:
            rot->x += thisv->unk_1E8.y;
            rot->z += thisv->unk_1E8.x + 0xFA0;
            break;
    }

    return 0;
}

void EnMm2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f headOffset = { 200.0f, 800.0f, 0.0f };
    EnMm2* thisv = (EnMm2*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&headOffset, &thisv->actor.focus.pos);
    }
}
