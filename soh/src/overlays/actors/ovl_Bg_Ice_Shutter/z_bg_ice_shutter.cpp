/*
 * File: z_bg_ice_shutter.c
 * Overlay: ovl_Bg_Ice_Shutter
 * Description: Vertical Ice Bars (Doors) in Ice Cavern
 */

#include "z_bg_ice_shutter.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgIceShutter_Init(Actor* thisx, GlobalContext* globalCtx);
void BgIceShutter_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgIceShutter_Update(Actor* thisx, GlobalContext* globalCtx);
void BgIceShutter_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80891CF4(BgIceShutter* thisx, GlobalContext* globalCtx);
void func_80891D6C(BgIceShutter* thisx, GlobalContext* globalCtx);
void func_80891DD4(BgIceShutter* thisx, GlobalContext* globalCtx);

ActorInit Bg_Ice_Shutter_InitVars = {
    ACTOR_BG_ICE_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_ICE_OBJECTS,
    sizeof(BgIceShutter),
    (ActorFunc)BgIceShutter_Init,
    (ActorFunc)BgIceShutter_Destroy,
    (ActorFunc)BgIceShutter_Update,
    (ActorFunc)BgIceShutter_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80891AC0(BgIceShutter* thisv) {
    f32 sp24;

    sp24 = Math_SinS(thisv->dyna.actor.shape.rot.x) * thisv->dyna.actor.velocity.y;
    thisv->dyna.actor.world.pos.y =
        (Math_CosS(thisv->dyna.actor.shape.rot.x) * thisv->dyna.actor.velocity.y) + thisv->dyna.actor.home.pos.y;
    thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->dyna.actor.shape.rot.y) * sp24) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->dyna.actor.shape.rot.y) * sp24) + thisv->dyna.actor.home.pos.z;
}

void BgIceShutter_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgIceShutter* thisv = (BgIceShutter*)thisx;
    f32 sp24;
    const CollisionHeader* colHeader;
    s32 sp28;
    f32 temp_f6;

    colHeader = NULL;
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    sp28 = thisv->dyna.actor.params & 0xFF;
    thisv->dyna.actor.params = (thisv->dyna.actor.params >> 8) & 0xFF;
    CollisionHeader_GetVirtual(&object_ice_objects_Col_002854, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (sp28 == 2) {
        thisv->dyna.actor.shape.rot.x = -0x4000;
    }

    if (sp28 != 1) {
        if (Flags_GetClear(globalCtx, thisv->dyna.actor.room)) {
            Actor_Kill(&thisv->dyna.actor);
        } else {
            thisv->actionFunc = func_80891CF4;
        }

    } else {
        if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params)) {
            Actor_Kill(&thisv->dyna.actor);
        } else {
            thisv->actionFunc = func_80891D6C;
        }
    }

    if (sp28 == 2) {
        temp_f6 = Math_SinS(thisv->dyna.actor.shape.rot.x) * 50.0f;
        thisv->dyna.actor.focus.pos.x =
            (Math_SinS(thisv->dyna.actor.shape.rot.y) * temp_f6) + thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.focus.pos.y = thisv->dyna.actor.home.pos.y;
        thisv->dyna.actor.focus.pos.z =
            thisv->dyna.actor.home.pos.z + (Math_CosS(thisv->dyna.actor.shape.rot.y) * temp_f6);
    } else {
        Actor_SetFocus(&thisv->dyna.actor, 50.0f);
    }
}

void BgIceShutter_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgIceShutter* thisv = (BgIceShutter*)thisx;
    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80891CF4(BgIceShutter* thisv, GlobalContext* globalCtx) {
    if (Flags_GetTempClear(globalCtx, thisv->dyna.actor.room)) {
        Flags_SetClear(globalCtx, thisv->dyna.actor.room);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        thisv->actionFunc = func_80891DD4;
        if (thisv->dyna.actor.shape.rot.x == 0) {
            OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        }
    }
}

void func_80891D6C(BgIceShutter* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params)) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 30, NA_SE_EV_SLIDE_DOOR_OPEN);
        thisv->actionFunc = func_80891DD4;
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
    }
}

void func_80891DD4(BgIceShutter* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->dyna.actor.speedXZ, 30.0f, 2.0f);
    if (Math_StepToF(&thisv->dyna.actor.velocity.y, 210.0f, thisv->dyna.actor.speedXZ)) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }

    func_80891AC0(thisv);
}

void BgIceShutter_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgIceShutter* thisv = (BgIceShutter*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgIceShutter_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, object_ice_objects_DL_002740);
}
