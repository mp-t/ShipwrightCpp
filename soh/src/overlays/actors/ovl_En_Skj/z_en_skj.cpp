#include "z_en_skj.h"
#include "overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.h"
#include "objects/object_skj/object_skj.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnSkj_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSkj_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSkj_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSkj_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnSkj_SariasSongShortStumpUpdate(Actor* thisx, GlobalContext* globalCtx);
void EnSkj_OcarinaMinigameShortStumpUpdate(Actor* thisx, GlobalContext* globalCtx);

void func_80AFF2A0(EnSkj* thisv);
void func_80AFF334(EnSkj* thisv);

void EnSkj_CalculateCenter(EnSkj* thisv);
void EnSkj_OcarinaGameSetupWaitForPlayer(EnSkj* thisv);
void EnSkj_SetupResetFight(EnSkj* thisv);
void EnSkj_SetupLeaveOcarinaGame(EnSkj* thisv);
void EnSkj_SetupPlayOcarinaGame(EnSkj* thisv);
void EnSkj_Backflip(EnSkj* thisv);
void EnSkj_SetupNeedleRecover(EnSkj* thisv);
void EnSkj_SetupSpawnDeathEffect(EnSkj* thisv);
void EnSkj_SetupStand(EnSkj* thisv);
void EnSkj_SetupWaitForSong(EnSkj* thisv);
void EnSkj_SetupTalk(EnSkj* thisv);
void EnSkj_SetupMaskTrade(EnSkj* thisv);
void EnSkj_SetupWrongSong(EnSkj* thisv);
void EnSkj_SetupAfterSong(EnSkj* thisv);
void func_80AFFE24(EnSkj* thisv);
void EnSkj_SetupPostSariasSong(EnSkj* thisv);
void EnSkj_JumpFromStump(EnSkj* thisv);
void EnSkj_SetupWaitForLandAnimFinish(EnSkj* thisv);
void EnSkj_SetupWalkToPlayer(EnSkj* thisv);
void EnSkj_SetupWaitForMaskTextClear(EnSkj* thisv);
void EnSkj_SetupWaitForTextClear(EnSkj* thisv);
void EnSkj_SetupDie(EnSkj* thisv);
void func_80AFF1F0(EnSkj* thisv);
void EnSkj_OfferNextRound(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SetupAskForMask(EnSkj* thisv, GlobalContext* globalCtx);
f32 EnSkj_GetItemXzRange(EnSkj* thisv);
s32 EnSkj_CollisionCheck(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SetupTakeMask(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_TurnPlayer(EnSkj* thisv, Player* player);

void EnSkj_SetupWaitForOcarina(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_StartOcarinaMinigame(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForOcarina(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForPlayback(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_FailedMiniGame(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WonOcarinaMiniGame(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitToGiveReward(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_GiveOcarinaGameReward(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_FinishOcarinaGameRound(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForNextRound(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForOfferResponse(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SetupWaitForOcarina(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_CleanupOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx);

void EnSkj_Fade(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitToShootNeedle(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SariasSongKidIdle(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForDeathAnim(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_PickNextFightAction(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForLandAnim(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_ResetFight(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_Fight(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_NeedleRecover(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SpawnDeathEffect(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitInRange(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForSong(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_AfterSong(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SariaSongTalk(EnSkj* thisv, GlobalContext* globalCtx);
void func_80AFFE44(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_ChangeModeAfterSong(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_StartMaskTrade(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForLanding(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForLandAnimFinish(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WalkToPlayer(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_AskForMask(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_TakeMask(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WaitForMaskTextClear(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_WrongSong(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_SariasSongWaitForTextClear(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_OcarinaGameWaitForPlayer(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_OcarinaGameIdle(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_PlayOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx);
void EnSkj_LeaveOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx);

void EnSkj_SpawnBlood(GlobalContext* globalCtx, Vec3f* pos);

void EnSkj_SetupWaitInRange(EnSkj* thisv);

#define songFailTimer multiuseTimer
#define battleExitTimer multiuseTimer

typedef enum {
    /* 0 */ SKJ_ANIM_BACKFLIP,
    /* 1 */ SKJ_ANIM_SHOOT_NEEDLE,
    /* 2 */ SKJ_ANIM_PLAY_FLUTE,
    /* 3 */ SKJ_ANIM_DIE,
    /* 4 */ SKJ_ANIM_HIT,
    /* 5 */ SKJ_ANIM_LAND,
    /* 6 */ SKJ_ANIM_LOOK_LEFT_RIGHT,
    /* 7 */ SKJ_ANIM_FIGHTING_STANCE,
    /* 8 */ SKJ_ANIM_WALK_TO_PLAYER,
    /* 9 */ SKJ_ANIM_WAIT
} SkullKidAnim;

typedef enum {
    /* 0 */ SKULL_KID_LEFT,
    /* 1 */ SKULL_KID_RIGHT
} SkullKidStumpSide;

typedef enum {
    /* 0 */ SKULL_KID_OCARINA_WAIT,
    /* 1 */ SKULL_KID_OCARINA_PLAY_NOTES,
    /* 2 */ SKULL_KID_OCARINA_LEAVE_GAME
} SkullKidOcarinaGameState;

typedef enum {
    /* 00 */ SKJ_ACTION_FADE,
    /* 01 */ SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE,
    /* 02 */ SKJ_ACTION_SARIA_SONG_IDLE,
    /* 03 */ SKJ_ACTION_WAIT_FOR_DEATH_ANIM,
    /* 04 */ SKJ_ACTION_PICK_NEXT_FIHGT_ACTION,
    /* 05 */ SKJ_ACTION_WAIT_FOR_LAND_ANIM,
    /* 06 */ SKJ_ACTION_RESET_FIGHT,
    /* 07 */ SKJ_ACTION_FIGHT,
    /* 08 */ SKJ_ACTION_NEEDLE_RECOVER,
    /* 09 */ SKJ_ACTION_SPAWN_DEATH_EFFECT,
    /* 10 */ SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE,
    /* 11 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG,
    /* 12 */ SKJ_ACTION_SARIA_SONG_AFTER_SONG,
    /* 13 */ SKJ_ACTION_SARIA_TALK,
    /* 14 */ SKJ_ACTION_UNK14,
    /* 15 */ SKJ_ACTION_SARIA_SONG_CHANGE_MODE,
    /* 16 */ SKJ_ACTION_SARIA_SONG_START_TRADE,
    /* 17 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING,
    /* 18 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM,
    /* 19 */ SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER,
    /* 20 */ SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK,
    /* 21 */ SKJ_ACTION_SARIA_SONG_TAKE_MASK,
    /* 22 */ SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT,
    /* 23 */ SKJ_ACTION_SARIA_SONG_WRONG_SONG,
    /* 24 */ SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT,
    /* 25 */ SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER,
    /* 26 */ SKJ_ACTION_OCARINA_GAME_IDLE,
    /* 27 */ SKJ_ACTION_OCARINA_GAME_PLAY,
    /* 28 */ SKJ_ACTION_OCARINA_GAME_LEAVE
} SkullKidAction;

typedef struct {
    u8 unk0;
    EnSkj* skullkid;
} EnSkjUnkStruct;

static EnSkjUnkStruct sSmallStumpSkullKid = { 0, NULL };
static EnSkjUnkStruct sOcarinaMinigameSkullKids[] = { { 0, NULL }, { 0, NULL } };

ActorInit En_Skj_InitVars = {
    ACTOR_EN_SKJ,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_SKJ,
    sizeof(EnSkj),
    (ActorFunc)EnSkj_Init,
    (ActorFunc)EnSkj_Destroy,
    (ActorFunc)EnSkj_Update,
    (ActorFunc)EnSkj_Draw,
    NULL,
};

static ColliderCylinderInitType1 D_80B01678 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x0, 0x08 },
        { 0xFFCFFFFF, 0x0, 0x0 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 8, 48, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 sOcarinaGameRewards[] = {
    GI_RUPEE_GREEN,
    GI_RUPEE_BLUE,
    GI_HEART_PIECE,
    GI_RUPEE_RED,
};

static AnimationMinimalInfo sAnimationInfo[] = {
    { &gSkullKidBackflipAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidShootNeedleAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidPlayFluteAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidDieAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidHitAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLandAnim, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidLookLeftAndRightAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidFightingStanceAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWalkToPlayerAnim, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidWaitAnim, ANIMMODE_LOOP, 0.0f },
};

static EnSkjActionFunc sActionFuncs[] = {
    EnSkj_Fade,
    EnSkj_WaitToShootNeedle,
    EnSkj_SariasSongKidIdle,
    EnSkj_WaitForDeathAnim,
    EnSkj_PickNextFightAction,
    EnSkj_WaitForLandAnim,
    EnSkj_ResetFight,
    EnSkj_Fight,
    EnSkj_NeedleRecover,
    EnSkj_SpawnDeathEffect,
    EnSkj_WaitInRange,
    EnSkj_WaitForSong,
    EnSkj_AfterSong,
    EnSkj_SariaSongTalk,
    func_80AFFE44,
    EnSkj_ChangeModeAfterSong,
    EnSkj_StartMaskTrade,
    EnSkj_WaitForLanding,
    EnSkj_WaitForLandAnimFinish,
    EnSkj_WalkToPlayer,
    EnSkj_AskForMask,
    EnSkj_TakeMask,
    EnSkj_WaitForMaskTextClear,
    EnSkj_WrongSong,
    EnSkj_SariasSongWaitForTextClear,
    EnSkj_OcarinaGameWaitForPlayer,
    EnSkj_OcarinaGameIdle,
    EnSkj_PlayOcarinaGame,
    EnSkj_LeaveOcarinaGame,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

static s32 D_80B01EA0; // gets set if ACTOR_FLAG_8 is set

void EnSkj_ChangeAnim(EnSkj* thisv, u8 index) {
    f32 endFrame = Animation_GetLastFrame(sAnimationInfo[index].animation);

    thisv->animIndex = index;
    Animation_Change(&thisv->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, endFrame,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

void EnSkj_SetupAction(EnSkj* thisv, u8 action) {
    thisv->action = action;
    thisv->actionFunc = sActionFuncs[action];

    switch (action) {
        case SKJ_ACTION_FADE:
        case SKJ_ACTION_WAIT_FOR_DEATH_ANIM:
        case SKJ_ACTION_PICK_NEXT_FIHGT_ACTION:
        case SKJ_ACTION_SPAWN_DEATH_EFFECT:
        case SKJ_ACTION_SARIA_SONG_START_TRADE:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM:
        case SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER:
        case SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK:
        case SKJ_ACTION_SARIA_SONG_TAKE_MASK:
        case SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT:
        case SKJ_ACTION_SARIA_SONG_WRONG_SONG:
        case SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT:
        case SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER:
        case SKJ_ACTION_OCARINA_GAME_IDLE:
        case SKJ_ACTION_OCARINA_GAME_PLAY:
        case SKJ_ACTION_OCARINA_GAME_LEAVE:
            thisv->unk_2D3 = 0;
            break;
        default:
            thisv->unk_2D3 = 1;
            break;
    }
}

void EnSkj_CalculateCenter(EnSkj* thisv) {
    Vec3f mult;

    mult.x = 0.0f;
    mult.y = 0.0f;
    mult.z = 120.0f;

    Matrix_RotateY((thisv->actor.shape.rot.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
    Matrix_MultVec3f(&mult, &thisv->center);

    thisv->center.x += thisv->actor.world.pos.x;
    thisv->center.z += thisv->actor.world.pos.z;
}

void EnSkj_SetNaviId(EnSkj* thisv) {
    switch (thisv->actor.params) {
        case 0:
            if (gSaveContext.itemGetInf[3] & 0x200) {
                thisv->actor.naviEnemyId = 0x41; // Skull kid with skull mask
            } else if (gSaveContext.itemGetInf[1] & 0x40) {
                thisv->actor.naviEnemyId = 0x40; // Skull kid after Saria's song but no mask
            } else {
                thisv->actor.naviEnemyId = 0x3F; // No Sarias song no skull mask
            }
            break;

        case 1:
        case 2:
            thisv->actor.naviEnemyId = 0x3F;
            break;

        default:
            thisv->actor.naviEnemyId = 0x36; // Skull kid as adult
            break;
    }
}

void EnSkj_Init(Actor* thisx, GlobalContext* globalCtx2) {
    s16 type = (thisx->params >> 0xA) & 0x3F;
    EnSkj* thisv = (EnSkj*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s32 pad;
    Player* player;

    Actor_ProcessInitChain(thisx, sInitChain);
    switch (type) {
        case 5: // Invisible on the small stump (sarias song))
            sSmallStumpSkullKid.unk0 = 1;
            sSmallStumpSkullKid.skullkid = (EnSkj*)thisx;
            thisv->actor.destroy = NULL;
            thisv->actor.draw = NULL;
            thisv->actor.update = EnSkj_SariasSongShortStumpUpdate;
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            thisv->actor.flags |= 0;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_PROP);
            break;

        case 6: // Invisible on the short stump (ocarina game)
            sSmallStumpSkullKid.unk0 = 1;
            sSmallStumpSkullKid.skullkid = (EnSkj*)thisx;
            thisv->actor.destroy = NULL;
            thisv->actor.draw = NULL;
            thisv->actor.update = EnSkj_OcarinaMinigameShortStumpUpdate;
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            thisv->actor.flags |= 0;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_PROP);
            thisv->actor.focus.pos.x = 1230.0f;
            thisv->actor.focus.pos.y = -90.0f;
            thisv->actor.focus.pos.z = 450.0f;
            thisv->actionFunc = EnSkj_SetupWaitForOcarina;
            break;

        default:
            thisv->actor.params = type;
            if (((thisv->actor.params != 0) && (thisv->actor.params != 1)) && (thisv->actor.params != 2)) {
                if (INV_CONTENT(ITEM_TRADE_ADULT) < ITEM_SAW) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            }

            EnSkj_SetNaviId(thisv);
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSkullKidSkel, &gSkullKidPlayFluteAnim, thisv->jointTable,
                               thisv->morphTable, 19);
            if ((type >= 0) && (type < 3)) {
                thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
                thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
                Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_NPC);
            }

            if ((type < 0) || (type >= 7)) {
                thisv->actor.flags &= ~ACTOR_FLAG_25;
            }

            if ((type > 0) && (type < 3)) {
                thisv->actor.targetMode = 7;
                thisv->posCopy = thisv->actor.world.pos;
                sOcarinaMinigameSkullKids[type - 1].unk0 = 1;
                sOcarinaMinigameSkullKids[type - 1].skullkid = thisv;
                thisv->minigameState = 0;
                thisv->alpha = 0;
                EnSkj_OcarinaGameSetupWaitForPlayer(thisv);
            } else {
                thisv->alpha = 255;
                EnSkj_SetupResetFight(thisv);
            }

            thisv->actor.colChkInfo.damageTable = &sDamageTable;
            thisv->actor.colChkInfo.health = 10;
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &D_80B01678);
            ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
            Actor_SetScale(thisx, 0.01f);
            thisv->actor.textId = thisv->textId = 0;
            thisv->multiuseTimer = 0;
            thisv->backflipFlag = 0;
            thisv->needlesToShoot = 3;
            thisv->hitsUntilDodge = 3;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.gravity = -1.0f;
            EnSkj_CalculateCenter(thisv);

            player = GET_PLAYER(globalCtx);
            osSyncPrintf("Player_X : %f\n", player->actor.world.pos.x);
            osSyncPrintf("Player_Z : %f\n", player->actor.world.pos.z);
            osSyncPrintf("World_X  : %f\n", thisv->actor.world.pos.x);
            osSyncPrintf("World_Z  : %f\n", thisv->actor.world.pos.z);
            osSyncPrintf("Center_X : %f\n", thisv->center.x);
            osSyncPrintf("Center_Z : %f\n\n", thisv->center.z);

            break;
    }
}

void EnSkj_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSkj* thisv = (EnSkj*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 EnSkj_RangeCheck(Player* player, EnSkj* thisv) {
    f32 xDiff = player->actor.world.pos.x - thisv->actor.world.pos.x;
    f32 zDiff = player->actor.world.pos.z - thisv->actor.world.pos.z;
    f32 yDiff = player->actor.world.pos.y - thisv->actor.world.pos.y;

    return (SQ(xDiff) + SQ(zDiff) <= 676.0f) && (yDiff >= 0.0f);
}

f32 EnSkj_GetItemXzRange(EnSkj* thisv) {
    EnSkj* temp_v0;
    f32 zDiff;
    f32 xDiff;

    temp_v0 = sSmallStumpSkullKid.skullkid;
    xDiff = temp_v0->actor.world.pos.x - thisv->actor.world.pos.x;
    zDiff = temp_v0->actor.world.pos.z - thisv->actor.world.pos.z;
    return sqrtf(SQ(xDiff) + SQ(zDiff)) + 26.0f;
}

f32 EnSkj_GetItemYRange(EnSkj* thisv) {
    return fabsf(sSmallStumpSkullKid.skullkid->actor.world.pos.y - thisv->actor.world.pos.y) + 10.0f;
}

s32 EnSkj_ShootNeedle(EnSkj* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f pos;
    Vec3f pos2;
    EnSkjneedle* needle;

    pos.x = 1.5f;
    pos.y = 0.0f;
    pos.z = 40.0f;

    Matrix_RotateY((thisv->actor.shape.rot.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
    Matrix_MultVec3f(&pos, &pos2);

    pos2.x += thisv->actor.world.pos.x;
    pos2.z += thisv->actor.world.pos.z;
    pos2.y = thisv->actor.world.pos.y + 27.0f;

    needle = (EnSkjneedle*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_SKJNEEDLE, pos2.x, pos2.y, pos2.z,
                                       thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 0);
    if (needle != NULL) {
        needle->killTimer = 100;
        needle->actor.speedXZ = 24.0f;
        return 1;
    }
    return 0;
}

void EnSkj_SpawnBlood(GlobalContext* globalCtx, Vec3f* pos) {
    EffectSparkInit effect;
    s32 sp20;

    effect.position.x = pos->x;
    effect.position.y = pos->y;
    effect.position.z = pos->z;
    effect.uDiv = 5;
    effect.vDiv = 5;

    effect.colorStart[0].r = 0;
    effect.colorStart[0].g = 0;
    effect.colorStart[0].b = 128;
    effect.colorStart[0].a = 255;

    effect.colorStart[1].r = 0;
    effect.colorStart[1].g = 0;
    effect.colorStart[1].b = 128;
    effect.colorStart[1].a = 255;

    effect.colorStart[2].r = 0;
    effect.colorStart[2].g = 0;
    effect.colorStart[2].b = 128;
    effect.colorStart[2].a = 255;

    effect.colorStart[3].r = 0;
    effect.colorStart[3].g = 0;
    effect.colorStart[3].b = 128;
    effect.colorStart[3].a = 255;

    effect.colorEnd[0].r = 0;
    effect.colorEnd[0].g = 0;
    effect.colorEnd[0].b = 32;
    effect.colorEnd[0].a = 0;

    effect.colorEnd[1].r = 0;
    effect.colorEnd[1].g = 0;
    effect.colorEnd[1].b = 32;
    effect.colorEnd[1].a = 0;

    effect.colorEnd[2].r = 0;
    effect.colorEnd[2].g = 0;
    effect.colorEnd[2].b = 64;
    effect.colorEnd[2].a = 0;

    effect.colorEnd[3].r = 0;
    effect.colorEnd[3].g = 0;
    effect.colorEnd[3].b = 64;
    effect.colorEnd[3].a = 0;

    effect.speed = 8.0f;
    effect.gravity = -1.0f;

    effect.timer = 0;
    effect.duration = 8;

    Effect_Add(globalCtx, &sp20, EFFECT_SPARK, 0, 1, &effect);
}

s32 EnSkj_CollisionCheck(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;
    Vec3f effectPos;

    if (!((thisv->unk_2D3 == 0) || (D_80B01EA0 != 0) || !(thisv->collider.base.acFlags & AC_HIT))) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != 0) {
            if (thisv->actor.colChkInfo.damageEffect == 0xF) {
                effectPos.x = thisv->collider.info.bumper.hitPos.x;
                effectPos.y = thisv->collider.info.bumper.hitPos.y;
                effectPos.z = thisv->collider.info.bumper.hitPos.z;

                EnSkj_SpawnBlood(globalCtx, &effectPos);
                EffectSsHitMark_SpawnFixedScale(globalCtx, 1, &effectPos);

                yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y;
                if ((thisv->action == 2) || (thisv->action == 6)) {
                    if ((yawDiff > 0x6000) || (yawDiff < -0x6000)) {
                        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
                        EnSkj_SetupDie(thisv);
                        return 1;
                    }
                }

                Actor_ApplyDamage(&thisv->actor);
                Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);

                if (thisv->actor.colChkInfo.health != 0) {
                    if (thisv->hitsUntilDodge != 0) {
                        thisv->hitsUntilDodge--;
                    }
                    if (thisv->dodgeResetTimer == 0) {
                        thisv->dodgeResetTimer = 60;
                    }
                    func_80AFF1F0(thisv);
                    return 1;
                }
                EnSkj_SetupDie(thisv);
                return 1;
            }
        } else {
            thisv->backflipFlag = 1;
            EnSkj_Backflip(thisv);
            return 1;
        }
    }
    return 0;
}

s32 func_80AFEDF8(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 yawDiff;

    if (thisv->actor.xzDistToPlayer < thisv->unk_2EC) {
        thisv = thisv;
        if (func_8002DDE4(globalCtx) != 0) {
            return 1;
        }
    }

    yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y;

    if ((yawDiff < thisv->unk_2C8) && (-thisv->unk_2C8 < yawDiff)) {
        return 1;
    }

    return 0;
}

void EnSkj_Backflip(EnSkj* thisv) {
    thisv->actor.velocity.y = 8.0f;
    thisv->actor.speedXZ = -8.0f;

    EnSkj_ChangeAnim(thisv, SKJ_ANIM_BACKFLIP);
    EnSkj_SetupAction(thisv, SKJ_ACTION_FADE);
}

void EnSkj_Fade(EnSkj* thisv, GlobalContext* globalCtx) {
    u32 alpha = thisv->alpha;

    if (thisv->unk_2D6 == 2) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
        thisv->unk_2D6 = 0;
    }

    alpha -= 20;

    if (thisv->backflipFlag != 0) {
        if (alpha > 255) {
            alpha = 0;
        }

        thisv->alpha = alpha;
        thisv->actor.shape.shadowAlpha = alpha;
    }

    if (thisv->actor.velocity.y <= 0.0f) {
        if (thisv->actor.bgCheckFlags & 2) {
            thisv->actor.bgCheckFlags &= ~2;
            func_80AFF2A0(thisv);
        }
    }
}

void EnSkj_SetupWaitToShootNeedle(EnSkj* thisv) {
    thisv->needlesToShoot = 3;
    thisv->needleShootTimer = 0;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_SHOOT_NEEDLE);
    EnSkj_SetupAction(thisv, SKJ_ACTION_WAIT_TO_SHOOT_NEEDLE);
}

void EnSkj_WaitToShootNeedle(EnSkj* thisv, GlobalContext* globalCtx) {
    u8 val;
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidShootNeedleAnim);

    if ((thisv->skelAnime.curFrame == lastFrame) && (thisv->needleShootTimer == 0)) {
        val = thisv->needlesToShoot;
        if (thisv->needlesToShoot != 0) {
            EnSkj_ShootNeedle(thisv, globalCtx);
            thisv->needleShootTimer = 4;
            val--;
            thisv->needlesToShoot = val;

        } else {
            EnSkj_SetupNeedleRecover(thisv);
        }
    }
}

void EnSkj_SetupResetFight(EnSkj* thisv) {
    thisv->unk_2C8 = 0xAAA;
    thisv->unk_2EC = 200.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_PLAY_FLUTE);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_IDLE);
}

void EnSkj_SariasSongKidIdle(EnSkj* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == 0) {
        if (!(gSaveContext.itemGetInf[1] & 0x40) && (thisv->actor.xzDistToPlayer < 200.0f)) {
            thisv->backflipFlag = 1;
            EnSkj_Backflip(thisv);
        } else if (sSmallStumpSkullKid.unk0 != 0) {
            Player* player = GET_PLAYER(globalCtx);
            if (EnSkj_RangeCheck(player, sSmallStumpSkullKid.skullkid)) {
                EnSkj_SetupWaitInRange(thisv);
                player->stateFlags2 |= 0x800000;
                player->unk_6A8 = &sSmallStumpSkullKid.skullkid->actor;
            }
        }
    } else {
        if (func_80AFEDF8(thisv, globalCtx) != 0) {
            func_80AFF334(thisv);
        }
    }
}

void EnSkj_SetupDie(EnSkj* thisv) {
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_DIE);
    EnSkj_SetupAction(thisv, SKJ_ACTION_WAIT_FOR_DEATH_ANIM);
}

void EnSkj_WaitForDeathAnim(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidDieAnim);

    if (thisv->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupSpawnDeathEffect(thisv);
    }
}

void func_80AFF1F0(EnSkj* thisv) {
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_HIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_PICK_NEXT_FIHGT_ACTION);
}

void EnSkj_PickNextFightAction(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidHitAnim);

    if (thisv->skelAnime.curFrame == lastFrame) {
        if (thisv->hitsUntilDodge == 0) {
            thisv->hitsUntilDodge = 3;
            EnSkj_Backflip(thisv);
        } else {
            EnSkj_SetupStand(thisv);
        }
    }
}

void func_80AFF2A0(EnSkj* thisv) {
    EnSkj_CalculateCenter(thisv);
    thisv->actor.speedXZ = 0.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_LAND);
    EnSkj_SetupAction(thisv, SKJ_ACTION_WAIT_FOR_LAND_ANIM);
}

void EnSkj_WaitForLandAnim(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidLandAnim);

    if (thisv->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupStand(thisv);
    }
}

void func_80AFF334(EnSkj* thisv) {
    thisv->unk_2C8 = 0x2000;
    thisv->battleExitTimer = 400;
    thisv->unk_2EC = 600.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_LOOK_LEFT_RIGHT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_RESET_FIGHT);
}

void EnSkj_ResetFight(EnSkj* thisv, GlobalContext* globalCtx) {
    if (thisv->battleExitTimer == 0) {
        EnSkj_SetupResetFight(thisv);
    } else if (func_80AFEDF8(thisv, globalCtx) != 0) {
        thisv->battleExitTimer = 600;
        EnSkj_SetupStand(thisv);
    }
}

void EnSkj_SetupStand(EnSkj* thisv) {
    thisv->needleShootTimer = 60;
    thisv->unk_2C8 = 0x2000;
    thisv->unk_2F0 = 0.0f;
    thisv->unk_2EC = 600.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_FIGHTING_STANCE);
    EnSkj_SetupAction(thisv, SKJ_ACTION_FIGHT);
}

void EnSkj_Fight(EnSkj* thisv, GlobalContext* globalCtx) {
    Vec3f pos1;
    Vec3f pos2;
    s32 pad[3];
    f32 prevPosX;
    f32 prevPosZ;
    f32 phi_f14;
    s16 yawDistToPlayer;

    if (thisv->needleShootTimer == 0) {
        EnSkj_SetupWaitToShootNeedle(thisv);
    } else if (thisv->battleExitTimer != 0) {
        yawDistToPlayer =
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, thisv->unk_2F0, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        Math_ApproachF(&thisv->unk_2F0, 2000.0f, 1.0f, 200.0f);

        pos1.x = 0.0f;
        pos1.y = 0.0f;
        pos1.z = -120.0f;

        Matrix_RotateY((thisv->actor.shape.rot.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
        Matrix_MultVec3f(&pos1, &pos2);
        prevPosX = thisv->actor.world.pos.x;
        prevPosZ = thisv->actor.world.pos.z;
        if (1) {}
        thisv->actor.world.pos.x = thisv->center.x + pos2.x;
        thisv->actor.world.pos.z = thisv->center.z + pos2.z;

        phi_f14 = sqrtf(SQ(thisv->actor.world.pos.x - prevPosX) + SQ(thisv->actor.world.pos.z - prevPosZ));
        phi_f14 = CLAMP_MAX(phi_f14, 10.0f);
        phi_f14 /= 10.0f;

        thisv->skelAnime.playSpeed = (yawDistToPlayer < 0) ? -(1.0f + phi_f14) : (1.0f + phi_f14);

    } else if (func_80AFEDF8(thisv, globalCtx) != 0) {
        thisv->backflipFlag = 1;
        EnSkj_Backflip(thisv);
    } else {
        EnSkj_SetupResetFight(thisv);
    }
}

void EnSkj_SetupNeedleRecover(EnSkj* thisv) {
    Animation_Reverse(&thisv->skelAnime);
    EnSkj_SetupAction(thisv, SKJ_ACTION_NEEDLE_RECOVER);
}

void EnSkj_NeedleRecover(EnSkj* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame == 0.0f) {
        EnSkj_SetupStand(thisv);
    }
}

void EnSkj_SetupSpawnDeathEffect(EnSkj* thisv) {
    thisv->backflipFlag = 1;
    EnSkj_SetupAction(thisv, SKJ_ACTION_SPAWN_DEATH_EFFECT);
}

void EnSkj_SpawnDeathEffect(EnSkj* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;
    Vec3f effectVel;
    Vec3f effectAccel;
    u32 phi_v0;

    phi_v0 = thisv->alpha - 4;

    if (phi_v0 > 255) {
        phi_v0 = 0;
    }
    thisv->alpha = phi_v0;
    thisv->actor.shape.shadowAlpha = phi_v0;

    effectPos.x = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.x;
    effectPos.y = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.y;
    effectPos.z = Rand_CenteredFloat(30.0f) + thisv->actor.world.pos.z;

    effectAccel.z = 0.0f;
    effectAccel.y = 0.0f;
    effectAccel.x = 0.0f;

    effectVel.z = 0.0f;
    effectVel.y = 0.0f;
    effectVel.x = 0.0f;

    EffectSsDeadDb_Spawn(globalCtx, &effectPos, &effectVel, &effectAccel, 100, 10, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                         1);
}

void EnSkj_SetupWaitInRange(EnSkj* thisv) {
    thisv->textId = 0x10BC;

    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
}

void EnSkj_WaitInRange(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    // When link pulls out the Ocarina center him on the stump
    // Link was probably supposed to be pointed towards skull kid as well
    if (player->stateFlags2 & 0x1000000) {
        player->stateFlags2 |= 0x2000000;
        player->unk_6A8 = &sSmallStumpSkullKid.skullkid->actor;
        player->actor.world.pos.x = sSmallStumpSkullKid.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = sSmallStumpSkullKid.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = sSmallStumpSkullKid.skullkid->actor.world.pos.z;
        EnSkj_TurnPlayer(sSmallStumpSkullKid.skullkid, player);
        func_8010BD88(globalCtx, OCARINA_ACTION_CHECK_SARIA);
        EnSkj_SetupWaitForSong(thisv);
    } else if (D_80B01EA0 != 0) {
        player->actor.world.pos.x = sSmallStumpSkullKid.skullkid->actor.world.pos.x;
        player->actor.world.pos.y = sSmallStumpSkullKid.skullkid->actor.world.pos.y;
        player->actor.world.pos.z = sSmallStumpSkullKid.skullkid->actor.world.pos.z;
        if ((Player_GetMask(globalCtx) == PLAYER_MASK_SKULL) && !(gSaveContext.itemGetInf[3] & 0x200)) {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            EnSkj_SetupMaskTrade(thisv);
        } else {
            EnSkj_SetupTalk(thisv);
        }
    } else if (!EnSkj_RangeCheck(player, sSmallStumpSkullKid.skullkid)) {
        EnSkj_SetupResetFight(thisv);
    } else {
        player->stateFlags2 |= 0x800000;
        if (gSaveContext.itemGetInf[1] & 0x40) {
            if (gSaveContext.itemGetInf[3] & 0x200) {
                thisv->textId = Text_GetFaceReaction(globalCtx, 0x15);
                if (thisv->textId == 0) {
                    thisv->textId = 0x1020;
                }
            } else if (Player_GetMask(globalCtx) == PLAYER_MASK_NONE) {
                thisv->textId = 0x10BC;
            } else if (Player_GetMask(globalCtx) == PLAYER_MASK_SKULL) {
                thisv->textId = 0x101B;
            } else {
                thisv->textId = Text_GetFaceReaction(globalCtx, 0x15);
            }
            func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
        }
    }
}

void EnSkj_SetupWaitForSong(EnSkj* thisv) {
    thisv->unk_2D6 = 0;
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_FOR_SONG);
}

void EnSkj_WaitForSong(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    // Played a song thats not Saria's song
    if (!(gSaveContext.itemGetInf[1] & 0x40) && ((globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_FAIL) ||
                                                 (globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_FAIL_NO_TEXT))) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        Message_CloseTextbox(globalCtx);
        player->unk_6A8 = &thisv->actor;
        func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
        EnSkj_SetupWrongSong(thisv);
    } else {
        if ((globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) && (thisv->unk_2D6 == 0)) {
            thisv->unk_2D6 = 1;
            EnSkj_ChangeAnim(thisv, SKJ_ANIM_PLAY_FLUTE);
        } else if ((thisv->unk_2D6 != 0) && (globalCtx->msgCtx.msgMode == MSGMODE_SONG_DEMONSTRATION_DONE)) {
            thisv->unk_2D6 = 0;
            EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
        }
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
            thisv->unk_2D6 = 0;
            EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
            EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_IN_RANGE);
        } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (!(gSaveContext.itemGetInf[1] & 0x40)) {
                // Saria's song has been played for the first titme
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                player->unk_6A8 = &thisv->actor;
                func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
                thisv->textId = 0x10BB;
                EnSkj_SetupAfterSong(thisv);
            } else {
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_05;
            }
        } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_02) {
            player->stateFlags2 &= ~0x1000000;
            Actor_Kill(&thisv->actor);
        } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        } else {
            if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
                gSaveContext.sunsSongState = 0;
                if (gSaveContext.itemGetInf[1] & 0x40) {
                    globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &thisv->actor;
                    func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
                    thisv->textId = 0x10BD;
                    EnSkj_SetupAfterSong(thisv);
                } else {
                    globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &thisv->actor;
                    func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
                    EnSkj_SetupWrongSong(thisv);
                }
            }
        }
    }
}

void EnSkj_SetupAfterSong(EnSkj* thisv) {
    thisv->unk_2D6 = 0;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_AFTER_SONG);
}

void EnSkj_AfterSong(EnSkj* thisv, GlobalContext* globalCtx) {
    if (D_80B01EA0 != 0) {
        EnSkj_SetupTalk(thisv);
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
    }
}

void EnSkj_SetupTalk(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_TALK);
}

void EnSkj_SariaSongTalk(EnSkj* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        if (gSaveContext.itemGetInf[1] & 0x40) {
            EnSkj_SetupWaitInRange(thisv);
        } else {
            func_80AFFE24(thisv);
            func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, EnSkj_GetItemXzRange(thisv),
                          EnSkj_GetItemYRange(thisv));
        }
    }
}

void func_80AFFE24(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_UNK14);
}

void func_80AFFE44(EnSkj* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        EnSkj_SetupPostSariasSong(thisv);
    } else {
        func_8002F434(&thisv->actor, globalCtx, GI_HEART_PIECE, EnSkj_GetItemXzRange(thisv), EnSkj_GetItemYRange(thisv));
    }
}

void EnSkj_SetupPostSariasSong(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_CHANGE_MODE);
}

void EnSkj_ChangeModeAfterSong(EnSkj* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        gSaveContext.itemGetInf[1] |= 0x40;
        EnSkj_SetNaviId(thisv);
        EnSkj_SetupWaitInRange(thisv);
    }
}

void EnSkj_SetupMaskTrade(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_START_TRADE);
}

void EnSkj_StartMaskTrade(EnSkj* thisv, GlobalContext* globalCtx) {
    u8 sp1F = Message_GetState(&globalCtx->msgCtx);

    func_8002DF54(globalCtx, &thisv->actor, 1);
    if ((sp1F == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        EnSkj_JumpFromStump(thisv);
    }
}

void EnSkj_JumpFromStump(EnSkj* thisv) {
    thisv->actor.velocity.y = 8.0f;
    thisv->actor.speedXZ = 2.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_BACKFLIP);
    Animation_Reverse(&thisv->skelAnime);
    thisv->skelAnime.curFrame = thisv->skelAnime.startFrame;
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING);
}

void EnSkj_WaitForLanding(EnSkj* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.velocity.y <= 0.0f) {
        if (thisv->actor.bgCheckFlags & 2) {
            thisv->actor.bgCheckFlags &= ~2;
            thisv->actor.speedXZ = 0.0f;
            EnSkj_SetupWaitForLandAnimFinish(thisv);
        }
    }
}

void EnSkj_SetupWaitForLandAnimFinish(EnSkj* thisv) {
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_LAND);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_FOR_LANDING_ANIM);
}

void EnSkj_WaitForLandAnimFinish(EnSkj* thisv, GlobalContext* globalCtx) {
    s16 lastFrame = Animation_GetLastFrame(&gSkullKidLandAnim);

    if (thisv->skelAnime.curFrame == lastFrame) {
        EnSkj_SetupWalkToPlayer(thisv);
    }
}

void EnSkj_SetupWalkToPlayer(EnSkj* thisv) {
    thisv->unk_2F0 = 0.0f;
    thisv->actor.speedXZ = 2.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WALK_TO_PLAYER);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WALK_TO_PLAYER);
}

void EnSkj_WalkToPlayer(EnSkj* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0xA, thisv->unk_2F0, 0);
    Math_ApproachF(&thisv->unk_2F0, 2000.0f, 1.0f, 100.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if (thisv->actor.xzDistToPlayer < 120.0f) {
        thisv->actor.speedXZ = 0.0f;
        EnSkj_SetupAskForMask(thisv, globalCtx);
    }
}

void EnSkj_SetupAskForMask(EnSkj* thisv, GlobalContext* globalCtx) {
    Message_StartTextbox(globalCtx, 0x101C, &thisv->actor);
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_ASK_FOR_MASK);
}

void EnSkj_AskForMask(EnSkj* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // Yes
                EnSkj_SetupTakeMask(thisv, globalCtx);
                break;
            case 1: // No
                Message_ContinueTextbox(globalCtx, 0x101D);
                EnSkj_SetupWaitForMaskTextClear(thisv);
                break;
        }
    }
}

