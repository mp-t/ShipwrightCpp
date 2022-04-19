/*
 * File: z_en_syateki_niw.c
 * Overlay: ovl_En_Syateki_Niw
 * Description: Hopping Cucco
 */

#include "z_en_syateki_niw.h"
#include "objects/object_niw/object_niw.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnSyatekiNiw_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiNiw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiNiw_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSyatekiNiw_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B11DEC(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B132A8(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B129EC(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B13464(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B123A8(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B11E78(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B12460(EnSyatekiNiw* thisv, GlobalContext* globalCtx);
void func_80B128D8(EnSyatekiNiw* thisv, GlobalContext* globalCtx);

void func_80B131B8(EnSyatekiNiw* thisv, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4);

ActorInit En_Syateki_Niw_InitVars = {
    ACTOR_EN_SYATEKI_NIW,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_NIW,
    sizeof(EnSyatekiNiw),
    (ActorFunc)EnSyatekiNiw_Init,
    (ActorFunc)EnSyatekiNiw_Destroy,
    (ActorFunc)EnSyatekiNiw_Update,
    (ActorFunc)EnSyatekiNiw_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 10, 20, 4, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void EnSyatekiNiw_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSyatekiNiw* thisv = (EnSyatekiNiw*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gCuccoSkel, &gCuccoAnim, thisv->jointTable, thisv->morphTable, 16);

    thisv->unk_29E = thisv->actor.params;
    if (thisv->unk_29E < 0) {
        thisv->unk_29E = 0;
    }

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    if (thisv->unk_29E == 0) {
        osSyncPrintf("\n\n");
        // "Archery range chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 射的場鶏 ☆☆☆☆☆ \n" VT_RST);
        Actor_SetScale(&thisv->actor, 0.01f);
    } else {
        osSyncPrintf("\n\n");
        // "Bomb chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ボムにわ！ ☆☆☆☆☆ \n" VT_RST);
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
        Actor_SetScale(&thisv->actor, 0.01f);
    }

    thisv->unk_2DC = thisv->actor.world.pos;
    thisv->unk_2E8 = thisv->actor.world.pos;
    thisv->actionFunc = func_80B11DEC;
}

