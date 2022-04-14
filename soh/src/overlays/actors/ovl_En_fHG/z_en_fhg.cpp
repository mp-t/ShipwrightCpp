/*
 * File: z_en_fhg.c
 * Overlay: ovl_En_fHG
 * Description: Phantom Ganon's Horse
 */

#include "z_en_fhg.h"
#include "objects/object_fhg/object_fhg.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"

#define FLAGS ACTOR_FLAG_4

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yRot;
} EnfHGPainting; // size = 0x10;

typedef enum {
    /*  0 */ INTRO_WAIT,
    /*  1 */ INTRO_START,
    /*  2 */ INTRO_FENCE,
    /*  3 */ INTRO_BACK,
    /*  4 */ INTRO_REVEAL,
    /*  5 */ INTRO_CUT,
    /*  6 */ INTRO_LAUGH,
    /*  7 */ INTRO_TITLE,
    /*  8 */ INTRO_RETREAT,
    /*  9 */ INTRO_FINISH,
    /* 15 */ INTRO_READY = 15
} EnfHGIntroState;

void EnfHG_Init(Actor* thisx, GlobalContext* globalCtx);
void EnfHG_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnfHG_Update(Actor* thisx, GlobalContext* globalCtx);
void EnfHG_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnfHG_SetupIntro(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_Intro(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_SetupApproach(EnfHG* thisv, GlobalContext* globalCtx, s16 paintingIndex);
void EnfHG_Approach(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_Attack(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_Damage(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_Retreat(EnfHG* thisv, GlobalContext* globalCtx);
void EnfHG_Done(EnfHG* thisv, GlobalContext* globalCtx);

const ActorInit En_fHG_InitVars = {
    ACTOR_EN_FHG,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_FHG,
    sizeof(EnfHG),
    (ActorFunc)EnfHG_Init,
    (ActorFunc)EnfHG_Destroy,
    (ActorFunc)EnfHG_Update,
    (ActorFunc)EnfHG_Draw,
    NULL,
};

static EnfHGPainting sPaintings[] = {
    { { 0.0f, 60.0f, -315.0f }, 0x0000 },   { { -260.0f, 60.0f, -145.0f }, 0x2AAA },
    { { -260.0f, 60.0f, 165.0f }, 0x5554 }, { { 0.0f, 60.0f, 315.0f }, 0x7FFE },
    { { 260.0f, 60.0f, 155.0f }, 0xAAA8 },  { { 260.0f, 60.0f, -155.0f }, 0xD552 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x1A, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_STOP),
};

void EnfHG_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnfHG* thisv = (EnfHG*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Flags_SetSwitch(globalCtx, 0x14);
    Actor_SetScale(&thisv->actor, 0.011499999f);
    thisv->actor.gravity = -3.5f;
    ActorShape_Init(&thisv->actor.shape, -2600.0f, NULL, 20.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    Skin_Init(globalCtx, &thisv->skin, &gPhantomHorseSkel, &gPhantomHorseRunningAnim);

    if (thisv->actor.params >= GND_FAKE_BOSS) {
        EnfHG_SetupApproach(thisv, globalCtx, thisv->actor.params - GND_FAKE_BOSS);
    } else {
        EnfHG_SetupIntro(thisv, globalCtx);
    }
}

void EnfHG_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnfHG* thisv = (EnfHG*)thisx;

    osSyncPrintf("F DT1\n");
    Skin_Free(globalCtx, &thisv->skin);
    osSyncPrintf("F DT2\n");
}

void EnfHG_SetupIntro(EnfHG* thisv, GlobalContext* globalCtx) {
    Animation_PlayLoop(&thisv->skin.skelAnime, &gPhantomHorseIdleAnim);
    thisv->actionFunc = EnfHG_Intro;
    thisv->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
    thisv->actor.world.pos.y = GND_BOSSROOM_CENTER_Y - 267.0f;
    thisv->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
}

void EnfHG_Intro(EnfHG* thisv, GlobalContext* globalCtx) {
    static Vec3f audioVec = { 0.0f, 0.0f, 50.0f };
    s32 pad64;
    Player* player = GET_PLAYER(globalCtx);
    BossGanondrof* bossGnd = (BossGanondrof*)thisv->actor.parent;
    s32 pad58;
    s32 pad54;

    if (thisv->cutsceneState != INTRO_FINISH) {
        SkelAnime_Update(&thisv->skin.skelAnime);
    }
    switch (thisv->cutsceneState) {
        case INTRO_WAIT:
            if ((fabsf(player->actor.world.pos.x - (GND_BOSSROOM_CENTER_X + 0.0f)) < 150.0f) &&
                (fabsf(player->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 0.0f)) < 150.0f)) {
                thisv->cutsceneState = INTRO_READY;
            }
            break;
        case INTRO_READY:
            if ((fabsf(player->actor.world.pos.x - (GND_BOSSROOM_CENTER_X + 0.0f)) < 100.0f) &&
                (fabsf(player->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 315.0f)) < 100.0f)) {
                thisv->cutsceneState = INTRO_START;
                if (gSaveContext.eventChkInf[7] & 4) {
                    thisv->timers[0] = 57;
                }
            }
            break;
        case INTRO_START:
            if (gSaveContext.eventChkInf[7] & 4) {
                if (thisv->timers[0] == 55) {
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_SHUTTER,
                                       GND_BOSSROOM_CENTER_X + 0.0f, GND_BOSSROOM_CENTER_Y - 97.0f,
                                       GND_BOSSROOM_CENTER_Z + 308.0f, 0, 0, 0, (SHUTTER_PG_BARS << 6));
                }
                if (thisv->timers[0] == 51) {
                    Audio_PlayActorSound2(thisv->actor.child, NA_SE_EV_SPEAR_FENCE);
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
                }
                if (thisv->timers[0] == 0) {
                    EnfHG_SetupApproach(thisv, globalCtx, Rand_ZeroOne() * 5.99f);
                    thisv->bossGndSignal = FHG_START_FIGHT;
                }
                break;
            }
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 8);
            thisv->cutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->cutsceneCamera, CAM_STAT_ACTIVE);
            thisv->cutsceneState = INTRO_FENCE;
            thisv->timers[0] = 60;
            thisv->actor.world.pos.y = GND_BOSSROOM_CENTER_Y - 7.0f;
            Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
            gSaveContext.eventChkInf[7] |= 4;
            Flags_SetSwitch(globalCtx, 0x23);
        case INTRO_FENCE:
            player->actor.world.pos.x = GND_BOSSROOM_CENTER_X + 0.0f;
            player->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 7.0f;
            player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z + 155.0f;
            player->actor.world.rot.y = player->actor.shape.rot.y = 0;
            player->actor.speedXZ = 0.0f;
            thisv->cameraEye.x = GND_BOSSROOM_CENTER_X + 0.0f;
            thisv->cameraEye.y = GND_BOSSROOM_CENTER_Y + 37.0f;
            thisv->cameraEye.z = GND_BOSSROOM_CENTER_Z + 170.0f;
            thisv->cameraAt.x = GND_BOSSROOM_CENTER_X + 0.0f;
            thisv->cameraAt.y = GND_BOSSROOM_CENTER_Y + 47.0f;
            thisv->cameraAt.z = GND_BOSSROOM_CENTER_Z + 315.0f;
            if (thisv->timers[0] == 25) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_SHUTTER,
                                   GND_BOSSROOM_CENTER_X + 0.0f, GND_BOSSROOM_CENTER_Y - 97.0f,
                                   GND_BOSSROOM_CENTER_Z + 308.0f, 0, 0, 0, (SHUTTER_PG_BARS << 6));
            }
            if (thisv->timers[0] == 21) {
                Audio_PlayActorSound2(thisv->actor.child, NA_SE_EV_SPEAR_FENCE);
            }
            if (thisv->timers[0] == 0) {
                thisv->cutsceneState = INTRO_BACK;
                thisv->timers[0] = 80;
            }
            break;
        case INTRO_BACK:
            if (thisv->timers[0] == 25) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GANON_HORSE_GROAN);
            }
            if (thisv->timers[0] == 20) {
                func_8002DF54(globalCtx, &thisv->actor, 9);
            }
            if (thisv->timers[0] == 1) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_OPENING_GANON);
            }
            Math_ApproachF(&thisv->cameraEye.x, GND_BOSSROOM_CENTER_X + 40.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraEye.y, GND_BOSSROOM_CENTER_Y + 37.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraEye.z, GND_BOSSROOM_CENTER_Z + 80.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.x, GND_BOSSROOM_CENTER_X - 100.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.y, GND_BOSSROOM_CENTER_Y + 47.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.z, GND_BOSSROOM_CENTER_Z + 335.0f, 0.05f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraSpeedMod, 1.0f, 1.0f, 0.01f);
            if (thisv->timers[0] == 0) {
                thisv->cutsceneState = INTRO_REVEAL;
                thisv->timers[0] = 50;
                thisv->cameraSpeedMod = 0.0f;
            }
            break;
        case INTRO_REVEAL:
            Math_ApproachF(&thisv->cameraEye.x, GND_BOSSROOM_CENTER_X + 70.0f, 0.1f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraEye.y, GND_BOSSROOM_CENTER_Y + 7.0f, 0.1f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraEye.z, GND_BOSSROOM_CENTER_Z + 200.0f, 0.1f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.x, GND_BOSSROOM_CENTER_X - 150.0f, 0.1f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.y, GND_BOSSROOM_CENTER_Y + 107.0f, 0.1f, thisv->cameraSpeedMod * 20.0f);
            Math_ApproachF(&thisv->cameraAt.z, GND_BOSSROOM_CENTER_Z - 65.0f, 0.1f, thisv->cameraSpeedMod * 40.0f);
            Math_ApproachF(&thisv->cameraSpeedMod, 1.0f, 1.0f, 0.05f);
            if (thisv->timers[0] == 5) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_SANDDUST);
            }
            if (thisv->timers[0] == 0) {
                thisv->cutsceneState = INTRO_CUT;
                thisv->timers[0] = 50;
                thisv->cameraSpeedMod = 0.0f;
            }
            break;
        case INTRO_CUT:
            thisv->cutsceneState = INTRO_LAUGH;
            thisv->cameraEye.x = GND_BOSSROOM_CENTER_X + 50.0f;
            thisv->cameraEye.y = GND_BOSSROOM_CENTER_Y + 17.0f;
            thisv->cameraEye.z = GND_BOSSROOM_CENTER_Z + 110.0f;
            thisv->cameraAt.x = GND_BOSSROOM_CENTER_X - 150.0f;
            thisv->cameraAt.y = GND_BOSSROOM_CENTER_Y + 207.0f;
            thisv->cameraAt.z = GND_BOSSROOM_CENTER_Z - 155.0f;
            thisv->cameraEyeVel.x = fabsf(thisv->cameraEye.x - (GND_BOSSROOM_CENTER_X + 20.0f));
            thisv->cameraEyeVel.y = fabsf(thisv->cameraEye.y - (GND_BOSSROOM_CENTER_Y + 102.0f));
            thisv->cameraEyeVel.z = fabsf(thisv->cameraEye.z - (GND_BOSSROOM_CENTER_Z + 25.0f));
            thisv->cameraAtVel.x = fabsf(thisv->cameraAt.x - (GND_BOSSROOM_CENTER_X - 150.0f));
            thisv->cameraAtVel.y = fabsf(thisv->cameraAt.y - (GND_BOSSROOM_CENTER_Y + 197.0f));
            thisv->cameraAtVel.z = fabsf(thisv->cameraAt.z - (GND_BOSSROOM_CENTER_Z - 65.0f));
            thisv->timers[0] = 250;
        case INTRO_LAUGH:
            Math_ApproachF(&thisv->cameraEye.x, GND_BOSSROOM_CENTER_X + 20.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.x);
            Math_ApproachF(&thisv->cameraEye.y, GND_BOSSROOM_CENTER_Y + 102.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.y);
            Math_ApproachF(&thisv->cameraEye.z, GND_BOSSROOM_CENTER_Z + 25.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.z);
            Math_ApproachF(&thisv->cameraAt.x, GND_BOSSROOM_CENTER_X - 150.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraAtVel.x);
            Math_ApproachF(&thisv->cameraAt.y, GND_BOSSROOM_CENTER_Y + 197.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraAtVel.y);
            Math_ApproachF(&thisv->cameraAt.z, GND_BOSSROOM_CENTER_Z - 65.0f, 0.05f,
                           thisv->cameraSpeedMod * thisv->cameraAtVel.z);
            Math_ApproachF(&thisv->cameraSpeedMod, 0.01f, 1.0f, 0.001f);
            if ((thisv->timers[0] == 245) || (thisv->timers[0] == 3)) {
                Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &gPhantomHorseRearingAnim, -8.0f);
                thisv->bossGndSignal = FHG_REAR;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GANON_HORSE_NEIGH);
                if (thisv->timers[0] == 3) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_VOICE);
                }
            }
            if (thisv->timers[0] == 192) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_SANDDUST);
            }
            if (thisv->timers[0] == 212) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_LAND2);
                Animation_Change(&thisv->skin.skelAnime, &gPhantomHorseIdleAnim, 0.3f, 0.0f, 5.0f, ANIMMODE_LOOP_INTERP,
                                 -10.0f);
            }
            if (thisv->timers[0] == 90) {
                globalCtx->envCtx.unk_BF = 2;
                globalCtx->envCtx.unk_D6 = 0x14;
            }
            if (thisv->timers[0] == 100) {
                thisv->bossGndSignal = FHG_LIGHTNING;
            }
            if (thisv->timers[0] == 60) {
                thisv->bossGndSignal = FHG_RIDE;
            }
            if (thisv->timers[0] == 130) {
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x5000FF);
            }
            if (thisv->timers[0] == 30) {
                bossGnd->work[GND_EYE_STATE] = GND_EYESTATE_BRIGHTEN;
            }
            if (thisv->timers[0] == 35) {
                func_80078914(&audioVec, NA_SE_EN_FANTOM_EYE);
            }
            if (thisv->timers[0] == 130) {
                bossGnd->work[GND_EYE_STATE] = GND_EYESTATE_FADE;
                func_80078914(&audioVec, NA_SE_EN_FANTOM_ST_LAUGH);
            }
            if (thisv->timers[0] == 20) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
            }
            if (thisv->timers[0] == 2) {
                thisv->cameraSpeedMod = 0.0f;
                thisv->cutsceneState = INTRO_TITLE;
                thisv->cameraEyeVel.x = fabsf(thisv->cameraEye.x - (GND_BOSSROOM_CENTER_X + 180.0f));
                thisv->cameraEyeVel.y = fabsf(thisv->cameraEye.y - (GND_BOSSROOM_CENTER_Y + 7.0f));
                thisv->cameraEyeVel.z = fabsf(thisv->cameraEye.z - (GND_BOSSROOM_CENTER_Z + 140.0f));
                thisv->timers[0] = 100;
                thisv->timers[1] = 34;
                thisv->cameraAtVel.x = fabsf(thisv->cameraAt.x - thisv->actor.world.pos.x);
                thisv->cameraAtVel.y = fabsf(thisv->cameraAt.y - ((thisv->actor.world.pos.y + 70.0f) - 20.0f));
                thisv->cameraAtVel.z = fabsf(thisv->cameraAt.z - thisv->actor.world.pos.z);
            }
            break;
        case INTRO_TITLE:
            if (thisv->timers[1] == 1) {
                Animation_Change(&thisv->skin.skelAnime, &gPhantomHorseIdleAnim, 0.5f, 0.0f,
                                 Animation_GetLastFrame(&gPhantomHorseIdleAnim), ANIMMODE_LOOP_INTERP, -3.0f);
            }
            Math_ApproachF(&thisv->cameraEye.x, GND_BOSSROOM_CENTER_X + 180.0f, 0.1f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.x);
            Math_ApproachF(&thisv->cameraEye.y, GND_BOSSROOM_CENTER_Y + 7.0f, 0.1f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.y);
            Math_ApproachF(&thisv->cameraEye.z, thisv->cameraPanZ + (GND_BOSSROOM_CENTER_Z + 140.0f), 0.1f,
                           thisv->cameraSpeedMod * thisv->cameraEyeVel.z);
            Math_ApproachF(&thisv->cameraPanZ, -100.0f, 0.1f, 1.0f);
            Math_ApproachF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.1f, thisv->cameraSpeedMod * 10.0f);
            Math_ApproachF(&thisv->cameraAt.y, (thisv->actor.world.pos.y + 70.0f) - 20.0f, 0.1f,
                           thisv->cameraSpeedMod * 10.0f);
            Math_ApproachF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.1f, thisv->cameraSpeedMod * 10.0f);
            Math_ApproachF(&thisv->actor.world.pos.y, 60.0f, 0.1f, 2.0f);
            thisv->actor.world.pos.y += 2.0f * Math_SinS(thisv->gallopTimer * 0x5DC);
            Math_ApproachF(&thisv->cameraSpeedMod, 1.0f, 1.0f, 0.05f);
            if (thisv->timers[0] == 75) {
                TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx,
                                       SEGMENTED_TO_VIRTUAL(gPhantomGanonTitleCardTex), 160, 180, 128, 40);
            }
            if (thisv->timers[0] == 0) {
                thisv->cutsceneState = INTRO_RETREAT;
                thisv->timers[0] = 200;
                thisv->timers[1] = 23;
                thisv->cameraSpeedMod = 0.0f;
                Animation_Change(&thisv->skin.skelAnime, &gPhantomHorseLeapAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gPhantomHorseLeapAnim), ANIMMODE_ONCE_INTERP, -4.0f);
                thisv->bossGndSignal = FHG_SPUR;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_VOICE);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            }
            break;
        case INTRO_RETREAT:
            if (thisv->timers[1] == 1) {
                Animation_Change(&thisv->skin.skelAnime, &gPhantomHorseLandAnim, 0.5f, 0.0f,
                                 Animation_GetLastFrame(&gPhantomHorseLandAnim), ANIMMODE_ONCE_INTERP, -3.0f);
                thisv->bossGndSignal = FHG_FINISH;
            }
            if (thisv->timers[0] == 170) {
                func_8002DF54(globalCtx, &thisv->actor, 8);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_MASIC2);
            }
            Math_ApproachF(&thisv->cameraEye.z, thisv->cameraPanZ + (GND_BOSSROOM_CENTER_Z + 100.0f), 0.1f,
                           thisv->cameraSpeedMod * 1.5f);
            Math_ApproachF(&thisv->cameraPanZ, -100.0f, 0.1f, 1.0f);
            Math_ApproachF(&thisv->actor.world.pos.z, GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f, 1.0f,
                           thisv->cameraSpeedMod * 10.0f);
            Math_ApproachF(&thisv->cameraSpeedMod, 1.0f, 1.0f, 0.05f);
            if ((fabsf(thisv->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f)) < 300.0f) &&
                !thisv->spawnedWarp) {
                thisv->spawnedWarp = true;
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                   GND_BOSSROOM_CENTER_X + 0.0f, thisv->actor.world.pos.y + 50.0f,
                                   GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f, 0, thisv->actor.shape.rot.y, 0,
                                   FHGFIRE_WARP_RETREAT);
                thisv->fhgFireKillWarp = true;
            }
            Math_ApproachF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.2f, 50.0f);
            Math_ApproachF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.2f, 50.0f);
            osSyncPrintf("TIME %d-------------------------------------------------\n", thisv->timers[0]);
            if (fabsf(thisv->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f)) < 1.0f) {
                globalCtx->envCtx.unk_BF = 0;
                globalCtx->envCtx.unk_D6 = 0x14;
                thisv->cutsceneState = INTRO_FINISH;
                Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseRunningAnim, -3.0f);
                thisv->bossGndSignal = FHG_START_FIGHT;
                thisv->timers[1] = 75;
                thisv->timers[0] = 140;
            }
            break;
        case INTRO_FINISH:
            EnfHG_Retreat(thisv, globalCtx);
            Math_ApproachF(&thisv->cameraEye.z, thisv->cameraPanZ + (GND_BOSSROOM_CENTER_Z + 100.0f), 0.1f,
                           thisv->cameraSpeedMod * 1.5f);
            Math_ApproachF(&thisv->cameraPanZ, -100.0f, 0.1f, 1.0f);
            Math_ApproachF(&thisv->cameraAt.y, (thisv->actor.world.pos.y + 70.0f) - 20.0f, 0.1f,
                           thisv->cameraSpeedMod * 10.0f);
            if (thisv->timers[1] == 0) {
                Camera* camera = Gameplay_GetCamera(globalCtx, 0);

                camera->eye = thisv->cameraEye;
                camera->eyeNext = thisv->cameraEye;
                camera->at = thisv->cameraAt;
                func_800C08AC(globalCtx, thisv->cutsceneCamera, 0);
                thisv->cutsceneCamera = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                thisv->actionFunc = EnfHG_Retreat;
            }
            break;
    }
    if (thisv->cutsceneCamera != 0) {
        Gameplay_CameraSetAtEye(globalCtx, thisv->cutsceneCamera, &thisv->cameraAt, &thisv->cameraEye);
    }
}