void EnSkj_SetupTakeMask(EnSkj* thisv, GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x101E);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_TAKE_MASK);
}

void EnSkj_TakeMask(EnSkj* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        Rupees_ChangeBy(10);
        gSaveContext.itemGetInf[3] |= 0x200;
        EnSkj_SetNaviId(thisv);
        Player_UnsetMask(globalCtx);
        Item_Give(globalCtx, ITEM_SOLD_OUT);
        Message_ContinueTextbox(globalCtx, 0x101F);
        EnSkj_SetupWaitForMaskTextClear(thisv);
    }
}

void EnSkj_SetupWaitForMaskTextClear(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_MASK_TEXT);
}

void EnSkj_WaitForMaskTextClear(EnSkj* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        func_8002DF54(globalCtx, &thisv->actor, 7);
        thisv->backflipFlag = 1;
        EnSkj_Backflip(thisv);
    }
}

void EnSkj_SetupWrongSong(EnSkj* thisv) {
    thisv->textId = 0x1041;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WRONG_SONG);
}

void EnSkj_WrongSong(EnSkj* thisv, GlobalContext* globalCtx) {
    if (D_80B01EA0 != 0) {
        EnSkj_SetupWaitForTextClear(thisv);
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, EnSkj_GetItemXzRange(thisv));
    }
}

