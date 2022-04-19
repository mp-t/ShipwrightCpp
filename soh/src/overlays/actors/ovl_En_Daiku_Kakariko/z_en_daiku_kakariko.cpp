/*
 * File: z_en_daiku_kakariko.c
 * Overlay: ovl_En_Daiku_Kakariko
 * Description: Kakariko Village Carpenters
 */

#include "z_en_daiku_kakariko.h"
#include "objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

typedef enum {
    /* 0x0 */ CARPENTER_ICHIRO,  // Red and purple pants, normal hair
    /* 0x1 */ CARPENTER_SABOORO, // Light blue pants
    /* 0x2 */ CARPENTER_JIRO,    // Green pants
    /* 0x3 */ CARPENTER_SHIRO    // Pink and purple pants, two-spiked hair
} KakarikoCarpenterType;

void EnDaikuKakariko_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDaikuKakariko_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDaikuKakariko_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDaikuKakariko_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDaikuKakariko_Wait(EnDaikuKakariko* thisv, GlobalContext* globalCtx);
void EnDaikuKakariko_Run(EnDaikuKakariko* thisv, GlobalContext* globalCtx);

ActorInit En_Daiku_Kakariko_InitVars = {
    ACTOR_EN_DAIKU_KAKARIKO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DAIKU,
    sizeof(EnDaikuKakariko),
    (ActorFunc)EnDaikuKakariko_Init,
    (ActorFunc)EnDaikuKakariko_Destroy,
    (ActorFunc)EnDaikuKakariko_Update,
    (ActorFunc)EnDaikuKakariko_Draw,
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

static CollisionCheckInfoInit2 sColChkInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

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
    /* 0 */ ENDAIKUKAKARIKO_ANIM_0,
    /* 1 */ ENDAIKUKAKARIKO_ANIM_1,
    /* 2 */ ENDAIKUKAKARIKO_ANIM_2,
    /* 3 */ ENDAIKUKAKARIKO_ANIM_3,
    /* 4 */ ENDAIKUKAKARIKO_ANIM_4,
    /* 5 */ ENDAIKUKAKARIKO_ANIM_5
} EnDaikuKakarikoAnimation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 2, -7.0f }, { &object_daiku_Anim_007DE0, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_00885C, 1.0f, 0, -7.0f }, { &object_daiku_Anim_000C44, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_000600, 1.0f, 0, -7.0f }, { &object_daiku_Anim_008164, 1.0f, 0, -7.0f },
};

void EnDaikuKakariko_ChangeAnim(EnDaikuKakariko* thisv, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = sAnimationInfo[index].morphFrames;
    }

    Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode, morphFrames);

    *currentIndex = index;
}

void EnDaikuKakariko_Init(Actor* thisx, GlobalContext* globalCtx) {
    static u16 initFlags[] = { 0x0080, 0x00B0, 0x0070, 0x0470 }; // List of inital values for thisv->flags
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        switch (globalCtx->sceneNum) {
            case SCENE_SPOT01:
                if (IS_DAY) {
                    thisv->flags |= 1;
                    thisv->flags |= initFlags[thisv->actor.params & 3];
                }
                break;
            case SCENE_KAKARIKO:
                if (IS_NIGHT) {
                    thisv->flags |= 2;
                }
                break;
            case SCENE_DRAG:
                thisv->flags |= 4;
                break;
        }
    }

    if (!(thisv->flags & 7)) {
        Actor_Kill(&thisv->actor);
    }

    if (IS_NIGHT) {
        thisv->flags |= 8;
    }

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_daiku_Skel_007958, NULL, thisv->jointTable, thisv->morphTable,
                       17);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);

    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInit);

    Animation_Change(&thisv->skelAnime, sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].animation),
                     sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].mode, sAnimationInfo[ENDAIKUKAKARIKO_ANIM_0].morphFrames);

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    thisv->actor.gravity = 0.0f;
    thisv->runSpeed = 3.0f;
    thisv->actor.uncullZoneForward = 1200.0f;
    thisv->actor.targetMode = 6;
    thisv->currentAnimIndex = -1;

    if (thisv->flags & 0x40) {
        thisv->actor.gravity = -1.0f;
    }

    if (thisv->flags & 0x10) {
        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_3, &thisv->currentAnimIndex);
        thisv->actionFunc = EnDaikuKakariko_Run;
    } else {
        if (thisv->flags & 8) {
            if (((thisv->actor.params & 3) == CARPENTER_SABOORO) || ((thisv->actor.params & 3) == CARPENTER_SHIRO)) {
                EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_5, &thisv->currentAnimIndex);
                thisv->flags |= 0x800;
            } else {
                EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_1, &thisv->currentAnimIndex);
            }

            thisv->skelAnime.curFrame = (s32)(Rand_ZeroOne() * thisv->skelAnime.endFrame);
        } else {
            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_0, &thisv->currentAnimIndex);
            thisv->skelAnime.curFrame = (s32)(Rand_ZeroOne() * thisv->skelAnime.endFrame);
        }

        thisv->flags |= 0x100;
        thisv->actionFunc = EnDaikuKakariko_Wait;
    }
}

