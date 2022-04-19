/*
 * File: z_door_toki.c
 * Overlay: ovl_Door_Toki
 * Description: Door of Time Collision
 */

#include "z_door_toki.h"
#include "objects/object_toki_objects/object_toki_objects.h"

#define FLAGS 0

void DoorToki_Init(Actor* thisx, GlobalContext* globalCtx);
void DoorToki_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DoorToki_Update(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Door_Toki_InitVars = {
    ACTOR_DOOR_TOKI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_TOKI_OBJECTS,
    sizeof(DoorToki),
    (ActorFunc)DoorToki_Init,
    (ActorFunc)DoorToki_Destroy,
    (ActorFunc)DoorToki_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void DoorToki_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    DoorToki* thisv = (DoorToki*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gDoorTokiCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void DoorToki_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DoorToki* thisv = (DoorToki*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void DoorToki_Update(Actor* thisx, GlobalContext* globalCtx) {
    DoorToki* thisv = (DoorToki*)thisx;

    if (gSaveContext.eventChkInf[4] & 0x800) {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    } else {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}
