/*
 * File: z_en_md.c
 * Overlay: ovl_En_Md
 * Description: Mido
 */

#include "z_en_md.h"
#include "objects/object_md/object_md.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnMd_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMd_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMd_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMd_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AAB874(EnMd* thisv, GlobalContext* globalCtx);
void func_80AAB8F8(EnMd* thisv, GlobalContext* globalCtx);
void func_80AAB948(EnMd* thisv, GlobalContext* globalCtx);
void func_80AABC10(EnMd* thisv, GlobalContext* globalCtx);
void func_80AABD0C(EnMd* thisv, GlobalContext* globalCtx);

const ActorInit En_Md_InitVars = {
    ACTOR_EN_MD,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MD,
    sizeof(EnMd),
    (ActorFunc)EnMd_Init,
    (ActorFunc)EnMd_Destroy,
    (ActorFunc)EnMd_Update,
    (ActorFunc)EnMd_Draw,
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
    { 36, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /*  0 */ ENMD_ANIM_0,
    /*  1 */ ENMD_ANIM_1,
    /*  2 */ ENMD_ANIM_2,
    /*  3 */ ENMD_ANIM_3,
    /*  4 */ ENMD_ANIM_4,
    /*  5 */ ENMD_ANIM_5,
    /*  6 */ ENMD_ANIM_6,
    /*  7 */ ENMD_ANIM_7,
    /*  8 */ ENMD_ANIM_8,
    /*  9 */ ENMD_ANIM_9,
    /* 10 */ ENMD_ANIM_10,
    /* 11 */ ENMD_ANIM_11,
    /* 12 */ ENMD_ANIM_12,
    /* 13 */ ENMD_ANIM_13,
} EnMdAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMidoRaiseHand1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoHaltAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoPutHandDownAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoAnnoyedPointedHeadIdle1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoAnnoyedPointedHeadIdle2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoAnim_92B0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoWalkingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoHandsOnHipsTransitionAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoHandsOnHipsIdleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gMidoSlamAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gMidoRaiseHand2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gMidoAngryHeadTurnAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
};

void func_80AAA250(EnMd* thisv) {
    f32 startFrame;

    startFrame = thisv->skelAnime.startFrame;
    thisv->skelAnime.startFrame = thisv->skelAnime.endFrame;
    thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
    thisv->skelAnime.endFrame = startFrame;
    thisv->skelAnime.playSpeed = -1.0f;
}

void func_80AAA274(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_3);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA308(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_4);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_5);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA39C(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            func_80AAA250(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_7);
                thisv->unk_20A++;
            } else {
                break;
            }
        case 2:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_8);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA474(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_7);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA508(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_2);
            func_80AAA250(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA5A4(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_9);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA638(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_9);
            func_80AAA250(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA6D4(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_11);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA768(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_12);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_3);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA7FC(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_13);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_6);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA890(EnMd* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_7);
            func_80AAA250(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_10);
                thisv->unk_20A++;
            }
    }
}

void func_80AAA92C(EnMd* thisv, u8 arg1) {
    thisv->unk_20B = arg1;
    thisv->unk_20A = 0;
}

void func_80AAA93C(EnMd* thisv) {
    switch (thisv->unk_20B) {
        case 1:
            func_80AAA274(thisv);
            break;
        case 2:
            func_80AAA308(thisv);
            break;
        case 3:
            func_80AAA39C(thisv);
            break;
        case 4:
            func_80AAA474(thisv);
            break;
        case 5:
            func_80AAA508(thisv);
            break;
        case 6:
            func_80AAA5A4(thisv);
            break;
        case 7:
            func_80AAA638(thisv);
            break;
        case 8:
            func_80AAA6D4(thisv);
            break;
        case 9:
            func_80AAA768(thisv);
            break;
        case 10:
            func_80AAA7FC(thisv);
            break;
        case 11:
            func_80AAA890(thisv);
    }
}

