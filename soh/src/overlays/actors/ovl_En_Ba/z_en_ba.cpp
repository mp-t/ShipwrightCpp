/*
 * File: z_en_ba.c
 * Overlay: ovl_En_Ba
 * Description: Tentacle from inside Lord Jabu-Jabu
 */

#include "z_en_ba.h"
#include "objects/object_bxa/object_bxa.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnBa_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBa_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBa_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBa_SetupIdle(EnBa* thisv);
void EnBa_SetupFallAsBlob(EnBa* thisv);
void EnBa_Idle(EnBa* thisv, GlobalContext* globalCtx);
void EnBa_FallAsBlob(EnBa* thisv, GlobalContext* globalCtx);
void EnBa_SwingAtPlayer(EnBa* thisv, GlobalContext* globalCtx);
void EnBa_RecoilFromDamage(EnBa* thisv, GlobalContext* globalCtx);
void EnBa_Die(EnBa* thisv, GlobalContext* globalCtx);
void EnBa_SetupSwingAtPlayer(EnBa* thisv);

const ActorInit En_Ba_InitVars = {
    ACTOR_EN_BA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BXA,
    sizeof(EnBa),
    (ActorFunc)EnBa_Init,
    (ActorFunc)EnBa_Destroy,
    (ActorFunc)EnBa_Update,
    (ActorFunc)EnBa_Draw,
    NULL,
};

static Vec3f D_809B8080 = { 0.0f, 0.0f, 32.0f };

static ColliderJntSphElementInit sJntSphElementInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000010, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 8, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 13, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    2,
    sJntSphElementInit,
};

void EnBa_SetupAction(EnBa* thisv, EnBaActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

static Vec3f D_809B80E4 = { 0.01f, 0.01f, 0.01f };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x15, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2500, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void EnBa_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBa* thisv = (EnBa*)thisx;
    Vec3f sp38 = D_809B80E4;
    s32 pad;
    s16 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 100.0f;
    for (i = 13; i >= 0; i--) {
        thisv->unk200[i] = sp38;
        thisv->unk2A8[i].x = -0x4000;
        thisv->unk158[i] = thisv->actor.world.pos;
        thisv->unk158[i].y = thisv->actor.world.pos.y - (i + 1) * 32.0f;
    }

    thisv->actor.targetMode = 4;
    thisv->upperParams = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    if (thisv->actor.params < EN_BA_DEAD_BLOB) {
        if (Flags_GetSwitch(globalCtx, thisv->upperParams)) {
            Actor_Kill(&thisv->actor);
            return;
        }
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
        Actor_SetScale(&thisv->actor, 0.01f);
        EnBa_SetupIdle(thisv);
        thisv->actor.colChkInfo.health = 4;
        thisv->actor.colChkInfo.mass = MASS_HEAVY;
        Collider_InitJntSph(globalCtx, &thisv->collider);
        Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
    } else {
        Actor_SetScale(&thisv->actor, 0.021f);
        EnBa_SetupFallAsBlob(thisv);
    }
}

void EnBa_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBa* thisv = (EnBa*)thisx;
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void EnBa_SetupIdle(EnBa* thisv) {
    thisv->unk14C = 2;
    thisv->unk31C = 1500;
    thisv->actor.speedXZ = 10.0f;
    EnBa_SetupAction(thisv, EnBa_Idle);
}

