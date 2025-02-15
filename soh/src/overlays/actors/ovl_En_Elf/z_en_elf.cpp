/*
 * File: z_en_elf.c
 * Overlay: ovl_En_Elf
 * Description: Fairy
 */

#include "z_en_elf.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

#define FAIRY_FLAG_TIMED (1 << 8)
#define FAIRY_FLAG_BIG (1 << 9)

void EnElf_Init(Actor* thisx, GlobalContext* globalCtx);
void EnElf_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnElf_Update(Actor* thisx, GlobalContext* globalCtx);
void EnElf_Draw(Actor* thisx, GlobalContext* globalCtx);
void func_80A053F0(Actor* thisx, GlobalContext* globalCtx);
void func_80A052F4(Actor* thisx, GlobalContext* globalCtx);
void func_80A05208(Actor* thisx, GlobalContext* globalCtx);
void func_80A05188(Actor* thisx, GlobalContext* globalCtx);
void func_80A05114(Actor* thisx, GlobalContext* globalCtx);
void func_80A05040(Actor* thisx, GlobalContext* globalCtx);

// Navi
void func_80A03CF8(EnElf* thisv, GlobalContext* globalCtx);

// Healing Fairies
void func_80A0329C(EnElf* thisv, GlobalContext* globalCtx);
void func_80A03610(EnElf* thisv, GlobalContext* globalCtx);

// Healing Fairies Revive From Death
void func_80A03990(EnElf* thisv, GlobalContext* globalCtx);
void func_80A03814(EnElf* thisv, GlobalContext* globalCtx);

// Kokiri Fairies
void func_80A0353C(EnElf* thisv, GlobalContext* globalCtx);

// Fairy Spawner
void func_80A03604(EnElf* thisv, GlobalContext* globalCtx);

// Move(?) functions
void func_80A0214C(EnElf* thisv, GlobalContext* globalCtx);
void func_80A02AA4(EnElf* thisv, GlobalContext* globalCtx);
void func_80A02A20(EnElf* thisv, GlobalContext* globalCtx);
void func_80A02B38(EnElf* thisv, GlobalContext* globalCtx);
void func_80A020A4(EnElf* thisv, GlobalContext* globalCtx);
void func_80A01FE0(EnElf* thisv, GlobalContext* globalCtx);

// misc
void func_80A04414(EnElf* thisv, GlobalContext* globalCtx);
void func_80A0461C(EnElf* thisv, GlobalContext* globalCtx);
void EnElf_SpawnSparkles(EnElf* thisv, GlobalContext* globalCtx, s32 sparkleLife);
void EnElf_GetCutsceneNextPos(Vec3f* vec, GlobalContext* globalCtx, s32 action);

ActorInit En_Elf_InitVars = {
    ACTOR_EN_ELF,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnElf),
    (ActorFunc)EnElf_Init,
    (ActorFunc)EnElf_Destroy,
    (ActorFunc)EnElf_Update,
    (ActorFunc)EnElf_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 8, ICHAIN_STOP),
};

static Color_RGBAf sInnerColors[] = {
    { 255.0f, 255.0f, 255.0f, 255.0f },
    { 255.0f, 220.0f, 220.0f, 255.0f },
};

static Color_RGBAf sOuterColors[] = {
    { 255.0f, 255.0f, 255.0f, 255.0f },
    { 255.0f, 50.0f, 100.0f, 255.0f },
};

typedef struct {
    u8 r, g, b;
} FairyColorFlags;

static FairyColorFlags sColorFlags[] = {
    { 0, 0, 0 }, { 1, 0, 0 }, { 1, 2, 0 }, { 1, 0, 2 }, { 0, 1, 0 }, { 2, 1, 0 }, { 0, 1, 2 },
    { 0, 0, 1 }, { 2, 0, 1 }, { 0, 2, 1 }, { 1, 1, 0 }, { 1, 0, 1 }, { 0, 1, 1 },
};

void EnElf_SetupAction(EnElf* thisv, EnElfActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void func_80A01C38(EnElf* thisv, s32 arg1) {
    thisv->unk_2A8 = arg1;

    switch (thisv->unk_2A8) {
        case 0:
            thisv->unk_2AE = 0x400;
            thisv->unk_2B0 = 0x200;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2C0 = 100;
            thisv->unk_2B4 = 5.0f;
            thisv->unk_2B8 = 20.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 12:
            thisv->unk_2AE = 0x400;
            thisv->unk_2B0 = 0x200;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2C0 = 100;
            thisv->unk_2B4 = 1.0f;
            thisv->unk_2B8 = 5.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 10:
            thisv->unk_2AE = 0x400;
            thisv->unk_2B0 = 0;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2B8 = 0.0f;
            thisv->unk_2B4 = 5.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 9:
            thisv->unk_2AE = 0x1000;
            thisv->unk_2B0 = 0x200;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2B4 = 3.0f;
            thisv->unk_2B8 = 10.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 7:
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2AE = 0x1E;
            thisv->unk_2C0 = 1;
            thisv->unk_2B4 = 0.0f;
            thisv->unk_2B8 = 0.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 8:
            thisv->unk_2AE = 0x1000;
            thisv->unk_2B0 = 0x200;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2B4 = 0.0f;
            thisv->unk_2B8 = 0.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 1:
            thisv->unk_2AE = 0x1000;
            thisv->unk_2B0 = 0x800;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2B4 = 5.0f;
            thisv->unk_2B8 = 7.5f;
            thisv->skelAnime.playSpeed = 2.0f;
            break;
        case 2:
            thisv->unk_2AE = 0x400;
            thisv->unk_2B0 = 0x1000;
            thisv->func_2C8 = func_80A02AA4;
            thisv->unk_2B4 = 10.0f;
            thisv->unk_2B8 = 20.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 3:
            thisv->unk_2B0 = 0x600;
            thisv->func_2C8 = func_80A02B38;
            thisv->unk_2B8 = 1.0f;
            thisv->unk_2B4 = 1.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
        case 4:
            thisv->unk_2B0 = 0x800;
            thisv->func_2C8 = func_80A02B38;
            thisv->unk_2B4 = 20.0f;
            thisv->unk_2B8 = 10.0;
            thisv->skelAnime.playSpeed = 2.0f;
            break;
        case 5:
            thisv->unk_2B0 = 0x200;
            thisv->func_2C8 = func_80A02B38;
            thisv->unk_2B4 = 10.0f;
            thisv->unk_2B8 = 10.0f;
            thisv->skelAnime.playSpeed = 0.5f;
            break;
        case 6:
            thisv->unk_2AE = 0x1000;
            thisv->unk_2B0 = 0x800;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2B4 = 60.0f;
            thisv->unk_2B8 = 20.0f;
            thisv->skelAnime.playSpeed = 2.0f;
            break;
        case 11:
            thisv->unk_2AE = 0x400;
            thisv->unk_2B0 = 0x2000;
            thisv->func_2C8 = func_80A02A20;
            thisv->unk_2C0 = 42;
            thisv->unk_2B4 = 5.0f;
            thisv->unk_2B8 = 1.0f;
            thisv->skelAnime.playSpeed = 1.0f;
            break;
    }
}

s32 func_80A01F90(Vec3f* thisv, Vec3f* arg1, f32 arg2) {
    return SQ(arg2) < (SQ(arg1->x - thisv->x) + SQ(arg1->z - thisv->z));
}

void func_80A01FE0(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!func_80A01F90(&thisv->actor.world.pos, &player->actor.world.pos, 30.0f)) {
        thisv->unk_2B8 = 0.5f;
    } else {
        thisv->unk_2B8 = 2.0f;
    }

    if (thisv->unk_2C0 > 0) {
        thisv->unk_2C0--;
    } else {
        thisv->unk_2A8 = 1;
        thisv->unk_2AC = 0x80;
        thisv->unk_2B8 = Rand_ZeroFloat(1.0f) + 0.5f;
        thisv->unk_2B0 = Rand_CenteredFloat(32767.0f);
        thisv->func_2C8 = func_80A0214C;
    }
}

