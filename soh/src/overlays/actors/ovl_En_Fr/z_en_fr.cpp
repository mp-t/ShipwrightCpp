#include "z_en_fr.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "vt.h"
#include "objects/object_fr/object_fr.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnFr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFr_UpdateIdle(Actor* thisx, GlobalContext* globalCtx);
void EnFr_UpdateActive(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Reset(void);

// Animation Functions
void EnFr_SetupJumpingOutOfWater(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_JumpingOutOfWater(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_OrientOnLogSpot(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ChooseJumpFromLogSpot(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_JumpingUp(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_JumpingBackIntoWater(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_DecrementBlinkTimerUpdate(EnFr* thisv);

// Activation
void EnFr_Idle(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_Activate(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ActivateCheckFrogSong(EnFr* thisv, GlobalContext* globalCtx);

// Listening for Child Songs
void func_80A1BE98(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ListeningToOcarinaNotes(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ChildSong(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ChildSongFirstTime(EnFr* thisv, GlobalContext* globalCtx);

// Frog Song for HP Functions
void EnFr_TalkBeforeFrogSong(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_SetupFrogSong(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_ContinueFrogSong(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_OcarinaMistake(EnFr* thisv, GlobalContext* globalCtx);

// Reward Functions
void EnFr_SetupReward(EnFr* thisv, GlobalContext* globalCtx, u8 unkCondition);
void EnFr_PrintTextBox(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_TalkBeforeReward(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_SetReward(EnFr* thisv, GlobalContext* globalCtx);

// Deactivate
void EnFr_Deactivate(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_GiveReward(EnFr* thisv, GlobalContext* globalCtx);
void EnFr_SetIdle(EnFr* thisv, GlobalContext* globalCtx);

/*
Frogs params WIP docs

Represents 6 Actor Instances for frogs:
    - 1 Prop actor instance set to where Link plays Ocarina, manages 5 frogs
    - 5 NPC actor instances for the frogs themselves

0: Prop Actor Instance (located where link detects ocarina, interacts with Link)
1: Frog 0 (Yellow)
2: Frog 1 (Blue)
3: Frog 2 (Red)
4: Frog 3 (Purple)
5: Frog 4 (White)

Note that because of the Prop Actor, actor.params is 1 shifted from frogIndex
Therefore, frogIndex = actor.params - 1


sEnFrPointers.flags = 1
     - Activate frogs, frogs will jump out of the water

sEnFrPointers.flags = 1 to 11:
     - Counter: Frogs will sequentially jump out based on counter:
         - 1: Frog 1 (Blue)
         - 3: frog 3 (Purple)
         - 5: frog 0 (Yellow)
         - 7: frog 2 (Red)
         - 9: frog 4 (White)
     - Will proceed when counter reachers 11

sEnFrPointers.flags = 12
     - Deactivate frogs, frogs will jump back into the water
*/

typedef struct {
    u8 flags;
    EnFr* frogs[5];
} EnFrPointers;

typedef struct {
    f32 xzDist;
    f32 yaw;
    f32 yDist;
} LogSpotToFromWater;

static EnFrPointers sEnFrPointers = {
    0x00,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

// Flags for gSaveContext.eventChkInf[13]
static u16 sSongIndex[] = {
    0x0002, 0x0004, 0x0010, 0x0008, 0x0020, 0x0040, 0x0001, 0x0000,
};

// Frog to Index for Song Flag (sSongIndex) Mapping
static u8 sFrogToSongIndex[] = {
    FROG_SARIA, FROG_SUNS, FROG_SOT, FROG_ZL, FROG_EPONA,
};

// Song to Frog Index Mapping
static s32 sSongToFrog[] = {
    FROG_PURPLE, FROG_WHITE, FROG_YELLOW, FROG_BLUE, FROG_RED,
};

ActorInit En_Fr_InitVars = {
    ACTOR_EN_FR,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FR,
    sizeof(EnFr),
    (ActorFunc)EnFr_Init,
    (ActorFunc)EnFr_Destroy,
    (ActorFunc)EnFr_Update,
    NULL,
    (ActorResetFunc)EnFr_Reset,
};

static Color_RGBA8 sEnFrColor[] = {
    { 200, 170, 0, 255 }, { 0, 170, 200, 255 }, { 210, 120, 100, 255 }, { 120, 130, 230, 255 }, { 190, 190, 190, 255 },
};

// Jumping back into water frog animation
//      sLogSpotToFromWater[frog].xzDist is magnitude of xz distance frog travels
//      sLogSpotToFromWater[frog].yaw is rot around y-axis of jumping back into water
//      sLogSpotToFromWater[frog].yDist is change in y distance frog travels
static LogSpotToFromWater sLogSpotToFromWater[] = {
    { 0.0f, 0.0f, 0.0f },              // Prop   (Where link pulls ocarina)
    { 80.0f, -0.375f * std::numbers::pi_v<float>, -80.0f }, // FROG_YELLOW
    { 80.0f, -0.5f * std::numbers::pi_v<float>, -80.0f },   // FROG_BLUE
    { 80.0f, -0.25f * std::numbers::pi_v<float>, -80.0f },  // FROG_RED
    { 80.0f, 0.875f * std::numbers::pi_v<float>, -80.0f },  // FROG_PURPLE
    { 80.0f, 0.5f * std::numbers::pi_v<float>, -80.0f },    // FROG_WHITE
};

// Timer values for the frog choir song
static s16 sTimerFrogSong[] = {
    40, 20, 15, 12, 12,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

// Counter to Coordinate Frog jumping out of water one at a time
// Used as part of sEnFrPointers.flags
static u8 sTimerJumpingOutOfWater[] = {
    5, 1, 7, 3, 9,
};

// targetScale (default = 150.0) Actor scale target for Math_ApproachF
// Used as a frog grows from hearing a new child song
static f32 sGrowingScale[] = {
    180.0f,
    210.0f,
    240.0f,
    270.0f,
};

static u8 sSmallFrogNotes[] = {
    5,  // C-Down Ocarina
    2,  // A Button Ocarina
    9,  // C-Right Ocarina
    11, // C-Left Ocarina
    14, // C Up Ocarina
};

static s8 sLargeFrogNotes[] = {
    -7,  // C-Down Ocarina
    -10, // A Button Ocarina
    -3,  // C-Right Ocarina
    -1,  // C-Left Ocarina
    2,   // C Up Ocarina
};

static u8 sJumpOrder[] = {
    FROG_BLUE, FROG_YELLOW, FROG_RED, FROG_PURPLE, FROG_WHITE, FROG_BLUE, FROG_YELLOW, FROG_RED,
};

static u8 sOcarinaNotes[] = {
    OCARINA_NOTE_A, OCARINA_NOTE_C_DOWN, OCARINA_NOTE_C_RIGHT, OCARINA_NOTE_C_LEFT, OCARINA_NOTE_C_UP,
};

void EnFr_OrientUnderwater(EnFr* thisv) {
    Vec3f vec1;
    Vec3f vec2;

    vec1.x = vec1.y = 0.0f;
    vec1.z = thisv->xzDistToLogSpot = sLogSpotToFromWater[thisv->actor.params].xzDist;
    Matrix_RotateY(sLogSpotToFromWater[thisv->actor.params].yaw, MTXMODE_NEW);
    Matrix_MultVec3f(&vec1, &vec2);
    thisv->actor.world.pos.x = thisv->posLogSpot.x + vec2.x;
    thisv->actor.world.pos.z = thisv->posLogSpot.z + vec2.z;
    thisv->actor.world.pos.y = sLogSpotToFromWater[thisv->actor.params].yDist + thisv->posLogSpot.y;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y =
        (s16)(sLogSpotToFromWater[thisv->actor.params].yaw * ((f32)0x8000 / std::numbers::pi_v<float>)) + 0x8000;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = 0.0f;
}

void EnFr_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* thisv = (EnFr*)thisx;

    if (thisv->actor.params == 0) {
        thisv->actor.destroy = NULL;
        thisv->actor.draw = NULL;
        thisv->actor.update = EnFr_UpdateIdle;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_4);
        thisv->actor.flags &= ~0;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
        thisv->actor.textId = 0x40AC;
        thisv->actionFunc = EnFr_Idle;
    } else {
        if ((thisv->actor.params >= 6) || (thisv->actor.params < 0)) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "The argument is wrong!!"
            osSyncPrintf("%s[%d] : 引数が間違っている！！(%d)\n", "../z_en_fr.c", 370, thisv->actor.params);
            osSyncPrintf(VT_RST);
            ASSERT(0, "0", "../z_en_fr.c", 372);
        }

        thisv->objBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP);
        if (thisv->objBankIndex < 0) {
            Actor_Kill(&thisv->actor);
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There is no bank!!"
            osSyncPrintf("%s[%d] : バンクが無いよ！！\n", "../z_en_fr.c", 380);
            osSyncPrintf(VT_RST);
            ASSERT(0, "0", "../z_en_fr.c", 382);
        }
    }
}

// Draw only the purple frog when ocarina is not pulled out on the log spot
void EnFr_DrawIdle(EnFr* thisv) {
    thisv->actor.draw = (thisv->actor.params - 1) != FROG_PURPLE ? NULL : EnFr_Draw;
}

void EnFr_DrawActive(EnFr* thisv) {
    thisv->actor.draw = EnFr_Draw;
}

void EnFr_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* thisv = (EnFr*)thisx;
    s32 pad;
    s32 frogIndex;
    s32 pad2;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objBankIndex)) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        frogIndex = thisv->actor.params - 1;
        sEnFrPointers.frogs[frogIndex] = thisv;
        Actor_ProcessInitChain(&thisv->actor, sInitChain);
        // frog
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_fr_Skel_00B498, &object_fr_Anim_001534,
                           thisv->jointTable, thisv->morphTable, 24);
        // butterfly
        SkelAnime_Init(globalCtx, &thisv->skelAnimeButterfly, &gButterflySkel, &gButterflyAnim,
                       thisv->jointTableButterfly, thisv->morphTableButterfly, 8);
        // When playing the song for the HP, the frog with the next note and the butterfly turns on its lightsource
        thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y,
                                  thisv->actor.home.pos.z, 255, 255, 255, -1);
        // Check to see if the song for a particular frog has been played.
        // If it has, the frog is larger. If not, the frog is smaller
        thisv->scale = gSaveContext.eventChkInf[13] & sSongIndex[sFrogToSongIndex[frogIndex]] ? 270.0f : 150.0f;
        // When the frogs are not active (link doesn't have his ocarina out),
        // Then shrink the frogs down by a factor of 10,000
        Actor_SetScale(&thisv->actor, thisv->scale * 0.0001f);
        thisv->actor.minVelocityY = -9999.0f;
        Actor_SetFocus(&thisv->actor, 10.0f);
        thisv->eyeTexIndex = 1;
        thisv->blinkTimer = (s16)(Rand_ZeroFloat(60.0f) + 20.0f);
        thisv->blinkFunc = EnFr_DecrementBlinkTimerUpdate;
        thisv->isBelowWaterSurfacePrevious = thisv->isBelowWaterSurfaceCurrent = false;
        thisv->isJumpingUp = false;
        thisv->posLogSpot = thisv->actor.world.pos;
        thisv->actionFunc = EnFr_SetupJumpingOutOfWater;
        thisv->isDeactivating = false;
        thisv->growingScaleIndex = 0;
        thisv->isActive = false;
        thisv->isJumpingToFrogSong = false;
        thisv->songIndex = FROG_NO_SONG;
        thisv->unusedButterflyActor = NULL;
        EnFr_OrientUnderwater(thisv);
        EnFr_DrawIdle(thisv);
        thisv->actor.update = EnFr_UpdateActive;
        thisv->isButterflyDrawn = false;
        thisv->xyAngleButterfly = 0x1000 * (s16)Rand_ZeroFloat(255.0f);
        thisv->posButterflyLight.x = thisv->posButterfly.x = thisv->posLogSpot.x;
        thisv->posButterflyLight.y = thisv->posButterfly.y = thisv->posLogSpot.y + 50.0f;
        thisv->posButterflyLight.z = thisv->posButterfly.z = thisv->posLogSpot.z;
        thisv->actor.flags &= ~ACTOR_FLAG_0;
    }
}

void EnFr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* thisv = (EnFr*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
}

void EnFr_IsDivingIntoWater(EnFr* thisv, GlobalContext* globalCtx) {
    WaterBox* waterBox;
    f32 waterSurface;

    if (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                &waterSurface, &waterBox)) {
        thisv->isBelowWaterSurfacePrevious = thisv->isBelowWaterSurfaceCurrent;
        thisv->isBelowWaterSurfaceCurrent = thisv->actor.world.pos.y <= waterSurface ? true : false;
    }
}

void EnFr_DivingIntoWater(EnFr* thisv, GlobalContext* globalCtx) {
    Vec3f vec;

    // Jumping into or out of water
    if (thisv->isBelowWaterSurfaceCurrent != thisv->isBelowWaterSurfacePrevious) {
        vec.x = thisv->actor.world.pos.x;
        vec.y = thisv->actor.world.pos.y - 10.0f;
        vec.z = thisv->actor.world.pos.z;
        EffectSsGSplash_Spawn(globalCtx, &vec, NULL, NULL, 1, 1);

        if (thisv->isBelowWaterSurfaceCurrent == false) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_DIVE_INTO_WATER_L);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

s32 EnFr_IsBelowLogSpot(EnFr* thisv, f32* yDistToLogSpot) {
    yDistToLogSpot[0] = thisv->actor.world.pos.y - thisv->posLogSpot.y;
    if ((thisv->actor.velocity.y < 0.0f) && (yDistToLogSpot[0] <= 0.0f)) {
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.world.pos.y = thisv->posLogSpot.y;
        return true;
    } else {
        return false;
    }
}

s32 EnFr_IsAboveAndWithin30DistXZ(Player* player, EnFr* thisv) {
    f32 xDistToPlayer = player->actor.world.pos.x - thisv->actor.world.pos.x;
    f32 zDistToPlayer = player->actor.world.pos.z - thisv->actor.world.pos.z;
    f32 yDistToPlayer = player->actor.world.pos.y - thisv->actor.world.pos.y;

    return ((SQ(xDistToPlayer) + SQ(zDistToPlayer)) <= SQ(30.0f)) && (yDistToPlayer >= 0.0f);
}

void EnFr_DecrementBlinkTimer(EnFr* thisv) {
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    } else {
        thisv->blinkFunc = EnFr_DecrementBlinkTimerUpdate;
    }
}

void EnFr_DecrementBlinkTimerUpdate(EnFr* thisv) {
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    } else if (thisv->eyeTexIndex) {
        thisv->eyeTexIndex = 0;
        thisv->blinkTimer = (s16)(Rand_ZeroFloat(60.0f) + 20.0f);
        thisv->blinkFunc = EnFr_DecrementBlinkTimer;
    } else {
        thisv->eyeTexIndex = 1;
        thisv->blinkTimer = 1;
    }
}

void EnFr_SetupJumpingOutOfWater(EnFr* thisv, GlobalContext* globalCtx) {
    if (sEnFrPointers.flags == sTimerJumpingOutOfWater[thisv->actor.params - 1]) {
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        EnFr_DrawActive(thisv);
        thisv->actionFunc = EnFr_JumpingOutOfWater;
    }
}

void EnFr_JumpingOutOfWater(EnFr* thisv, GlobalContext* globalCtx) {
    Vec3f vec1;
    Vec3f vec2;

    if (thisv->skelAnime.curFrame == 6.0f) {
        sEnFrPointers.flags++;
        thisv->skelAnime.playSpeed = 0.0f;
    } else if (thisv->skelAnime.curFrame == 3.0f) {
        thisv->actor.gravity = -10.0f;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.velocity.y = 47.0f;
    }

    vec1.x = vec1.y = 0.0f;
    vec1.z = thisv->xzDistToLogSpot;
    Matrix_RotateY(((thisv->actor.world.rot.y + 0x8000) / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);
    Matrix_MultVec3f(&vec1, &vec2);
    thisv->actor.world.pos.x = thisv->posLogSpot.x + vec2.x;
    thisv->actor.world.pos.z = thisv->posLogSpot.z + vec2.z;
    if (thisv->skelAnime.curFrame >= 3.0f) {
        Math_ApproachF(&thisv->xzDistToLogSpot, 0.0f, 1.0f, 10.0f);
    }

    if (EnFr_IsBelowLogSpot(thisv, &vec2.y)) {
        thisv->actor.gravity = 0.0f;
        thisv->actionFunc = EnFr_OrientOnLogSpot;
        thisv->unusedFloat = 0.0f;
    }

    if ((thisv->actor.velocity.y <= 0.0f) && (vec2.y < 40.0f)) {
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnFr_OrientOnLogSpot(EnFr* thisv, GlobalContext* globalCtx) {
    s16 rotYRemaining = Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 10000, 100);

    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;

    if ((rotYRemaining == 0) && (thisv->skelAnime.curFrame == thisv->skelAnime.endFrame)) {
        sEnFrPointers.flags++;
        thisv->actionFunc = EnFr_ChooseJumpFromLogSpot;
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
    }
}

void EnFr_ChooseJumpFromLogSpot(EnFr* thisv, GlobalContext* globalCtx) {
    if (sEnFrPointers.flags == 12) {
        thisv->actor.world.rot.y = ((f32)0x8000 / std::numbers::pi_v<float>) * sLogSpotToFromWater[thisv->actor.params].yaw;
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        thisv->actionFunc = EnFr_JumpingBackIntoWater;
    } else if (thisv->isJumpingUp) {
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        thisv->actionFunc = EnFr_JumpingUp;
    }
}

void EnFr_JumpingUp(EnFr* thisv, GlobalContext* globalCtx) {
    f32 yDistToLogSpot;

    if (thisv->skelAnime.curFrame == 6.0f) {
        thisv->skelAnime.playSpeed = 0.0f;
    } else if (thisv->skelAnime.curFrame == 3.0f) {
        thisv->actor.gravity = -10.0f;
        thisv->actor.velocity.y = 25.0f;
        if (thisv->isJumpingToFrogSong) {
            thisv->isJumpingToFrogSong = false;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_EAT);
        }
    }

    if (EnFr_IsBelowLogSpot(thisv, &yDistToLogSpot)) {
        thisv->isJumpingUp = false;
        thisv->actor.gravity = 0.0f;
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_0011C0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0011C0), ANIMMODE_LOOP, 0.0f);
        thisv->actionFunc = EnFr_ChooseJumpFromLogSpot;
    } else if ((thisv->actor.velocity.y <= 0.0f) && (yDistToLogSpot < 40.0f)) {
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnFr_JumpingBackIntoWater(EnFr* thisv, GlobalContext* globalCtx) {
    f32 yUnderwater = sLogSpotToFromWater[thisv->actor.params].yDist + thisv->posLogSpot.y;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 2, 10000, 100);
    if (thisv->skelAnime.curFrame == 6.0f) {
        thisv->skelAnime.playSpeed = 0.0f;
    } else if (thisv->skelAnime.curFrame == 3.0f) {
        thisv->actor.speedXZ = 6.0f;
        thisv->actor.gravity = -10.0f;
        thisv->actor.velocity.y = 25.0f;
    }

    // Final Spot Reached
    if ((thisv->actor.velocity.y < 0.0f) && (thisv->actor.world.pos.y < yUnderwater)) {
        Animation_Change(&thisv->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
        thisv->actionFunc = EnFr_SetupJumpingOutOfWater;
        EnFr_DrawIdle(thisv);
        thisv->isDeactivating = true;
        EnFr_OrientUnderwater(thisv);
    }
}

void EnFr_SetScaleActive(EnFr* thisv, GlobalContext* globalCtx) {
    switch (thisv->isGrowing) {
        case false:
            Math_ApproachF(&thisv->scale, sGrowingScale[thisv->growingScaleIndex], 2.0f, 25.0f);
            if (thisv->scale >= sGrowingScale[thisv->growingScaleIndex]) {
                thisv->scale = sGrowingScale[thisv->growingScaleIndex];
                if (thisv->growingScaleIndex < 3) {
                    thisv->isGrowing = true;
                } else {
                    thisv->isJumpingUp = false;
                    thisv->isActive = false;
                }
            }
            break;
        case true:
            Math_ApproachF(&thisv->scale, 150.0f, 2.0f, 25.0f);
            if (thisv->scale <= 150.0f) {
                thisv->scale = 150.0f;
                thisv->growingScaleIndex++;
                if (thisv->growingScaleIndex >= 4) {
                    thisv->growingScaleIndex = 3;
                }
                thisv->isGrowing = false;
            }
            break;
    }
}

void EnFr_ButterflyPath(EnFr* thisv, GlobalContext* globalCtx) {
    s16 rotY = thisv->actor.shape.rot.y;
    f32 sin;
    Vec3f vec1;
    Vec3f vec2;

    thisv->xyAngleButterfly += 0x1000;
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = 0.0f;
    vec1.z = 25.0f;
    Matrix_MultVec3f(&vec1, &vec2);
    sin = Math_SinS(thisv->xyAngleButterfly * 2) * 5.0f;
    thisv->posButterfly.x = (Math_SinS(rotY) * sin) + vec2.x;
    thisv->posButterfly.y = (2.0f * Math_CosS(thisv->xyAngleButterfly)) + (thisv->posLogSpot.y + 50.0f);
    thisv->posButterfly.z = (Math_CosS(rotY) * sin) + vec2.z;
    Matrix_Translate(thisv->posButterfly.x, thisv->posButterfly.y, thisv->posButterfly.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = 0.0f;
    vec1.y = -15.0f;
    vec1.z = 20.0f;
    Matrix_MultVec3f(&vec1, &thisv->posButterflyLight);
}

void EnFr_UpdateActive(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* thisv = (EnFr*)thisx;

    thisv->jumpCounter++;
    Actor_SetScale(&thisv->actor, thisv->scale * 0.0001f);

    if (thisv->isActive) {
        EnFr_SetScaleActive(thisv, globalCtx);
    } else {
        Actor_SetFocus(&thisv->actor, 10.0f);
        thisv->blinkFunc(thisv);
        thisv->actionFunc(thisv, globalCtx);
        EnFr_IsDivingIntoWater(thisv, globalCtx);
        EnFr_DivingIntoWater(thisv, globalCtx);
        SkelAnime_Update(&thisv->skelAnime);
        SkelAnime_Update(&thisv->skelAnimeButterfly);
        EnFr_ButterflyPath(thisv, globalCtx);
        Actor_MoveForward(&thisv->actor);
    }
}

s32 EnFr_SetupJumpingUp(EnFr* thisv, s32 frogIndex) {
    EnFr* frog = sEnFrPointers.frogs[frogIndex];
    u8 semitone;

    if (frog != NULL && frog->isJumpingUp == false) {
        semitone = frog->growingScaleIndex == 3 ? sLargeFrogNotes[frogIndex] : sSmallFrogNotes[frogIndex];
        if (thisv->songIndex == FROG_CHOIR_SONG) {
            frog->isJumpingToFrogSong = true;
        }
        frog->isJumpingUp = true;
        Audio_PlaySoundTransposed(&frog->actor.projectedPos, NA_SE_EV_FROG_JUMP, semitone);
        return true;
    } else {
        return false;
    }
}

void EnFr_Idle(EnFr* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags2 & 0x2000000) {
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }

        OnePointCutscene_Init(globalCtx, 4110, ~0x62, &thisv->actor, MAIN_CAM);
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        player->actor.world.pos.x = thisv->actor.world.pos.x; // x = 990.0f
        player->actor.world.pos.y = thisv->actor.world.pos.y; // y = 205.0f
        player->actor.world.pos.z = thisv->actor.world.pos.z; // z = -1220.0f
        player->currentYaw = player->actor.world.rot.y = player->actor.shape.rot.y = thisv->actor.world.rot.y;
        thisv->reward = GI_NONE;
        thisv->actionFunc = EnFr_Activate;
    } else if (EnFr_IsAboveAndWithin30DistXZ(player, thisv)) {
        player->unk_6A8 = &thisv->actor;
    }
}

void EnFr_Activate(EnFr* thisv, GlobalContext* globalCtx) {
    if (globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_PLAYING) {
        globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
        sEnFrPointers.flags = 1;
        thisv->actionFunc = EnFr_ActivateCheckFrogSong;
    } else if (globalCtx->msgCtx.msgMode == MSGMODE_PAUSED) { // Goes to Frogs 2 Song
        sEnFrPointers.flags = 1;
        thisv->actionFunc = EnFr_ActivateCheckFrogSong;
    }
}

void EnFr_ActivateCheckFrogSong(EnFr* thisv, GlobalContext* globalCtx) {
    if (sEnFrPointers.flags == 11) {
        // Check if all 6 child songs have been played for the frogs
        if ((gSaveContext.eventChkInf[13] & 0x2)        // ZL
            && (gSaveContext.eventChkInf[13] & 0x4)     // Epona
            && (gSaveContext.eventChkInf[13] & 0x10)    // Saria
            && (gSaveContext.eventChkInf[13] & 0x8)     // Suns
            && (gSaveContext.eventChkInf[13] & 0x20)    // SoT
            && (gSaveContext.eventChkInf[13] & 0x40)) { // SoS
            thisv->actionFunc = EnFr_TalkBeforeFrogSong;
            thisv->songIndex = FROG_CHOIR_SONG;
            Message_StartTextbox(globalCtx, 0x40AB, &thisv->actor);
        } else {
            thisv->songIndex = FROG_ZL;
            thisv->actionFunc = func_80A1BE98;
        }
    }
}

void func_80A1BE98(EnFr* thisv, GlobalContext* globalCtx) {
    EnFr* frog;
    s32 frogIndex;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        if (frog != NULL && frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
            continue;
        } else {
            return;
        }
    }

    func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_NOWARP);
    thisv->actionFunc = EnFr_ListeningToOcarinaNotes;
}

void EnFr_ListeningToOcarinaNotes(EnFr* thisv, GlobalContext* globalCtx) {
    thisv->songIndex = FROG_NO_SONG;
    switch (globalCtx->msgCtx.ocarinaMode) { // Ocarina Song played
        case OCARINA_MODE_07:
            thisv->songIndex = FROG_ZL;
            break;
        case OCARINA_MODE_06:
            thisv->songIndex = FROG_EPONA;
            break;
        case OCARINA_MODE_05:
            thisv->songIndex = FROG_SARIA;
            break;
        case OCARINA_MODE_08:
            thisv->songIndex = FROG_SUNS;
            break;
        case OCARINA_MODE_09:
            thisv->songIndex = FROG_SOT;
            break;
        case OCARINA_MODE_0A:
            thisv->songIndex = FROG_STORMS;
            break;
        case OCARINA_MODE_04:
            EnFr_OcarinaMistake(thisv, globalCtx);
            break;
        case OCARINA_MODE_01:                           // Ocarina note played, but no song played
            switch (globalCtx->msgCtx.lastOcaNoteIdx) { // Jumping frogs in open ocarina based on ocarina note played
                case OCARINA_NOTE_A:
                    EnFr_SetupJumpingUp(thisv, FROG_BLUE);
                    break;
                case OCARINA_NOTE_C_DOWN:
                    EnFr_SetupJumpingUp(thisv, FROG_YELLOW);
                    break;
                case OCARINA_NOTE_C_RIGHT:
                    EnFr_SetupJumpingUp(thisv, FROG_RED);
                    break;
                case OCARINA_NOTE_C_LEFT:
                    EnFr_SetupJumpingUp(thisv, FROG_PURPLE);
                    break;
                case OCARINA_NOTE_C_UP:
                    EnFr_SetupJumpingUp(thisv, FROG_WHITE);
                    break;
            }
    }
    if (thisv->songIndex != FROG_NO_SONG) {
        thisv->jumpCounter = 0;
        thisv->actionFunc = EnFr_ChildSong;
    }
}

void EnFr_ChildSong(EnFr* thisv, GlobalContext* globalCtx) {
    EnFr* frog;
    u8 songIndex;

    if (thisv->jumpCounter < 48) {
        if (thisv->jumpCounter % 4 == 0) {
            EnFr_SetupJumpingUp(thisv, sJumpOrder[(thisv->jumpCounter >> 2) & 7]);
        }
    } else {
        songIndex = thisv->songIndex;
        if (songIndex == FROG_STORMS) {
            thisv->actor.textId = 0x40AA;
            EnFr_SetupReward(thisv, globalCtx, false);
        } else if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            frog = sEnFrPointers.frogs[sSongToFrog[songIndex]];
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            if (frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
                frog->isJumpingUp = true;
                frog->isActive = true;
                Audio_PlayActorSound2(&frog->actor, NA_SE_EV_FROG_GROW_UP);
                thisv->actionFunc = EnFr_ChildSongFirstTime;
            } else {
                thisv->jumpCounter = 48;
            }
        } else {
            thisv->actor.textId = 0x40A9;
            EnFr_SetupReward(thisv, globalCtx, true);
        }
    }
}

void EnFr_ChildSongFirstTime(EnFr* thisv, GlobalContext* globalCtx) {
    EnFr* frog = sEnFrPointers.frogs[sSongToFrog[thisv->songIndex]];

    if (frog->isActive == false) {
        thisv->actor.textId = 0x40A9;
        EnFr_SetupReward(thisv, globalCtx, true);
    }
}

void EnFr_TalkBeforeFrogSong(EnFr* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        thisv->frogSongTimer = 2;
        thisv->actionFunc = EnFr_SetupFrogSong;
    }
}

void EnFr_CheckOcarinaInputFrogSong(u8 ocarinaNote) {
    EnFr* frog;
    s32 frogIndexButterfly;
    s32 frogIndex;

    switch (ocarinaNote) {
        case 0:
            frogIndexButterfly = FROG_BLUE;
            break;
        case 1:
            frogIndexButterfly = FROG_YELLOW;
            break;
        case 2:
            frogIndexButterfly = FROG_RED;
            break;
        case 3:
            frogIndexButterfly = FROG_PURPLE;
            break;
        case 4:
            frogIndexButterfly = FROG_WHITE;
    }
    // Turn on or off butterfly above frog
    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        frog->isButterflyDrawn = frogIndex == frogIndexButterfly ? true : false;
    }
}

void EnFr_DeactivateButterfly() {
    s32 frogIndex;
    EnFr* frog;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        frog->isButterflyDrawn = false;
    }
}

u8 EnFr_GetNextNoteFrogSong(u8 ocarinaNoteIndex) {
    if (!(gSaveContext.eventChkInf[13] & 1)) {
        return gFrogsSongPtr[ocarinaNoteIndex];
    } else {
        return sOcarinaNotes[(s32)Rand_ZeroFloat(60.0f) % 5];
    }
}

void EnFr_SetupFrogSong(EnFr* thisv, GlobalContext* globalCtx) {
    if (thisv->frogSongTimer != 0) {
        thisv->frogSongTimer--;
    } else {
        thisv->frogSongTimer = 40;
        thisv->ocarinaNoteIndex = 0;
        func_8010BD58(globalCtx, OCARINA_ACTION_FROGS);
        thisv->ocarinaNote = EnFr_GetNextNoteFrogSong(thisv->ocarinaNoteIndex);
        EnFr_CheckOcarinaInputFrogSong(thisv->ocarinaNote);
        thisv->actionFunc = EnFr_ContinueFrogSong;
    }
}

s32 EnFr_IsFrogSongComplete(EnFr* thisv, GlobalContext* globalCtx) {
    u8 index;
    u8 ocarinaNote;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    u8 ocarinaNoteIndex;

    if (thisv->ocarinaNote == (*msgCtx).lastOcaNoteIdx) { // required to match, possibly an array?
        thisv->ocarinaNoteIndex++;
        ocarinaNoteIndex = thisv->ocarinaNoteIndex;
        if (1) {}
        if (ocarinaNoteIndex >= 14) { // Frog Song is completed
            thisv->ocarinaNoteIndex = 13;
            return true;
        }
        // The first four notes have more frames to receive an input
        index = ocarinaNoteIndex < 4 ? (s32)ocarinaNoteIndex : 4;
        ocarinaNote = EnFr_GetNextNoteFrogSong(ocarinaNoteIndex);
        thisv->ocarinaNote = ocarinaNote;
        EnFr_CheckOcarinaInputFrogSong(ocarinaNote);
        thisv->frogSongTimer = sTimerFrogSong[index];
    }
    return false;
}

void EnFr_OcarinaMistake(EnFr* thisv, GlobalContext* globalCtx) {
    Message_CloseTextbox(globalCtx);
    thisv->reward = GI_NONE;
    func_80078884(NA_SE_SY_OCARINA_ERROR);
    Audio_OcaSetInstrument(0);
    sEnFrPointers.flags = 12;
    EnFr_DeactivateButterfly();
    thisv->actionFunc = EnFr_Deactivate;
}

void EnFr_ContinueFrogSong(EnFr* thisv, GlobalContext* globalCtx) {
    s32 counter;
    EnFr* frog;
    s32 i;

    if (thisv->frogSongTimer == 0) {
        EnFr_OcarinaMistake(thisv, globalCtx);
    } else {
        thisv->frogSongTimer--;
        if (globalCtx->msgCtx.msgMode == MSGMODE_FROGS_PLAYING) {
            counter = 0;
            for (i = 0; i < ARRAY_COUNT(sEnFrPointers.frogs); i++) {
                frog = sEnFrPointers.frogs[i];
                if (frog != NULL && frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
                    continue;
                } else {
                    counter++;
                }
            }
            if (counter == 0 && CHECK_BTN_ALL(globalCtx->state.input[0].press.button, BTN_B)) {
                EnFr_OcarinaMistake(thisv, globalCtx);
                return;
            }
        }

        if (globalCtx->msgCtx.msgMode == MSGMODE_FROGS_WAITING) {
            globalCtx->msgCtx.msgMode = MSGMODE_FROGS_START;
            switch (globalCtx->msgCtx.lastOcaNoteIdx) {
                case OCARINA_NOTE_A:
                    EnFr_SetupJumpingUp(thisv, FROG_BLUE);
                    break;
                case OCARINA_NOTE_C_DOWN:
                    EnFr_SetupJumpingUp(thisv, FROG_YELLOW);
                    break;
                case OCARINA_NOTE_C_RIGHT:
                    EnFr_SetupJumpingUp(thisv, FROG_RED);
                    break;
                case OCARINA_NOTE_C_LEFT:
                    EnFr_SetupJumpingUp(thisv, FROG_PURPLE);
                    break;
                case OCARINA_NOTE_C_UP:
                    EnFr_SetupJumpingUp(thisv, FROG_WHITE);
            }
            if (EnFr_IsFrogSongComplete(thisv, globalCtx)) {
                thisv->actor.textId = 0x40AC;
                EnFr_SetupReward(thisv, globalCtx, false);
            }
        }
    }
}

void EnFr_SetupReward(EnFr* thisv, GlobalContext* globalCtx, u8 unkCondition) {
    EnFr_DeactivateButterfly();
    if (unkCondition) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    } else {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
    }

    Audio_OcaSetInstrument(0);
    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
    thisv->actionFunc = EnFr_PrintTextBox;
}

void EnFr_PrintTextBox(EnFr* thisv, GlobalContext* globalCtx) {
    Message_StartTextbox(globalCtx, thisv->actor.textId, &thisv->actor);
    thisv->actionFunc = EnFr_TalkBeforeReward;
}

void EnFr_TalkBeforeReward(EnFr* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        thisv->frogSongTimer = 100;
        Message_CloseTextbox(globalCtx);
        thisv->actionFunc = EnFr_SetReward;
    }
}

