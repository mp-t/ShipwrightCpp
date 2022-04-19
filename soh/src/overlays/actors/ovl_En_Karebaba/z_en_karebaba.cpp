/**
 * File: z_en_karebaba.c
 * Overlay: ovl_En_Karebaba
 * Description: Withered Deku Baba
 */

#include "z_en_karebaba.h"
#include "objects/object_dekubaba/object_dekubaba.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnKarebaba_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKarebaba_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKarebaba_Update(Actor* thisx, GlobalContext* globalCtx);
void EnKarebaba_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnKarebaba_SetupGrow(EnKarebaba* thisv);
void EnKarebaba_SetupIdle(EnKarebaba* thisv);
void EnKarebaba_Grow(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Idle(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Awaken(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Spin(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Dying(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_DeadItemDrop(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Retract(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Dead(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Regrow(EnKarebaba* thisv, GlobalContext* globalCtx);
void EnKarebaba_Upright(EnKarebaba* thisv, GlobalContext* globalCtx);

ActorInit En_Karebaba_InitVars = {
    ACTOR_EN_KAREBABA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DEKUBABA,
    sizeof(EnKarebaba),
    (ActorFunc)EnKarebaba_Init,
    (ActorFunc)EnKarebaba_Destroy,
    (ActorFunc)EnKarebaba_Update,
    (ActorFunc)EnKarebaba_Draw,
    NULL,
};

static ColliderCylinderInit sBodyColliderInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 7, 25, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sHeadColliderInit = {
    {
        COLTYPE_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 4, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColCheckInfoInit = { 1, 15, 80, MASS_HEAVY };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2500, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x09, ICHAIN_STOP),
};

void EnKarebaba_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKarebaba* thisv = (EnKarebaba*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, thisv->jointTable,
                   thisv->morphTable, 8);
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, &thisv->actor, &sBodyColliderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);
    Collider_InitCylinder(globalCtx, &thisv->headCollider);
    Collider_SetCylinder(globalCtx, &thisv->headCollider, &thisv->actor, &sHeadColliderInit);
    Collider_UpdateCylinder(&thisv->actor, &thisv->headCollider);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, DamageTable_Get(1), &sColCheckInfoInit);

    thisv->boundFloor = NULL;

    if (thisv->actor.params == 0) {
        EnKarebaba_SetupGrow(thisv);
    } else {
        EnKarebaba_SetupIdle(thisv);
    }
}

void EnKarebaba_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKarebaba* thisv = (EnKarebaba*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->headCollider);
}

void EnKarebaba_ResetCollider(EnKarebaba* thisv) {
    thisv->bodyCollider.dim.radius = 7;
    thisv->bodyCollider.dim.height = 25;
    thisv->bodyCollider.base.colType = COLTYPE_HARD;
    thisv->bodyCollider.base.acFlags |= AC_HARD;
    thisv->bodyCollider.info.bumper.dmgFlags = ~0x00300000;
    thisv->headCollider.dim.height = 25;
}

void EnKarebaba_SetupGrow(EnKarebaba* thisv) {
    Actor_SetScale(&thisv->actor, 0.0f);
    thisv->actor.shape.rot.x = -0x4000;
    thisv->actionFunc = EnKarebaba_Grow;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 14.0f;
}

void EnKarebaba_SetupIdle(EnKarebaba* thisv) {
    Actor_SetScale(&thisv->actor, 0.005f);
    thisv->actor.shape.rot.x = -0x4000;
    thisv->actionFunc = EnKarebaba_Idle;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + 14.0f;
}

void EnKarebaba_SetupAwaken(EnKarebaba* thisv) {
    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DUMMY482);
    thisv->actionFunc = EnKarebaba_Awaken;
}

void EnKarebaba_SetupUpright(EnKarebaba* thisv) {
    if (thisv->actionFunc != EnKarebaba_Spin) {
        Actor_SetScale(&thisv->actor, 0.01f);
        thisv->bodyCollider.base.colType = COLTYPE_HIT6;
        thisv->bodyCollider.base.acFlags &= ~AC_HARD;
        thisv->bodyCollider.info.bumper.dmgFlags = !LINK_IS_ADULT ? 0x07C00710 : 0x0FC00710;
        thisv->bodyCollider.dim.radius = 15;
        thisv->bodyCollider.dim.height = 80;
        thisv->headCollider.dim.height = 80;
    }

    thisv->actor.params = 40;
    thisv->actionFunc = EnKarebaba_Upright;
}

void EnKarebaba_SetupSpin(EnKarebaba* thisv) {
    thisv->actor.params = 40;
    thisv->actionFunc = EnKarebaba_Spin;
}

void EnKarebaba_SetupDying(EnKarebaba* thisv) {
    thisv->actor.params = 0;
    thisv->actor.gravity = -0.8f;
    thisv->actor.velocity.y = 4.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
    thisv->actor.speedXZ = 3.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_DEAD);
    thisv->actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    thisv->actionFunc = EnKarebaba_Dying;
}

void EnKarebaba_SetupDeadItemDrop(EnKarebaba* thisv, GlobalContext* globalCtx) {
    Actor_SetScale(&thisv->actor, 0.03f);
    thisv->actor.shape.rot.x -= 0x4000;
    thisv->actor.shape.yOffset = 1000.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.shape.shadowScale = 3.0f;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_MISC);
    thisv->actor.params = 200;
    thisv->actor.flags &= ~ACTOR_FLAG_5;
    thisv->actionFunc = EnKarebaba_DeadItemDrop;
}

void EnKarebaba_SetupRetract(EnKarebaba* thisv) {
    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, -3.0f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    EnKarebaba_ResetCollider(thisv);
    thisv->actionFunc = EnKarebaba_Retract;
}

void EnKarebaba_SetupDead(EnKarebaba* thisv) {
    Animation_Change(&thisv->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
    EnKarebaba_ResetCollider(thisv);
    thisv->actor.shape.rot.x = -0x4000;
    thisv->actor.params = 200;
    thisv->actor.parent = NULL;
    thisv->actor.shape.shadowScale = 0.0f;
    Math_Vec3f_Copy(&thisv->actor.world.pos, &thisv->actor.home.pos);
    thisv->actionFunc = EnKarebaba_Dead;
}

void EnKarebaba_SetupRegrow(EnKarebaba* thisv) {
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.shape.shadowScale = 22.0f;
    thisv->headCollider.dim.radius = sHeadColliderInit.dim.radius;
    Actor_SetScale(&thisv->actor, 0.0f);
    thisv->actionFunc = EnKarebaba_Regrow;
}

void EnKarebaba_Grow(EnKarebaba* thisv, GlobalContext* globalCtx) {
    f32 scale;

    thisv->actor.params++;
    scale = thisv->actor.params * 0.05f;
    Actor_SetScale(&thisv->actor, 0.005f * scale);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (14.0f * scale);
    if (thisv->actor.params == 20) {
        EnKarebaba_SetupIdle(thisv);
    }
}

void EnKarebaba_Idle(EnKarebaba* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer < 200.0f && fabsf(thisv->actor.yDistToPlayer) < 30.0f) {
        EnKarebaba_SetupAwaken(thisv);
    }
}

void EnKarebaba_Awaken(EnKarebaba* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.scale.x, 0.01f, 0.0005f);
    thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;
    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 60.0f, 5.0f)) {
        EnKarebaba_SetupUpright(thisv);
    }
    thisv->actor.shape.rot.y += 0x1999;
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnKarebaba_Upright(EnKarebaba* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.params != 0) {
        thisv->actor.params--;
    }

    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_MOUTH);
    }

    if (thisv->bodyCollider.base.acFlags & AC_HIT) {
        EnKarebaba_SetupDying(thisv);
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
    } else if (Math_Vec3f_DistXZ(&thisv->actor.home.pos, &player->actor.world.pos) > 240.0f) {
        EnKarebaba_SetupRetract(thisv);
    } else if (thisv->actor.params == 0) {
        EnKarebaba_SetupSpin(thisv);
    }
}

