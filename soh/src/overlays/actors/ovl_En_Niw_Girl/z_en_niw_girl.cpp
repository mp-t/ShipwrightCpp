/*
 * File: z_en_niw_girl.c
 * Overlay: ovl_En_Niw_Girl
 * Description: Girl that chases a cuckoo around in Hyrule Market and Kakariko Village
 */

#include "z_en_niw_girl.h"
#include "objects/object_gr/object_gr.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnNiwGirl_Init(Actor* thisx, GlobalContext* globalCtx);
void EnNiwGirl_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnNiwGirl_Update(Actor* thisx, GlobalContext* globalCtx);
void EnNiwGirl_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnNiwGirl_Talk(EnNiwGirl* thisv, GlobalContext* globalCtx);
void func_80AB94D0(EnNiwGirl* thisv, GlobalContext* globalCtx);
void func_80AB9210(EnNiwGirl* thisv, GlobalContext* globalCtx);

const ActorInit En_Niw_Girl_InitVars = {
    ACTOR_EN_NIW_GIRL,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GR,
    sizeof(EnNiwGirl),
    (ActorFunc)EnNiwGirl_Init,
    (ActorFunc)EnNiwGirl_Destroy,
    (ActorFunc)EnNiwGirl_Update,
    (ActorFunc)EnNiwGirl_Draw,
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
    { 10, 30, 0, { 0, 0, 0 } },
};

void EnNiwGirl_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnNiwGirl* thisv = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f vec1;
    Vec3f vec2;
    s32 pad2;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gNiwGirlSkel, &gNiwGirlRunAnim, thisv->jointTable, thisv->morphTable,
                       17);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.targetMode = 6;
    if (thisv->actor.params < 0) {
        thisv->actor.params = 0;
    }
    thisv->path = ((thisv->actor.params >> 8) & 0xFF);
    thisv->actor.gravity = -3.0f;
    Matrix_RotateY((thisv->actor.shape.rot.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
    vec2.x = vec2.y = vec2.z = 0.0f;
    vec1.x = vec1.y = 0.0f;
    vec1.z = 50.0;
    Matrix_MultVec3f(&vec1, &vec2);
    thisv->chasedEnNiw = (EnNiw*)Actor_SpawnAsChild(
        &globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_NIW, thisv->actor.world.pos.x + vec2.x,
        thisv->actor.world.pos.y + vec2.y, thisv->actor.world.pos.z + vec2.z, 0, thisv->actor.world.rot.y, 0, 0xA);
    if (thisv->chasedEnNiw != NULL) {
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ シツレイしちゃうわね！プンプン ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ きゃははははは、まてー ☆☆☆☆☆ %d\n" VT_RST, thisv->path);
        osSyncPrintf("\n\n");
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        thisv->actionFunc = EnNiwGirl_Talk;
    } else {
        osSyncPrintf("\n\n");
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ なぜか、セットできむぅあせん ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ んんがくく ☆☆☆☆☆ %d\n" VT_RST, thisv->path);
        osSyncPrintf("\n\n");
        Actor_Kill(&thisv->actor);
    }
}

void EnNiwGirl_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnNiwGirl_Jump(EnNiwGirl* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&gNiwGirlRunAnim);
    Animation_Change(&thisv->skelAnime, &gNiwGirlRunAnim, 1.0f, 0.0f, frameCount, 0, -10.0f);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = func_80AB9210;
}

void func_80AB9210(EnNiwGirl* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->path];
    f32 xDistBetween;
    f32 zDistBetween;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.speedXZ, 3.0f, 0.2f, 0.4f);

    // Find the X and Z distance between the girl and the cuckoo she is chasing
    xDistBetween = thisv->chasedEnNiw->actor.world.pos.x - thisv->actor.world.pos.x;
    zDistBetween = thisv->chasedEnNiw->actor.world.pos.z - thisv->actor.world.pos.z;
    if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_NONE) {
        thisv->chasedEnNiw->path = 0;
    }
    if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) < 70.0f) {
        thisv->chasedEnNiw->path = (thisv->path + 1);
        thisv->chasedEnNiw->unk_2EC = path->count;
    } else if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) > 150.0f) {
        thisv->chasedEnNiw->path = 0;
    }

    // Change her angle so that she is always facing the cuckoo
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(xDistBetween, zDistBetween) * (0x8000 / std::numbers::pi_v<float>), 3,
                       thisv->unk_27C, 0);
    Math_ApproachF(&thisv->unk_27C, 5000.0f, 30.0f, 150.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

    // Only allow Link to talk to her when she is playing the jumping animation
    if ((thisv->jumpTimer == 0) || (Player_GetMask(globalCtx) != PLAYER_MASK_NONE)) {
        thisv->jumpTimer = 60;
        thisv->actionFunc = EnNiwGirl_Talk;
    }
}

