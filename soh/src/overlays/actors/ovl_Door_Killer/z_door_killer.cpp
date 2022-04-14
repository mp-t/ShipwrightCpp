/*
 * File: z_door_killer.c
 * Overlay: ovl_Door_Killer
 * Description: Fake doors which attack player
 */

#include "z_door_killer.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"
#include "objects/object_haka_door/object_haka_door.h"
#include "objects/object_door_killer/object_door_killer.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0 */ DOOR_KILLER_DOOR,
    /* 1 */ DOOR_KILLER_RUBBLE_PIECE_1,
    /* 2 */ DOOR_KILLER_RUBBLE_PIECE_2,
    /* 3 */ DOOR_KILLER_RUBBLE_PIECE_3,
    /* 4 */ DOOR_KILLER_RUBBLE_PIECE_4
} DoorKillerBehaviour;

void DoorKiller_Init(Actor* thisx, GlobalContext* globalCtx);
void DoorKiller_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DoorKiller_Update(Actor* thisx, GlobalContext* globalCtx);
void DoorKiller_Wait(DoorKiller* thisv, GlobalContext* globalCtx);
void DoorKiller_SetProperties(DoorKiller* thisv, GlobalContext* globalCtx);
void DoorKiller_DrawDoor(Actor* thisx, GlobalContext* globalCtx);
void DoorKiller_DrawRubble(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Door_Killer_InitVars = {
    ACTOR_DOOR_KILLER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_DOOR_KILLER,
    sizeof(DoorKiller),
    (ActorFunc)DoorKiller_Init,
    (ActorFunc)DoorKiller_Destroy,
    (ActorFunc)DoorKiller_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x0001FFEE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 100 }, 100 },
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
    sJntSphItemsInit,
};

static DoorKillerTextureEntry sDoorTextures[4] = {
    { OBJECT_HIDAN_OBJECTS, gFireTempleDoorKillerTex },
    { OBJECT_MIZU_OBJECTS, object_mizu_objects_Tex_0035C0 },
    { OBJECT_HAKA_DOOR, object_haka_door_Tex_000000 },
    { OBJECT_GAMEPLAY_KEEP, gWoodenDoorTex },
};

