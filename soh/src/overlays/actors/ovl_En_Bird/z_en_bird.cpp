/*
 * File: z_en_bird.c
 * Overlay: ovl_En_Bird
 * Description: An unused brown bird
 */

#include "z_en_bird.h"
#include "objects/object_bird/object_bird.h"

#define FLAGS 0

void EnBird_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBird_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBird_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBird_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809C1E00(EnBird* thisv, s16 params);
void func_809C1E40(EnBird* thisv, GlobalContext* globalCtx);
void func_809C1D60(EnBird* thisv, GlobalContext* globalCtx);
void func_809C1CAC(EnBird* thisv, s16 params);

ActorInit En_Bird_InitVars = {
    ACTOR_EN_BIRD,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BIRD,
    sizeof(EnBird),
    (ActorFunc)EnBird_Init,
    (ActorFunc)EnBird_Destroy,
    (ActorFunc)EnBird_Update,
    (ActorFunc)EnBird_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 5600, ICHAIN_STOP),
};

void EnBird_SetupAction(EnBird* thisv, EnBirdActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBird_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBird* thisv = (EnBird*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.01);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gBirdSkel, &gBirdFlyAnim, NULL, NULL, 0);
    ActorShape_Init(&thisv->actor.shape, 5500, ActorShadow_DrawCircle, 4);
    thisv->unk_194 = 0;
    thisv->unk_198 = 0;
    thisv->unk_1C0 = 0x9C4;
    thisv->actor.colChkInfo.mass = 0;
    thisv->unk_1A8 = 1.5f;
    thisv->unk_1AC = 0.5f;
    thisv->unk_1A0 = 0.0f;
    thisv->unk_1A4 = 0.0f;
    thisv->unk_1B8 = 0.0f;
    thisv->unk_1B0 = 40.0f;
    thisv->unk_1BC = 70.0f;
    func_809C1CAC(thisv, thisv->actor.params);
}

void EnBird_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_809C1CAC(EnBird* thisv, s16 params) {
    f32 frameCount = Animation_GetLastFrame(&gBirdFlyAnim);
    f32 playbackSpeed = thisv->unk_19C ? 0.0f : 1.0f;
    const AnimationHeader* anim = &gBirdFlyAnim;

    thisv->unk_198 = Rand_S16Offset(5, 0x23);
    Animation_Change(&thisv->skelAnime, anim, playbackSpeed, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    EnBird_SetupAction(thisv, func_809C1D60);
}

void func_809C1D60(EnBird* thisv, GlobalContext* globalCtx) {
    f32 fVar2 = sinf(thisv->unk_1B4);

    thisv->actor.shape.yOffset = thisv->actor.shape.yOffset + fVar2 * thisv->unk_1A0;
    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);

    if (thisv->unk_19C != 0) {
        thisv->skelAnime.playSpeed = thisv->actor.speedXZ + thisv->actor.speedXZ;
    }

    SkelAnime_Update(&thisv->skelAnime);
    thisv->unk_198 -= 1;

    if (thisv->unk_198 <= 0) {
        func_809C1E00(thisv, thisv->actor.params);
    }
}

void func_809C1E00(EnBird* thisv, s16 params) {
    thisv->unk_198 = Rand_S16Offset(0x14, 0x2D);
    EnBird_SetupAction(thisv, func_809C1E40);
}

void func_809C1E40(EnBird* thisv, GlobalContext* globalCtx) {
    f32 fVar4 = sinf(thisv->unk_1B4);

    thisv->actor.shape.yOffset += fVar4 * thisv->unk_1A0;
    Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->unk_1A8, 0.1f, thisv->unk_1AC, 0.0f);

    if (thisv->unk_1B0 < Math_Vec3f_DistXZ(&thisv->actor.world.pos, &thisv->actor.home.pos) || thisv->unk_198 < 4) {
        Math_StepToAngleS(&thisv->actor.world.rot.y, Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos),
                          thisv->unk_1C0);
    } else {
        fVar4 = sinf(thisv->unk_1B4);
        thisv->actor.world.rot.y += (s16)(fVar4 * thisv->unk_1A4);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    SkelAnime_Update(&thisv->skelAnime);
    thisv->unk_198 -= 1;
    if (thisv->unk_198 < 0) {
        func_809C1CAC(thisv, thisv->actor.params);
    }
}

void EnBird_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBird* thisv = (EnBird*)thisx;

    thisv->unk_1B4 += thisv->unk_1B8;
    thisv->actionFunc(thisv, globalCtx);
}

void EnBird_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBird* thisv = (EnBird*)thisx;

    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, NULL);
}
