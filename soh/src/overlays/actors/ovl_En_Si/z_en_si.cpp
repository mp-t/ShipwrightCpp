/*
 * File: z_en_si.c
 * Overlay: En_Si
 * Description: Gold Skulltula token
 */

#include "z_en_si.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_9)

void EnSi_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 func_80AFB748(EnSi* thisv, GlobalContext* globalCtx);
void func_80AFB768(EnSi* thisv, GlobalContext* globalCtx);
void func_80AFB89C(EnSi* thisv, GlobalContext* globalCtx);
void func_80AFB950(EnSi* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000090, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 18, 2, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 D_80AFBADC = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_Si_InitVars = {
    ACTOR_EN_SI,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSi),
    (ActorFunc)EnSi_Init,
    (ActorFunc)EnSi_Destroy,
    (ActorFunc)EnSi_Update,
    (ActorFunc)EnSi_Draw,
    NULL,
};

void EnSi_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* thisv = (EnSi*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &D_80AFBADC);
    Actor_SetScale(&thisv->actor, 0.025f);
    thisv->unk_19C = 0;
    thisv->actionFunc = func_80AFB768;
    thisv->actor.shape.yOffset = 42.0f;
}

void EnSi_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* thisv = (EnSi*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

s32 func_80AFB748(EnSi* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
    }
    return 0;
}

void func_80AFB768(EnSi* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        thisv->actionFunc = func_80AFB89C;
    } else {
        Math_SmoothStepToF(&thisv->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
        Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
        thisv->actor.shape.rot.y += 0x400;

        if (!Player_InCsMode(globalCtx)) {
            func_80AFB748(thisv, globalCtx);

            if (thisv->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                thisv->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
                Item_Give(globalCtx, ITEM_SKULL_TOKEN);
                player->actor.freezeTimer = 10;
                Message_StartTextbox(globalCtx, 0xB4, NULL);
                Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                thisv->actionFunc = func_80AFB950;
            } else {
                Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
        }
    }
}

void func_80AFB89C(EnSi* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&thisv->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
    thisv->actor.shape.rot.y += 0x400;

    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_13)) {
        Item_Give(globalCtx, ITEM_SKULL_TOKEN);
        player->actor.freezeTimer = 10;
        Message_StartTextbox(globalCtx, 0xB4, NULL);
        Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
        thisv->actionFunc = func_80AFB950;
    }
}

void func_80AFB950(EnSi* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_CLOSING) {
        player->actor.freezeTimer = 10;
    } else {
        SET_GS_FLAGS((thisv->actor.params & 0x1F00) >> 8, thisv->actor.params & 0xFF);
        Actor_Kill(&thisv->actor);
    }
}

void EnSi_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* thisv = (EnSi*)thisx;

    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
    thisv->actionFunc(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 16.0f);
}

void EnSi_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* thisv = (EnSi*)thisx;

    if (thisv->actionFunc != func_80AFB950) {
        func_8002ED80(&thisv->actor, globalCtx, 0);
        func_8002EBCC(&thisv->actor, globalCtx, 0);
        GetItem_Draw(globalCtx, GID_SKULL_TOKEN_2);
    }
}
