/*
 * File: z_en_bombf.c
 * Overlay: ovl_En_Bombf
 * Description: Bomb Flower
 */

#include "z_en_bombf.h"
#include "objects/object_bombf/object_bombf.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

void EnBombf_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBombf_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBombf_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBombf_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBombf_Move(EnBombf* thisv, GlobalContext* globalCtx);
void EnBombf_GrowBomb(EnBombf* thisv, GlobalContext* globalCtx);
void EnBombf_WaitForRelease(EnBombf* thisv, GlobalContext* globalCtx);
void EnBombf_Explode(EnBombf* thisv, GlobalContext* globalCtx);
void EnBombf_SetupGrowBomb(EnBombf* thisv, s16 params);

ActorInit En_Bombf_InitVars = {
    ACTOR_EN_BOMBF,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BOMBF,
    sizeof(EnBombf),
    (ActorFunc)EnBombf_Init,
    (ActorFunc)EnBombf_Destroy,
    (ActorFunc)EnBombf_Update,
    (ActorFunc)EnBombf_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x0003F828, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 9, 18, 10, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000008, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

void EnBombf_SetupAction(EnBombf* thisv, EnBombfActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBombf_Init(Actor* thisx, GlobalContext* globalCtx) {
    f32 shapeUnk10 = 0.0f;
    s32 pad;
    EnBombf* thisv = (EnBombf*)thisx;

    Actor_SetScale(thisx, 0.01f);
    thisv->unk_200 = 1;
    Collider_InitCylinder(globalCtx, &thisv->bombCollider);
    Collider_InitJntSph(globalCtx, &thisv->explosionCollider);
    Collider_SetCylinder(globalCtx, &thisv->bombCollider, thisx, &sCylinderInit);
    Collider_SetJntSph(globalCtx, &thisv->explosionCollider, thisx, &sJntSphInit, &thisv->explosionColliderItems[0]);

    if (thisx->params == BOMBFLOWER_BODY) {
        shapeUnk10 = 1000.0f;
    }

    ActorShape_Init(&thisx->shape, shapeUnk10, ActorShadow_DrawCircle, 12.0f);
    thisx->focus.pos = thisx->world.pos;

    if (Actor_FindNearby(globalCtx, thisx, ACTOR_BG_DDAN_KD, ACTORCAT_BG, 10000.0f) != NULL) {
        thisx->flags |= ACTOR_FLAG_5;
    }

    thisx->colChkInfo.cylRadius = 10.0f;
    thisx->colChkInfo.cylHeight = 10;
    thisx->targetMode = 0;

    if (thisx->params == BOMBFLOWER_BODY) {
        thisv->timer = 140;
        thisv->flashSpeedScale = 15;
        thisx->gravity = -1.5f;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_EXPLOSIVE);
        thisx->colChkInfo.mass = 200;
        thisx->flags &= ~ACTOR_FLAG_0;
        EnBombf_SetupAction(thisv, EnBombf_Move);
    } else {
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
        thisv->bumpOn = true;
        thisv->flowerBombScale = 1.0f;
        EnBombf_SetupGrowBomb(thisv, thisx->params);
    }

    thisx->uncullZoneScale += 31000.0f;
    thisx->uncullZoneForward += 31000.0f;
}

void EnBombf_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBombf* thisv = (EnBombf*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->bombCollider);
    Collider_DestroyJntSph(globalCtx, &thisv->explosionCollider);
}

void EnBombf_SetupGrowBomb(EnBombf* thisv, s16 params) {
    EnBombf_SetupAction(thisv, EnBombf_GrowBomb);
}

