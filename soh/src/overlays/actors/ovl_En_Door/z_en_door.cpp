/*
 * File: z_en_door.c
 * Overlay: ovl_En_Door
 * Description: Doors with handles
 */

#include "z_en_door.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"
#include "objects/object_haka_door/object_haka_door.h"

#define FLAGS ACTOR_FLAG_4

#define DOOR_AJAR_SLAM_RANGE 120.0f
#define DOOR_AJAR_OPEN_RANGE (2 * DOOR_AJAR_SLAM_RANGE)

#define DOOR_CHECK_RANGE 40.0f

void EnDoor_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDoor_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDoor_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDoor_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDoor_SetupType(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_Idle(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_WaitForCheck(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_Check(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_AjarWait(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_AjarOpen(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_AjarClose(EnDoor* thisv, GlobalContext* globalCtx);
void EnDoor_Open(EnDoor* thisv, GlobalContext* globalCtx);

const ActorInit En_Door_InitVars = {
    ACTOR_EN_DOOR,
    ACTORCAT_DOOR,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnDoor),
    (ActorFunc)EnDoor_Init,
    (ActorFunc)EnDoor_Destroy,
    (ActorFunc)EnDoor_Update,
    (ActorFunc)EnDoor_Draw,
    NULL,
};

/**
 * Controls which object and display lists to use in a given scene
 */
static EnDoorInfo sDoorInfo[] = {
    { SCENE_HIDAN, 1, OBJECT_HIDAN_OBJECTS },
    { SCENE_MIZUSIN, 2, OBJECT_MIZU_OBJECTS },
    { SCENE_HAKADAN, 3, OBJECT_HAKA_DOOR },
    { SCENE_HAKADANCH, 3, OBJECT_HAKA_DOOR },
    { SCENE_BMORI1, 0, OBJECT_GAMEPLAY_KEEP }, // Hacky fix, but behavior same as console.
    // KEEP objects should remain last and in thisv order
    { -1, 0, OBJECT_GAMEPLAY_KEEP },
    { -1, 4, OBJECT_GAMEPLAY_FIELD_KEEP },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_STOP),
};

static AnimationHeader* D_809FCECC[] = { &gDoor3Anim, &gDoor1Anim, &gDoor4Anim, &gDoor2Anim };

static u8 sDoorAnimOpenFrames[] = { 25, 25, 25, 25 };

static u8 sDoorAnimCloseFrames[] = { 60, 70, 60, 70 };

static Gfx* D_809FCEE4[5][2] = {
    { gDoorLeftDL, gDoorRightDL },
    { gFireTempleDoorWithHandleFrontDL, gFireTempleDoorWithHandleBackDL },
    { gWaterTempleDoorLeftDL, gWaterTempleDoorRightDL },
    { object_haka_door_DL_0013B8, object_haka_door_DL_001420 },
    { gFieldDoor1DL, gFieldDoor2DL },
};

void EnDoor_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnDoor* thisv = (EnDoor*)thisx;
    EnDoorInfo* objectInfo;
    s32 i;
    s32 objBankIndex;
    f32 xOffset;
    f32 zOffset;

    objectInfo = &sDoorInfo[0];
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDoorSkel, &gDoor3Anim, thisv->jointTable, thisv->morphTable, 5);
    for (i = 0; i < ARRAY_COUNT(sDoorInfo) - 2; i++, objectInfo++) {
        if (globalCtx->sceneNum == objectInfo->sceneNum) {
            break;
        }
    }

    // Due to Object_GetIndex always returning 0, doors always use the OBJECT_GAMEPLAY_FIELD_KEEP door.
    if (i >= ARRAY_COUNT(sDoorInfo) - 2 && Object_GetIndex(&globalCtx->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP) >= 0) {
        objectInfo++;
    }

    thisv->dListIndex = objectInfo->dListIndex;
    objBankIndex = Object_GetIndex(&globalCtx->objectCtx, objectInfo->objectId);
    if (objBankIndex < 0) {
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->requiredObjBankIndex = objBankIndex;
    thisv->dListIndex = objectInfo->dListIndex;
    if (thisv->actor.objBankIndex == thisv->requiredObjBankIndex) {
        EnDoor_SetupType(thisv, globalCtx);
    } else {
        thisv->actionFunc = EnDoor_SetupType;
    }

    // Double doors
    if (thisv->actor.params & 0x40) {
        EnDoor* other;

        xOffset = Math_CosS(thisv->actor.shape.rot.y) * 30.0f;
        zOffset = Math_SinS(thisv->actor.shape.rot.y) * 30.0f;
        other = (EnDoor*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_DOOR,
                                            thisv->actor.world.pos.x + xOffset, thisv->actor.world.pos.y,
                                            thisv->actor.world.pos.z - zOffset, 0, thisv->actor.shape.rot.y + 0x8000, 0,
                                            thisv->actor.params & ~0x40);
        if (other != NULL) {
            other->unk_192 = 1;
        }
        thisv->actor.world.pos.x -= xOffset;
        thisv->actor.world.pos.z += zOffset;
    }
    Actor_SetFocus(&thisv->actor, 70.0f);
}

void EnDoor_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    TransitionActorEntry* transitionEntry;
    EnDoor* thisv = (EnDoor*)thisx;

    transitionEntry = &globalCtx->transiActorCtx.list[(u16)thisv->actor.params >> 0xA];
    if (transitionEntry->id < 0) {
        transitionEntry->id = -transitionEntry->id;
    }
}

void EnDoor_SetupType(EnDoor* thisv, GlobalContext* globalCtx) {
    s32 doorType;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->requiredObjBankIndex)) {
        doorType = thisv->actor.params >> 7 & 7;
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        thisv->actor.objBankIndex = thisv->requiredObjBankIndex;
        thisv->actionFunc = EnDoor_Idle;
        if (doorType == DOOR_EVENING) {
            doorType =
                (gSaveContext.dayTime > 0xC000 && gSaveContext.dayTime < 0xE000) ? DOOR_SCENEEXIT : DOOR_CHECKABLE;
        }
        thisv->actor.world.rot.y = 0x0000;
        if (doorType == DOOR_LOCKED) {
            if (!Flags_GetSwitch(globalCtx, thisv->actor.params & 0x3F)) {
                thisv->lockTimer = 10;
            }
        } else if (doorType == DOOR_AJAR) {
            if (Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) > DOOR_AJAR_SLAM_RANGE) {
                thisv->actionFunc = EnDoor_AjarWait;
                thisv->actor.world.rot.y = -0x1800;
            }
        } else if (doorType == DOOR_CHECKABLE) {
            thisv->actor.textId = (thisv->actor.params & 0x3F) + 0x0200;
            if (thisv->actor.textId == 0x0229 && !(gSaveContext.eventChkInf[1] & 0x10)) {
                // Talon's house door. If Talon has not been woken up at Hyrule Castle
                // thisv door should be openable at any time of day. Note that there is no
                // check for time of day as the scene setup for Lon Lon merely initializes
                // the door with a different text id between day and night setups
                doorType = DOOR_SCENEEXIT;
            } else {
                thisv->actionFunc = EnDoor_WaitForCheck;
                thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_27;
            }
        }
        // Replace the door type it was loaded with by the new type
        thisv->actor.params = (thisv->actor.params & ~0x380) | (doorType << 7);
    }
}

