#include "z_en_in.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "objects/object_in/object_in.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnIn_Init(Actor* thisx, GlobalContext* globalCtx);
void EnIn_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnIn_Update(Actor* thisx, GlobalContext* globalCtx);
void EnIn_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnIn_Reset(void);

void func_80A79FB0(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A304(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A4C8(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A568(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A848(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7ABD4(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7AEF0(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7B018(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7B024(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7AE84(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A770(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A940(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7AA40(EnIn* thisv, GlobalContext* globalCtx);
void func_80A7A4BC(EnIn* thisv, GlobalContext* globalCtx);

ActorInit En_In_InitVars = {
    ACTOR_EN_IN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_IN,
    sizeof(EnIn),
    (ActorFunc)EnIn_Init,
    (ActorFunc)EnIn_Destroy,
    (ActorFunc)EnIn_Update,
    (ActorFunc)EnIn_Draw,
    (ActorResetFunc)EnIn_Reset,
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum {
    /* 0 */ ENIN_ANIM_0,
    /* 1 */ ENIN_ANIM_1,
    /* 2 */ ENIN_ANIM_2,
    /* 3 */ ENIN_ANIM_3,
    /* 4 */ ENIN_ANIM_4,
    /* 5 */ ENIN_ANIM_5,
    /* 6 */ ENIN_ANIM_6,
    /* 7 */ ENIN_ANIM_7,
    /* 8 */ ENIN_ANIM_8,
    /* 9 */ ENIN_ANIM_9
} EnInAnimation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &object_in_Anim_001CC0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_001CC0, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_013C6C, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_013C6C, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_000CB0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_0003B4, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &object_in_Anim_001BE0, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_013D60, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &object_in_Anim_01431C, 1.0f, ANIMMODE_LOOP, 0.0f }, { &object_in_Anim_014CA8, 1.0f, ANIMMODE_LOOP, 0.0f },
};

static const AnimationHeader* D_80A7B918[] = {
    &object_in_Anim_0151C8, &object_in_Anim_015DF0, &object_in_Anim_016B3C, &object_in_Anim_015814,
    &object_in_Anim_01646C, &object_in_Anim_0175D0, &object_in_Anim_017B58, &object_in_Anim_018C38,
};

static const Gfx* sAdultEraDLs[] = {
    NULL,
    NULL,
    gIngoAdultEraLeftThighDL,
    gIngoAdultEraLeftLegDL,
    gIngoAdultEraLeftFootDL,
    gIngoAdultEraRightThighDL,
    gIngoAdultEraRightLegDL,
    gIngoAdultEraRightFootDL,
    gIngoAdultEraTorsoDL,
    gIngoAdultEraChestDL,
    gIngoAdultEraLeftShoulderDL,
    gIngoAdultEraLeftArmDL,
    gIngoAdultEraLeftHandDL,
    gIngoAdultEraRightShoulderDL,
    gIngoAdultEraRightArmDL,
    gIngoAdultEraRightHandDL,
    gIngoAdultEraHeadDL,
    gIngoAdultEraLeftEyebrowDL,
    gIngoAdultEraRightEyebrowDL,
    gIngoAdultEraMustacheDL,
};

u16 func_80A78FB0(GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[1] & 0x10) {
        if (gSaveContext.infTable[9] & 0x80) {
            return 0x2046;
        } else {
            return 0x2045;
        }
    }
    if (gSaveContext.infTable[9] & 0x10) {
        return 0x2040;
    } else {
        return 0x203F;
    }
}

u16 func_80A79010(GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u16 temp_v0 = Text_GetFaceReaction(globalCtx, 25);

    if (temp_v0 != 0) {
        return temp_v0;
    }
    if (gSaveContext.eventChkInf[1] & 0x100) {
        if (IS_DAY) {
            return 0x205F;
        } else {
            return 0x2057;
        }
    }
    if (IS_NIGHT) {
        return 0x204E;
    }
    switch (gSaveContext.eventInf[0] & 0xF) {
        case 1:
            if (!(player->stateFlags1 & 0x800000)) {
                return 0x2036;
            } else if (gSaveContext.eventChkInf[1] & 0x800) {
                if (gSaveContext.infTable[10] & 4) {
                    return 0x2036;
                } else {
                    return 0x2038;
                }
            } else {
                return 0x2037;
            }
        case 3:
            if ((gSaveContext.eventInf[0] & 0x40) || (gSaveContext.eventInf[0] & 0x20)) {
                return 0x203E;
            } else {
                return 0x203D;
            }
        case 4:
            return 0x203A;
        case 5:
        case 6:
            return 0x203C;
        case 7:
            return 0x205B;
        case 2:
        default:
            if (gSaveContext.infTable[0x9] & 0x400) {
                return 0x2031;
            } else {
                return 0x2030;
            }
    }
}

u16 func_80A79168(GlobalContext* globalCtx, Actor* thisx) {
    u16 temp_v0 = Text_GetFaceReaction(globalCtx, 25);

    if (temp_v0 != 0) {
        return temp_v0;
    }
    if (!LINK_IS_ADULT) {
        return func_80A78FB0(globalCtx);
    } else {
        return func_80A79010(globalCtx);
    }
}

s16 func_80A791CC(GlobalContext* globalCtx, Actor* thisx) {
    s32 ret = 0;

    switch (thisx->textId) {
        case 0x2045:
            gSaveContext.infTable[9] |= 0x80;
            break;
        case 0x203E:
            ret = 2;
            break;
        case 0x203F:
            gSaveContext.eventChkInf[1] |= 2;
            gSaveContext.infTable[9] |= 0x10;
            break;
    }
    return ret;
}

s16 func_80A7924C(GlobalContext* globalCtx, Actor* thisx) {
    EnIn* thisv = (EnIn*)thisx;
    s32 sp18 = 1;

    switch (thisv->actor.textId) {
        case 0x2030:
        case 0x2031:
            if (globalCtx->msgCtx.choiceIndex == 1) {
                thisv->actor.textId = 0x2032;
            } else if (gSaveContext.rupees < 10) {
                thisv->actor.textId = 0x2033;
            } else {
                thisv->actor.textId = 0x2034;
            }
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            gSaveContext.infTable[9] |= 0x400;
            break;
        case 0x2034:
            if (globalCtx->msgCtx.choiceIndex == 1) {
                Rupees_ChangeBy(-10);
                thisv->actor.textId = 0x205C;
            } else {
                thisv->actor.textId = 0x2035;
            }
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            break;
        case 0x2036:
        case 0x2037:
            if (globalCtx->msgCtx.choiceIndex == 1) {
                sp18 = 2;
            } else {
                thisv->actor.textId = 0x201F;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            }
            break;
        case 0x2038:
            if (globalCtx->msgCtx.choiceIndex == 0 && gSaveContext.rupees >= 50) {
                sp18 = 2;
            } else {
                thisv->actor.textId = 0x2039;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                gSaveContext.infTable[10] |= 4;
            }
            break;
        case 0x205B:
            if (globalCtx->msgCtx.choiceIndex == 0 && gSaveContext.rupees >= 50) {
                sp18 = 2;
            } else {
                Message_ContinueTextbox(globalCtx, thisv->actor.textId = 0x2039);
                gSaveContext.eventInf[0] &= ~0xF;
                gSaveContext.eventInf[0] &= ~0x20;
                gSaveContext.eventInf[0] &= ~0x40;
                thisv->actionFunc = func_80A7A4C8;
            }
            break;
    }
    if (!gSaveContext.rupees) {}

    return sp18;
}

s16 func_80A7949C(GlobalContext* globalCtx, Actor* thisx) {
    s32 phi_v1 = 1;

    if (thisx->textId == 0x2035) {
        Rupees_ChangeBy(-10);
        thisx->textId = 0x205C;
        Message_ContinueTextbox(globalCtx, thisx->textId);
    } else {
        phi_v1 = 2;
    }
    return phi_v1;
}

s16 func_80A79500(GlobalContext* globalCtx, Actor* thisx) {
    s16 sp1E = 1;

    osSyncPrintf("message_check->(%d[%x])\n", Message_GetState(&globalCtx->msgCtx), thisx->textId);
    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            sp1E = func_80A791CC(globalCtx, thisx);
            break;
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                sp1E = func_80A7924C(globalCtx, thisx);
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                sp1E = func_80A7949C(globalCtx, thisx);
            }
            break;
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return sp1E;
}

void func_80A795C8(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a3;

    if (thisv->skelAnime.animation == &object_in_Anim_0003B4 || thisv->skelAnime.animation == &object_in_Anim_001BE0 ||
        thisv->skelAnime.animation == &object_in_Anim_013D60) {
        phi_a3 = 1;
    } else {
        phi_a3 = 0;
    }
    if (thisv->actionFunc == func_80A7A568) {
        phi_a3 = 4;
    }
    if (thisv->actionFunc == func_80A7B024) {
        thisv->unk_308.unk_18 = globalCtx->view.eye;
        thisv->unk_308.unk_14 = 60.0f;
    } else {
        thisv->unk_308.unk_18 = player->actor.world.pos;
        thisv->unk_308.unk_14 = 16.0f;
    }
    func_80034A14(&thisv->actor, &thisv->unk_308, 1, phi_a3);
}

void func_80A79690(SkelAnime* skelAnime, EnIn* thisv, GlobalContext* globalCtx) {
    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->moveFlags |= 3;
        AnimationContext_SetMoveActor(globalCtx, &thisv->actor, skelAnime, 1.0f);
    }
}

void EnIn_ChangeAnim(EnIn* thisv, s32 index) {
    Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode,
                     sAnimationInfo[index].morphFrames);
}

s32 func_80A7975C(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params != 1 || thisv->actor.shape.rot.z != 1 || !LINK_IS_ADULT) {
        return 0;
    }
    thisv->animationIdx = 1;
    thisv->collider.base.ocFlags1 &= ~OC1_ON;
    Animation_Change(&thisv->skelAnime, D_80A7B918[thisv->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(D_80A7B918[thisv->animationIdx]), 2, 0.0f);
    thisv->actionFunc = func_80A7A304;
    return 1;
}

s32 func_80A79830(EnIn* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT20 && LINK_IS_CHILD && IS_DAY && thisv->actor.shape.rot.z == 1 &&
        !(gSaveContext.eventChkInf[1] & 0x10)) {
        return 1;
    }
    if (globalCtx->sceneNum == SCENE_MALON_STABLE && LINK_IS_CHILD && IS_DAY && thisv->actor.shape.rot.z == 3 &&
        (gSaveContext.eventChkInf[1] & 0x10)) {
        return 1;
    }
    if (globalCtx->sceneNum == SCENE_MALON_STABLE && LINK_IS_CHILD && IS_NIGHT) {
        if ((thisv->actor.shape.rot.z == 2) && !(gSaveContext.eventChkInf[1] & 0x10)) {
            return 1;
        }
        if ((thisv->actor.shape.rot.z == 4) && (gSaveContext.eventChkInf[1] & 0x10)) {
            return 1;
        }
    }
    if (globalCtx->sceneNum == SCENE_SPOT20 && LINK_IS_ADULT && IS_DAY) {
        if ((thisv->actor.shape.rot.z == 5) && !(gSaveContext.eventChkInf[1] & 0x100)) {
            return 2;
        }
        if ((thisv->actor.shape.rot.z == 7) && (gSaveContext.eventChkInf[1] & 0x100)) {
            return 4;
        }
    }
    if (globalCtx->sceneNum == SCENE_SOUKO && LINK_IS_ADULT && IS_NIGHT) {
        if (thisv->actor.shape.rot.z == 6 && !(gSaveContext.eventChkInf[1] & 0x100)) {
            return 3;
        }
        if (thisv->actor.shape.rot.z == 8 && (gSaveContext.eventChkInf[1] & 0x100)) {
            return 3;
        }
    }
    return 0;
}

void EnIn_UpdateEyes(EnIn* thisv) {
    if (thisv->eyeIndex != 3) {
        if (DECR(thisv->blinkTimer) == 0) {
            thisv->eyeIndex++;
            if (thisv->eyeIndex >= 3) {
                thisv->blinkTimer = Rand_S16Offset(30, 30);
                thisv->eyeIndex = 0;
            }
        }
    }
}

void func_80A79AB4(EnIn* thisv, GlobalContext* globalCtx) {
    s32 i;
    u32 f = 0;

    if (thisv->skelAnime.animation != &object_in_Anim_014CA8) {
        f = globalCtx->gameplayFrames;
    }
    for (i = 0; i < ARRAY_COUNT(thisv->unk_330); i++) {
        thisv->unk_330[i].y = (2068 + 50 * i) * f;
        thisv->unk_330[i].z = (2368 + 50 * i) * f;
    }
}

void func_80A79BAC(EnIn* thisv, GlobalContext* globalCtx, s32 index, u32 arg3) {
    s16 entrances[] = { 0x0558, 0x04CA, 0x0157 };

    globalCtx->nextEntranceIndex = entrances[index];
    if (index == 2) {
        gSaveContext.nextCutsceneIndex = 0xFFF0;
    }
    globalCtx->fadeTransition = arg3;
    globalCtx->sceneLoadFlag = 0x14;
    func_8002DF54(globalCtx, &thisv->actor, 8);
    Interface_ChangeAlpha(1);
    if (index == 0) {
        AREG(6) = 0;
    }
    gSaveContext.timer1State = 0;
}

void func_80A79C78(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f sp48;
    Vec3f sp3C;
    Vec3s zeroVec = { 0, 0, 0 };

    thisv->camId = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->camId, CAM_STAT_ACTIVE);
    sp48.x = thisv->actor.world.pos.x;
    sp48.y = thisv->actor.world.pos.y + 60.0f;
    sp48.z = thisv->actor.world.pos.z;
    sp3C.x = sp48.x;
    sp3C.y = sp48.y - 22.0f;
    sp3C.z = sp48.z + 40.0f;
    Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &sp48, &sp3C);
    thisv->actor.shape.rot.y = Math_Vec3f_Yaw(&thisv->actor.world.pos, &sp3C);
    thisv->unk_308.unk_08 = zeroVec;
    thisv->unk_308.unk_0E = zeroVec;
    Message_StartTextbox(globalCtx, 0x2025, NULL);
    thisv->unk_308.unk_00 = 1;
    player->actor.world.pos = thisv->actor.world.pos;
    player->actor.world.pos.x += 100.0f * Math_SinS(thisv->actor.shape.rot.y);
    player->actor.world.pos.z += 100.0f * Math_CosS(thisv->actor.shape.rot.y);
    if (player->rideActor != NULL) {
        player->rideActor->world.pos = player->actor.world.pos;
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    ShrinkWindow_SetVal(0x20);
    Interface_ChangeAlpha(2);
}

static s32 D_80A7B998 = 0;

void EnIn_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnIn* thisv = (EnIn*)thisx;
    RespawnData* respawn = &gSaveContext.respawn[RESPAWN_MODE_DOWN];
    Vec3f respawnPos;

    thisv->ingoObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_IN);
    if (thisv->ingoObjBankIndex < 0 && thisv->actor.params > 0) {
        thisv->actionFunc = NULL;
        Actor_Kill(&thisv->actor);
        return;
    }
    respawnPos = respawn->pos;
    // hardcoded coords for lon lon entrance
    if (D_80A7B998 == 0 && respawnPos.x == 1107.0f && respawnPos.y == 0.0f && respawnPos.z == -3740.0f) {
        gSaveContext.eventInf[0] = 0;
        D_80A7B998 = 1;
    }
    thisv->actionFunc = func_80A79FB0;
}

void EnIn_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnIn* thisv = (EnIn*)thisx;

    if (thisv->actionFunc != NULL && thisv->actionFunc != func_80A79FB0) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void func_80A79FB0(EnIn* thisv, GlobalContext* globalCtx) {
    s32 sp3C = 0;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->ingoObjBankIndex) || thisv->actor.params <= 0) {
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gIngoSkel, NULL, thisv->jointTable, thisv->morphTable, 20);
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
        if (func_80A7975C(thisv, globalCtx)) {
            gSaveContext.eventInf[0] &= ~0x8000;
            return;
        }
        Actor_SetScale(&thisv->actor, 0.01f);
        thisv->actor.targetMode = 6;
        thisv->unk_308.unk_00 = 0;
        thisv->actionFunc = func_80A7A4BC;

        switch (func_80A79830(thisv, globalCtx)) {
            case 1:
                EnIn_ChangeAnim(thisv, ENIN_ANIM_9);
                thisv->actionFunc = func_80A7A4BC;
                break;
            case 3:
                EnIn_ChangeAnim(thisv, ENIN_ANIM_7);
                thisv->actionFunc = func_80A7A4BC;
                if (!(gSaveContext.eventChkInf[1] & 0x100)) {
                    thisv->actor.params = 5;
                }
                break;
            case 4:
                EnIn_ChangeAnim(thisv, ENIN_ANIM_8);
                thisv->eyeIndex = 3;
                thisv->actionFunc = func_80A7A4BC;
                break;
            case 0:
                Actor_Kill(&thisv->actor);
                break;
            default:
                switch (gSaveContext.eventInf[0] & 0xF) {
                    case 0:
                    case 2:
                    case 3:
                    case 4:
                    case 7:
                        if (thisv->actor.params == 2) {
                            sp3C = 1;
                        }
                        break;
                    case 1:
                        if (thisv->actor.params == 3) {
                            sp3C = 1;
                        }
                        break;
                    case 5:
                    case 6:
                        if (thisv->actor.params == 4) {
                            sp3C = 1;
                        }
                        break;
                }
                if (sp3C != 1) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
                switch (gSaveContext.eventInf[0] & 0xF) {
                    case 0:
                    case 2:
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_2);
                        thisv->actionFunc = func_80A7A4C8;
                        gSaveContext.eventInf[0] = 0;
                        break;
                    case 1:
                        thisv->actor.targetMode = 3;
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_2);
                        thisv->actionFunc = func_80A7A568;
                        func_80088B34(0x3C);
                        break;
                    case 3:
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_4);
                        thisv->actionFunc = func_80A7A770;
                        break;
                    case 4:
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_6);
                        thisv->unk_1EC = 8;
                        thisv->actionFunc = func_80A7A940;
                        break;
                    case 5:
                    case 6:
                        thisv->actor.targetMode = 3;
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_6);
                        thisv->unk_1EC = 8;
                        thisv->actionFunc = func_80A7AA40;
                        break;
                    case 7:
                        EnIn_ChangeAnim(thisv, ENIN_ANIM_2);
                        thisv->actionFunc = func_80A7A848;
                        break;
                }
        }
    }
}

