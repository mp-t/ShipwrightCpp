/*
 * File: z_obj_bombiwa.c
 * Overlay: ovl_Obj_Bombiwa
 * Description: Round, brown, breakable boulder
 */

#include "z_obj_bombiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_bombiwa/object_bombiwa.h"

#define FLAGS 0

void ObjBombiwa_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjBombiwa_InitCollision(Actor* thisx, GlobalContext* globalCtx);
void ObjBombiwa_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjBombiwa_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjBombiwa_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjBombiwa_Break(ObjBombiwa* thisv, GlobalContext* globalCtx);

ActorInit Obj_Bombiwa_InitVars = {
    ACTOR_OBJ_BOMBIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BOMBIWA,
    sizeof(ObjBombiwa),
    (ActorFunc)ObjBombiwa_Init,
    (ActorFunc)ObjBombiwa_Destroy,
    (ActorFunc)ObjBombiwa_Update,
    (ActorFunc)ObjBombiwa_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC1FFFE, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 55, 70, 0, { 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 350, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static s16 sEffectScales[] = {
    17, 14, 10, 8, 7, 5, 3, 2,
};

void ObjBombiwa_InitCollision(Actor* thisx, GlobalContext* globalCtx) {
    ObjBombiwa* thisv = (ObjBombiwa*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
}

void ObjBombiwa_Init(Actor* thisx, GlobalContext* globalCtx) {
    Actor_ProcessInitChain(thisx, sInitChain);
    ObjBombiwa_InitCollision(thisx, globalCtx);
    if ((Flags_GetSwitch(globalCtx, thisx->params & 0x3F) != 0)) {
        Actor_Kill(thisx);
    } else {
        CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);
        if (thisx->shape.rot.y == 0) {
            s16 rand = (s16)Rand_ZeroFloat(65536.0f);

            thisx->world.rot.y = rand;
            thisx->shape.rot.y = rand;
        }
        thisx->shape.yOffset = -200.0f;
        thisx->world.pos.y = thisx->home.pos.y + 20.0f;
    }
}

void ObjBombiwa_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjBombiwa* thisv = (ObjBombiwa*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void ObjBombiwa_Break(ObjBombiwa* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f velocity;
    const Gfx* dlist;
    s16 arg5;
    s16 scale;
    s32 i;

    dlist = object_bombiwa_DL_0009E0;
    for (i = 0; i < ARRAY_COUNT(sEffectScales); i++) {
        pos.x = ((Rand_ZeroOne() - 0.5f) * 10.0f) + thisv->actor.home.pos.x;
        pos.y = ((Rand_ZeroOne() * 5.0f) + thisv->actor.home.pos.y) + 8.0f;
        pos.z = ((Rand_ZeroOne() - 0.5f) * 10.0f) + thisv->actor.home.pos.z;
        velocity.x = (Rand_ZeroOne() - 0.5f) * 15.0f;
        velocity.y = (Rand_ZeroOne() * 16.0f) + 5.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 15.0f;
        scale = sEffectScales[i];
        arg5 = (scale >= 11) ? 37 : 33;
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, -400, arg5, 10, 2, 0, scale, 1, 0, 80, KAKERA_COLOR_NONE,
                             OBJECT_BOMBIWA, dlist);
    }
    func_80033480(globalCtx, &thisv->actor.world.pos, 60.0f, 8, 100, 160, 1);
}

void ObjBombiwa_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjBombiwa* thisv = (ObjBombiwa*)thisx;
    s32 pad;

    if ((func_80033684(globalCtx, &thisv->actor) != NULL) ||
        ((thisv->collider.base.acFlags & AC_HIT) && (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x40000040))) {
        ObjBombiwa_Break(thisv, globalCtx);
        Flags_SetSwitch(globalCtx, thisv->actor.params & 0x3F);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
        if (((thisv->actor.params >> 0xF) & 1) != 0) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        Actor_Kill(&thisv->actor);
    } else {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisv->actor.xzDistToPlayer < 800.0f) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

void ObjBombiwa_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, object_bombiwa_DL_0009E0);
}
