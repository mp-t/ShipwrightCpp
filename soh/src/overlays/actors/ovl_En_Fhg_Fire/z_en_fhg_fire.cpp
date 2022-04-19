/*
 * File: z_en_fhg_fire.c
 * Overlay: ovl_En_Fhg_Fire
 * Description: Phantom Ganon's Lighting Attack
 */

#include "z_en_fhg_fire.h"
#include "objects/object_fhg/object_fhg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "overlays/actors/ovl_En_fHG/z_en_fhg.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /*  0 */ STRIKE_INIT,
    /* 10 */ STRIKE_BURST = 10,
    /* 11 */ STRIKE_TRAILS
} StrikeMode;

typedef enum {
    /* 0 */ TRAIL_INIT,
    /* 1 */ TRAIL_APPEAR,
    /* 2 */ TRAIL_DISSIPATE
} TrailMode;

typedef enum {
    /* 0 */ BALL_FIZZLE,
    /* 1 */ BALL_BURST,
    /* 2 */ BALL_IMPACT
} BallKillMode;

void EnFhgFire_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFhgFire_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFhgFire_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFhgFire_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFhgFire_LightningStrike(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_LightningTrail(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_LightningShock(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_LightningBurst(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_SpearLight(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_EnergyBall(EnFhgFire* thisv, GlobalContext* globalCtx);
void EnFhgFire_PhantomWarp(EnFhgFire* thisv, GlobalContext* globalCtx);

ActorInit En_Fhg_Fire_InitVars = {
    0,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_FHG,
    sizeof(EnFhgFire),
    (ActorFunc)EnFhgFire_Init,
    (ActorFunc)EnFhgFire_Destroy,
    (ActorFunc)EnFhgFire_Update,
    (ActorFunc)EnFhgFire_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK6,
        { 0x00100700, 0x03, 0x20 },
        { 0x0D900700, 0x00, 0x00 },
        TOUCH_ON,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, 10, { 0, 0, 0 } },
};

void EnFhgFire_SetUpdate(EnFhgFire* thisv, EnFhgFireUpdateFunc updateFunc) {
    thisv->updateFunc = updateFunc;
}

void EnFhgFire_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFhgFire* thisv = (EnFhgFire*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    if ((thisv->actor.params == FHGFIRE_LIGHTNING_SHOCK) || (thisv->actor.params == FHGFIRE_LIGHTNING_BURST) ||
        (thisv->actor.params == FHGFIRE_ENERGY_BALL)) {
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    }
    thisv->fwork[FHGFIRE_ALPHA] = 200.0f;
    Actor_SetScale(&thisv->actor, 0.0f);

    if (thisv->actor.params == FHGFIRE_LIGHTNING_STRIKE) {
        EnFhgFire_SetUpdate(thisv, EnFhgFire_LightningStrike);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_THUNDER);
    } else if (thisv->actor.params >= FHGFIRE_LIGHTNING_TRAIL) {
        EnFhgFire_SetUpdate(thisv, EnFhgFire_LightningTrail);
        thisv->actor.shape.rot = thisv->actor.world.rot;
    }
    if (thisv->actor.params == FHGFIRE_LIGHTNING_SHOCK) {
        thisv->actor.draw = NULL;
        EnFhgFire_SetUpdate(thisv, EnFhgFire_LightningShock);
        thisv->actor.speedXZ = 30.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_SPARK);
    } else if (thisv->actor.params == FHGFIRE_LIGHTNING_BURST) {
        EnFhgFire_SetUpdate(thisv, EnFhgFire_LightningBurst);
        thisv->fwork[FHGFIRE_ALPHA] = 255.0f;
        thisv->work[FHGFIRE_TIMER] = 32;
        thisv->work[FHGFIRE_FX_TIMER] = 50;
        thisv->lensFlareTimer = 10;

        thisv->fwork[FHGFIRE_BURST_SCALE] = thisv->actor.world.rot.x / 100.0f;
        thisv->collider.dim.radius = thisv->actor.world.rot.x * 0.13f;
        thisv->collider.dim.height = thisv->actor.world.rot.x * 0.13f;
        thisv->collider.dim.yShift = 0;
    } else if (thisv->actor.params == FHGFIRE_SPEAR_LIGHT) {
        osSyncPrintf("yari hikari ct 1\n"); // "light spear"
        EnFhgFire_SetUpdate(thisv, EnFhgFire_SpearLight);
        osSyncPrintf("yari hikari ct 2\n");
        thisv->work[FHGFIRE_TIMER] = thisv->actor.world.rot.x;
        thisv->work[FHGFIRE_FIRE_MODE] = thisv->actor.world.rot.y;
    } else if ((thisv->actor.params == FHGFIRE_WARP_EMERGE) || (thisv->actor.params == FHGFIRE_WARP_RETREAT) ||
               (thisv->actor.params == FHGFIRE_WARP_DEATH)) {
        Actor_SetScale(&thisv->actor, 7.0f);
        EnFhgFire_SetUpdate(thisv, EnFhgFire_PhantomWarp);
        if (thisv->actor.params == FHGFIRE_WARP_DEATH) {
            thisv->work[FHGFIRE_TIMER] = 440;
            thisv->actor.scale.z = 1.0f;
        } else {
            thisv->work[FHGFIRE_TIMER] = 76;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_S);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_S2);
        }
    } else if (thisv->actor.params == FHGFIRE_ENERGY_BALL) {
        f32 dxL;
        f32 dyL;
        f32 dzL;
        f32 dxzL;

        thisv->actor.speedXZ = (thisv->actor.world.rot.x == 0) ? 8.0f : 3.0f;
        EnFhgFire_SetUpdate(thisv, EnFhgFire_EnergyBall);

        thisv->work[FHGFIRE_TIMER] = 70;
        thisv->work[FHGFIRE_FX_TIMER] = 2;

        dxL = player->actor.world.pos.x - thisv->actor.world.pos.x;
        dyL = player->actor.world.pos.y + 30.0f - thisv->actor.world.pos.y;
        dzL = player->actor.world.pos.z - thisv->actor.world.pos.z;
        thisv->actor.world.rot.y = Math_FAtan2F(dxL, dzL) * (0x8000 / std::numbers::pi_v<float>);
        dxzL = sqrtf(SQ(dxL) + SQ(dzL));
        thisv->actor.world.rot.x = Math_FAtan2F(dyL, dxzL) * (0x8000 / std::numbers::pi_v<float>);
        thisv->collider.dim.radius = 40;
        thisv->collider.dim.height = 50;
        thisv->collider.dim.yShift = -25;
        thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                  thisv->actor.world.pos.z, 255, 255, 255, 255);
    }
}

