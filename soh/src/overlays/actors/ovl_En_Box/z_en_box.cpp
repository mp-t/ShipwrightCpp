#include "z_en_box.h"
#include "objects/object_box/object_box.h"

#define FLAGS 0

// movement flags

/*
set on init unless treasure flag is set
if clear, chest moves (Actor_MoveForward) (falls, likely)
ends up cleared from SWITCH_FLAG_FALL types when switch flag is set
*/
#define ENBOX_MOVE_IMMOBILE (1 << 0)
/*
set in the logic for SWITCH_FLAG_FALL types
otherwise unused
*/
#define ENBOX_MOVE_UNUSED (1 << 1)
/*
set with 50% chance on init for SWITCH_FLAG_FALL types
only used for SWITCH_FLAG_FALL types
ends up "blinking" (set/clear every frame) once switch flag is set,
if some collision-related condition (?) is met
only used for signum of z rotation
*/
#define ENBOX_MOVE_FALL_ANGLE_SIDE (1 << 2)
/*
when set, gets cleared next EnBox_Update call and clip to the floor
*/
#define ENBOX_MOVE_STICK_TO_GROUND (1 << 4)

typedef enum {
    ENBOX_STATE_0, // waiting for player near / player available / player ? (IDLE)
    ENBOX_STATE_1, // used only temporarily, maybe "player is ready" ?
    ENBOX_STATE_2  // waiting for something message context-related
} EnBoxStateUnk1FB;

void EnBox_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBox_FallOnSwitchFlag(EnBox*, GlobalContext*);
void func_809C9700(EnBox*, GlobalContext*);
void EnBox_AppearOnSwitchFlag(EnBox*, GlobalContext*);
void EnBox_AppearOnRoomClear(EnBox*, GlobalContext*);
void EnBox_AppearInit(EnBox*, GlobalContext*);
void EnBox_AppearAnimation(EnBox*, GlobalContext*);
void EnBox_WaitOpen(EnBox*, GlobalContext*);
void EnBox_Open(EnBox*, GlobalContext*);

const ActorInit En_Box_InitVars = {
    ACTOR_EN_BOX,
    ACTORCAT_CHEST,
    FLAGS,
    OBJECT_BOX,
    sizeof(EnBox),
    (ActorFunc)EnBox_Init,
    (ActorFunc)EnBox_Destroy,
    (ActorFunc)EnBox_Update,
    (ActorFunc)EnBox_Draw,
    NULL,
};

static AnimationHeader* sAnimations[4] = { &gTreasureChestAnim_00024C, &gTreasureChestAnim_000128,
                                           &gTreasureChestAnim_00043C, &gTreasureChestAnim_00043C };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_STOP),
};

static UNK_TYPE sUnused;

void EnBox_SetupAction(EnBox* thisv, EnBoxActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBox_ClipToGround(EnBox* thisv, GlobalContext* globalCtx) {
    f32 newY;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f pos;

    pos = thisv->dyna.actor.world.pos;
    pos.y += 1.0f;
    newY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &poly, &bgId, &thisv->dyna.actor, &pos);
    if (newY != BGCHECK_Y_MIN) {
        thisv->dyna.actor.world.pos.y = newY;
    }
}

