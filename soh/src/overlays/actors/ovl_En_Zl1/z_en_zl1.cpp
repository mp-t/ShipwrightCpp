/*
 * File: z_en_zl1.c
 * Overlay: ovl_En_Zl1
 * Description: Child Princess Zelda (at window)
 */

#include "z_en_zl1.h"
#include "objects/object_zl1/object_zl1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnZl1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnZl1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnZl1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnZl1_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B4AE18(EnZl1* thisv);
void func_80B4AF18(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4B010(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4B240(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4B8B4(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4BBC4(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4BC78(EnZl1* thisv, GlobalContext* globalCtx);
void func_80B4BF2C(EnZl1* thisv, GlobalContext* globalCtx);

extern CutsceneData D_80B4C5D0[];

#include "z_en_zl1_camera_data.c"

const ActorInit En_Zl1_InitVars = {
    ACTOR_EN_ZL1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZL1,
    sizeof(EnZl1),
    (ActorFunc)EnZl1_Init,
    (ActorFunc)EnZl1_Destroy,
    (ActorFunc)EnZl1_Update,
    (ActorFunc)EnZl1_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 46, 0, { 0, 0, 0 } },
};

static void* D_80B4E61C[] = {
    gChildZelda1EyeOpenLookingUpRightTex,
    gChildZelda1EyeHalf2Tex,
    gChildZelda1EyeClosedTex,
    gChildZelda1EyeHalf2Tex,
};
static void* D_80B4E62C[] = { gChildZelda1MouthNeutralTex };

void func_80B4AB40(void) {
}

void func_80B4AB48(void) {
}

void EnZl1_Init(Actor* thisx, GlobalContext* globalCtx) {
    f32 frameCount;
    EnZl1* thisv = (EnZl1*)thisx;

    frameCount = Animation_GetLastFrame(&gChildZelda1Anim_12118);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gChildZelda1Skel, NULL, NULL, NULL, 0);
    Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_12118, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Actor_SetScale(&thisv->actor, 0.01f);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    thisv->actor.targetMode = 0;

    if (gSaveContext.sceneSetupIndex >= 4) {
        frameCount = Animation_GetLastFrame(&gChildZelda1Anim_00438);
        Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_00438, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
        thisv->unk_1E6 = 0;
        thisv->actionFunc = func_80B4BC78;
    } else if (Flags_GetEventChkInf(9) && Flags_GetEventChkInf(0x25) && Flags_GetEventChkInf(0x37)) {
        Actor_Kill(&thisv->actor);
    } else if ((Flags_GetEventChkInf(9) && Flags_GetEventChkInf(0x25)) ||
               (Flags_GetEventChkInf(9) && Flags_GetEventChkInf(0x37))) {
        frameCount = Animation_GetLastFrame(&gChildZelda1Anim_00438);
        Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_00438, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
        thisv->actor.textId = 0x703D;
        thisv->actionFunc = func_80B4AF18;
    } else if (Flags_GetEventChkInf(0x40)) {
        frameCount = Animation_GetLastFrame(&gChildZelda1Anim_00438);
        Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_00438, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
        thisv->actor.textId = 0x703C;
        thisv->actionFunc = func_80B4AF18;
    } else {
        thisv->actor.textId = 0xFFFF;
        thisv->actionFunc = func_80B4B010;
    }
}

void EnZl1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnZl1* thisv = (EnZl1*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B4AE18(EnZl1* thisv) {
    if ((thisv->skelAnime.animation == &gChildZelda1Anim_10B38) && (thisv->skelAnime.curFrame < 26.0f)) {
        thisv->unk_1F4 = gChildZelda1EyeOpenLookingRightTex;
        thisv->unk_1F8 = gChildZelda1EyeOpenLookingLeftTex;
        thisv->unk_1FC = 2;
    } else {
        if (DECR(thisv->unk_1FC) == 0) {
            thisv->unk_1FC = Rand_S16Offset(0x1E, 0xA);
        }
        thisv->unk_1FE = (thisv->unk_1FC < 4) ? thisv->unk_1FC : 0;

        thisv->unk_1F4 = D_80B4E61C[thisv->unk_1FE];
        thisv->unk_1F8 = D_80B4E61C[thisv->unk_1FE];
        thisv->unk_1EC = D_80B4E62C[thisv->unk_1F2];
    }
}

void func_80B4AF18(EnZl1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    func_80038290(globalCtx, &thisv->actor, &thisv->unk_200, &thisv->unk_206, thisv->actor.focus.pos);

    if (thisv->unk_1E6 != 0) {
        if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
            thisv->unk_1E6 = 0;
        }
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->unk_1E6 = 1;
    } else if (thisv->actor.world.pos.y <= player->actor.world.pos.y) {
        func_8002F2F4(&thisv->actor, globalCtx);
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void func_80B4B010(EnZl1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad2;
    s32 pad3;
    s32 pad;
    Vec3f vec1 = { -460.0f, 118.0f, 0.0f };
    Vec3f vec2 = { -406.0f, 110.0f, 0.0f };
    Vec3f playerPos = { -398.0f, 84.0f, 0.0f };
    s16 rotDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_10B38, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gChildZelda1Anim_10B38), ANIMMODE_ONCE_INTERP, -10.0f);
        thisv->unk_1E8 = Gameplay_CreateSubCamera(globalCtx);
        Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
        Gameplay_ChangeCameraStatus(globalCtx, thisv->unk_1E8, CAM_STAT_ACTIVE);
        func_800C0808(globalCtx, thisv->unk_1E8, player, CAM_SET_FREE0);
        globalCtx->envCtx.screenFillColor[0] = 255;
        globalCtx->envCtx.screenFillColor[1] = 255;
        globalCtx->envCtx.screenFillColor[2] = 255;
        globalCtx->envCtx.screenFillColor[3] = 24;
        globalCtx->envCtx.fillScreen = true;
        Gameplay_CameraSetAtEye(globalCtx, thisv->unk_1E8, &vec1, &vec2);
        Gameplay_CameraSetFov(globalCtx, thisv->unk_1E8, 30.0f);
        ShrinkWindow_SetVal(0x20);
        Interface_ChangeAlpha(2);
        player->actor.world.pos = playerPos;
        player->actor.speedXZ = 0.0f;
        thisv->unk_1E2 = 0;
        thisv->actionFunc = func_80B4B240;
        Audio_PlayFanfare(NA_BGM_APPEAR);
    } else {
        if (1) {} // necessary to match
        rotDiff = ABS(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y);
        if ((rotDiff < 0x238E) && !(player->actor.world.pos.y < thisv->actor.world.pos.y)) {
            func_8002F2F4(&thisv->actor, globalCtx);
        }
    }
}

void func_80B4B240(EnZl1* thisv, GlobalContext* globalCtx) {
    Vec3f sp74 = { -427.0f, 108.0, 26.0 };
    Vec3f sp68 = { -340.0f, 108.0f, 98.0f };
    s32 pad;
    Vec3f sp58 = { -434.0f, 84.0f, 0.0f };
    u8 sp54[] = { 0x00, 0x00, 0x02 };
    s32 pad2;
    Player* player = GET_PLAYER(globalCtx);
    AnimationHeader* animHeaderSeg;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    f32 frameCount;
    s32 sp3C = 0;

    switch (thisv->unk_1E2) {
        case 0:
            switch ((s16)thisv->skelAnime.curFrame) {
                case 14:
                    thisv->unk_1E4 = 0;
                    break;
                case 15:
                    if (DECR(thisv->unk_1E4) != 0) {
                        thisv->skelAnime.curFrame = 15.0f;
                    }
                    break;
                case 64:
                    animHeaderSeg = &gChildZelda1Anim_11348;
                    sp3C = 1;
                    thisv->actor.textId = 0x702E;
                    Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
                    thisv->unk_1E2++;
                    break;
            }
            break;
        case 1:
            if ((Message_GetState(msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                globalCtx->envCtx.fillScreen = false;
                Gameplay_CameraSetAtEye(globalCtx, thisv->unk_1E8, &sp74, &sp68);
                Gameplay_CameraSetFov(globalCtx, thisv->unk_1E8, 25.0f);
                player->actor.world.pos = sp58;
                thisv->actor.textId = 0x702F;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->unk_1E2++;
            }
            break;
        case 2:
            if ((Message_GetState(msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                if (msgCtx->choiceIndex == 0) {
                    animHeaderSeg = &gChildZelda1Anim_13F10;
                    sp3C = 2;
                    thisv->unk_1E2++;
                } else {
                    animHeaderSeg = &gChildZelda1Anim_116E4;
                    sp3C = 2;
                    thisv->unk_1E2 = 6;
                }
            }
            break;
        case 3:
            frameCount = Animation_GetLastFrame(&gChildZelda1Anim_13F10);
            if (thisv->skelAnime.curFrame == frameCount) {
                animHeaderSeg = &gChildZelda1Anim_143A8;
                sp3C = 1;
                thisv->actor.textId = 0x7032;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->unk_1E2++;
            }
            break;
        case 4:
            if ((Message_GetState(msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                if (msgCtx->choiceIndex == 0) {
                    animHeaderSeg = &gChildZelda1Anim_132D8;
                    sp3C = 2;
                    thisv->unk_1E2 = 9;
                } else {
                    thisv->actor.textId = 0x7034;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->unk_1E2++;
                }
            }
            break;
        case 5:
            if ((Message_GetState(msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                thisv->actor.textId = 0x7033;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->unk_1E2--;
            }
            break;
        case 6:
            frameCount = Animation_GetLastFrame(&gChildZelda1Anim_116E4);
            if (thisv->skelAnime.curFrame == frameCount) {
                animHeaderSeg = &gChildZelda1Anim_12B88;
                sp3C = 1;
                thisv->actor.textId = 0x7031;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->unk_1E2++;
            }
            break;
        case 7:
            if ((Message_GetState(msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                thisv->actor.textId = 0x7030;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->unk_1E2++;
            }
            break;
        case 8:
            if ((Message_GetState(msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                if (msgCtx->choiceIndex == 0) {
                    animHeaderSeg = &gChildZelda1Anim_138E0;
                    sp3C = 2;
                    thisv->unk_1E2 = 3;
                } else {
                    thisv->actor.textId = 0x7031;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->unk_1E2--;
                }
            }
            break;
        case 9:
            frameCount = Animation_GetLastFrame(&gChildZelda1Anim_132D8);
            if (thisv->skelAnime.curFrame == frameCount) {
                animHeaderSeg = &gChildZelda1Anim_00438;
                sp3C = 1;
                globalCtx->csCtx.segment = D_80B4C5D0;
                gSaveContext.cutsceneTrigger = 1;
                thisv->actionFunc = func_80B4B8B4;
                thisv->unk_1E2++;
            }
            break;
    }
    if (sp3C != 0) {
        frameCount = Animation_GetLastFrame(animHeaderSeg);
        Animation_Change(&thisv->skelAnime, animHeaderSeg, 1.0f, 0.0f, frameCount, sp54[sp3C], -10.0f);
    }
    func_80038290(globalCtx, &thisv->actor, &thisv->unk_200, &thisv->unk_206, thisv->actor.focus.pos);
}

void func_80B4B7F4(CsCmdActorAction* npcAction, Vec3f* pos) {
    pos->x = npcAction->startPos.x;
    pos->y = npcAction->startPos.y;
    pos->z = npcAction->startPos.z;
}

void func_80B4B834(CsCmdActorAction* npcAction, Vec3f* pos) {
    pos->x = npcAction->endPos.x;
    pos->y = npcAction->endPos.y;
    pos->z = npcAction->endPos.z;
}

void func_80B4B874(EnZl1* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80B4B8B4(EnZl1* thisv, GlobalContext* globalCtx) {
    AnimationHeader* spB0[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &gChildZelda1Anim_12B04,
        &gChildZelda1Anim_12118,
        &gChildZelda1Anim_10B38,
    };
    u8 spA4[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02,
    };
    Vec3f sp98 = { -421.0f, 143.0f, -5.0f };
    Vec3f sp8C = { -512.0f, 105.0f, -4.0f };
    s32 pad2;
    f32 actionLength;
    CsCmdActorAction* npcAction;
    Vec3f sp74;
    Vec3f sp68;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    f32 frameCount;
    Vec3f sp48;

    SkelAnime_Update(&thisv->skelAnime);
    func_80B4B874(thisv, globalCtx);
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        thisv->actionFunc = func_80B4BBC4;
        return;
    }

    npcAction = globalCtx->csCtx.npcActions[0];
    if (npcAction != NULL) {
        func_80B4B7F4(npcAction, &sp74);
        func_80B4B834(npcAction, &sp68);
        if (thisv->unk_1E6 == 0) {
            sp48 = sp74;
            thisv->actor.home.pos = sp48;
            thisv->actor.world.pos = sp48;
        }
        if (thisv->unk_1E6 != npcAction->action) {
            frameCount = Animation_GetLastFrame(spB0[npcAction->action]);
            Animation_Change(&thisv->skelAnime, spB0[npcAction->action], 1.0f, 0.0f, frameCount, spA4[npcAction->action],
                             -10.0f);
            thisv->unk_1E6 = npcAction->action;
        }
        thisv->actor.velocity = velocity;
        if (globalCtx->csCtx.frames < npcAction->endFrame) {
            actionLength = npcAction->endFrame - npcAction->startFrame;
            thisv->actor.velocity.x = (sp68.x - sp74.x) / actionLength;
            thisv->actor.velocity.y = (sp68.y - sp74.y) / actionLength;
            thisv->actor.velocity.y += thisv->actor.gravity;
            if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
                thisv->actor.velocity.y = thisv->actor.minVelocityY;
            }
            thisv->actor.velocity.z = (sp68.z - sp74.z) / actionLength;
        }
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_200, &thisv->unk_206, thisv->actor.focus.pos);
        Gameplay_CameraSetAtEye(globalCtx, thisv->unk_1E8, &sp98, &sp8C);
        Gameplay_CameraSetFov(globalCtx, thisv->unk_1E8, 70.0f);
    }
}

void func_80B4BBC4(EnZl1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 frameCount = Animation_GetLastFrame(&gChildZelda1Anim_00438);
    Player* player = GET_PLAYER(globalCtx);

    Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_00438, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    func_8002DF54(globalCtx, &thisv->actor, 1);
    func_8002F7DC(&player->actor, NA_SE_VO_LI_SURPRISE_KID);
    thisv->actor.textId = 0x7039;
    Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
    thisv->unk_1E2 = 0;
    thisv->actionFunc = func_80B4BF2C;
}

void func_80B4BC78(EnZl1* thisv, GlobalContext* globalCtx) {
    AnimationHeader* sp90[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &gChildZelda1Anim_12B04,
        &gChildZelda1Anim_12118,
        &gChildZelda1Anim_10B38,
    };
    u8 sp84[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02,
    };
    s32 pad2;
    f32 actionLength;
    Vec3f sp70;
    Vec3f sp64;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    CsCmdActorAction* npcAction;
    s32 pad;
    f32 frameCount;

    if (SkelAnime_Update(&thisv->skelAnime) && (thisv->skelAnime.animation == &gChildZelda1Anim_10B38)) {
        frameCount = Animation_GetLastFrame(&gChildZelda1Anim_11348);
        Animation_Change(&thisv->skelAnime, &gChildZelda1Anim_11348, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -10.0f);
    }
    func_80B4B874(thisv, globalCtx);
    npcAction = globalCtx->csCtx.npcActions[0];
    if (npcAction != NULL) {
        func_80B4B7F4(npcAction, &sp70);
        func_80B4B834(npcAction, &sp64);
        if (thisv->unk_1E6 == 0) {
            thisv->actor.world.pos = thisv->actor.home.pos = sp70;
        }

        if (thisv->unk_1E6 != npcAction->action) {
            frameCount = Animation_GetLastFrame(sp90[npcAction->action]);
            Animation_Change(&thisv->skelAnime, sp90[npcAction->action], 1.0f, 0.0f, frameCount, sp84[npcAction->action],
                             -10.0f);
            thisv->unk_1E6 = npcAction->action;
        }
        thisv->actor.velocity = velocity;
        if (globalCtx->csCtx.frames < npcAction->endFrame) {
            actionLength = npcAction->endFrame - npcAction->startFrame;
            thisv->actor.velocity.x = (sp64.x - sp70.x) / actionLength;
            thisv->actor.velocity.y = (sp64.y - sp70.y) / actionLength;
            thisv->actor.velocity.y += thisv->actor.gravity;
            if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
                thisv->actor.velocity.y = thisv->actor.minVelocityY;
            }
            thisv->actor.velocity.z = (sp64.z - sp70.z) / actionLength;
        }
    }
}

void func_80B4BF2C(EnZl1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    Player* player = GET_PLAYER(globalCtx);

    switch (thisv->unk_1E2) {
        case 0:
            if ((Message_GetState(msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
                if (msgCtx->choiceIndex == 0) {
                    thisv->actor.textId = 0x703B;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->unk_1E2++;
                } else {
                    thisv->actor.textId = 0x703A;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->unk_1E2 = 0;
                }
            }
            break;
        case 1:
            if ((Message_GetState(msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
                thisv->actor.textId = 0xFFFF;
                globalCtx->talkWithPlayer(globalCtx, &thisv->actor);
                func_8002F434(&thisv->actor, globalCtx, GI_LETTER_ZELDA, 120.0f, 10.0f);
                globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                globalCtx->msgCtx.stateTimer = 4;
                thisv->unk_1E2++;
            } else {
                break;
            }
        case 2:
            if (Actor_HasParent(&thisv->actor, globalCtx)) {
                Gameplay_CopyCamera(globalCtx, MAIN_CAM, thisv->unk_1E8);
                Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
                Gameplay_ClearCamera(globalCtx, thisv->unk_1E8);
                thisv->actor.parent = NULL;
                thisv->unk_1E2++;
            } else {
                func_8002F434(&thisv->actor, globalCtx, GI_LETTER_ZELDA, 120.0f, 10.0f);
            }
            break;
        case 3:
            if ((Message_GetState(msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
                thisv->actor.textId = 0x703C;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                Flags_SetEventChkInf(0x40);
                thisv->unk_1E2 = 6;
            }
            break;
        case 4:
            if (player->actor.world.pos.y < thisv->actor.world.pos.y) {
                break;
            } else {
                if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
                    thisv->unk_1E2++;
                } else {
                    func_8002F2F4(&thisv->actor, globalCtx);
                }
            }
            break;
        case 5:
            if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
                thisv->unk_1E2--;
            }
            break;
        case 6:
            if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
                func_8002DF54(globalCtx, &thisv->actor, 7);
                Interface_ChangeAlpha(50);
                thisv->actor.flags &= ~ACTOR_FLAG_8;
                thisv->unk_1E2 = 4;
            }
            break;
    }
    func_80038290(globalCtx, &thisv->actor, &thisv->unk_200, &thisv->unk_206, thisv->actor.focus.pos);
}

void EnZl1_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnZl1* thisv = (EnZl1*)thisx;

    if ((thisv->actionFunc != func_80B4B8B4) && (thisv->actionFunc != func_80B4BC78)) {
        SkelAnime_Update(&thisv->skelAnime);
    }
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 5);
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc != func_80B4B8B4) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->actor.world.rot.x, 0xA, 0x3E8, 1);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 0xA, 0x3E8, 1);
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->actor.world.rot.z, 0xA, 0x3E8, 1);
    func_80B4AE18(thisv);
}

s32 EnZl1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnZl1* thisv = (EnZl1*)thisx;

    if ((limbIndex == 4) || (limbIndex == 3) || (limbIndex == 6) || (limbIndex == 5)) {
        *dList = NULL;
    }

    if (limbIndex != 10) {
        if (limbIndex == 17) {
            rot->x += thisv->unk_200.y;
            rot->y += thisv->unk_200.z;
            rot->z += thisv->unk_200.x;
        }
    } else {
        rot->x += thisv->unk_206.y;
        rot->y += thisv->unk_206.x;
        rot->z += thisv->unk_206.z;
    }
    return 0;
}

void EnZl1_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    EnZl1* thisv = (EnZl1*)thisx;

    if (limbIndex == 17) {
        Matrix_MultVec3f(&vec, &thisv->actor.focus.pos);
    }
}

void EnZl1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnZl1* thisv = (EnZl1*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_girlB.c", 2011);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(thisv->unk_1F4));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(thisv->unk_1F8));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(thisv->unk_1EC));

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnZl1_OverrideLimbDraw, EnZl1_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_girlB.c", 2046);
}
