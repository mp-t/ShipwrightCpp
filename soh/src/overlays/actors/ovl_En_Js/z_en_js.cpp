/*
 * File: z_en_js.c
 * Overlay: ovl_En_Js
 * Description: Magic Carpet Salesman
 */

#include "z_en_js.h"
#include "objects/object_js/object_js.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnJs_Init(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Update(Actor* thisx, GlobalContext* globalCtx);
void EnJs_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A89304(EnJs* thisv, GlobalContext* globalCtx);

ActorInit En_Js_InitVars = {
    ACTOR_EN_JS,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_JS,
    sizeof(EnJs),
    (ActorFunc)EnJs_Init,
    (ActorFunc)EnJs_Destroy,
    (ActorFunc)EnJs_Update,
    (ActorFunc)EnJs_Draw,
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

void En_Js_SetupAction(EnJs* thisv, EnJsActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnJs_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* thisv = (EnJs*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gCarpetMerchantSkel, &gCarpetMerchantSlappingKneeAnim,
                       thisv->jointTable, thisv->morphTable, 13);
    Animation_PlayOnce(&thisv->skelAnime, &gCarpetMerchantSlappingKneeAnim);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&thisv->actor, 0.01f);
    En_Js_SetupAction(thisv, func_80A89304);
    thisv->unk_284 = 0;
    thisv->actor.gravity = -1.0f;
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_JSJUTAN, thisv->actor.world.pos.x,
                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
}

void EnJs_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* thisv = (EnJs*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

u8 func_80A88F64(EnJs* thisv, GlobalContext* globalCtx, u16 textId) {
    s16 yawDiff;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        return 1;
    } else {
        thisv->actor.textId = textId;
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x1800 && thisv->actor.xzDistToPlayer < 100.0f) {
            thisv->unk_284 |= 1;
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
        return 0;
    }
}

void func_80A89008(EnJs* thisv) {
    En_Js_SetupAction(thisv, func_80A89304);
    Animation_Change(&thisv->skelAnime, &gCarpetMerchantSlappingKneeAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantSlappingKneeAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89078(EnJs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80A89008(thisv);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    }
}

void func_80A890C0(EnJs* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        En_Js_SetupAction(thisv, func_80A89078);
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 1000.0f);
    }
}

void func_80A8910C(EnJs* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actor.textId = 0x6078;
        En_Js_SetupAction(thisv, func_80A890C0);
        thisv->actor.flags |= ACTOR_FLAG_16;
    }
}

void func_80A89160(EnJs* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        En_Js_SetupAction(thisv, func_80A8910C);
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_BOMBCHUS_10, 10000.0f, 50.0f);
    }
}

void func_80A891C4(EnJs* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < 200) {
                    Message_ContinueTextbox(globalCtx, 0x6075);
                    func_80A89008(thisv);
                } else {
                    Rupees_ChangeBy(-200);
                    En_Js_SetupAction(thisv, func_80A89160);
                }
                break;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x6074);
                func_80A89008(thisv);
        }
    }
}

void func_80A89294(EnJs* thisv) {
    En_Js_SetupAction(thisv, func_80A891C4);
    Animation_Change(&thisv->skelAnime, &gCarpetMerchantIdleAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gCarpetMerchantIdleAnim), ANIMMODE_ONCE, -4.0f);
}

void func_80A89304(EnJs* thisv, GlobalContext* globalCtx) {
    if (func_80A88F64(thisv, globalCtx, 0x6077)) {
        func_80A89294(thisv);
    }
}

void EnJs_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* thisv = (EnJs*)thisx;
    s32 pad;
    s32 pad2;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    if (thisv->actor.bgCheckFlags & 1) {
        if (SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId) == 1) {
            Math_ApproachF(&thisv->actor.shape.yOffset, sREG(80) + -2000.0f, 1.0f, (sREG(81) / 10.0f) + 50.0f);
        }
    } else {
        Math_ApproachZeroF(&thisv->actor.shape.yOffset, 1.0f, (sREG(81) / 10.0f) + 50.0f);
    }
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->skelAnime.curFrame = 0.0f;
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->unk_284 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_278, &thisv->unk_27E, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->unk_278.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->unk_278.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->unk_27E.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&thisv->unk_27E.y, 0, 6, 0x1838, 0x64);
    }
    thisv->unk_284 &= ~0x1;

    if (DECR(thisv->unk_288) == 0) {
        thisv->unk_288 = Rand_S16Offset(0x3C, 0x3C);
    }

    thisv->unk_286 = thisv->unk_288;

    if (thisv->unk_286 >= 3) {
        thisv->unk_286 = 0;
    }
}

s32 EnJs_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnJs* thisv = (EnJs*)thisx;

    if (limbIndex == 12) {
        rot->y -= thisv->unk_278.y;
    }
    return false;
}

void EnJs_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80A896DC = { 0.0f, 0.0f, 0.0f };
    EnJs* thisv = (EnJs*)thisx;

    if (limbIndex == 12) {
        Matrix_MultVec3f(&D_80A896DC, &thisv->actor.focus.pos);
    }
}
void EnJs_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnJs* thisv = (EnJs*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnJs_OverrideLimbDraw, EnJs_PostLimbDraw, thisv);
}
