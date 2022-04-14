#include "z_en_go2.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

/*
FLAGS

gSaveContext.eventChkInf[2] & 0x8 - DC entrance boulder blown up as child

InfTable

gSaveContext.infTable[11] & 0x10
gSaveContext.infTable[14] & 0x1 - Talked to DMT Goron at DC entrance (Before DC is opened as child)
gSaveContext.infTable[14] & 0x8 - Talked to GC Goron in bottom level stairwell
gSaveContext.infTable[14] & 0x40 - Talked to GC Goron at LW entrance (Before LW shortcut is opened)
gSaveContext.infTable[14] & 0x800 - Talked to DMT Goron at Bomb Flower with goron bracelet
gSaveContext.infTable[15] & 0x1 - Talked to Goron at GC Entrance (Before goron ruby is obtained)
gSaveContext.infTable[15] & 0x10 - Talked to Goron at GC Island (Before goron ruby is obtained)
gSaveContext.infTable[15] & 0x100 - (not on cloud modding) Talked to GC Goron outside Darunias door (after opening door,
before getting goron bracelet) gSaveContext.infTable[16] & 0x200 - Obtained Fire Tunic from Goron Link
gSaveContext.infTable[16] & 0x400 - (not on cloud modding)
gSaveContext.infTable[16] & 0x800 - Spoke to Goron Link About Volvagia
gSaveContext.infTable[16] & 0x1000 - Stopped Goron Link's Rolling
gSaveContext.infTable[16] & 0x2000 - EnGo Exclusive
gSaveContext.infTable[16] & 0x4000 - Spoke to Goron Link
gSaveContext.infTable[16] & 0x8000 - (not on cloud modding)

gSaveContext.infTable[17] & 0x4000 - Bomb bag upgrade obtained from rolling Goron

EnGo
pathIndex: thisv->actor.params & 0xF
Goron: thisv->actor.params & 0xF0

EnGo2
(thisv->actor.params & 0x3E0) >> 5
(thisv->actor.params & 0xFC00) >> 0xA - Gorons in Fire Temple
thisv->actor.params & 0x1F

Gorons only move when thisv->unk_194.unk_00 == 0
*/

void EnGo2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGo2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGo2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGo2_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnGo2_StopRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_CurledUp(EnGo2* thisv, GlobalContext* globalCtx);

