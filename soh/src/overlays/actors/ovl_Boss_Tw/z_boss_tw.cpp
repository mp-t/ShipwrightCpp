#include "z_boss_tw.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_tw/object_tw.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /*  0 */ TWEFF_NONE,
    /*  1 */ TWEFF_DOT,
    /*  2 */ TWEFF_2,
    /*  3 */ TWEFF_3,
    /*  4 */ TWEFF_RING,
    /*  5 */ TWEFF_PLYR_FRZ,
    /*  6 */ TWEFF_FLAME,
    /*  7 */ TWEFF_MERGEFLAME,
    /*  8 */ TWEFF_SHLD_BLST,
    /*  9 */ TWEFF_SHLD_DEFL,
    /* 10 */ TWEFF_SHLD_HIT
} TwEffType;

typedef enum {
    /* 0 */ EFF_ARGS,
    /* 1 */ EFF_UNKS1,
    /* 2 */ EFF_WORK_MAX
} EffectWork;

typedef enum {
    /* 0 */ EFF_SCALE,
    /* 1 */ EFF_DIST,
    /* 2 */ EFF_ROLL,
    /* 3 */ EFF_YAW,
    /* 4 */ EFF_FWORK_MAX
} EffectFWork;

typedef enum {
    /* 0x00 */ TW_KOTAKE,
    /* 0x01 */ TW_KOUME,
    /* 0x02 */ TW_TWINROVA,
    /* 0x64 */ TW_FIRE_BLAST = 0x64,
    /* 0x65 */ TW_FIRE_BLAST_GROUND,
    /* 0x66 */ TW_ICE_BLAST,
    /* 0x67 */ TW_ICE_BLAST_GROUND,
    /* 0x68 */ TW_DEATHBALL_KOTAKE,
    /* 0x69 */ TW_DEATHBALL_KOUME
} TwinrovaType;

typedef struct {
    /* 0x0000 */ u8 type;
    /* 0x0001 */ u8 frame;
    /* 0x0004 */ Vec3f pos;
    /* 0x0010 */ Vec3f curSpeed;
    /* 0x001C */ Vec3f accel;
    /* 0x0028 */ Color_RGB8 color;
    /* 0x002C */ s16 alpha;
    /* 0x002E */ s16 work[EFF_WORK_MAX];
    /* 0x0034 */ f32 workf[EFF_FWORK_MAX];
    /* 0x0044 */ Actor* target;
} BossTwEffect;

void BossTw_Init(Actor* thisx, GlobalContext* globalCtx);
void BossTw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossTw_Update(Actor* thisx, GlobalContext* globalCtx);
void BossTw_Draw(Actor* thisx, GlobalContext* globalCtx);
void BossTw_Reset(void);

void BossTw_TwinrovaDamage(BossTw* thisv, GlobalContext* globalCtx, u8 arg2);
void BossTw_TwinrovaSetupFly(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_DrawEffects(GlobalContext* globalCtx);
void BossTw_TwinrovaLaugh(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaFly(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaGetUp(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupGetUp(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupLaugh(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaDoneBlastShoot(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupDoneBlastShoot(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupShootBlast(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupChargeBlast(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaArriveAtTarget(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaDeathCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaIntroCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_CSWait(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_DeathCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaMergeCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupMergeCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_MergeCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_Spin(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_Laugh(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupLaugh(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_FinishBeamShoot(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupFinishBeamShoot(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupHitByBeam(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_HitByBeam(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_Wait(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_ShootBeam(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_FlyTo(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupShootBeam(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TurnToPlayer(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaUpdate(Actor* thisx, GlobalContext* globalCtx);
void BossTw_TwinrovaDraw(Actor* thisx, GlobalContext* globalCtx);
void BossTw_SetupWait(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupIntroCS(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupFlyTo(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_SetupCSWait(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_BlastUpdate(Actor* thisx, GlobalContext* globalCtx);
void BossTw_BlastDraw(Actor* thisx, GlobalContext* globalCtx);
void BossTw_BlastFire(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_BlastIce(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_DeathBall(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_DrawDeathBall(Actor* thisx, GlobalContext* globalCtx);
void BossTw_TwinrovaStun(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSpin(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaShootBlast(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaChargeBlast(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_TwinrovaSetupSpin(BossTw* thisv, GlobalContext* globalCtx);
void BossTw_UpdateEffects(GlobalContext* globalCtx);

ActorInit Boss_Tw_InitVars = {
    ACTOR_BOSS_TW,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_TW,
    sizeof(BossTw),
    (ActorFunc)BossTw_Init,
    (ActorFunc)BossTw_Destroy,
    (ActorFunc)BossTw_Update,
    (ActorFunc)BossTw_Draw,
    (ActorResetFunc)BossTw_Reset,
};

static Vec3f D_8094A7D0 = { 0.0f, 0.0f, 1000.0f };
static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };

static ColliderCylinderInit sCylinderInitBlasts = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x30 },
        { 0x00100000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 35, -17, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitKoumeKotake = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x20 },
        { 0xFFCDFFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 45, 120, -30, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitTwinrova = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x20 },
        { 0xFFCDFFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 45, 120, -30, { 0, 0, 0 } },
};

static Vec3f sTwinrovaPillarPos[] = {
    { 580.0f, 380.0f, 0.0f },
    { 0.0f, 380.0f, 580.0f },
    { -580.0f, 380.0f, 0.0f },
    { 0.0f, 380.0f, -580.0f },
};

static u8 sTwInitalized = false;

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

static s8 sEnvType;
static u8 sGroundBlastType;
static BossTw* sKotakePtr;
static BossTw* sKoumePtr;
static BossTw* sTwinrovaPtr;
static u8 sShieldFireCharge;
static u8 sShieldIceCharge;
static f32 D_8094C854;
static f32 D_8094C858;
static u8 sTwinrovaBlastType;
static u8 sFixedBlastType;
static u8 sFixedBlatSeq;
static u8 sFreezeState;
static Vec3f sShieldHitPos;
static s16 sShieldHitYaw;
static u8 sBeamDivertTimer;
static u8 D_8094C86F;
static u8 D_8094C870;
static s16 D_8094C872;
static s16 D_8094C874;
static s16 D_8094C876;
static u8 D_8094C878;
static s16 D_8094C87A;
static s16 D_8094C87C;
static u8 D_8094C87E;
static BossTwEffect sTwEffects[150];

void BossTw_AddDotEffect(GlobalContext* globalCtx, Vec3f* initalPos, Vec3f* initalSpeed, Vec3f* accel, f32 scale,
                         s16 args, s16 countLimit) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < countLimit; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_DOT;
            eff->pos = *initalPos;
            eff->curSpeed = *initalSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->alpha = 255;
            eff->frame = (s16)Rand_ZeroFloat(10.0f);
            eff->work[EFF_ARGS] = args;
            break;
        }
    }
}

void BossTw_AddDmgCloud(GlobalContext* globalCtx, s16 type, Vec3f* initialPos, Vec3f* initalSpeed, Vec3f* accel,
                        f32 scale, s16 alpha, s16 args, s16 countLimit) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < countLimit; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = type;
            eff->pos = *initialPos;
            eff->curSpeed = *initalSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->alpha = alpha;
            eff->frame = (s16)Rand_ZeroFloat(100.0f);
            break;
        }
    }
}

void BossTw_AddRingEffect(GlobalContext* globalCtx, Vec3f* initalPos, f32 scale, f32 arg3, s16 alpha, s16 args,
                          s16 arg6, s16 arg7) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < arg7; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_RING;
            eff->pos = *initalPos;
            eff->curSpeed = sZeroVector;
            eff->accel = sZeroVector;
            eff->workf[EFF_SCALE] = scale * 0.0025f;
            eff->workf[EFF_DIST] = arg3 * 0.0025f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = arg6;
            eff->alpha = alpha;
            eff->workf[EFF_ROLL] = Rand_ZeroFloat(std::numbers::pi_v<float>);
            eff->frame = 0;
            break;
        }
    }
}

void BossTw_AddPlayerFreezeEffect(GlobalContext* globalCtx, Actor* target) {
    BossTwEffect* eff;
    s16 i;

    for (eff = static_cast<BossTwEffect*>(globalCtx->specialEffects), i = 0; i < ARRAY_COUNT(sTwEffects); i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_PLYR_FRZ;
            eff->curSpeed = sZeroVector;
            eff->accel = sZeroVector;
            eff->frame = 0;
            eff->target = target;
            eff->workf[EFF_DIST] = 0.0f;
            eff->workf[EFF_SCALE] = 0.0f;
            eff->workf[EFF_ROLL] = 0.0f;
            if (target == NULL) {
                eff->work[EFF_ARGS] = 100;
            } else {
                eff->work[EFF_ARGS] = 20;
            }
            break;
        }
    }
}

void BossTw_AddFlameEffect(GlobalContext* globalCtx, Vec3f* initalPos, Vec3f* initalSpeed, Vec3f* accel, f32 scale,
                           s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < ARRAY_COUNT(sTwEffects); i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_FLAME;
            eff->pos = *initalPos;
            eff->curSpeed = *initalSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->alpha = 0;
            eff->frame = (s16)Rand_ZeroFloat(1000.0f);
            break;
        }
    }
}

void BossTw_AddMergeFlameEffect(GlobalContext* globalCtx, Vec3f* initialPos, f32 scale, f32 dist, s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < ARRAY_COUNT(sTwEffects); i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_MERGEFLAME;
            eff->pos = *initialPos;
            eff->curSpeed = sZeroVector;
            eff->accel = sZeroVector;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->workf[EFF_DIST] = dist;
            eff->workf[EFF_ROLL] = Rand_ZeroFloat(2.0f * std::numbers::pi_v<float>);
            eff->alpha = 0;
            eff->frame = (s16)Rand_ZeroFloat(1000.0f);
            break;
        }
    }
}

void BossTw_AddShieldBlastEffect(GlobalContext* globalCtx, Vec3f* initalPos, Vec3f* initalSpeed, Vec3f* accel,
                                 f32 scale, f32 arg5, s16 alpha, s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = static_cast<BossTwEffect*>(globalCtx->specialEffects); i < ARRAY_COUNT(sTwEffects); i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_SHLD_BLST;
            eff->pos = *initalPos;
            eff->curSpeed = *initalSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->workf[EFF_DIST] = arg5 / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->alpha = alpha;
            eff->frame = (s16)Rand_ZeroFloat(1000.0f);
            break;
        }
    }
}

void BossTw_AddShieldDeflectEffect(GlobalContext* globalCtx, f32 arg1, s16 arg2) {
    s16 i;
    s16 j;
    BossTwEffect* eff;
    Player* player = GET_PLAYER(globalCtx);

    sShieldHitPos = player->bodyPartsPos[15];
    sShieldHitYaw = player->actor.shape.rot.y;

    for (i = 0; i < 8; i++) {
        for (eff = static_cast<BossTwEffect*>(globalCtx->specialEffects), j = 0; j < ARRAY_COUNT(sTwEffects); j++, eff++) {
            if (eff->type == TWEFF_NONE) {
                eff->type = TWEFF_SHLD_DEFL;
                eff->pos = sShieldHitPos;
                eff->curSpeed = sZeroVector;
                eff->accel = sZeroVector;
                eff->workf[EFF_ROLL] = i * (std::numbers::pi_v<float> / 4.0f);
                eff->workf[EFF_YAW] = std::numbers::pi_v<float> / 2.0f;
                eff->workf[EFF_DIST] = 0.0f;
                eff->workf[EFF_SCALE] = arg1 / 1000.0f;
                eff->work[EFF_ARGS] = arg2;
                eff->work[EFF_UNKS1] = 0;
                eff->alpha = 255;
                eff->frame = (s16)Rand_ZeroFloat(1000.0f);
                break;
            }
        }
    }
}

void BossTw_AddShieldHitEffect(GlobalContext* globalCtx, f32 arg1, s16 arg2) {
    s16 i;
    s16 j;
    BossTwEffect* eff;
    Player* player = GET_PLAYER(globalCtx);

    sShieldHitPos = player->bodyPartsPos[15];
    sShieldHitYaw = player->actor.shape.rot.y;

    for (i = 0; i < 8; i++) {
        for (eff = static_cast<BossTwEffect*>(globalCtx->specialEffects), j = 0; j < ARRAY_COUNT(sTwEffects); j++, eff++) {
            if (eff->type == TWEFF_NONE) {
                eff->type = TWEFF_SHLD_HIT;
                eff->pos = sShieldHitPos;
                eff->curSpeed = sZeroVector;
                eff->accel = sZeroVector;
                eff->workf[EFF_ROLL] = i * (std::numbers::pi_v<float> / 4.0f);
                eff->workf[EFF_YAW] = std::numbers::pi_v<float> / 2.0f;
                eff->workf[EFF_DIST] = 0.0f;
                eff->workf[EFF_SCALE] = arg1 / 1000.0f;
                eff->work[EFF_ARGS] = arg2;
                eff->work[EFF_UNKS1] = 0;
                eff->alpha = 255;
                eff->frame = (s16)Rand_ZeroFloat(1000.0f);
                break;
            }
        }
    }
}

void BossTw_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;
    s16 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);

    if (thisv->actor.params >= TW_FIRE_BLAST) {
        // Blasts
        Actor_SetScale(&thisv->actor, 0.01f);
        thisv->actor.update = BossTw_BlastUpdate;
        thisv->actor.draw = BossTw_BlastDraw;
        thisv->actor.flags &= ~ACTOR_FLAG_0;

        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitBlasts);

        if (thisv->actor.params == TW_FIRE_BLAST || thisv->actor.params == TW_FIRE_BLAST_GROUND) {
            thisv->actionFunc = BossTw_BlastFire;
            thisv->collider.info.toucher.effect = 1;
        } else if (thisv->actor.params == TW_ICE_BLAST || thisv->actor.params == TW_ICE_BLAST_GROUND) {
            thisv->actionFunc = BossTw_BlastIce;
        } else if (thisv->actor.params >= TW_DEATHBALL_KOTAKE) {
            thisv->actionFunc = BossTw_DeathBall;
            thisv->actor.draw = BossTw_DrawDeathBall;
            thisv->workf[TAIL_ALPHA] = 128.0f;

            if (thisx->params == TW_DEATHBALL_KOTAKE) {
                thisx->world.rot.y = sTwinrovaPtr->actor.world.rot.y + 0x4000;
            } else {
                thisx->world.rot.y = sTwinrovaPtr->actor.world.rot.y - 0x4000;
            }
        }

        thisv->timers[1] = 150;
        return;
    }

    Actor_SetScale(&thisv->actor, 2.5 * 0.01f);
    thisv->actor.colChkInfo.mass = 255;
    thisv->actor.colChkInfo.health = 0;
    Collider_InitCylinder(globalCtx, &thisv->collider);

    if (!sTwInitalized) {
        sTwInitalized = true;
        globalCtx->envCtx.unk_BF = 1;
        globalCtx->envCtx.unk_BE = 1;
        globalCtx->envCtx.unk_BD = 1;
        globalCtx->envCtx.unk_D8 = 0.0f;

        D_8094C874 = D_8094C876 = D_8094C878 = D_8094C87A = D_8094C87C = D_8094C87E = D_8094C870 = D_8094C86F =
            D_8094C872 = sBeamDivertTimer = sEnvType = sGroundBlastType = sFreezeState = sTwinrovaBlastType =
                sFixedBlatSeq = sShieldFireCharge = sShieldIceCharge = 0;

        D_8094C858 = D_8094C854 = 0.0f;
        sFixedBlastType = Rand_ZeroFloat(1.99f);
        globalCtx->specialEffects = sTwEffects;

        for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
            sTwEffects[i].type = TWEFF_NONE;
        }
    }

    if (thisv->actor.params == TW_KOTAKE) {
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitKoumeKotake);
        thisv->actor.naviEnemyId = 0x33;
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_tw_Skel_0070E0, &object_tw_Anim_006F28, NULL, NULL, 0);

        if (gSaveContext.eventChkInf[7] & 0x20) {
            // began twinrova battle
            BossTw_SetupFlyTo(thisv, globalCtx);
            thisv->actor.world.pos.x = -600.0f;
            thisv->actor.world.pos.y = 400.0f;
            thisv->actor.world.pos.z = 0.0f;
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
        } else {
            BossTw_SetupCSWait(thisv, globalCtx);
        }

        Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_006F28, -3.0f);
        thisv->visible = true;
    } else if (thisv->actor.params == TW_KOUME) {
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitKoumeKotake);
        thisv->actor.naviEnemyId = 0x32;
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_tw_Skel_01F888, &object_tw_Anim_006F28, NULL, NULL, 0);

        if (gSaveContext.eventChkInf[7] & 0x20) {
            // began twinrova battle
            BossTw_SetupFlyTo(thisv, globalCtx);
            thisv->actor.world.pos.x = 600.0f;
            thisv->actor.world.pos.y = 400.0f;
            thisv->actor.world.pos.z = 0.0f;
        } else {
            BossTw_SetupCSWait(thisv, globalCtx);
        }

        Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_006F28, -3.0f);
        thisv->visible = true;
    } else {
        // Twinrova
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInitTwinrova);
        thisv->actor.naviEnemyId = 0x5B;
        thisv->actor.colChkInfo.health = 24;
        thisv->actor.update = BossTw_TwinrovaUpdate;
        thisv->actor.draw = BossTw_TwinrovaDraw;
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_tw_Skel_032020, &object_tw_Anim_0244B4, NULL, NULL, 0);
        Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_0244B4, -3.0f);

        if (gSaveContext.eventChkInf[7] & 0x20) {
            // began twinrova battle
            BossTw_SetupWait(thisv, globalCtx);
        } else {
            BossTw_TwinrovaSetupIntroCS(thisv, globalCtx);
            thisv->actor.world.pos.x = 0.0f;
            thisv->actor.world.pos.y = 1000.0f;
            thisv->actor.world.pos.z = 0.0f;
        }

        thisv->actor.params = TW_TWINROVA;
        sTwinrovaPtr = thisv;

        if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) {
            // twinrova has been defeated.
            Actor_Kill(&thisv->actor);
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, 600.0f, 230.0f, 0.0f, 0,
                               0, 0, WARP_DUNGEON_ADULT);
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, -600.0f, 230.0f, 0.0f, 0, 0, 0, 0);
        } else {
            sKotakePtr = (BossTw*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                                     thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                                     thisv->actor.world.pos.z, 0, 0, 0, TW_KOTAKE);
            sKoumePtr = (BossTw*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                                    thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                                    thisv->actor.world.pos.z, 0, 0, 0, TW_KOUME);
            sKotakePtr->actor.parent = &sKoumePtr->actor;
            sKoumePtr->actor.parent = &sKotakePtr->actor;
        }
    }

    thisv->fogR = globalCtx->lightCtx.fogColor[0];
    thisv->fogG = globalCtx->lightCtx.fogColor[1];
    thisv->fogB = globalCtx->lightCtx.fogColor[2];
    thisv->fogNear = globalCtx->lightCtx.fogNear;
    thisv->fogFar = 1000.0f;
}

void BossTw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BossTw* thisv = (BossTw*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    if (thisx->params < TW_FIRE_BLAST) {
        SkelAnime_Free(&thisv->skelAnime, globalCtx);
    }

    if (thisx->params == TW_TWINROVA) {
        sTwInitalized = false;
    }
}

void BossTw_SetupTurnToPlayer(BossTw* thisv, GlobalContext* globalCtx) {
    BossTw* otherTw = (BossTw*)thisv->actor.parent;

    thisv->actionFunc = BossTw_TurnToPlayer;

    if ((otherTw != NULL) && (otherTw->actionFunc == BossTw_ShootBeam)) {
        thisv->timers[0] = 40;
    } else {
        thisv->timers[0] = 60;
    }

    thisv->rotateSpeed = 0.0f;
}

void BossTw_TurnToPlayer(BossTw* thisv, GlobalContext* globalCtx) {
    BossTw* otherTw = (BossTw*)thisv->actor.parent;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.0f);
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.shape.rot.x, 0, 5, thisv->rotateSpeed);
    Math_ApproachF(&thisv->rotateSpeed, 4096.0f, 1.0f, 200.0f);
    func_8002D908(&thisv->actor);
    func_8002D7EC(&thisv->actor);
    if (thisv->timers[0] == 0) {
        if ((otherTw->actionFunc != BossTw_ShootBeam) && thisv->work[CAN_SHOOT]) {
            thisv->work[CAN_SHOOT] = false;
            BossTw_SetupShootBeam(thisv, globalCtx);
            thisv->actor.speedXZ = 0.0f;
        } else {
            BossTw_SetupFlyTo(thisv, globalCtx);
        }
    }
}

void BossTw_SetupFlyTo(BossTw* thisv, GlobalContext* globalCtx) {
    static Vec3f sPillarPositions[] = {
        { 600.0f, 400.0f, 0.0f }, { 0.0f, 400.0f, 600.0f }, { -600.0f, 400.0f, 0.0f }, { 0.0f, 400.0f, -600.0f }
    };
    BossTw* otherTw = (BossTw*)thisv->actor.parent;

    thisv->unk_5F8 = 1;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = BossTw_FlyTo;
    thisv->rotateSpeed = 0.0f;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_006F28, -10.0f);
    if ((Rand_ZeroOne() < 0.5f) && (otherTw != NULL && otherTw->actionFunc == BossTw_ShootBeam)) {
        // Other Sister is shooting a beam, go near them.
        thisv->targetPos.x = otherTw->actor.world.pos.x + Rand_CenteredFloat(200.0f);
        thisv->targetPos.y = Rand_ZeroFloat(200.0f) + 340.0f;
        thisv->targetPos.z = otherTw->actor.world.pos.z + Rand_CenteredFloat(200.0f);
        thisv->timers[0] = (s16)Rand_ZeroFloat(50.0f) + 50;
    } else if (Rand_ZeroOne() < 0.5f) {
        // Fly to a random spot.
        thisv->targetPos.x = Rand_CenteredFloat(800.0f);
        thisv->targetPos.y = Rand_ZeroFloat(200.0f) + 340.0f;
        thisv->targetPos.z = Rand_CenteredFloat(800.0f);
        thisv->timers[0] = (s16)Rand_ZeroFloat(50.0f) + 50;
    } else {
        // fly to a random pillar.
        s16 idx = Rand_ZeroFloat(ARRAY_COUNT(sPillarPositions) - 0.01f);

        thisv->targetPos = sPillarPositions[idx];
        thisv->timers[0] = 200;
        thisv->work[CAN_SHOOT] = true;
    }
}

void BossTw_FlyTo(BossTw* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 pitchTarget;
    f32 yawTarget;
    f32 xzDist;

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
    Math_ApproachF(&thisv->scepterAlpha, 0.0f, 1.0f, 10.0f);
    SkelAnime_Update(&thisv->skelAnime);

    xDiff = thisv->targetPos.x - thisv->actor.world.pos.x;
    yDiff = thisv->targetPos.y - thisv->actor.world.pos.y;
    zDiff = thisv->targetPos.z - thisv->actor.world.pos.z;

    yawTarget = (s16)(Math_FAtan2F(xDiff, zDiff) * (32768.0f / std::numbers::pi_v<float>));
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    pitchTarget = (s16)(Math_FAtan2F(yDiff, xzDist) * (32768.0f / std::numbers::pi_v<float>));

    Math_ApproachS(&thisv->actor.world.rot.x, pitchTarget, 0xA, thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.world.rot.y, yawTarget, 0xA, thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.shape.rot.y, yawTarget, 0xA, thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.shape.rot.x, pitchTarget, 0xA, thisv->rotateSpeed);
    Math_ApproachF(&thisv->rotateSpeed, 4096.0f, 1.0f, 100.0f);
    Math_ApproachF(&thisv->actor.speedXZ, 10.0f, 1.0f, 1.0f);
    func_8002D908(&thisv->actor);
    func_8002D7EC(&thisv->actor);

    if ((thisv->timers[0] == 0) || (xzDist < 70.0f)) {
        BossTw_SetupTurnToPlayer(thisv, globalCtx);
    }
}

void BossTw_SetupShootBeam(BossTw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->actionFunc = BossTw_ShootBeam;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_007688, -5.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_007688);
    thisv->timers[1] = 70;
    thisv->targetPos = player->actor.world.pos;
    thisv->csState1 = 0;
    thisv->beamDist = 0.0f;
    thisv->beamReflectionDist = 0.0f;
    thisv->beamShootState = -1;
    thisv->beamScale = 0.01f;
    thisv->beamReflectionOrigin = thisv->beamOrigin;
    thisv->flameAlpha = 0.0f;
    thisv->spawnPortalAlpha = 0.0f;
    thisv->spawnPortalScale = 2000.0f;
    thisv->updateRate1 = 0.0f;
    thisv->portalRotation = 0.0f;
    thisv->updateRate2 = 0.0f;
}

