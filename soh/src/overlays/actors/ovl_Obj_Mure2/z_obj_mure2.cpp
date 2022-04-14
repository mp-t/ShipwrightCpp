/*
 * File: z_obj_mure2.c
 * Overlay: ovl_Obj_Mure2
 * Description: Rock/Bush groups
 */

#include "z_obj_mure2.h"

#define FLAGS 0

typedef void (*ObjMure2SetPosFunc)(Vec3f* vec, ObjMure2* thisv);

typedef struct {
    s16 radius;
    s16 angle;
} Mure2sScatteredShrubInfo;

void ObjMure2_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjMure2_Update(Actor* thisx, GlobalContext* globalCtx);

void ObjMure2_SetPosShrubCircle(Vec3f* vec, ObjMure2* thisv);
void ObjMure2_SetPosShrubScattered(Vec3f* vec, ObjMure2* thisv);
void ObjMure2_SetPosRockCircle(Vec3f* vec, ObjMure2* thisv);
void ObjMure2_Wait(ObjMure2* thisv, GlobalContext* globalCtx);
void func_80B9A668(ObjMure2* thisv, GlobalContext* globalCtx);
void func_80B9A6F8(ObjMure2* thisv, GlobalContext* globalCtx);
void ObjMure2_SetupWait(ObjMure2* thisv);
void func_80B9A658(ObjMure2* thisv);
void func_80B9A6E8(ObjMure2* thisv);

const ActorInit Obj_Mure2_InitVars = {
    ACTOR_OBJ_MURE2,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure2),
    (ActorFunc)ObjMure2_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjMure2_Update,
    NULL,
    NULL,
};

static f32 sDistSquared1[] = { SQ(1600.0f), SQ(1600.0f), SQ(1600.0f) };

static f32 sDistSquared2[] = { SQ(1705.0f), SQ(1705.0f), SQ(1705.0f) };

static s16 D_80B9A818[] = { 9, 12, 8 };

static s16 sActorSpawnIDs[] = { ACTOR_EN_KUSA, ACTOR_EN_KUSA, ACTOR_EN_ISHI };

void ObjMure2_SetPosShrubCircle(Vec3f* vec, ObjMure2* thisv) {
    s32 i;

    Math_Vec3f_Copy(vec, &thisv->actor.world.pos);
    for (i = 1; i < D_80B9A818[thisv->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &thisv->actor.world.pos);
        (vec + i)->x += (80.0f * Math_SinS((i - 1) * 0x2000));
        (vec + i)->z += (80.0f * Math_CosS((i - 1) * 0x2000));
    }
}

static Mure2sScatteredShrubInfo sScatteredShrubInfo[] = {
    { 40, 0x0666 }, { 40, 0x2CCC }, { 40, 0x5999 }, { 40, 0x8666 }, { 20, 0xC000 }, { 80, 0x1333 },
    { 80, 0x4000 }, { 80, 0x6CCC }, { 80, 0x9333 }, { 80, 0xACCC }, { 80, 0xC666 }, { 60, 0xE000 },
};

void ObjMure2_SetPosShrubScattered(Vec3f* vec, ObjMure2* thisv) {
    s32 i;

    for (i = 0; i < D_80B9A818[thisv->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &thisv->actor.world.pos);
        (vec + i)->x += (sScatteredShrubInfo[i].radius * Math_CosS(sScatteredShrubInfo[i].angle));
        (vec + i)->z -= (sScatteredShrubInfo[i].radius * Math_SinS(sScatteredShrubInfo[i].angle));
    }
}

void ObjMure2_SetPosRockCircle(Vec3f* vec, ObjMure2* thisv) {
    s32 i;

    for (i = 0; i < D_80B9A818[thisv->actor.params & 3]; i++) {
        Math_Vec3f_Copy(vec + i, &thisv->actor.world.pos);
        (vec + i)->x += (80.0f * Math_SinS(i * 0x2000));
        (vec + i)->z += (80.0f * Math_CosS(i * 0x2000));
    }
}

void ObjMure2_SetActorSpawnParams(s16* params, ObjMure2* thisv) {
    static s16 actorSpawnParams[] = { 0, 0, 0 };
    s16 dropTable = (thisv->actor.params >> 8) & 0xF;

    if (dropTable >= 13) {
        dropTable = 0;
    }
    *params = actorSpawnParams[thisv->actor.params & 3] & 0xF0FF;
    *params |= (dropTable << 8);
}