void func_80A7A304(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.animation == &object_in_Anim_015814 || thisv->skelAnime.animation == &object_in_Anim_01646C) {
        if (thisv->skelAnime.curFrame == 8.0f) {
            Audio_PlaySoundRandom(&thisv->actor.projectedPos, NA_SE_VO_IN_LASH_0,
                                  NA_SE_VO_IN_LASH_1 - NA_SE_VO_IN_LASH_0 + 1);
        }
    }
    if (thisv->skelAnime.animation == &object_in_Anim_018C38 && thisv->skelAnime.curFrame == 20.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_IN_CRY_0);
    }
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        thisv->animationIdx %= 8;
        thisv->unk_1E8 = thisv->animationIdx;
        if (thisv->animationIdx == 3 || thisv->animationIdx == 4) {
            Audio_PlaySoundGeneral(NA_SE_IT_LASH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (Rand_ZeroOne() < 0.3f) {
                Audio_PlaySoundGeneral(NA_SE_IT_INGO_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
        }
        Animation_Change(&thisv->skelAnime, D_80A7B918[thisv->animationIdx], 1.0f, 0.0f,
                         Animation_GetLastFrame(D_80A7B918[thisv->animationIdx]), 2, -10.0f);
    }
}

void func_80A7A4BC(EnIn* thisv, GlobalContext* globalCtx) {
}

void func_80A7A4C8(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_308.unk_00 == 2) {
        func_80A79BAC(thisv, globalCtx, 1, 0x20);
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x000F) | 0x0001;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
        gSaveContext.infTable[10] &= ~4;
        Environment_ForcePlaySequence(NA_BGM_HORSE);
        globalCtx->msgCtx.stateTimer = 0;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        thisv->unk_308.unk_00 = 0;
    }
}

