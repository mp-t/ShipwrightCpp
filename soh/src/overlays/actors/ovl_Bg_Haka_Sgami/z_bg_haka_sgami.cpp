/*
 * File: z_bg_haka_sgami.c
 * Overlay: ovl_Bg_Haka_Sgami
 * Description: Spinning Scythe Trap
 */

#include "z_bg_haka_sgami.h"
#include "objects/object_haka_objects/object_haka_objects.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

typedef enum {
    /* 0 */ SCYTHE_TRAP_SHADOW_TEMPLE,
    /* 1 */ SCYTHE_TRAP_SHADOW_TEMPLE_INVISIBLE,
    /* 2 */ SCYTHE_TRAP_ICE_CAVERN
} SpinningScytheTrapMode;

#define SCYTHE_SPIN_TIME 32

void BgHakaSgami_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHakaSgami_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHakaSgami_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHakaSgami_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHakaSgami_SetupSpin(BgHakaSgami* thisv, GlobalContext* globalCtx);
void BgHakaSgami_Spin(BgHakaSgami* thisv, GlobalContext* globalCtx);

ActorInit Bg_Haka_Sgami_InitVars = {
    ACTOR_BG_HAKA_SGAMI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHakaSgami),
    (ActorFunc)BgHakaSgami_Init,
    (ActorFunc)BgHakaSgami_Destroy,
    (ActorFunc)BgHakaSgami_Update,
    NULL,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[4] = {
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { 365.0f, 45.0f, 27.0f }, { 130.0f, 45.0f, 150.0f }, { 290.0f, 45.0f, 145.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { 250.0f, 45.0f, 90.0f }, { 50.0f, 45.0f, 80.0f }, { 160.0f, 45.0f, 160.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { -305.0f, 33.0f, -7.0f }, { -220.0f, 33.0f, 40.0f }, { -130.0f, 33.0f, -5.0f } } },
    },
    {
        {
            ELEMTYPE_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { { { -190.0f, 33.0f, 40.0f }, { -30.0f, 33.0f, 15.0f }, { -70.0f, 33.0f, -30.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    4,
    sTrisElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 80, 130, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 80, 130, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 4, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaSgami_Init(Actor* thisx, GlobalContext* globalCtx) {
    static u8 sP1StartColor[] = { 250, 250, 250, 200 };
    static u8 sP2StartColor[] = { 200, 200, 200, 130 };
    static u8 sP1EndColor[] = { 200, 200, 200, 60 };
    static u8 sP2EndColor[] = { 150, 150, 150, 20 };
    BgHakaSgami* thisv = (BgHakaSgami*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;
    ColliderTris* colliderScythe = &thisv->colliderScythe;

    Actor_ProcessInitChain(thisx, sInitChain);

    thisv->unk_151 = thisx->params & 0xFF;
    thisx->params = (thisx->params >> 8) & 0xFF;

    if (thisv->unk_151 != 0) {
        thisx->flags |= ACTOR_FLAG_7;
    }

    Collider_InitTris(globalCtx, colliderScythe);
    Collider_SetTris(globalCtx, colliderScythe, thisx, &sTrisInit, thisv->colliderScytheItems);
    Collider_InitCylinder(globalCtx, &thisv->colliderScytheCenter);
    Collider_SetCylinder(globalCtx, &thisv->colliderScytheCenter, thisx, &sCylinderInit);

    thisv->colliderScytheCenter.dim.pos.x = thisx->world.pos.x;
    thisv->colliderScytheCenter.dim.pos.y = thisx->world.pos.y;
    thisv->colliderScytheCenter.dim.pos.z = thisx->world.pos.z;

    CollisionCheck_SetInfo(&thisx->colChkInfo, NULL, &sColChkInfoInit);

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = sP1StartColor[i];
        blureInit.p2StartColor[i] = sP2StartColor[i];
        blureInit.p1EndColor[i] = sP1EndColor[i];
        blureInit.p2EndColor[i] = sP2EndColor[i];
    }
    blureInit.elemDuration = 10;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;
    Effect_Add(globalCtx, &thisv->blureEffectIndex[0], EFFECT_BLURE1, 0, 0, &blureInit);
    Effect_Add(globalCtx, &thisv->blureEffectIndex[1], EFFECT_BLURE1, 0, 0, &blureInit);

    if (thisx->params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        thisv->requiredObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_HAKA_OBJECTS);
        thisx->flags &= ~ACTOR_FLAG_0;
    } else {
        thisv->requiredObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_ICE_OBJECTS);
        thisv->colliderScytheCenter.dim.radius = 30;
        thisv->colliderScytheCenter.dim.height = 70;
        Actor_SetFocus(thisx, 40.0f);
    }

    if (thisv->requiredObjBankIndex < 0) {
        Actor_Kill(thisx);
        return;
    }

    thisv->actionFunc = BgHakaSgami_SetupSpin;
}

void BgHakaSgami_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaSgami* thisv = (BgHakaSgami*)thisx;

    Effect_Delete(globalCtx, thisv->blureEffectIndex[0]);
    Effect_Delete(globalCtx, thisv->blureEffectIndex[1]);
    Collider_DestroyTris(globalCtx, &thisv->colliderScythe);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderScytheCenter);
}

void BgHakaSgami_SetupSpin(BgHakaSgami* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->requiredObjBankIndex)) {
        thisv->actor.objBankIndex = thisv->requiredObjBankIndex;
        thisv->actor.draw = BgHakaSgami_Draw;
        thisv->timer = SCYTHE_SPIN_TIME;
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        thisv->actionFunc = BgHakaSgami_Spin;
    }
}

