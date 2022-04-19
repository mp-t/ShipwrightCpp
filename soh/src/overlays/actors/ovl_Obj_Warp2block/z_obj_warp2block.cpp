/*
 * File: z_obj_warp2block.c
 * Overlay: ovl_Obj_Warp2Block
 * Description: Navi Infospot (Green, Time Block)
 */

#include "z_obj_warp2block.h"
#include "objects/object_timeblock/object_timeblock.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

void ObjWarp2block_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjWarp2block_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjWarp2block_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjWarp2block_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjWarp2block_Spawn(ObjWarp2block* thisv, GlobalContext* globalCtx);
s32 func_80BA1ECC(ObjWarp2block* thisv, GlobalContext* globalCtx);
void ObjWarp2block_SwapWithChild(ObjWarp2block* thisv, GlobalContext* globalCtx);
s32 func_80BA2218(ObjWarp2block* thisv, GlobalContext* globalCtx);
s32 func_80BA228C(ObjWarp2block* thisv, GlobalContext* globalCtx);
s32 func_80BA2304(ObjWarp2block* thisv, GlobalContext* globalCtx);
void ObjWarp2block_SetInactive(ObjWarp2block* thisv);
void ObjWarp2block_DoNothing(ObjWarp2block* thisv, GlobalContext* globalCtx);
void func_80BA24E8(ObjWarp2block* thisv);
void func_80BA24F8(ObjWarp2block* thisv, GlobalContext* globalCtx);
void func_80BA2600(ObjWarp2block* thisv);
void func_80BA2610(ObjWarp2block* thisv, GlobalContext* globalCtx);

ActorInit Obj_Warp2block_InitVars = {
    ACTOR_OBJ_WARP2BLOCK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_TIMEBLOCK,
    sizeof(ObjWarp2block),
    (ActorFunc)ObjWarp2block_Init,
    (ActorFunc)ObjWarp2block_Destroy,
    (ActorFunc)ObjWarp2block_Update,
    (ActorFunc)ObjWarp2block_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 focus;
    /* 0x08 */ s16 params;
} Warp2BlockSpawnData; // size = 0x0C

static Warp2BlockSpawnData sSpawnData[] = {
    { 1.0f, 60.0f, 0x0018 },
    { 0.6f, 40.0f, 0x0019 },
};

static f32 sDistances[] = { 60.0f, 100.0f, 140.0f, 180.0f, 220.0f, 260.0f, 300.0f, 300.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 sColors[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

void ObjWarp2block_Spawn(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_EFFECT, thisv->dyna.actor.world.pos.x,
                thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z, 0, 0, 0,
                sSpawnData[(thisv->dyna.actor.params >> 8) & 1].params);

    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_EFFECT, thisv->dyna.actor.child->world.pos.x,
                thisv->dyna.actor.child->world.pos.y, thisv->dyna.actor.child->world.pos.z, 0, 0, 0,
                sSpawnData[(thisv->dyna.actor.child->params >> 8) & 1].params);
}

s32 func_80BA1ECC(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Actor* temp_a3;
    Player* player;
    Vec3f sp20;
    f32 temp_f2;

    if (func_80043590(&thisv->dyna)) {
        return 0;
    }

    temp_a3 = thisv->dyna.actor.child;
    player = GET_PLAYER(globalCtx);
    if ((thisv->dyna.actor.xzDistToPlayer <= sDistances[(((thisv->dyna.actor.params >> 0xB) & 7))]) ||
        (temp_a3->xzDistToPlayer <= sDistances[(((temp_a3->params >> 0xB) & 7))])) {

        func_8002DBD0(&thisv->dyna.actor, &sp20, &player->actor.world.pos);
        temp_f2 = (thisv->dyna.actor.scale.x * 50.0f) + 6.0f;

        if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
            return 0;
        }

        func_8002DBD0(temp_a3, &sp20, &player->actor.world.pos);
        temp_f2 = (temp_a3->scale.x * 50.0f) + 6.0f;

        if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

void ObjWarp2block_SwapWithChild(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    Vec3f tempVec;
    Vec3s tempRot;
    s32 temp;

    Math_Vec3f_Copy(&tempVec, &thisv->dyna.actor.world.pos);
    Math_Vec3f_Copy(&thisv->dyna.actor.world.pos, &thisv->dyna.actor.child->world.pos);
    Math_Vec3f_Copy(&thisv->dyna.actor.child->world.pos, &tempVec);

    temp = thisv->dyna.actor.world.rot.y;
    thisv->dyna.actor.world.rot.y = thisv->dyna.actor.child->world.rot.y;
    thisv->dyna.actor.child->world.rot.y = temp;

    temp = thisv->dyna.actor.shape.rot.y;
    thisv->dyna.actor.shape.rot.y = thisv->dyna.actor.child->shape.rot.y;
    thisv->dyna.actor.child->shape.rot.y = temp;

    temp = thisv->dyna.actor.home.rot.z;
    thisv->dyna.actor.home.rot.z = thisv->dyna.actor.child->home.rot.z;
    thisv->dyna.actor.child->home.rot.z = temp;

    Math_Vec3f_Copy(&tempVec, &thisv->dyna.actor.scale);
    Math_Vec3f_Copy(&thisv->dyna.actor.scale, &thisv->dyna.actor.child->scale);
    Math_Vec3f_Copy(&thisv->dyna.actor.child->scale, &tempVec);

    Math_Vec3f_Copy(&tempVec, &thisv->dyna.actor.focus.pos);
    Math_Vec3f_Copy(&thisv->dyna.actor.focus.pos, &thisv->dyna.actor.child->focus.pos);
    Math_Vec3f_Copy(&thisv->dyna.actor.child->focus.pos, &tempVec);

    tempRot = thisv->dyna.actor.focus.rot;
    thisv->dyna.actor.focus.rot = thisv->dyna.actor.child->focus.rot;
    thisv->dyna.actor.child->focus.rot = tempRot;

    temp = thisv->dyna.actor.params & 0x7FFF;
    thisv->dyna.actor.params = (thisv->dyna.actor.params & 0x8000) | (thisv->dyna.actor.child->params & 0x7FFF);
    thisv->dyna.actor.child->params = (thisv->dyna.actor.child->params & 0x8000) | (temp & 0x7FFF);

    if (Math3D_Vec3fDistSq(&thisv->dyna.actor.world.pos, &thisv->dyna.actor.home.pos) < 0.01f) {
        Flags_UnsetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
    } else {
        Flags_SetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
    }
}

s32 func_80BA2218(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (func_80BA1ECC(thisv, globalCtx)) {
        if (player->stateFlags2 & 0x1000000) {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
            thisv->func_168 = func_80BA228C;
        } else {
            player->stateFlags2 |= 0x800000;
        }
    }

    return 0;
}

s32 func_80BA228C(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        thisv->func_168 = func_80BA2218;
    }

    if (globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_TIME) {
        if (thisv->unk_172 == 0xFE) {
            thisv->unk_16E = 0x6E;
        } else {
            thisv->unk_16E--;
            if (thisv->unk_16E == 0) {
                return 1;
            }
        }
    }
    return 0;
}

