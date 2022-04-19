/*
 * File: z_en_ta.c
 * Overlay: ovl_En_Ta
 * Description: Talon
 */

#include "z_en_ta.h"
#include "vt.h"
#include "objects/object_ta/object_ta.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnTa_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTa_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTa_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B14634(EnTa* thisv, GlobalContext* globalCtx);
void func_80B146F8(EnTa* thisv, GlobalContext* globalCtx);
void func_80B14754(EnTa* thisv, GlobalContext* globalCtx);
void func_80B14C18(EnTa* thisv, GlobalContext* globalCtx);
void func_80B14CAC(EnTa* thisv, GlobalContext* globalCtx);
void func_80B14D98(EnTa* thisv, GlobalContext* globalCtx);
void func_80B154FC(EnTa* thisv, GlobalContext* globalCtx);
void func_80B16504(EnTa* thisv, GlobalContext* globalCtx);
void func_80B16608(EnTa* thisv, GlobalContext* globalCtx);
void func_80B166CC(EnTa* thisv);
void func_80B16700(EnTa* thisv);
void func_80B167C0(EnTa* thisv);
void func_80B167FC(EnTa* thisv);
void func_80B16854(EnTa* thisv);
void func_80B16938(EnTa* thisv);

ActorInit En_Ta_InitVars = {
    ACTOR_EN_TA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TA,
    sizeof(EnTa),
    (ActorFunc)EnTa_Init,
    (ActorFunc)EnTa_Destroy,
    (ActorFunc)EnTa_Update,
    (ActorFunc)EnTa_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

void func_80B13AA0(EnTa* thisv, EnTaActionFunc arg1, EnTaUnkFunc arg2) {
    thisv->actionFunc = arg1;
    thisv->unk_260 = arg2;
}

void func_80B13AAC(EnTa* thisv, GlobalContext* globalCtx) {
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 24);

    if (gSaveContext.eventInf[0] & 0x400) {
        if (gSaveContext.eventInf[0] & 0x100) {
            if (gSaveContext.itemGetInf[0] & 4) {
                thisv->actor.textId = 0x2088;
            } else {
                thisv->actor.textId = 0x2086;
            }
        } else {
            thisv->actor.textId = 0x2085;
        }
        gSaveContext.eventInf[0] &= ~0x100;
    } else if (faceReaction == 0) {
        if (gSaveContext.infTable[7] & 0x4000) {
            if (gSaveContext.itemGetInf[0] & 4) {
                thisv->actor.textId = 0x208B;
            } else {
                thisv->actor.textId = 0x207F;
            }
        } else {
            thisv->actor.textId = 0x207E;
        }
    } else {
        thisv->actor.textId = faceReaction;
    }
}