void EnfHG_SetupApproach(EnfHG* thisv, GlobalContext* globalCtx, s16 paintingIndex) {
    s16 oppositeIndex[6] = { 3, 4, 5, 0, 1, 2 };

    Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
    thisv->actionFunc = EnfHG_Approach;
    thisv->curPainting = paintingIndex;
    thisv->targetPainting = oppositeIndex[thisv->curPainting];

    osSyncPrintf("KABE NO 1 = %d\n", thisv->curPainting);
    osSyncPrintf("KABE NO 2 = %d\n", thisv->targetPainting);

    thisv->actor.world.pos.x = (1.3f * sPaintings[thisv->curPainting].pos.x) + (GND_BOSSROOM_CENTER_X - 4.0f);
    thisv->actor.world.pos.y = sPaintings[thisv->curPainting].pos.y + (GND_BOSSROOM_CENTER_Y + 153.0f);
    thisv->actor.world.pos.z = (1.3f * sPaintings[thisv->curPainting].pos.z) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
    thisv->actor.world.rot.y = sPaintings[thisv->curPainting].yRot;

    osSyncPrintf("XP1  = %f\n", thisv->actor.world.pos.x);
    osSyncPrintf("ZP1  = %f\n", thisv->actor.world.pos.z);

    thisv->inPaintingPos.x = (sPaintings[thisv->targetPainting].pos.x * 1.3f) + (GND_BOSSROOM_CENTER_X - 4.0f);
    thisv->inPaintingPos.y = sPaintings[thisv->targetPainting].pos.y + (GND_BOSSROOM_CENTER_Y + 33.0f);
    thisv->inPaintingPos.z = (sPaintings[thisv->targetPainting].pos.z * 1.3f) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
    thisv->inPaintingVelX = (fabsf(thisv->inPaintingPos.x - thisv->actor.world.pos.x) * 2) * 0.01f;

    if (thisv->inPaintingVelX < 1.0f) {
        thisv->inPaintingVelX = 1.0f;
    }
    thisv->inPaintingVelZ = (fabsf(thisv->inPaintingPos.z - thisv->actor.world.pos.z) * 2) * 0.01f;
    if (thisv->inPaintingVelZ < 1.0f) {
        thisv->inPaintingVelZ = 1.0f;
    }

    thisv->timers[0] = 100;
    thisv->actor.scale.x = 0.002f;
    thisv->actor.scale.y = 0.002f;
    thisv->actor.scale.z = 0.001f;
    thisv->approachRate = 0.0f;

    thisv->warpColorFilterR = globalCtx->lightCtx.fogColor[0];
    thisv->warpColorFilterG = globalCtx->lightCtx.fogColor[1];
    thisv->warpColorFilterB = globalCtx->lightCtx.fogColor[2];
    thisv->warpColorFilterUnk1 = 0.0f;
    thisv->warpColorFilterUnk2 = 0.0f;
    thisv->turnRot = 0;
    thisv->turnTarget = 0;
    thisv->spawnedWarp = false;
}