void BossTw_SpawnGroundBlast(BossTw* thisv, GlobalContext* globalCtx, s16 blastType) {
    BossTw* groundBlast;
    s16 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        velocity.x = Rand_CenteredFloat(20.0f);
        velocity.y = Rand_ZeroFloat(10.0f);
        velocity.z = Rand_CenteredFloat(20.0f);
        accel.y = 0.2f;
        accel.x = Rand_CenteredFloat(0.25f);
        accel.z = Rand_CenteredFloat(0.25f);
        pos = thisv->groundBlastPos;
        BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 8, blastType, 75);
    }

    if (blastType == 1) {
        sGroundBlastType = 1;
        groundBlast = (BossTw*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                                  thisv->groundBlastPos.x, thisv->groundBlastPos.y,
                                                  thisv->groundBlastPos.z, 0, 0, 0, TW_FIRE_BLAST_GROUND);
        if (groundBlast != NULL) {
            if (sTwinrovaPtr->actionFunc == BossTw_Wait) {
                groundBlast->timers[0] = 100;
            } else {
                groundBlast->timers[0] = 50;
            }
            sKoumePtr->workf[KM_GD_FLM_A] = sKoumePtr->workf[KM_GD_SMOKE_A] = sKoumePtr->workf[KM_GRND_CRTR_A] = 255.0f;
            sKoumePtr->workf[KM_GD_FLM_SCL] = 1.0f;
            sKoumePtr->workf[KM_GD_CRTR_SCL] = 0.005f;
            sKoumePtr->groundBlastPos2 = groundBlast->actor.world.pos;
            sEnvType = 4;
        }
    } else {
        sGroundBlastType = 2;
        groundBlast = (BossTw*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                                  thisv->groundBlastPos.x, thisv->groundBlastPos.y,
                                                  thisv->groundBlastPos.z, 0, 0, 0, TW_ICE_BLAST_GROUND);
        if (groundBlast != NULL) {
            if (sTwinrovaPtr->actionFunc == BossTw_Wait) {
                groundBlast->timers[0] = 100;
            } else {
                groundBlast->timers[0] = 50;
            }

            sKotakePtr->workf[UNK_F11] = 50.0f;
            sKotakePtr->workf[UNK_F9] = 250.0f;
            sKotakePtr->workf[UNK_F12] = 0.005f;
            sKotakePtr->workf[UNK_F14] = 1.0f;
            sKotakePtr->workf[UNK_F16] = 70.0f;
            sKotakePtr->groundBlastPos2 = groundBlast->actor.world.pos;
            sEnvType = 3;
        }
    }
}

s32 BossTw_BeamHitPlayerCheck(BossTw* thisv, GlobalContext* globalCtx) {
    Vec3f offset;
    Vec3f beamDistFromPlayer;
    Player* player = GET_PLAYER(globalCtx);
    s16 i;

    offset.x = player->actor.world.pos.x - thisv->beamOrigin.x;
    offset.y = player->actor.world.pos.y - thisv->beamOrigin.y;
    offset.z = player->actor.world.pos.z - thisv->beamOrigin.z;

    Matrix_RotateX(-thisv->beamPitch, MTXMODE_NEW);
    Matrix_RotateY(-thisv->beamYaw, MTXMODE_APPLY);
    Matrix_MultVec3f(&offset, &beamDistFromPlayer);

    if (fabsf(beamDistFromPlayer.x) < 20.0f && fabsf(beamDistFromPlayer.y) < 50.0f && beamDistFromPlayer.z > 100.0f &&
        beamDistFromPlayer.z <= thisv->beamDist) {
        if (sTwinrovaPtr->timers[2] == 0) {
            sTwinrovaPtr->timers[2] = 150;
            thisv->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
            func_8002F6D4(globalCtx, &thisv->actor, 3.0f, thisv->actor.shape.rot.y, 0.0f, 0x20);

            if (thisv->actor.params == 0) {
                if (sFreezeState == 0) {
                    sFreezeState = 1;
                }
            } else if (!player->isBurning) {
                for (i = 0; i < ARRAY_COUNT(player->flameTimers); i++) {
                    player->flameTimers[i] = Rand_S16Offset(0, 200);
                }

                player->isBurning = true;
                func_8002F7DC(&player->actor, player->ageProperties->unk_92 + NA_SE_VO_LI_DEMO_DAMAGE);
            }
        }

        return true;
    }
    return false;
}

/**
 * Checks if the beam shot by `thisv` will be reflected
 * returns 0 if the beam will not be reflected,
 * returns 1 if the beam will be reflected,
 * and returns 2 if the beam will be diverted backwards
 */
s32 BossTw_CheckBeamReflection(BossTw* thisv, GlobalContext* globalCtx) {
    Vec3f offset;
    Vec3f vec;
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags1 & 0x400000 &&
        (s16)(player->actor.shape.rot.y - thisv->actor.shape.rot.y + 0x8000) < 0x2000 &&
        (s16)(player->actor.shape.rot.y - thisv->actor.shape.rot.y + 0x8000) > -0x2000) {
        // player is shielding and facing angles are less than 45 degrees in either direction
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 10.0f;

        // set beam check point to 10 units in front of link.
        Matrix_RotateY(player->actor.shape.rot.y / 32768.0f * std::numbers::pi_v<float>, MTXMODE_NEW);
        Matrix_MultVec3f(&offset, &vec);

        // calculates a vector where the origin is at the beams origin,
        // and the positive z axis is pointing in the direction the beam
        // is shooting
        offset.x = player->actor.world.pos.x + vec.x - thisv->beamOrigin.x;
        offset.y = player->actor.world.pos.y + vec.y - thisv->beamOrigin.y;
        offset.z = player->actor.world.pos.z + vec.z - thisv->beamOrigin.z;

        Matrix_RotateX(-thisv->beamPitch, MTXMODE_NEW);
        Matrix_RotateY(-thisv->beamYaw, MTXMODE_APPLY);
        Matrix_MultVec3f(&offset, &vec);

        if (fabsf(vec.x) < 30.0f && fabsf(vec.y) < 70.0f && vec.z > 100.0f && vec.z <= thisv->beamDist) {
            // if the beam's origin is within 30 x units, 70 y units, is farther than 100 units
            // and the distance from the beams origin to 10 units in front of link is less than the beams
            // current distance (the distance of the beam is equal to or longer than the distance to 10 units
            // in front of link)
            if (Player_HasMirrorShieldEquipped(globalCtx)) {
                // player has mirror shield equipped
                thisv->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
                return 1;
            }

            if (sBeamDivertTimer > 10) {
                return 0;
            }

            if (sBeamDivertTimer == 0) {
                // beam hit the shield, normal shield equipped,
                // divert the beam backwards from link's Y rotation
                BossTw_AddShieldDeflectEffect(globalCtx, 10.0f, thisv->actor.params);
                globalCtx->envCtx.unk_D8 = 1.0f;
                thisv->timers[0] = 10;
                func_80078884(NA_SE_IT_SHIELD_REFLECT_MG2);
            }

            sBeamDivertTimer++;
            thisv->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
            return 2;
        }
    }

    return 0;
}

s32 BossTw_BeamReflHitCheck(BossTw* thisv, Vec3f* pos) {
    Vec3f offset;
    Vec3f beamDistFromTarget;

    offset.x = pos->x - thisv->beamReflectionOrigin.x;
    offset.y = pos->y - thisv->beamReflectionOrigin.y;
    offset.z = pos->z - thisv->beamReflectionOrigin.z;

    Matrix_RotateX(-thisv->beamReflectionPitch, MTXMODE_NEW);
    Matrix_RotateY(-thisv->beamReflectionYaw, MTXMODE_APPLY);
    Matrix_MultVec3f(&offset, &beamDistFromTarget);

    if (fabsf(beamDistFromTarget.x) < 50.0f && fabsf(beamDistFromTarget.y) < 50.0f && beamDistFromTarget.z > 100.0f &&
        beamDistFromTarget.z <= thisv->beamReflectionDist) {
        thisv->beamReflectionDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z)) * 1.1f;
        return true;
    } else {
        return false;
    }
}

f32 BossTw_GetFloorY(Vec3f* pos) {
    Vec3f posRotated;

    if (fabsf(pos->x) < 350.0f && fabsf(pos->z) < 350.0f && pos->y < 240.0f) {
        if (pos->y > 200.0f) {
            return 240.0f;
        }
        return 35.0f;
    }

    if (fabsf(pos->x) < 110.0f && ((fabsf(pos->z - 600.0f) < 110.0f) || (fabsf(pos->z + 600.0f) < 110.0f)) &&
        (pos->y < 230.0f)) {
        if (pos->y > 190.0f) {
            return 230.0f;
        }
        return 35.0f;
    }

    if (fabsf(pos->z) < 110.0f && ((fabsf(pos->x - 600.0f) < 110.0f) || (fabsf(pos->x + 600.0f) < 110.0f)) &&
        (pos->y < 230.0f)) {
        if (pos->y > 190.0f) {
            return 230.0f;
        }
        return 35.0f;
    }

    if (pos->y < -20.0f) {
        return 0.0f;
    }

    if (fabsf(pos->x) > 1140.0f || fabsf(pos->z) > 1140.0f) {
        return 35.0f;
    }

    Matrix_Push();
    Matrix_RotateY((45.0f * (std::numbers::pi_v<float> / 180.0f)), MTXMODE_NEW);
    Matrix_MultVec3f(pos, &posRotated);
    Matrix_Pop();

    if (fabsf(posRotated.x) > 920.0f || fabsf(posRotated.z) > 920.0f) {
        return 35.0f;
    }

    return -100.0f;
}

void BossTw_ShootBeam(BossTw* thisv, GlobalContext* globalCtx) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 floorY;
    Vec3f sp130;
    Vec3s sp128;
    Player* player = GET_PLAYER(globalCtx);
    BossTw* otherTw = (BossTw*)thisv->actor.parent;
    Input* input = &globalCtx->state.input[0];

    Math_ApproachF(&thisv->actor.world.pos.y, 400.0f, 0.05f, thisv->actor.speedXZ);
    Math_ApproachF(&thisv->actor.speedXZ, 5.0f, 1.0f, 0.25f);
    SkelAnime_Update(&thisv->skelAnime);
    thisv->beamRoll += -0.3f;

    if (thisv->timers[1] != 0) {
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, thisv->rotateSpeed);
        if ((player->stateFlags1 & 0x400000) &&
            ((s16)((player->actor.shape.rot.y - thisv->actor.shape.rot.y) + 0x8000) < 0x2000) &&
            ((s16)((player->actor.shape.rot.y - thisv->actor.shape.rot.y) + 0x8000) > -0x2000)) {
            Math_ApproachF(&thisv->targetPos.x, player->bodyPartsPos[15].x, 1.0f, 400.0f);
            Math_ApproachF(&thisv->targetPos.y, player->bodyPartsPos[15].y, 1.0f, 400.0f);
            Math_ApproachF(&thisv->targetPos.z, player->bodyPartsPos[15].z, 1.0f, 400.0f);
        } else {
            Math_ApproachF(&thisv->targetPos.x, player->actor.world.pos.x, 1.0f, 400.0f);
            Math_ApproachF(&thisv->targetPos.y, player->actor.world.pos.y + 30.0f, 1.0f, 400.0f);
            Math_ApproachF(&thisv->targetPos.z, player->actor.world.pos.z, 1.0f, 400.0f);
        }

        thisv->timers[0] = 70;
        thisv->groundBlastPos.x = thisv->groundBlastPos.y = thisv->groundBlastPos.z = 0.0f;
        thisv->portalRotation += thisv->updateRate2 * 0.0025f;
        Math_ApproachF(&thisv->spawnPortalAlpha, 255.0f, 1.0f, 10.0f);
        Math_ApproachF(&thisv->updateRate2, 50.0f, 1.0f, 2.0f);

        if (thisv->timers[1] < 50) {
            if (thisv->timers[1] < 10) {
                if (thisv->timers[1] == 9) {
                    globalCtx->envCtx.unk_D8 = 0.5f;
                    globalCtx->envCtx.unk_BD = 3 - thisv->actor.params;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_MASIC_SET);
                }

                if (thisv->timers[1] == 5) {
                    thisv->scepterAlpha = 255;
                }

                if (thisv->timers[1] > 4) {
                    s16 j;
                    for (j = 0; j < 2; j++) {
                        for (i = 0; i < ARRAY_COUNT(thisv->scepterFlamePos); i++) {
                            Vec3f pos;
                            Vec3f velocity;
                            Vec3f accel;

                            pos.x = thisv->scepterFlamePos[i].x;
                            pos.y = thisv->scepterFlamePos[i].y;
                            pos.z = thisv->scepterFlamePos[i].z;
                            velocity.x = Rand_CenteredFloat(10.0f);
                            velocity.y = Rand_CenteredFloat(10.0f);
                            velocity.z = Rand_CenteredFloat(10.0f);
                            accel.x = 0.0f;
                            accel.y = 0.0f;
                            accel.z = 0.0f;
                            BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &accel, Rand_ZeroFloat(10.0f) + 25.0f,
                                                  thisv->actor.params);
                        }
                    }
                }
            }

            if (thisv->timers[1] < 20) {
                Math_ApproachF(&thisv->flameAlpha, 0, 1.0f, 20.0f);
                Math_ApproachF(&thisv->spawnPortalAlpha, 0, 1.0f, 30.0f);
            } else {
                Math_ApproachF(&thisv->flameAlpha, 255.0f, 1.0f, 10.0f);
                if (thisv->actor.params == 1) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_MS_FIRE - SFX_FLAG);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_MS_FREEZE - SFX_FLAG);
                }
            }

            thisv->flameRotation += thisv->updateRate1 * 0.0025f;
            Math_ApproachF(&thisv->spawnPortalScale, 0.0f, 0.1f, thisv->updateRate1);
            Math_ApproachF(&thisv->updateRate1, 50.0f, 1.0f, 2.0f);
        }

        if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
            Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_009398, 0.0f);
            thisv->workf[ANIM_SW_TGT] = 10000.0f;
        }

        if (thisv->timers[1] == 1) {
            Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_003614, 0.0f);
            thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_003614);
            thisv->unk_4DC = 0.0f;
            thisv->spawnPortalAlpha = 0.0f;
            thisv->flameAlpha = 0.0f;
            sBeamDivertTimer = 0;
        }
    } else {
        if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
            Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_003E34, 0.0f);
            thisv->workf[ANIM_SW_TGT] = 10000.0f;
        }

        if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT] - 5.0f)) {
            thisv->beamShootState = 0;
            sEnvType = thisv->actor.params + 1;
        }

        if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT] - 13.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_THROW_MASIC);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_SHOOT_VOICE);
        }

        xDiff = thisv->targetPos.x - thisv->beamOrigin.x;
        yDiff = thisv->targetPos.y - thisv->beamOrigin.y;
        zDiff = thisv->targetPos.z - thisv->beamOrigin.z;

        thisv->beamYaw = Math_FAtan2F(xDiff, zDiff);
        thisv->beamPitch = -Math_FAtan2F(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff)));

        switch (thisv->beamShootState) {
            case -1:
                break;
            case 0:
                if (thisv->timers[0] != 0) {
                    s32 beamReflection = BossTw_CheckBeamReflection(thisv, globalCtx);

                    if (beamReflection == 1) {
                        Vec3f pos;
                        Vec3f velocity;
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
                            velocity.x = Rand_CenteredFloat(15.0f);
                            velocity.y = Rand_CenteredFloat(15.0f);
                            velocity.z = Rand_CenteredFloat(15.0f);
                            pos = player->bodyPartsPos[15];
                            BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 5,
                                                thisv->actor.params, 150);
                        }

                        thisv->beamShootState = 1;
                        func_80078914(&player->actor.projectedPos, NA_SE_IT_SHIELD_REFLECT_MG);
                        Matrix_MtxFToYXZRotS(&player->shieldMf, &sp128, 0);
                        sp128.y += 0x8000;
                        sp128.x = -sp128.x;
                        thisv->magicDir.x = sp128.x;
                        thisv->magicDir.y = sp128.y;
                        thisv->groundBlastPos.x = 0.0f;
                        thisv->groundBlastPos.y = 0.0f;
                        thisv->groundBlastPos.z = 0.0f;
                        globalCtx->envCtx.unk_D8 = 1.0f;
                        func_800AA000(0.0f, 0x64, 5, 4);
                    } else if (beamReflection == 0) {
                        BossTw_BeamHitPlayerCheck(thisv, globalCtx);

                        if (thisv->csState1 == 0) {
                            Math_ApproachF(&thisv->beamDist, 2.0f * sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff)), 1.0f,
                                           40.0f);
                        }
                    }
                }

                SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->beamReflectionOrigin,
                                             &thisv->unk_54C, &thisv->actor.projectedW);

                if (thisv->actor.params == 1) {
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_FIRE - SFX_FLAG, &thisv->unk_54C, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                } else {
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG, &thisv->unk_54C, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                break;

            case 1:
                if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
                    Player* player = GET_PLAYER(globalCtx);

                    thisv->beamDist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
                    Math_ApproachF(&thisv->beamReflectionDist, 2000.0f, 1.0f, 40.0f);
                    Math_ApproachF(&thisv->targetPos.x, player->bodyPartsPos[15].x, 1.0f, 400.0f);
                    Math_ApproachF(&thisv->targetPos.y, player->bodyPartsPos[15].y, 1.0f, 400.0f);
                    Math_ApproachF(&thisv->targetPos.z, player->bodyPartsPos[15].z, 1.0f, 400.0f);
                    if ((thisv->work[CS_TIMER_1] % 4) == 0) {
                        BossTw_AddRingEffect(globalCtx, &player->bodyPartsPos[15], 0.5f, 3.0f, 0xFF, thisv->actor.params,
                                             1, 150);
                    }
                } else {
                    thisv->beamShootState = 0;
                    thisv->beamReflectionDist = 0.0f;
                }

                SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &thisv->unk_530, &thisv->unk_558,
                                             &thisv->actor.projectedW);

                if (thisv->actor.params == 1) {
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_FIRE - SFX_FLAG, &thisv->unk_558, 4U, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_REFL_FIRE - SFX_FLAG, &thisv->unk_558, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                } else {
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG, &thisv->unk_558, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                    Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_REFL_FREEZE - SFX_FLAG, &thisv->unk_558, 4, &D_801333E0,
                                           &D_801333E0, &D_801333E8);
                }
                break;
        }

        if (thisv->timers[0] == 0 && (sEnvType == 1 || sEnvType == 2)) {
            sEnvType = 0;
        }

        if (thisv->timers[0] == 0) {
            Math_ApproachF(&thisv->beamScale, 0.0f, 1.0f, 0.0005f);

            if (thisv->beamScale == 0.0f) {
                BossTw_SetupFinishBeamShoot(thisv, globalCtx);
                thisv->beamReflectionDist = 0.0f;
                thisv->beamDist = 0.0f;
            }
        }
    }

    Matrix_Translate(thisv->beamOrigin.x, thisv->beamOrigin.y, thisv->beamOrigin.z, MTXMODE_NEW);
    Matrix_RotateY(thisv->beamYaw, MTXMODE_APPLY);
    Matrix_RotateX(thisv->beamPitch, MTXMODE_APPLY);

    sp130.x = 0.0f;
    sp130.y = 0.0f;
    sp130.z = thisv->beamDist + -5.0f;

    Matrix_MultVec3f(&sp130, &thisv->beamReflectionOrigin);

    if ((thisv->csState1 == 0) && (thisv->beamShootState == 0) && (thisv->timers[0] != 0)) {
        thisv->groundBlastPos.y = BossTw_GetFloorY(&thisv->beamReflectionOrigin);

        if (thisv->groundBlastPos.y >= 0.0f) {
            thisv->csState1 = 1;
            thisv->groundBlastPos.x = thisv->beamReflectionOrigin.x;
            thisv->groundBlastPos.z = thisv->beamReflectionOrigin.z;
            BossTw_SpawnGroundBlast(thisv, globalCtx, thisv->actor.params);
            thisv->timers[0] = 20;
        }
    }

    if (thisv->beamShootState == 1) {
        if (thisv->csState1 == 0) {
            Matrix_MtxFToYXZRotS(&player->shieldMf, &sp128, 0);
            sp128.y += 0x8000;
            sp128.x = -sp128.x;
            Math_ApproachS(&thisv->magicDir.x, sp128.x, 5, 0x2000);
            Math_ApproachS(&thisv->magicDir.y, sp128.y, 5, 0x2000);
            thisv->beamReflectionPitch = (thisv->magicDir.x / 32768.0f) * std::numbers::pi_v<float>;
            thisv->beamReflectionYaw = (thisv->magicDir.y / 32768.0f) * std::numbers::pi_v<float>;
        }

        Matrix_Translate(thisv->beamReflectionOrigin.x, thisv->beamReflectionOrigin.y, thisv->beamReflectionOrigin.z,
                         MTXMODE_NEW);
        Matrix_RotateY(thisv->beamReflectionYaw, MTXMODE_APPLY);
        Matrix_RotateX(thisv->beamReflectionPitch, MTXMODE_APPLY);

        sp130.x = 0.0f;
        sp130.y = 0.0f;
        sp130.z = thisv->beamReflectionDist + -170.0f;

        Matrix_MultVec3f(&sp130, &thisv->unk_530);

        if (thisv->csState1 == 0) {
            sp130.z = 0.0f;

            for (i = 0; i < 200; i++) {
                Vec3f spBC;

                Matrix_MultVec3f(&sp130, &spBC);
                floorY = BossTw_GetFloorY(&spBC);
                thisv->groundBlastPos.y = floorY;

                if (floorY >= 0.0f) {
                    if ((thisv->groundBlastPos.y != 35.0f) && (0.0f < thisv->beamReflectionPitch) &&
                        (thisv->timers[0] != 0)) {
                        thisv->csState1 = 1;
                        thisv->groundBlastPos.x = spBC.x;
                        thisv->groundBlastPos.z = spBC.z;
                        BossTw_SpawnGroundBlast(thisv, globalCtx, thisv->actor.params);
                        thisv->timers[0] = 20;
                    } else {
                        for (i = 0; i < 5; i++) {
                            Vec3f velocity;
                            Vec3f accel;

                            velocity.x = Rand_CenteredFloat(20.0f);
                            velocity.y = Rand_CenteredFloat(20.0f);
                            velocity.z = Rand_CenteredFloat(20.0f);

                            accel.x = 0.0f;
                            accel.y = 0.0f;
                            accel.z = 0.0f;

                            BossTw_AddFlameEffect(globalCtx, &thisv->unk_530, &velocity, &accel,
                                                  Rand_ZeroFloat(10.0f) + 25.0f, thisv->actor.params);
                        }

                        thisv->beamReflectionDist = sp130.z;
                        Math_ApproachF(&globalCtx->envCtx.unk_D8, 0.8f, 1.0f, 0.2f);
                    }
                    break;
                }

                sp130.z += 20.0f;

                if (thisv->beamReflectionDist < sp130.z) {
                    break;
                }
            }
        }

        if (BossTw_BeamReflHitCheck(thisv, &thisv->actor.world.pos) && (thisv->work[CS_TIMER_1] % 4) == 0) {
            BossTw_AddRingEffect(globalCtx, &thisv->unk_530, 0.5f, 3.0f, 255, thisv->actor.params, 1, 150);
        }

        if (BossTw_BeamReflHitCheck(thisv, &otherTw->actor.world.pos) && otherTw->actionFunc != BossTw_HitByBeam) {
            for (i = 0; i < 50; i++) {
                Vec3f pos;
                Vec3f velocity;
                Vec3f accel;

                pos.x = otherTw->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                pos.y = otherTw->actor.world.pos.y + Rand_CenteredFloat(50.0f);
                pos.z = otherTw->actor.world.pos.z + Rand_CenteredFloat(50.0f);

                velocity.x = Rand_CenteredFloat(20.0f);
                velocity.y = Rand_CenteredFloat(20.0f);
                velocity.z = Rand_CenteredFloat(20.0f);

                accel.x = 0.0f;
                accel.y = 0.0f;
                accel.z = 0.0f;

                BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &accel, Rand_ZeroFloat(10.0f) + 25.0f,
                                      thisv->actor.params);
            }

            BossTw_SetupHitByBeam(otherTw, globalCtx);
            Audio_PlayActorSound2(&otherTw->actor, NA_SE_EN_TWINROBA_DAMAGE_VOICE);
            globalCtx->envCtx.unk_D8 = 1.0f;
            otherTw->actor.colChkInfo.health++;
        }
    }
}

void BossTw_SetupFinishBeamShoot(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_FinishBeamShoot;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_004548, 0.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_004548);
}

void BossTw_FinishBeamShoot(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->scepterAlpha, 0.0f, 1.0f, 10.0f);

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        if (sTwinrovaPtr->timers[2] == 0) {
            BossTw_SetupFlyTo(thisv, globalCtx);
        } else {
            BossTw_SetupLaugh(thisv, globalCtx);
        }

        thisv->scepterAlpha = 0.0f;
    }
}

void BossTw_SetupHitByBeam(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_HitByBeam;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_00578C, 0.0f);
    thisv->timers[0] = 53;
    thisv->actor.speedXZ = 0.0f;

    if (thisv->actor.params == 0) {
        thisv->work[FOG_TIMER] = 20;
    }
}

void BossTw_HitByBeam(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if ((thisv->work[CS_TIMER_1] % 4) == 0) {
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;

        pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(80.0f);
        pos.y = thisv->actor.world.pos.y + Rand_CenteredFloat(80.0f);
        pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(80.0f);

        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;

        accel.x = 0.0f;
        accel.y = 0.1f;
        accel.z = 0.0f;

        BossTw_AddDmgCloud(globalCtx, thisv->actor.params + 2, &pos, &velocity, &accel, Rand_ZeroFloat(10.0f) + 15.0f, 0,
                           0, 150);
    }

    if (thisv->actor.params == 1) {
        Math_ApproachF(&thisv->fogR, 255.0f, 1.0f, 30.0f);
        Math_ApproachF(&thisv->fogG, 255.0f, 1.0f, 30.0f);
        Math_ApproachF(&thisv->fogB, 255.0f, 1.0f, 30.0f);
        Math_ApproachF(&thisv->fogNear, 900.0f, 1.0f, 30.0f);
        Math_ApproachF(&thisv->fogFar, 1099.0f, 1.0f, 30.0f);
    }

    Math_ApproachF(&thisv->actor.world.pos.y, ((Math_SinS(thisv->work[CS_TIMER_1] * 1500) * 20.0f) + 350.0f) + 50.0f,
                   0.1f, thisv->actor.speedXZ);
    Math_ApproachF(&thisv->actor.speedXZ, 5.0f, 1.0f, 1.0f);

    thisv->actor.world.pos.y -= 50.0f;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 4);
    thisv->actor.world.pos.y += 50.0f;

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->timers[0] == 1) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_006530, 0.0f);
        thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_006530);
    }

    if ((thisv->timers[0] == 0) && Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        BossTw_SetupFlyTo(thisv, globalCtx);
    }
}

