#include "z_en_du.h"
#include "objects/object_du/object_du.h"
#include "scenes/overworld/spot18/spot18_scene.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_25)

void EnDu_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDu_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDu_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809FE3B4(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE3C0(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE638(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE890(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE4A4(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE6CC(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE740(EnDu* thisv, GlobalContext* globalCtx);
void func_809FE798(EnDu* thisv, GlobalContext* globalCtx);
void func_809FEC14(EnDu* thisv, GlobalContext* globalCtx);
void func_809FEC70(EnDu* thisv, GlobalContext* globalCtx);
void func_809FECE4(EnDu* thisv, GlobalContext* globalCtx);
void func_809FEB08(EnDu* thisv, GlobalContext* globalCtx);

ActorInit En_Du_InitVars = {
    ACTOR_EN_DU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DU,
    sizeof(EnDu),
    (ActorFunc)EnDu_Init,
    (ActorFunc)EnDu_Destroy,
    (ActorFunc)EnDu_Update,
    (ActorFunc)EnDu_Draw,
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
    /*  0 */ ENDU_ANIM_0,
    /*  1 */ ENDU_ANIM_1,
    /*  2 */ ENDU_ANIM_2,
    /*  3 */ ENDU_ANIM_3,
    /*  4 */ ENDU_ANIM_4,
    /*  5 */ ENDU_ANIM_5,
    /*  6 */ ENDU_ANIM_6,
    /*  7 */ ENDU_ANIM_7,
    /*  8 */ ENDU_ANIM_8,
    /*  9 */ ENDU_ANIM_9,
    /* 10 */ ENDU_ANIM_10,
    /* 11 */ ENDU_ANIM_11,
    /* 12 */ ENDU_ANIM_12,
    /* 13 */ ENDU_ANIM_13,
    /* 14 */ ENDU_ANIM_14
} EnDuAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaHitBreastAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },
    { &gDaruniaDancingLoop1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingLoop3Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaWrongSongEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDaruniaDancingLoop4Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gDaruniaDancingEndAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -6.0f },
};

void EnDu_SetupAction(EnDu* thisv, EnDuActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

u16 func_809FDC38(GlobalContext* globalCtx, Actor* actor) {
    u16 reaction = Text_GetFaceReaction(globalCtx, 0x21);

    if (reaction != 0) {
        return reaction;
    }
    if (CUR_UPG_VALUE(UPG_STRENGTH) != 0) {
        if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
            return 0x301E;
        } else {
            return 0x301D;
        }
    }
    if (gSaveContext.infTable[0x11] & 8) {
        return 0x301B;
    } else {
        return 0x301A;
    }
}

s16 func_809FDCDC(GlobalContext* globalCtx, Actor* actor) {
    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            switch (actor->textId) {
                case 0x301A:
                    gSaveContext.infTable[0x11] |= 8;
                    break;
                case 0x301C:
                case 0x301F:
                    return 2;
                case 0x3020:
                    gSaveContext.eventChkInf[0x2] |= 4;
                    break;
            }
            return 0;
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                return 3;
            }
            break;
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return 1;
}

s32 func_809FDDB4(EnDu* thisv, GlobalContext* globalCtx) {
    if (globalCtx->sceneNum == SCENE_SPOT18 && LINK_IS_CHILD) {
        return 1;
    } else if (globalCtx->sceneNum == SCENE_HIDAN && !(gSaveContext.infTable[0x11] & 0x400) && LINK_IS_ADULT) {
        return 1;
    }
    return 0;
}

void func_809FDE24(EnDu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a3 = 0;

    if (thisv->unk_1F4.unk_00 == 0) {
        phi_a3 = 1;
    }
    if (thisv->actionFunc == func_809FE890) {
        phi_a3 = 1;
    }
    thisv->unk_1F4.unk_18 = player->actor.world.pos;
    thisv->unk_1F4.unk_14 = 10.0f;
    func_80034A14(&thisv->actor, &thisv->unk_1F4, 3, phi_a3);
}

void func_809FDE9C(EnDu* thisv) {
    if (thisv->blinkTimer > 0) {
        thisv->blinkTimer--;
    } else {
        thisv->blinkTimer = 0;
    }
    if (thisv->blinkTimer < 3) {
        thisv->eyeTexIndex = thisv->blinkTimer;
    }

    switch (thisv->unk_1EC) {
        case 0:
            if (thisv->blinkTimer == 0) {
                thisv->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;
        case 1:
            if (thisv->blinkTimer == 0) {
                thisv->eyeTexIndex = 2;
            }
            break;
        case 2:
            if (thisv->blinkTimer == 0) {
                thisv->eyeTexIndex = 2;
            }
            break;
        case 3:
            if (thisv->blinkTimer == 0) {
                thisv->eyeTexIndex = 0;
            }
            break;
    }

    switch (thisv->unk_1ED) {
        case 1:
            thisv->mouthTexIndex = 1;
            break;
        case 2:
            thisv->mouthTexIndex = 2;
            break;
        case 3:
            thisv->mouthTexIndex = 3;
            break;
        default:
            thisv->mouthTexIndex = 0;
            break;
    }

    if (thisv->unk_1EE == 1) {
        thisv->noseTexIndex = 1;
    } else {
        thisv->noseTexIndex = 0;
    }
}

void func_809FDFC0(const CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->startPos.x;
    dst->y = csAction->startPos.y;
    dst->z = csAction->startPos.z;
}

void func_809FE000(const CsCmdActorAction* csAction, Vec3f* dst) {
    dst->x = csAction->endPos.x;
    dst->y = csAction->endPos.y;
    dst->z = csAction->endPos.z;
}

void func_809FE040(EnDu* thisv) {
    s32 animationIndices[] = {
        ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_8, ENDU_ANIM_9, ENDU_ANIM_10, ENDU_ANIM_10, ENDU_ANIM_13,
    };

    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        thisv->unk_1E6++;
        if (thisv->unk_1E6 >= 8) {
            thisv->unk_1E6 = 0;
        }
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animationIndices[thisv->unk_1E6]);
    }
}

void func_809FE104(EnDu* thisv) {
    s32 animationIndices[] = {
        ENDU_ANIM_8,
        ENDU_ANIM_8,
        ENDU_ANIM_11,
        ENDU_ANIM_12,
    };

    if (thisv->unk_1E6 < 4) {
        if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
            thisv->unk_1E6++;
            if (thisv->unk_1E6 < 4) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animationIndices[thisv->unk_1E6]);
            }
        }
    }
}