void func_80A020A4(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (func_80A01F90(&thisv->actor.world.pos, &player->actor.world.pos, 50.0f)) {
        if (thisv->unk_2C0 > 0) {
            thisv->unk_2C0--;
        } else {
            thisv->unk_2A8 = 1;
            thisv->unk_2AC = 0x80;
            thisv->unk_2B8 = Rand_ZeroFloat(1.0f) + 0.5f;
            thisv->unk_2B0 = Rand_CenteredFloat(32767.0f);
            thisv->func_2C8 = func_80A0214C;
        }
    }
}

void func_80A0214C(EnElf* thisv, GlobalContext* globalCtx) {
    f32 xzDistToPlayer;

    if (thisv->unk_2C0 > 0) {
        thisv->unk_2C0--;
    } else {
        xzDistToPlayer = thisv->actor.xzDistToPlayer;
        if (xzDistToPlayer < 50.0f) {
            if (Rand_ZeroOne() < 0.2f) {
                thisv->unk_2A8 = 2;
                thisv->unk_2AC = 0x400;
                thisv->unk_2B8 = 2.0f;
                thisv->func_2C8 = func_80A020A4;
                thisv->actor.speedXZ = 1.5f;
                thisv->unk_2C0 = (s16)Rand_ZeroFloat(8.0f) + 4;
            } else {
                thisv->unk_2C0 = 10;
            }
        } else {
            if (xzDistToPlayer > 150.0f) {
                xzDistToPlayer = 150.0f;
            }

            xzDistToPlayer = ((xzDistToPlayer - 50.0f) * 0.95f) + 0.05f;

            if (Rand_ZeroOne() < xzDistToPlayer) {
                thisv->unk_2A8 = 3;
                thisv->unk_2AC = 0x200;
                thisv->unk_2B8 = (xzDistToPlayer * 2.0f) + 1.0f;
                thisv->func_2C8 = func_80A01FE0;
                thisv->unk_2C0 = (s16)Rand_ZeroFloat(16.0f) + 0x10;
            } else {
                thisv->unk_2C0 = 10;
            }
        }
    }

    if (Rand_ZeroOne() < 0.1f) {
        thisv->unk_2A8 = 1;
        thisv->unk_2AC = 0x80;
        thisv->unk_2B8 = Rand_ZeroFloat(0.5f) + 0.5f;
        thisv->unk_2B0 = Rand_CenteredFloat(32767.0f);
    }
}

void func_80A0232C(EnElf* thisv, GlobalContext* globalCtx) {
    if (func_80A01F90(&thisv->unk_28C, &thisv->actor.world.pos, 100.0f)) {
        thisv->unk_2A8 = 0;
        thisv->unk_2AC = 0x200;
        thisv->func_2C8 = func_80A0214C;
        thisv->unk_2B8 = 1.5f;
    } else {
        thisv->func_2C8(thisv, globalCtx);
    }
}

f32 EnElf_GetColorValue(s32 colorFlag) {
    switch (colorFlag) {
        case 1:
            return Rand_ZeroFloat(55.0f) + 200.0f;
        case 2:
            return Rand_ZeroFloat(255.0f);
        default:
            return 0.0f;
    }
}

void EnElf_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnElf* thisv = (EnElf*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 colorConfig;
    s32 i;

    Actor_ProcessInitChain(thisx, sInitChain);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gFairySkel, &gFairyAnim, thisv->jointTable, thisv->morphTable, 15);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 15.0f);
    thisx->shape.shadowAlpha = 0xFF;

    Lights_PointGlowSetInfo(&thisv->lightInfoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 255, 255,
                            255, 0);
    thisv->lightNodeGlow = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfoGlow);

    Lights_PointNoGlowSetInfo(&thisv->lightInfoNoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 255,
                              255, 255, 0);
    thisv->lightNodeNoGlow = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfoNoGlow);

    thisv->fairyFlags = 0;
    thisv->disappearTimer = 600;
    thisv->unk_2A4 = 0.0f;
    colorConfig = 0;

    switch (thisx->params) {
        case FAIRY_NAVI:
            thisx->room = -1;
            EnElf_SetupAction(thisv, func_80A03CF8);
            func_80A01C38(thisv, 0);
            thisv->fairyFlags |= 4;
            thisx->update = func_80A053F0;
            thisv->elfMsg = NULL;
            thisv->unk_2C7 = 0x14;

            if ((gSaveContext.naviTimer >= 25800) || (gSaveContext.naviTimer < 3000)) {
                gSaveContext.naviTimer = 0;
            }
            break;
        case FAIRY_REVIVE_BOTTLE:
            colorConfig = -1;
            EnElf_SetupAction(thisv, func_80A03610);
            thisv->unk_2B8 = Math_Vec3f_DistXZ(&thisx->world.pos, &player->actor.world.pos);
            thisv->unk_2AC = player->actor.shape.rot.y;
            thisv->unk_2B0 = -0x1000;
            thisv->unk_28C.y = thisx->world.pos.y - player->actor.world.pos.y;
            thisv->unk_2AA = 0;
            thisv->unk_2B4 = 0.0f;
            break;
        case FAIRY_REVIVE_DEATH:
            colorConfig = -1;
            EnElf_SetupAction(thisv, func_80A03990);
            thisv->unk_2B8 = 0.0f;
            thisv->unk_2AC = player->actor.shape.rot.y;
            thisv->unk_2B0 = 0;
            thisv->unk_28C.y = thisx->world.pos.y - player->actor.world.pos.y;
            thisv->unk_2AA = 0;
            thisv->unk_2B4 = 7.0f;
            break;
        case FAIRY_HEAL_BIG:
            thisv->fairyFlags |= FAIRY_FLAG_BIG;
            thisx->shape.shadowDraw = ActorShadow_DrawWhiteCircle;
        case FAIRY_HEAL_TIMED:
            thisv->fairyFlags |= FAIRY_FLAG_TIMED;
        case FAIRY_HEAL:
            colorConfig = -1;
            EnElf_SetupAction(thisv, func_80A0329C);
            thisv->unk_2B4 = Rand_ZeroFloat(10.0f) + 10.0f;
            thisv->unk_2AA = 0;
            thisv->unk_2AE = (s16)(Rand_ZeroFloat(1048.0f)) + 0x200;
            thisv->unk_28C = thisx->world.pos;
            thisv->unk_2BC = Rand_CenteredFloat(32767.0f);
            thisv->func_2C8 = func_80A0214C;
            func_80A0232C(thisv, globalCtx);
            thisv->unk_2C0 = 0;
            thisv->disappearTimer = 240;
            break;
        case FAIRY_KOKIRI:
            colorConfig = Rand_ZeroFloat(11.99f) + 1.0f;
            EnElf_SetupAction(thisv, func_80A0353C);
            func_80A01C38(thisv, 0);
            break;
        case FAIRY_SPAWNER:
            EnElf_SetupAction(thisv, func_80A03604);
            func_80A01C38(thisv, 8);

            for (i = 0; i < 8; i++) {
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ELF, thisx->world.pos.x,
                            thisx->world.pos.y - 30.0f, thisx->world.pos.z, 0, 0, 0, FAIRY_HEAL);
            }
            break;
        default:
            ASSERT(0, "0", "../z_en_elf.c", 1103);
            break;
    }

    thisv->unk_2A0 = 3.0f;
    thisv->innerColor = sInnerColors[0];

    if (colorConfig > 0) {
        thisv->outerColor.r = EnElf_GetColorValue(sColorFlags[colorConfig].r);
        thisv->outerColor.g = EnElf_GetColorValue(sColorFlags[colorConfig].g);
        thisv->outerColor.b = EnElf_GetColorValue(sColorFlags[colorConfig].b);
        thisv->outerColor.a = 0.0f;
    } else {
        thisv->innerColor = sInnerColors[-colorConfig];
        thisv->outerColor = sOuterColors[-colorConfig];
    }
}