void func_80AAAA24(EnMd* thisv) {
    if (thisv->unk_1E0.unk_00 != 0) {
        switch (thisv->actor.textId) {
            case 0x102F:
                if ((thisv->unk_208 == 0) && (thisv->unk_20B != 1)) {
                    func_80AAA92C(thisv, 1);
                }
                if ((thisv->unk_208 == 2) && (thisv->unk_20B != 2)) {
                    func_80AAA92C(thisv, 2);
                }
                if ((thisv->unk_208 == 5) && (thisv->unk_20B != 8)) {
                    func_80AAA92C(thisv, 8);
                }
                if ((thisv->unk_208 == 11) && (thisv->unk_20B != 9)) {
                    func_80AAA92C(thisv, 9);
                }
                break;
            case 0x1033:
                if ((thisv->unk_208 == 0) && (thisv->unk_20B != 1)) {
                    func_80AAA92C(thisv, 1);
                }
                if ((thisv->unk_208 == 1) && (thisv->unk_20B != 2)) {
                    func_80AAA92C(thisv, 2);
                }
                if ((thisv->unk_208 == 5) && (thisv->unk_20B != 10)) {
                    func_80AAA92C(thisv, 10);
                }
                if ((thisv->unk_208 == 7) && (thisv->unk_20B != 9)) {
                    func_80AAA92C(thisv, 9);
                }
                break;
            case 0x1030:
            case 0x1034:
            case 0x1045:
                if ((thisv->unk_208 == 0) && (thisv->unk_20B != 1)) {
                    func_80AAA92C(thisv, 1);
                }
                break;
            case 0x1046:
                if ((thisv->unk_208 == 0) && (thisv->unk_20B != 6)) {
                    func_80AAA92C(thisv, 6);
                }
                break;
        }
    } else if (thisv->skelAnime.animation != &gMidoHandsOnHipsIdleAnim) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_10);
        func_80AAA92C(thisv, 0);
    }

    func_80AAA93C(thisv);
}

s16 func_80AAAC78(EnMd* thisv, GlobalContext* globalCtx) {
    s16 dialogState = Message_GetState(&globalCtx->msgCtx);

    if ((thisv->unk_209 == TEXT_STATE_AWAITING_NEXT) || (thisv->unk_209 == TEXT_STATE_EVENT) ||
        (thisv->unk_209 == TEXT_STATE_CLOSING) || (thisv->unk_209 == TEXT_STATE_DONE_HAS_NEXT)) {
        if (thisv->unk_209 != dialogState) {
            thisv->unk_208++;
        }
    }

    thisv->unk_209 = dialogState;
    return dialogState;
}

u16 EnMd_GetTextKokiriForest(GlobalContext* globalCtx, EnMd* thisv) {
    u16 reactionText = Text_GetFaceReaction(globalCtx, 0x11);

    if (reactionText != 0) {
        return reactionText;
    }

    thisv->unk_208 = 0;
    thisv->unk_209 = TEXT_STATE_NONE;

    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        return 0x1045;
    }

    if (gSaveContext.eventChkInf[0] & 0x10) {
        return 0x1034;
    }

    if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == 1) && (CUR_EQUIP_VALUE(EQUIP_SWORD) == 1)) {
        return 0x1033;
    }

    if (gSaveContext.infTable[0] & 0x1000) {
        return 0x1030;
    }

    return 0x102F;
}

u16 EnMd_GetTextKokiriHome(GlobalContext* globalCtx, EnMd* thisv) {
    thisv->unk_208 = 0;
    thisv->unk_209 = TEXT_STATE_NONE;

    if (gSaveContext.eventChkInf[4] & 1) {
        return 0x1028;
    }

    return 0x1046;
}

u16 EnMd_GetTextLostWoods(GlobalContext* globalCtx, EnMd* thisv) {
    thisv->unk_208 = 0;
    thisv->unk_209 = TEXT_STATE_NONE;

    if (gSaveContext.eventChkInf[4] & 0x100) {
        if (gSaveContext.infTable[1] & 0x200) {
            return 0x1071;
        }
        return 0x1070;
    }

    if (gSaveContext.eventChkInf[0] & 0x400) {
        return 0x1068;
    }

    if (gSaveContext.infTable[1] & 0x20) {
        return 0x1061;
    }

    return 0x1060;
}