void EnDoor_Idle(EnDoor* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 doorType;
    Vec3f playerPosRelToDoor;
    s16 phi_v0;

    doorType = thisv->actor.params >> 7 & 7;
    func_8002DBD0(&thisv->actor, &playerPosRelToDoor, &player->actor.world.pos);
    if (thisv->playerIsOpening != 0) {
        thisv->actionFunc = EnDoor_Open;
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, D_809FCECC[thisv->animStyle],
                                   (player->stateFlags1 & 0x8000000) ? 0.75f : 1.5f);
        if (thisv->lockTimer != 0) {
            gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex]--;
            Flags_SetSwitch(globalCtx, thisv->actor.params & 0x3F);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
        }
    } else if (!Player_InCsMode(globalCtx)) {
        if (fabsf(playerPosRelToDoor.y) < 20.0f && fabsf(playerPosRelToDoor.x) < 20.0f &&
            fabsf(playerPosRelToDoor.z) < 50.0f) {
            phi_v0 = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
            if (playerPosRelToDoor.z > 0.0f) {
                phi_v0 = 0x8000 - phi_v0;
            }
            if (ABS(phi_v0) < 0x3000) {
                if (thisv->lockTimer != 0) {
                    if (gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] <= 0) {
                        Player* player2 = GET_PLAYER(globalCtx);

                        player2->naviTextId = -0x203;
                        return;
                    } else {
                        player->doorTimer = 10;
                    }
                }
                player->doorType = (doorType == DOOR_AJAR) ? PLAYER_DOORTYPE_AJAR : PLAYER_DOORTYPE_HANDLE;
                player->doorDirection = (playerPosRelToDoor.z >= 0.0f) ? 1.0f : -1.0f;
                player->doorActor = &thisv->actor;
            }
        } else if (doorType == DOOR_AJAR && thisv->actor.xzDistToPlayer > DOOR_AJAR_OPEN_RANGE) {
            thisv->actionFunc = EnDoor_AjarOpen;
        }
    }
}

void EnDoor_WaitForCheck(EnDoor* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDoor_Check;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, DOOR_CHECK_RANGE);
    }
}

