/*
 * File: z_bg_hidan_rock.c
 * Overlay: ovl_Bg_Hidan_Rock
 * Description: Stone blocks (Fire Temple)
 */

#include "z_bg_hidan_rock.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanRock_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRock_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRock_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRock_Reset(void);

void func_8088B24C(BgHidanRock* thisv);

void func_8088B268(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B5F4(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B634(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B69C(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B79C(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B90C(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B954(BgHidanRock* thisv, GlobalContext* globalCtx);
void func_8088B990(BgHidanRock* thisv, GlobalContext* globalCtx);

void func_8088BC40(GlobalContext* globalCtx, BgHidanRock* thisv);

static Vec3f D_8088BF60 = { 3310.0f, 120.0f, 0.0f };

ActorInit Bg_Hidan_Rock_InitVars = {
    ACTOR_BG_HIDAN_ROCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanRock),
    (ActorFunc)BgHidanRock_Init,
    (ActorFunc)BgHidanRock_Destroy,
    (ActorFunc)BgHidanRock_Update,
    (ActorFunc)BgHidanRock_Draw,
    (ActorResetFunc)BgHidanRock_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 45, 77, -40, { 3310, 120, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -600, ICHAIN_STOP),
};

void BgHidanRock_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRock* thisv = (BgHidanRock*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);

    thisv->type = thisx->params & 0xFF;
    thisv->unk_169 = 0;

    thisx->params = ((thisx->params) >> 8) & 0xFF;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sCylinderInit);

    if (thisv->type == 0) {
        if (Flags_GetSwitch(globalCtx, thisx->params)) {
            Math_Vec3f_Copy(&thisx->home.pos, &D_8088BF60);
            Math_Vec3f_Copy(&thisx->world.pos, &D_8088BF60);
            thisv->timer = 60;
            thisv->actionFunc = func_8088B5F4;
        } else {
            thisv->actionFunc = func_8088B268;
        }
        thisx->flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
        CollisionHeader_GetVirtual(&gFireTempleStoneBlock1Col, &colHeader);
    } else {
        CollisionHeader_GetVirtual(&gFireTempleStoneBlock2Col, &colHeader);
        thisv->collider.dim.pos.x = thisx->home.pos.x;
        thisv->collider.dim.pos.y = thisx->home.pos.y;
        thisv->collider.dim.pos.z = thisx->home.pos.z;
        thisv->actionFunc = func_8088B634;
    }

    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    thisv->timer = 0;
}

void BgHidanRock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRock* thisv = (BgHidanRock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    Audio_StopSfxByPos(&thisv->unk_170);
}

void func_8088B24C(BgHidanRock* thisv) {
    thisv->dyna.actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    thisv->actionFunc = func_8088B990;
}

static f32 D_8088BFC0 = 0.0f;
void func_8088B268(BgHidanRock* thisv, GlobalContext* globalCtx) {
    f32 sp2C;
    s32 temp_v1;
    s32 frame;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->dyna.unk_150 != 0.0f) {
        if (thisv->timer == 0) {
            if (D_8088BFC0 == 0.0f) {
                if (thisv->dyna.unk_150 > 0.0f) {
                    D_8088BFC0 += 0.01f;
                } else {
                    D_8088BFC0 -= 0.01f;
                }
            }

            thisv->dyna.actor.speedXZ += 0.05f;
            thisv->dyna.actor.speedXZ = CLAMP_MAX(thisv->dyna.actor.speedXZ, 2.0f);

            if (D_8088BFC0 > 0.0f) {
                temp_v1 = Math_StepToF(&D_8088BFC0, 20.0f, thisv->dyna.actor.speedXZ);
            } else {
                temp_v1 = Math_StepToF(&D_8088BFC0, -20.0f, thisv->dyna.actor.speedXZ);
            }

            thisv->dyna.actor.world.pos.x = (Math_SinS(thisv->dyna.unk_158) * D_8088BFC0) + thisv->dyna.actor.home.pos.x;
            thisv->dyna.actor.world.pos.z = (Math_CosS(thisv->dyna.unk_158) * D_8088BFC0) + thisv->dyna.actor.home.pos.z;

            if (temp_v1) {
                player->stateFlags2 &= ~0x10;
                thisv->dyna.unk_150 = 0.0f;
                thisv->dyna.actor.home.pos.x = thisv->dyna.actor.world.pos.x;
                thisv->dyna.actor.home.pos.z = thisv->dyna.actor.world.pos.z;
                D_8088BFC0 = 0.0f;
                thisv->dyna.actor.speedXZ = 0.0f;
                thisv->timer = 5;
            }

            func_8002F974(&thisv->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
        } else {
            player->stateFlags2 &= ~0x10;
            thisv->dyna.unk_150 = 0.0f;
            if (thisv->timer != 0) {
                thisv->timer--;
            }
        }
    } else {
        thisv->timer = 0;
    }

    sp2C = thisv->dyna.actor.world.pos.z - D_8088BF60.z;
    if (sp2C < 0.5f) {
        Flags_SetSwitch(globalCtx, thisv->dyna.actor.params);
        Math_Vec3f_Copy(&thisv->dyna.actor.home.pos, &D_8088BF60);
        thisv->dyna.actor.world.pos.x = D_8088BF60.x;
        thisv->dyna.actor.world.pos.z = D_8088BF60.z;
        thisv->dyna.actor.speedXZ = 0.0f;
        D_8088BFC0 = 0.0f;
        player->stateFlags2 &= ~0x10;
        thisv->actionFunc = func_8088B79C;
    }

    frame = globalCtx->gameplayFrames & 0xFF;
    if (globalCtx->gameplayFrames & 0x100) {
        thisv->unk_16C = 0.0f;
    } else if (frame < 128) {
        thisv->unk_16C = sinf(frame * (4 * 0.001f * std::numbers::pi_v<float>)) * 19.625f;
    } else if (frame < 230) {
        thisv->unk_16C = 19.625f;
    } else {
        thisv->unk_16C -= 1.0f;
        thisv->unk_16C = CLAMP_MIN(thisv->unk_16C, 0.0f);
    }

    if (sp2C < 100.0f) {
        thisv->unk_16C = CLAMP_MAX(thisv->unk_16C, 6.125f);
    }
}

void func_8088B5F4(BgHidanRock* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        func_8088B24C(thisv);
    }
}

void func_8088B634(BgHidanRock* thisv, GlobalContext* globalCtx) {
    if (func_8004356C(&thisv->dyna)) {
        thisv->timer = 20;
        thisv->dyna.actor.world.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4000;
        thisv->actionFunc = func_8088B69C;
    }
}

void func_8088B69C(BgHidanRock* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer != 0) {
        thisv->dyna.actor.world.pos.x =
            thisv->dyna.actor.home.pos.x + 5.0f * Math_SinS(thisv->dyna.actor.world.rot.y + thisv->timer * 0x4000);
        thisv->dyna.actor.world.pos.z =
            thisv->dyna.actor.home.pos.z + 5.0f * Math_CosS(thisv->dyna.actor.world.rot.y + thisv->timer * 0x4000);
    } else {
        thisv->dyna.actor.world.pos.x = thisv->dyna.actor.home.pos.x;
        thisv->dyna.actor.world.pos.z = thisv->dyna.actor.home.pos.z;
        func_8088B24C(thisv);
    }

    if (!(thisv->timer % 4)) {
        func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 0xB4, 0x0A, 0x64);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }
}