void EnSyatekiNiw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSyatekiNiw* thisv = (EnSyatekiNiw*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B11A94(EnSyatekiNiw* thisv, GlobalContext* globalCtx, s16 arg2) {
    if (thisv->unk_254 == 0) {
        if (arg2 == 0) {
            thisv->unk_264 = 0.0f;
        } else {
            thisv->unk_264 = -10000.0f;
        }

        thisv->unk_28E += 1;
        thisv->unk_254 = 3;
        if (!(thisv->unk_28E & 1)) {
            thisv->unk_264 = 0.0f;
            if (arg2 == 0) {
                thisv->unk_254 = Rand_ZeroFloat(30.0f);
            }
        }
    }

    if (thisv->unk_258 == 0) {
        thisv->unk_292++;
        thisv->unk_292 &= 1;
        switch (arg2) {
            case 0:
                thisv->unk_26C = 0.0f;
                thisv->unk_268 = 0.0f;
                break;

            case 1:
                thisv->unk_258 = 3;
                thisv->unk_26C = 7000.0f;
                thisv->unk_268 = 7000.0f;
                if (thisv->unk_292 == 0) {
                    thisv->unk_26C = 0.0f;
                    thisv->unk_268 = 0.0f;
                }
                break;

            case 2:
                thisv->unk_258 = 2;
                thisv->unk_268 = thisv->unk_26C = -10000.0f;
                thisv->unk_280 = thisv->unk_278 = 25000.0f;
                thisv->unk_284 = thisv->unk_27C = 6000.0f;
                if (thisv->unk_292 == 0) {
                    thisv->unk_278 = 8000.0f;
                    thisv->unk_280 = 8000.0f;
                }
                break;

            case 3:
                thisv->unk_258 = 2;
                thisv->unk_278 = 10000.0f;
                thisv->unk_280 = 10000.0f;
                if (thisv->unk_292 == 0) {
                    thisv->unk_278 = 3000.0f;
                    thisv->unk_280 = 3000.0f;
                }
                break;

            case 4:
                thisv->unk_254 = thisv->unk_256 = 5;
                break;

            case 5:
                thisv->unk_258 = 5;
                thisv->unk_278 = 14000.0f;
                thisv->unk_280 = 14000.0f;
                if (thisv->unk_292 == 0) {
                    thisv->unk_278 = 10000.0f;
                    thisv->unk_280 = 10000.0f;
                }
                break;
        }
    }

    if (thisv->unk_264 != thisv->unk_2BC.x) {
        Math_ApproachF(&thisv->unk_2BC.x, thisv->unk_264, 0.5f, 4000.0f);
    }

    if (thisv->unk_26C != thisv->unk_2A4.x) {
        Math_ApproachF(&thisv->unk_2A4.x, thisv->unk_26C, 0.8f, 7000.0f);
    }

    if (thisv->unk_280 != thisv->unk_2A4.y) {
        Math_ApproachF(&thisv->unk_2A4.y, thisv->unk_280, 0.8f, 7000.0f);
    }

    if (thisv->unk_284 != thisv->unk_2A4.z) {
        Math_ApproachF(&thisv->unk_2A4.z, thisv->unk_284, 0.8f, 7000.0f);
    }

    if (thisv->unk_268 != thisv->unk_2B0.x) {
        Math_ApproachF(&thisv->unk_2B0.x, thisv->unk_268, 0.8f, 7000.0f);
    }

    if (thisv->unk_278 != thisv->unk_2B0.y) {
        Math_ApproachF(&thisv->unk_2B0.y, thisv->unk_278, 0.8f, 7000.0f);
    }

    if (thisv->unk_27C != thisv->unk_2B0.z) {
        Math_ApproachF(&thisv->unk_2B0.z, thisv->unk_27C, 0.8f, 7000.0f);
    }
}

void func_80B11DEC(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    if (thisv->unk_29E != 0) {
        Actor_SetScale(&thisv->actor, thisv->unk_2F4);
    }

    thisv->actionFunc = func_80B11E78;
}

void func_80B11E78(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    Vec3f dustVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.2f, 0.0f };
    Color_RGBA8 dustPrimColor = { 0, 0, 0, 255 };
    Color_RGBA8 dustEnvColor = { 0, 0, 0, 255 };
    Vec3f dustPos;
    f32 tmpf2;
    f32 sp4C;
    f32 sp50;
    f32 tmpf1;
    s16 sp4A;

    if ((thisv->unk_29C != 0) && (thisv->unk_29E == 0) && (thisv->actor.bgCheckFlags & 1)) {
        thisv->unk_29C = 0;
        thisv->actionFunc = func_80B123A8;
        return;
    }

    sp4A = 0;
    if ((thisv->unk_25E == 0) && (thisv->unk_25C == 0)) {
        thisv->unk_294++;
        if (thisv->unk_294 >= 8) {
            thisv->unk_25E = Rand_ZeroFloat(30.0f);
            thisv->unk_294 = Rand_ZeroFloat(3.99f);

            switch (thisv->unk_29E) {
                case 0:
                    sp50 = Rand_CenteredFloat(100.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 100.0f;
                    } else {
                        sp50 += 100.0f;
                    }

                    sp4C = Rand_CenteredFloat(100.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 100.0f;
                    } else {
                        sp4C += 100.0f;
                    }

                    thisv->unk_2E8.x = thisv->unk_2DC.x + sp50;
                    thisv->unk_2E8.z = thisv->unk_2DC.z + sp4C;

                    if (thisv->unk_2E8.x < -150.0f) {
                        thisv->unk_2E8.x = -150.0f;
                    }

                    if (thisv->unk_2E8.x > 150.0f) {
                        thisv->unk_2E8.x = 150.0f;
                    }

                    if (thisv->unk_2E8.z < -60.0f) {
                        thisv->unk_2E8.z = -60.0f;
                    }

                    if (thisv->unk_2E8.z > -40.0f) {
                        thisv->unk_2E8.z = -40.0f;
                    }
                    break;

                case 1:
                    sp50 = Rand_CenteredFloat(50.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 50.0f;
                    } else {
                        sp50 += 50.0f;
                    }

                    sp4C = Rand_CenteredFloat(30.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 30.0f;
                    } else {
                        sp4C += 30.0f;
                    }

                    thisv->unk_2E8.x = thisv->unk_2DC.x + sp50;
                    thisv->unk_2E8.z = thisv->unk_2DC.z + sp4C;
                    break;
            }
        } else {
            thisv->unk_25C = 4;
            if (thisv->actor.bgCheckFlags & 1) {
                thisv->actor.velocity.y = 2.5f;
                if ((Rand_ZeroFloat(10.0f) < 1.0f) && (thisv->unk_29E == 0)) {
                    thisv->unk_25C = 0xC;
                    thisv->actor.velocity.y = 10.0f;
                }
            }
        }
    }
    if (thisv->unk_25C != 0) {
        sp4A = 1;
        Math_ApproachF(&thisv->actor.world.pos.x, thisv->unk_2E8.x, 1.0f, thisv->unk_2C8.y);
        Math_ApproachF(&thisv->actor.world.pos.z, thisv->unk_2E8.z, 1.0f, thisv->unk_2C8.y);
        Math_ApproachF(&thisv->unk_2C8.y, 3.0f, 1.0f, 0.3f);
        tmpf1 = thisv->unk_2E8.x - thisv->actor.world.pos.x;
        tmpf2 = thisv->unk_2E8.z - thisv->actor.world.pos.z;

        if (fabsf(tmpf1) < 10.0f) {
            tmpf1 = 0;
        }

        if (fabsf(tmpf2) < 10.0f) {
            tmpf2 = 0.0f;
        }

        if ((tmpf1 == 0.0f) && (tmpf2 == 0.0f)) {
            thisv->unk_25C = 0;
            thisv->unk_294 = 7;
        }

        Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_FAtan2F(tmpf1, tmpf2) * (0x8000 / std::numbers::pi_v<float>), 3, thisv->unk_2C8.z,
                           0);
        Math_ApproachF(&thisv->unk_2C8.z, 10000.0f, 1.0f, 1000.0f);
    }

    if (thisv->unk_260 == 0) {
        func_80B11A94(thisv, globalCtx, sp4A);
        return;
    }

    if ((globalCtx->gameplayFrames % 4) == 0) {
        dustVelocity.y = Rand_CenteredFloat(5.0f);
        dustAccel.y = 0.2f;
        dustPos = thisv->actor.world.pos;
        func_8002836C(globalCtx, &dustPos, &dustVelocity, &dustAccel, &dustPrimColor, &dustEnvColor, 600, 40, 30);
    }
}

