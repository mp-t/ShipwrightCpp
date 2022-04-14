/*
 * File: z_en_hs2.c
 * Overlay: ovl_En_Hs2
 * Description: Carpenter's Son (Child Link version)
 */

#include "z_en_hs2.h"
#include "vt.h"
#include "objects/object_hs/object_hs.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnHs2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHs2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHs2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHs2_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_80A6F1A4(EnHs2* thisv, GlobalContext* globalCtx);

const ActorInit En_Hs2_InitVars = {
    ACTOR_EN_HS2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_HS,
    sizeof(EnHs2),
    (ActorFunc)EnHs2_Init,
    (ActorFunc)EnHs2_Destroy,
    (ActorFunc)EnHs2_Update,
    (ActorFunc)EnHs2_Draw,
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
    { 40, 40, 0, { 0, 0, 0 } },
};

void EnHs2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHs2* thisv = (EnHs2*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, thisv->jointTable,
                       thisv->morphTable, 16);
    Animation_PlayLoop(&thisv->skelAnime, &object_hs_Anim_0005C0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
    thisv->actionFunc = func_80A6F1A4;
    thisv->unk_2A8 = 0;
    thisv->actor.targetMode = 6;
}

void EnHs2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHs2* thisv = (EnHs2*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80A6F0B4(EnHs2* thisv, GlobalContext* globalCtx, u16 textId, EnHs2ActionFunc actionFunc) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = actionFunc;
        return 1;
    }

    thisv->actor.textId = textId;
    if (ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) < 0x2151 &&
        thisv->actor.xzDistToPlayer < 100.0f) {
        thisv->unk_2A8 |= 0x1;
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
    return 0;
}

void func_80A6F164(EnHs2* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80A6F1A4;
    }
    thisv->unk_2A8 |= 0x1;
}

void func_80A6F1A4(EnHs2* thisv, GlobalContext* globalCtx) {
    u16 textId;

    textId = Text_GetFaceReaction(globalCtx, 9);
    if (textId == 0) {
        textId = 0x5069;
    }

    func_80A6F0B4(thisv, globalCtx, textId, func_80A6F164);
}

void EnHs2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHs2* thisv = (EnHs2*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        thisv->skelAnime.curFrame = 0.0f;
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->unk_2A8 & 0x1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_29C, &thisv->unk_2A2, thisv->actor.focus.pos);
        thisv->unk_2A8 &= ~1;
    } else {
        Math_SmoothStepToS(&thisv->unk_29C.x, 12800, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.y, 0, 6, 6200, 100);
    }
}

s32 EnHs2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHs2* thisv = (EnHs2*)thisx;

    switch (limbIndex) {
        case 12:
        case 13:
            *dList = NULL;
            return false;
        case 9:
            rot->x += thisv->unk_29C.y;
            rot->z += thisv->unk_29C.x;
            break;
        case 10:
            *dList = NULL;
            return false;
        case 11:
            *dList = NULL;
            return false;
    }
    return false;
}

void EnHs2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A6F4CC = { 300.0f, 1000.0f, 0.0f };
    EnHs2* thisv = (EnHs2*)thisx;

    if (limbIndex == 9) {
        Matrix_MultVec3f(&D_80A6F4CC, &thisv->actor.focus.pos);
    }
}

void EnHs2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHs2* thisv = (EnHs2*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnHs2_OverrideLimbDraw, EnHs2_PostLimbDraw, thisv);
}
