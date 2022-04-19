#include "z_bg_jya_bombchuiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_jya_obj/object_jya_obj.h"
#define FLAGS ACTOR_FLAG_0

void BgJyaBombchuiwa_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgJyaBombchuiwa_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgJyaBombchuiwa_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgJyaBombchuiwa_Draw(Actor* thisvx, GlobalContext* globalCtx);

void BgJyaBombchuiwa_WaitForExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx);
void BgJyaBombchuiwa_SetupWaitForExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx);
void func_808949B8(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx);
void BgJyaBombchuiwa_CleanUpAfterExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx);
void BgJyaBombchuiwa_SpawnLightRay(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx);

ActorInit Bg_Jya_Bombchuiwa_InitVars = {
    ACTOR_BG_JYA_BOMBCHUIWA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaBombchuiwa),
    (ActorFunc)BgJyaBombchuiwa_Init,
    (ActorFunc)BgJyaBombchuiwa_Destroy,
    (ActorFunc)BgJyaBombchuiwa_Update,
    (ActorFunc)BgJyaBombchuiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { -300, 0, 0 }, 40 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 3, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaBombchuiwa_SetupCollider(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, &thisv->colliderItems);
}

void BgJyaBombchuiwa_SetDrawFlags(BgJyaBombchuiwa* thisv, u8 drawFlags) {
    thisv->drawFlags &= ~7;
    thisv->drawFlags |= drawFlags;
}

void BgJyaBombchuiwa_Init(Actor* thisvx, GlobalContext* globalCtx) {
    BgJyaBombchuiwa* thisv = (BgJyaBombchuiwa*)thisvx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    BgJyaBombchuiwa_SetupCollider(thisv, globalCtx);
    if (Flags_GetSwitch(globalCtx, thisv->actor.params & 0x3F)) {
        BgJyaBombchuiwa_SpawnLightRay(thisv, globalCtx);
    } else {
        BgJyaBombchuiwa_SetupWaitForExplosion(thisv, globalCtx);
    }
    Actor_SetFocus(&thisv->actor, 0.0f);
}

void BgJyaBombchuiwa_Destroy(Actor* thisvx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgJyaBombchuiwa* thisv = (BgJyaBombchuiwa*)thisvx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BgJyaBombchuiwa_Break(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f velocity;
    s16 scale;
    s16 arg5;
    s16 arg6;
    s16 arg7;
    s32 i;

    for (i = 0; i < 20; i++) {
        pos.x = Rand_ZeroOne() * 10.0f + thisv->actor.world.pos.x - 10.0f;
        pos.y = Rand_ZeroOne() * 40.0f + thisv->actor.world.pos.y - 20.0f;
        pos.z = Rand_ZeroOne() * 50.0f + thisv->actor.world.pos.z - 25.0f;
        velocity.x = Rand_ZeroOne() * 3.0f - 0.3f;
        velocity.y = Rand_ZeroOne() * 18.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 15.0f;
        scale = (s32)(Rand_ZeroOne() * 20.0f) + 1;
        if (scale > 10) {
            arg5 = 5;
        } else {
            arg5 = 1;
        }
        if (Rand_ZeroOne() < 0.4f) {
            arg5 |= 0x40;
            arg6 = 0xC;
            arg7 = 8;
        } else {
            arg5 |= 0x20;
            arg6 = 0xC;
            arg7 = 8;
            if (scale < 8) {
                arg6 = 0x46;
                arg7 = 0x28;
            }
        }
        EffectSsKakera_Spawn(globalCtx, &pos, &velocity, &pos, -300, arg5, arg6, arg7, 0, scale, 1, 15, 80,
                             KAKERA_COLOR_NONE, OBJECT_JYA_OBJ, gBombiwaEffectDL);
    }
    func_80033480(globalCtx, &thisv->actor.world.pos, 100.0f, 8, 100, 160, 0);
}

void BgJyaBombchuiwa_SetupWaitForExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = BgJyaBombchuiwa_WaitForExplosion;
    BgJyaBombchuiwa_SetDrawFlags(thisv, 3);
    thisv->timer = 0;
}

void BgJyaBombchuiwa_WaitForExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    if ((thisv->collider.base.acFlags & AC_HIT) || (thisv->timer > 0)) {
        if (thisv->timer == 0) {
            OnePointCutscene_Init(globalCtx, 3410, -99, &thisv->actor, MAIN_CAM);
        }
        thisv->timer++;
        if (thisv->timer > 10) {
            BgJyaBombchuiwa_Break(thisv, globalCtx);
            BgJyaBombchuiwa_CleanUpAfterExplosion(thisv, globalCtx);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        }
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void BgJyaBombchuiwa_CleanUpAfterExplosion(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = func_808949B8;
    BgJyaBombchuiwa_SetDrawFlags(thisv, 4);
    thisv->lightRayIntensity = 0.3f;
    thisv->timer = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void func_808949B8(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    thisv->timer++;
    if (thisv->timer & 4) {
        func_80033480(globalCtx, &thisv->actor.world.pos, 60.0f, 3, 100, 100, 0);
    }
    if (Math_StepToF(&thisv->lightRayIntensity, 1.0f, 0.028)) {
        BgJyaBombchuiwa_SpawnLightRay(thisv, globalCtx);
    }
}

void BgJyaBombchuiwa_SpawnLightRay(BgJyaBombchuiwa* thisv, GlobalContext* globalCtx) {
    thisv->actionFunc = NULL;
    thisv->lightRayIntensity = 153.0f;
    BgJyaBombchuiwa_SetDrawFlags(thisv, 4);
    if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_MIR_RAY, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                    thisv->actor.world.pos.z, 0, 0, 0, 0) == NULL) {
        // "Occurrence failure"
        osSyncPrintf("Ｅｒｒｏｒ : Mir_Ray 発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_jya_bombchuiwa.c", 410,
                     thisv->actor.params);
    }
}

void BgJyaBombchuiwa_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgJyaBombchuiwa* thisv = (BgJyaBombchuiwa*)thisvx;

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void BgJyaBombchuiwa_DrawRock(GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 436);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 439),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwa2DL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 443);
}

void BgJyaBombchuiwa_DrawLight(Actor* thisvx, GlobalContext* globalCtx) {
    BgJyaBombchuiwa* thisv = (BgJyaBombchuiwa*)thisvx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 453);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 457),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(thisv->lightRayIntensity * 153.0f), 153));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight1DL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(thisv->lightRayIntensity * 255.0f), 255));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight2DL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 472);
}

void BgJyaBombchuiwa_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    static Vec3f D_80894F88 = { -920.0f, 480.0f, -889.0f };
    static Vec3s D_80894F94 = { 0, 0, 0 };
    BgJyaBombchuiwa* thisv = (BgJyaBombchuiwa*)thisvx;

    if (thisv->drawFlags & 1) {
        Gfx_DrawDListOpa(globalCtx, gBombchuiwaDL);
        Collider_UpdateSpheres(0, &thisv->collider);
    }

    if (thisv->drawFlags & 2) {
        BgJyaBombchuiwa_DrawRock(globalCtx);
    }
    if (thisv->drawFlags & 4) {
        Matrix_SetTranslateRotateYXZ(D_80894F88.x, D_80894F88.y, D_80894F88.z, &D_80894F94);
        Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
        if (thisv->drawFlags & 4) {
            BgJyaBombchuiwa_DrawLight(thisvx, globalCtx);
        }
    }
}