void EnSkj_SetupWaitForTextClear(EnSkj* thisv) {
    EnSkj_SetupAction(thisv, SKJ_ACTION_SARIA_SONG_WAIT_FOR_TEXT);
}

void EnSkj_SariasSongWaitForTextClear(EnSkj* thisv, GlobalContext* globalCtx) {
    u8 state = Message_GetState(&globalCtx->msgCtx);
    Player* player = GET_PLAYER(globalCtx);

    if (state == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        EnSkj_SetupWaitInRange(thisv);
        player->stateFlags2 |= 0x800000;
        player->unk_6A8 = (Actor*)sSmallStumpSkullKid.skullkid;
    }
}

void EnSkj_OcarinaGameSetupWaitForPlayer(EnSkj* thisv) {
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_OCARINA_GAME_WAIT_FOR_PLAYER);
}

void EnSkj_OcarinaGameWaitForPlayer(EnSkj* thisv, GlobalContext* globalCtx) {
    if (thisv->playerInRange) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        EnSkj_SetupAction(thisv, SKJ_ACTION_OCARINA_GAME_IDLE);
    }
}

s32 EnSkj_IsLeavingGame(EnSkj* thisv) {
    s32 paramDecr = thisv->actor.params - 1;

    if (sOcarinaMinigameSkullKids[paramDecr].unk0 == 2) {
        EnSkj_SetupLeaveOcarinaGame(thisv);
        return true;
    }
    return false;
}