void EnFr_SetReward(EnFr* thisv, GlobalContext* globalCtx) {
    u16 songIndex;

    sEnFrPointers.flags = 12;
    songIndex = thisv->songIndex;
    thisv->actionFunc = EnFr_Deactivate;
    thisv->reward = GI_NONE;
    if ((songIndex >= FROG_ZL) && (songIndex <= FROG_SOT)) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            thisv->reward = GI_RUPEE_PURPLE;
        } else {
            thisv->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_STORMS) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            thisv->reward = GI_HEART_PIECE;
        } else {
            thisv->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_CHOIR_SONG) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            thisv->reward = GI_HEART_PIECE;
        } else {
            thisv->reward = GI_RUPEE_PURPLE;
        }
    }
}

void EnFr_Deactivate(EnFr* thisv, GlobalContext* globalCtx) {
    EnFr* frogLoop1;
    EnFr* frogLoop2;
    s32 frogIndex;

    // Originally was going to have separate butterfly actor
    // Changed to include butterfly as part of frog actor
    // This unused code would have frozen the butterfly actor above frog
    if (thisv->unusedButterflyActor != NULL) {
        thisv->unusedButterflyActor->freezeTimer = 10;
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frogLoop1 = sEnFrPointers.frogs[frogIndex];
        if (frogLoop1 == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There are no frogs!?"
            osSyncPrintf("%s[%d]カエルがいない！？\n", "../z_en_fr.c", 1604);
            osSyncPrintf(VT_RST);
            return;
        } else if (frogLoop1->isDeactivating != true) {
            return;
        }
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frogLoop2 = sEnFrPointers.frogs[frogIndex];
        if (frogLoop2 == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There are no frogs!?"
            osSyncPrintf("%s[%d]カエルがいない！？\n", "../z_en_fr.c", 1618);
            osSyncPrintf(VT_RST);
            return;
        }
        frogLoop2->isDeactivating = false;
    }

    globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FROG_CRY_0);
    if (thisv->reward == GI_NONE) {
        thisv->actionFunc = EnFr_Idle;
    } else {
        thisv->actionFunc = EnFr_GiveReward;
        func_8002F434(&thisv->actor, globalCtx, thisv->reward, 30.0f, 100.0f);
    }
}

