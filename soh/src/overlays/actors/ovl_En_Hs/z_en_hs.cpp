/*
 * File: z_en_hs.c
 * Overlay: ovl_En_Hs
 * Description: Carpenter's Son
 */

#include "z_en_hs.h"
#include "vt.h"
#include "objects/object_hs/object_hs.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnHs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHs_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A6E9AC(EnHs* thisv, GlobalContext* globalCtx);
void func_80A6E6B0(EnHs* thisv, GlobalContext* globalCtx);

ActorInit En_Hs_InitVars = {
    ACTOR_EN_HS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_HS,
    sizeof(EnHs),
    (ActorFunc)EnHs_Init,
    (ActorFunc)EnHs_Destroy,
    (ActorFunc)EnHs_Update,
    (ActorFunc)EnHs_Draw,
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

void func_80A6E3A0(EnHs* thisv, EnHsActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnHs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHs* thisv = (EnHs*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, thisv->jointTable,
                       thisv->morphTable, 16);
    Animation_PlayLoop(&thisv->skelAnime, &object_hs_Anim_0005C0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);

    if (!LINK_IS_ADULT) {
        thisv->actor.params = 0;
    } else {
        thisv->actor.params = 1;
    }

    if (thisv->actor.params == 1) {
        // "chicken shop (adult era)"
        osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(大人の時) \n" VT_RST);
        func_80A6E3A0(thisv, func_80A6E9AC);
        if (gSaveContext.itemGetInf[3] & 1) {
            // "chicken shop closed"
            osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコ屋閉店 \n" VT_RST);
            Actor_Kill(&thisv->actor);
        }
    } else {
        // "chicken shop (child era)"
        osSyncPrintf(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
        func_80A6E3A0(thisv, func_80A6E9AC);
    }

    thisv->unk_2A8 = 0;
    thisv->actor.targetMode = 6;
}

void EnHs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHs* thisv = (EnHs*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80A6E53C(EnHs* thisv, GlobalContext* globalCtx, u16 textId, EnHsActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        func_80A6E3A0(thisv, actionFunc);
        return 1;
    }

    thisv->actor.textId = textId;
    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if ((ABS(yawDiff) <= 0x2150) && (thisv->actor.xzDistToPlayer < 100.0f)) {
        thisv->unk_2A8 |= 1;
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }

    return 0;
}

void func_80A6E5EC(EnHs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80A6E3A0(thisv, func_80A6E6B0);
    }

    thisv->unk_2A8 |= 1;
}

void func_80A6E630(EnHs* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        func_80088AA0(180);
        func_80A6E3A0(thisv, func_80A6E6B0);
        gSaveContext.eventInf[1] &= ~1;
    }

    thisv->unk_2A8 |= 1;
}

void func_80A6E6B0(EnHs* thisv, GlobalContext* globalCtx) {
    func_80A6E53C(thisv, globalCtx, 0x10B6, func_80A6E5EC);
}

void func_80A6E6D8(EnHs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80A6E3A0(thisv, func_80A6E9AC);
    }
}

void func_80A6E70C(EnHs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80A6E3A0(thisv, func_80A6E9AC);
    }
}

void func_80A6E740(EnHs* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        func_80A6E3A0(thisv, func_80A6E630);
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_ODD_MUSHROOM, 10000.0f, 50.0f);
    }

    thisv->unk_2A8 |= 1;
}

void func_80A6E7BC(EnHs* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                func_80A6E3A0(thisv, func_80A6E740);
                func_8002F434(&thisv->actor, globalCtx, GI_ODD_MUSHROOM, 10000.0f, 50.0f);
                break;
            case 1:
                Message_ContinueTextbox(globalCtx, 0x10B4);
                func_80A6E3A0(thisv, func_80A6E70C);
                break;
        }

        Animation_Change(&thisv->skelAnime, &object_hs_Anim_0005C0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_hs_Anim_0005C0), ANIMMODE_LOOP, 8.0f);
    }

    thisv->unk_2A8 |= 1;
}

void func_80A6E8CC(EnHs* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x10B3);
        func_80A6E3A0(thisv, func_80A6E7BC);
        Animation_Change(&thisv->skelAnime, &object_hs_Anim_000528, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_hs_Anim_000528), ANIMMODE_LOOP, 8.0f);
    }

    if (thisv->unk_2AA > 0) {
        thisv->unk_2AA--;
        if (thisv->unk_2AA == 0) {
            func_8002F7DC(&player->actor, NA_SE_EV_CHICKEN_CRY_M);
        }
    }

    thisv->unk_2A8 |= 1;
}

void func_80A6E9AC(EnHs* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (func_8002F368(globalCtx) == 7) {
            player->actor.textId = 0x10B2;
            func_80A6E3A0(thisv, func_80A6E8CC);
            Animation_Change(&thisv->skelAnime, &object_hs_Anim_000304, 1.0f, 0.0f,
                             Animation_GetLastFrame(&object_hs_Anim_000304), ANIMMODE_LOOP, 8.0f);
            thisv->unk_2AA = 40;
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            player->actor.textId = 0x10B1;
            func_80A6E3A0(thisv, func_80A6E6D8);
        }
    } else {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        thisv->actor.textId = 0x10B1;
        if ((ABS(yawDiff) <= 0x2150) && (thisv->actor.xzDistToPlayer < 100.0f)) {
            func_8002F298(&thisv->actor, globalCtx, 100.0f, 7);
        }
    }
}

void EnHs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHs* thisv = (EnHs*)thisx;
    s32 pad;

    Collider_UpdateCylinder(thisx, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->skelAnime.curFrame = 0.0f;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->unk_2A8 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_29C, &thisv->unk_2A2, thisv->actor.focus.pos);
        thisv->unk_2A8 &= ~1;
    } else {
        Math_SmoothStepToS(&thisv->unk_29C.x, 12800, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.y, 0, 6, 6200, 100);
    }
}

s32 EnHs_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHs* thisv = (EnHs*)thisx;

    switch (limbIndex) {
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
        case 12:
            if (thisv->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
        case 13:
            if (thisv->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
    }
    return false;
}

void EnHs_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A6EDFC = { 300.0f, 1000.0f, 0.0f };
    EnHs* thisv = (EnHs*)thisx;

    if (limbIndex == 9) {
        Matrix_MultVec3f(&D_80A6EDFC, &thisv->actor.focus.pos);
    }
}

void EnHs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHs* thisv = (EnHs*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnHs_OverrideLimbDraw, EnHs_PostLimbDraw, thisv);
}
