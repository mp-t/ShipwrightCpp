/*
 * File: z_demo_sa.c
 * Overlay: Demo_Sa
 * Description: Saria (Cutscene)
 */

#include "z_demo_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/object_sa/object_sa.h"

#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void DemoSa_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoSa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoSa_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoSa_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8098EBB8(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098EBD8(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098EBF8(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098EC28(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098EC60(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098EC94(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098ECCC(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F0E8(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F118(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F16C(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F3F0(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F714(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F734(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F77C(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F7BC(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098F7FC(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098FC44(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098FC64(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098FC9C(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098FCD4(DemoSa* thisv, GlobalContext* globalCtx);
void func_8098FD0C(DemoSa* thisv, GlobalContext* globalCtx);

void DemoSa_DrawNothing(DemoSa* thisv, GlobalContext* globalCtx);
void DemoSa_DrawOpa(DemoSa* thisv, GlobalContext* globalCtx);
void DemoSa_DrawXlu(DemoSa* thisv, GlobalContext* globalCtx);

typedef enum {
    /* 0 */ SARIA_EYE_OPEN,
    /* 1 */ SARIA_EYE_HALF,
    /* 2 */ SARIA_EYE_CLOSED,
    /* 3 */ SARIA_EYE_SUPRISED,
    /* 4 */ SARIA_EYE_SAD
} SariaEyeState;

typedef enum {
    /* 0 */ SARIA_MOUTH_CLOSED2,
    /* 1 */ SARIA_MOUTH_SUPRISED,
    /* 2 */ SARIA_MOUTH_CLOSED,
    /* 3 */ SARIA_MOUTH_SMILING_OPEN,
    /* 4 */ SARIA_MOUTH_FROWNING
} SariaMouthState;

static const void* sEyeTextures[] = {
    gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
};

static const void* sMouthTextures[] = {
    gSariaMouthClosed2Tex,     gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
};

static u32 D_80990108 = 0;

#include "z_demo_sa_cutscene_data.cpp" EARLY

static DemoSaActionFunc sActionFuncs[] = {
    func_8098EBB8, func_8098EBD8, func_8098EBF8, func_8098EC28, func_8098EC60, func_8098EC94, func_8098ECCC,
    func_8098F0E8, func_8098F118, func_8098F16C, func_8098F3F0, func_8098F714, func_8098F734, func_8098F77C,
    func_8098F7BC, func_8098F7FC, func_8098FC44, func_8098FC64, func_8098FC9C, func_8098FCD4, func_8098FD0C,
};

static DemoSaDrawFunc sDrawFuncs[] = {
    DemoSa_DrawNothing,
    DemoSa_DrawOpa,
    DemoSa_DrawXlu,
};

ActorInit Demo_Sa_InitVars = {
    ACTOR_DEMO_SA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SA,
    sizeof(DemoSa),
    (ActorFunc)DemoSa_Init,
    (ActorFunc)DemoSa_Destroy,
    (ActorFunc)DemoSa_Update,
    (ActorFunc)DemoSa_Draw,
    NULL,
};

void DemoSa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DemoSa* thisv = (DemoSa*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
}

void func_8098E480(DemoSa* thisv) {
    s32 pad[2];
    s16* eyeIndex = &thisv->eyeIndex;
    s16* blinkTimer = &thisv->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(0x3C, 0x3C);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void DemoSa_SetEyeIndex(DemoSa* thisv, s16 eyeIndex) {
    thisv->eyeIndex = eyeIndex;
}

void DemoSa_SetMouthIndex(DemoSa* thisv, s16 mouthIndex) {
    thisv->mouthIndex = mouthIndex;
}

void func_8098E530(DemoSa* thisv) {
    thisv->action = 7;
    thisv->drawConfig = 0;
    thisv->alpha = 0;
    thisv->unk_1A8 = 0;
    thisv->actor.shape.shadowAlpha = 0;
    thisv->unk_1A0 = 0.0f;
}

void func_8098E554(DemoSa* thisv, GlobalContext* globalCtx) {
    u32* something = &D_80990108;

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (thisv->actor.params == 2) {
                func_8098E530(thisv);
            }
            *something = 0;
        }
    } else if (*something == 0) {
        *something = 1;
    }
}

