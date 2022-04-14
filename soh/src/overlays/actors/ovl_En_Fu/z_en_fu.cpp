/*
 * File: z_en_fu.c
 * Overlay: ovl_En_Fu
 * Description: Windmill Man
 */

#include "z_en_fu.h"
#include "objects/object_fu/object_fu.h"
#include "scenes/indoors/hakasitarelay/hakasitarelay_scene.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

#define FU_RESET_LOOK_ANGLE (1 << 0)
#define FU_WAIT (1 << 1)

void EnFu_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFu_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFu_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFu_WaitChild(EnFu* thisv, GlobalContext* globalCtx);
void func_80A1DA04(EnFu* thisv, GlobalContext* globalCtx);

void EnFu_WaitAdult(EnFu* thisv, GlobalContext* globalCtx);
void EnFu_TeachSong(EnFu* thisv, GlobalContext* globalCtx);
void EnFu_WaitForPlayback(EnFu* thisv, GlobalContext* globalCtx);
void func_80A1DBA0(EnFu* thisv, GlobalContext* globalCtx);
void func_80A1DBD4(EnFu* thisv, GlobalContext* globalCtx);
void func_80A1DB60(EnFu* thisv, GlobalContext* globalCtx);

const ActorInit En_Fu_InitVars = {
    ACTOR_EN_FU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FU,
    sizeof(EnFu),
    (ActorFunc)EnFu_Init,
    (ActorFunc)EnFu_Destroy,
    (ActorFunc)EnFu_Update,
    (ActorFunc)EnFu_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static Vec3f sMtxSrc = {
    700.0f,
    700.0f,
    0.0f,
};

typedef enum {
    /* 0x00 */ FU_FACE_CALM,
    /* 0x01 */ FU_FACE_MAD
} EnFuFace;

void EnFu_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFu* thisv = (EnFu*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelanime, &gWindmillManSkel, &gWindmillManPlayStillAnim, thisv->jointTable,
                       thisv->morphTable, FU_LIMB_MAX);
    Animation_PlayLoop(&thisv->skelanime, &gWindmillManPlayStillAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    if (!LINK_IS_ADULT) {
        thisv->actionFunc = EnFu_WaitChild;
        thisv->facialExpression = FU_FACE_CALM;
    } else {
        thisv->actionFunc = EnFu_WaitAdult;
        thisv->facialExpression = FU_FACE_MAD;
        thisv->skelanime.playSpeed = 2.0f;
    }
    thisv->behaviorFlags = 0;
    thisv->actor.targetMode = 6;
}

void EnFu_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFu* thisv = (EnFu*)thisx;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80A1D94C(EnFu* thisv, GlobalContext* globalCtx, u16 textID, EnFuActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = actionFunc;
        return true;
    }
    thisv->actor.textId = textID;
    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x2301) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    } else {
        thisv->behaviorFlags |= FU_RESET_LOOK_ANGLE;
    }
    return false;
}

void func_80A1DA04(EnFu* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->behaviorFlags &= ~FU_WAIT;
        thisv->actionFunc = EnFu_WaitChild;

        if (thisv->skelanime.animation == &gWindmillManPlayAndMoveHeadAnim) {
            Animation_Change(&thisv->skelanime, &gWindmillManPlayStillAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gWindmillManPlayStillAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

void EnFu_WaitChild(EnFu* thisv, GlobalContext* globalCtx) {
    u16 textID = Text_GetFaceReaction(globalCtx, 0xB);

    if (textID == 0) {
        textID = (gSaveContext.eventChkInf[6] & 0x80) ? 0x5033 : 0x5032;
    }

    // if ACTOR_FLAG_8 is set and textID is 0x5033, change animation
    // if func_80A1D94C returns 1, actionFunc is set to func_80A1DA04
    if (func_80A1D94C(thisv, globalCtx, textID, func_80A1DA04)) {
        if (textID == 0x5033) {
            Animation_Change(&thisv->skelanime, &gWindmillManPlayAndMoveHeadAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gWindmillManPlayAndMoveHeadAnim), ANIMMODE_ONCE, -4.0f);
        }
    }
}

void func_80A1DB60(EnFu* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        thisv->actionFunc = EnFu_WaitAdult;
        gSaveContext.eventChkInf[5] |= 0x800;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    }
}

void func_80A1DBA0(EnFu* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnFu_WaitAdult;
    }
}

