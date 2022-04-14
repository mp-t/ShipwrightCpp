#include "z_en_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_sa/object_sa.h"
#include "scenes/overworld/spot04/spot04_scene.h"
#include "scenes/overworld/spot05/spot05_scene.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnSa_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSa_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSa_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AF6448(EnSa* thisv, GlobalContext* globalCtx);
void func_80AF67D0(EnSa* thisv, GlobalContext* globalCtx);
void func_80AF683C(EnSa* thisv, GlobalContext* globalCtx);
void func_80AF68E4(EnSa* thisv, GlobalContext* globalCtx);
void func_80AF6B20(EnSa* thisv, GlobalContext* globalCtx);

typedef enum {
    /* 0 */ SARIA_EYE_OPEN,
    /* 1 */ SARIA_EYE_HALF,
    /* 2 */ SARIA_EYE_CLOSED,
    /* 3 */ SARIA_EYE_SUPRISED,
    /* 4 */ SARIA_EYE_SAD
} SariaEyeState;

typedef enum {
    /* 0 */ SARIA_MOUTH_CLOSED2,
    /* 1 */ SARIA_MOUTH_SUPRISED,
    /* 2 */ SARIA_MOUTH_CLOSED,
    /* 3 */ SARIA_MOUTH_SMILING_OPEN,
    /* 4 */ SARIA_MOUTH_FROWNING
} SariaMouthState;