void func_8098E5C8(DemoSa* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 DemoSa_UpdateSkelAnime(DemoSa* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

const CsCmdActorAction* DemoSa_GetNpcAction(GlobalContext* globalCtx, s32 idx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[idx];
    }
    return NULL;
}

s32 func_8098E654(DemoSa* thisv, GlobalContext* globalCtx, u16 arg2, s32 arg3) {
    const CsCmdActorAction* npcAction = DemoSa_GetNpcAction(globalCtx, arg3);

    if ((npcAction != NULL) && (npcAction->action == arg2)) {
        return 1;
    }
    return 0;
}

s32 func_8098E6A0(DemoSa* thisv, GlobalContext* globalCtx, u16 arg2, s32 arg3) {
    const CsCmdActorAction* npcAction = DemoSa_GetNpcAction(globalCtx, arg3);

    if ((npcAction != NULL) && (npcAction->action != arg2)) {
        return 1;
    }
    return 0;
}

void func_8098E6EC(DemoSa* thisv, GlobalContext* globalCtx, s32 actionIdx) {
    const CsCmdActorAction* npcAction = DemoSa_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_8098E76C(DemoSa* thisv, const AnimationHeader* animHeaderSeg, u8 arg2, f32 transitionRate, s32 arg4) {
    s32 pad[2];
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        fc = 0.0f;
        unk0 = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animHeaderSeg, playbackSpeed, unk0, fc, arg2, transitionRate);
}

void func_8098E7FC(DemoSa* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    thisv->actor.shape.yOffset = -10000.0f;
    DemoSa_SetEyeIndex(thisv, SARIA_EYE_HALF);
    DemoSa_SetMouthIndex(thisv, SARIA_MOUTH_CLOSED2);
}

void func_8098E86C(DemoSa* thisv, GlobalContext* globalCtx) {
    Vec3f* world = &thisv->actor.world.pos;
    f32 posX = world->x;
    f32 posY = world->y;
    f32 posZ = world->z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void func_8098E8C8(DemoSa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       0xB);
    Item_Give(globalCtx, ITEM_MEDALLION_FOREST);
}

void func_8098E944(DemoSa* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.yOffset += (250.0f / 3.0f);
}

void func_8098E960(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == 0) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(globalCtx);
        thisv->action = 1;
        globalCtx->csCtx.segment = D_8099010C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(globalCtx, ITEM_MEDALLION_FOREST);
        player->actor.world.rot.y = player->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
    }
}

void func_8098E9EC(DemoSa* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* npcAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        npcAction = globalCtx->csCtx.npcActions[4];
        if ((npcAction != NULL) && (npcAction->action == 2)) {
            thisv->action = 2;
            thisv->drawConfig = 1;
            func_8098E86C(thisv, globalCtx);
        }
    }
}

void func_8098EA3C(DemoSa* thisv) {
    if (thisv->actor.shape.yOffset >= 0.0f) {
        thisv->action = 3;
        thisv->actor.shape.yOffset = 0.0f;
    }
}

void func_8098EA68(DemoSa* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* npcAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        npcAction = globalCtx->csCtx.npcActions[4];
        if ((npcAction != NULL) && (npcAction->action == 3)) {
            Animation_Change(&thisv->skelAnime, &gSariaGiveForestMedallionAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gSariaGiveForestMedallionAnim), ANIMMODE_ONCE, -4.0f);
            thisv->action = 4;
        }
    }
}

void func_8098EB00(DemoSa* thisv, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gSariaGiveForestMedallionStandAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSariaGiveForestMedallionStandAnim), ANIMMODE_LOOP, 0.0f);
        thisv->action = 5;
    }
}

void func_8098EB6C(DemoSa* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* npcAction;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        npcAction = globalCtx->csCtx.npcActions[6];
        if ((npcAction != NULL) && (npcAction->action == 2)) {
            thisv->action = 6;
            func_8098E8C8(thisv, globalCtx);
        }
    }
}

void func_8098EBB8(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E960(thisv, globalCtx);
}

void func_8098EBD8(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E9EC(thisv, globalCtx);
}

