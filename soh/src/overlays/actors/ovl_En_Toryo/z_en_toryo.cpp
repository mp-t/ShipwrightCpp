/*
 * File: z_en_tory.c
 * Overlay: ovl_En_Toryo
 * Description: Boss Carpenter
 */

#include "z_en_toryo.h"
#include "objects/object_toryo/object_toryo.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnToryo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Update(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B20914(EnToryo* thisv, GlobalContext* globalCtx);
s32 EnToryo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnToryo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx);

ActorInit En_Toryo_InitVars = {
    ACTOR_EN_TORYO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TORYO,
    sizeof(EnToryo),
    (ActorFunc)EnToryo_Init,
    (ActorFunc)EnToryo_Destroy,
    (ActorFunc)EnToryo_Update,
    (ActorFunc)EnToryo_Draw,
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
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static AnimationSpeedInfo sEnToryoAnimation = { &object_toryo_Anim_000E50, 1.0f, 0, 0 };

static Vec3f sMultVec = { 800.0f, 1000.0f, 0.0f };

void EnToryo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* thisv = (EnToryo*)thisx;
    s32 pad;

    switch (globalCtx->sceneNum) {
        case SCENE_SPOT09:
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                thisv->stateFlags |= 1;
            }
            break;
        case SCENE_SPOT01:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_DAY) {
                thisv->stateFlags |= 2;
            }
            break;
        case SCENE_KAKARIKO:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_NIGHT) {
                thisv->stateFlags |= 4;
            }
            break;
    }

    if ((thisv->stateFlags & 7) == 0) {
        Actor_Kill(&thisv->actor);
    }

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 42.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_toryo_Skel_007150, NULL, thisv->jointTable, thisv->morphTable,
                       17);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    Animation_Change(&thisv->skelAnime, sEnToryoAnimation.animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sEnToryoAnimation.animation), sEnToryoAnimation.mode,
                     sEnToryoAnimation.morphFrames);
    thisv->stateFlags |= 8;
    thisv->actor.targetMode = 6;
    thisv->actionFunc = func_80B20914;
}

void EnToryo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* thisv = (EnToryo*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80B203D8(EnToryo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_EVENT:
            ret = 1;
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                if (globalCtx->msgCtx.choiceIndex == 0) {
                    Message_CloseTextbox(globalCtx);
                    thisv->actor.parent = NULL;
                    player->exchangeItemId = EXCH_ITEM_NONE;
                    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                    thisv->actor.textId = 0x601B;
                    ret = 3;
                } else {
                    thisv->actor.textId = 0x606F;
                    ret = 2;
                }
            }
            break;
        case TEXT_STATE_DONE:
            switch (thisv->actor.textId) {
                case 0x5028:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 4;
                        ret = 0;
                    }
                    break;
                case 0x601B:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        ret = 4;
                    }
                    break;
                case 0x606F:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 2;
                        ret = 0;
                    }
                    break;
                case 0x606A:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 1;
                        ret = 0;
                    }
                    break;
                case 0x606B:
                case 0x606C:
                case 0x606D:
                case 0x606E:
                default:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        ret = 0;
                    }
                    break;
            }
            break;
    }
    return ret;
}

s32 func_80B205CC(EnToryo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = 5;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            ret = 5;
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                ret = 0;
            }
            break;
    }
    return ret;
}

u32 func_80B20634(EnToryo* thisv, GlobalContext* globalCtx) {
    u32 ret;

    if (thisv->unk_1E0 != 0) {
        if (thisv->unk_1E0 == 10) {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            if (gSaveContext.infTable[23] & 2) {
                ret = 0x606E;
            } else {
                ret = 0x606D;
            }
        } else {
            ret = 0x200F;
        }
    }
    //! @bug return value may be unitialized
    return ret;
}

s32 func_80B206A0(EnToryo* thisv, GlobalContext* globalCtx) {
    s32 textId = Text_GetFaceReaction(globalCtx, 0);
    s32 ret = textId;

    if (textId == 0) {
        if ((thisv->stateFlags & 1)) {
            if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
                ret = 0x606C;
            } else if ((gSaveContext.infTable[23] & 1)) {
                ret = 0x606B;
            } else {
                ret = 0x606A;
            }
        } else if ((thisv->stateFlags & 2)) {
            if ((gSaveContext.infTable[23] & 4)) {
                ret = 0x5029;
            } else {
                ret = 0x5028;
            }
        } else {
            ret = textId;
            if ((thisv->stateFlags & 4)) {
                ret = 0x506C;
            }
        }
    }
    return ret;
}