void func_80A1DBD4(EnFu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_04) {
        thisv->actionFunc = EnFu_WaitAdult;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        thisv->actionFunc = func_80A1DB60;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gSongOfStormsCs);
        gSaveContext.cutsceneTrigger = 1;
        Item_Give(globalCtx, ITEM_SONG_STORMS);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
        gSaveContext.eventChkInf[6] |= 0x20;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        player->stateFlags2 &= ~0x1000000;
        thisv->actionFunc = EnFu_WaitAdult;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        player->stateFlags2 |= 0x800000;
    }
}

void EnFu_WaitForPlayback(EnFu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    player->stateFlags2 |= 0x800000;
    // if dialog state is 7, player has played back the song
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        func_8010BD58(globalCtx, OCARINA_ACTION_PLAYBACK_STORMS);
        thisv->actionFunc = func_80A1DBD4;
    }
}

void EnFu_TeachSong(EnFu* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    player->stateFlags2 |= 0x800000;
    // if dialog state is 2, start song demonstration
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->behaviorFlags &= ~FU_WAIT;
        Audio_OcaSetInstrument(4); // seems to be related to setting instrument type
        func_8010BD58(globalCtx, OCARINA_ACTION_TEACH_STORMS);
        thisv->actionFunc = EnFu_WaitForPlayback;
    }
}

void EnFu_WaitAdult(EnFu* thisv, GlobalContext* globalCtx) {
    static s16 yawDiff;
    Player* player = GET_PLAYER(globalCtx);

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if ((gSaveContext.eventChkInf[5] & 0x800)) {
        func_80A1D94C(thisv, globalCtx, 0x508E, func_80A1DBA0);
    } else if (player->stateFlags2 & 0x1000000) {
        thisv->actor.textId = 0x5035;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        thisv->actionFunc = EnFu_TeachSong;
        thisv->behaviorFlags |= FU_WAIT;
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80A1DBA0;
    } else if (ABS(yawDiff) < 0x2301) {
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            thisv->actor.textId = 0x5034;
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
            player->stateFlags2 |= 0x800000;
        }
    }
}

void EnFu_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFu* thisv = (EnFu*)thisx;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if ((!(thisv->behaviorFlags & FU_WAIT)) && (SkelAnime_Update(&thisv->skelanime) != 0)) {
        Animation_Change(&thisv->skelanime, thisv->skelanime.animation, 1.0f, 0.0f,
                         Animation_GetLastFrame(thisv->skelanime.animation), ANIMMODE_ONCE, 0.0f);
    }
    thisv->actionFunc(thisv, globalCtx);
    if ((thisv->behaviorFlags & FU_RESET_LOOK_ANGLE)) {
        Math_SmoothStepToS(&thisv->lookAngleOffset.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->lookAngleOffset.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.y, 0, 6, 6200, 100);
        thisv->behaviorFlags &= ~FU_RESET_LOOK_ANGLE;
    } else {
        func_80038290(globalCtx, &thisv->actor, &thisv->lookAngleOffset, &thisv->unk_2A2, thisv->actor.focus.pos);
    }
}

s32 EnFu_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnFu* thisv = (EnFu*)thisx;
    s32 pad;

    if (limbIndex == FU_LIMB_UNK) {
        return false;
    }
    switch (limbIndex) {
        case FU_LIMB_HEAD:
            rot->x += thisv->lookAngleOffset.y;
            rot->z += thisv->lookAngleOffset.x;
            break;
        case FU_LIMB_CHEST_MUSIC_BOX:
            break;
    }

    if (!(thisv->behaviorFlags & FU_WAIT)) {
        return false;
    }

    if (limbIndex == FU_LIMB_CHEST_MUSIC_BOX) {
        rot->y += (Math_SinS((globalCtx->state.frames * (limbIndex * 50 + 0x814))) * 200.0f);
        rot->z += (Math_CosS((globalCtx->state.frames * (limbIndex * 50 + 0x940))) * 200.0f);
    }
    return false;
}

void EnFu_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFu* thisv = (EnFu*)thisx;

    if (limbIndex == FU_LIMB_HEAD) {
        Matrix_MultVec3f(&sMtxSrc, &thisv->actor.focus.pos);
    }
}

void EnFu_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* sEyesSegments[] = { gWindmillManEyeClosedTex, gWindmillManEyeAngryTex };
    static void* sMouthSegments[] = { gWindmillManMouthOpenTex, gWindmillManMouthAngryTex };
    s32 pad;
    EnFu* thisv = (EnFu*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fu.c", 773);

    func_800943C8(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyesSegments[thisv->facialExpression]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthSegments[thisv->facialExpression]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, thisv->skelanime.dListCount,
                          EnFu_OverrideLimbDraw, EnFu_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fu.c", 791);
}