void EnBox_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBox* thisv = (EnBox*)thisx;
    AnimationHeader* anim;
    CollisionHeader* colHeader;
    f32 animFrameStart;
    f32 endFrame;

    animFrameStart = 0.0f;
    anim = sAnimations[((void)0, gSaveContext.linkAge)];
    colHeader = NULL;
    endFrame = Animation_GetLastFrame(anim);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gTreasureChestCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    func_8003ECA8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);

    thisv->movementFlags = 0;
    thisv->type = thisx->params >> 12 & 0xF;
    thisv->iceSmokeTimer = 0;
    thisv->unk_1FB = ENBOX_STATE_0;
    thisv->dyna.actor.gravity = -5.5f;
    thisv->switchFlag = thisv->dyna.actor.world.rot.z;
    thisv->dyna.actor.minVelocityY = -50.0f;

    if (globalCtx) {} // helps the compiler store globalCtx2 into s1

    if (Flags_GetTreasure(globalCtx, thisv->dyna.actor.params & 0x1F)) {
        thisv->alpha = 255;
        thisv->iceSmokeTimer = 100;
        EnBox_SetupAction(thisv, EnBox_Open);
        thisv->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
        animFrameStart = endFrame;
    } else if ((thisv->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG || thisv->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) &&
               !Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        if (Rand_ZeroOne() < 0.5f) {
            thisv->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        thisv->unk_1A8 = -12;
        EnBox_SetupAction(thisv, EnBox_FallOnSwitchFlag);
        thisv->alpha = 0;
        thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if ((thisv->type == ENBOX_TYPE_ROOM_CLEAR_BIG || thisv->type == ENBOX_TYPE_ROOM_CLEAR_SMALL) &&
               !Flags_GetClear(globalCtx, thisv->dyna.actor.room)) {
        EnBox_SetupAction(thisv, EnBox_AppearOnRoomClear);
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - 50.0f;
        thisv->alpha = 0;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if (thisv->type == ENBOX_TYPE_9 || thisv->type == ENBOX_TYPE_10) {
        EnBox_SetupAction(thisv, func_809C9700);
        thisv->dyna.actor.flags |= ACTOR_FLAG_25;
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - 50.0f;
        thisv->alpha = 0;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if (thisv->type == ENBOX_TYPE_SWITCH_FLAG_BIG && !Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        EnBox_SetupAction(thisv, EnBox_AppearOnSwitchFlag);
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - 50.0f;
        thisv->alpha = 0;
        thisv->dyna.actor.flags |= ACTOR_FLAG_4;
    } else {
        if (thisv->type == ENBOX_TYPE_4 || thisv->type == ENBOX_TYPE_6) {
            thisv->dyna.actor.flags |= ACTOR_FLAG_7;
        }
        EnBox_SetupAction(thisv, EnBox_WaitOpen);
        thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
        thisv->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
    }

    thisv->dyna.actor.world.rot.y += 0x8000;
    thisv->dyna.actor.home.rot.z = thisv->dyna.actor.world.rot.z = thisv->dyna.actor.shape.rot.z = 0;

    SkelAnime_Init(globalCtx, &thisv->skelanime, &gTreasureChestSkel, anim, thisv->jointTable, thisv->morphTable, 5);
    Animation_Change(&thisv->skelanime, anim, 1.5f, animFrameStart, endFrame, ANIMMODE_ONCE, 0.0f);

    switch (thisv->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_SetScale(&thisv->dyna.actor, 0.005f);
            Actor_SetFocus(&thisv->dyna.actor, 20.0f);
            break;
        default:
            Actor_SetScale(&thisv->dyna.actor, 0.01f);
            Actor_SetFocus(&thisv->dyna.actor, 40.0f);
    }
}

void EnBox_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* thisv = (EnBox*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void EnBox_RandomDustKinematic(EnBox* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    f32 randomRadius = Rand_ZeroOne() * 25.0f;
    s16 randomAngle = Rand_ZeroOne() * 0x10000;

    *pos = thisv->dyna.actor.world.pos;
    pos->x += Math_SinS(randomAngle) * randomRadius;
    pos->z += Math_CosS(randomAngle) * randomRadius;

    velocity->y = 1.0f;
    velocity->x = Math_SinS(randomAngle);
    velocity->z = Math_CosS(randomAngle);

    accel->x = 0.0f;
    accel->y = 0.0f;
    accel->z = 0.0f;
}

/**
 * Spawns dust randomly around the chest when the chest hits the ground after falling (FALL types)
 */
void EnBox_SpawnDust(EnBox* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        EnBox_RandomDustKinematic(thisv, &pos, &velocity, &accel);
        func_8002873C(globalCtx, &pos, &velocity, &accel, 100, 30, 15);
    }
}

/**
 * Used while the chest is falling (FALL types)
 */
void EnBox_Fall(EnBox* thisv, GlobalContext* globalCtx) {
    f32 yDiff;

    thisv->alpha = 255;
    thisv->movementFlags &= ~ENBOX_MOVE_IMMOBILE;
    if (thisv->dyna.actor.bgCheckFlags & 1) {
        thisv->movementFlags |= ENBOX_MOVE_UNUSED;
        if (thisv->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
            thisv->movementFlags &= ~ENBOX_MOVE_FALL_ANGLE_SIDE;
        } else {
            thisv->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        if (thisv->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG) {
            thisv->dyna.actor.velocity.y = -thisv->dyna.actor.velocity.y * 0.55f;
        } else {
            thisv->dyna.actor.velocity.y = -thisv->dyna.actor.velocity.y * 0.65f;
        }
        if (thisv->dyna.actor.velocity.y < 5.5f) {
            thisv->dyna.actor.shape.rot.z = 0;
            thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
            EnBox_SetupAction(thisv, EnBox_WaitOpen);
            OnePointCutscene_EndCutscene(globalCtx, thisv->unk_1AC);
        }
        Audio_PlaySoundGeneral(NA_SE_EV_COFFIN_CAP_BOUND, &thisv->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnBox_SpawnDust(thisv, globalCtx);
    }
    yDiff = thisv->dyna.actor.world.pos.y - thisv->dyna.actor.floorHeight;
    if (thisv->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
        thisv->dyna.actor.shape.rot.z = yDiff * 50.0f;
    } else {
        thisv->dyna.actor.shape.rot.z = -yDiff * 50.0f;
    }
}

void EnBox_FallOnSwitchFlag(EnBox* thisv, GlobalContext* globalCtx) {
    s32 treasureFlag = thisv->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&thisv->dyna.actor, globalCtx);
    }

    if (thisv->unk_1A8 >= 0) {
        EnBox_SetupAction(thisv, EnBox_Fall);
        thisv->unk_1AC = OnePointCutscene_Init(globalCtx, 4500, 9999, &thisv->dyna.actor, MAIN_CAM);
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    } else if (thisv->unk_1A8 >= -11) {
        thisv->unk_1A8++;
    } else if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        thisv->unk_1A8++;
    }
}

// used for types 9, 10
void func_809C9700(EnBox* thisv, GlobalContext* globalCtx) {
    s32 treasureFlag = thisv->dyna.actor.params & 0x1F;
    Player* player = GET_PLAYER(globalCtx);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&thisv->dyna.actor, globalCtx);
    }

    if (Math3D_Vec3fDistSq(&thisv->dyna.actor.world.pos, &player->actor.world.pos) > 22500.0f) {
        thisv->unk_1FB = ENBOX_STATE_0;
    } else {
        if (thisv->unk_1FB == ENBOX_STATE_0) {
            if (!(player->stateFlags2 & 0x1000000)) {
                player->stateFlags2 |= 0x800000;
                return;
            }
            thisv->unk_1FB = ENBOX_STATE_1;
        }

        if (thisv->unk_1FB == ENBOX_STATE_1) {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
            thisv->unk_1FB = ENBOX_STATE_2;
        } else if (thisv->unk_1FB == ENBOX_STATE_2 && globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            if ((globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_LULLABY && thisv->type == ENBOX_TYPE_9) ||
                (globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS && thisv->type == ENBOX_TYPE_10)) {
                thisv->dyna.actor.flags &= ~ACTOR_FLAG_25;
                EnBox_SetupAction(thisv, EnBox_AppearInit);
                OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
                thisv->unk_1A8 = 0;
                thisv->unk_1FB = ENBOX_STATE_0;
            } else {
                thisv->unk_1FB = ENBOX_STATE_0;
            }
        }
    }
}

