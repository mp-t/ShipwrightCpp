/*
 * File: z_bg_gnd_firemeiro.c
 * Overlay: ovl_Bg_Gnd_Firemeiro
 * Description: Sinking lava platform (Ganon's Castle)
 */

#include "z_bg_gnd_firemeiro.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgGndFiremeiro_Init(Actor* thisx, GlobalContext* globalCtx);
void BgGndFiremeiro_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgGndFiremeiro_Update(Actor* thisx, GlobalContext* globalCtx);
void BgGndFiremeiro_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgGndFiremeiro_Sink(BgGndFiremeiro* thisv, GlobalContext* globalCtx);
void BgGndFiremeiro_Shake(BgGndFiremeiro* thisv, GlobalContext* globalCtx);
void BgGndFiremeiro_Rise(BgGndFiremeiro* thisv, GlobalContext* globalCtx);

ActorInit Bg_Gnd_Firemeiro_InitVars = {
    ACTOR_BG_GND_FIREMEIRO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndFiremeiro),
    (ActorFunc)BgGndFiremeiro_Init,
    (ActorFunc)BgGndFiremeiro_Destroy,
    (ActorFunc)BgGndFiremeiro_Update,
    (ActorFunc)BgGndFiremeiro_Draw,
    NULL,
};

void BgGndFiremeiro_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgGndFiremeiro* thisv = (BgGndFiremeiro*)thisx;
    const CollisionHeader* colHeader = NULL;

    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&thisv->dyna.actor, 0.1f);
    thisv->initPos = thisv->dyna.actor.world.pos;

    if (thisv->dyna.actor.params == 0) {
        DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
        CollisionHeader_GetVirtual(&gFireTrialPlatformCol, &colHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
        thisv->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgGndFiremeiro* thisv = (BgGndFiremeiro*)thisx;

    if (thisv->dyna.actor.params == 0) {
        if (1) {}
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void BgGndFiremeiro_Sink(BgGndFiremeiro* thisv, GlobalContext* globalCtx) {
    f32 sunkHeight = thisv->initPos.y - 150.0f;

    if (func_8004356C(&thisv->dyna)) {
        thisv->timer = 10;
    }

    if (sunkHeight < thisv->dyna.actor.world.pos.y) {
        thisv->dyna.actor.world.pos.y -= 0.5f;

        if (thisv->dyna.actor.world.pos.y < sunkHeight) {
            thisv->dyna.actor.world.pos.y = sunkHeight;
        }

        func_8002F948(&thisv->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Shake(BgGndFiremeiro* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 randSign;

    if (func_8004356C(&thisv->dyna)) { // Player standing on it
        if (thisv->timer > 0) {
            thisv->timer--;

            randSign = ((thisv->timer & 1) ? 2.0f : -2.0f);

            thisv->dyna.actor.world.pos = thisv->initPos;
            thisv->dyna.actor.world.pos.x += randSign * Math_SinS(thisv->timer * 0x2FFF);
            thisv->dyna.actor.world.pos.z += randSign * Math_CosS(thisv->timer * 0x2FFF);
            thisv->dyna.actor.world.pos.y += Math_CosS(thisv->timer * 0x7FFF);

            if (!(thisv->timer % 4)) {
                Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
            }
        } else {
            thisv->timer = 10;
            thisv->dyna.actor.world.pos = thisv->initPos;
            thisv->actionFunc = BgGndFiremeiro_Sink;
        }
    } else {
        thisv->dyna.actor.world.pos = thisv->initPos;
        thisv->actionFunc = BgGndFiremeiro_Rise;
    }
}

void BgGndFiremeiro_Rise(BgGndFiremeiro* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* thisx = &thisv->dyna.actor;

    if ((player->currentBoots != PLAYER_BOOTS_HOVER) && func_8004356C(&thisv->dyna)) { // Player standing on it
        if (thisx->world.pos.y < thisv->initPos.y) {
            thisv->actionFunc = BgGndFiremeiro_Sink;
            thisv->timer = 20;
        } else {
            thisv->actionFunc = BgGndFiremeiro_Shake;
            thisv->timer = 20;
        }
    } else {
        if (thisx->world.pos.y < thisv->initPos.y) {
            thisx->world.pos.y += 2.0f;
            if (thisv->initPos.y < thisx->world.pos.y) {
                thisx->world.pos.y = thisv->initPos.y;
            }
        }
    }
}

void BgGndFiremeiro_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgGndFiremeiro* thisv = (BgGndFiremeiro*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgGndFiremeiro_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 280);
    func_800943C8(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 282),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFireTrialPlatformDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 285);
}
