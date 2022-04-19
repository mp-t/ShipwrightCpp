/*
 * File: z_obj_mure.c
 * Overlay: ovl_Obj_Mure
 * Description: Spawns Fish, Bug, Butterfly
 */

#include "z_obj_mure.h"

#define FLAGS 0

void ObjMure_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjMure_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjMure_Update(Actor* thisx, GlobalContext* globalCtx);

void ObjMure_InitialAction(ObjMure* thisv, GlobalContext* globalCtx);
void ObjMure_CulledState(ObjMure* thisv, GlobalContext* globalCtx);
void ObjMure_ActiveState(ObjMure* thisv, GlobalContext* globalCtx);

s32 ObjMure_GetMaxChildSpawns(ObjMure* thisv);

ActorInit Obj_Mure_InitVars = {
    ACTOR_OBJ_MURE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure),
    (ActorFunc)ObjMure_Init,
    (ActorFunc)ObjMure_Destroy,
    (ActorFunc)ObjMure_Update,
    NULL,
    NULL,
};

typedef enum {
    /* 0 */ OBJMURE_TYPE_GRASS,
    /* 1 */ OBJMURE_TYPE_UNDEFINED,
    /* 2 */ OBJMURE_TYPE_FISH,
    /* 3 */ OBJMURE_TYPE_BUGS,
    /* 4 */ OBJMURE_TYPE_BUTTERFLY
} ObjMureType;

typedef enum {
    /* 0 */ OBJMURE_CHILD_STATE_0,
    /* 1 */ OBJMURE_CHILD_STATE_1, // Dead
    /* 2 */ OBJMURE_CHILD_STATE_2
} ObjMureChildState;

static f32 sZClip[] = { 1600.0f, 1600.0f, 1000.0f, 1000.0f, 1000.0f };

static s32 sMaxChildSpawns[] = { 12, 9, 8, 0 };

static s16 sSpawnActorIds[] = { ACTOR_EN_KUSA, 0, ACTOR_EN_FISH, ACTOR_EN_INSECT, ACTOR_EN_BUTTE };

static s16 sSpawnParams[] = { 0, 2, -1, 0, -1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

s32 ObjMure_SetCullingImpl(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure* thisv = (ObjMure*)thisx;
    s32 result;

    switch (thisv->type) {
        case OBJMURE_TYPE_FISH:
        case OBJMURE_TYPE_BUGS:
        case OBJMURE_TYPE_BUTTERFLY:
            Actor_ProcessInitChain(&thisv->actor, sInitChain);
            result = true;
            break;
        default:
            // "Error : Culling is not set.(%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Error : カリングの設定がされていません。(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 204,
                         thisv->actor.params);
            return false;
    }
    return result;
}

s32 ObjMure_SetCulling(Actor* thisx, GlobalContext* globalCtx) {
    if (!ObjMure_SetCullingImpl(thisx, globalCtx)) {
        return false;
    }
    return true;
}