const ActorInit En_Sa_InitVars = {
    ACTOR_EN_SA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SA,
    sizeof(EnSa),
    (ActorFunc)EnSa_Init,
    (ActorFunc)EnSa_Destroy,
    (ActorFunc)EnSa_Update,
    (ActorFunc)EnSa_Draw,
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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum {
    /*  0 */ ENSA_ANIM1_0,
    /*  1 */ ENSA_ANIM1_1,
    /*  2 */ ENSA_ANIM1_2,
    /*  3 */ ENSA_ANIM1_3,
    /*  4 */ ENSA_ANIM1_4,
    /*  5 */ ENSA_ANIM1_5,
    /*  6 */ ENSA_ANIM1_6,
    /*  7 */ ENSA_ANIM1_7,
    /*  8 */ ENSA_ANIM1_8,
    /*  9 */ ENSA_ANIM1_9,
    /* 10 */ ENSA_ANIM1_10,
    /* 11 */ ENSA_ANIM1_11
} EnSaAnimation1;

static AnimationFrameCountInfo sAnimationInfo1[] = {
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSariaLookUpArmExtendedAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaWaveAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaRunAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaLookOverShoulderAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaStopPlayingOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaOcarinaToMouthAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaLinkLearnedSariasSongAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaReturnToOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
};

typedef enum {
    /* 0 */ ENSA_ANIM2_0,
    /* 1 */ ENSA_ANIM2_1,
    /* 2 */ ENSA_ANIM2_2,
    /* 3 */ ENSA_ANIM2_3,
    /* 4 */ ENSA_ANIM2_4,
    /* 5 */ ENSA_ANIM2_5,
    /* 6 */ ENSA_ANIM2_6,
    /* 7 */ ENSA_ANIM2_7,
    /* 8 */ ENSA_ANIM2_8,
    /* 9 */ ENSA_ANIM2_9
} EnSaAnimation2;

static AnimationInfo sAnimationInfo2[] = {
    { &gSariaTransitionHandsSideToChestToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToBackAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -4.0f },
    { &gSariaRightArmExtendedWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOutAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaStandHandsOnHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaExtendRightArmAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsBehindBackWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOnFaceAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

s16 func_80AF5560(EnSa* thisv, GlobalContext* globalCtx) {
    s16 textState = Message_GetState(&globalCtx->msgCtx);

    if (thisv->unk_209 == TEXT_STATE_AWAITING_NEXT || thisv->unk_209 == TEXT_STATE_EVENT ||
        thisv->unk_209 == TEXT_STATE_CLOSING || thisv->unk_209 == TEXT_STATE_DONE_HAS_NEXT) {
        if (textState != thisv->unk_209) {
            thisv->unk_208++;
        }
    }
    thisv->unk_209 = textState;
    return textState;
}

u16 func_80AF55E0(GlobalContext* globalCtx, Actor* thisx) {
    EnSa* thisv = (EnSa*)thisx;
    u16 reaction = Text_GetFaceReaction(globalCtx, 0x10);

    if (reaction != 0) {
        return reaction;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
        return 0x10AD;
    }
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        thisv->unk_208 = 0;
        thisv->unk_209 = TEXT_STATE_NONE;
        if (gSaveContext.infTable[0] & 0x20) {
            return 0x1048;
        } else {
            return 0x1047;
        }
    }
    if (gSaveContext.eventChkInf[0] & 4) {
        thisv->unk_208 = 0;
        thisv->unk_209 = TEXT_STATE_NONE;
        if (gSaveContext.infTable[0] & 8) {
            return 0x1032;
        } else {
            return 0x1031;
        }
    }
    if (gSaveContext.infTable[0] & 1) {
        thisv->unk_208 = 0;
        thisv->unk_209 = TEXT_STATE_NONE;
        if (gSaveContext.infTable[0] & 2) {
            return 0x1003;
        } else {
            return 0x1002;
        }
    }
    return 0x1001;
}

s16 func_80AF56F4(GlobalContext* globalCtx, Actor* thisx) {
    s16 ret = 1;
    EnSa* thisv = (EnSa*)thisx;

    switch (func_80AF5560(thisv, globalCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisv->actor.textId) {
                case 0x1002:
                    gSaveContext.infTable[0] |= 2;
                    ret = 0;
                    break;
                case 0x1031:
                    gSaveContext.eventChkInf[0] |= 8;
                    gSaveContext.infTable[0] |= 8;
                    ret = 0;
                    break;
                case 0x1047:
                    gSaveContext.infTable[0] |= 0x20;
                    ret = 0;
                    break;
                default:
                    ret = 0;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return ret;
}

void func_80AF57D8(EnSa* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum != SCENE_SPOT05 ||
        ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) < 0x1555 || thisv->unk_1E0.unk_00 != 0) {
        func_800343CC(globalCtx, &thisv->actor, &thisv->unk_1E0.unk_00, thisv->collider.dim.radius + 30.0f, func_80AF55E0,
                      func_80AF56F4);
    }
}

f32 func_80AF5894(EnSa* thisv) {
    f32 endFrame = thisv->skelAnime.endFrame;
    f32 startFrame = thisv->skelAnime.startFrame;

    thisv->skelAnime.startFrame = endFrame;
    thisv->skelAnime.curFrame = endFrame;
    thisv->skelAnime.endFrame = startFrame;
    thisv->skelAnime.playSpeed = -1.0f;
    return startFrame;
}

void func_80AF58B8(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_3);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_2);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF594C(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_8);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF59E0(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_7);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF5A74(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_1);
            func_80AF5894(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF5B10(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_4);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF5BA4(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_6);
            func_80AF5894(thisv);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_9);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF5C40(EnSa* thisv) {
    switch (thisv->unk_20A) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_5);
            thisv->unk_20A++;
        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo2, ENSA_ANIM2_0);
                thisv->unk_20A++;
            }
            break;
    }
}

void func_80AF5CD4(EnSa* thisv, u8 arg1) {
    thisv->unk_20B = arg1;
    thisv->unk_20A = 0;
}

void func_80AF5CE4(EnSa* thisv) {
    switch (thisv->unk_20B) {
        case 1:
            func_80AF58B8(thisv);
            break;
        case 2:
            func_80AF594C(thisv);
            break;
        case 3:
            func_80AF59E0(thisv);
            break;
        case 4:
            func_80AF5A74(thisv);
            break;
        case 5:
            func_80AF5B10(thisv);
            break;
        case 6:
            func_80AF5BA4(thisv);
            break;
        case 7:
            func_80AF5C40(thisv);
            break;
    }
}

