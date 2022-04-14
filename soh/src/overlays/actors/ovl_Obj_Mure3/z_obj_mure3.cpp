/*
 * File: z_obj_mure3.c
 * Overlay: ovl_Obj_Mure3
 * Description: Tower of Rupees
 */

#include "z_obj_mure3.h"

#define FLAGS 0

void ObjMure3_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjMure3_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjMure3_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B9AF24(ObjMure3* thisv);
void func_80B9AF34(ObjMure3* thisv, GlobalContext* globalCtx);
void func_80B9AF54(ObjMure3* thisv);
void func_80B9AF64(ObjMure3* thisv, GlobalContext* globalCtx);
void func_80B9AFEC(ObjMure3* thisv);
void func_80B9AFFC(ObjMure3* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Mure3_InitVars = {
    ACTOR_OBJ_MURE3,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure3),
    (ActorFunc)ObjMure3_Init,
    (ActorFunc)ObjMure3_Destroy,
    (ActorFunc)ObjMure3_Update,
    NULL,
    NULL,
};

static s16 sRupeeCounts[] = { 5, 5, 7, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 100, ICHAIN_STOP),
};

void func_80B9A9D0(ObjMure3* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f spawnPos;

    Math_Vec3f_Copy(&spawnPos, &thisv->actor.world.pos);
    for (i = 0; i < 5; i++, spawnPos.y += 20.0f) {
        if (!((thisv->unk_16C >> i) & 1)) {
            thisv->unk_150[i] = Item_DropCollectible2(globalCtx, &spawnPos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (thisv->unk_150[i] != NULL) {
                thisv->unk_150[i]->actor.room = thisv->actor.room;
            }
        }
    }
}

void func_80B9AA90(ObjMure3* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f spawnPos;
    f32 sn = Math_SinS(thisv->actor.world.rot.y);
    f32 cos = Math_CosS(thisv->actor.world.rot.y);
    f32 radius;

    spawnPos.y = thisv->actor.world.pos.y;

    for (i = 0, radius = -40.0f; i < 5; i++, radius += 20.0f) {
        if (!((thisv->unk_16C >> i) & 1)) {
            spawnPos.x = thisv->actor.world.pos.x + (sn * radius);
            spawnPos.z = thisv->actor.world.pos.z + (cos * radius);
            thisv->unk_150[i] = Item_DropCollectible2(globalCtx, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (thisv->unk_150[i] != NULL) {
                thisv->unk_150[i]->actor.room = thisv->actor.room;
            }
        }
    }
}

void func_80B9ABA0(ObjMure3* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f spawnPos;
    s16 yRot;

    spawnPos.y = thisv->actor.world.pos.y;
    yRot = thisv->actor.world.rot.y;
    for (i = 0; i < 6; i++) {
        if (!((thisv->unk_16C >> i) & 1)) {
            spawnPos.x = (Math_SinS(yRot) * 40.0f) + thisv->actor.world.pos.x;
            spawnPos.z = (Math_CosS(yRot) * 40.0f) + thisv->actor.world.pos.z;
            thisv->unk_150[i] = Item_DropCollectible2(globalCtx, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (thisv->unk_150[i] != NULL) {
                thisv->unk_150[i]->actor.room = thisv->actor.room;
            }
        }
        yRot += 0x2AAA;
    }
    if (!((thisv->unk_16C >> 6) & 1)) {
        spawnPos.x = thisv->actor.world.pos.x;
        spawnPos.z = thisv->actor.world.pos.z;
        thisv->unk_150[6] = Item_DropCollectible2(globalCtx, &spawnPos, 0x4000 | ITEM00_RUPEE_RED);
        if (thisv->unk_150[6] != NULL) {
            thisv->unk_150[6]->actor.room = thisv->actor.room;
        }
    }
}

void func_80B9ACE4(ObjMure3* thisv, GlobalContext* globalCtx) {
    s16 count = sRupeeCounts[(thisv->actor.params >> 13) & 7];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &thisv->unk_150[i];

        if (!((thisv->unk_16C >> i) & 1) && (*collectible != NULL)) {
            if (Actor_HasParent(&(*collectible)->actor, globalCtx) || ((*collectible)->actor.update == NULL)) {
                thisv->unk_16C |= (1 << i);
            } else {
                Actor_Kill(&(*collectible)->actor);
            }
        }
        *collectible = NULL;
    }
}

void func_80B9ADCC(ObjMure3* thisv, GlobalContext* globalCtx) {
    s16 count = sRupeeCounts[(thisv->actor.params >> 13) & 7];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &thisv->unk_150[i];

        if ((*collectible != NULL) && !((thisv->unk_16C >> i) & 1)) {
            if (Actor_HasParent(&(*collectible)->actor, globalCtx)) {
                Flags_SetSwitch(globalCtx, thisv->actor.params & 0x3F);
            }
            if ((*collectible)->actor.update == NULL) {
                thisv->unk_16C |= (1 << i);
                thisv->unk_150[i] = NULL;
            }
        }
    }
}

void ObjMure3_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjMure3* thisv = (ObjMure3*)thisx;

    if (Flags_GetSwitch(globalCtx, thisv->actor.params & 0x3F)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    func_80B9AF24(thisv);
}

void ObjMure3_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_80B9AF24(ObjMure3* thisv) {
    thisv->actionFunc = func_80B9AF34;
}

void func_80B9AF34(ObjMure3* thisv, GlobalContext* globalCtx) {
    func_80B9AF54(thisv);
}

void func_80B9AF54(ObjMure3* thisv) {
    thisv->actionFunc = func_80B9AF64;
}

void func_80B9AF64(ObjMure3* thisv, GlobalContext* globalCtx) {
    static ObjMure3SpawnFunc spawnFuncs[] = { func_80B9A9D0, func_80B9AA90, func_80B9ABA0 };

    if (Math3D_Dist1DSq(thisv->actor.projectedPos.x, thisv->actor.projectedPos.z) < SQ(1150.0f)) {
        thisv->actor.flags |= ACTOR_FLAG_4;
        spawnFuncs[(thisv->actor.params >> 13) & 7](thisv, globalCtx);
        func_80B9AFEC(thisv);
    }
}

void func_80B9AFEC(ObjMure3* thisv) {
    thisv->actionFunc = func_80B9AFFC;
}

void func_80B9AFFC(ObjMure3* thisv, GlobalContext* globalCtx) {
    func_80B9ADCC(thisv, globalCtx);
    if (Math3D_Dist1DSq(thisv->actor.projectedPos.x, thisv->actor.projectedPos.z) >= SQ(1450.0f)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        func_80B9ACE4(thisv, globalCtx);
        func_80B9AF54(thisv);
    }
}

void ObjMure3_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure3* thisv = (ObjMure3*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}
