/*
 * File: z_en_ru1.c
 * Overlay: En_Ru1
 * Description: Ruto (child)
 */

#include "z_en_ru1.h"
#include "objects/object_ru1/object_ru1.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_26)

void EnRu1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRu1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRu1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRu1_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AEC0B4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC100(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC130(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC17C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC1D4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC244(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEC2C0(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECA94(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECAB4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECAD4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECB18(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECB60(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECBB8(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECC1C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AECC84(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED304(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED324(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED344(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED374(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED3A4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED3E0(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AED414(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF29C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF2AC(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF2D0(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF354(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF3A8(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEBD4(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEC5C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEECF0(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEED58(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEDCC(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEE34(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEE9C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEF08(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEEF5C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEF9D8(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFA2C(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFAAC(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFB04(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFB68(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFCE8(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFBC8(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFC24(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFECC(EnRu1* thisv, GlobalContext* globalCtx);
void func_80AEFF40(EnRu1* thisv, GlobalContext* globalCtx);

void func_80AF0278(EnRu1* thisv, GlobalContext* globalCtx, s32 limbIndex, Vec3s* rot);

void EnRu1_DrawNothing(EnRu1* thisv, GlobalContext* globalCtx);
void EnRu1_DrawOpa(EnRu1* thisv, GlobalContext* globalCtx);
void EnRu1_DrawXlu(EnRu1* thisv, GlobalContext* globalCtx);

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 25, 80, 0, { 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000101, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x01, 0x00, 0x01 },
    { 20, 30, 0, { 0 } },
};

static const void* sEyeTextures[] = {
    gRutoChildEyeOpenTex,     gRutoChildEyeHalfTex,  gRutoChildEyeClosedTex,
    gRutoChildEyeRollLeftTex, gRutoChildEyeHalf2Tex, gRutoChildEyeHalfWithBlushTex,
};

static const void* sMouthTextures[] = {
    gRutoChildMouthClosedTex,
    gRutoChildMouthFrownTex,
    gRutoChildMouthOpenTex,
};

static s32 sUnused = 0;

#include "z_en_ru1_cutscene_data.cpp" EARLY

static u32 D_80AF1938 = 0;

static EnRu1ActionFunc sActionFuncs[] = {
    func_80AEC0B4, func_80AEC100, func_80AEC130, func_80AEC17C, func_80AEC1D4, func_80AEC244, func_80AEC2C0,
    func_80AECA94, func_80AECAB4, func_80AECAD4, func_80AECB18, func_80AECB60, func_80AECBB8, func_80AECC1C,
    func_80AECC84, func_80AED304, func_80AED324, func_80AED344, func_80AED374, func_80AED3A4, func_80AED3E0,
    func_80AED414, func_80AEF29C, func_80AEF2AC, func_80AEF2D0, func_80AEF354, func_80AEF3A8, func_80AEEBD4,
    func_80AEEC5C, func_80AEECF0, func_80AEED58, func_80AEEDCC, func_80AEEE34, func_80AEEE9C, func_80AEEF08,
    func_80AEEF5C, func_80AEF9D8, func_80AEFA2C, func_80AEFAAC, func_80AEFB04, func_80AEFB68, func_80AEFCE8,
    func_80AEFBC8, func_80AEFC24, func_80AEFECC, func_80AEFF40,
};

static EnRu1PreLimbDrawFunc sPreLimbDrawFuncs[] = {
    func_80AF0278,
};

static Vec3f sMultVec = { 0.0f, 10.0f, 0.0f };

static EnRu1DrawFunc sDrawFuncs[] = {
    EnRu1_DrawNothing,
    EnRu1_DrawOpa,
    EnRu1_DrawXlu,
};

ActorInit En_Ru1_InitVars = {
    ACTOR_EN_RU1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_RU1,
    sizeof(EnRu1),
    (ActorFunc)EnRu1_Init,
    (ActorFunc)EnRu1_Destroy,
    (ActorFunc)EnRu1_Update,
    (ActorFunc)EnRu1_Draw,
    NULL,
};

void func_80AEAC10(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void func_80AEAC54(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider2);
    if (thisv->unk_34C != 0) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
    } else if (thisv->actor.xzDistToPlayer > 32.0f) {
        thisv->unk_34C = 1;
    }
}

void func_80AEACDC(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[5];

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider2);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider2.base);
}

void func_80AEAD20(Actor* thisx, GlobalContext* globalCtx) {
    EnRu1* thisv = (EnRu1*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit1);

    Collider_InitCylinder(globalCtx, &thisv->collider2);
    Collider_SetCylinderType1(globalCtx, &thisv->collider2, &thisv->actor, &sCylinderInit2);
}

void EnRu1_DestroyColliders(EnRu1* thisv, GlobalContext* globalCtx) {
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    Collider_DestroyCylinder(globalCtx, &thisv->collider2);
}

void func_80AEADD8(EnRu1* thisv) {
    thisv->unk_34C = 0;
}

u8 func_80AEADE0(EnRu1* thisv) {
    u8 params = thisv->actor.params >> 8;

    return params;
}

u8 func_80AEADF0(EnRu1* thisv) {
    s16 params = thisv->actor.params;

    return params;
}

void EnRu1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnRu1* thisv = (EnRu1*)thisx;

    D_80AF1938 = 0;
    EnRu1_DestroyColliders(thisv, globalCtx);
}

void EnRu1_UpdateEyes(EnRu1* thisv) {
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

void EnRu1_SetEyeIndex(EnRu1* thisv, s16 eyeIndex) {
    thisv->eyeIndex = eyeIndex;
}

void EnRu1_SetMouthIndex(EnRu1* thisv, s16 mouthIndex) {
    thisv->mouthIndex = mouthIndex;
}

void func_80AEAECC(EnRu1* thisv, GlobalContext* globalCtx) {
    f32* velocityY = &thisv->actor.velocity.y;
    f32 velocityYHeld = *velocityY;

    *velocityY = -4.0f;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 19.0f, 25.0f, 30.0f, 7);
    *velocityY = velocityYHeld;
}

s32 EnRu1_IsCsStateIdle(GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        return true;
    }
    return false;
}

const CsCmdActorAction* func_80AEAF58(GlobalContext* globalCtx, s32 npcActionIdx) {
    s32 pad[2];
    const CsCmdActorAction* ret = NULL;

    if (!EnRu1_IsCsStateIdle(globalCtx)) {
        ret = globalCtx->csCtx.npcActions[npcActionIdx];
    }
    return ret;
}

s32 func_80AEAFA0(GlobalContext* globalCtx, u16 action, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction = func_80AEAF58(globalCtx, npcActionIdx);

    if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action == action)) {
        return true;
    }
    return false;
}

s32 func_80AEAFE0(GlobalContext* globalCtx, u16 action, s32 npcActionIdx) {
    const CsCmdActorAction* csCmdNPCAction = func_80AEAF58(globalCtx, npcActionIdx);

    if ((csCmdNPCAction != NULL) && (csCmdNPCAction->action != action)) {
        return true;
    }
    return false;
}

s32 func_80AEB020(EnRu1* thisv, GlobalContext* globalCtx) {
    Actor* actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_NPC].head;
    EnRu1* someEnRu1;

    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_RU1) {
            someEnRu1 = (EnRu1*)actorIt;
            if (someEnRu1 != thisv) {
                if ((someEnRu1->action == 31) || (someEnRu1->action == 32) || (someEnRu1->action == 24)) {
                    return true;
                }
            }
        }
        actorIt = actorIt->next;
    }
    return false;
}

BgBdanObjects* EnRu1_FindSwitch(GlobalContext* globalCtx) {
    Actor* actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_BG].head;

    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_BG_BDAN_OBJECTS && actorIt->params == 0) {
            return (BgBdanObjects*)actorIt;
        }
        actorIt = actorIt->next;
    }
    // "There is no stand"
    osSyncPrintf(VT_FGCOL(RED) "お立ち台が無い!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    return NULL;
}

void func_80AEB0EC(EnRu1* thisv, s32 cameraSetting) {
    if (thisv->unk_28C != NULL) {
        thisv->unk_28C->cameraSetting = cameraSetting;
    }
}

s32 func_80AEB104(EnRu1* thisv) {
    if (thisv->unk_28C != NULL) {
        return thisv->unk_28C->cameraSetting;
    } else {
        return 0;
    }
}

Actor* func_80AEB124(GlobalContext* globalCtx) {
    Actor* actorIt = globalCtx->actorCtx.actorLists[ACTORCAT_BOSS].head;

    while (actorIt != NULL) {
        if ((actorIt->id == ACTOR_DEMO_EFFECT) && ((actorIt->params & 0xFF) == 0x15)) {
            return actorIt;
        }
        actorIt = actorIt->next;
    }
    return NULL;
}

s32 func_80AEB174(GlobalContext* globalCtx) {
    return (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx);
}

s32 func_80AEB1B4(GlobalContext* globalCtx) {
    return Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING;
}

void func_80AEB1D8(EnRu1* thisv) {
    thisv->action = 36;
    thisv->drawConfig = 0;
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.velocity.z = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
    func_80AEB0EC(thisv, 0);
}

void func_80AEB220(EnRu1* thisv, GlobalContext* globalCtx) {
    if ((EnRu1_IsCsStateIdle(globalCtx)) && (thisv->actor.params == 0xA)) {
        func_80AEB1D8(thisv);
    }
}

void func_80AEB264(EnRu1* thisv, const AnimationHeader* animation, u8 arg2, f32 transitionRate, s32 arg4) {
    s32 pad[2];
    const AnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);
    f32 frameCount = Animation_GetLastFrame(animHeader);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animHeader, playbackSpeed, unk0, fc, arg2, transitionRate);
}

s32 EnRu1_UpdateSkelAnime(EnRu1* thisv) {
    // why?
    if (thisv->action != 32) {
        return SkelAnime_Update(&thisv->skelAnime);
    } else {
        return SkelAnime_Update(&thisv->skelAnime);
    }
}

void func_80AEB364(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80AEB3A4(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 1;
    func_80AEB364(thisv, globalCtx);
}

void func_80AEB3CC(EnRu1* thisv) {
    thisv->skelAnime.moveFlags &= ~0x1;
}

void func_80AEB3DC(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB264(thisv, &gRutoChildWaitHandsBehindBackAnim, 0, 0, 0);
    thisv->action = 0;
    thisv->drawConfig = 1;
    EnRu1_SetEyeIndex(thisv, 4);
    EnRu1_SetMouthIndex(thisv, 0);
}

const CsCmdActorAction* func_80AEB438(GlobalContext* globalCtx) {
    return func_80AEAF58(globalCtx, 3);
}

s32 func_80AEB458(GlobalContext* globalCtx, u16 action) {
    return func_80AEAFA0(globalCtx, action, 3);
}

s32 func_80AEB480(GlobalContext* globalCtx, u16 action) {
    return func_80AEAFE0(globalCtx, action, 3);
}

void EnRu1_SpawnRipple(EnRu1* thisv, GlobalContext* globalCtx, s16 radiusMax, s16 life) {
    Vec3f pos;
    Actor* thisx = &thisv->actor;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
    pos.z = thisv->actor.world.pos.z;
    EffectSsGRipple_Spawn(globalCtx, &pos, 100, radiusMax, life);
}

void func_80AEB50C(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->unk_270 += 1.0f;
    if (thisv->unk_270 >= kREG(3) + 10.0f) {
        EnRu1_SpawnRipple(thisv, globalCtx, kREG(1) + 500, 0);
        thisv->unk_270 = 0.0f;
    }
}

void func_80AEB59C(EnRu1* thisv, GlobalContext* globalCtx) {
    EnRu1_SpawnRipple(thisv, globalCtx, kREG(2) + 500, 0);
    EnRu1_SpawnRipple(thisv, globalCtx, kREG(2) + 500, kREG(3) + 10.0f);
    EnRu1_SpawnRipple(thisv, globalCtx, kREG(2) + 500, (kREG(3) + 10.0f) * 2.0f);
}

void EnRu1_SpawnSplash(EnRu1* thisv, GlobalContext* globalCtx) {
    Vec3f pos;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
    pos.z = thisv->actor.world.pos.z;

    EffectSsGSplash_Spawn(globalCtx, &pos, 0, 0, 1, 0);
}

void func_80AEB6E0(EnRu1* thisv, GlobalContext* globalCtx) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->moveFlags |= 3;
        AnimationContext_SetMoveActor(globalCtx, &thisv->actor, skelAnime, 1.0f);
    }
}

void func_80AEB738(EnRu1* thisv, GlobalContext* globalCtx) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    skelAnime->baseTransl = skelAnime->jointTable[0];
    skelAnime->prevTransl = skelAnime->jointTable[0];
    if (skelAnime->baseTransl.y < skelAnime->jointTable[0].y) {
        skelAnime->moveFlags |= 3;
        AnimationContext_SetMoveActor(globalCtx, &thisv->actor, skelAnime, 1.0f);
    }
}

void func_80AEB7D0(EnRu1* thisv) {
    thisv->skelAnime.moveFlags &= ~0x3;
}

f32 func_80AEB7E0(const CsCmdActorAction* csCmdNPCAction, GlobalContext* globalCtx) {
    s32 csCtxFrames = globalCtx->csCtx.frames;

    if ((csCtxFrames < csCmdNPCAction->endFrame) && (csCmdNPCAction->endFrame - csCmdNPCAction->startFrame > 0)) {
        return (Math_CosS(((csCtxFrames - csCmdNPCAction->startFrame) /
                           (f32)(csCmdNPCAction->endFrame - csCmdNPCAction->startFrame)) *
                          32768.0f) *
                -0.5f) +
               0.5f;
    }
    return 1.0f;
}

f32 func_80AEB87C(f32 arg0, s32 arg1, s32 arg2) {
    return (((f32)arg2 - arg1) * arg0) + arg1;
}

void func_80AEB89C(EnRu1* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* npcAction = func_80AEB438(globalCtx);
    s16 npcActionRotY;

    if (npcAction != NULL) {
        npcActionRotY = npcAction->rot.y;
        thisv->actor.shape.rot.y = npcActionRotY;
        thisv->actor.world.rot.y = npcActionRotY;
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;
    }
}

void func_80AEB914(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB89C(thisv, globalCtx);
}

void func_80AEB934(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB89C(thisv, globalCtx);
}

void func_80AEB954(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB6E0(thisv, globalCtx);
}

void func_80AEB974(EnRu1* thisv, GlobalContext* globalCtx) {
    Vec3f* thisPos;
    f32 sp30;
    const CsCmdActorAction* csCmdNPCAction = func_80AEB438(globalCtx);
    s32 pad;

    if (csCmdNPCAction != NULL) {
        sp30 = func_80AEB7E0(csCmdNPCAction, globalCtx);
        thisPos = &thisv->actor.world.pos;
        thisPos->x = func_80AEB87C(sp30, csCmdNPCAction->startPos.x, csCmdNPCAction->endPos.x);
        thisPos->y = func_80AEB87C(sp30, csCmdNPCAction->startPos.y, csCmdNPCAction->endPos.y);
        thisPos->z = func_80AEB87C(sp30, csCmdNPCAction->startPos.z, csCmdNPCAction->endPos.z);
    }
}

void func_80AEBA0C(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB6E0(thisv, globalCtx);
}

void func_80AEBA2C(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f* unk_364 = &thisv->unk_364;
    Vec3f* thisPos;
    f32 temp_ret_2;
    const CsCmdActorAction* csCmdNPCAction = func_80AEB438(globalCtx);
    s32 pad2;

    if (csCmdNPCAction != NULL) {
        temp_ret_2 = func_80AEB7E0(csCmdNPCAction, globalCtx);
        thisPos = &thisv->actor.world.pos;
        thisPos->x = func_80AEB87C(temp_ret_2, unk_364->x, csCmdNPCAction->endPos.x);
        thisPos->y = func_80AEB87C(temp_ret_2, unk_364->y, csCmdNPCAction->endPos.y);
        thisPos->z = func_80AEB87C(temp_ret_2, unk_364->z, csCmdNPCAction->endPos.z);
    }
}

void func_80AEBAFC(EnRu1* thisv) {
    if (thisv->unk_298 == 0) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_EV_DIVE_INTO_WATER);
        thisv->unk_298 = 1;
    }
}

void func_80AEBB3C(EnRu1* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 5.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_FACE_UP);
    }
}

void func_80AEBB78(EnRu1* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if (Animation_OnFrame(skelAnime, 4.0f) || Animation_OnFrame(skelAnime, 13.0f) ||
        Animation_OnFrame(skelAnime, 22.0f) || Animation_OnFrame(skelAnime, 31.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_SWIM);
    }
}

void func_80AEBBF4(EnRu1* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 8.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_SUBMERGE);
    }
}

void func_80AEBC30(GlobalContext* globalCtx) {
    Player* player;

    if (globalCtx->csCtx.frames == 0xCD) {
        player = GET_PLAYER(globalCtx);
        Audio_PlaySoundGeneral(NA_SE_EV_DIVE_INTO_WATER, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void func_80AEBC84(EnRu1* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames == 0x82) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_LAUGH_0);
    }
}

void func_80AEBCB8(EnRu1* thisv, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gRutoChildSwimOnBackAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildSwimOnBackAnim), ANIMMODE_LOOP, -8.0f);
    }
}

void func_80AEBD1C(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEB480(globalCtx, 2)) {
        thisv->action = 1;
        thisv->drawConfig = 0;
        func_80AEB914(thisv, globalCtx);
        func_80AEAECC(thisv, globalCtx);
        EnRu1_SpawnSplash(thisv, globalCtx);
        func_80AEB59C(thisv, globalCtx);
    }
}

void func_80AEBD94(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    f32 frameCount;

    if (func_80AEB480(globalCtx, 3)) {
        frameCount = Animation_GetLastFrame(&gRutoChildAnim_009060);
        func_80AEB934(thisv, globalCtx);
        func_80AEB738(thisv, globalCtx);
        Animation_Change(&thisv->skelAnime, &gRutoChildAnim_009060, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        thisv->action = 2;
        thisv->drawConfig = 1;
    }
}

void func_80AEBE3C(EnRu1* thisv, GlobalContext* globalCtx, s32 arg2) {
    s32 pad[2];
    f32 frameCount;

    if (arg2 != 0) {
        frameCount = Animation_GetLastFrame(&gRutoChildTreadWaterAnim);
        func_80AEB7D0(thisv);
        Animation_Change(&thisv->skelAnime, &gRutoChildTreadWaterAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
        thisv->action = 3;
    } else {
        func_80AEB954(thisv, globalCtx);
    }
}

void func_80AEBEC8(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    f32 frameCount;

    if (func_80AEB458(globalCtx, 6)) {
        frameCount = Animation_GetLastFrame(&gRutoChildTransitionToSwimOnBackAnim);
        func_80AEB738(thisv, globalCtx);
        Animation_Change(&thisv->skelAnime, &gRutoChildTransitionToSwimOnBackAnim, 1.0f, 0, frameCount, ANIMMODE_ONCE,
                         -8.0f);
        thisv->action = 4;
    }
}

void func_80AEBF60(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEB480(globalCtx, 6)) {
        func_80AEB7D0(thisv);
        thisv->action = 5;
        thisv->unk_364 = thisv->actor.world.pos;
    } else {
        func_80AEBA0C(thisv, globalCtx);
    }
}

void func_80AEBFD8(EnRu1* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction = func_80AEB438(globalCtx);
    f32 frameCount;
    u16 csCtxFrames;
    u16 endFrame;

    if (csCmdNPCAction != NULL) {
        csCtxFrames = globalCtx->csCtx.frames;
        endFrame = csCmdNPCAction->endFrame;
        if (csCtxFrames >= endFrame - 2) {
            frameCount = Animation_GetLastFrame(&gRutoChildTransitionFromSwimOnBackAnim);
            Animation_Change(&thisv->skelAnime, &gRutoChildTransitionFromSwimOnBackAnim, 1.0, 0, frameCount,
                             ANIMMODE_ONCE, -8.0f);
            thisv->action = 6;
        }
    }
}

void func_80AEC070(EnRu1* thisv, GlobalContext* globalCtx, UNK_TYPE arg2) {
    if ((func_80AEB458(globalCtx, 8)) && (arg2 != 0)) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AEC0B4(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB89C(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEBC84(thisv, globalCtx);
    func_80AEBC30(globalCtx);
    func_80AEBD1C(thisv, globalCtx);
}

void func_80AEC100(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEBAFC(thisv);
    func_80AEBD94(thisv, globalCtx);
}

void func_80AEC130(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something = EnRu1_UpdateSkelAnime(thisv);

    func_80AEAECC(thisv, globalCtx);
    func_80AEBB3C(thisv);
    func_80AEBE3C(thisv, globalCtx, something);
}

void func_80AEC17C(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB974(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEB50C(thisv, globalCtx);
    func_80AEBEC8(thisv, globalCtx);
}

void func_80AEC1D4(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    something = EnRu1_UpdateSkelAnime(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AEB50C(thisv, globalCtx);
    func_80AEBCB8(thisv, something);
    func_80AEBBF4(thisv);
    func_80AEBF60(thisv, globalCtx);
}

void func_80AEC244(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    something = EnRu1_UpdateSkelAnime(thisv);
    func_80AEBA2C(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AEB50C(thisv, globalCtx);
    func_80AEBCB8(thisv, something);
    func_80AEBB78(thisv);
    func_80AEBFD8(thisv, globalCtx);
}

void func_80AEC2C0(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    something = EnRu1_UpdateSkelAnime(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AEB50C(thisv, globalCtx);
    func_80AEC070(thisv, globalCtx, something);
}

void func_80AEC320(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 actorRoom;

    if (!(gSaveContext.infTable[20] & 2)) {
        func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
        thisv->action = 7;
        EnRu1_SetMouthIndex(thisv, 1);
    } else if ((gSaveContext.infTable[20] & 0x80) && !(gSaveContext.infTable[20] & 1) &&
               !(gSaveContext.infTable[20] & 0x20)) {
        if (!func_80AEB020(thisv, globalCtx)) {
            func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
            actorRoom = thisv->actor.room;
            thisv->action = 22;
            thisv->actor.room = -1;
            thisv->drawConfig = 0;
            thisv->roomNum1 = actorRoom;
            thisv->roomNum3 = actorRoom;
            thisv->roomNum2 = actorRoom;
        } else {
            Actor_Kill(&thisv->actor);
        }
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AEC40C(EnRu1* thisv) {
    f32 unk_26C = thisv->unk_26C;

    if (unk_26C < 8.0f) {
        thisv->actor.speedXZ = (((kREG(3) * 0.01f) + 2.7f) / 8.0f) * unk_26C;
    } else {
        thisv->actor.speedXZ = (kREG(3) * 0.01f) + 2.7f;
    }
    thisv->actor.velocity.y = -1.0f;
    Actor_MoveForward(&thisv->actor);
}

void func_80AEC4CC(EnRu1* thisv) {
    thisv->actor.velocity.y = -1.0f;
    Actor_MoveForward(&thisv->actor);
}

void func_80AEC4F4(EnRu1* thisv) {
    f32* speedXZ = &thisv->actor.speedXZ;
    f32* unk_26C = &thisv->unk_26C;

    if (thisv->unk_26C < 8.0f) {
        *unk_26C += 1.0f;
        *speedXZ *= (8.0f - *unk_26C) / 8.0f;
        thisv->actor.velocity.y = -*unk_26C * (((kREG(4) * 0.01f) + 13.0f) / 8.0f);
    } else {
        *speedXZ = 0.0f;
        thisv->actor.velocity.y = -((kREG(4) * 0.01f) + 13.0f);
    }
    Actor_MoveForward(&thisv->actor);
}

s32 func_80AEC5FC(EnRu1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 thisPosZ = thisv->actor.world.pos.z;
    f32 playerPosZ = player->actor.world.pos.z;

    if ((playerPosZ - thisPosZ <= 265.0f) && (player->actor.world.pos.y >= thisv->actor.world.pos.y)) {
        return true;
    }
    return false;
}

void func_80AEC650(EnRu1* thisv) {
    s32 pad[2];

    if (thisv->unk_280 == 0) {
        if (Animation_OnFrame(&thisv->skelAnime, 2.0f) || Animation_OnFrame(&thisv->skelAnime, 7.0f)) {
            func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_DIRT);
        }
    }
}

void func_80AEC6B0(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_EV_FALL_DOWN_DIRT);
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_FALL);
}

void func_80AEC6E4(EnRu1* thisv, GlobalContext* globalCtx) {
    if ((func_80AEAFA0(globalCtx, 4, 3)) && (thisv->unk_280 == 0)) {
        Animation_Change(&thisv->skelAnime, &gRutoChildBringArmsUpAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildBringArmsUpAnim), ANIMMODE_ONCE, -8.0f);
        thisv->unk_280 = 1;
        func_80AEC6B0(thisv);
    }
}

void func_80AEC780(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    if ((func_80AEC5FC(thisv, globalCtx)) && (!Gameplay_InCsMode(globalCtx)) && (!(player->stateFlags1 & 0x206000)) &&
        (player->actor.bgCheckFlags & 1)) {

        globalCtx->csCtx.segment = &D_80AF0880;
        gSaveContext.cutsceneTrigger = 1;
        player->linearVelocity = 0.0f;
        thisv->action = 8;
    }
}

void func_80AEC81C(EnRu1* thisv, GlobalContext* globalCtx) {
    const CsCmdActorAction* csCmdNPCAction;
    s16 newRotY;

    if (func_80AEAFE0(globalCtx, 1, 3)) {
        csCmdNPCAction = globalCtx->csCtx.npcActions[3];
        thisv->actor.world.pos.x = csCmdNPCAction->startPos.x;
        thisv->actor.world.pos.y = csCmdNPCAction->startPos.y;
        thisv->actor.world.pos.z = csCmdNPCAction->startPos.z;
        newRotY = csCmdNPCAction->rot.y;
        thisv->actor.shape.rot.y = newRotY;
        thisv->actor.world.rot.y = newRotY;
        thisv->action = 9;
        thisv->drawConfig = 1;
    }
}

void func_80AEC8B8(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEAFA0(globalCtx, 3, 3)) {
        Animation_Change(&thisv->skelAnime, &gRutoChildTurnAroundAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildTurnAroundAnim), ANIMMODE_ONCE, -8.0f);
        thisv->action = 10;
    }
}

void func_80AEC93C(EnRu1* thisv, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gRutoChildWalkAnim, 1.0f, 0, Animation_GetLastFrame(&gRutoChildWalkAnim),
                         ANIMMODE_LOOP, -8.0f);
        thisv->actor.world.rot.y += 0x8000;
        thisv->action = 0xB;
        thisv->unk_26C = 0.0f;
    }
}

void func_80AEC9C4(EnRu1* thisv) {
    thisv->unk_26C += 1.0f;
    if (thisv->unk_26C >= 8.0f) {
        thisv->action = 12;
        thisv->unk_26C = 0.0f;
        thisv->actor.velocity.y = -1.0f;
    }
}

void func_80AECA18(EnRu1* thisv) {
    if (!(thisv->actor.bgCheckFlags & 1)) {
        thisv->action = 13;
        thisv->unk_26C = 0.0f;
        thisv->actor.velocity.y = 0.0f;
    }
}

void func_80AECA44(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEAFA0(globalCtx, 5, 3)) {
        gSaveContext.infTable[20] |= 2;
        thisv->action = 14;
    }
}

void func_80AECA94(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEC780(thisv, globalCtx);
}

void func_80AECAB4(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEC81C(thisv, globalCtx);
}

void func_80AECAD4(EnRu1* thisv, GlobalContext* globalCtx) {
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEC8B8(thisv, globalCtx);
}

void func_80AECB18(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    something = EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEC93C(thisv, something);
}

void func_80AECB60(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEC40C(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEC650(thisv);
    func_80AEC9C4(thisv);
}

void func_80AECBB8(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEC4CC(thisv);
    func_80AEC6E4(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEC650(thisv);
    func_80AECA18(thisv);
}

void func_80AECC1C(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEC4F4(thisv);
    func_80AEC6E4(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEC650(thisv);
    func_80AECA44(thisv, globalCtx);
}

void func_80AECC84(EnRu1* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AECCB0(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f* pos;
    s16 yawTowardsPlayer;
    f32 spawnX;
    f32 spawnY;
    f32 spawnZ;
    s32 pad2[2];

    yawTowardsPlayer = thisv->actor.yawTowardsPlayer;
    pos = &thisv->actor.world.pos;
    spawnX = ((kREG(1) + 12.0f) * Math_SinS(yawTowardsPlayer)) + pos->x;
    spawnY = pos->y;
    spawnZ = ((kREG(1) + 12.0f) * Math_CosS(yawTowardsPlayer)) + pos->z;
    thisv->blueWarp = (DoorWarp1*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1,
                                                    spawnX, spawnY, spawnZ, 0, yawTowardsPlayer, 0, WARP_BLUE_RUTO);
}

void func_80AECDA0(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB264(thisv, &gRutoChildWaitHandsOnHipsAnim, 0, 0, 0);
    thisv->action = 15;
    thisv->actor.shape.yOffset = -10000.0f;
    EnRu1_SetEyeIndex(thisv, 5);
    EnRu1_SetMouthIndex(thisv, 2);
}

void func_80AECE04(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.yOffset += (250.0f / 3.0f);
}

void func_80AECE20(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad2;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f* playerPos = &player->actor.world.pos;
    s16 shapeRotY = player->actor.shape.rot.y;
    s32 pad;
    f32 unk_27C = thisv->unk_27C;
    Vec3f* pos = &thisv->actor.world.pos;

    pos->x = (Math_SinS(shapeRotY) * unk_27C) + playerPos->x;
    pos->y = playerPos->y;
    pos->z = (Math_CosS(shapeRotY) * unk_27C) + playerPos->z;
}

void func_80AECEB4(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f* player_unk_450 = &player->unk_450;
    Vec3f* pos = &thisv->actor.world.pos;
    s16 shapeRotY = thisv->actor.shape.rot.y;

    player_unk_450->x = ((kREG(2) + 30.0f) * Math_SinS(shapeRotY)) + pos->x;
    player_unk_450->z = ((kREG(2) + 30.0f) * Math_CosS(shapeRotY)) + pos->z;
}

s32 func_80AECF6C(EnRu1* thisv, GlobalContext* globalCtx) {
    s16* shapeRotY;
    Player* player = GET_PLAYER(globalCtx);
    Player* otherPlayer;
    s16 temp_f16;
    f32 temp1;
    f32 temp2;
    s32 pad2[5];

    thisv->unk_26C += 1.0f;
    if ((player->actor.speedXZ == 0.0f) && (thisv->unk_26C >= 3.0f)) {
        otherPlayer = GET_PLAYER(globalCtx);
        player->actor.world.pos.x = otherPlayer->unk_450.x;
        player->actor.world.pos.y = otherPlayer->unk_450.y;
        player->actor.world.pos.z = otherPlayer->unk_450.z;
        shapeRotY = &player->actor.shape.rot.y;
        temp1 = thisv->actor.world.pos.x - player->actor.world.pos.x;
        temp2 = thisv->actor.world.pos.z - player->actor.world.pos.z;
        temp_f16 = Math_FAtan2F(temp1, temp2) * (0x8000 / std::numbers::pi_v<float>);
        if (*shapeRotY != temp_f16) {
            Math_SmoothStepToS(shapeRotY, temp_f16, 0x14, 0x1838, 0x64);
            player->actor.world.rot.y = *shapeRotY;
        } else {
            return true;
        }
    }
    return false;
}

s32 func_80AED084(EnRu1* thisv, s32 state) {
    if (thisv->blueWarp != NULL && thisv->blueWarp->rutoWarpState == state) {
        return true;
    }
    return false;
}

void func_80AED0B0(EnRu1* thisv, s32 state) {
    if (thisv->blueWarp != NULL) {
        thisv->blueWarp->rutoWarpState = state;
    }
}

void func_80AED0C8(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->action = 16;
}

void func_80AED0D8(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->action = 17;
    thisv->drawConfig = 1;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    func_80AECCB0(thisv, globalCtx);
}

void func_80AED110(EnRu1* thisv) {
    if (thisv->actor.shape.yOffset >= 0.0f) {
        thisv->action = 18;
        thisv->actor.shape.yOffset = 0.0f;
        func_80AED0B0(thisv, WARP_BLUE_RUTO_STATE_READY);
    }
}

void func_80AED154(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AED084(thisv, WARP_BLUE_RUTO_STATE_ENTERED)) {
        thisv->action = 0x13;
        thisv->unk_26C = 0.0f;
        func_80AECEB4(thisv, globalCtx);
    }
}

void func_80AED19C(EnRu1* thisv, s32 cond) {
    if (cond) {
        Animation_Change(&thisv->skelAnime, &gRutoChildTransitionHandsOnHipToCrossArmsAndLegsAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildTransitionHandsOnHipToCrossArmsAndLegsAnim), ANIMMODE_ONCE,
                         -8.0f);
        thisv->action = 20;
        func_80AED0B0(thisv, WARP_BLUE_RUTO_STATE_3);
    }
}

void func_80AED218(EnRu1* thisv, UNK_TYPE arg1) {
    if (func_80AED084(thisv, WARP_BLUE_RUTO_STATE_TALKING)) {
        if (arg1 != 0) {
            Animation_Change(&thisv->skelAnime, &gRutoChildWaitSittingAnim, 1.0f, 0,
                             Animation_GetLastFrame(&gRutoChildWaitSittingAnim), ANIMMODE_LOOP, -8.0f);
        }
    } else if (func_80AED084(thisv, WARP_BLUE_RUTO_STATE_WARPING)) {
        Animation_Change(&thisv->skelAnime, &gRutoChildWaitInBlueWarpAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildWaitInBlueWarpAnim), ANIMMODE_ONCE, -8.0f);
        thisv->action = 21;
        thisv->unk_27C = thisv->actor.xzDistToPlayer;
    }
}

void func_80AED304(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED0C8(thisv, globalCtx);
}

void func_80AED324(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED0D8(thisv, globalCtx);
}

void func_80AED344(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AECE04(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AED110(thisv);
}

void func_80AED374(EnRu1* thisv, GlobalContext* globalCtx) {
    EnRu1_UpdateSkelAnime(thisv);
    func_80AED154(thisv, globalCtx);
}

void func_80AED3A4(EnRu1* thisv, GlobalContext* globalCtx) {
    EnRu1_UpdateSkelAnime(thisv);
    func_80AED19C(thisv, func_80AECF6C(thisv, globalCtx));
}

void func_80AED3E0(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEAECC(thisv, globalCtx);
    func_80AED218(thisv, EnRu1_UpdateSkelAnime(thisv));
}

void func_80AED414(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AECE20(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
}

void func_80AED44C(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 actorRoom;

    if ((gSaveContext.infTable[20] & 2) && !(gSaveContext.infTable[20] & 0x20) && !(gSaveContext.infTable[20] & 1) &&
        !(gSaveContext.infTable[20] & 0x80)) {
        if (!func_80AEB020(thisv, globalCtx)) {
            func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
            actorRoom = thisv->actor.room;
            thisv->action = 22;
            thisv->actor.room = -1;
            thisv->roomNum1 = actorRoom;
            thisv->roomNum3 = actorRoom;
            thisv->roomNum2 = actorRoom;
        } else {
            Actor_Kill(&thisv->actor);
        }
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AED4FC(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_EV_LAND_DIRT);
}

void func_80AED520(EnRu1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Audio_PlaySoundGeneral(NA_SE_PL_PULL_UP_RUTO, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_LIFT);
}

void func_80AED57C(EnRu1* thisv) {
    if (thisv->actor.speedXZ != 0.0f) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_THROW);
    }
}

void func_80AED5B8(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_CRASH);
}

void func_80AED5DC(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_UNBALLANCE);
}

void func_80AED600(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_DISCOVER);
}

s32 func_80AED624(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 curRoomNum = globalCtx->roomCtx.curRoom.num;

    if (thisv->roomNum2 != curRoomNum) {
        Actor_Kill(&thisv->actor);
        return false;
    } else if (((thisv->roomNum1 != curRoomNum) || (thisv->roomNum2 != curRoomNum)) &&
               (thisv->actor.yDistToWater > kREG(16) + 50.0f) && (thisv->action != 33)) {
        thisv->action = 33;
        thisv->drawConfig = 2;
        thisv->alpha = 0xFF;
        thisv->unk_2A4 = 0.0f;
    }
    return true;
}

void func_80AED6DC(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 curRoomNum = globalCtx->roomCtx.curRoom.num;

    thisv->roomNum2 = curRoomNum;
    thisv->unk_288 = 0.0f;
}

void func_80AED6F8(GlobalContext* globalCtx) {
    s8 curRoomNum;

    if ((!(gSaveContext.infTable[20] & 0x80))) {
        curRoomNum = globalCtx->roomCtx.curRoom.num;
        if (curRoomNum == 2) {
            gSaveContext.infTable[20] |= 0x80;
        }
    }
}

void func_80AED738(EnRu1* thisv, GlobalContext* globalCtx) {
    u32 temp_v0;

    if (func_80AED624(thisv, globalCtx)) {
        thisv->unk_2A4 += 1.0f;
        if (thisv->unk_2A4 < 20.0f) {
            temp_v0 = ((20.0f - thisv->unk_2A4) * 255.0f) / 20.0f;
            thisv->alpha = temp_v0;
            thisv->actor.shape.shadowAlpha = temp_v0;
        } else {
            Actor_Kill(&thisv->actor);
        }
    }
}

void func_80AED83C(EnRu1* thisv) {
    s32 pad[2];
    Vec3s* tempPtr;
    Vec3s* tempPtr2;

    tempPtr = &thisv->unk_374.unk_08;
    Math_SmoothStepToS(&tempPtr->x, 0, 0x14, 0x1838, 0x64);
    Math_SmoothStepToS(&tempPtr->y, 0, 0x14, 0x1838, 0x64);
    tempPtr2 = &thisv->unk_374.unk_0E;
    Math_SmoothStepToS(&tempPtr2->x, 0, 0x14, 0x1838, 0x64);
    Math_SmoothStepToS(&tempPtr2->y, 0, 0x14, 0x1838, 0x64);
}

void func_80AED8DC(EnRu1* thisv) {
    s32 temp_hi;
    s16* unk_2AC = &thisv->unk_2AC;
    s16* someY = &thisv->unk_374.unk_08.y;
    s16* unk_29E = &thisv->unk_29E;
    s32 pad[2];

    if (DECR(*unk_2AC) == 0) {
        *unk_2AC = Rand_S16Offset(0xA, 0x19);
        temp_hi = *unk_2AC % 5;
        if (temp_hi == 0) {
            thisv->unk_2B0 = 1;
        } else if (temp_hi == 1) {
            thisv->unk_2B0 = 2;
        } else {
            thisv->unk_2B0 = 0;
        }
        *unk_29E = 0;
    }

    if (thisv->unk_2B0 == 0) {
        Math_SmoothStepToS(unk_29E, 0 - *someY, 1, 0x190, 0x190);
        Math_SmoothStepToS(someY, 0, 3, ABS(*unk_29E), 0x64);
    } else if (thisv->unk_2B0 == 1) {
        Math_SmoothStepToS(unk_29E, -0x2AAA - *someY, 1, 0x190, 0x190);
        Math_SmoothStepToS(someY, -0x2AAA, 3, ABS(*unk_29E), 0x64);
    } else {
        Math_SmoothStepToS(unk_29E, 0x2AAA - *someY, 1, 0x190, 0x190);
        Math_SmoothStepToS(someY, 0x2AAA, 3, ABS(*unk_29E), 0x64);
    }
}

void func_80AEDAE0(EnRu1* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, thisv->actor.floorBgId);

    if (dynaPolyActor == NULL || dynaPolyActor->actor.id == ACTOR_EN_BOX) {
        thisv->actor.bgCheckFlags &= ~0x19;
    }
}

void func_80AEDB30(EnRu1* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* dynaPolyActor;
    f32* velocityY;
    f32* speedXZ;
    f32* gravity;
    s16 wallYaw;
    s16 rotY;
    s32 temp_a1_2;
    s32 temp_a0;
    s32 phi_v1;

    if (thisv->actor.bgCheckFlags & 1) {
        velocityY = &thisv->actor.velocity.y;
        dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, thisv->actor.floorBgId);
        if (*velocityY <= 0.0f) {
            speedXZ = &thisv->actor.speedXZ;
            if (dynaPolyActor != NULL) {
                if (dynaPolyActor->actor.id != ACTOR_EN_BOX) {
                    *speedXZ = 0.0f;
                }
            } else {
                if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
                    *speedXZ *= (kREG(19) * 0.01f) + 0.8f;
                } else {
                    *speedXZ = 0.0f;
                }
            }
            gravity = &thisv->actor.gravity;
            if (dynaPolyActor != NULL) {
                if (dynaPolyActor->actor.id != ACTOR_EN_BOX) {
                    *velocityY = 0.0f;
                    thisv->actor.minVelocityY = 0.0f;
                    *gravity = 0.0f;
                } else {
                    *velocityY *= -1.0f;
                }
            } else {
                *velocityY *= -((kREG(20) * 0.01f) + 0.6f);
                if (*velocityY <= -*gravity * ((kREG(20) * 0.01f) + 0.6f)) {
                    *velocityY = 0.0f;
                    thisv->actor.minVelocityY = 0.0f;
                    *gravity = 0.0f;
                }
            }
            func_80AED4FC(thisv);
        }
    }
    if (thisv->actor.bgCheckFlags & 0x10) {
        speedXZ = &thisv->actor.speedXZ;
        velocityY = &thisv->actor.velocity.y;
        if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
            *speedXZ *= (kREG(19) * 0.01f) + 0.8f;
        } else {
            *speedXZ = 0.0f;
        }
        if (*velocityY >= 0.0f) {
            *velocityY *= -((kREG(20) * 0.01f) + 0.6f);
            func_80AED4FC(thisv);
        }
    }
    if (thisv->actor.bgCheckFlags & 8) {
        speedXZ = &thisv->actor.speedXZ;
        if (*speedXZ != 0.0f) {
            rotY = thisv->actor.world.rot.y;
            wallYaw = thisv->actor.wallYaw;
            temp_a0 = (wallYaw * 2) - rotY;
            temp_a1_2 = temp_a0 + 0x8000;
            if ((s16)((temp_a0 - wallYaw) + 0x8000) >= 0) {
                phi_v1 = (s16)(temp_a1_2 - wallYaw);
            } else {
                phi_v1 = -(s16)(temp_a1_2 - wallYaw);
            }
            if (phi_v1 < 0x4001) {
                if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
                    *speedXZ *= (kREG(21) * 0.01f) + 0.6f;
                } else {
                    *speedXZ = 0.0f;
                }
                thisv->actor.world.rot.y = temp_a1_2;
                func_80AED4FC(thisv);
                func_80AED5B8(thisv);
            }
        }
    }
}

