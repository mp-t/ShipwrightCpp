#include "z_en_heishi4.h"
#include "objects/object_sd/object_sd.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnHeishi4_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi4_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi4_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHeishi4_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A56544(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A5673C(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56328(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A563BC(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56B40(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56614(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56874(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56900(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56994(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56A50(EnHeishi4* thisv, GlobalContext* globalCtx);
void func_80A56ACC(EnHeishi4* thisv, GlobalContext* globalCtx);

const ActorInit En_Heishi4_InitVars = {
    ACTOR_EN_HEISHI4,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SD,
    sizeof(EnHeishi4),
    (ActorFunc)EnHeishi4_Init,
    (ActorFunc)EnHeishi4_Destroy,
    (ActorFunc)EnHeishi4_Update,
    (ActorFunc)EnHeishi4_Draw,
    NULL,
};

static u32 sFaceReactionSets[] = { 6, 7 };

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
    { 33, 40, 0, { 0, 0, 0 } },
};

void EnHeishi4_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHeishi4* thisv = (EnHeishi4*)thisx;

    Actor_SetScale(thisx, 0.01f);
    thisv->type = thisx->params & 0xFF;
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    thisv->pos = thisx->world.pos;
    thisx->targetMode = 6;
    if (thisv->type == HEISHI4_AT_MARKET_DYING) {
        thisv->height = 30.0f;
        ActorShape_Init(&thisx->shape, 0.0f, NULL, 30.0f);
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gEnHeishiSkel, &gEnHeishiDyingGuardAnim_00C444, thisv->jointTable,
                       thisv->morphTable, 17);
    } else {
        thisv->height = 60.0f;
        ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, thisv->jointTable,
                       thisv->morphTable, 17);
    }
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sCylinderInit);
    thisv->collider.dim.yShift = 0;
    thisv->collider.dim.radius = 15;
    thisv->collider.dim.height = 70;
    switch (thisv->type) {
        case HEISHI4_AT_KAKRIKO_ENTRANCE:
        case HEISHI4_AT_IMPAS_HOUSE:
            thisv->actionFunc = func_80A56328;
            break;
        case HEISHI4_AT_MARKET_DYING:
            thisv->collider.dim.radius = 28;
            thisv->collider.dim.height = 5;
            thisv->actionFunc = func_80A5673C;
            break;
        case HEISHI4_AT_MARKET_NIGHT:
            thisv->actionFunc = func_80A56544;
            break;
    }
    thisv->unk_27C = ((thisx->params >> 8) & 0xFF);
    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 兵士２セット完了！ ☆☆☆☆☆ %d\n" VT_RST, thisx->params);
    osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 識別完了！\t    ☆☆☆☆☆ %d\n" VT_RST, thisv->type);
    osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ メッセージ完了！   ☆☆☆☆☆ %x\n\n" VT_RST, (thisx->params >> 8) & 0xF);
    osSyncPrintf("\n\n");
}

void EnHeishi4_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHeishi4* thisv = (EnHeishi4*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80A56328(EnHeishi4* thisv, GlobalContext* globalCtx) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = func_80A563BC;
}

void func_80A563BC(EnHeishi4* thisv, GlobalContext* globalCtx) {
    s16 reactionOffset;

    thisv->unk_2B4 = 0;
    reactionOffset = thisv->type - 4;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(globalCtx, sFaceReactionSets[reactionOffset]) != 0) {
        thisv->actor.textId = Text_GetFaceReaction(globalCtx, sFaceReactionSets[reactionOffset]);
        thisv->unk_2B4 = 1;
        thisv->actionFunc = func_80A56B40;
    } else {
        if (gSaveContext.eventChkInf[8] & 1) {
            thisv->actor.textId = 0x5065;
            thisv->actionFunc = func_80A56B40;
            return;
        }
        if (gSaveContext.eventChkInf[4] & 0x20) {
            thisv->actor.textId = 0x5068;
            thisv->actionFunc = func_80A56B40;
            return;
        }
        if (thisv->type == HEISHI4_AT_IMPAS_HOUSE) {
            if (thisv->unk_284 == 0) {
                thisv->actor.textId = 0x5079;
            } else {
                thisv->actor.textId = 0x507A;
            }
        } else if (IS_DAY) {
            if (thisv->unk_284 == 0) {
                thisv->actor.textId = 0x5063;
            } else {
                thisv->actor.textId = 0x5064;
            }
        } else if (thisv->unk_284 == 0) {
            thisv->actor.textId = 0x5066;
        } else {
            thisv->actor.textId = 0x5067;
        }

        thisv->actionFunc = func_80A56B40;
    }
}

void func_80A56544(EnHeishi4* thisv, GlobalContext* globalCtx) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ ぎゃぁ！オトナだー ☆☆☆☆☆ \n" VT_RST);
        Actor_Kill(&thisv->actor);
    } else {
        thisv->actionFunc = func_80A56614;
    }
}

void func_80A56614(EnHeishi4* thisv, GlobalContext* globalCtx) {
    s16 reactionOffset;

    reactionOffset = thisv->type - 4;
    thisv->unk_2B4 = 0;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(globalCtx, sFaceReactionSets[reactionOffset]) != 0) {
        thisv->actor.textId = Text_GetFaceReaction(globalCtx, sFaceReactionSets[reactionOffset]);
        thisv->unk_2B4 = 1;
        thisv->actionFunc = func_80A56B40;
        return;
    }
    if (globalCtx->sceneNum == SCENE_MIHARIGOYA) {
        if (IS_DAY) {
            thisv->actor.textId = 0x7004;
        } else {
            thisv->actor.textId = 0x709A;
        }
    } else if (globalCtx->sceneNum != SCENE_MARKET_NIGHT) {
        if (IS_DAY) {
            thisv->actor.textId = 0x7002;
        } else {
            thisv->actor.textId = 0x7003;
        }
    } else {
        thisv->actor.textId = 0x7003;
    }

    thisv->actionFunc = func_80A56B40;
}

