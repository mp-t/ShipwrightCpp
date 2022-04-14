/*
 * File: z_bg_umajump.c
 * Overlay: ovl_Bg_Umajump
 * Description: Hoppable horse fence
 */

#include "z_bg_umajump.h"
#include "objects/object_umajump/object_umajump.h"

#define FLAGS 0

void BgUmaJump_Init(Actor* thisx, GlobalContext* globalCtx);
void BgUmaJump_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgUmaJump_Update(Actor* thisx, GlobalContext* globalCtx);
void BgUmaJump_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Umajump_InitVars = {
    ACTOR_BG_UMAJUMP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_UMAJUMP,
    sizeof(BgUmaJump),
    (ActorFunc)BgUmaJump_Init,
    (ActorFunc)BgUmaJump_Destroy,
    (ActorFunc)BgUmaJump_Update,
    (ActorFunc)BgUmaJump_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgUmaJump_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgUmaJump* thisv = (BgUmaJump*)thisx;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gJumpableHorseFenceCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.actor.params == 1) {
        if (!Flags_GetEventChkInf(0x18) && (DREG(1) == 0)) {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
        thisv->dyna.actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    }
}

void BgUmaJump_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgUmaJump* thisv = (BgUmaJump*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgUmaJump_Update(Actor* thisx, GlobalContext* globalCtx) {
}

void BgUmaJump_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gJumpableHorseFenceDL);
}