void EnFhgFire_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFhgFire* thisv = (EnFhgFire*)thisx;

    if ((thisv->actor.params == FHGFIRE_LIGHTNING_SHOCK) || (thisv->actor.params == FHGFIRE_LIGHTNING_BURST) ||
        (thisv->actor.params == FHGFIRE_ENERGY_BALL)) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }

    if (thisv->actor.params == FHGFIRE_ENERGY_BALL) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    }
}

void EnFhgFire_LightningStrike(EnFhgFire* thisv, GlobalContext* globalCtx) {
    Camera* camera = Gameplay_GetCamera(globalCtx, 0);
    s16 i;

    switch (thisv->work[FHGFIRE_FIRE_MODE]) {
        case STRIKE_INIT:
            thisv->work[FHGFIRE_FIRE_MODE] = STRIKE_BURST;
            thisv->work[FHGFIRE_TIMER] = 7;
            break;
        case STRIKE_BURST:
            thisv->actor.shape.rot.y =
                Camera_GetInputDirYaw(camera) + 0x8000 * (thisv->work[FHGFIRE_VARIANCE_TIMER] & 0xFF);
            Math_ApproachF(&thisv->fwork[FHGFIRE_SCALE], 1.0f, 1.0f, 0.2f);

            if (thisv->work[FHGFIRE_TIMER] == 0) {
                thisv->work[FHGFIRE_FIRE_MODE] = STRIKE_TRAILS;
                thisv->actor.shape.rot.z += 0x8000;
                thisv->work[FHGFIRE_TIMER] = 37;
                thisv->actor.world.pos.y -= 200.0f;

                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                   thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 500, 0, 0,
                                   FHGFIRE_LIGHTNING_BURST);
                {
                    Vec3f sp7C;
                    Vec3f sp70 = { 0.0f, -1.0f, 0.0f };

                    for (i = 0; i < 35; i++) {
                        sp7C.x = Rand_CenteredFloat(30.f);
                        sp7C.y = Rand_ZeroFloat(5.0f) + 3.0f;
                        sp7C.z = Rand_CenteredFloat(30.f);
                        sp70.y = -0.2f;
                        EffectSsFhgFlash_SpawnLightBall(globalCtx, &thisv->actor.world.pos, &sp7C, &sp70,
                                                        (s16)(Rand_ZeroOne() * 100.0f) + 240, FHGFLASH_LIGHTBALL_GREEN);
                    }
                }
                func_80033E88(&thisv->actor, globalCtx, 4, 10);
            }

            break;
        case STRIKE_TRAILS:
            thisv->actor.shape.rot.y =
                Camera_GetInputDirYaw(camera) + (thisv->work[FHGFIRE_VARIANCE_TIMER] & 0xFF) * 0x8000;

            Math_ApproachF(&thisv->fwork[FHGFIRE_SCALE], 0.0f, 1.0f, 0.2f);
            if (thisv->work[FHGFIRE_TIMER] == 30) {
                s16 randY = (Rand_ZeroOne() < 0.5f) ? 0x1000 : 0;

                for (i = 0; i < 8; i++) {
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                       thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0,
                                       (i * 0x2000) + randY, 0x4000, FHGFIRE_LIGHTNING_TRAIL + i);
                }

                for (i = 0; i < 8; i++) {
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                       thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0,
                                       (i * 0x2000) + randY, 0, FHGFIRE_LIGHTNING_SHOCK);
                }
            }

            if (thisv->work[FHGFIRE_TIMER] == 0) {
                Actor_Kill(&thisv->actor);
            }
    }

    Actor_SetScale(&thisv->actor, thisv->fwork[FHGFIRE_SCALE]);
}