void func_80A0299C(EnElf* thisv, s32 arg1) {
}

void func_80A029A8(EnElf* thisv, s16 increment) {
    if (thisv->disappearTimer < 600) {
        thisv->disappearTimer += increment;
    }
}

void EnElf_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnElf* thisv = (EnElf*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNodeGlow);
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNodeNoGlow);
}

void func_80A02A20(EnElf* thisv, GlobalContext* globalCtx) {
    thisv->unk_28C.x = Math_SinS(thisv->unk_2AC) * thisv->unk_2B8;
    thisv->unk_28C.y = Math_SinS(thisv->unk_2AA) * thisv->unk_2B4;
    thisv->unk_28C.z = Math_CosS(thisv->unk_2AC) * thisv->unk_2B8;
    thisv->unk_2AC += thisv->unk_2B0;
    thisv->unk_2AA += thisv->unk_2AE;
}

void func_80A02AA4(EnElf* thisv, GlobalContext* globalCtx) {
    f32 xzScale;

    xzScale = (Math_CosS(thisv->unk_2AA) * thisv->unk_2B4) + thisv->unk_2B8;

    thisv->unk_28C.x = Math_SinS(thisv->unk_2AC) * xzScale;
    thisv->unk_28C.y = 0.0f;
    thisv->unk_28C.z = Math_CosS(thisv->unk_2AC) * xzScale;

    thisv->unk_2AC += thisv->unk_2B0;
    thisv->unk_2AA += thisv->unk_2AE;
}

void func_80A02B38(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_2AA = (thisv->unk_2AC * 2) & 0xFFFF;
    thisv->unk_28C.x = Math_SinS(thisv->unk_2AC) * thisv->unk_2B8;
    thisv->unk_28C.y = Math_SinS(thisv->unk_2AA) * thisv->unk_2B4;
    thisv->unk_28C.z = -Math_SinS(player->actor.shape.rot.y) * thisv->unk_28C.x;
    thisv->unk_28C.x = Math_CosS(player->actor.shape.rot.y) * thisv->unk_28C.x;
    thisv->unk_2AC += thisv->unk_2B0;
}

void func_80A02BD8(EnElf* thisv, Vec3f* targetPos, f32 arg2) {
    f32 yVelTarget;
    f32 yVelDirection;

    yVelTarget = ((targetPos->y + thisv->unk_28C.y) - thisv->actor.world.pos.y) * arg2;
    yVelDirection = (yVelTarget >= 0.0f) ? 1.0f : -1.0f;
    yVelTarget = fabsf(yVelTarget);
    yVelTarget = CLAMP(yVelTarget, 0.0f, 20.0f) * yVelDirection;
    Math_StepToF(&thisv->actor.velocity.y, yVelTarget, 32.0f);
}

void func_80A02C98(EnElf* thisv, Vec3f* targetPos, f32 arg2) {
    f32 xVelTarget;
    f32 zVelTarget;
    f32 xVelDirection;
    f32 zVelDirection;

    xVelTarget = ((targetPos->x + thisv->unk_28C.x) - thisv->actor.world.pos.x) * arg2;
    zVelTarget = ((targetPos->z + thisv->unk_28C.z) - thisv->actor.world.pos.z) * arg2;

    xVelDirection = (xVelTarget >= 0.0f) ? 1.0f : -1.0f;
    zVelDirection = (zVelTarget >= 0.0f) ? 1.0f : -1.0f;

    xVelTarget = fabsf(xVelTarget);
    zVelTarget = fabsf(zVelTarget);

    xVelTarget = CLAMP(xVelTarget, 0.0f, 20.0f) * xVelDirection;
    zVelTarget = CLAMP(zVelTarget, 0.0f, 20.0f) * zVelDirection;

    func_80A02BD8(thisv, targetPos, arg2);
    Math_StepToF(&thisv->actor.velocity.x, xVelTarget, 1.5f);
    Math_StepToF(&thisv->actor.velocity.z, zVelTarget, 1.5f);
    func_8002D7EC(&thisv->actor);
}

void func_80A02E30(EnElf* thisv, Vec3f* targetPos) {
    func_80A02BD8(thisv, targetPos, 0.2f);
    thisv->actor.velocity.x = (targetPos->x + thisv->unk_28C.x) - thisv->actor.world.pos.x;
    thisv->actor.velocity.z = (targetPos->z + thisv->unk_28C.z) - thisv->actor.world.pos.z;
    func_8002D7EC(&thisv->actor);
    thisv->actor.world.pos.x = targetPos->x + thisv->unk_28C.x;
    thisv->actor.world.pos.z = targetPos->z + thisv->unk_28C.z;
}

void func_80A02EC0(EnElf* thisv, Vec3f* targetPos) {
    func_80A02BD8(thisv, targetPos, 0.2f);
    thisv->actor.velocity.x = thisv->actor.velocity.z = 0.0f;
    func_8002D7EC(&thisv->actor);
    thisv->actor.world.pos.x = targetPos->x + thisv->unk_28C.x;
    thisv->actor.world.pos.z = targetPos->z + thisv->unk_28C.z;
}

void func_80A02F2C(EnElf* thisv, Vec3f* targetPos) {
    f32 yVelTarget;
    f32 yVelDirection;

    yVelTarget = (((Math_SinS(thisv->unk_2AA) * thisv->unk_2B4) + targetPos->y) - thisv->actor.world.pos.y) * 0.2f;
    yVelDirection = (yVelTarget >= 0.0f) ? 1.0f : -1.0f;
    thisv->unk_2AA += thisv->unk_2AE;
    yVelTarget = fabsf(yVelTarget);
    yVelTarget = CLAMP(yVelTarget, 0.0f, 20.0f) * yVelDirection;
    Math_StepToF(&thisv->actor.velocity.y, yVelTarget, 1.5f);
}

