/*
 * File: z_en_mk.c
 * Overlay: ovl_En_Mk
 * Description: Lakeside Professor
 */

#include "z_en_mk.h"
#include "objects/object_mk/object_mk.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnMk_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMk_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMk_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMk_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnMk_Wait(EnMk* thisv, GlobalContext* globalCtx);

const ActorInit En_Mk_InitVars = {
    ACTOR_EN_MK,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MK,
    sizeof(EnMk),
    (ActorFunc)EnMk_Init,
    (ActorFunc)EnMk_Destroy,
    (ActorFunc)EnMk_Update,
    (ActorFunc)EnMk_Draw,
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

void EnMk_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMk* thisv = (EnMk*)thisx;
    s32 swimFlag;

    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.gravity = -1.0f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_mk_Skel_005DF0, &object_mk_Anim_000D88, thisv->jointTable,
                       thisv->morphTable, 13);
    Animation_PlayLoop(&thisv->skelAnime, &object_mk_Anim_000D88);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.colChkInfo.mass = 0xFF;
    Actor_SetScale(&thisv->actor, 0.01f);

    thisv->actionFunc = EnMk_Wait;
    thisv->flags = 0;
    thisv->swimFlag = 0;
    thisv->actor.targetMode = 6;

    if (gSaveContext.itemGetInf[1] & 1) {
        thisv->flags |= 4;
    }
}

void EnMk_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMk* thisv = (EnMk*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80AACA40(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = EnMk_Wait;
    }

    thisv->flags |= 1;
}

void func_80AACA94(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx) != 0) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_80AACA40;
        func_80088AA0(240);
        gSaveContext.eventInf[1] &= ~1;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_EYEDROPS, 10000.0f, 50.0f);
    }
}

void func_80AACB14(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80AACA94;
        func_8002F434(&thisv->actor, globalCtx, GI_EYEDROPS, 10000.0f, 50.0f);
    }
}

void func_80AACB6C(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80AACB14;
    }

    thisv->flags |= 1;
}

void func_80AACBAC(EnMk* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
        thisv->actor.shape.rot.y -= 0x800;
    } else {
        thisv->actionFunc = func_80AACB6C;
        Message_ContinueTextbox(globalCtx, 0x4030);
    }
}

void func_80AACC04(EnMk* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->timer = 16;
        thisv->actionFunc = func_80AACBAC;
        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        thisv->flags &= ~2;
    }
}

void func_80AACCA0(EnMk* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
        thisv->actor.shape.rot.y += 0x800;
    } else {
        thisv->timer = 120;
        thisv->actionFunc = func_80AACC04;
        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000724, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000724), ANIMMODE_LOOP, -4.0f);
        thisv->flags &= ~2;
    }
}

void func_80AACD48(EnMk* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = func_80AACCA0;
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        player->exchangeItemId = EXCH_ITEM_NONE;
        thisv->timer = 16;
        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        thisv->flags &= ~2;
    }

    thisv->flags |= 1;
}

void func_80AACE2C(EnMk* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x4001);
        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000AC0), ANIMMODE_ONCE, -4.0f);
        thisv->flags &= ~2;
        thisv->actionFunc = func_80AACD48;
    }

    thisv->flags |= 1;
}

void func_80AACEE8(EnMk* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x4000);
        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_mk_Anim_000AC0), ANIMMODE_LOOP, -4.0f);
        thisv->flags &= ~2;
        thisv->actionFunc = func_80AACE2C;
    }

    thisv->flags |= 1;
}

void func_80AACFA0(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_80AACA40;
        gSaveContext.itemGetInf[1] |= 1;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, 10000.0f, 50.0f);
    }
}

void func_80AAD014(EnMk* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80AACFA0;
        func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, 10000.0f, 50.0f);
    }

    thisv->flags |= 1;
}