void EnDaikuKakariko_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnDaikuKakariko_GetTalkState(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    s32 talkState = 2;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        switch (thisv->actor.textId) {
            case 0x6061:
                gSaveContext.infTable[23] |= 0x40;
                break;
            case 0x6064:
                gSaveContext.infTable[23] |= 0x100;
                break;
        }
        talkState = 0;
    }
    return talkState;
}

void EnDaikuKakariko_HandleTalking(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    static s32 maskReactionSets[] = { 1, 2, 3, 4 };
    s16 sp26;
    s16 sp24;

    if (thisv->talkState == 2) {
        thisv->talkState = EnDaikuKakariko_GetTalkState(thisv, globalCtx);
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->talkState = 2;
    } else {
        Actor_GetScreenPos(globalCtx, &thisv->actor, &sp26, &sp24);

        if ((sp26 >= 0) && (sp26 <= 320) && (sp24 >= 0) && (sp24 <= 240) && (thisv->talkState == 0) &&
            (func_8002F2CC(&thisv->actor, globalCtx, 100.0f) == 1)) {
            thisv->actor.textId = Text_GetFaceReaction(globalCtx, maskReactionSets[thisv->actor.params & 3]);

            if (thisv->actor.textId == 0) {
                switch (thisv->actor.params & 3) {
                    case 0:
                        if (thisv->flags & 8) {
                            thisv->actor.textId = 0x5076;
                        } else {
                            thisv->actor.textId = 0x5075;
                        }
                        break;
                    case 1:
                        if (thisv->flags & 1) {
                            thisv->actor.textId = 0x502A;
                        } else {
                            thisv->actor.textId = 0x5074;
                        }
                        break;
                    case 2:
                        if (thisv->flags & 1) {
                            thisv->actor.textId = 0x506A;
                        } else {
                            thisv->actor.textId = 0x506B;
                        }
                        break;
                    case 3:
                        if (thisv->flags & 1) {
                            thisv->actor.textId = 0x5077;
                        } else {
                            thisv->actor.textId = 0x5078;
                        }
                        break;
                }
            }
        }
    }
}

void EnDaikuKakariko_Talk(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_3, &thisv->currentAnimIndex);
    }

    EnDaikuKakariko_HandleTalking(thisv, globalCtx);

    if (thisv->talkState == 0) {
        if (thisv->flags & 0x10) {
            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_3, &thisv->currentAnimIndex);
            thisv->flags &= ~0x0300;
            thisv->actionFunc = EnDaikuKakariko_Run;
            return;
        }

        if (!(thisv->flags & 8)) {
            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_0, &thisv->currentAnimIndex);
        }

        if ((thisv->flags & 0x800) == 0) {
            thisv->flags &= ~0x0200;
            thisv->flags |= 0x100;
        }

        thisv->actionFunc = EnDaikuKakariko_Wait;
    }
}

void EnDaikuKakariko_Wait(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    EnDaikuKakariko_HandleTalking(thisv, globalCtx);

    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_0, &thisv->currentAnimIndex);
    }

    if (thisv->talkState != 0) {
        if (!(thisv->flags & 8)) {
            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_4, &thisv->currentAnimIndex);
        }

        if (!(thisv->flags & 0x800)) {
            thisv->flags |= 0x200;
            thisv->flags &= ~0x0100;
        }

        thisv->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_StopRunning(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->timer--;

        if (thisv->timer <= 0) {
            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_3, &thisv->currentAnimIndex);
            thisv->actionFunc = EnDaikuKakariko_Run;
        } else {
            thisv->skelAnime.curFrame = thisv->skelAnime.startFrame;
        }
    }

    EnDaikuKakariko_HandleTalking(thisv, globalCtx);

    if (thisv->talkState != 0) {
        thisv->flags |= 0x200;
        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_4, &thisv->currentAnimIndex);
        thisv->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_Run(EnDaikuKakariko* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Path* path;
    Vec3s* pathPos;
    f32 xDist;
    f32 zDist;
    s16 runAngle;
    f32 runDist;
    s16 angleStepDiff;
    s32 run;

    do {
        path = &globalCtx->setupPathList[(thisv->actor.params >> 8) & 0xFF];
        pathPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[thisv->waypoint];
        xDist = pathPos->x - thisv->actor.world.pos.x;
        zDist = pathPos->z - thisv->actor.world.pos.z;
        runAngle = Math_FAtan2F(xDist, zDist) * (32768.0f / std::numbers::pi_v<float>);
        runDist = sqrtf((xDist * xDist) + (zDist * zDist));

        run = false;

        if (runDist <= 10.0f) {
            if (thisv->pathContinue == false) {
                thisv->waypoint++;

                if (thisv->waypoint >= path->count) {
                    if (thisv->flags & 0x20) {
                        thisv->waypoint = path->count - 2;
                        thisv->pathContinue = true;
                        thisv->run = run = false;

                        if (thisv->flags & 0x400) {
                            thisv->timer = 2;
                            EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_0, &thisv->currentAnimIndex);
                            thisv->actionFunc = EnDaikuKakariko_StopRunning;
                            return;
                        }
                    } else {
                        thisv->waypoint = 0;
                        run = true;
                    }
                } else {
                    thisv->run = run = true;
                }
            } else {
                thisv->waypoint--;

                if (thisv->waypoint < 0) {
                    thisv->waypoint = 1;
                    thisv->pathContinue = false;
                    thisv->run = run = false;

                    if (thisv->flags & 0x400) {
                        thisv->timer = 2;
                        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_0, &thisv->currentAnimIndex);
                        thisv->actionFunc = EnDaikuKakariko_StopRunning;
                        return;
                    }
                } else {
                    run = true;
                }
            }
        }
    } while (run);

    angleStepDiff = Math_SmoothStepToS(&thisv->actor.shape.rot.y, runAngle, 1, 5000, 0);

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

    if (thisv->run == false) {
        if (angleStepDiff == 0) {
            thisv->run = true;
        } else {
            thisv->actor.speedXZ = 0.0f;
        }
    }

    if (thisv->run == true) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->runSpeed, 0.8f, runDist, 0.0f);
    }

    Actor_MoveForward(&thisv->actor);

    if (thisv->flags & 0x40) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    } else if (thisv->flags & 0x80) {
        thisv->runFlag |= 1;
        thisv->flags &= ~0x0080;
    } else if (thisv->runFlag & 1) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
        thisv->runFlag &= ~1;
    }

    SkelAnime_Update(&thisv->skelAnime);
    EnDaikuKakariko_HandleTalking(thisv, globalCtx);

    if (thisv->talkState != 0) {
        thisv->flags |= 0x200;
        EnDaikuKakariko_ChangeAnim(thisv, ENDAIKUKAKARIKO_ANIM_4, &thisv->currentAnimIndex);
        thisv->actionFunc = EnDaikuKakariko_Talk;
    }
}

