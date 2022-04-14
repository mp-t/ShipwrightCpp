/*
 * File: z_demo_im.c
 * Overlay: Demo_Im
 * Description: Impa
 */

#include "z_demo_im.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "scenes/indoors/nakaniwa/nakaniwa_scene.h"
#include "objects/object_im/object_im.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

void DemoIm_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoIm_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_809856F8(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985718(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985738(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985770(DemoIm* thisv, GlobalContext* globalCtx);
void func_809857B0(DemoIm* thisv, GlobalContext* globalCtx);
void func_809857F0(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985830(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985C10(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985C40(DemoIm* thisv, GlobalContext* globalCtx);
void func_80985C94(DemoIm* thisv, GlobalContext* globalCtx);
void DemoIm_DrawTranslucent(DemoIm* thisv, GlobalContext* globalCtx);
void func_809863BC(DemoIm* thisv, GlobalContext* globalCtx);
void func_809863DC(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986430(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986494(DemoIm* thisv, GlobalContext* globalCtx);
void func_809864D4(DemoIm* thisv, GlobalContext* globalCtx);
void func_809868E8(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986908(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986948(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986D40(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986DC8(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986E20(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986E40(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986EAC(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986F08(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986F28(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986F88(DemoIm* thisv, GlobalContext* globalCtx);
void func_80986FA8(DemoIm* thisv, GlobalContext* globalCtx);
void func_80987288(DemoIm* thisv, GlobalContext* globalCtx);
void func_809872A8(DemoIm* thisv, GlobalContext* globalCtx);
void func_809872F0(DemoIm* thisv, GlobalContext* globalCtx);
void func_80987330(DemoIm* thisv, GlobalContext* globalCtx);
void DemoIm_DrawNothing(DemoIm* thisv, GlobalContext* globalCtx);
void DemoIm_DrawSolid(DemoIm* thisv, GlobalContext* globalCtx);

static void* sEyeTextures[] = {
    gImpaEyeOpenTex,
    gImpaEyeHalfTex,
    gImpaEyeClosedTex,
};

static u32 D_8098783C = 0;

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 25, 80, 0, { 0, 0, 0 } },
};

#include "z_demo_im_cutscene_data.c" EARLY

static DemoImActionFunc sActionFuncs[] = {
    func_809856F8, func_80985718, func_80985738, func_80985770, func_809857B0, func_809857F0, func_80985830,
    func_80985C10, func_80985C40, func_80985C94, func_809863BC, func_809863DC, func_80986430, func_80986494,
    func_809864D4, func_809868E8, func_80986908, func_80986948, func_80986D40, func_80986DC8, func_80986E20,
    func_80986E40, func_80986EAC, func_80986F08, func_80986F28, func_80986F88, func_80986FA8, func_80987288,
    func_809872A8, func_809872F0, func_80987330,
};

static Vec3f D_809887D8 = { 0.0f, 10.0f, 0.0f };

static DemoImDrawFunc sDrawFuncs[] = {
    DemoIm_DrawNothing,
    DemoIm_DrawSolid,
    DemoIm_DrawTranslucent,
};

const ActorInit Demo_Im_InitVars = {
    ACTOR_DEMO_IM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_IM,
    sizeof(DemoIm),
    (ActorFunc)DemoIm_Init,
    (ActorFunc)DemoIm_Destroy,
    (ActorFunc)DemoIm_Update,
    (ActorFunc)DemoIm_Draw,
    NULL,
};

void func_80984BE0(DemoIm* thisv) {
    s32 pad[3];
    s16* blinkTimer = &thisv->blinkTimer;
    s16* eyeIndex = &thisv->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void func_80984C68(DemoIm* thisv) {
    thisv->action = 7;
    thisv->drawConfig = 0;
    thisv->alpha = 0;
    thisv->unk_270 = 0;
    thisv->actor.shape.shadowAlpha = 0;
    thisv->unk_268 = 0.0f;
}

void func_80984C8C(DemoIm* thisv, GlobalContext* globalCtx) {
    u32* something = &D_8098783C;

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (thisv->actor.params == 2) {
                func_80984C68(thisv);
            }
            *something = 0;
        }
    } else {
        if (*something == 0) {
            *something = 1;
        }
    }
}

void DemoIm_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* thisv = (DemoIm*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
}

void DemoIm_DestroyCollider(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* thisv = (DemoIm*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void DemoIm_UpdateCollider(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void func_80984DB8(DemoIm* thisv) {
    s32 pad[2];
    Vec3s* vec1 = &thisv->unk_2D4.unk_08;
    Vec3s* vec2 = &thisv->unk_2D4.unk_0E;

    Math_SmoothStepToS(&vec1->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec1->y, 0, 20, 6200, 100);

    Math_SmoothStepToS(&vec2->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&vec2->y, 0, 20, 6200, 100);
}

void func_80984E58(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 yawDiff;
    s16 phi_a3;

    thisv->unk_2D4.unk_18 = player->actor.world.pos;
    thisv->unk_2D4.unk_14 = kREG(16) + 4.0f;

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
    phi_a3 = (ABS(yawDiff) < 0x18E3) ? 2 : 1;
    func_80034A14(&thisv->actor, &thisv->unk_2D4, kREG(17) + 0xC, phi_a3);
}

void func_80984F10(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_2D4.unk_18 = player->actor.world.pos;
    thisv->unk_2D4.unk_14 = kREG(16) + 12.0f;

    func_80034A14(&thisv->actor, &thisv->unk_2D4, kREG(17) + 0xC, 2);
}

void func_80984F94(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_2D4.unk_18 = player->actor.world.pos;
    thisv->unk_2D4.unk_14 = kREG(16) + 4.0f;
    func_80034A14(&thisv->actor, &thisv->unk_2D4, kREG(17) + 0xC, 4);
}

void DemoIm_UpdateBgCheckInfo(DemoIm* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 DemoIm_UpdateSkelAnime(DemoIm* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

s32 DemoIm_IsCsStateIdle(GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorAction* DemoIm_GetNpcAction(GlobalContext* globalCtx, s32 actionIdx) {
    s32 pad[2];
    CsCmdActorAction* ret = NULL;

    if (!DemoIm_IsCsStateIdle(globalCtx)) {
        ret = globalCtx->csCtx.npcActions[actionIdx];
    }
    return ret;
}

s32 func_809850E8(DemoIm* thisv, GlobalContext* globalCtx, u16 action, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        if (npcAction->action == action) {
            return true;
        }
    }
    return false;
}

s32 func_80985134(DemoIm* thisv, GlobalContext* globalCtx, u16 action, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        if (npcAction->action != action) {
            return true;
        }
    }
    return false;
}

void func_80985180(DemoIm* thisv, GlobalContext* globalCtx, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_80985200(DemoIm* thisv, GlobalContext* globalCtx, s32 actionIdx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, actionIdx);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
}

void DemoIm_ChangeAnim(DemoIm* thisv, AnimationHeader* animHeaderSeg, u8 animMode, f32 transitionRate,
                       s32 playBackwards) {
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (!playBackwards) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        endFrame = 0.0f;
        startFrame = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animHeaderSeg, playbackSpeed, startFrame, endFrame, animMode, transitionRate);
}

void func_80985310(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->actor.shape.yOffset = -10000.0f;
}

void func_80985358(DemoIm* thisv, GlobalContext* globalCtx) {
    f32 posX = thisv->actor.world.pos.x;
    f32 posY = thisv->actor.world.pos.y;
    f32 posZ = thisv->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void func_809853B4(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerX = player->actor.world.pos.x;
    f32 playerY = player->actor.world.pos.y + 80.0f;
    f32 playerZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_EFFECT, playerX, playerY, playerZ, 0,
                       0, 0, 0xD);
    Item_Give(globalCtx, ITEM_MEDALLION_SHADOW);
}

void func_80985430(DemoIm* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.yOffset += 250.0f / 3.0f;
}

void func_8098544C(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 pad[2];

    if ((gSaveContext.chamberCutsceneNum == 4) && (gSaveContext.sceneSetupIndex < 4)) {
        Player* player = GET_PLAYER(globalCtx);

        thisv->action = 1;
        globalCtx->csCtx.segment = D_8098786C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(globalCtx, ITEM_MEDALLION_SHADOW);
        player->actor.world.rot.y = player->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
    }
}

void func_809854DC(DemoIm* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[5] != NULL) &&
        (globalCtx->csCtx.npcActions[5]->action == 2)) {
        Animation_Change(&thisv->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        thisv->action = 2;
        thisv->drawConfig = 1;
        func_80985358(thisv, globalCtx);
    }
}

void func_8098557C(DemoIm* thisv) {
    if (thisv->actor.shape.yOffset >= 0.0f) {
        thisv->action = 3;
        thisv->actor.shape.yOffset = 0.0f;
    }
}

void func_809855A8(DemoIm* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[5] != NULL) &&
        (globalCtx->csCtx.npcActions[5]->action == 3)) {
        Animation_Change(&thisv->skelAnime, &gImpaRaiseArmsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaRaiseArmsAnim),
                         ANIMMODE_ONCE, 4.0f);
        thisv->action = 4;
    }
}

void func_80985640(DemoIm* thisv, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gImpaPresentShadowMedallionAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPresentShadowMedallionAnim), ANIMMODE_LOOP, 0.0f);
        thisv->action = 5;
    }
}

void func_809856AC(DemoIm* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[6] != NULL) &&
        (globalCtx->csCtx.npcActions[6]->action == 2)) {
        thisv->action = 6;
        func_809853B4(thisv, globalCtx);
    }
}

void func_809856F8(DemoIm* thisv, GlobalContext* globalCtx) {
    func_8098544C(thisv, globalCtx);
}

void func_80985718(DemoIm* thisv, GlobalContext* globalCtx) {
    func_809854DC(thisv, globalCtx);
}

void func_80985738(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80985430(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_8098557C(thisv);
}

void func_80985770(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809855A8(thisv, globalCtx);
}

void func_809857B0(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    sp1C = DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80985640(thisv, sp1C);
}

void func_809857F0(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809856AC(thisv, globalCtx);
}

void func_80985830(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
}

void func_80985860(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = 7;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_809858A8(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void DemoIm_SpawnLightBall(DemoIm* thisv, GlobalContext* globalCtx) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_6K, thisv->actor.world.pos.x,
                       (kREG(17) + 24.0f) + thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 6);
}

void func_80985948(DemoIm* thisv, GlobalContext* globalCtx) {
    if (func_809850E8(thisv, globalCtx, 4, 5)) {
        Animation_Change(&thisv->skelAnime, &gImpaPrepareSealGanonAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gImpaPrepareSealGanonAnim), ANIMMODE_ONCE, 0.0f);
        thisv->action = 8;
        thisv->drawConfig = 2;
        thisv->alpha = 0;
        thisv->actor.shape.shadowAlpha = 0;
        thisv->unk_268 = 0.0f;
        func_809858A8();
    }
}

void func_809859E0(DemoIm* thisv, GlobalContext* globalCtx) {
    f32* unk_268 = &thisv->unk_268;
    s32 alpha = 255;

    if (func_809850E8(thisv, globalCtx, 4, 5)) {
        *unk_268 += 1.0f;
        if (*unk_268 >= kREG(5) + 10.0f) {
            thisv->action = 9;
            thisv->drawConfig = 1;
            *unk_268 = kREG(5) + 10.0f;
            thisv->alpha = thisv->actor.shape.shadowAlpha = alpha;
            return;
        }
    } else {
        *unk_268 -= 1.0f;
        if (*unk_268 <= 0.0f) {
            thisv->action = 7;
            thisv->drawConfig = 0;
            *unk_268 = 0.0f;
            thisv->alpha = 0;
            thisv->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    thisv->actor.shape.shadowAlpha = thisv->alpha = (*unk_268 / (kREG(5) + 10.0f)) * 255.0f;
}

void func_80985B34(DemoIm* thisv, GlobalContext* globalCtx) {
    if (func_80985134(thisv, globalCtx, 4, 5)) {
        Animation_Change(&thisv->skelAnime, &gImpaSealGanonAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaSealGanonAnim),
                         ANIMMODE_ONCE, -8.0f);
        thisv->action = 8;
        thisv->drawConfig = 2;
        thisv->unk_268 = kREG(5) + 10.0f;
        thisv->alpha = 255;
        if (thisv->unk_270 == 0) {
            DemoIm_SpawnLightBall(thisv, globalCtx);
            thisv->unk_270 = 1;
        }
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80985C10(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80985948(thisv, globalCtx);
    func_80984C8C(thisv, globalCtx);
}

void func_80985C40(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809859E0(thisv, globalCtx);
    func_80984C8C(thisv, globalCtx);
}

void func_80985C94(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80985B34(thisv, globalCtx);
    func_80984C8C(thisv, globalCtx);
}

void DemoIm_DrawTranslucent(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    void* eyeTex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 281);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_im_inKenjyanomaDemo02.c", 308);
}

void func_80985E60(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = 10;
    thisv->unk_280 = 1;
}

void func_80985EAC(DemoIm* thisv, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.frames >= 80) && (globalCtx->csCtx.frames < 243)) {
        func_80984F10(thisv, globalCtx);
    } else {
        func_80984DB8(thisv);
    }
}

void func_80985EF4(DemoIm* thisv) {
    if (!Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&gImpaWhistlingAnim) - 1.0f)) {
        DemoIm_UpdateSkelAnime(thisv);
    }
}

void func_80985F54(DemoIm* thisv) {
    thisv->action = 10;
    thisv->drawConfig = 0;
}

void func_80985F64(DemoIm* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(thisv, globalCtx, 5);
    thisv->action = 11;
    thisv->drawConfig = 1;
}

void func_80985FE8(DemoIm* thisv, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaWhistlingAnim),
                         ANIMMODE_LOOP, -8.0f);
    }
}

void func_8098604C(DemoIm* thisv) {
    f32 frameCount = Animation_GetLastFrame(&gImpaStartWhistlingAnim);

    Animation_Change(&thisv->skelAnime, &gImpaStartWhistlingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    thisv->action = 12;
    thisv->drawConfig = 1;
    thisv->unk_2D0 = 1;
}

void func_809860C8(DemoIm* thisv) {
    thisv->action = 13;
    thisv->drawConfig = 1;
}

void func_809860DC(DemoIm* thisv, s32 arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                         ANIMMODE_LOOP, -8.0f);
        thisv->unk_2D0 = 0;
    }
}

void func_80986148(DemoIm* thisv) {
    Animation_Change(&thisv->skelAnime, &gImpaStartWhistlingAnim, -1.0f,
                     Animation_GetLastFrame(&gImpaStartWhistlingAnim), 0.0f, ANIMMODE_ONCE, -8.0f);
    thisv->action = 14;
    thisv->drawConfig = 1;
}

void func_809861C4(DemoIm* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = thisv->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 9:
                    func_80986148(thisv);
                    break;
                case 7:
                    Animation_Change(&thisv->skelAnime, &gImpaWhistlingAnim, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&gImpaWhistlingAnim), ANIMMODE_LOOP, -8.0f);
                    thisv->action = 12;
                    break;
                default:
                    osSyncPrintf("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_274 = action;
        }
    }
}

void func_8098629C(DemoIm* thisv, GlobalContext* globalCtx) {
    if (DemoIm_IsCsStateIdle(globalCtx)) {
        thisv->action = 21;
        thisv->drawConfig = 1;
        thisv->unk_280 = 1;
    }
}

void func_809862E0(DemoIm* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = thisv->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 1:
                    func_80985F54(thisv);
                    break;
                case 2:
                    func_80985F64(thisv, globalCtx);
                    break;
                case 7:
                    func_8098604C(thisv);
                    break;
                case 8:
                    func_809860C8(thisv);
                    break;
                case 9:
                    func_80986148(thisv);
                    break;
                default:
                    osSyncPrintf("Demo_Im_Ocarina_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_274 = action;
        }
    }
}