void EnSa_ChangeAnim(EnSa* thisv, s32 index) {
    Animation_Change(&thisv->skelAnime, sAnimationInfo1[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo1[index].animation), sAnimationInfo1[index].mode,
                     sAnimationInfo1[index].morphFrames);
}

s32 func_80AF5DFC(EnSa* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.cutsceneIndex >= 0xFFF0 && gSaveContext.cutsceneIndex != 0xFFFD) {
        if (globalCtx->sceneNum == SCENE_SPOT04) {
            return 4;
        }
        if (globalCtx->sceneNum == SCENE_SPOT05) {
            return 5;
        }
    }
    if (globalCtx->sceneNum == SCENE_KOKIRI_HOME5 && !LINK_IS_ADULT &&
        INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY && !(gSaveContext.eventChkInf[4] & 1)) {
        return 1;
    }
    if (globalCtx->sceneNum == SCENE_SPOT05 && (gSaveContext.eventChkInf[4] & 1)) {
        return CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? 2 : 5;
    }
    if (globalCtx->sceneNum == SCENE_SPOT04 && !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        if (gSaveContext.infTable[0] & 1) {
            return 1;
        }
        return 4;
    }
    return 0;
}

void func_80AF5F34(EnSa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a3 = 0;

    if (globalCtx->sceneNum == SCENE_SPOT04) {
        phi_a3 = (thisv->actionFunc == func_80AF68E4) ? 1 : 4;
    }
    if (globalCtx->sceneNum == SCENE_SPOT05) {
        phi_a3 = (thisv->skelAnime.animation == &gSariaPlayingOcarinaAnim) ? 1 : 3;
    }
    if (globalCtx->sceneNum == SCENE_SPOT05 && thisv->actionFunc == func_80AF6448 &&
        thisv->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        phi_a3 = 1;
    }
    if (globalCtx->sceneNum == SCENE_SPOT05 && thisv->actionFunc == func_80AF68E4 &&
        thisv->skelAnime.animation == &gSariaOcarinaToMouthAnim) {
        phi_a3 = 1;
    }
    thisv->unk_1E0.unk_18 = player->actor.world.pos;
    thisv->unk_1E0.unk_14 = 4.0f;
    func_80034A14(&thisv->actor, &thisv->unk_1E0, 2, phi_a3);
}

s32 func_80AF603C(EnSa* thisv) {
    if (thisv->skelAnime.animation != &gSariaPlayingOcarinaAnim &&
        thisv->skelAnime.animation != &gSariaOcarinaToMouthAnim) {
        return 0;
    }
    if (thisv->unk_1E0.unk_00 != 0) {
        return 0;
    }
    thisv->unk_20E = 0;
    if (thisv->rightEyeIndex != SARIA_EYE_CLOSED) {
        return 0;
    }
    return 1;
}

void func_80AF609C(EnSa* thisv) {
    s16 phi_v1;

    if (func_80AF603C(thisv) == 0) {
        if (thisv->unk_20E == 0) {
            phi_v1 = 0;
        } else {
            thisv->unk_20E--;
            phi_v1 = thisv->unk_20E;
        }
        if (phi_v1 == 0) {
            thisv->rightEyeIndex++;
            if (thisv->rightEyeIndex < SARIA_EYE_SUPRISED) {
                thisv->leftEyeIndex = thisv->rightEyeIndex;
            } else {
                thisv->unk_20E = Rand_S16Offset(30, 30);
                thisv->leftEyeIndex = SARIA_EYE_OPEN;
                thisv->rightEyeIndex = thisv->leftEyeIndex;
            }
        }
    }
}