void func_8088B79C(BgHidanRock* thisv, GlobalContext* globalCtx) {
    thisv->timer--;
    if (thisv->dyna.actor.bgCheckFlags & 2) {
        if (thisv->type == 0) {
            thisv->timer = 60;
            thisv->actionFunc = func_8088B5F4;
        } else {
            thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - 15.0f;
            thisv->actionFunc = func_8088B90C;
            thisv->dyna.actor.flags &= ~(ACTOR_FLAG_4 | ACTOR_FLAG_5);
        }

        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        Audio_PlayActorSound2(
            &thisv->dyna.actor,
            SurfaceType_GetSfx(&globalCtx->colCtx, thisv->dyna.actor.floorPoly, thisv->dyna.actor.floorBgId) + 0x800);
    }

    thisv->unk_16C -= 0.5f;
    thisv->unk_16C = CLAMP_MIN(thisv->unk_16C, 0.0f);

    if (thisv->type == 0) {
        if (func_8004356C(&thisv->dyna)) {
            if (thisv->unk_169 == 0) {
                thisv->unk_169 = 3;
            }
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
        } else if (!func_8004356C(&thisv->dyna)) {
            if (thisv->unk_169 != 0) {
                Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            }
            thisv->unk_169 = 0;
        }
    }
}

void func_8088B90C(BgHidanRock* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 1.0f)) {
        thisv->actionFunc = func_8088B634;
    }
}

