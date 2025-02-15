/*
 * File: z_bg_hidan_dalm.c
 * Overlay: ovl_Bg_Hidan_Dalm
 * Description: Hammerable Totem Pieces (Fire Temple)
 */

#include "z_bg_hidan_dalm.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanDalm_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanDalm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanDalm_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanDalm_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHidanDalm_Wait(BgHidanDalm* thisv, GlobalContext* globalCtx);
void BgHidanDalm_Shrink(BgHidanDalm* thisv, GlobalContext* globalCtx);

ActorInit Bg_Hidan_Dalm_InitVars = {
    ACTOR_BG_HIDAN_DALM,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanDalm),
    (ActorFunc)BgHidanDalm_Init,
    (ActorFunc)BgHidanDalm_Destroy,
    (ActorFunc)BgHidanDalm_Update,
    (ActorFunc)BgHidanDalm_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementInit[4] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 305.0f, 0.0f, -300.0f }, { 305.0f, 600.0f, -300.0f }, { 305.0f, 600.0f, 300.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 305.0f, 0.0f, -300.0f }, { 305.0f, 600.0f, 300.0f }, { 305.0f, 0.0f, 300.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -305.0f, 0.0f, -300.0f }, { -305.0f, 600.0f, 300.0f }, { -305.0f, 600.0f, -300.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -305.0f, 0.0f, -300.0f }, { -305.0f, 0.0f, 300.0f }, { -305.0f, 600.0f, 300.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    4,
    sTrisElementInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -200, ICHAIN_STOP),
};

void BgHidanDalm_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanDalm* thisv = (BgHidanDalm*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gFireTempleHammerableTotemCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    Collider_InitTris(globalCtx, &thisv->collider);
    Collider_SetTris(globalCtx, &thisv->collider, thisx, &sTrisInit, thisv->colliderItems);

    thisv->switchFlag = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        Actor_Kill(thisx);
    } else {
        thisv->actionFunc = BgHidanDalm_Wait;
    }
}

void BgHidanDalm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanDalm* thisv = (BgHidanDalm*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyTris(globalCtx, &thisv->collider);
}

void BgHidanDalm_Wait(BgHidanDalm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->collider.base.acFlags & AC_HIT) && !Player_InCsMode(globalCtx) &&
        (player->swordAnimation == 22 || player->swordAnimation == 23)) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if ((thisv->collider.elements[0].info.bumperFlags & BUMP_HIT) ||
            (thisv->collider.elements[1].info.bumperFlags & BUMP_HIT)) {
            thisv->dyna.actor.world.rot.y -= 0x4000;
        } else {
            thisv->dyna.actor.world.rot.y += 0x4000;
        }
        thisv->dyna.actor.world.pos.x += 32.5f * Math_SinS(thisv->dyna.actor.world.rot.y);
        thisv->dyna.actor.world.pos.z += 32.5f * Math_CosS(thisv->dyna.actor.world.rot.y);

        func_8002DF54(globalCtx, &thisv->dyna.actor, 8);
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
        thisv->actionFunc = BgHidanDalm_Shrink;
        thisv->dyna.actor.bgCheckFlags &= ~2;
        thisv->dyna.actor.bgCheckFlags &= ~8;
        thisv->dyna.actor.speedXZ = 10.0f;
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        func_8002F7DC(&GET_PLAYER(globalCtx)->actor, NA_SE_IT_HAMMER_HIT);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_DARUMA_VANISH);
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgHidanDalm_Shrink(BgHidanDalm* thisv, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f velocity;
    Vec3f pos;

    if (Math_StepToF(&thisv->dyna.actor.scale.x, 0.0f, 0.004f)) {
        func_8002DF54(globalCtx, &thisv->dyna.actor, 7);
        Actor_Kill(&thisv->dyna.actor);
    }

    thisv->dyna.actor.scale.y = thisv->dyna.actor.scale.z = thisv->dyna.actor.scale.x;

    pos.x = thisv->dyna.actor.world.pos.x;
    pos.y = thisv->dyna.actor.world.pos.y + thisv->dyna.actor.scale.x * 160.0f;
    pos.z = thisv->dyna.actor.world.pos.z;

    for (i = 0; i < 4; i++) {
        velocity.x = 5.0f * Math_SinS(thisv->dyna.actor.world.rot.y + 0x8000) + (Rand_ZeroOne() - 0.5f) * 5.0f;
        velocity.z = 5.0f * Math_CosS(thisv->dyna.actor.world.rot.y + 0x8000) + (Rand_ZeroOne() - 0.5f) * 5.0f;
        velocity.y = (Rand_ZeroOne() - 0.5f) * 1.5f;
        EffectSsKiraKira_SpawnSmallYellow(globalCtx, &pos, &velocity, &accel);
    }
}

void BgHidanDalm_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanDalm* thisv = (BgHidanDalm*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->dyna.actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 10.0f, 15.0f, 32.0f, 5);
}

/**
 * Update vertices of collider tris based on the current matrix
 */
void BgHidanDalm_UpdateCollider(BgHidanDalm* thisv) {
    Vec3f pos2;
    Vec3f pos1;
    Vec3f pos0;

    Matrix_MultVec3f(&sTrisElementInit[0].dim.vtx[0], &pos0);
    Matrix_MultVec3f(&sTrisElementInit[0].dim.vtx[1], &pos1);
    Matrix_MultVec3f(&sTrisElementInit[0].dim.vtx[2], &pos2);
    Collider_SetTrisVertices(&thisv->collider, 0, &pos0, &pos1, &pos2);
    Matrix_MultVec3f(&sTrisElementInit[1].dim.vtx[2], &pos1);
    Collider_SetTrisVertices(&thisv->collider, 1, &pos0, &pos2, &pos1);

    Matrix_MultVec3f(&sTrisElementInit[2].dim.vtx[0], &pos0);
    Matrix_MultVec3f(&sTrisElementInit[2].dim.vtx[1], &pos1);
    Matrix_MultVec3f(&sTrisElementInit[2].dim.vtx[2], &pos2);
    Collider_SetTrisVertices(&thisv->collider, 2, &pos0, &pos1, &pos2);
    Matrix_MultVec3f(&sTrisElementInit[3].dim.vtx[1], &pos2);
    Collider_SetTrisVertices(&thisv->collider, 3, &pos0, &pos2, &pos1);
}

void BgHidanDalm_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanDalm* thisv = (BgHidanDalm*)thisx;

    if (thisv->dyna.actor.params == 0) {
        Gfx_DrawDListOpa(globalCtx, gFireTempleHammerableTotemBodyDL);
    } else {
        Gfx_DrawDListOpa(globalCtx, gFireTempleHammerableTotemHeadDL);
    }

    if (thisv->actionFunc == BgHidanDalm_Wait) {
        BgHidanDalm_UpdateCollider(thisv);
    }
}