void func_80A03018(EnElf* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    Player* player = GET_PLAYER(globalCtx);
    s16 targetYaw;
    Vec3f* unk_28C = &thisv->unk_28C;

    Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->unk_2B8, 0.2f, 0.5f, 0.01f);

    switch (thisv->unk_2A8) {
        case 0:
            targetYaw = Math_Atan2S(-(thisv->actor.world.pos.z - unk_28C->z), -(thisv->actor.world.pos.x - unk_28C->x));
            break;

        case 3:
            targetYaw = Math_Atan2S(-(thisv->actor.world.pos.z - player->actor.world.pos.z),
                                    -(thisv->actor.world.pos.x - player->actor.world.pos.x));
            break;

        case 2:
            targetYaw = Math_Atan2S(thisv->actor.world.pos.z - player->actor.world.pos.z,
                                    thisv->actor.world.pos.x - player->actor.world.pos.x);
            break;

        default:
            targetYaw = thisv->unk_2B0;
            break;
    }

    Math_SmoothStepToS(&thisv->unk_2BC, targetYaw, 10, thisv->unk_2AC, 0x20);
    thisv->actor.world.rot.y = thisv->unk_2BC;
    Actor_MoveForward(&thisv->actor);
}

void func_80A03148(EnElf* thisv, Vec3f* arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 xVelTarget;
    f32 zVelTarget;
    f32 xzVelocity;
    f32 clampedXZ;

    xVelTarget = ((arg1->x + thisv->unk_28C.x) - thisv->actor.world.pos.x) * arg4;
    zVelTarget = ((arg1->z + thisv->unk_28C.z) - thisv->actor.world.pos.z) * arg4;
    arg4 += 0.3f;
    arg3 += 30.0f;

    func_80A02BD8(thisv, arg1, arg4);

    xzVelocity = sqrtf(SQ(xVelTarget) + SQ(zVelTarget));

    thisv->actor.speedXZ = clampedXZ = CLAMP(xzVelocity, arg2, arg3);

    if ((xzVelocity != clampedXZ) && (xzVelocity != 0.0f)) {
        xzVelocity = clampedXZ / xzVelocity;
        xVelTarget *= xzVelocity;
        zVelTarget *= xzVelocity;
    }

    Math_StepToF(&thisv->actor.velocity.x, xVelTarget, 5.0f);
    Math_StepToF(&thisv->actor.velocity.z, zVelTarget, 5.0f);
    func_8002D7EC(&thisv->actor);
}

void func_80A0329C(EnElf* thisv, GlobalContext* globalCtx) {
    Player* refActor = GET_PLAYER(globalCtx);
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    f32 heightDiff;

    SkelAnime_Update(&thisv->skelAnime);

    if (Rand_ZeroOne() < 0.05f) {
        thisv->unk_2B4 = Rand_ZeroFloat(10.0f) + 10.0f;
        thisv->unk_2AE = (s16)(Rand_ZeroFloat(1024.0f)) + 0x200;
    }

    func_80A0232C(thisv, globalCtx);
    thisv->unk_28C.y = player->bodyPartsPos[0].y;
    func_80A02F2C(thisv, &thisv->unk_28C);
    func_80A03018(thisv, globalCtx);

    if ((thisv->unk_2A8 == 2) || (thisv->unk_2A8 == 3)) {
        EnElf_SpawnSparkles(thisv, globalCtx, 16);
    }

    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (!Player_InCsMode(globalCtx)) {
        heightDiff = thisv->actor.world.pos.y - refActor->actor.world.pos.y;

        if ((heightDiff > 0.0f) && (heightDiff < 60.0f)) {
            if (!func_80A01F90(&thisv->actor.world.pos, &refActor->actor.world.pos, 10.0f)) {
                Health_ChangeBy(globalCtx, 128);
                if (thisv->fairyFlags & FAIRY_FLAG_BIG) {
                    Magic_Fill(globalCtx);
                }
                thisv->unk_2B8 = 50.0f;
                thisv->unk_2AC = refActor->actor.shape.rot.y;
                thisv->unk_2B0 = -0x1000;
                thisv->unk_28C.y = 30.0f;
                thisv->unk_2B4 = 0.0f;
                thisv->unk_2AA = 0;
                EnElf_SetupAction(thisv, func_80A03610);
                return;
            }
        }

        if (thisv->fairyFlags & FAIRY_FLAG_TIMED) {
            if (thisv->disappearTimer > 0) {
                thisv->disappearTimer--;
            } else {
                thisv->disappearTimer--;

                if (thisv->disappearTimer > -10) {
                    Actor_SetScale(&thisv->actor, ((thisv->disappearTimer + 10) * 0.008f) * 0.1f);
                } else {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            }
        }

        if (!(thisv->fairyFlags & FAIRY_FLAG_BIG)) {
            // GI_MAX in thisv case allows the player to catch the actor in a bottle
            func_8002F434(&thisv->actor, globalCtx, GI_MAX, 80.0f, 60.0f);
        }
    }
}

void func_80A0353C(EnElf* thisv, GlobalContext* globalCtx) {
    Vec3f parentPos;
    Actor* parent;

    SkelAnime_Update(&thisv->skelAnime);
    func_80A02A20(thisv, globalCtx);
    parent = thisv->actor.parent;

    if ((parent != NULL) && (parent->update != NULL)) {
        parentPos = thisv->actor.parent->world.pos;
        parentPos.y += ((1500.0f * thisv->actor.scale.y) + 40.0f);
        func_80A02C98(thisv, &parentPos, 0.2f);
    } else {
        Actor_Kill(&thisv->actor);
    }

    thisv->unk_2BC = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
}

void func_80A03604(EnElf* thisv, GlobalContext* globalCtx) {
}

void func_80A03610(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToF(&thisv->unk_2B8, 30.0f, 0.1f, 4.0f, 1.0f);

    thisv->unk_28C.x = Math_CosS(thisv->unk_2AC) * thisv->unk_2B8;
    thisv->unk_28C.y = thisv->unk_28C.y + thisv->unk_2B4;

    switch (thisv->unk_2AA) {
        case 0:
            if (thisv->unk_2B4 < 2.0f) {
                thisv->unk_2B4 += 0.1f;
            } else {
                thisv->unk_2AA++;
            }
            break;
        case 1:
            if (thisv->unk_2B4 > -1.0f) {
                thisv->unk_2B4 -= 0.2f;
            }
    }

    thisv->unk_28C.z = Math_SinS(thisv->unk_2AC) * -thisv->unk_2B8;
    thisv->unk_2AC += thisv->unk_2B0;
    func_80A02C98(thisv, &player->actor.world.pos, 0.2f);

    if (thisv->unk_2B4 < 0.0f) {
        if ((thisv->unk_28C.y < 20.0f) && (thisv->unk_28C.y > 0.0f)) {
            Actor_SetScale(&thisv->actor, (thisv->unk_28C.y * 0.008f) * 0.05f);
        }
    }

    if (thisv->unk_28C.y < -10.0f) {
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->unk_2BC = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
    EnElf_SpawnSparkles(thisv, globalCtx, 32);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void func_80A03814(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_28C.y > 200.0f) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->unk_2AE >= 0x7E) {
        thisv->unk_2B8 += 0.1f;
        thisv->unk_2B4 += 0.5f;
        thisv->unk_28C.y += thisv->unk_2B4;
    } else {
        thisv->unk_2AE++;

        if (thisv->unk_2B8 < 30.0f) {
            thisv->unk_2B8 += 0.5f;
        }

        if (thisv->unk_28C.y > 0.0f) {
            thisv->unk_28C.y -= 0.7f;
        }
    }

    thisv->unk_28C.x = Math_CosS(thisv->unk_2AC) * thisv->unk_2B8;
    thisv->unk_28C.z = Math_SinS(thisv->unk_2AC) * -thisv->unk_2B8;
    thisv->unk_2AC += thisv->unk_2B0;
    func_80A02E30(thisv, &player->bodyPartsPos[0]);
    thisv->unk_2BC = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
    EnElf_SpawnSparkles(thisv, globalCtx, 32);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void func_80A03990(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);

    thisv->unk_28C.z = 0.0f;
    thisv->unk_28C.x = 0.0f;
    thisv->unk_28C.y += thisv->unk_2B4;
    thisv->unk_2B4 -= 0.35f;

    if (thisv->unk_2B4 <= 0.0f) {
        EnElf_SetupAction(thisv, func_80A03814);
        thisv->unk_2B0 = 0x800;
        thisv->unk_2AE = 0;
        thisv->unk_2B4 = 0.0f;
        thisv->unk_2B8 = 1.0f;
    }

    func_80A02E30(thisv, &player->bodyPartsPos[0]);
    Actor_SetScale(&thisv->actor, (1.0f - (SQ(thisv->unk_2B4) * SQ(1.0f / 9.0f))) * 0.008f);
    thisv->unk_2BC = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
    EnElf_SpawnSparkles(thisv, globalCtx, 32);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
}

void func_80A03AB0(EnElf* thisv, GlobalContext* globalCtx) {
    if (thisv->fairyFlags & 4) {
        func_80A04414(thisv, globalCtx);
    }

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->func_2C8 == NULL) {
        ASSERT(0, "0", "../z_en_elf.c", 1725);
    }

    thisv->func_2C8(thisv, globalCtx);
}

