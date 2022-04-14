/*
 * File: z_obj_lift.c
 * Overlay: ovl_Obj_Lift
 * Description: Square, collapsing platform
 */

#include "z_obj_lift.h"
#include "objects/object_d_lift/object_d_lift.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS ACTOR_FLAG_4

void ObjLift_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjLift_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjLift_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjLift_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B9651C(ObjLift* thisv);
void func_80B9664C(ObjLift* thisv);
void func_80B967C0(ObjLift* thisv);

void func_80B96560(ObjLift* thisv, GlobalContext* globalCtx);
void func_80B96678(ObjLift* thisv, GlobalContext* globalCtx);
void func_80B96840(ObjLift* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Lift_InitVars = {
    ACTOR_OBJ_LIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_D_LIFT,
    sizeof(ObjLift),
    (ActorFunc)ObjLift_Init,
    (ActorFunc)ObjLift_Destroy,
    (ActorFunc)ObjLift_Update,
    (ActorFunc)ObjLift_Draw,
    NULL,
};

static s16 sFallTimerDurations[] = { 0, 10, 20, 30, 40, 50, 60 };

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 z;
} ObjLiftFramgentScale; // size = 0x4

static ObjLiftFramgentScale sFragmentScales[] = {
    { 120, -120 }, { 120, 0 },     { 120, 120 }, { 0, -120 },   { 0, 0 },
    { 0, 120 },    { -120, -120 }, { -120, 0 },  { -120, 120 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_CONTINUE),   ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE), ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2000, ICHAIN_STOP),
};

static f32 sScales[] = { 0.1f, 0.05f };
static f32 sMaxFallDistances[] = { -18.0f, -9.0f };

void ObjLift_SetupAction(ObjLift* thisv, ObjLiftActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ObjLift_InitDynaPoly(ObjLift* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 flags) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_lift.c", 188,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void func_80B96160(ObjLift* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f* temp_s3;
    s32 pad0;
    s32 i;

    temp_s3 = &thisv->dyna.actor.world.pos;

    for (i = 0; i < ARRAY_COUNT(sFragmentScales); i++) {
        pos.x = sFragmentScales[i].x * thisv->dyna.actor.scale.x + temp_s3->x;
        pos.y = temp_s3->y;
        pos.z = sFragmentScales[i].z * thisv->dyna.actor.scale.z + temp_s3->z;
        velocity.x = sFragmentScales[i].x * thisv->dyna.actor.scale.x * 0.8f;
        velocity.y = Rand_ZeroOne() * 10.0f + 6.0f;
        velocity.z = sFragmentScales[i].z * thisv->dyna.actor.scale.z * 0.8f;
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, temp_s3, -256, (Rand_ZeroOne() < 0.5f) ? 64 : 32, 15, 15, 0,
                             (Rand_ZeroOne() * 50.0f + 50.0f) * thisv->dyna.actor.scale.x, 0, 32, 50, KAKERA_COLOR_NONE,
                             OBJECT_D_LIFT, gCollapsingPlatformDL);
    }

    if (((thisv->dyna.actor.params >> 1) & 1) == 0) {
        func_80033480(globalCtx, &thisv->dyna.actor.world.pos, 120.0f, 12, 120, 100, 1);
    } else if (((thisv->dyna.actor.params >> 1) & 1) == 1) {
        func_80033480(globalCtx, &thisv->dyna.actor.world.pos, 60.0f, 8, 60, 100, 1);
    }
}

void ObjLift_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjLift* thisv = (ObjLift*)thisx;

    ObjLift_InitDynaPoly(thisv, globalCtx, &gCollapsingPlatformCol, DPM_PLAYER);

    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 2) & 0x3F)) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }

    Actor_SetScale(&thisv->dyna.actor, sScales[(thisv->dyna.actor.params >> 1) & 1]);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->unk168.x = Rand_ZeroOne() * 65535.5f;
    thisv->unk168.y = Rand_ZeroOne() * 65535.5f;
    thisv->unk168.z = Rand_ZeroOne() * 65535.5f;
    func_80B9651C(thisv);
    osSyncPrintf("(Dungeon Lift)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void ObjLift_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjLift* thisv = (ObjLift*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80B9651C(ObjLift* thisv) {
    thisv->timer = sFallTimerDurations[(thisv->dyna.actor.params >> 8) & 7];
    ObjLift_SetupAction(thisv, func_80B96560);
}