void func_809863BC(DemoIm* thisv, GlobalContext* globalCtx) {
    func_809862E0(thisv, globalCtx);
}

void func_809863DC(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80985EAC(thisv, globalCtx);
    func_80984BE0(thisv);
    func_809862E0(thisv, globalCtx);
}

void func_80986430(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(thisv);
    func_80985EAC(thisv, globalCtx);
    func_80984BE0(thisv);
    func_80985FE8(thisv, sp24);
    func_809862E0(thisv, globalCtx);
}

void func_80986494(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    func_80985EF4(thisv);
    func_80984BE0(thisv);
    func_809861C4(thisv, globalCtx);
}

void func_809864D4(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809860DC(thisv, sp24);
    func_8098629C(thisv, globalCtx);
}

void func_8098652C(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = 15;
}

void func_80986570(DemoIm* thisv, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&thisv->skelAnime, 7.0f) && (thisv->actor.bgCheckFlags & 1)) {
        u32 sfxId = SFX_FLAG;

        sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
        Audio_PlaySoundGeneral(sfxId, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void func_809865F8(DemoIm* thisv, GlobalContext* globalCtx, s32 arg2) {
    s32 pad[2];

    if (arg2 != 0) {
        f32* unk_278 = &thisv->unk_278;

        if (*unk_278 >= 0.0f) {
            if (thisv->unk_27C == 0) {
                Vec3f* thisPos = &thisv->actor.world.pos;
                s16 shapeRotY = thisv->actor.shape.rot.y;
                f32 spawnPosX = thisPos->x + (Math_SinS(shapeRotY) * 30.0f);
                f32 spawnPosY = thisPos->y;
                f32 spawnPosZ = thisPos->z + (Math_CosS(shapeRotY) * 30.0f);

                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ARROW, spawnPosX, spawnPosY, spawnPosZ, 0xFA0,
                            thisv->actor.shape.rot.y, 0, ARROW_CS_NUT);
                thisv->unk_27C = 1;
            }
        } else {
            *unk_278 += 1.0f;
        }
    }
}

void func_80986700(DemoIm* thisv) {
    thisv->action = 15;
    thisv->drawConfig = 0;
}

void func_80986710(DemoIm* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gImpaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gImpaIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    func_80985180(thisv, globalCtx, 5);
    thisv->action = 16;
    thisv->drawConfig = 1;
}

void func_80986794(DemoIm* thisv) {
    Animation_Change(&thisv->skelAnime, &gImpaThrowDekuNutAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gImpaThrowDekuNutAnim), ANIMMODE_ONCE, -8.0f);
    thisv->action = 17;
    thisv->drawConfig = 1;
}