u16 EnMd_GetText(GlobalContext* globalCtx, Actor* thisx) {
    EnMd* thisv = (EnMd*)thisx;

    switch (globalCtx->sceneNum) {
        case SCENE_SPOT04:
            return EnMd_GetTextKokiriForest(globalCtx, thisv);
        case SCENE_KOKIRI_HOME4:
            return EnMd_GetTextKokiriHome(globalCtx, thisv);
        case SCENE_SPOT10:
            return EnMd_GetTextLostWoods(globalCtx, thisv);
        default:
            return 0;
    }
}

s16 func_80AAAF04(GlobalContext* globalCtx, Actor* thisx) {
    EnMd* thisv = (EnMd*)thisx;
    switch (func_80AAAC78(thisv, globalCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return 1;
        case TEXT_STATE_CLOSING:
            switch (thisv->actor.textId) {
                case 0x1028:
                    gSaveContext.eventChkInf[0] |= 0x8000;
                    break;
                case 0x102F:
                    gSaveContext.eventChkInf[0] |= 4;
                    gSaveContext.infTable[0] |= 0x1000;
                    break;
                case 0x1060:
                    gSaveContext.infTable[1] |= 0x20;
                    break;
                case 0x1070:
                    gSaveContext.infTable[1] |= 0x200;
                    break;
                case 0x1033:
                case 0x1067:
                    return 2;
            }
            return 0;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                return 2;
            }
        default:
            return 1;
    }
}

u8 EnMd_ShouldSpawn(EnMd* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT04) {
        if (!(gSaveContext.eventChkInf[1] & 0x1000) && !(gSaveContext.eventChkInf[4] & 1)) {
            return 1;
        }
    }

    if (globalCtx->sceneNum == SCENE_KOKIRI_HOME4) {
        if (((gSaveContext.eventChkInf[1] & 0x1000) != 0) || ((gSaveContext.eventChkInf[4] & 1) != 0)) {
            if (!LINK_IS_ADULT) {
                return 1;
            }
        }
    }

    if (globalCtx->sceneNum == SCENE_SPOT10) {
        return 1;
    }

    return 0;
}

void EnMd_UpdateEyes(EnMd* thisv) {
    if (DECR(thisv->blinkTimer) == 0) {
        thisv->eyeIdx++;
        if (thisv->eyeIdx > 2) {
            thisv->blinkTimer = Rand_S16Offset(30, 30);
            thisv->eyeIdx = 0;
        }
    }
}

void func_80AAB158(EnMd* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 absYawDiff;
    s16 temp;
    s16 temp2;
    s16 yawDiff;

    if (thisv->actor.xzDistToPlayer < 170.0f) {
        yawDiff = (f32)thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        absYawDiff = ABS(yawDiff);

        temp = (absYawDiff <= func_800347E8(2)) ? 2 : 1;
        temp2 = 1;
    } else {
        temp = 1;
        temp2 = 0;
    }

    if (thisv->unk_1E0.unk_00 != 0) {
        temp = 4;
    }

    if (thisv->actionFunc == func_80AABD0C) {
        temp = 1;
        temp2 = 0;
    }
    if (thisv->actionFunc == func_80AAB8F8) {
        temp = 4;
        temp2 = 1;
    }

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) || gDbgCamEnabled) {
        thisv->unk_1E0.unk_18 = globalCtx->view.eye;
        thisv->unk_1E0.unk_14 = 40.0f;
        temp = 2;
    } else {
        thisv->unk_1E0.unk_18 = player->actor.world.pos;
        thisv->unk_1E0.unk_14 = (gSaveContext.linkAge > 0) ? 0.0f : -18.0f;
    }

    func_80034A14(&thisv->actor, &thisv->unk_1E0, 2, temp);
    if (thisv->actionFunc != func_80AABC10) {
        if (temp2) {
            func_800343CC(globalCtx, &thisv->actor, &thisv->unk_1E0.unk_00, thisv->collider.dim.radius + 30.0f,
                          EnMd_GetText, func_80AAAF04);
        }
    }
}