void ObjMure_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure* thisv = (ObjMure*)thisx;

    thisv->chNum = (thisx->params >> 0xC) & 0x0F;
    thisv->ptn = (thisx->params >> 8) & 0x07;
    thisv->svNum = (thisx->params >> 5) & 0x03;
    thisv->type = thisx->params & 0x1F;

    if (thisv->ptn >= 4) {
        osSyncPrintf("Error 群れな敵 (%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 237, thisx->params);
        Actor_Kill(&thisv->actor);
        return;
    } else if (thisv->type >= 5) {
        osSyncPrintf("Error 群れな敵 (%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 245, thisx->params);
        Actor_Kill(&thisv->actor);
        return;
    } else if (!ObjMure_SetCulling(thisx, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->actionFunc = ObjMure_InitialAction;
    osSyncPrintf("群れな敵 (arg_data 0x%04x)(chNum(%d) ptn(%d) svNum(%d) type(%d))\n", thisx->params, thisv->chNum,
                 thisv->ptn, thisv->svNum, thisv->type);
    if (ObjMure_GetMaxChildSpawns(thisv) <= 0) {
        osSyncPrintf("Warning : 個体数が設定されていません(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 268,
                     thisx->params);
    }
}

void ObjMure_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

s32 ObjMure_GetMaxChildSpawns(ObjMure* thisv) {
    if (thisv->chNum == 0) {
        return sMaxChildSpawns[thisv->ptn];
    }
    return thisv->chNum;
}

void ObjMure_GetSpawnPos(Vec3f* outPos, Vec3f* inPos, s32 ptn, s32 idx) {
    if (ptn >= 4) {
        osSyncPrintf("おかしなの (%s %d)\n", "../z_obj_mure.c", 307);
    }
    *outPos = *inPos;
}

void ObjMure_SpawnActors0(ObjMure* thisv, GlobalContext* globalCtx) {
    ActorContext* ac;
    s32 i;
    Vec3f pos;
    s32 pad;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);

    for (i = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            // "Error: I already have a child(%s %d)(arg_data 0x%04x)"
            osSyncPrintf("Error : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 333,
                         thisv->actor.params);
        }
        switch (thisv->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_1:
                break;
            case OBJMURE_CHILD_STATE_2:
                ac = &globalCtx->actorCtx;
                ObjMure_GetSpawnPos(&pos, &thisv->actor.world.pos, thisv->ptn, i);
                thisv->children[i] =
                    Actor_Spawn(ac, globalCtx, sSpawnActorIds[thisv->type], pos.x, pos.y, pos.z, thisv->actor.world.rot.x,
                                thisv->actor.world.rot.y, thisv->actor.world.rot.z, sSpawnParams[thisv->type]);
                if (thisv->children[i] != NULL) {
                    thisv->children[i]->flags |= ACTOR_FLAG_ENKUSA_CUT;
                    thisv->children[i]->room = thisv->actor.room;
                } else {
                    osSyncPrintf("warning 発生失敗 (%s %d)\n", "../z_obj_mure.c", 359);
                }
                break;
            default:
                ac = &globalCtx->actorCtx;
                ObjMure_GetSpawnPos(&pos, &thisv->actor.world.pos, thisv->ptn, i);
                thisv->children[i] =
                    Actor_Spawn(ac, globalCtx, sSpawnActorIds[thisv->type], pos.x, pos.y, pos.z, thisv->actor.world.rot.x,
                                thisv->actor.world.rot.y, thisv->actor.world.rot.z, sSpawnParams[thisv->type]);
                if (thisv->children[i] != NULL) {
                    thisv->children[i]->room = thisv->actor.room;
                } else {
                    osSyncPrintf("warning 発生失敗 (%s %d)\n", "../z_obj_mure.c", 382);
                }
                break;
        }
    }
}

void ObjMure_SpawnActors1(ObjMure* thisv, GlobalContext* globalCtx) {
    ActorContext* ac = (ActorContext*)globalCtx; // fake match
    Actor* actor = &thisv->actor;
    Vec3f spawnPos;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            osSyncPrintf("Error : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure.c", 407, actor->params);
        }
        ac = &globalCtx->actorCtx;
        ObjMure_GetSpawnPos(&spawnPos, &actor->world.pos, thisv->ptn, i);
        thisv->children[i] = Actor_Spawn(ac, globalCtx, sSpawnActorIds[thisv->type], spawnPos.x, spawnPos.y, spawnPos.z,
                                        actor->world.rot.x, actor->world.rot.y, actor->world.rot.z,
                                        (thisv->type == 4 && i == 0) ? 1 : sSpawnParams[thisv->type]);
        if (thisv->children[i] != NULL) {
            thisv->childrenStates[i] = OBJMURE_CHILD_STATE_0;
            thisv->children[i]->room = actor->room;
        } else {
            thisv->childrenStates[i] = OBJMURE_CHILD_STATE_1;
            osSyncPrintf("warning 発生失敗 (%s %d)\n", "../z_obj_mure.c", 438);
        }
    }
}

void ObjMure_SpawnActors(ObjMure* thisv, GlobalContext* globalCtx) {
    switch (thisv->svNum) {
        case 0:
            ObjMure_SpawnActors0(thisv, globalCtx);
            break;
        case 1:
            ObjMure_SpawnActors1(thisv, globalCtx);
            break;
    }
}

void ObjMure_KillActorsImpl(ObjMure* thisv, GlobalContext* globalCtx) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        switch (thisv->childrenStates[i]) {
            case OBJMURE_CHILD_STATE_1:
                thisv->children[i] = NULL;
                break;
            case OBJMURE_CHILD_STATE_2:
                if (thisv->children[i] != NULL) {
                    Actor_Kill(thisv->children[i]);
                    thisv->children[i] = NULL;
                }
                break;
            default:
                if (thisv->children[i] != NULL) {
                    if (Actor_HasParent(thisv->children[i], globalCtx)) {
                        thisv->children[i] = NULL;
                    } else {
                        Actor_Kill(thisv->children[i]);
                        thisv->children[i] = NULL;
                    }
                }
                break;
        }
    }
}

void ObjMure_KillActors(ObjMure* thisv, GlobalContext* globalCtx) {
    ObjMure_KillActorsImpl(thisv, globalCtx);
}

void ObjMure_CheckChildren(ObjMure* thisv, GlobalContext* globalCtx) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            if (thisv->childrenStates[i] == OBJMURE_CHILD_STATE_0) {
                if (thisv->children[i]->update != NULL) {
                    if (thisv->children[i]->flags & ACTOR_FLAG_ENKUSA_CUT) {
                        thisv->childrenStates[i] = OBJMURE_CHILD_STATE_2;
                    }
                } else {
                    thisv->childrenStates[i] = OBJMURE_CHILD_STATE_1;
                    thisv->children[i] = NULL;
                }
            } else if (thisv->childrenStates[i] == OBJMURE_CHILD_STATE_2 && thisv->children[i]->update == NULL) {
                thisv->childrenStates[i] = OBJMURE_CHILD_STATE_1;
                thisv->children[i] = NULL;
            }
        }
    }
}

void ObjMure_InitialAction(ObjMure* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = ObjMure_CulledState;
}