void func_80A5673C(EnHeishi4* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[4] & 0x20) {
        osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ マスターソード祝入手！ ☆☆☆☆☆ \n" VT_RST);
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->unk_284 = 0;
    if (gSaveContext.eventChkInf[8] & 1) {
        if (!(gSaveContext.infTable[6] & 0x1000)) {
            f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardAnim_00C444);
            Animation_Change(&thisv->skelAnime, &gEnHeishiDyingGuardAnim_00C444, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP,
                             -10.0f);
            thisv->actor.textId = 0x7007;
            thisv->unk_282 = TEXT_STATE_EVENT;
            thisv->unk_284 = 1;
            osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ デモ開始！ ☆☆☆☆☆ \n" VT_RST);
        } else {
            thisv->actor.textId = 0x7008;
            thisv->unk_282 = TEXT_STATE_DONE;
            osSyncPrintf(VT_FGCOL(BLUE) " ☆☆☆☆☆ 返事なし ☆☆☆☆☆ \n" VT_RST);
        }
        thisv->actionFunc = func_80A56874;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void func_80A56874(EnHeishi4* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_284 != 0) {
        SkelAnime_Update(&thisv->skelAnime);
    }
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->unk_284 == 0) {
            thisv->actionFunc = func_80A5673C;

        } else {
            thisv->actionFunc = func_80A56900;
        }
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
    }
}

void func_80A56900(EnHeishi4* thisv, GlobalContext* globalCtx) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardTalkAnim);

    Animation_Change(&thisv->skelAnime, &gEnHeishiDyingGuardTalkAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = func_80A56994;
}

void func_80A56994(EnHeishi4* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    func_80038290(globalCtx, &thisv->actor, &thisv->unk_260, &thisv->unk_266, thisv->actor.focus.pos);
    if ((thisv->unk_282 == Message_GetState(&globalCtx->msgCtx)) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        gSaveContext.infTable[6] |= 0x1000;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = func_80A56A50;
    }
}

void func_80A56A50(EnHeishi4* thisv, GlobalContext* globalCtx) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardDieAnim);

    thisv->unk_288 = frames;
    Animation_Change(&thisv->skelAnime, &gEnHeishiDyingGuardDieAnim, 1.0f, 0.0f, frames, ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = func_80A56ACC;
}

void func_80A56ACC(EnHeishi4* thisv, GlobalContext* globalCtx) {
    f32 currentFrame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_288 <= currentFrame) {
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actionFunc = func_80A5673C;
    }
}

void func_80A56B40(EnHeishi4* thisv, GlobalContext* globalCtx) {
    s16 reactionOffset;

    SkelAnime_Update(&thisv->skelAnime);
    reactionOffset = (thisv->type - 4);
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(globalCtx, sFaceReactionSets[reactionOffset]) != 0) {
        if (thisv->unk_2B4 == 0) {
            if ((thisv->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (thisv->type == HEISHI4_AT_IMPAS_HOUSE)) {
                thisv->actionFunc = func_80A563BC;
                return;
            }
            if (thisv->type == HEISHI4_AT_MARKET_NIGHT) {
                thisv->actionFunc = func_80A56614;
                return;
            }
        }
    } else {
        if (thisv->unk_2B4 != 0) {
            if ((thisv->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (thisv->type == HEISHI4_AT_IMPAS_HOUSE)) {
                thisv->actionFunc = func_80A563BC;
                return;
            }
            if (thisv->type == HEISHI4_AT_MARKET_NIGHT) {
                thisv->actionFunc = func_80A56614;
                return;
            }
        }
    }
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if ((thisv->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (thisv->type == HEISHI4_AT_IMPAS_HOUSE)) {
            thisv->unk_284 = 1;
            thisv->actionFunc = func_80A563BC;
            return;
        }
        if (thisv->type == HEISHI4_AT_MARKET_NIGHT) {
            thisv->actionFunc = func_80A56614;
            return;
        }
    }
    func_8002F2F4(&thisv->actor, globalCtx);
}

void EnHeishi4_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHeishi4* thisv = (EnHeishi4*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    thisx->world.pos.x = thisv->pos.x;
    thisx->world.pos.y = thisv->pos.y;
    thisx->world.pos.z = thisv->pos.z;
    Actor_SetFocus(thisx, thisv->height);
    if (thisv->type != HEISHI4_AT_MARKET_DYING) {
        thisv->unk_28C.unk_18 = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            thisv->unk_28C.unk_18.y = (player->actor.world.pos.y - 10.0f);
        }
        func_80034A14(thisx, &thisv->unk_28C, 2, 4);
        thisv->unk_260 = thisv->unk_28C.unk_08;
        thisv->unk_266 = thisv->unk_28C.unk_0E;
    }
    thisv->unk_27E += 1;
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(globalCtx, thisx, 10.0f, 10.0f, 30.0f, 0x1D);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnHeishi_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnHeishi4* thisv = (EnHeishi4*)thisx;

    if (limbIndex == 9) {
        rot->x += thisv->unk_266.y;
    }
    if (limbIndex == 16) {
        rot->x += thisv->unk_260.y;
        rot->z += thisv->unk_260.z;
    }
    return false;
}

void EnHeishi4_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHeishi4* thisv = (EnHeishi4*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnHeishi_OverrideLimbDraw, NULL,
                      thisv);
}
