#include "z_en_daiku.h"
#include "overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef struct {
    Vec3f eyePosDeltaLocal;
    s32 maxFramesActive;
} EnDaikuEscapeSubCamParam;

// state flags

// probably related to animating torso and head to look towards the player
#define ENDAIKU_STATEFLAG_1 (1 << 1)
// same
#define ENDAIKU_STATEFLAG_2 (1 << 2)
// the gerudo guard appeared (after talking to the carpenter)
#define ENDAIKU_STATEFLAG_GERUDOFIGHTING (1 << 3)
// the gerudo guard was defeated
#define ENDAIKU_STATEFLAG_GERUDODEFEATED (1 << 4)

typedef enum {
    /* 0 */ ENDAIKU_STATE_CAN_TALK,
    /* 2 */ ENDAIKU_STATE_TALKING = 2,
    /* 3 */ ENDAIKU_STATE_NO_TALK
} EnDaikuTalkState;

void EnDaiku_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDaiku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDaiku_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDaiku_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDaiku_TentIdle(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_Jailed(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_WaitFreedom(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_InitEscape(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_EscapeRotate(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_InitSubCamera(EnDaiku* thisv, GlobalContext* globalCtx);
void EnDaiku_EscapeRun(EnDaiku* thisv, GlobalContext* globalCtx);
s32 EnDaiku_OverrideLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnDaiku_PostLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3s* rot, void* thisx);

const ActorInit En_Daiku_InitVars = {
    ACTOR_EN_DAIKU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DAIKU,
    sizeof(EnDaiku),
    (ActorFunc)EnDaiku_Init,
    (ActorFunc)EnDaiku_Destroy,
    (ActorFunc)EnDaiku_Update,
    (ActorFunc)EnDaiku_Draw,
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
    { 18, 66, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = { 0, 0, 0, 0, MASS_IMMOVABLE };

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

typedef enum {
    /* 0 */ ENDAIKU_ANIM_SHOUT,
    /* 1 */ ENDAIKU_ANIM_STAND,
    /* 2 */ ENDAIKU_ANIM_CELEBRATE,
    /* 3 */ ENDAIKU_ANIM_RUN,
    /* 4 */ ENDAIKU_ANIM_SIT
} EnDaikuAnimation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 0, 0 }, { &object_daiku_Anim_007DE0, 1.0f, 0, 0 },
    { &object_daiku_Anim_00885C, 1.0f, 0, 0 }, { &object_daiku_Anim_000C44, 1.0f, 0, 0 },
    { &object_daiku_Anim_008164, 1.0f, 0, 0 },
};

static EnDaikuEscapeSubCamParam sEscapeSubCamParams[] = {
    { { 0, 130, 220 }, 100 },
    { { -20, 22, 280 }, 110 },
    { { 50, 180, 350 }, 100 },
    { { -40, 60, 60 }, 120 },
};

void EnDaiku_ChangeAnim(EnDaiku* thisv, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if (*currentIndex < 0 || *currentIndex == index) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode, morphFrames);

    *currentIndex = index;
}

void EnDaiku_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDaiku* thisv = (EnDaiku*)thisx;
    s32 pad;
    s32 noKill = true;
    s32 isFree = false;

    if ((thisv->actor.params & 3) == 0 && (gSaveContext.eventChkInf[9] & 1)) {
        isFree = true;
    } else if ((thisv->actor.params & 3) == 1 && (gSaveContext.eventChkInf[9] & 2)) {
        isFree = true;
    } else if ((thisv->actor.params & 3) == 2 && (gSaveContext.eventChkInf[9] & 4)) {
        isFree = true;
    } else if ((thisv->actor.params & 3) == 3 && (gSaveContext.eventChkInf[9] & 8)) {
        isFree = true;
    }

    if (isFree == true && globalCtx->sceneNum == SCENE_GERUDOWAY) {
        noKill = false;
    } else if (isFree == false && globalCtx->sceneNum == SCENE_TENT) {
        noKill = false;
    }

    thisv->startFightSwitchFlag = thisv->actor.shape.rot.z & 0x3F;
    thisv->actor.shape.rot.z = 0;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_daiku_Skel_007958, NULL, thisv->jointTable, thisv->morphTable,
                       17);

    if (!noKill) {
        Actor_Kill(&thisv->actor);
        return;
    }

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);

    Animation_Change(&thisv->skelAnime, sAnimationInfo[ENDAIKU_ANIM_SHOUT].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[ENDAIKU_ANIM_SHOUT].animation),
                     sAnimationInfo[ENDAIKU_ANIM_SHOUT].mode, sAnimationInfo[ENDAIKU_ANIM_SHOUT].morphFrames);

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    thisv->actor.targetMode = 6;
    thisv->currentAnimIndex = -1;
    thisv->runSpeed = 5.0f;
    thisv->initRot = thisv->actor.world.rot;
    thisv->initPos = thisv->actor.world.pos;

    if (globalCtx->sceneNum == SCENE_GERUDOWAY) {
        EnDaiku_ChangeAnim(thisv, ENDAIKU_ANIM_STAND, &thisv->currentAnimIndex);
        thisv->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        thisv->actionFunc = EnDaiku_Jailed;
    } else {
        if ((thisv->actor.params & 3) == 1 || (thisv->actor.params & 3) == 3) {
            EnDaiku_ChangeAnim(thisv, ENDAIKU_ANIM_SIT, &thisv->currentAnimIndex);
            thisv->stateFlags |= ENDAIKU_STATEFLAG_1;
        } else {
            EnDaiku_ChangeAnim(thisv, ENDAIKU_ANIM_SHOUT, &thisv->currentAnimIndex);
            thisv->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        }

        thisv->skelAnime.curFrame = (s32)(Rand_ZeroOne() * thisv->skelAnime.endFrame);
        thisv->actionFunc = EnDaiku_TentIdle;
    }
}