void EnDu_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDu* thisv = (EnDu*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gDaruniaSkel, NULL, 0, 0, 0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    if (func_809FDDB4(thisv, globalCtx) == 0) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_0);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.targetMode = 1;
    thisv->unk_1F4.unk_00 = 0;

    if (gSaveContext.cutsceneIndex >= 0xFFF0) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDarunia01Cs);
        gSaveContext.cutsceneTrigger = 1;
        EnDu_SetupAction(thisv, func_809FE890);
    } else if (globalCtx->sceneNum == 4) {
        EnDu_SetupAction(thisv, func_809FE638);
    } else if (!LINK_IS_ADULT) {
        EnDu_SetupAction(thisv, func_809FE3C0);
    } else {
        EnDu_SetupAction(thisv, func_809FE3B4);
    }
}

void EnDu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnDu* thisv = (EnDu*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_809FE3B4(EnDu* thisv, GlobalContext* globalCtx) {
}

void func_809FE3C0(EnDu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags2 & 0x1000000) {
        func_8010BD88(globalCtx, OCARINA_ACTION_CHECK_SARIA);
        player->stateFlags2 |= 0x2000000;
        player->unk_6A8 = &thisv->actor;
        EnDu_SetupAction(thisv, func_809FE4A4);
        return;
    }
    if (thisv->unk_1F4.unk_00 == 2) {
        func_8002DF54(globalCtx, &thisv->actor, 7);
        thisv->unk_1F4.unk_00 = 0;
    }
    if (thisv->actor.xzDistToPlayer < 116.0f + thisv->collider.dim.radius) {
        player->stateFlags2 |= 0x800000;
    }
}

void func_809FE4A4(EnDu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
        EnDu_SetupAction(thisv, func_809FE3C0);
    } else if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_06) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaWrongCs);
        gSaveContext.cutsceneTrigger = 1;
        thisv->unk_1E8 = 1;
        EnDu_SetupAction(thisv, func_809FE890);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGoronCityDaruniaCorrectCs);
        gSaveContext.cutsceneTrigger = 1;
        thisv->unk_1E8 = 0;
        EnDu_SetupAction(thisv, func_809FE890);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    } else {
        player->stateFlags2 |= 0x800000;
    }
}