void BossTw_SetupLaugh(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_Laugh;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_0088C8, 0.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_0088C8);
    thisv->actor.speedXZ = 0.0f;
}

void BossTw_Laugh(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 10.0f)) {
        if (thisv->actor.params == TW_KOUME) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_LAUGH);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_LAUGH2);
        }
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        BossTw_SetupFlyTo(thisv, globalCtx);
    }
}

void BossTw_SetupSpin(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_Spin;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_007CA8, -3.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_007CA8);
    thisv->actor.speedXZ = 0.0f;
    SkelAnime_Update(&thisv->skelAnime);
    thisv->timers[0] = 20;
}

void BossTw_Spin(BossTw* thisv, GlobalContext* globalCtx) {
    if (thisv->timers[0] != 0) {
        thisv->collider.base.colType = COLTYPE_METAL;
        thisv->actor.shape.rot.y -= 0x3000;

        if ((thisv->timers[0] % 4) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_ROLL);
        }
    } else {
        SkelAnime_Update(&thisv->skelAnime);
        Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 3, 0x2000);

        if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
            BossTw_SetupFlyTo(thisv, globalCtx);
        }
    }
}

void BossTw_SetupMergeCS(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_MergeCS;
    thisv->rotateSpeed = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_006F28, -10.0f);
}

void BossTw_MergeCS(BossTw* thisv, GlobalContext* globalCtx) {
    Math_ApproachF(&thisv->scepterAlpha, 0.0f, 1.0f, 10.0f);
    SkelAnime_Update(&thisv->skelAnime);
}

void BossTw_SetupWait(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_Wait;
    thisv->visible = false;
    thisv->actor.world.pos.y = -2000.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void BossTw_Wait(BossTw* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params == TW_TWINROVA) && (sKoumePtr->actionFunc == BossTw_FlyTo) &&
        (sKotakePtr->actionFunc == BossTw_FlyTo) &&
        ((sKoumePtr->actor.colChkInfo.health + sKotakePtr->actor.colChkInfo.health) >= 4)) {

        BossTw_TwinrovaSetupMergeCS(thisv, globalCtx);
        BossTw_SetupMergeCS(sKotakePtr, globalCtx);
        BossTw_SetupMergeCS(sKoumePtr, globalCtx);
    }
}

void BossTw_TwinrovaSetupMergeCS(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaMergeCS;
    thisv->csState2 = 0;
    thisv->csState1 = 0;
}

void BossTw_TwinrovaMergeCS(BossTw* thisv, GlobalContext* globalCtx) {
    s16 i;
    Vec3f spB0;
    Vec3f spA4;
    Player* player = GET_PLAYER(globalCtx);

    switch (thisv->csState2) {
        case 0:
            thisv->csState2 = 1;
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 0x39);
            thisv->subCamId = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, 0, CAM_STAT_WAIT);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->subCamId, CAM_STAT_ACTIVE);
            thisv->subCamDist = 800.0f;
            thisv->subCamYaw = std::numbers::pi_v<float>;
            sKoumePtr->actor.world.rot.x = 0;
            sKoumePtr->actor.shape.rot.x = 0;
            sKotakePtr->actor.world.rot.x = 0;
            sKotakePtr->actor.shape.rot.x = 0;
            thisv->workf[UNK_F9] = 0.0f;
            thisv->workf[UNK_F10] = 0.0f;
            thisv->workf[UNK_F11] = 600.0f;
            Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xC800FF);
            thisv->work[CS_TIMER_2] = 0;
            // fallthrough
        case 1:
            if (thisv->work[CS_TIMER_2] == 20) {
                Message_StartTextbox(globalCtx, 0x6059, NULL);
            }

            if (thisv->work[CS_TIMER_2] == 80) {
                Message_StartTextbox(globalCtx, 0x605A, NULL);
            }

            thisv->subCamAt.x = 0.0f;
            thisv->subCamAt.y = 440.0f;
            thisv->subCamAt.z = 0.0f;

            spB0.x = 0.0f;
            spB0.y = 0.0f;
            spB0.z = thisv->subCamDist;

            Matrix_RotateY(thisv->subCamYaw, MTXMODE_NEW);
            Matrix_MultVec3f(&spB0, &spA4);

            thisv->subCamEye.x = spA4.x;
            thisv->subCamEye.y = 300.0f;
            thisv->subCamEye.z = spA4.z;

            Math_ApproachF(&thisv->subCamYaw, 0.3f, 0.02f, 0.03f);
            Math_ApproachF(&thisv->subCamDist, 200.0f, 0.1f, 5.0f);
            break;

        case 2:
            spB0.x = 0.0f;
            spB0.y = 0.0f;
            spB0.z = thisv->subCamDist;
            Matrix_RotateY(thisv->subCamYaw, MTXMODE_NEW);
            Matrix_MultVec3f(&spB0, &spA4);
            thisv->subCamEye.x = spA4.x;
            thisv->subCamEye.z = spA4.z;
            Math_ApproachF(&thisv->subCamEye.y, 420.0f, 0.1f, thisv->subCamUpdateRate * 20.0f);
            Math_ApproachF(&thisv->subCamAt.y, 470.0f, 0.1f, thisv->subCamUpdateRate * 6.0f);
            Math_ApproachF(&thisv->subCamYaw, 0.3f, 0.02f, 0.03f);
            Math_ApproachF(&thisv->subCamDist, 60.0f, 0.1f, thisv->subCamUpdateRate * 32.0f);
            Math_ApproachF(&thisv->subCamUpdateRate, 1, 1, 0.1f);
            break;
    }

    if (thisv->subCamId != 0) {
        if (thisv->unk_5F9 == 0) {
            Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt, &thisv->subCamEye);
        } else {
            Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt2, &thisv->subCamEye2);
        }
    }

    switch (thisv->csState1) {
        case 0:
            Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
            Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
            spB0.x = thisv->workf[UNK_F11];
            spB0.y = 400.0f;
            spB0.z = 0.0f;
            Matrix_RotateY(thisv->workf[UNK_F9], MTXMODE_NEW);
            Matrix_MultVec3f(&spB0, &spA4);
            sKoumePtr->actor.world.pos.x = spA4.x;
            sKoumePtr->actor.world.pos.y = spA4.y;
            sKoumePtr->actor.world.pos.z = spA4.z;
            sKoumePtr->actor.shape.rot.y = (thisv->workf[UNK_F9] / std::numbers::pi_v<float>) * 32768.0f;
            sKotakePtr->actor.world.pos.x = -spA4.x;
            sKotakePtr->actor.world.pos.y = spA4.y;
            sKotakePtr->actor.world.pos.z = -spA4.z;
            sKotakePtr->actor.shape.rot.y = ((thisv->workf[UNK_F9] / std::numbers::pi_v<float>) * 32768.0f) + 32768.0f;
            Math_ApproachF(&thisv->workf[UNK_F11], 0.0f, 0.1f, 7.0f);
            thisv->workf[UNK_F9] -= thisv->workf[UNK_F10];
            Math_ApproachF(&thisv->workf[UNK_F10], 0.5f, 1, 0.0039999997f);
            if (thisv->workf[UNK_F11] < 10.0f) {
                if (!thisv->work[PLAYED_CHRG_SFX]) {
                    Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_POWERUP);
                    thisv->work[PLAYED_CHRG_SFX] = true;
                }

                Math_ApproachF(&sKoumePtr->actor.scale.x, 0.005000001f, 1, 0.0003750001f);

                for (i = 0; i < 4; i++) {
                    Vec3f pos;
                    f32 yOffset;
                    f32 xScale;

                    xScale = sKoumePtr->actor.scale.x * 3000.0f;
                    yOffset = Rand_CenteredFloat(xScale * 2.0f);
                    pos.x = 3000.0f;
                    pos.y = 400.0f + yOffset;
                    pos.z = 0.0f;
                    BossTw_AddMergeFlameEffect(globalCtx, &pos, Rand_ZeroFloat(5.0f) + 10.0f,
                                               sqrtf(SQ(xScale) - SQ(yOffset)), Rand_ZeroFloat(1.99f));
                }

                if (sKoumePtr->actor.scale.x <= 0.0051f) {
                    Vec3f pos;
                    Vec3f velocity;
                    Vec3f accel;

                    thisv->actor.world.pos.y = 400.0f;

                    for (i = 0; i < 50; i++) {
                        pos = thisv->actor.world.pos;
                        velocity.x = Rand_CenteredFloat(20.0f);
                        velocity.y = Rand_CenteredFloat(20.0f);
                        velocity.z = Rand_CenteredFloat(20.0f);
                        pos.x += velocity.x;
                        pos.y += velocity.y;
                        pos.z += velocity.z;
                        accel.z = accel.y = accel.x = 0.0f;
                        BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &accel, Rand_ZeroFloat(10.0f) + 25.0f,
                                              velocity.x < 0.0f);
                    }

                    thisv->csState1 = 1;
                    thisv->visible = true;
                    thisv->actor.flags |= ACTOR_FLAG_0;
                    thisv->actor.shape.rot.y = 0;
                    BossTw_SetupWait(sKotakePtr, globalCtx);
                    BossTw_SetupWait(sKoumePtr, globalCtx);
                    Actor_SetScale(&thisv->actor, 0.0f);
                    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_038E2C, 0.0f);
                    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_038E2C);
                    thisv->timers[0] = 50;
                    func_8002DF54(globalCtx, &thisv->actor, 2);
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
                }
            }

            sKotakePtr->actor.scale.x = sKotakePtr->actor.scale.y = sKotakePtr->actor.scale.z =
                sKoumePtr->actor.scale.y = sKoumePtr->actor.scale.z = sKoumePtr->actor.scale.x;
            break;

        case 1:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
                Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, -15.0f);
            }

            sEnvType = -1;
            globalCtx->envCtx.unk_BD = 4;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, 1, 1, 0.1f);
            // fallthrough
        case 2:
            SkelAnime_Update(&thisv->skelAnime);
            Math_ApproachF(&thisv->actor.scale.x, 0.0069999993f, 1, 0.0006999999f);
            thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;

            if (thisv->timers[0] == 1) {
                thisv->csState2 = 2;
                thisv->subCamUpdateRate = 0.0f;
                thisv->timers[1] = 65;
                thisv->timers[2] = 90;
                thisv->timers[3] = 50;
                player->actor.world.pos.x = 0.0f;
                player->actor.world.pos.y = 240.0f;
                player->actor.world.pos.z = 270.0f;
                player->actor.world.rot.y = player->actor.shape.rot.y = -0x8000;
                thisv->subCamEye2.x = 0.0f;
                thisv->subCamEye2.y = 290.0f;
                thisv->subCamEye2.z = 222.0f;
                thisv->subCamAt2.x = player->actor.world.pos.x;
                thisv->subCamAt2.y = player->actor.world.pos.y + 54.0f;
                thisv->subCamAt2.z = player->actor.world.pos.z;
            }

            if (thisv->timers[3] == 19) {
                func_8002DF54(globalCtx, &thisv->actor, 5);
            }

            if (thisv->timers[3] == 16) {
                func_8002F7DC(&player->actor, player->ageProperties->unk_92 + NA_SE_VO_LI_SURPRISE);
            }

            if ((thisv->timers[3] != 0) && (thisv->timers[3] < 20)) {
                thisv->unk_5F9 = 1;
                Math_ApproachF(&thisv->subCamEye2.z, 242.0f, 0.2f, 100.0f);
            } else {
                thisv->unk_5F9 = 0;
            }

            if (thisv->timers[1] == 8) {
                thisv->work[TW_BLINK_IDX] = 8;
                func_80078884(NA_SE_EN_TWINROBA_YOUNG_WINK);
            }
            if (thisv->timers[2] == 4) {
                sEnvType = 0;
                globalCtx->envCtx.unk_BE = 5;
            }

            if (thisv->timers[2] == 1) {
                Camera* cam = Gameplay_GetCamera(globalCtx, MAIN_CAM);

                cam->eye = thisv->subCamEye;
                cam->eyeNext = thisv->subCamEye;
                cam->at = thisv->subCamAt;
                func_800C08AC(globalCtx, thisv->subCamId, 0);
                thisv->subCamId = 0;
                thisv->csState2 = thisv->subCamId;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                thisv->work[TW_PLLR_IDX] = 0;
                thisv->targetPos = sTwinrovaPillarPos[0];
                BossTw_TwinrovaSetupFly(thisv, globalCtx);
            }
            break;
    }
}

void BossTw_SetupDeathCS(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_DeathCS;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_0004A4, -3.0f);
    thisv->unk_5F8 = 0;
    thisv->work[CS_TIMER_2] = Rand_ZeroFloat(20.0f);
}

void BossTw_DeathCS(BossTw* thisv, GlobalContext* globalCtx) {
    if (thisv->timers[0] == 0) {
        SkelAnime_Update(&thisv->skelAnime);
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->work[YAW_TGT], 5, thisv->rotateSpeed);
    Math_ApproachF(&thisv->rotateSpeed, 20480.0f, 1.0f, 1000.0f);

    if (sTwinrovaPtr->work[CS_TIMER_2] > 140) {
        Math_ApproachF(&thisv->fogR, 100.0f, 1.0f, 15.0f);
        Math_ApproachF(&thisv->fogG, 255.0f, 1.0f, 15.0f);
        Math_ApproachF(&thisv->fogB, 255.0f, 1.0f, 15.0f);
        Math_ApproachF(&thisv->fogNear, 850.0f, 1.0f, 15.0f);
        Math_ApproachF(&thisv->fogFar, 1099.0f, 1.0f, 15.0f);
    }
}

