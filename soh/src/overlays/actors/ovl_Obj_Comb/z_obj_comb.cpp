/*
 * File: z_obj_comb.c
 * Overlay: ovl_Obj_Comb
 * Description: Beehive
 */

#include "z_obj_comb.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void ObjComb_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjComb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjComb_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjComb_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjComb_Break(ObjComb* thisv, GlobalContext* globalCtx);
void ObjComb_ChooseItemDrop(ObjComb* thisv, GlobalContext* globalCtx);
void ObjComb_SetupWait(ObjComb* thisv);
void ObjComb_Wait(ObjComb* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Comb_InitVars = {
    ACTOR_OBJ_COMB,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(ObjComb),
    (ActorFunc)ObjComb_Init,
    (ActorFunc)ObjComb_Destroy,
    (ActorFunc)ObjComb_Update,
    (ActorFunc)ObjComb_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4001FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 900, ICHAIN_STOP),
};

void ObjComb_Break(ObjComb* thisv, GlobalContext* globalCtx) {
    Vec3f pos1;
    Vec3f pos;
    Vec3f velocity;
    const Gfx* dlist = gFieldBeehiveFragmentDL;
    s16 scale;
    s16 angle = 0;
    s16 gravity;
    u8 arg5;
    u8 arg6;
    f32 rand1;
    f32 rand2;
    s32 i;

    for (i = 0; i < 31; i++) {
        angle += 0x4E20;
        rand1 = Rand_ZeroOne() * 10.0f;

        pos1.x = Math_SinS(angle) * rand1;
        pos1.y = (i - 15) * 0.7f;
        pos1.z = Math_CosS(angle) * rand1;

        Math_Vec3f_Sum(&pos1, &thisv->actor.world.pos, &pos);

        velocity.x = (Rand_ZeroOne() - 0.5f) + pos1.x * 0.5f;
        velocity.y = (Rand_ZeroOne() - 0.5f) + pos1.y * 0.6f;
        velocity.z = (Rand_ZeroOne() - 0.5f) + pos1.z * 0.5f;

        scale = Rand_ZeroOne() * 72.0f + 25.0f;

        if (scale < 40) {
            gravity = -200;
            arg6 = 40;
        } else if (scale < 70) {
            gravity = -280;
            arg6 = 30;
        } else {
            gravity = -340;
            arg6 = 20;
        }

        rand2 = Rand_ZeroOne();

        if (rand2 < 0.1f) {
            arg5 = 96;
        } else if (rand2 < 0.8f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, gravity, arg5, arg6, 4, 0, scale, 0, 0, 80,
                             KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_FIELD_KEEP, dlist);
    }

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y - 10.0f;
    pos.z = thisv->actor.world.pos.z;
    func_80033480(globalCtx, &pos, 40.0f, 6, 70, 60, 1);
}

void ObjComb_ChooseItemDrop(ObjComb* thisv, GlobalContext* globalCtx) {
    s16 params = thisv->actor.params & 0x1F;

    if ((params > 0) || (params < 0x1A)) {
        if (params == 6) {
            if (Flags_GetCollectible(globalCtx, (thisv->actor.params >> 8) & 0x3F)) {
                params = -1;
            } else {
                params = (params | (((thisv->actor.params >> 8) & 0x3F) << 8));
            }
        } else if (Rand_ZeroOne() < 0.5f) {
            params = -1;
        }
        if (params >= 0) {
            Item_DropCollectible(globalCtx, &thisv->actor.world.pos, params);
        }
    }
}

void ObjComb_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjComb* thisv = (ObjComb*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->colliderItems);
    ObjComb_SetupWait(thisv);
}

void ObjComb_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjComb* thisv = (ObjComb*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void ObjComb_SetupWait(ObjComb* thisv) {
    thisv->actionFunc = ObjComb_Wait;
}

void ObjComb_Wait(ObjComb* thisv, GlobalContext* globalCtx) {
    s32 dmgFlags;

    thisv->unk_1B0 -= 50;
    if (thisv->unk_1B0 < 0) {
        thisv->unk_1B0 = 0;
    }

    if ((thisv->collider.base.acFlags & AC_HIT) != 0) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        dmgFlags = thisv->collider.elements[0].info.acHitInfo->toucher.dmgFlags;
        if (dmgFlags & 0x4001F866) {
            thisv->unk_1B0 = 1500;
        } else {
            ObjComb_Break(thisv, globalCtx);
            ObjComb_ChooseItemDrop(thisv, globalCtx);
            Actor_Kill(&thisv->actor);
        }
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->actor.update != NULL) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void ObjComb_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjComb* thisv = (ObjComb*)thisx;

    thisv->unk_1B2 += 0x2EE0;
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.shape.rot.x = Math_SinS(thisv->unk_1B2) * thisv->unk_1B0 + thisv->actor.home.rot.x;
}

void ObjComb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjComb* thisv = (ObjComb*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_comb.c", 369);

    func_80093D18(globalCtx->state.gfxCtx);

    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y + (118.0f * thisv->actor.scale.y),
                     thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(thisv->actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateX(thisv->actor.shape.rot.x * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(thisv->actor.shape.rot.z * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_Translate(0, -(thisv->actor.scale.y * 118.0f), 0, MTXMODE_APPLY);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_comb.c", 394),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gFieldBeehiveDL);

    Collider_UpdateSpheres(0, &thisv->collider);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_comb.c", 402);
}