void func_8098EBF8(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E944(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098EA3C(thisv);
}

void func_8098EC28(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098EA68(thisv, globalCtx);
}

void func_8098EC60(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    func_8098EB00(thisv, DemoSa_UpdateSkelAnime(thisv));
}

void func_8098EC94(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098EB6C(thisv, globalCtx);
}

void func_8098ECCC(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
}

void func_8098ECF4(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSariaSealGanonAnim);

    SkelAnime_InitFlex(globalCtx, skelAnime, &gSariaSkel, NULL, NULL, NULL, 0);
    Animation_Change(skelAnime, &gSariaSealGanonAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    thisv->action = 7;
    thisv->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(thisv, SARIA_EYE_CLOSED);
    DemoSa_SetMouthIndex(thisv, SARIA_MOUTH_CLOSED);
}

void func_8098EDB0(DemoSa* thisv) {
    f32 curFrame = thisv->skelAnime.curFrame;

    if ((thisv->skelAnime.mode == 2) && (curFrame >= 32.0f)) {
        DemoSa_SetEyeIndex(thisv, SARIA_EYE_HALF);
        DemoSa_SetMouthIndex(thisv, SARIA_MOUTH_CLOSED2);
    }
}

void func_8098EE08(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void func_8098EE28(DemoSa* thisv, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_6K, thisv->actor.world.pos.x,
                       (kREG(23) + 25.0f) + thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 4);
}

void func_8098EEA8(DemoSa* thisv, GlobalContext* globalCtx) {
    if (func_8098E654(thisv, globalCtx, 4, 4)) {
        thisv->action = 8;
        thisv->drawConfig = 2;
        thisv->alpha = 0;
        thisv->actor.shape.shadowAlpha = 0;
        thisv->unk_1A0 = 0.0f;
        func_8098EE08();
    }
}

void func_8098EEFC(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 alpha = 255;
    f32* unk_1A0 = &thisv->unk_1A0;

    if (func_8098E654(thisv, globalCtx, 4, 4)) {
        *unk_1A0 += 1.0f;
        if ((kREG(5) + 10.0f) <= *unk_1A0) {
            thisv->action = 9;
            thisv->drawConfig = 1;
            *unk_1A0 = kREG(5) + 10.0f;
            thisv->alpha = alpha;
            thisv->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_1A0 -= 1.0f;
        if (*unk_1A0 <= 0.0f) {
            thisv->action = 7;
            thisv->drawConfig = 0;
            *unk_1A0 = 0.0f;
            thisv->alpha = 0;
            thisv->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    thisv->actor.shape.shadowAlpha = thisv->alpha = (*unk_1A0 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_8098F050(DemoSa* thisv, GlobalContext* globalCtx) {
    if (func_8098E6A0(thisv, globalCtx, 4, 4)) {
        thisv->action = 8;
        thisv->drawConfig = 2;
        thisv->unk_1A0 = kREG(5) + 10.0f;
        thisv->alpha = 255;
        if (thisv->unk_1A8 == 0) {
            func_8098EE28(thisv, globalCtx);
            thisv->unk_1A8 = 1;
        }
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098F0E8(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098EEA8(thisv, globalCtx);
    func_8098E554(thisv, globalCtx);
}

void func_8098F118(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
    func_8098EEFC(thisv, globalCtx);
    func_8098E554(thisv, globalCtx);
}

void func_8098F16C(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098EDB0(thisv);
    func_8098F050(thisv, globalCtx);
    func_8098E554(thisv, globalCtx);
}

void DemoSa_DrawXlu(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    const void* sp78 = sEyeTextures[eyeIndex];
    s16 mouthIndex = thisv->mouthIndex;
    s32 pad2;
    const void* sp6C = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 296);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sp78));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(sp6C));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_sa_inKenjyanomaDemo02.c", 325);
}

void func_8098F390(DemoSa* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSariaSkel, &gSariaWaitArmsToSideAnim, NULL, NULL, 0);
    thisv->action = 10;
    thisv->drawConfig = 1;
}

void func_8098F3F0(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
}

void func_8098F420(DemoSa* thisv, GlobalContext* globalCtx) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSariaSkel, &gSariaSitting3Anim, NULL, NULL, 0);
    thisv->action = 11;
    thisv->drawConfig = 0;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_8098F480(DemoSa* thisv) {
    s32 alpha = 255;
    f32* unk_1A0 = &thisv->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        thisv->actor.shape.shadowAlpha = thisv->alpha = alpha;
    } else {
        thisv->actor.shape.shadowAlpha = thisv->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void func_8098F50C(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E6EC(thisv, globalCtx, 4);
    thisv->action = 12;
    thisv->drawConfig = 2;
}

void func_8098F544(DemoSa* thisv) {
    if (thisv->unk_1A0 >= kREG(17) + 10.0f) {
        thisv->action = 13;
        thisv->drawConfig = 1;
    }
}

void func_8098F590(DemoSa* thisv) {
    func_8098E76C(thisv, &gSariaSitting1Anim, 2, -8.0f, 0);
    thisv->action = 14;
}

void func_8098F5D0(DemoSa* thisv) {
    func_8098E76C(thisv, &gSariaSitting2Anim, 2, 0.0f, 0);
    thisv->action = 15;
}

void func_8098F610(DemoSa* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(thisv, &gSariaSitting3Anim, 0, 0.0f, 0);
        thisv->action = 13;
    }
}

void func_8098F654(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 unk_1AC;
    s32 action;
    const CsCmdActorAction* npcAction = DemoSa_GetNpcAction(globalCtx, 4);

    if (npcAction != NULL) {
        action = npcAction->action;
        unk_1AC = thisv->unk_1AC;
        if (action != unk_1AC) {
            switch (action) {
                case 7:
                    func_8098F50C(thisv, globalCtx);
                    break;
                case 8:
                    func_8098F590(thisv);
                    break;
                case 9:
                    func_8098F5D0(thisv);
                    break;
                default:
                    osSyncPrintf("Demo_Sa_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_1AC = action;
        }
    }
}

void func_8098F714(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098F654(thisv, globalCtx);
}

void func_8098F734(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
    func_8098F480(thisv);
    func_8098F544(thisv);
}

void func_8098F77C(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
    func_8098F654(thisv, globalCtx);
}

void func_8098F7BC(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
    func_8098F654(thisv, globalCtx);
}

void func_8098F7FC(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 sp1C;

    func_8098E5C8(thisv, globalCtx);
    sp1C = DemoSa_UpdateSkelAnime(thisv);
    func_8098E480(thisv);
    func_8098F610(thisv, sp1C);
}

void func_8098F83C(DemoSa* thisv, GlobalContext* globalCtx) {
    Vec3f* thisPos = &thisv->actor.world.pos;

    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSariaSkel, &gSariaWaitOnBridgeAnim, NULL, NULL, 0);
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_ELF, thisPos->x, thisPos->y, thisPos->z,
                       0, 0, 0, FAIRY_KOKIRI);
    thisv->action = 16;
    thisv->drawConfig = 0;
    thisv->actor.shape.shadowAlpha = 0;
    DemoSa_SetEyeIndex(thisv, SARIA_EYE_SAD);
    DemoSa_SetMouthIndex(thisv, SARIA_MOUTH_CLOSED);
}

void func_8098F8F8(DemoSa* thisv) {
    s32 alpha = 255;
    f32* unk_1A0 = &thisv->unk_1A0;
    f32 temp_f0;

    *unk_1A0 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;

    if (temp_f0 <= *unk_1A0) {
        thisv->actor.shape.shadowAlpha = thisv->alpha = alpha;
    } else {
        thisv->actor.shape.shadowAlpha = thisv->alpha = (*unk_1A0 / temp_f0) * 255.0f;
    }
}

void func_8098F984(DemoSa* thisv) {
    thisv->action = 16;
    thisv->drawConfig = 0;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_8098F998(DemoSa* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1AC == 4) {
        func_8098E6EC(thisv, globalCtx, 1);
        thisv->action = 17;
        thisv->drawConfig = 2;
        thisv->unk_1B0 = 0;
        thisv->actor.shape.shadowAlpha = 0;
    } else {
        func_8098E76C(thisv, &gSariaWaitOnBridgeAnim, 0, 0.0f, 0);
        thisv->action = 18;
        thisv->drawConfig = 1;
        thisv->unk_1B0 = 0;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
    DemoSa_SetEyeIndex(thisv, SARIA_EYE_SAD);
}

void func_8098FA2C(DemoSa* thisv) {
    if (thisv->unk_1A0 >= kREG(17) + 10.0f) {
        thisv->action = 18;
        thisv->drawConfig = 1;
        thisv->unk_1B0 = 0;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_8098FA84(DemoSa* thisv) {
    func_8098E76C(thisv, &gSariaHoldOcarinaAnim, 0, 0.0f, 0);
    thisv->action = 19;
    thisv->drawConfig = 1;
    thisv->unk_1B0 = 1;
    thisv->actor.shape.shadowAlpha = 0xFF;
    DemoSa_SetEyeIndex(thisv, SARIA_EYE_CLOSED);
}

void func_8098FAE0(DemoSa* thisv) {
    func_8098E76C(thisv, &gSariaGiveLinkOcarinaAnim, 2, -8.0f, 0);
    thisv->action = 20;
    thisv->drawConfig = 1;
    thisv->unk_1B0 = 1;
    thisv->actor.shape.shadowAlpha = 0xFF;
}

void func_8098FB34(DemoSa* thisv, s32 arg1) {
    if (arg1 != 0) {
        func_8098E76C(thisv, &gSariaHoldOutOcarinaAnim, 0, 0, 0);
    }
}

void func_8098FB68(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 unk_1AC;
    s32 action;
    const CsCmdActorAction* npcAction = DemoSa_GetNpcAction(globalCtx, 1);

    if (npcAction != NULL) {
        action = npcAction->action;
        unk_1AC = thisv->unk_1AC;
        if (action != unk_1AC) {
            switch (action) {
                case 4:
                    func_8098F984(thisv);
                    break;
                case 12:
                    func_8098F998(thisv, globalCtx);
                    break;
                case 13:
                    func_8098FA84(thisv);
                    break;
                case 14:
                    func_8098FAE0(thisv);
                    break;
                default:
                    osSyncPrintf("Demo_Sa_inPresent_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_1AC = action;
        }
    }
}

void func_8098FC44(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098FB68(thisv, globalCtx);
}

void func_8098FC64(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098F8F8(thisv);
    func_8098FA2C(thisv);
}

void func_8098FC9C(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098FB68(thisv, globalCtx);
}

void func_8098FCD4(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    DemoSa_UpdateSkelAnime(thisv);
    func_8098FB68(thisv, globalCtx);
}

void func_8098FD0C(DemoSa* thisv, GlobalContext* globalCtx) {
    func_8098E5C8(thisv, globalCtx);
    func_8098FB34(thisv, DemoSa_UpdateSkelAnime(thisv));
    func_8098FB68(thisv, globalCtx);
}

void DemoSa_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoSa* thisv = (DemoSa*)thisx;

    if (thisv->action < 0 || thisv->action >= 21 || sActionFuncs[thisv->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[thisv->action](thisv, globalCtx);
}

void DemoSa_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoSa* thisv = (DemoSa*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);

    switch (thisv->actor.params) {
        case 2:
            func_8098ECF4(thisv, globalCtx);
            break;
        case 3:
            func_8098F390(thisv, globalCtx);
            break;
        case 4:
            func_8098F420(thisv, globalCtx);
            break;
        case 5:
            func_8098F83C(thisv, globalCtx);
            break;
        default:
            func_8098E7FC(thisv, globalCtx);
    }
}

s32 DemoSa_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoSa* thisv = (DemoSa*)thisx;

    if ((limbIndex == 15) && (thisv->unk_1B0 != 0)) {
        *dList = gSariaRightHandAndOcarinaDL;
    }
    return false;
}

void DemoSa_DrawNothing(DemoSa* thisv, GlobalContext* globalCtx) {
}

void DemoSa_DrawOpa(DemoSa* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    const void* eyeTex = sEyeTextures[eyeIndex];
    s32 pad2;
    s16 mouthIndex = thisv->mouthIndex;
    const void* mouthTex = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_sa.c", 602);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoSa_OverrideLimbDraw, NULL, &thisv->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_sa.c", 626);
}

void DemoSa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoSa* thisv = (DemoSa*)thisx;

    if (thisv->drawConfig < 0 || thisv->drawConfig >= 3 || sDrawFuncs[thisv->drawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[thisv->drawConfig](thisv, globalCtx);
}