s32 func_80BA2304(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    s32 ret = thisv->func_168(thisv, globalCtx);

    thisv->unk_172 = globalCtx->msgCtx.lastPlayedSong;

    return ret;
}

void ObjWarp2block_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjWarp2block* thisv = (ObjWarp2block*)thisx;
    const CollisionHeader* collisionHeader;

    collisionHeader = NULL;
    thisv->dyna.actor.world.rot.z = thisv->dyna.actor.shape.rot.z = 0;
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    Actor_SetScale(&thisv->dyna.actor, sSpawnData[(thisv->dyna.actor.params >> 8) & 1].scale);
    thisv->func_168 = func_80BA2218;
    Actor_SetFocus(&thisv->dyna.actor, sSpawnData[(thisv->dyna.actor.params >> 8) & 1].focus);

    if ((thisv->dyna.actor.params >> 0xF) & 1) {
        func_80BA24E8(thisv);
        if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
            thisv->dyna.actor.draw = NULL;
        }
        DynaPolyActor_Init(&thisv->dyna, 0);
        CollisionHeader_GetVirtual(&gSongOfTimeBlockCol, &collisionHeader);
        thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, collisionHeader);
    } else {
        ObjWarp2block_SetInactive(thisv);
    }

    osSyncPrintf("時のブロック(ワープ２) (<arg> %04xH <type> color:%d range:%d)\n", thisv->dyna.actor.params & 0xFFFF,
                 thisv->dyna.actor.home.rot.z & 7, (thisv->dyna.actor.params >> 0xB) & 7);
}

void ObjWarp2block_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjWarp2block* thisv = (ObjWarp2block*)thisx;
    if ((thisv->dyna.actor.params >> 0xF) & 1) {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    }
}

void ObjWarp2block_SetInactive(ObjWarp2block* thisv) {
    thisv->actionFunc = ObjWarp2block_DoNothing;
    thisv->dyna.actor.draw = NULL;
}

void ObjWarp2block_DoNothing(ObjWarp2block* thisv, GlobalContext* globalCtx) {
}

void func_80BA24E8(ObjWarp2block* thisv) {
    thisv->actionFunc = func_80BA24F8;
}

void func_80BA24F8(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    Actor* current = globalCtx->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (current != NULL) {
        if (current->id == ACTOR_OBJ_WARP2BLOCK && !((current->params >> 0xF) & 1) &&
            ((thisv->dyna.actor.params & 0x3F) == (current->params & 0x3F))) {
            thisv->dyna.actor.child = current;
            if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
                ObjWarp2block_SwapWithChild(thisv, globalCtx);
                thisv->dyna.actor.draw = ObjWarp2block_Draw;
            }
            func_80BA2600(thisv);
            return;
        }

        current = current->next;
    }

    thisv->unk_174++;
    if (thisv->unk_174 > 60) {
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("Error : 時のブロック(ワープ２)が対でセットされていません(%s %d)\n", "../z_obj_warp2block.c", 505);
        osSyncPrintf(VT_RST);
        Actor_Kill(&thisv->dyna.actor);
    }
}

void func_80BA2600(ObjWarp2block* thisv) {
    thisv->actionFunc = func_80BA2610;
}

void func_80BA2610(ObjWarp2block* thisv, GlobalContext* globalCtx) {
    if ((func_80BA2304(thisv, globalCtx) != 0) && (thisv->unk_16C <= 0)) {
        ObjWarp2block_Spawn(thisv, globalCtx);
        thisv->unk_16C = 0xA0;
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        thisv->unk_170 = 0xC;
    }

    if (thisv->unk_170 > 0) {
        thisv->unk_170--;
        if (thisv->unk_170 == 0) {
            ObjWarp2block_SwapWithChild(thisv, globalCtx);
        }
    }
    if (thisv->unk_16C == 0x32) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void ObjWarp2block_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjWarp2block* thisv = (ObjWarp2block*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->unk_16C > 0) {
        thisv->unk_16C--;
    }
}

void ObjWarp2block_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Color_RGB8* sp44;

    sp44 = &sColors[thisx->home.rot.z & 7];

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_warp2block.c", 584);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_warp2block.c", 588),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sp44->r, sp44->g, sp44->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_warp2block.c", 594);
}