void func_8098680C(DemoIm* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = thisv->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 1:
                    func_80986700(thisv);
                    break;
                case 2:
                    func_80986710(thisv, globalCtx);
                    break;
                case 10:
                    func_80986794(thisv);
                    break;
                case 11:
                    Actor_Kill(&thisv->actor);
                    break;
                default:
                    osSyncPrintf("Demo_Im_Spot00_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_274 = action;
        }
    }
}

void func_809868E8(DemoIm* thisv, GlobalContext* globalCtx) {
    func_8098680C(thisv, globalCtx);
}

void func_80986908(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_8098680C(thisv, globalCtx);
}

void func_80986948(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 sp24;

    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    sp24 = DemoIm_UpdateSkelAnime(thisv);
    func_80986570(thisv, globalCtx);
    func_80984BE0(thisv);
    func_809865F8(thisv, globalCtx, sp24);
    func_8098680C(thisv, globalCtx);
}

void func_809869B0(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = 18;
    thisv->actor.shape.shadowAlpha = 0;
}

s32 func_809869F8(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = thisv->actor.world.pos.x;

    if ((thisPosX - (kREG(16) + 30.0f) > playerPosX) && !(thisv->actor.flags & ACTOR_FLAG_6)) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986A5C(DemoIm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 playerPosX = player->actor.world.pos.x;
    f32 thisPosX = thisv->actor.world.pos.x;

    if ((thisPosX - (kREG(17) + 130.0f) < playerPosX) && (!Gameplay_InCsMode(globalCtx))) {
        return true;
    } else {
        return false;
    }
}

s32 func_80986AD0(DemoIm* thisv, GlobalContext* globalCtx) {
    thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
    if (!Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actor.textId = 0x708E;
        func_8002F2F4(&thisv->actor, globalCtx);
    } else {
        return true;
    }
    return false;
}

void func_80986B2C(GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        Player* player = GET_PLAYER(globalCtx);

        globalCtx->nextEntranceIndex = 0xCD;
        globalCtx->fadeTransition = 38;
        globalCtx->sceneLoadFlag = 0x14;
        func_8002DF54(globalCtx, &player->actor, 8);
    }
}

