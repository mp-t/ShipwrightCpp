/*
 * File: z_en_kz.c
 * Overlay: ovl_En_Kz
 * Description: King Zora
 */

#include "z_en_kz.h"
#include "objects/object_kz/object_kz.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnKz_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKz_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKz_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKz_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnKz_PreMweepWait(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_SetupMweep(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_Mweep(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_StopMweep(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_Wait(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_SetupGetItem(EnKz* thisv, GlobalContext* globalCtx);
void EnKz_StartTimer(EnKz* thisv, GlobalContext* globalCtx);

ActorInit En_Kz_InitVars = {
    ACTOR_EN_KZ,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KZ,
    sizeof(EnKz),
    (ActorFunc)EnKz_Init,
    (ActorFunc)EnKz_Destroy,
    (ActorFunc)EnKz_Update,
    (ActorFunc)EnKz_Draw,
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
    { 80, 120, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENKZ_ANIM_0,
    /* 1 */ ENKZ_ANIM_1,
    /* 2 */ ENKZ_ANIM_2
} EnKzAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gKzIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gKzMweepAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
};

u16 EnKz_GetTextNoMaskChild(GlobalContext* globalCtx, EnKz* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
        return 0x402B;
    } else if (gSaveContext.eventChkInf[3] & 8) {
        return 0x401C;
    } else {
        player->exchangeItemId = EXCH_ITEM_LETTER_RUTO;
        return 0x401A;
    }
}

u16 EnKz_GetTextNoMaskAdult(GlobalContext* globalCtx, EnKz* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_FROG) {
        if (!(gSaveContext.infTable[19] & 0x200)) {
            if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 2)) {
                return 0x401F;
            } else {
                return 0x4012;
            }
        } else {
            return CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
        }
    } else {
        player->exchangeItemId = EXCH_ITEM_PRESCRIPTION;
        return 0x4012;
    }
}

u16 EnKz_GetText(GlobalContext* globalCtx, Actor* thisx) {
    EnKz* thisv = (EnKz*)thisx;
    u16 reactionText = Text_GetFaceReaction(globalCtx, 0x1E);

    if (reactionText != 0) {
        return reactionText;
    }

    if (LINK_IS_ADULT) {
        return EnKz_GetTextNoMaskAdult(globalCtx, thisv);
    } else {
        return EnKz_GetTextNoMaskChild(globalCtx, thisv);
    }
}

