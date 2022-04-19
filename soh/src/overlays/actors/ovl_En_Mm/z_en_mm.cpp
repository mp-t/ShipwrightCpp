/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Running Man (child)
 */

#include "z_en_mm.h"
#include "objects/object_mm/object_mm.h"
#include "objects/object_link_child/object_link_child.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ RM_ANIM_RUN,
    /* 1 */ RM_ANIM_SIT,
    /* 2 */ RM_ANIM_SIT_WAIT,
    /* 3 */ RM_ANIM_STAND,
    /* 4 */ RM_ANIM_SPRINT,
    /* 5 */ RM_ANIM_EXCITED, // plays when talking to him with bunny hood on
    /* 6 */ RM_ANIM_HAPPY    // plays when you sell him the bunny hood
} RunningManAnimIndex;

typedef enum {
    /* 0 */ RM_MOUTH_CLOSED,
    /* 1 */ RM_MOUTH_OPEN
} RunningManMouthTex;

void EnMm_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMm_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMm_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AAE598(EnMm* thisv, GlobalContext* globalCtx);
void func_80AAE294(EnMm* thisv, GlobalContext* globalCtx);
void func_80AAE50C(EnMm* thisv, GlobalContext* globalCtx);
void func_80AAE224(EnMm* thisv, GlobalContext* globalCtx);
s32 func_80AADA70(void);

s32 EnMm_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnMm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void*);

ActorInit En_Mm_InitVars = {
    ACTOR_EN_MM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MM,
    sizeof(EnMm),
    (ActorFunc)EnMm_Init,
    (ActorFunc)EnMm_Destroy,
    (ActorFunc)EnMm_Update,
    (ActorFunc)EnMm_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit[] = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
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
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static AnimationSpeedInfo sAnimationInfo[] = {
    { &gRunningManRunAnim, 1.0f, ANIMMODE_LOOP, -7.0f },     { &gRunningManSitStandAnim, -1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSitWaitAnim, 1.0f, ANIMMODE_LOOP, -7.0f }, { &gRunningManSitStandAnim, 1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSprintAnim, 1.0f, ANIMMODE_LOOP, -7.0f },  { &gRunningManExcitedAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
    { &gRunningManHappyAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
};

typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} EnMmPathInfo;

static EnMmPathInfo sPathInfo[] = {
    { 0, 1, 0, 0 },
    { 1, 1, 0, 1 },
    { 1, 3, 2, 1 },
    { -1, 0, 2, 0 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_STOP),
};

void EnMm_ChangeAnim(EnMm* thisv, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    if (sAnimationInfo[index].playSpeed >= 0.0f) {
        Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed, 0.0f,
                         Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode,
                         morphFrames);
    } else {
        Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, sAnimationInfo[index].playSpeed,
                         Animation_GetLastFrame(sAnimationInfo[index].animation), 0.0f, sAnimationInfo[index].mode,
                         morphFrames);
    }

    *currentIndex = index;
}

void EnMm_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnMm* thisv = (EnMm*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 21.0f);

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gRunningManSkel, NULL, thisv->jointTable, thisv->morphTable, 16);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, sColChkInfoInit);

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    Animation_Change(&thisv->skelAnime, sAnimationInfo[RM_ANIM_RUN].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[RM_ANIM_RUN].animation), sAnimationInfo[RM_ANIM_RUN].mode,
                     sAnimationInfo[RM_ANIM_RUN].morphFrames);

    thisv->path = thisv->actor.params & 0xFF;
    thisv->unk_1F0 = 2;
    thisv->unk_1E8 = 0;
    thisv->actor.targetMode = 2;
    thisv->actor.gravity = -1.0f;
    thisv->speedXZ = 3.0f;
    thisv->unk_204 = thisv->actor.objBankIndex;

    if (func_80AADA70() == 1) {
        thisv->mouthTexIndex = RM_MOUTH_OPEN;
        EnMm_ChangeAnim(thisv, RM_ANIM_RUN, &thisv->curAnimIndex);
        thisv->actionFunc = func_80AAE598;
    } else {
        thisv->mouthTexIndex = RM_MOUTH_CLOSED;
        EnMm_ChangeAnim(thisv, RM_ANIM_SIT_WAIT, &thisv->curAnimIndex);
        thisv->actionFunc = func_80AAE294;
    }
}

