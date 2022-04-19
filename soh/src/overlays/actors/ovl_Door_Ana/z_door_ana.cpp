/*
 * File: z_door_ana.c
 * Overlay: ovl_Door_Ana
 * Description: Grottos Entrances/Exits
 */

#include "z_door_ana.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS ACTOR_FLAG_25

void DoorAna_Init(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Update(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Draw(Actor* thisx, GlobalContext* globalCtx);

void DoorAna_WaitClosed(DoorAna* thisv, GlobalContext* globalCtx);
void DoorAna_WaitOpen(DoorAna* thisv, GlobalContext* globalCtx);
void DoorAna_GrabPlayer(DoorAna* thisv, GlobalContext* globalCtx);

ActorInit Door_Ana_InitVars = {
    ACTOR_DOOR_ANA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(DoorAna),
    (ActorFunc)DoorAna_Init,
    (ActorFunc)DoorAna_Destroy,
    (ActorFunc)DoorAna_Update,
    (ActorFunc)DoorAna_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 50, 10, 0, { 0 } },
};

// array of entrance table entries to grotto destinations
static s16 entrances[] = {
    0x036D, 0x003F, 0x0598, 0x059C, 0x05A0, 0x05A4, 0x05A8, 0x05AC,
    0x05B0, 0x05B4, 0x05B8, 0x05BC, 0x05C0, 0x05C4, 0x05FC,
};

void DoorAna_SetupAction(DoorAna* thisv, DoorAnaActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void DoorAna_Init(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* thisv = (DoorAna*)thisx;

    thisv->actor.shape.rot.z = 0;
    thisv->actor.shape.rot.y = thisv->actor.shape.rot.z;
    // init block for grottos that are initially "hidden" (require explosives/hammer/song of storms to open)
    if ((thisv->actor.params & 0x300) != 0) {
        // only allocate collider for grottos that need bombing/hammering open
        if ((thisv->actor.params & 0x200) != 0) {
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        } else {
            thisv->actor.flags |= ACTOR_FLAG_4;
        }
        Actor_SetScale(&thisv->actor, 0);
        DoorAna_SetupAction(thisv, DoorAna_WaitClosed);
    } else {
        DoorAna_SetupAction(thisv, DoorAna_WaitOpen);
    }
    thisv->actor.targetMode = 0;
}

void DoorAna_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* thisv = (DoorAna*)thisx;

    // free collider if it has one
    if ((thisv->actor.params & 0x200) != 0) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

// update routine for grottos that are currently "hidden"/unopened
void DoorAna_WaitClosed(DoorAna* thisv, GlobalContext* globalCtx) {
    u32 openGrotto = false;

    if (!(thisv->actor.params & 0x200)) {
        // opening with song of storms
        if (thisv->actor.xyzDistToPlayerSq < 40000.0f && Flags_GetEnv(globalCtx, 5)) {
            openGrotto = true;
            thisv->actor.flags &= ~ACTOR_FLAG_4;
        }
    } else {
        // bombing/hammering open a grotto
        if (thisv->collider.base.acFlags & AC_HIT) {
            openGrotto = true;
            Collider_DestroyCylinder(globalCtx, &thisv->collider);
        } else {
            Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
    // open the grotto
    if (openGrotto) {
        thisv->actor.params &= ~0x0300;
        DoorAna_SetupAction(thisv, DoorAna_WaitOpen);
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    func_8002F5F0(&thisv->actor, globalCtx);
}

// update routine for grottos that are open
void DoorAna_WaitOpen(DoorAna* thisv, GlobalContext* globalCtx) {
    Player* player;
    s32 destinationIdx;

    player = GET_PLAYER(globalCtx);
    if (Math_StepToF(&thisv->actor.scale.x, 0.01f, 0.001f)) {
        if ((thisv->actor.targetMode != 0) && (globalCtx->sceneLoadFlag == 0) && (player->stateFlags1 & 0x80000000) &&
            (player->unk_84F == 0)) {
            destinationIdx = ((thisv->actor.params >> 0xC) & 7) - 1;
            Gameplay_SetupRespawnPoint(globalCtx, RESPAWN_MODE_RETURN, 0x4FF);
            gSaveContext.respawn[RESPAWN_MODE_RETURN].pos.y = thisv->actor.world.pos.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].yaw = thisv->actor.home.rot.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].data = thisv->actor.params & 0xFFFF;
            if (destinationIdx < 0) {
                destinationIdx = thisv->actor.home.rot.z + 1;
            }
            globalCtx->nextEntranceIndex = entrances[destinationIdx];
            DoorAna_SetupAction(thisv, DoorAna_GrabPlayer);
        } else {
            if (!Player_InCsMode(globalCtx) && !(player->stateFlags1 & 0x8800000) &&
                thisv->actor.xzDistToPlayer <= 15.0f && -50.0f <= thisv->actor.yDistToPlayer &&
                thisv->actor.yDistToPlayer <= 15.0f) {
                player->stateFlags1 |= 0x80000000;
                thisv->actor.targetMode = 1;
            } else {
                thisv->actor.targetMode = 0;
            }
        }
    }
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
}

// update function for after the player has triggered the grotto
void DoorAna_GrabPlayer(DoorAna* thisv, GlobalContext* globalCtx) {
    Player* player;

    if (thisv->actor.yDistToPlayer <= 0.0f && 15.0f < thisv->actor.xzDistToPlayer) {
        player = GET_PLAYER(globalCtx);
        player->actor.world.pos.x = Math_SinS(thisv->actor.yawTowardsPlayer) * 15.0f + thisv->actor.world.pos.x;
        player->actor.world.pos.z = Math_CosS(thisv->actor.yawTowardsPlayer) * 15.0f + thisv->actor.world.pos.z;
    }
}

void DoorAna_Update(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* thisv = (DoorAna*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    // changes the grottos facing angle based on camera angle
    thisv->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000;
}

void DoorAna_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_door_ana.c", 440);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_door_ana.c", 446),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGrottoDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_door_ana.c", 449);
}
