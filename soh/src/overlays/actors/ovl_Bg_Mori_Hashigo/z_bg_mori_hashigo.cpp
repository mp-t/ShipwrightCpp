/*
 * File: z_bg_mori_hashigo.c
 * Overlay: ovl_Bg_Mori_Hashigo
 * Description: Falling ladder and clasp that holds it. Unused.
 */

#include "z_bg_mori_hashigo.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS 0

void BgMoriHashigo_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashigo_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashigo_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriHashigo_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriHashigo_SetupWaitForMoriTex(BgMoriHashigo* thisv);
void BgMoriHashigo_WaitForMoriTex(BgMoriHashigo* thisv, GlobalContext* globalCtx);
void BgMoriHashigo_SetupClasp(BgMoriHashigo* thisv);
void BgMoriHashigo_Clasp(BgMoriHashigo* thisv, GlobalContext* globalCtx);
void BgMoriHashigo_SetupLadderWait(BgMoriHashigo* thisv);
void BgMoriHashigo_LadderWait(BgMoriHashigo* thisv, GlobalContext* globalCtx);
void BgMoriHashigo_SetupLadderFall(BgMoriHashigo* thisv);
void BgMoriHashigo_LadderFall(BgMoriHashigo* thisv, GlobalContext* globalCtx);
void BgMoriHashigo_SetupLadderRest(BgMoriHashigo* thisv);

const ActorInit Bg_Mori_Hashigo_InitVars = {
    ACTOR_BG_MORI_HASHIGO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriHashigo),
    (ActorFunc)BgMoriHashigo_Init,
    (ActorFunc)BgMoriHashigo_Destroy,
    (ActorFunc)BgMoriHashigo_Update,
    NULL,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChainClasp[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE), ICHAIN_U8(targetMode, 3, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 40, ICHAIN_CONTINUE),    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainLadder[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgMoriHashigo_InitDynapoly(BgMoriHashigo* thisv, GlobalContext* globalCtx, const CollisionHeader* collision,
                                s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader;
    s32 pad2;

    colHeader = NULL;
    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashigo.c", 164,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void BgMoriHashigo_InitCollider(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->dyna.actor, &sJntSphInit, thisv->colliderItems);

    thisv->collider.elements[0].dim.worldSphere.center.x = (s16)thisv->dyna.actor.world.pos.x;
    thisv->collider.elements[0].dim.worldSphere.center.y = (s16)thisv->dyna.actor.world.pos.y + 21;
    thisv->collider.elements[0].dim.worldSphere.center.z = (s16)thisv->dyna.actor.world.pos.z;
    thisv->collider.elements[0].dim.worldSphere.radius = 19;
}

s32 BgMoriHashigo_SpawnLadder(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    f32 sn;
    f32 cs;
    Vec3f pos;
    Actor* ladder;

    cs = Math_CosS(thisv->dyna.actor.shape.rot.y);
    sn = Math_SinS(thisv->dyna.actor.shape.rot.y);

    pos.x = 6.0f * sn + thisv->dyna.actor.world.pos.x;
    pos.y = -210.0f + thisv->dyna.actor.world.pos.y;
    pos.z = 6.0f * cs + thisv->dyna.actor.world.pos.z;

    ladder = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_BG_MORI_HASHIGO, pos.x, pos.y,
                                pos.z, thisv->dyna.actor.world.rot.x, thisv->dyna.actor.world.rot.y,
                                thisv->dyna.actor.world.rot.z, 0);
    if (ladder != NULL) {
        return true;
    } else {
        // "Ladder failure"
        osSyncPrintf("Error : 梯子の発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashigo.c", 220,
                     thisv->dyna.actor.params);
        return false;
    }
}

s32 BgMoriHashigo_InitClasp(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChainClasp);
    thisv->dyna.actor.flags |= ACTOR_FLAG_0;
    Actor_SetFocus(&thisv->dyna.actor, 55.0f);
    BgMoriHashigo_InitCollider(thisv, globalCtx);
    if ((thisv->dyna.actor.params == HASHIGO_CLASP) && !BgMoriHashigo_SpawnLadder(thisv, globalCtx)) {
        return false;
    } else {
        return true;
    }
}

s32 BgMoriHashigo_InitLadder(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    BgMoriHashigo_InitDynapoly(thisv, globalCtx, &gMoriHashigoCol, DPM_UNK);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChainLadder);
    return true;
}