void func_80AEDEF4(EnRu1* thisv, GlobalContext* globalCtx) {
    f32* speedXZ = &thisv->actor.speedXZ;
    DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, thisv->actor.floorBgId);

    if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_EN_BOX) {
        if (*speedXZ != 0.0f) {
            *speedXZ *= 1.1f;
        } else {
            *speedXZ = 1.0f;
        }
    }
    if (*speedXZ >= (kREG(27) * 0.01f) + 3.0f) {
        *speedXZ *= (kREG(22) * 0.01f) + 0.98f;
    } else {
        *speedXZ = 0.0f;
    }
}

void func_80AEDFF4(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEDB30(thisv, globalCtx);
    func_80AEDEF4(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
}

void func_80AEE02C(EnRu1* thisv) {
    thisv->actor.velocity.x = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.velocity.z = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.minVelocityY = 0.0f;
}

void func_80AEE050(EnRu1* thisv) {
    s32 pad;
    f32 sp28;
    f32 sp24;
    f32 temp_f10;
    EnRu1* thisx = thisv; // necessary to match

    if (thisv->unk_350 == 0) {
        if ((thisv->actor.minVelocityY == 0.0f) && (thisv->actor.speedXZ == 0.0f)) {
            thisv->unk_350 = 1;
            func_80AEE02C(thisv);
            thisv->unk_35C = 0;
            thisv->unk_358 = (thisv->actor.yDistToWater - 10.0f) * 0.5f;
            thisv->unk_354 = thisv->actor.world.pos.y + thisx->unk_358; // thisx only used here
        } else {
            thisv->actor.gravity = 0.0f;
            thisv->actor.minVelocityY *= 0.2f;
            thisv->actor.velocity.y *= 0.2f;
            if (thisv->actor.minVelocityY >= -0.1f) {
                thisv->actor.minVelocityY = 0.0f;
                thisv->actor.velocity.y = 0.0f;
            }
            thisv->actor.speedXZ *= 0.5f;
            if (thisv->actor.speedXZ <= 0.1f) {
                thisv->actor.speedXZ = 0.0f;
            }
            thisv->actor.velocity.x = Math_SinS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
            thisv->actor.velocity.z = Math_CosS(thisv->actor.world.rot.y) * thisv->actor.speedXZ;
            func_8002D7EC(&thisv->actor);
        }
    } else {
        if (thisv->unk_350 == 1) {
            if (thisv->unk_358 <= 1.0f) {
                func_80AEE02C(thisv);
                thisv->unk_350 = 2;
                thisv->unk_360 = 0.0f;
            } else {
                sp28 = thisv->unk_358;
                sp24 = thisv->unk_354;
                temp_f10 = Math_CosS(thisv->unk_35C) * -sp28;
                thisv->actor.world.pos.y = temp_f10 + sp24;
                thisv->unk_35C += 0x3E8;
                thisv->unk_358 *= 0.95f;
            }
        } else {
            thisv->unk_360 += 1.0f;
            if (thisv->unk_360 > 0.0f) {
                thisv->unk_350 = 3;
            }
        }
    }
}

s32 func_80AEE264(EnRu1* thisv, GlobalContext* globalCtx) {
    if (!Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        if ((gSaveContext.infTable[20] & 8)) {
            thisv->actor.textId = 0x404E;
            func_8002F2F4(&thisv->actor, globalCtx);
        } else if (gSaveContext.infTable[20] & 4) {
            thisv->actor.textId = 0x404D;
            func_8002F2F4(&thisv->actor, globalCtx);
        } else {
            thisv->actor.textId = 0x404C;
            func_8002F2F4(&thisv->actor, globalCtx);
        }
        return false;
    }
    return true;
}

void func_80AEE2F8(EnRu1* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* dynaPolyActor;
    s32 floorBgId;

    if ((thisv->actor.bgCheckFlags & 1) && (thisv->actor.floorBgId != BGCHECK_SCENE)) {
        floorBgId = thisv->actor.floorBgId;
        dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, floorBgId);
        if ((dynaPolyActor != NULL) && (dynaPolyActor->actor.id == ACTOR_BG_BDAN_SWITCH)) {
            if (((dynaPolyActor->actor.params >> 8) & 0x3F) == 0x38) {
                gSaveContext.infTable[20] |= 1;
                return;
            }
        }
    }
    gSaveContext.infTable[20] &= ~0x1;
}

