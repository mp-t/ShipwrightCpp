/*
 * File: z_en_arow_trap.c
 * Overlay: ovl_En_Arow_Trap
 * Description: An unused trap that reflects arrows.
 */

#include "z_en_arow_trap.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#define FLAGS ACTOR_FLAG_4

void EnArowTrap_Init(Actor* thisx, GlobalContext* globalCtx);
void EnArowTrap_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnArowTrap_Update(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Arow_Trap_InitVars = {
    ACTOR_EN_AROW_TRAP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnArowTrap),
    (ActorFunc)EnArowTrap_Init,
    (ActorFunc)EnArowTrap_Destroy,
    (ActorFunc)EnArowTrap_Update,
    NULL,
    NULL,
};

void EnArowTrap_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnArowTrap* thisv = (EnArowTrap*)thisx;

    Actor_SetScale(&thisv->actor, 0.01);
    thisv->unk_14C = 0;
    thisv->attackTimer = 80;
    thisv->actor.focus.pos = thisv->actor.world.pos;
}

void EnArowTrap_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnArowTrap_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnArowTrap* thisv = (EnArowTrap*)thisx;

    if (thisv->actor.xzDistToPlayer <= 400) {
        thisv->attackTimer--;

        if (thisv->attackTimer == 0) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ARROW, thisv->actor.world.pos.x,
                        thisv->actor.world.pos.y, thisv->actor.world.pos.z, thisv->actor.shape.rot.x,
                        thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, ARROW_NORMAL_SILENT);
            thisv->attackTimer = 80;
        }
    }
}
