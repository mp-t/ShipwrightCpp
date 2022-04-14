/*
 * File: z_en_gb.c
 * Overlay: ovl_En_Gb
 * Description: Poe Seller
 */

#include "z_en_gb.h"
#include "objects/object_ps/object_ps.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnGb_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGb_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGb_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGb_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A2F83C(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2FC70(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2FA50(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2F9C0(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2F94C(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2FB40(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2FBB0(EnGb* thisv, GlobalContext* globalCtx);
void func_80A2FC0C(EnGb* thisv, GlobalContext* globalCtx);

void EnGb_DrawCagedSouls(EnGb* thisv, GlobalContext* globalCtx);
void EnGb_UpdateCagedSouls(EnGb* thisv, GlobalContext* globalCtx);

const ActorInit En_Gb_InitVars = {
    ACTOR_EN_GB,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_PS,
    sizeof(EnGb),
    (ActorFunc)EnGb_Init,
    (ActorFunc)EnGb_Destroy,
    (ActorFunc)EnGb_Update,
    (ActorFunc)EnGb_Draw,
    NULL,
};

static EnGbCagedSoulInfo sCagedSoulInfo[] = {
    { { 255, 255, 170, 255 }, { 255, 200, 0, 255 }, gPoeSellerAngrySoulTex, -15 },
    { { 255, 255, 170, 255 }, { 0, 150, 0, 255 }, gPoeSellerHappySoulTex, -12 },
    { { 255, 170, 255, 255 }, { 100, 0, 150, 255 }, gPoeSellerSadSoulTex, -8 },
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 40, 75, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sBottlesCylindersInit[] = {
    {
        {
            COLTYPE_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 4, 20, 0, { 0, 0, 0 } },
    },
    {
        {
            COLTYPE_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 4, 20, 0, { 0, 0, 0 } },
    },
    {
        {
            COLTYPE_NONE,
            AT_NONE,
            AC_NONE,
            OC1_ON | OC1_TYPE_ALL,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 10, 20, 0, { 0, 0, 0 } },
    },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 6, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2200, ICHAIN_STOP),
};

// relative positions of poe souls
static Vec3f sCagedSoulPositions[] = {
    { -8.0f, 112.0f, -8.0f },
    { -3.0f, 112.0f, 29.0f },
    { 31.0f, 112.0f, 29.0f },
    { 31.0f, 112.0f, -8.0f },
};

// relative positions of bottles
static Vec3f sBottlesPositions[] = {
    { -48.0f, 0.0f, 34.0f },
    { -55.0f, 0.0f, 49.0f },
    { -48.0f, 0.0f, 60.0f },
};

void func_80A2F180(EnGb* thisv) {
    if (gSaveContext.infTable[0xB] & 0x40) {
        thisv->textId = 0x70F5;
    } else {
        thisv->textId = 0x70F4;
    }
}

