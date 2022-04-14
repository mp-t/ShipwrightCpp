/*
 * File: z_obj_roomtimer.c
 * Overlay: ovl_Obj_Roomtimer
 * Description: Starts Timer 1 with a value specified in params
 */

#include "z_obj_roomtimer.h"

#define FLAGS ACTOR_FLAG_4

void ObjRoomtimer_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjRoomtimer_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjRoomtimer_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B9D054(ObjRoomtimer* thisv, GlobalContext* globalCtx);
void func_80B9D0B0(ObjRoomtimer* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Roomtimer_InitVars = {
    ACTOR_OBJ_ROOMTIMER,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjRoomtimer),
    (ActorFunc)ObjRoomtimer_Init,
    (ActorFunc)ObjRoomtimer_Destroy,
    (ActorFunc)ObjRoomtimer_Update,
    (ActorFunc)NULL,
    NULL,
};

void ObjRoomtimer_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* thisv = (ObjRoomtimer*)thisx;
    s16 params = thisv->actor.params;

    thisv->switchFlag = (params >> 10) & 0x3F;
    thisv->actor.params = params & 0x3FF;
    params = thisv->actor.params;

    if (params != 0x3FF) {
        if (params > 600) {
            thisv->actor.params = 600;
        } else {
            thisv->actor.params = params;
        }
    }

    thisv->actionFunc = func_80B9D054;
}

void ObjRoomtimer_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* thisv = (ObjRoomtimer*)thisx;

    if ((thisv->actor.params != 0x3FF) && (gSaveContext.timer1Value > 0)) {
        gSaveContext.timer1State = 10;
    }
}

void func_80B9D054(ObjRoomtimer* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params != 0x3FF) {
        func_80088B34(thisv->actor.params);
    }

    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
    thisv->actionFunc = func_80B9D0B0;
}

void func_80B9D0B0(ObjRoomtimer* thisv, GlobalContext* globalCtx) {
    if (Flags_GetTempClear(globalCtx, thisv->actor.room)) {
        if (thisv->actor.params != 0x3FF) {
            gSaveContext.timer1State = 10;
        }
        Flags_SetClear(globalCtx, thisv->actor.room);
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Actor_Kill(&thisv->actor);
    } else {
        if ((thisv->actor.params != 0x3FF) && (gSaveContext.timer1Value == 0)) {
            Audio_PlaySoundGeneral(NA_SE_OC_ABYSS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            Gameplay_TriggerVoidOut(globalCtx);
            Actor_Kill(&thisv->actor);
        }
    }
}

void ObjRoomtimer_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjRoomtimer* thisv = (ObjRoomtimer*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}