void EnBa_Idle(EnBa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 i;
    s32 pad;
    Vec3s sp5C;

    if ((thisv->actor.colChkInfo.mass == MASS_IMMOVABLE) && (thisv->actor.xzDistToPlayer > 175.0f)) {
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 330.0f, 1.0f, 7.0f, 0.0f);
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0;
        Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 100.0f, 1.0f, 10.0f, 0.0f);
    }
    thisv->unk2FC = thisv->actor.world.pos;
    if (globalCtx->gameplayFrames % 16 == 0) {
        thisv->unk308.z += Rand_CenteredFloat(180.0f);
        thisv->unk314 += Rand_CenteredFloat(180.0f);
        thisv->unk308.x = Math_SinF(thisv->unk308.z) * 80.0f;
        thisv->unk308.y = Math_CosF(thisv->unk314) * 80.0f;
    }
    thisv->unk2FC.y -= 448.0f;
    thisv->unk2FC.x += thisv->unk308.x;
    thisv->unk2FC.z += thisv->unk308.y;
    func_80033AEC(&thisv->unk2FC, &thisv->unk158[13], 1.0f, thisv->actor.speedXZ, 0.0f, 0.0f);
    for (i = 12; i >= 0; i--) {
        func_80035844(&thisv->unk158[i + 1], &thisv->unk158[i], &sp5C, 0);
        Matrix_Translate(thisv->unk158[i + 1].x, thisv->unk158[i + 1].y, thisv->unk158[i + 1].z, MTXMODE_NEW);
        Matrix_RotateZYX(sp5C.x, sp5C.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i]);
    }
    func_80035844(&thisv->unk158[0], &thisv->unk2FC, &sp5C, 0);
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->unk2A8[0].y, 3, thisv->unk31C, 182);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->unk2A8[0].x, 3, thisv->unk31C, 182);
    Matrix_RotateZYX(thisv->actor.shape.rot.x - 0x8000, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_809B8080, &thisv->unk158[0]);
    thisv->unk2A8[13].y = sp5C.y;
    thisv->unk2A8[13].x = sp5C.x + 0x8000;

    for (i = 0; i < 13; i++) {
        Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
        Math_SmoothStepToS(&thisv->unk2A8[i].y, thisv->unk2A8[i + 1].y, 3, thisv->unk31C, 182);
        Math_SmoothStepToS(&thisv->unk2A8[i].x, thisv->unk2A8[i + 1].x, 3, thisv->unk31C, 182);
        Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i + 1]);
    }
    thisv->unk2A8[13].x = thisv->unk2A8[12].x;
    thisv->unk2A8[13].y = thisv->unk2A8[12].y;
    if (!(player->stateFlags1 & 0x4000000) && (thisv->actor.xzDistToPlayer <= 175.0f) &&
        (thisv->actor.world.pos.y == thisv->actor.home.pos.y + 100.0f)) {
        EnBa_SetupSwingAtPlayer(thisv);
    }
}

void EnBa_SetupFallAsBlob(EnBa* thisv) {
    thisv->unk14C = 0;
    thisv->actor.speedXZ = Rand_CenteredFloat(8.0f);
    thisv->actor.world.rot.y = Rand_CenteredFloat(65535.0f);
    thisv->unk318 = 20;
    thisv->actor.gravity = -2.0f;
    EnBa_SetupAction(thisv, EnBa_FallAsBlob);
}

/**
 * Action function of the pink fleshy blobs that spawn and fall to the floor when a tentacle dies
 */
void EnBa_FallAsBlob(EnBa* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.scale.y -= 0.001f;
        thisv->actor.scale.x += 0.0005f;
        thisv->actor.scale.z += 0.0005f;
        thisv->unk318--;
        if (thisv->unk318 == 0) {
            Actor_Kill(&thisv->actor);
        }
    } else {
        Actor_MoveForward(&thisv->actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 30.0f, 28.0f, 80.0f, 5);
    }
}

void EnBa_SetupSwingAtPlayer(EnBa* thisv) {
    thisv->unk14C = 3;
    thisv->unk318 = 20;
    thisv->unk31A = 0;
    thisv->unk31C = 1500;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.speedXZ = 20.0f;
    EnBa_SetupAction(thisv, EnBa_SwingAtPlayer);
}