void func_80B123A8(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    thisv->unk_27C = 6000.0f;
    thisv->unk_288 = -10000.0f;
    thisv->unk_2B0.z = 6000.0f;
    thisv->unk_2B0.y = 10000.0f;
    thisv->actionFunc = func_80B12460;
    thisv->unk_2A4.z = 6000.0f;
    thisv->unk_284 = 6000.0f;
    thisv->unk_2B0.x = -10000.0f;
    thisv->unk_268 = -10000.0f;
    thisv->unk_2A4.y = -10000.0f;
    thisv->unk_2A4.x = -10000.0f;
    thisv->unk_26C = -10000.0f;
}

void func_80B12460(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 phi_f16 = 0.0f;

    player->actor.freezeTimer = 10;
    switch (thisv->unk_29A) {
        case 0:
            thisv->unk_296 = 2;
            thisv->unk_2C8.y = 0.0f;
            thisv->unk_29A = 1;
            break;

        case 1:
            thisv->actor.speedXZ = 2.0f;
            if (thisv->unk_25C == 0) {
                thisv->unk_25C = 3;
                thisv->actor.velocity.y = 3.5f;
            }

            if (thisv->unk_25A == 0) {
                thisv->unk_298++;
                thisv->unk_298 &= 1;
                thisv->unk_25A = 5;
            }

            phi_f16 = (thisv->unk_298 == 0) ? 5000.0f : -5000.0f;
            if (thisv->actor.world.pos.z > 100.0f) {
                thisv->actor.speedXZ = 2.0f;
                thisv->actor.gravity = -0.3f;
                thisv->actor.velocity.y = 5.0f;
                thisv->unk_29A = 2;
            }
            break;

        case 2:
            if ((player->actor.world.pos.z - 40.0f) < thisv->actor.world.pos.z) {
                thisv->actor.speedXZ = 0.0f;
            }

            if ((thisv->actor.bgCheckFlags & 1) && (thisv->actor.world.pos.z > 110.0f)) {
                thisv->actor.velocity.y = 0.0f;
                thisv->actor.gravity = 0.0f;
                thisv->unk_284 = 0.0f;
                thisv->unk_27C = 0.0f;
                thisv->unk_278 = 0.0f;
                thisv->unk_280 = 0.0f;
                thisv->unk_288 = 0.0f;
                thisv->actor.speedXZ = 0.5f;
                thisv->unk_254 = thisv->unk_256 = 0;
                thisv->unk_28E = thisv->unk_290 = 0;
                thisv->unk_296 = 1;
                thisv->unk_29A = 3;
            }
            break;

        case 3:
            if ((player->actor.world.pos.z - 50.0f) < thisv->actor.world.pos.z) {
                thisv->actor.speedXZ = 0.0f;
                thisv->unk_262 = 0x3C;
                thisv->unk_25A = 0x14;
                thisv->unk_264 = 10000.0f;
                thisv->unk_29A = 4;
            }
            break;

        case 4:
            if (thisv->unk_25A == 0) {
                thisv->unk_296 = 4;
                thisv->unk_264 = 5000.0f;
                thisv->unk_26C = 0.0f;
                thisv->unk_268 = 0.0f;
                thisv->unk_284 = 0.0f;
                thisv->unk_27C = 0.0f;
                thisv->unk_280 = 14000.0f;
                thisv->unk_278 = 14000.0f;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_M);
                thisv->unk_254 = thisv->unk_256 = thisv->unk_25A = 0x1E;
                thisv->unk_29A = 5;
            }
            break;

        case 5:
            if (thisv->unk_25A == 1) {
                thisv->unk_258 = 0;
                thisv->unk_296 = 5;
                thisv->unk_256 = thisv->unk_258;
                thisv->unk_254 = thisv->unk_258;
                thisv->actor.speedXZ = 1.0f;
            }

            if ((thisv->unk_25A == 0) && ((player->actor.world.pos.z - 30.0f) < thisv->actor.world.pos.z)) {
                Audio_PlaySoundGeneral(NA_SE_VO_LI_DOWN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                thisv->unk_25E = 0x14;
                thisv->unk_29A = 6;
                thisv->actor.speedXZ = 0.0f;
            }
            break;

        case 6:
            if (thisv->unk_25E == 1) {
                globalCtx->sceneLoadFlag = 0x14;
                globalCtx->nextEntranceIndex = gSaveContext.entranceIndex;
                globalCtx->shootingGalleryStatus = 0;
                player->actor.freezeTimer = 20;
                thisv->unk_25E = 0x14;
                thisv->actionFunc = func_80B128D8;
            }
            break;
    }

    Math_SmoothStepToS(&thisv->actor.world.rot.y,
                       (s16)(Math_FAtan2F(player->actor.world.pos.x - thisv->actor.world.pos.x,
                                          player->actor.world.pos.z - thisv->actor.world.pos.z) *
                             (0x8000 / std::numbers::pi_v<float>)) +
                           phi_f16,
                       5, thisv->unk_2C8.y, 0);
    Math_ApproachF(&thisv->unk_2C8.y, 3000.0f, 1.0f, 500.0f);
    if (thisv->unk_296 == 2) {
        thisv->unk_256 = 10;
        thisv->unk_254 = thisv->unk_256;
    }

    func_80B11A94(thisv, globalCtx, thisv->unk_296);
}