s32 func_80AEE394(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    CollisionContext* colCtx;
    DynaPolyActor* dynaPolyActor;
    s32 floorBgId;

    if ((thisv->actor.bgCheckFlags & 1) && thisv->actor.floorBgId != BGCHECK_SCENE) {
        colCtx = &globalCtx->colCtx;
        floorBgId = thisv->actor.floorBgId; // necessary match, can't move thisv out of thisv block unfortunately
        dynaPolyActor = DynaPoly_GetActor(colCtx, floorBgId);
        if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_BG_BDAN_OBJECTS &&
            dynaPolyActor->actor.params == 0 && !Player_InCsMode(globalCtx) && globalCtx->msgCtx.msgLength == 0) {
            func_80AEE02C(thisv);
            globalCtx->csCtx.segment = &D_80AF10A4;
            gSaveContext.cutsceneTrigger = 1;
            thisv->action = 36;
            thisv->drawConfig = 0;
            thisv->unk_28C = (BgBdanObjects*)dynaPolyActor;
            thisv->actor.shape.shadowAlpha = 0;
            return true;
        }
    }
    return false;
}

void func_80AEE488(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 curRoomNum;

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        curRoomNum = globalCtx->roomCtx.curRoom.num;
        thisv->roomNum3 = curRoomNum;
        thisv->action = 31;
        func_80AED520(thisv, globalCtx);
    } else if ((!func_80AEE394(thisv, globalCtx)) && (!(thisv->actor.bgCheckFlags & 1))) {
        thisv->actor.minVelocityY = -((kREG(24) * 0.01f) + 6.8f);
        thisv->actor.gravity = -((kREG(23) * 0.01f) + 1.3f);
        thisv->action = 28;
    }
}

