/*
 * File: z_en_takara_man.c
 * Overlay: ovl_En_Takara_Man
 * Description: Treasure Chest Game Man
 */

#include "z_en_takara_man.h"
#include "vt.h"
#include "objects/object_ts/object_ts.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_27)

void EnTakaraMan_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTakaraMan_Reset(Actor* thisx, GlobalContext* globalCtx);
void EnTakaraMan_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTakaraMan_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B176E0(EnTakaraMan* thisv, GlobalContext* globalCtx);
void func_80B1778C(EnTakaraMan* thisv, GlobalContext* globalCtx);
void func_80B17B14(EnTakaraMan* thisv, GlobalContext* globalCtx);
void func_80B17934(EnTakaraMan* thisv, GlobalContext* globalCtx);
void func_80B17A6C(EnTakaraMan* thisv, GlobalContext* globalCtx);
void func_80B17AC4(EnTakaraMan* thisv, GlobalContext* globalCtx);

ActorInit En_Takara_Man_InitVars = {
    ACTOR_EN_TAKARA_MAN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TS,
    sizeof(EnTakaraMan),
    (ActorFunc)EnTakaraMan_Init,
    NULL,
    (ActorFunc)EnTakaraMan_Update,
    (ActorFunc)EnTakaraMan_Draw,
    (ActorResetFunc)EnTakaraMan_Reset,
};

static u8 sTakaraIsInitialized = false;

void EnTakaraMan_Reset(Actor* thisx, GlobalContext* globalCtx) {
    sTakaraIsInitialized = false;
}

void EnTakaraMan_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnTakaraMan* thisv = (EnTakaraMan*)thisx;

    if (sTakaraIsInitialized) {
        Actor_Kill(&thisv->actor);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST); // "Already initialized"
        return;
    }

    sTakaraIsInitialized = true;
    osSyncPrintf("\n\n");
    // "Bun! %x" (needs a better translation)
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ ばぅん！ ☆☆☆☆☆ %x\n" VT_RST, globalCtx->actorCtx.flags.chest);
    globalCtx->actorCtx.flags.chest = 0;
    gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] = -1;
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ts_Skel_004FE0, &object_ts_Anim_000498, thisv->jointTable,
                       thisv->morphTable, 10);
    thisx->focus.pos = thisx->world.pos;
    thisv->pos = thisx->world.pos;
    thisx->world.pos.x = 133.0f;
    thisx->world.pos.y = -12.0f;
    thisx->world.pos.z = 102.0f;
    Actor_SetScale(&thisv->actor, 0.013f);
    thisv->height = 90.0f;
    thisv->originalRoomNum = thisx->room;
    thisx->room = -1;
    thisx->world.rot.y = thisx->shape.rot.y = -0x4E20;
    thisx->targetMode = 1;
    thisv->actionFunc = func_80B176E0;
}

void func_80B176E0(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ts_Anim_000498);

    Animation_Change(&thisv->skelAnime, &object_ts_Anim_000498, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    if (!thisv->unk_214) {
        thisv->actor.textId = 0x6D;
        thisv->dialogState = TEXT_STATE_CHOICE;
    }
    thisv->actionFunc = func_80B1778C;
}

void func_80B1778C(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    s16 absYawDiff;
    s16 yawDiff;

    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx) && thisv->dialogState != TEXT_STATE_DONE) {
        if (!thisv->unk_214) {
            thisv->actionFunc = func_80B17934;
        } else {
            thisv->actionFunc = func_80B17B14;
        }
    } else {
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        if (globalCtx->roomCtx.curRoom.num == 6 && !thisv->unk_21A) {
            thisv->actor.textId = 0x6E;
            thisv->unk_21A = 1;
            thisv->dialogState = TEXT_STATE_DONE;
        }

        if (!thisv->unk_21A && thisv->unk_214) {
            if (Flags_GetSwitch(globalCtx, 0x32)) {
                thisv->actor.textId = 0x84;
                thisv->dialogState = TEXT_STATE_EVENT;
            } else {
                thisv->actor.textId = 0x704C;
                thisv->dialogState = TEXT_STATE_DONE;
            }
        }

        absYawDiff = ABS(yawDiff);
        if (absYawDiff < 0x4300) {
            if (globalCtx->roomCtx.curRoom.num != thisv->originalRoomNum) {
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                thisv->unk_218 = 0;
            } else {
                if (!thisv->unk_218) {
                    thisv->actor.flags |= ACTOR_FLAG_0;
                    thisv->unk_218 = 1;
                }
                func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
            }
        }
    }
}

void func_80B17934(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    if (thisv->dialogState == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // Yes
                if (gSaveContext.rupees >= 10) {
                    Message_CloseTextbox(globalCtx);
                    Rupees_ChangeBy(-10);
                    thisv->unk_214 = 1;
                    thisv->actor.parent = NULL;
                    func_8002F434(&thisv->actor, globalCtx, GI_DOOR_KEY, 2000.0f, 1000.0f);
                    thisv->actionFunc = func_80B17A6C;
                } else {
                    Message_CloseTextbox(globalCtx);
                    thisv->actor.textId = 0x85;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->dialogState = TEXT_STATE_EVENT;
                    thisv->actionFunc = func_80B17B14;
                }
                break;
            case 1: // No
                Message_CloseTextbox(globalCtx);
                thisv->actor.textId = 0x2D;
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                thisv->dialogState = TEXT_STATE_EVENT;
                thisv->actionFunc = func_80B17B14;
                break;
        }
    }
}

void func_80B17A6C(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = func_80B17AC4;
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_DOOR_KEY, 2000.0f, 1000.0f);
    }
}

void func_80B17AC4(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = func_80B176E0;
    }
}

void func_80B17B14(EnTakaraMan* thisv, GlobalContext* globalCtx) {
    if (thisv->dialogState == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = func_80B176E0;
    }
}

void EnTakaraMan_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnTakaraMan* thisv = (EnTakaraMan*)thisx;

    if (thisv->eyeTimer != 0) {
        thisv->eyeTimer--;
    }

    Actor_SetFocus(&thisv->actor, thisv->height);
    func_80038290(globalCtx, &thisv->actor, &thisv->unk_22C, &thisv->unk_232, thisv->actor.focus.pos);
    if (thisv->eyeTimer == 0) {
        thisv->eyeTextureIdx++;
        if (thisv->eyeTextureIdx >= 2) {
            thisv->eyeTextureIdx = 0;
            thisv->eyeTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    thisv->unk_212++;
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnTakaraMan_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    EnTakaraMan* thisv = (EnTakaraMan*)thisx;

    if (limbIndex == 1) {
        rot->x += thisv->unk_232.y;
    }
    if (limbIndex == 8) {
        rot->x += thisv->unk_22C.y;
        rot->z += thisv->unk_22C.z;
    }
    return false;
}

void EnTakaraMan_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = {
        object_ts_Tex_000970,
        object_ts_Tex_000D70,
    };
    EnTakaraMan* thisv = (EnTakaraMan*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_takara_man.c", 528);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnTakaraMan_OverrideLimbDraw, NULL, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_takara_man.c", 544);
}