void func_80AF6130(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->startPos.x;
    dst->y = csAction->startPos.y;
    dst->z = csAction->startPos.z;
}

void func_80AF6170(CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->endPos.x;
    dst->y = csAction->endPos.y;
    dst->z = csAction->endPos.z;
}

void EnSa_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSa* thisv = (EnSa*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSariaSkel, NULL, thisv->jointTable, thisv->morphTable, 17);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);

    switch (func_80AF5DFC(thisv, globalCtx)) {
        case 2:
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_11);
            thisv->actionFunc = func_80AF6448;
            break;
        case 5:
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_11);
            thisv->actionFunc = func_80AF683C;
            break;
        case 1:
            thisv->actor.gravity = -1.0f;
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_0);
            thisv->actionFunc = func_80AF6448;
            break;
        case 4:
            thisv->unk_210 = 0;
            thisv->actor.gravity = -1.0f;
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gSpot04Cs_10E20);
            gSaveContext.cutsceneTrigger = 1;
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_4);
            thisv->actionFunc = func_80AF68E4;
            break;
        case 3:
            thisv->unk_210 = 0;
            thisv->actor.gravity = -1.0f;
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_0);
            thisv->actionFunc = func_80AF68E4;
            break;
        case 0:
            Actor_Kill(&thisv->actor);
            return;
    }

    Actor_SetScale(&thisv->actor, 0.01f);

    thisv->actor.targetMode = 6;
    thisv->unk_1E0.unk_00 = 0;
    thisv->alpha = 255;
    thisv->unk_21A = thisv->actor.shape.rot;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_ELF, thisv->actor.world.pos.x,
                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);
}

void EnSa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSa* thisv = (EnSa*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80AF6448(EnSa* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT04) {
        if (thisv->unk_1E0.unk_00 != 0) {
            switch (thisv->actor.textId) {
                case 0x1002:
                    if (thisv->unk_208 == 0 && thisv->unk_20B != 1) {
                        func_80AF5CD4(thisv, 1);
                        thisv->mouthIndex = 1;
                    }
                    if (thisv->unk_208 == 2 && thisv->unk_20B != 2) {
                        func_80AF5CD4(thisv, 2);
                        thisv->mouthIndex = 1;
                    }
                    if (thisv->unk_208 == 5) {
                        thisv->mouthIndex = 0;
                    }
                    break;
                case 0x1003:
                    if (thisv->unk_208 == 0 && thisv->unk_20B != 4) {
                        func_80AF5CD4(thisv, 4);
                    }
                    break;
                case 0x1031:
                    if (thisv->unk_208 == 0 && thisv->unk_20B != 4 &&
                        thisv->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        func_80AF5CD4(thisv, 4);
                        thisv->mouthIndex = 3;
                    }
                    if (thisv->unk_208 == 2 && thisv->unk_20B != 5) {
                        func_80AF5CD4(thisv, 5);
                        thisv->mouthIndex = 2;
                    }
                    if (thisv->unk_208 == 4 && thisv->unk_20B != 6) {
                        func_80AF5CD4(thisv, 6);
                        thisv->mouthIndex = 0;
                    }
                    break;
                case 0x1032:
                    if (thisv->unk_208 == 0 && thisv->unk_20B != 4 &&
                        thisv->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        func_80AF5CD4(thisv, 4);
                    }
                    break;
                case 0x1047:
                    if (thisv->unk_208 == 1 && thisv->unk_20B != 7) {
                        func_80AF5CD4(thisv, 7);
                    }
                    break;
                case 0x1048:
                    if (thisv->unk_208 == 0 && thisv->unk_20B != 7) {
                        func_80AF5CD4(thisv, 7);
                    }
                    break;
            }
        } else if (!CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) &&
                   ((gSaveContext.infTable[0] & 2) || (gSaveContext.infTable[0] & 8))) {
            if (thisv->unk_20B != 3) {
                func_80AF5CD4(thisv, 3);
            }
        } else {
            func_80AF5CD4(thisv, 0);
        }
        func_80AF5CE4(thisv);
    }
    if (thisv->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        thisv->skelAnime.playSpeed = -1.0f;
        if ((s32)thisv->skelAnime.curFrame == 0) {
            EnSa_ChangeAnim(thisv, ENSA_ANIM1_6);
        }
    }
    if (thisv->unk_1E0.unk_00 != 0 && globalCtx->sceneNum == SCENE_SPOT05) {
        Animation_Change(&thisv->skelAnime, &gSariaStopPlayingOcarinaAnim, 1.0f, 0.0f, 10.0f, ANIMMODE_ONCE, -10.0f);
        thisv->actionFunc = func_80AF67D0;
    }
}

