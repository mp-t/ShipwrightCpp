/*
 * File: z_en_yukabyun.c
 * Overlay: ovl_En_Yukabyun
 * Description: Flying floor tile
 */

#include "z_en_yukabyun.h"
#include "objects/object_yukabyun/object_yukabyun.h"

#define FLAGS ACTOR_FLAG_4

void EnYukabyun_Init(Actor* thisx, GlobalContext* globalCtx);
void EnYukabyun_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnYukabyun_Update(Actor* thisx, GlobalContext* globalCtx);
void EnYukabyun_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B43A94(EnYukabyun* thisv, GlobalContext* globalCtx);
void func_80B43AD4(EnYukabyun* thisv, GlobalContext* globalCtx);
void func_80B43B6C(EnYukabyun* thisv, GlobalContext* globalCtx);

ActorInit En_Yukabyun_InitVars = {
    ACTOR_EN_YUKABYUN,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_YUKABYUN,
    sizeof(EnYukabyun),
    (ActorFunc)EnYukabyun_Init,
    (ActorFunc)EnYukabyun_Destroy,
    (ActorFunc)EnYukabyun_Update,
    (ActorFunc)EnYukabyun_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_ON,
        OCELEM_ON,
    },
    { 28, 8, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 16, ICHAIN_STOP),
};

static const void* D_80B43F64[] = { gFloorTileEnemyTopTex, gFloorTileEnemyBottomTex };

void EnYukabyun_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnYukabyun* thisv = (EnYukabyun*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.4f);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->actor.params++;
    thisv->unk_152 = 0;
    thisv->unk_150 = (u8)(thisv->actor.params) * 0xA + 0x14;
    thisv->actionfunc = func_80B43A94;
}

void EnYukabyun_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnYukabyun* thisv = (EnYukabyun*)thisx;
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B43A94(EnYukabyun* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_150 != 0) {
        thisv->unk_150--;
    }
    if (thisv->unk_150 == 0) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_12;
        thisv->actionfunc = func_80B43AD4;
    }
}

void func_80B43AD4(EnYukabyun* thisv, GlobalContext* globalCtx) {
    thisv->unk_150 += 0xA0;
    thisv->actor.shape.rot.y += thisv->unk_150;
    if (thisv->unk_150 >= 0x2000) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->actor.speedXZ = 10.0f;
        thisv->actionfunc = func_80B43B6C;
    }
    Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 30.0f, 1.0f);
    func_8002F974(&thisv->actor, NA_SE_EN_YUKABYUN_FLY - SFX_FLAG);
}

void func_80B43B6C(EnYukabyun* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.y += thisv->unk_150;
    if (thisv->actor.xzDistToPlayer > 5000.0f) {
        Actor_Kill(&thisv->actor);
        return;
    }
    func_8002F974(&thisv->actor, NA_SE_EN_YUKABYUN_FLY - SFX_FLAG);
}

void EnYukabyun_Break(EnYukabyun* thisv, GlobalContext* globalCtx) {
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 8.0f, 0, 1300, 300, 15, OBJECT_YUKABYUN, 10,
                             gFloorTileEnemyFragmentDL);
    Actor_Kill(&thisv->actor);
}

void EnYukabyun_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnYukabyun* thisv = (EnYukabyun*)thisx;
    s32 pad;

    if (((thisv->collider.base.atFlags & AT_HIT) || (thisv->collider.base.acFlags & AC_HIT) ||
         ((thisv->collider.base.ocFlags1 & OC1_HIT) && !(thisv->collider.base.oc->id == ACTOR_EN_YUKABYUN))) ||
        ((thisv->actionfunc == func_80B43B6C) && (thisv->actor.bgCheckFlags & 8))) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        thisv->collider.base.acFlags &= ~AC_HIT;
        thisv->collider.base.ocFlags1 &= ~OC1_HIT;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 30, NA_SE_EN_OCTAROCK_ROCK);
        thisv->actionfunc = EnYukabyun_Break;
    }

    thisv->actionfunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);

    if (!(thisv->actionfunc == func_80B43A94 || thisv->actionfunc == EnYukabyun_Break)) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 20.0f, 8.0f, 5);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

        thisv->actor.flags |= ACTOR_FLAG_24;

        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    Actor_SetFocus(&thisv->actor, 4.0f);
}

void EnYukabyun_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnYukabyun* thisv = (EnYukabyun*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_yukabyun.c", 366);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80B43F64[thisv->unk_152]));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_yukabyun.c", 373),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFloorTileEnemyDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_yukabyun.c", 378);
}