void EnElf_UpdateLights(EnElf* thisv, GlobalContext* globalCtx) {
    s16 glowLightRadius;
    Player* player;

    glowLightRadius = 100;

    if (thisv->unk_2A8 == 8) {
        glowLightRadius = 0;
    }

    if (thisv->fairyFlags & 0x20) {
        player = GET_PLAYER(globalCtx);
        Lights_PointNoGlowSetInfo(&thisv->lightInfoNoGlow, player->actor.world.pos.x,
                                  (s16)(player->actor.world.pos.y) + 60.0f, player->actor.world.pos.z, 255, 255, 255,
                                  200);
    } else {
        Lights_PointNoGlowSetInfo(&thisv->lightInfoNoGlow, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                  thisv->actor.world.pos.z, 255, 255, 255, -1);
    }

    Lights_PointGlowSetInfo(&thisv->lightInfoGlow, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                            thisv->actor.world.pos.z, 255, 255, 255, glowLightRadius);

    thisv->unk_2BC = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);

    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
}

void func_80A03CF8(EnElf* thisv, GlobalContext* globalCtx) {
    Vec3f nextPos;
    Vec3f prevPos;
    Player* player = GET_PLAYER(globalCtx);
    Actor* arrowPointedActor;
    f32 xScale;
    f32 distFromLinksHead;

    func_80A0461C(thisv, globalCtx);
    func_80A03AB0(thisv, globalCtx);

    xScale = 0.0f;

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[8] != NULL)) {
        EnElf_GetCutsceneNextPos(&nextPos, globalCtx, 8);

        if (globalCtx->csCtx.npcActions[8]->action == 5) {
            if (1) {}
            EnElf_SpawnSparkles(thisv, globalCtx, 16);
        }

        prevPos = thisv->actor.world.pos;

        if (thisv->unk_2A8 == 0xA) {
            func_80A02EC0(thisv, &nextPos);
        } else {
            func_80A02C98(thisv, &nextPos, 0.2f);
        }

        if ((globalCtx->sceneNum == SCENE_LINK_HOME) && (gSaveContext.sceneSetupIndex == 4)) {
            // play dash sound as Navi enters Links house in the intro
            if (1) {}
            if (globalCtx->csCtx.frames == 55) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FAIRY_DASH);
            }

            // play dash sound in intervals as Navi is waking up Link in the intro
            if (thisv->unk_2A8 == 6) {
                if (thisv->fairyFlags & 0x40) {
                    if (prevPos.y < thisv->actor.world.pos.y) {
                        thisv->fairyFlags &= ~0x40;
                    }
                } else {
                    if (thisv->actor.world.pos.y < prevPos.y) {
                        thisv->fairyFlags |= 0x40;
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FAIRY_DASH);
                    }
                }
            }
        }
    } else {
        distFromLinksHead = Math_Vec3f_DistXYZ(&player->bodyPartsPos[8], &thisv->actor.world.pos);

        switch (thisv->unk_2A8) {
            case 7:
                func_80A02C98(thisv, &player->bodyPartsPos[8], 1.0f - thisv->unk_2AE * (1.0f / 30.0f));
                xScale = Math_Vec3f_DistXYZ(&player->bodyPartsPos[8], &thisv->actor.world.pos);

                if (distFromLinksHead < 7.0f) {
                    thisv->unk_2C0 = 0;
                    xScale = 0.0f;
                } else if (distFromLinksHead < 25.0f) {
                    xScale = (xScale - 5.0f) * 0.05f;
                    xScale = 1.0f - xScale;
                    xScale = (1.0f - SQ(xScale)) * 0.008f;
                } else {
                    xScale = 0.008f;
                }
                EnElf_SpawnSparkles(thisv, globalCtx, 16);
                break;
            case 8:
                func_80A02C98(thisv, &player->bodyPartsPos[8], 0.2f);
                thisv->actor.world.pos = player->bodyPartsPos[8];
                func_80A029A8(thisv, 1);
                break;
            case 11:
                nextPos = player->bodyPartsPos[8];
                nextPos.y += 1500.0f * thisv->actor.scale.y;
                func_80A02E30(thisv, &nextPos);
                EnElf_SpawnSparkles(thisv, globalCtx, 16);

                if (thisv->unk_2B8 <= 19.0f) {
                    thisv->unk_2B8 += 1.0f;
                }

                if (thisv->unk_2B8 >= 21.0f) {
                    thisv->unk_2B8 -= 1.0f;
                }

                if (thisv->unk_2C0 < 0x20) {
                    thisv->unk_2B0 = (thisv->unk_2C0 * 0xF0) + 0x200;
                    func_80A0299C(thisv, 1);
                }
                break;
            case 12:
                nextPos = GET_ACTIVE_CAM(globalCtx)->eye;
                nextPos.y += (-2000.0f * thisv->actor.scale.y);
                func_80A03148(thisv, &nextPos, 0.0f, 20.0f, 0.2f);
                break;
            default:
                func_80A029A8(thisv, 1);
                nextPos = globalCtx->actorCtx.targetCtx.naviRefPos;
                nextPos.y += (1500.0f * thisv->actor.scale.y);
                arrowPointedActor = globalCtx->actorCtx.targetCtx.arrowPointedActor;

                if (arrowPointedActor != NULL) {
                    func_80A03148(thisv, &nextPos, 0.0f, 20.0f, 0.2f);

                    if (thisv->actor.speedXZ >= 5.0f) {
                        EnElf_SpawnSparkles(thisv, globalCtx, 16);
                    }
                } else {
                    if ((thisv->timer % 32) == 0) {
                        thisv->unk_2A0 = Rand_ZeroFloat(7.0f) + 3.0f;
                    }

                    if (thisv->fairyFlags & 2) {
                        if (distFromLinksHead < 30.0f) {
                            thisv->fairyFlags ^= 2;
                        }

                        func_80A03148(thisv, &nextPos, 0.0f, 20.0f, 0.2f);
                        EnElf_SpawnSparkles(thisv, globalCtx, 16);
                    } else {
                        if (distFromLinksHead > 100.0f) {
                            thisv->fairyFlags |= 2;

                            if (thisv->unk_2C7 == 0) {
                                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FAIRY_DASH);
                            }

                            thisv->unk_2C0 = 0x64;
                        }
                        func_80A03148(thisv, &nextPos, 0.0f, thisv->unk_2A0, 0.2f);
                    }
                }
                break;
        }
    }

    if (thisv->unk_2A8 == 7) {
        thisv->actor.scale.x = xScale;
    } else if (thisv->unk_2A8 == 8) {
        thisv->actor.scale.x = 0.0f;
    } else {
        Math_SmoothStepToF(&thisv->actor.scale.x, 0.008f, 0.3f, 0.00080000004f, 0.000080000005f);
    }

    EnElf_UpdateLights(thisv, globalCtx);
}

