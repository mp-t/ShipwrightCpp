/*
 * File: z_bg_mori_kaitenkabe.c
 * Overlay: ovl_Bg_Mori_Kaitenkabe
 * Description: Rotating wall in Forest Temple basement
 */

#include "z_bg_mori_kaitenkabe.h"
#include "objects/object_mori_objects/object_mori_objects.h"

#define FLAGS 0

void BgMoriKaitenkabe_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriKaitenkabe_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriKaitenkabe_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMoriKaitenkabe_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgMoriKaitenkabe_WaitForMoriTex(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx);
void BgMoriKaitenkabe_SetupWait(BgMoriKaitenkabe* thisv);
void BgMoriKaitenkabe_Wait(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx);
void BgMoriKaitenkabe_SetupRotate(BgMoriKaitenkabe* thisv);
void BgMoriKaitenkabe_Rotate(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx);

const ActorInit Bg_Mori_Kaitenkabe_InitVars = {
    ACTOR_BG_MORI_KAITENKABE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MORI_OBJECTS,
    sizeof(BgMoriKaitenkabe),
    (ActorFunc)BgMoriKaitenkabe_Init,
    (ActorFunc)BgMoriKaitenkabe_Destroy,
    (ActorFunc)BgMoriKaitenkabe_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgMoriKaitenkabe_CrossProduct(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

void BgMoriKaitenkabe_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriKaitenkabe* thisv = (BgMoriKaitenkabe*)thisvx;
    const CollisionHeader* colHeader = NULL;

    // "Forest Temple object 【Rotating Wall (arg_data: 0x% 04x)】 appears"
    osSyncPrintf("◯◯◯森の神殿オブジェクト【回転壁(arg_data : 0x%04x)】出現 \n", thisv->dyna.actor.params);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gMoriKaitenkabeCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    thisv->moriTexObjIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (thisv->moriTexObjIndex < 0) {
        Actor_Kill(&thisv->dyna.actor);
        // "【Rotating wall】 Bank danger!"
        osSyncPrintf("【回転壁】 バンク危険！(%s %d)\n", "../z_bg_mori_kaitenkabe.c", 176);
    } else {
        thisv->actionFunc = BgMoriKaitenkabe_WaitForMoriTex;
    }
}

void BgMoriKaitenkabe_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriKaitenkabe* thisv = (BgMoriKaitenkabe*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void BgMoriKaitenkabe_WaitForMoriTex(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->moriTexObjIndex)) {
        BgMoriKaitenkabe_SetupWait(thisv);
        thisv->dyna.actor.draw = BgMoriKaitenkabe_Draw;
    }
}

void BgMoriKaitenkabe_SetupWait(BgMoriKaitenkabe* thisv) {
    thisv->actionFunc = BgMoriKaitenkabe_Wait;
    thisv->timer = 0;
}

void BgMoriKaitenkabe_Wait(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx) {
    Vec3f push;
    Vec3f leverArm;
    Vec3f torque;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 > 0.001f) {
        thisv->timer++;
        if ((thisv->timer > 28) && !Player_InCsMode(globalCtx)) {
            BgMoriKaitenkabe_SetupRotate(thisv);
            func_8002DF54(globalCtx, &thisv->dyna.actor, 8);
            Math_Vec3f_Copy(&thisv->lockedPlayerPos, &player->actor.world.pos);
            push.x = Math_SinS(thisv->dyna.unk_158);
            push.y = 0.0f;
            push.z = Math_CosS(thisv->dyna.unk_158);
            leverArm.x = thisv->dyna.actor.world.pos.x - player->actor.world.pos.x;
            leverArm.y = 0.0f;
            leverArm.z = thisv->dyna.actor.world.pos.z - player->actor.world.pos.z;
            BgMoriKaitenkabe_CrossProduct(&torque, &push, &leverArm);
            thisv->rotDirection = (torque.y > 0.0f) ? 1.0f : -1.0f;
        }
    } else {
        thisv->timer = 0;
    }
    if (fabsf(thisv->dyna.unk_150) > 0.001f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
}

void BgMoriKaitenkabe_SetupRotate(BgMoriKaitenkabe* thisv) {
    thisv->actionFunc = BgMoriKaitenkabe_Rotate;
    thisv->rotSpeed = 0.0f;
    thisv->rotYdeg = 0.0f;
}

void BgMoriKaitenkabe_Rotate(BgMoriKaitenkabe* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* thisvx = &thisv->dyna.actor;
    s16 rotY;

    Math_StepToF(&thisv->rotSpeed, 0.6f, 0.02f);
    if (Math_StepToF(&thisv->rotYdeg, thisv->rotDirection * 45.0f, thisv->rotSpeed)) {
        BgMoriKaitenkabe_SetupWait(thisv);
        func_8002DF54(globalCtx, thisvx, 7);
        if (thisv->rotDirection > 0.0f) {
            thisvx->home.rot.y += 0x2000;
        } else {
            thisvx->home.rot.y -= 0x2000;
        }
        thisvx->world.rot.y = thisvx->shape.rot.y = thisvx->home.rot.y;
        func_800788CC(NA_SE_EV_STONEDOOR_STOP);
    } else {
        rotY = thisv->rotYdeg * (0x10000 / 360.0f);
        thisvx->world.rot.y = thisvx->shape.rot.y = thisvx->home.rot.y + rotY;
        func_800788CC(NA_SE_EV_WALL_SLIDE - SFX_FLAG);
    }
    if (fabsf(thisv->dyna.unk_150) > 0.001f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
    Math_Vec3f_Copy(&player->actor.world.pos, &thisv->lockedPlayerPos);
}

void BgMoriKaitenkabe_Update(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriKaitenkabe* thisv = (BgMoriKaitenkabe*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgMoriKaitenkabe_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgMoriKaitenkabe* thisv = (BgMoriKaitenkabe*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 347);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[thisv->moriTexObjIndex].segment);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 352),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gMoriKaitenkabeDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 356);
}