void func_80AF67D0(EnSa* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1E0.unk_00 == 0) {
        Animation_Change(&thisv->skelAnime, &gSariaStopPlayingOcarinaAnim, 0.0f, 10.0f, 0.0f, ANIMMODE_ONCE, -10.0f);
        thisv->actionFunc = func_80AF6448;
    }
}

void func_80AF683C(EnSa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!(player->actor.world.pos.z >= -2220.0f) && !Gameplay_InCsMode(globalCtx)) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(spot05_scene_Cs_005730);
        gSaveContext.cutsceneTrigger = 1;
        thisv->actionFunc = func_80AF68E4;
    }
}

void func_80AF68E4(EnSa* thisv, GlobalContext* globalCtx) {
    s16 phi_v0;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f D_80AF7448 = { 0.0f, 0.0f, 0.0f };
    CsCmdActorAction* csAction;
    f32 temp_f0;
    f32 gravity;

    if ((gSaveContext.cutsceneTrigger != 1) && (globalCtx->csCtx.state == CS_STATE_IDLE)) {
        thisv->actionFunc = func_80AF6B20;
        return;
    }
    csAction = globalCtx->csCtx.npcActions[1];
    if (csAction != NULL) {
        func_80AF6130(csAction, &startPos);
        func_80AF6170(csAction, &endPos);

        if (thisv->unk_210 == 0) {
            thisv->actor.world.pos = startPos;
        }
        if (thisv->unk_210 != csAction->action) {
            switch (csAction->action) {
                case 2:
                    thisv->mouthIndex = 1;
                    break;
                case 9:
                    thisv->mouthIndex = 1;
                    break;
                default:
                    thisv->mouthIndex = 0;
                    break;
            }
            EnSa_ChangeAnim(thisv, csAction->action);
            thisv->unk_210 = csAction->action;
        }
        if (phi_v0) {}
        if (csAction->action == 3) {
            if (thisv->unk_20C == 0) {
                phi_v0 = 0;
            } else {
                thisv->unk_20C--;
                phi_v0 = thisv->unk_20C;
            }
            if (phi_v0 == 0) {
                Audio_PlaySoundGeneral(NA_SE_PL_WALK_GROUND, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                thisv->unk_20C = 8;
            }
        }
        thisv->actor.shape.rot.x = csAction->urot.x;
        thisv->actor.shape.rot.y = csAction->urot.y;
        thisv->actor.shape.rot.z = csAction->urot.z;
        thisv->actor.velocity = D_80AF7448;

        if (globalCtx->csCtx.frames < csAction->endFrame) {
            temp_f0 = csAction->endFrame - csAction->startFrame;
            thisv->actor.velocity.x = (endPos.x - startPos.x) / temp_f0;
            thisv->actor.velocity.y = (endPos.y - startPos.y) / temp_f0;
            gravity = thisv->actor.gravity;
            if (globalCtx->sceneNum == SCENE_SPOT05) {
                gravity = 0.0f;
            }
            thisv->actor.velocity.y += gravity;
            if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
                thisv->actor.velocity.y = thisv->actor.minVelocityY;
            }
            thisv->actor.velocity.z = (endPos.z - startPos.z) / temp_f0;
        }
    }
}

