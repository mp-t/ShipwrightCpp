/*
 * File: z_en_sth.c
 * Overlay: ovl_En_Sth
 * Description: Uncursed House of Skulltula People
 */

#include "vt.h"
#include "z_en_sth.h"
#include "objects/object_ahg/object_ahg.h"
#include "objects/object_boj/object_boj.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnSth_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Update2(Actor* thisx, GlobalContext* globalCtx);
void EnSth_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSth_WaitForObjectLoaded(EnSth* thisv, GlobalContext* globalCtx);
void EnSth_ParentRewardObtainedWait(EnSth* thisv, GlobalContext* globalCtx);
void EnSth_RewardUnobtainedWait(EnSth* thisv, GlobalContext* globalCtx);
void EnSth_ChildRewardObtainedWait(EnSth* thisv, GlobalContext* globalCtx);

ActorInit En_Sth_InitVars = {
    ACTOR_EN_STH,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnSth),
    (ActorFunc)EnSth_Init,
    (ActorFunc)EnSth_Destroy,
    (ActorFunc)EnSth_Update,
    NULL,
    NULL,
};

#include "overlays/ovl_En_Sth/ovl_En_Sth.h"

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

static s16 sObjectIds[6] = {
    OBJECT_AHG, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ, OBJECT_BOJ,
};

static const FlexSkeletonHeader* sSkeletons[6] = {
    &object_ahg_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
    &object_boj_Skel_0000F0,
};

static const AnimationHeader* sAnimations[6] = {
    &sParentDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim, &sChildDanceAnim,
};

static EnSthActionFunc sRewardObtainedWaitActions[6] = {
    EnSth_ParentRewardObtainedWait, EnSth_ChildRewardObtainedWait, EnSth_ChildRewardObtainedWait,
    EnSth_ChildRewardObtainedWait,  EnSth_ChildRewardObtainedWait, EnSth_ChildRewardObtainedWait,
};

static u16 sEventFlags[6] = {
    0x0000, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000,
};

static s16 sGetItemIds[6] = {
    GI_RUPEE_GOLD, GI_WALLET_ADULT, GI_STONE_OF_AGONY, GI_WALLET_GIANT, GI_BOMBCHUS_10, GI_HEART_PIECE,
};

static Vec3f D_80B0B49C = { 700.0f, 400.0f, 0.0f };

static Color_RGB8 sTunicColors[6] = {
    { 190, 110, 0 }, { 0, 180, 110 }, { 0, 255, 80 }, { 255, 160, 60 }, { 190, 230, 250 }, { 240, 230, 120 },
};

void EnSth_SetupAction(EnSth* thisv, EnSthActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnSth_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* thisv = (EnSth*)thisx;

    s16 objectId;
    s32 params = thisv->actor.params;
    s32 objectBankIdx;

    osSyncPrintf(VT_FGCOL(BLUE) "金スタル屋 no = %d\n" VT_RST, params); // "Gold Skulltula Shop"
    if (thisv->actor.params == 0) {
        if (gSaveContext.inventory.gsTokens < 100) {
            Actor_Kill(&thisv->actor);
            // "Gold Skulltula Shop I still can't be a human"
            osSyncPrintf("金スタル屋 まだ 人間に戻れない \n");
            return;
        }
    } else if (gSaveContext.inventory.gsTokens < (thisv->actor.params * 10)) {
        Actor_Kill(&thisv->actor);
        // "Gold Skulltula Shop I still can't be a human"
        osSyncPrintf(VT_FGCOL(BLUE) "金スタル屋 まだ 人間に戻れない \n" VT_RST);
        return;
    }

    objectId = sObjectIds[params];
    if (objectId != 1) {
        objectBankIdx = Object_GetIndex(&globalCtx->objectCtx, objectId);
    } else {
        objectBankIdx = 0;
    }

    osSyncPrintf("bank_ID = %d\n", objectBankIdx);
    if (objectBankIdx < 0) {
        ASSERT(0, "0", "../z_en_sth.c", 1564);
    }
    thisv->objectBankIdx = objectBankIdx;
    thisv->drawFunc = EnSth_Draw;
    Actor_SetScale(&thisv->actor, 0.01f);
    EnSth_SetupAction(thisv, EnSth_WaitForObjectLoaded);
    thisv->actor.draw = NULL;
    thisv->unk_2B2 = 0;
    thisv->actor.targetMode = 6;
}