void DoorKiller_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    f32 randF;
    DoorKiller* thisv = (DoorKiller*)thisx;
    s32 bankIndex;
    s32 i;

    // Look in the object bank for one of the four objects containing door textures
    bankIndex = -1;
    for (i = 0; bankIndex < 0; i++) {
        bankIndex = Object_GetIndex(&globalCtx->objectCtx, sDoorTextures[i].objectId);
        thisv->textureEntryIndex = i;
    }
    osSyncPrintf("bank_ID = %d\n", bankIndex);
    osSyncPrintf("status = %d\n", thisv->textureEntryIndex);
    thisv->doorObjBankIndex = bankIndex;
    thisv->texture = sDoorTextures[thisv->textureEntryIndex].texture;

    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->timer = 0;
    thisv->hasHitPlayerOrGround = 0;
    thisv->animStyle = 0;
    thisv->playerIsOpening = 0;

    switch ((u8)(thisv->actor.params & 0xFF)) {
        case DOOR_KILLER_DOOR:
            // `jointTable` is used for both the `jointTable` and `morphTable` args here. Because thisv actor doesn't
            // play any animations it does not cause problems, but it would need to be changed otherwise.
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_door_killer_Skel_001BC8, NULL, thisv->jointTable,
                               thisv->jointTable, 9);
            thisv->actionFunc = DoorKiller_SetProperties;
            DoorKiller_SetProperties(thisv, globalCtx);

            // manually set the overall rotation of the door
            thisv->jointTable[1].x = thisv->jointTable[1].z = 0x4000;

            // Set a cylinder collider to detect link attacks and larger sphere collider to detect explosions
            Collider_InitCylinder(globalCtx, &thisv->colliderCylinder);
            Collider_SetCylinder(globalCtx, &thisv->colliderCylinder, &thisv->actor, &sCylinderInit);
            Collider_InitJntSph(globalCtx, &thisv->colliderJntSph);
            Collider_SetJntSph(globalCtx, &thisv->colliderJntSph, &thisv->actor, &sJntSphInit, thisv->colliderJntSphItems);
            thisv->colliderJntSph.elements[0].dim.worldSphere.radius = 80;
            thisv->colliderJntSph.elements[0].dim.worldSphere.center.x = (s16)thisv->actor.world.pos.x;
            thisv->colliderJntSph.elements[0].dim.worldSphere.center.y = (s16)thisv->actor.world.pos.y + 50;
            thisv->colliderJntSph.elements[0].dim.worldSphere.center.z = (s16)thisv->actor.world.pos.z;

            // If tied to a switch flag and that switch flag is already set, kill the actor.
            if ((((thisv->actor.params >> 8) & 0x3F) != 0x3F) &&
                Flags_GetSwitch(globalCtx, ((thisv->actor.params >> 8) & 0x3F))) {
                Actor_Kill(&thisv->actor);
            }
            break;
        case DOOR_KILLER_RUBBLE_PIECE_1:
        case DOOR_KILLER_RUBBLE_PIECE_2:
        case DOOR_KILLER_RUBBLE_PIECE_3:
        case DOOR_KILLER_RUBBLE_PIECE_4:
            thisv->actionFunc = DoorKiller_SetProperties;
            DoorKiller_SetProperties(thisv, globalCtx);

            thisv->actor.gravity = -0.6f;
            thisv->actor.minVelocityY = -6.0f;

            // Random trajectories for rubble pieces
            randF = Rand_CenteredFloat(8.0f);
            thisv->actor.velocity.z = Rand_ZeroFloat(8.0f);
            thisv->actor.velocity.x = (Math_CosS(thisv->actor.world.rot.y) * randF) +
                                     (Math_SinS(thisv->actor.world.rot.y) * thisv->actor.velocity.z);
            thisv->actor.velocity.z = (-Math_SinS(thisv->actor.world.rot.y) * randF) +
                                     (Math_CosS(thisv->actor.world.rot.y) * thisv->actor.velocity.z);
            thisv->actor.velocity.y = Rand_ZeroFloat(4.0f) + 4.0f;

            // These are used as the x,y,z rotational velocities in DoorKiller_FallAsRubble
            thisv->actor.world.rot.x = Rand_CenteredFloat(0x1000);
            thisv->actor.world.rot.y = Rand_CenteredFloat(0x1000);
            thisv->actor.world.rot.z = Rand_CenteredFloat(0x1000);
            thisv->timer = 80;
            break;
    }
}

void DoorKiller_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;

    if ((thisx->params & 0xFF) == DOOR_KILLER_DOOR) {
        Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinder);
        Collider_DestroyJntSph(globalCtx, &thisv->colliderJntSph);
    }
}

void DoorKiller_SpawnRubble(Actor* thisx, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_KILLER, thisx->world.pos.x, thisx->world.pos.y + 9.0f,
                thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z,
                DOOR_KILLER_RUBBLE_PIECE_1);
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_KILLER, thisx->world.pos.x + 7.88f,
                thisx->world.pos.y + 39.8f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_2);
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_KILLER, thisx->world.pos.x - 15.86f,
                thisx->world.pos.y + 61.98f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_3);
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_KILLER, thisx->world.pos.x + 3.72f,
                thisx->world.pos.y + 85.1f, thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y,
                thisx->shape.rot.z, DOOR_KILLER_RUBBLE_PIECE_4);
}

/**
 * action function for the individual door pieces that spawn and fall down when the door is destroyed
 */
