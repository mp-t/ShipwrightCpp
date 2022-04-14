/*
 * File: z_en_dy_extra.c
 * Overlay: ovl_En_Dy_Extra
 * Description: Spiral Beams (Great Fairy Fountains)
 */

#include "z_en_dy_extra.h"
#include "objects/object_dy_obj/object_dy_obj.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnDyExtra_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDyExtra_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDyExtra_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDyExtra_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDyExtra_WaitForTrigger(EnDyExtra* thisv, GlobalContext* globalCtx);
void EnDyExtra_FallAndKill(EnDyExtra* thisv, GlobalContext* globalCtx);

const ActorInit En_Dy_Extra_InitVars = {
    ACTOR_EN_DY_EXTRA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DY_OBJ,
    sizeof(EnDyExtra),
    (ActorFunc)EnDyExtra_Init,
    (ActorFunc)EnDyExtra_Destroy,
    (ActorFunc)EnDyExtra_Update,
    (ActorFunc)EnDyExtra_Draw,
    NULL,
};

void EnDyExtra_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnDyExtra_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDyExtra* thisv = (EnDyExtra*)thisx;

    osSyncPrintf("\n\n");
    // "Big fairy effect"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 大妖精効果 ☆☆☆☆☆ %d\n" VT_RST, thisv->actor.params);
    thisv->type = thisv->actor.params;
    thisv->scale.x = 0.025f;
    thisv->scale.y = 0.039f;
    thisv->scale.z = 0.025f;
    thisv->unk_168 = thisv->actor.world.pos;
    thisv->actor.gravity = -0.2f;
    thisv->unk_158 = 1.0f;
    thisv->timer = 60;
    thisv->actionFunc = EnDyExtra_WaitForTrigger;
}

void EnDyExtra_WaitForTrigger(EnDyExtra* thisv, GlobalContext* globalCtx) {
    Math_ApproachF(&thisv->actor.gravity, 0.0f, 0.1f, 0.005f);
    if (thisv->actor.world.pos.y < -55.0f) {
        thisv->actor.velocity.y = 0.0f;
    }
    if (thisv->timer == 0 && thisv->trigger != 0) {
        thisv->timer = 200;
        thisv->actionFunc = EnDyExtra_FallAndKill;
    }
}

void EnDyExtra_FallAndKill(EnDyExtra* thisv, GlobalContext* globalCtx) {
    Math_ApproachF(&thisv->actor.gravity, 0.0f, 0.1f, 0.005f);
    if (thisv->timer == 0 || thisv->unk_158 < 0.02f) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Math_ApproachZeroF(&thisv->unk_158, 0.03f, 0.05f);
    if (thisv->actor.world.pos.y < -55.0f) {
        thisv->actor.velocity.y = 0.0f;
    }
}

void EnDyExtra_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDyExtra* thisv = (EnDyExtra*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    thisv->actor.scale.x = thisv->scale.x;
    thisv->actor.scale.y = thisv->scale.y;
    thisv->actor.scale.z = thisv->scale.z;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_SPIRAL_HEAL_BEAM - SFX_FLAG);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
}

void EnDyExtra_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Color_RGBA8 primColors[] = { { 255, 255, 170, 255 }, { 255, 255, 170, 255 } };
    static Color_RGBA8 envColors[] = { { 255, 100, 255, 255 }, { 100, 255, 255, 255 } };
    static u8 D_809FFC50[] = { 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02,
                               0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x01, 0x02 };
    EnDyExtra* thisv = (EnDyExtra*)thisx;
    s32 pad;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    Vtx* vertices = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(gGreatFairySpiralBeamVtx));
    s32 i;
    u8 unk[3];

    unk[0] = 0.0f;
    unk[1] = (s8)(thisv->unk_158 * 240.0f);
    unk[2] = (s8)(thisv->unk_158 * 255.0f);

    for (i = 0; i < 27; i++) {
        if (D_809FFC50[i]) {
            vertices[i].v.cn[3] = unk[D_809FFC50[i]];
        }
    }

    OPEN_DISPS(gfxCtx, "../z_en_dy_extra.c", 294);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->state.frames * 2, 0, 0x20, 0x40, 1,
                                globalCtx->state.frames, globalCtx->state.frames * -8, 0x10, 0x10));
    gDPPipeSync(POLY_XLU_DISP++);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dy_extra.c", 307),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColors[thisv->type].r, primColors[thisv->type].g,
                    primColors[thisv->type].b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, envColors[thisv->type].r, envColors[thisv->type].g, envColors[thisv->type].b, 128);
    gSPDisplayList(POLY_XLU_DISP++, gGreatFairySpiralBeamDL);

    CLOSE_DISPS(gfxCtx, "../z_en_dy_extra.c", 325);
}