void EnSkj_SetupIdle(EnSkj* thisv) {
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_WAIT);
    EnSkj_SetupAction(thisv, SKJ_ACTION_OCARINA_GAME_IDLE);
}

void EnSkj_Appear(EnSkj* thisv) {
    if (thisv->alpha != 255) {
        thisv->alpha += 20;

        if (thisv->alpha > 255) {
            thisv->alpha = 255;
        }
    }
}

void EnSkj_OcarinaGameIdle(EnSkj* thisv, GlobalContext* globalCtx) {
    EnSkj_Appear(thisv);

    if ((EnSkj_IsLeavingGame(thisv) == false) && (thisv->minigameState != 0)) {
        EnSkj_SetupPlayOcarinaGame(thisv);
    }
}

void EnSkj_SetupPlayOcarinaGame(EnSkj* thisv) {
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_PLAY_FLUTE);
    EnSkj_SetupAction(thisv, SKJ_ACTION_OCARINA_GAME_PLAY);
}

void EnSkj_PlayOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx) {
    EnSkj_Appear(thisv);

    if (!EnSkj_IsLeavingGame(thisv) && (thisv->minigameState == 0)) {
        EnSkj_SetupIdle(thisv);
    }
}

void EnSkj_SetupLeaveOcarinaGame(EnSkj* thisv) {
    thisv->actor.velocity.y = 8.0f;
    thisv->actor.speedXZ = -8.0f;
    EnSkj_ChangeAnim(thisv, SKJ_ANIM_BACKFLIP);
    EnSkj_SetupAction(thisv, SKJ_ACTION_OCARINA_GAME_LEAVE);
}