void EnGb_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGb* thisv = (EnGb*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 i;
    f32 rand;
    Vec3f focusOffset;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gPoeSellerCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gPoeSellerSkel, &gPoeSellerIdleAnim, thisv->jointTable,
                       thisv->morphTable, 12);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->dyna.actor, &sCylinderInit);

    for (i = 0; i < ARRAY_COUNT(sBottlesCylindersInit); i++) {
        Collider_InitCylinder(globalCtx, &thisv->bottlesColliders[i]);
        Collider_SetCylinderType1(globalCtx, &thisv->bottlesColliders[i], &thisv->dyna.actor, &sBottlesCylindersInit[i]);
    }

    thisv->light = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->dyna.actor.home.pos.x, thisv->dyna.actor.home.pos.y,
                              thisv->dyna.actor.home.pos.z, 255, 255, 255, 200);

    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&thisv->dyna.actor, 0.01f);
    thisv->dyna.actor.colChkInfo.mass = 0xFF;
    thisv->dyna.actor.speedXZ = 0.0f;
    thisv->dyna.actor.velocity.y = 0.0f;
    thisv->dyna.actor.gravity = -1.0f;
    thisv->actionTimer = (s16)Rand_ZeroFloat(100.0f) + 100;

    for (i = 0; i < ARRAY_COUNT(sCagedSoulPositions); i++) {
        thisv->cagedSouls[i].infoIdx = (s32)Rand_ZeroFloat(30.0f) % 3;
        thisv->cagedSouls[i].unk_14.x = thisv->cagedSouls[i].translation.x =
            sCagedSoulPositions[i].x + thisv->dyna.actor.world.pos.x;
        thisv->cagedSouls[i].unk_14.y = thisv->cagedSouls[i].translation.y =
            sCagedSoulPositions[i].y + thisv->dyna.actor.world.pos.y;
        thisv->cagedSouls[i].unk_14.z = thisv->cagedSouls[i].translation.z =
            sCagedSoulPositions[i].z + thisv->dyna.actor.world.pos.z;
        thisv->cagedSouls[i].unk_1 = 1;
        thisv->cagedSouls[i].unk_3 = thisv->cagedSouls[i].unk_2 = 0;
        thisv->cagedSouls[i].unk_20 = thisv->cagedSouls[i].unk_24 = 0.0f;
        thisv->cagedSouls[i].unk_6 = Rand_ZeroFloat(40.0f);
        thisv->cagedSouls[i].rotate180 = thisv->cagedSouls[i].unk_6 & 1;
        thisv->cagedSouls[i].unk_28 = 0.2f;
    }

    rand = Rand_ZeroOne();
    thisv->lightColor.r = (s8)(rand * 30.0f) + 225;
    thisv->lightColor.g = (s8)(rand * 100.0f) + 155;
    thisv->lightColor.b = (s8)(rand * 160.0f) + 95;
    thisv->lightColor.a = 200;
    Matrix_Translate(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateZYX(thisv->dyna.actor.world.rot.x, thisv->dyna.actor.world.rot.y, thisv->dyna.actor.world.rot.z,
                     MTXMODE_APPLY);
    focusOffset.x = focusOffset.y = 0.0f;
    focusOffset.z = 44.0f;
    Matrix_MultVec3f(&focusOffset, &thisv->dyna.actor.focus.pos);
    thisv->dyna.actor.focus.pos.y += 62.5f;
    func_80A2F180(thisv);
    thisv->actionFunc = func_80A2F83C;
}

void EnGb_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGb* thisv = (EnGb*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->light);
    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80A2F608(EnGb* thisv) {
    s32 i;
    Vec3f sp48;
    Vec3f sp3C;

    Matrix_Translate(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateZYX(thisv->dyna.actor.world.rot.x, thisv->dyna.actor.world.rot.y, thisv->dyna.actor.world.rot.z,
                     MTXMODE_APPLY);
    sp48.x = sp48.y = 0.0f;
    sp48.z = 25.0f;
    Matrix_MultVec3f(&sp48, &sp3C);
    thisv->collider.dim.pos.x = sp3C.x;
    thisv->collider.dim.pos.y = sp3C.y;
    thisv->collider.dim.pos.z = sp3C.z;

    for (i = 0; i < ARRAY_COUNT(sBottlesPositions); i++) {
        Matrix_Translate(thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_RotateZYX(thisv->dyna.actor.world.rot.x, thisv->dyna.actor.world.rot.y, thisv->dyna.actor.world.rot.z,
                         MTXMODE_APPLY);
        Matrix_MultVec3f(&sBottlesPositions[i], &sp3C);
        thisv->bottlesColliders[i].dim.pos.x = sp3C.x;
        thisv->bottlesColliders[i].dim.pos.y = sp3C.y;
        thisv->bottlesColliders[i].dim.pos.z = sp3C.z;
    }
}

s32 func_80A2F760(EnGb* thisv) {
    s32 i;
    for (i = 0; i < ARRAY_COUNT(thisv->cagedSouls); i++) {
        if (thisv->cagedSouls[i].unk_3) {
            return 1;
        }
    }
    return 0;
}

void func_80A2F7C0(EnGb* thisv) {
    Animation_Change(&thisv->skelAnime, &gPoeSellerSwingStickAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gPoeSellerSwingStickAnim), ANIMMODE_ONCE, 0.0f);
    Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_NALE_MAGIC);
    thisv->actionFunc = func_80A2FC70;
}