void func_80A46B40(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_GoronDmtBombFlowerAnimation(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_GoronRollingBigContinueRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_ContinueRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_SlowRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_GroundRolling(EnGo2* thisv, GlobalContext* globalCtx);

void EnGo2_ReverseRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_SetupGetItem(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_SetGetItem(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_BiggoronEyedrops(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_GoronLinkStopRolling(EnGo2* thisv, GlobalContext* globalCtx);
void EnGo2_GoronFireGenericAction(EnGo2* thisv, GlobalContext* globalCtx);

static void* sDustTex[] = { gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex };

static Vec3f sPos = { 0.0f, 0.0f, 0.0f };
static Vec3f sVelocity = { 0.0f, 0.0f, 0.0f };
static Vec3f sAccel = { 0.0f, 0.3f, 0.0f };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 40, 65, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

const ActorInit En_Go2_InitVars = {
    ACTOR_EN_GO2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGo2),
    (ActorFunc)EnGo2_Init,
    (ActorFunc)EnGo2_Destroy,
    (ActorFunc)EnGo2_Update,
    (ActorFunc)EnGo2_Draw,
    NULL,
};

static EnGo2DataStruct1 D_80A4816C[14] = {
    { 0, 0, 0, 68, 148 }, { 0, 0, 0, 24, 52 }, { 0, 320, 380, 400, 120 }, { 0, 0, 0, 30, 68 }, { 0, 0, 0, 46, 90 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 },
};

static EnGo2DataStruct2 D_80A481F8[14] = {
    { 30.0f, 0.026f, 6, 60.0f }, { 24.0f, 0.008f, 6, 30.0f }, { 28.0f, 0.16f, 5, 380.0f }, { 28.0f, 0.01f, 7, 40.0f },
    { 30.0f, 0.015f, 6, 30.0f }, { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
};

static f32 D_80A482D8[14][2] = {
    { 80.0f, 80.0f }, { -10.0f, -10.0f }, { 800.0f, 800.0f }, { 0.0f, 0.0f },   { 20.0f, 40.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f }, { 20.0f, 20.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f },
};

typedef enum {
    /*  0 */ ENGO2_ANIM_0,
    /*  1 */ ENGO2_ANIM_1,
    /*  2 */ ENGO2_ANIM_2,
    /*  3 */ ENGO2_ANIM_3,
    /*  4 */ ENGO2_ANIM_4,
    /*  5 */ ENGO2_ANIM_5,
    /*  6 */ ENGO2_ANIM_6,
    /*  7 */ ENGO2_ANIM_7,
    /*  8 */ ENGO2_ANIM_8,
    /*  9 */ ENGO2_ANIM_9,
    /* 10 */ ENGO2_ANIM_10,
    /* 11 */ ENGO2_ANIM_11,
    /* 12 */ ENGO2_ANIM_12
} EnGo2Animation;

static AnimationInfo sAnimationInfo[] = {
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, 0.0f },  { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_0029A8, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_010590, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_003768, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0038E4, 1.0f, 0.0f, -1.0f, 0x02, -8.0f },
    { &gGoronAnim_002D80, 1.0f, 0.0f, -1.0f, 0x02, -8.0f }, { &gGoronAnim_00161C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_001A00, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0021D0, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x01, -8.0f }, { &gGoronAnim_000750, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_000D5C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
};

static EnGo2DustEffectData sDustEffectData[2][4] = {
    {
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
        { 12, 0.1f, 0.2f, 12, 26.0f, 0.0f },
        { 12, 0.1f, 0.3f, 4, 10.0f, 0.0f },
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
    },
    {
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
    },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

void EnGo2_AddDust(EnGo2* thisv, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep) {
    EnGoEffect* dustEffect = thisv->dustEffects;
    s16 i;
    s16 timer;

    for (i = 0; i < ARRAY_COUNT(thisv->dustEffects); i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            if (1) {}
            timer = initialTimer;
            dustEffect->timer = timer;
            dustEffect->type = 1;
            dustEffect->initialTimer = initialTimer;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            return;
        }
    }
}

void EnGo2_UpdateDust(EnGo2* thisv) {
    EnGoEffect* dustEffect = thisv->dustEffects;
    f32 randomNumber;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;
            if (dustEffect->timer == 0) {
                dustEffect->type = 0;
            }
            dustEffect->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            randomNumber = Rand_ZeroOne() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

void EnGo2_DrawDust(EnGo2* thisv, GlobalContext* globalCtx) {
    EnGoEffect* dustEffect = thisv->dustEffects;
    s16 alpha;
    s16 firstDone;
    s16 index;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go2_eff.c", 111);

    firstDone = false;
    func_80093D84(globalCtx->state.gfxCtx);
    if (1) {}

    for (i = 0; i < ARRAY_COUNT(thisv->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD40);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
                firstDone = true;
            }

            alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go2_eff.c", 137),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sDustTex[index]));
            gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD50);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go2_eff.c", 151);
}

s32 EnGo2_SpawnDust(EnGo2* thisv, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius,
                    f32 yAccel) {
    Vec3f pos = sPos;
    Vec3f velocity = sVelocity;
    Vec3f accel = sAccel;
    s32 i;
    s16 angle;

    pos = thisv->actor.world.pos; // overwrites sPos data
    pos.y = thisv->actor.floorHeight;
    angle = (Rand_ZeroOne() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.y += Rand_ZeroOne() * yAccel;
        pos.x = (Math_SinS(angle) * radius) + thisv->actor.world.pos.x;
        pos.z = (Math_CosS(angle) * radius) + thisv->actor.world.pos.z;
        EnGo2_AddDust(thisv, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}

void EnGo2_GetItem(EnGo2* thisv, GlobalContext* globalCtx, s32 getItemId) {
    thisv->getItemId = getItemId;
    func_8002F434(&thisv->actor, globalCtx, getItemId, thisv->actor.xzDistToPlayer + 1.0f,
                  fabsf(thisv->actor.yDistToPlayer) + 1.0f);
}

s32 EnGo2_GetDialogState(EnGo2* thisv, GlobalContext* globalCtx) {
    s16 dialogState = Message_GetState(&globalCtx->msgCtx);

    if ((thisv->dialogState == TEXT_STATE_AWAITING_NEXT) || (thisv->dialogState == TEXT_STATE_EVENT) ||
        (thisv->dialogState == TEXT_STATE_CLOSING) || (thisv->dialogState == TEXT_STATE_DONE_HAS_NEXT)) {
        if (dialogState != thisv->dialogState) {
            thisv->unk_20C++;
        }
    }

    thisv->dialogState = dialogState;
    return dialogState;
}

u16 EnGo2_GoronFireGenericGetTextId(EnGo2* thisv) {
    switch ((thisv->actor.params & 0xFC00) >> 0xA) {
        case 3:
            return 0x3069;
        case 5:
            return 0x306A;
        case 4:
            return 0x306B;
        case 2:
            return 0x306C;
        case 10:
            return 0x306D;
        case 8:
            return 0x306E;
        case 11:
            return 0x306F;
        case 1:
            return 0x3070;
        default:
            return 0x3052;
    }
}

u16 EnGo2_GetTextIdGoronCityRollingBig(GlobalContext* globalCtx, EnGo2* thisv) {
    if (gSaveContext.infTable[17] & 0x4000) {
        return 0x3013;
    } else if (CUR_CAPACITY(UPG_BOMB_BAG) >= 20 && thisv->waypoint > 7 && thisv->waypoint < 12) {
        return 0x3012;
    } else {
        return 0x3011;
    }
}

s16 EnGo2_GetStateGoronCityRollingBig(GlobalContext* globalCtx, EnGo2* thisv) {
    s32 bombBagUpgrade;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return 2;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                if (thisv->actor.textId == 0x3012) {
                    thisv->actionFunc = EnGo2_SetupGetItem;
                    bombBagUpgrade = CUR_CAPACITY(UPG_BOMB_BAG) == 30 ? GI_BOMB_BAG_40 : GI_BOMB_BAG_30;
                    EnGo2_GetItem(thisv, globalCtx, bombBagUpgrade);
                    Message_CloseTextbox(globalCtx);
                    gSaveContext.infTable[17] |= 0x4000;
                    return 2;
                } else {
                    return 2;
                }
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtBombFlower(GlobalContext* globalCtx, EnGo2* thisv) {
    return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3027 : 0x300A;
}

// DMT Goron by Bomb Flower Choice
s16 EnGo2_GetStateGoronDmtBombFlower(GlobalContext* globalCtx, EnGo2* thisv) {
    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_CLOSING:
            if ((thisv->actor.textId == 0x300B) && (gSaveContext.infTable[14] & 0x800) == 0) {
                gSaveContext.infTable[14] |= 0x800;
                return 2;
            } else {
                return 0;
            }
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                // Ask question to DMT Goron by bomb flower
                if (thisv->actor.textId == 0x300A) {
                    if (globalCtx->msgCtx.choiceIndex == 0) {
                        thisv->actor.textId = CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x300B : 0x300C;
                    } else {
                        thisv->actor.textId = 0x300D;
                    }
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                }
                return 1;
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtRollingSmall(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return (gSaveContext.eventChkInf[2] & 0x8) ? 0x3026 : 0x3009;
    }
}

s16 EnGo2_GetStateGoronDmtRollingSmall(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtDcEntrance(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return gSaveContext.eventChkInf[2] & 0x8 ? 0x3021 : gSaveContext.infTable[14] & 0x1 ? 0x302A : 0x3008;
    }
}

s16 EnGo2_GetStateGoronDmtDcEntrance(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x3008) {
            gSaveContext.infTable[14] |= 0x1;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityEntrance(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return gSaveContext.infTable[15] & 0x1 ? 0x3015 : 0x3014;
    }
}

s16 EnGo2_GetStateGoronCityEntrance(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x3014) {
            gSaveContext.infTable[15] |= 0x1;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityIsland(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3067;
    } else {
        return gSaveContext.infTable[15] & 0x10 ? 0x3017 : 0x3016;
    }
}

s16 EnGo2_GetStateGoronCityIsland(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x3016) {
            gSaveContext.infTable[15] |= 0x10;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLowestFloor(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
        return 0x3043;
    } else if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return CUR_UPG_VALUE(UPG_STRENGTH) != 0    ? 0x302C
               : !Flags_GetSwitch(globalCtx, 0x1B) ? 0x3017
               : gSaveContext.infTable[15] & 0x100 ? 0x3019
                                                   : 0x3018;
    }
}

s16 EnGo2_GetStateGoronCityLowestFloor(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x3018) {
            gSaveContext.infTable[15] |= 0x100;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLink(GlobalContext* globalCtx, EnGo2* thisv) {
    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        return gSaveContext.infTable[16] & 0x8000 ? 0x3042 : 0x3041;
    } else if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
        return gSaveContext.infTable[16] & 0x4000 ? 0x3038 : 0x3037;
    } else if (gSaveContext.infTable[16] & 0x1000) {
        thisv->unk_20C = 0;
        thisv->dialogState = TEXT_STATE_NONE;
        return gSaveContext.infTable[16] & 0x400 ? 0x3033 : 0x3032;
    } else {
        return 0x3030;
    }
}

s16 EnGo2_GetStateGoronCityLink(GlobalContext* globalCtx, EnGo2* thisv) {
    switch (EnGo2_GetDialogState(thisv, globalCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisv->actor.textId) {
                case 0x3036:
                    EnGo2_GetItem(thisv, globalCtx, GI_TUNIC_GORON);
                    thisv->actionFunc = EnGo2_SetupGetItem;
                    return 2;
                case 0x3037:
                    gSaveContext.infTable[16] |= 0x4000;
                default:
                    return 0;
            }
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                if (thisv->actor.textId == 0x3034) {
                    if (globalCtx->msgCtx.choiceIndex == 0) {
                        thisv->actor.textId = gSaveContext.infTable[16] & 0x800 ? 0x3033 : 0x3035;
                        if (thisv->actor.textId == 0x3035) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    } else {
                        thisv->actor.textId = gSaveContext.infTable[16] & 0x800 ? 0x3036 : 0x3033;
                        if (thisv->actor.textId == 0x3036) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    }
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                    thisv->unk_20C = 0;
                }
            } else {
                break;
            }
            return 1;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                switch (thisv->actor.textId) {
                    case 0x3035:
                        gSaveContext.infTable[16] |= 0x800;
                    case 0x3032:
                    case 0x3033:
                        thisv->actor.textId = 0x3034;
                        Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                        return 1;
                    default:
                        return 2;
                }
            }
    }
    return 1;
}

u16 EnGo2_GetTextIdGoronDmtBiggoron(GlobalContext* globalCtx, EnGo2* thisv) {
    Player* player = GET_PLAYER(globalCtx);

    if (gSaveContext.bgsFlag) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_CLAIM_CHECK) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) {
        player->exchangeItemId = EXCH_ITEM_EYEDROPS;
        return 0x3058;
    } else {
        player->exchangeItemId = EXCH_ITEM_SWORD_BROKEN;
        return 0x3053;
    }
}

s16 EnGo2_GetStateGoronDmtBiggoron(GlobalContext* globalCtx, EnGo2* thisv) {
    s32 unusedPad;
    u8 dialogState = thisv->dialogState;

    switch (EnGo2_GetDialogState(thisv, globalCtx)) {
        case TEXT_STATE_DONE:
            if (thisv->actor.textId == 0x305E) {
                if (!gSaveContext.bgsFlag) {
                    EnGo2_GetItem(thisv, globalCtx, GI_SWORD_BGS);
                    thisv->actionFunc = EnGo2_SetupGetItem;
                    return 2;
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        case TEXT_STATE_DONE_FADING:
            switch (thisv->actor.textId) {
                case 0x305E:
                    if (func_8002F368(globalCtx) != EXCH_ITEM_CLAIM_CHECK) {
                        break;
                    }
                case 0x3059:
                    if (dialogState == TEXT_STATE_NONE) {
                        func_800F4524(&D_801333D4, NA_SE_EN_GOLON_WAKE_UP, 60);
                    }
                case 0x3054:
                    if (dialogState == TEXT_STATE_NONE) {
                        Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
            }
            return 1;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                if ((thisv->actor.textId == 0x3054) || (thisv->actor.textId == 0x3055)) {
                    if (globalCtx->msgCtx.choiceIndex == 0) {
                        EnGo2_GetItem(thisv, globalCtx, GI_PRESCRIPTION);
                        thisv->actionFunc = EnGo2_SetupGetItem;
                        return 2;
                    }
                    thisv->actor.textId = 0x3056;
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                }
                return 1;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                if (thisv->actor.textId == 0x3059) {
                    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                    thisv->actionFunc = EnGo2_BiggoronEyedrops;
                }
                return 2;
            }
    }
    return 1;
}

u16 EnGo2_GetTextIdGoronFireGeneric(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Flags_GetSwitch(globalCtx, (thisv->actor.params & 0xFC00) >> 0xA)) {
        return 0x3071;
    } else {
        return 0x3051;
    }
}

s16 EnGo2_GetStateGoronFireGeneric(GlobalContext* globalCtx, EnGo2* thisv) {
    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return 0;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                if (thisv->actor.textId == 0x3071) {
                    thisv->actor.textId = EnGo2_GoronFireGenericGetTextId(thisv);
                    Message_ContinueTextbox(globalCtx, thisv->actor.textId);
                }
                return 1;
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityStairwell(GlobalContext* globalCtx, EnGo2* thisv) {
    return !LINK_IS_ADULT ? gSaveContext.infTable[14] & 0x8 ? 0x3022 : 0x300E : 0x3043;
}

s16 EnGo2_GetStateGoronCityStairwell(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x300E) {
            gSaveContext.infTable[14] |= 0x8;
        }
        return 0;
    } else {
        return 1;
    }
}

// Goron in child market bazaar after obtaining Goron Ruby
u16 EnGo2_GetTextIdGoronMarketBazaar(GlobalContext* globalCtx, EnGo2* thisv) {
    return 0x7122;
}

s16 EnGo2_GetStateGoronMarketBazaar(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLostWoods(GlobalContext* globalCtx, EnGo2* thisv) {
    if (!LINK_IS_ADULT) {
        if (Flags_GetSwitch(globalCtx, 0x1C)) {
            return 0x302F;
        } else {
            return gSaveContext.infTable[14] & 0x40 ? 0x3025 : 0x3024;
        }
    } else {
        return 0x3043;
    }
}

s16 EnGo2_GetStateGoronCityLostWoods(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        if (thisv->actor.textId == 0x3024) {
            gSaveContext.infTable[14] |= 0x40;
        }
        return 0;
    } else {
        return 1;
    }
}

// Goron at base of DMT summit
u16 EnGo2_GetTextIdGoronDmtFairyHint(GlobalContext* globalCtx, EnGo2* thisv) {
    if (!LINK_IS_ADULT) {
        return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3065 : 0x3064;
    } else {
        return 0x3043;
    }
}

s16 EnGo2_GetStateGoronDmtFairyHint(GlobalContext* globalCtx, EnGo2* thisv) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextId(GlobalContext* globalCtx, Actor* thisx) {
    EnGo2* thisv = (EnGo2*)thisx;
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 0x20);

    if (faceReaction) {
        return faceReaction;
    } else {
        switch (thisv->actor.params & 0x1F) {
            case GORON_CITY_ROLLING_BIG:
                return EnGo2_GetTextIdGoronCityRollingBig(globalCtx, thisv);
            case GORON_CITY_LINK:
                return EnGo2_GetTextIdGoronCityLink(globalCtx, thisv);
            case GORON_DMT_BIGGORON:
                return EnGo2_GetTextIdGoronDmtBiggoron(globalCtx, thisv);
            case GORON_FIRE_GENERIC:
                return EnGo2_GetTextIdGoronFireGeneric(globalCtx, thisv);
            case GORON_DMT_BOMB_FLOWER:
                return EnGo2_GetTextIdGoronDmtBombFlower(globalCtx, thisv);
            case GORON_DMT_ROLLING_SMALL:
                return EnGo2_GetTextIdGoronDmtRollingSmall(globalCtx, thisv);
            case GORON_DMT_DC_ENTRANCE:
                return EnGo2_GetTextIdGoronDmtDcEntrance(globalCtx, thisv);
            case GORON_CITY_ENTRANCE:
                return EnGo2_GetTextIdGoronCityEntrance(globalCtx, thisv);
            case GORON_CITY_ISLAND:
                return EnGo2_GetTextIdGoronCityIsland(globalCtx, thisv);
            case GORON_CITY_LOWEST_FLOOR:
                return EnGo2_GetTextIdGoronCityLowestFloor(globalCtx, thisv);
            case GORON_CITY_STAIRWELL:
                return EnGo2_GetTextIdGoronCityStairwell(globalCtx, thisv);
            case GORON_CITY_LOST_WOODS:
                return EnGo2_GetTextIdGoronCityLostWoods(globalCtx, thisv);
            case GORON_DMT_FAIRY_HINT:
                return EnGo2_GetTextIdGoronDmtFairyHint(globalCtx, thisv);
            case GORON_MARKET_BAZAAR:
                return EnGo2_GetTextIdGoronMarketBazaar(globalCtx, thisv);
        }
    }
}

s16 EnGo2_GetState(GlobalContext* globalCtx, Actor* thisx) {
    EnGo2* thisv = (EnGo2*)thisx;
    switch (thisv->actor.params & 0x1F) {
        case GORON_CITY_ROLLING_BIG:
            return EnGo2_GetStateGoronCityRollingBig(globalCtx, thisv);
        case GORON_CITY_LINK:
            return EnGo2_GetStateGoronCityLink(globalCtx, thisv);
        case GORON_DMT_BIGGORON:
            return EnGo2_GetStateGoronDmtBiggoron(globalCtx, thisv);
        case GORON_FIRE_GENERIC:
            return EnGo2_GetStateGoronFireGeneric(globalCtx, thisv);
        case GORON_DMT_BOMB_FLOWER:
            return EnGo2_GetStateGoronDmtBombFlower(globalCtx, thisv);
        case GORON_DMT_ROLLING_SMALL:
            return EnGo2_GetStateGoronDmtRollingSmall(globalCtx, thisv);
        case GORON_DMT_DC_ENTRANCE:
            return EnGo2_GetStateGoronDmtDcEntrance(globalCtx, thisv);
        case GORON_CITY_ENTRANCE:
            return EnGo2_GetStateGoronCityEntrance(globalCtx, thisv);
        case GORON_CITY_ISLAND:
            return EnGo2_GetStateGoronCityIsland(globalCtx, thisv);
        case GORON_CITY_LOWEST_FLOOR:
            return EnGo2_GetStateGoronCityLowestFloor(globalCtx, thisv);
        case GORON_CITY_STAIRWELL:
            return EnGo2_GetStateGoronCityStairwell(globalCtx, thisv);
        case GORON_CITY_LOST_WOODS:
            return EnGo2_GetStateGoronCityLostWoods(globalCtx, thisv);
        case GORON_DMT_FAIRY_HINT:
            return EnGo2_GetStateGoronDmtFairyHint(globalCtx, thisv);
        case GORON_MARKET_BAZAAR:
            return EnGo2_GetStateGoronMarketBazaar(globalCtx, thisv);
    }
}

s32 func_80A44790(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params & 0x1F) != GORON_DMT_BIGGORON && (thisv->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG) {
        return func_800343CC(globalCtx, &thisv->actor, &thisv->unk_194.unk_00, thisv->unk_218, EnGo2_GetTextId,
                             EnGo2_GetState);
    } else if (((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) && ((thisv->collider.base.ocFlags2 & 1) == 0)) {
        return false;
    } else {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            thisv->unk_194.unk_00 = 1;
            return true;
        } else if (thisv->unk_194.unk_00 != 0) {
            thisv->unk_194.unk_00 = EnGo2_GetState(globalCtx, &thisv->actor);
            return false;
        } else if (func_8002F2CC(&thisv->actor, globalCtx, thisv->unk_218)) {
            thisv->actor.textId = EnGo2_GetTextId(globalCtx, &thisv->actor);
        }
        return false;
    }
}

