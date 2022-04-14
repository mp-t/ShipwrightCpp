/*
 * File: z_oceff_storm.c
 * Overlay: ovl_Oceff_Storm
 * Description: Song of Storms Effect
 */

#include "z_oceff_storm.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

void OceffStorm_Init(Actor* thisx, GlobalContext* globalCtx);
void OceffStorm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void OceffStorm_Update(Actor* thisx, GlobalContext* globalCtx);
void OceffStorm_Draw(Actor* thisx, GlobalContext* globalCtx);

void OceffStorm_Draw2(Actor* thisx, GlobalContext* globalCtx);

void OceffStorm_DefaultAction(OceffStorm* thisv, GlobalContext* globalCtx);
void OceffStorm_UnkAction(OceffStorm* thisv, GlobalContext* globalCtx);

const ActorInit Oceff_Storm_InitVars = {
    ACTOR_OCEFF_STORM,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(OceffStorm),
    (ActorFunc)OceffStorm_Init,
    (ActorFunc)OceffStorm_Destroy,
    (ActorFunc)OceffStorm_Update,
    (ActorFunc)OceffStorm_Draw,
    NULL,
};

void OceffStorm_SetupAction(OceffStorm* thisv, OceffStormActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void OceffStorm_Init(Actor* thisx, GlobalContext* globalCtx) {
    OceffStorm* thisv = (OceffStorm*)thisx;
    OceffStorm_SetupAction(thisv, OceffStorm_DefaultAction);
    thisv->posYOffAdd = 0;
    thisv->counter = 0;
    thisv->primColorAlpha = 0;
    thisv->vtxAlpha = 0;
    thisv->actor.scale.y = 0.0f;
    thisv->actor.scale.z = 0.0f;
    thisv->actor.scale.x = 0.0f;
    thisv->posYOff = thisv->posYOffAdd;

    if (thisv->actor.params == 1) {
        OceffStorm_SetupAction(thisv, OceffStorm_UnkAction);
        thisv->actor.draw = OceffStorm_Draw2;
    } else {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_OKARINA_EFFECT, thisv->actor.world.pos.x,
                    thisv->actor.world.pos.y - 30.0f, thisv->actor.world.pos.z, 0, 0, 0, 1);
    }
}

void OceffStorm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    OceffStorm* thisv = (OceffStorm*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    func_800876C8(globalCtx);
    if (gSaveContext.nayrusLoveTimer != 0) {
        player->stateFlags3 |= 0x40;
    }
}

void OceffStorm_DefaultAction(OceffStorm* thisv, GlobalContext* globalCtx) {
    if (thisv->counter < 20) {
        thisv->primColorAlpha = (s8)(thisv->counter * 5.0f);
    } else if (thisv->counter > 80) {
        thisv->primColorAlpha = (s8)((100 - thisv->counter) * 5.0f);
    } else {
        thisv->primColorAlpha = 100;
    }

    if (thisv->counter < 10 || thisv->counter >= 90) {
        thisv->vtxAlpha = 0;
    } else {
        if (thisv->counter <= 65) {
            if (thisv->vtxAlpha <= 200) {
                thisv->vtxAlpha += 10;
            }
            thisv->actor.scale.x = thisv->actor.scale.z = 0.4f;
            thisv->actor.scale.y = 0.3f;
        } else if (thisv->counter > 65) {
            thisv->vtxAlpha = (90 - thisv->counter) * 10;
        } else {
            thisv->vtxAlpha = 255;
            thisv->actor.scale.x = thisv->actor.scale.z = 0.4f;
        }
    }

    if (thisv->counter > 60) {
        thisv->actor.world.pos.y += thisv->posYOff * 0.01f;
        thisv->posYOff += thisv->posYOffAdd;
        thisv->posYOffAdd += 10;
    }

    if (thisv->counter < 100) {
        thisv->counter++;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void OceffStorm_UnkAction(OceffStorm* thisv, GlobalContext* globalCtx) {
    if (thisv->primColorAlpha < 100) {
        thisv->primColorAlpha += 5;
    }
    //! @bug Actor_Kill is never called so the actor will stay alive forever
}

void OceffStorm_Update(Actor* thisx, GlobalContext* globalCtx) {
    OceffStorm* thisv = (OceffStorm*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    thisv->actor.world.pos = player->actor.world.pos;
    thisv->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx));
    thisv->actionFunc(thisv, globalCtx);
}

#include "overlays/ovl_Oceff_Storm/ovl_Oceff_Storm.h"

void OceffStorm_Draw2(Actor* thisx, GlobalContext* globalCtx) {
    u32 scroll = globalCtx->state.frames & 0xFFF;
    OceffStorm* thisv = (OceffStorm*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_oceff_storm.c", 449);

    gDPPipeSync(POLY_XLU_DISP++);

    if (1) {}

    POLY_XLU_DISP = func_80093F34(POLY_XLU_DISP);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 150, thisv->primColorAlpha);
    gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, scroll * 8, scroll * 4, 64, 64, 1,
                                                     scroll * 4, scroll * 4, 64, 64));
    gSPTextureRectangle(POLY_XLU_DISP++, 0, 0, (SCREEN_WIDTH << 2), (SCREEN_HEIGHT << 2), G_TX_RENDERTILE, 0, 0, 140,
                        (1 << 15) | (31 << 10) | 884);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_oceff_storm.c", 477);
}

void OceffStorm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    u32 scroll = globalCtx->state.frames & 0xFFF;
    OceffStorm* thisv = (OceffStorm*)thisx;
    Vtx* vtxPtr = ResourceMgr_LoadVtxByName(sCylinderVtx);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_oceff_storm.c", 486);

    func_80093D84(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 200, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 0, 128);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);

    vtxPtr[0].v.cn[3] = vtxPtr[6].v.cn[3] = vtxPtr[16].v.cn[3] = vtxPtr[25].v.cn[3] = thisv->vtxAlpha >> 1;
    vtxPtr[10].v.cn[3] = vtxPtr[22].v.cn[3] = thisv->vtxAlpha;

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_oceff_storm.c", 498),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, sCylinderMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, scroll * 4, (0 - scroll) * 8, 32, 32,
                                                     1, scroll * 8, (0 - scroll) * 12, 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, sCylinderModelDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_oceff_storm.c", 512);

    OceffStorm_Draw2(&thisv->actor, globalCtx);
}