void EnTa_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnTa* thisv = (EnTa*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gTalonSkel, &gTalonStandAnim, thisv->jointTable, thisv->morphTable,
                       17);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);

    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->unk_2E0 = 0;
    thisv->unk_2CE = 0;
    thisv->unk_2E2 = 0;
    thisv->blinkTimer = 20;
    thisv->unk_2B0 = func_80B166CC;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.targetMode = 6;
    thisv->actor.velocity.y = -4.0f;
    thisv->actor.minVelocityY = -4.0f;
    thisv->actor.gravity = -1.0f;

    switch (thisv->actor.params) {
        case 1:
            osSyncPrintf(VT_FGCOL(CYAN) " 追放タロン \n" VT_RST);
            if (gSaveContext.eventChkInf[6] & 0x800) {
                Actor_Kill(&thisv->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_Kill(&thisv->actor);
            } else if (gSaveContext.eventChkInf[6] & 0x400) {
                func_80B13AA0(thisv, func_80B14CAC, func_80B167C0);
                thisv->eyeIndex = 0;
                Animation_PlayOnce(&thisv->skelAnime, &gTalonStandAnim);
                thisv->currentAnimation = &gTalonStandAnim;
            } else {
                func_80B13AA0(thisv, func_80B14754, func_80B167FC);
                thisv->eyeIndex = 2;
                Animation_PlayOnce(&thisv->skelAnime, &gTalonSleepAnim);
                thisv->currentAnimation = &gTalonSleepAnim;
                thisv->actor.shape.shadowScale = 54.0f;
            }
            break;
        case 2:
            osSyncPrintf(VT_FGCOL(CYAN) " 出戻りタロン \n" VT_RST);
            if (!(gSaveContext.eventChkInf[6] & 0x800)) {
                Actor_Kill(&thisv->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_Kill(&thisv->actor);
            } else if (globalCtx->sceneNum == SCENE_MALON_STABLE && !IS_DAY) {
                Actor_Kill(&thisv->actor);
                osSyncPrintf(VT_FGCOL(CYAN) " 夜はいない \n" VT_RST);
            } else {
                func_80B13AA0(thisv, func_80B14D98, func_80B167C0);
                thisv->eyeIndex = 0;
                Animation_PlayOnce(&thisv->skelAnime, &gTalonStandAnim);
                thisv->currentAnimation = &gTalonStandAnim;
            }
            break;
        default:
            osSyncPrintf(VT_FGCOL(CYAN) " その他のタロン \n" VT_RST);
            if (globalCtx->sceneNum == SCENE_SPOT15) {
                if (gSaveContext.eventChkInf[1] & 0x10) {
                    Actor_Kill(&thisv->actor);
                } else if (gSaveContext.eventChkInf[1] & 0x8) {
                    func_80B13AA0(thisv, func_80B14C18, func_80B167C0);
                    thisv->eyeIndex = 0;
                    Animation_PlayOnce(&thisv->skelAnime, &gTalonStandAnim);
                    thisv->currentAnimation = &gTalonStandAnim;
                } else {
                    func_80B13AA0(thisv, func_80B14634, func_80B167FC);
                    thisv->eyeIndex = 2;
                    Animation_PlayOnce(&thisv->skelAnime, &gTalonSleepAnim);
                    thisv->currentAnimation = &gTalonSleepAnim;
                    thisv->actor.shape.shadowScale = 54.0f;
                }
            } else if (globalCtx->sceneNum == SCENE_SOUKO) {
                osSyncPrintf(VT_FGCOL(CYAN) " ロンロン牧場の倉庫 の タロン\n" VT_RST);
                if (!(gSaveContext.eventChkInf[1] & 0x10)) {
                    Actor_Kill(&thisv->actor);
                } else if (LINK_IS_ADULT) {
                    Actor_Kill(&thisv->actor);
                } else {
                    if (IS_DAY) {
                        thisv->actor.flags |= ACTOR_FLAG_4;
                        thisv->unk_2C4[0] = thisv->unk_2C4[1] = thisv->unk_2C4[2] = 7;
                        thisv->superCuccos[0] = (EnNiw*)Actor_Spawn(
                            &globalCtx->actorCtx, globalCtx, ACTOR_EN_NIW, thisv->actor.world.pos.x + 5.0f,
                            thisv->actor.world.pos.y + 3.0f, thisv->actor.world.pos.z + 26.0f, 0, 0, 0, 0xD);
                        thisv->superCuccos[1] = (EnNiw*)Actor_Spawn(
                            &globalCtx->actorCtx, globalCtx, ACTOR_EN_NIW, thisv->actor.world.pos.x - 20.0f,
                            thisv->actor.world.pos.y + 40.0f, thisv->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        thisv->superCuccos[2] = (EnNiw*)Actor_Spawn(
                            &globalCtx->actorCtx, globalCtx, ACTOR_EN_NIW, thisv->actor.world.pos.x + 20.0f,
                            thisv->actor.world.pos.y + 40.0f, thisv->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        func_80B13AAC(thisv, globalCtx);

                        if (gSaveContext.eventInf[0] & 0x400) {
                            func_80B13AA0(thisv, func_80B16608, func_80B16938);
                            Animation_Change(&thisv->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                                             Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                                             Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 0.0f);
                            gSaveContext.eventInf[0] &= ~0x400;
                        } else {
                            func_80B13AA0(thisv, func_80B16504, func_80B16854);
                            thisv->eyeIndex = 0;
                            Animation_PlayOnce(&thisv->skelAnime, &gTalonSitSleepingAnim);
                            thisv->currentAnimation = &gTalonSitSleepingAnim;
                        }
                    } else {
                        func_80B13AA0(thisv, func_80B146F8, func_80B167FC);
                        thisv->eyeIndex = 2;
                        Animation_PlayOnce(&thisv->skelAnime, &gTalonSleepAnim);
                        thisv->currentAnimation = &gTalonSleepAnim;
                        thisv->actor.shape.shadowScale = 54.0f;
                    }
                }
            } else {
                func_80B13AA0(thisv, func_80B14634, func_80B167FC);
                thisv->eyeIndex = 2;
                Animation_PlayOnce(&thisv->skelAnime, &gTalonSleepAnim);
                thisv->currentAnimation = &gTalonSleepAnim;
                thisv->actor.shape.shadowScale = 54.0f;
            }
            break;
    }
}

void func_80B14248(EnTa* thisv) {
    if (thisv->actor.shape.shadowScale > 36.0f) {
        thisv->actor.shape.shadowScale -= 0.8f;
    }
}

void EnTa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnTa* thisv = (EnTa*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);

    if (thisv->actor.params != 1 && thisv->actor.params != 2 && globalCtx->sceneNum == SCENE_SOUKO) {
        gSaveContext.timer1State = 0;
    }

    if (thisv->unk_2E0 & 0x200) {
        func_800F5B58();
    }
}

s32 func_80B142F4(EnTa* thisv, GlobalContext* globalCtx, u16 textId) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        return true;
    }

    thisv->actor.textId = textId;

    if ((ABS((s16)(thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y)) <= 0x4300) &&
        (thisv->actor.xzDistToPlayer < 100.0f)) {
        thisv->unk_2E0 |= 1;
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
    return false;
}

void func_80B14398(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B14754, func_80B167FC);
    }
}