void EnGo2_SetColliderDim(EnGo2* thisv) {
    u8 index = thisv->actor.params & 0x1F;

    thisv->collider.dim.radius = D_80A4816C[index].radius;
    thisv->collider.dim.height = D_80A4816C[index].height;
}

void EnGo2_SetShape(EnGo2* thisv) {
    u8 index = thisv->actor.params & 0x1F;

    thisv->actor.shape.shadowScale = D_80A481F8[index].shape_unk_10;
    Actor_SetScale(&thisv->actor, D_80A481F8[index].scale);
    thisv->actor.targetMode = D_80A481F8[index].actor_unk_1F;
    thisv->unk_218 = D_80A481F8[index].unk_218;
    thisv->unk_218 += thisv->collider.dim.radius;
}

void EnGo2_CheckCollision(EnGo2* thisv, GlobalContext* globalCtx) {
    Vec3s pos;
    f32 xzDist;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y;
    pos.z = thisv->actor.world.pos.z;
    xzDist = D_80A4816C[thisv->actor.params & 0x1F].xzDist;
    pos.x += (s16)(xzDist * Math_SinS(thisv->actor.shape.rot.y));
    pos.z += (s16)(xzDist * Math_CosS(thisv->actor.shape.rot.y));
    pos.y += D_80A4816C[thisv->actor.params & 0x1F].yDist;
    thisv->collider.dim.pos = pos;
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnGo2_SwapInitialFrameAnimFrameCount(EnGo2* thisv) {
    f32 initialFrame;

    initialFrame = thisv->skelAnime.startFrame;
    thisv->skelAnime.startFrame = thisv->skelAnime.endFrame;
    thisv->skelAnime.endFrame = initialFrame;
}

s32 func_80A44AB0(EnGo2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 arg2;

    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        return false;
    } else {
        if ((thisv->actionFunc != EnGo2_SlowRolling) && (thisv->actionFunc != EnGo2_ReverseRolling) &&
            (thisv->actionFunc != EnGo2_ContinueRolling)) {
            return false;
        } else {
            if (thisv->collider.base.acFlags & 2) {
                Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                thisv->actor.flags &= ~ACTOR_FLAG_24;
                thisv->collider.base.acFlags &= ~0x2;
                EnGo2_StopRolling(thisv, globalCtx);
                return true;
            }
            if (player->invincibilityTimer <= 0) {
                thisv->collider.base.ocFlags1 |= 8;
            } else {
                return false;
            }
            if (thisv->collider.base.ocFlags2 & 1) {
                thisv->collider.base.ocFlags2 &= ~1;

                arg2 = thisv->actionFunc == EnGo2_ContinueRolling ? 1.5f : thisv->actor.speedXZ * 1.5f;

                globalCtx->damagePlayer(globalCtx, -4);
                func_8002F71C(globalCtx, &thisv->actor, arg2, thisv->actor.yawTowardsPlayer, 6.0f);
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                thisv->collider.base.ocFlags1 &= ~0x8;
            }
        }
    }
    return false;
}

