/*
 * File: z_bg_ydan_maruta.c
 * Overlay: ovl_Bg_Ydan_Maruta
 * Description: Rotating spike log and falling ladder in Deku Tree
 */

#include "z_bg_ydan_maruta.h"
#include "objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS 0

void BgYdanMaruta_Init(Actor* thisx, GlobalContext* globalCtx);
void BgYdanMaruta_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgYdanMaruta_Update(Actor* thisx, GlobalContext* globalCtx);
void BgYdanMaruta_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808BEFF4(BgYdanMaruta* thisv, GlobalContext* globalCtx);
void BgYdanMaruta_DoNothing(BgYdanMaruta* thisv, GlobalContext* globalCtx);
void func_808BF078(BgYdanMaruta* thisv, GlobalContext* globalCtx);
void func_808BF108(BgYdanMaruta* thisv, GlobalContext* globalCtx);
void func_808BF1EC(BgYdanMaruta* thisv, GlobalContext* globalCtx);

ActorInit Bg_Ydan_Maruta_InitVars = {
    ACTOR_BG_YDAN_MARUTA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_YDAN_OBJECTS,
    sizeof(BgYdanMaruta),
    (ActorFunc)BgYdanMaruta_Init,
    (ActorFunc)BgYdanMaruta_Destroy,
    (ActorFunc)BgYdanMaruta_Update,
    (ActorFunc)BgYdanMaruta_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_WOOD,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 220.0f, -10.0f, 0.0f }, { 220.0f, 10.0f, 0.0f }, { -220.0f, 10.0f, 0.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_WOOD,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 16.0f, 0.0f, 0.0f }, { 16.0f, 135.0f, 0.0f }, { -16.0f, 135.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgYdanMaruta_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgYdanMaruta* thisv = (BgYdanMaruta*)thisx;
    Vec3f sp4C[3];
    s32 i;
    f32 sinRotY;
    f32 cosRotY;
    const CollisionHeader* colHeader = NULL;
    ColliderTrisElementInit* triInit;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    Collider_InitTris(globalCtx, &thisv->collider);
    Collider_SetTris(globalCtx, &thisv->collider, &thisv->dyna.actor, &sTrisInit, thisv->elements);

    thisv->switchFlag = thisv->dyna.actor.params & 0xFFFF;
    thisx->params = (thisx->params >> 8) & 0xFF; // thisx is required to match here

    if (thisv->dyna.actor.params == 0) {
        triInit = &sTrisElementsInit[0];
        thisv->actionFunc = func_808BEFF4;
    } else {
        triInit = &sTrisElementsInit[1];
        DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
        CollisionHeader_GetVirtual(&gDTFallingLadderCol, &colHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
        thisx->home.pos.y += -280.0f;
        if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
            thisx->world.pos.y = thisx->home.pos.y;
            thisv->actionFunc = BgYdanMaruta_DoNothing;
        } else {
            thisv->actionFunc = func_808BF078;
        }
    }

    sinRotY = Math_SinS(thisv->dyna.actor.shape.rot.y);
    cosRotY = Math_CosS(thisv->dyna.actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        sp4C[i].x = (triInit->dim.vtx[i].x * cosRotY) + thisv->dyna.actor.world.pos.x;
        sp4C[i].y = triInit->dim.vtx[i].y + thisv->dyna.actor.world.pos.y;
        sp4C[i].z = thisv->dyna.actor.world.pos.z - (triInit->dim.vtx[i].x * sinRotY);
    }

    Collider_SetTrisVertices(&thisv->collider, 0, &sp4C[0], &sp4C[1], &sp4C[2]);

    sp4C[1].x = (triInit->dim.vtx[2].x * cosRotY) + thisv->dyna.actor.world.pos.x;
    sp4C[1].y = triInit->dim.vtx[0].y + thisv->dyna.actor.world.pos.y;
    sp4C[1].z = thisv->dyna.actor.world.pos.z - (triInit->dim.vtx[2].x * sinRotY);

    Collider_SetTrisVertices(&thisv->collider, 1, &sp4C[0], &sp4C[2], &sp4C[1]);
}

void BgYdanMaruta_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgYdanMaruta* thisv = (BgYdanMaruta*)thisx;

    Collider_DestroyTris(globalCtx, &thisv->collider);
    if (thisv->dyna.actor.params == 1) {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void func_808BEFF4(BgYdanMaruta* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.atFlags & AT_HIT) {
        func_8002F71C(globalCtx, &thisv->dyna.actor, 7.0f, thisv->dyna.actor.shape.rot.y, 6.0f);
    }
    thisv->dyna.actor.shape.rot.x += 0x360;
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_TOGE_STICK_ROLLING - SFX_FLAG);
}

void func_808BF078(BgYdanMaruta* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->unk_16A = 20;
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        thisv->actionFunc = func_808BF108;
        OnePointCutscene_Init(globalCtx, 3010, 50, &thisv->dyna.actor, MAIN_CAM);
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void func_808BF108(BgYdanMaruta* thisv, GlobalContext* globalCtx) {
    s16 temp;

    if (thisv->unk_16A != 0) {
        thisv->unk_16A--;
    }
    if (thisv->unk_16A == 0) {
        thisv->actionFunc = func_808BF1EC;
    }

    if (1) {}

    temp = (thisv->unk_16A % 4) - 2;
    if (temp == -2) {
        temp = 0;
    } else {
        temp *= 2;
    }

    thisv->dyna.actor.world.pos.x = (Math_CosS(thisv->dyna.actor.shape.rot.y) * temp) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z = (Math_SinS(thisv->dyna.actor.shape.rot.y) * temp) + thisv->dyna.actor.home.pos.z;

    func_8002F974(&thisv->dyna.actor, NA_SE_EV_TRAP_OBJ_SLIDE - SFX_FLAG);
}

void func_808BF1EC(BgYdanMaruta* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.velocity.y += 1.0f;
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.velocity.y)) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_LADDER_DOUND);
        thisv->actionFunc = BgYdanMaruta_DoNothing;
    }
}

void BgYdanMaruta_DoNothing(BgYdanMaruta* thisv, GlobalContext* globalCtx) {
}

void BgYdanMaruta_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgYdanMaruta* thisv = (BgYdanMaruta*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgYdanMaruta_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgYdanMaruta* thisv = (BgYdanMaruta*)thisx;

    if (thisv->dyna.actor.params == 0) {
        Gfx_DrawDListOpa(globalCtx, gDTRollingSpikeTrapDL);
    } else {
        Gfx_DrawDListOpa(globalCtx, gDTFallingLadderDL);
    }
}