void func_80AEE568(EnRu1* thisv, GlobalContext* globalCtx) {
    if (!func_80AEE394(thisv, globalCtx)) {
        if ((thisv->actor.bgCheckFlags & 1) && (thisv->actor.speedXZ == 0.0f) && (thisv->actor.minVelocityY == 0.0f)) {
            func_80AEE02C(thisv);
            func_8002F580(&thisv->actor, globalCtx);
            thisv->action = 27;
            func_80AEADD8(thisv);
        } else if (thisv->actor.yDistToWater > 0.0f) {
            thisv->action = 29;
            thisv->unk_350 = 0;
        }
    }
}

void func_80AEE628(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s8 curRoomNum = globalCtx->roomCtx.curRoom.num;

    if (EnRu1_IsCsStateIdle(globalCtx)) {
        Animation_Change(&thisv->skelAnime, &gRutoChildSittingAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildSittingAnim), ANIMMODE_LOOP, -8.0f);
        gSaveContext.infTable[20] |= 0x10;
        thisv->action = 31;
    }
    thisv->roomNum3 = curRoomNum;
}

s32 func_80AEE6D0(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s8 curRoomNum = globalCtx->roomCtx.curRoom.num;

    if (!(gSaveContext.infTable[20] & 0x10) && (func_80AEB124(globalCtx) != 0)) {
        if (!Player_InCsMode(globalCtx)) {
            Animation_Change(&thisv->skelAnime, &gRutoChildSeesSapphireAnim, 1.0f, 0,
                             Animation_GetLastFrame(&gRutoChildSquirmAnim), ANIMMODE_LOOP, -8.0f);
            func_80AED600(thisv);
            thisv->action = 34;
            thisv->unk_26C = 0.0f;
            globalCtx->csCtx.segment = &D_80AF1728;
            gSaveContext.cutsceneTrigger = 1;
        }
        thisv->roomNum3 = curRoomNum;
        return true;
    }
    thisv->roomNum3 = curRoomNum;
    return false;
}

