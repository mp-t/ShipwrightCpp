/*
 * File: z_bg_haka_huta.c
 * Overlay: ovl_Bg_Haka_Huta
 * Description: Coffin Lid
 */

#include "z_bg_haka_huta.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHakaHuta_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaHuta_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaHuta_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgHakaHuta_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgHakaHuta_SpawnDust(BgHakaHuta* thisv, GlobalContext* globalCtx);
void BgHakaHuta_PlaySound(BgHakaHuta* thisv, GlobalContext* globalCtx, u16 sfx);
void BgHakaHuta_SpawnEnemies(BgHakaHuta* thisv, GlobalContext* globalCtx);
void BgHakaHuta_Open(BgHakaHuta* thisv, GlobalContext* globalCtx);
void BgHakaHuta_SlideOpen(BgHakaHuta* thisv, GlobalContext* globalCtx);
void func_8087D720(BgHakaHuta* thisv, GlobalContext* globalCtx);
void BgHakaHuta_DoNothing(BgHakaHuta* thisv, GlobalContext* globalCtx);

ActorInit Bg_Haka_Huta_InitVars = {
    ACTOR_BG_HAKA_HUTA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HAKACH_OBJECTS,
    sizeof(BgHakaHuta),
    (ActorFunc)BgHakaHuta_Init,
    (ActorFunc)BgHakaHuta_Destroy,
    (ActorFunc)BgHakaHuta_Update,
    (ActorFunc)BgHakaHuta_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaHuta_Init(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaHuta* thisv = (BgHakaHuta*)thisvx;
    s16 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisvx, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gBotwCoffinLidCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisvx, colHeader);
    thisv->unk_16A = (thisvx->params >> 8) & 0xFF;
    thisvx->params &= 0xFF;
    if (Flags_GetSwitch(globalCtx, thisvx->params)) {
        thisv->counter = -1;
        thisv->actionFunc = func_8087D720;
    } else {
        thisv->actionFunc = BgHakaHuta_SpawnEnemies;
    }
}

void BgHakaHuta_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaHuta* thisv = (BgHakaHuta*)thisvx;
    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgHakaHuta_SpawnDust(BgHakaHuta* thisv, GlobalContext* globalCtx) {
    static Vec3f sEffectAccel[] = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 30, 20, 50, 255 };
    static Color_RGBA8 envColor = { 0, 0, 0, 255 };
    f32 scale;
    f32 phi_f20;
    Vec3f effectPos;
    Vec3f effectVel;
    s32 i;
    f32 new_Xpos;
    f32 xPosOffset;

    phi_f20 = (thisv->dyna.actor.world.rot.y == 0) ? 1.0f : -1.0f;
    effectVel.y = 0.0f;
    effectVel.z = 0.0f;
    effectVel.x = -0.5f * phi_f20;
    effectPos.y = thisv->dyna.actor.world.pos.y;
    effectPos.z = thisv->dyna.actor.world.pos.z;
    new_Xpos = 50 - ((thisv->dyna.actor.world.pos.x - thisv->dyna.actor.home.pos.x) * phi_f20);
    xPosOffset = new_Xpos * phi_f20;

    for (i = 0; i < 4; i++) {
        if (i == 2) {
            effectPos.z += 120.0f * phi_f20;
        }
        effectPos.x = thisv->dyna.actor.home.pos.x - (Rand_ZeroOne() * xPosOffset);
        scale = ((Rand_ZeroOne() * 10.0f) + 50.0f);
        func_8002829C(globalCtx, &effectPos, &effectVel, sEffectAccel, &primColor, &envColor, scale, 0xA);
    }
}

void BgHakaHuta_PlaySound(BgHakaHuta* thisv, GlobalContext* globalCtx, u16 sfx) {
    Vec3f pos;

    pos.z = (thisv->dyna.actor.shape.rot.y == 0) ? thisv->dyna.actor.world.pos.z + 120.0f
                                                : thisv->dyna.actor.world.pos.z - 120.0f;
    pos.x = thisv->dyna.actor.world.pos.x;
    pos.y = thisv->dyna.actor.world.pos.y;
    SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &pos, 30, sfx);
}