void func_80A7A568(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 phi_a2;
    s32 phi_a3;

    if (!(gSaveContext.eventChkInf[1] & 0x800) && (player->stateFlags1 & 0x800000)) {
        gSaveContext.infTable[10] |= 0x800;
    }
    if (gSaveContext.timer1State == 10) {
        Audio_PlaySoundGeneral(NA_SE_SY_FOUND, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        func_80A79C78(thisv, globalCtx);
        thisv->actionFunc = func_80A7B024;
        gSaveContext.timer1State = 0;
    } else if (thisv->unk_308.unk_00 == 2) {
        if (globalCtx->msgCtx.choiceIndex == 0) {
            if (gSaveContext.rupees < 50) {
                globalCtx->msgCtx.stateTimer = 4;
                globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                thisv->unk_308.unk_00 = 0;
                return;
            }
            gSaveContext.eventInf[0] =
                (gSaveContext.eventInf[0] & ~0x10) | (((EnHorse*)GET_PLAYER(globalCtx)->rideActor)->type << 4);
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | 2;
            phi_a2 = 2;
            phi_a3 = 2;
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FOUND, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (!(gSaveContext.eventChkInf[1] & 0x800)) {
                if (gSaveContext.infTable[10] & 0x800) {
                    gSaveContext.eventChkInf[1] |= 0x800;
                    gSaveContext.infTable[10] |= 0x800;
                }
            }
            gSaveContext.eventInf[0] &= ~0xF;
            phi_a2 = 0;
            phi_a3 = 0x20;
        }
        func_80A79BAC(thisv, globalCtx, phi_a2, phi_a3);
        globalCtx->msgCtx.stateTimer = 0;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        thisv->unk_308.unk_00 = 0;
    }
}

void func_80A7A770(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_308.unk_00 == 0) {
        thisv->actor.flags |= ACTOR_FLAG_16;
    } else if (thisv->unk_308.unk_00 == 2) {
        Rupees_ChangeBy(-50);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        EnIn_ChangeAnim(thisv, ENIN_ANIM_3);
        thisv->actionFunc = func_80A7A848;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x0F) | 7;
        thisv->unk_308.unk_00 = 0;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & 0xFFFF) | 0x20;
        if (!(gSaveContext.eventInf[0] & 0x40)) {
            globalCtx->msgCtx.stateTimer = 4;
            globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
    }
}