void func_80A2F83C(EnGb* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!func_80A2F760(thisv)) {
        if (thisv->actionTimer != 0) {
            thisv->actionTimer--;
        } else {
            func_80A2F7C0(thisv);
            return;
        }
    }
    if (Actor_ProcessTalkRequest(&thisv->dyna.actor, globalCtx)) {
        switch (func_8002F368(globalCtx)) {
            case EXCH_ITEM_NONE:
                func_80A2F180(thisv);
                thisv->actionFunc = func_80A2F94C;
                break;
            case EXCH_ITEM_POE:
                player->actor.textId = 0x70F6;
                thisv->actionFunc = func_80A2F9C0;
                break;
            case EXCH_ITEM_BIG_POE:
                player->actor.textId = 0x70F7;
                thisv->actionFunc = func_80A2FA50;
                break;
        }
        return;
    }
    if (thisv->dyna.actor.xzDistToPlayer < 100.0f) {
        func_8002F298(&thisv->dyna.actor, globalCtx, 100.0f, EXCH_ITEM_POE);
    }
}

void func_80A2F94C(EnGb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        if (!(gSaveContext.infTable[0xB] & 0x40)) {
            gSaveContext.infTable[0xB] |= 0x40;
        }
        func_80A2F180(thisv);
        thisv->actionFunc = func_80A2F83C;
    }
}

void func_80A2F9C0(EnGb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        if (!(gSaveContext.infTable[0xB] & 0x40)) {
            gSaveContext.infTable[0xB] |= 0x40;
        }
        func_80A2F180(thisv);
        Player_UpdateBottleHeld(globalCtx, GET_PLAYER(globalCtx), ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        Rupees_ChangeBy(10);
        thisv->actionFunc = func_80A2F83C;
    }
}

void func_80A2FA50(EnGb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        if (!(gSaveContext.infTable[0xB] & 0x40)) {
            gSaveContext.infTable[0xB] |= 0x40;
        }
        func_80A2F180(thisv);
        Player_UpdateBottleHeld(globalCtx, GET_PLAYER(globalCtx), ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        Rupees_ChangeBy(50);
        HIGH_SCORE(HS_POE_POINTS) += 100;
        if (HIGH_SCORE(HS_POE_POINTS) != 1000) {
            if (HIGH_SCORE(HS_POE_POINTS) > 1100) {
                HIGH_SCORE(HS_POE_POINTS) = 1100;
            }
            thisv->actionFunc = func_80A2F83C;
        } else {
            Player* player = GET_PLAYER(globalCtx);

            player->exchangeItemId = EXCH_ITEM_NONE;
            thisv->textId = 0x70F8;
            Message_ContinueTextbox(globalCtx, thisv->textId);
            thisv->actionFunc = func_80A2FB40;
        }
    }
}

void func_80A2FB40(EnGb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        func_8002F434(&thisv->dyna.actor, globalCtx, GI_BOTTLE, 100.0f, 10.0f);
        thisv->actionFunc = func_80A2FBB0;
    }
}

void func_80A2FBB0(EnGb* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->dyna.actor, globalCtx)) {
        thisv->dyna.actor.parent = NULL;
        thisv->actionFunc = func_80A2FC0C;
    } else {
        func_8002F434(&thisv->dyna.actor, globalCtx, GI_BOTTLE, 100.0f, 10.0f);
    }
}

void func_80A2FC0C(EnGb* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE && Message_ShouldAdvance(globalCtx)) {
        Actor_ProcessTalkRequest(&thisv->dyna.actor, globalCtx);
        func_80A2F180(thisv);
        thisv->actionFunc = func_80A2F83C;
    }
}