void EnBa_SwingAtPlayer(EnBa* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 temp;
    s16 i;
    Vec3s sp58;
    s16 phi_fp;

    Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 60.0f, 1.0f, 10.0f, 0.0f);
    if ((thisv->actor.xzDistToPlayer <= 175.0f) || (thisv->unk31A != 0)) {
        if (thisv->unk318 == 20) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BALINADE_HAND_UP);
            thisv->unk31C = 1500;
        }
        if (thisv->unk318 != 0) {
            thisv->unk31A = 10;
            thisv->unk318--;
            if (thisv->unk318 >= 11) {
                thisv->unk2FC = player->actor.world.pos;
                thisv->unk2FC.y += 30.0f;
                phi_fp = thisv->actor.yawTowardsPlayer;
            } else {
                phi_fp = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->unk2FC);
            }
            Math_SmoothStepToS(&thisv->unk31C, 1500, 1, 30, 0);
            func_80035844(&thisv->actor.world.pos, &thisv->unk158[0], &sp58, 0);
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, sp58.y, 1, thisv->unk31C, 0);
            Math_SmoothStepToS(&thisv->actor.shape.rot.x, (sp58.x + 0x8000), 1, thisv->unk31C, 0);
            Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
            Matrix_RotateZYX((thisv->actor.shape.rot.x - 0x8000), thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
            Matrix_MultVec3f(&D_809B8080, &thisv->unk158[0]);

            for (i = 0; i < 13; i++) {
                Math_SmoothStepToS(&thisv->unk2A8[i].x, (i * 1200) - 0x4000, 1, thisv->unk31C, 0);
                Math_SmoothStepToS(&thisv->unk2A8[i].y, phi_fp, 1, thisv->unk31C, 0);
                Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
                Matrix_RotateZYX((thisv->unk2A8[i].x - 0x8000), thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
                Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i + 1]);
            }
        } else {
            if (thisv->unk31A == 10) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BALINADE_HAND_DOWN);
            }
            if (thisv->unk31A != 0) {
                thisv->unk31C = 8000;
                thisv->actor.speedXZ = 30.0f;
                phi_fp = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->unk2FC);
                temp = Math_Vec3f_Pitch(&thisv->actor.world.pos, &thisv->unk158[0]) + 0x8000;
                Math_SmoothStepToS(&thisv->actor.shape.rot.y, phi_fp, 1, thisv->unk31C, 0);
                Math_SmoothStepToS(&thisv->actor.shape.rot.x, temp, 1, thisv->unk31C, 0);
                Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                 MTXMODE_NEW);
                Matrix_RotateZYX(thisv->actor.shape.rot.x - 0x8000, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
                Matrix_MultVec3f(&D_809B8080, thisv->unk158);

                for (i = 0; i < 13; i++) {
                    temp = -Math_CosS(thisv->unk31A * 0xCCC) * (i * 1200);
                    Math_SmoothStepToS(&thisv->unk2A8[i].x, temp - 0x4000, 1, thisv->unk31C, 0);
                    Math_SmoothStepToS(&thisv->unk2A8[i].y, phi_fp, 1, thisv->unk31C, 0);
                    Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
                    Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
                    Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i + 1]);
                }
                thisv->unk31A--;
            } else if ((thisv->actor.xzDistToPlayer > 175.0f) || (player->stateFlags1 & 0x4000000)) {
                EnBa_SetupIdle(thisv);
            } else {
                EnBa_SetupSwingAtPlayer(thisv);
                thisv->unk318 = 27;
                thisv->unk31C = 750;
            }
        }
        thisv->unk2A8[13].x = thisv->unk2A8[12].x;
        thisv->unk2A8[13].y = thisv->unk2A8[12].y;
        if (thisv->collider.base.atFlags & 2) {
            thisv->collider.base.atFlags &= ~2;
            if (thisv->collider.base.at == &player->actor) {
                func_8002F71C(globalCtx, &thisv->actor, 8.0f, thisv->actor.yawTowardsPlayer, 8.0f);
            }
        }
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        return;
    }
    if ((thisv->actor.xzDistToPlayer > 175.0f) || (player->stateFlags1 & 0x4000000)) {
        EnBa_SetupIdle(thisv);
    } else {
        EnBa_SetupSwingAtPlayer(thisv);
        thisv->unk318 = 27;
        thisv->unk31C = 750;
    }
}

void func_809B7174(EnBa* thisv) {
    thisv->unk14C = 1;
    thisv->unk31C = 1500;
    thisv->unk318 = 20;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->actor.speedXZ = 10.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BALINADE_HAND_DAMAGE);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 12);
    EnBa_SetupAction(thisv, EnBa_RecoilFromDamage);
}