void EnKarebaba_Spin(EnKarebaba* thisv, GlobalContext* globalCtx) {
    s32 value;
    f32 cos60;

    if (thisv->actor.params != 0) {
        thisv->actor.params--;
    }

    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        if (1) {} // Here for matching purposes only.

        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_JR_MOUTH);
    }

    value = 20 - thisv->actor.params;
    value = 20 - ABS(value);

    if (value > 10) {
        value = 10;
    }

    thisv->headCollider.dim.radius = sHeadColliderInit.dim.radius + (value * 2);
    thisv->actor.shape.rot.x = 0xC000 - (value * 0x100);
    thisv->actor.shape.rot.y += value * 0x2C0;
    thisv->actor.world.pos.y = (Math_SinS(thisv->actor.shape.rot.x) * -60.0f) + thisv->actor.home.pos.y;

    cos60 = Math_CosS(thisv->actor.shape.rot.x) * 60.0f;

    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y) * cos60) + thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y) * cos60) + thisv->actor.home.pos.z;

    if (thisv->bodyCollider.base.acFlags & AC_HIT) {
        EnKarebaba_SetupDying(thisv);
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
    } else if (thisv->actor.params == 0) {
        EnKarebaba_SetupUpright(thisv);
    }
}

void EnKarebaba_Dying(EnKarebaba* thisv, GlobalContext* globalCtx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f position;
    Vec3f rotation;

    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.1f);

    if (thisv->actor.params == 0) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x4800, 0x71C);
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if (thisv->actor.scale.x > 0.005f && ((thisv->actor.bgCheckFlags & 2) || (thisv->actor.bgCheckFlags & 8))) {
            thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z = 0.0f;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 3.0f, 0, 12, 5, 15, HAHEN_OBJECT_DEFAULT, 10,
                                     NULL);
        }

        if (thisv->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
            thisv->actor.params = 1;
        }
    } else if (thisv->actor.params == 1) {
        Math_Vec3f_Copy(&position, &thisv->actor.world.pos);
        rotation.z = Math_SinS(thisv->actor.shape.rot.x) * 20.0f;
        rotation.x = -20.0f * Math_CosS(thisv->actor.shape.rot.x) * Math_SinS(thisv->actor.shape.rot.y);
        rotation.y = -20.0f * Math_CosS(thisv->actor.shape.rot.x) * Math_CosS(thisv->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            func_800286CC(globalCtx, &position, &zeroVec, &zeroVec, 500, 50);
            position.x += rotation.x;
            position.y += rotation.z;
            position.z += rotation.y;
        }

        func_800286CC(globalCtx, &thisv->actor.home.pos, &zeroVec, &zeroVec, 500, 100);
        EnKarebaba_SetupDeadItemDrop(thisv, globalCtx);
    }
}