void ObjMure_CulledState(ObjMure* thisv, GlobalContext* globalCtx) {
    if (fabsf(thisv->actor.projectedPos.z) < sZClip[thisv->type] || CVar_GetS32("gDisableDrawDistance", 0) != 0) {
        thisv->actionFunc = ObjMure_ActiveState;
        thisv->actor.flags |= ACTOR_FLAG_4;
        ObjMure_SpawnActors(thisv, globalCtx);
    }
}

void ObjMure_SetFollowTargets(ObjMure* thisv, f32 randMax) {
    s32 index;
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    s32 i;

    for (i = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            thisv->children[i]->child = NULL;
            if (Rand_ZeroOne() <= randMax) {
                index = Rand_ZeroOne() * (maxChildren - 0.5f);
                if (i != index) {
                    thisv->children[i]->child = thisv->children[index];
                }
            }
        }
    }
}

/**
 * Selects a child that will follow after the player
 * `idx1` is the index + 1 of the child that will follow the player. If `idx1` is zero, no actor will follow the player
 */
void ObjMure_SetChildToFollowPlayer(ObjMure* thisv, s32 idx1) {
    s32 maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    s32 i;
    s32 i2;
    s32 j;

    for (i = 0, i2 = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            if (i2 < idx1) {
                i2++;
                thisv->children[i]->child = thisv->children[i];
                for (j = 0; j < maxChildren; j++) {
                    if (i != j && thisv->children[j]->child == thisv->children[i]) {
                        thisv->children[j]->child = NULL;
                    }
                }
            } else if (thisv->children[i]->child == thisv->children[i]) {
                thisv->children[i]->child = NULL;
            }
        }
    }
}

// Fish, Bugs
void ObjMure_GroupBehavior0(ObjMure* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_1A4 <= 0) {
        if (thisv->unk_1A6) {
            thisv->unk_1A6 = false;
            ObjMure_SetFollowTargets(thisv, (Rand_ZeroOne() * 0.5f) + 0.1f);
            if (thisv->actor.xzDistToPlayer < 60.0f) {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            thisv->unk_1A6 = true;
            if (thisv->actor.xzDistToPlayer < 60.0f) {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(thisv, (Rand_ZeroOne() * 0.2f) + 0.8f);
            } else {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
                ObjMure_SetFollowTargets(thisv, (Rand_ZeroOne() * 0.2f) + 0.6f);
            }
        }
    }
    if (thisv->actor.xzDistToPlayer < 120.0f) {
        thisv->unk_1A8++;
    } else {
        thisv->unk_1A8 = 0;
    }
    if (thisv->unk_1A8 >= 80) {
        ObjMure_SetChildToFollowPlayer(thisv, 1);
    } else {
        ObjMure_SetChildToFollowPlayer(thisv, 0);
    }
}

// Butterflies
void ObjMure_GroupBehavior1(ObjMure* thisv, GlobalContext* globalCtx) {
    s32 maxChildren;
    s32 i;

    if (thisv->unk_1A4 <= 0) {
        if (thisv->unk_1A6) {
            thisv->unk_1A6 = false;
            ObjMure_SetFollowTargets(thisv, Rand_ZeroOne() * 0.2f);
            if (thisv->actor.xzDistToPlayer < 60.0f) {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 5.5f) + 4;
            } else {
                thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 40.5f) + 4;
            }
        } else {
            thisv->unk_1A6 = true;
            ObjMure_SetFollowTargets(thisv, Rand_ZeroOne() * 0.7f);
            thisv->unk_1A4 = (s16)(Rand_ZeroOne() * 10.5f) + 4;
        }
    }

    maxChildren = ObjMure_GetMaxChildSpawns(thisv);
    for (i = 0; i < maxChildren; i++) {
        if (thisv->children[i] != NULL) {
            if (thisv->children[i]->child != NULL && thisv->children[i]->child->update == NULL) {
                thisv->children[i]->child = NULL;
            }
        }
    }
}

static ObjMureActionFunc sTypeGroupBehaviorFunc[] = {
    NULL, NULL, ObjMure_GroupBehavior0, ObjMure_GroupBehavior0, ObjMure_GroupBehavior1,
};

void ObjMure_ActiveState(ObjMure* thisv, GlobalContext* globalCtx) {
    ObjMure_CheckChildren(thisv, globalCtx);
    if (sZClip[thisv->type] + 40.0f <= fabsf(thisv->actor.projectedPos.z) &&
        CVar_GetS32("gDisableDrawDistance", 1) != 0) {
        thisv->actionFunc = ObjMure_CulledState;
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        ObjMure_KillActors(thisv, globalCtx);
    } else if (sTypeGroupBehaviorFunc[thisv->type] != NULL) {
        sTypeGroupBehaviorFunc[thisv->type](thisv, globalCtx);
    }
}

void ObjMure_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjMure* thisv = (ObjMure*)thisx;

    if (thisv->unk_1A4 > 0) {
        thisv->unk_1A4--;
    }
    thisv->actionFunc(thisv, globalCtx);
}