void EnSth_SetupShapeColliderUpdate2AndDraw(EnSth* thisv, GlobalContext* globalCtx) {
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.update = EnSth_Update2;
    thisv->actor.draw = thisv->drawFunc;
}

void EnSth_SetupAfterObjectLoaded(EnSth* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16* params;

    EnSth_SetupShapeColliderUpdate2AndDraw(thisv, globalCtx);
    gSegments[6] = reinterpret_cast<std::uintptr_t>(PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objectBankIdx].segment));
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, sSkeletons[thisv->actor.params], NULL, thisv->jointTable,
                       thisv->morphTable, 16);
    Animation_PlayLoop(&thisv->skelAnime, sAnimations[thisv->actor.params]);

    thisv->eventFlag = sEventFlags[thisv->actor.params];
    params = &thisv->actor.params;
    if (gSaveContext.eventChkInf[13] & thisv->eventFlag) {
        EnSth_SetupAction(thisv, sRewardObtainedWaitActions[*params]);
    } else {
        EnSth_SetupAction(thisv, EnSth_RewardUnobtainedWait);
    }
}

void EnSth_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* thisv = (EnSth*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnSth_WaitForObjectLoaded(EnSth* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objectBankIdx)) {
        thisv->actor.objBankIndex = thisv->objectBankIdx;
        thisv->actionFunc = EnSth_SetupAfterObjectLoaded;
    }
}

void EnSth_FacePlayer(EnSth* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 diffRot = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if (ABS(diffRot) <= 0x4000) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 6, 0xFA0, 0x64);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2AC, thisv->actor.focus.pos);
    } else {
        if (diffRot < 0) {
            Math_SmoothStepToS(&thisv->headRot.y, -0x2000, 6, 0x1838, 0x100);
        } else {
            Math_SmoothStepToS(&thisv->headRot.y, 0x2000, 6, 0x1838, 0x100);
        }
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xC, 0x3E8, 0x64);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
}

void EnSth_LookAtPlayer(EnSth* thisv, GlobalContext* globalCtx) {
    s16 diffRot = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

    if ((ABS(diffRot) <= 0x4300) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        func_80038290(globalCtx, &thisv->actor, &thisv->headRot, &thisv->unk_2AC, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->unk_2AC.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->unk_2AC.y, 0, 6, 0x1838, 0x64);
    }
}

void EnSth_RewardObtainedTalk(EnSth* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        if (thisv->actor.params == 0) {
            EnSth_SetupAction(thisv, EnSth_ParentRewardObtainedWait);
        } else {
            EnSth_SetupAction(thisv, EnSth_ChildRewardObtainedWait);
        }
    }
    EnSth_FacePlayer(thisv, globalCtx);
}

void EnSth_ParentRewardObtainedWait(EnSth* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        EnSth_SetupAction(thisv, EnSth_RewardObtainedTalk);
    } else {
        thisv->actor.textId = 0x23;
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(thisv, globalCtx);
}

void EnSth_GivePlayerItem(EnSth* thisv, GlobalContext* globalCtx) {
    u16 getItemId = sGetItemIds[thisv->actor.params];

    switch (thisv->actor.params) {
        case 1:
        case 3:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    getItemId = GI_WALLET_ADULT;
                    break;

                case 1:
                    getItemId = GI_WALLET_GIANT;
                    break;
            }
            break;
    }

    func_8002F434(&thisv->actor, globalCtx, getItemId, 10000.0f, 50.0f);
}

void EnSth_GiveReward(EnSth* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        EnSth_SetupAction(thisv, EnSth_RewardObtainedTalk);
        gSaveContext.eventChkInf[13] |= thisv->eventFlag;
    } else {
        EnSth_GivePlayerItem(thisv, globalCtx);
    }
    EnSth_FacePlayer(thisv, globalCtx);
}

