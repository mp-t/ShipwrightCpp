/*
 * File: z_en_anubice_fire.c
 * Overlay: ovl_En_Anubice_Fire
 * Description: Anubis Fire Attack
 */

#include "z_en_anubice_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_anubice/object_anubice.h"

#define FLAGS ACTOR_FLAG_4

void EnAnubiceFire_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceFire_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceFire_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAnubiceFire_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809B26EC(EnAnubiceFire* thisv, GlobalContext* globalCtx);
void func_809B27D8(EnAnubiceFire* thisv, GlobalContext* globalCtx);
void func_809B2B48(EnAnubiceFire* thisv, GlobalContext* globalCtx);

ActorInit En_Anubice_Fire_InitVars = {
    ACTOR_EN_ANUBICE_FIRE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ANUBICE,
    sizeof(EnAnubiceFire),
    (ActorFunc)EnAnubiceFire_Init,
    (ActorFunc)EnAnubiceFire_Destroy,
    (ActorFunc)EnAnubiceFire_Update,
    (ActorFunc)EnAnubiceFire_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

void EnAnubiceFire_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceFire* thisv = (EnAnubiceFire*)thisx;
    s32 i;

    Collider_InitCylinder(globalCtx, &thisv->cylinder);
    Collider_SetCylinder(globalCtx, &thisv->cylinder, &thisv->actor, &sCylinderInit);

    thisv->unk_15A = 30;
    thisv->unk_154 = 2.0f;
    thisv->scale = 0.0f;

    for (i = 0; i < 6; i++) {
        thisv->unk_160[i] = thisv->actor.world.pos;
    }

    thisv->unk_15E = 0;
    thisv->actionFunc = func_809B26EC;
}

void EnAnubiceFire_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceFire* thisv = (EnAnubiceFire*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->cylinder);
}

void func_809B26EC(EnAnubiceFire* thisv, GlobalContext* globalCtx) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };

    Matrix_Push();
    Matrix_RotateY(BINANG_TO_RAD(thisv->actor.world.rot.y), MTXMODE_NEW);
    Matrix_RotateX(BINANG_TO_RAD(thisv->actor.world.rot.x), MTXMODE_APPLY);
    velocity.z = 15.0f;
    Matrix_MultVec3f(&velocity, &thisv->actor.velocity);
    Matrix_Pop();

    thisv->actionFunc = func_809B27D8;
    thisv->actor.world.rot.x = thisv->actor.world.rot.y = thisv->actor.world.rot.z = 0;
}

void func_809B27D8(EnAnubiceFire* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Color_RGBA8 primColor = { 255, 255, 0, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 255 };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp78 = { 0.0f, 0.0f, 0.0f };

    thisv->actor.world.rot.z += 5000;
    if (thisv->unk_15A == 0) {
        thisv->unk_154 = 0.0f;
    }

    Math_ApproachF(&thisv->scale, thisv->unk_154, 0.2f, 0.4f);
    if ((thisv->unk_15A == 0) && (thisv->scale < 0.1f)) {
        Actor_Kill(&thisv->actor);
    } else if ((thisv->actor.params == 0) && (thisv->cylinder.base.atFlags & 4)) {
        if (Player_HasMirrorShieldEquipped(globalCtx)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_SHIELD_REFLECT_SW);
            thisv->cylinder.base.atFlags &= 0xFFE9;
            thisv->cylinder.base.atFlags |= 8;
            thisv->cylinder.info.toucher.dmgFlags = 2;
            thisv->unk_15A = 30;
            thisv->actor.params = 1;
            thisv->actor.velocity.x *= -1.0f;
            thisv->actor.velocity.y *= -0.5f;
            thisv->actor.velocity.z *= -1.0f;
        } else {
            thisv->unk_15A = 0;
            EffectSsBomb2_SpawnLayered(globalCtx, &thisv->actor.world.pos, &sp78, &sp84, 10, 5);
            thisv->actor.velocity.x = thisv->actor.velocity.y = thisv->actor.velocity.z = 0.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_ANUBIS_FIREBOMB);
            thisv->actionFunc = func_809B2B48;
        }
    } else if (!(thisv->scale < .4f)) {
        f32 scale = 1000.0f;
        f32 life = 10.0f;
        s32 i;

        for (i = 0; i < 10; i++) {
            pos.x = thisv->actor.world.pos.x + (Rand_ZeroOne() - 0.5f) * (thisv->scale * 20.0f);
            pos.y = thisv->actor.world.pos.y + (Rand_ZeroOne() - 0.5f) * (thisv->scale * 20.0f);
            pos.z = thisv->actor.world.pos.z;
            EffectSsKiraKira_SpawnDispersed(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, scale, life);
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_ANUBIS_FIRE - SFX_FLAG);
    }
}