void ObjMure2_SpawnActors(ObjMure2* thisv, GlobalContext* globalCtx) {
    static ObjMure2SetPosFunc setPosFunc[] = {
        ObjMure2_SetPosShrubCircle,
        ObjMure2_SetPosShrubScattered,
        ObjMure2_SetPosRockCircle,
    };
    s32 actorNum = thisv->actor.params & 3;
    s32 i;
    Vec3f spawnPos[12];
    s16 params;

    setPosFunc[actorNum](spawnPos, thisv);
    ObjMure2_SetActorSpawnParams(&params, thisv);

    for (i = 0; i < D_80B9A818[actorNum]; i++) {
        if (thisv->actorSpawnPtrList[i] != NULL) {
            // "Warning : I already have a child (%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Warning : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure2.c", 269,
                         thisv->actor.params);
            continue;
        }

        if (((thisv->currentActorNum >> i) & 1) == 0) {
            thisv->actorSpawnPtrList[i] =
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, sActorSpawnIDs[actorNum], spawnPos[i].x, spawnPos[i].y,
                            spawnPos[i].z, thisv->actor.world.rot.x, 0, thisv->actor.world.rot.z, params);
            if (thisv->actorSpawnPtrList[i] != NULL) {
                thisv->actorSpawnPtrList[i]->room = thisv->actor.room;
            }
        }
    }
}

void ObjMure2_CleanupAndDie(ObjMure2* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < D_80B9A818[thisv->actor.params & 3]; i++) {
        if (((thisv->currentActorNum >> i) & 1) == 0) {
            if (thisv->actorSpawnPtrList[i] != NULL) {
                if (Actor_HasParent(thisv->actorSpawnPtrList[i], globalCtx)) {
                    thisv->currentActorNum |= (1 << i);
                } else {
                    Actor_Kill(thisv->actorSpawnPtrList[i]);
                }
                thisv->actorSpawnPtrList[i] = NULL;
            }
        } else {
            thisv->actorSpawnPtrList[i] = NULL;
        }
    }
}

void func_80B9A534(ObjMure2* thisv) {
    s32 i;

    for (i = 0; i < D_80B9A818[thisv->actor.params & 3]; i++) {
        if (thisv->actorSpawnPtrList[i] != NULL && (((thisv->currentActorNum >> i) & 1) == 0) &&
            (thisv->actorSpawnPtrList[i]->update == NULL)) {
            thisv->currentActorNum |= (1 << i);
            thisv->actorSpawnPtrList[i] = NULL;
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 2100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 100, ICHAIN_STOP),
};

void ObjMure2_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure2* thisv = (ObjMure2*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        thisv->actor.uncullZoneForward += 1200.0f;
    }
    ObjMure2_SetupWait(thisv);
}

void ObjMure2_SetupWait(ObjMure2* thisv) {
    thisv->actionFunc = ObjMure2_Wait;
}

void ObjMure2_Wait(ObjMure2* thisv, GlobalContext* globalCtx) {
    func_80B9A658(thisv);
}

void func_80B9A658(ObjMure2* thisv) {
    thisv->actionFunc = func_80B9A668;
}

void func_80B9A668(ObjMure2* thisv, GlobalContext* globalCtx) {
    if (Math3D_Dist1DSq(thisv->actor.projectedPos.x, thisv->actor.projectedPos.z) <
            (sDistSquared1[thisv->actor.params & 3] * thisv->unk_184) ||
        CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        thisv->actor.flags |= ACTOR_FLAG_4;
        ObjMure2_SpawnActors(thisv, globalCtx);
        func_80B9A6E8(thisv);
    }
}

void func_80B9A6E8(ObjMure2* thisv) {
    thisv->actionFunc = func_80B9A6F8;
}

void func_80B9A6F8(ObjMure2* thisv, GlobalContext* globalCtx) {
    func_80B9A534(thisv);

    if (CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        return;
    }

    if ((sDistSquared2[thisv->actor.params & 3] * thisv->unk_184) <=
            Math3D_Dist1DSq(thisv->actor.projectedPos.x, thisv->actor.projectedPos.z)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        ObjMure2_CleanupAndDie(thisv, globalCtx);
        func_80B9A658(thisv);
    }
}

void ObjMure2_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure2* thisv = (ObjMure2*)thisx;

    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        thisv->unk_184 = 1.0f;
    } else {
        thisv->unk_184 = 4.0f;
    }
    thisv->actionFunc(thisv, globalCtx);
}