void BgHakaHuta_SpawnEnemies(BgHakaHuta* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params) && !Player_InCsMode(globalCtx)) {
        thisv->counter = 25;
        thisv->actionFunc = BgHakaHuta_Open;
        OnePointCutscene_Init(globalCtx, 6001, 999, &thisv->dyna.actor, MAIN_CAM);
        if (thisv->unk_16A == 2) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_FIREFLY,
                        (thisv->dyna.actor.world.pos.x + (-25.0f) * Math_CosS(thisv->dyna.actor.shape.rot.y) +
                         40.0f * Math_SinS(thisv->dyna.actor.shape.rot.y)),
                        thisv->dyna.actor.world.pos.y - 10.0f,
                        (thisv->dyna.actor.world.pos.z - (-25.0f) * Math_SinS(thisv->dyna.actor.shape.rot.y) +
                         Math_CosS(thisv->dyna.actor.shape.rot.y) * 40.0f),
                        0, thisv->dyna.actor.shape.rot.y + 0x8000, 0, 2);

            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_FIREFLY,
                        (thisv->dyna.actor.world.pos.x + (-25.0f) * (Math_CosS(thisv->dyna.actor.shape.rot.y)) +
                         Math_SinS(thisv->dyna.actor.shape.rot.y) * 80.0f),
                        thisv->dyna.actor.world.pos.y - 10.0f,
                        (thisv->dyna.actor.world.pos.z - (-25.0f) * (Math_SinS(thisv->dyna.actor.shape.rot.y)) +
                         Math_CosS(thisv->dyna.actor.shape.rot.y) * 80.0f),
                        0, thisv->dyna.actor.shape.rot.y, 0, 2);

        } else if (thisv->unk_16A == 1) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_RD,
                        (thisv->dyna.actor.home.pos.x + (-25.0f) * (Math_CosS(thisv->dyna.actor.shape.rot.y)) +
                         Math_SinS(thisv->dyna.actor.shape.rot.y) * 100.0f),
                        thisv->dyna.actor.home.pos.y - 40.0f,
                        (thisv->dyna.actor.home.pos.z - (-25.0f) * (Math_SinS(thisv->dyna.actor.shape.rot.y)) +
                         Math_CosS(thisv->dyna.actor.shape.rot.y) * 100.0f),
                        0, thisv->dyna.actor.shape.rot.y, 0, 0xFD);
        }
    }
}

void BgHakaHuta_Open(BgHakaHuta* thisv, GlobalContext* globalCtx) {
    f32 posOffset;

    if (thisv->counter != 0) {
        thisv->counter--;
    }
    posOffset = (thisv->dyna.actor.world.rot.y == 0) ? 4.0f : -4.0f;
    Math_StepToF(&thisv->dyna.actor.world.pos.x, thisv->dyna.actor.home.pos.x + posOffset, 2.0f);
    if (thisv->counter == 0) {
        thisv->counter = 37;
        BgHakaHuta_PlaySound(thisv, globalCtx, NA_SE_EV_COFFIN_CAP_OPEN);
        thisv->actionFunc = BgHakaHuta_SlideOpen;
    }
}

void BgHakaHuta_SlideOpen(BgHakaHuta* thisv, GlobalContext* globalCtx) {
    f32 posOffset;

    if (thisv->counter != 0) {
        thisv->counter--;
    }
    posOffset = (thisv->dyna.actor.world.rot.y == 0) ? 24.0f : -24.0f;
    if (!Math_StepToF(&thisv->dyna.actor.world.pos.x, thisv->dyna.actor.home.pos.x + posOffset, 0.5f)) {
        BgHakaHuta_SpawnDust(thisv, globalCtx);
    }
    if (thisv->counter == 0) {
        BgHakaHuta_PlaySound(thisv, globalCtx, NA_SE_EV_COFFIN_CAP_BOUND);
        thisv->actionFunc = func_8087D720;
    }
}

void func_8087D720(BgHakaHuta* thisv, GlobalContext* globalCtx) {
    static Vec3f D_8087D958 = { 30.0f, 0.0f, 0.0f };
    static Vec3f D_8087D964 = { 0.03258f, 0.3258f, -0.9449f };
    MtxF mtx;
    Vec3f vec;
    s32 quakeIndex;

    thisv->counter++;
    if (thisv->counter == 6) {
        thisv->actionFunc = BgHakaHuta_DoNothing;
        quakeIndex = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
        Quake_SetSpeed(quakeIndex, 0x7530);
        Quake_SetQuakeValues(quakeIndex, 4, 0, 0, 0);
        Quake_SetCountdown(quakeIndex, 2);
    } else if (thisv->counter == 0) {
        thisv->counter = 6;
        thisv->actionFunc = BgHakaHuta_DoNothing;
    }

    D_8087D958.x = thisv->counter + 24.0f;
    if (D_8087D958.x > 30.0f) {
        D_8087D958.x = 30.0f;
    }
    Matrix_RotateY(thisv->dyna.actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
    Matrix_RotateAxis(thisv->counter * (191 * std::numbers::pi_v<float> / 3750), &D_8087D964, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_8087D958, &vec);
    thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x + vec.x;
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + vec.y;
    thisv->dyna.actor.world.pos.z = thisv->dyna.actor.home.pos.z + vec.z;
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &thisv->dyna.actor.shape.rot, 0);
}

void BgHakaHuta_DoNothing(BgHakaHuta* thisv, GlobalContext* globalCtx) {
}

void BgHakaHuta_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgHakaHuta* thisv = (BgHakaHuta*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgHakaHuta_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gBotwCoffinLidDL);
}