void EnNiwGirl_Talk(EnNiwGirl* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gNiwGirlJumpAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gNiwGirlJumpAnim), 0,
                     -10.0f);
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actor.textId = 0x7000;
    if ((gSaveContext.eventChkInf[8] & 1) && (thisv->unk_27A == 0)) {
        thisv->actor.textId = 0x70EA;
    }
    switch (Player_GetMask(globalCtx)) {
        case PLAYER_MASK_KEATON:
            thisv->actor.textId = 0x7118;
            break;
        case PLAYER_MASK_SPOOKY:
            thisv->actor.textId = 0x7119;
            break;
        case PLAYER_MASK_BUNNY:
        case PLAYER_MASK_ZORA:
        case PLAYER_MASK_GERUDO:
            thisv->actor.textId = 0x711A;
            break;
        case PLAYER_MASK_SKULL:
        case PLAYER_MASK_GORON:
        case PLAYER_MASK_TRUTH:
            thisv->actor.textId = 0x711B;
            break;
    }
    thisv->unk_270 = 6;
    thisv->actionFunc = func_80AB94D0;
}

void func_80AB94D0(EnNiwGirl* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_NONE) {
        thisv->chasedEnNiw->path = 0;
    }
    Math_ApproachZeroF(&thisv->actor.speedXZ, 0.8f, 0.2f);
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->actor.textId == 0x70EA) {
            thisv->unk_27A = 1;
        }
    } else {
        if ((thisv->jumpTimer == 0) && Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_NONE) {
            thisv->jumpTimer = Rand_ZeroFloat(100.0f) + 250.0f;
            thisv->actionFunc = EnNiwGirl_Jump;
        } else {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
}

void EnNiwGirl_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnNiwGirl* thisv = (EnNiwGirl*)thisx;
    EnNiwGirlActionFunc tempActionFunc;
    Player* player = GET_PLAYER(globalCtx);

    Actor_SetScale(&thisv->actor, 0.013f);
    thisv->unkUpTimer++;
    tempActionFunc = func_80AB94D0;
    if (thisv->blinkTimer == 0) {
        thisv->eyeIndex++;
        if (thisv->eyeIndex >= 3) {
            thisv->eyeIndex = 0;
            thisv->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    thisv->unk_280 = 30.0f;
    Actor_SetFocus(&thisv->actor, 30.0f);
    if (tempActionFunc == thisv->actionFunc) {
        thisv->unk_2D4.unk_18 = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            thisv->unk_2D4.unk_18.y = player->actor.world.pos.y - 10.0f;
        }
        func_80034A14(&thisv->actor, &thisv->unk_2D4, 2, 4);
        thisv->unk_260 = thisv->unk_2D4.unk_08;
        thisv->unk_266 = thisv->unk_2D4.unk_0E;
    } else {
        Math_SmoothStepToS(&thisv->unk_266.y, 0, 5, 3000, 0);
        Math_SmoothStepToS(&thisv->unk_260.y, 0, 5, 3000, 0);
        Math_SmoothStepToS(&thisv->unk_260.z, 0, 5, 3000, 0);
    }
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    }
    if (thisv->jumpTimer != 0) {
        thisv->jumpTimer--;
    }
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 100.0f, 100.0f, 200.0f, 0x1C);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnNiwGirlOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnNiwGirl* thisv = (EnNiwGirl*)thisx;

    if (limbIndex == 3) {
        rot->x += thisv->unk_266.y;
    }
    if (limbIndex == 4) {
        rot->x += thisv->unk_260.y;
        rot->z += thisv->unk_260.z;
    }
    return false;
}

static Vec3f sConstVec3f = { 0.2f, 0.2f, 0.2f };

void EnNiwGirl_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gNiwGirlEyeOpenTex, gNiwGirlEyeHalfTex, gNiwGirlEyeClosedTex };
    EnNiwGirl* thisv = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f sp4C = sConstVec3f;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_niw_girl.c", 573);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnNiwGirlOverrideLimbDraw, NULL, thisv);
    func_80033C30(&thisv->actor.world.pos, &sp4C, 255, globalCtx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_niw_girl.c", 592);
}