void BgMoriHashigo_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashigo* thisv = (BgMoriHashigo*)thisvx;

    if (thisv->dyna.actor.params == HASHIGO_CLASP) {
        if (!BgMoriHashigo_InitClasp(thisv, globalCtx)) {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
    } else if (thisv->dyna.actor.params == HASHIGO_LADDER) {
        if (!BgMoriHashigo_InitLadder(thisv, globalCtx)) {
            Actor_Kill(&thisv->dyna.actor);
            return;
        }
    }
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        // "Bank danger!"
        osSyncPrintf("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", thisv->dyna.actor.params,
                     "../z_bg_mori_hashigo.c", 312);
        Actor_Kill(&thisv->dyna.actor);
    } else {
        BgMoriHashigo_SetupWaitForMoriTex(thisv);
        // "(Forest Temple Ladder and its clasp)"
        osSyncPrintf("(森の神殿 梯子とその留め金)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
    }
}

void BgMoriHashigo_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashigo* thisv = (BgMoriHashigo*)thisvx;

    if (thisv->dyna.actor.params == HASHIGO_LADDER) {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
    if (thisv->dyna.actor.params == HASHIGO_CLASP) {
        Collider_DestroyJntSph(globalCtx, &thisv->collider);
    }
}

void BgMoriHashigo_SetupWaitForMoriTex(BgMoriHashigo* thisv) {
    thisv->actionFunc = BgMoriHashigo_WaitForMoriTex;
}

void BgMoriHashigo_WaitForMoriTex(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        if (thisv->dyna.actor.params == HASHIGO_CLASP) {
            BgMoriHashigo_SetupClasp(thisv);
        } else if (thisv->dyna.actor.params == HASHIGO_LADDER) {
            BgMoriHashigo_SetupLadderWait(thisv);
        }
        thisv->dyna.actor.draw = BgMoriHashigo_Draw;
    }
}

void BgMoriHashigo_SetupClasp(BgMoriHashigo* thisv) {
    thisv->actionFunc = BgMoriHashigo_Clasp;
}

void BgMoriHashigo_Clasp(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    if (thisv->hitTimer <= 0) {
        if (thisv->collider.base.acFlags & AC_HIT) {
            thisv->collider.base.acFlags &= ~AC_HIT;
            thisv->hitTimer = 10;
        } else {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

void BgMoriHashigo_SetupLadderWait(BgMoriHashigo* thisv) {
    thisv->actionFunc = BgMoriHashigo_LadderWait;
}

void BgMoriHashigo_LadderWait(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    BgMoriHashigo* clasp = (BgMoriHashigo*)thisv->dyna.actor.parent;

    if (clasp->hitTimer > 0) {
        BgMoriHashigo_SetupLadderFall(thisv);
    }
}

void BgMoriHashigo_SetupLadderFall(BgMoriHashigo* thisv) {
    thisv->bounceCounter = 0;
    thisv->actionFunc = BgMoriHashigo_LadderFall;
    thisv->dyna.actor.gravity = -1.0f;
    thisv->dyna.actor.minVelocityY = -10.0f;
    thisv->dyna.actor.velocity.y = 2.0f;
}

void BgMoriHashigo_LadderFall(BgMoriHashigo* thisv, GlobalContext* globalCtx) {
    static f32 bounceSpeed[3] = { 4.0f, 2.7f, 1.7f };
    Actor* thisvx = &thisv->dyna.actor;

    Actor_MoveForward(thisvx);
    if ((thisvx->bgCheckFlags & 1) && (thisvx->velocity.y < 0.0f)) {
        if (thisv->bounceCounter >= ARRAY_COUNT(bounceSpeed)) {
            BgMoriHashigo_SetupLadderRest(thisv);
        } else {
            Actor_UpdateBgCheckInfo(globalCtx, thisvx, 0.0f, 0.0f, 0.0f, 0x1C);
            thisvx->velocity.y = bounceSpeed[thisv->bounceCounter];
            thisv->bounceCounter++;
        }
    } else {
        Actor_UpdateBgCheckInfo(globalCtx, thisvx, 0.0f, 0.0f, 0.0f, 0x1C);
    }
}

void BgMoriHashigo_SetupLadderRest(BgMoriHashigo* thisv) {
    thisv->actionFunc = NULL;
    thisv->dyna.actor.gravity = 0.0f;
    thisv->dyna.actor.velocity.y = 0.0f;
    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
}

void BgMoriHashigo_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashigo* thisv = (BgMoriHashigo*)thisvx;

    if (thisv->hitTimer > 0) {
        thisv->hitTimer--;
    }
    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgMoriHashigo_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriHashigo* thisv = (BgMoriHashigo*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hashigo.c", 516);
    func_80093D18(globalCtx->state.gfxCtx);
    if (1) {}
    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hashigo.c", 521),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch (thisv->dyna.actor.params) {
        case HASHIGO_CLASP:
            gSPDisplayList(POLY_OPA_DISP++, gMoriHashigoClaspDL);
            break;
        case HASHIGO_LADDER:
            gSPDisplayList(POLY_OPA_DISP++, gMoriHashigoLadderDL);
            break;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hashigo.c", 531);
}