void EnDaiku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDaiku* thisv = (EnDaiku*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnDaiku_UpdateTalking(EnDaiku* thisv, GlobalContext* globalCtx) {
    s32 newTalkState = ENDAIKU_STATE_TALKING;

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) {
        if (globalCtx->sceneNum == SCENE_GERUDOWAY) {
            if (Message_ShouldAdvance(globalCtx)) {
                if (thisv->actor.textId == 0x6007) {
                    Flags_SetSwitch(globalCtx, thisv->startFightSwitchFlag);
                    newTalkState = ENDAIKU_STATE_CAN_TALK;
                } else {
                    thisv->actionFunc = EnDaiku_InitEscape;
                    newTalkState = ENDAIKU_STATE_NO_TALK;
                }
            }
        } else if (globalCtx->sceneNum == SCENE_TENT) {
            if (Message_ShouldAdvance(globalCtx)) {
                switch (thisv->actor.textId) {
                    case 0x6061:
                        gSaveContext.infTable[23] |= 0x40;
                        break;
                    case 0x6064:
                        gSaveContext.infTable[23] |= 0x100;
                        break;
                }

                newTalkState = ENDAIKU_STATE_CAN_TALK;
            }
        }
    }

    return newTalkState;
}

void EnDaiku_UpdateText(EnDaiku* thisv, GlobalContext* globalCtx) {
    s32 carpenterType;
    s32 freedCount;
    s16 sp2E;
    s16 sp2C;

    if (thisv->talkState == ENDAIKU_STATE_TALKING) {
        thisv->talkState = EnDaiku_UpdateTalking(thisv, globalCtx);
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->talkState = ENDAIKU_STATE_TALKING;
    } else {
        Actor_GetScreenPos(globalCtx, &thisv->actor, &sp2E, &sp2C);
        if (sp2E >= 0 && sp2E <= 320 && sp2C >= 0 && sp2C <= 240 && thisv->talkState == ENDAIKU_STATE_CAN_TALK &&
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f) == 1) {
            if (globalCtx->sceneNum == SCENE_GERUDOWAY) {
                if (thisv->stateFlags & ENDAIKU_STATEFLAG_GERUDODEFEATED) {
                    freedCount = 0;
                    for (carpenterType = 0; carpenterType < 4; carpenterType++) {
                        if (gSaveContext.eventChkInf[9] & (1 << carpenterType)) {
                            freedCount++;
                        }
                    }

                    switch (freedCount) {
                        case 0:
                            thisv->actor.textId = 0x605B;
                            break;
                        case 1:
                            thisv->actor.textId = 0x605C;
                            break;
                        case 2:
                            thisv->actor.textId = 0x605D;
                            break;
                        case 3:
                            thisv->actor.textId = 0x605E;
                            break;
                    }
                } else if (!(thisv->stateFlags &
                             (ENDAIKU_STATEFLAG_GERUDOFIGHTING | ENDAIKU_STATEFLAG_GERUDODEFEATED))) {
                    thisv->actor.textId = 0x6007;
                }
            } else if (globalCtx->sceneNum == SCENE_TENT) {
                switch (thisv->actor.params & 3) {
                    case 0:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            thisv->actor.textId = 0x6060;
                        } else {
                            thisv->actor.textId = 0x605F;
                        }
                        break;
                    case 1:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            thisv->actor.textId = 0x6063;
                        } else {
                            if (!(gSaveContext.infTable[23] & 0x40)) {
                                thisv->actor.textId = 0x6061;
                            } else {
                                thisv->actor.textId = 0x6062;
                            }
                        }
                        break;
                    case 2:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            thisv->actor.textId = 0x6066;
                        } else {
                            if (!(gSaveContext.infTable[23] & 0x100)) {
                                thisv->actor.textId = 0x6064;
                            } else {
                                thisv->actor.textId = 0x6065;
                            }
                        }
                        break;
                    case 3:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            thisv->actor.textId = 0x6068;
                        } else {
                            thisv->actor.textId = 0x6067;
                        }
                        break;
                }
            }
        }
    }
}

