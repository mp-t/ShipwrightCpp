/*
 * File: z_bg_spot18_shutter.c
 * Overlay: Bg_Spot18_Shutter
 * Description:
 */

#include "z_bg_spot18_shutter.h"
#include "objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgSpot18Shutter_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Shutter_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Shutter_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Shutter_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B95AC(BgSpot18Shutter* thisv, GlobalContext* globalCtx);
void func_808B95B8(BgSpot18Shutter* thisv, GlobalContext* globalCtx);
void func_808B9618(BgSpot18Shutter* thisv, GlobalContext* globalCtx);
void func_808B9698(BgSpot18Shutter* thisv, GlobalContext* globalCtx);
void func_808B971C(BgSpot18Shutter* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot18_Shutter_InitVars = {
    ACTOR_BG_SPOT18_SHUTTER,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SPOT18_OBJ,
    sizeof(BgSpot18Shutter),
    (ActorFunc)BgSpot18Shutter_Init,
    (ActorFunc)BgSpot18Shutter_Destroy,
    (ActorFunc)BgSpot18Shutter_Update,
    (ActorFunc)BgSpot18Shutter_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpot18Shutter_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot18Shutter* thisv = (BgSpot18Shutter*)thisx;
    s32 param = (thisv->dyna.actor.params >> 8) & 1;
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    if (param == 0) {
        if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
            if (gSaveContext.infTable[16] & 0x200) {
                thisv->actionFunc = func_808B95AC;
                thisv->dyna.actor.world.pos.y += 180.0f;
            } else {
                thisv->actionFunc = func_808B9618;
            }
        } else {
            if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
                thisv->actionFunc = func_808B95AC;
                thisv->dyna.actor.world.pos.y += 180.0f;
            } else {
                thisv->actionFunc = func_808B95B8;
            }
        }
    } else {
        if (gSaveContext.infTable[16] & 0x200) {
            thisv->dyna.actor.world.pos.x += 125.0f * Math_CosS(thisv->dyna.actor.world.rot.y);
            thisv->dyna.actor.world.pos.z -= 125.0f * Math_SinS(thisv->dyna.actor.world.rot.y);
            thisv->actionFunc = func_808B95AC;
        } else {
            thisv->actionFunc = func_808B9618;
        }
    }

    CollisionHeader_GetVirtual(&gGoronCityDoorCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
}

void BgSpot18Shutter_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Shutter* thisv = (BgSpot18Shutter*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808B95AC(BgSpot18Shutter* thisv, GlobalContext* globalCtx) {
}

void func_808B95B8(BgSpot18Shutter* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        Actor_SetFocus(&thisv->dyna.actor, 70.0f);
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        thisv->actionFunc = func_808B9698;
    }
}

void func_808B9618(BgSpot18Shutter* thisv, GlobalContext* globalCtx) {
    if (gSaveContext.infTable[16] & 0x200) {
        Actor_SetFocus(&thisv->dyna.actor, 70.0f);
        if (((thisv->dyna.actor.params >> 8) & 1) == 0) {
            thisv->actionFunc = func_808B9698;
        } else {
            thisv->actionFunc = func_808B971C;
            OnePointCutscene_Init(globalCtx, 4221, 140, &thisv->dyna.actor, MAIN_CAM);
        }
    }
}

void func_808B9698(BgSpot18Shutter* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 180.0f, 1.44f)) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
        thisv->actionFunc = func_808B95AC;
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
}

void func_808B971C(BgSpot18Shutter* thisv, GlobalContext* globalCtx) {
    f32 sin = Math_SinS(thisv->dyna.actor.world.rot.y);
    f32 cos = Math_CosS(thisv->dyna.actor.world.rot.y);
    s32 flag = true;

    flag &= Math_StepToF(&thisv->dyna.actor.world.pos.x, thisv->dyna.actor.home.pos.x + (125.0f * cos), fabsf(cos));
    flag &= Math_StepToF(&thisv->dyna.actor.world.pos.z, thisv->dyna.actor.home.pos.z - (125.0f * sin), fabsf(sin));

    if (flag) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
        thisv->actionFunc = func_808B95AC;
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
}

void BgSpot18Shutter_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Shutter* thisv = (BgSpot18Shutter*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot18Shutter_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGoronCityDoorDL);
}