void EnKarebaba_DeadItemDrop(EnKarebaba* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params != 0) {
        thisv->actor.params--;
    }

    if (Actor_HasParent(&thisv->actor, globalCtx) || thisv->actor.params == 0) {
        EnKarebaba_SetupDead(thisv);
    } else {
        func_8002F554(&thisv->actor, globalCtx, GI_STICKS_1);
    }
}

void EnKarebaba_Retract(EnKarebaba* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.scale.x, 0.005f, 0.0005f);
    thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;

    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y + 14.0f, 5.0f)) {
        EnKarebaba_SetupIdle(thisv);
    }

    thisv->actor.shape.rot.y += 0x1999;
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnKarebaba_Dead(EnKarebaba* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.params != 0) {
        thisv->actor.params--;
    }
    if (thisv->actor.params == 0) {
        EnKarebaba_SetupRegrow(thisv);
    }
}

void EnKarebaba_Regrow(EnKarebaba* thisv, GlobalContext* globalCtx) {
    f32 scaleFactor;

    thisv->actor.params++;
    scaleFactor = thisv->actor.params * 0.05f;
    Actor_SetScale(&thisv->actor, 0.005f * scaleFactor);
    thisv->actor.world.pos.y = thisv->actor.home.pos.y + (14.0f * scaleFactor);

    if (thisv->actor.params == 20) {
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_2;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
        EnKarebaba_SetupIdle(thisv);
    }
}

