
/*
 * File: z_bg_spot18_obj.c
 * Overlay: ovl_Bg_Spot18_Obj
 * Description:
 */

#include "z_bg_spot18_obj.h"
#include "objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS 0

void BgSpot18Obj_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Obj_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Obj_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot18Obj_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 func_808B8910(BgSpot18Obj* thisv, GlobalContext* globalCtx);
s32 func_808B8A5C(BgSpot18Obj* thisv, GlobalContext* globalCtx);
s32 func_808B8A98(BgSpot18Obj* thisv, GlobalContext* globalCtx);
s32 func_808B8B08(BgSpot18Obj* thisv, GlobalContext* globalCtx);
s32 func_808B8BB4(BgSpot18Obj* thisv, GlobalContext* globalCtx);
s32 func_808B8C90(BgSpot18Obj* thisv, GlobalContext* globalCtx);
void func_808B8DC0(BgSpot18Obj* thisv);
void func_808B8DD0(BgSpot18Obj* thisv, GlobalContext* globalCtx);
void func_808B8E64(BgSpot18Obj* thisv);
void func_808B8E7C(BgSpot18Obj* thisv, GlobalContext* globalCtx);
void func_808B8EE0(BgSpot18Obj* thisv);
void func_808B8F08(BgSpot18Obj* thisv, GlobalContext* globalCtx);
void func_808B9030(BgSpot18Obj* thisv);
void func_808B9040(BgSpot18Obj* thisv, GlobalContext* globalCtx);

ActorInit Bg_Spot18_Obj_InitVars = {
    ACTOR_BG_SPOT18_OBJ,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT18_OBJ,
    sizeof(BgSpot18Obj),
    (ActorFunc)BgSpot18Obj_Init,
    (ActorFunc)BgSpot18Obj_Destroy,
    (ActorFunc)BgSpot18Obj_Update,
    (ActorFunc)BgSpot18Obj_Draw,
    NULL,
};

static u8 D_808B90F0[2][2] = { { 0x01, 0x01 }, { 0x01, 0x00 } };

static f32 D_808B90F4[] = {
    0.1f,
    0.1f,
};

static const CollisionHeader* D_808B90FC[] = {
    &gGoronCityStatueCol,
    &gGoronCityStatueSpearCol,
};

static u32 D_808B9104[] = {
    0,
    0,
};

static BgSpot18ObjInitFunc D_808B910C[] = {
    func_808B8A98,
    func_808B8910,
    func_808B8A5C,
    func_808B8B08,
};

static InitChainEntry sInitChain1[] = {
    ICHAIN_F32(minVelocityY, -10, ICHAIN_CONTINUE),       ICHAIN_F32(gravity, -4, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1400, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

static InitChainEntry sInitChain2[] = {
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 700, ICHAIN_STOP),
};

static BgSpot18ObjInitFunc D_808B913C[] = {
    func_808B8BB4,
    func_808B8C90,
};

static const Gfx(*sDlists[]) = {
    gGoronCityStatueDL,
    gGoronCityStatueSpearDL,
};

s32 func_808B8910(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    s32 age;

    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        age = 1;
    } else if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        age = 0;
    } else {
        osSyncPrintf("Error : リンク年齢不詳 (%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 182,
                     thisv->dyna.actor.params);
        return 0;
    }

    switch (D_808B90F0[thisv->dyna.actor.params & 0xF][age]) {
        case 0:
        case 1:
            if (D_808B90F0[thisv->dyna.actor.params & 0xF][age] == 0) {
                osSyncPrintf("出現しない Object (0x%04x)\n", thisv->dyna.actor.params);
            }
            return D_808B90F0[thisv->dyna.actor.params & 0xF][age];
        case 2:
            osSyncPrintf("Error : Obj出現判定が設定されていない(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 202,
                         thisv->dyna.actor.params);
            break;
        default:
            osSyncPrintf("Error : Obj出現判定失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 210,
                         thisv->dyna.actor.params);
    }
    return 0;
}

s32 func_808B8A5C(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    Actor_SetScale(&thisv->dyna.actor, D_808B90F4[thisv->dyna.actor.params & 0xF]);
    return 1;
}