void EnMm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnMm* thisv = (EnMm*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80AADA70(void) {
    s32 isDay = false;

    if ((gSaveContext.dayTime > 0x3555) && (gSaveContext.dayTime <= 0xD556)) {
        isDay = true;
    }

    return isDay;
}

s32 func_80AADAA0(EnMm* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 sp1C = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                if (globalCtx->msgCtx.choiceIndex == 0) {
                    player->actor.textId = 0x202D;
                    thisv->unk_254 &= ~1;
                    EnMm_ChangeAnim(thisv, RM_ANIM_HAPPY, &thisv->curAnimIndex);
                } else {
                    player->actor.textId = 0x202C;
                    gSaveContext.infTable[23] |= 0x1000;
                }
                sp1C = 2;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                Player_UnsetMask(globalCtx);
                Item_Give(globalCtx, ITEM_SOLD_OUT);
                gSaveContext.itemGetInf[3] |= 0x800;
                Rupees_ChangeBy(500);
                player->actor.textId = 0x202E;
                sp1C = 2;
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                if ((player->actor.textId == 0x202E) || (player->actor.textId == 0x202C)) {
                    thisv->unk_254 |= 1;
                    EnMm_ChangeAnim(thisv, RM_ANIM_SIT_WAIT, &thisv->curAnimIndex);
                }
                sp1C = 0;
            }
            break;
    }

    return sp1C;
}

s32 EnMm_GetTextId(EnMm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 textId;

    textId = Text_GetFaceReaction(globalCtx, 0x1C);

    if (gSaveContext.itemGetInf[3] & 0x800) {
        if (textId == 0) {
            textId = 0x204D;
        }
    } else if (player->currentMask == PLAYER_MASK_BUNNY) {
        textId = (gSaveContext.infTable[23] & 0x1000) ? 0x202B : 0x202A;
    } else if (textId == 0) {
        textId = 0x2029;
    }

    return textId;
}

void func_80AADCD0(EnMm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 yawDiff;
    s16 sp26;
    s16 sp24;

    if (thisv->unk_1E0 == 2) {
        Message_ContinueTextbox(globalCtx, player->actor.textId);
        thisv->unk_1E0 = 1;
    } else if (thisv->unk_1E0 == 1) {
        thisv->unk_1E0 = func_80AADAA0(thisv, globalCtx);
    } else {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            thisv->unk_1E0 = 1;

            if (thisv->curAnimIndex != 5) {
                if ((thisv->actor.textId == 0x202A) || (thisv->actor.textId == 0x202B)) {
                    EnMm_ChangeAnim(thisv, RM_ANIM_EXCITED, &thisv->curAnimIndex);
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
        } else {
            Actor_GetScreenPos(globalCtx, &thisv->actor, &sp26, &sp24);
            yawDiff = ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y));

            if ((sp26 >= 0) && (sp26 <= 0x140) && (sp24 >= 0) && (sp24 <= 0xF0) && (yawDiff <= 17152.0f) &&
                (thisv->unk_1E0 != 3) && func_8002F2CC(&thisv->actor, globalCtx, 100.0f)) {
                thisv->actor.textId = EnMm_GetTextId(thisv, globalCtx);
            }
        }
    }
}

s32 EnMm_GetPointCount(Path* pathList, s32 pathNum) {
    return (pathList + pathNum)->count;
}

s32 func_80AADE60(Path* pathList, Vec3f* pos, s32 pathNum, s32 waypoint) {
    Vec3s* pointPos;
    pointPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL((pathList + pathNum)->points))[waypoint];

    pos->x = pointPos->x;
    pos->y = pointPos->y;
    pos->z = pointPos->z;

    return 0;
}

