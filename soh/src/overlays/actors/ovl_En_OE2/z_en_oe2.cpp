/*
 * File: z_en_oe2.c
 * Overlay: ovl_En_Oe2
 * Description: Blue Navi Target Spot
 */

#include "z_en_oe2.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnOE2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOE2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOE2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnOE2_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnOE2_DoNothing(EnOE2* thisv, GlobalContext* globalCtx);

ActorInit En_OE2_InitVars = {
    ACTOR_EN_OE2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OE2,
    sizeof(EnOE2),
    (ActorFunc)EnOE2_Init,
    (ActorFunc)EnOE2_Destroy,
    (ActorFunc)EnOE2_Update,
    (ActorFunc)EnOE2_Draw,
    NULL,
};

void EnOE2_SetupAction(EnOE2* thisv, EnOE2ActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnOE2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOE2* thisv = (EnOE2*)thisx;

    EnOE2_SetupAction(thisv, EnOE2_DoNothing);
}

void EnOE2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnOE2_DoNothing(EnOE2* thisv, GlobalContext* globalCtx) {
}

void EnOE2_Update(Actor* thisx, GlobalContext* globalCtx) {
}

void EnOE2_Draw(Actor* thisx, GlobalContext* globalCtx) {
}