void func_80A7A848(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_308.unk_00 == 2) {
        if ((globalCtx->msgCtx.choiceIndex == 0 && gSaveContext.rupees < 50) || globalCtx->msgCtx.choiceIndex == 1) {
            gSaveContext.eventInf[0] &= ~0xF;
            thisv->actionFunc = func_80A7A4C8;
        } else {
            func_80A79BAC(thisv, globalCtx, 2, 0x26);
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | 2;
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
            globalCtx->msgCtx.stateTimer = 0;
            globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
        thisv->unk_308.unk_00 = 0;
        gSaveContext.eventInf[0] &= ~0x20;
        gSaveContext.eventInf[0] &= ~0x40;
    }
}

void func_80A7A940(EnIn* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_308.unk_00 == 0) {
        thisv->actor.flags |= ACTOR_FLAG_16;
        return;
    }
    if (thisv->unk_1EC != 0) {
        thisv->unk_1EC--;
        if (thisv->unk_1EC == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_IN_LOST);
        }
    }
    if (thisv->unk_308.unk_00 == 2) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        func_80A79BAC(thisv, globalCtx, 2, 0x26);
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x000F) | 0x0002;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
        globalCtx->msgCtx.stateTimer = 0;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        thisv->unk_308.unk_00 = 0;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & 0xFFFF) | 0x40;
    }
}

