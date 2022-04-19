/*
 * File: z_bg_jya_zurerukabe.c
 * Overlay: ovl_Bg_Jya_Zurerukabe
 * Description: Sliding, Climbable Brick Wall
 */

#include "z_bg_jya_zurerukabe.h"
#include "objects/object_jya_obj/object_jya_obj.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaZurerukabe_Init(Actor* thisx, GlobalContext* globalCtx);
void BgJyaZurerukabe_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgJyaZurerukabe_Update(Actor* thisx, GlobalContext* globalCtx);
void BgJyaZurerukabe_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8089B4C8(BgJyaZurerukabe* thisv, GlobalContext* globalCtx);
void func_8089B7B4(BgJyaZurerukabe* thisv);
void func_8089B7C4(BgJyaZurerukabe* thisv, GlobalContext* globalCtx);
void func_8089B80C(BgJyaZurerukabe* thisv);
void func_8089B870(BgJyaZurerukabe* thisv, GlobalContext* globalCtx);

static f32 D_8089B9C0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

ActorInit Bg_Jya_Zurerukabe_InitVars = {
    ACTOR_BG_JYA_ZURERUKABE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaZurerukabe),
    (ActorFunc)BgJyaZurerukabe_Init,
    (ActorFunc)BgJyaZurerukabe_Destroy,
    (ActorFunc)BgJyaZurerukabe_Update,
    (ActorFunc)BgJyaZurerukabe_Draw,
    NULL,
};

static s16 D_8089B9F0[4] = { 943, 1043, 1243, 1343 };

static s16 D_8089B9F8[4] = { -1, 1, -1, 1 };

static s16 D_8089BA00[4] = { 48, 48, 36, 36 };

static f32 D_8089BA08[4] = { 8.0f, 8.0f, 10.0f, 10.0f };

static s16 D_8089BA18[6][2] = {
    { 0x0388, 0x0395 }, { 0x03EA, 0x03FF }, { 0x0454, 0x0467 },
    { 0x04B4, 0x04C1 }, { 0x0518, 0x0528 }, { 0x0581, 0x0590 },
};

static s16 D_8089BA30[6] = {
    0, 0, 1, 2, 2, 3,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaZurerukabe_InitDynaPoly(BgJyaZurerukabe* thisv, GlobalContext* globalCtx, const CollisionHeader* collision,
                                  s32 flag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_zurerukabe.c", 194,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void func_8089B4C8(BgJyaZurerukabe* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((player->stateFlags1 == 0x200000) && (player->actor.wallPoly != NULL)) {
        s32 i;

        for (i = 0; i < ARRAY_COUNT(D_8089BA18); i++) {
            f32 posY = player->actor.world.pos.y;

            if ((posY >= D_8089BA18[i][0]) && (posY <= D_8089BA18[i][1])) {
                break;
            }
        }

        switch (i) {
            case 0:
            case 2:
            case 3:
            case 5:
                if (fabsf(D_8089B9C0[D_8089BA30[i]]) > 1.0f) {
                    func_8002F6D4(globalCtx, &thisv->dyna.actor, 1.5f, thisv->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
            case 1:
            case 4:
                if (fabsf(D_8089B9C0[D_8089BA30[i]] - D_8089B9C0[D_8089BA30[i + 1]]) > 1.0f) {
                    func_8002F6D4(globalCtx, &thisv->dyna.actor, 1.5f, thisv->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
        }
    }
}

void BgJyaZurerukabe_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaZurerukabe* thisv = (BgJyaZurerukabe*)thisx;
    s32 i;

    BgJyaZurerukabe_InitDynaPoly(thisv, globalCtx, &gZurerukabeCol, DPM_UNK);
    Actor_ProcessInitChain(thisx, sInitChain);

    for (i = 0; i < ARRAY_COUNT(D_8089B9F0); i++) {
        if (fabsf(D_8089B9F0[i] - thisv->dyna.actor.home.pos.y) < 1.0f) {
            thisv->unk_168 = i;
            break;
        }
    }

    if (i == ARRAY_COUNT(D_8089B9F0)) {
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("home pos が変更されたみたい(%s %d)(arg_data 0x%04x)\n", "../z_bg_jya_zurerukabe.c", 299,
                     thisv->dyna.actor.params);
        osSyncPrintf(VT_RST);
    }

    thisv->unk_16E = D_8089B9F8[thisv->unk_168];
    func_8089B7B4(thisv);
    osSyncPrintf("(jya ずれる壁)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void BgJyaZurerukabe_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaZurerukabe* thisv = (BgJyaZurerukabe*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    D_8089B9C0[thisv->unk_168] = 0.0f;
}

void func_8089B7B4(BgJyaZurerukabe* thisv) {
    thisv->actionFunc = func_8089B7C4;
}

void func_8089B7C4(BgJyaZurerukabe* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_16A <= 0) {
        func_8089B80C(thisv);
    }
    D_8089B9C0[thisv->unk_168] = 0.0f;
}

void func_8089B80C(BgJyaZurerukabe* thisv) {
    thisv->actionFunc = func_8089B870;
    thisv->unk_16A = D_8089BA00[thisv->unk_168];
    if (ABS(thisv->unk_16C) == 4) {
        thisv->unk_16E = -thisv->unk_16E;
    }
    thisv->unk_16C += thisv->unk_16E;
}

void func_8089B870(BgJyaZurerukabe* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.x, thisv->dyna.actor.home.pos.x + (thisv->unk_16C * 75),
                     D_8089BA08[thisv->unk_168])) {
        func_8089B7B4(thisv);
    }

    D_8089B9C0[thisv->unk_168] = D_8089BA08[thisv->unk_168] * thisv->unk_16E;
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void BgJyaZurerukabe_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgJyaZurerukabe* thisv = (BgJyaZurerukabe*)thisx;

    if (thisv->unk_16A > 0) {
        thisv->unk_16A--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->unk_168 == 0) {
        func_8089B4C8(thisv, globalCtx);
    }
}

void BgJyaZurerukabe_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gZurerukabeDL);
}