void EnFhgFire_LightningTrail(EnFhgFire* thisv, GlobalContext* globalCtx) {
    osSyncPrintf("FF MOVE 1\n");
    thisv->actor.shape.rot.x += (s16)(Rand_ZeroOne() * 4000.0f) + 0x4000;

    switch (thisv->work[FHGFIRE_FIRE_MODE]) {
        case TRAIL_INIT:
            thisv->work[FHGFIRE_FIRE_MODE] = TRAIL_APPEAR;
            thisv->work[FHGFIRE_TIMER] = (s16)(Rand_ZeroOne() * 7.0f) + 7;
        case TRAIL_APPEAR:
            Math_ApproachF(&thisv->fwork[FHGFIRE_SCALE], 1.7f, 1.0f, 0.34f);

            if (thisv->work[FHGFIRE_TIMER] == 0) {
                thisv->work[FHGFIRE_FIRE_MODE] = TRAIL_DISSIPATE;
                thisv->work[FHGFIRE_TIMER] = 10;
                thisv->actor.world.pos.z += Math_SinS(thisv->actor.shape.rot.y) * -200.0f * thisv->fwork[FHGFIRE_SCALE];
                thisv->actor.world.pos.x += Math_CosS(thisv->actor.shape.rot.y) * 200.0f * thisv->fwork[FHGFIRE_SCALE];
                thisv->actor.shape.rot.y += 0x8000;
            }
            break;
        case TRAIL_DISSIPATE:
            Math_ApproachZeroF(&thisv->fwork[FHGFIRE_SCALE], 1.0f, 0.34f);
            if (thisv->work[FHGFIRE_TIMER] == 0) {
                Actor_Kill(&thisv->actor);
            }
            break;
    }

    Actor_SetScale(&thisv->actor, thisv->fwork[FHGFIRE_SCALE]);
    if (thisv->actor.scale.x > 1.0f) {
        thisv->actor.scale.x = 1.0f;
    }

    osSyncPrintf("FF MOVE 2\n");
}

void EnFhgFire_LightningShock(EnFhgFire* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f pos;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
    }

    if (Rand_ZeroOne() < 0.5f) {
        pos = thisv->actor.world.pos;
        pos.y -= 20.0f;
        EffectSsFhgFlash_SpawnShock(globalCtx, &thisv->actor, &pos, 200, FHGFLASH_SHOCK_NO_ACTOR);
    }

    Actor_MoveForward(&thisv->actor);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    if (player->invincibilityTimer == 0) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 1);
    if (thisv->actor.bgCheckFlags & 8) {
        Actor_Kill(&thisv->actor);
    }
}