void EnKarebaba_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnKarebaba* thisv = (EnKarebaba*)thisx;
    f32 height;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actionFunc != EnKarebaba_Dead) {
        if (thisv->actionFunc == EnKarebaba_Dying) {
            Actor_MoveForward(&thisv->actor);
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 15.0f, 10.0f, 5);
        } else {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 4);
            if (thisv->boundFloor == NULL) {
                thisv->boundFloor = thisv->actor.floorPoly;
            }
        }
        if (thisv->actionFunc != EnKarebaba_Dying && thisv->actionFunc != EnKarebaba_DeadItemDrop) {
            if (thisv->actionFunc != EnKarebaba_Regrow && thisv->actionFunc != EnKarebaba_Grow) {
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->headCollider.base);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
            }
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->headCollider.base);
            Actor_SetFocus(&thisv->actor, (thisv->actor.scale.x * 10.0f) / 0.01f);
            height = thisv->actor.home.pos.y + 40.0f;
            thisv->actor.focus.pos.x = thisv->actor.home.pos.x;
            thisv->actor.focus.pos.y = CLAMP_MAX(thisv->actor.focus.pos.y, height);
            thisv->actor.focus.pos.z = thisv->actor.home.pos.z;
        }
    }
}

void EnKarebaba_DrawBaseShadow(EnKarebaba* thisv, GlobalContext* globalCtx) {
    MtxF mf;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1013);

    func_80094044(globalCtx->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);
    func_80038A28(thisv->boundFloor, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, &mf);
    Matrix_Mult(&mf, MTXMODE_NEW);
    Matrix_Scale(0.15f, 1.0f, 0.15f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1029),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1034);
}

void EnKarebaba_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Color_RGBA8 black = { 0, 0, 0, 0 };
    static const Gfx* stemDLists[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    EnKarebaba* thisv = (EnKarebaba*)thisx;
    s32 i;
    s32 stemSections;
    f32 scale;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1056);

    func_80093D18(globalCtx->state.gfxCtx);

    if (thisv->actionFunc == EnKarebaba_DeadItemDrop) {
        if (thisv->actor.params > 40 || (thisv->actor.params & 1)) {
            Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1066),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
        }
    } else if (thisv->actionFunc != EnKarebaba_Dead) {
        func_80026230(globalCtx, &black, 1, 2);
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, NULL);
        Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, MTXMODE_NEW);

        if ((thisv->actionFunc == EnKarebaba_Regrow) || (thisv->actionFunc == EnKarebaba_Grow)) {
            scale = thisv->actor.params * 0.0005f;
        } else {
            scale = 0.01f;
        }

        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        Matrix_RotateZYX(thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, 0, MTXMODE_APPLY);

        if (thisv->actionFunc == EnKarebaba_Dying) {
            stemSections = 2;
        } else {
            stemSections = 3;
        }

        for (i = 0; i < stemSections; i++) {
            Matrix_Translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1116),
                      G_MTX_LOAD | G_MTX_NOPUSH | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, stemDLists[i]);

            if (i == 0 && thisv->actionFunc == EnKarebaba_Dying) {
                Matrix_MultVec3f(&zeroVec, &thisv->actor.focus.pos);
            }
        }

        func_80026608(globalCtx);
    }

    func_80026230(globalCtx, &black, 1, 2);
    Matrix_Translate(thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, MTXMODE_NEW);

    if (thisv->actionFunc != EnKarebaba_Grow) {
        scale = 0.01f;
    }

    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateY(thisv->actor.home.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1144),
              G_MTX_LOAD | G_MTX_NOPUSH | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

    if (thisv->actionFunc == EnKarebaba_Dying) {
        Matrix_RotateZYX(-0x4000, (s16)(thisv->actor.shape.rot.y - thisv->actor.home.rot.y), 0, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1155),
                  G_MTX_LOAD | G_MTX_NOPUSH | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);
    }

    func_80026608(globalCtx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_karebaba.c", 1163);

    if (thisv->boundFloor != NULL) {
        EnKarebaba_DrawBaseShadow(thisv, globalCtx);
    }
}