s32 EnGo2_UpdateWaypoint(EnGo2* thisv, GlobalContext* globalCtx) {
    s32 change;

    if (thisv->path == NULL) {
        return 0;
    }

    change = (u8)(thisv->path->count - 1);
    if (thisv->reverse) {
        thisv->waypoint--;
        if (thisv->waypoint < 0) {
            thisv->waypoint = change - 1;
        }
    } else {
        thisv->waypoint++;
        if (thisv->waypoint >= change) {
            thisv->waypoint = 0;
        }
    }

    return 1;
}

s32 EnGo2_Orient(EnGo2* thisv, GlobalContext* globalCtx) {
    s16 targetYaw;
    f32 waypointDistSq = Path_OrientAndGetDistSq(&thisv->actor, thisv->path, thisv->waypoint, &targetYaw);

    Math_SmoothStepToS(&thisv->actor.world.rot.y, targetYaw, 6, 4000, 1);
    if (waypointDistSq > 0.0f && waypointDistSq < SQ(30.0f)) {
        return EnGo2_UpdateWaypoint(thisv, globalCtx);
    } else {
        return 0;
    }
}

s32 func_80A44D84(EnGo2* thisv) {
    s16 targetYaw;

    Path_OrientAndGetDistSq(&thisv->actor, thisv->path, thisv->waypoint, &targetYaw);
    thisv->actor.world.rot.y = targetYaw;
    return 1;
}

s32 EnGo2_IsWakingUp(EnGo2* thisv) {
    s16 yawDiff;
    f32 xyzDist = (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    f32 yDist = (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s16 yawDiffAbs;

    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if ((thisv->collider.base.ocFlags2 & 1) == 0) {
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            return false;
        } else {
            thisv->actor.flags |= ACTOR_FLAG_0;
            return true;
        }
    }

    xyzDist = SQ(xyzDist);
    yawDiff = (f32)thisv->actor.yawTowardsPlayer - (f32)thisv->actor.shape.rot.y;
    yawDiffAbs = ABS(yawDiff);
    if (thisv->actor.xyzDistToPlayerSq <= xyzDist && fabsf(thisv->actor.yDistToPlayer) < yDist && yawDiffAbs < 0x2AA8) {
        return true;
    } else {
        return false;
    }
}

s32 EnGo2_IsRollingOnGround(EnGo2* thisv, s16 arg1, f32 arg2, s16 arg3) {
    if ((thisv->actor.bgCheckFlags & 1) == 0 || thisv->actor.velocity.y > 0.0f) {
        return false;
    }

    if (DECR(thisv->unk_590)) {
        if (!arg3) {
            return true;
        } else {
            thisv->actor.world.pos.y =
                (thisv->unk_590 & 1) ? thisv->actor.world.pos.y + 1.5f : thisv->actor.world.pos.y - 1.5f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
            return true;
        }
    }

    if (thisv->unk_59C >= 2) {
        Audio_PlayActorSound2(&thisv->actor, (thisv->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG
                                                ? NA_SE_EN_GOLON_LAND_BIG
                                                : NA_SE_EN_DODO_M_GND);
    }

    thisv->unk_59C--;
    if (thisv->unk_59C <= 0) {
        if (thisv->unk_59C == 0) {
            thisv->unk_590 = Rand_S16Offset(60, 30);
            thisv->unk_59C = 0;
            thisv->actor.velocity.y = 0.0f;
            return true;
        } else {
            thisv->unk_59C = arg1;
        }
    }

    thisv->actor.velocity.y = ((f32)thisv->unk_59C / (f32)arg1) * arg2;
    return true;
}

void EnGo2_BiggoronSetTextId(EnGo2* thisv, GlobalContext* globalCtx, Player* player) {
    u16 textId;

    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if (gSaveContext.bgsFlag) {
            if (func_8002F368(globalCtx) == EXCH_ITEM_CLAIM_CHECK) {
                thisv->actor.textId = 0x3003;
            } else {
                thisv->actor.textId = 0x305E;
            }
            player->actor.textId = thisv->actor.textId;

        } else if (!gSaveContext.bgsFlag && (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK)) {
            if (func_8002F368(globalCtx) == EXCH_ITEM_CLAIM_CHECK) {
                if (Environment_GetBgsDayCount() >= 3) {
                    textId = 0x305E;
                } else {
                    textId = 0x305D;
                }
                thisv->actor.textId = textId;
            } else {
                if (Environment_GetBgsDayCount() >= 3) {
                    textId = 0x3002;
                } else {
                    textId = 0x305D;
                }
                thisv->actor.textId = textId;
            }
            player->actor.textId = thisv->actor.textId;

        } else if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) &&
                   (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_CLAIM_CHECK)) {
            if (func_8002F368(globalCtx) == EXCH_ITEM_EYEDROPS) {
                thisv->actor.textId = 0x3059;
            } else {
                thisv->actor.textId = 0x3058;
            }
            if (thisv->actor.textId == 0x3059) {
                gSaveContext.timer2State = 0;
            }
            player->actor.textId = thisv->actor.textId;

        } else if (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_SWORD_BROKEN) {
            if (func_8002F368(globalCtx) == EXCH_ITEM_SWORD_BROKEN) {
                if (gSaveContext.infTable[11] & 0x10) {
                    textId = 0x3055;
                } else {
                    textId = 0x3054;
                }
                thisv->actor.textId = textId;
            } else {
                thisv->actor.textId = 0x3053;
            }
            player->actor.textId = thisv->actor.textId;
        } else {
            thisv->actor.textId = 0x3053;
            player->actor.textId = thisv->actor.textId;
        }
    }
}

void func_80A45288(EnGo2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 linkAge;

    if (thisv->actionFunc != EnGo2_GoronFireGenericAction) {
        thisv->unk_194.unk_18 = player->actor.world.pos;
        linkAge = gSaveContext.linkAge;
        thisv->unk_194.unk_14 = D_80A482D8[thisv->actor.params & 0x1F][linkAge];
        func_80034A14(&thisv->actor, &thisv->unk_194, 4, thisv->unk_26E);
    }
    if ((thisv->actionFunc != EnGo2_SetGetItem) && (thisv->isAwake == true)) {
        if (func_80A44790(thisv, globalCtx)) {
            EnGo2_BiggoronSetTextId(thisv, globalCtx, player);
        }
    }
}

void func_80A45360(EnGo2* thisv, f32* alpha) {
    f32 alphaTarget =
        (thisv->skelAnime.animation == &gGoronAnim_004930) && (thisv->skelAnime.curFrame <= 32.0f) ? 0.0f : 255.0f;

    Math_ApproachF(alpha, alphaTarget, 0.4f, 100.0f);
    thisv->actor.shape.shadowAlpha = (u8)(u32)*alpha;
}

void EnGo2_RollForward(EnGo2* thisv) {
    f32 speedXZ = thisv->actor.speedXZ;

    if (thisv->unk_194.unk_00 != 0) {
        thisv->actor.speedXZ = 0.0f;
    }

    if (thisv->actionFunc != EnGo2_ContinueRolling) {
        Actor_MoveForward(&thisv->actor);
    }

    thisv->actor.speedXZ = speedXZ;
}