void func_809FE638(EnDu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!(player->stateFlags1 & 0x20000000)) {
        OnePointCutscene_Init(globalCtx, 3330, -99, &thisv->actor, MAIN_CAM);
        player->actor.shape.rot.y = player->actor.world.rot.y = thisv->actor.world.rot.y + 0x7FFF;
        Audio_PlayFanfare(NA_BGM_APPEAR);
        EnDu_SetupAction(thisv, func_809FE6CC);
        thisv->unk_1E2 = 0x32;
    }
}

void func_809FE6CC(EnDu* thisv, GlobalContext* globalCtx) {
    s16 phi_v1;

    if (thisv->unk_1E2 == 0) {
        phi_v1 = 0;
    } else {
        thisv->unk_1E2--;
        phi_v1 = thisv->unk_1E2;
    }
    if (phi_v1 == 0) {
        thisv->actor.textId = 0x3039;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        thisv->unk_1F4.unk_00 = 1;
        EnDu_SetupAction(thisv, func_809FE740);
    }
}

void func_809FE740(EnDu* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1F4.unk_00 == 0) {
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        thisv->unk_1E2 = 0x5A;
        EnDu_SetupAction(thisv, func_809FE798);
    }
}

void func_809FE798(EnDu* thisv, GlobalContext* globalCtx) {
    s32 phi_v0;

    if (thisv->unk_1E2 == 0) {
        phi_v0 = 0;
    } else {
        thisv->unk_1E2--;
        phi_v0 = thisv->unk_1E2;
    }
    if (phi_v0 != 0) {
        switch (thisv->unk_1E2) {
            case 0x50:
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHAIN_KEY_UNLOCK_B);
                break;
            case 0x3C:
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_SLIDE_DOOR_OPEN);
                break;
            case 0xF:
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                break;
            case 5:
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_STONE_BOUND);
                break;
        }
        if (thisv->unk_1E2 >= 0x3D) {
            thisv->actor.world.pos.x -= 10.0f;
        }
    } else {
        Actor_Kill(&thisv->actor);
        gSaveContext.infTable[0x11] |= 0x400;
    }
}

void func_809FE890(EnDu* thisv, GlobalContext* globalCtx) {
    f32 frame;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    const CsCmdActorAction* csAction;

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        func_8002DF54(globalCtx, &thisv->actor, 1);
        EnDu_SetupAction(thisv, func_809FEB08);
        return;
    }
    csAction = globalCtx->csCtx.npcActions[2];

    if (csAction != NULL) {
        func_809FDFC0(csAction, &startPos);
        func_809FE000(csAction, &endPos);
        if (thisv->unk_1EA == 0) {
            func_809FDFC0(csAction, &startPos);
            thisv->actor.world.pos = startPos;
        }
        if (thisv->unk_1EA != csAction->action) {
            if (csAction->action == 1) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_1);
            }
            if (csAction->action == 7 || csAction->action == 8) {
                thisv->unk_1E6 = 0;
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_7);
            }
            thisv->unk_1EA = csAction->action;
            if (thisv->unk_1EA == 7) {
                thisv->blinkTimer = 11;
                thisv->unk_1EC = 2;
                thisv->unk_1ED = 2;
                thisv->unk_1EE = 1;
            }
            if (thisv->unk_1EA == 8) {
                thisv->blinkTimer = 11;
                thisv->unk_1EC = 3;
                thisv->unk_1ED = 3;
                thisv->unk_1EE = 0;
            }
        }
        if (thisv->unk_1EA == 7) {
            func_809FE040(thisv);
        }
        if (thisv->unk_1EA == 8) {
            func_809FE104(thisv);
        }
        thisv->actor.shape.rot.x = csAction->urot.x;
        thisv->actor.shape.rot.y = csAction->urot.y;
        thisv->actor.shape.rot.z = csAction->urot.z;
        thisv->actor.velocity = velocity;

        if (globalCtx->csCtx.frames < csAction->endFrame) {
            frame = csAction->endFrame - csAction->startFrame;

            thisv->actor.velocity.x = (endPos.x - startPos.x) / frame;
            thisv->actor.velocity.y = (endPos.y - startPos.y) / frame;
            thisv->actor.velocity.y += thisv->actor.gravity;
            if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
                thisv->actor.velocity.y = thisv->actor.minVelocityY;
            }
            thisv->actor.velocity.z = (endPos.z - startPos.z) / frame;
        }
    }
}