void BgHakaSgami_Spin(BgHakaSgami* thisv, GlobalContext* globalCtx) {
    static Vec3f blureEffectVertices2[] = {
        { -20.0f, 50.0f, 130.0f },
        { -50.0f, 33.0f, 20.0f },
    };
    static Vec3f blureEffectVertices1[] = {
        { 380.0f, 50.0f, 50.0f },
        { 310.0f, 33.0f, 0.0f },
    };
    s32 i;
    s32 j;
    Vec3f scytheVertices[3];
    f32 actorRotYSin;
    f32 actorRotYCos;
    s32 iterateCount;
    ColliderTrisElementInit* elementInit;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actor.shape.rot.y += ((s16)(512.0f * sinf(thisv->timer * (std::numbers::pi_v<float> / 16.0f))) + 0x400) >> 1;

    if (thisv->timer == 0) {
        thisv->timer = SCYTHE_SPIN_TIME;
    }

    actorRotYSin = Math_SinS(thisv->actor.shape.rot.y);
    actorRotYCos = Math_CosS(thisv->actor.shape.rot.y);

    iterateCount = (thisv->actor.params != 0) ? 4 : 2;

    for (i = iterateCount - 2; i < iterateCount; i++) {
        elementInit = &sTrisInit.elements[i];

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = thisv->actor.world.pos.x + elementInit->dim.vtx[j].z * actorRotYSin +
                                  elementInit->dim.vtx[j].x * actorRotYCos;
            scytheVertices[j].y = thisv->actor.world.pos.y + elementInit->dim.vtx[j].y;
            scytheVertices[j].z = thisv->actor.world.pos.z + elementInit->dim.vtx[j].z * actorRotYCos -
                                  elementInit->dim.vtx[j].x * actorRotYSin;
        }

        Collider_SetTrisVertices(&thisv->colliderScythe, i, &scytheVertices[0], &scytheVertices[1], &scytheVertices[2]);

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = (2 * thisv->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * thisv->actor.world.pos.z) - scytheVertices[j].z;
        }

        Collider_SetTrisVertices(&thisv->colliderScythe, (i + 2) % 4, &scytheVertices[0], &scytheVertices[1],
                                 &scytheVertices[2]);
    }

    if ((thisv->unk_151 == 0) || (globalCtx->actorCtx.unk_03 != 0)) {
        scytheVertices[0].x = thisv->actor.world.pos.x + blureEffectVertices1[thisv->actor.params].z * actorRotYSin +
                              blureEffectVertices1[thisv->actor.params].x * actorRotYCos;
        scytheVertices[0].y = thisv->actor.world.pos.y + blureEffectVertices1[thisv->actor.params].y;
        scytheVertices[0].z = thisv->actor.world.pos.z + blureEffectVertices1[thisv->actor.params].z * actorRotYCos -
                              blureEffectVertices1[thisv->actor.params].x * actorRotYSin;
        scytheVertices[1].x = thisv->actor.world.pos.x + blureEffectVertices2[thisv->actor.params].z * actorRotYSin +
                              blureEffectVertices2[thisv->actor.params].x * actorRotYCos;
        scytheVertices[1].y = thisv->actor.world.pos.y + blureEffectVertices2[thisv->actor.params].y;
        scytheVertices[1].z = thisv->actor.world.pos.z + blureEffectVertices2[thisv->actor.params].z * actorRotYCos -
                              blureEffectVertices2[thisv->actor.params].x * actorRotYSin;
        EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureEffectIndex[0])), &scytheVertices[0], &scytheVertices[1]);

        for (j = 0; j < 2; j++) {
            scytheVertices[j].x = (2 * thisv->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * thisv->actor.world.pos.z) - scytheVertices[j].z;
        }

        EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->blureEffectIndex[1])), &scytheVertices[0], &scytheVertices[1]);
    }

    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderScythe.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderScytheCenter.base);
    func_8002F974(&thisv->actor, NA_SE_EV_ROLLCUTTER_MOTOR - SFX_FLAG);
}

void BgHakaSgami_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaSgami* thisv = (BgHakaSgami*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    if (!(player->stateFlags1 & 0x300000C0) || (thisv->actionFunc == BgHakaSgami_SetupSpin)) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgHakaSgami_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaSgami* thisv = (BgHakaSgami*)thisx;

    if (thisv->unk_151 != 0) {
        Gfx_DrawDListXlu(globalCtx, object_haka_objects_DL_00BF20);
    } else if (thisv->actor.params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        Gfx_DrawDListOpa(globalCtx, object_haka_objects_DL_00BF20);
    } else {
        Gfx_DrawDListOpa(globalCtx, object_ice_objects_DL_0021F0);
    }
}