void BossTw_SetupCSWait(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_CSWait;
    thisv->visible = false;
    thisv->actor.world.pos.y = -2000.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

/**
 * Do nothing while waiting for the inital cutscene to start
 */
void BossTw_CSWait(BossTw* thisv, GlobalContext* globalCtx) {
}

void BossTw_TwinrovaSetupIntroCS(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaIntroCS;
    thisv->visible = false;
    thisv->actor.world.pos.y = -2000.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void BossTw_TwinrovaIntroCS(BossTw* thisv, GlobalContext* globalCtx) {
    u8 updateCam = 0;
    s16 i;
    Vec3f sp90;
    Vec3f sp84;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->csSfxTimer > 220 && thisv->csSfxTimer < 630) {
        func_80078884(NA_SE_EN_TWINROBA_UNARI - SFX_FLAG);
    }

    if (thisv->csSfxTimer == 180) {
        func_80078914(&D_8094A7D0, NA_SE_EN_TWINROBA_LAUGH);
        func_80078914(&D_8094A7D0, NA_SE_EN_TWINROBA_LAUGH2);
        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KOTAKE_KOUME);
    }

    thisv->csSfxTimer++;

    switch (thisv->csState2) {
        case 0:
            thisv->csSfxTimer = 0;

            if (SQ(player->actor.world.pos.x) + SQ(player->actor.world.pos.z) < SQ(150.0f)) {
                player->actor.world.pos.x = player->actor.world.pos.z = .0f;
                thisv->csState2 = 1;
                func_80064520(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 0x39);
                thisv->subCamId = Gameplay_CreateSubCamera(globalCtx);
                Gameplay_ChangeCameraStatus(globalCtx, 0, CAM_STAT_WAIT);
                Gameplay_ChangeCameraStatus(globalCtx, thisv->subCamId, CAM_STAT_ACTIVE);
                thisv->subCamEye.x = 0.0f;
                thisv->subCamEye.y = 350;
                thisv->subCamEye.z = 200;

                thisv->subCamEyeTarget.x = 450;
                thisv->subCamEyeTarget.y = 900;

                thisv->subCamAt.x = 0;
                thisv->subCamAt.y = 270;
                thisv->subCamAt.z = 0;

                thisv->subCamAtTarget.x = 0;
                thisv->subCamAtTarget.y = 240;
                thisv->subCamAtTarget.z = 140;

                thisv->subCamEyeTarget.z = 530;
                thisv->subCamEyeStep.x = fabsf(thisv->subCamEyeTarget.x - thisv->subCamEye.x);
                thisv->subCamEyeStep.y = fabsf(thisv->subCamEyeTarget.y - thisv->subCamEye.y);
                thisv->subCamEyeStep.z = fabsf(thisv->subCamEyeTarget.z - thisv->subCamEye.z);
                thisv->subCamAtStep.x = fabsf(thisv->subCamAtTarget.x - thisv->subCamAt.x);
                thisv->subCamAtStep.y = fabsf(thisv->subCamAtTarget.y - thisv->subCamAt.y);
                thisv->subCamAtStep.z = fabsf(thisv->subCamAtTarget.z - thisv->subCamAt.z);

                thisv->subCamDistStep = 0.05f;
                thisv->work[CS_TIMER_1] = 0;
            }
            break;

        case 1:
            updateCam = 1;

            if (thisv->work[CS_TIMER_1] == 30) {
                Message_StartTextbox(globalCtx, 0x6048, NULL);
            }

            Math_ApproachF(&thisv->subCamUpdateRate, 0.01f, 1.0f, 0.0001f);

            if (thisv->work[CS_TIMER_1] > 100) {
                globalCtx->envCtx.unk_BD = 0;
                Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.03f);
            }

            if (thisv->work[CS_TIMER_1] == 180) {
                func_80078884(NA_SE_EN_TWINROBA_APPEAR_MS);
            }

            if (thisv->work[CS_TIMER_1] > 180) {
                thisv->spawnPortalScale = 0.05f;
                Math_ApproachF(&thisv->spawnPortalAlpha, 255.0f, 1.0f, 5.f);

                if (thisv->work[CS_TIMER_1] >= 236) {
                    thisv->csState2 = 2;
                    sKoumePtr->visible = 1;
                    Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_0004A4, 0.0f);
                    sKoumePtr->actor.world.pos.x = 0.0f;
                    sKoumePtr->actor.world.pos.y = 80.0f;
                    sKoumePtr->actor.world.pos.z = 600.0f;
                    sKoumePtr->actor.shape.rot.y = sKoumePtr->actor.world.rot.y = -0x8000;

                    thisv->subCamEye.x = -30;
                    thisv->subCamEye.y = 260;
                    thisv->subCamEye.z = 470;

                    thisv->subCamAt.x = 0.0F;
                    thisv->subCamAt.y = 270;
                    thisv->subCamAt.z = 600.0F;

                    thisv->work[CS_TIMER_1] = 0;

                    Actor_SetScale(&sKoumePtr->actor, 0.014999999f);
                }
            }
            break;

        case 2:
            SkelAnime_Update(&sKoumePtr->skelAnime);
            Math_ApproachF(&sKoumePtr->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            thisv->subCamEye.x -= 0.2f;
            thisv->subCamEye.z += 0.2f;

            if (thisv->work[CS_TIMER_1] > 50) {
                thisv->csState2 = 3;

                thisv->subCamEyeTarget.x = -30;
                thisv->subCamEyeTarget.y = 260;
                thisv->subCamEyeTarget.z = 530;

                thisv->subCamAtTarget.x = 0.0f;
                thisv->subCamAtTarget.y = 265;
                thisv->subCamAtTarget.z = 580;

                thisv->subCamEyeStep.x = fabsf(thisv->subCamEyeTarget.x - thisv->subCamEye.x);
                thisv->subCamEyeStep.y = fabsf(thisv->subCamEyeTarget.y - thisv->subCamEye.y);
                thisv->subCamEyeStep.z = fabsf(thisv->subCamEyeTarget.z - thisv->subCamEye.z);
                thisv->subCamAtStep.x = fabsf(thisv->subCamAtTarget.x - thisv->subCamAt.x);
                thisv->subCamAtStep.y = fabsf(thisv->subCamAtTarget.y - thisv->subCamAt.y);
                thisv->subCamAtStep.z = fabsf(thisv->subCamAtTarget.z - thisv->subCamAt.z);
                thisv->subCamUpdateRate = 0;
                thisv->subCamDistStep = 0.1f;
                thisv->work[CS_TIMER_1] = 0;
            }
            break;

        case 3:
            SkelAnime_Update(&sKoumePtr->skelAnime);
            updateCam = 1;
            Math_ApproachF(&sKoumePtr->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            Math_ApproachF(&thisv->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (thisv->work[CS_TIMER_1] == 30) {
                Message_StartTextbox(globalCtx, 0x6049, NULL);
            }

            if (thisv->work[CS_TIMER_1] > 80) {
                thisv->csState2 = 4;
                thisv->actor.speedXZ = 0;

                thisv->subCamEyeTarget.x = -80.0f;
                thisv->subCamEyeTarget.y = 260.0f;
                thisv->subCamEyeTarget.z = 430.0f;

                thisv->subCamAtTarget.x = sKoumePtr->actor.world.pos.x;
                thisv->subCamAtTarget.y = sKoumePtr->actor.world.pos.y + 20.0f;
                thisv->subCamAtTarget.z = sKoumePtr->actor.world.pos.z;

                thisv->subCamEyeStep.x = fabsf(thisv->subCamEyeTarget.x - thisv->subCamEye.x);
                thisv->subCamEyeStep.y = fabsf(thisv->subCamEyeTarget.y - thisv->subCamEye.y);
                thisv->subCamEyeStep.z = fabsf(thisv->subCamEyeTarget.z - thisv->subCamEye.z);
                thisv->subCamAtStep.x = fabsf(thisv->subCamAtTarget.x - thisv->subCamAt.x);
                thisv->subCamAtStep.y = fabsf(thisv->subCamAtTarget.y - thisv->subCamAt.y);
                thisv->subCamAtStep.z = fabsf(thisv->subCamAtTarget.z - thisv->subCamAt.z);
                thisv->subCamUpdateRate = 0.0f;
                thisv->subCamDistStep = 0.05f;
                Animation_MorphToPlayOnce(&sKoumePtr->skelAnime, &object_tw_Anim_000AAC, 0.0f);
                thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_000AAC);
                thisv->work[CS_TIMER_1] = 0;
            }
            break;

        case 4:
            updateCam = 1;
            SkelAnime_Update(&sKoumePtr->skelAnime);
            thisv->subCamAtTarget.y = 20.0f + sKoumePtr->actor.world.pos.y;
            Math_ApproachF(&sKoumePtr->actor.world.pos.y, 350, 0.1f, thisv->actor.speedXZ);
            Math_ApproachF(&thisv->actor.speedXZ, 9.0f, 1.0f, 0.9f);
            Math_ApproachF(&thisv->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (thisv->work[CS_TIMER_1] >= 30) {
                if (thisv->work[CS_TIMER_1] < 45) {
                    globalCtx->envCtx.unk_BE = 0;
                    globalCtx->envCtx.unk_BD = 2;
                    globalCtx->envCtx.unk_D8 = 1.0f;
                } else {
                    Math_ApproachZeroF(&globalCtx->envCtx.unk_D8, 1.0f, 0.1f);
                }

                if (thisv->work[CS_TIMER_1] == 30) {
                    for (i = 0; i < 50; i++) {
                        Vec3f pos;
                        Vec3f velocity;

                        pos.x = sKoumePtr->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                        pos.y = sKoumePtr->actor.world.pos.y + Rand_CenteredFloat(50.0f);
                        pos.z = sKoumePtr->actor.world.pos.z + Rand_CenteredFloat(50.0f);
                        velocity.x = Rand_CenteredFloat(20.0f);
                        velocity.y = Rand_CenteredFloat(20.0f);
                        velocity.z = Rand_CenteredFloat(20.0f);
                        BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &sZeroVector, Rand_ZeroFloat(10.0f) + 25.0f,
                                              1);
                    }

                    Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    globalCtx->envCtx.unk_D8 = 0;
                }

                if (thisv->work[CS_TIMER_1] >= 35) {
                    if (thisv->work[CS_TIMER_1] < 50) {
                        Math_ApproachF(&sKoumePtr->actor.scale.x,
                                       ((Math_SinS(thisv->work[CS_TIMER_1] * 0x4200) * 20.0f) / 10000.0f) + 0.024999999f,
                                       1.0f, 0.005f);
                    } else {
                        if (thisv->work[CS_TIMER_1] == 50) {
                            Animation_MorphToPlayOnce(&sKoumePtr->skelAnime, &object_tw_Anim_0088C8, -5);
                            thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_0088C8);
                        }

                        if (thisv->work[CS_TIMER_1] == 60) {
                            Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_LAUGH);
                        }

                        if (Animation_OnFrame(&sKoumePtr->skelAnime, thisv->workf[ANIM_SW_TGT])) {
                            Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_006F28, 0.f);
                            thisv->workf[ANIM_SW_TGT] = 1000.0f;
                        }

                        Math_ApproachF(&sKoumePtr->actor.scale.x, 0.024999999f, 0.1f, 0.005f);
                    }

                    Actor_SetScale(&sKoumePtr->actor, sKoumePtr->actor.scale.x);
                    sKoumePtr->actor.shape.rot.y = -0x8000;
                    sKoumePtr->unk_5F8 = 1;

                    if (thisv->work[CS_TIMER_1] == 0x64) {
                        thisv->csState2 = 10;
                        thisv->work[CS_TIMER_1] = 0;
                        thisv->subCamYawStep = 0.0f;
                        sKotakePtr->visible = 1;
                        Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_0004A4, 0.0f);
                        sKotakePtr->actor.world.pos.x = 0.0f;
                        sKotakePtr->actor.world.pos.y = 80.0f;
                        sKotakePtr->actor.world.pos.z = -600.0f;
                        sKotakePtr->actor.shape.rot.y = sKotakePtr->actor.world.rot.y = 0;
                        thisv->work[CS_TIMER_1] = 0;

                        thisv->subCamEye.x = -30.0f;
                        thisv->subCamEye.y = 260.0f;
                        thisv->subCamEye.z = -470.0f;

                        thisv->subCamAt.x = 0;
                        thisv->subCamAt.y = 270.0f;
                        thisv->subCamAt.z = -600.0f;
                        Actor_SetScale(&sKotakePtr->actor, 0.014999999f);
                    }
                } else {
                    sKoumePtr->actor.shape.rot.y = sKoumePtr->actor.shape.rot.y + (s16)thisv->subCamYawStep;
                }
            } else {
                if ((thisv->work[CS_TIMER_1] % 8) == 0) {
                    Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_ROLL);
                }

                sKoumePtr->actor.shape.rot.y = sKoumePtr->actor.shape.rot.y + (s16)thisv->subCamYawStep;
                Math_ApproachF(&thisv->subCamYawStep, 12288.0f, 1.0f, 384.0f);

                if (Animation_OnFrame(&sKoumePtr->skelAnime, thisv->workf[ANIM_SW_TGT])) {
                    Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_006F28, 0.0f);
                    thisv->workf[ANIM_SW_TGT] = 1000.0f;
                }
            }
            break;

        case 10:
            SkelAnime_Update(&sKotakePtr->skelAnime);
            Math_ApproachF(&sKotakePtr->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            thisv->subCamEye.x -= 0.2f;
            thisv->subCamEye.z -= 0.2f;

            if (thisv->work[CS_TIMER_1] >= 0x33) {
                thisv->csState2 = 11;
                thisv->subCamEyeTarget.x = -30;
                thisv->subCamEyeTarget.y = 260;
                thisv->subCamEyeTarget.z = -530;
                thisv->subCamAtTarget.x = 0;
                thisv->subCamAtTarget.y = 265;
                thisv->subCamAtTarget.z = -580;
                thisv->subCamEyeStep.x = fabsf(thisv->subCamEyeTarget.x - thisv->subCamEye.x);
                thisv->subCamEyeStep.y = fabsf(thisv->subCamEyeTarget.y - thisv->subCamEye.y);
                thisv->subCamEyeStep.z = fabsf(thisv->subCamEyeTarget.z - thisv->subCamEye.z);
                thisv->subCamAtStep.x = fabsf(thisv->subCamAtTarget.x - thisv->subCamAt.x);
                thisv->subCamAtStep.y = fabsf(thisv->subCamAtTarget.y - thisv->subCamAt.y);
                thisv->subCamAtStep.z = fabsf(thisv->subCamAtTarget.z - thisv->subCamAt.z);
                thisv->subCamUpdateRate = 0;
                thisv->subCamDistStep = 0.1f;
                thisv->work[CS_TIMER_1] = 0;
            }
            break;

        case 11:
            SkelAnime_Update(&sKotakePtr->skelAnime);
            updateCam = 1;
            Math_ApproachF(&sKotakePtr->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            Math_ApproachF(&thisv->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (thisv->work[CS_TIMER_1] == 30) {
                Message_StartTextbox(globalCtx, 0x604A, NULL);
            }

            if (thisv->work[CS_TIMER_1] > 80) {
                thisv->csState2 = 12;
                thisv->actor.speedXZ = 0;

                thisv->subCamEyeTarget.y = 260.0f;
                thisv->subCamEyeTarget.x = -80.0f;
                thisv->subCamEyeTarget.z = -430.0f;

                thisv->subCamAtTarget.x = sKotakePtr->actor.world.pos.x;
                thisv->subCamAtTarget.y = sKotakePtr->actor.world.pos.y + 20.0f;
                thisv->subCamAtTarget.z = sKotakePtr->actor.world.pos.z;

                thisv->subCamEyeStep.x = fabsf(thisv->subCamEyeTarget.x - thisv->subCamEye.x);
                thisv->subCamEyeStep.y = fabsf(thisv->subCamEyeTarget.y - thisv->subCamEye.y);
                thisv->subCamEyeStep.z = fabsf(thisv->subCamEyeTarget.z - thisv->subCamEye.z);
                thisv->subCamAtStep.x = fabsf(thisv->subCamAtTarget.x - thisv->subCamAt.x);
                thisv->subCamAtStep.y = fabsf(thisv->subCamAtTarget.y - thisv->subCamAt.y);
                thisv->subCamAtStep.z = fabsf(thisv->subCamAtTarget.z - thisv->subCamAt.z);
                thisv->subCamUpdateRate = 0;
                thisv->subCamDistStep = 0.05f;
                Animation_MorphToPlayOnce(&sKotakePtr->skelAnime, &object_tw_Anim_000AAC, 0);
                thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_000AAC);
                thisv->work[CS_TIMER_1] = 0;
            }
            break;

        case 12:
            updateCam = 1;
            SkelAnime_Update(&sKotakePtr->skelAnime);
            thisv->subCamAtTarget.y = sKotakePtr->actor.world.pos.y + 20.0f;
            Math_ApproachF(&sKotakePtr->actor.world.pos.y, 350, 0.1f, thisv->actor.speedXZ);
            Math_ApproachF(&thisv->actor.speedXZ, 9.0f, 1.0f, 0.9f);
            Math_ApproachF(&thisv->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (thisv->work[CS_TIMER_1] >= 30) {
                if (thisv->work[CS_TIMER_1] < 45) {
                    globalCtx->envCtx.unk_BD = 3;
                    globalCtx->envCtx.unk_D8 = 1.0f;
                } else {
                    Math_ApproachZeroF(&globalCtx->envCtx.unk_D8, 1.0f, 0.1f);
                }

                if (thisv->work[CS_TIMER_1] == 30) {
                    for (i = 0; i < 50; i++) {
                        Vec3f pos;
                        Vec3f velocity;
                        pos.x = sKotakePtr->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                        pos.y = sKotakePtr->actor.world.pos.y + Rand_CenteredFloat(50.0f);
                        pos.z = sKotakePtr->actor.world.pos.z + Rand_CenteredFloat(50.0f);
                        velocity.x = Rand_CenteredFloat(20.0f);
                        velocity.y = Rand_CenteredFloat(20.0f);
                        velocity.z = Rand_CenteredFloat(20.0f);
                        BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &sZeroVector, Rand_ZeroFloat(10.f) + 25.0f,
                                              0);
                    }

                    Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    globalCtx->envCtx.unk_D8 = 0.0f;
                }

                if (thisv->work[CS_TIMER_1] >= 35) {
                    if (thisv->work[CS_TIMER_1] < 50) {
                        Math_ApproachF(&sKotakePtr->actor.scale.x,
                                       ((Math_SinS(thisv->work[CS_TIMER_1] * 0x4200) * 20.0f) / 10000.0f) + 0.024999999f,
                                       1.0f, 0.005f);
                    } else {
                        if (thisv->work[CS_TIMER_1] == 50) {
                            Animation_MorphToPlayOnce(&sKotakePtr->skelAnime, &object_tw_Anim_0088C8, -5.0f);
                            thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_0088C8);
                        }

                        if (thisv->work[CS_TIMER_1] == 60) {
                            Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_LAUGH2);
                        }

                        if (Animation_OnFrame(&sKotakePtr->skelAnime, thisv->workf[ANIM_SW_TGT])) {
                            Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_006F28, 0.0f);
                            thisv->workf[ANIM_SW_TGT] = 1000.0f;
                        }

                        Math_ApproachF(&sKotakePtr->actor.scale.x, 0.024999999f, 0.1f, 0.005f);
                    }

                    Actor_SetScale(&sKotakePtr->actor, sKotakePtr->actor.scale.x);
                    sKotakePtr->actor.shape.rot.y = 0;
                    sKotakePtr->unk_5F8 = 1;

                    if (thisv->work[CS_TIMER_1] == 100) {
                        thisv->csState2 = 20;
                        thisv->work[CS_TIMER_1] = 0;

                        thisv->workf[UNK_F11] = 600.0f;

                        thisv->subCamEye.x = 800.0f;
                        thisv->subCamEye.y = 300.0f;
                        thisv->subCamEye.z = 0;

                        thisv->subCamAt.x = 0.0f;
                        thisv->subCamAt.y = 400.0f;
                        thisv->subCamAt.z = 0;

                        thisv->workf[UNK_F9] = -std::numbers::pi_v<float> / 2.0f;
                        thisv->workf[UNK_F10] = 0.0f;

                        thisv->subCamEyeStep.x = 0.0f;
                        thisv->spawnPortalAlpha = 0.0f;
                    }
                } else {
                    sKotakePtr->actor.shape.rot.y = sKotakePtr->actor.shape.rot.y + (s16)thisv->subCamYawStep;
                }
            } else {
                if ((thisv->work[CS_TIMER_1] % 8) == 0) {
                    Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_ROLL);
                }

                sKotakePtr->actor.shape.rot.y = sKotakePtr->actor.shape.rot.y + (s16)thisv->subCamYawStep;
                Math_ApproachF(&thisv->subCamYawStep, 12288.0f, 1.0f, 384.0f);

                if (Animation_OnFrame(&sKotakePtr->skelAnime, thisv->workf[ANIM_SW_TGT])) {
                    Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_006F28, 0.0f);
                    thisv->workf[ANIM_SW_TGT] = 1000.0f;
                }
            }
            break;

        case 20:
            if (thisv->work[CS_TIMER_1] > 20 && thisv->work[CS_TIMER_1] < 120) {
                globalCtx->envCtx.unk_BD = 1;
                Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.015f);
            }

            if (thisv->work[CS_TIMER_1] == 90) {
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x5A00FF);
            }

            if (thisv->work[CS_TIMER_1] == 120) {
                sEnvType = 0;
                globalCtx->envCtx.unk_BE = 1;
                globalCtx->envCtx.unk_BD = 1;
                globalCtx->envCtx.unk_D8 = 0.0f;
                TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gTwinrovaTitleCardTex), 160, 180, 128, 40); // OTRTODO
                gSaveContext.eventChkInf[7] |= 0x20;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
            }

            if (thisv->work[CS_TIMER_1] >= 160) {
                if (thisv->work[CS_TIMER_1] == 160) {
                    thisv->subCamEyeStep.x = 0.0f;
                }
                Math_ApproachF(&thisv->subCamEye.x, 0.0f, 0.05f, thisv->subCamEyeStep.x * 0.5f);
                Math_ApproachF(&thisv->subCamEye.z, 1000.0f, 0.05f, thisv->subCamEyeStep.x);
                Math_ApproachF(&thisv->subCamEyeStep.x, 40.0f, 1.0f, 1);
            } else {
                Math_ApproachF(&thisv->subCamEye.x, 300.0f, 0.05f, thisv->subCamEyeStep.x);
                Math_ApproachF(&thisv->subCamEyeStep.x, 5.0f, 1.0f, 0.5f);
            }

            if (thisv->work[CS_TIMER_1] < 200) {
                Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
                Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
                sp90.x = thisv->workf[UNK_F11];
                sp90.y = 400.0f;
                sp90.z = 0.0f;
                Matrix_RotateY(thisv->workf[UNK_F9], MTXMODE_NEW);
                Matrix_MultVec3f(&sp90, &sp84);
                sKoumePtr->actor.world.pos.x = sp84.x;
                sKoumePtr->actor.world.pos.y = sp84.y;
                sKoumePtr->actor.world.pos.z = sp84.z;
                sKoumePtr->actor.world.rot.y = sKoumePtr->actor.shape.rot.y = (thisv->workf[UNK_F9] / std::numbers::pi_v<float>) * 32768.0f;
                sKotakePtr->actor.world.pos.x = -sp84.x;
                sKotakePtr->actor.world.pos.y = sp84.y;
                sKotakePtr->actor.world.pos.z = -sp84.z;
                sKotakePtr->actor.shape.rot.y = sKotakePtr->actor.world.rot.y =
                    ((thisv->workf[UNK_F9] / std::numbers::pi_v<float>) * 32768.0f) + 32768.0f;
                Math_ApproachF(&thisv->workf[UNK_F11], 80.0f, 0.1f, 5.0f);
                thisv->workf[UNK_F9] -= thisv->workf[UNK_F10];
                Math_ApproachF(&thisv->workf[UNK_F10], 0.19999999f, 1.0f, 0.0019999994f);
            }

            if (thisv->work[CS_TIMER_1] == 200) {
                sKoumePtr->actionFunc = BossTw_FlyTo;
                sKotakePtr->actionFunc = BossTw_FlyTo;
                sKoumePtr->targetPos.x = 600.0f;
                sKoumePtr->targetPos.y = 400.0f;
                sKoumePtr->targetPos.z = 0.0f;
                sKoumePtr->timers[0] = 100;
                sKotakePtr->targetPos.x = -600.0f;
                sKotakePtr->targetPos.y = 400.0f;
                sKotakePtr->targetPos.z = 0.0f;
                sKotakePtr->timers[0] = 100;
            }

            if (thisv->work[CS_TIMER_1] == 260) {
                Camera* cam = Gameplay_GetCamera(globalCtx, MAIN_CAM);

                cam->eye = thisv->subCamEye;
                cam->eyeNext = thisv->subCamEye;
                cam->at = thisv->subCamAt;
                func_800C08AC(globalCtx, thisv->subCamId, 0);
                thisv->subCamId = 0;
                thisv->csState2 = thisv->subCamId;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                BossTw_SetupWait(thisv, globalCtx);
            }
            break;
    }

    if (thisv->subCamId != 0) {
        if (updateCam) {
            Math_ApproachF(&thisv->subCamEye.x, thisv->subCamEyeTarget.x, thisv->subCamDistStep,
                           thisv->subCamEyeStep.x * thisv->subCamUpdateRate);
            Math_ApproachF(&thisv->subCamEye.y, thisv->subCamEyeTarget.y, thisv->subCamDistStep,
                           thisv->subCamEyeStep.y * thisv->subCamUpdateRate);
            Math_ApproachF(&thisv->subCamEye.z, thisv->subCamEyeTarget.z, thisv->subCamDistStep,
                           thisv->subCamEyeStep.z * thisv->subCamUpdateRate);
            Math_ApproachF(&thisv->subCamAt.x, thisv->subCamAtTarget.x, thisv->subCamDistStep,
                           thisv->subCamAtStep.x * thisv->subCamUpdateRate);
            Math_ApproachF(&thisv->subCamAt.y, thisv->subCamAtTarget.y, thisv->subCamDistStep,
                           thisv->subCamAtStep.y * thisv->subCamUpdateRate);
            Math_ApproachF(&thisv->subCamAt.z, thisv->subCamAtTarget.z, thisv->subCamDistStep,
                           thisv->subCamAtStep.z * thisv->subCamUpdateRate);
        }

        Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt, &thisv->subCamEye);
    }
}

void BossTw_DeathBall(BossTw* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;
    s16 i;
    s16 yaw;

    if ((thisv->work[CS_TIMER_1] % 16) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_FB_FLY);
    }

    if (sTwinrovaPtr->csState2 < 2) {
        if (thisv->timers[0] == 0) {
            thisv->timers[0] = 20;
            thisv->targetPos.x = Rand_CenteredFloat(100.0f) + sTwinrovaPtr->actor.world.pos.x;
            thisv->targetPos.y = Rand_CenteredFloat(50.0f) + 400.0f;
            thisv->targetPos.z = Rand_CenteredFloat(100.0f) + sTwinrovaPtr->actor.world.pos.z;
        }

        thisv->timers[1] = 10;
        thisv->rotateSpeed = 8192.0f;
        thisv->actor.speedXZ = 5.0f;
    } else {
        if (thisv->timers[1] == 9) {
            thisv->targetPos.y = 413.0f;
            thisv->actor.world.pos.z = 0.0f;
            thisv->actor.world.pos.x = 0.0f;
            for (i = 0; i < ARRAY_COUNT(thisv->blastTailPos); i++) {
                thisv->blastTailPos[i] = thisv->actor.world.pos;
            }
        }

        if (thisv->actor.params == 0x69) {
            thisv->targetPos.x = sKoumePtr->actor.world.pos.x;
            thisv->targetPos.z = sKoumePtr->actor.world.pos.z;
        } else {
            thisv->targetPos.x = sKotakePtr->actor.world.pos.x;
            thisv->targetPos.z = sKotakePtr->actor.world.pos.z;
        }

        Math_ApproachF(&thisv->targetPos.y, 263.0f, 1.0f, 2.0f);

        if (thisv->targetPos.y == 263.0f) {
            Math_ApproachF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.2f);
            if (sTwinrovaPtr->csState2 == 3) {
                Actor_Kill(&thisv->actor);
            }
        }
    }

    xDiff = thisv->targetPos.x - thisv->actor.world.pos.x;
    yDiff = thisv->targetPos.y - thisv->actor.world.pos.y;
    zDiff = thisv->targetPos.z - thisv->actor.world.pos.z;

    yaw = Math_FAtan2F(xDiff, zDiff) * (32768 / std::numbers::pi_v<float>);
    Math_ApproachS(&thisv->actor.world.rot.x, Math_FAtan2F(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff))) * (32768 / std::numbers::pi_v<float>), 5,
                   thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.world.rot.y, yaw, 5, thisv->rotateSpeed);
    func_8002D908(&thisv->actor);
    func_8002D7EC(&thisv->actor);
}

void BossTw_TwinrovaSetupDeathCS(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaDeathCS;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_024374, -3.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->csState2 = thisv->csState1 = 0;
    thisv->work[CS_TIMER_1] = thisv->work[CS_TIMER_2] = 0;
    thisv->work[INVINC_TIMER] = 10000;
    BossTw_SetupDeathCS(sKoumePtr, globalCtx);
    BossTw_SetupDeathCS(sKotakePtr, globalCtx);
    sKotakePtr->timers[0] = 8;
    thisv->workf[UNK_F19] = 1.0f;
}

void BossTw_DeathCSMsgSfx(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;
    s32 pad3;
    s16 msgId2;
    s16 msgId1;
    u8 kotakeAnim;
    u8 koumeAnim;
    u8 sp35;

    msgId2 = 0;
    msgId1 = 0;
    kotakeAnim = 0;
    koumeAnim = 0;
    sp35 = 0;

    if (thisv->work[CS_TIMER_2] == 80) {
        koumeAnim = 1;
    }

    if (thisv->work[CS_TIMER_2] == 80) {
        msgId2 = 0x604B;
        sp35 = 50;
    }

    if (thisv->work[CS_TIMER_2] == 140) {
        kotakeAnim = koumeAnim = 2;
    }

    if (thisv->work[CS_TIMER_2] == 170) {
        kotakeAnim = 3;
        sKotakePtr->work[YAW_TGT] = -0x4000;
        sKotakePtr->rotateSpeed = 0.0f;
        Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_SENSE);
        msgId2 = 0x604C;
    }

    if (thisv->work[CS_TIMER_2] == 210) {
        D_8094C874 = 30;
    }

    if (thisv->work[CS_TIMER_2] == 270) {
        koumeAnim = 3;
        sKoumePtr->work[YAW_TGT] = 0x4000;
        sKoumePtr->rotateSpeed = 0.0f;
        Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_SENSE);
    }

    if (thisv->work[CS_TIMER_2] == 290) {
        msgId2 = 0x604D;
        sp35 = 35;
    }

    if (thisv->work[CS_TIMER_2] == 350) {
        koumeAnim = kotakeAnim = 2;
        sKoumePtr->work[YAW_TGT] = sKotakePtr->work[YAW_TGT] = 0;
        sKoumePtr->rotateSpeed = sKotakePtr->rotateSpeed = 0.0f;
    }

    if (thisv->work[CS_TIMER_2] == 380) {
        koumeAnim = kotakeAnim = 3;
    }

    if (thisv->work[CS_TIMER_2] == 400) {
        koumeAnim = kotakeAnim = 2;
    }

    if (thisv->work[CS_TIMER_2] == 430) {
        koumeAnim = 4;
        D_8094C874 = 435;
        D_8094C878 = 1;
    }

    if (thisv->work[CS_TIMER_2] > 440 && thisv->work[CS_TIMER_2] < 860) {
        func_80078884(NA_SE_EN_TWINROBA_FIGHT - SFX_FLAG);
    }

    if (thisv->work[CS_TIMER_2] == 430) {
        msgId2 = 0x604E;
    }

    if (thisv->work[CS_TIMER_2] == 480) {
        kotakeAnim = 4;
        sKotakePtr->work[YAW_TGT] = -0x4000;
    }

    if (thisv->work[CS_TIMER_2] == 500) {
        koumeAnim = 2;
    }

    if (thisv->work[CS_TIMER_2] == 480) {
        msgId1 = 0x604F;
    }

    if (thisv->work[CS_TIMER_2] == 530) {
        koumeAnim = 4;
        sKoumePtr->work[YAW_TGT] = 0x4000;
        D_8094C87A = 335;
        D_8094C87E = 1;
    }

    if (thisv->work[CS_TIMER_2] == 530) {
        msgId2 = 0x6050;
    }

    if (thisv->work[CS_TIMER_2] == 580) {
        msgId1 = 0x6051;
    }

    if (thisv->work[CS_TIMER_2] == 620) {
        msgId2 = 0x6052;
    }

    if (thisv->work[CS_TIMER_2] == 660) {
        msgId1 = 0x6053;
    }

    if (thisv->work[CS_TIMER_2] == 700) {
        msgId2 = 0x6054;
    }

    if (thisv->work[CS_TIMER_2] == 740) {
        msgId1 = 0x6055;
    }

    if (thisv->work[CS_TIMER_2] == 780) {
        msgId2 = 0x6056;
    }

    if (thisv->work[CS_TIMER_2] == 820) {
        msgId1 = 0x6057;
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x5000FF);
    }

    if (thisv->work[CS_TIMER_2] == 860) {
        koumeAnim = kotakeAnim = 3;
    }

    if (thisv->work[CS_TIMER_2] == 900) {
        Audio_PlayActorSound2(&sKoumePtr->actor, NA_SE_EN_TWINROBA_DIE);
        Audio_PlayActorSound2(&sKotakePtr->actor, NA_SE_EN_TWINROBA_DIE);
    }

    if (thisv->work[CS_TIMER_2] == 930) {
        msgId2 = 0x6058;
    }

    if (msgId2 != 0) {
        Message_StartTextbox(globalCtx, msgId2, NULL);

        if (sp35) {
            D_8094C876 = 10;
            D_8094C874 = sp35;
            D_8094C878 = 0;
        }
    }

    if (msgId1 != 0) {
        Message_StartTextbox(globalCtx, msgId1, NULL);
    }

    switch (kotakeAnim) {
        case 1:
            Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_00230C, -5.0f);
            break;
        case 2:
            Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_001D10, -5.0f);
            break;
        case 3:
            Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_0017E0, -5.0f);
            break;
        case 4:
            Animation_MorphToLoop(&sKotakePtr->skelAnime, &object_tw_Anim_0012A4, -5.0f);
            break;
    }

    switch (koumeAnim) {
        case 1:
            Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_00230C, -5.0f);
            break;
        case 2:
            Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_001D10, -5.0f);
            break;
        case 3:
            Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_0017E0, -5.0f);
            break;
        case 4:
            Animation_MorphToLoop(&sKoumePtr->skelAnime, &object_tw_Anim_0012A4, -5.0f);
            break;
    }

    if (thisv->work[CS_TIMER_2] >= 120 && thisv->work[CS_TIMER_2] < 500) {
        Math_ApproachF(&thisv->workf[UNK_F18], 255.0f, 0.1f, 5.0f);
    }

    if (thisv->work[CS_TIMER_2] >= 150) {
        Math_ApproachF(&sKoumePtr->workf[UNK_F17], (Math_SinS(thisv->work[CS_TIMER_1] * 2000) * 0.05f) + 0.4f, 0.1f,
                       0.01f);
        Math_ApproachF(&sKotakePtr->workf[UNK_F17], (Math_CosS(thisv->work[CS_TIMER_1] * 1700) * 0.05f) + 0.4f, 0.1f,
                       0.01f);

        if (thisv->work[CS_TIMER_2] >= 880) {
            Math_ApproachF(&sKotakePtr->actor.world.pos.y, 2000.0f, 1.0f, thisv->actor.speedXZ);
            Math_ApproachF(&sKoumePtr->actor.world.pos.y, 2000.0f, 1.0f, thisv->actor.speedXZ);
            Math_ApproachF(&thisv->actor.speedXZ, 10.0f, 1.0f, 0.25f);

            if (thisv->work[CS_TIMER_2] >= 930) {
                Math_ApproachF(&thisv->workf[UNK_F19], 5.0f, 1.0f, 0.05f);
                Math_ApproachF(&thisv->workf[UNK_F18], 0.0f, 1.0f, 3.0f);
            }

            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GOTO_HEAVEN - SFX_FLAG);
        } else {
            f32 yTarget = Math_CosS(thisv->work[CS_TIMER_2] * 1700) * 4.0f;
            Math_ApproachF(&sKotakePtr->actor.world.pos.y, 20.0f + (263.0f + yTarget), 0.1f, thisv->actor.speedXZ);
            yTarget = Math_SinS(thisv->work[CS_TIMER_2] * 1500) * 4.0f;
            Math_ApproachF(&sKoumePtr->actor.world.pos.y, 20.0f + (263.0f + yTarget), 0.1f, thisv->actor.speedXZ);
            Math_ApproachF(&thisv->actor.speedXZ, 1.0f, 1.0f, 0.05f);
        }
    }
}