void EnDaikuKakariko_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad2;

    if (thisv->currentAnimIndex == 3) {
        if (((s32)thisv->skelAnime.curFrame == 6) || ((s32)thisv->skelAnime.curFrame == 15)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if (thisv->flags & 4) {
        thisv->collider.dim.pos.x -= 27;
        thisv->collider.dim.pos.z -= 27;
        thisv->collider.dim.radius = 63;
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    thisv->actionFunc(thisv, globalCtx);

    thisv->npcInfo.unk_18.x = player->actor.focus.pos.x;
    thisv->npcInfo.unk_18.y = player->actor.focus.pos.y;
    thisv->npcInfo.unk_18.z = player->actor.focus.pos.z;

    if (thisv->flags & 0x100) {
        thisv->neckAngleTarget.x = 5900;
        thisv->flags |= 0x1000;
        func_80034A14(&thisv->actor, &thisv->npcInfo, 0, 2);
    } else if (thisv->flags & 0x200) {
        thisv->neckAngleTarget.x = 5900;
        thisv->flags |= 0x1000;
        func_80034A14(&thisv->actor, &thisv->npcInfo, 0, 4);
    }

    Math_SmoothStepToS(&thisv->neckAngle.x, thisv->neckAngleTarget.x, 1, 1820, 0);
}

s32 EnDaikuKakariko_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     void* thisx) {
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;
    Vec3s angle;

    switch (limbIndex) {
        case 8:
            angle = thisv->npcInfo.unk_0E;
            Matrix_RotateX(-(angle.y * (std::numbers::pi_v<float> / 32768.0f)), MTXMODE_APPLY);
            Matrix_RotateZ(-(angle.x * (std::numbers::pi_v<float> / 32768.0f)), MTXMODE_APPLY);
            break;
        case 15:
            Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            angle = thisv->npcInfo.unk_08;

            if (thisv->flags & 0x1000) {
                osSyncPrintf("<%d>\n", thisv->neckAngle.x);
                Matrix_RotateX((angle.y + thisv->neckAngle.y) * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);
                Matrix_RotateZ((angle.x + thisv->neckAngle.x) * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);
            } else {
                Matrix_RotateX(angle.y * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);
                Matrix_RotateZ(angle.x * (std::numbers::pi_v<float> / 32768.0f), MTXMODE_APPLY);
            }

            Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            break;
    }

    return 0;
}

void EnDaikuKakariko_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static const Gfx* carpenterHeadDLists[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                          object_daiku_DL_005880 };
    static Vec3f unkVec = { 700.0f, 1100.0f, 0.0f };
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku_kakariko.c", 1104);

    if (limbIndex == 15) {
        Matrix_MultVec3f(&unkVec, &thisv->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, carpenterHeadDLists[thisv->actor.params & 3]);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku_kakariko.c", 1113);
}

void EnDaikuKakariko_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDaikuKakariko* thisv = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku_kakariko.c", 1124);

    func_80093D18(globalCtx->state.gfxCtx);

    if ((thisx->params & 3) == CARPENTER_ICHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if ((thisx->params & 3) == CARPENTER_SABOORO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if ((thisx->params & 3) == CARPENTER_JIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if ((thisx->params & 3) == CARPENTER_SHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnDaikuKakariko_OverrideLimbDraw, EnDaikuKakariko_PostLimbDraw, thisx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_daiku_kakariko.c", 1151);
}