void func_80AEE7C4(EnRu1* thisv, GlobalContext* globalCtx) {
    f32 frameCount;
    s32 pad[13];
    Player* player;
    f32* unk_370 = &thisv->unk_370;

    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        frameCount = Animation_GetLastFrame(&gRutoChildSittingAnim);
        Animation_Change(&thisv->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
        func_80AED6DC(thisv, globalCtx);
        thisv->actor.speedXZ *= (kREG(25) * 0.01f) + 1.0f;
        thisv->actor.velocity.y *= (kREG(26) * 0.01f) + 1.0f;
        thisv->actor.minVelocityY = -((kREG(24) * 0.01f) + 6.8f);
        thisv->actor.gravity = -((kREG(23) * 0.01f) + 1.3f);
        func_80AED57C(thisv);
        thisv->action = 28;
        *unk_370 = 0.0f;
        return;
    }

    if (func_80AEE6D0(thisv, globalCtx)) {
        *unk_370 = 0.0f;
        return;
    }

    player = GET_PLAYER(globalCtx);
    if (player->stateFlags2 & 0x10000000) {
        thisv->unk_370 += 1.0f;
        if (thisv->action != 32) {
            if (*unk_370 > 30.0f) {
                if (Rand_S16Offset(0, 3) == 0) {
                    frameCount = Animation_GetLastFrame(&gRutoChildSquirmAnim);
                    Animation_Change(&thisv->skelAnime, &gRutoChildSquirmAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP,
                                     -8.0f);
                    func_80AED5DC(thisv);
                    thisv->action = 32;
                }
                *unk_370 = 0.0f;
            }
        } else {
            if (*unk_370 > 50.0f) {
                frameCount = Animation_GetLastFrame(&gRutoChildSittingAnim);
                Animation_Change(&thisv->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
                thisv->action = 31;
                *unk_370 = 0.0f;
            }
        }
    } else {
        frameCount = Animation_GetLastFrame(&gRutoChildSittingAnim);
        Animation_Change(&thisv->skelAnime, &gRutoChildSittingAnim, 1.0f, 0, frameCount, ANIMMODE_LOOP, -8.0f);
        *unk_370 = 0.0f;
    }
}

s32 func_80AEEAC8(EnRu1* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1) {
        func_80AEE02C(thisv);
        func_8002F580(&thisv->actor, globalCtx);
        thisv->action = 27;
        func_80AEADD8(thisv);
        return true;
    }
    return false;
}