void EnBombf_GrowBomb(EnBombf* thisv, GlobalContext* globalCtx) {
    EnBombf* bombFlower;
    s32 pad;
    s32 pad1;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad2;

    if (thisv->flowerBombScale >= 1.0f) {
        if (Actor_HasParent(&thisv->actor, globalCtx)) {
            bombFlower = (EnBombf*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOMBF, thisv->actor.world.pos.x,
                                               thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
            if (bombFlower != NULL) {
                func_8002F5C4(&thisv->actor, &bombFlower->actor, globalCtx);
                thisv->timer = 180;
                thisv->flowerBombScale = 0.0f;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_PL_PULL_UP_ROCK);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
            } else {
                player->actor.child = NULL;
                player->heldActor = NULL;
                player->interactRangeActor = NULL;
                thisv->actor.parent = NULL;
                player->stateFlags1 &= ~0x800;
            }
        } else if (thisv->bombCollider.base.acFlags & AC_HIT) {
            thisv->bombCollider.base.acFlags &= ~AC_HIT;

            if (thisv->bombCollider.base.ac->category != ACTORCAT_BOSS) {
                bombFlower =
                    (EnBombf*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOMBF, thisv->actor.world.pos.x,
                                          thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
                if (bombFlower != NULL) {
                    bombFlower->unk_200 = 1;
                    bombFlower->timer = 0;
                    thisv->timer = 180;
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                    thisv->flowerBombScale = 0.0f;
                }
            }
        } else {
            if (Player_IsBurningStickInRange(globalCtx, &thisv->actor.world.pos, 30.0f, 50.0f)) {
                bombFlower =
                    (EnBombf*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOMBF, thisv->actor.world.pos.x,
                                          thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
                if (bombFlower != NULL) {
                    bombFlower->timer = 100;
                    thisv->timer = 180;
                    thisv->actor.flags &= ~ACTOR_FLAG_0;
                    thisv->flowerBombScale = 0.0f;
                }
            } else {
                if (!Actor_HasParent(&thisv->actor, globalCtx)) {
                    func_8002F580(&thisv->actor, globalCtx);
                } else {
                    player->actor.child = NULL;
                    player->heldActor = NULL;
                    player->interactRangeActor = NULL;
                    thisv->actor.parent = NULL;
                    player->stateFlags1 &= ~0x800;
                    thisv->actor.world.pos = thisv->actor.home.pos;
                }
            }
        }
    } else {
        if (thisv->timer == 0) {
            thisv->flowerBombScale += 0.05f;
            if (thisv->flowerBombScale >= 1.0f) {
                thisv->actor.flags |= ACTOR_FLAG_0;
            }
        }

        if (Actor_HasParent(&thisv->actor, globalCtx)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            thisv->actor.parent = NULL;
            player->stateFlags1 &= ~0x800;
            thisv->actor.world.pos = thisv->actor.home.pos;
        }
    }
}

void EnBombf_Move(EnBombf* thisv, GlobalContext* globalCtx) {
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        // setting flowerBombScale does not do anything in the context of a bomb that link picks up
        // thisv and the assignment below are probably left overs
        thisv->flowerBombScale = 0.0f;
        EnBombf_SetupAction(thisv, EnBombf_WaitForRelease);
        thisv->actor.room = -1;
        return;
    }

    thisv->flowerBombScale = 1.0f;

    if (!(thisv->actor.bgCheckFlags & 1)) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.025f, 0.0f);
    } else {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 1.5f, 0.0f);
        if ((thisv->actor.bgCheckFlags & 2) && (thisv->actor.velocity.y < -6.0f)) {
            func_8002F850(globalCtx, &thisv->actor);
            thisv->actor.velocity.y *= -0.5f;
        } else if (thisv->timer >= 4) {
            func_8002F580(&thisv->actor, globalCtx);
        }
    }
}

void EnBombf_WaitForRelease(EnBombf* thisv, GlobalContext* globalCtx) {
    // if parent is NULL bomb has been released
    if (Actor_HasNoParent(&thisv->actor, globalCtx)) {
        EnBombf_SetupAction(thisv, EnBombf_Move);
        EnBombf_Move(thisv, globalCtx);
    } else {
        thisv->actor.velocity.y = 0.0f;
    }
}

void EnBombf_Explode(EnBombf* thisv, GlobalContext* globalCtx) {
    Player* player;

    if (thisv->explosionCollider.elements[0].dim.modelSphere.radius == 0) {
        thisv->actor.flags |= ACTOR_FLAG_5;
        func_800AA000(thisv->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
    }

    thisv->explosionCollider.elements[0].dim.modelSphere.radius += 8;
    thisv->explosionCollider.elements[0].dim.worldSphere.radius =
        thisv->explosionCollider.elements[0].dim.modelSphere.radius;

    if (thisv->actor.params == BOMBFLOWER_EXPLOSION) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->explosionCollider.base);
    }

    if (globalCtx->envCtx.adjLight1Color[0] != 0) {
        globalCtx->envCtx.adjLight1Color[0] -= 25;
    }

    if (globalCtx->envCtx.adjLight1Color[1] != 0) {
        globalCtx->envCtx.adjLight1Color[1] -= 25;
    }

    if (globalCtx->envCtx.adjLight1Color[2] != 0) {
        globalCtx->envCtx.adjLight1Color[2] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[0] != 0) {
        globalCtx->envCtx.adjAmbientColor[0] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[1] != 0) {
        globalCtx->envCtx.adjAmbientColor[1] -= 25;
    }

    if (globalCtx->envCtx.adjAmbientColor[2] != 0) {
        globalCtx->envCtx.adjAmbientColor[2] -= 25;
    }

    if (thisv->timer == 0) {
        player = GET_PLAYER(globalCtx);

        if ((player->stateFlags1 & 0x800) && (player->heldActor == &thisv->actor)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            player->stateFlags1 &= ~0x800;
        }

        Actor_Kill(&thisv->actor);
    }
}