s16 func_80A9C6C0(GlobalContext* globalCtx, Actor* thisx) {
    EnKz* thisv = (EnKz*)thisx;
    s16 ret = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_DONE:
            ret = 0;
            switch (thisv->actor.textId) {
                case 0x4012:
                    gSaveContext.infTable[19] |= 0x200;
                    ret = 2;
                    break;
                case 0x401B:
                    ret = !Message_ShouldAdvance(globalCtx) ? 1 : 2;
                    break;
                case 0x401F:
                    gSaveContext.infTable[19] |= 0x200;
                    break;
            }
            break;
        case TEXT_STATE_DONE_FADING:
            if (thisv->actor.textId != 0x4014) {
                if (thisv->actor.textId == 0x401B && !thisv->sfxPlayed) {
                    Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    thisv->sfxPlayed = true;
                }
            } else if (!thisv->sfxPlayed) {
                Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->sfxPlayed = true;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (!Message_ShouldAdvance(globalCtx)) {
                break;
            }
            if (thisv->actor.textId == 0x4014) {
                if (globalCtx->msgCtx.choiceIndex == 0) {
                    EnKz_SetupGetItem(thisv, globalCtx);
                    ret = 2;
                } else {
                    thisv->actor.textId = 0x4016;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                }
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                ret = 2;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return ret;
}

void EnKz_UpdateEyes(EnKz* thisv) {
    if (DECR(thisv->blinkTimer) == 0) {
        thisv->eyeIdx += 1;
        if (thisv->eyeIdx >= 3) {
            thisv->blinkTimer = Rand_S16Offset(30, 30);
            thisv->eyeIdx = 0;
        }
    }
}

s32 func_80A9C95C(GlobalContext* globalCtx, EnKz* thisv, s16* arg2, f32 unkf, callback1_800343CC callback1,
                  callback2_800343CC callback2) {
    Player* player = GET_PLAYER(globalCtx);
    s16 sp32;
    s16 sp30;
    f32 xzDistToPlayer;
    f32 yaw;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        *arg2 = 1;
        return 1;
    }

    if (*arg2 != 0) {
        *arg2 = callback2(globalCtx, &thisv->actor);
        return 0;
    }

    yaw = Math_Vec3f_Yaw(&thisv->actor.home.pos, &player->actor.world.pos);
    yaw -= thisv->actor.shape.rot.y;
    if ((fabsf(yaw) > 1638.0f) || (thisv->actor.xzDistToPlayer < 265.0f)) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        return 0;
    }

    thisv->actor.flags |= ACTOR_FLAG_0;

    Actor_GetScreenPos(globalCtx, &thisv->actor, &sp32, &sp30);
    if (!((sp32 >= -30) && (sp32 < 361) && (sp30 >= -10) && (sp30 < 241))) {
        return 0;
    }

    xzDistToPlayer = thisv->actor.xzDistToPlayer;
    thisv->actor.xzDistToPlayer = Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos);
    if (func_8002F2CC(&thisv->actor, globalCtx, unkf) == 0) {
        thisv->actor.xzDistToPlayer = xzDistToPlayer;
        return 0;
    }
    thisv->actor.xzDistToPlayer = xzDistToPlayer;
    thisv->actor.textId = callback1(globalCtx, &thisv->actor);

    return 0;
}

void func_80A9CB18(EnKz* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (func_80A9C95C(globalCtx, thisv, &thisv->unk_1E0.unk_00, 340.0f, EnKz_GetText, func_80A9C6C0)) {
        if ((thisv->actor.textId == 0x401A) && !(gSaveContext.eventChkInf[3] & 8)) {
            if (func_8002F368(globalCtx) == EXCH_ITEM_LETTER_RUTO) {
                thisv->actor.textId = 0x401B;
                thisv->sfxPlayed = false;
            } else {
                thisv->actor.textId = 0x401A;
            }
            player->actor.textId = thisv->actor.textId;
            return;
        }

        if (LINK_IS_ADULT) {
            if ((INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_PRESCRIPTION) &&
                (func_8002F368(globalCtx) == EXCH_ITEM_PRESCRIPTION)) {
                thisv->actor.textId = 0x4014;
                thisv->sfxPlayed = false;
                player->actor.textId = thisv->actor.textId;
                thisv->isTrading = true;
                return;
            }

            thisv->isTrading = false;
            if (gSaveContext.infTable[19] & 0x200) {
                thisv->actor.textId = CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? 0x4045 : 0x401A;
                player->actor.textId = thisv->actor.textId;
            } else {
                thisv->actor.textId = CHECK_OWNED_EQUIP(EQUIP_TUNIC, 2) ? 0x401F : 0x4012;
                player->actor.textId = thisv->actor.textId;
            }
        }
    }
}

