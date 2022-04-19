/*
 * File: z_bg_po_syokudai.c
 * Overlay: ovl_Bg_Po_Syokudai
 * Description: Golden Torch Stand (Poe Sisters)
 */

#include "z_bg_po_syokudai.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_syokudai/object_syokudai.h"

#define FLAGS 0

typedef enum {
    POE_FLAME_PURPLE, // Meg
    POE_FLAME_RED,    // Joelle
    POE_FLAME_BLUE,   // Beth
    POE_FLAME_GREEN   // Amy
} PoeFlameColor;

#define POE_TORCH_FLAG 0x1C

void BgPoSyokudai_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgPoSyokudai_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgPoSyokudai_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgPoSyokudai_Draw(Actor* thisvx, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static Color_RGBA8 sPrimColors[] = {
    { 255, 170, 255, 255 },
    { 255, 200, 0, 255 },
    { 0, 170, 255, 255 },
    { 170, 255, 0, 255 },
};

static Color_RGBA8 sEnvColors[] = {
    { 100, 0, 255, 255 },
    { 255, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 0, 150, 0, 255 },
};

ActorInit Bg_Po_Syokudai_InitVars = {
    ACTOR_BG_PO_SYOKUDAI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SYOKUDAI,
    sizeof(BgPoSyokudai),
    (ActorFunc)BgPoSyokudai_Init,
    (ActorFunc)BgPoSyokudai_Destroy,
    (ActorFunc)BgPoSyokudai_Update,
    (ActorFunc)BgPoSyokudai_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgPoSyokudai_Init(Actor* thisvx, GlobalContext* globalCtx) {
    BgPoSyokudai* thisv = (BgPoSyokudai*)thisvx;
    s32 pad;

    Actor_ProcessInitChain(thisvx, sInitChain);

    thisv->flameColor = (thisvx->params >> 8) & 0xFF;
    thisvx->params &= 0x3F;

    thisvx->colChkInfo.mass = MASS_IMMOVABLE;

    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointGlowSetInfo(&thisv->lightInfo, thisvx->world.pos.x, (s16)thisvx->world.pos.y + 65, thisvx->world.pos.z, 0,
                            0, 0, 0);

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, thisvx, &sCylinderInit);

    thisv->collider.dim.pos.x = thisvx->world.pos.x;
    thisv->collider.dim.pos.y = thisvx->world.pos.y;
    thisv->collider.dim.pos.z = thisvx->world.pos.z;

    if (thisv->flameColor == POE_FLAME_PURPLE && Flags_GetSwitch(globalCtx, POE_TORCH_FLAG + POE_FLAME_GREEN) &&
        Flags_GetSwitch(globalCtx, POE_TORCH_FLAG + POE_FLAME_BLUE) &&
        Flags_GetSwitch(globalCtx, POE_TORCH_FLAG + POE_FLAME_RED) && !Flags_GetSwitch(globalCtx, thisvx->params)) {

        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, 119.0f, 225.0f, -1566.0f, 0, 0, 0,
                    thisvx->params);
        globalCtx->envCtx.unk_BF = 0x4;

    } else if (!Flags_GetSwitch(globalCtx, POE_TORCH_FLAG + POE_FLAME_PURPLE) && !Flags_GetSwitch(globalCtx, 0x1B)) {

        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisvx->world.pos.x,
                    thisvx->world.pos.y + 52.0f, thisvx->world.pos.z, 0, 0, 0,
                    (thisv->flameColor << 8) + thisvx->params + 0x1000);

    } else if (!Flags_GetSwitch(globalCtx, thisvx->params)) {
        if (globalCtx->envCtx.unk_BF == 0xFF) {
            globalCtx->envCtx.unk_BF = 4;
        }
    }

    thisv->flameTextureScroll = (s16)(Rand_ZeroOne() * 20.0f);
}

void BgPoSyokudai_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgPoSyokudai* thisv = (BgPoSyokudai*)thisvx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);

    if (globalCtx->envCtx.unk_BF != 0xFF) {
        globalCtx->envCtx.unk_BF = 0xFF;
    }
}

void BgPoSyokudai_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgPoSyokudai* thisv = (BgPoSyokudai*)thisvx;
    s32 pad;

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        func_8002F974(&thisv->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
    thisv->flameTextureScroll++;
}

void BgPoSyokudai_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    BgPoSyokudai* thisv = (BgPoSyokudai*)thisvx;
    f32 lightBrightness;
    u8 red;
    u8 green;
    u8 blue;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_syokudai.c", 315);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_po_syokudai.c", 319),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoldenTorchDL);

    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        Color_RGBA8* primColor = &sPrimColors[thisv->flameColor];
        Color_RGBA8* envColor = &sEnvColors[thisv->flameColor];

        lightBrightness = (0.3f * Rand_ZeroOne()) + 0.7f;

        red = (u8)(primColor->r * lightBrightness);
        green = (u8)(primColor->g * lightBrightness);
        blue = (u8)(primColor->b * lightBrightness);

        Lights_PointSetColorAndRadius(&thisv->lightInfo, red, green, blue, 200);

        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (thisv->flameTextureScroll * -20) & 0x1FF, 32, 128));

        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, primColor->r, primColor->g, primColor->b, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor->r, envColor->g, envColor->b, 255);

        Matrix_Translate(0.0f, 52.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->actor.shape.rot.y + 0x8000) *
                           (std::numbers::pi_v<float> / 0x8000),
                       MTXMODE_APPLY);
        Matrix_Scale(0.0027f, 0.0027f, 0.0027f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_po_syokudai.c", 368),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_syokudai.c", 373);
}