void func_80A7AA40(EnIn* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f sp30;
    Vec3f sp24;

    thisv->camId = Gameplay_CreateSubCamera(globalCtx);
    thisv->activeCamId = globalCtx->activeCamera;
    Gameplay_ChangeCameraStatus(globalCtx, thisv->activeCamId, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->camId, CAM_STAT_ACTIVE);

    thisv->unk_2F0 = 0.0f;
    thisv->unk_2F4 = 50.0f;
    thisv->unk_2F8 = 0.0f;
    thisv->unk_2FC = 0.0f;
    thisv->unk_300 = 50.0f;
    thisv->unk_304 = 50.0f;

    sp30 = thisv->actor.world.pos;
    sp24 = thisv->actor.world.pos;

    sp30.x += thisv->unk_2F0;
    sp30.y += thisv->unk_2F4;
    sp30.z += thisv->unk_2F8;

    sp24.x += thisv->unk_2FC;
    sp24.y += thisv->unk_300;
    sp24.z += thisv->unk_304;

    Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &sp30, &sp24);
    thisv->actor.textId = 0x203B;
    Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
    thisv->unk_308.unk_00 = 1;
    thisv->unk_1FC = 0;
    globalCtx->csCtx.frames = 0;
    ShrinkWindow_SetVal(0x20);
    Interface_ChangeAlpha(2);
    thisv->actionFunc = func_80A7ABD4;
}