void func_80B128D8(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_25E == 1) {
        gSaveContext.timer1State = 0;
    }
}

void func_80B128F8(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    s16 sp26;
    s16 sp24;

    Actor_SetFocus(&thisv->actor, thisv->unk_2D4);
    Actor_GetScreenPos(globalCtx, &thisv->actor, &sp26, &sp24);
    if ((thisv->actor.projectedPos.z > 200.0f) && (thisv->actor.projectedPos.z < 800.0f) && (sp26 > 0) &&
        (sp26 < SCREEN_WIDTH) && (sp24 > 0) && (sp24 < SCREEN_HEIGHT)) {
        thisv->actor.speedXZ = 5.0f;
        thisv->unk_298 = Rand_ZeroFloat(1.99f);
        thisv->unk_2D8 = Rand_CenteredFloat(8000.0f) + -10000.0f;
        thisv->unk_262 = 0x1E;
        thisv->unk_25E = 0x64;
        thisv->actionFunc = func_80B129EC;
    }
}

void func_80B129EC(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    f32 phi_f2;
    s16 sp2E;
    s16 sp2C;
    f32 tmpf2;

    Actor_SetFocus(&thisv->actor, thisv->unk_2D4);
    Actor_GetScreenPos(globalCtx, &thisv->actor, &sp2E, &sp2C);
    if ((thisv->unk_25E == 0) || (thisv->actor.projectedPos.z < -70.0f) || (sp2E < 0) || (sp2E > SCREEN_WIDTH) ||
        (sp2C < 0) || (sp2C > SCREEN_HEIGHT)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->unk_2A0 = 1;
    if (thisv->unk_25C == 0) {
        thisv->unk_298++;
        thisv->unk_298 &= 1;
        thisv->unk_25C = (s16)Rand_CenteredFloat(4.0f) + 5;
        if ((Rand_ZeroFloat(5.0f) < 1.0f) && (thisv->actor.bgCheckFlags & 1)) {
            thisv->actor.velocity.y = 4.0f;
        }
    }

    phi_f2 = (thisv->unk_298 == 0) ? 5000.0f : -5000.0f;
    tmpf2 = thisv->unk_2D8 + phi_f2;
    Math_SmoothStepToS(&thisv->actor.world.rot.y, tmpf2, 3, thisv->unk_2C8.y, 0);
    Math_ApproachF(&thisv->unk_2C8.y, 3000.0f, 1.0f, 500.0f);
    func_80B11A94(thisv, globalCtx, 2);
}

void func_80B12BA4(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        switch (thisv->unk_29E) {
            case 0:
                if (thisv->unk_29C == 0) {
                    thisv->unk_262 = 0x1E;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_A);
                    thisv->unk_29C = 1;
                    thisv->unk_2A0 = 1;
                    thisv->actionFunc = func_80B123A8;
                    thisv->actor.gravity = -3.0f;
                }
                break;

            case 1:
                thisv->unk_262 = 0x1E;
                thisv->unk_2F8 = 1;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_A);
                thisv->unk_260 = 100;
                thisv->unk_2A0 = 1;
                thisv->unk_25E = thisv->unk_260;
                break;
        }
    }
}