void func_80B143D4(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B146F8, func_80B167FC);
    }
}

void func_80B14410(EnTa* thisv) {
    if (!LINK_IS_ADULT) {
        func_80B13AA0(thisv, func_80B14C18, func_80B167C0);
        gSaveContext.eventChkInf[1] |= 0x8;
    } else {
        func_80B13AA0(thisv, func_80B14CAC, func_80B167C0);
        gSaveContext.eventChkInf[6] |= 0x400;
    }
}

void func_80B1448C(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B14410(thisv);
    }
    func_80B14248(thisv);
    thisv->unk_2E0 |= 0x4;
}

void func_80B144D8(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B14410(thisv);
        thisv->blinkTimer = 1;
        thisv->unk_2B0 = func_80B16700;
    }

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) {
        thisv->eyeIndex = 1;
        func_80B13AA0(thisv, func_80B1448C, func_80B167C0);
    }
    func_80B14248(thisv);
    thisv->unk_2E0 |= 4;
}

void func_80B14570(EnTa* thisv, GlobalContext* globalCtx) {
    thisv->unk_2E0 |= 4;

    if (thisv->unk_2CC == 0) {
        func_80B13AA0(thisv, func_80B144D8, func_80B167C0);
        thisv->unk_2CE = 3;
        thisv->unk_2CC = 60;
        Animation_PlayOnce(&thisv->skelAnime, &gTalonWakeUpAnim);
        thisv->currentAnimation = &gTalonStandAnim;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_SURPRISE);
    }
}

void func_80B145F8(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B14634, func_80B167FC);
    }
}

void func_80B14634(EnTa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        s32 exchangeItemId = func_8002F368(globalCtx);

        switch (exchangeItemId) {
            case EXCH_ITEM_CHICKEN:
                player->actor.textId = 0x702B;
                func_80B13AA0(thisv, func_80B14570, func_80B167C0);
                thisv->unk_2CC = 40;
                break;
            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x702A;
                }
                func_80B13AA0(thisv, func_80B145F8, func_80B167FC);
                break;
        }
    } else {
        thisv->actor.textId = 0x702A;
        func_8002F298(&thisv->actor, globalCtx, 100.0f, 3);
    }
}

void func_80B146F8(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B143D4, func_80B167FC);
    }
    thisv->actor.textId = 0x204B;
    func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
}

void func_80B14754(EnTa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        s32 exchangeItemId = func_8002F368(globalCtx);

        switch (exchangeItemId) {
            case EXCH_ITEM_POCKET_CUCCO:
                player->actor.textId = 0x702B;
                func_80B13AA0(thisv, func_80B14570, func_80B167C0);
                thisv->unk_2CC = 40;
                break;
            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x5015;
                }
                func_80B13AA0(thisv, func_80B14398, func_80B167FC);
                break;
        }
    } else {
        thisv->actor.textId = 0x5015;
        func_8002F298(&thisv->actor, globalCtx, 100.0f, 6);
    }
}

void func_80B14818(EnTa* thisv, GlobalContext* globalCtx) {
    s32 framesMod12 = (s32)globalCtx->state.frames % 12;

    if (framesMod12 == 0 || framesMod12 == 6) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_WALK_GROUND);
    }
    if (thisv->actor.speedXZ < 6.0f) {
        thisv->actor.speedXZ += 0.4f;
    }
    Actor_MoveForward(&thisv->actor);
}

void func_80B14898(EnTa* thisv, GlobalContext* globalCtx) {
    func_80033480(globalCtx, &thisv->actor.world.pos, 50.0f, 2, 250, 20, 1);
    func_80B14818(thisv, globalCtx);

    if (thisv->unk_2CC == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80B1490C(EnTa* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y += 0xC00;
    thisv->actor.shape.rot.y += 0xC00;

    if (thisv->unk_2CC == 0) {
        func_80B13AA0(thisv, func_80B14898, func_80B167C0);
        thisv->unk_2CC = 60;
    }
}

void func_80B1496C(EnTa* thisv, GlobalContext* globalCtx) {
    func_80033480(globalCtx, &thisv->actor.world.pos, 50.0f, 2, 250, 20, 1);
    func_80B14818(thisv, globalCtx);

    if (thisv->unk_2CC == 0) {
        func_80B13AA0(thisv, func_80B1490C, func_80B167C0);
        thisv->unk_2CC = 5;
    }
}

void func_80B149F4(EnTa* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y -= 0xD00;
    thisv->actor.shape.rot.y -= 0xD00;

    if (thisv->unk_2CC == 0) {
        func_80B13AA0(thisv, func_80B1496C, func_80B167C0);
        thisv->unk_2CC = 65;
    }
}

void func_80B14A54(EnTa* thisv, GlobalContext* globalCtx) {
    func_80033480(globalCtx, &thisv->actor.world.pos, 50.0f, 2, 250, 20, 1);
    func_80B14818(thisv, globalCtx);

    if (thisv->unk_2CC == 20) {
        Message_CloseTextbox(globalCtx);
    }
    if (thisv->unk_2CC == 0) {
        thisv->unk_2CC = 5;
        func_80B13AA0(thisv, func_80B149F4, func_80B167C0);
    }
}

void func_80B14AF4(EnTa* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y -= 0xC00;
    thisv->actor.shape.rot.y -= 0xC00;

    if (thisv->unk_2CC == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_CRY_1);
        func_80B13AA0(thisv, func_80B14A54, func_80B167C0);
        thisv->unk_2CC = 65;
        thisv->actor.flags |= ACTOR_FLAG_4;
    }
}

void func_80B14B6C(EnTa* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) {
        OnePointCutscene_Init(globalCtx, 4175, -99, &thisv->actor, MAIN_CAM);
        func_80B13AA0(thisv, func_80B14AF4, func_80B167C0);
        thisv->unk_2CC = 5;
        gSaveContext.eventChkInf[1] |= 0x10;
        Animation_PlayOnce(&thisv->skelAnime, &gTalonRunTransitionAnim);
        thisv->currentAnimation = &gTalonRunAnim;
    }
    thisv->unk_2E0 |= 1;
}