s32 EnKz_FollowPath(EnKz* thisv, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* pointPos;
    f32 pathDiffX;
    f32 pathDiffZ;

    if ((thisv->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &globalCtx->setupPathList[(thisv->actor.params & 0xFF00) >> 8];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += thisv->waypoint;

    pathDiffX = pointPos->x - thisv->actor.world.pos.x;
    pathDiffZ = pointPos->z - thisv->actor.world.pos.z;
    Math_SmoothStepToS(&thisv->actor.world.rot.y, (Math_FAtan2F(pathDiffX, pathDiffZ) * (0x8000 / std::numbers::pi_v<float>)), 0xA, 0x3E8, 1);

    if ((SQ(pathDiffX) + SQ(pathDiffZ)) < 10.0f) {
        thisv->waypoint++;
        if (thisv->waypoint >= path->count) {
            thisv->waypoint = 0;
        }
        return 1;
    }
    return 0;
}

s32 EnKz_SetMovedPos(EnKz* thisv, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* lastPointPos;

    if ((thisv->actor.params & 0xFF00) == 0xFF00) {
        return 0;
    }

    path = &globalCtx->setupPathList[(thisv->actor.params & 0xFF00) >> 8];
    lastPointPos = SEGMENTED_TO_VIRTUAL(path->points);
    lastPointPos += path->count - 1;

    thisv->actor.world.pos.x = lastPointPos->x;
    thisv->actor.world.pos.y = lastPointPos->y;
    thisv->actor.world.pos.z = lastPointPos->z;

    return 1;
}

void EnKz_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKz* thisv = (EnKz*)thisx;
    s32 pad;

    SkelAnime_InitFlex(globalCtx, &thisv->skelanime, &gKzSkel, NULL, thisv->jointTable, thisv->morphTable, 12);
    ActorShape_Init(&thisv->actor.shape, 0.0, NULL, 0.0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    Actor_SetScale(&thisv->actor, 0.01);
    thisv->actor.targetMode = 3;
    thisv->unk_1E0.unk_00 = 0;
    Animation_ChangeByInfo(&thisv->skelanime, sAnimationInfo, ENKZ_ANIM_0);

    if (gSaveContext.eventChkInf[3] & 8) {
        EnKz_SetMovedPos(thisv, globalCtx);
    }

    if (LINK_IS_ADULT) {
        if (!(gSaveContext.infTable[19] & 0x100)) {
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BG_ICE_SHELTER,
                               thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0,
                               0x04FF);
        }
        thisv->actionFunc = EnKz_Wait;
    } else {
        thisv->actionFunc = EnKz_PreMweepWait;
    }
}

void EnKz_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKz* thisv = (EnKz*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnKz_PreMweepWait(EnKz* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1E0.unk_00 == 2) {
        Animation_ChangeByInfo(&thisv->skelanime, sAnimationInfo, ENKZ_ANIM_2);
        thisv->unk_1E0.unk_00 = 0;
        thisv->actionFunc = EnKz_SetupMweep;
    } else {
        func_80034F54(globalCtx, thisv->unk_2A6, thisv->unk_2BE, 12);
    }
}

void EnKz_SetupMweep(EnKz* thisv, GlobalContext* globalCtx) {
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f initPos;

    thisv->cutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
    thisv->gameplayCamera = globalCtx->activeCamera;
    Gameplay_ChangeCameraStatus(globalCtx, thisv->gameplayCamera, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->cutsceneCamera, CAM_STAT_ACTIVE);
    pos = thisv->actor.world.pos;
    initPos = thisv->actor.home.pos;
    pos.y += 60.0f;
    initPos.y += -100.0f;
    initPos.z += 260.0f;
    Gameplay_CameraSetAtEye(globalCtx, thisv->cutsceneCamera, &pos, &initPos);
    func_8002DF54(globalCtx, &thisv->actor, 8);
    thisv->actor.speedXZ = 0.1f;
    thisv->actionFunc = EnKz_Mweep;
}

void EnKz_Mweep(EnKz* thisv, GlobalContext* globalCtx) {
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f initPos;

    pos = thisv->actor.world.pos;
    initPos = thisv->actor.home.pos;
    pos.y += 60.0f;
    initPos.y += -100.0f;
    initPos.z += 260.0f;
    Gameplay_CameraSetAtEye(globalCtx, thisv->cutsceneCamera, &pos, &initPos);
    if ((EnKz_FollowPath(thisv, globalCtx) == 1) && (thisv->waypoint == 0)) {
        Animation_ChangeByInfo(&thisv->skelanime, sAnimationInfo, ENKZ_ANIM_1);
        Inventory_ReplaceItem(globalCtx, ITEM_LETTER_RUTO, ITEM_BOTTLE);
        EnKz_SetMovedPos(thisv, globalCtx);
        gSaveContext.eventChkInf[3] |= 8;
        thisv->actor.speedXZ = 0.0;
        thisv->actionFunc = EnKz_StopMweep;
    }
    if (thisv->skelanime.curFrame == 13.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_KZ_MOVE);
    }
}