s32 func_808B8A98(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    const CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(D_808B90FC[thisv->dyna.actor.params & 0xF], &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    return 1;
}

s32 func_808B8B08(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.flags |= D_808B9104[thisv->dyna.actor.params & 0xF];
    return 1;
}

s32 func_808B8B38(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_808B910C); i++) {
        if (D_808B910C[i](thisv, globalCtx) == 0) {
            return 0;
        }
    }
    return 1;
}

s32 func_808B8BB4(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain1);

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        func_808B9030(thisv);
    } else if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
        func_808B9030(thisv);
        thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->dyna.actor.world.rot.y) * 80.0f) + thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->dyna.actor.world.rot.y) * 80.0f) + thisv->dyna.actor.home.pos.z;
    } else {
        func_808B8E64(thisv);
    }
    return 1;
}

s32 func_808B8C90(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain2);
    func_808B8DC0(thisv);
    return 1;
}

s32 func_808B8CC8(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    if ((D_808B913C[thisv->dyna.actor.params & 0xF] != NULL) &&
        (!D_808B913C[thisv->dyna.actor.params & 0xF](thisv, globalCtx))) {
        return 0;
    }
    return 1;
}

void BgSpot18Obj_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Obj* thisv = (BgSpot18Obj*)thisx;

    osSyncPrintf("Spot18 Object [arg_data : 0x%04x]\n", thisv->dyna.actor.params);
    if (!func_808B8B38(thisv, globalCtx)) {
        Actor_Kill(&thisv->dyna.actor);
    } else if (!func_808B8CC8(thisv, globalCtx)) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgSpot18Obj_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Obj* thisv = (BgSpot18Obj*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_808B8DC0(BgSpot18Obj* thisv) {
    thisv->actionFunc = func_808B8DD0;
}

void func_808B8DD0(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
}

void func_808B8DDC(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 20.0f, 46.0f, 0.0f, 28);
}

void func_808B8E20(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (fabsf(thisv->dyna.unk_150) > 0.001f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
}

void func_808B8E64(BgSpot18Obj* thisv) {
    thisv->unk_168 = 20;
    thisv->actionFunc = func_808B8E7C;
}

void func_808B8E7C(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    if (thisv->dyna.unk_150 < -0.001f) {
        if (thisv->unk_168 <= 0) {
            func_808B8EE0(thisv);
        }
    } else {
        thisv->unk_168 = 20;
    }
    func_808B8E20(thisv, globalCtx);
}

void func_808B8EE0(BgSpot18Obj* thisv) {
    thisv->actionFunc = func_808B8F08;
    thisv->dyna.actor.world.rot.y = 0;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.velocity.z = 0.0f;
    thisv->dyna.actor.velocity.y = 0.0f;
    thisv->dyna.actor.velocity.x = 0.0f;
}

void func_808B8F08(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    Math_StepToF(&thisv->dyna.actor.speedXZ, 1.2f, 0.1f);
    Actor_MoveForward(&thisv->dyna.actor);
    func_808B8DDC(thisv, globalCtx);

    if (Math3D_Dist2DSq(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.z, thisv->dyna.actor.home.pos.x,
                        thisv->dyna.actor.home.pos.z) >= 6400.0f) {
        func_808B9030(thisv);
        thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->dyna.actor.world.rot.y) * 80.0f) + thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->dyna.actor.world.rot.y) * 80.0f) + thisv->dyna.actor.home.pos.z;
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void func_808B9030(BgSpot18Obj* thisv) {
    thisv->actionFunc = func_808B9040;
}

void func_808B9040(BgSpot18Obj* thisv, GlobalContext* globalCtx) {
    func_808B8E20(thisv, globalCtx);
}

void BgSpot18Obj_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot18Obj* thisv = (BgSpot18Obj*)thisx;

    if (thisv->unk_168 > 0) {
        thisv->unk_168 -= 1;
    }
    thisv->actionFunc(thisv, globalCtx);
}

void BgSpot18Obj_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, sDlists[thisx->params & 0xF]);
}