void func_80B14C18(EnTa* thisv, GlobalContext* globalCtx) {
    if (func_80B142F4(thisv, globalCtx, 0x702C)) {
        func_80B13AA0(thisv, func_80B14B6C, func_80B167C0);
    }
    func_80B14248(thisv);
}

void func_80B14C60(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B14CAC, func_80B167C0);
    }
    thisv->unk_2E0 |= 1;
}

void func_80B14CAC(EnTa* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[1] & 0x100) {
        if (func_80B142F4(thisv, globalCtx, 0x5017)) {
            func_80B13AA0(thisv, func_80B14C60, func_80B167C0);
            gSaveContext.eventChkInf[6] |= 0x800;
        }
    } else if (func_80B142F4(thisv, globalCtx, 0x5016)) {
        func_80B13AA0(thisv, func_80B14C60, func_80B167C0);
    }
    func_80B14248(thisv);
}

void func_80B14D4C(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B13AA0(thisv, func_80B14D98, func_80B167C0);
    }
    thisv->unk_2E0 |= 1;
}

void func_80B14D98(EnTa* thisv, GlobalContext* globalCtx) {
    if (func_80B142F4(thisv, globalCtx, 0x2055)) {
        func_80B13AA0(thisv, func_80B14D4C, func_80B167C0);
    }
}

s32 func_80B14DD8(void) {
    if (gSaveContext.rupees < 30) {
        return 0;
    } else if (!Inventory_HasEmptyBottle()) {
        return 1;
    } else {
        return 2;
    }
}

void func_80B14E28(EnTa* thisv, GlobalContext* globalCtx) {
    Vec3f b;
    Vec3f a;

    thisv->unk_2D0 = Gameplay_CreateSubCamera(globalCtx);
    thisv->unk_2D2 = globalCtx->activeCamera;
    Gameplay_ChangeCameraStatus(globalCtx, thisv->unk_2D2, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->unk_2D0, CAM_STAT_ACTIVE);

    b.x = 1053.0f;
    b.y = 11.0f;
    b.z = 22.0f;

    a.x = 1053.0f;
    a.y = 45.0f;
    a.z = -40.0f;

    Gameplay_CameraSetAtEye(globalCtx, thisv->unk_2D0, &a, &b);
}

void func_80B14EDC(EnTa* thisv, GlobalContext* globalCtx) {
    Gameplay_ChangeCameraStatus(globalCtx, thisv->unk_2D2, CAM_STAT_ACTIVE);
    Gameplay_ClearCamera(globalCtx, thisv->unk_2D0);
}

void func_80B14F20(EnTa* thisv, EnTaActionFunc arg1) {
    func_80B13AA0(thisv, arg1, func_80B16854);
    thisv->eyeIndex = 2;
    Animation_Change(&thisv->skelAnime, &gTalonSitSleepingAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gTalonSitSleepingAnim), ANIMMODE_ONCE, -5.0f);
    thisv->unk_2E2 = 0;
    thisv->currentAnimation = &gTalonSitSleepingAnim;
}

void func_80B14FAC(EnTa* thisv, EnTaActionFunc arg1) {
    thisv->eyeIndex = 1;
    func_80B13AA0(thisv, arg1, func_80B16938);
    thisv->unk_2E0 &= ~0x10;
    Animation_Change(&thisv->skelAnime, &gTalonSitWakeUpAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gTalonSitWakeUpAnim),
                     ANIMMODE_ONCE, -5.0f);
}

void func_80B15034(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_80B14F20(thisv, func_80B16504);
        func_80B13AAC(thisv, globalCtx);
    }
    thisv->unk_2E0 |= 1;
}

s32 func_80B150AC(EnTa* thisv, GlobalContext* globalCtx, s32 idx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* interactRangeActor;

    if (player->stateFlags1 & 0x800) {
        interactRangeActor = player->interactRangeActor;
        if (interactRangeActor != NULL && interactRangeActor->id == ACTOR_EN_NIW &&
            interactRangeActor == &thisv->superCuccos[idx]->actor) {
            return true;
        }
    }
    return false;
}