s32 func_80AADEF0(EnMm* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 zDiff;
    Vec3f waypointPos;
    s32 phi_a2;
    s32 phi_v1;

    func_80AADE60(globalCtx->setupPathList, &waypointPos, thisv->path, thisv->waypoint);

    xDiff = waypointPos.x - thisv->actor.world.pos.x;
    zDiff = waypointPos.z - thisv->actor.world.pos.z;

    thisv->yawToWaypoint = (s32)(Math_FAtan2F(xDiff, zDiff) * (0x8000 / std::numbers::pi_v<float>));
    thisv->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));

    while ((thisv->distToWaypoint <= 10.44f) && (thisv->unk_1E8 != 0)) {
        thisv->waypoint += sPathInfo[thisv->unk_1E8].unk_00;

        phi_a2 = sPathInfo[thisv->unk_1E8].unk_08;

        switch (phi_a2) {
            case 0:
                phi_a2 = 0;
                break;
            case 1:
                phi_a2 = EnMm_GetPointCount(globalCtx->setupPathList, thisv->path) - 1;
                break;
            case 2:
                phi_a2 = thisv->unk_1F0;
                break;
        }

        phi_v1 = sPathInfo[thisv->unk_1E8].unk_0C;

        switch (phi_v1) {
            case 0:
                phi_v1 = 0;
                break;
            case 1:
                phi_v1 = EnMm_GetPointCount(globalCtx->setupPathList, thisv->path) - 1;
                break;
            case 2:
                phi_v1 = thisv->unk_1F0;
                break;
        }

        if ((sPathInfo[thisv->unk_1E8].unk_00 >= 0 && (thisv->waypoint < phi_a2 || phi_v1 < thisv->waypoint)) ||
            (sPathInfo[thisv->unk_1E8].unk_00 < 0 && (phi_a2 < thisv->waypoint || thisv->waypoint < phi_v1))) {
            thisv->unk_1E8 = sPathInfo[thisv->unk_1E8].unk_04;
            thisv->waypoint = sPathInfo[thisv->unk_1E8].unk_08;
        }

        func_80AADE60(globalCtx->setupPathList, &waypointPos, thisv->path, thisv->waypoint);

        xDiff = waypointPos.x - thisv->actor.world.pos.x;
        zDiff = waypointPos.z - thisv->actor.world.pos.z;

        thisv->yawToWaypoint = (s32)(Math_FAtan2F(xDiff, zDiff) * (0x8000 / std::numbers::pi_v<float>));
        thisv->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->yawToWaypoint, 1, 2500, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->speedXZ, 0.6f, thisv->distToWaypoint, 0.0f);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    return 0;
}

void func_80AAE224(EnMm* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actionFunc = func_80AAE598;
        thisv->unk_1E8 = 0;
        thisv->mouthTexIndex = RM_MOUTH_CLOSED;
        thisv->unk_254 |= 1;
        thisv->unk_1E0 = 0;
        thisv->actor.speedXZ = 0.0f;
        EnMm_ChangeAnim(thisv, RM_ANIM_SIT_WAIT, &thisv->curAnimIndex);
    }
}

void func_80AAE294(EnMm* thisv, GlobalContext* globalCtx) {
    f32 floorYNorm;
    Vec3f dustPos;

    if (!Player_InCsMode(globalCtx)) {
        SkelAnime_Update(&thisv->skelAnime);

        if (thisv->curAnimIndex == 0) {
            if (((s32)thisv->skelAnime.curFrame == 1) || ((s32)thisv->skelAnime.curFrame == 6)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_WALK_GROUND);
            }
        }

        if (thisv->curAnimIndex == 4) {
            if (((thisv->skelAnime.curFrame - thisv->skelAnime.playSpeed < 9.0f) && (thisv->skelAnime.curFrame >= 9.0f)) ||
                ((thisv->skelAnime.curFrame - thisv->skelAnime.playSpeed < 19.0f) &&
                 (thisv->skelAnime.curFrame >= 19.0f))) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
            }
        }

        if (gSaveContext.itemGetInf[3] & 0x800) {
            thisv->speedXZ = 10.0f;
            thisv->skelAnime.playSpeed = 2.0f;
        } else {
            thisv->speedXZ = 3.0f;
            thisv->skelAnime.playSpeed = 1.0f;
        }

        func_80AADEF0(thisv, globalCtx);

        if (func_80AADA70() == 0) {
            if (thisv->actor.floorPoly != NULL) {
                floorYNorm = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.y);

                if ((floorYNorm > 0.9848f) || (floorYNorm < -0.9848f)) {
                    if (thisv->sitTimer > 30) {
                        EnMm_ChangeAnim(thisv, RM_ANIM_SIT, &thisv->curAnimIndex);
                        thisv->actionFunc = func_80AAE224;
                    } else {
                        thisv->sitTimer++;
                    }
                } else {
                    thisv->sitTimer = 0;
                }
            }
        }

        if (gSaveContext.itemGetInf[3] & 0x800) {
            dustPos.x = thisv->actor.world.pos.x;
            dustPos.y = thisv->actor.world.pos.y;
            dustPos.z = thisv->actor.world.pos.z;

            if (gSaveContext.gameMode != 3) {
                func_80033480(globalCtx, &dustPos, 50.0f, 2, 350, 20, 0);
            }

            if (thisv->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                func_8002F71C(globalCtx, &thisv->actor, 3.0f, thisv->actor.yawTowardsPlayer, 4.0f);
            }
        }
    }
}