void func_80986BA0(DemoIm* thisv, GlobalContext* globalCtx) {
    if (func_809869F8(thisv, globalCtx)) {
        thisv->action = 21;
        thisv->drawConfig = 1;
        thisv->unk_280 = 1;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986BE4(DemoIm* thisv, s32 arg1) {
    if (arg1 != 0) {
        thisv->action = 22;
    }
}

void func_80986BF8(DemoIm* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.eventChkInf[4] & 1) {
        thisv->action = 24;
        thisv->drawConfig = 1;
        thisv->unk_280 = 1;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986C30(DemoIm* thisv, GlobalContext* globalCtx) {
    if (func_80986A5C(thisv, globalCtx)) {
        globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gZeldasCourtyardLullabyCs);
        gSaveContext.cutsceneTrigger = 1;
        gSaveContext.eventChkInf[5] |= 0x200;
        Item_Give(globalCtx, ITEM_SONG_LULLABY);
        func_80985F54(thisv);
    }
}

void func_80986CC8(DemoIm* thisv) {
    if (gSaveContext.eventChkInf[4] & 1) {
        thisv->action = 26;
        thisv->drawConfig = 1;
        thisv->unk_280 = 1;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80986CFC(DemoIm* thisv, GlobalContext* globalCtx) {
    if (func_80986A5C(thisv, globalCtx)) {
        gSaveContext.eventChkInf[4] |= 0x1000;
        thisv->action = 19;
    }
}

void func_80986D40(DemoIm* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.sceneSetupIndex == 6) {
        thisv->action = 19;
        thisv->drawConfig = 1;
    } else if (gSaveContext.eventChkInf[8] & 1) {
        Actor_Kill(&thisv->actor);
    } else if (!(gSaveContext.eventChkInf[5] & 0x200)) {
        thisv->action = 23;
    } else {
        thisv->action = 20;
    }
}

void func_80986DC8(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80984E58(thisv, globalCtx);
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
}

void func_80986E20(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80986BA0(thisv, globalCtx);
}

void func_80986E40(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80984E58(thisv, globalCtx);
    DemoIm_UpdateCollider(thisv, globalCtx);
    func_80986BE4(thisv, func_80986AD0(thisv, globalCtx));
}

void func_80986EAC(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80984F94(thisv, globalCtx);
    DemoIm_UpdateCollider(thisv, globalCtx);
    func_80986B2C(globalCtx);
}

void func_80986F08(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80986BF8(thisv, globalCtx);
}

void func_80986F28(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80984E58(thisv, globalCtx);
    DemoIm_UpdateCollider(thisv, globalCtx);
    func_80986C30(thisv, globalCtx);
}

void func_80986F88(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80986CC8(thisv);
}

void func_80986FA8(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80984E58(thisv, globalCtx);
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    DemoIm_UpdateCollider(thisv, globalCtx);
    func_80986CFC(thisv, globalCtx);
}

void func_80987018(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_ChangeAnim(thisv, &gImpaIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = 27;
    thisv->drawConfig = 0;
    thisv->actor.shape.shadowAlpha = 0;
}

void func_80987064(DemoIm* thisv) {
    f32* unk_268 = &thisv->unk_268;
    f32 temp;
    s32 alpha = 255;

    *unk_268 += 1.0f;
    temp = kREG(17) + 10.0f;

    if (*unk_268 >= temp) {
        thisv->actor.shape.shadowAlpha = thisv->alpha = alpha;
    } else {
        thisv->actor.shape.shadowAlpha = thisv->alpha = (*unk_268 / temp) * 255.0f;
    }
}

void func_809870F0(DemoIm* thisv, GlobalContext* globalCtx) {
    func_80985200(thisv, globalCtx, 5);
    thisv->action = 28;
    thisv->drawConfig = 2;
}

void func_80987128(DemoIm* thisv) {
    if (thisv->unk_268 >= kREG(17) + 10.0f) {
        thisv->action = 29;
        thisv->drawConfig = 1;
    }
}

void func_80987174(DemoIm* thisv) {
    DemoIm_ChangeAnim(thisv, &object_im_Anim_0101C8, ANIMMODE_ONCE, -8.0f, false);
    thisv->action = 30;
}

void func_809871B4(DemoIm* thisv, s32 arg1) {
    if (arg1 != 0) {
        DemoIm_ChangeAnim(thisv, &object_im_Anim_00FB10, ANIMMODE_LOOP, 0.0f, false);
    }
}

void func_809871E8(DemoIm* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = DemoIm_GetNpcAction(globalCtx, 5);

    if (npcAction != NULL) {
        u32 action = npcAction->action;
        u32 unk_274 = thisv->unk_274;

        if (action != unk_274) {
            switch (action) {
                case 12:
                    func_809870F0(thisv, globalCtx);
                    break;
                case 13:
                    func_80987174(thisv);
                    break;
                default:
                    osSyncPrintf("Demo_Im_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            thisv->unk_274 = action;
        }
    }
}

void func_80987288(DemoIm* thisv, GlobalContext* globalCtx) {
    func_809871E8(thisv, globalCtx);
}

void func_809872A8(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_80987064(thisv);
    func_80987128(thisv);
}

void func_809872F0(DemoIm* thisv, GlobalContext* globalCtx) {
    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809871E8(thisv, globalCtx);
}

void func_80987330(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 sp1C;

    DemoIm_UpdateBgCheckInfo(thisv, globalCtx);
    sp1C = DemoIm_UpdateSkelAnime(thisv);
    func_80984BE0(thisv);
    func_809871B4(thisv, sp1C);
}

void DemoIm_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* thisv = (DemoIm*)thisx;

    if ((thisv->action < 0) || (thisv->action >= 31) || (sActionFuncs[thisv->action] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[thisv->action](thisv, globalCtx);
}

void DemoIm_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* thisv = (DemoIm*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    DemoIm_InitCollider(thisx, globalCtx);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gImpaSkel, NULL, thisv->jointTable, thisv->morphTable, 17);
    thisx->flags &= ~ACTOR_FLAG_0;

    switch (thisv->actor.params) {
        case 2:
            func_80985860(thisv, globalCtx);
            break;
        case 3:
            func_80985E60(thisv, globalCtx);
            break;
        case 4:
            func_8098652C(thisv, globalCtx);
            break;
        case 5:
            func_809869B0(thisv, globalCtx);
            break;
        case 6:
            func_80987018(thisv, globalCtx);
            break;
        default:
            func_80985310(thisv, globalCtx);
    }
}

void DemoIm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm_DestroyCollider(thisx, globalCtx);
}

s32 DemoIm_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoIm* thisv = (DemoIm*)thisx;
    s32* unk_2D0 = &thisv->unk_2D0;

    if (thisv->unk_280 != 0) {
        Vec3s* unk_2D4_unk_0E = &thisv->unk_2D4.unk_0E;
        Vec3s* unk_2D4_unk_08 = &thisv->unk_2D4.unk_08;

        switch (limbIndex) {
            case IMPA_LIMB_CHEST:
                rot->x += unk_2D4_unk_0E->y;
                rot->y -= unk_2D4_unk_0E->x;
                break;
            case IMPA_LIMB_HEAD:
                rot->x += unk_2D4_unk_08->y;
                rot->z += unk_2D4_unk_08->x;
                break;
        }
    }

    if ((*unk_2D0 != 0) && (limbIndex == IMPA_LIMB_RIGHT_HAND)) {
        *dList = gImpaHandPointingDL;
    }

    return false;
}

void DemoIm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIm* thisv = (DemoIm*)thisx;

    if (limbIndex == IMPA_LIMB_HEAD) {
        Vec3f sp28 = D_809887D8;
        Vec3f dest;

        Matrix_MultVec3f(&sp28, &dest);
        thisv->actor.focus.pos.x = dest.x;
        thisv->actor.focus.pos.y = dest.y;
        thisv->actor.focus.pos.z = dest.z;
        thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
        thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
        thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    }
}

void DemoIm_DrawNothing(DemoIm* thisv, GlobalContext* globalCtx) {
}

void DemoIm_DrawSolid(DemoIm* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    void* eyeTexture = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_im.c", 904);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoIm_OverrideLimbDraw, DemoIm_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_im.c", 925);
}

void DemoIm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoIm* thisv = (DemoIm*)thisx;

    if ((thisv->drawConfig < 0) || (thisv->drawConfig >= 3) || (sDrawFuncs[thisv->drawConfig] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[thisv->drawConfig](thisv, globalCtx);
}