void func_80AF6B20(EnSa* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT05) {
        Item_Give(globalCtx, ITEM_SONG_SARIA);
        EnSa_ChangeAnim(thisv, ENSA_ANIM1_6);
    }

    if (globalCtx->sceneNum == SCENE_SPOT04) {
        EnSa_ChangeAnim(thisv, ENSA_ANIM1_4);
        thisv->actor.world.pos = thisv->actor.home.pos;
        thisv->actor.world.rot = thisv->unk_21A;
        thisv->mouthIndex = 0;
        gSaveContext.infTable[0] |= 1;
    }

    thisv->actionFunc = func_80AF6448;
}

void EnSa_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSa* thisv = (EnSa*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->skelAnime.animation == &gSariaOcarinaToMouthAnim &&
        thisv->skelAnime.curFrame >= Animation_GetLastFrame(&gSariaOcarinaToMouthAnim)) {
        EnSa_ChangeAnim(thisv, ENSA_ANIM1_6);
    }

    if (thisv->actionFunc != func_80AF68E4) {
        thisv->alpha = func_80034DD4(&thisv->actor, globalCtx, thisv->alpha, 400.0f);
    } else {
        thisv->alpha = 255;
    }

    thisv->actor.shape.shadowAlpha = thisv->alpha;

    if (thisv->actionFunc == func_80AF68E4) {
        thisv->actor.world.pos.x += thisv->actor.velocity.x;
        thisv->actor.world.pos.y += thisv->actor.velocity.y;
        thisv->actor.world.pos.z += thisv->actor.velocity.z;
    } else {
        func_8002D7EC(&thisv->actor);
    }

    if (globalCtx->sceneNum != SCENE_SPOT05) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    }

    func_80AF609C(thisv);
    thisv->actionFunc(thisv, globalCtx);
    func_80AF57D8(thisv, globalCtx);
    func_80AF5F34(thisv, globalCtx);
}

s32 EnSa_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnSa* thisv = (EnSa*)thisx;
    s32 pad;
    Vec3s sp18;

    if (limbIndex == 16) {
        Matrix_Translate(900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp18 = thisv->unk_1E0.unk_08;
        Matrix_RotateX(BINANG_TO_RAD(sp18.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp18.x), MTXMODE_APPLY);
        Matrix_Translate(-900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 9) {
        sp18 = thisv->unk_1E0.unk_0E;
        Matrix_RotateY(BINANG_TO_RAD(sp18.y), MTXMODE_APPLY);
        Matrix_RotateX(BINANG_TO_RAD(sp18.x), MTXMODE_APPLY);
    }

    if (globalCtx->sceneNum == SCENE_SPOT05 && limbIndex == 15) {
        *dList = gSariaRightHandAndOcarinaDL;
    }

    return 0;
}

void EnSa_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnSa* thisv = (EnSa*)thisx;
    Vec3f D_80AF7454 = { 400.0, 0.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&D_80AF7454, &thisv->actor.focus.pos);
    }
}

void EnSa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* mouthTextures[] = {
        gSariaMouthClosed2Tex,  gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
        gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    };
    static void* eyeTextures[] = {
        gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
    };
    EnSa* thisv = (EnSa*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sa.c", 1444);

    if (thisv->alpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->rightEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->leftEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthIndex]));
        func_80034BA0(globalCtx, &thisv->skelAnime, EnSa_OverrideLimbDraw, EnSa_PostLimbDraw, &thisv->actor, thisv->alpha);
    } else if (thisv->alpha != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->rightEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->leftEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthIndex]));
        func_80034CC4(globalCtx, &thisv->skelAnime, EnSa_OverrideLimbDraw, EnSa_PostLimbDraw, &thisv->actor, thisv->alpha);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sa.c", 1497);
}