void EnSkj_LeaveOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx) {
    s32 paramsDecr = thisv->actor.params - 1;

    sOcarinaMinigameSkullKids[paramsDecr].unk0 = 0;
    sOcarinaMinigameSkullKids[paramsDecr].skullkid = NULL;
    thisv->backflipFlag = 1;
    EnSkj_Backflip(thisv);
}

void EnSkj_Update(Actor* thisx, GlobalContext* globalCtx) {
    Vec3f dropPos;
    s32 pad;
    EnSkj* thisv = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&thisv->actor, globalCtx);

    thisv->timer++;

    if (thisv->multiuseTimer != 0) {
        thisv->multiuseTimer--;
    }

    if (thisv->needleShootTimer != 0) {
        thisv->needleShootTimer--;
    }

    if (thisv->dodgeResetTimer != 0) {
        thisv->dodgeResetTimer--;
    }

    if (thisv->dodgeResetTimer == 0) {
        thisv->hitsUntilDodge = 3;
    }

    if ((thisv->backflipFlag != 0) && (thisv->alpha == 0)) {
        if (thisv->action == 9) {
            dropPos.x = thisv->actor.world.pos.x;
            dropPos.y = thisv->actor.world.pos.y;
            dropPos.z = thisv->actor.world.pos.z;

            Item_DropCollectible(globalCtx, &dropPos, ITEM00_RUPEE_ORANGE);
        }
        Actor_Kill(&thisv->actor);
        return;
    }

    Actor_SetFocus(&thisv->actor, 30.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.textId = thisv->textId;
    EnSkj_CollisionCheck(thisv, globalCtx);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if ((thisv->unk_2D3 != 0) && (D_80B01EA0 == 0)) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

        if (thisv->actor.colorFilterTimer == 0) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    SkelAnime_Update(&thisv->skelAnime);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 20.0f, 7);
}