u8 EnMd_FollowPath(EnMd* thisv, GlobalContext* globalCtx) {
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
    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_FAtan2F(pathDiffX, pathDiffZ) * (65536.0f / (2 * std::numbers::pi_v<float>)), 4, 4000,
                       1);

    if ((SQ(pathDiffX) + SQ(pathDiffZ)) < 100.0f) {
        thisv->waypoint++;
        if (thisv->waypoint >= path->count) {
            thisv->waypoint = 0;
        }
        return 1;
    }
    return 0;
}

u8 EnMd_SetMovedPos(EnMd* thisv, GlobalContext* globalCtx) {
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

void func_80AAB5A4(EnMd* thisv, GlobalContext* globalCtx) {
    f32 temp;

    if (globalCtx->sceneNum != SCENE_KOKIRI_HOME4) {
        temp = (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
                (globalCtx->sceneNum == SCENE_SPOT04))
                   ? 100.0f
                   : 400.0f;
        thisv->alpha = func_80034DD4(&thisv->actor, globalCtx, thisv->alpha, temp);
        thisv->actor.shape.shadowAlpha = thisv->alpha;
    } else {
        thisv->alpha = 255;
        thisv->actor.shape.shadowAlpha = thisv->alpha;
    }
}

void EnMd_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMd* thisv = (EnMd*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gMidoSkel, NULL, thisv->jointTable, thisv->morphTable, 17);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    if (!EnMd_ShouldSpawn(thisv, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENMD_ANIM_0);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.targetMode = 6;
    thisv->alpha = 255;
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_ELF, thisv->actor.world.pos.x,
                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);

    if (((globalCtx->sceneNum == SCENE_SPOT04) && !(gSaveContext.eventChkInf[0] & 0x10)) ||
        ((globalCtx->sceneNum == SCENE_SPOT04) && (gSaveContext.eventChkInf[0] & 0x10) &&
         CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) ||
        ((globalCtx->sceneNum == SCENE_SPOT10) && !(gSaveContext.eventChkInf[0] & 0x400))) {
        thisv->actor.home.pos = thisv->actor.world.pos;
        thisv->actionFunc = func_80AAB948;
        return;
    }

    if (globalCtx->sceneNum != SCENE_KOKIRI_HOME4) {
        EnMd_SetMovedPos(thisv, globalCtx);
    }

    thisv->actionFunc = func_80AAB874;
}

void EnMd_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMd* thisv = (EnMd*)thisx;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80AAB874(EnMd* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(globalCtx, thisv->unk_214, thisv->unk_236, 17);
    } else if ((thisv->unk_1E0.unk_00 == 0) && (thisv->unk_20B != 7)) {
        func_80AAA92C(thisv, 7);
    }

    func_80AAAA24(thisv);
}

void func_80AAB8F8(EnMd* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(globalCtx, thisv->unk_214, thisv->unk_236, 17);
    }
    func_80AAA93C(thisv);
}