void EnFhgFire_LightningBurst(EnFhgFire* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    globalCtx->envCtx.fillScreen = true;
    thisv->actor.shape.rot.y += 0x1000;

    if (thisv->work[FHGFIRE_FX_TIMER] == 49) {
        globalCtx->envCtx.unk_BF = 1;
        globalCtx->envCtx.unk_D6 = 0xFF;
    }
    if (thisv->work[FHGFIRE_FX_TIMER] == 31) {
        globalCtx->envCtx.unk_BF = 0x00;
        globalCtx->envCtx.unk_D6 = 0x14;
    }
    if (thisv->work[FHGFIRE_FX_TIMER] >= 48) {
        globalCtx->envCtx.screenFillColor[0] = globalCtx->envCtx.screenFillColor[1] =
            globalCtx->envCtx.screenFillColor[2] = 255;

        if ((thisv->work[FHGFIRE_TIMER] % 2) != 0) {
            globalCtx->envCtx.screenFillColor[3] = 70;
        } else {
            globalCtx->envCtx.screenFillColor[3] = 0;
        }
    } else {
        globalCtx->envCtx.screenFillColor[3] = 0;
    }

    if (thisv->work[FHGFIRE_TIMER] <= 20) {
        Math_ApproachZeroF(&thisv->fwork[FHGFIRE_ALPHA], 1.0f, 45.0f);
        Math_ApproachZeroF(&thisv->fwork[FHGFIRE_SCALE], 1.0f, 0.5f);
    } else {
        Math_ApproachF(&thisv->fwork[FHGFIRE_SCALE], thisv->fwork[FHGFIRE_BURST_SCALE], 0.5f, 3.0f);
    }

    Actor_SetScale(&thisv->actor, thisv->fwork[FHGFIRE_SCALE]);
    if (thisv->fwork[FHGFIRE_BURST_SCALE] > 3.0f) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (player->invincibilityTimer == 0) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    if (thisv->work[FHGFIRE_TIMER] == 0) {
        Actor_Kill(&thisv->actor);
        globalCtx->envCtx.fillScreen = false;
    }

    if (thisv->lensFlareTimer != 0) {
        thisv->lensFlareTimer--;
        thisv->lensFlareOn = true;
        Math_ApproachF(&thisv->lensFlareScale, 40.0f, 0.3f, 10.0f);
    } else {
        Math_ApproachZeroF(&thisv->lensFlareScale, 1.0f, 5.0f);
        if (thisv->lensFlareScale == 0.0f) {
            thisv->lensFlareOn = false;
        }
    }

    gCustomLensFlareOn = thisv->lensFlareOn;
    gCustomLensFlarePos = thisv->actor.world.pos;
    gLensFlareScale = thisv->lensFlareScale;
    gLensFlareColorIntensity = 10.0f;
    gLensFlareScreenFillAlpha = 0;
}