void EnSkj_SariasSongShortStumpUpdate(Actor* thisx, GlobalContext* globalCtx) {
    EnSkj* thisv = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&thisv->actor, globalCtx);

    if (BREG(0) != 0) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
    }
}

void EnSkj_TurnPlayer(EnSkj* thisv, Player* player) {
    Math_SmoothStepToS(&player->actor.shape.rot.y, thisv->actor.world.rot.y, 5, 2000, 0);
    player->actor.world.rot.y = player->actor.shape.rot.y;
    player->currentYaw = player->actor.shape.rot.y;
}

void EnSkj_SetupWaitForOcarina(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (EnSkj_RangeCheck(player, thisv)) {
        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->playerInRange = true;
        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->playerInRange = true;

        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            func_800F5BF0(NATURE_ID_KOKIRI_REGION);
            EnSkj_TurnPlayer(thisv, player);
            player->unk_6A8 = &thisv->actor;
            Message_StartTextbox(globalCtx, 0x10BE, &thisv->actor);
            thisv->actionFunc = EnSkj_StartOcarinaMinigame;
        } else {
            thisv->actionFunc = EnSkj_WaitForOcarina;
        }
    }
}

void EnSkj_WaitForOcarina(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags2 & 0x1000000) {
        player->stateFlags2 |= 0x2000000;
        func_800F5BF0(NATURE_ID_KOKIRI_REGION);
        EnSkj_TurnPlayer(thisv, player);
        player->unk_6A8 = &thisv->actor;
        Message_StartTextbox(globalCtx, 0x10BE, &thisv->actor);
        thisv->actionFunc = EnSkj_StartOcarinaMinigame;
    } else if (EnSkj_RangeCheck(player, thisv)) {
        player->stateFlags2 |= 0x800000;
    }
}