void DoorKiller_FallAsRubble(DoorKiller* thisv, GlobalContext* globalCtx) {
    thisv->actor.velocity.y += thisv->actor.gravity;
    if (thisv->actor.velocity.y < thisv->actor.minVelocityY) {
        thisv->actor.velocity.y = thisv->actor.minVelocityY;
    }

    thisv->actor.velocity.x *= 0.98f;
    thisv->actor.velocity.z *= 0.98f;

    // world.rot is repurposed to be the rotation velocity for the rubble pieces
    thisv->actor.shape.rot.x += thisv->actor.world.rot.x;
    thisv->actor.shape.rot.y += thisv->actor.world.rot.y;
    thisv->actor.shape.rot.z += thisv->actor.world.rot.z;

    if (thisv->timer != 0) {
        thisv->timer--;
    } else {
        Actor_Kill(&thisv->actor);
    }
    func_8002D7EC(&thisv->actor);
}

s32 DoorKiller_IsHit(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;
    if ((thisv->colliderCylinder.base.acFlags & 2) && (thisv->colliderCylinder.info.acHitInfo != NULL)) {
        return true;
    }
    return false;
}

void DoorKiller_SetAC(DoorKiller* thisv, GlobalContext* globalCtx) {
    Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCylinder);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCylinder.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderJntSph.base);
}

void DoorKiller_Die(DoorKiller* thisv, GlobalContext* globalCtx) {
    s32 switchFlag = (thisv->actor.params >> 8) & 0x3F;

    // Can set a switch flag on death based on params
    if (switchFlag != 0x3F) {
        Flags_SetSwitch(globalCtx, switchFlag);
    }
    Actor_Kill(&thisv->actor);
}

/**
 * After slamming on the floor, rise back upright
 */
void DoorKiller_RiseBackUp(DoorKiller* thisv, GlobalContext* globalCtx) {
    s32 i;
    s16 rotation;

    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->actionFunc = DoorKiller_Wait;
        thisv->timer = 16;
        DoorKiller_SetAC(thisv, globalCtx);
        return;
    }

    thisv->actor.shape.rot.x = (thisv->timer >= 8) ? (thisv->timer * 0x800) - 0x4000 : 0;

    if (thisv->timer >= 12) {
        rotation = (-thisv->timer * -500) - 8000;
    } else if (thisv->timer >= 8) {
        rotation = -2000;
    } else if (thisv->timer >= 5) {
        rotation = (thisv->timer * -500) + 2000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        thisv->jointTable[i].z = -rotation;
    }

    if (thisv->timer < 8) {
        rotation = Math_SinS(thisv->timer * 0x2000) * thisv->timer * 100.0f;
        for (i = 2; i < 9; i++) {
            thisv->jointTable[i].y = rotation;
        }
    }
}

/**
 * After wobbling, fall over and slam onto the floor, damaging the player if they are in the way. Uses manual distance
 * check for damaging player, not AT system.
 */