void func_80A454CC(EnGo2* thisv) {
    switch (thisv->actor.params & 0x1F) {
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_STAIRWELL:
        case GORON_DMT_FAIRY_HINT:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_9);
            break;
        case GORON_DMT_BIGGORON:
            if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN && INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_4);
                break;
            }
        default:
            thisv->skelAnime.playSpeed = 0.0f;
            break;
    }
}

f32 EnGo2_GetTargetXZSpeed(EnGo2* thisv) {
    f32 yDist = (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s32 index = thisv->actor.params & 0x1F;

    if (index == GORON_CITY_LINK && (fabsf(thisv->actor.yDistToPlayer) < yDist) &&
        (thisv->actor.xzDistToPlayer < 400.0f)) {
        return 9.0f;
    } else {
        return index == GORON_CITY_ROLLING_BIG ? 3.6000001f : 6.0f;
    }
}

s32 EnGo2_IsCameraModified(EnGo2* thisv, GlobalContext* globalCtx) {
    Camera* camera = globalCtx->cameraPtrs[MAIN_CAM];

    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if (EnGo2_IsWakingUp(thisv)) {
            Camera_ChangeSetting(camera, CAM_SET_DIRECTED_YAW);
            func_8005AD1C(camera, 4);
        } else if (!EnGo2_IsWakingUp(thisv) && (camera->setting == CAM_SET_DIRECTED_YAW)) {
            Camera_ChangeSetting(camera, CAM_SET_DUNGEON1);
            func_8005ACFC(camera, 4);
        }
    }

    if ((thisv->actor.params & 0x1F) == GORON_FIRE_GENERIC || (thisv->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG ||
        (thisv->actor.params & 0x1F) == GORON_CITY_STAIRWELL || (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON ||
        (thisv->actor.params & 0x1F) == GORON_MARKET_BAZAAR) {
        return true;
    } else if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
        return true;
    } else {
        return false;
    }
}

void EnGo2_DefaultWakingUp(EnGo2* thisv) {
    if (EnGo2_IsWakingUp(thisv)) {
        thisv->unk_26E = 2;
    } else {
        thisv->unk_26E = 1;
    }

    if (thisv->unk_194.unk_00 != 0) {
        thisv->unk_26E = 4;
    }

    thisv->isAwake = true;
}

void EnGo2_WakingUp(EnGo2* thisv) {
    f32 xyzDist = (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    s32 isTrue = true;

    xyzDist = SQ(xyzDist);
    thisv->unk_26E = 1;
    if ((thisv->actor.xyzDistToPlayerSq <= xyzDist) || (thisv->unk_194.unk_00 != 0)) {
        thisv->unk_26E = 4;
    }

    thisv->isAwake = isTrue;
}

void EnGo2_BiggoronWakingUp(EnGo2* thisv) {
    if (EnGo2_IsWakingUp(thisv) || thisv->unk_194.unk_00 != 0) {
        thisv->unk_26E = 2;
        thisv->isAwake = true;
    } else {
        thisv->unk_26E = 1;
        thisv->isAwake = false;
    }
}

void EnGo2_SelectGoronWakingUp(EnGo2* thisv) {
    switch (thisv->actor.params & 0x1F) {
        case GORON_DMT_BOMB_FLOWER:
            thisv->isAwake = true;
            thisv->unk_26E = EnGo2_IsWakingUp(thisv) ? 2 : 1;
            break;
        case GORON_FIRE_GENERIC:
            EnGo2_WakingUp(thisv);
            break;
        case GORON_DMT_BIGGORON:
            EnGo2_BiggoronWakingUp(thisv);
            break;
        case GORON_CITY_LINK:
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                EnGo2_WakingUp(thisv);
                break;
            }
        default:
            EnGo2_DefaultWakingUp(thisv);
    }
}

void EnGo2_EyeMouthTexState(EnGo2* thisv) {
    switch (thisv->eyeMouthTexState) {
        case 1:
            thisv->blinkTimer = 0;
            thisv->eyeTexIndex = 0;
            thisv->mouthTexIndex = 0;
            break;
        case 2:
            thisv->blinkTimer = 0;
            thisv->eyeTexIndex = 1;
            thisv->mouthTexIndex = 0;
            break;
        // case 3 only when biggoron is given eyedrops. Biggoron smiles. (only use of second mouth texture)
        case 3:
            thisv->blinkTimer = 0;
            thisv->eyeTexIndex = 0;
            thisv->mouthTexIndex = 1;
            break;
        default:
            if (DECR(thisv->blinkTimer) == 0) {
                thisv->eyeTexIndex++;
                if (thisv->eyeTexIndex >= 4) {
                    thisv->blinkTimer = Rand_S16Offset(30, 30);
                    thisv->eyeTexIndex = 1;
                }
            }
    }
}

void EnGo2_SitDownAnimation(EnGo2* thisv) {
    if ((thisv->skelAnime.playSpeed != 0.0f) && (thisv->skelAnime.animation == &gGoronAnim_004930)) {
        if (thisv->skelAnime.playSpeed > 0.0f && thisv->skelAnime.curFrame == 14.0f) {
            if ((thisv->actor.params & 0x1F) != GORON_DMT_BIGGORON) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_SIT_DOWN);
            } else {
                func_800F4524(&D_801333D4, NA_SE_EN_GOLON_SIT_DOWN, 60);
            }
        }
        if (thisv->skelAnime.playSpeed < 0.0f) {
            if (thisv->skelAnime.curFrame == 1.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
            }
            if (thisv->skelAnime.curFrame == 40.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_SIT_DOWN);
            }
        }
    }
}

void EnGo2_GetDustData(EnGo2* thisv, s32 index2) {
    s32 index1 = (thisv->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG ? 1 : 0;
    EnGo2DustEffectData* dustEffectData = &sDustEffectData[index1][index2];

    EnGo2_SpawnDust(thisv, dustEffectData->initialTimer, dustEffectData->scale, dustEffectData->scaleStep,
                    dustEffectData->numDustEffects, dustEffectData->radius, dustEffectData->yAccel);
}

void EnGo2_RollingAnimation(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_10);
        thisv->skelAnime.playSpeed = -0.5f;
    } else {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
        thisv->skelAnime.playSpeed = -1.0f;
    }
    EnGo2_SwapInitialFrameAnimFrameCount(thisv);
    thisv->unk_26E = 1;
    thisv->unk_211 = false;
    thisv->isAwake = false;
    thisv->actionFunc = EnGo2_CurledUp;
}

void EnGo2_WakeUp(EnGo2* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.playSpeed == 0.0f) {
        if ((thisv->actor.params & 0x1F) != GORON_DMT_BIGGORON) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_WAKE_UP);
        } else {
            func_800F4524(&D_801333D4, NA_SE_EN_GOLON_WAKE_UP, 60);
        }
    }
    if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        OnePointCutscene_Init(globalCtx, 4200, -99, &thisv->actor, MAIN_CAM);
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_10);
        thisv->skelAnime.playSpeed = 0.5f;
    } else {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
        thisv->skelAnime.playSpeed = 1.0f;
    }
    thisv->actionFunc = func_80A46B40;
}

void EnGo2_GetItemAnimation(EnGo2* thisv, GlobalContext* globalCtx) {
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
    thisv->unk_211 = true;
    thisv->actionFunc = func_80A46B40;
    thisv->skelAnime.playSpeed = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
}

void EnGo2_SetupRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG || (thisv->actor.params & 0x1F) == GORON_CITY_LINK) {
        thisv->collider.info.bumperFlags = 1;
        thisv->actor.speedXZ = gSaveContext.infTable[17] & 0x4000 ? 6.0f : 3.6000001f;
    } else {
        thisv->actor.speedXZ = 6.0f;
    }
    thisv->actor.flags |= ACTOR_FLAG_24;
    thisv->animTimer = 10;
    thisv->actor.shape.yOffset = 1800.0f;
    thisv->actor.speedXZ += thisv->actor.speedXZ; // Speeding up
    thisv->actionFunc = EnGo2_ContinueRolling;
}

void EnGo2_StopRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (((thisv->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG) && ((thisv->actor.params & 0x1F) != GORON_CITY_LINK)) {
        if ((thisv->actor.params & 0x1F) == GORON_DMT_ROLLING_SMALL) {
            bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                       thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
        }
    } else {
        thisv->collider.info.bumperFlags = 0;
    }

    thisv->actor.shape.rot = thisv->actor.world.rot;
    thisv->unk_59C = 0;
    thisv->unk_590 = 0;
    thisv->actionFunc = EnGo2_GroundRolling;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.speedXZ = 0.0f;
}

s32 EnGo2_IsFreeingGoronInFire(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params & 0x1F) != GORON_FIRE_GENERIC) {
        return false;
    }

    // shaking curled up
    thisv->actor.world.pos.x += (globalCtx->state.frames & 1) ? 1.0f : -1.0f;
    if (Flags_GetSwitch(globalCtx, (thisv->actor.params & 0xFC00) >> 0xA)) {
        return true;
    }
    return false;
}

