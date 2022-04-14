/*
 * File: z_en_ani.c
 * Overlay: ovl_En_Ani
 * Description: Kakariko Roof Guy
 */

#include "z_en_ani.h"
#include "objects/object_ani/object_ani.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnAni_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAni_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAni_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAni_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 EnAni_SetText(EnAni* thisv, GlobalContext* globalCtx, u16 textId);
void func_809B04F0(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0524(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0558(EnAni* thisv, GlobalContext* globalCtx);
void func_809B05F0(EnAni* thisv, GlobalContext* globalCtx);
void func_809B064C(EnAni* thisv, GlobalContext* globalCtx);
void func_809B07F8(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0988(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0994(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0A28(EnAni* thisv, GlobalContext* globalCtx);
void func_809B0A6C(EnAni* thisv, GlobalContext* globalCtx);

const ActorInit En_Ani_InitVars = {
    ACTOR_EN_ANI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ANI,
    sizeof(EnAni),
    (ActorFunc)EnAni_Init,
    (ActorFunc)EnAni_Destroy,
    (ActorFunc)EnAni_Update,
    (ActorFunc)EnAni_Draw,
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
    { 30, 40, 0, { 0 } },
};

void EnAni_SetupAction(EnAni* thisv, EnAniActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 850, ICHAIN_STOP),
};

void EnAni_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnAni* thisv = (EnAni*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, -2800.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gRoofManSkel, &gRoofManIdleAnim, thisv->jointTable,
                       thisv->morphTable, 0x10);
    Animation_PlayOnce(&thisv->skelAnime, &gRoofManIdleAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    if (!LINK_IS_ADULT) {
        EnAni_SetupAction(thisv, func_809B064C);
    } else {
        EnAni_SetupAction(thisv, func_809B07F8);
    }
    thisv->unk_2AA = 0;
    thisv->unk_2A8 = 0;
    thisv->actor.minVelocityY = -1.0f;
    thisv->actor.velocity.y = -1.0f;
}

void EnAni_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAni* thisv = (EnAni*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnAni_SetText(EnAni* thisv, GlobalContext* globalCtx, u16 textId) {
    thisv->actor.textId = textId;
    thisv->unk_2A8 |= 1;
    func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    return 0;
}

void func_809B04F0(EnAni* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        EnAni_SetupAction(thisv, func_809B064C);
    }
}

void func_809B0524(EnAni* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        EnAni_SetupAction(thisv, func_809B07F8);
    }
}

void func_809B0558(EnAni* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        if (!LINK_IS_ADULT) {
            EnAni_SetupAction(thisv, func_809B04F0);
        } else {
            EnAni_SetupAction(thisv, func_809B0524);
        }
        gSaveContext.itemGetInf[1] |= 0x20;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, 10000.0f, 200.0f);
    }
}

void func_809B05F0(EnAni* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        EnAni_SetupAction(thisv, func_809B0558);
    }
    func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, 10000.0f, 200.0f);
}

void func_809B064C(EnAni* thisv, GlobalContext* globalCtx) {
    u16 textId;
    s16 yawDiff;
    u16 textId2;

    textId2 = Text_GetFaceReaction(globalCtx, 0xA);
    textId = textId2 & 0xFFFF;

    if (!textId) {}

    if (textId2 == 0) {
        textId = !IS_DAY ? 0x5051 : 0x5050;
    }

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->actor.textId == 0x5056) {
            EnAni_SetupAction(thisv, func_809B04F0);
        } else if (thisv->actor.textId == 0x5055) {
            EnAni_SetupAction(thisv, func_809B05F0);
        } else {
            EnAni_SetupAction(thisv, func_809B04F0);
        }
    } else if (yawDiff >= -0x36AF && yawDiff < 0 && thisv->actor.xzDistToPlayer < 150.0f &&
               -80.0f < thisv->actor.yDistToPlayer) {
        if (gSaveContext.itemGetInf[1] & 0x20) {
            EnAni_SetText(thisv, globalCtx, 0x5056);
        } else {
            EnAni_SetText(thisv, globalCtx, 0x5055);
        }
    } else if (yawDiff >= -0x3E7 && yawDiff < 0x36B0 && thisv->actor.xzDistToPlayer < 350.0f) {
        EnAni_SetText(thisv, globalCtx, textId);
    }
}