/**
 * The carpenter is idling in the tent.
 */
void EnDaiku_TentIdle(EnDaiku* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    EnDaiku_UpdateText(thisv, globalCtx);
}

/**
 * The carpenter is jailed in a Gerudo fortress cell, talking to him starts a fight against a gerudo guard
 */
void EnDaiku_Jailed(EnDaiku* thisv, GlobalContext* globalCtx) {
    EnGeldB* gerudo;
    s32 temp_t9;
    s32 temp_v1;

    if (!(thisv->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING)) {
        EnDaiku_UpdateText(thisv, globalCtx);
    }
    SkelAnime_Update(&thisv->skelAnime);

    gerudo = (EnGeldB*)Actor_Find(&globalCtx->actorCtx, ACTOR_EN_GELDB, ACTORCAT_ENEMY);
    if (gerudo == NULL) {
        thisv->stateFlags |= ENDAIKU_STATEFLAG_GERUDODEFEATED;
        thisv->stateFlags &= ~ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        EnDaiku_ChangeAnim(thisv, ENDAIKU_ANIM_CELEBRATE, &thisv->currentAnimIndex);
        thisv->actionFunc = EnDaiku_WaitFreedom;
    } else if (!(thisv->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING) && !gerudo->invisible) {
        thisv->stateFlags |= ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    }
}

/**
 * The player defeated the gerudo guard and the carpenter is waiting for the cell door to be opened, and for the player
 * to then talk to him
 */
void EnDaiku_WaitFreedom(EnDaiku* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Flags_GetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F)) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        EnDaiku_UpdateText(thisv, globalCtx);
    }
}

/**
 * The carpenter is free, initializes his running away animation
 */
void EnDaiku_InitEscape(EnDaiku* thisv, GlobalContext* globalCtx) {
    Path* path;
    f32 dxz;
    f32 dx;
    f32 dz;
    Vec3s* pointPos;
    s32 exitLoop;

    Audio_PlayFanfare(NA_BGM_APPEAR);
    EnDaiku_ChangeAnim(thisv, ENDAIKU_ANIM_RUN, &thisv->currentAnimIndex);
    thisv->stateFlags &= ~(ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2);

    gSaveContext.eventChkInf[9] |= 1 << (thisv->actor.params & 3);

    thisv->actor.gravity = -1.0f;
    thisv->escapeSubCamTimer = sEscapeSubCamParams[thisv->actor.params & 3].maxFramesActive;
    EnDaiku_InitSubCamera(thisv, globalCtx);

    exitLoop = false;
    path = &globalCtx->setupPathList[thisv->actor.params >> 4 & 0xF];
    while (!exitLoop) {
        pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->waypoint;
        dx = pointPos->x - thisv->actor.world.pos.x;
        dz = pointPos->z - thisv->actor.world.pos.z;
        thisv->rotYtowardsPath = Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>);
        dxz = sqrtf(SQ(dx) + SQ(dz));
        if (dxz > 10.0f) {
            exitLoop = true;
        } else {
            thisv->waypoint++;
        }
    }

    thisv->actionFunc = EnDaiku_EscapeRotate;
}