void EnSkj_StartOcarinaMinigame(EnSkj* thisv, GlobalContext* globalCtx) {
    u8 dialogState = Message_GetState(&globalCtx->msgCtx);
    Player* player = GET_PLAYER(globalCtx);

    EnSkj_TurnPlayer(thisv, player);

    if (dialogState == TEXT_STATE_CLOSING) {
        func_8010BD58(globalCtx, OCARINA_ACTION_MEMORY_GAME);
        if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
            sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_PLAY_NOTES;
        }
        thisv->songFailTimer = 160;
        thisv->actionFunc = EnSkj_WaitForPlayback;
    }
}

void EnSkj_WaitForPlayback(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    EnSkj_TurnPlayer(thisv, player);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) { // failed the game
        Message_CloseTextbox(globalCtx);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &thisv->actor;
        func_8002F2CC(&thisv->actor, globalCtx, 26.0f);
        thisv->textId = 0x102D;
        thisv->actionFunc = EnSkj_FailedMiniGame;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0F) { // completed the game
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Message_CloseTextbox(globalCtx);
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player->unk_6A8 = &thisv->actor;
        func_8002F2CC(&thisv->actor, globalCtx, 26.0f);
        thisv->textId = 0x10BF;
        thisv->actionFunc = EnSkj_WonOcarinaMiniGame;
    } else { // playing the game
        switch (globalCtx->msgCtx.msgMode) {
            case MSGMODE_MEMORY_GAME_LEFT_SKULLKID_WAIT:
                if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
                    sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
                        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    Message_UpdateOcarinaGame(globalCtx);
                }
                break;
            case MSGMODE_MEMORY_GAME_RIGHT_SKULLKID_WAIT:
                if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
                    sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid->minigameState = SKULL_KID_OCARINA_WAIT;
                }
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    Message_UpdateOcarinaGame(globalCtx);
                    thisv->songFailTimer = 160;
                }
                break;
            case MSGMODE_MEMORY_GAME_PLAYER_PLAYING:
                if (thisv->songFailTimer != 0) {
                    thisv->songFailTimer--;
                } else { // took too long, game failed
                    func_80078884(NA_SE_SY_OCARINA_ERROR);
                    Message_CloseTextbox(globalCtx);
                    globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                    player->unk_6A8 = &thisv->actor;
                    func_8002F2CC(&thisv->actor, globalCtx, 26.0f);
                    thisv->textId = 0x102D;
                    thisv->actionFunc = EnSkj_FailedMiniGame;
                }
                break;
            case MSGMODE_MEMORY_GAME_START_NEXT_ROUND:
                if (!Audio_IsSfxPlaying(NA_SE_SY_METRONOME)) {
                    if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
                        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid->minigameState =
                            SKULL_KID_OCARINA_PLAY_NOTES;
                    }
                    thisv->songFailTimer = 160;
                    Audio_OcaSetInstrument(6); // related instrument sound (flute?)
                    Audio_OcaSetSongPlayback(OCARINA_SONG_MEMORY_GAME + 1, 1);
                    globalCtx->msgCtx.msgMode = MSGMODE_MEMORY_GAME_LEFT_SKULLKID_PLAYING;
                    globalCtx->msgCtx.stateTimer = 2;
                }
                break;
        }
    }
}

