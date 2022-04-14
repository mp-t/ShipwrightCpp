/*
 * File: z_en_kakasi.c
 * Overlay: ovl_En_Kakasi
 * Description: Pierre the Scarecrow
 */

#include "z_en_kakasi.h"
#include "vt.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_25)

void EnKakasi_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A8F660(EnKakasi* thisv, GlobalContext* globalCtx);
void func_80A8F75C(EnKakasi* thisv, GlobalContext* globalCtx);
void func_80A8F8D0(EnKakasi* thisv, GlobalContext* globalCtx);
void func_80A8F9C8(EnKakasi* thisv, GlobalContext* globalCtx);
void func_80A8FBB8(EnKakasi* thisv, GlobalContext* globalCtx);
void func_80A8FAA4(EnKakasi* thisv, GlobalContext* globalCtx);

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
        BUMP_NONE | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

const ActorInit En_Kakasi_InitVars = {
    ACTOR_EN_KAKASI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi),
    (ActorFunc)EnKakasi_Init,
    (ActorFunc)EnKakasi_Destroy,
    (ActorFunc)EnKakasi_Update,
    (ActorFunc)EnKakasi_Draw,
    NULL,
};

void EnKakasi_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi* thisv = (EnKakasi*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    SkelAnime_Free(&thisv->skelanime, globalCtx); // OTR - Fixed thisv memory leak
    //! @bug SkelAnime_Free is not called
}

void EnKakasi_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi* thisv = (EnKakasi*)thisx;

    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ Ｌｅｔ’ｓ ＤＡＮＣＥ！ ☆☆☆☆☆ %f\n" VT_RST, thisv->actor.world.pos.y);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.targetMode = 6;
    SkelAnime_InitFlex(globalCtx, &thisv->skelanime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);

    thisv->rot = thisv->actor.world.rot;
    thisv->actor.flags |= ACTOR_FLAG_10;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;

    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actionFunc = func_80A8F660;
}

void func_80A8F28C(EnKakasi* thisv) {
    thisv->unk_1A4 = 0;
    thisv->skelanime.playSpeed = 0.0f;
    thisv->unk_1A8 = thisv->unk_1AC = 0;

    Math_ApproachZeroF(&thisv->skelanime.curFrame, 0.5f, 1.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->rot.x, 5, 0x2710, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->rot.y, 5, 0x2710, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->rot.z, 5, 0x2710, 0);
}

void func_80A8F320(EnKakasi* thisv, GlobalContext* globalCtx, s16 arg) {
    s16 ocarinaNote = globalCtx->msgCtx.lastOcaNoteIdx;
    s16 currentFrame;

    if (arg != 0) {
        if (thisv->unk_19C[3] == 0) {
            thisv->unk_19C[3] = (s16)Rand_ZeroFloat(10.99f) + 30;
            thisv->unk_1A6 = (s16)Rand_ZeroFloat(4.99f);
        }

        thisv->unk_19A = (s16)Rand_ZeroFloat(2.99f) + 5;
        ocarinaNote = thisv->unk_1A6;
    }
    switch (ocarinaNote) {
        case OCARINA_NOTE_A:
            thisv->unk_19A++;
            if (thisv->unk_1A4 == 0) {
                thisv->unk_1A4 = 1;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KAKASHI_ROLL);
            }
            break;
        case OCARINA_NOTE_C_DOWN:
            thisv->unk_19A++;
            thisv->unk_1B8 = 1.0f;
            break;
        case OCARINA_NOTE_C_RIGHT:
            thisv->unk_19A++;
            if (thisv->unk_1AC == 0) {
                thisv->unk_1AC = 0x1388;
            }
            break;
        case OCARINA_NOTE_C_LEFT:
            thisv->unk_19A++;
            if (thisv->unk_1A8 == 0) {
                thisv->unk_1A8 = 0x1388;
            }
            break;
        case OCARINA_NOTE_C_UP:
            thisv->unk_19A++;
            thisv->unk_1B8 = 2.0f;
            break;
    }

    if (thisv->unk_19A > 8) {
        thisv->unk_19A = 8;
    }

    if (thisv->unk_19A != 0) {
        thisv->actor.gravity = -1.0f;
        if (thisv->unk_19A == 8 && (thisv->actor.bgCheckFlags & 1)) {
            thisv->actor.velocity.y = 3.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_KAKASHI_JUMP);
        }
        Math_ApproachF(&thisv->skelanime.playSpeed, thisv->unk_1B8, 0.1f, 0.2f);
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->unk_1A8, 5, 0x3E8, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->unk_1AC, 5, 0x3E8, 0);

        if (thisv->unk_1A8 != 0 && fabsf(thisv->actor.shape.rot.x - thisv->unk_1A8) < 50.0f) {
            thisv->unk_1A8 *= -1.0f;
        }
        if (thisv->unk_1AC != 0 && fabsf(thisv->actor.shape.rot.z - thisv->unk_1AC) < 50.0f) {
            thisv->unk_1AC *= -1.0f;
        }

        if (thisv->unk_1A4 != 0) {
            thisv->actor.shape.rot.y += 0x1000;
            if (thisv->actor.shape.rot.y == 0) {
                thisv->unk_1A4 = 0;
            }
        }
        currentFrame = thisv->skelanime.curFrame;
        if (currentFrame == 11 || currentFrame == 17) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KAKASHI_SWING);
        }
        SkelAnime_Update(&thisv->skelanime);
    }
}