void func_80B15100(EnTa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        s32 unk_2CA;

        Animation_Change(&thisv->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        thisv->unk_2E0 &= ~0x10;
        Message_CloseTextbox(globalCtx);
        unk_2CA = thisv->unk_2CA;
        thisv->actionFunc = func_80B154FC;
        thisv->superCuccos[unk_2CA]->actor.gravity = 0.1f;
        thisv->superCuccos[unk_2CA]->actor.velocity.y = 0.0f;
        thisv->superCuccos[unk_2CA]->actor.speedXZ = 0.0f;
        thisv->superCuccos[unk_2CA]->actor.parent = NULL;

        if (player->interactRangeActor == &thisv->superCuccos[unk_2CA]->actor) {
            player->interactRangeActor = NULL;
        }
        if (player->heldActor == &thisv->superCuccos[unk_2CA]->actor) {
            player->heldActor = NULL;
        }
        player->stateFlags1 &= ~0x800;
        thisv->superCuccos[unk_2CA] = NULL;
    }
    thisv->unk_2E0 |= 1;
}

void func_80B15260(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80B15100;
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 1000.0f);
    }
    thisv->unk_2E0 |= 1;
}

s32 EnTa_GetSuperCuccosCount(EnTa* thisv, GlobalContext* globalCtx) {
    s32 count;
    s32 i;

    for (count = 0, i = 0; i < ARRAY_COUNT(thisv->superCuccos); i++) {
        if (thisv->superCuccos[i] != NULL) {
            count++;
        }
    }
    return count;
}

void func_80B15308(EnTa* thisv) {
    if (thisv->unk_2E0 & 0x10) {
        if (thisv->unk_2E0 & 0x100) {
            Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 17.0f, 22.0f, ANIMMODE_ONCE, 0.0f);
            thisv->unk_2E0 &= ~0x100;
        } else {
            Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 21.0f, 16.0f, ANIMMODE_ONCE, 3.0f);
            thisv->unk_2E0 |= 0x100;
        }
        thisv->unk_2E0 &= ~0x10;
    }
}

void func_80B153D4(EnTa* thisv, GlobalContext* globalCtx) {
    func_80B15308(thisv);

    if (thisv->unk_2CC == 0) {
        if (thisv->unk_2E0 & 0x80) {
            thisv->unk_2E0 &= ~0x80;
            func_80B14EDC(thisv, globalCtx);
        }
    }
}

void func_80B15424(EnTa* thisv, GlobalContext* globalCtx) {
    func_80B15308(thisv);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        globalCtx->nextEntranceIndex = 0x5E4;

        if (gSaveContext.eventInf[0] & 0x100) {
            globalCtx->fadeTransition = 46;
            gSaveContext.nextTransition = 3;
        } else {
            globalCtx->fadeTransition = 38;
            gSaveContext.nextTransition = 2;
        }

        globalCtx->sceneLoadFlag = 0x14;
        gSaveContext.eventInf[0] |= 0x400;
        thisv->actionFunc = func_80B153D4;
        thisv->unk_2CC = 22;
    }
}

void func_80B154FC(EnTa* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(thisv->superCuccos); i++) {
        if (thisv->superCuccos[i] != NULL) {
            if (thisv->superCuccos[i]->actor.gravity > -2.0f) {
                thisv->superCuccos[i]->actor.gravity -= 0.03f;
            }

            if (func_80B150AC(thisv, globalCtx, i)) {
                if (thisv->unk_2C4[i] > 0) {
                    thisv->unk_2C4[i]--;
                } else {
                    thisv->unk_2CA = i;
                    Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
                    thisv->unk_2E0 &= ~0x10;

                    switch (EnTa_GetSuperCuccosCount(thisv, globalCtx)) {
                        case 1:
                            gSaveContext.timer1State = 0;
                            func_8002DF54(globalCtx, &thisv->actor, 1);

                            Message_StartTextbox(globalCtx, 0x2084, &thisv->actor);
                            thisv->actionFunc = func_80B15424;
                            Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE,
                                             -10.0f);
                            thisv->unk_2E0 &= ~0x10;
                            thisv->unk_2E0 &= ~0x100;
                            gSaveContext.eventInf[0] |= 0x100;
                            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
                            thisv->unk_2E0 &= ~0x200;
                            Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                            return;
                        case 2:
                            thisv->actor.textId = 0x2083;
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_CRY_1);
                            break;
                        case 3:
                            thisv->actor.textId = 0x2082;
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_SURPRISE);
                            break;
                    }
                    thisv->actionFunc = func_80B15260;
                    thisv->actor.flags |= ACTOR_FLAG_16;
                    func_8002F2CC(&thisv->actor, globalCtx, 1000.0f);
                    return;
                }
            } else {
                thisv->unk_2C4[i] = 7;
            }
        }
    }

    if (gSaveContext.timer1Value == 10) {
        func_800F5918();
    }

    if (gSaveContext.timer1Value == 0 && !Gameplay_InCsMode(globalCtx)) {
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);
        thisv->unk_2E0 &= ~0x200;
        func_80078884(NA_SE_SY_FOUND);
        gSaveContext.timer1State = 0;
        func_8002DF54(globalCtx, &thisv->actor, 1);
        Message_StartTextbox(globalCtx, 0x2081, &thisv->actor);
        thisv->actionFunc = func_80B15424;
        func_80B14E28(thisv, globalCtx);
        gSaveContext.eventInf[0] &= ~0x100;
        thisv->unk_2E0 |= 0x80;
        Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
        thisv->unk_2E0 &= ~0x10;
        thisv->unk_2E0 &= ~0x100;
    }

    thisv->unk_2E0 |= 1;
}