void BossTw_TwinrovaDeathCS(BossTw* thisv, GlobalContext* globalCtx) {
    s16 i;
    Vec3f spD0;
    Player* player = GET_PLAYER(globalCtx);
    Camera* mainCam = Gameplay_GetCamera(globalCtx, MAIN_CAM);

    SkelAnime_Update(&thisv->skelAnime);
    thisv->work[UNK_S8] += 20;

    if (thisv->work[UNK_S8] > 255) {
        thisv->work[UNK_S8] = 255;
    }

    Math_ApproachF(&thisv->workf[UNK_F12], 0.0f, 1.0f, 0.05f);
    thisv->unk_5F8 = 1;

    switch (thisv->csState1) {
        case 0:
            if (thisv->work[CS_TIMER_1] == 15) {
                Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_0216DC, -3.0f);
            }

            if (thisv->work[CS_TIMER_1] >= 15) {
                Math_ApproachF(&thisv->actor.world.pos.y, 400.0f, 0.05f, 10.0f);
            }

            if (thisv->work[CS_TIMER_1] >= 55) {
                if (thisv->work[CS_TIMER_1] == 55) {
                    globalCtx->envCtx.unk_D8 = 0;
                }

                sEnvType = -1;
                globalCtx->envCtx.unk_BE = 5;
                globalCtx->envCtx.unk_BD = 0;
                Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.015f);
                Math_ApproachF(&thisv->actor.scale.x, 0.00024999998f, 0.1f, 0.00005f);
                thisv->actor.shape.rot.y += (s16)thisv->actor.speedXZ;
                thisv->workf[UNK_F13] += thisv->actor.speedXZ;
                if (thisv->workf[UNK_F13] > 65536.0f) {
                    thisv->workf[UNK_F13] -= 65536.0f;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_ROLL);
                }
                Math_ApproachF(&thisv->actor.speedXZ, 12288.0f, 1.0f, 256.0f);
                if (thisv->work[CS_TIMER_1] == 135) {
                    Vec3f spBC;
                    Vec3f spB0;
                    Vec3f spA4 = { 0.0f, 0.0f, 0.0f };
                    func_80078884(NA_SE_EN_TWINROBA_TRANSFORM);
                    for (i = 0; i < 100; i++) {
                        spB0.x = Rand_CenteredFloat(5.0f);
                        spB0.y = Rand_CenteredFloat(5.0f);
                        spB0.z = Rand_CenteredFloat(5.0f);
                        spBC = thisv->actor.world.pos;
                        spBC.x += spB0.x;
                        spBC.y += spB0.y;
                        spBC.z += spB0.z;
                        BossTw_AddFlameEffect(globalCtx, &spBC, &spB0, &spA4, Rand_ZeroFloat(2.0f) + 5,
                                              Rand_ZeroFloat(1.99f));
                    }
                    thisv->csState1 = 1;
                    thisv->visible = false;
                    thisv->actor.scale.x = 0.0f;
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                       thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0,
                                       0, TW_DEATHBALL_KOUME);
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW,
                                       thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0,
                                       0, TW_DEATHBALL_KOTAKE);
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                }
            }
            Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
            break;
        case 1:
            break;
    }

    switch (thisv->csState2) {
        case 0:
            thisv->csState2 = 1;
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 8);
            thisv->subCamId = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, 0, CAM_STAT_WAIT);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->subCamId, CAM_STAT_ACTIVE);
            thisv->subCamEye = mainCam->eye;
            thisv->subCamAt = mainCam->at;
            Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
            break;
        case 1:
            spD0.x = Math_SinS(thisv->actor.world.rot.y) * 200.0f;
            spD0.z = Math_CosS(thisv->actor.world.rot.y) * 200.0f;
            Math_ApproachF(&thisv->subCamEye.x, spD0.x + thisv->actor.world.pos.x, 0.1f, 50.0f);
            Math_ApproachF(&thisv->subCamEye.y, 300.0f, 0.1f, 50.0f);
            Math_ApproachF(&thisv->subCamEye.z, spD0.z + thisv->actor.world.pos.z, 0.1f, 50.0f);
            Math_ApproachF(&thisv->subCamAt.x, thisv->actor.world.pos.x, 0.1f, 50.0f);
            Math_ApproachF(&thisv->subCamAt.y, thisv->actor.world.pos.y, 0.1f, 50.0f);
            Math_ApproachF(&thisv->subCamAt.z, thisv->actor.world.pos.z, 0.1f, 50.0f);
            if (thisv->work[CS_TIMER_1] == 170) {
                thisv->csState2 = 2;
                thisv->work[CS_TIMER_2] = 0;
                thisv->subCamEye.z = 170.0f;
                thisv->subCamDist = 170.0f;
                thisv->subCamEye.x = 0.0f;
                thisv->subCamAt.x = 0.0f;
                thisv->subCamAt.z = 0.0f;
                thisv->subCamEye.y = 260.0f;
                player->actor.shape.rot.y = -0x8000;
                player->actor.world.pos.x = -40.0f;
                player->actor.world.pos.y = 240.0f;
                player->actor.world.pos.z = 90.0f;
                sKoumePtr->actor.world.pos.x = -37.0f;
                sKotakePtr->actor.world.pos.x = 37.0f;
                sKotakePtr->actor.world.pos.y = 263.0f;
                sKoumePtr->actor.world.pos.y = sKotakePtr->actor.world.pos.y;
                thisv->subCamAt.y = sKoumePtr->actor.world.pos.y + 17.0f;
                sKotakePtr->actor.world.pos.z = 0.0f;
                sKoumePtr->actor.world.pos.z = sKotakePtr->actor.world.pos.z;
                sKoumePtr->work[YAW_TGT] = sKotakePtr->work[YAW_TGT] = sKoumePtr->actor.shape.rot.x =
                    sKotakePtr->actor.shape.rot.x = sKoumePtr->actor.shape.rot.y = sKotakePtr->actor.shape.rot.y = 0;
                func_8002DF54(globalCtx, &sKoumePtr->actor, 1);
                sKoumePtr->actor.flags |= ACTOR_FLAG_0;
            }
            break;
        case 2:
            if (thisv->work[CS_TIMER_2] == 100) {
                Vec3f pos;
                Vec3f velocity;
                Vec3f accel = { 0.0f, 0.0f, 0.0f };
                s32 zero = 0;

                for (i = 0; i < 50; i++) {
                    velocity.x = Rand_CenteredFloat(3.0f);
                    velocity.y = Rand_CenteredFloat(3.0f);
                    velocity.z = Rand_CenteredFloat(3.0f);
                    pos = sKoumePtr->actor.world.pos;
                    pos.x += velocity.x * 2.0f;
                    pos.y += velocity.y * 2.0f;
                    pos.z += velocity.z * 2.0f;
                    BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &accel, Rand_ZeroFloat(2.0f) + 5, 1);

                    // fake code needed to match, tricks the compiler into allocating more stack
                    if (1) {}
                    if (zero) {
                        accel.x *= 2.0;
                    }

                    velocity.x = Rand_CenteredFloat(3.0f);
                    velocity.y = Rand_CenteredFloat(3.0f);
                    velocity.z = Rand_CenteredFloat(3.0f);
                    pos = sKotakePtr->actor.world.pos;
                    pos.x += velocity.x * 2.0f;
                    pos.y += velocity.y * 2.0f;
                    pos.z += velocity.z * 2.0f;
                    BossTw_AddFlameEffect(globalCtx, &pos, &velocity, &accel, Rand_ZeroFloat(2.0f) + 5, 0);
                }

                Actor_SetScale(&sKoumePtr->actor, 0.0f);
                Actor_SetScale(&sKotakePtr->actor, 0.0f);
                sKoumePtr->visible = 1;
                sKotakePtr->visible = 1;
                func_80078884(NA_SE_EN_TWINROBA_TRANSFORM);
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KOTAKE_KOUME);
                thisv->csState2 = 3;
                thisv->work[CS_TIMER_2] = 0;
                thisv->subCamYaw = thisv->subCamYawStep = thisv->actor.speedXZ = thisv->subCamDistStep = 0.0f;
            }
            break;
        case 3:
            BossTw_DeathCSMsgSfx(thisv, globalCtx);
            if (thisv->work[CS_TIMER_2] < 150) {
                globalCtx->envCtx.unk_BE = 1;
                globalCtx->envCtx.unk_BD = 0;
                Math_ApproachZeroF(&globalCtx->envCtx.unk_D8, 1.0f, 0.1f);
            } else {
                globalCtx->envCtx.unk_BE = 1;
                globalCtx->envCtx.unk_BD = 6;
                Math_ApproachF(&globalCtx->envCtx.unk_D8, (Math_SinS(thisv->work[CS_TIMER_2] * 4096) / 4.0f) + 0.75f,
                               1.0f, 0.1f);
            }

            Math_ApproachF(&thisv->subCamAt.y, sKoumePtr->actor.world.pos.y + 17.0f, 0.05f, 10.0f);

            if (thisv->work[CS_TIMER_2] >= 50) {
                Math_ApproachF(&thisv->subCamDist, 110.0f, 0.05f, thisv->subCamDistStep);
                Math_ApproachF(&thisv->subCamDistStep, 1.0f, 1.0f, 0.025f);
                thisv->subCamEye.x = thisv->subCamDist * sinf(thisv->subCamYaw);
                thisv->subCamEye.z = thisv->subCamDist * cosf(thisv->subCamYaw);
                if (thisv->work[CS_TIMER_2] >= 151) {
                    thisv->subCamYaw += thisv->subCamYawStep;
                    if (thisv->work[CS_TIMER_2] >= 800) {
                        Math_ApproachF(&thisv->subCamYawStep, 0.0f, 1.0f, 0.0001f);
                    } else {
                        Math_ApproachF(&thisv->subCamYawStep, 0.015f, 1.0f, 0.0001f);
                    }
                }
            }
            Math_ApproachF(&sKoumePtr->actor.scale.x, 0.009999999f, 0.1f, 0.001f);
            Actor_SetScale(&sKoumePtr->actor, sKoumePtr->actor.scale.x);
            Actor_SetScale(&sKotakePtr->actor, sKoumePtr->actor.scale.x);
            if (thisv->work[CS_TIMER_2] >= 1020) {
                mainCam = Gameplay_GetCamera(globalCtx, MAIN_CAM);
                mainCam->eye = thisv->subCamEye;
                mainCam->eyeNext = thisv->subCamEye;
                mainCam->at = thisv->subCamAt;
                func_800C08AC(globalCtx, thisv->subCamId, 0);
                thisv->csState2 = 4;
                thisv->subCamId = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, 600.0f, 230.0f,
                                   0.0f, 0, 0, 0, WARP_DUNGEON_ADULT);
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, -600.0f, 230.f, 0.0f, 0, 0, 0, 0);
                thisv->actor.world.pos.y = -2000.0f;
                thisv->workf[UNK_F18] = 0.0f;
                sKoumePtr->visible = sKotakePtr->visible = false;
                if (&thisv->subCamEye) {} // fixes regalloc, may be fake
                Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
            }
            break;
        case 4:
            sEnvType = 0;
            break;
    }

    if (thisv->subCamId) {
        if (1) {}
        Gameplay_CameraSetAtEye(globalCtx, thisv->subCamId, &thisv->subCamAt, &thisv->subCamEye);
    }
}

static s16 D_8094A900[] = {
    0, 1, 2, 2, 1,
};

static s16 D_8094A90C[] = {
    0, 1, 2, 2, 2, 2, 2, 2, 1,
};

void BossTw_Update(Actor* thisx, GlobalContext* globalCtx) {
    BossTw* thisv = (BossTw*)thisx;
    Player* player = GET_PLAYER(globalCtx);
    s16 i;
    s32 pad;

    thisv->collider.base.colType = COLTYPE_HIT3;
    Math_ApproachF(&thisv->fogR, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogG, globalCtx->lightCtx.fogColor[1], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogB, globalCtx->lightCtx.fogColor[2], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogNear, globalCtx->lightCtx.fogNear, 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogFar, 1000.0f, 1.0f, 10.0f);
    thisv->work[CS_TIMER_1]++;
    thisv->work[CS_TIMER_2]++;
    thisv->work[TAIL_IDX]++;

    if (thisv->work[TAIL_IDX] >= ARRAY_COUNT(thisv->blastTailPos)) {
        thisv->work[TAIL_IDX] = 0;
    }

    thisv->blastTailPos[thisv->work[TAIL_IDX]] = thisv->actor.world.pos;

    if (1) {}
    if (1) {}

    for (i = 0; i < 5; i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }

    if (thisv->work[INVINC_TIMER] != 0) {
        thisv->work[INVINC_TIMER]--;
    }

    if (thisv->work[FOG_TIMER] != 0) {
        thisv->work[FOG_TIMER]--;
    }

    if (thisv->actionFunc == BossTw_FlyTo || thisv->actionFunc == BossTw_Spin ||
        thisv->actionFunc == BossTw_TurnToPlayer) {
        if ((s16)(player->actor.shape.rot.y - thisv->actor.yawTowardsPlayer + 0x8000) < 0x1000 &&
            (s16)(player->actor.shape.rot.y - thisv->actor.yawTowardsPlayer + 0x8000) > -0x1000 && player->unk_A73) {
            BossTw_SetupSpin(thisv, globalCtx);
        }
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actionFunc != BossTw_Wait) {
        thisv->collider.dim.radius = 45;

        if (thisv->actionFunc == BossTw_Spin) {
            thisv->collider.dim.radius *= 2;
        }

        thisv->collider.dim.height = 120;
        thisv->collider.dim.yShift = -30;

        if (thisv->work[INVINC_TIMER] == 0) {
            if (thisv->collider.base.acFlags & AC_HIT) {
                thisv->collider.base.acFlags &= ~AC_HIT;
            }

            Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        if (thisv->actor.params == 0) {
            thisv->workf[OUTR_CRWN_TX_X2] += 1.0f;
            thisv->workf[OUTR_CRWN_TX_Y2] -= 7.0f;
            thisv->workf[INNR_CRWN_TX_Y1] += 1.0f;
        } else {
            thisv->workf[OUTR_CRWN_TX_X2] += 0.0f;
            thisv->workf[INNR_CRWN_TX_X2] += 0.0f;
            thisv->workf[OUTR_CRWN_TX_Y2] += -15.0f;
            thisv->workf[INNR_CRWN_TX_Y2] += -10.0f;
        }

        if (((thisv->work[CS_TIMER_2] % 32) == 0) && (Rand_ZeroOne() < 0.3f)) {
            thisv->work[BLINK_IDX] = 4;
        }

        thisv->eyeTexIdx = D_8094A900[thisv->work[BLINK_IDX]];

        if (thisv->work[BLINK_IDX] != 0) {
            thisv->work[BLINK_IDX]--;
        }

        if (thisv->actionFunc != BossTw_MergeCS && thisv->unk_5F8 != 0) {
            Vec3f pos;
            Vec3f velocity = { 0.0f, 0.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.0f, 0.0f };

            if (thisv->scepterAlpha > 0.0f) {
                for (i = 0; i <= 0; i++) {
                    pos = thisv->scepterFlamePos[0];
                    pos.x += Rand_CenteredFloat(70.0f);
                    pos.y += Rand_CenteredFloat(70.0f);
                    pos.z += Rand_CenteredFloat(70.0f);
                    accel.y = 0.4f;
                    accel.x = Rand_CenteredFloat(0.5f);
                    accel.z = Rand_CenteredFloat(0.5f);
                    BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 8,
                                        thisv->actor.params, 37);
                }
            }

            for (i = 0; i <= 0; i++) {
                pos = thisv->crownPos;
                pos.x += Rand_CenteredFloat(70.0f);
                pos.y += Rand_CenteredFloat(70.0f);
                pos.z += Rand_CenteredFloat(70.0f);
                accel.y = 0.4f;
                accel.x = Rand_CenteredFloat(0.5f);
                accel.z = Rand_CenteredFloat(0.5f);
                BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 8,
                                    thisv->actor.params, 37);
            }
        }
    }
}

void BossTw_TwinrovaUpdate(Actor* thisx, GlobalContext* globalCtx2) {
    s16 i;
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    thisv->actor.flags &= ~ACTOR_FLAG_10;
    thisv->unk_5F8 = 0;
    thisv->collider.base.colType = COLTYPE_HIT3;

    Math_ApproachF(&thisv->fogR, globalCtx->lightCtx.fogColor[0], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogG, globalCtx->lightCtx.fogColor[1], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogB, globalCtx->lightCtx.fogColor[2], 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogNear, globalCtx->lightCtx.fogNear, 1.0f, 10.0f);
    Math_ApproachF(&thisv->fogFar, 1000.0f, 1.0f, 10.0f);

    thisv->work[CS_TIMER_1]++;
    thisv->work[CS_TIMER_2]++;

    for (i = 0; i < 5; i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }

    if (thisv->work[INVINC_TIMER] != 0) {
        thisv->work[INVINC_TIMER]--;
    }

    if (thisv->work[FOG_TIMER] != 0) {
        thisv->work[FOG_TIMER]--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actionFunc != BossTw_TwinrovaShootBlast && thisv->actionFunc != BossTw_TwinrovaChargeBlast &&
        thisv->visible && thisv->unk_5F8 == 0 &&
        (s16)(player->actor.shape.rot.y - thisv->actor.yawTowardsPlayer + 0x8000) < 0x1000 &&
        (s16)(player->actor.shape.rot.y - thisv->actor.yawTowardsPlayer + 0x8000) > -0x1000 && player->unk_A73 != 0) {
        BossTw_TwinrovaSetupSpin(thisv, globalCtx);
    }

    thisv->eyeTexIdx = D_8094A900[thisv->work[BLINK_IDX]];
    if (thisv->work[BLINK_IDX] != 0) {
        thisv->work[BLINK_IDX]--;
    }

    if ((thisv->work[CS_TIMER_2] % 32) == 0) {
        if (thisv->actionFunc != BossTw_TwinrovaMergeCS) {
            if (Rand_ZeroOne() < 0.3f) {
                thisv->work[BLINK_IDX] = 4;
            }
        }
    }

    if (thisv->actionFunc == BossTw_TwinrovaMergeCS) {
        thisv->leftEyeTexIdx = D_8094A90C[thisv->work[TW_BLINK_IDX]];
        if (thisv->work[TW_BLINK_IDX] != 0) {
            thisv->work[TW_BLINK_IDX]--;
        }
    } else {
        if (thisv->actionFunc == BossTw_TwinrovaStun) {
            thisv->eyeTexIdx = 1;
        }

        if (thisv->actionFunc == BossTw_TwinrovaDeathCS) {
            thisv->eyeTexIdx = 2;
        }

        thisv->leftEyeTexIdx = thisv->eyeTexIdx;
    }

    if (thisv->visible && thisv->unk_5F8 == 0) {
        Vec3f pos;
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel;

        if (thisv->work[UNK_S8] != 0) {
            thisv->work[UNK_S8] -= 20;
            if (thisv->work[UNK_S8] < 0) {
                thisv->work[UNK_S8] = 0;
            }
        }

        Math_ApproachF(&thisv->workf[UNK_F12], 1.0f, 1.0f, 0.05f);
        accel.y = 0.4f;

        for (i = 0; i < 2; i++) {
            pos = thisv->leftScepterPos;
            pos.x += Rand_CenteredFloat(30.0f);
            pos.y += Rand_CenteredFloat(30.0f);
            pos.z += Rand_CenteredFloat(30.0f);
            accel.x = Rand_CenteredFloat(0.5f);
            accel.z = Rand_CenteredFloat(0.5f);
            BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 7, 0, 75);
        }

        for (i = 0; i < 2; i++) {
            pos = thisv->rightScepterPos;
            pos.x += Rand_CenteredFloat(30.0f);
            pos.y += Rand_CenteredFloat(30.0f);
            pos.z += Rand_CenteredFloat(30.0f);
            accel.x = Rand_CenteredFloat(0.5f);
            accel.z = Rand_CenteredFloat(0.5f);
            BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 7, 1, 75);
        }
    }

    thisv->collider.dim.radius = 35;

    if (thisv->actionFunc == BossTw_TwinrovaSpin) {
        thisv->collider.dim.radius *= 2;
    }

    thisv->collider.dim.height = 150;
    thisv->collider.dim.yShift = -60;
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if (thisv->work[INVINC_TIMER] == 0) {
        if (thisv->actionFunc != BossTw_TwinrovaStun) {
            if (thisv->twinrovaStun != 0) {
                thisv->twinrovaStun = 0;
                thisv->work[FOG_TIMER] = 10;
                BossTw_TwinrovaDamage(thisv, globalCtx, 0);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_DAMAGE);
            } else if (thisv->collider.base.acFlags & AC_HIT) {
                ColliderInfo* info = thisv->collider.info.acHitInfo;

                thisv->collider.base.acFlags &= ~AC_HIT;
                if (info->toucher.dmgFlags & (DMG_SLINGSHOT | DMG_ARROW)) {}
            }
        } else if (thisv->collider.base.acFlags & AC_HIT) {
            u8 damage;
            u8 swordDamage;
            ColliderInfo* info = thisv->collider.info.acHitInfo;

            thisv->collider.base.acFlags &= ~AC_HIT;
            swordDamage = false;
            damage = CollisionCheck_GetSwordDamage(info->toucher.dmgFlags);

            if (damage == 0) {
                damage = 2;
            } else {
                swordDamage = true;
            }

            if (!(info->toucher.dmgFlags & DMG_HOOKSHOT)) {
                if (((s8)thisv->actor.colChkInfo.health < 3) && !swordDamage) {
                    damage = 0;
                }

                BossTw_TwinrovaDamage(thisv, globalCtx, damage);
            }
        }
    }

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    osSyncPrintf("OooooooooooooooooooooooooooooooooCC\n");
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    globalCtx->envCtx.unk_DC = 2;

    switch (sEnvType) {
        case 0:
            Math_ApproachZeroF(&globalCtx->envCtx.unk_D8, 1.0f, 0.02f);
            break;
        case 1:
            globalCtx->envCtx.unk_BD = 3;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, 0.5f, 1.0f, 0.05f);
            break;
        case 2:
            globalCtx->envCtx.unk_BD = 2;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, (Math_SinS(thisv->work[CS_TIMER_1] * 0x3000) * 0.03f) + 0.5f, 1.0f,
                           0.05f);
            break;
        case 3:
            globalCtx->envCtx.unk_BD = 3;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.1f);
            break;
        case 4:
            globalCtx->envCtx.unk_BD = 2;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, (Math_SinS(thisv->work[CS_TIMER_1] * 0x3E00) * 0.05f) + 0.95f,
                           1.0f, 0.1f);
            break;
        case 5:
            globalCtx->envCtx.unk_BD = 0;
            Math_ApproachF(&globalCtx->envCtx.unk_D8, 1.0f, 1.0f, 0.05f);
            break;
        case -1:
            break;
    }

    BossTw_UpdateEffects(globalCtx);

    if (sFreezeState == 1) {
        sFreezeState = 2;
        BossTw_AddPlayerFreezeEffect(globalCtx, NULL);
        func_80078914(&player->actor.projectedPos, NA_SE_VO_LI_FREEZE);
        func_80078914(&player->actor.projectedPos, NA_SE_PL_FREEZE);

        if (sShieldFireCharge != 0) {
            sShieldFireCharge = 4;
        }
    }

    if (player->isBurning && sShieldIceCharge != 0) {
        sShieldIceCharge = 4;
    }
}

s32 BossTw_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossTw* thisv = (BossTw*)thisx;

    if (limbIndex == 21) {
        if (thisv->unk_5F8 == 0) {
            if (thisv->actor.params == 0) {
                *dList = object_tw_DL_012CE0;
            } else {
                *dList = object_tw_DL_0134B8;
            }
        }
    }

    if (limbIndex == 14) {
        if (thisv->actionFunc == BossTw_DeathCS) {
            *dList = NULL;
        } else if (thisv->scepterAlpha == 0.0f) {
            if (thisv->actor.params == 0) {
                *dList = object_tw_DL_012B38;
            } else {
                *dList = object_tw_DL_013310;
            }
        }
    }

    return false;
}

void BossTw_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_8094A944 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_8094A950 = { 0.0f, 2000.0f, -2000.0f };
    static Vec3f D_8094A95C[] = {
        { 0.0f, 0.0f, -10000.0f }, { 0.0f, 0.0f, -8000.0f },  { 0.0f, 0.0f, -9000.0f },
        { 0.0f, 0.0f, -11000.0f }, { 0.0f, 0.0f, -12000.0f },
    };
    BossTw* thisv = (BossTw*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6168);

    switch (limbIndex) {
        case 21:
            Matrix_MultVec3f(&D_8094A944, &thisv->actor.focus.pos);
            Matrix_MultVec3f(&D_8094A950, &thisv->crownPos);

            if (thisv->unk_5F8 != 0) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6190),
                          G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
                if (thisv->actor.params == 0) {
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_013AE8));
                } else {
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_013D68));
                }
            }
            break;
        case 14:
            Matrix_MultVec3f(&D_8094A95C[0], &thisv->scepterFlamePos[0]);
            Matrix_MultVec3f(&D_8094A95C[1], &thisv->scepterFlamePos[1]);
            Matrix_MultVec3f(&D_8094A95C[2], &thisv->scepterFlamePos[2]);
            Matrix_MultVec3f(&D_8094A95C[3], &thisv->scepterFlamePos[3]);
            Matrix_MultVec3f(&D_8094A95C[4], &thisv->scepterFlamePos[4]);

            if (thisv->scepterAlpha > 0.0f) {
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6221),
                          G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
                if (thisv->actor.params == 0) {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 225, 255, (s16)thisv->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_013E98));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)thisv->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_013F98));
                } else {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 20, 0, (s16)thisv->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_014070));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 70, 0, (s16)thisv->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_014158));
                }
            }
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6236);
}