void EnMk_Wait(EnMk* thisv, GlobalContext* globalCtx) {
    s16 angle;
    s32 swimFlag;
    Player* player = GET_PLAYER(globalCtx);
    s32 playerExchangeItem;

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        playerExchangeItem = func_8002F368(globalCtx);

        if (thisv->actor.textId != 0x4018) {
            player->actor.textId = thisv->actor.textId;
            thisv->actionFunc = func_80AACA40;
        } else {
            if (INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYEDROPS) {
                player->actor.textId = 0x4032;
                thisv->actionFunc = func_80AACA40;
            } else {
                switch (playerExchangeItem) {
                    case EXCH_ITEM_NONE:
                        if (thisv->swimFlag >= 8) {
                            if (gSaveContext.itemGetInf[1] & 1) {
                                player->actor.textId = 0x4075;
                                thisv->actionFunc = func_80AACA40;
                            } else {
                                player->actor.textId = 0x4074;
                                thisv->actionFunc = func_80AAD014;
                                thisv->swimFlag = 0;
                            }
                        } else {
                            if (thisv->swimFlag == 0) {
                                player->actor.textId = 0x4018;
                                thisv->actionFunc = func_80AACA40;
                            } else {
                                player->actor.textId = 0x406C + thisv->swimFlag;
                                thisv->actionFunc = func_80AACA40;
                            }
                        }
                        break;
                    case EXCH_ITEM_FROG:
                        player->actor.textId = 0x4019;
                        thisv->actionFunc = func_80AACEE8;
                        Animation_Change(&thisv->skelAnime, &object_mk_Anim_000368, 1.0f, 0.0f,
                                         Animation_GetLastFrame(&object_mk_Anim_000368), ANIMMODE_ONCE, -4.0f);
                        thisv->flags &= ~2;
                        gSaveContext.timer2State = 0;
                        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                        break;
                    default:
                        player->actor.textId = 0x4018;
                        thisv->actionFunc = func_80AACA40;
                        break;
                }
            }
        }
    } else {
        thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0x1A);

        if (thisv->actor.textId == 0) {
            thisv->actor.textId = 0x4018;
        }

        angle = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

        if ((ABS(angle) < 0x2151) && (thisv->actor.xzDistToPlayer < 100.0f)) {
            func_8002F298(&thisv->actor, globalCtx, 100.0f, EXCH_ITEM_FROG);
            thisv->flags |= 1;
        }
    }
}

void EnMk_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMk* thisv = (EnMk*)thisx;
    s32 pad;
    Vec3s vec;
    Player* player;
    s16 swimFlag;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);

    if ((!(thisv->flags & 2)) && (SkelAnime_Update(&thisv->skelAnime))) {
        thisv->flags |= 2;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->flags & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->headRotation, &vec, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->headRotation.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->headRotation.y, 0, 6, 6200, 100);
    }

    player = GET_PLAYER(globalCtx);

    if (thisv->flags & 8) {
        if (!(player->stateFlags2 & 0x400)) {
            thisv->flags &= ~8;
        }
    } else {
        if (player->currentBoots == PLAYER_BOOTS_IRON) {
            thisv->flags |= 8;
        } else if (player->stateFlags2 & 0x400) {
            swimFlag = player->actor.yDistToWater;

            if (swimFlag > 0) {
                if (swimFlag >= 320) {
                    if (swimFlag >= 355) {
                        swimFlag = 8;
                    } else {
                        swimFlag = 7;
                    }
                } else if (swimFlag < 80) {
                    swimFlag = 1;
                } else {
                    swimFlag *= 0.025f;
                }

                if (thisv->swimFlag < swimFlag) {
                    thisv->swimFlag = swimFlag;

                    if ((!(thisv->flags & 4)) && (thisv->swimFlag >= 8)) {
                        thisv->flags |= 4;
                        func_80078884(NA_SE_SY_CORRECT_CHIME);
                    }
                }
            }
        }
    }
}

s32 EnMk_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMk* thisv = (EnMk*)thisx;

    if (limbIndex == 11) {
        rot->y -= thisv->headRotation.y;
        rot->z += thisv->headRotation.x;
    }

    return false;
}

void EnMk_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80AAD64C = { 1000.0f, -100.0f, 0.0f };
    EnMk* thisv = (EnMk*)thisx;

    if (limbIndex == 11) {
        Matrix_MultVec3f(&D_80AAD64C, &thisv->actor.focus.pos);
    }
}

void EnMk_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnMk* thisv = (EnMk*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnMk_OverrideLimbDraw, EnMk_PostLimbDraw, &thisv->actor);
}
