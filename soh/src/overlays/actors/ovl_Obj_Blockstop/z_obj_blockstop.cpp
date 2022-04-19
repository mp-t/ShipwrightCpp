/*
 * File: z_obj_blockstop.c
 * Overlay: ovl_Obj_Blockstop
 * Description: Stops blocks and sets relevant flags when the block is in position.
 */

#include "z_obj_blockstop.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"

#define FLAGS 0

void ObjBlockstop_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjBlockstop_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjBlockstop_Update(Actor* thisx, GlobalContext* globalCtx);

ActorInit Obj_Blockstop_InitVars = {
    ACTOR_OBJ_BLOCKSTOP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjBlockstop),
    (ActorFunc)ObjBlockstop_Init,
    (ActorFunc)ObjBlockstop_Destroy,
    (ActorFunc)ObjBlockstop_Update,
    NULL,
    NULL,
};

void ObjBlockstop_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjBlockstop* thisv = (ObjBlockstop*)thisx;

    if (Flags_GetSwitch(globalCtx, thisv->actor.params)) {
        Actor_Kill(&thisv->actor);
    } else {
        thisv->actor.world.pos.y++;
    }
}

void ObjBlockstop_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ObjBlockstop_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjBlockstop* thisv = (ObjBlockstop*)thisx;
    DynaPolyActor* dynaPolyActor;
    Vec3f sp4C;
    s32 bgId;
    s32 pad;

    if (BgCheck_EntityLineTest2(&globalCtx->colCtx, &thisv->actor.home.pos, &thisv->actor.world.pos, &sp4C,
                                &thisv->actor.floorPoly, false, false, true, true, &bgId, &thisv->actor)) {
        dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, bgId);

        if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_OBJ_OSHIHIKI) {
            if ((dynaPolyActor->actor.params & 0x000F) == PUSHBLOCK_HUGE_START_ON ||
                (dynaPolyActor->actor.params & 0x000F) == PUSHBLOCK_HUGE_START_OFF) {
                func_80078884(NA_SE_SY_CORRECT_CHIME);
            } else {
                func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            }

            Flags_SetSwitch(globalCtx, thisv->actor.params);
            Actor_Kill(&thisv->actor);
        }
    }
}