void func_80AAB948(EnMd* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 temp;
    Actor* actorToBlock = &GET_PLAYER(globalCtx)->actor;
    s16 yaw;

    func_80AAAA24(thisv);

    if (thisv->unk_1E0.unk_00 == 0) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;

        yaw = Math_Vec3f_Yaw(&thisv->actor.home.pos, &actorToBlock->world.pos);

        thisv->actor.world.pos.x = thisv->actor.home.pos.x;
        thisv->actor.world.pos.x += 60.0f * Math_SinS(yaw);

        thisv->actor.world.pos.z = thisv->actor.home.pos.z;
        thisv->actor.world.pos.z += 60.0f * Math_CosS(yaw);

        temp = fabsf((f32)thisv->actor.yawTowardsPlayer - yaw) * 0.001f * 3.0f;
        thisv->skelAnime.playSpeed = CLAMP(temp, 1.0f, 3.0f);
    }

    if (thisv->unk_1E0.unk_00 == 2) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
            (globalCtx->sceneNum == SCENE_SPOT04)) {
            globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        }

        if (globalCtx->sceneNum == SCENE_SPOT04) {
            gSaveContext.eventChkInf[0] |= 0x10;
        }
        if (globalCtx->sceneNum == SCENE_SPOT10) {
            gSaveContext.eventChkInf[0] |= 0x400;
        }

        func_80AAA92C(thisv, 3);
        func_80AAA93C(thisv);
        thisv->waypoint = 1;
        thisv->unk_1E0.unk_00 = 0;
        thisv->actionFunc = func_80AABD0C;
        thisv->actor.speedXZ = 1.5f;
        return;
    }

    if (thisv->skelAnime.animation == &gMidoHandsOnHipsIdleAnim) {
        func_80034F54(globalCtx, thisv->unk_214, thisv->unk_236, 17);
    }

    if ((thisv->unk_1E0.unk_00 == 0) && (globalCtx->sceneNum == SCENE_SPOT10)) {
        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            player->unk_6A8 = &thisv->actor;
            func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_SARIA);
            thisv->actionFunc = func_80AABC10;
            return;
        }

        if (thisv->actor.xzDistToPlayer < (30.0f + thisv->collider.dim.radius)) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80AABC10(EnMd* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        thisv->actionFunc = func_80AAB948;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        thisv->actor.textId = 0x1067;
        func_8002F2CC(&thisv->actor, globalCtx, thisv->collider.dim.radius + 30.0f);

        thisv->actionFunc = func_80AAB948;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= 0x800000;
    }
}

void func_80AABD0C(EnMd* thisv, GlobalContext* globalCtx) {
    func_80034F54(globalCtx, thisv->unk_214, thisv->unk_236, 17);
    func_80AAA93C(thisv);

    if (!(EnMd_FollowPath(thisv, globalCtx)) || (thisv->waypoint != 0)) {
        thisv->actor.shape.rot = thisv->actor.world.rot;
        return;
    }

    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && !(gSaveContext.eventChkInf[1] & 0x1000) &&
        (globalCtx->sceneNum == SCENE_SPOT04)) {
        Message_CloseTextbox(globalCtx);
        gSaveContext.eventChkInf[1] |= 0x1000;
        Actor_Kill(&thisv->actor);
        return;
    }

    func_80AAA92C(thisv, 11);

    thisv->skelAnime.playSpeed = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.home.pos = thisv->actor.world.pos;
    thisv->actionFunc = func_80AAB8F8;
}

void EnMd_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMd* thisv = (EnMd*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    SkelAnime_Update(&thisv->skelAnime);
    EnMd_UpdateEyes(thisv);
    func_80AAB5A4(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    func_80AAB158(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnMd_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnMd* thisv = (EnMd*)thisx;
    Vec3s vec;

    if (limbIndex == 16) {
        Matrix_Translate(1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = thisv->unk_1E0.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_Translate(-1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 9) {
        vec = thisv->unk_1E0.unk_0E;
        Matrix_RotateX((vec.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateY((vec.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
    }

    if (((limbIndex == 9) || (limbIndex == 10)) || (limbIndex == 13)) {
        rot->y += Math_SinS(thisv->unk_214[limbIndex]) * 200.0f;
        rot->z += Math_CosS(thisv->unk_236[limbIndex]) * 200.0f;
    }

    return false;
}

void EnMd_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnMd* thisv = (EnMd*)thisx;
    Vec3f vec = { 400.0f, 0.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&vec, &thisv->actor.focus.pos);
    }
}

void EnMd_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* sEyeTextures[] = {
        gMidoEyeOpenTex,
        gMidoEyeHalfTex,
        gMidoEyeClosedTex,
    };
    EnMd* thisv = (EnMd*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_md.c", 1280);

    if (thisv->alpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeIdx]));
        func_80034BA0(globalCtx, &thisv->skelAnime, EnMd_OverrideLimbDraw, EnMd_PostLimbDraw, &thisv->actor, thisv->alpha);
    } else if (thisv->alpha != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeIdx]));
        func_80034CC4(globalCtx, &thisv->skelAnime, EnMd_OverrideLimbDraw, EnMd_PostLimbDraw, &thisv->actor, thisv->alpha);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_md.c", 1317);
}
