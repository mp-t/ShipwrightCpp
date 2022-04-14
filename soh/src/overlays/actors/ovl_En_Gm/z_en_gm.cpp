/*
 * File: z_en_gm.c
 * Overlay: ovl_En_Gm
 * Description: Medi-Goron
 */

#include "z_en_gm.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "objects/object_gm/object_gm.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnGm_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGm_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGm_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A3D838(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DFBC(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DB04(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DC44(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DBF4(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DD7C(EnGm* thisv, GlobalContext* globalCtx);
void EnGm_ProcessChoiceIndex(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DF00(EnGm* thisv, GlobalContext* globalCtx);
void func_80A3DF60(EnGm* thisv, GlobalContext* globalCtx);

const ActorInit En_Gm_InitVars = {
    ACTOR_EN_GM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGm),
    (ActorFunc)EnGm_Init,
    (ActorFunc)EnGm_Destroy,
    (ActorFunc)EnGm_Update,
    NULL,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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
    { 100, 120, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnGm_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGm* thisv = (EnGm*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    // "Medi Goron"
    osSyncPrintf(VT_FGCOL(GREEN) "%s[%d] : 中ゴロン[%d]" VT_RST "\n", "../z_en_gm.c", 133, thisv->actor.params);

    thisv->objGmBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_GM);

    if (thisv->objGmBankIndex < 0) {
        osSyncPrintf(VT_COL(RED, WHITE));
        // "There is no model bank! !! (Medi Goron)"
        osSyncPrintf("モデル バンクが無いよ！！（中ゴロン）\n");
        osSyncPrintf(VT_RST);
        ASSERT(0, "0", "../z_en_gm.c", 145);
    }

    thisv->updateFunc = func_80A3D838;
}

void EnGm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGm* thisv = (EnGm*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80A3D7C8(void) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        return 0;
    } else if (!(gBitFlags[2] & gSaveContext.inventory.equipment)) {
        return 1;
    } else if (gBitFlags[3] & gSaveContext.inventory.equipment) {
        return 2;
    } else {
        return 3;
    }
}

void func_80A3D838(EnGm* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objGmBankIndex)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGoronSkel, NULL, thisv->jointTable, thisv->morphTable, 18);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->objGmBankIndex].segment);
        Animation_Change(&thisv->skelAnime, &object_gm_Anim_0002B8, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_gm_Anim_0002B8), ANIMMODE_LOOP, 0.0f);
        thisv->actor.draw = EnGm_Draw;
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
        Actor_SetScale(&thisv->actor, 0.05f);
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        thisv->eyeTexIndex = 0;
        thisv->blinkTimer = 20;
        thisv->actor.textId = 0x3049;
        thisv->updateFunc = func_80A3DFBC;
        thisv->actionFunc = func_80A3DB04;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.gravity = -1.0f;
        thisv->actor.velocity.y = 0.0f;
    }
}

void EnGm_UpdateEye(EnGm* thisv) {
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    } else {
        thisv->eyeTexIndex++;

        if (thisv->eyeTexIndex >= 3) {
            thisv->eyeTexIndex = 0;
            thisv->blinkTimer = Rand_ZeroFloat(60.0f) + 20.0f;
        }
    }
}

void EnGm_SetTextID(EnGm* thisv) {
    switch (func_80A3D7C8()) {
        case 0:
            if (gSaveContext.infTable[11] & 1) {
                thisv->actor.textId = 0x304B;
            } else {
                thisv->actor.textId = 0x304A;
            }
            break;
        case 1:
            if (gSaveContext.infTable[11] & 2) {
                thisv->actor.textId = 0x304F;
            } else {
                thisv->actor.textId = 0x304C;
            }
            break;
        case 2:
            thisv->actor.textId = 0x304E;
            break;
        case 3:
            thisv->actor.textId = 0x304D;
            break;
    }
}

void func_80A3DB04(EnGm* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;
    Player* player = GET_PLAYER(globalCtx);

    dx = thisv->talkPos.x - player->actor.world.pos.x;
    dz = thisv->talkPos.z - player->actor.world.pos.z;

    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        EnGm_SetTextID(thisv);
        thisv->actionFunc = func_80A3DC44;
    } else if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80A3DBF4;
    } else if ((thisv->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        func_8002F2CC(&thisv->actor, globalCtx, 415.0f);
    }
}

void func_80A3DBF4(EnGm* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = func_80A3DB04;
    }
}

void func_80A3DC44(EnGm* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    EnGm_SetTextID(thisv);

    dx = thisv->talkPos.x - player->actor.world.pos.x;
    dz = thisv->talkPos.z - player->actor.world.pos.z;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        switch (func_80A3D7C8()) {
            case 0:
                gSaveContext.infTable[11] |= 1;
            case 3:
                thisv->actionFunc = func_80A3DD7C;
                return;
            case 1:
                gSaveContext.infTable[11] |= 2;
            case 2:
                thisv->actionFunc = EnGm_ProcessChoiceIndex;
            default:
                return;
        }

        thisv->actionFunc = EnGm_ProcessChoiceIndex;
    }
    if ((thisv->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        func_8002F2CC(&thisv->actor, globalCtx, 415.0f);
    }
}

void func_80A3DD7C(EnGm* thisv, GlobalContext* globalCtx) {
    u8 dialogState = Message_GetState(&globalCtx->msgCtx);

    if ((dialogState == TEXT_STATE_DONE || dialogState == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = func_80A3DC44;
        if (dialogState == TEXT_STATE_EVENT) {
            globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            globalCtx->msgCtx.stateTimer = 4;
        }
    }
}

void EnGm_ProcessChoiceIndex(EnGm* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < 200) {
                    Message_ContinueTextbox(globalCtx, 0xC8);
                    thisv->actionFunc = func_80A3DD7C;
                } else {
                    func_8002F434(&thisv->actor, globalCtx, GI_SWORD_KNIFE, 415.0f, 10.0f);
                    thisv->actionFunc = func_80A3DF00;
                }
                break;
            case 1: // no
                Message_ContinueTextbox(globalCtx, 0x3050);
                thisv->actionFunc = func_80A3DD7C;
                break;
        }
    }
}

void func_80A3DF00(EnGm* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_80A3DF60;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_SWORD_KNIFE, 415.0f, 10.0f);
    }
}

void func_80A3DF60(EnGm* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        Rupees_ChangeBy(-200);
        thisv->actionFunc = func_80A3DC44;
    }
}

void func_80A3DFBC(EnGm* thisv, GlobalContext* globalCtx) {
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->objGmBankIndex].segment);
    thisv->timer++;
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
    thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
    thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    EnGm_UpdateEye(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnGm_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGm* thisv = (EnGm*)thisx;

    thisv->updateFunc(thisv, globalCtx);
}

void func_80A3E090(EnGm* thisv) {
    Vec3f vec1;
    Vec3f vec2;

    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 2600.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &vec2);
    thisv->collider.dim.pos.x = vec2.x;
    thisv->collider.dim.pos.y = vec2.y;
    thisv->collider.dim.pos.z = vec2.z;
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 4300.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &thisv->talkPos);
    Matrix_Pop();
    Matrix_Translate(0.0f, 0.0f, 3800.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &thisv->actor.focus.pos);
    thisv->actor.focus.pos.y += 100.0f;
}

void EnGm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    EnGm* thisv = (EnGm*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_gm.c", 613);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_gm.c", 629);

    func_80A3E090(thisv);
}