void EnBa_RecoilFromDamage(EnBa* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3s sp6C;

    Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 330.0f, 1.0f, 30.0f, 0.0f);
    thisv->unk2FC = thisv->actor.world.pos;
    if (globalCtx->gameplayFrames % 16 == 0) {
        thisv->unk308.z += Rand_CenteredFloat(180.0f);
        thisv->unk314 += Rand_CenteredFloat(180.0f);
        thisv->unk308.x = Math_SinF(thisv->unk308.z) * 80.0f;
        thisv->unk308.y = Math_CosF(thisv->unk314) * 80.0f;
    }
    thisv->unk2FC.y -= 448.0f;
    thisv->unk2FC.x += thisv->unk308.x;
    thisv->unk2FC.z += thisv->unk308.y;
    func_80033AEC(&thisv->unk2FC, &thisv->unk158[13], 1.0f, thisv->actor.speedXZ, 0.0f, 0.0f);
    for (i = 12; i >= 0; i--) {
        func_80035844(&thisv->unk158[i + 1], &thisv->unk158[i], &sp6C, 0);
        Matrix_Translate(thisv->unk158[i + 1].x, thisv->unk158[i + 1].y, thisv->unk158[i + 1].z, MTXMODE_NEW);
        Matrix_RotateZYX(sp6C.x, sp6C.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i]);
    }
    func_80035844(&thisv->actor.world.pos, &thisv->unk158[0], &sp6C, 0);
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, sp6C.y, 3, thisv->unk31C, 182);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, sp6C.x + 0x8000, 3, thisv->unk31C, 182);
    Matrix_RotateZYX(thisv->actor.shape.rot.x - 0x8000, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_809B8080, &thisv->unk158[0]);

    for (i = 0; i < 13; i++) {
        func_80035844(&thisv->unk158[i], &thisv->unk158[i + 1], &sp6C, 0);
        Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
        Math_SmoothStepToS(&thisv->unk2A8[i].y, sp6C.y, 3, thisv->unk31C, 182);
        Math_SmoothStepToS(&thisv->unk2A8[i].x, sp6C.x + 0x8000, 3, thisv->unk31C, 182);
        Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i + 1]);
    }

    thisv->unk2A8[13].x = thisv->unk2A8[12].x;
    thisv->unk2A8[13].y = thisv->unk2A8[12].y;
    thisv->unk318--;
    if (thisv->unk318 == 0) {
        EnBa_SetupIdle(thisv);
    }
}

void func_809B75A0(EnBa* thisv, GlobalContext* globalCtx2) {
    s16 unk_temp;
    s32 i;
    Vec3f sp74 = { 0.0f, 0.0f, 0.0f };
    GlobalContext* globalCtx = globalCtx2;

    thisv->unk31C = 2500;
    EffectSsDeadSound_SpawnStationary(globalCtx, &thisv->actor.projectedPos, NA_SE_EN_BALINADE_HAND_DEAD, 1, 1, 40);
    thisv->unk14C = 0;

    for (i = 7; i < 14; i++) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BA, thisv->unk158[i].x, thisv->unk158[i].y,
                    thisv->unk158[i].z, 0, 0, 0, EN_BA_DEAD_BLOB);
    }
    unk_temp = Math_Vec3f_Pitch(&thisv->actor.world.pos, &thisv->unk158[0]) + 0x8000;
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, thisv->unk31C, 0);
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, unk_temp, 1, thisv->unk31C, 0);
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.shape.rot.x - 0x8000, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_809B8080, &thisv->unk158[0]);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    for (i = 5; i < 13; i++) {
        Math_SmoothStepToS(&thisv->unk2A8[i].x, thisv->unk2A8[5].x, 1, thisv->unk31C, 0);
        Math_SmoothStepToS(&thisv->unk2A8[i].y, thisv->unk2A8[5].y, 1, thisv->unk31C, 0);
        Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&sp74, &thisv->unk158[i + 1]);
    }
    thisv->unk31A = 15;
    EnBa_SetupAction(thisv, EnBa_Die);
}