void EnSyatekiNiw_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSyatekiNiw* thisv = (EnSyatekiNiw*)thisx;
    s32 pad;
    s16 i;
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;

    if (1) {}
    if (1) {}
    if (1) {}

    func_80B132A8(thisv, globalCtx);
    thisv->unk_28C++;
    if (thisv->unk_254 != 0) {
        thisv->unk_254--;
    }

    if (thisv->unk_258 != 0) {
        thisv->unk_258--;
    }

    if (thisv->unk_25A != 0) {
        thisv->unk_25A--;
    }

    if (thisv->unk_25C != 0) {
        thisv->unk_25C--;
    }

    if (thisv->unk_25E != 0) {
        thisv->unk_25E--;
    }

    if (thisv->unk_262 != 0) {
        thisv->unk_262--;
    }

    if (thisv->unk_260 != 0) {
        thisv->unk_260--;
    }

    thisv->actor.shape.rot = thisv->actor.world.rot;
    thisv->actor.shape.shadowScale = 15.0f;

    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 0x1D);

    if (thisv->unk_2A0 != 0) {
        for (i = 0; i < 20; i++) {
            sp78.x = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.x;
            sp78.y = Rand_CenteredFloat(10.0f) + (thisv->actor.world.pos.y + 20.0f);
            sp78.z = Rand_CenteredFloat(10.0f) + thisv->actor.world.pos.z;
            sp6C.x = Rand_CenteredFloat(3.0f);
            sp6C.y = (Rand_ZeroFloat(2.0f) * 0.5f) + 2.0f;
            sp6C.z = Rand_CenteredFloat(3.0f);
            sp60.z = sp60.x = 0.0f;
            sp60.y = -0.15f;
            func_80B131B8(thisv, &sp78, &sp6C, &sp60, Rand_ZeroFloat(8.0f) + 8.0f);
        }

        thisv->unk_2A0 = 0;
    }

    func_80B12BA4(thisv, globalCtx);
    if (thisv->unk_262 == 0) {
        if (thisv->actionFunc == func_80B11E78) {
            thisv->unk_262 = 0x12C;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_N);
        } else {
            thisv->unk_262 = 0x1E;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_A);
        }
    }

    i = 0;
    switch (thisv->unk_29E) {
        case 0:
            if (globalCtx->shootingGalleryStatus != 0) {
                i = 1;
            }
            break;

        case 1:
            i = 1;
            break;
    }

    if (i != 0) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