void func_80B1585C(EnTa* thisv, GlobalContext* globalCtx) {
    s32 i;

    if (thisv->unk_2CC > 35) {
        for (i = 1; i < ARRAY_COUNT(thisv->superCuccos); i++) {
            if (thisv->superCuccos[i] != NULL) {
                Math_SmoothStepToS(&thisv->superCuccos[i]->actor.world.rot.y, i * -10000 - 3000, 2, 0x800, 0x100);
                thisv->superCuccos[i]->actor.shape.rot.y = thisv->superCuccos[i]->actor.world.rot.y;
            }
        }
    } else if (thisv->unk_2CC == 35) {
        for (i = 0; i < ARRAY_COUNT(thisv->superCuccos); i++) {
            thisv->unk_2C4[i] = (s32)(Rand_CenteredFloat(6.0f) + 10.0f);

            if (thisv->superCuccos[i] != NULL) {
                EnNiw* niw = thisv->superCuccos[i];

                niw->unk_308 = 1;
                niw->actor.gravity = 0.0f;
            }
        }
    } else {
        for (i = 0; i < ARRAY_COUNT(thisv->superCuccos); i++) {
            if (thisv->unk_2CC < 35 - thisv->unk_2C4[i]) {
                if (thisv->superCuccos[i] != NULL) {
                    if (thisv->superCuccos[i]->actor.gravity > -2.0f) {
                        thisv->superCuccos[i]->actor.gravity -= 0.03f;
                    }
                }
            }
        }
    }

    if (thisv->unk_2CC == 0) {
        func_80B13AA0(thisv, func_80B154FC, func_80B16938);
        thisv->unk_2E0 &= ~0x10;
        Animation_Change(&thisv->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim) - 1.0f,
                         Animation_GetLastFrame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        func_8002DF54(globalCtx, &thisv->actor, 7);
    }
}

void func_80B15AD4(EnTa* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_2CC == 0 && thisv->unk_2E0 & 0x20) {
        func_80B13AA0(thisv, func_80B1585C, func_80B16938);
        thisv->unk_2E0 &= ~0x10;
        Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 1.0f,
                         Animation_GetLastFrame(&gTalonSitHandsUpAnim), ANIMMODE_ONCE, 0.0f);
        thisv->unk_2CC = 50;
        func_80088B34(0x1E);
        func_800F5ACC(NA_BGM_TIMED_MINI_GAME);
        thisv->unk_2E0 |= 0x200;
        Message_CloseTextbox(globalCtx);
        func_8002DF54(globalCtx, &thisv->actor, 1);
    }

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->unk_2E0 |= 0x20;
    }

    thisv->unk_2E0 |= 1;
}

void func_80B15BF8(EnTa* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_2E0 & 0x10) {
        func_80B13AA0(thisv, func_80B15AD4, func_80B16938);
        thisv->unk_2E0 &= ~0x10;
        Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 0.0f, 1.0f, ANIMMODE_ONCE, 0.0f);
        thisv->unk_2CC = 5;
    }
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->unk_2E0 |= 0x20;
    }
    thisv->unk_2E0 |= 1;
}

void func_80B15CC8(EnTa* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_2E0 & 0x10) {
        func_80B13AA0(thisv, func_80B15BF8, func_80B16938);
        thisv->unk_2E0 &= ~0x10;
        Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 29.0f, 0.0f, ANIMMODE_ONCE, 10.0f);
    }
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->unk_2E0 |= 0x20;
    }
    thisv->unk_2E0 |= 1;
}

void func_80B15D90(EnTa* thisv, GlobalContext* globalCtx) {
    func_80B13AA0(thisv, func_80B15CC8, func_80B16938);
    thisv->unk_2E0 &= ~0x10;
    Animation_Change(&thisv->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
    Message_ContinueTextbox(globalCtx, 0x2080);
    thisv->unk_2E0 &= ~0x20;
}

void func_80B15E28(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80B14F20(thisv, func_80B16504);
        func_80B13AAC(thisv, globalCtx);
    }
    thisv->unk_2E0 |= 1;
}