void EnBa_Die(EnBa* thisv, GlobalContext* globalCtx) {
    Vec3f sp6C = { 0.0f, 0.0f, 0.0f };
    s16 temp;
    s32 i;

    if (thisv->unk31A != 0) {
        thisv->actor.speedXZ = 30.0f;
        thisv->unk31C = 8000;
        thisv->actor.world.pos.y += 8.0f;
        temp = Math_Vec3f_Pitch(&thisv->actor.world.pos, &thisv->unk158[0]) + 0x8000;
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 1, thisv->unk31C, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.x, temp, 1, thisv->unk31C, 0);
        Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateZYX(thisv->actor.shape.rot.x - 0x8000, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_809B8080, &thisv->unk158[0]);
        for (i = 0; i < 5; i++) {
            temp = -Math_CosS(thisv->unk31A * 0x444) * (i * 400);
            Math_SmoothStepToS(&thisv->unk2A8[i].x, temp - 0x4000, 1, thisv->unk31C, 0);
            Math_SmoothStepToS(&thisv->unk2A8[i].y, thisv->actor.yawTowardsPlayer, 1, thisv->unk31C, 0);
            Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
            Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
            Matrix_MultVec3f(&D_809B8080, &thisv->unk158[i + 1]);
        }
        for (i = 5; i < 13; i++) {
            Math_SmoothStepToS(&thisv->unk2A8[i].x, thisv->unk2A8[5].x, 1, thisv->unk31C, 0);
            Math_SmoothStepToS(&thisv->unk2A8[i].y, thisv->unk2A8[5].y, 1, thisv->unk31C, 0);
            Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
            Matrix_RotateZYX(thisv->unk2A8[i].x - 0x8000, thisv->unk2A8[i].y, 0, MTXMODE_APPLY);
            Matrix_MultVec3f(&sp6C, &thisv->unk158[i + 1]);
        }
        thisv->unk31A--;
    } else {
        Flags_SetSwitch(globalCtx, thisv->upperParams);
        Actor_Kill(&thisv->actor);
    }
}

void EnBa_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBa* thisv = (EnBa*)thisx;

    if ((thisv->actor.params < EN_BA_DEAD_BLOB) && (thisv->collider.base.acFlags & 2)) {
        thisv->collider.base.acFlags &= ~2;
        thisv->actor.colChkInfo.health--;
        if (thisv->actor.colChkInfo.health == 0) {
            func_809B75A0(thisv, globalCtx);
        } else {
            func_809B7174(thisv);
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actor.params < EN_BA_DEAD_BLOB) {
        thisv->actor.focus.pos = thisv->unk158[6];
    }
    if (thisv->unk14C >= 2) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

static void* D_809B8118[] = {
    object_bxa_Tex_0024F0,
    object_bxa_Tex_0027F0,
    object_bxa_Tex_0029F0,
};

void EnBa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBa* thisv = (EnBa*)thisx;
    s32 pad;
    s16 i;
    Mtx* mtx = Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx) * 14);
    Vec3f unused = { 0.0f, 0.0f, 448.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_ba.c", 933);
    func_80093D18(globalCtx->state.gfxCtx);
    if (thisv->actor.params < EN_BA_DEAD_BLOB) {
        Matrix_Push();
        gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_809B8118[thisv->actor.params]));
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 16, 16, 1, 0,
                                    (globalCtx->gameplayFrames * -10) % 128, 32, 32));
        for (i = 0; i < 14; i++, mtx++) {
            Matrix_Translate(thisv->unk158[i].x, thisv->unk158[i].y, thisv->unk158[i].z, MTXMODE_NEW);
            Matrix_RotateZYX(thisv->unk2A8[i].x, thisv->unk2A8[i].y, thisv->unk2A8[i].z, MTXMODE_APPLY);
            Matrix_Scale(thisv->unk200[i].x, thisv->unk200[i].y, thisv->unk200[i].z, MTXMODE_APPLY);
            if ((i == 6) || (i == 13)) {
                switch (i) {
                    case 13:
                        Collider_UpdateSpheres(i, &thisv->collider);
                        break;
                    default:
                        Matrix_Scale(0.5f, 0.5f, 1.0f, MTXMODE_APPLY);
                        Collider_UpdateSpheres(8, &thisv->collider);
                        break;
                }
            }
            Matrix_ToMtx(mtx, "../z_en_ba.c", 970);
        }
        Matrix_Pop();
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ba.c", 973),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_000890);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->gameplayFrames * 2) % 128,
                                    (globalCtx->gameplayFrames * 2) % 128, 32, 32, 1,
                                    (globalCtx->gameplayFrames * -5) % 128, (globalCtx->gameplayFrames * -5) % 128, 32,
                                    32));
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 125, 100, 255);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_ba.c", 991),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_001D80);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_ba.c", 995);
}