void EnKz_StopMweep(EnKz* thisv, GlobalContext* globalCtx) {
    Gameplay_ChangeCameraStatus(globalCtx, thisv->gameplayCamera, CAM_STAT_ACTIVE);
    Gameplay_ClearCamera(globalCtx, thisv->cutsceneCamera);
    func_8002DF54(globalCtx, &thisv->actor, 7);
    thisv->actionFunc = EnKz_Wait;
}

void EnKz_Wait(EnKz* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1E0.unk_00 == 2) {
        thisv->actionFunc = EnKz_SetupGetItem;
        EnKz_SetupGetItem(thisv, globalCtx);
    } else {
        func_80034F54(globalCtx, thisv->unk_2A6, thisv->unk_2BE, 12);
    }
}

void EnKz_SetupGetItem(EnKz* thisv, GlobalContext* globalCtx) {
    s32 getItemId;
    f32 xzRange;
    f32 yRange;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->unk_1E0.unk_00 = 1;
        thisv->actionFunc = EnKz_StartTimer;
    } else {
        getItemId = thisv->isTrading == true ? GI_FROG : GI_TUNIC_ZORA;
        yRange = fabsf(thisv->actor.yDistToPlayer) + 1.0f;
        xzRange = thisv->actor.xzDistToPlayer + 1.0f;
        func_8002F434(&thisv->actor, globalCtx, getItemId, xzRange, yRange);
    }
}

void EnKz_StartTimer(EnKz* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_FROG) {
            func_80088AA0(180); // start timer2 with 3 minutes
            gSaveContext.eventInf[1] &= ~1;
        }
        thisv->unk_1E0.unk_00 = 0;
        thisv->actionFunc = EnKz_Wait;
    }
}

void EnKz_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnKz* thisv = (EnKz*)thisx;
    s32 pad;

    if (LINK_IS_ADULT && !(gSaveContext.infTable[19] & 0x100)) {
        gSaveContext.infTable[19] |= 0x100;
    }
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    SkelAnime_Update(&thisv->skelanime);
    EnKz_UpdateEyes(thisv);
    Actor_MoveForward(&thisv->actor);
    if (thisv->actionFunc != EnKz_StartTimer) {
        func_80A9CB18(thisv, globalCtx);
    }
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnKz_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnKz* thisv = (EnKz*)thisx;

    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 10) {
        rot->y += Math_SinS(thisv->unk_2A6[limbIndex]) * 200.0f;
        rot->z += Math_CosS(thisv->unk_2BE[limbIndex]) * 200.0f;
    }
    if (limbIndex) {}
    return false;
}

void EnKz_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnKz* thisv = (EnKz*)thisx;
    Vec3f mult = { 2600.0f, 0.0f, 0.0f };

    if (limbIndex == 11) {
        Matrix_MultVec3f(&mult, &thisv->actor.focus.pos);
    }
}

void EnKz_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* sEyeSegments[] = {
        gKzEyeOpenTex,
        gKzEyeHalfTex,
        gKzEyeClosedTex,
    };
    EnKz* thisv = (EnKz*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_kz.c", 1259);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeSegments[thisv->eyeIdx]));
    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, thisv->skelanime.dListCount,
                          EnKz_OverrideLimbDraw, EnKz_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_kz.c", 1281);
}