void func_809B2B48(EnAnubiceFire* thisv, GlobalContext* globalCtx) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Color_RGBA8 primColor = { 255, 255, 0, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 255 };
    s32 pad;
    s32 i;

    if (thisv->unk_15C == 0) {
        for (i = 0; i < 20; i++) {
            pos.x = thisv->actor.world.pos.x;
            pos.y = thisv->actor.world.pos.y;
            pos.z = thisv->actor.world.pos.z;
            accel.x = Rand_CenteredFloat(8.0f);
            accel.y = Rand_CenteredFloat(2.0f);
            accel.z = Rand_CenteredFloat(8.0f);
            EffectSsKiraKira_SpawnDispersed(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 2000, 10);
        }

        thisv->unk_15C = 2;
        thisv->unk_15E++;
        if (thisv->unk_15E >= 6) {
            Actor_Kill(&thisv->actor);
        }
    }
}

void EnAnubiceFire_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnAnubiceFire* thisv = (EnAnubiceFire*)thisx;
    s32 pad;
    s32 i;

    Actor_SetScale(&thisv->actor, thisv->scale);
    thisv->actionFunc(thisv, globalCtx);
    func_8002D7EC(&thisv->actor);
    thisv->unk_160[0] = thisv->actor.world.pos;

    if (1) {}

    for (i = 4; i >= 0; i--) {
        thisv->unk_160[i + 1] = thisv->unk_160[i];
    }

    if (thisv->unk_15A != 0) {
        thisv->unk_15A--;
    }

    if (thisv->unk_15C != 0) {
        thisv->unk_15C--;
    }

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 5.0f, 10.0f, 0x1D);
    if (!(thisv->scale < 0.6f || thisv->actionFunc == func_809B2B48)) {
        thisv->cylinder.dim.radius = thisv->scale * 15.0f + 5.0f;
        thisv->cylinder.dim.height = thisv->scale * 15.0f + 5.0f;
        thisv->cylinder.dim.yShift = thisv->scale * -0.75f + -15.0f;

        if (thisv->unk_15A != 0) {
            Collider_UpdateCylinder(&thisv->actor, &thisv->cylinder);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder.base);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder.base);
        }

        if (BgCheck_SphVsFirstPoly(&globalCtx->colCtx, &thisv->actor.world.pos, 30.0f)) {
            thisv->actor.velocity.x = thisv->actor.velocity.y = thisv->actor.velocity.z = 0.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_ANUBIS_FIREBOMB);
            thisv->actionFunc = func_809B2B48;
        }
    }
}

void EnAnubiceFire_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* D_809B3270[] = {
        gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex,
    };
    EnAnubiceFire* thisv = (EnAnubiceFire*)thisx;
    s32 pad[2];
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_anubice_fire.c", 503);
    func_80093D84(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    gDPPipeSync(POLY_XLU_DISP++);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_809B3270[0]));

    Matrix_Push();
    for (i = thisv->unk_15E; i < 6; ++i) {
        f32 scale = thisv->actor.scale.x - (i * 0.2f);

        if (scale < 0.0f) {
            scale = 0.0f;
        }

        if (scale >= 0.1f) {
            Matrix_Translate(thisv->unk_160[i].x, thisv->unk_160[i].y, thisv->unk_160[i].z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_RotateZ(thisv->actor.world.rot.z + i * 1000.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_anubice_fire.c", 546),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gAnubiceFireAttackDL);
        }

        if (thisv->scale < 0.1f) {
            break;
        }
    }
    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_anubice_fire.c", 556);
}