void func_80941BC0(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6341);

    Matrix_Push();
    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_Translate(thisv->groundBlastPos2.x, thisv->groundBlastPos2.y, thisv->groundBlastPos2.z, MTXMODE_NEW);
    Matrix_Scale(thisv->workf[UNK_F12], thisv->workf[UNK_F12], thisv->workf[UNK_F12], MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6358),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)thisv->workf[UNK_F11]);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 40, 30, 80);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01BC00));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 215, 215, 215, (s16)thisv->workf[UNK_F11] * thisv->workf[UNK_F14]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, (u32)thisv->workf[UNK_F16] & 0x3F,
                                (thisv->work[CS_TIMER_2] * 4) & 0x3F, 0x10, 0x10));
    Matrix_Push();
    Matrix_RotateY(thisv->workf[UNK_F15], MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6423),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01C1C0));
    Matrix_Pop();
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6427),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPSegment(POLY_XLU_DISP++, 0xD,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->work[CS_TIMER_2] & 0x7F,
                                (thisv->work[CS_TIMER_2] * 8) & 0xFF, 0x20, 0x40, 1,
                                (-thisv->work[CS_TIMER_2] * 2) & 0x3F, 0, 0x10, 0x10));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)thisv->workf[UNK_F9]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
    gDPSetRenderMode(POLY_XLU_DISP++,
                     Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_DEC | FORCE_BL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA),
                     G_RM_ZB_OVL_SURF2);
    gSPSetGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A790));
    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6461);
}

void func_80942180(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6468);

    Matrix_Push();

    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_Translate(thisv->groundBlastPos2.x, thisv->groundBlastPos2.y, thisv->groundBlastPos2.z, MTXMODE_NEW);
    Matrix_Scale(thisv->workf[KM_GD_CRTR_SCL], thisv->workf[KM_GD_CRTR_SCL], thisv->workf[KM_GD_CRTR_SCL], MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (-thisv->work[CS_TIMER_1]) & 0x7F, 0, 0x20, 0x20, 1,
                                (thisv->work[CS_TIMER_1] * 2) & 0x7F, 0, 0x20, 0x20));
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6497),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 40, 00, (s16)thisv->workf[KM_GRND_CRTR_A]);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 245, 255, 128);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_019D40));

    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6514),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->work[CS_TIMER_1] & 0x7F,
                                (-thisv->work[CS_TIMER_1] * 6) & 0xFF, 0x20, 0x40, 1,
                                (thisv->work[CS_TIMER_1] * 2) & 0x7F, (-thisv->work[CS_TIMER_1] * 6) & 0xFF, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 80, 0, 0, (s16)thisv->workf[KM_GD_SMOKE_A]);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 100);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_018FC0));

    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (-thisv->work[CS_TIMER_1] * 3) & 0x7F, 0, 0x20, 0x20, 1, 0,
                                (-thisv->work[CS_TIMER_1] * 10) & 0xFF, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 50, 0, (s16)(thisv->workf[KM_GD_FLM_A] * 0.7f));
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 235, 240, 128);
    Matrix_Scale(thisv->workf[KM_GD_FLM_SCL], thisv->workf[KM_GD_FLM_SCL], thisv->workf[KM_GD_FLM_SCL], MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6575),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_019938));

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6579);
}

void func_809426F0(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6587);

    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (u8)(-thisv->work[CS_TIMER_2] * 15), 0x20, 0x40, 1, 0, 0,
                                0x40, 0x40));
    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 5000.0f, MTXMODE_APPLY);
    Matrix_Scale(thisv->spawnPortalScale / 2000.0f, thisv->spawnPortalScale / 2000.0f, thisv->spawnPortalScale / 2000.0f,
                 MTXMODE_APPLY);
    Matrix_RotateZ(thisv->portalRotation, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6614),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);

    if (thisv->actor.params == 0) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 135, 175, 165, (s16)thisv->spawnPortalAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01CEE0));
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (s16)thisv->spawnPortalAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01DBE8));
    }

    Matrix_Pop();

    if (thisv->actor.params == 0) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)thisv->flameAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A998));
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s16)thisv->flameAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
    }

    for (i = 0; i < 8; i++) {
        Matrix_Push();
        Matrix_Translate(0.0f, 0.0f, 5000.0f, MTXMODE_APPLY);
        Matrix_RotateZ(((i * std::numbers::pi_v<float>) * 2.0f * 0.125f) + thisv->flameRotation, MTXMODE_APPLY);
        Matrix_Translate(0.0f, thisv->spawnPortalScale * 1.5f, 0.0f, MTXMODE_APPLY);
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, ((thisv->work[CS_TIMER_2] * 3) + (i * 10)) & 0x7F,
                                    (u8)((-thisv->work[CS_TIMER_2] * 15) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
        Matrix_Scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6751),
                  G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A430));
        Matrix_Pop();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6756);
}

void func_80942C70(Actor* thisx, GlobalContext* globalCtx) {
    BossTw* thisv = (BossTw*)thisx;
    s16 alpha;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6765);

    if (thisv->beamDist != 0.0f) {
        Matrix_Push();
        gSPSegment(POLY_XLU_DISP++, 0xC,
                   Gfx_TexScroll(globalCtx->state.gfxCtx, 0, (u8)(thisv->work[CS_TIMER_1] * -0xF), 0x20, 0x40));
        alpha = thisv->beamScale * 100.0f * 255.0f;

        if (thisv->actor.params == 1) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 60, alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 100, 255, 128);
        }

        Matrix_Translate(thisv->beamOrigin.x, thisv->beamOrigin.y, thisv->beamOrigin.z, MTXMODE_NEW);
        Matrix_RotateY(thisv->beamYaw, MTXMODE_APPLY);
        Matrix_RotateX(thisv->beamPitch, MTXMODE_APPLY);
        Matrix_RotateZ(thisv->beamRoll, MTXMODE_APPLY);
        Matrix_Scale(thisv->beamScale, thisv->beamScale, (thisv->beamDist * 0.01f * 98.0f) / 20000.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6846),
                  G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01DDF0));

        if (thisv->beamReflectionDist > 10.0f) {
            Matrix_Translate(thisv->beamReflectionOrigin.x, thisv->beamReflectionOrigin.y, thisv->beamReflectionOrigin.z,
                             MTXMODE_NEW);
            Matrix_RotateY(thisv->beamReflectionYaw, MTXMODE_APPLY);
            Matrix_RotateX(thisv->beamReflectionPitch, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->beamRoll, MTXMODE_APPLY);
            Matrix_Scale(thisv->beamScale, thisv->beamScale, (thisv->beamReflectionDist * 0.01f * 100.0f) / 20000.0f,
                         MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6870),
                      G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01DDF0));
        }

        Matrix_Pop();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6878);
}

void func_80943028(Actor* thisx, GlobalContext* globalCtx) {
    BossTw* thisv = (BossTw*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6885);

    Matrix_Push();
    Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y + 57.0f, thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisv->workf[UNK_F17], thisv->workf[UNK_F17], thisv->workf[UNK_F17], MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6908),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01F608));
    func_80094044(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 200);
    Matrix_Translate(thisv->actor.world.pos.x, 240.0f, thisv->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale((thisv->actor.scale.x * 4000.0f) / 100.0f, 1.0f, (thisv->actor.scale.x * 4000.0f) / 100.0f,
                 MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6926),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gCircleShadowDL));
    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6933);
}

static const void* sEyeTextures[] = {
    object_tw_Tex_00A438,
    object_tw_Tex_00B238,
    object_tw_Tex_00B638,
};

void BossTw_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    static Vec3f D_8094A9A4 = { 0.0f, 200.0f, 2000.0f };
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 6947);

    if (thisv->visible) {
        gSPSegment(POLY_OPA_DISP++, 10, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeTexIdx]));
        gSPSegment(POLY_XLU_DISP++, 10, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeTexIdx]));
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (s16)thisv->workf[OUTR_CRWN_TX_X1] & 0x7F,
                                    (s16)thisv->workf[OUTR_CRWN_TX_Y1] & 0x7F, 0x20, 0x20, 1,
                                    (s16)thisv->workf[OUTR_CRWN_TX_X2] & 0x7F, (s16)thisv->workf[OUTR_CRWN_TX_Y2] & 0xFF,
                                    0x20, 0x40));

        if (thisv->actor.params == TW_KOTAKE) {
            gSPSegment(POLY_XLU_DISP++, 9,
                       Gfx_TexScroll(globalCtx->state.gfxCtx, (s16)thisv->workf[INNR_CRWN_TX_X1] & 0x7F,
                                     (s16)thisv->workf[INNR_CRWN_TX_Y1] & 0xFF, 0x20, 0x40));
        } else {
            gSPSegment(POLY_XLU_DISP++, 9,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (s16)thisv->workf[INNR_CRWN_TX_X1] & 0x7F,
                                        (s16)thisv->workf[INNR_CRWN_TX_Y1] & 0x7F, 0x20, 0x20, 1,
                                        (s16)thisv->workf[INNR_CRWN_TX_X2] & 0x7F,
                                        (s16)thisv->workf[INNR_CRWN_TX_Y2] & 0xFF, 0x20, 0x40));
        }

        func_80093D18(globalCtx->state.gfxCtx);
        func_80093D84(globalCtx->state.gfxCtx);

        if (thisv->work[FOG_TIMER] & 2) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
        } else {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, (u32)thisv->fogR, (u32)thisv->fogG, (u32)thisv->fogB, 0,
                                       thisv->fogNear, thisv->fogFar);
        }

        Matrix_Push();
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, BossTw_OverrideLimbDraw, BossTw_PostLimbDraw, thisv);
        Matrix_Pop();
        POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);
    }

    if (thisv->actor.params == TW_KOTAKE) {
        if (thisv->workf[UNK_F9] > 0.0f) {
            if (thisv->workf[UNK_F11] > 0.0f) {
                Vec3f diff;
                diff.x = thisv->groundBlastPos2.x - player->actor.world.pos.x;
                diff.y = thisv->groundBlastPos2.y - player->actor.world.pos.y;
                diff.z = thisv->groundBlastPos2.z - player->actor.world.pos.z;

                if ((fabsf(diff.y) < 10.0f) && (player->actor.bgCheckFlags & 1) &&
                    (sqrtf(SQ(diff.x) + SQ(diff.z)) < (thisv->workf[UNK_F12] * 4600.0f)) && (sFreezeState == 0) &&
                    (thisv->workf[UNK_F11] > 200.0f)) {
                    sFreezeState = 1;
                    sTwinrovaPtr->timers[2] = 100;
                }
            }

            func_80941BC0(thisv, globalCtx);
        }
    } else {
        func_80942180(thisv, globalCtx);
    }

    if (thisv->visible) {
        if (thisv->actionFunc == BossTw_DeathCS) {
            func_80943028(&thisv->actor, globalCtx);
        } else {
            func_809426F0(thisv, globalCtx);
            Matrix_MultVec3f(&D_8094A9A4, &thisv->beamOrigin);
            func_80942C70(&thisv->actor, globalCtx);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7123);
}

const void* D_8094A9B0[] = {
    object_tw_Tex_02A9B0,
    object_tw_Tex_02A070,
    object_tw_Tex_02A470,
};

s32 BossTw_TwinrovaOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                    void* thisx) {
    BossTw* thisv = (BossTw*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7139);

    switch (limbIndex) {
        case 21:
            gSPSegment(POLY_OPA_DISP++, 0xC,
                       Gfx_TexScroll(globalCtx->state.gfxCtx, 0, (s16)(f32)thisv->work[CS_TIMER_1], 8, 8));
            gSPSegment(POLY_OPA_DISP++, 8, SEGMENTED_TO_VIRTUAL(D_8094A9B0[thisv->eyeTexIdx]));
            gSPSegment(POLY_OPA_DISP++, 9, SEGMENTED_TO_VIRTUAL(D_8094A9B0[thisv->leftEyeTexIdx]));
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, thisv->work[UNK_S8]);
            break;
        case 17:
        case 41:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 0xA,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, 0,
                                        -thisv->work[CS_TIMER_1] * 0xF, 0x20, 0x40));
            break;
        case 18:
        case 42:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 0xB,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, 0,
                                        -thisv->work[CS_TIMER_1] * 0xA, 0x20, 0x40));
            break;
        case 16:
        case 32:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, thisv->work[CS_TIMER_1],
                                        -thisv->work[CS_TIMER_1] * 7, 0x20, 0x40));
            break;
        case 15:
        case 31:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 9,
                       Gfx_TexScroll(globalCtx->state.gfxCtx, 0, thisv->work[CS_TIMER_1], 0x20, 0x40));
            break;
        case 19:
            if (thisv->unk_5F8 != 0) {
                *dList = object_tw_DL_02D940;
            }
            break;

        case 20:
            if (thisv->unk_5F8 != 0) {
                *dList = object_tw_DL_02D890;
            }
            break;
    }

    if (thisv->unk_5F8 != 0 && ((limbIndex == 34) || (limbIndex == 40))) {
        *dList = NULL;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7251);

    return false;
}

void BossTw_TwinrovaPostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_8094A9BC = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_8094A9C8 = { 0.0f, 2000.0f, -2000.0f };
    static Vec3f D_8094A9D4 = { 13000.0f, 0.0f, 0.0f };
    static Vec3f D_8094A9E0 = { 13000.0f, 0.0f, 0.0f };

    BossTw* thisv = (BossTw*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7262);

    switch (limbIndex) {
        case 34:
            Matrix_MultVec3f(&D_8094A9D4, &thisv->leftScepterPos);
            break;
        case 40:
            Matrix_MultVec3f(&D_8094A9E0, &thisv->rightScepterPos);
            break;
        case 21:
            Matrix_MultVec3f(&D_8094A9BC, &thisv->actor.focus.pos);
            Matrix_MultVec3f(&D_8094A9C8, &thisv->crownPos);
            break;
        case 15:
        case 16:
        case 17:
        case 18:
        case 31:
        case 32:
        case 41:
        case 42:
            Matrix_Push();
            Matrix_Scale(thisv->workf[UNK_F12], thisv->workf[UNK_F12], thisv->workf[UNK_F12], MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7295),
                      G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
            Matrix_Pop();
            gSPDisplayList(POLY_XLU_DISP++, *dList);
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7301);
}

void BossTw_ShieldChargeDraw(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s16 temp_t0;
    s16 temp_a0;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7311);

    Matrix_Push();

    temp_t0 = sShieldFireCharge | sShieldIceCharge;

    if (temp_t0 == 1) {
        func_80078884(NA_SE_IT_SHIELD_CHARGE_LV1 & ~SFX_FLAG);
    } else if (temp_t0 == 2) {
        func_80078884(NA_SE_IT_SHIELD_CHARGE_LV2 & ~SFX_FLAG);
    } else if (temp_t0 == 3) {
        func_80078884(NA_SE_IT_SHIELD_CHARGE_LV3 & ~SFX_FLAG);
    }

    if (temp_t0 != 0 && temp_t0 < 4) {
        Math_ApproachF(&D_8094C854, 255.0f, 1.0f, 20.0f);
        if (temp_t0 == 3) {
            temp_t0 *= 3;
        }
    } else if (temp_t0 == 0) {
        D_8094C854 = 0.0f;
    } else {
        Math_ApproachF(&D_8094C854, 0.0f, 1.0f, 10.0f);
        if (D_8094C854 == 0.0f) {
            sShieldIceCharge = 0;
            sShieldFireCharge = 0;
        }

        temp_t0 = 1;
    }

    if (Player_HasMirrorShieldEquipped(globalCtx)) {
        if (temp_t0 != 0) {
            Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
            Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7362),
                      G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
            temp_a0 = (Math_SinS(thisv->work[CS_TIMER_1] * 2730 * temp_t0) * D_8094C854 * 0.5f) + (D_8094C854 * 0.5f);
            if (sShieldFireCharge != 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 245, 255, temp_a0);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01E0E0));
                gSPSegment(POLY_XLU_DISP++, 8,
                           Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (thisv->work[CS_TIMER_1] * 2) * temp_t0, 0, 0x20,
                                            0x20, 1, (-thisv->work[CS_TIMER_1] * 2) * temp_t0, 0, 0x20, 0x20));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 20, 0, (s16)D_8094C854);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01E020));
            } else {
                gDPSetEnvColor(POLY_XLU_DISP++, 225, 255, 255, temp_a0);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01E3A0));
                gSPSegment(POLY_XLU_DISP++, 8,
                           Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (-thisv->work[CS_TIMER_1] * 5) * temp_t0,
                                            0x20, 0x40, 1, (thisv->work[CS_TIMER_1] * 4) * temp_t0, 0, 0x20, 0x20));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 175, 205, 195, (s16)D_8094C854);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01E2C0));
            }
        }
    }

    if (D_8094C86F != 0) {
        f32 step = D_8094C872 > 0 ? 100.0f : 60.0f;

        D_8094C86F--;
        Math_ApproachF(&D_8094C858, 255.0f, 1.0f, step);
    } else {
        f32 step = D_8094C872 > 0 ? 40.0f : 20.0f;

        Math_ApproachF(&D_8094C858, 0.0f, 1.0f, step);
    }

    if (Player_HasMirrorShieldEquipped(globalCtx) && D_8094C858 > 0.0f) {
        f32 scale = D_8094C872 > 0 ? 1.3f : 1.0f;

        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7486),
                  G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
        if (sShieldFireCharge != 0) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 220, 20, (s16)D_8094C858);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 20, 110);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)D_8094C858);
            gDPSetEnvColor(POLY_XLU_DISP++, 185, 225, 205, 150);
        }

        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, thisv->work[CS_TIMER_1] * D_8094C872, 0x20, 0x40, 1,
                                    0, thisv->work[CS_TIMER_1] * D_8094C872, 0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01E9F0));
    }

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7531);
}

void BossTw_SpawnPortalDraw(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7546);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(
        POLY_XLU_DISP++, 8,
        Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, -thisv->work[CS_TIMER_1] * 15, 0x20, 0x40, 1, 0, 0, 0x40, 0x40));

    Matrix_Push();

    Matrix_Translate(0.0f, 232.0f, -600.0f, MTXMODE_NEW);
    Matrix_Scale(thisv->spawnPortalScale, thisv->spawnPortalScale, thisv->spawnPortalScale, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s16)thisv->spawnPortalAlpha);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7582),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01EC68));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 135, 175, 165, (s16)thisv->spawnPortalAlpha);
    Matrix_Translate(0.0f, 2.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7596),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01CEE0));

    Matrix_Translate(0.0f, 232.0f, 600.0f, MTXMODE_NEW);
    Matrix_Scale(thisv->spawnPortalScale, thisv->spawnPortalScale, thisv->spawnPortalScale, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s16)thisv->spawnPortalAlpha);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7617),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01EC68));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (s16)thisv->spawnPortalAlpha);
    Matrix_Translate(0.0f, 2.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7631),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01DBE8));

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7635);
}

void func_80944C50(BossTw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 scale;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7645);

    Matrix_Push();
    Matrix_Translate(0.0f, 750.0f, 0.0f, MTXMODE_NEW);
    Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);
    Matrix_Push();
    Matrix_Scale(thisv->workf[UNK_F19], thisv->workf[UNK_F19], thisv->workf[UNK_F19], MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7671),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01F390));

    Matrix_Pop();
    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -sKoumePtr->work[CS_TIMER_1] * 2, 0, 0x20, 0x20, 1,
                                -sKoumePtr->work[CS_TIMER_1] * 2, 0, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)thisv->workf[UNK_F18] / 2);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7694),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01F238));

    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -sKoumePtr->work[CS_TIMER_1] * 5,
                                -sKoumePtr->work[CS_TIMER_1] * 2, 0x20, 0x40, 1, 0, -sKoumePtr->work[CS_TIMER_1] * 2,
                                0x10, 0x10));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)(thisv->workf[UNK_F18] * 0.3f));

    scale = thisv->workf[UNK_F18] / 150.0f;
    scale = CLAMP_MAX(scale, 1.0f);

    Matrix_Scale(scale, 1.0f, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7728),
              G_MTX_LOAD | G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01EEB0));
    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7732);
}

void BossTw_TwinrovaDraw(Actor* thisx, GlobalContext* globalCtx2) {
    static Vec3f D_8094A9EC = { 0.0f, 200.0f, 2000.0f };
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7748);

    if (thisv->visible) {
        func_80093D18(globalCtx->state.gfxCtx);
        func_80093D84(globalCtx->state.gfxCtx);

        POLY_OPA_DISP = (thisv->work[FOG_TIMER] & 2) ? Gfx_SetFog2(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099)
                                                    : Gfx_SetFog2(POLY_OPA_DISP, (u32)thisv->fogR, (u32)thisv->fogG,
                                                                  (u32)thisv->fogB, 0, thisv->fogNear, thisv->fogFar);

        Matrix_Push();
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, BossTw_TwinrovaOverrideLimbDraw, BossTw_TwinrovaPostLimbDraw,
                              thisx);
        Matrix_Pop();

        Matrix_MultVec3f(&D_8094A9EC, &thisv->beamOrigin);
        POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP, globalCtx->lightCtx.fogColor[0], globalCtx->lightCtx.fogColor[1],
                                    globalCtx->lightCtx.fogColor[2], 0, globalCtx->lightCtx.fogNear, 1000);
    }

    BossTw_DrawEffects(globalCtx);
    BossTw_ShieldChargeDraw(thisv, globalCtx);

    if (thisv->spawnPortalAlpha > 0.0f) {
        BossTw_SpawnPortalDraw(thisv, globalCtx);
    }

    if (thisv->workf[UNK_F18] > 0.0f) {
        func_80944C50(thisv, globalCtx);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 7804);
}

void BossTw_BlastFire(BossTw* thisv, GlobalContext* globalCtx) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 distXZ;
    Player* player = GET_PLAYER(globalCtx);
    Player* player2 = player;

    switch (thisv->actor.params) {
        case TW_FIRE_BLAST:
            switch (thisv->csState1) {
                case 0:
                    Actor_SetScale(&thisv->actor, 0.03f);
                    thisv->csState1 = 1;
                    xDiff = player->actor.world.pos.x - thisv->actor.world.pos.x;
                    yDiff = (player->actor.world.pos.y + 30.0f) - thisv->actor.world.pos.y;
                    zDiff = player->actor.world.pos.z - thisv->actor.world.pos.z;
                    // yaw
                    thisv->actor.world.rot.y = Math_FAtan2F(xDiff, zDiff) * (32768 / std::numbers::pi_v<float>);
                    // pitch
                    distXZ = sqrtf(SQ(xDiff) + SQ(zDiff));
                    thisv->actor.world.rot.x = Math_FAtan2F(yDiff, distXZ) * (32768 / std::numbers::pi_v<float>);
                    thisv->actor.speedXZ = 20.0f;

                    for (i = 0; i < 50; i++) {
                        thisv->blastTailPos[i] = thisv->actor.world.pos;
                    }
                    thisv->workf[TAIL_ALPHA] = 255.0f;
                    // fallthrough
                case 1:
                case 10:
                    thisv->blastActive = true;
                    if (thisv->timers[0] == 0) {
                        func_8002D908(&thisv->actor);
                        func_8002D7EC(&thisv->actor);
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_SHOOT_FIRE & ~SFX_FLAG);
                    } else {
                        Vec3f velocity;
                        Vec3f velDir;
                        Vec3s blastDir;
                        s16 alpha;

                        thisv->actor.world.pos = player2->bodyPartsPos[15];
                        thisv->actor.world.pos.y = -2000.0f;
                        Matrix_MtxFToYXZRotS(&player2->shieldMf, &blastDir, 0);
                        blastDir.x = -blastDir.x;
                        blastDir.y = blastDir.y + 0x8000;
                        Math_ApproachS(&thisv->magicDir.x, blastDir.x, 0xA, 0x800);
                        Math_ApproachS(&thisv->magicDir.y, blastDir.y, 0xA, 0x800);

                        if (thisv->timers[0] == 50) {
                            D_8094C86F = 10;
                            D_8094C872 = 7;
                            globalCtx->envCtx.unk_D8 = 1.0f;
                        }

                        if (thisv->timers[0] <= 50) {
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_SHOOT_FIRE & ~SFX_FLAG);
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_REFL_FIRE & ~SFX_FLAG);
                            Matrix_RotateY((thisv->magicDir.y / 32678.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
                            Matrix_RotateX((thisv->magicDir.x / 32678.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
                            velDir.x = 0.0f;
                            velDir.y = 0.0f;
                            velDir.z = 50.0f;
                            Matrix_MultVec3f(&velDir, &velocity);
                            alpha = thisv->timers[0] * 10;
                            alpha = CLAMP_MAX(alpha, 255);

                            BossTw_AddShieldBlastEffect(globalCtx, &player2->bodyPartsPos[15], &velocity, &sZeroVector,
                                                        10.0f, 80.0f, alpha, 1);
                        }

                        if (thisv->timers[0] == 1) {
                            sEnvType = 0;
                            sShieldFireCharge++;
                            Actor_Kill(&thisv->actor);
                        }

                        return;
                    }

                    thisv->groundBlastPos.y = BossTw_GetFloorY(&thisv->actor.world.pos);

                    if (thisv->groundBlastPos.y >= 0.0f) {
                        if (thisv->groundBlastPos.y != 35.0f) {
                            thisv->groundBlastPos.x = thisv->actor.world.pos.x;
                            thisv->groundBlastPos.z = thisv->actor.world.pos.z;
                            BossTw_SpawnGroundBlast(thisv, globalCtx, 1);
                        } else {
                            Vec3f velocity;
                            Vec3f accel;

                            for (i = 0; i < 50; i++) {
                                velocity.x = Rand_CenteredFloat(20.0f);
                                velocity.y = Rand_CenteredFloat(20.0f);
                                velocity.z = Rand_CenteredFloat(20.0f);
                                accel.x = 0.0f;
                                accel.y = 0.0f;
                                accel.z = 0.0f;
                                BossTw_AddFlameEffect(globalCtx, &thisv->actor.world.pos, &velocity, &accel,
                                                      Rand_ZeroFloat(10.0f) + 25.0f, thisv->blastType);
                            }

                            globalCtx->envCtx.unk_D8 = 0.5f;
                        }

                        thisv->csState1 = 2;
                        thisv->timers[0] = 20;
                    } else {
                        Vec3f pos;
                        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < 10; i++) {
                            pos = thisv->blastTailPos[(s16)Rand_ZeroFloat(29.9f)];
                            pos.x += Rand_CenteredFloat(40.0f);
                            pos.y += Rand_CenteredFloat(40.0f);
                            pos.z += Rand_CenteredFloat(40.0f);
                            accel.y = 0.4f;
                            accel.x = Rand_CenteredFloat(0.5f);
                            accel.z = Rand_CenteredFloat(0.5f);
                            BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 8, 1,
                                                75);
                        }
                    }
                    break;
                case 2:
                    Math_ApproachF(&thisv->workf[TAIL_ALPHA], 0.0f, 1.0f, 15.0f);
                    if (thisv->timers[0] == 0) {
                        Actor_Kill(&thisv->actor);
                    }
                    break;
            }
            break;

        case TW_FIRE_BLAST_GROUND:
            if (thisv->timers[0] != 0) {
                if (thisv->timers[0] == 1) {
                    sEnvType = 0;
                }

                if (sGroundBlastType == 2) {
                    thisv->timers[0] = 0;
                }

                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_FIRE_EXP - SFX_FLAG);

                xDiff = sKoumePtr->groundBlastPos2.x - player->actor.world.pos.x;
                yDiff = sKoumePtr->groundBlastPos2.y - player->actor.world.pos.y;
                zDiff = sKoumePtr->groundBlastPos2.z - player->actor.world.pos.z;

                if (!player->isBurning && (player->actor.bgCheckFlags & 1) && (fabsf(yDiff) < 10.0f) &&
                    (sqrtf(SQ(xDiff) + SQ(zDiff)) < (sKoumePtr->workf[UNK_F13] * 4550.0f))) {
                    s16 j;

                    for (j = 0; j < 18; j++) {
                        player->flameTimers[j] = Rand_S16Offset(0, 200);
                    }

                    player->isBurning = 1;

                    if (thisv->work[BURN_TMR] == 0) {
                        func_8002F7DC(&player->actor, player->ageProperties->unk_92 + NA_SE_VO_LI_DEMO_DAMAGE);
                        thisv->work[BURN_TMR] = 40;
                    }

                    sTwinrovaPtr->timers[2] = 100;
                }

                Math_ApproachF(&sKoumePtr->workf[UNK_F13], 0.04f, 0.1f, 0.002f);
                break;
            }

            {
                f32 sp4C = sGroundBlastType == 2 ? 3.0f : 1.0f;

                Math_ApproachF(&sKoumePtr->workf[UNK_F9], 0.0f, 1.0f, 10.0f * sp4C);
                Math_ApproachF(&sKoumePtr->workf[UNK_F12], 0.0f, 1.0f, 0.03f * sp4C);
                Math_ApproachF(&sKoumePtr->workf[TAIL_ALPHA], 0.0f, 1.0f, 3.0f * sp4C);
                Math_ApproachF(&sKoumePtr->workf[UNK_F11], 0.0f, 1.0f, 6.0f * sp4C);
            }

            if (sKoumePtr->workf[TAIL_ALPHA] <= 0.0f) {
                Actor_Kill(&thisv->actor);
            }

            break;
    }
}