s32 SyatekiNiw_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnSyatekiNiw* thisv = (EnSyatekiNiw*)thisx;
    Vec3f sp0 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 13) {
        rot->y += (s16)thisv->unk_2BC.x;
    }

    if (limbIndex == 11) {
        rot->x += (s16)thisv->unk_2B0.z;
        rot->y += (s16)thisv->unk_2B0.y;
        rot->z += (s16)thisv->unk_2B0.x;
    }

    if (limbIndex == 7) {
        rot->x += (s16)thisv->unk_2A4.z;
        rot->y += (s16)thisv->unk_2A4.y;
        rot->z += (s16)thisv->unk_2A4.x;
    }

    return false;
}

void EnSyatekiNiw_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSyatekiNiw* thisv = (EnSyatekiNiw*)thisx;
    Color_RGBA8 sp30 = { 0, 0, 0, 255 };

    if (thisv->actionFunc != func_80B128F8) {
        func_80093D18(globalCtx->state.gfxCtx);
        if (thisv->unk_260 != 0) {
            func_80026230(globalCtx, &sp30, 0, 0x14);
        }

        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, SyatekiNiw_OverrideLimbDraw, NULL, thisv);
        func_80026608(globalCtx);
        func_80B13464(thisv, globalCtx);
    }
}

void func_80B131B8(EnSyatekiNiw* thisv, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4) {
    s16 i;
    EnSyatekiNiw_1* ptr = &thisv->unk_348[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 == 0) {
            ptr->unk_00 = 1;
            ptr->unk_04 = *arg1;
            ptr->unk_10 = *arg2;
            ptr->unk_1C = *arg3;
            ptr->unk_34 = 0;
            ptr->unk_2C = (arg4 / 1000.0f);
            ptr->unk_28 = (s16)Rand_ZeroFloat(20.0f) + 0x28;
            ptr->unk_2A = Rand_ZeroFloat(1000.0f);
            return;
        }
    }
}

void func_80B132A8(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    s16 i;
    EnSyatekiNiw_1* ptr = &thisv->unk_348[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 != 0) {
            ptr->unk_04.x += ptr->unk_10.x;
            ptr->unk_04.y += ptr->unk_10.y;
            ptr->unk_04.z += ptr->unk_10.z;
            ptr->unk_34++;
            ptr->unk_10.x += ptr->unk_1C.x;
            ptr->unk_10.y += ptr->unk_1C.y;
            ptr->unk_10.z += ptr->unk_1C.z;
            if (ptr->unk_00 == 1) {
                ptr->unk_2A++;
                Math_ApproachF(&ptr->unk_10.x, 0.0f, 1.0f, 0.05f);
                Math_ApproachF(&ptr->unk_10.z, 0.0f, 1.0f, 0.05f);
                if (ptr->unk_10.y < -0.5f) {
                    ptr->unk_10.y = 0.5f;
                }

                ptr->unk_30 = (Math_SinS(ptr->unk_2A * 3000) * std::numbers::pi_v<float>) * 0.2f;
                if (ptr->unk_28 < ptr->unk_34) {
                    ptr->unk_00 = 0;
                }
            }
        }
    }
}

void func_80B13464(EnSyatekiNiw* thisv, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s16 i;
    EnSyatekiNiw_1* ptr = &thisv->unk_348[0];
    u8 flag = 0;

    OPEN_DISPS(gfxCtx, "../z_en_syateki_niw.c", 1234);

    func_80093D84(globalCtx->state.gfxCtx);

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 == 1) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gCuccoParticleAppearDL);
                flag++;
            }

            Matrix_Translate(ptr->unk_04.x, ptr->unk_04.y, ptr->unk_04.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(ptr->unk_2C, ptr->unk_2C, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(ptr->unk_30, MTXMODE_APPLY);
            Matrix_Translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_syateki_niw.c", 1251),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gCuccoParticleAliveDL);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_en_syateki_niw.c", 1257);
}