void DoorKiller_FallOver(DoorKiller* thisv, GlobalContext* globalCtx) {
    s32 i;
    s16 rotation;

    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->actionFunc = DoorKiller_RiseBackUp;
        thisv->timer = 16;
        return;
    }

    thisv->actor.shape.rot.x = (thisv->timer >= 4) ? 0x8000 + (-thisv->timer * 0x1000) : 0x4000;

    if (thisv->timer >= 6) {
        rotation = (-thisv->timer * -500) - 4000;
    } else if (thisv->timer >= 4) {
        rotation = -1000;
    } else if (thisv->timer >= 3) {
        rotation = (thisv->timer * -500) + 1000;
    } else {
        rotation = 0;
    }

    for (i = 2; i < 9; i++) {
        thisv->jointTable[i].z = rotation;
    }

    if (thisv->timer == 4) {
        // spawn 20 random dust particles just before slamming down
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 1.0f, 0.0f };
        Vec3f pos;
        s32 j;
        f32 randF;

        for (j = 0; j != 20; j++) {
            pos.y = 0.0f;
            randF = Rand_CenteredFloat(40.0f);
            pos.z = Rand_ZeroFloat(100.0f);
            pos.x = (Math_CosS(thisv->actor.world.rot.y) * randF) + (Math_SinS(thisv->actor.world.rot.y) * pos.z);
            pos.z = (-Math_SinS(thisv->actor.world.rot.y) * randF) + (Math_CosS(thisv->actor.world.rot.y) * pos.z);
            velocity.x = pos.x * 0.2f;
            velocity.z = pos.z * 0.2f;
            accel.x = -(velocity.x) * 0.1f;
            accel.z = -(velocity.z) * 0.1f;
            pos.x += thisv->actor.world.pos.x;
            pos.y += thisv->actor.world.pos.y;
            pos.z += thisv->actor.world.pos.z;
            func_8002865C(globalCtx, &pos, &velocity, &accel, 300, 30);
        }
    }
    if (!(thisv->hasHitPlayerOrGround & 1)) {
        Vec3f playerPosRelToDoor;
        Player* player = GET_PLAYER(globalCtx);
        func_8002DBD0(&thisv->actor, &playerPosRelToDoor, &player->actor.world.pos);
        if ((fabsf(playerPosRelToDoor.y) < 20.0f) && (fabsf(playerPosRelToDoor.x) < 20.0f) &&
            (playerPosRelToDoor.z < 100.0f) && (playerPosRelToDoor.z > 0.0f)) {
            thisv->hasHitPlayerOrGround |= 1;
            func_8002F6D4(globalCtx, &thisv->actor, 6.0f, thisv->actor.yawTowardsPlayer, 6.0f, 16);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KDOOR_HIT);
            func_8002F7DC(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    if (!(thisv->hasHitPlayerOrGround & 1) && (thisv->timer == 2)) {
        thisv->hasHitPlayerOrGround |= 1;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KDOOR_HIT_GND);
    }
}

/**
 * Wobble around, signifying the door is about to fall over. Does not set AC and so cannot be destroyed during thisv.
 */
void DoorKiller_Wobble(DoorKiller* thisv, GlobalContext* globalCtx) {
    s16 rotation;
    s32 i;

    if ((thisv->timer == 16) || (thisv->timer == 8)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_KDOOR_WAVE);
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->actionFunc = DoorKiller_FallOver;
        thisv->timer = 8;
        thisv->hasHitPlayerOrGround &= ~1;
        return;
    }

    rotation = Math_SinS(thisv->timer * 0x2000) * thisv->timer * 100.0f;
    for (i = 2; i < 9; i++) {
        thisv->jointTable[i].y = rotation;
    }
    rotation = (u16)(s32)(-Math_CosS(thisv->timer * 0x1000) * 1000.0f) + 1000;
    for (i = 2; i < 9; i++) {
        thisv->jointTable[i].z = rotation;
    }
}

/**
 * Idle while the player attempts to open the door and then begin to wobble
 */
void DoorKiller_WaitBeforeWobble(DoorKiller* thisv, GlobalContext* globalCtx) {
    if (thisv->timer > 0) {
        thisv->timer--;
    } else {
        thisv->timer = 16;
        thisv->actionFunc = DoorKiller_Wobble;
    }
}