void EnDoor_Check(EnDoor* thisv, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDoor_WaitForCheck;
    }
}

void EnDoor_AjarWait(EnDoor* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        thisv->actionFunc = EnDoor_AjarClose;
    }
}

void EnDoor_AjarOpen(EnDoor* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        thisv->actionFunc = EnDoor_AjarClose;
    } else if (Math_ScaledStepToS(&thisv->actor.world.rot.y, -0x1800, 0x100)) {
        thisv->actionFunc = EnDoor_AjarWait;
    }
}

void EnDoor_AjarClose(EnDoor* thisv, GlobalContext* globalCtx) {
    if (Math_ScaledStepToS(&thisv->actor.world.rot.y, 0, 0x700)) {
        thisv->actionFunc = EnDoor_Idle;
    }
}

void EnDoor_Open(EnDoor* thisv, GlobalContext* globalCtx) {
    s32 i;
    s32 numEffects;

    if (DECR(thisv->lockTimer) == 0) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->actionFunc = EnDoor_Idle;
            thisv->playerIsOpening = 0;
        } else if (Animation_OnFrame(&thisv->skelAnime, sDoorAnimOpenFrames[thisv->animStyle])) {
            Audio_PlayActorSound2(&thisv->actor,
                                  (globalCtx->sceneNum == SCENE_HAKADAN || globalCtx->sceneNum == SCENE_HAKADANCH ||
                                   globalCtx->sceneNum == SCENE_HIDAN)
                                      ? NA_SE_EV_IRON_DOOR_OPEN
                                      : NA_SE_OC_DOOR_OPEN);
            if (thisv->skelAnime.playSpeed < 1.5f) {
                numEffects = (s32)(Rand_ZeroOne() * 30.0f) + 50;
                for (i = 0; i < numEffects; i++) {
                    EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, 60.0f, 100.0f, 50.0f, 0.15f);
                }
            }
        } else if (Animation_OnFrame(&thisv->skelAnime, sDoorAnimCloseFrames[thisv->animStyle])) {
            Audio_PlayActorSound2(&thisv->actor,
                                  (globalCtx->sceneNum == SCENE_HAKADAN || globalCtx->sceneNum == SCENE_HAKADANCH ||
                                   globalCtx->sceneNum == SCENE_HIDAN)
                                      ? NA_SE_EV_IRON_DOOR_CLOSE
                                      : NA_SE_EV_DOOR_CLOSE);
        }
    }
}

void EnDoor_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnDoor* thisv = (EnDoor*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

s32 EnDoor_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    s32 pad;
    TransitionActorEntry* transitionEntry;
    Gfx** temp_a2;
    s32 pad2;
    s16 phi_v0_2;
    s32 phi_v0;
    EnDoor* thisv = (EnDoor*)thisx;

    if (limbIndex == 4) {
        temp_a2 = D_809FCEE4[thisv->dListIndex];
        transitionEntry = &globalCtx->transiActorCtx.list[(u16)thisv->actor.params >> 0xA];
        rot->z += thisv->actor.world.rot.y;
        if ((globalCtx->roomCtx.prevRoom.num >= 0) ||
            (transitionEntry->sides[0].room == transitionEntry->sides[1].room)) {
            phi_v0_2 = ((thisv->actor.shape.rot.y + thisv->skelAnime.jointTable[3].z) + rot->z) -
                       Math_Vec3f_Yaw(&globalCtx->view.eye, &thisv->actor.world.pos);
            *dList = (ABS(phi_v0_2) < 0x4000) ? temp_a2[0] : temp_a2[1];
        } else {
            phi_v0 = thisv->unk_192;
            if (transitionEntry->sides[0].room != thisv->actor.room) {
                phi_v0 ^= 1;
            }
            *dList = temp_a2[phi_v0];
        }
    }
    return false;
}

void EnDoor_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnDoor* thisv = (EnDoor*)thisx;

    if (thisv->actor.objBankIndex == thisv->requiredObjBankIndex) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_door.c", 910);

        func_80093D18(globalCtx->state.gfxCtx);
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnDoor_OverrideLimbDraw,
                          NULL, &thisv->actor);
        if (thisv->actor.world.rot.y != 0) {
            if (1) {}
            if (thisv->actor.world.rot.y > 0) {
                gSPDisplayList(POLY_OPA_DISP++, gDoorRightDL);
            } else {
                gSPDisplayList(POLY_OPA_DISP++, gDoorLeftDL);
            }
        }
        if (thisv->lockTimer != 0) {
            Actor_DrawDoorLock(globalCtx, thisv->lockTimer, DOORLOCK_NORMAL);
        }

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_door.c", 941);
    }
}