void EnElf_ChangeColor(Color_RGBAf* dest, Color_RGBAf* newColor, Color_RGBAf* curColor, f32 rate) {
    Color_RGBAf rgbaDiff;

    rgbaDiff.r = (newColor->r - curColor->r);
    rgbaDiff.g = (newColor->g - curColor->g);
    rgbaDiff.b = (newColor->b - curColor->b);
    rgbaDiff.a = (newColor->a - curColor->a);

    dest->r += (rgbaDiff.r * rate);
    dest->g += (rgbaDiff.g * rate);
    dest->b += (rgbaDiff.b * rate);
    dest->a += (rgbaDiff.a * rate);
}

void func_80A04414(EnElf* thisv, GlobalContext* globalCtx) {
    Actor* arrowPointedActor = globalCtx->actorCtx.targetCtx.arrowPointedActor;
    Player* player = GET_PLAYER(globalCtx);
    f32 transitionRate;
    u16 targetSound;

    if (globalCtx->actorCtx.targetCtx.unk_40 != 0.0f) {
        thisv->unk_2C6 = 0;
        thisv->unk_29C = 1.0f;

        if (thisv->unk_2C7 == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FAIRY_DASH);
        }

    } else {
        if (thisv->unk_2C6 == 0) {
            if ((arrowPointedActor == NULL) ||
                (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &globalCtx->actorCtx.targetCtx.naviRefPos) < 50.0f)) {
                thisv->unk_2C6 = 1;
            }
        } else if (thisv->unk_29C != 0.0f) {
            if (Math_StepToF(&thisv->unk_29C, 0.0f, 0.25f) != 0) {
                thisv->innerColor = globalCtx->actorCtx.targetCtx.naviInner;
                thisv->outerColor = globalCtx->actorCtx.targetCtx.naviOuter;
            } else {
                transitionRate = 0.25f / thisv->unk_29C;
                EnElf_ChangeColor(&thisv->innerColor, &globalCtx->actorCtx.targetCtx.naviInner, &thisv->innerColor,
                                  transitionRate);
                EnElf_ChangeColor(&thisv->outerColor, &globalCtx->actorCtx.targetCtx.naviOuter, &thisv->outerColor,
                                  transitionRate);
            }
        }
    }

    if (thisv->fairyFlags & 1) {
        if ((arrowPointedActor == NULL) || (player->unk_664 == NULL)) {
            thisv->fairyFlags ^= 1;
        }
    } else {
        if ((arrowPointedActor != NULL) && (player->unk_664 != NULL)) {
            if (arrowPointedActor->category == ACTORCAT_NPC) {
                targetSound = NA_SE_VO_NAVY_HELLO;
            } else {
                targetSound =
                    (arrowPointedActor->category == ACTORCAT_ENEMY) ? NA_SE_VO_NAVY_ENEMY : NA_SE_VO_NAVY_HEAR;
            }

            if (thisv->unk_2C7 == 0) {
                Audio_PlayActorSound2(&thisv->actor, targetSound);
            }

            thisv->fairyFlags |= 1;
        }
    }
}

void func_80A0461C(EnElf* thisv, GlobalContext* globalCtx) {
    s32 temp;
    Actor* arrowPointedActor;
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if (globalCtx->csCtx.npcActions[8] != NULL) {
            switch (globalCtx->csCtx.npcActions[8]->action) {
                case 4:
                    temp = 9;
                    break;
                case 3:
                    temp = 6;
                    break;
                case 1:
                    temp = 10;
                    break;
                default:
                    temp = 0;
                    break;
            }
        } else {
            temp = 0;
            thisv->unk_2C0 = 100;
        }

    } else {
        arrowPointedActor = globalCtx->actorCtx.targetCtx.arrowPointedActor;

        if ((player->stateFlags1 & 0x400) || ((YREG(15) & 0x10) && func_800BC56C(globalCtx, 2))) {
            temp = 12;
            thisv->unk_2C0 = 100;
        } else if (arrowPointedActor == NULL || arrowPointedActor->category == ACTORCAT_NPC) {
            if (arrowPointedActor != NULL) {
                thisv->unk_2C0 = 100;
                player->stateFlags2 |= 0x100000;
                temp = 0;
            } else {
                switch (thisv->unk_2A8) {
                    case 0:
                        if (thisv->unk_2C0 != 0) {
                            thisv->unk_2C0--;
                            temp = 0;
                        } else {
                            if (thisv->unk_2C7 == 0) {
                                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_NAVY_VANISH);
                            }
                            temp = 7;
                        }
                        break;
                    case 7:
                        if (thisv->unk_2C0 != 0) {
                            if (thisv->unk_2AE > 0) {
                                thisv->unk_2AE--;
                                temp = 7;
                            } else {
                                player->stateFlags2 |= 0x100000;
                                temp = 0;
                            }
                        } else {
                            temp = 8;
                            func_80A029A8(thisv, 10);
                        }
                        break;
                    case 8:
                        temp = 8;
                        break;
                    case 11:
                        temp = thisv->unk_2A8;
                        if (thisv->unk_2C0 > 0) {
                            thisv->unk_2C0--;
                        } else {
                            temp = 0;
                        }
                        break;
                    default:
                        temp = 0;
                        break;
                }
            }
        } else {
            temp = 1;
        }

        switch (temp) {
            case 0:
                if (!(player->stateFlags2 & 0x100000)) {
                    temp = 7;
                    if (thisv->unk_2C7 == 0) {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_NAVY_VANISH);
                    }
                }
                break;
            case 8:
                if (player->stateFlags2 & 0x100000) {
                    func_80A0299C(thisv, 0x32);
                    thisv->unk_2C0 = 42;
                    temp = 11;
                    if (thisv->unk_2C7 == 0) {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FAIRY_DASH);
                    }
                }
                break;
            case 7:
                player->stateFlags2 &= ~0x100000;
                break;
            default:
                player->stateFlags2 |= 0x100000;
                break;
        }
    }

    if (temp != thisv->unk_2A8) {
        func_80A01C38(thisv, temp);

        if (temp == 11) {
            thisv->unk_2B8 = Math_Vec3f_DistXZ(&player->bodyPartsPos[8], &thisv->actor.world.pos);
            thisv->unk_2AC = Math_Vec3f_Yaw(&thisv->actor.world.pos, &player->bodyPartsPos[8]);
        }
    }
}