void DoorKiller_Wait(DoorKiller* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f playerPosRelToDoor;
    s16 angleToFacingPlayer;

    func_8002DBD0(&thisv->actor, &playerPosRelToDoor, &player->actor.world.pos);

    // playerIsOpening is set by player
    if (thisv->playerIsOpening) {
        thisv->actionFunc = DoorKiller_WaitBeforeWobble;
        thisv->timer = 10;
        thisv->playerIsOpening = 0;
        return;
    }

    if (DoorKiller_IsHit(&thisv->actor, globalCtx)) {
        // AC cylinder: wobble if hit by most weapons, die if hit by explosives or hammer
        if ((thisv->colliderCylinder.info.acHitInfo->toucher.dmgFlags & 0x1FFA6) != 0) {
            thisv->timer = 16;
            thisv->actionFunc = DoorKiller_Wobble;
        } else if ((thisv->colliderCylinder.info.acHitInfo->toucher.dmgFlags & 0x48) != 0) {
            DoorKiller_SpawnRubble(&thisv->actor, globalCtx);
            thisv->actionFunc = DoorKiller_Die;
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
        }
    } else if (Actor_GetCollidedExplosive(globalCtx, &thisv->colliderJntSph.base) != NULL) {
        // AC sphere: die if hit by explosive
        DoorKiller_SpawnRubble(&thisv->actor, globalCtx);
        thisv->actionFunc = DoorKiller_Die;
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EN_KDOOR_BREAK);
    } else if (!Player_InCsMode(globalCtx) && (fabsf(playerPosRelToDoor.y) < 20.0f) &&
               (fabsf(playerPosRelToDoor.x) < 20.0f) && (playerPosRelToDoor.z < 50.0f) &&
               (playerPosRelToDoor.z > 0.0f)) {
        // Set player properties to make the door openable if within range
        angleToFacingPlayer = player->actor.shape.rot.y - thisv->actor.shape.rot.y;
        if (playerPosRelToDoor.z > 0.0f) {
            angleToFacingPlayer = 0x8000 - angleToFacingPlayer;
        }
        if (ABS(angleToFacingPlayer) < 0x3000) {
            player->doorType = PLAYER_DOORTYPE_FAKE;
            player->doorDirection = (playerPosRelToDoor.z >= 0.0f) ? 1.0f : -1.0f;
            player->doorActor = &thisv->actor;
        }
    }

    DoorKiller_SetAC(thisv, globalCtx);
}

/**
 * Grabs the virtual address of the texture from the relevant door object
 */
void DoorKiller_UpdateTexture(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->doorObjBankIndex].segment);
    thisv->texture = SEGMENTED_TO_VIRTUAL(thisv->texture);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisx->objBankIndex].segment);
}

/**
 * Gets the correct door texture, defines the appropriate draw fucntion and action function based on type behaviour
 * (door or rubble).
 */
void DoorKiller_SetProperties(DoorKiller* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->doorObjBankIndex)) {
        DoorKiller_UpdateTexture(&thisv->actor, globalCtx);
        switch (thisv->actor.params & 0xFF) {
            case DOOR_KILLER_DOOR:
                thisv->actionFunc = DoorKiller_Wait;
                thisv->actor.draw = DoorKiller_DrawDoor;
                break;
            case DOOR_KILLER_RUBBLE_PIECE_1:
            case DOOR_KILLER_RUBBLE_PIECE_2:
            case DOOR_KILLER_RUBBLE_PIECE_3:
            case DOOR_KILLER_RUBBLE_PIECE_4:
                thisv->actionFunc = DoorKiller_FallAsRubble;
                thisv->actor.draw = DoorKiller_DrawRubble;
                break;
        }
    }
}

void DoorKiller_Update(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void DoorKiller_SetTexture(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;
    void* doorTexture = thisv->texture;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_door_killer.c", 883);
    gSPSegment(POLY_OPA_DISP++, 0x08, doorTexture);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_door_killer.c", 885);
}

void DoorKiller_DrawDoor(Actor* thisx, GlobalContext* globalCtx) {
    DoorKiller* thisv = (DoorKiller*)thisx;

    func_800943C8(globalCtx->state.gfxCtx);
    DoorKiller_SetTexture(&thisv->actor, globalCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, NULL);
}

void DoorKiller_DrawRubble(Actor* thisx, GlobalContext* globalCtx) {
    static Gfx* dLists[] = { object_door_killer_DL_001250, object_door_killer_DL_001550, object_door_killer_DL_0017B8,
                             object_door_killer_DL_001A58 };
    s32 rubblePieceIndex = (thisx->params & 0xFF) - 1;
    DoorKiller* thisv = (DoorKiller*)thisx;

    if ((thisv->timer >= 20) || ((thisv->timer & 1) == 0)) {
        DoorKiller_SetTexture(thisx, globalCtx);
        Gfx_DrawDListOpa(globalCtx, dLists[rubblePieceIndex]);
    }
}