s32 EnGo2_IsGoronDmtBombFlower(EnGo2* thisv) {
    if ((thisv->actor.params & 0x1F) != GORON_DMT_BOMB_FLOWER || thisv->unk_194.unk_00 != 2) {
        return false;
    }

    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_3);
    thisv->unk_194.unk_00 = 0;
    thisv->isAwake = false;
    thisv->unk_26E = 1;
    thisv->actionFunc = EnGo2_GoronDmtBombFlowerAnimation;
    return true;
}

s32 EnGo2_IsGoronRollingBig(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG || (thisv->unk_194.unk_00 != 2)) {
        return false;
    }
    thisv->unk_194.unk_00 = 0;
    EnGo2_RollingAnimation(thisv, globalCtx);
    thisv->actionFunc = EnGo2_GoronRollingBigContinueRolling;
    return true;
}

s32 EnGo2_IsGoronFireGeneric(EnGo2* thisv) {
    if ((thisv->actor.params & 0x1F) != GORON_FIRE_GENERIC || thisv->unk_194.unk_00 == 0) {
        return false;
    }
    thisv->actionFunc = EnGo2_GoronFireGenericAction;
    return true;
}

s32 EnGo2_IsGoronLinkReversing(EnGo2* thisv) {
    if ((thisv->actor.params & 0x1F) != GORON_CITY_LINK || (thisv->waypoint >= thisv->unk_216) ||
        !EnGo2_IsWakingUp(thisv)) {
        return false;
    }
    return true;
}

s32 EnGo2_IsRolling(EnGo2* thisv) {
    if (thisv->unk_194.unk_00 == 0 || thisv->actor.speedXZ < 1.0f) {
        return false;
    }
    if (EnGo2_IsRollingOnGround(thisv, 2, 20.0 / 3.0f, 0)) {
        if ((thisv->unk_590 >= 9) && (thisv->unk_59C == 0)) {
            thisv->unk_590 = 8;
        }
        EnGo2_GetDustData(thisv, 0);
    }
    return true;
}

void EnGo2_GoronLinkAnimation(EnGo2* thisv, GlobalContext* globalCtx) {
    s32 animation = ARRAY_COUNT(sAnimationInfo);

    if ((thisv->actor.params & 0x1F) == GORON_CITY_LINK) {
        if ((thisv->actor.textId == 0x3035 && thisv->unk_20C == 0) ||
            (thisv->actor.textId == 0x3036 && thisv->unk_20C == 0)) {
            if (thisv->skelAnime.animation != &gGoronAnim_000D5C) {
                animation = ENGO2_ANIM_12;
                thisv->eyeMouthTexState = 0;
            }
        }

        if ((thisv->actor.textId == 0x3032 && thisv->unk_20C == 12) || (thisv->actor.textId == 0x3033) ||
            (thisv->actor.textId == 0x3035 && thisv->unk_20C == 6)) {
            if (thisv->skelAnime.animation != &gGoronAnim_000750) {
                animation = ENGO2_ANIM_11;
                thisv->eyeMouthTexState = 1;
            }
        }

        if (thisv->skelAnime.animation == &gGoronAnim_000750) {
            if (thisv->skelAnime.curFrame == 20.0f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_CRY);
            }
        }

        if (animation != ARRAY_COUNT(sAnimationInfo)) {
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animation);
        }
    }
}

void EnGo2_GoronFireCamera(EnGo2* thisv, GlobalContext* globalCtx) {
    s16 yaw;

    thisv->camId = Gameplay_CreateSubCamera(globalCtx);
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
    Gameplay_ChangeCameraStatus(globalCtx, thisv->camId, CAM_STAT_ACTIVE);
    Path_CopyLastPoint(thisv->path, &thisv->at);
    yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->at) + 0xE38;
    thisv->eye.x = Math_SinS(yaw) * 100.0f + thisv->actor.world.pos.x;
    thisv->eye.z = Math_CosS(yaw) * 100.0f + thisv->actor.world.pos.z;
    thisv->eye.y = thisv->actor.world.pos.y + 20.0f;
    thisv->at.x = thisv->actor.world.pos.x;
    thisv->at.y = thisv->actor.world.pos.y + 40.0f;
    thisv->at.z = thisv->actor.world.pos.z;
    Gameplay_CameraSetAtEye(globalCtx, thisv->camId, &thisv->at, &thisv->eye);
}

void EnGo2_GoronFireClearCamera(EnGo2* thisv, GlobalContext* globalCtx) {
    Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
    Gameplay_ClearCamera(globalCtx, thisv->camId);
}

void EnGo2_BiggoronAnimation(EnGo2* thisv) {
    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN && INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS &&
        (thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON && thisv->unk_194.unk_00 == 0) {
        if (DECR(thisv->animTimer) == 0) {
            thisv->animTimer = Rand_S16Offset(30, 30);
            func_800F4524(&D_801333D4, NA_SE_EN_GOLON_EYE_BIG, 60);
        }
    }
}

void EnGo2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGo2* thisv = (EnGo2*)thisx;
    s32 pad;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 28.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gGoronSkel, NULL, thisv->jointTable, thisv->morphTable, 18);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);

    // Not GORON_CITY_ROLLING_BIG, GORON_CITY_LINK, GORON_DMT_BIGGORON
    switch (thisv->actor.params & 0x1F) {
        case GORON_FIRE_GENERIC:
        case GORON_DMT_BOMB_FLOWER:
        case GORON_DMT_ROLLING_SMALL:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
        case GORON_DMT_FAIRY_HINT:
        case GORON_MARKET_BAZAAR:
            thisv->actor.flags &= ~ACTOR_FLAG_4;
            thisv->actor.flags &= ~ACTOR_FLAG_5;
    }

    EnGo2_SetColliderDim(thisv);
    EnGo2_SetShape(thisv);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_0);
    thisv->actor.gravity = -1.0f;
    thisv->alpha = thisv->actor.shape.shadowAlpha = 0;
    thisv->reverse = 0;
    thisv->isAwake = false;
    thisv->unk_211 = false;
    thisv->goronState = 0;
    thisv->waypoint = 0;
    thisv->unk_216 = thisv->actor.shape.rot.z;
    thisv->unk_26E = 1;
    thisv->path = Path_GetByIndex(globalCtx, (thisv->actor.params & 0x3E0) >> 5, 0x1F);
    switch (thisv->actor.params & 0x1F) {
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
                Actor_Kill(&thisv->actor);
            }
            thisv->actionFunc = EnGo2_CurledUp;
            break;
        case GORON_MARKET_BAZAAR:
            if ((LINK_IS_ADULT) || !CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                Actor_Kill(&thisv->actor);
            }
            EnGo2_GetItemAnimation(thisv, globalCtx);
            break;
        case GORON_CITY_LINK:
            if ((gSaveContext.infTable[16] & 0x200)) {
                Path_CopyLastPoint(thisv->path, &thisv->actor.world.pos);
                thisv->actor.home.pos = thisv->actor.world.pos;
                if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                    EnGo2_GetItemAnimation(thisv, globalCtx);
                } else {
                    thisv->actionFunc = EnGo2_CurledUp;
                }
            } else {
                gSaveContext.infTable[16] &= ~0x1000;
                thisv->collider.dim.height = (D_80A4816C[thisv->actor.params & 0x1F].height * 0.6f);
                EnGo2_SetupRolling(thisv, globalCtx);
                thisv->isAwake = true;
            }
            break;
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_ROLLING_SMALL:
            thisv->collider.dim.height = (D_80A4816C[thisv->actor.params & 0x1F].height * 0.6f);
            EnGo2_SetupRolling(thisv, globalCtx);
            break;
        case GORON_FIRE_GENERIC:
            if (Flags_GetSwitch(globalCtx, (thisv->actor.params & 0xFC00) >> 0xA)) {
                Actor_Kill(&thisv->actor);
            } else {
                thisv->isAwake = true;
                thisv->actionFunc = EnGo2_CurledUp;
            }
            break;
        case GORON_DMT_BIGGORON:
            thisv->actor.shape.shadowDraw = NULL;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN) &&
                (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS)) {
                thisv->eyeMouthTexState = 1;
            }
            thisv->collider.base.acFlags = 0;
            thisv->collider.base.ocFlags1 = 0xD; // OC_PLAYER | OC_NO_PUSH | OC_ON
            thisv->actionFunc = EnGo2_CurledUp;
            break;
        case GORON_DMT_BOMB_FLOWER:
            if (gSaveContext.infTable[14] & 0x800) {
                Path_CopyLastPoint(thisv->path, &thisv->actor.world.pos);
                thisv->actor.home.pos = thisv->actor.world.pos;
            }
        case GORON_DMT_DC_ENTRANCE:
        case GORON_DMT_FAIRY_HINT:
        default:
            thisv->actionFunc = EnGo2_CurledUp;
    }
}