void func_80A7ABD4(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f sp48;
    Vec3f sp3C;

    if (player->rideActor != NULL) {
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;
    if (thisv->actor.textId == 0x203B) {
        if (thisv->unk_1EC != 0) {
            thisv->unk_1EC--;
            if (thisv->unk_1EC == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_IN_LOST);
            }
        }
    }
    if (thisv->unk_308.unk_00 != 0) {
        if (thisv->unk_308.unk_00 == 2) {
            if (thisv->actor.textId == 0x203B) {
                thisv->actor.textId = 0x203C;
                Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
                thisv->unk_308.unk_00 = 1;
                EnIn_ChangeAnim(thisv, ENIN_ANIM_3);
            } else {
                globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                thisv->unk_308.unk_00 = 0;
            }
        }
    } else {
        if (globalCtx->csCtx.frames++ >= 50) {
            thisv->actionFunc = func_80A7AE84;
            return;
        }
        if (globalCtx->csCtx.frames == 44) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_RONRON_DOOR_CLOSE);
        }
        Math_SmoothStepToF(&thisv->unk_2F0, 0.0f, 0.06f, 10000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->unk_2F4, 50.0f, 0.06f, 10000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->unk_2F8, 0.0f, 0.06f, 10000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->unk_2FC, 0.0f, 0.06f, 10000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->unk_300, 150.0f, 0.06f, 10000.0f, 0.0f);
        Math_SmoothStepToF(&thisv->unk_304, 300.0f, 0.06f, 10000.0f, 0.0f);

        sp48 = thisv->actor.world.pos;
        sp3C = thisv->actor.world.pos;

        sp48.x += thisv->unk_2F0;
        sp48.y += thisv->unk_2F4;
        sp48.z += thisv->unk_2F8;
        sp3C.x += thisv->unk_2FC;
        sp3C.y += thisv->unk_300;
        sp3C.z += thisv->unk_304;
        Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &sp48, &sp3C);
    }
}