void EnBombf_Update(Actor* thisx, GlobalContext* globalCtx) {
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f bomb2Accel = { 0.0f, 0.1f, 0.0f };
    Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f dustAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 dustColor = { 255, 255, 255, 255 };
    s32 pad[2];
    EnBombf* thisv = (EnBombf*)thisx;

    if ((thisv->unk_200 != 0) && (thisv->timer != 0)) {
        thisv->timer--;
    }

    if ((!thisv->bumpOn) && (!Actor_HasParent(thisx, globalCtx)) &&
        ((thisx->xzDistToPlayer >= 20.0f) || (ABS(thisx->yDistToPlayer) >= 80.0f))) {
        thisv->bumpOn = true;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisx->params == BOMBFLOWER_BODY) {
        Actor_MoveForward(thisx);
    }

    if (thisx->gravity != 0.0f) {
        DREG(6) = 1;
        Actor_UpdateBgCheckInfo(globalCtx, thisx, 5.0f, 10.0f, 0.0f, 0x1F);
        DREG(6) = 0;
    }

    if (thisx->params == BOMBFLOWER_BODY) {

        if ((thisx->velocity.y > 0.0f) && (thisx->bgCheckFlags & 0x10)) {
            thisx->velocity.y = -thisx->velocity.y;
        }

        // rebound bomb off the wall it hits
        if ((thisx->speedXZ != 0.0f) && (thisx->bgCheckFlags & 8)) {

            if (ABS((s16)(thisx->wallYaw - thisx->world.rot.y)) > 0x4000) {
                if (1) {}
                thisx->world.rot.y = ((thisx->wallYaw - thisx->world.rot.y) + thisx->wallYaw) - 0x8000;
            }
            Audio_PlayActorSound2(thisx, NA_SE_EV_BOMB_BOUND);
            Actor_MoveForward(thisx);
            DREG(6) = 1;
            Actor_UpdateBgCheckInfo(globalCtx, thisx, 5.0f, 10.0f, 0.0f, 0x1F);
            DREG(6) = 0;
            thisx->speedXZ *= 0.7f;
            thisx->bgCheckFlags &= ~8;
        }

        if ((thisv->bombCollider.base.acFlags & AC_HIT) || ((thisv->bombCollider.base.ocFlags1 & OC1_HIT) &&
                                                           (thisv->bombCollider.base.oc->category == ACTORCAT_ENEMY))) {
            thisv->unk_200 = 1;
            thisv->timer = 0;
        } else {
            // if a lit stick touches the bomb, set timer to 100
            if ((thisv->timer > 100) && Player_IsBurningStickInRange(globalCtx, &thisx->world.pos, 30.0f, 50.0f)) {
                thisv->timer = 100;
            }
        }

        if (thisv->unk_200 != 0) {
            dustAccel.y = 0.2f;
            effPos = thisx->world.pos;
            effPos.y += 25.0f;
            if (thisv->timer < 127) {
                // spawn spark effect on even frames
                if ((globalCtx->gameplayFrames % 2) == 0) {
                    EffectSsGSpk_SpawnFuse(globalCtx, thisx, &effPos, &effVelocity, &effAccel);
                }
                Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);

                effPos.y += 3.0f;
                func_8002829C(globalCtx, &effPos, &effVelocity, &dustAccel, &dustColor, &dustColor, 50, 5);
            }

            // double bomb flash speed and adjust red color at certain times during the countdown
            if ((thisv->timer == 3) || (thisv->timer == 30) || (thisv->timer == 50) || (thisv->timer == 70)) {
                thisv->flashSpeedScale >>= 1;
            }

            if ((thisv->timer < 100) && ((thisv->timer & (thisv->flashSpeedScale + 1)) != 0)) {
                Math_SmoothStepToF(&thisv->flashIntensity, 150.0f, 1.0f, 150.0f / thisv->flashSpeedScale, 0.0f);
            } else {
                Math_SmoothStepToF(&thisv->flashIntensity, 0.0f, 1.0f, 150.0f / thisv->flashSpeedScale, 0.0f);
            }

            if (thisv->timer < 3) {
                Actor_SetScale(thisx, thisx->scale.x + 0.002f);
            }

            if (thisv->timer == 0) {
                effPos = thisx->world.pos;

                effPos.y += 10.0f;

                if (Actor_HasParent(thisx, globalCtx)) {
                    effPos.y += 30.0f;
                }

                EffectSsBomb2_SpawnLayered(globalCtx, &effPos, &effVelocity, &bomb2Accel, 100, 19);

                effPos.y = thisx->floorHeight;
                if (thisx->floorHeight > BGCHECK_Y_MIN) {
                    EffectSsBlast_SpawnWhiteShockwave(globalCtx, &effPos, &effVelocity, &effAccel);
                }

                Audio_PlayActorSound2(thisx, NA_SE_IT_BOMB_EXPLOSION);
                globalCtx->envCtx.adjLight1Color[0] = globalCtx->envCtx.adjLight1Color[1] =
                    globalCtx->envCtx.adjLight1Color[2] = 250;
                globalCtx->envCtx.adjAmbientColor[0] = globalCtx->envCtx.adjAmbientColor[1] =
                    globalCtx->envCtx.adjAmbientColor[2] = 250;
                Camera_AddQuake(&globalCtx->mainCamera, 2, 0xB, 8);
                thisx->params = BOMBFLOWER_EXPLOSION;
                thisv->timer = 10;
                thisx->flags |= ACTOR_FLAG_5;
                EnBombf_SetupAction(thisv, EnBombf_Explode);
            }
        }
    }

    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 10.0f;

    if (thisx->params <= BOMBFLOWER_BODY) {

        Collider_UpdateCylinder(thisx, &thisv->bombCollider);

        if ((thisv->flowerBombScale >= 1.0f) && (thisv->bumpOn)) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bombCollider.base);
        }

        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->bombCollider.base);
    }

    if ((thisx->scale.x >= 0.01f) && (thisx->params != BOMBFLOWER_EXPLOSION)) {
        if (thisx->yDistToWater >= 20.0f) {
            EffectSsDeadSound_SpawnStationary(globalCtx, &thisx->projectedPos, NA_SE_IT_BOMB_UNEXPLOSION, true,
                                              DEADSOUND_REPEAT_MODE_OFF, 10);
            Actor_Kill(thisx);
            return;
        }
        if (thisx->bgCheckFlags & 0x40) {
            thisx->bgCheckFlags &= ~0x40;
            Audio_PlayActorSound2(thisx, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

Gfx* EnBombf_NewMtxDList(GraphicsContext* gfxCtx, GlobalContext* globalCtx) {
    Gfx* displayList;
    Gfx* displayListHead;

    displayList = static_cast<Gfx*>(Graph_Alloc(gfxCtx, 5 * sizeof(Gfx)));
    displayListHead = displayList;
    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
    gSPMatrix(displayListHead++, Matrix_NewMtx(gfxCtx, "../z_en_bombf.c", 1021),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPEndDisplayList(displayListHead);
    return displayList;
}

void EnBombf_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnBombf* thisv = (EnBombf*)thisx;

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bombf.c", 1034);

    if (thisx->params <= BOMBFLOWER_BODY) {
        func_80093D18(globalCtx->state.gfxCtx);

        if (thisx->params != BOMBFLOWER_BODY) {
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bombf.c", 1041),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gBombFlowerLeavesDL);
            gSPDisplayList(POLY_OPA_DISP++, gBombFlowerBaseLeavesDL);

            Matrix_Translate(0.0f, 1000.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(thisv->flowerBombScale, thisv->flowerBombScale, thisv->flowerBombScale, MTXMODE_APPLY);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 255, 200, 255);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)thisv->flashIntensity, 20, 10, 0);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bombf.c", 1054),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   SEGMENTED_TO_VIRTUAL(EnBombf_NewMtxDList(globalCtx->state.gfxCtx, globalCtx)));
        gSPDisplayList(POLY_OPA_DISP++, gBombFlowerBombAndSparkDL);
    } else {
        Collider_UpdateSpheres(0, &thisv->explosionCollider);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bombf.c", 1063);
}