void EnBox_AppearOnSwitchFlag(EnBox* thisv, GlobalContext* globalCtx) {
    s32 treasureFlag = thisv->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&thisv->dyna.actor, globalCtx);
    }

    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        EnBox_SetupAction(thisv, EnBox_AppearInit);
        thisv->unk_1A8 = -30;
    }
}

void EnBox_AppearOnRoomClear(EnBox* thisv, GlobalContext* globalCtx) {
    s32 treasureFlag = thisv->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&thisv->dyna.actor, globalCtx);
    }

    if (Flags_GetTempClear(globalCtx, thisv->dyna.actor.room) && !Player_InCsMode(globalCtx)) {
        Flags_SetClear(globalCtx, thisv->dyna.actor.room);
        EnBox_SetupAction(thisv, EnBox_AppearInit);
        OnePointCutscene_Attention(globalCtx, &thisv->dyna.actor);
        if (OnePointCutscene_CheckForCategory(globalCtx, thisv->dyna.actor.category)) {
            thisv->unk_1A8 = 0;
        } else {
            thisv->unk_1A8 = -30;
        }
    }
}

/**
 * The chest is ready to appear, possibly waiting for camera/cutscene-related stuff to happen
 */
void EnBox_AppearInit(EnBox* thisv, GlobalContext* globalCtx) {
    if (func_8005B198() == thisv->dyna.actor.category || thisv->unk_1A8 != 0) {
        EnBox_SetupAction(thisv, EnBox_AppearAnimation);
        thisv->unk_1A8 = 0;
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_KANKYO, thisv->dyna.actor.home.pos.x,
                    thisv->dyna.actor.home.pos.y, thisv->dyna.actor.home.pos.z, 0, 0, 0, 0x0011);
        Audio_PlaySoundGeneral(NA_SE_EV_TRE_BOX_APPEAR, &thisv->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnBox_AppearAnimation(EnBox* thisv, GlobalContext* globalCtx) {
    func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);

    if (thisv->unk_1A8 < 0) {
        thisv->unk_1A8++;
    } else if (thisv->unk_1A8 < 40) {
        thisv->unk_1A8++;
        thisv->dyna.actor.world.pos.y += 1.25f;
    } else if (thisv->unk_1A8 < 60) {
        thisv->alpha += 12;
        thisv->unk_1A8++;
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y;
    } else {
        EnBox_SetupAction(thisv, EnBox_WaitOpen);
    }
}

/**
 * Chest is ready to be open
 */
void EnBox_WaitOpen(EnBox* thisv, GlobalContext* globalCtx) {
    f32 frameCount;
    AnimationHeader* anim;
    s32 linkAge;
    s32 pad;
    Vec3f sp4C;
    Player* player;

    thisv->alpha = 255;
    thisv->movementFlags |= ENBOX_MOVE_IMMOBILE;
    if (thisv->unk_1F4 != 0) { // unk_1F4 is modified by player code
        linkAge = gSaveContext.linkAge;
        anim = sAnimations[(thisv->unk_1F4 < 0 ? 2 : 0) + linkAge];
        frameCount = Animation_GetLastFrame(anim);
        Animation_Change(&thisv->skelanime, anim, 1.5f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
        EnBox_SetupAction(thisv, EnBox_Open);
        if (thisv->unk_1F4 > 0) {
            switch (thisv->type) {
                case ENBOX_TYPE_SMALL:
                case ENBOX_TYPE_6:
                case ENBOX_TYPE_ROOM_CLEAR_SMALL:
                case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
                    break;
                default:
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_DEMO_TRE_LGT,
                                       thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y,
                                       thisv->dyna.actor.world.pos.z, thisv->dyna.actor.shape.rot.x,
                                       thisv->dyna.actor.shape.rot.y, thisv->dyna.actor.shape.rot.z, 0xFFFF);
                    Audio_PlayFanfare(NA_BGM_OPEN_TRE_BOX | 0x900);
            }
        }
        osSyncPrintf("Actor_Environment_Tbox_On() %d\n", thisv->dyna.actor.params & 0x1F);
        Flags_SetTreasure(globalCtx, thisv->dyna.actor.params & 0x1F);
    } else {
        player = GET_PLAYER(globalCtx);
        func_8002DBD0(&thisv->dyna.actor, &sp4C, &player->actor.world.pos);
        if (sp4C.z > -50.0f && sp4C.z < 0.0f && fabsf(sp4C.y) < 10.0f && fabsf(sp4C.x) < 20.0f &&
            Player_IsFacingActor(&thisv->dyna.actor, 0x3000, globalCtx)) {
            func_8002F554(&thisv->dyna.actor, globalCtx, 0 - (thisv->dyna.actor.params >> 5 & 0x7F));
        }
        if (Flags_GetTreasure(globalCtx, thisv->dyna.actor.params & 0x1F)) {
            EnBox_SetupAction(thisv, EnBox_Open);
        }
    }
}

/**
 * Plays an animation to its end, playing sounds at key points
 */
void EnBox_Open(EnBox* thisv, GlobalContext* globalCtx) {
    u16 sfxId;

    thisv->dyna.actor.flags &= ~ACTOR_FLAG_7;

    if (SkelAnime_Update(&thisv->skelanime)) {
        if (thisv->unk_1F4 > 0) {
            if (thisv->unk_1F4 < 120) {
                thisv->unk_1F4++;
            } else {
                Math_StepToF(&thisv->unk_1B0, 0.0f, 0.05f);
            }
        } else {
            if (thisv->unk_1F4 > -120) {
                thisv->unk_1F4--;
            } else {
                Math_StepToF(&thisv->unk_1B0, 0.0f, 0.05f);
            }
        }
    } else {
        sfxId = 0;

        if (Animation_OnFrame(&thisv->skelanime, 30.0f)) {
            sfxId = NA_SE_EV_TBOX_UNLOCK;
        } else if (Animation_OnFrame(&thisv->skelanime, 90.0f)) {
            sfxId = NA_SE_EV_TBOX_OPEN;
        }

        if (sfxId != 0) {
            Audio_PlaySoundGeneral(sfxId, &thisv->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        if (thisv->skelanime.jointTable[3].z > 0) {
            thisv->unk_1B0 = (0x7D00 - thisv->skelanime.jointTable[3].z) * 0.00006f;
            if (thisv->unk_1B0 < 0.0f) {
                thisv->unk_1B0 = 0.0f;
            } else if (thisv->unk_1B0 > 1.0f) {
                thisv->unk_1B0 = 1.0f;
            }
        }
    }
}

void EnBox_SpawnIceSmoke(EnBox* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    f32 f0;

    thisv->iceSmokeTimer++;
    func_8002F974(&thisv->dyna.actor, NA_SE_EN_MIMICK_BREATH - SFX_FLAG);
    if (Rand_ZeroOne() < 0.3f) {
        f0 = 2.0f * Rand_ZeroOne() - 1.0f;
        pos = thisv->dyna.actor.world.pos;
        if (thisv->type == ENBOX_TYPE_SMALL || thisv->type == ENBOX_TYPE_6 || thisv->type == ENBOX_TYPE_ROOM_CLEAR_SMALL ||
            thisv->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) {
            pos.x += f0 * 10.0f * Math_SinS(thisv->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 10.0f * Math_CosS(thisv->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * Rand_ZeroOne() - 1.0f;
            vel.x = f0 * 0.8f * Math_SinS(thisv->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 0.8f * Math_CosS(thisv->dyna.actor.world.rot.y);
        } else {
            pos.x += f0 * 20.0f * Math_SinS(thisv->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 20.0f * Math_CosS(thisv->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * Rand_ZeroOne() - 1.0f;
            vel.x = f0 * 1.6f * Math_SinS(thisv->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 1.6f * Math_CosS(thisv->dyna.actor.world.rot.y);
        }
        EffectSsIceSmoke_Spawn(globalCtx, &pos, &vel, &accel, 150);
    }
}

void EnBox_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* thisv = (EnBox*)thisx;

    if (thisv->movementFlags & ENBOX_MOVE_STICK_TO_GROUND) {
        thisv->movementFlags &= ~ENBOX_MOVE_STICK_TO_GROUND;
        EnBox_ClipToGround(thisv, globalCtx);
    }

    thisv->actionFunc(thisv, globalCtx);

    if (!(thisv->movementFlags & ENBOX_MOVE_IMMOBILE)) {
        Actor_MoveForward(&thisv->dyna.actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 0.0f, 0.0f, 0.0f, 0x1C);
    }

    switch (thisv->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_SetFocus(&thisv->dyna.actor, 20.0f);
            break;
        default:
            Actor_SetFocus(&thisv->dyna.actor, 40.0f);
    }

    if ((thisv->dyna.actor.params >> 5 & 0x7F) == 0x7C && thisv->actionFunc == EnBox_Open &&
        thisv->skelanime.curFrame > 45 && thisv->iceSmokeTimer < 100) {
        EnBox_SpawnIceSmoke(thisv, globalCtx);
    }
}

void EnBox_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnBox* thisv = (EnBox*)thisx;
    s32 pad;

    if (limbIndex == 1) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_box.c", 1492),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (thisv->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestFrontDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestFrontDL);
        }
    } else if (limbIndex == 3) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_box.c", 1502),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (thisv->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestSideAndLidDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestSideAndTopDL);
        }
    }
}

Gfx* EnBox_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = Graph_Alloc(gfxCtx, sizeof(Gfx));
    ASSERT(dList != NULL, "gfxp != NULL", "../z_en_box.c", 1528);

    dListHead = dList;
    gSPEndDisplayList(dListHead++);

    return dList;
}

// set render mode with a focus on transparency
Gfx* func_809CA4A0(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1546);

    dList = dListHead;
    gDPSetRenderMode(dListHead++,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA),
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* func_809CA518(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1564);

    dList = dListHead;
    gDPSetRenderMode(dListHead++,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA),
                     G_RM_AA_ZB_OPA_SURF2);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnBox_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* thisv = (EnBox*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_box.c", 1581);

    /*
    thisv->dyna.actor.flags & ACTOR_FLAG_7 is set by Init (if type is 4 or 6)
    and cleared by Open
    */
    if ((thisv->alpha == 255 && !(thisv->type == ENBOX_TYPE_4 || thisv->type == ENBOX_TYPE_6)) ||
        (!CHECK_FLAG_ALL(thisv->dyna.actor.flags, ACTOR_FLAG_7) &&
         (thisv->type == ENBOX_TYPE_4 || thisv->type == ENBOX_TYPE_6))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, EnBox_EmptyDList(globalCtx->state.gfxCtx));
        func_80093D18(globalCtx->state.gfxCtx);
        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, NULL,
                                       EnBox_PostLimbDraw, thisv, POLY_OPA_DISP);
    } else if (thisv->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, thisv->alpha);
        if (thisv->type == ENBOX_TYPE_4 || thisv->type == ENBOX_TYPE_6) {
            gSPSegment(POLY_XLU_DISP++, 0x08, func_809CA518(globalCtx->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, func_809CA4A0(globalCtx->state.gfxCtx));
        }
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelanime.skeleton, thisv->skelanime.jointTable, NULL,
                                       EnBox_PostLimbDraw, thisv, POLY_XLU_DISP);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_box.c", 1639);
}