void func_80B20768(EnToryo* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 sp32;
    s16 sp30;

    if (thisv->unk_1E4 == 3) {
        Actor_ProcessTalkRequest(&thisv->actor, globalCtx);
        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        thisv->unk_1E4 = 1;
    }

    if (thisv->unk_1E4 == 1) {
        thisv->unk_1E4 = func_80B203D8(thisv, globalCtx);
    }

    if (thisv->unk_1E4 == 5) {
        thisv->unk_1E4 = func_80B205CC(thisv, globalCtx);
        return;
    }

    if (thisv->unk_1E4 == 2) {
        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        thisv->unk_1E4 = 1;
    }

    if (thisv->unk_1E4 == 4) {
        if (Actor_HasParent(&thisv->actor, globalCtx)) {
            thisv->actor.parent = NULL;
            thisv->unk_1E4 = 5;
        } else {
            func_8002F434(&thisv->actor, globalCtx, GI_SWORD_BROKEN, 100.0f, 10.0f);
        }
        return;
    }

    if (thisv->unk_1E4 == 0) {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            thisv->unk_1E0 = func_8002F368(globalCtx);
            if (thisv->unk_1E0 != 0) {
                player->actor.textId = func_80B20634(thisv, globalCtx);
                thisv->actor.textId = player->actor.textId;
            }
            thisv->unk_1E4 = 1;
            return;
        }

        Actor_GetScreenPos(globalCtx, &thisv->actor, &sp32, &sp30);
        if ((sp32 >= 0) && (sp32 < 0x141) && (sp30 >= 0) && (sp30 < 0xF1)) {
            thisv->actor.textId = func_80B206A0(thisv, globalCtx);
            func_8002F298(&thisv->actor, globalCtx, 100.0f, 10);
        }
    }
}

void func_80B20914(EnToryo* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    func_80B20768(thisv, globalCtx);
    if (thisv->unk_1E4 != 0) {
        thisv->stateFlags |= 0x10;
    } else {
        thisv->stateFlags &= ~0x10;
    }
}

void EnToryo_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* thisv = (EnToryo*)thisx;
    ColliderCylinder* collider = &thisv->collider;
    Player* player = GET_PLAYER(globalCtx);
    f32 rot;

    Collider_UpdateCylinder(thisx, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, (Collider*)collider);

    thisv->actionFunc(thisv, globalCtx);

    if ((thisv->stateFlags & 8)) {
        thisv->unk_1EC.unk_18.x = player->actor.focus.pos.x;
        thisv->unk_1EC.unk_18.y = player->actor.focus.pos.y;
        thisv->unk_1EC.unk_18.z = player->actor.focus.pos.z;

        if ((thisv->stateFlags & 0x10)) {
            func_80034A14(thisx, &thisv->unk_1EC, 0, 4);
            return;
        }

        rot = thisx->yawTowardsPlayer - thisx->shape.rot.y;
        if ((rot < 14563.0f) && (rot > -14563.0f)) {
            func_80034A14(thisx, &thisv->unk_1EC, 0, 2);
        } else {
            func_80034A14(thisx, &thisv->unk_1EC, 0, 1);
        }
    }
}

void EnToryo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* thisv = (EnToryo*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnToryo_OverrideLimbDraw, EnToryo_PostLimbDraw, thisv);
}

s32 EnToryo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnToryo* thisv = (EnToryo*)thisx;

    if ((thisv->stateFlags & 8)) {
        switch (limbIndex) {
            case 8:
                rot->x += thisv->unk_1EC.unk_0E.y;
                rot->y -= thisv->unk_1EC.unk_0E.x;
                break;
            case 15:
                rot->x += thisv->unk_1EC.unk_08.y;
                rot->z += thisv->unk_1EC.unk_08.x;
                break;
        }
    }
    return 0;
}

void EnToryo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnToryo* thisv = (EnToryo*)thisx;

    switch (limbIndex) {
        case 15:
            Matrix_MultVec3f(&sMultVec, &thisv->actor.focus.pos);
            break;
    }
}