void func_80B15E80(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = func_80B15E28;
        if (!(thisv->unk_2E0 & 0x2)) {
            gSaveContext.itemGetInf[0] |= 4;
        }
        thisv->unk_2E0 &= ~0x2;
    } else if (thisv->unk_2E0 & 2) {
        func_8002F434(&thisv->actor, globalCtx, GI_MILK, 10000.0f, 50.0f);
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_MILK_BOTTLE, 10000.0f, 50.0f);
    }
    thisv->unk_2E0 |= 1;
}

void func_80B15F54(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->unk_2E0 &= ~0x2;
        func_80B13AA0(thisv, func_80B15E80, func_80B16938);
        func_8002F434(&thisv->actor, globalCtx, GI_MILK_BOTTLE, 10000.0f, 50.0f);
    }
}

void func_80B15FE8(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                switch (func_80B14DD8()) {
                    case 0:
                        Message_ContinueTextbox(globalCtx, 0x85);
                        func_80B13AA0(thisv, func_80B15034, func_80B16938);
                        break;
                    case 1:
                        Message_ContinueTextbox(globalCtx, 0x208A);
                        func_80B13AA0(thisv, func_80B15E28, func_80B16938);
                        break;
                    case 2:
                        thisv->unk_2E0 |= 2;
                        func_80B13AA0(thisv, func_80B15E80, func_80B16938);
                        Rupees_ChangeBy(-30);
                        func_8002F434(&thisv->actor, globalCtx, GI_MILK, 10000.0f, 50.0f);
                        break;
                }
                break;
            case 1:
                if (gSaveContext.rupees < 10) {
                    Message_ContinueTextbox(globalCtx, 0x85);
                    func_80B13AA0(thisv, func_80B15034, func_80B16938);
                } else {
                    Rupees_ChangeBy(-10);
                    func_80B15D90(thisv, globalCtx);
                }
                break;
            case 2:
                func_80B14F20(thisv, func_80B16504);
                func_80B13AAC(thisv, globalCtx);
                break;
        }
    }

    if (thisv->unk_2E0 & 0x10) {
        thisv->unk_2E0 |= 1;
    }
}

void func_80B161C0(EnTa* thisv, GlobalContext* globalCtx) {
    s32 price;

    if (thisv->actor.textId == 0x2085) {
        price = 5;
    } else {
        price = 10;
    }

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees < price) {
                    Message_ContinueTextbox(globalCtx, 0x85);
                    func_80B13AA0(thisv, func_80B15034, func_80B16938);
                } else {
                    Rupees_ChangeBy(-price);
                    func_80B15D90(thisv, globalCtx);
                }
                break;
            case 1:
                func_80B14F20(thisv, func_80B16504);
                func_80B13AAC(thisv, globalCtx);
                break;
        }
    }

    if (thisv->unk_2E0 & 0x10) {
        thisv->unk_2E0 |= 1;
    }
}

void func_80B162E8(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0x2087);
        func_80B13AA0(thisv, func_80B15F54, func_80B16938);
    }

    if (thisv->unk_2E0 & 0x10) {
        thisv->unk_2E0 |= 1;
    }
}

void func_80B16364(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        gSaveContext.infTable[7] |= 0x4000;
        if (gSaveContext.itemGetInf[0] & 4) {
            Message_ContinueTextbox(globalCtx, 0x208B);
            func_80B13AA0(thisv, func_80B15FE8, func_80B16938);
        } else {
            Message_ContinueTextbox(globalCtx, 0x207F);
            func_80B13AA0(thisv, func_80B161C0, func_80B16938);
        }
    }

    if (thisv->unk_2E0 & 0x10) {
        thisv->unk_2E0 |= 1;
    }
}

void func_80B1642C(EnTa* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        if (Inventory_HasEmptyBottle()) {
            Message_CloseTextbox(globalCtx);
            thisv->unk_2E0 |= 2;
            func_80B13AA0(thisv, func_80B15E80, func_80B16938);
            func_8002F434(&thisv->actor, globalCtx, GI_MILK, 10000.0f, 50.0f);
        } else {
            Message_ContinueTextbox(globalCtx, 0x208A);
            func_80B13AA0(thisv, func_80B15E28, func_80B16938);
        }
    }
}

void func_80B16504(EnTa* thisv, GlobalContext* globalCtx) {
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 0x18);

    func_80B13AAC(thisv, globalCtx);

    if (func_80B142F4(thisv, globalCtx, thisv->actor.textId)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_SURPRISE);

        if (faceReaction != 0) {
            func_80B14FAC(thisv, func_80B15E28);
        } else {
            gSaveContext.infTable[7] |= 0x4000;

            switch (thisv->actor.textId) {
                case 0x207E:
                case 0x207F:
                    func_80B14FAC(thisv, func_80B161C0);
                    break;
                case 0x208B:
                    func_80B14FAC(thisv, func_80B15FE8);
                    break;
                default:
                    func_80B14FAC(thisv, func_80B16364);
                    break;
            }
        }
    }
    thisv->unk_2E0 &= ~1;
}