void func_809FEB08(EnDu* thisv, GlobalContext* globalCtx) {
    thisv->blinkTimer = 11;
    thisv->unk_1EC = 0;
    thisv->unk_1ED = 0;
    thisv->unk_1EE = 0;

    if (thisv->unk_1E8 == 1) {
        func_8002DF54(globalCtx, &thisv->actor, 7);
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_1);
        EnDu_SetupAction(thisv, func_809FE3C0);
        return;
    }
    if (CUR_UPG_VALUE(UPG_STRENGTH) <= 0) {
        thisv->actor.textId = 0x301C;
        EnDu_SetupAction(thisv, func_809FEC14);
    } else {
        thisv->actor.textId = 0x301F;
        EnDu_SetupAction(thisv, func_809FE3C0);
    }
    Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_14);
    thisv->unk_1F4.unk_00 = 1;
}

void func_809FEC14(EnDu* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1F4.unk_00 == 2) {
        func_8002DF54(globalCtx, &thisv->actor, 7);
        EnDu_SetupAction(thisv, func_809FEC70);
        func_809FEC70(thisv, globalCtx);
    }
}

void func_809FEC70(EnDu* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        EnDu_SetupAction(thisv, func_809FECE4);
    } else {
        f32 xzRange = thisv->actor.xzDistToPlayer + 1.0f;
        func_8002F434(&thisv->actor, globalCtx, GI_BRACELET, xzRange, fabsf(thisv->actor.yDistToPlayer) + 1.0f);
    }
}

void func_809FECE4(EnDu* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1F4.unk_00 == 3) {
        thisv->unk_1F4.unk_00 = 0;
        EnDu_SetupAction(thisv, func_809FE3C0);
    }
}

void EnDu_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDu* thisv = (EnDu*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    if (thisv->skelAnime.animation == &gDaruniaDancingEndAnim &&
        Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENDU_ANIM_1);
    }

    SkelAnime_Update(&thisv->skelAnime);
    func_809FDE9C(thisv);
    func_809FDE24(thisv, globalCtx);

    if (thisv->actionFunc == func_809FE890) {
        thisv->actor.world.pos.x += thisv->actor.velocity.x;
        thisv->actor.world.pos.y += thisv->actor.velocity.y;
        thisv->actor.world.pos.z += thisv->actor.velocity.z;
    } else {
        func_8002D7EC(&thisv->actor);
    }

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    if (thisv->actionFunc != func_809FE4A4) {
        func_800343CC(globalCtx, &thisv->actor, &thisv->unk_1F4.unk_00, thisv->collider.dim.radius + 116.0f, func_809FDC38,
                      func_809FDCDC);
    }
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnDu_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnDu* thisv = (EnDu*)thisx;
    Vec3s sp1C;

    if (limbIndex == 16) {
        Matrix_Translate(2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp1C = thisv->unk_1F4.unk_08;
        Matrix_RotateX(BINANG_TO_RAD(sp1C.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp1C.x), MTXMODE_APPLY);
        Matrix_Translate(-2400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8) {
        sp1C = thisv->unk_1F4.unk_0E;
        Matrix_RotateY(BINANG_TO_RAD(sp1C.y), MTXMODE_APPLY);
        Matrix_RotateX(BINANG_TO_RAD(sp1C.x), MTXMODE_APPLY);
    }
    return 0;
}

void EnDu_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnDu* thisv = (EnDu*)thisx;
    Vec3f D_809FF40C = { 0.0f, -1000.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_MultVec3f(&D_809FF40C, &thisv->actor.focus.pos);
    }
}

void EnDu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = {
        gDaruniaEyeOpenTex,
        gDaruniaEyeOpeningTex,
        gDaruniaEyeShutTex,
        gDaruniaEyeClosingTex,
    };
    static const void* mouthTextures[] = {
        gDaruniaMouthSeriousTex,
        gDaruniaMouthGrinningTex,
        gDaruniaMouthOpenTex,
        gDaruniaMouthHappyTex,
    };
    static const void* noseTextures[] = {
        gDaruniaNoseSeriousTex,
        gDaruniaNoseHappyTex,
    };
    EnDu* thisv = (EnDu*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_du.c", 1470);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(noseTextures[thisv->noseTexIndex]));

    func_80034BA0(globalCtx, &thisv->skelAnime, EnDu_OverrideLimbDraw, EnDu_PostLimbDraw, &thisv->actor, 255);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_du.c", 1487);
}