void EnFhgFire_SpearLight(EnFhgFire* thisv, GlobalContext* globalCtx) {
    BossGanondrof* bossGnd;
    s16 i;

    osSyncPrintf("yari hikari 1\n");
    bossGnd = (BossGanondrof*)thisv->actor.parent;
    if ((thisv->work[FHGFIRE_VARIANCE_TIMER] % 2) != 0) {
        Actor_SetScale(&thisv->actor, 6.0f);
    } else {
        Actor_SetScale(&thisv->actor, 5.25f);
    }

    thisv->actor.world.pos = bossGnd->spearTip;
    thisv->actor.shape.rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x4000;

    osSyncPrintf("yari hikari 2\n");
    if (thisv->work[FHGFIRE_FIRE_MODE] == FHGFIRE_LIGHT_GREEN) {
        Vec3f ballPos;
        Vec3f ballVel = { 0.0f, 0.0f, 0.0f };
        Vec3f ballAccel = { 0.0f, 0.0f, 0.0f };

        osSyncPrintf("FLASH !!\n");

        for (i = 0; i < 2; i++) {
            ballPos.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
            ballPos.y = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.y;
            ballPos.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
            ballAccel.y = -0.08f;

            EffectSsFhgFlash_SpawnLightBall(globalCtx, &ballPos, &ballVel, &ballAccel,
                                            (s16)(Rand_ZeroOne() * 80.0f) + 150, FHGFLASH_LIGHTBALL_GREEN);
        }
    }

    if (thisv->work[FHGFIRE_TIMER] == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnFhgFire_EnergyBall(EnFhgFire* thisv, GlobalContext* globalCtx) {
    f32 dxL;
    f32 dyL;
    f32 dzL;
    f32 dxzL;
    f32 dxPG;
    f32 dyPG;
    f32 dzPG;
    u8 killMode = BALL_FIZZLE;
    u8 canBottleReflect1;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->work[FHGFIRE_KILL_TIMER] != 0) {
        thisv->work[FHGFIRE_KILL_TIMER]--;
        if (thisv->work[FHGFIRE_KILL_TIMER] == 0) {
            Actor_Kill(&thisv->actor);
            return;
        }
    } else {
        s32 canBottleReflect2;
        BossGanondrof* bossGnd = (BossGanondrof*)thisv->actor.parent;

        dxPG = bossGnd->targetPos.x - thisv->actor.world.pos.x;
        dyPG = bossGnd->targetPos.y - thisv->actor.world.pos.y;
        dzPG = bossGnd->targetPos.z - thisv->actor.world.pos.z;
        dxL = player->actor.world.pos.x - thisv->actor.world.pos.x;
        dyL = player->actor.world.pos.y + 40.0f - thisv->actor.world.pos.y;
        dzL = player->actor.world.pos.z - thisv->actor.world.pos.z;
        func_8002D908(&thisv->actor);
        func_8002D7EC(&thisv->actor);
        if (thisv->work[FHGFIRE_VARIANCE_TIMER] & 1) {
            Actor_SetScale(&thisv->actor, 6.0f);
        } else {
            Actor_SetScale(&thisv->actor, 5.25f);
        }
        thisv->actor.shape.rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x4000;
        {
            u8 lightBallColor1 = FHGFLASH_LIGHTBALL_GREEN;
            s16 i1;
            Vec3f spD4;
            Vec3f spC8 = { 0.0f, 0.0f, 0.0f };
            Vec3f spBC = { 0.0f, 0.0f, 0.0f };

            if (thisv->work[FHGFIRE_FIRE_MODE] >= FHGFIRE_LIGHT_BLUE) {
                lightBallColor1 = FHGFLASH_LIGHTBALL_LIGHTBLUE;
            }
            for (i1 = 0; i1 < 3; i1++) {
                spD4.x = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.x;
                spD4.y = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.y;
                spD4.z = Rand_CenteredFloat(20.0f) + thisv->actor.world.pos.z;
                spBC.y = -0.08f;
                EffectSsFhgFlash_SpawnLightBall(globalCtx, &spD4, &spC8, &spBC, (s16)(Rand_ZeroOne() * 80.0f) + 150,
                                                lightBallColor1);
            }
        }
        switch (thisv->work[FHGFIRE_FIRE_MODE]) {
            case FHGFIRE_LIGHT_GREEN:
                canBottleReflect1 =
                    ((player->stateFlags1 & 2) &&
                     (ABS((s16)(player->actor.shape.rot.y - (s16)(bossGnd->actor.yawTowardsPlayer + 0x8000))) <
                      0x2000) &&
                     (sqrtf(SQ(dxL) + SQ(dyL) + SQ(dzL)) <= 25.0f))
                        ? true
                        : false;
                if ((thisv->collider.base.acFlags & AC_HIT) || canBottleReflect1) {
                    ColliderInfo* hurtbox = thisv->collider.info.acHitInfo;
                    s16 i2;
                    Vec3f spA8;
                    Vec3f sp9C = { 0.0f, -0.5f, 0.0f };
                    s16 angleModX;
                    s16 angleModY;

                    for (i2 = 0; i2 < 30; i2++) {
                        spA8.x = Rand_CenteredFloat(20.0f);
                        spA8.y = Rand_CenteredFloat(20.0f);
                        spA8.z = Rand_CenteredFloat(20.0f);
                        EffectSsFhgFlash_SpawnLightBall(globalCtx, &thisv->actor.world.pos, &spA8, &sp9C,
                                                        (s16)(Rand_ZeroOne() * 25.0f) + 50, FHGFLASH_LIGHTBALL_GREEN);
                    }
                    canBottleReflect2 = canBottleReflect1;
                    if (!canBottleReflect2 && (hurtbox->toucher.dmgFlags & 0x00100000)) {
                        killMode = BALL_IMPACT;
                        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(thisv->actor.xyzDistToPlayerSq, 0xFF, 0x14, 0x96);
                    } else {
                        if (bossGnd->flyMode == GND_FLY_NEUTRAL) {
                            angleModX = Rand_CenteredFloat(0x2000);
                            angleModY = Rand_CenteredFloat(0x2000);
                            thisv->actor.speedXZ = 15.0f;
                        } else {
                            angleModX = 0;
                            angleModY = 0;
                            thisv->work[FHGFIRE_RETURN_COUNT]++;
                            if ((thisv->work[FHGFIRE_RETURN_COUNT] > 3) && (Rand_ZeroOne() < 0.5f)) {
                                thisv->work[FHGFIRE_RETURN_COUNT] = 100;
                            }

                            if (!canBottleReflect2 && (player->swordAnimation >= 24)) {
                                thisv->actor.speedXZ = 20.0f;
                                thisv->work[FHGFIRE_RETURN_COUNT] = 4;
                            } else {
                                thisv->actor.speedXZ += 1.0f;
                            }
                        }
                        thisv->actor.world.rot.y = (s16)(Math_FAtan2F(dxPG, dzPG) * (0x8000 / std::numbers::pi_v<float>)) + angleModY;
                        thisv->actor.world.rot.x =
                            (s16)(Math_FAtan2F(dyPG, sqrtf((dxPG * dxPG) + (dzPG * dzPG))) * (0x8000 / std::numbers::pi_v<float>)) +
                            angleModX;
                        thisv->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_BLUE;
                        thisv->work[FHGFIRE_FX_TIMER] = 2;
                        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_REFLECT_MG, &player->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                        func_800AA000(thisv->actor.xyzDistToPlayerSq, 0xB4, 0x14, 0x64);
                    }
                } else if (sqrtf(SQ(dxL) + SQ(dyL) + SQ(dzL)) <= 25.0f) {
                    killMode = BALL_BURST;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
                    if ((bossGnd->flyMode >= GND_FLY_VOLLEY) && (thisv->work[FHGFIRE_RETURN_COUNT] >= 2)) {
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_LAUGH);
                    }
                    func_8002F698(globalCtx, &thisv->actor, 3.0f, thisv->actor.world.rot.y, 0.0f, 3, 0x10);
                }
                break;
            case FHGFIRE_LIGHT_BLUE:
                if ((bossGnd->flyMode == GND_FLY_RETURN) && (thisv->work[FHGFIRE_RETURN_COUNT] < 100)) {
                    thisv->actor.world.rot.y = Math_FAtan2F(dxPG, dzPG) * (0x8000 / std::numbers::pi_v<float>);
                    if ((sqrtf(SQ(dxPG) + SQ(dzPG)) < (150.0f + (thisv->actor.speedXZ * 8.0f)))) {
                        thisv->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_REFLECT;
                        bossGnd->returnSuccess = true;
                        thisv->work[FHGFIRE_TIMER] = 8;
                    }
                } else {
                    if (thisv->work[FHGFIRE_RETURN_COUNT] >= 100) {
                        if ((sqrtf(SQ(dxPG) + SQ(dyPG) + SQ(dzPG)) < 100.0f)) {
                            bossGnd->returnSuccess = true;
                        }
                        thisv->actor.world.rot.y = Math_FAtan2F(dxPG, dzPG) * (0x8000 / std::numbers::pi_v<float>);
                        thisv->actor.world.rot.x = Math_FAtan2F(dyPG, sqrtf(SQ(dxPG) + SQ(dzPG))) * (0x8000 / std::numbers::pi_v<float>);
                    }
                    if ((fabsf(dxPG) < 30.0f) && (fabsf(dzPG) < 30.0f) && (fabsf(dyPG) < 45.0f)) {
                        killMode = BALL_IMPACT;
                        bossGnd->returnCount = thisv->work[FHGFIRE_RETURN_COUNT] + 1;
                        Audio_PlaySoundGeneral(NA_SE_EN_FANTOM_HIT_THUNDER, &bossGnd->actor.projectedPos, 4,
                                               &D_801333E0, &D_801333E0, &D_801333E8);
                        Audio_PlaySoundGeneral(NA_SE_EN_FANTOM_DAMAGE, &bossGnd->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                    }
                }
                break;
            case FHGFIRE_LIGHT_REFLECT:
                if (thisv->work[FHGFIRE_TIMER] == 0) {
                    s16 i3;
                    Vec3f sp88;
                    Vec3f sp7C = { 0.0f, -0.5f, 0.0f };

                    for (i3 = 0; i3 < 30; i3++) {
                        sp88.x = Rand_CenteredFloat(20.0f);
                        sp88.y = Rand_CenteredFloat(20.0f);
                        sp88.z = Rand_CenteredFloat(20.0f);
                        EffectSsFhgFlash_SpawnLightBall(globalCtx, &thisv->actor.world.pos, &sp88, &sp7C,
                                                        (s16)(Rand_ZeroOne() * 40.0f) + 80, FHGFLASH_LIGHTBALL_GREEN);
                    }
                    thisv->actor.world.rot.y = Math_FAtan2F(dxL, dzL) * (0x8000 / std::numbers::pi_v<float>);
                    dxzL = sqrtf(SQ(dxL) + SQ(dzL));
                    thisv->actor.world.rot.x = Math_FAtan2F(dyL, dxzL) * (0x8000 / std::numbers::pi_v<float>);
                    thisv->work[FHGFIRE_FIRE_MODE] = FHGFIRE_LIGHT_GREEN;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_SWORD_REFLECT_MG);
                    thisv->actor.speedXZ += 2.0f;
                }
                break;
        }

        osSyncPrintf("F_FIRE_MODE %d\n", thisv->work[FHGFIRE_FIRE_MODE]);
        osSyncPrintf("fly_mode    %d\n", bossGnd->flyMode);
        if (thisv->work[FHGFIRE_FX_TIMER] == 0) {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 50.0f, 100.0f, 7);
            if ((thisv->actor.bgCheckFlags & 0x19) || killMode) {
                u8 lightBallColor2 = FHGFLASH_LIGHTBALL_GREEN;
                s16 i4;
                Vec3f sp6C;
                Vec3f sp60 = { 0.0f, -1.0f, 0.0f };

                if (thisv->work[FHGFIRE_FIRE_MODE] > FHGFIRE_LIGHT_GREEN) {
                    lightBallColor2 = FHGFLASH_LIGHTBALL_LIGHTBLUE;
                }
                for (i4 = 0; i4 < 30; i4++) {
                    sp6C.x = Rand_CenteredFloat(20.0f);
                    sp6C.y = Rand_CenteredFloat(20.0f);
                    sp6C.z = Rand_CenteredFloat(20.0f);
                    sp60.y = -0.1f;
                    EffectSsFhgFlash_SpawnLightBall(globalCtx, &thisv->actor.world.pos, &sp6C, &sp60,
                                                    (s16)(Rand_ZeroOne() * 50.0f) + 100, lightBallColor2);
                }
                if (killMode == BALL_BURST) {
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_FHG_FIRE,
                                       thisv->actor.world.pos.x, player->actor.world.pos.y + 20.0f,
                                       thisv->actor.world.pos.z, 0xC8, 0, 0, FHGFIRE_LIGHTNING_BURST);
                }
                bossGnd->flyMode = GND_FLY_NEUTRAL;
                thisv->work[FHGFIRE_KILL_TIMER] = 30;
                thisv->actor.draw = NULL;
                if (killMode == BALL_FIZZLE) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_THUNDER_GND);
                }
                return;
            } else {
                Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
                osSyncPrintf("BEFORE setAC   %d\n", thisv->collider.base.shape);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
                osSyncPrintf("AFTER  setAC\n");
            }
        }
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, (s16)thisv->actor.world.pos.x, (s16)thisv->actor.world.pos.y,
                                  (s16)thisv->actor.world.pos.z, 255, 255, 255, 200);
        if (thisv->actor.speedXZ > 20.0f) {
            thisv->actor.speedXZ = 20.0f;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_FIRE - SFX_FLAG);
        // "Why ah ah ah ah"
        osSyncPrintf("なぜだああああああああ      %d\n", thisv->work[FHGFIRE_VARIANCE_TIMER]);
    }
}