void EnSkj_FailedMiniGame(EnSkj* thisv, GlobalContext* globalCtx) {
    if (D_80B01EA0) {
        thisv->actionFunc = EnSkj_WaitForNextRound;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 26.0f);
    }
}

void EnSkj_WaitForNextRound(EnSkj* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        EnSkj_OfferNextRound(thisv, globalCtx);
    }
}

void EnSkj_OfferNextRound(EnSkj* thisv, GlobalContext* globalCtx) {
    Message_ContinueTextbox(globalCtx, 0x102E);
    thisv->actionFunc = EnSkj_WaitForOfferResponse;
}

void EnSkj_WaitForOfferResponse(EnSkj* thisv, GlobalContext* globalCtx) {
    Player* player;

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                player = GET_PLAYER(globalCtx);
                player->stateFlags3 |= 0x20; // makes player take ocarina out right away after closing box
                thisv->actionFunc = EnSkj_SetupWaitForOcarina;
                break;
            case 1: // no
                thisv->actionFunc = EnSkj_CleanupOcarinaGame;
                break;
        }
    }
}

void EnSkj_WonOcarinaMiniGame(EnSkj* thisv, GlobalContext* globalCtx) {
    if (D_80B01EA0) {
        thisv->actionFunc = EnSkj_WaitToGiveReward;
    } else {
        func_8002F2CC(&thisv->actor, globalCtx, 26.0f);
    }
}

void EnSkj_WaitToGiveReward(EnSkj* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        func_8002F434(&thisv->actor, globalCtx, sOcarinaGameRewards[gSaveContext.ocarinaGameRoundNum], 26.0f, 26.0f);
        thisv->actionFunc = EnSkj_GiveOcarinaGameReward;
    }
}

void EnSkj_GiveOcarinaGameReward(EnSkj* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnSkj_FinishOcarinaGameRound;
    } else {
        func_8002F434(&thisv->actor, globalCtx, sOcarinaGameRewards[gSaveContext.ocarinaGameRoundNum], 26.0f, 26.0f);
    }
}

void EnSkj_FinishOcarinaGameRound(EnSkj* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        s32 ocarinaGameRoundNum = gSaveContext.ocarinaGameRoundNum;

        if (gSaveContext.ocarinaGameRoundNum < 3) {
            gSaveContext.ocarinaGameRoundNum++;
        }

        if (ocarinaGameRoundNum == 2) {
            gSaveContext.itemGetInf[1] |= 0x80;
            thisv->actionFunc = EnSkj_CleanupOcarinaGame;
        } else {
            EnSkj_OfferNextRound(thisv, globalCtx);
        }
    }
}

void EnSkj_CleanupOcarinaGame(EnSkj* thisv, GlobalContext* globalCtx) {
    if (sOcarinaMinigameSkullKids[SKULL_KID_LEFT].skullkid != NULL) {
        sOcarinaMinigameSkullKids[SKULL_KID_LEFT].unk0 = 2;
    }

    if (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].skullkid != NULL) {
        sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].unk0 = 2;
    }

    if ((sOcarinaMinigameSkullKids[SKULL_KID_LEFT].unk0 == 2) &&
        (sOcarinaMinigameSkullKids[SKULL_KID_RIGHT].unk0 == 2)) {
        func_800F5C2C();
        Actor_Kill(&thisv->actor);
    }
}

void EnSkj_OcarinaMinigameShortStumpUpdate(Actor* thisx, GlobalContext* globalCtx) {
    EnSkj* thisv = (EnSkj*)thisx;

    D_80B01EA0 = Actor_ProcessTalkRequest(&thisv->actor, globalCtx);
    thisv->timer++;

    thisv->actor.focus.pos.x = 1230.0f;
    thisv->actor.focus.pos.y = -90.0f;
    thisv->actor.focus.pos.z = 450.0f;

    if (BREG(0) != 0) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
    }

    thisv->actionFunc(thisv, globalCtx);

    thisv->actor.textId = thisv->textId;
    thisv->actor.xzDistToPlayer = 50.0;
}

s32 EnSkj_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return 0;
}

void EnSkj_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_skj.c", 2417);

    if ((limbIndex == 11) && (gSaveContext.itemGetInf[3] & 0x200)) {
        func_80093D18(globalCtx->state.gfxCtx);
        Matrix_Push();
        Matrix_RotateZYX(-0x4000, 0, 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_skj.c", 2430),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gSKJskullMaskDL);
        Matrix_Pop();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_skj.c", 2437);
}

Gfx* EnSkj_TranslucentDL(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 3
    dList = dListHead = static_cast<Gfx*>(Graph_Alloc(gfxCtx, sizeof(Gfx)));
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* EnSkj_OpaqueDL(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* dList;
    Gfx* dListHead;

    //! @bug This only allocates space for 1 command but uses 2
    dList = dListHead = static_cast<Gfx*>(Graph_Alloc(gfxCtx, sizeof(Gfx)));
    gDPSetEnvColor(dListHead++, 0, 0, 0, alpha);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnSkj_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnSkj* thisv = (EnSkj*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_skj.c", 2475);

    func_80093D18(globalCtx->state.gfxCtx);

    if (thisv->alpha < 255) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, EnSkj_TranslucentDL(globalCtx->state.gfxCtx, thisv->alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, EnSkj_OpaqueDL(globalCtx->state.gfxCtx, thisv->alpha));
    }

    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          EnSkj_OverrideLimbDraw, EnSkj_PostLimbDraw, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_skj.c", 2495);
}
