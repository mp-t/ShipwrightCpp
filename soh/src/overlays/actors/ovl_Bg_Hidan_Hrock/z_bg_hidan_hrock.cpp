/*
 * File: z_bg_hidan_hrock.c
 * Overlay: ovl_Bg_Hidan_Hrock
 * Description: Huge stone spike platform (Fire Temple)
 */

#include "z_bg_hidan_hrock.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanHrock_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanHrock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanHrock_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanHrock_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8088960C(BgHidanHrock* thisv, GlobalContext* globalCtx);
void func_808896B8(BgHidanHrock* thisv, GlobalContext* globalCtx);
void func_808894A4(BgHidanHrock* thisv, GlobalContext* globalCtx);

ActorInit Bg_Hidan_Hrock_InitVars = {
    ACTOR_BG_HIDAN_HROCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanHrock),
    (ActorFunc)BgHidanHrock_Init,
    (ActorFunc)BgHidanHrock_Destroy,
    (ActorFunc)BgHidanHrock_Update,
    (ActorFunc)BgHidanHrock_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, 40.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, -40.0f } } },
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
    2,
    sTrisElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_STOP),
};

void BgHidanHrock_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanHrock* thisv = (BgHidanHrock*)thisx;
    ColliderTrisElementInit* colliderElementInit;
    Vec3f vertices[3];
    f32 cosRotY;
    f32 sinRotY;
    s32 i;
    s32 j;
    const CollisionHeader* collisionHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisv->unk_16A = thisx->params & 0x3F;
    thisx->params = (thisx->params >> 8) & 0xFF;
    Collider_InitTris(globalCtx, &thisv->collider);
    Collider_SetTris(globalCtx, &thisv->collider, thisx, &sTrisInit, thisv->colliderItems);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);

    sinRotY = Math_SinS(thisx->shape.rot.y);
    cosRotY = Math_CosS(thisx->shape.rot.y);

    if (thisx->params == 0) {
        sinRotY *= 1.5f;
        cosRotY *= 1.5f;
    }

    for (i = 0; i < 2; i++) {
        colliderElementInit = &sTrisInit.elements[i];

        if (1) {
            for (j = 0; j < 3; j++) {
                Vec3f* vtx = &colliderElementInit->dim.vtx[j];

                vertices[j].x = vtx->z * sinRotY + (thisx->home.pos.x + vtx->x * cosRotY);
                vertices[j].y = vtx->y + thisx->home.pos.y;
                vertices[j].z = vtx->z * cosRotY + (thisx->home.pos.z - vtx->x * sinRotY);
            }
        }
        Collider_SetTrisVertices(&thisv->collider, i, &vertices[0], &vertices[1], &vertices[2]);
    }

    if (Flags_GetSwitch(globalCtx, thisv->unk_16A)) {
        thisv->actionFunc = func_808894A4;
        if (thisx->params == 0) {
            thisx->world.pos.y -= 2800.0f;
            thisx->uncullZoneForward = 3000.0f;
        } else if (thisx->params == 1) {
            thisx->world.pos.y -= 800.0f;
        } else if (thisx->params == 2) {
            thisx->world.pos.y -= 240.0f;
        }
    } else {
        if (thisx->params == 0) {
            thisx->flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
            thisx->uncullZoneForward = 3000.0f;
        }
        thisv->actionFunc = func_808896B8;
    }

    if (thisx->params == 0) {
        CollisionHeader_GetVirtual(&gFireTempleTallestPillarAboveRoomBeforeBossCol, &collisionHeader);
    } else {
        CollisionHeader_GetVirtual(&gFireTemplePillarInsertedInGroundCol, &collisionHeader);
    }

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, collisionHeader);
}

void BgHidanHrock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanHrock* thisv = (BgHidanHrock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyTris(globalCtx, &thisv->collider);
}

void func_808894A4(BgHidanHrock* thisv, GlobalContext* globalCtx) {
}

void func_808894B0(BgHidanHrock* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_168 != 0) {
        thisv->unk_168--;
    }

    thisv->dyna.actor.world.pos.x =
        (Math_SinS(thisv->dyna.actor.world.rot.y + (thisv->unk_168 << 0xE)) * 5.0f) + thisv->dyna.actor.home.pos.x;
    thisv->dyna.actor.world.pos.z =
        (Math_CosS(thisv->dyna.actor.world.rot.y + (thisv->unk_168 << 0xE)) * 5.0f) + thisv->dyna.actor.home.pos.z;

    if (!(thisv->unk_168 % 4)) {
        func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }

    if (thisv->unk_168 == 0) {
        if (thisv->dyna.actor.params == 0) {
            thisv->dyna.actor.home.pos.y -= 2800.0f;
        } else if (thisv->dyna.actor.params == 1) {
            thisv->dyna.actor.home.pos.y -= 800.0f;
        } else {
            thisv->dyna.actor.home.pos.y -= 240.0f;
        }

        thisv->actionFunc = func_8088960C;
        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.world.pos.z = thisv->dyna.actor.home.pos.z;
    }
}

void func_8088960C(BgHidanHrock* thisv, GlobalContext* globalCtx) {
    thisv->dyna.actor.velocity.y++;

    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, thisv->dyna.actor.velocity.y)) {
        thisv->dyna.actor.flags &= ~(ACTOR_FLAG_4 | ACTOR_FLAG_5);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);

        if (thisv->dyna.actor.params == 0) {
            if (globalCtx->roomCtx.curRoom.num == 10) {
                thisv->dyna.actor.room = 10;
            } else {
                Actor_Kill(&thisv->dyna.actor);
            }
        }

        thisv->actionFunc = func_808894A4;
    }
}

void func_808896B8(BgHidanHrock* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & 2) {
        thisv->collider.base.acFlags &= ~2;
        thisv->actionFunc = func_808894B0;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;

        if (thisv->dyna.actor.params == 0) {
            thisv->dyna.actor.room = -1;
        }

        thisv->unk_168 = 20;
        Flags_SetSwitch(globalCtx, thisv->unk_16A);
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (func_8004356C(&thisv->dyna)) {
        Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y - 5.0f, 1.0f);
    } else {
        Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 1.0f);
    }
}

void BgHidanHrock_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanHrock* thisv = (BgHidanHrock*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgHidanHrock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const Gfx* dlists[] = {
        gFireTempleTallestPillarAboveRoomBeforeBossDL,
        gFireTemplePillarInsertedInGroundDL,
        gFireTemplePillarInsertedInGroundDL,
    };

    Gfx_DrawDListOpa(globalCtx, dlists[thisx->params]);
}