void EnFhgFire_PhantomWarp(EnFhgFire* thisv, GlobalContext* globalCtx) {
    EnfHG* horse = (EnfHG*)thisv->actor.parent;
    f32 scrollDirection;

    thisv->fwork[FHGFIRE_WARP_TEX_1_X] += 25.0f * thisv->fwork[FHGFIRE_WARP_TEX_SPEED];
    thisv->fwork[FHGFIRE_WARP_TEX_1_Y] -= 40.0f * thisv->fwork[FHGFIRE_WARP_TEX_SPEED];
    thisv->fwork[FHGFIRE_WARP_TEX_2_X] += 5.0f * thisv->fwork[FHGFIRE_WARP_TEX_SPEED];
    thisv->fwork[FHGFIRE_WARP_TEX_2_Y] -= 30.0f * thisv->fwork[FHGFIRE_WARP_TEX_SPEED];

    if (thisv->actor.params == FHGFIRE_WARP_DEATH) {
        if (thisv->work[FHGFIRE_TIMER] > 70) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_L - SFX_FLAG);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_L2 - SFX_FLAG);
        }

        if (thisv->work[FHGFIRE_TIMER] == 70) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_S);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FANTOM_WARP_S2);
        }
    }

    if (thisv->work[FHGFIRE_TIMER] > 50) {
        scrollDirection = 1.0f;
        if (thisv->actor.params > FHGFIRE_WARP_EMERGE) {
            scrollDirection = -1.0f;
        }
        Math_ApproachF(&thisv->fwork[FHGFIRE_WARP_TEX_SPEED], scrollDirection, 1.0f, 0.04f);
        Math_ApproachF(&thisv->fwork[FHGFIRE_WARP_ALPHA], 255.0f, 1.0f, 10.2f);
    } else if (thisv->work[FHGFIRE_TIMER] <= 25) {
        Math_ApproachZeroF(&thisv->fwork[FHGFIRE_WARP_TEX_SPEED], 1.0f, 0.04f);
        Math_ApproachZeroF(&thisv->fwork[FHGFIRE_WARP_ALPHA], 1.0f, 10.2f);
    }

    osSyncPrintf("EFC 1\n");
    if ((thisv->work[FHGFIRE_TIMER] == 0) || ((thisv->actor.params == FHGFIRE_WARP_EMERGE) && horse->fhgFireKillWarp)) {
        Actor_Kill(&thisv->actor);
    }
    osSyncPrintf("EFC 2\n");
}