void BossTw_BlastIce(BossTw* thisv, GlobalContext* globalCtx) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 xzDist;
    Player* player = GET_PLAYER(globalCtx);
    Player* player2 = player;

    switch (thisv->actor.params) {
        case TW_ICE_BLAST:
            switch (thisv->csState1) {
                case 0:
                    Actor_SetScale(&thisv->actor, 0.03f);
                    thisv->csState1 = 1;
                    xDiff = player->actor.world.pos.x - thisv->actor.world.pos.x;
                    yDiff = (player->actor.world.pos.y + 30.0f) - thisv->actor.world.pos.y;
                    zDiff = player->actor.world.pos.z - thisv->actor.world.pos.z;
                    thisv->actor.world.rot.y = Math_FAtan2F(xDiff, zDiff) * (32768 / std::numbers::pi_v<float>);
                    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
                    thisv->actor.world.rot.x = Math_FAtan2F(yDiff, xzDist) * (32768 / std::numbers::pi_v<float>);
                    thisv->actor.speedXZ = 20.0f;
                    for (i = 0; i < 50; i++) {
                        thisv->blastTailPos[i] = thisv->actor.world.pos;
                    }

                    thisv->workf[TAIL_ALPHA] = 255.0f;
                    // fallthrough
                case 1:
                case 10:
                    thisv->blastActive = true;

                    if (thisv->timers[0] == 0) {
                        func_8002D908(&thisv->actor);
                        func_8002D7EC(&thisv->actor);
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG);
                    } else {
                        Vec3f velocity;
                        Vec3f spF4;
                        Vec3s reflDir;
                        s16 alpha;

                        thisv->actor.world.pos = player2->bodyPartsPos[15];
                        thisv->actor.world.pos.y = -2000.0f;
                        Matrix_MtxFToYXZRotS(&player2->shieldMf, &reflDir, 0);
                        reflDir.x = -reflDir.x;
                        reflDir.y += 0x8000;
                        Math_ApproachS(&thisv->magicDir.x, reflDir.x, 0xA, 0x800);
                        Math_ApproachS(&thisv->magicDir.y, reflDir.y, 0xA, 0x800);

                        if (thisv->timers[0] == 50) {
                            D_8094C86F = 10;
                            D_8094C872 = 7;
                            globalCtx->envCtx.unk_D8 = 1.0f;
                        }

                        if (thisv->timers[0] <= 50) {
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG);
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_REFL_FREEZE - SFX_FLAG);
                            Matrix_RotateY((thisv->magicDir.y / 32678.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
                            Matrix_RotateX((thisv->magicDir.x / 32678.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
                            spF4.x = 0.0f;
                            spF4.y = 0.0f;
                            spF4.z = 50.0f;
                            Matrix_MultVec3f(&spF4, &velocity);
                            alpha = thisv->timers[0] * 10;
                            alpha = CLAMP_MAX(alpha, 255);

                            BossTw_AddShieldBlastEffect(globalCtx, &player2->bodyPartsPos[15], &velocity, &sZeroVector,
                                                        10.0f, 80.0f, alpha, 0);
                        }

                        if (thisv->timers[0] == 1) {
                            sEnvType = 0;
                            sShieldIceCharge++;
                            Actor_Kill(&thisv->actor);
                        }

                        break;
                    }

                    thisv->groundBlastPos.y = BossTw_GetFloorY(&thisv->actor.world.pos);

                    if (thisv->groundBlastPos.y >= 0.0f) {
                        if (thisv->groundBlastPos.y != 35.0f) {
                            thisv->groundBlastPos.x = thisv->actor.world.pos.x;
                            thisv->groundBlastPos.z = thisv->actor.world.pos.z;
                            BossTw_SpawnGroundBlast(thisv, globalCtx, 0);
                        } else {
                            for (i = 0; i < 50; i++) {
                                Vec3f velocity;
                                Vec3f accel;

                                velocity.x = Rand_CenteredFloat(20.0f);
                                velocity.y = Rand_CenteredFloat(20.0f);
                                velocity.z = Rand_CenteredFloat(20.0f);
                                accel.x = 0.0f;
                                accel.y = 0.0f;
                                accel.z = 0.0f;
                                BossTw_AddFlameEffect(globalCtx, &thisv->actor.world.pos, &velocity, &accel,
                                                      Rand_ZeroFloat(10.0f) + 25.0f, thisv->blastType);
                            }

                            globalCtx->envCtx.unk_D8 = 0.5f;
                        }

                        thisv->csState1 = 2;
                        thisv->timers[0] = 20;
                    } else {
                        Vec3f pos;
                        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < 10; i++) {
                            pos = thisv->blastTailPos[(s16)Rand_ZeroFloat(29.9f)];
                            pos.x += Rand_CenteredFloat(40.0f);
                            pos.y += Rand_CenteredFloat(40.0f);
                            pos.z += Rand_CenteredFloat(40.0f);
                            accel.y = 0.4f;
                            accel.x = Rand_CenteredFloat(0.5f);
                            accel.z = Rand_CenteredFloat(0.5f);
                            BossTw_AddDotEffect(globalCtx, &pos, &velocity, &accel, ((s16)Rand_ZeroFloat(2.0f) + 8), 0,
                                                75);
                        }
                    }
                    break;

                case 2:
                    Math_ApproachF(&thisv->workf[TAIL_ALPHA], 0.0f, 1.0f, 15.0f);
                    if (thisv->timers[0] == 0) {
                        Actor_Kill(&thisv->actor);
                    }
                    break;
            }
            break;

        case TW_ICE_BLAST_GROUND:
            if (thisv->timers[0] != 0) {
                if (thisv->timers[0] == 1) {
                    sEnvType = 0;
                }

                if (sGroundBlastType == 1) {
                    thisv->timers[0] = 0;
                }

                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);

                if (thisv->timers[0] > (sTwinrovaPtr->actionFunc == BossTw_Wait ? 70 : 20)) {
                    s32 pad;
                    Vec3f pos;
                    Vec3f velocity;
                    Vec3f accel;

                    pos.x = sKotakePtr->groundBlastPos2.x + Rand_CenteredFloat(320.0f);
                    pos.z = sKotakePtr->groundBlastPos2.z + Rand_CenteredFloat(320.0f);
                    pos.y = sKotakePtr->groundBlastPos2.y;
                    velocity.x = 0.0f;
                    velocity.y = 0.0f;
                    velocity.z = 0.0f;
                    accel.x = 0.0f;
                    accel.y = 0.13f;
                    accel.z = 0.0f;
                    BossTw_AddDmgCloud(globalCtx, 3, &pos, &velocity, &accel, Rand_ZeroFloat(5.0f) + 20.0f, 0, 0, 80);
                    velocity.x = Rand_CenteredFloat(10.0f);
                    velocity.z = Rand_CenteredFloat(10.0f);
                    velocity.y = Rand_ZeroFloat(3.0f) + 3.0f;
                    pos.x = sKotakePtr->groundBlastPos2.x + (velocity.x * 0.5f);
                    pos.z = sKotakePtr->groundBlastPos2.z + (velocity.z * 0.5f);
                    BossTw_AddDmgCloud(globalCtx, 3, &pos, &velocity, &accel, Rand_ZeroFloat(5.0f) + 15.0f, 255, 2,
                                       130);
                }

                Math_ApproachF(&sKotakePtr->workf[UNK_F9], 80.0f, 1.0f, 3.0f);
                Math_ApproachF(&sKotakePtr->workf[UNK_F11], 255.0f, 1.0f, 10.0f);
                Math_ApproachF(&sKotakePtr->workf[UNK_F12], 0.04f, 0.1f, 0.002f);
                Math_ApproachF(&sKotakePtr->workf[UNK_F16], 70.0f, 1.0f, 5.0f);

                if ((thisv->timers[0] == 70) || (thisv->timers[0] == 30)) {
                    sKotakePtr->workf[UNK_F16] = 10.0f;
                }

                if ((thisv->timers[0] % 4) == 0) {
                    sKotakePtr->workf[UNK_F15] = (2.0f * (s16)Rand_ZeroFloat(9.9f) * std::numbers::pi_v<float>) / 10.0f;
                }
            } else {
                f32 sp80;

                if (sGroundBlastType == 1) {
                    if (sKotakePtr->workf[UNK_F11] > 1.0f) {
                        for (i = 0; i < 3; i++) {
                            Vec3f pos;
                            Vec3f velocity;
                            Vec3f accel;
                            pos.x = Rand_CenteredFloat(280.0f) + sKotakePtr->groundBlastPos2.x;
                            pos.z = Rand_CenteredFloat(280.0f) + sKotakePtr->groundBlastPos2.z;
                            pos.y = sKotakePtr->groundBlastPos2.y + 30.0f;
                            velocity.x = 0.0f;
                            velocity.y = 0.0f;
                            velocity.z = 0.0f;
                            accel.x = 0.0f;
                            accel.y = 0.13f;
                            accel.z = 0.0f;
                            BossTw_AddDmgCloud(globalCtx, 3, &pos, &velocity, &accel, Rand_ZeroFloat(5.0f) + 20, 0, 0,
                                               80);
                        }
                    }
                    sp80 = 3.0f;
                } else {
                    sp80 = 1.0f;
                }

                Math_ApproachF(&sKotakePtr->workf[UNK_F14], 0.0f, 1.0f, 0.2f * sp80);
                Math_ApproachF(&sKotakePtr->workf[UNK_F11], 0.0f, 1.0f, 5.0f * sp80);
                Math_ApproachF(&sKotakePtr->workf[UNK_F9], 0.0f, 1.0f, sp80);

                if (sKotakePtr->workf[UNK_F9] <= 0.0f) {
                    Actor_Kill(&thisv->actor);
                }
            }
            break;
    }
}

s32 BossTw_BlastShieldCheck(BossTw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = false;
    ColliderInfo* info;

    if (1) {}

    if (thisv->csState1 == 1) {
        if (thisv->collider.base.acFlags & AC_HIT) {
            thisv->collider.base.acFlags &= ~AC_HIT;
            thisv->collider.base.atFlags &= ~AT_HIT;
            info = thisv->collider.info.acHitInfo;

            if (info->toucher.dmgFlags & DMG_SHIELD) {
                thisv->work[INVINC_TIMER] = 7;
                globalCtx->envCtx.unk_D8 = 1.0f;
                func_800AA000(0.0f, 100, 5, 4);

                if (Player_HasMirrorShieldEquipped(globalCtx)) {
                    if (thisv->blastType == 1) {
                        if (sShieldIceCharge != 0) {
                            sShieldIceCharge = 0;
                            BossTw_AddShieldDeflectEffect(globalCtx, 10.0f, 1);
                        } else {
                            BossTw_AddShieldHitEffect(globalCtx, 10.0f, 1);
                            sShieldFireCharge++;
                            D_8094C86F = (sShieldFireCharge * 2) + 8;
                            D_8094C872 = -7;
                        }
                    } else {
                        if (sShieldFireCharge != 0) {
                            sShieldFireCharge = 0;
                            if (1) {}
                            BossTw_AddShieldDeflectEffect(globalCtx, 10.0f, 0);
                        } else {
                            BossTw_AddShieldHitEffect(globalCtx, 10.0f, 0);
                            sShieldIceCharge++;
                            D_8094C86F = (sShieldIceCharge * 2) + 8;
                            D_8094C872 = -7;
                        }
                    }

                    if ((sShieldIceCharge >= 3) || (sShieldFireCharge >= 3)) {
                        thisv->timers[0] = 80;
                        thisv->csState1 = 10;
                        Matrix_MtxFToYXZRotS(&player->shieldMf, &thisv->magicDir, 0);
                        thisv->magicDir.y += 0x8000;
                        thisv->magicDir.x = -thisv->magicDir.x;
                        D_8094C86F = 8;
                    } else {
                        thisv->csState1 = 2;
                        thisv->timers[0] = 20;
                        sEnvType = 0;
                    }
                } else {
                    BossTw_AddShieldDeflectEffect(globalCtx, 10.0f, thisv->blastType);
                    thisv->csState1 = 2;
                    thisv->timers[0] = 20;
                    sEnvType = 0;
                    sShieldIceCharge = 0;
                    sShieldFireCharge = 0;
                    func_80078884(NA_SE_IT_SHIELD_REFLECT_MG2);
                }

                ret = true;
            }
        }
    }

    return ret;
}

void BossTw_BlastUpdate(Actor* thisx, GlobalContext* globalCtx) {
    BossTw* thisv = (BossTw*)thisx;
    ColliderCylinder* collider;
    s16 i;

    thisv->work[CS_TIMER_1]++;
    thisv->work[CS_TIMER_2]++;
    thisv->work[TAIL_IDX]++;

    if (thisv->work[TAIL_IDX] > 29) {
        thisv->work[TAIL_IDX] = 0;
    }

    thisv->blastTailPos[thisv->work[TAIL_IDX]] = thisv->actor.world.pos;

    thisv->actionFunc(thisv, globalCtx);

    for (i = 0; i < 5; i++) {
        if (thisv->timers[i] != 0) {
            thisv->timers[i]--;
        }
    }

    if (thisv->work[INVINC_TIMER] != 0) {
        thisv->work[INVINC_TIMER]--;
    }

    if (thisv->work[BURN_TMR] != 0) {
        thisv->work[BURN_TMR]--;
    }

    thisv->actor.focus.pos = thisv->actor.world.pos;
    collider = &thisv->collider;
    Collider_UpdateCylinder(&thisv->actor, collider);

    if (thisv->blastActive && thisv->work[INVINC_TIMER] == 0 && !BossTw_BlastShieldCheck(thisv, globalCtx)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &collider->base);
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &collider->base);
    }

    thisv->blastActive = false;
}

void BossTw_BlastDraw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;
    f32 scaleFactor;
    s16 tailIdx;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 8818);

    func_80093D84(globalCtx->state.gfxCtx);

    switch (thisv->actor.params) {
        case TW_FIRE_BLAST:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)thisv->workf[TAIL_ALPHA]);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
            for (i = 9; i >= 0; i--) {
                gSPSegment(POLY_XLU_DISP++, 8,
                           Gfx_TwoTexScroll(
                               globalCtx->state.gfxCtx, 0, ((thisv->work[CS_TIMER_1] * 3) + (i * 10)) & 0x7F,
                               ((-thisv->work[CS_TIMER_1] * 15) + (i * 50)) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                tailIdx = ((thisv->work[TAIL_IDX] - i) + 30) % 30;
                Matrix_Translate(thisv->blastTailPos[tailIdx].x, thisv->blastTailPos[tailIdx].y,
                                 thisv->blastTailPos[tailIdx].z, MTXMODE_NEW);
                scaleFactor = 1.0f - (i * 0.09f);
                Matrix_Scale(thisv->actor.scale.x * scaleFactor, thisv->actor.scale.y * scaleFactor,
                             thisv->actor.scale.z * scaleFactor, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 8865),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A430));
            }
            break;

        case TW_FIRE_BLAST_GROUND:
            break;

        case TW_ICE_BLAST:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s8)thisv->workf[TAIL_ALPHA]);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A998));
            for (i = 9; i >= 0; i--) {
                gSPSegment(POLY_XLU_DISP++, 8,
                           Gfx_TwoTexScroll(
                               globalCtx->state.gfxCtx, 0, ((thisv->work[CS_TIMER_1] * 3) + (i * 0xA)) & 0x7F,
                               (u8)((-thisv->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                tailIdx = ((thisv->work[TAIL_IDX] - i) + 30) % 30;
                Matrix_Translate(thisv->blastTailPos[tailIdx].x, thisv->blastTailPos[tailIdx].y,
                                 thisv->blastTailPos[tailIdx].z, MTXMODE_NEW);
                scaleFactor = 1.0f - (i * 0.09f);
                Matrix_Scale(thisv->actor.scale.x * scaleFactor, thisv->actor.scale.y * scaleFactor,
                             thisv->actor.scale.z * scaleFactor, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9004),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AB00));
            }
            break;

        case TW_ICE_BLAST_GROUND:
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9013);
}

void BossTw_DrawDeathBall(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossTw* thisv = (BossTw*)thisx;
    f32 scaleFactor;
    s16 tailIdx;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9028);

    func_80093D84(globalCtx->state.gfxCtx);

    if (thisv->actor.params == TW_DEATHBALL_KOUME) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)thisv->workf[TAIL_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);

        for (i = 9; i >= 0; i--) {
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (((thisv->work[CS_TIMER_1] * 3) + (i * 0xA))) & 0x7F,
                                        (u8)((-thisv->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20,
                                        0x20));
            tailIdx = ((thisv->work[TAIL_IDX] - i) + 30) % 30;
            Matrix_Translate(thisv->blastTailPos[tailIdx].x, thisv->blastTailPos[tailIdx].y,
                             thisv->blastTailPos[tailIdx].z, MTXMODE_NEW);
            scaleFactor = (1.0f - (i * 0.09f));
            Matrix_Scale(thisv->actor.scale.x * scaleFactor, thisv->actor.scale.y * scaleFactor,
                         thisv->actor.scale.z * scaleFactor, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9071),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A430));
        }
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s8)thisv->workf[TAIL_ALPHA]);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A998));

        for (i = 9; i >= 0; i--) {
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (((thisv->work[CS_TIMER_1] * 3) + (i * 0xA))) & 0x7F,
                                        (u8)((-thisv->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20,
                                        0x20));
            tailIdx = ((thisv->work[TAIL_IDX] - i) + 30) % 30;
            Matrix_Translate(thisv->blastTailPos[tailIdx].x, thisv->blastTailPos[tailIdx].y,
                             thisv->blastTailPos[tailIdx].z, MTXMODE_NEW);
            scaleFactor = (1.0f - (i * 0.09f));
            Matrix_Scale(thisv->actor.scale.x * scaleFactor, thisv->actor.scale.y * scaleFactor,
                         thisv->actor.scale.z * scaleFactor, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9107),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AB00));
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_tw.c", 9111);
}