void func_80B96560(ObjLift* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 quakeIndex;

    if (func_8004356C(&thisv->dyna)) {
        if (thisv->timer <= 0) {
            if (((thisv->dyna.actor.params >> 8) & 7) == 7) {
                func_80B967C0(thisv);
            } else {
                quakeIndex = Quake_Add(GET_ACTIVE_CAM(globalCtx), 1);
                Quake_SetSpeed(quakeIndex, 10000);
                Quake_SetQuakeValues(quakeIndex, 2, 0, 0, 0);
                Quake_SetCountdown(quakeIndex, 20);
                func_80B9664C(thisv);
            }
        }
    } else {
        thisv->timer = sFallTimerDurations[(thisv->dyna.actor.params >> 8) & 7];
    }
}

void func_80B9664C(ObjLift* thisv) {
    thisv->timer = 20;
    ObjLift_SetupAction(thisv, func_80B96678);
}

void func_80B96678(ObjLift* thisv, GlobalContext* globalCtx) {
    if (thisv->timer <= 0) {
        func_80B967C0(thisv);
    } else {
        thisv->unk168.x += 10000;
        thisv->dyna.actor.world.rot.x = (s16)(Math_SinS(thisv->unk168.x) * 300.0f) + thisv->dyna.actor.home.rot.x;
        thisv->dyna.actor.world.rot.z = (s16)(Math_CosS(thisv->unk168.x) * 300.0f) + thisv->dyna.actor.home.rot.z;
        thisv->dyna.actor.shape.rot.x = thisv->dyna.actor.world.rot.x;
        thisv->dyna.actor.shape.rot.z = thisv->dyna.actor.world.rot.z;
        thisv->unk168.y += 18000;
        thisv->dyna.actor.world.pos.y = Math_SinS(thisv->unk168.y) + thisv->dyna.actor.home.pos.y;
        thisv->unk168.z += 18000;
        thisv->dyna.actor.world.pos.x = Math_SinS(thisv->unk168.z) * 3.0f + thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.world.pos.z = Math_CosS(thisv->unk168.z) * 3.0f + thisv->dyna.actor.home.pos.z;
    }

    if ((thisv->timer & 3) == 3) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 16, NA_SE_EV_BLOCK_SHAKE);
    }
}

void func_80B967C0(ObjLift* thisv) {
    ObjLift_SetupAction(thisv, func_80B96840);
    Math_Vec3f_Copy(&thisv->dyna.actor.world.pos, &thisv->dyna.actor.home.pos);
    thisv->dyna.actor.shape.rot = thisv->dyna.actor.world.rot = thisv->dyna.actor.home.rot;
}

void func_80B96840(ObjLift* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 bgId;
    Vec3f sp2C;

    Actor_MoveForward(&thisv->dyna.actor);
    Math_Vec3f_Copy(&sp2C, &thisv->dyna.actor.prevPos);
    sp2C.y += sMaxFallDistances[(thisv->dyna.actor.params >> 1) & 1];
    thisv->dyna.actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->dyna.actor.floorPoly, &bgId, &thisv->dyna.actor, &sp2C);

    if ((thisv->dyna.actor.floorHeight - thisv->dyna.actor.world.pos.y) >=
        (sMaxFallDistances[(thisv->dyna.actor.params >> 1) & 1] - 0.001f)) {
        func_80B96160(thisv, globalCtx);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->dyna.actor.world.pos, 20, NA_SE_EV_BOX_BREAK);
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 2) & 0x3F);
        Actor_Kill(&thisv->dyna.actor);
    }
}

void ObjLift_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjLift* thisv = (ObjLift*)thisx;

    if (thisv->timer > 0) {
        thisv->timer--;
    }

    thisv->actionFunc(thisv, globalCtx);
}

void ObjLift_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gCollapsingPlatformDL);
}