void EnFhgFire_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFhgFire* thisv = (EnFhgFire*)thisx;

    thisv->work[FHGFIRE_VARIANCE_TIMER]++;

    if (thisv->work[FHGFIRE_TIMER] != 0) {
        thisv->work[FHGFIRE_TIMER]--;
    }
    if (thisv->work[FHGFIRE_FX_TIMER] != 0) {
        thisv->work[FHGFIRE_FX_TIMER]--;
    }

    thisv->updateFunc(thisv, globalCtx);
}

static const void* sDustTextures[] = {
    gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
};

void EnFhgFire_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnFhgFire* thisv = (EnFhgFire*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1723);

    if (thisv->actor.params == FHGFIRE_LIGHTNING_BURST) {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)thisv->fwork[FHGFIRE_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 165, 255, 75, 0);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1745),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gPhantomLightningBlastDL));
    } else if ((thisv->actor.params == FHGFIRE_SPEAR_LIGHT) || (thisv->actor.params == FHGFIRE_ENERGY_BALL)) {
        osSyncPrintf("yari hikari draw 1\n");
        Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)thisv->fwork[FHGFIRE_ALPHA]);

        if (thisv->work[FHGFIRE_FIRE_MODE] > FHGFIRE_LIGHT_GREEN) {
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 165, 255, 75, 0);
        }
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_RotateZ((thisv->actor.shape.rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1801),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);
    } else if ((thisv->actor.params == FHGFIRE_WARP_EMERGE) || (thisv->actor.params == FHGFIRE_WARP_RETREAT) ||
               (thisv->actor.params == FHGFIRE_WARP_DEATH)) {
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (u8)thisv->fwork[FHGFIRE_WARP_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 90, 50, 95, (s8)(thisv->fwork[FHGFIRE_WARP_ALPHA] * 0.5f));
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1833),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (s16)thisv->fwork[FHGFIRE_WARP_TEX_1_X],
                                    (s16)thisv->fwork[FHGFIRE_WARP_TEX_1_Y], 0x40, 0x40, 1,
                                    (s16)thisv->fwork[FHGFIRE_WARP_TEX_2_X], (s16)thisv->fwork[FHGFIRE_WARP_TEX_2_Y],
                                    0x40, 0x40));
        gSPDisplayList(POLY_XLU_DISP++, gPhantomWarpDL);
    } else {
        osSyncPrintf("FF DRAW 1\n");
        Matrix_Translate(0.0f, -100.0f, 0.0f, MTXMODE_APPLY);
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)thisv->fwork[FHGFIRE_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 30, 0);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1892),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPhantomLightningDL);
        osSyncPrintf("FF DRAW 2\n");
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_fhg_fire.c", 1900);
}
