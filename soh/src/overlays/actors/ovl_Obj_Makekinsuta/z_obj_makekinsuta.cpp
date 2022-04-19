/*
 * File: z_obj_makekinsuta.c
 * Overlay: ovl_Obj_Makekinsuta
 * Description: Skulltula Sprouting from Bean Spot
 */

#include "z_obj_makekinsuta.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void ObjMakekinsuta_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjMakekinsuta_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B98320(ObjMakekinsuta* thisv, GlobalContext* globalCtx);
void ObjMakekinsuta_DoNothing(ObjMakekinsuta* thisv, GlobalContext* globalCtx);

ActorInit Obj_Makekinsuta_InitVars = {
    ACTOR_OBJ_MAKEKINSUTA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMakekinsuta),
    (ActorFunc)ObjMakekinsuta_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjMakekinsuta_Update,
    NULL,
    NULL,
};

void ObjMakekinsuta_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjMakekinsuta* thisv = (ObjMakekinsuta*)thisx;

    if ((thisv->actor.params & 0x6000) == 0x4000) {
        osSyncPrintf(VT_FGCOL(BLUE));
        // "Gold Star Enemy(arg_data %x)"
        osSyncPrintf("金スタ発生敵(arg_data %x)\n", thisv->actor.params);
        osSyncPrintf(VT_RST);
    } else {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Invalid Argument (arg_data %x)(%s %d)"
        osSyncPrintf("引数不正 (arg_data %x)(%s %d)\n", thisv->actor.params, "../z_obj_makekinsuta.c", 119);
        osSyncPrintf(VT_RST);
    }
    thisv->actionFunc = func_80B98320;
}

void func_80B98320(ObjMakekinsuta* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_152 != 0) {
        if (thisv->timer >= 60 && !func_8002DEEC(GET_PLAYER(globalCtx))) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_SW, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                        thisv->actor.world.pos.z, 0, thisv->actor.shape.rot.y, 0, (thisv->actor.params | 0x8000));
            thisv->actionFunc = ObjMakekinsuta_DoNothing;
        } else {
            thisv->timer++;
        }
    } else {
        thisv->timer = 0;
    }
}

void ObjMakekinsuta_DoNothing(ObjMakekinsuta* thisv, GlobalContext* globalCtx) {
}

void ObjMakekinsuta_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjMakekinsuta* thisv = (ObjMakekinsuta*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}