void EnSth_RewardUnobtainedTalk(EnSth* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        EnSth_SetupAction(thisv, EnSth_GiveReward);
        EnSth_GivePlayerItem(thisv, globalCtx);
    }
    EnSth_FacePlayer(thisv, globalCtx);
}

void EnSth_RewardUnobtainedWait(EnSth* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        EnSth_SetupAction(thisv, EnSth_RewardUnobtainedTalk);
    } else {
        if (thisv->actor.params == 0) {
            thisv->actor.textId = 0x28;
        } else {
            thisv->actor.textId = 0x21;
        }
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(thisv, globalCtx);
}

void EnSth_ChildRewardObtainedWait(EnSth* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        EnSth_SetupAction(thisv, EnSth_RewardObtainedTalk);
    } else {
        if (gSaveContext.inventory.gsTokens < 50) {
            thisv->actor.textId = 0x20;
        } else {
            thisv->actor.textId = 0x1F;
        }
        if (thisv->actor.xzDistToPlayer < 100.0f) {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
    EnSth_LookAtPlayer(thisv, globalCtx);
}

void EnSth_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* thisv = (EnSth*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void EnSth_Update2(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* thisv = (EnSth*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->skelAnime.curFrame = 0.0f;
    }
    thisv->actionFunc(thisv, globalCtx);

    // Likely an unused blink timer and eye index
    if (DECR(thisv->unk_2B6) == 0) {
        thisv->unk_2B6 = Rand_S16Offset(0x3C, 0x3C);
    }
    thisv->unk_2B4 = thisv->unk_2B6;
    if (thisv->unk_2B4 >= 3) {
        thisv->unk_2B4 = 0;
    }
}

s32 EnSth_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSth* thisv = (EnSth*)thisx;

    s32 temp_v1;

    if (limbIndex == 15) {
        rot->x += thisv->headRot.y;
        rot->z += thisv->headRot.x;
        *dList = D_80B0A050;
    }

    if (thisv->unk_2B2 & 2) {
        thisv->unk_2B2 &= ~2;
        return 0;
    }

    if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
        temp_v1 = limbIndex * 0x32;
        rot->y += (Math_SinS(globalCtx->state.frames * (temp_v1 + 0x814)) * 200.0f);
        rot->z += (Math_CosS(globalCtx->state.frames * (temp_v1 + 0x940)) * 200.0f);
    }
    return 0;
}

void EnSth_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnSth* thisv = (EnSth*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80B0B49C, &thisv->actor.focus.pos);
        if (thisv->actor.params != 0) { // Children
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sth.c", 2079);

            gSPDisplayList(POLY_OPA_DISP++, D_80B0A3C0);

            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sth.c", 2081);
        }
    }
}

Gfx* EnSth_AllocColorDList(GraphicsContext* globalCtx, u8 envR, u8 envG, u8 envB, u8 envA) {
    Gfx* dList;

    dList = static_cast<Gfx*>(Graph_Alloc(globalCtx, 2 * sizeof(Gfx)));
    gDPSetEnvColor(dList, envR, envG, envB, envA);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void EnSth_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSth* thisv = (EnSth*)thisx;
    Color_RGB8* envColor1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sth.c", 2133);

    gSegments[6] = reinterpret_cast<std::uintptr_t>(PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objectBankIdx].segment));
    func_800943C8(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               EnSth_AllocColorDList(globalCtx->state.gfxCtx, sTunicColors[thisv->actor.params].r,
                                     sTunicColors[thisv->actor.params].g, sTunicColors[thisv->actor.params].b, 255));

    if (thisv->actor.params == 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, EnSth_AllocColorDList(globalCtx->state.gfxCtx, 190, 110, 0, 255));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09, EnSth_AllocColorDList(globalCtx->state.gfxCtx, 90, 110, 130, 255));
    }
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnSth_OverrideLimbDraw, EnSth_PostLimbDraw, &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sth.c", 2176);
}