void func_80A8F660(EnKakasi* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&thisv->skelanime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);

    thisv->actor.textId = 0x4076;
    thisv->unk_196 = TEXT_STATE_DONE;
    if (!LINK_IS_ADULT) {
        thisv->unk_194 = false;
        if (gSaveContext.scarecrowCustomSongSet) {
            thisv->actor.textId = 0x407A;
            thisv->unk_196 = TEXT_STATE_EVENT;
        }
    } else {
        thisv->unk_194 = true;
        if (gSaveContext.scarecrowCustomSongSet) {
            thisv->actor.textId = 0x4079;
            thisv->unk_196 = TEXT_STATE_EVENT;
        }
    }
    thisv->actionFunc = func_80A8F75C;
}

void func_80A8F75C(EnKakasi* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    func_80A8F28C(thisv);
    SkelAnime_Update(&thisv->skelanime);
    thisv->camId = SUBCAM_NONE;
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->unk_196 == TEXT_STATE_EVENT) {
            thisv->actionFunc = func_80A8F9C8;
        } else {
            thisv->actionFunc = func_80A8F660;
        }
    } else {
        s16 yawTowardsPlayer = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;

        if (!(thisv->actor.xzDistToPlayer > 120.0f)) {
            s16 absyawTowardsPlayer = ABS(yawTowardsPlayer);

            if (absyawTowardsPlayer < 0x4300) {
                if (!thisv->unk_194) {
                    if (player->stateFlags2 & 0x1000000) {
                        thisv->camId = OnePointCutscene_Init(globalCtx, 2260, -99, &thisv->actor, MAIN_CAM);

                        func_8010BD58(globalCtx, OCARINA_ACTION_SCARECROW_LONG_RECORDING);
                        thisv->unk_19A = 0;
                        thisv->unk_1B8 = 0.0;
                        player->stateFlags2 |= 0x800000;
                        thisv->actionFunc = func_80A8F8D0;
                        return;
                    }
                    if (thisv->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= 0x800000;
                    }
                }
                func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
            }
        }
    }
}

void func_80A8F8D0(EnKakasi* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04 && globalCtx->msgCtx.msgMode == MSGMODE_NONE) {
        // "end?"
        osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆☆ 終り？ ☆☆☆☆☆ \n" VT_RST);

        if (thisv->unk_19A != 0) {
            Message_CloseTextbox(globalCtx);
            thisv->actor.textId = 0x4077;
            thisv->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
            thisv->actionFunc = func_80A8F9C8;
        } else {
            OnePointCutscene_EndCutscene(globalCtx, thisv->camId);
            thisv->camId = SUBCAM_NONE;
            thisv->actionFunc = func_80A8F660;
        }
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        func_80A8F320(thisv, globalCtx, 0);
        player->stateFlags2 |= 0x800000;
    }
}

void func_80A8F9C8(EnKakasi* thisv, GlobalContext* globalCtx) {
    func_80A8F28C(thisv);
    SkelAnime_Update(&thisv->skelanime);
    func_8002DF54(globalCtx, NULL, 8);

    if (thisv->unk_196 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {

        if (thisv->camId != SUBCAM_NONE) {
            func_8005B1A4(globalCtx->cameraPtrs[thisv->camId]);
        }
        thisv->camId = OnePointCutscene_Init(globalCtx, 2270, -99, &thisv->actor, MAIN_CAM);
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        func_8002DF54(globalCtx, NULL, 8);
        func_8010BD58(globalCtx, OCARINA_ACTION_SCARECROW_LONG_PLAYBACK);
        thisv->actionFunc = func_80A8FAA4;
    }
}

void func_80A8FAA4(EnKakasi* thisv, GlobalContext* globalCtx) {
    if (globalCtx->msgCtx.ocarinaMode != OCARINA_MODE_0F) {
        func_80A8F320(thisv, globalCtx, 1);
        return;
    }

    osSyncPrintf("game_play->message.msg_mode=%d\n", globalCtx->msgCtx.msgMode);

    if (globalCtx->msgCtx.msgMode == MSGMODE_NONE) {
        if (thisv->unk_194) {
            thisv->actor.textId = 0x4077;
            thisv->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        } else {
            thisv->actor.textId = 0x4078;
            thisv->unk_196 = TEXT_STATE_EVENT;
            Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        }
        thisv->actionFunc = func_80A8FBB8;
        OnePointCutscene_EndCutscene(globalCtx, thisv->camId);
        thisv->camId = SUBCAM_NONE;
        thisv->camId = OnePointCutscene_Init(globalCtx, 2260, -99, &thisv->actor, MAIN_CAM);
        func_8005B1A4(globalCtx->cameraPtrs[thisv->camId]);
    }
}

void func_80A8FBB8(EnKakasi* thisv, GlobalContext* globalCtx) {
    func_80A8F28C(thisv);
    SkelAnime_Update(&thisv->skelanime);

    if (thisv->unk_196 == Message_GetState(&globalCtx->msgCtx) && Message_ShouldAdvance(globalCtx)) {
        func_8005B1A4(globalCtx->cameraPtrs[thisv->camId]);
        Message_CloseTextbox(globalCtx);
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actionFunc = func_80A8F660;
    }
}

void EnKakasi_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi* thisv = (EnKakasi*)thisx;
    s32 pad;
    s32 i;

    thisv->unk_198++;
    thisv->actor.world.rot = thisv->actor.shape.rot;
    for (i = 0; i < ARRAY_COUNT(thisv->unk_19C); i++) {
        if (thisv->unk_19C[i] != 0) {
            thisv->unk_19C[i]--;
        }
    }

    thisv->height = 60.0f;
    Actor_SetFocus(&thisv->actor, thisv->height);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 28);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnKakasi_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi* thisv = (EnKakasi*)thisx;

    if (BREG(3) != 0) {
        osSyncPrintf("\n\n");
        // "flag!"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ フラグ！ ☆☆☆☆☆ %d\n" VT_RST, gSaveContext.scarecrowCustomSongSet);
    }
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, thisv->skelanime.dListCount,
                          NULL, NULL, thisv);
}