void func_809B07F8(EnAni* thisv, GlobalContext* globalCtx) {
    s16 pad;
    s16 yawDiff;
    u16 textId;

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->actor.textId == 0x5056) {
            EnAni_SetupAction(thisv, func_809B0524);
        } else if (thisv->actor.textId == 0x5055) {
            EnAni_SetupAction(thisv, func_809B05F0);
        } else {
            EnAni_SetupAction(thisv, func_809B0524);
        }
    } else if (yawDiff > -0x36B0 && yawDiff < 0 && thisv->actor.xzDistToPlayer < 150.0f &&
               -80.0f < thisv->actor.yDistToPlayer) {
        if (gSaveContext.itemGetInf[1] & 0x20) {
            EnAni_SetText(thisv, globalCtx, 0x5056);
        } else {
            EnAni_SetText(thisv, globalCtx, 0x5055);
        }
    } else if (yawDiff > -0x3E8 && yawDiff < 0x36B0 && thisv->actor.xzDistToPlayer < 350.0f) {
        if (!(gSaveContext.eventChkInf[2] & 0x8000)) {
            textId = 0x5052;
        } else {
            textId = (gSaveContext.itemGetInf[1] & 0x20) ? 0x5054 : 0x5053;
        }
        EnAni_SetText(thisv, globalCtx, textId);
    }
}

void func_809B0988(EnAni* thisv, GlobalContext* globalCtx) {
}

void func_809B0994(EnAni* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.npcActions[0]->action == 4) {
        Animation_Change(&thisv->skelAnime, &gRoofManGettingUpAfterKnockbackAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRoofManGettingUpAfterKnockbackAnim), ANIMMODE_ONCE, -4.0f);
        thisv->unk_2AA++;
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    }
}

void func_809B0A28(EnAni* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_2AA++;
    }
}

void func_809B0A6C(EnAni* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->skelAnime.curFrame = 0.0f;
    }
    if (globalCtx->csCtx.npcActions[0]->action == 2) {
        Animation_Change(&thisv->skelAnime, &gRoofManKnockbackAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRoofManKnockbackAnim), ANIMMODE_ONCE, 0.0f);
        thisv->actor.shape.shadowDraw = NULL;
        thisv->unk_2AA++;
    }
}

void EnAni_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnAni* thisv = (EnAni*)thisx;
    s32 pad[2];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[0] != NULL)) {
        switch (thisv->unk_2AA) {
            case 0:
                func_809B0A6C(thisv, globalCtx);
                break;
            case 1:
                func_809B0A28(thisv, globalCtx);
                break;
            case 2:
                func_809B0994(thisv, globalCtx);
                break;
            case 3:
                func_809B0A28(thisv, globalCtx);
                break;
            case 4:
                func_809B0988(thisv, globalCtx);
                break;
        }

        if (globalCtx->csCtx.frames == 100) {
            func_800788CC(NA_SE_IT_EARTHQUAKE);
        }
    } else {
        if (SkelAnime_Update(&thisv->skelAnime) != 0) {
            thisv->skelAnime.curFrame = 0.0f;
        }
        thisv->actionFunc(thisv, globalCtx);
    }

    if (thisv->unk_2A8 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_29C, &thisv->unk_2A2, thisv->actor.focus.pos);
        thisv->unk_2A2.z = 0;
        thisv->unk_2A2.y = thisv->unk_2A2.z;
        thisv->unk_2A2.x = thisv->unk_2A2.z;
    } else {
        Math_SmoothStepToS(&thisv->unk_29C.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_29C.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2A2.y, 0, 6, 6200, 100);
    }

    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }
    thisv->eyeIndex = thisv->blinkTimer;
    if (thisv->eyeIndex >= 3) {
        thisv->eyeIndex = 0;
    }
}

s32 EnAni_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnAni* thisv = (EnAni*)thisx;

    if (limbIndex == 15) {
        rot->x += thisv->unk_29C.y;
        rot->z += thisv->unk_29C.x;
    }
    return false;
}

void EnAni_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f sMultVec = { 800.0f, 500.0f, 0.0f };
    EnAni* thisv = (EnAni*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&sMultVec, &thisv->actor.focus.pos);
    }
}

void EnAni_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gRoofManEyeOpenTex,
        gRoofManEyeHalfTex,
        gRoofManEyeClosedTex,
    };
    EnAni* thisv = (EnAni*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ani.c", 719);

    func_800943C8(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnAni_OverrideLimbDraw, EnAni_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ani.c", 736);
}