/**
 * The carpenter is rotating towards where he is going next
 */
void EnDaiku_EscapeRotate(EnDaiku* thisv, GlobalContext* globalCtx) {
    s16 diff;

    diff = Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->rotYtowardsPath, 1, 0x1388, 0);
    SkelAnime_Update(&thisv->skelAnime);
    if (diff == 0) {
        thisv->actionFunc = EnDaiku_EscapeRun;
        thisv->actionFunc(thisv, globalCtx);
    }
}

void EnDaiku_InitSubCamera(EnDaiku* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f eyePosDeltaLocal;
    Vec3f eyePosDeltaWorld;

    thisv->subCamActive = true;
    thisv->escapeSubCamTimer = sEscapeSubCamParams[thisv->actor.params & 3].maxFramesActive;

    eyePosDeltaLocal.x = sEscapeSubCamParams[thisv->actor.params & 3].eyePosDeltaLocal.x;
    eyePosDeltaLocal.y = sEscapeSubCamParams[thisv->actor.params & 3].eyePosDeltaLocal.y;
    eyePosDeltaLocal.z = sEscapeSubCamParams[thisv->actor.params & 3].eyePosDeltaLocal.z;
    Matrix_RotateY(thisv->actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
    Matrix_MultVec3f(&eyePosDeltaLocal, &eyePosDeltaWorld);

    thisv->subCamEyeInit.x = thisv->subCamEye.x = thisv->actor.world.pos.x + eyePosDeltaWorld.x;
    thisv->subCamEyeInit.y = thisv->subCamEye.y = thisv->actor.world.pos.y + eyePosDeltaWorld.y;
    if (1) {}
    thisv->subCamEyeInit.z = thisv->subCamEye.z = thisv->actor.world.pos.z + eyePosDeltaWorld.z;

    if (1) {}
    thisv->subCamAtTarget.x = thisv->subCamAt.x = thisv->actor.world.pos.x;
    thisv->subCamAtTarget.y = thisv->subCamAt.y = thisv->actor.world.pos.y + 60.0f;
    if (1) {}
    thisv->subCamAtTarget.z = thisv->subCamAt.z = thisv->actor.world.pos.z;

    thisv->subCamId = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->subCamId, CAM_STAT_ACTIVE);

    Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt, &thisv->subCamEye);
    Gameplay_CameraSetFov(globalCtx, thisv->subCamId, globalCtx->mainCamera.fov);
    func_8002DF54(globalCtx, &thisv->actor, 1);
}

void EnDaiku_UpdateSubCamera(EnDaiku* thisv, GlobalContext* globalCtx) {
    s32 pad;

    thisv->subCamAtTarget.x = thisv->actor.world.pos.x;
    thisv->subCamAtTarget.y = thisv->actor.world.pos.y + 60.0f;
    thisv->subCamAtTarget.z = thisv->actor.world.pos.z;

    Math_SmoothStepToF(&thisv->subCamAt.x, thisv->subCamAtTarget.x, 1.0f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&thisv->subCamAt.y, thisv->subCamAtTarget.y, 1.0f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&thisv->subCamAt.z, thisv->subCamAtTarget.z, 1.0f, 1000.0f, 0.0f);

    Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt, &thisv->subCamEye);
}