void EnfHG_Approach(EnfHG* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("STANDBY !!\n");
    osSyncPrintf("XP2  = %f\n", thisv->actor.world.pos.x);
    osSyncPrintf("ZP2  = %f\n", thisv->actor.world.pos.z);
    if (thisv->actor.params == GND_REAL_BOSS) {
        thisv->hoofSfxPos.x = thisv->actor.projectedPos.x / (thisv->actor.scale.x * 100.0f);
        thisv->hoofSfxPos.y = thisv->actor.projectedPos.y / (thisv->actor.scale.x * 100.0f);
        thisv->hoofSfxPos.z = thisv->actor.projectedPos.z / (thisv->actor.scale.x * 100.0f);
        if ((thisv->gallopTimer % 8) == 0) {
            func_80078914(&thisv->hoofSfxPos, NA_SE_EV_HORSE_RUN);
        }
    }
    SkelAnime_Update(&thisv->skin.skelAnime);
    Math_ApproachF(&thisv->actor.scale.x, 0.011499999f, 1.0f, thisv->approachRate);
    Math_ApproachF(&thisv->approachRate, 0.0002f, 1.0f, 0.0000015f);
    Math_ApproachF(&thisv->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
    thisv->actor.scale.y = thisv->actor.scale.x;
    if (thisv->timers[0] == 0) {
        osSyncPrintf("arg_data ------------------------------------>%d\n", thisv->actor.params);
        if (thisv->actor.params != GND_REAL_BOSS) {
            thisv->timers[0] = 140;
            thisv->actionFunc = EnfHG_Retreat;
            Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
            thisv->turnTarget = -0x8000;
        } else {
            thisv->actionFunc = EnfHG_Attack;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            thisv->timers[0] = 40;
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                               thisv->actor.world.pos.x, thisv->actor.world.pos.y + 50.0f, thisv->actor.world.pos.z, 0,
                               thisv->actor.shape.rot.y + 0x8000, 0, FHGFIRE_WARP_EMERGE);
            thisv->fhgFireKillWarp = false;
        }
    }
}

