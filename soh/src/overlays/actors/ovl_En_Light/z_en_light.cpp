/*
 * File: z_en_light.c
 * Overlay: ovl_En_Light
 * Description: Flame
 */

#include "z_en_light.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS 0

void EnLight_Init(Actor* thisx, GlobalContext* globalCtx);
void EnLight_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnLight_Update(Actor* thisx, GlobalContext* globalCtx);
void EnLight_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnLight_UpdateSwitch(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Light_InitVars = {
    ACTOR_EN_LIGHT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnLight),
    (ActorFunc)EnLight_Init,
    (ActorFunc)EnLight_Destroy,
    (ActorFunc)EnLight_Update,
    (ActorFunc)EnLight_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ Color_RGBA8 primColor;
    /* 0x04 */ Color_RGB8 envColor;
    /* 0x07 */ u8 scale;
} FlameParams;

static FlameParams D_80A9E840[] = {
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },     { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 40 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },     { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },
    { { 255, 0, 170, 255 }, { 200, 0, 0 }, 75 },     { { 255, 255, 170, 255 }, { 255, 50, 0 }, 75 },
    { { 255, 255, 170, 255 }, { 255, 255, 0 }, 75 }, { { 255, 255, 170, 255 }, { 100, 255, 0 }, 75 },
    { { 255, 170, 255, 255 }, { 255, 0, 100 }, 75 }, { { 255, 170, 255, 255 }, { 100, 0, 255 }, 75 },
    { { 170, 255, 255, 255 }, { 0, 0, 255 }, 75 },   { { 170, 255, 255, 255 }, { 0, 150, 255 }, 75 },
};

void EnLight_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnLight* thisv = (EnLight*)thisx;
    s16 yOffset;

    if (gSaveContext.gameMode == 3) {
        // special case for the credits
        yOffset = (thisv->actor.params < 0) ? 1 : 40;
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, yOffset + (s16)thisv->actor.world.pos.y,
                                  thisv->actor.world.pos.z, 255, 255, 180, -1);
    } else {
        yOffset = (thisv->actor.params < 0) ? 1 : 40;
        Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, yOffset + (s16)thisv->actor.world.pos.y,
                                thisv->actor.world.pos.z, 255, 255, 180, -1);
    }

    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Actor_SetScale(&thisv->actor, D_80A9E840[thisv->actor.params & 0xF].scale * 0.0001f);
    thisv->timer = (s32)(Rand_ZeroOne() * 255.0f);

    if ((thisv->actor.params & 0x400) != 0) {
        thisv->actor.update = EnLight_UpdateSwitch;
    }
}

void EnLight_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnLight* thisv = (EnLight*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
}

void EnLight_UpdatePosRot(EnLight* thisv, GlobalContext* globalCtx) {
    // update yaw for billboard effect
    thisv->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000;

    if (thisv->actor.parent != NULL) {
        Math_Vec3f_Copy(&thisv->actor.world.pos, &(thisv->actor.parent)->world.pos);
        thisv->actor.world.pos.y += 17.0f;
    }

    thisv->timer++;
}

void EnLight_Update(Actor* thisx, GlobalContext* globalCtx) {
    f32 intensity;
    FlameParams* flameParams;
    s16 radius;
    EnLight* thisv = (EnLight*)thisx;

    flameParams = &D_80A9E840[thisv->actor.params & 0xF];
    intensity = (Rand_ZeroOne() * 0.5f) + 0.5f;
    radius = (thisv->actor.params < 0) ? 100 : 300;
    Lights_PointSetColorAndRadius(&thisv->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  radius);
    EnLight_UpdatePosRot(thisv, globalCtx);

    if (thisv->actor.params >= 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void EnLight_UpdateSwitch(Actor* thisx, GlobalContext* globalCtx) {
    f32 intensity;
    FlameParams* flameParams;
    EnLight* thisv = (EnLight*)thisx;
    f32 scale;

    flameParams = &D_80A9E840[thisv->actor.params & 0xF];
    scale = thisv->actor.scale.x / ((f32)flameParams->scale * 0.0001);

    if ((thisv->actor.params & 0x800) != 0) {
        if (Flags_GetSwitch(globalCtx, (thisv->actor.params & 0x3F0) >> 4)) {
            Math_StepToF(&scale, 1.0f, 0.05f);
        } else {
            if (scale < 0.1f) {
                Actor_SetScale(&thisv->actor, 0.0f);
                return;
            }
            Math_StepToF(&scale, 0.0f, 0.05f);
        }
    } else {
        if (Flags_GetSwitch(globalCtx, (thisv->actor.params & 0x3F0) >> 4)) {
            if (scale < 0.1f) {
                Actor_SetScale(&thisv->actor, 0.0f);
                return;
            }
            Math_StepToF(&scale, 0.0f, 0.05f);
        } else {
            Math_StepToF(&scale, 1.0f, 0.05f);
        }
    }

    Actor_SetScale(&thisv->actor, ((f32)flameParams->scale * 0.0001) * scale);
    intensity = (Rand_ZeroOne() * 0.5f) + 0.5f;
    Lights_PointSetColorAndRadius(&thisv->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  300.0f * scale);
    EnLight_UpdatePosRot(thisv, globalCtx);

    if (thisv->actor.params >= 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void EnLight_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnLight* thisv = (EnLight*)thisx;
    s32 pad;
    FlameParams* flameParams;
    const Gfx* dList;

    if (1) {}

    flameParams = &D_80A9E840[thisv->actor.params & 0xF];

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_light.c", 441);

    func_80093D84(globalCtx->state.gfxCtx);

    if (thisv->actor.params >= 0) {
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (thisv->timer * -20) & 511, 32, 128));

        dList = gEffFire1DL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, flameParams->primColor.r, flameParams->primColor.g,
                        flameParams->primColor.b, flameParams->primColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, flameParams->envColor.r, flameParams->envColor.g, flameParams->envColor.b, 0);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 16, 32, 1, ((thisv->timer * 2) & 63),
                                    (thisv->timer * -6) & 127 * 1, 16, 32));

        dList = gUnusedCandleDL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0xC0, 0xC0, 255, 200, 0, 0);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    }

    Matrix_RotateY((s16)((Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y) + 0x8000) *
                       (std::numbers::pi_v<float> / 32768.0f),
                   MTXMODE_APPLY);

    if (thisv->actor.params & 1) {
        Matrix_RotateY(std::numbers::pi_v<float>, MTXMODE_APPLY);
    }

    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_light.c", 488),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, dList);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_light.c", 491);
}