void func_80A2FC70(EnGb* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame == Animation_GetLastFrame(&gPoeSellerSwingStickAnim)) {
        Animation_Change(&thisv->skelAnime, &gPoeSellerIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gPoeSellerIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        thisv->actionFunc = func_80A2F83C;
    } else if (thisv->skelAnime.curFrame == 18.0f) {
        thisv->cagedSouls[1].unk_1 = 3;
        thisv->cagedSouls[1].unk_3 = 1;
        thisv->cagedSouls[2].unk_1 = 3;
        thisv->cagedSouls[2].unk_3 = 1;
        thisv->cagedSouls[3].unk_1 = 3;
        thisv->cagedSouls[3].unk_3 = 1;
        if (thisv->actionFunc) {} // these ifs cannot just contain a constant
        thisv->cagedSouls[0].unk_1 = 3;
        thisv->cagedSouls[0].unk_3 = 1;
        if (thisv->actionFunc) {}
        thisv->actionTimer = (s16)Rand_ZeroFloat(600.0f) + 600;
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_WOOD_HIT);
    }
}

void EnGb_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnGb* thisv = (EnGb*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    s32 i;
    f32 rand;

    thisv->frameTimer++;
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actionFunc(thisv, globalCtx);
    thisv->dyna.actor.textId = thisv->textId;
    func_80A2F608(thisv);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

    for (i = 0; i < ARRAY_COUNT(thisv->bottlesColliders); i++) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bottlesColliders[i].base);
    }

    rand = Rand_ZeroOne();
    thisv->lightColor.r = (s8)(rand * 30.0f) + 225;
    thisv->lightColor.g = (s8)(rand * 100.0f) + 155;
    thisv->lightColor.b = (s8)(rand * 160.0f) + 95;
    thisv->lightColor.a = 200;
    EnGb_UpdateCagedSouls(thisv, globalCtx);
}

void EnGb_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnGb* thisv = (EnGb*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_gb.c", 763);

    func_80093D18(globalCtx->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, 255);

    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y,
                              thisv->dyna.actor.world.pos.z, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                              thisv->lightColor.a);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, &thisv->dyna.actor);
    EnGb_DrawCagedSouls(thisv, globalCtx);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_gb.c", 796);
}