void EnGo2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnGo2_CurledUp(EnGo2* thisv, GlobalContext* globalCtx) {
    u8 index = thisv->actor.params & 0x1F;
    s16 height;
    s32 quake;

    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
            quake = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);
            Quake_SetSpeed(quake, -0x3CB0);
            Quake_SetQuakeValues(quake, 8, 0, 0, 0);
            Quake_SetCountdown(quake, 16);
        } else {
            EnGo2_GetDustData(thisv, 1);
        }
        thisv->skelAnime.playSpeed = 0.0f;
    }

    if ((s32)thisv->skelAnime.curFrame == 0) {
        thisv->collider.dim.height = (D_80A4816C[index].height * 0.6f);
    } else {
        height = D_80A4816C[index].height;
        thisv->collider.dim.height =
            ((D_80A4816C[index].height * 0.4f * (thisv->skelAnime.curFrame / thisv->skelAnime.startFrame)) +
             (height * 0.6f));
    }
    if (EnGo2_IsFreeingGoronInFire(thisv, globalCtx)) {
        thisv->isAwake = false;
        EnGo2_WakeUp(thisv, globalCtx);
    }
    if (((thisv->actor.params & 0x1F) != GORON_FIRE_GENERIC) && EnGo2_IsWakingUp(thisv)) {
        EnGo2_WakeUp(thisv, globalCtx);
    }
}

void func_80A46B40(EnGo2* thisv, GlobalContext* globalCtx) {
    u8 index = (thisv->actor.params & 0x1F);
    f32 height;

    if (thisv->unk_211 == true) {
        EnGo2_BiggoronAnimation(thisv);
        EnGo2_GoronLinkAnimation(thisv, globalCtx);
        EnGo2_SelectGoronWakingUp(thisv);

        if (!EnGo2_IsGoronRollingBig(thisv, globalCtx) && !EnGo2_IsGoronFireGeneric(thisv)) {
            if (EnGo2_IsGoronDmtBombFlower(thisv)) {
                return;
            }
        } else {
            return;
        }
    } else {
        if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
            if ((thisv->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
                thisv->actor.flags |= ACTOR_FLAG_0;
            }
            func_80A454CC(thisv);
            thisv->unk_211 = true;
            thisv->collider.dim.height = D_80A4816C[index].height;
        } else {
            height = D_80A4816C[index].height;
            thisv->collider.dim.height =
                (s16)((height * 0.4f * (thisv->skelAnime.curFrame / thisv->skelAnime.endFrame)) + (height * 0.6f));
        }
    }
    if ((!EnGo2_IsCameraModified(thisv, globalCtx)) && (!EnGo2_IsWakingUp(thisv))) {
        EnGo2_RollingAnimation(thisv, globalCtx);
    }
}

void EnGo2_GoronDmtBombFlowerAnimation(EnGo2* thisv, GlobalContext* globalCtx) {
    f32 float1 = thisv->skelAnime.endFrame;
    f32 float2 = thisv->skelAnime.curFrame * ((f32)0x8000 / float1);

    thisv->actor.speedXZ = Math_SinS(float2);
    if ((EnGo2_Orient(thisv, globalCtx)) && (thisv->waypoint == 0)) {
        EnGo2_GetItemAnimation(thisv, globalCtx);
    }
}

void EnGo2_GoronRollingBigContinueRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        EnGo2_GetDustData(thisv, 1);
        thisv->skelAnime.playSpeed = 0.0f;
        EnGo2_SetupRolling(thisv, globalCtx);
    }
}

void EnGo2_ContinueRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    f32 float1 = 1000.0f;

    if (((thisv->actor.params & 0x1F) != GORON_DMT_ROLLING_SMALL || !(thisv->actor.xyzDistToPlayerSq > SQ(float1))) &&
        DECR(thisv->animTimer) == 0) {
        thisv->actionFunc = EnGo2_SlowRolling;
        thisv->actor.speedXZ *= 0.5f; // slowdown
    }
    EnGo2_GetDustData(thisv, 2);
}

void EnGo2_SlowRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    s32 orientation;
    s32 index;

    if (!EnGo2_IsRolling(thisv)) {
        if (EnGo2_IsRollingOnGround(thisv, 4, 8.0f, 1) == true) {
            if (EnGo2_IsGoronLinkReversing(thisv)) {
                thisv->actionFunc = EnGo2_ReverseRolling;
                return;
            }
            EnGo2_GetDustData(thisv, 3);
        }
        orientation = EnGo2_Orient(thisv, globalCtx);
        index = thisv->actor.params & 0x1F;
        if (index != GORON_CITY_LINK) {
            if ((index == GORON_DMT_ROLLING_SMALL) && (orientation == 1) && (thisv->waypoint == 0)) {
                EnGo2_StopRolling(thisv, globalCtx);
                return;
            }
        } else if ((orientation == 2) && (thisv->waypoint == 1)) {
            EnGo2_StopRolling(thisv, globalCtx);
            return;
        }
        Math_ApproachF(&thisv->actor.speedXZ, EnGo2_GetTargetXZSpeed(thisv), 0.4f, 0.6f);
        thisv->actor.shape.rot = thisv->actor.world.rot;
    }
}

void EnGo2_GroundRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    if (EnGo2_IsRollingOnGround(thisv, 4, 8.0f, 0)) {
        EnGo2_GetDustData(thisv, 0);
        if (thisv->unk_59C == 0) {
            switch (thisv->actor.params & 0x1F) {
                case GORON_CITY_LINK:
                    thisv->goronState = 0;
                    thisv->actionFunc = EnGo2_GoronLinkStopRolling;
                    break;
                case GORON_CITY_ROLLING_BIG:
                    EnGo2_WakeUp(thisv, globalCtx);
                    break;
                default:
                    thisv->actionFunc = EnGo2_CurledUp;
            }
        }
    }
}

void EnGo2_ReverseRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    if (!EnGo2_IsRolling(thisv)) {
        Math_ApproachF(&thisv->actor.speedXZ, 0.0f, 0.6f, 0.8f);
        if (thisv->actor.speedXZ >= 1.0f) {
            EnGo2_GetDustData(thisv, 3);
        }
        if ((s32)thisv->actor.speedXZ == 0) {
            thisv->actor.world.rot.y ^= 0x8000;
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
            thisv->reverse ^= 1;
            EnGo2_UpdateWaypoint(thisv, globalCtx);
            EnGo2_SetupRolling(thisv, globalCtx);
        }
    }
}

void EnGo2_SetupGetItem(EnGo2* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actor.parent = NULL;
        thisv->actionFunc = EnGo2_SetGetItem;
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, thisv->actor.xzDistToPlayer + 1.0f,
                      fabsf(thisv->actor.yDistToPlayer) + 1.0f);
    }
}

void EnGo2_SetGetItem(EnGo2* thisv, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        thisv->unk_194.unk_00 = 0;
        switch (thisv->getItemId) {
            case GI_CLAIM_CHECK:
                Environment_ClearBgsDayCount();
                EnGo2_GetItemAnimation(thisv, globalCtx);
                return;
            case GI_TUNIC_GORON:
                gSaveContext.infTable[16] |= 0x200;
                EnGo2_GetItemAnimation(thisv, globalCtx);
                return;
            case GI_SWORD_BGS:
                gSaveContext.bgsFlag = true;
                break;
            case GI_BOMB_BAG_30:
            case GI_BOMB_BAG_40:
                EnGo2_RollingAnimation(thisv, globalCtx);
                thisv->actionFunc = EnGo2_GoronRollingBigContinueRolling;
                return;
        }
        thisv->actionFunc = func_80A46B40;
    }
}

void EnGo2_BiggoronEyedrops(EnGo2* thisv, GlobalContext* globalCtx) {
    switch (thisv->goronState) {
        case 0:
            Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_5);
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            thisv->actor.shape.rot.y += 0x5B0;
            thisv->unk_26E = 1;
            thisv->animTimer = thisv->skelAnime.endFrame + 60.0f + 60.0f; // eyeDrops animation timer
            thisv->eyeMouthTexState = 2;
            thisv->unk_20C = 0;
            thisv->goronState++;
            func_800F483C(0x28, 5);
            OnePointCutscene_Init(globalCtx, 4190, -99, &thisv->actor, MAIN_CAM);
            break;
        case 1:
            if (DECR(thisv->animTimer)) {
                if (thisv->animTimer == 60 || thisv->animTimer == 120) {
                    func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
                    func_800F4524(&D_801333D4, NA_SE_EV_GORON_WATER_DROP, 60);
                }
            } else {
                func_800F4524(&D_801333D4, NA_SE_EN_GOLON_GOOD_BIG, 60);
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_6);
                Message_ContinueTextbox(globalCtx, 0x305A);
                thisv->eyeMouthTexState = 3;
                thisv->goronState++;
                func_800F483C(0x7F, 5);
            }
            break;
        case 2:
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
                thisv->eyeMouthTexState = 0;
            }
            if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
                thisv->actor.flags |= ACTOR_FLAG_0;
                thisv->unk_26E = 2;
                thisv->skelAnime.playSpeed = 0.0f;
                thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
                EnGo2_GetItem(thisv, globalCtx, GI_CLAIM_CHECK);
                thisv->actionFunc = EnGo2_SetupGetItem;
                thisv->goronState = 0;
            }
            break;
    }
}