void EnFr_GiveReward(EnFr* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnFr_SetIdle;
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->reward, 30.0f, 100.0f);
    }
}

void EnFr_SetIdle(EnFr* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        thisv->actionFunc = EnFr_Idle;
    }
}

void EnFr_UpdateIdle(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* thisv = (EnFr*)thisx;

    if (BREG(0)) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
    }
    thisv->jumpCounter++;
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnFr_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if ((limbIndex == 7) || (limbIndex == 8)) {
        *dList = NULL;
    }
    return 0;
}

void EnFr_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnFr* thisv = (EnFr*)thisx;

    if ((limbIndex == 7) || (limbIndex == 8)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fr.c", 1735);
        Matrix_Push();
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fr.c", 1738),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fr.c", 1741);
    }
}

void EnFr_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const void* eyeTextures[] = {
        object_fr_Tex_0059A0,
        object_fr_Tex_005BA0,
    };
    s16 lightRadius;
    EnFr* thisv = (EnFr*)thisx;
    s16 frogIndex = thisv->actor.params - 1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fr.c", 1754);
    func_80093D18(globalCtx->state.gfxCtx);
    // For the frogs 2 HP, the frog with the next note and the butterfly lights up
    lightRadius = thisv->isButterflyDrawn ? 95 : -1;
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->posButterflyLight.x, thisv->posButterflyLight.y,
                              thisv->posButterflyLight.z, 255, 255, 255, lightRadius);
    gDPSetEnvColor(POLY_OPA_DISP++, sEnFrColor[frogIndex].r, sEnFrColor[frogIndex].g, sEnFrColor[frogIndex].b, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnFr_OverrideLimbDraw, EnFr_PostLimbDraw, thisv);
    if (thisv->isButterflyDrawn) {
        Matrix_Translate(thisv->posButterfly.x, thisv->posButterfly.y, thisv->posButterfly.z, MTXMODE_NEW);
        Matrix_Scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);
        Matrix_RotateZYX(thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, MTXMODE_APPLY);
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnimeButterfly.skeleton, thisv->skelAnimeButterfly.jointTable, NULL, NULL,
                          NULL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fr.c", 1816);
}

void EnFr_Reset(void) {
    sEnFrPointers.flags = 0;
    sEnFrPointers.frogs[0] = NULL;
    sEnFrPointers.frogs[1] = NULL;
    sEnFrPointers.frogs[2] = NULL;
    sEnFrPointers.frogs[3] = NULL;
    sEnFrPointers.frogs[4] = NULL;
}