void EnGb_UpdateCagedSouls(EnGb* thisv, GlobalContext* globalCtx) {
    f32 temp_f20;
    s16 rot;
    s32 i;

    for (i = 0; i < 4; i++) {
        switch (thisv->cagedSouls[i].unk_1) {
            case 0:
                Math_ApproachF(&thisv->cagedSouls[i].unk_20, 1.0f, 0.02f, thisv->cagedSouls[i].unk_24);
                Math_ApproachF(&thisv->cagedSouls[i].unk_24, 1.0f, 0.001f, 1.0f);
                if ((thisv->cagedSouls[i].unk_28 - .01f) <= thisv->cagedSouls[i].unk_20) {
                    thisv->cagedSouls[i].unk_20 = thisv->cagedSouls[i].unk_28;
                    thisv->cagedSouls[i].unk_24 = 0.0f;
                    thisv->cagedSouls[i].unk_1 = 1;
                    thisv->cagedSouls[i].unk_2 = 2;
                    thisv->cagedSouls[i].unk_6 = 0;
                }
                break;
            case 1:
                if (thisv->cagedSouls[i].unk_6 != 0) {
                    thisv->cagedSouls[i].unk_6--;
                } else {
                    thisv->cagedSouls[i].unk_3 = 0;
                    thisv->cagedSouls[i].unk_24 = 0.0f;
                    thisv->cagedSouls[i].unk_1 = thisv->cagedSouls[i].unk_2;
                }
                break;
            case 2:
                Math_ApproachF(&thisv->cagedSouls[i].unk_20, 0.0f, 0.02f, thisv->cagedSouls[i].unk_24);
                Math_ApproachF(&thisv->cagedSouls[i].unk_24, 1.0f, 0.001f, 1.0f);
                if (thisv->cagedSouls[i].unk_20 <= 0.01f) {
                    thisv->cagedSouls[i].unk_28 = thisv->cagedSouls[i].unk_28 + 0.2f;
                    if (thisv->cagedSouls[i].unk_28 > 1.0f) {
                        thisv->cagedSouls[i].unk_28 = 1.0f;
                    }
                    thisv->cagedSouls[i].unk_20 = 0.0f;
                    thisv->cagedSouls[i].unk_24 = 0.0f;
                    thisv->cagedSouls[i].unk_1 = 1;
                    thisv->cagedSouls[i].unk_2 = 0;
                    thisv->cagedSouls[i].unk_6 = 0;
                }
                break;
            case 3:
                Math_ApproachF(&thisv->cagedSouls[i].unk_20, 0.0f, 0.5f, 1.0f);
                if (thisv->cagedSouls[i].unk_20 <= 0.01f) {
                    thisv->cagedSouls[i].unk_28 = 0.2f;
                    thisv->cagedSouls[i].unk_20 = 0.0f;
                    thisv->cagedSouls[i].unk_24 = 0.0f;
                    thisv->cagedSouls[i].unk_1 = 1;
                    thisv->cagedSouls[i].unk_2 = 0;
                    thisv->cagedSouls[i].unk_6 = (s16)Rand_ZeroFloat(60.0f) + 60;
                }
                break;
        }

        temp_f20 = thisv->cagedSouls[i].unk_20 * 60.0f;
        if ((i == 0) || (i == 3)) {
            thisv->cagedSouls[i].translation.x = thisv->cagedSouls[i].unk_14.x;
            thisv->cagedSouls[i].translation.y = thisv->cagedSouls[i].unk_14.y + temp_f20;
            thisv->cagedSouls[i].translation.z = thisv->cagedSouls[i].unk_14.z;
        } else if (i == 1) {
            rot = thisv->dyna.actor.world.rot.y - 0x4000;
            thisv->cagedSouls[i].translation.x = thisv->cagedSouls[i].unk_14.x + Math_SinS(rot) * temp_f20;
            thisv->cagedSouls[i].translation.z = thisv->cagedSouls[i].unk_14.z + Math_CosS(rot) * temp_f20;
            thisv->cagedSouls[i].translation.y = thisv->cagedSouls[i].unk_14.y;
        } else {
            rot = thisv->dyna.actor.world.rot.y + 0x4000;
            thisv->cagedSouls[i].translation.x = thisv->cagedSouls[i].unk_14.x + Math_SinS(rot) * temp_f20;
            thisv->cagedSouls[i].translation.z = thisv->cagedSouls[i].unk_14.z + Math_CosS(rot) * temp_f20;
            thisv->cagedSouls[i].translation.y = thisv->cagedSouls[i].unk_14.y;
        }
    }
}

void EnGb_DrawCagedSouls(EnGb* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_gb.c", 914);

    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < 4; i++) {
        s32 idx = thisv->cagedSouls[i].infoIdx;

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (u32)(sCagedSoulInfo[idx].timerMultiplier * thisv->frameTimer) % 512, 32, 128));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sCagedSoulInfo[idx].texture));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, sCagedSoulInfo[idx].prim.r, sCagedSoulInfo[idx].prim.g,
                        sCagedSoulInfo[idx].prim.b, sCagedSoulInfo[idx].prim.a);
        gDPSetEnvColor(POLY_XLU_DISP++, sCagedSoulInfo[idx].env.r, sCagedSoulInfo[idx].env.g, sCagedSoulInfo[idx].env.b,
                       sCagedSoulInfo[idx].env.a);

        Matrix_Push();
        Matrix_Translate(thisv->cagedSouls[i].translation.x, thisv->cagedSouls[i].translation.y,
                         thisv->cagedSouls[i].translation.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);

        if (thisv->cagedSouls[i].rotate180) {
            Matrix_RotateZYX(0, -0x8000, 0, MTXMODE_APPLY);
        }
        Matrix_Scale(0.007f, 0.007f, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_gb.c", 955),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPoeSellerCagedSoulDL);

        Matrix_Pop();
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_gb.c", 962);
}