void BossTw_UpdateEffects(GlobalContext* globalCtx) {
    static Color_RGB8 sDotColors[] = {
        { 255, 128, 0 },   { 255, 0, 0 },     { 255, 255, 0 },   { 255, 0, 0 },
        { 100, 100, 100 }, { 255, 255, 255 }, { 150, 150, 150 }, { 255, 255, 255 },
    };
    Vec3f sp11C;
    BossTwEffect* eff = static_cast<BossTwEffect*>(globalCtx->specialEffects);
    Player* player = GET_PLAYER(globalCtx);
    u8 sp113 = 0;
    s16 i;
    s16 j;
    s16 colorIdx;
    Vec3f off;
    Vec3f spF4;
    Vec3f spE8;
    Vec3f spDC;
    Vec3f spD0;
    f32 phi_f22;
    Vec3f spC0;
    Vec3f spB4;
    Vec3f spA8;
    s16 spA6;
    f32 phi_f0;
    Actor* unk44;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (eff->type != 0) {
            eff->pos.x += eff->curSpeed.x;
            eff->pos.y += eff->curSpeed.y;
            eff->pos.z += eff->curSpeed.z;
            eff->frame++;
            eff->curSpeed.x += eff->accel.x;
            eff->curSpeed.y += eff->accel.y;
            eff->curSpeed.z += eff->accel.z;

            if (eff->type == 1) {
                colorIdx = eff->frame % 4;

                if (eff->work[EFF_ARGS] == 0) {
                    colorIdx += 4;
                }

                eff->color.r = sDotColors[colorIdx].r;
                eff->color.g = sDotColors[colorIdx].g;
                eff->color.b = sDotColors[colorIdx].b;
                eff->alpha -= 20;

                if (eff->alpha <= 0) {
                    eff->alpha = 0;
                    eff->type = TWEFF_NONE;
                }

            } else if ((eff->type == 3) || (eff->type == 2)) {
                if (eff->work[EFF_ARGS] == 2) {
                    eff->alpha -= 20;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else if (eff->work[EFF_ARGS] == 0) {
                    eff->alpha += 10;
                    if (eff->alpha >= 100) {
                        eff->work[EFF_ARGS]++;
                    }
                } else {
                    eff->alpha -= 3;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_FLAME) {
                if (eff->work[EFF_UNKS1] != 0) {
                    eff->alpha = (eff->alpha - (i & 7)) - 0xD;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else {
                    eff->alpha += 300;
                    if (eff->alpha >= 255) {
                        eff->alpha = 255;
                        eff->work[EFF_UNKS1]++;
                    }
                }
            } else if (eff->type == TWEFF_SHLD_BLST) {
                D_8094C870 = 1;
                eff->work[EFF_UNKS1]++;
                if (eff->work[EFF_UNKS1] > 30) {
                    eff->alpha -= 10;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }

                Math_ApproachF(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.1f, 0.003f);
                off.x = sTwinrovaPtr->actor.world.pos.x - eff->pos.x;
                off.y = (sTwinrovaPtr->actor.world.pos.y - eff->pos.y) * 0.5f;
                off.z = sTwinrovaPtr->actor.world.pos.z - eff->pos.z;

                if (sTwinrovaPtr->actionFunc != BossTw_TwinrovaStun) {
                    if ((SQ(off.x) + SQ(off.y) + SQ(off.z)) < SQ(60.0f)) {
                        for (j = 0; j < 50; j++) {
                            spF4.x = sTwinrovaPtr->actor.world.pos.x + Rand_CenteredFloat(35.0f);
                            spF4.y = sTwinrovaPtr->actor.world.pos.y + Rand_CenteredFloat(70.0f);
                            spF4.z = sTwinrovaPtr->actor.world.pos.z + Rand_CenteredFloat(35.0f);
                            spE8.x = Rand_CenteredFloat(20.0f);
                            spE8.y = Rand_CenteredFloat(20.0f);
                            spE8.z = Rand_CenteredFloat(20.0f);
                            spDC.x = 0.0f;
                            spDC.y = 0.0f;
                            spDC.z = 0.0f;
                            BossTw_AddFlameEffect(globalCtx, &spF4, &spE8, &spDC, Rand_ZeroFloat(10.0f) + 25.0f,
                                                  eff->work[EFF_ARGS]);
                        }

                        sTwinrovaPtr->twinrovaStun = 1;
                        globalCtx->envCtx.unk_D8 = 1.0f;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_MERGEFLAME) {
                sp11C.x = 0.0f;
                sp11C.y = eff->pos.y;
                sp11C.z = eff->workf[EFF_DIST];
                Matrix_RotateY(sTwinrovaPtr->workf[UNK_F9] + eff->workf[EFF_ROLL], MTXMODE_NEW);
                Matrix_MultVec3f(&sp11C, &eff->pos);

                if (eff->work[EFF_UNKS1] != 0) {
                    eff->alpha -= 60;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else {
                    eff->alpha += 60;
                    if (eff->alpha >= 255) {
                        eff->alpha = 255;
                        eff->work[EFF_UNKS1]++;
                    }
                }
            } else if (eff->type == TWEFF_SHLD_DEFL) {
                eff->work[EFF_UNKS1]++;
                sp11C.x = 0.0f;
                sp11C.y = 0.0f;
                sp11C.z = -eff->workf[EFF_DIST];
                Matrix_RotateY((sShieldHitYaw / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
                Matrix_RotateX(-0.2f, MTXMODE_APPLY);
                Matrix_RotateZ(eff->workf[EFF_ROLL], MTXMODE_APPLY);
                Matrix_RotateY(eff->workf[EFF_YAW], MTXMODE_APPLY);
                Matrix_MultVec3f(&sp11C, &eff->pos);
                eff->pos.x += sShieldHitPos.x;
                eff->pos.y += sShieldHitPos.y;
                eff->pos.z += sShieldHitPos.z;

                if (eff->work[EFF_UNKS1] < 10) {
                    Math_ApproachF(&eff->workf[EFF_DIST], 50.0f, 0.5f, 100.0f);
                } else {
                    Math_ApproachF(&eff->workf[EFF_YAW], 0.0f, 0.5f, 10.0f);
                    Math_ApproachF(&eff->workf[EFF_DIST], 1000.0f, 1.0f, 10.0f);
                    if (eff->work[EFF_UNKS1] >= 0x10) {
                        if ((eff->work[EFF_UNKS1] == 16) && (sp113 == 0)) {
                            sp113 = 1;
                            spD0 = eff->pos;
                            if (eff->pos.y > 40.0f) {
                                spD0.y = 220.0f;
                            } else {
                                spD0.y = -50.0f;
                            }
                            sTwinrovaPtr->groundBlastPos.y = phi_f0 = BossTw_GetFloorY(&spD0);
                            if (phi_f0 >= 0.0f) {
                                if (sTwinrovaPtr->groundBlastPos.y != 35.0f) {
                                    sTwinrovaPtr->groundBlastPos.x = eff->pos.x;
                                    sTwinrovaPtr->groundBlastPos.z = eff->pos.z;
                                    BossTw_SpawnGroundBlast(sTwinrovaPtr, globalCtx, eff->work[EFF_ARGS]);
                                }
                            }
                        }
                        eff->alpha -= 300;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                }

                BossTw_AddFlameEffect(globalCtx, &eff->pos, &sZeroVector, &sZeroVector, 10, eff->work[EFF_ARGS]);
            } else if (eff->type == TWEFF_SHLD_HIT) {
                eff->work[EFF_UNKS1]++;
                sp11C.x = 0.0f;
                sp11C.y = 0.0f;
                sp11C.z = -eff->workf[EFF_DIST];
                Matrix_RotateY((sShieldHitYaw / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_NEW);
                Matrix_RotateX(-0.2f, MTXMODE_APPLY);
                Matrix_RotateZ(eff->workf[EFF_ROLL], MTXMODE_APPLY);
                Matrix_RotateY(eff->workf[EFF_YAW], MTXMODE_APPLY);
                Matrix_MultVec3f(&sp11C, &eff->pos);
                eff->pos.x += sShieldHitPos.x;
                eff->pos.y += sShieldHitPos.y;
                eff->pos.z += sShieldHitPos.z;

                if (eff->work[EFF_UNKS1] < 5) {
                    Math_ApproachF(&eff->workf[EFF_DIST], 40.0f, 0.5f, 100.0f);
                } else {
                    Math_ApproachF(&eff->workf[EFF_DIST], 0.0f, 0.2f, 5.0f);
                    if (eff->work[EFF_UNKS1] >= 11) {
                        eff->alpha -= 30;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                }

                BossTw_AddFlameEffect(globalCtx, &eff->pos, &sZeroVector, &sZeroVector, 10, eff->work[EFF_ARGS]);
            } else if (eff->type == 4) {
                if (eff->work[EFF_UNKS1] == 0) {
                    Math_ApproachF(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.05f, 1.0f);

                    if (eff->frame >= 16) {
                        eff->alpha -= 10;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                } else {
                    Math_ApproachF(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.1f, 2.0f);
                    eff->alpha -= 15;

                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_PLYR_FRZ) {
                if (eff->work[EFF_ARGS] < eff->frame) {
                    phi_f0 = 1.0f;

                    if (eff->target != NULL || sGroundBlastType == 1) {
                        phi_f0 *= 3.0f;
                    }

                    Math_ApproachF(&eff->workf[EFF_SCALE], 0.0f, 1.0f, 0.0005f * phi_f0);

                    if (eff->workf[EFF_SCALE] == 0.0f) {
                        eff->type = TWEFF_NONE;
                        if (eff->target == NULL) {
                            player->stateFlags2 &= ~0x8000;
                            sFreezeState = 0;
                        }
                    }
                } else {
                    if (sGroundBlastType == 1) {
                        eff->frame = 100;
                    }
                    Math_ApproachF(&eff->workf[EFF_DIST], 0.8f, 0.2f, 0.04f);

                    if (eff->target == NULL) {
                        Math_ApproachF(&eff->workf[EFF_SCALE], 0.012f, 1.0f, 0.002f);
                        eff->workf[EFF_ROLL] += eff->workf[EFF_DIST];

                        if (eff->workf[EFF_ROLL] >= 0.8f) {
                            eff->workf[EFF_ROLL] -= 0.8f;
                            player->stateFlags2 |= 0x8000;
                        } else {
                            player->stateFlags2 &= ~0x8000;
                        }

                        if ((sKotakePtr->workf[UNK_F11] > 10.0f) && (sKotakePtr->workf[UNK_F11] < 200.0f)) {
                            eff->frame = 100;
                        }

                        if (!(globalCtx->gameplayFrames & 1)) {
                            globalCtx->damagePlayer(globalCtx, -1);
                        }
                    } else {
                        Math_ApproachF(&eff->workf[EFF_SCALE], 0.042f, 1.0f, 0.002f);
                    }

                    if ((eff->workf[EFF_DIST] > 0.4f) && ((eff->frame & 7) == 0)) {
                        spA6 = Rand_ZeroFloat(17.9f);

                        if (eff->target == NULL) {
                            spC0.x = player->bodyPartsPos[spA6].x + Rand_CenteredFloat(5.0f);
                            spC0.y = player->bodyPartsPos[spA6].y + Rand_CenteredFloat(5.0f);
                            spC0.z = player->bodyPartsPos[spA6].z + Rand_CenteredFloat(5.0f);
                            phi_f22 = 10.0f;
                        } else {
                            unk44 = eff->target;
                            spC0.x = unk44->world.pos.x + Rand_CenteredFloat(40.0f);
                            spC0.y = unk44->world.pos.y + Rand_CenteredFloat(40.0f);
                            spC0.z = unk44->world.pos.z + Rand_CenteredFloat(40.0f);
                            phi_f22 = 20.0f;
                        }

                        spB4.x = 0.0f;
                        spB4.y = 0.0f;
                        spB4.z = 0.0f;
                        spA8.x = 0.0f;
                        spA8.y = 0.1f;
                        spA8.z = 0.0f;

                        BossTw_AddDmgCloud(globalCtx, 3, &spC0, &spB4, &spA8, phi_f22 + Rand_ZeroFloat(phi_f22 * 0.5f),
                                           0, 0, 150);
                    }
                }
            }
        }
        eff++;
    }
}

static s32 sRandSeed0;
static s32 sRandSeed1;
static s32 sRandSeed2;

void BossTw_InitRand(s32 seed0, s32 seed1, s32 seed2) {
    sRandSeed0 = seed0;
    sRandSeed1 = seed1;
    sRandSeed2 = seed2;
}

f32 BossTw_RandZeroOne(void) {
    f32 rand;

    // Wichmann-Hill algorithm
    sRandSeed0 = (sRandSeed0 * 171) % 30269;
    sRandSeed1 = (sRandSeed1 * 172) % 30307;
    sRandSeed2 = (sRandSeed2 * 170) % 30323;

    rand = (sRandSeed0 / 30269.0f) + (sRandSeed1 / 30307.0f) + (sRandSeed2 / 30323.0f);
    while (rand >= 1.0f) {
        rand -= 1.0f;
    }

    return fabsf(rand);
}

void BossTw_DrawEffects(GlobalContext* globalCtx) {
    u8 sp18F = 0;
    s16 i;
    s16 j;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_s4;
    BossTwEffect* currentEffect = static_cast<BossTwEffect*>(globalCtx->specialEffects);
    BossTwEffect* effectHead;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    effectHead = currentEffect;

    OPEN_DISPS(gfxCtx, "../z_boss_tw.c", 9592);

    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (currentEffect->type == 1) {
            if (sp18F == 0) {
                gSPDisplayList(POLY_XLU_DISP++, object_tw_DL_01A528);
                sp18F++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, currentEffect->color.r, currentEffect->color.g,
                            currentEffect->color.b, currentEffect->alpha);
            Matrix_Translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9617),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_tw_DL_01A5A8);
        }

        currentEffect++;
    }

    sp18F = 0;
    currentEffect = effectHead;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (currentEffect->type == 3) {
            if (sp18F == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A998));
                sp18F++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (currentEffect->frame * 3) & 0x7F,
                                        (currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_Translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9660),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AB00));
        }
        currentEffect++;
    }

    sp18F = 0;
    currentEffect = effectHead;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (currentEffect->type == 2) {
            if (sp18F == 0) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
                sp18F++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, currentEffect->alpha);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (currentEffect->frame * 3) & 0x7F,
                                        (currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_Translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9709),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A430));
        }

        currentEffect++;
    }

    sp18F = 0;
    currentEffect = effectHead;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (currentEffect->type == 4) {
            if (sp18F == 0) {
                sp18F++;
            }

            gSPSegment(POLY_XLU_DISP++, 0xD,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, currentEffect->frame & 0x7F,
                                        (currentEffect->frame * 8) & 0xFF, 0x20, 0x40, 1,
                                        (currentEffect->frame * -2) & 0x7F, 0, 0x10, 0x10));

            if (currentEffect->work[EFF_ARGS] == 1) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 65, 0, currentEffect->alpha);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 128);
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
            }

            Matrix_Translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

            if (currentEffect->work[EFF_UNKS1] == 0) {
                Matrix_Translate(0.0f, 0.0f, 60.0f, MTXMODE_APPLY);
            } else {
                Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            }

            Matrix_RotateZ(currentEffect->workf[EFF_ROLL], MTXMODE_APPLY);
            Matrix_RotateX(std::numbers::pi_v<float> / 2.0f, MTXMODE_APPLY);
            Matrix_Scale(currentEffect->workf[EFF_SCALE], 1.0f, currentEffect->workf[EFF_SCALE], MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9775),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
            gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A790));
        }

        currentEffect++;
    }

    sp18F = 0;
    currentEffect = effectHead;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        Actor* actor;
        Vec3f off;

        if (currentEffect->type == TWEFF_PLYR_FRZ) {
            if (sp18F == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AA50));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, 255);
                gSPSegment(POLY_XLU_DISP++, 8,
                           Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                sp18F++;
                BossTw_InitRand(1, 0x71AC, 0x263A);
            }

            actor = currentEffect->target;
            phi_s4 = actor == NULL ? 70 : 20;

            for (j = 0; j < phi_s4; j++) {
                off.x = (BossTw_RandZeroOne() - 0.5f) * 30.0f;
                off.y = currentEffect->workf[EFF_DIST] * j;
                off.z = (BossTw_RandZeroOne() - 0.5f) * 30.0f;

                if (actor != NULL) {
                    Matrix_Translate(actor->world.pos.x + off.x, actor->world.pos.y + off.y, actor->world.pos.z + off.z,
                                     MTXMODE_NEW);
                } else {
                    Matrix_Translate(player->actor.world.pos.x + off.x, player->actor.world.pos.y + off.y,
                                     player->actor.world.pos.z + off.z, MTXMODE_NEW);
                }

                Matrix_Scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE],
                             currentEffect->workf[EFF_SCALE], MTXMODE_APPLY);
                Matrix_RotateY(BossTw_RandZeroOne() * std::numbers::pi_v<float>, MTXMODE_APPLY);
                Matrix_RotateX((BossTw_RandZeroOne() - 0.5f) * std::numbers::pi_v<float> * 0.5f, MTXMODE_APPLY);
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9855),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AB00));
            }
        }

        currentEffect++;
    }

    sp18F = 0;
    currentEffect = effectHead;

    for (i = 0; i < ARRAY_COUNT(sTwEffects); i++) {
        if (currentEffect->type >= 6) {
            if (currentEffect->work[EFF_ARGS] == 0) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A998));
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, currentEffect->alpha);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
            }

            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (currentEffect->frame * 3) & 0x7F,
                                        (-currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_Translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_tw.c", 9911),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            if (currentEffect->work[EFF_ARGS] == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01AB00));
            } else {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_tw_DL_01A430));
            }
        }

        currentEffect++;
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_tw.c", 9920);
}

void BossTw_TwinrovaSetupArriveAtTarget(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaArriveAtTarget;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, -3.0f);
    thisv->work[CS_TIMER_1] = Rand_ZeroFloat(100.0f);
    thisv->timers[1] = 25;
    thisv->rotateSpeed = 0.0f;
}

void BossTw_TwinrovaArriveAtTarget(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.world.pos.x, thisv->targetPos.x, 0.1f, fabsf(thisv->actor.velocity.x) * 1.5f);
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetPos.y, 0.1f, fabsf(thisv->actor.velocity.y) * 1.5f);
    Math_ApproachF(&thisv->targetPos.y, 380.0f, 1.0f, 2.0f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->targetPos.z, 0.1f, fabsf(thisv->actor.velocity.z) * 1.5f);

    if (thisv->timers[1] == 1) {
        BossTw_TwinrovaSetupChargeBlast(thisv, globalCtx);
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, thisv->rotateSpeed);
    Math_ApproachF(&thisv->rotateSpeed, 4096.0f, 1.0f, 350.0f);
}

void BossTw_TwinrovaSetupChargeBlast(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaChargeBlast;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_036FBC, -5.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_036FBC);
    thisv->csState1 = 0;
}

void BossTw_TwinrovaChargeBlast(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    Math_ApproachF(&thisv->actor.world.pos.x, thisv->targetPos.x, 0.03f, fabsf(thisv->actor.velocity.x) * 1.5f);
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetPos.y, 0.03f, fabsf(thisv->actor.velocity.y) * 1.5f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->targetPos.z, 0.03f, fabsf(thisv->actor.velocity.z) * 1.5f);
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 0x1000);

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        if ((s8)thisv->actor.colChkInfo.health < 10) {
            sTwinrovaBlastType = Rand_ZeroFloat(1.99f);
        } else {
            if (++sFixedBlatSeq >= 4) {
                sFixedBlatSeq = 1;
                sFixedBlastType = !sFixedBlastType;
            }

            sTwinrovaBlastType = sFixedBlastType;
        }

        BossTw_TwinrovaSetupShootBlast(thisv, globalCtx);
    }
}

void BossTw_TwinrovaSetupShootBlast(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaShootBlast;

    if (sTwinrovaBlastType == 0) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_022700, 0.0f);
    } else {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_023750, 0.0f);
    }

    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_023750);
}

void BossTw_TwinrovaShootBlast(BossTw* thisv, GlobalContext* globalCtx) {
    BossTw* twMagic;
    Vec3f* magicSpawnPos;
    s32 magicParams;
    s16 i;

    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 8.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_THROW_MASIC);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_SHOOTVC);
    }

    if (Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        if (sTwinrovaBlastType != 0) {
            magicParams = TW_FIRE_BLAST;
            magicSpawnPos = &thisv->rightScepterPos;
        } else {
            magicParams = TW_ICE_BLAST;
            magicSpawnPos = &thisv->leftScepterPos;
        }

        twMagic =
            (BossTw*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_BOSS_TW, magicSpawnPos->x,
                                        magicSpawnPos->y, magicSpawnPos->z, 0, 0, 0, magicParams);

        if (twMagic != NULL) {
            twMagic->blastType = magicParams == TW_ICE_BLAST ? 0 : 1;
        }

        sEnvType = twMagic->blastType + 1;

        {
            Vec3f velocity = { 0.0f, 0.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.0f, 0.0f };

            for (i = 0; i < 100; i++) {
                velocity.x = Rand_CenteredFloat(30.0f);
                velocity.y = Rand_CenteredFloat(30.0f);
                velocity.z = Rand_CenteredFloat(30.0f);
                BossTw_AddDotEffect(globalCtx, magicSpawnPos, &velocity, &accel, (s16)Rand_ZeroFloat(2.0f) + 11,
                                    twMagic->blastType, 75);
            }
        }
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        BossTw_TwinrovaSetupDoneBlastShoot(thisv, globalCtx);
    }

    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 0x1000);
}

void BossTw_TwinrovaSetupDoneBlastShoot(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaDoneBlastShoot;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, -10.0f);
    thisv->timers[1] = 60;
}

void BossTw_TwinrovaDoneBlastShoot(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->timers[1] == 0 && D_8094C870 == 0) {
        if (sTwinrovaPtr->timers[2] == 0) {
            BossTw_TwinrovaSetupFly(thisv, globalCtx);
        } else {
            BossTw_TwinrovaSetupLaugh(thisv, globalCtx);
        }
    }

    D_8094C870 = 0;
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 0x1000);
}

void BossTw_TwinrovaDamage(BossTw* thisv, GlobalContext* globalCtx, u8 damage) {
    if (thisv->actionFunc != BossTw_TwinrovaStun) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_0338F0, -15.0f);
        thisv->timers[0] = 150;
        thisv->timers[1] = 20;
        thisv->csState1 = 0;
        thisv->actor.velocity.y = 0.0f;
    } else {
        thisv->work[FOG_TIMER] = 10;
        thisv->work[INVINC_TIMER] = 20;
        Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_024374, -3.0f);
        thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_024374);
        thisv->csState1 = 1;

        if ((s8)(thisv->actor.colChkInfo.health -= damage) < 0) {
            thisv->actor.colChkInfo.health = 0;
        }

        if ((s8)thisv->actor.colChkInfo.health <= 0) {
            BossTw_TwinrovaSetupDeathCS(thisv, globalCtx);
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_DEAD);
            return;
        }

        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_DAMAGE2);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_CUTBODY);
    }

    thisv->actionFunc = BossTw_TwinrovaStun;
}

void BossTw_TwinrovaStun(BossTw* thisv, GlobalContext* globalCtx) {
    s16 cloudType;

    thisv->unk_5F8 = 1;
    thisv->actor.flags |= ACTOR_FLAG_10;

    cloudType = sTwinrovaBlastType == 0 ? 3 : 2;

    if ((thisv->work[CS_TIMER_1] % 8) == 0) {
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;
        pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(20.0f);
        pos.y = thisv->actor.world.pos.y + Rand_CenteredFloat(40.0f) + 20;
        pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(20.0f);
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;
        accel.x = 0.0f;
        accel.y = 0.1f;
        accel.z = 0.0f;
        BossTw_AddDmgCloud(globalCtx, cloudType, &pos, &velocity, &accel, Rand_ZeroFloat(5.0f) + 10.0f, 0, 0, 150);
    }

    SkelAnime_Update(&thisv->skelAnime);
    thisv->work[UNK_S8] += 20;

    if (thisv->work[UNK_S8] > 255) {
        thisv->work[UNK_S8] = 255;
    }

    Math_ApproachF(&thisv->workf[UNK_F12], 0.0f, 1.0f, 0.05f);
    thisv->actor.world.pos.y += thisv->actor.velocity.y;
    Math_ApproachF(&thisv->actor.velocity.y, -5.0f, 1.0f, 0.5f);
    thisv->actor.world.pos.y -= 30.0f;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 4);
    thisv->actor.world.pos.y += 30.0f;

    if (thisv->csState1 == 0) {
        if (thisv->timers[1] == 0) {
            thisv->csState1 = 1;
            thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_0343B4);
            Animation_Change(&thisv->skelAnime, &object_tw_Anim_0343B4, 1.0f, 0.0f, thisv->workf[ANIM_SW_TGT], 3, 0.0f);
        }
    } else if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        thisv->workf[ANIM_SW_TGT] = 1000.0f;
        Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_035030, 0.0f);
    }

    if (thisv->actor.bgCheckFlags & 1) {
        thisv->actor.velocity.y = 0.0f;
    }

    if (thisv->timers[0] == 0) {
        BossTw_TwinrovaSetupGetUp(thisv, globalCtx);
    }
}

void BossTw_TwinrovaSetupGetUp(BossTw* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_035988, 0.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_035988);
    thisv->actionFunc = BossTw_TwinrovaGetUp;
    thisv->timers[0] = 50;
}

void BossTw_TwinrovaGetUp(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetPos.y, 0.05f, 5.0f);

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        thisv->workf[ANIM_SW_TGT] = 1000.0f;
        Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, 0.0f);
    }

    if (thisv->timers[0] == 0) {
        BossTw_TwinrovaSetupFly(thisv, globalCtx);
    }
}

void BossTw_TwinrovaSetupFly(BossTw* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 zDiff;
    f32 yDiff;
    f32 xzDist;
    Player* player = GET_PLAYER(globalCtx);

    do {
        thisv->work[TW_PLLR_IDX] += (s16)(((s16)Rand_ZeroFloat(2.99f)) + 1);
        thisv->work[TW_PLLR_IDX] %= 4;
        thisv->targetPos = sTwinrovaPillarPos[thisv->work[TW_PLLR_IDX]];
        xDiff = thisv->targetPos.x - player->actor.world.pos.x;
        zDiff = thisv->targetPos.z - player->actor.world.pos.z;
        xzDist = SQ(xDiff) + SQ(zDiff);
    } while (!(xzDist > SQ(300.0f)));

    thisv->targetPos.y = 480.0f;
    xDiff = thisv->targetPos.x - thisv->actor.world.pos.x;
    yDiff = thisv->targetPos.y - thisv->actor.world.pos.y;
    zDiff = thisv->targetPos.z - thisv->actor.world.pos.z;
    thisv->actionFunc = BossTw_TwinrovaFly;
    thisv->rotateSpeed = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = Math_FAtan2F(xDiff, zDiff) * (32768 / std::numbers::pi_v<float>);
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    thisv->actor.world.rot.x = Math_FAtan2F(yDiff, xzDist) * (32768 / std::numbers::pi_v<float>);
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, -10.0f);
}

void BossTw_TwinrovaFly(BossTw* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;
    f32 yaw;
    f32 xzDist;

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
    SkelAnime_Update(&thisv->skelAnime);
    xDiff = thisv->targetPos.x - thisv->actor.world.pos.x;
    yDiff = thisv->targetPos.y - thisv->actor.world.pos.y;
    zDiff = thisv->targetPos.z - thisv->actor.world.pos.z;
    // Convert from radians to degrees, then degrees to binary angle
    yaw = (s16)(Math_FAtan2F(xDiff, zDiff) * ((180.0f / std::numbers::pi_v<float>) * (65536.0f / 360.0f)));
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    Math_ApproachS(&thisv->actor.world.rot.x,
                   (f32)(s16)(Math_FAtan2F(yDiff, xzDist) * ((180.0f / std::numbers::pi_v<float>) * (65536.0f / 360.0f))), 0xA,
                   thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.world.rot.y, yaw, 0xA, thisv->rotateSpeed);
    Math_ApproachS(&thisv->actor.shape.rot.y, yaw, 0xA, thisv->rotateSpeed);
    Math_ApproachF(&thisv->rotateSpeed, 2000.0f, 1.0f, 100.0f);
    Math_ApproachF(&thisv->actor.speedXZ, 30.0f, 1.0f, 2.0f);
    func_8002D908(&thisv->actor);
    Math_ApproachF(&thisv->actor.world.pos.x, thisv->targetPos.x, 0.1f, fabsf(thisv->actor.velocity.x) * 1.5f);
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->targetPos.y, 0.1f, fabsf(thisv->actor.velocity.y) * 1.5f);
    Math_ApproachF(&thisv->targetPos.y, 380.0f, 1.0f, 2.0f);
    Math_ApproachF(&thisv->actor.world.pos.z, thisv->targetPos.z, 0.1f, fabsf(thisv->actor.velocity.z) * 1.5f);

    if (xzDist < 200.0f) {
        BossTw_TwinrovaSetupArriveAtTarget(thisv, globalCtx);
    }
}

void BossTw_TwinrovaSetupSpin(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaSpin;
    Animation_MorphToLoop(&thisv->skelAnime, &object_tw_Anim_032BF8, 0.0f);
    thisv->timers[0] = 20;
    thisv->actor.speedXZ = 0.0f;
}

void BossTw_TwinrovaSpin(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timers[0] != 0) {
        thisv->collider.base.colType = COLTYPE_METAL;
        thisv->actor.shape.rot.y -= 0x3000;

        if ((thisv->timers[0] % 4) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_ROLL);
        }
    } else {
        BossTw_TwinrovaSetupFly(thisv, globalCtx);
    }
}

void BossTw_TwinrovaSetupLaugh(BossTw* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BossTw_TwinrovaLaugh;
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_tw_Anim_03A2D0, 0.0f);
    thisv->workf[ANIM_SW_TGT] = Animation_GetLastFrame(&object_tw_Anim_03A2D0);
    thisv->actor.speedXZ = 0.0f;
}

void BossTw_TwinrovaLaugh(BossTw* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 10.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_YOUNG_LAUGH);
    }

    if (Animation_OnFrame(&thisv->skelAnime, thisv->workf[ANIM_SW_TGT])) {
        BossTw_TwinrovaSetupFly(thisv, globalCtx);
    }
}

void BossTw_Reset(void) {
    sTwInitalized = false;
    memset(sTwEffects, 0, sizeof(sTwEffects));
}