void func_80AEEB24(EnRu1* thisv, GlobalContext* globalCtx) {
    if ((func_80AEEAC8(thisv, globalCtx) == 0) && (thisv->unk_350 == 3)) {
        thisv->action = 30;
        func_80AEE02C(thisv);
        thisv->actor.gravity = -0.1f;
        thisv->actor.minVelocityY = -((kREG(18) * 0.1f) + 0.7f);
    }
}

void func_80AEEBB4(EnRu1* thisv, GlobalContext* globalCtx) {
    func_8002F580(&thisv->actor, globalCtx);
}

void func_80AEEBD4(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAC54(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEEBB4(thisv, globalCtx);
    func_80AEE488(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
    func_80AEDAE0(thisv, globalCtx);
}

void func_80AEEC5C(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEACDC(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    func_80AEE2F8(thisv, globalCtx);
    func_80AEDFF4(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEE568(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
    func_80AEDAE0(thisv, globalCtx);
}

void func_80AEECF0(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEE050(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEEB24(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
}

void func_80AEED58(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEEAC8(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
    func_80AEDAE0(thisv, globalCtx);
}

void func_80AEEDCC(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED8DC(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEE2F8(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AED6F8(globalCtx);
    func_80AEE7C4(thisv, globalCtx);
}

void func_80AEEE34(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEE2F8(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AED6F8(globalCtx);
    func_80AEE7C4(thisv, globalCtx);
}

void func_80AEEE9C(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEDFF4(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AED738(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
}

void func_80AEEF08(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateEyes(thisv);
    func_80AEE628(thisv, globalCtx);
}

void func_80AEEF5C(EnRu1* thisv, GlobalContext* globalCtx) {
}

void func_80AEEF68(EnRu1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 something;

    thisv->unk_374.unk_18 = player->actor.world.pos;
    thisv->unk_374.unk_14 = kREG(16) - 3.0f;
    something = kREG(17) + 0xC;
    func_80034A14(&thisv->actor, &thisv->unk_374, something, 2);
}

void func_80AEEFEC(EnRu1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 something;

    thisv->unk_374.unk_18 = player->actor.world.pos;
    thisv->unk_374.unk_14 = kREG(16) - 3.0f;
    something = kREG(17) + 0xC;
    func_80034A14(&thisv->actor, &thisv->unk_374, something, 4);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
}

void func_80AEF080(EnRu1* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 11.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_EV_LAND_DIRT);
    }
}

s32 func_80AEF0BC(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 frameCount;

    if (gSaveContext.infTable[20] & 4) {
        frameCount = Animation_GetLastFrame(&gRutoChildSitAnim);
        Animation_Change(&thisv->skelAnime, &gRutoChildSitAnim, 1.0f, 0, frameCount, ANIMMODE_ONCE, -8.0f);
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        thisv->action = 26;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        return true;
    }
    return false;
}

void func_80AEF170(EnRu1* thisv, GlobalContext* globalCtx, s32 cond) {
    if (cond) {
        thisv->action = 25;
    }
}

void func_80AEF188(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEB174(globalCtx) && !func_80AEF0BC(thisv, globalCtx)) {
        Message_CloseTextbox(globalCtx);
        gSaveContext.infTable[20] |= 4;
        thisv->action = 24;
    }
}

void func_80AEF1F0(EnRu1* thisv, GlobalContext* globalCtx, UNK_TYPE arg2) {
    if (arg2 != 0) {
        Animation_Change(&thisv->skelAnime, &gRutoChildSittingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRutoChildSittingAnim), ANIMMODE_LOOP, 0.0f);
        Message_CloseTextbox(globalCtx);
        gSaveContext.infTable[20] |= 8;
        func_80AED6DC(thisv, globalCtx);
        func_8002F580(&thisv->actor, globalCtx);
        thisv->action = 27;
        func_80AEADD8(thisv);
    }
}

void func_80AEF29C(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->action = 23;
}

void func_80AEF2AC(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->action = 24;
    thisv->drawConfig = 1;
    thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
}

void func_80AEF2D0(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 cond;

    func_80AEEF68(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAC10(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    cond = func_80AEE264(thisv, globalCtx);
    func_80AED624(thisv, globalCtx);
    func_80AEF170(thisv, globalCtx, cond);
}

void func_80AEF354(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEEFEC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEF188(thisv, globalCtx);
}

void func_80AEF3A8(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    func_80AED83C(thisv);
    something = EnRu1_UpdateSkelAnime(thisv);
    func_80AEF080(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEF1F0(thisv, globalCtx, something);
}

void func_80AEF40C(EnRu1* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if (Animation_OnFrame(skelAnime, 2.0f) || Animation_OnFrame(skelAnime, 7.0f) ||
        Animation_OnFrame(skelAnime, 12.0f) || Animation_OnFrame(skelAnime, 18.0f) ||
        Animation_OnFrame(skelAnime, 25.0f) || Animation_OnFrame(skelAnime, 33.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_DIRT);
    }
}

void func_80AEF4A8(EnRu1* thisv, GlobalContext* globalCtx) {
    SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.projectedPos, 20, NA_SE_VO_RT_FALL);
}

void func_80AEF4E0(EnRu1* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 5.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_LAUGH_0);
    }
}

void func_80AEF51C(EnRu1* thisv) {
    func_80078914(&thisv->actor.projectedPos, NA_SE_VO_RT_THROW);
}

void func_80AEF540(EnRu1* thisv) {
    if (func_80AEB104(thisv) == 2) {
        EnRu1_SetEyeIndex(thisv, 3);
        EnRu1_SetMouthIndex(thisv, 2);
        if (thisv->skelAnime.mode != 2) {
            func_80AEB264(thisv, &gRutoChildShutterAnim, 2, -8.0f, 0);
            func_80AEF51C(thisv);
        }
    }
}

void func_80AEF5B8(EnRu1* thisv) {
    f32 curFrame;

    if (D_80AF1938 == 0) {
        curFrame = thisv->skelAnime.curFrame;
        if (curFrame >= 60.0f) {
            EnRu1_SetEyeIndex(thisv, 3);
            EnRu1_SetMouthIndex(thisv, 0);
            func_80AED57C(thisv);
            D_80AF1938 = 1;
        }
    }
}

void func_80AEF624(EnRu1* thisv, GlobalContext* globalCtx) {
    f32 frameCount;
    const CsCmdActorAction* csCmdNPCAction;
    const CsCmdActorAction* csCmdNPCAction2;
    s16 newRotTmp;

    if (func_80AEAFE0(globalCtx, 1, 3)) {
        frameCount = Animation_GetLastFrame(&gRutoChildWalkToAndHoldUpSapphireAnim);
        // thisv weird part with the redundant variable is necessary to match for some reason
        csCmdNPCAction2 = globalCtx->csCtx.npcActions[3];
        csCmdNPCAction = csCmdNPCAction2;
        thisv->actor.world.pos.x = csCmdNPCAction->startPos.x;
        thisv->actor.world.pos.y = csCmdNPCAction->startPos.y;
        thisv->actor.world.pos.z = csCmdNPCAction->startPos.z;
        newRotTmp = csCmdNPCAction->rot.x;
        thisv->actor.shape.rot.x = newRotTmp;
        thisv->actor.world.rot.x = newRotTmp;
        newRotTmp = csCmdNPCAction->rot.y;
        thisv->actor.shape.rot.y = newRotTmp;
        thisv->actor.world.rot.y = newRotTmp;
        newRotTmp = csCmdNPCAction->rot.z;
        thisv->actor.shape.rot.z = newRotTmp;
        thisv->actor.world.rot.z = newRotTmp;
        Animation_Change(&thisv->skelAnime, &gRutoChildWalkToAndHoldUpSapphireAnim, 1.0f, 0.0f, frameCount,
                         ANIMMODE_ONCE, 0.0f);
        func_80AEB3A4(thisv, globalCtx);
        thisv->action = 37;
        thisv->drawConfig = 1;
        thisv->actor.shape.shadowAlpha = 0xFF;
    }
}

void func_80AEF728(EnRu1* thisv, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gRutoChildHoldArmsUpAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gRutoChildHoldArmsUpAnim), ANIMMODE_LOOP, 0.0f);
        func_80AEB3CC(thisv);
        thisv->action = 38;
    }
}

void func_80AEF79C(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEAFE0(globalCtx, 2, 3)) {
        Animation_Change(&thisv->skelAnime, &gRutoChildBringHandsDownAnim, 1.0f, 0,
                         Animation_GetLastFrame(&gRutoChildBringHandsDownAnim), ANIMMODE_ONCE, -8.0f);
        thisv->action = 39;
    }
}

void func_80AEF820(EnRu1* thisv, UNK_TYPE arg1) {
    if (arg1 != 0) {
        Animation_Change(&thisv->skelAnime, &gRutoChildWait2Anim, 1.0f, 0, Animation_GetLastFrame(&gRutoChildWait2Anim),
                         ANIMMODE_LOOP, -8.0f);
        thisv->action = 40;
    }
}

void func_80AEF890(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s8 curRoomNum;

    if ((gSaveContext.sceneSetupIndex < 4) && (EnRu1_IsCsStateIdle(globalCtx))) {
        curRoomNum = globalCtx->roomCtx.curRoom.num;
        gSaveContext.infTable[20] |= 0x20;
        Flags_SetSwitch(globalCtx, func_80AEADE0(thisv));
        func_80AEB0EC(thisv, 1);
        thisv->action = 42;
        thisv->actor.room = curRoomNum;
    }
}

void func_80AEF930(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEB104(thisv) == 3) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->actor.textId = 0x4048;
        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        func_80AEF4A8(thisv, globalCtx);
        thisv->action = 43;
        thisv->drawConfig = 0;
    }
}

void func_80AEF99C(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEB1B4(globalCtx) != 0) {
        func_80AEB0EC(thisv, 4);
        Actor_Kill(&thisv->actor);
    }
}

void func_80AEF9D8(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEF624(thisv, globalCtx);
    func_80AEB220(thisv, globalCtx);
}

void func_80AEFA2C(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    func_80AED83C(thisv);
    func_80AEB364(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    something = EnRu1_UpdateSkelAnime(thisv);
    func_80AEF4E0(thisv);
    func_80AEF5B8(thisv);
    func_80AEF40C(thisv);
    func_80AEF728(thisv, something);
    func_80AEB220(thisv, globalCtx);
}

void func_80AEFAAC(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    func_80AEF79C(thisv, globalCtx);
    func_80AEB220(thisv, globalCtx);
}

void func_80AEFB04(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 something;

    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    something = EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEF820(thisv, something);
    func_80AEB220(thisv, globalCtx);
}

void func_80AEFB68(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEF890(thisv, globalCtx);
    func_80AEB220(thisv, globalCtx);
}

void func_80AEFBC8(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEAECC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEF540(thisv);
    func_80AEF930(thisv, globalCtx);
}

void func_80AEFC24(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AED83C(thisv);
    func_80AEF99C(thisv, globalCtx);
}

void func_80AEFC54(EnRu1* thisv, GlobalContext* globalCtx) {
    if ((gSaveContext.infTable[20] & 0x20) && !(gSaveContext.infTable[20] & 0x40)) {
        func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
        thisv->action = 41;
        thisv->unk_28C = EnRu1_FindSwitch(globalCtx);
        func_80AEB0EC(thisv, 1);
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void func_80AEFCE8(EnRu1* thisv, GlobalContext* globalCtx) {
    thisv->unk_28C = EnRu1_FindSwitch(globalCtx);
    if (thisv->unk_28C != NULL) {
        thisv->action = 42;
        thisv->drawConfig = 1;
        func_80AEB0EC(thisv, 1);
    }
}

void func_80AEFD38(EnRu1* thisv, GlobalContext* globalCtx) {
    if ((gSaveContext.eventChkInf[3] & 0x80) && LINK_IS_CHILD) {
        func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        thisv->action = 44;
        thisv->drawConfig = 1;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

s32 func_80AEFDC0(EnRu1* thisv, GlobalContext* globalCtx) {
    if (!Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        thisv->actor.textId = Text_GetFaceReaction(globalCtx, 0x1F);
        if (thisv->actor.textId == 0) {
            thisv->actor.textId = 0x402C;
        }
        func_8002F2F4(&thisv->actor, globalCtx);
        return false;
    }
    return true;
}

s32 func_80AEFE38(EnRu1* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
        return true;
    }
    return false;
}

void func_80AEFE84(EnRu1* thisv, GlobalContext* globalCtx, s32 cond) {
    if (cond) {
        thisv->action = 45;
    }
}

void func_80AEFE9C(EnRu1* thisv, GlobalContext* globalCtx) {
    if (func_80AEFE38(thisv, globalCtx)) {
        thisv->action = 44;
    }
}

void func_80AEFECC(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEEF68(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAC10(thisv, globalCtx);
    func_80AEAECC(thisv, globalCtx);
    func_80AEFE84(thisv, globalCtx, func_80AEFDC0(thisv, globalCtx));
}

void func_80AEFF40(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEEFEC(thisv, globalCtx);
    EnRu1_UpdateSkelAnime(thisv);
    EnRu1_UpdateEyes(thisv);
    func_80AEAECC(thisv, globalCtx);
    func_80AEFE9C(thisv, globalCtx);
}

void func_80AEFF94(EnRu1* thisv, GlobalContext* globalCtx) {
    s8 actorRoom;

    if ((gSaveContext.infTable[20] & 2) && (gSaveContext.infTable[20] & 1) && !(gSaveContext.infTable[20] & 0x20) &&
        (!(func_80AEB020(thisv, globalCtx)))) {
        func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
        actorRoom = thisv->actor.room;
        thisv->action = 22;
        thisv->actor.room = -1;
        thisv->drawConfig = 0;
        thisv->roomNum1 = actorRoom;
        thisv->roomNum3 = actorRoom;
        thisv->roomNum2 = actorRoom;
        // "Ruto switch set"
        osSyncPrintf("スイッチルトセット!!!!!!!!!!!!!!!!!!!!!!\n");
    } else {
        // "Ruto switch not set"
        osSyncPrintf("スイッチルトセットしない!!!!!!!!!!!!!!!!!!!!!!\n");
        Actor_Kill(&thisv->actor);
    }
}

void func_80AF0050(EnRu1* thisv, GlobalContext* globalCtx) {
    func_80AEB264(thisv, &gRutoChildWait2Anim, 0, 0, 0);
    thisv->action = 36;
    thisv->roomNum1 = thisv->actor.room;
    thisv->unk_28C = EnRu1_FindSwitch(globalCtx);
    thisv->actor.room = -1;
}

void EnRu1_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnRu1* thisv = (EnRu1*)thisx;

    if (thisv->action < 0 || thisv->action >= ARRAY_COUNT(sActionFuncs) || sActionFuncs[thisv->action] == NULL) {
        // "Main mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sActionFuncs[thisv->action](thisv, globalCtx);
}

void EnRu1_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRu1* thisv = (EnRu1*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gRutoChildSkel, NULL, thisv->jointTable, thisv->morphTable, 17);
    func_80AEAD20(&thisv->actor, globalCtx);
    switch (func_80AEADF0(thisv)) {
        case 0:
            func_80AECDA0(thisv, globalCtx);
            break;
        case 1:
            func_80AEB3DC(thisv, globalCtx);
            break;
        case 2:
            func_80AEC320(thisv, globalCtx);
            break;
        case 3:
            func_80AED44C(thisv, globalCtx);
            break;
        case 4:
            func_80AEFC54(thisv, globalCtx);
            break;
        case 5:
            func_80AEFD38(thisv, globalCtx);
            break;
        case 6:
            func_80AEFF94(thisv, globalCtx);
            break;
        case 10:
            func_80AF0050(thisv, globalCtx);
            break;
        default:
            Actor_Kill(&thisv->actor);
            // "Relevant arge_data = %d unacceptable"
            osSyncPrintf("該当 arge_data = %d 無し\n", func_80AEADF0(thisv));
            break;
    }
}

void func_80AF0278(EnRu1* thisv, GlobalContext* globalCtx, s32 limbIndex, Vec3s* rot) {
    Vec3s* vec1 = &thisv->unk_374.unk_0E;
    Vec3s* vec2 = &thisv->unk_374.unk_08;

    switch (limbIndex) {
        case RUTO_CHILD_LEFT_UPPER_ARM:
            rot->x += vec1->y;
            rot->y -= vec1->x;
            break;
        case RUTO_CHILD_TORSO:
            rot->x += vec2->y;
            rot->z += vec2->x;
            break;
    }
}

s32 EnRu1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    EnRu1* thisv = (EnRu1*)thisx;

    if ((thisv->unk_290 < 0) || (thisv->unk_290 > 0) || (*sPreLimbDrawFuncs[thisv->unk_290] == NULL)) {
        // "Neck rotation mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "首回しモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sPreLimbDrawFuncs[thisv->unk_290](thisv, globalCtx, limbIndex, rot);
    }
    return false;
}

void EnRu1_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnRu1* thisv = (EnRu1*)thisx;
    Vec3f vec1;
    Vec3f vec2;

    if (limbIndex == RUTO_CHILD_TORSO) {
        vec1 = sMultVec;
        Matrix_MultVec3f(&vec1, &vec2);
        thisv->actor.focus.pos.x = vec2.x;
        thisv->actor.focus.pos.y = vec2.y;
        thisv->actor.focus.pos.z = vec2.z;
        thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
        thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
        thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    }
}

void EnRu1_DrawNothing(EnRu1* thisv, GlobalContext* globalCtx) {
}

void EnRu1_DrawOpa(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    const void* eyeTex = sEyeTextures[eyeIndex];
    s16 mouthIndex = thisv->mouthIndex;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* mouthTex = sMouthTextures[mouthIndex];
    s32 pad1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ru1.c", 1282);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnRu1_OverrideLimbDraw, EnRu1_PostLimbDraw, thisv, POLY_OPA_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ru1.c", 1309);
}

void EnRu1_DrawXlu(EnRu1* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 eyeIndex = thisv->eyeIndex;
    const void* eyeTex = sEyeTextures[eyeIndex];
    s16 mouthIndex = thisv->mouthIndex;
    SkelAnime* skelAnime = &thisv->skelAnime;
    const void* mouthTex = sMouthTextures[mouthIndex];
    s32 pad1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ru1.c", 1324);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnRu1_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ru1.c", 1353);
}

void EnRu1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnRu1* thisv = (EnRu1*)thisx;

    if (thisv->drawConfig < 0 || thisv->drawConfig >= ARRAY_COUNT(sDrawFuncs) || sDrawFuncs[thisv->drawConfig] == 0) {
        // "Draw mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[thisv->drawConfig](thisv, globalCtx);
}
