/*
 * File: z_en_vase.c
 * Overlay: ovl_En_Vase
 * Description: An unused, orange pot based on ALTTP. Lacks collision.
 */

#include "z_en_vase.h"
#include "objects/object_vase/object_vase.h"

#define FLAGS ACTOR_FLAG_4

void EnVase_Init(Actor* thisx, GlobalContext* globalCtx);
void EnVase_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnVase_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Vase_InitVars = {
    ACTOR_EN_VASE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_VASE,
    sizeof(EnVase),
    (ActorFunc)EnVase_Init,
    (ActorFunc)EnVase_Destroy,
    (ActorFunc)Actor_Noop,
    (ActorFunc)EnVase_Draw,
    NULL,
};

void EnVase_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnVase* thisv = (EnVase*)thisx;

    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 6.0f);
}

void EnVase_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnVase_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gUnusedVaseDL);
}