void EnDaiku_EscapeSuccess(EnDaiku* thisv, GlobalContext* globalCtx) {
    static Vec3f D_809E4148 = { 0.0f, 0.0f, 120.0f };
    Actor* gerudoGuard;
    Vec3f vec;

    Gameplay_ClearCamera(globalCtx, thisv->subCamId);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
    thisv->subCamActive = false;

    if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        Matrix_RotateY(thisv->initRot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
        Matrix_MultVec3f(&D_809E4148, &vec);
        gerudoGuard =
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_GE3, thisv->initPos.x + vec.x, thisv->initPos.y + vec.y,
                        thisv->initPos.z + vec.z, 0, Math_FAtan2F(-vec.x, -vec.z) * (0x8000 / std::numbers::pi_v<float>), 0, 2);

        if (gerudoGuard == NULL) {
            Actor_Kill(&thisv->actor);
        }
    } else {
        func_8002DF54(globalCtx, &thisv->actor, 7);
    }
}

/**
 * The carpenter is running away
 */
void EnDaiku_EscapeRun(EnDaiku* thisv, GlobalContext* globalCtx) {
    s32 pad1;
    Path* path;
    s16 ry;
    f32 dx;
    f32 dz;
    s32 pad2;
    f32 dxz;
    Vec3s* pointPos;

    path = &globalCtx->setupPathList[thisv->actor.params >> 4 & 0xF];
    pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + thisv->waypoint;
    dx = pointPos->x - thisv->actor.world.pos.x;
    dz = pointPos->z - thisv->actor.world.pos.z;
    ry = Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>);
    dxz = sqrtf(SQ(dx) + SQ(dz));
    if (dxz <= 20.88f) {
        thisv->waypoint++;
        if (thisv->waypoint >= path->count) {
            if (thisv->subCamActive) {
                EnDaiku_EscapeSuccess(thisv, globalCtx);
            }
            Actor_Kill(&thisv->actor);
            return;
        }
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, ry, 1, 0xFA0, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->runSpeed, 0.6f, dxz, 0.0f);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    if (thisv->subCamActive) {
        EnDaiku_UpdateSubCamera(thisv, globalCtx);
        if (thisv->escapeSubCamTimer-- <= 0) {
            EnDaiku_EscapeSuccess(thisv, globalCtx);
        }
    }

    SkelAnime_Update(&thisv->skelAnime);
}

void EnDaiku_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDaiku* thisv = (EnDaiku*)thisx;
    s32 curFrame;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->currentAnimIndex == ENDAIKU_ANIM_RUN) {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame == 6 || curFrame == 15) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->stateFlags & ENDAIKU_STATEFLAG_1) {
        thisv->unk_244.unk_18.x = player->actor.focus.pos.x;
        thisv->unk_244.unk_18.y = player->actor.focus.pos.y;
        thisv->unk_244.unk_18.z = player->actor.focus.pos.z;

        if (thisv->stateFlags & ENDAIKU_STATEFLAG_2) {
            func_80034A14(&thisv->actor, &thisv->unk_244, 0, 4);
        } else {
            func_80034A14(&thisv->actor, &thisv->unk_244, 0, 2);
        }
    }
}

void EnDaiku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDaiku* thisv = (EnDaiku*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku.c", 1227);

    func_80093D18(globalCtx->state.gfxCtx);

    if ((thisx->params & 3) == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if ((thisx->params & 3) == 1) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if ((thisx->params & 3) == 2) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if ((thisx->params & 3) == 3) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDaiku_OverrideLimbDraw, EnDaiku_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku.c", 1255);
}

s32 EnDaiku_OverrideLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDaiku* thisv = (EnDaiku*)thisx;

    switch (limb) {
        case 8: // torso
            rot->x += thisv->unk_244.unk_0E.y;
            rot->y -= thisv->unk_244.unk_0E.x;
            break;
        case 15: // head
            rot->x += thisv->unk_244.unk_08.y;
            rot->z += thisv->unk_244.unk_08.x;
            break;
    }

    return false;
}

void EnDaiku_PostLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3s* rot, void* thisx) {
    static Gfx* hairDLists[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                 object_daiku_DL_005880 };
    static Vec3f targetPosHeadLocal = { 700, 1100, 0 };
    EnDaiku* thisv = (EnDaiku*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku.c", 1323);

    if (limb == 15) { // head
        Matrix_MultVec3f(&targetPosHeadLocal, &thisv->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, hairDLists[thisv->actor.params & 3]);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku.c", 1330);
}