void EnfHG_Attack(EnfHG* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("KABE OUT !!\n");
    thisv->bossGndInPainting = false;
    SkelAnime_Update(&thisv->skin.skelAnime);
    if (thisv->timers[0] != 0) {
        Math_ApproachF(&thisv->actor.scale.z, 0.011499999f, 1.0f, 0.0002f);
        if (thisv->timers[0] == 1) {
            thisv->bossGndSignal = FHG_RAISE_SPEAR;
            thisv->timers[1] = 50;
            Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &gPhantomHorseLeapAnim, 0.0f);
        }
        Math_ApproachF(&thisv->warpColorFilterR, 255.0f, 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterG, 255.0f, 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterB, 255.0f, 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterUnk1, -60.0f, 1.0f, 5.0f);
    } else {
        Math_ApproachF(&thisv->warpColorFilterR, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterG, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterB, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
        Math_ApproachF(&thisv->warpColorFilterUnk1, 0.0f, 1.0f, 5.0f);
        if (thisv->timers[1] == 29) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_MASIC2);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_VOICE);
        }
        if (thisv->hitTimer == 0) {
            if (thisv->timers[1] == 24) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                   thisv->actor.world.pos.x, (thisv->actor.world.pos.y + 100.0f) + 25.0f,
                                   thisv->actor.world.pos.z, 0, 0, 0, FHGFIRE_LIGHTNING_STRIKE);
            }
            if (thisv->timers[1] == 45) {
                Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseAirAnim, 0.0f);
            }
            if (thisv->timers[1] == 38) {
                thisv->bossGndSignal = FHG_LIGHTNING;
            }
            if (thisv->timers[1] == 16) {
                Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &gPhantomHorseLandAnim, 0.0f);
                thisv->bossGndSignal = FHG_RESET;
            }
        }
        Math_ApproachF(&thisv->actor.scale.z, 0.011499999f, 1.0f, 0.002f);
        Math_ApproachF(&thisv->actor.world.pos.x, thisv->inPaintingPos.x, 1.0f, thisv->inPaintingVelX);
        Math_ApproachF(&thisv->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
        Math_ApproachF(&thisv->actor.world.pos.z, thisv->inPaintingPos.z, 1.0f, thisv->inPaintingVelZ);
    }
    if (thisv->hitTimer == 20) {
        thisv->actionFunc = EnfHG_Damage;
        thisv->spawnedWarp = false;
        Animation_Change(&thisv->skin.skelAnime, &gPhantomHorseLandAnim, -1.0f, 0.0f,
                         Animation_GetLastFrame(&gPhantomHorseLandAnim), ANIMMODE_ONCE, -5.0f);
        thisv->timers[0] = 10;
        thisv->bossGndSignal = FHG_RESET;
        thisv->damageSpeedMod = 1.0f;
    } else {
        f32 dx = thisv->actor.world.pos.x - thisv->inPaintingPos.x;
        f32 dz = thisv->actor.world.pos.z - thisv->inPaintingPos.z;
        f32 dxz = sqrtf(SQ(dx) + SQ(dz));

        if (dxz < 350.0f) {
            thisv->bossGndInPainting = true;
        }
        if ((dxz < 300.0f) && !thisv->spawnedWarp) {
            thisv->spawnedWarp = true;
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->inPaintingPos.x,
                               thisv->actor.world.pos.y + 50.0f, thisv->inPaintingPos.z, 0, thisv->actor.shape.rot.y, 0,
                               FHGFIRE_WARP_RETREAT);
            thisv->fhgFireKillWarp = true;
        }
        osSyncPrintf("SPD X %f\n", thisv->inPaintingVelX);
        osSyncPrintf("SPD Z %f\n", thisv->inPaintingVelZ);
        osSyncPrintf("X=%f\n", dx);
        osSyncPrintf("Z=%f\n", dz);
        if (dxz == 0.0f) {
            thisv->timers[0] = 140;
            thisv->actionFunc = EnfHG_Retreat;
            Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
            thisv->bossGndSignal = FHG_RIDE;
        }
    }
}