void EnElf_SpawnSparkles(EnElf* thisv, GlobalContext* globalCtx, s32 sparkleLife) {
    static Vec3f sparkleVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f sparkleAccel = { 0.0f, -0.025f, 0.0f };
    s32 pad;
    Vec3f sparklePos;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    sparklePos.x = Rand_CenteredFloat(6.0f) + thisv->actor.world.pos.x;
    sparklePos.y = (Rand_ZeroOne() * 6.0f) + thisv->actor.world.pos.y;
    sparklePos.z = Rand_CenteredFloat(6.0f) + thisv->actor.world.pos.z;

    primColor.r = thisv->innerColor.r;
    primColor.g = thisv->innerColor.g;
    primColor.b = thisv->innerColor.b;

    envColor.r = thisv->outerColor.r;
    envColor.g = thisv->outerColor.g;
    envColor.b = thisv->outerColor.b;

    EffectSsKiraKira_SpawnDispersed(globalCtx, &sparklePos, &sparkleVelocity, &sparkleAccel, &primColor, &envColor,
                                    1000, sparkleLife);
}

void func_80A04D90(EnElf* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 bgId;

    thisv->actor.floorHeight = BgCheck_EntityRaycastFloor5(globalCtx, &globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                          &thisv->actor, &thisv->actor.world.pos);
    thisv->actor.shape.shadowAlpha = 0x32;
}

// move to talk to player
void func_80A04DE4(EnElf* thisv, GlobalContext* globalCtx) {
    Vec3f headCopy;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f naviRefPos;

    if (thisv->fairyFlags & 0x10) {
        naviRefPos = globalCtx->actorCtx.targetCtx.naviRefPos;

        if ((player->unk_664 == NULL) || (&player->actor == player->unk_664) || (&thisv->actor == player->unk_664)) {
            naviRefPos.x = player->bodyPartsPos[7].x + (Math_SinS(player->actor.shape.rot.y) * 20.0f);
            naviRefPos.y = player->bodyPartsPos[7].y + 5.0f;
            naviRefPos.z = player->bodyPartsPos[7].z + (Math_CosS(player->actor.shape.rot.y) * 20.0f);
        }

        thisv->actor.focus.pos = naviRefPos;
        thisv->fairyFlags &= ~0x10;
    }

    func_80A03AB0(thisv, globalCtx);
    headCopy = thisv->actor.focus.pos;

    func_80A03148(thisv, &headCopy, 0, 20.0f, 0.2f);

    if (thisv->actor.speedXZ >= 5.0f) {
        EnElf_SpawnSparkles(thisv, globalCtx, 16);
    }

    Math_SmoothStepToF(&thisv->actor.scale.x, 0.008f, 0.3f, 0.00080000004f, 0.000080000005f);
    EnElf_UpdateLights(thisv, globalCtx);
}

// move after talking to player
void func_80A04F94(EnElf* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->unk_2BC, 5, 0x1000, 0x400);
    thisv->timer++;
    Math_StepToF(&thisv->unk_2A4, 1.0f, 0.05f);
    Environment_AdjustLights(globalCtx, SQ(thisv->unk_2A4), player->actor.projectedPos.z + 780.0f, 0.2f, 0.5f);
}

// ask to talk to saria again
void func_80A05040(Actor* thisx, GlobalContext* globalCtx) {
    EnElf* thisv = (EnElf*)thisx;

    func_80A04DE4(thisv, globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                Message_ContinueTextbox(globalCtx, ElfMessage_GetSariaText(globalCtx));
                thisv->actor.update = func_80A05114;
                break;
            case 1: // no
                Message_CloseTextbox(globalCtx);
                thisv->actor.update = func_80A053F0;
                func_80A01C38(thisv, 0);
                thisv->fairyFlags &= ~0x20;
                break;
        }
    }

    func_80A04F94(thisv, globalCtx);
}

void func_80A05114(Actor* thisx, GlobalContext* globalCtx) {
    EnElf* thisv = (EnElf*)thisx;

    func_80A04DE4(thisv, globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, 0xE3);
        thisv->actor.update = func_80A05040;
    }

    func_80A04F94(thisv, globalCtx);
}

void func_80A05188(Actor* thisx, GlobalContext* globalCtx) {
    EnElf* thisv = (EnElf*)thisx;

    func_80A04DE4(thisv, globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_ContinueTextbox(globalCtx, ElfMessage_GetSariaText(globalCtx));
        thisv->actor.update = func_80A05114;
    }

    func_80A04F94(thisv, globalCtx);
}

// ask to talk to navi
void func_80A05208(Actor* thisx, GlobalContext* globalCtx) {
    s32 naviCUpText;
    EnElf* thisv = (EnElf*)thisx;

    func_80A04DE4(thisv, globalCtx);

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0: // yes
                naviCUpText = ElfMessage_GetCUpText(globalCtx);

                if (naviCUpText != 0) {
                    Message_ContinueTextbox(globalCtx, naviCUpText);
                } else {
                    Message_ContinueTextbox(globalCtx, 0x15F);
                }

                thisv->actor.update = func_80A052F4;
                break;
            case 1: // no
                Message_CloseTextbox(globalCtx);
                thisv->actor.update = func_80A053F0;
                func_80A01C38(thisv, 0);
                thisv->fairyFlags &= ~0x20;
                break;
        }
    }

    func_80A04F94(thisv, globalCtx);
}

// ask to talk to saria
void func_80A052F4(Actor* thisx, GlobalContext* globalCtx) {
    EnElf* thisv = (EnElf*)thisx;

    func_80A04DE4(thisv, globalCtx);

    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(globalCtx)) {
            globalCtx->msgCtx.unk_E3F2 = 0xFF;

            switch (globalCtx->msgCtx.choiceIndex) {
                case 0: // yes
                    thisv->actor.update = func_80A05188;
                    Message_ContinueTextbox(globalCtx, 0xE2);
                    break;
                case 1: // no
                    thisv->actor.update = func_80A05208;
                    Message_ContinueTextbox(globalCtx, 0xE1);
                    break;
            }
        }
    } else if (Actor_TextboxIsClosing(thisx, globalCtx)) {
        thisv->actor.update = func_80A053F0;
        func_80A01C38(thisv, 0);
        thisv->fairyFlags &= ~0x20;
    }

    func_80A04F94(thisv, globalCtx);
}