void EnGo2_GoronLinkStopRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    switch (thisv->goronState) {
        case 0:
            if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_NONE) {
                return;
            } else {
                Message_StartTextbox(globalCtx, 0x3031, NULL);
                player->actor.freezeTimer = 10;
                thisv->goronState++;
            }
        case 1:
            break;
        default:
            return;
    }

    if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_CLOSING) {
        player->actor.freezeTimer = 10;
    } else {
        gSaveContext.infTable[16] |= 0x1000;
        thisv->unk_26E = 1;
        thisv->unk_211 = false;
        thisv->isAwake = false;
        thisv->actionFunc = EnGo2_CurledUp;
    }
}

void EnGo2_GoronFireGenericAction(EnGo2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3s D_80A4854C = { 0x00, 0x00, 0x00 };

    switch (thisv->goronState) {
        case 0: // Wake up
            if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
                EnGo2_GoronFireCamera(thisv, globalCtx);
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENGO2_ANIM_2);
                thisv->waypoint = 1;
                thisv->skelAnime.playSpeed = 2.0f;
                func_80A44D84(thisv);
                thisv->actor.shape.rot = thisv->actor.world.rot;
                thisv->animTimer = 60;
                thisv->actor.gravity = 0.0f;
                thisv->actor.speedXZ = 2.0f;
                thisv->unk_194.unk_08 = D_80A4854C;
                thisv->unk_194.unk_0E = D_80A4854C;
                thisv->goronState++;
                thisv->goronState++;
                player->actor.world.rot.y = thisv->actor.world.rot.y;
                player->actor.shape.rot.y = thisv->actor.world.rot.y;
                player->actor.world.pos.x =
                    (f32)((Math_SinS(thisv->actor.world.rot.y) * -30.0f) + thisv->actor.world.pos.x);
                player->actor.world.pos.z =
                    (f32)((Math_CosS(thisv->actor.world.rot.y) * -30.0f) + thisv->actor.world.pos.z);
                func_8002DF54(globalCtx, &thisv->actor, 8);
                Audio_PlayFanfare(NA_BGM_APPEAR);
            }
            break;
        case 2: // Walking away
            if (DECR(thisv->animTimer)) {
                if (!(thisv->animTimer % 8)) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
                }
                Actor_MoveForward(&thisv->actor);
            } else {
                thisv->animTimer = 0;
                thisv->actor.speedXZ = 0.0f;
                if ((((thisv->actor.params & 0xFC00) >> 0xA) != 1) && (((thisv->actor.params & 0xFC00) >> 0xA) != 2) &&
                    (((thisv->actor.params & 0xFC00) >> 0xA) != 4) && (((thisv->actor.params & 0xFC00) >> 0xA) != 5) &&
                    (((thisv->actor.params & 0xFC00) >> 0xA) != 9) && (((thisv->actor.params & 0xFC00) >> 0xA) != 11)) {
                    thisv->goronState++;
                }
                thisv->goronState++;
            }
            break;
        case 3: // Walking away
            thisv->animTimer++;
            if (!(thisv->animTimer % 8) && (thisv->animTimer < 10)) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MORIBLIN_WALK);
            }
            if (thisv->animTimer == 10) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_IRON_DOOR_OPEN);
            }
            if (thisv->animTimer > 44) {
                SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EV_IRON_DOOR_CLOSE);
            } else {
                break;
            }
        case 4: // Finalize walking away
            Message_CloseTextbox(globalCtx);
            EnGo2_GoronFireClearCamera(thisv, globalCtx);
            func_8002DF54(globalCtx, &thisv->actor, 7);
            Actor_Kill(&thisv->actor);
            break;
        case 1:
            break;
    }
}

void EnGo2_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGo2* thisv = (EnGo2*)thisx;

    func_80A45360(thisv, &thisv->alpha);
    EnGo2_SitDownAnimation(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    EnGo2_RollForward(thisv);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, (f32)thisv->collider.dim.height * 0.5f,
                            (f32)thisv->collider.dim.radius * 0.6f, 0.0f, 5);
    if (thisv->unk_194.unk_00 == 0) {
        func_80A44AB0(thisv, globalCtx);
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->unk_211 == true) {
        func_80034F54(globalCtx, thisv->unk_226, thisv->unk_24A, 18);
    }
    func_80A45288(thisv, globalCtx);
    EnGo2_EyeMouthTexState(thisv);
    EnGo2_CheckCollision(thisv, globalCtx);
}

s32 EnGo2_DrawCurledUp(EnGo2* thisv, GlobalContext* globalCtx) {
    Vec3f D_80A48554 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 2881);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go2.c", 2884),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00BD80);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 2889);
    Matrix_MultVec3f(&D_80A48554, &thisv->actor.focus.pos);

    return 1;
}

s32 EnGo2_DrawRolling(EnGo2* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f D_80A48560 = { 0.0f, 0.0f, 0.0f };
    f32 speedXZ;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 2914);
    func_80093D18(globalCtx->state.gfxCtx);
    speedXZ = thisv->actionFunc == EnGo2_ReverseRolling ? 0.0f : thisv->actor.speedXZ;
    Matrix_RotateZYX((globalCtx->state.frames * ((s16)speedXZ * 1400)), 0, thisv->actor.shape.rot.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go2.c", 2926),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00C140);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 2930);
    Matrix_MultVec3f(&D_80A48560, &thisv->actor.focus.pos);
    return 1;
}

s32 EnGo2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo2* thisv = (EnGo2*)thisx;
    Vec3s vec1;
    f32 float1;

    if (limb == 17) {
        Matrix_Translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec1 = thisv->unk_194.unk_08;
        float1 = (vec1.y / (f32)0x8000) * std::numbers::pi_v<float>;
        Matrix_RotateX(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * std::numbers::pi_v<float>;
        Matrix_RotateZ(float1, MTXMODE_APPLY);
        Matrix_Translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limb == 10) {
        vec1 = thisv->unk_194.unk_0E;
        float1 = (vec1.y / (f32)0x8000) * std::numbers::pi_v<float>;
        Matrix_RotateY(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * std::numbers::pi_v<float>;
        Matrix_RotateX(float1, MTXMODE_APPLY);
    }
    if ((limb == 10) || (limb == 11) || (limb == 14)) {
        float1 = Math_SinS(thisv->unk_226[limb]);
        rot->y += float1 * 200.0f;
        float1 = Math_CosS(thisv->unk_24A[limb]);
        rot->z += float1 * 200.0f;
    }
    return 0;
}

void EnGo2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo2* thisv = (EnGo2*)thisx;
    Vec3f D_80A4856C = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 17) {
        Matrix_MultVec3f(&D_80A4856C, &thisv->actor.focus.pos);
    }
}

void EnGo2_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnGo2* thisv = (EnGo2*)thisx;
    void* eyeTextures[] = { gGoronCsEyeClosed2Tex, gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    void* mouthTextures[] = { gGoronCsMouthNeutralTex, gGoronCsMouthSmileTex };

    EnGo2_UpdateDust(thisv);
    Matrix_Push();
    EnGo2_DrawDust(thisv, globalCtx);
    Matrix_Pop();

    if ((thisv->actionFunc == EnGo2_CurledUp) && (thisv->skelAnime.playSpeed == 0.0f) &&
        (thisv->skelAnime.curFrame == 0.0f)) {
        if (1) {}
        EnGo2_DrawCurledUp(thisv, globalCtx);
    } else if (thisv->actionFunc == EnGo2_SlowRolling || thisv->actionFunc == EnGo2_ReverseRolling ||
               thisv->actionFunc == EnGo2_ContinueRolling) {
        EnGo2_DrawRolling(thisv, globalCtx);
    } else {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 3063);
        func_80093D18(globalCtx->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTextures[thisv->mouthTexIndex]));

        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnGo2_OverrideLimbDraw, EnGo2_PostLimbDraw, thisv);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go2.c", 3081);
    }
}