void EnfHG_Damage(EnfHG* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;
    f32 dxz2;

    osSyncPrintf("REVISE !!\n");
    SkelAnime_Update(&thisv->skin.skelAnime);
    Math_ApproachF(&thisv->warpColorFilterR, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
    Math_ApproachF(&thisv->warpColorFilterG, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
    Math_ApproachF(&thisv->warpColorFilterB, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
    Math_ApproachF(&thisv->warpColorFilterUnk1, 0.0f, 1.0f, 5.0f);
    Math_ApproachF(&thisv->actor.scale.z, 0.011499999f, 1.0f, 0.002f);
    if (thisv->timers[0] != 0) {
        Math_ApproachZeroF(&thisv->damageSpeedMod, 1.0f, 0.1f);
        if (thisv->timers[0] == 1) {
            thisv->targetPainting = thisv->curPainting;
            thisv->inPaintingPos.x = (sPaintings[thisv->targetPainting].pos.x * 1.3f) + (GND_BOSSROOM_CENTER_X - 4.0f);
            thisv->inPaintingPos.y = sPaintings[thisv->targetPainting].pos.y;
            thisv->inPaintingPos.z = (sPaintings[thisv->targetPainting].pos.z * 1.3f) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
        }
    } else {
        Math_ApproachF(&thisv->damageSpeedMod, 1.0f, 1.0f, 0.1f);
    }
    Math_ApproachF(&thisv->actor.world.pos.x, thisv->inPaintingPos.x, 1.0f, thisv->damageSpeedMod * thisv->inPaintingVelX);
    Math_ApproachF(&thisv->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->inPaintingPos.z, 1.0f, thisv->damageSpeedMod * thisv->inPaintingVelZ);
    dx = thisv->actor.world.pos.x - thisv->inPaintingPos.x;
    dz = thisv->actor.world.pos.z - thisv->inPaintingPos.z;
    dxz2 = sqrtf(SQ(dx) + SQ(dz));
    if ((dxz2 < 300.0f) && (!thisv->spawnedWarp)) {
        thisv->spawnedWarp = true;
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE, thisv->inPaintingPos.x,
                           thisv->actor.world.pos.y + 50.0f, thisv->inPaintingPos.z, 0, thisv->actor.shape.rot.y + 0x8000,
                           0, FHGFIRE_WARP_RETREAT);
    }
    if (dxz2 == 0.0f) {
        BossGanondrof* bossGnd = (BossGanondrof*)thisv->actor.parent;

        thisv->timers[0] = 140;
        thisv->actionFunc = EnfHG_Retreat;
        Animation_MorphToLoop(&thisv->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
        if (bossGnd->actor.colChkInfo.health > 24) {
            thisv->bossGndSignal = FHG_RIDE;
        } else {
            bossGnd->flyMode = GND_FLY_NEUTRAL;
        }
        thisv->turnTarget = -0x8000;
    }
}

void EnfHG_Retreat(EnfHG* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("KABE IN !!\n");
    if (thisv->turnTarget != 0) {
        Math_ApproachS(&thisv->turnRot, thisv->turnTarget, 5, 2000);
    }
    if (thisv->actor.params == GND_REAL_BOSS) {
        thisv->hoofSfxPos.x = thisv->actor.projectedPos.x / (thisv->actor.scale.x * 100.0f);
        thisv->hoofSfxPos.y = thisv->actor.projectedPos.y / (thisv->actor.scale.x * 100.0f);
        thisv->hoofSfxPos.z = thisv->actor.projectedPos.z / (thisv->actor.scale.x * 100.0f);
        if ((thisv->gallopTimer % 8) == 0) {
            func_80078914(&thisv->hoofSfxPos, NA_SE_EV_HORSE_RUN);
        }
    }
    SkelAnime_Update(&thisv->skin.skelAnime);
    Math_ApproachF(&thisv->actor.scale.z, 0.001f, 1.0f, 0.001f);
    Math_ApproachF(&thisv->actor.scale.x, 0.002f, 0.05f, 0.0001f);
    Math_ApproachF(&thisv->actor.world.pos.y, 200.0f, 0.05f, 1.0f);
    thisv->actor.scale.y = thisv->actor.scale.x;
    if ((thisv->timers[0] == 80) && (thisv->actor.params == GND_REAL_BOSS)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_LAUGH);
    }
    if (thisv->timers[0] == 0) {
        BossGanondrof* bossGnd = (BossGanondrof*)thisv->actor.parent;
        s16 paintingIdxReal;
        s16 paintingIdxFake;

        if (thisv->actor.params != GND_REAL_BOSS) {
            thisv->killActor = true;
            bossGnd->killActor = true;
        } else if (bossGnd->flyMode != GND_FLY_PAINTING) {
            thisv->actionFunc = EnfHG_Done;
            thisv->actor.draw = NULL;
        } else {
            paintingIdxReal = Rand_ZeroOne() * 5.99f;
            EnfHG_SetupApproach(thisv, globalCtx, paintingIdxReal);
            do {
                paintingIdxFake = Rand_ZeroOne() * 5.99f;
            } while (paintingIdxFake == paintingIdxReal);
            osSyncPrintf("ac1 = %x `````````````````````````````````````````````````\n",
                         Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_GANONDROF,
                                            thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                            0, 0, 0, paintingIdxFake + GND_FAKE_BOSS));
        }
    }
}

void EnfHG_Done(EnfHG* thisv, GlobalContext* globalCtx) {
    thisv->bossGndInPainting = false;
}

void EnfHG_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnfHG* thisv = (EnfHG*)thisx;
    u8 i;

    if (thisv->killActor) {
        Actor_Kill(&thisv->actor);
        return;
    }
    thisv->gallopTimer++;
    thisv->bossGndInPainting = true;
    for (i = 0; i < 5; i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->hitTimer != 0) {
        thisv->hitTimer--;
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    thisv->actor.shape.yOffset = Math_SinS(thisv->hitTimer * 0x9000) * 700.0f * (thisv->hitTimer / 20.0f);
    thisv->actor.shape.rot.z = (s16)(Math_SinS(thisv->hitTimer * 0x7000) * 1500.0f) * (thisv->hitTimer / 20.0f);
}

void EnfHG_PostDraw(Actor* thisx, GlobalContext* globalCtx, Skin* skin) {
}

void EnfHG_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnfHG* thisv = (EnfHG*)thisx;
    BossGanondrof* bossGnd = (BossGanondrof*)thisv->actor.parent;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fhg.c", 2439);
    func_80093D18(globalCtx->state.gfxCtx);

    POLY_OPA_DISP = ((bossGnd->work[GND_INVINC_TIMER] & 4) && (bossGnd->flyMode == GND_FLY_PAINTING))
                        ? Gfx_SetFog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099)
                        : Gfx_SetFog(POLY_OPA_DISP, (u32)thisv->warpColorFilterR, (u32)thisv->warpColorFilterG,
                                     (u32)thisv->warpColorFilterB, 0, (s32)thisv->warpColorFilterUnk1 + 995,
                                     (s32)thisv->warpColorFilterUnk2 + 1000);
    func_800A6330(&thisv->actor, globalCtx, &thisv->skin, EnfHG_PostDraw, SKIN_TRANSFORM_IS_FHG);
    POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fhg.c", 2480);
}