void func_80A053F0(Actor* thisx, GlobalContext* globalCtx) {
    u8 unk2C7;
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    EnElf* thisv = (EnElf*)thisx;

    if (player->naviTextId == 0) {
        if (player->unk_664 == NULL) {
            if (((gSaveContext.naviTimer >= 600) && (gSaveContext.naviTimer <= 3000)) || (nREG(89) != 0)) {
                player->naviTextId = ElfMessage_GetCUpText(globalCtx);

                if (player->naviTextId == 0x15F) {
                    player->naviTextId = 0;
                }
            }
        }
    } else if (player->naviTextId < 0) {
        // trigger dialog instantly for negative message IDs
        thisx->flags |= ACTOR_FLAG_16;
    }

    if (Actor_ProcessTalkRequest(thisx, globalCtx)) {
        func_800F4524(&D_801333D4, NA_SE_VO_SK_LAUGH, 0x20);
        thisx->focus.pos = thisx->world.pos;

        if (thisx->textId == ElfMessage_GetCUpText(globalCtx)) {
            thisv->fairyFlags |= 0x80;
            gSaveContext.naviTimer = 3001;
        }

        thisv->fairyFlags |= 0x10;
        thisv->fairyFlags |= 0x20;
        thisx->update = func_80A052F4;
        func_80A01C38(thisv, 3);

        if (thisv->elfMsg != NULL) {
            thisv->elfMsg->actor.flags |= ACTOR_FLAG_8;
        }

        thisx->flags &= ~ACTOR_FLAG_16;
    } else {
        thisv->actionFunc(thisv, globalCtx);
        thisx->shape.rot.y = thisv->unk_2BC;
        nREG(80) = gSaveContext.sceneFlags[127].chest;

        if (nREG(81) != 0) {
            if (gSaveContext.sceneFlags[127].chest) {
                LOG_NUM("z_common_data.memory.information.room_inf[127][ 0 ]", gSaveContext.sceneFlags[127].chest,
                        "../z_en_elf.c", 2595);
            }
        }

        if (!Gameplay_InCsMode(globalCtx)) {
            if (gSaveContext.naviTimer < 25800) {
                gSaveContext.naviTimer++;
            } else if (!(thisv->fairyFlags & 0x80)) {
                gSaveContext.naviTimer = 0;
            }
        }
    }

    thisv->elfMsg = NULL;
    thisv->timer++;

    if (thisv->unk_2A4 > 0.0f) {
        Math_StepToF(&thisv->unk_2A4, 0.0f, 0.05f);
        Environment_AdjustLights(globalCtx, SQ(thisv->unk_2A4) * thisv->unk_2A4, player->actor.projectedPos.z + 780.0f,
                                 0.2f, 0.5f);
    }

    // temp probably fake match
    unk2C7 = thisv->unk_2C7;
    if (unk2C7 > 0) {
        thisv->unk_2C7--;
    }

    if ((thisv->unk_2C7 == 0) && (globalCtx->csCtx.state != CS_STATE_IDLE)) {
        thisv->unk_2C7 = 1;
    }

    func_80A04D90(thisv, globalCtx);
}

void EnElf_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnElf* thisv = (EnElf*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.shape.rot.y = thisv->unk_2BC;
    thisv->timer++;

    if (thisv->fairyFlags & FAIRY_FLAG_BIG) {
        func_80A04D90(thisv, globalCtx);
    }
}

s32 EnElf_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    f32 scale;
    Vec3f mtxMult;
    EnElf* thisv = (EnElf*)thisx;

    if (limbIndex == 8) {
        scale = ((Math_SinS(thisv->timer * 4096) * 0.1f) + 1.0f) * 0.012f;

        if (thisv->fairyFlags & FAIRY_FLAG_BIG) {
            scale *= 2.0f;
        }

        scale *= (thisv->actor.scale.x * 124.99999f);
        Matrix_MultVec3f(&zeroVec, &mtxMult);
        Matrix_Translate(mtxMult.x, mtxMult.y, mtxMult.z, MTXMODE_NEW);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    }

    // do not draw wings for big fairies
    if (thisv->fairyFlags & FAIRY_FLAG_BIG) {
        if (limbIndex == 4 || limbIndex == 7 || limbIndex == 11 || limbIndex == 14) {
            *dList = NULL;
        }
    }

    return false;
}

void EnElf_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    f32 alphaScale;
    s32 envAlpha;
    EnElf* thisv = (EnElf*)thisx;
    s32 pad1;
    Gfx* dListHead;
    Player* player = GET_PLAYER(globalCtx);

    if ((thisv->unk_2A8 != 8) && !(thisv->fairyFlags & 8)) {
        if (!(player->stateFlags1 & 0x100000) || (kREG(90) < thisv->actor.projectedPos.z)) {
            dListHead = static_cast<Gfx*>(Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Gfx) * 4));

            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_elf.c", 2730);

            func_80094B58(globalCtx->state.gfxCtx);

            envAlpha = (thisv->timer * 50) & 0x1FF;
            envAlpha = (envAlpha > 255) ? 511 - envAlpha : envAlpha;

            alphaScale = thisv->disappearTimer < 0 ? (thisv->disappearTimer * (7.0f / 6000.0f)) + 1.0f : 1.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08, dListHead);
            gDPPipeSync(dListHead++);
            gDPSetPrimColor(dListHead++, 0, 0x01, (u8)thisv->innerColor.r, (u8)thisv->innerColor.g,
                            (u8)thisv->innerColor.b, (u8)(thisv->innerColor.a * alphaScale));

            if (thisv->fairyFlags & 4) {
                gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_CLD_SURF2);
            } else {
                gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
            }

            gSPEndDisplayList(dListHead++);
            gDPSetEnvColor(POLY_XLU_DISP++, (u8)thisv->outerColor.r, (u8)thisv->outerColor.g, (u8)thisv->outerColor.b,
                           (u8)(envAlpha * alphaScale));
            POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           EnElf_OverrideLimbDraw, NULL, thisv, POLY_XLU_DISP);

            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_elf.c", 2793);
        }
    }
}

void EnElf_GetCutsceneNextPos(Vec3f* vec, GlobalContext* globalCtx, s32 action) {
    Vec3f startPos;
    Vec3f endPos;
    const CsCmdActorAction* npcAction = globalCtx->csCtx.npcActions[action];
    f32 lerp;

    startPos.x = npcAction->startPos.x;
    startPos.y = npcAction->startPos.y;
    startPos.z = npcAction->startPos.z;

    endPos.x = npcAction->endPos.x;
    endPos.y = npcAction->endPos.y;
    endPos.z = npcAction->endPos.z;

    lerp = Environment_LerpWeight(npcAction->endFrame, npcAction->startFrame, globalCtx->csCtx.frames);

    vec->x = ((endPos.x - startPos.x) * lerp) + startPos.x;
    vec->y = ((endPos.y - startPos.y) * lerp) + startPos.y;
    vec->z = ((endPos.z - startPos.z) * lerp) + startPos.z;
}