void func_80B16608(EnTa* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        switch (thisv->actor.textId) {
            case 0x2085:
                thisv->actionFunc = func_80B161C0;
                break;
            case 0x2086:
                thisv->actionFunc = func_80B162E8;
                break;
            case 0x2088:
                thisv->actionFunc = func_80B1642C;
                break;
        }
        thisv->actor.flags &= ~ACTOR_FLAG_16;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F2CC(&thisv->actor, globalCtx, 1000.0f);
    }
    thisv->unk_2E0 |= 1;
}

void func_80B166CC(EnTa* thisv) {
    s16 temp_v0 = thisv->blinkTimer - 1;

    if (temp_v0 != 0) {
        thisv->blinkTimer = temp_v0;
    } else {
        thisv->unk_2B0 = func_80B16700;
    }
}

void func_80B16700(EnTa* thisv) {
    s16 blinkTimer = thisv->blinkTimer - 1;

    if (blinkTimer != 0) {
        thisv->blinkTimer = blinkTimer;
    } else {
        s16 nextEyeIndex = thisv->eyeIndex + 1;
        s16 blinkTimer = 3;

        if (nextEyeIndex >= blinkTimer) {
            thisv->eyeIndex = 0;
            if (thisv->unk_2CE > 0) {
                thisv->unk_2CE--;
                blinkTimer = 1;
            } else {
                blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
            }
            thisv->blinkTimer = blinkTimer;
            thisv->unk_2B0 = func_80B166CC;
        } else {
            thisv->eyeIndex = nextEyeIndex;
            thisv->blinkTimer = 1;
        }
    }
}

void func_80B167C0(EnTa* thisv) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_PlayOnce(&thisv->skelAnime, thisv->currentAnimation);
    }
}

void func_80B167FC(EnTa* thisv) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Animation_PlayOnce(&thisv->skelAnime, thisv->currentAnimation);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_TA_SLEEP);
    }
    thisv->unk_2E0 |= 0xC;
}

void func_80B16854(EnTa* thisv) {
    if (thisv->unk_2E2 > 0) {
        thisv->unk_2E2--;
    } else {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            Animation_PlayOnce(&thisv->skelAnime, thisv->currentAnimation);
            thisv->unk_2E2 = Rand_ZeroFloat(100.0f) + 100.0f;
        }

        if (thisv->skelAnime.curFrame < 96.0f && thisv->skelAnime.curFrame >= 53.0f) {
            thisv->eyeIndex = 1;
        } else {
            thisv->eyeIndex = 2;
        }
        thisv->unk_2E0 |= 8;
    }
    thisv->unk_2E0 |= 4;
}

void func_80B16938(EnTa* thisv) {
    if (!(thisv->unk_2E0 & 0x10)) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->unk_2E0 |= 0x10;
        }
        thisv->unk_2E0 |= 8;
    }
}

void EnTa_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnTa* thisv = (EnTa*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    thisv->unk_260(thisv);
    thisv->actionFunc(thisv, globalCtx);

    if (!(thisv->unk_2E0 & 4)) {
        thisv->unk_2B0(thisv);
    }

    if (thisv->unk_2E0 & 1) {
        func_80038290(globalCtx, &thisv->actor, &thisv->unk_2D4, &thisv->unk_2DA, thisv->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&thisv->unk_2D4.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2D4.y, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2DA.x, 0, 6, 6200, 100);
        Math_SmoothStepToS(&thisv->unk_2DA.y, 0, 6, 6200, 100);
    }

    thisv->unk_2E0 &= ~0x5;

    if (thisv->unk_2CC > 0) {
        thisv->unk_2CC--;
    }
}

s32 EnTa_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTa* thisv = (EnTa*)thisx;

    switch (limbIndex) {
        case 8:
            rot->x += thisv->unk_2DA.y;
            rot->y -= thisv->unk_2DA.x;
            break;
        case 15:
            rot->x += thisv->unk_2D4.y;
            rot->z += thisv->unk_2D4.x;
            break;
    }

    if (thisv->unk_2E0 & 0x8) {
        thisv->unk_2E0 &= ~0x8;
    } else if ((limbIndex == 8) || (limbIndex == 10) || (limbIndex == 13)) {
        s32 limbIdx50 = limbIndex * 50;

        rot->y += Math_SinS(globalCtx->state.frames * (limbIdx50 + 0x814)) * 200.0f;
        rot->z += Math_CosS(globalCtx->state.frames * (limbIdx50 + 0x940)) * 200.0f;
    }

    return false;
}

void EnTa_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_80B16E7C = {
        1100.0f,
        1000.0f,
        0.0f,
    };
    EnTa* thisv = (EnTa*)thisx;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80B16E7C, &thisv->actor.focus.pos);
    }
}

void EnTa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = {
        gTalonEyeOpenTex,
        gTalonEyeHalfTex,
        gTalonEyeClosedTex,
    };
    EnTa* thisv = (EnTa*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ta.c", 2381);

    func_800943C8(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(gTalonHeadSkinTex));

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnTa_OverrideLimbDraw, EnTa_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ta.c", 2400);
}