void func_80AAE50C(EnMm* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->sitTimer = 0;
        thisv->actionFunc = func_80AAE294;

        if (gSaveContext.itemGetInf[3] & 0x800) {
            EnMm_ChangeAnim(thisv, RM_ANIM_SPRINT, &thisv->curAnimIndex);
            thisv->mouthTexIndex = RM_MOUTH_CLOSED;
        } else {
            EnMm_ChangeAnim(thisv, RM_ANIM_RUN, &thisv->curAnimIndex);
            thisv->mouthTexIndex = RM_MOUTH_OPEN;
        }

        thisv->unk_1E8 = 1;
    }
}

void func_80AAE598(EnMm* thisv, GlobalContext* globalCtx) {
    func_80038290(globalCtx, &thisv->actor, &thisv->unk_248, &thisv->unk_24E, thisv->actor.focus.pos);
    SkelAnime_Update(&thisv->skelAnime);

    if ((func_80AADA70() != 0) && (thisv->unk_1E0 == 0)) {
        thisv->unk_1E0 = 3;
        thisv->actionFunc = func_80AAE50C;
        thisv->unk_254 &= ~1;
        EnMm_ChangeAnim(thisv, RM_ANIM_STAND, &thisv->curAnimIndex);
    }
}

void EnMm_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnMm* thisv = (EnMm*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    func_80AADCD0(thisv, globalCtx);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnMm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* mouthTextures[] = { gRunningManMouthOpenTex, gRunningManMouthClosedTex };
    s32 pad;
    EnMm* thisv = (EnMm*)thisx;

    if (0) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_mm.c", 1065);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthTexIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnMm_OverrideLimbDraw, EnMm_PostLimbDraw, thisv);

    if (gSaveContext.itemGetInf[3] & 0x800) {
        s32 linkChildObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_LINK_CHILD);

        if (linkChildObjBankIndex >= 0) {
            Mtx* mtx;
            Vec3s sp50;
            Mtx* mtx2;

            mtx = static_cast<Mtx*>(Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx) * 2));

            Matrix_Put(&thisv->unk_208);
            mtx2 = Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_mm.c", 1111);

            gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[linkChildObjBankIndex].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0B, mtx);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx2 - 7);

            sp50.x = 994;
            sp50.y = 3518;
            sp50.z = -13450;

            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, -240.0f, &sp50);
            Matrix_ToMtx(mtx++, "../z_en_mm.c", 1124);

            sp50.x = -994;
            sp50.y = -3518;
            sp50.z = -13450;

            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, 240.0f, &sp50);
            Matrix_ToMtx(mtx, "../z_en_mm.c", 1131);

            gSPDisplayList(POLY_OPA_DISP++, gLinkChildBunnyHoodDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[thisv->actor.objBankIndex].segment);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_mm.c", 1141);
}

s32 EnMm_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMm* thisv = (EnMm*)thisx;

    if (thisv->unk_254 & 1) {
        switch (limbIndex) {
            case 8:
                rot->x += thisv->unk_24E.y;
                rot->y -= thisv->unk_24E.x;
                break;
            case 15:
                rot->x += thisv->unk_248.y;
                rot->z += (thisv->unk_248.x + 0xFA0);
                break;
            default:
                break;
        }
    }

    return 0;
}

void EnMm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f headOffset = { 200.0f, 800.0f, 0.0f };
    EnMm* thisv = (EnMm*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&headOffset, &thisv->actor.focus.pos);
        Matrix_Translate(260.0f, 20.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateY(0.0f, MTXMODE_APPLY);
        Matrix_RotateX(0.0f, MTXMODE_APPLY);
        Matrix_RotateZ(4.0f * std::numbers::pi_v<float> / 5.0f, MTXMODE_APPLY);
        Matrix_Translate(-260.0f, 58.0f, 10.0f, MTXMODE_APPLY);
        Matrix_Get(&thisv->unk_208);
    }
}