void func_80A7AE84(EnIn* thisv, GlobalContext* globalCtx) {
    Gameplay_ChangeCameraStatus(globalCtx, thisv->activeCamId, CAM_STAT_ACTIVE);
    Gameplay_ClearCamera(globalCtx, thisv->camId);
    func_8002DF54(globalCtx, &thisv->actor, 7);
    Interface_ChangeAlpha(0x32);
    thisv->actionFunc = func_80A7AEF0;
}

void func_80A7AEF0(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 yaw;
    Vec3f pos = thisv->actor.world.pos;

    pos.x += 90.0f * Math_SinS(thisv->actor.shape.rot.y);
    pos.z += 90.0f * Math_CosS(thisv->actor.shape.rot.y);
    yaw = Math_Vec3f_Yaw(&pos, &player->actor.world.pos);
    if (ABS(yaw) > 0x4000) {
        globalCtx->nextEntranceIndex = 0x0476;
        globalCtx->sceneLoadFlag = 0x14;
        globalCtx->fadeTransition = 5;
        thisv->actionFunc = func_80A7B018;
    } else if (thisv->unk_308.unk_00 == 2) {
        globalCtx->msgCtx.stateTimer = 4;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        thisv->unk_308.unk_00 = 0;
    }
}

void func_80A7B018(EnIn* thisv, GlobalContext* globalCtx) {
}

void func_80A7B024(EnIn* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->rideActor != NULL) {
        player->rideActor->freezeTimer = 10;
    }
    player->actor.freezeTimer = 10;
    if (thisv->unk_308.unk_00 == 2) {
        if (1) {}
        if (!(gSaveContext.eventChkInf[1] & 0x800) && (gSaveContext.infTable[10] & 0x800)) {
            gSaveContext.eventChkInf[1] |= 0x800;
            gSaveContext.infTable[10] |= 0x800;
        }
        func_80A79BAC(thisv, globalCtx, 0, 0x26);
        gSaveContext.eventInf[0] = gSaveContext.eventInf[0] & ~0xF;
        gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0x8000) | 0x8000;
        globalCtx->msgCtx.stateTimer = 4;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        thisv->unk_308.unk_00 = 0;
    }
}