void func_8088B954(BgHidanRock* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        thisv->actionFunc = func_8088B79C;
        thisv->dyna.actor.velocity.y = 0.0f;
    }
}

void func_8088B990(BgHidanRock* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->timer++;
    if (thisv->dyna.unk_150 != 0.0f) {
        thisv->dyna.actor.speedXZ = 0.0f;
        player->stateFlags2 &= ~0x10;
    }

    if ((thisv->type == 0 && (Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 1820.0f,
                                                0.25f, 20.0f, 0.5f) < 0.1f)) ||
        ((thisv->type != 0) && (Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 480.0,
                                                  0.25f, 20.0f, 0.5f) < 0.1f))) {
        if (thisv->type == 0) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        thisv->timer = 20;
        thisv->actionFunc = func_8088B954;
    }

    thisv->unk_16C = (thisv->dyna.actor.world.pos.y + 50.0f - thisv->dyna.actor.home.pos.y + 40.0f) / 80.0f;
    if (thisv->type == 0) {
        if (func_8004356C(&thisv->dyna)) {
            if (thisv->unk_169 == 0) {
                thisv->unk_169 = 3;
            }
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
        } else if (!func_8004356C(&thisv->dyna)) {
            if (thisv->unk_169 != 0) {
                Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            }
            thisv->unk_169 = 0;
        }
    }
}

void BgHidanRock_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRock* thisv = (BgHidanRock*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc == func_8088B79C) {
        Actor_MoveForward(&thisv->dyna.actor);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->dyna.actor, 0.0f, 0.0f, 0.0f, 4);
    }

    if (thisv->unk_16C > 0.0f) {
        thisv->collider.dim.height = sCylinderInit.dim.height * thisv->unk_16C;
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

static const void* sVerticalFlamesTexs[] = {
    gFireTempleBigVerticalFlame0Tex, gFireTempleBigVerticalFlame1Tex, gFireTempleBigVerticalFlame2Tex,
    gFireTempleBigVerticalFlame3Tex, gFireTempleBigVerticalFlame4Tex, gFireTempleBigVerticalFlame5Tex,
    gFireTempleBigVerticalFlame6Tex, gFireTempleBigVerticalFlame7Tex,
};

void func_8088BC40(GlobalContext* globalCtx, BgHidanRock* thisv) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_rock.c", 808);

    POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);

    if (thisv->type == 0) {
        Matrix_Translate(D_8088BF60.x, D_8088BF60.y - 40.0f, D_8088BF60.z, MTXMODE_NEW);
    } else {
        Matrix_Translate(thisv->dyna.actor.home.pos.x, thisv->dyna.actor.home.pos.y - 40.0f, thisv->dyna.actor.home.pos.z,
                         MTXMODE_NEW);
    }

    Matrix_RotateZYX(0, Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000, 0, MTXMODE_APPLY);
    Matrix_Translate(-10.5f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(6.0f, thisv->unk_16C, 6.0f, MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sVerticalFlamesTexs[globalCtx->gameplayFrames & 7]));
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_hidan_rock.c", 853),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFireTempleBigVerticalFlameDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_rock.c", 857);
}

void BgHidanRock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRock* thisv = (BgHidanRock*)thisx;
    s32 pad;

    if (thisv->type == 0) {
        Gfx_DrawDListOpa(globalCtx, gFireTempleStoneBlock1DL);
    } else {
        Gfx_DrawDListOpa(globalCtx, gFireTempleStoneBlock2DL);
    }

    if (thisv->unk_16C > 0.0f) {
        if (thisv->type == 0) {
            SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &D_8088BF60, &thisv->unk_170);
        } else {
            SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &thisv->dyna.actor.home.pos, &thisv->unk_170);
        }

        func_80078914(&thisv->unk_170, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        func_8088BC40(globalCtx, thisv);
    }
}

void BgHidanRock_Reset(void) {
    D_8088BFC0 = 0;
}