void EnIn_Update(Actor* thisx, GlobalContext* globalCtx) {
    ColliderCylinder* collider;
    EnIn* thisv = (EnIn*)thisx;

    if (thisv->actionFunc == func_80A79FB0) {
        thisv->actionFunc(thisv, globalCtx);
        return;
    }
    collider = &thisv->collider;
    Collider_UpdateCylinder(&thisv->actor, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &collider->base);
    if (thisv->actionFunc != func_80A7A304) {
        SkelAnime_Update(&thisv->skelAnime);
        if (thisv->skelAnime.animation == &object_in_Anim_001BE0 && ((gSaveContext.eventInf[0] & 0xF) != 6)) {
            func_80A79690(&thisv->skelAnime, thisv, globalCtx);
        }
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    }
    EnIn_UpdateEyes(thisv);
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc != func_80A7A304) {
        func_80A79AB4(thisv, globalCtx);
        if (gSaveContext.timer2Value < 6 && gSaveContext.timer2State != 0 && thisv->unk_308.unk_00 == 0) {
            if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {}
        } else {
            func_800343CC(globalCtx, &thisv->actor, &thisv->unk_308.unk_00,
                          ((thisv->actor.targetMode == 6) ? 80.0f : 320.0f) + thisv->collider.dim.radius, func_80A79168,
                          func_80A79500);
            if (thisv->unk_308.unk_00 != 0) {
                thisv->unk_1FA = thisv->unk_1F8;
                thisv->unk_1F8 = Message_GetState(&globalCtx->msgCtx);
            }
        }
        func_80A795C8(thisv, globalCtx);
    }
}

s32 EnIn_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnIn* thisv = (EnIn*)thisx;
    Vec3s sp2C;

    if (thisv->actor.params > 0 && limbIndex != INGO_HEAD_LIMB) {
        if (sAdultEraDLs[limbIndex] != NULL) {
            *dList = sAdultEraDLs[limbIndex];
        }
    }
    if (limbIndex == INGO_HEAD_LIMB) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp2C = thisv->unk_308.unk_08;
        Matrix_RotateZ(BINANG_TO_RAD(sp2C.x), MTXMODE_APPLY);
        Matrix_RotateX(BINANG_TO_RAD(sp2C.y), MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == INGO_CHEST_LIMB) {
        sp2C = thisv->unk_308.unk_0E;
        Matrix_RotateX(BINANG_TO_RAD(sp2C.x), MTXMODE_APPLY);
        Matrix_RotateY(BINANG_TO_RAD(sp2C.y), MTXMODE_APPLY);
    }
    if (limbIndex == INGO_CHEST_LIMB || limbIndex == INGO_LEFT_SHOULDER_LIMB || limbIndex == INGO_RIGHT_SHOULDER_LIMB) {
        rot->y += Math_SinS(thisv->unk_330[limbIndex].y) * 200.0f;
        rot->z += Math_CosS(thisv->unk_330[limbIndex].z) * 200.0f;
    }
    return 0;
}

void EnIn_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnIn* thisv = (EnIn*)thisx;
    Vec3f D_80A7B9A8 = { 1600.0, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_in.c", 2335);

    if (limbIndex == INGO_HEAD_LIMB) {
        Matrix_MultVec3f(&D_80A7B9A8, &thisv->actor.focus.pos);
        thisv->actor.focus.rot = thisv->actor.world.rot;
    }
    if (limbIndex == INGO_LEFT_HAND_LIMB && thisv->skelAnime.animation == &object_in_Anim_014CA8) {
        gSPDisplayList(POLY_OPA_DISP++, gIngoChildEraBasketDL);
    }
    if (limbIndex == INGO_RIGHT_HAND_LIMB && thisv->skelAnime.animation == &object_in_Anim_014CA8) {
        gSPDisplayList(POLY_OPA_DISP++, gIngoChildEraPitchForkDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_in.c", 2365);
}

void EnIn_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = { gIngoEyeOpenTex, gIngoEyeHalfTex, gIngoEyeClosedTex, gIngoEyeClosed2Tex };

    EnIn* thisv = (EnIn*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_in.c", 2384);
    if (thisv->actionFunc != func_80A79FB0) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gIngoHeadGradient2Tex));
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnIn_OverrideLimbDraw, EnIn_PostLimbDraw, &thisv->actor);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_in.c", 2416);
}

void EnIn_Reset(void) {
    D_80A7B998 = 0;
}