#include "z_en_clear_tag.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnClearTag_Init(Actor* thisx, GlobalContext* globalCtx);
void EnClearTag_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnClearTag_Update(Actor* thisx, GlobalContext* globalCtx);
void EnClearTag_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnClearTag_Reset(void);

void EnClearTag_UpdateEffects(GlobalContext* globalCtx);
void EnClearTag_DrawEffects(GlobalContext* globalCtx);

void EnClearTag_CreateDebrisEffect(GlobalContext* globalCtx, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                                   f32 scale, f32 floorHeight);
void EnClearTag_CreateFireEffect(GlobalContext* globalCtx, Vec3f* pos, f32 scale);
void EnClearTag_CreateSmokeEffect(GlobalContext* globalCtx, Vec3f* position, f32 scale);
void EnClearTag_CreateFlashEffect(GlobalContext* globalCtx, Vec3f* position, f32 scale, f32 floorHeight,
                                  Vec3f* floorTangent);

void EnClearTag_CalculateFloorTangent(EnClearTag* thisv);

ActorInit En_Clear_Tag_InitVars = {
    ACTOR_EN_CLEAR_TAG,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnClearTag),
    (ActorFunc)EnClearTag_Init,
    (ActorFunc)EnClearTag_Destroy,
    (ActorFunc)EnClearTag_Update,
    (ActorFunc)EnClearTag_Draw,
    (ActorResetFunc)EnClearTag_Reset,
};

static u8 sIsEffectsInitialized = false;

static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };

static ColliderCylinderInit sArwingCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit sLaserCylinderInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static UNK_TYPE4 D_809D5C98 = 0; // unused
static UNK_TYPE4 D_809D5C9C = 0; // unused

static EnClearTagEffect sClearTagEffects[CLEAR_TAG_EFFECT_MAX_COUNT];

#include "overlays/ovl_En_Clear_Tag/ovl_En_Clear_Tag.h"

/**
 * Creates a debris effect.
 * Debris effects are spawned when the Arwing dies. It spawns fire effects.
 */
void EnClearTag_CreateDebrisEffect(GlobalContext* globalCtx, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                                   f32 scale, f32 floorHeight) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;

    // Look for an available effect to allocate a Debris effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->type = CLEAR_TAG_EFFECT_DEBRIS;

            effect->position = *position;
            effect->velocity = *velocity;
            effect->acceleration = *acceleration;

            effect->scale = scale;

            // Set the debris effects to spawn in a circle.
            effect->rotationY = Rand_ZeroFloat(std::numbers::pi_v<float> * 2);
            effect->rotationX = Rand_ZeroFloat(std::numbers::pi_v<float> * 2);

            effect->timer = effect->bounces = 0;

            effect->floorHeight = floorHeight;

            effect->random = (s16)Rand_ZeroFloat(10.0f);

            return;
        }
    }
}

/**
 * Creates a fire effect.
 * Fire effects are spawned by debris effects. Fire effects spawn smoke effects
 */
void EnClearTag_CreateFireEffect(GlobalContext* globalCtx, Vec3f* pos, f32 scale) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;

    // Look for an available effect to allocate a fire effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random = (s16)Rand_ZeroFloat(100.0f);
            effect->type = CLEAR_TAG_EFFECT_FIRE;

            effect->position = *pos;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;
            effect->acceleration.y = 0.15f;

            effect->scale = scale;

            effect->primColor.a = 200.0f;

            return;
        }
    }
}

/**
 * Creates a smoke effect.
 * Smoke effects are spawned by fire effects.
 */
void EnClearTag_CreateSmokeEffect(GlobalContext* globalCtx, Vec3f* position, f32 scale) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;

    // Look for an available effect to allocate a smoke effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random = (s16)Rand_ZeroFloat(100.0f);
            effect->type = CLEAR_TAG_EFFECT_SMOKE;

            effect->position = *position;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;

            effect->scale = scale;
            effect->maxScale = scale * 2.0f;

            effect->primColor.r = 200.0f;
            effect->primColor.g = 20.0f;
            effect->primColor.b = 0.0f;
            effect->primColor.a = 255.0f;
            effect->envColor.r = 255.0f;
            effect->envColor.g = 215.0f;
            effect->envColor.b = 255.0f;

            return;
        }
    }
}

/**
 * Creates a flash effect.
 * Flash effects are spawned when the Arwing dies.
 * Flash effects two components: 1) a billboard flash, and 2) a light effect on the ground.
 */
void EnClearTag_CreateFlashEffect(GlobalContext* globalCtx, Vec3f* position, f32 scale, f32 floorHeight,
                                  Vec3f* floorTangent) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;

    // Look for an available effect to allocate a flash effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->type = CLEAR_TAG_EFFECT_FLASH;

            effect->position = *position;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;

            effect->scale = 0.0f;
            effect->maxScale = scale * 2.0f;

            effect->floorHeight = floorHeight;
            effect->floorTangent = *floorTangent;

            effect->primColor.a = 180.0f;

            return;
        }
    }
}

/**
 * EnClear_Tag destructor.
 * This just destroys the collider.
 */
void EnClearTag_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnClearTag* thisv = (EnClearTag*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

/**
 * EnClear_Tag constructor.
 * This allocates a collider, initializes effects, and sets up ClearTag instance data.
 */
void EnClearTag_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnClearTag* thisv = (EnClearTag*)thisx;
    s32 defaultCutsceneTimer = 100;
    s16 i;
    s16 j;

    Collider_InitCylinder(globalCtx, &thisv->collider);

    // Initialize the Arwing laser.
    if (thisv->actor.params == CLEAR_TAG_LASER) {
        thisv->state = CLEAR_TAG_STATE_LASER;
        thisv->timers[CLEAR_TAG_TIMER_LASER_DEATH] = 70;
        thisv->actor.speedXZ = 35.0f;
        func_8002D908(&thisv->actor);
        for (j = 0; j <= 0; j++) {
            func_8002D7EC(&thisv->actor);
        }
        thisv->actor.scale.x = 0.4f;
        thisv->actor.scale.y = 0.4f;
        thisv->actor.scale.z = 2.0f;
        thisv->actor.speedXZ = 70.0f;
        thisv->actor.shape.rot.x = -thisv->actor.shape.rot.x;

        func_8002D908(&thisv->actor);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sLaserCylinderInit);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_SWORD_REFLECT_MG);
    } else { // Initialize the Arwing.
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.targetMode = 5;
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sArwingCylinderInit);
        thisv->actor.colChkInfo.health = 3;

        // Update the Arwing to play the intro cutscene.
        if (thisv->actor.params == CLEAR_TAG_CUTSCENE_ARWING) {
            thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 70;
            thisv->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] = 250;
            thisv->state = CLEAR_TAG_STATE_DEMO;
            thisv->actor.world.rot.x = 0x4000;
            thisv->cutsceneMode = CLEAR_TAG_CUTSCENE_MODE_SETUP;
            thisv->cutsceneTimer = defaultCutsceneTimer;
            thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_BG_INFO] = 20;
        }

        // Initialize all effects to available if effects have not been initialized.
        if (!sIsEffectsInitialized) {
            sIsEffectsInitialized = true;
            globalCtx->specialEffects = &sClearTagEffects[0];
            for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++) {
                sClearTagEffects[i].type = CLEAR_TAG_EFFECT_AVAILABLE;
            }
            thisv->drawMode = CLEAR_TAG_DRAW_MODE_ALL;
        }
    }
}

/**
 * Calculate a floor tangent.
 * This is used for the ground flash display lists and Arwing shadow display lists to snap onto the floor.
 */
void EnClearTag_CalculateFloorTangent(EnClearTag* thisv) {
    // If there is a floor poly below the Arwing, calculate the floor tangent.
    if (thisv->actor.floorPoly != NULL) {
        f32 x = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.x);
        f32 y = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.y);
        f32 z = COLPOLY_GET_NORMAL(thisv->actor.floorPoly->normal.z);

        thisv->floorTangent.x = -Math_FAtan2F(-z * y, 1.0f);
        thisv->floorTangent.z = Math_FAtan2F(-x * y, 1.0f);
    }
}

/**
 * EnClear_Tag update function.
 *
 * ClearTag has three variants:
 * CLEAR_TAG_CUTSCENE_ARWING which is the regular Arwing and plays a cutscene on spawn.
 * CLEAR_TAG_ARWING which is a regular Arwing.
 * CLEAR_TAG_LASER which are the lasers fired by the Arwing.
 *
 * This function controls the Arwing flying. A target position is set and the Arwing flies toward it based on
 * calculated directions from the current position.
 * This function fires Arwing lasers when the state is CLEAR_TAG_STATE_TARGET_LOCKED.
 * This function controls the cutscene that plays when the Arwing has params for
 * cutscene. The cutscene stops playing when the Arwing is a specified distance from the starting point.
 */
void EnClearTag_Update(Actor* thisx, GlobalContext* globalCtx2) {
    u8 hasAtHit = false;
    s16 i;
    s16 xRotationTarget;
    s16 rotationScale;
    GlobalContext* globalCtx = globalCtx2;
    EnClearTag* thisv = (EnClearTag*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    thisv->frameCounter++;

    if (thisv->drawMode != CLEAR_TAG_DRAW_MODE_EFFECT) {
        for (i = 0; i < 3; i++) {
            if (thisv->timers[i] != 0) {
                thisv->timers[i]--;
            }
        }

        if (thisv->cutsceneTimer != 0) {
            thisv->cutsceneTimer--;
        }

        switch (thisv->state) {
            case CLEAR_TAG_STATE_DEMO:
            case CLEAR_TAG_STATE_TARGET_LOCKED:
            case CLEAR_TAG_STATE_FLYING: {
                f32 vectorToTargetX;
                f32 vectorToTargetY;
                f32 vectorToTargetZ;
                s16 worldRotationTargetX;
                s16 worldRotationTargetY;
                f32 loseTargetLockDistance;
                s16 worldRotationTargetZ;
                s32 pad;

                // Check if the Arwing should crash.
                if (thisv->collider.base.acFlags & AC_HIT) {

                    thisv->collider.base.acFlags &= ~AC_HIT;
                    thisv->crashingTimer = 20;
                    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 5);
                    thisv->acceleration.x = Rand_CenteredFloat(15.0f);
                    thisv->acceleration.y = Rand_CenteredFloat(15.0f);
                    thisv->acceleration.z = Rand_CenteredFloat(15.0f);

                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_THUNDER_GND);
                    thisv->actor.colChkInfo.health--;
                    if ((s8)thisv->actor.colChkInfo.health <= 0) {
                        thisv->state = CLEAR_TAG_STATE_CRASHING;
                        thisv->actor.velocity.y = 0.0f;
                        goto state_crashing;
                    }
                }
                Actor_SetScale(&thisv->actor, 0.2f);
                thisv->actor.speedXZ = 7.0f;

                if (thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] == 0) {
                    if (thisv->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] == 0) {
                        thisv->state = CLEAR_TAG_STATE_TARGET_LOCKED;
                        thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 300;
                    } else {
                        thisv->state = CLEAR_TAG_STATE_FLYING;
                        thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = (s16)Rand_ZeroFloat(50.0f) + 20;

                        if (thisv->actor.params == CLEAR_TAG_ARWING) {
                            // Set the Arwing to fly in a circle around the player.
                            f32 targetCircleX = Math_SinS(player->actor.shape.rot.y) * 400.0f;
                            f32 targetCircleZ = Math_CosS(player->actor.shape.rot.y) * 400.0f;

                            thisv->targetPosition.x =
                                Rand_CenteredFloat(700.0f) + (player->actor.world.pos.x + targetCircleX);
                            thisv->targetPosition.y = Rand_ZeroFloat(200.0f) + player->actor.world.pos.y + 150.0f;
                            thisv->targetPosition.z =
                                Rand_CenteredFloat(700.0f) + (player->actor.world.pos.z + targetCircleZ);
                        } else {
                            // Set the Arwing to fly to a random position.
                            thisv->targetPosition.x = Rand_CenteredFloat(700.0f);
                            thisv->targetPosition.y = Rand_ZeroFloat(200.0f) + 150.0f;
                            thisv->targetPosition.z = Rand_CenteredFloat(700.0f);
                        }
                    }

                    thisv->targetDirection.x = thisv->targetDirection.y = thisv->targetDirection.z = 0.0f;
                }

                rotationScale = 10;
                xRotationTarget = 0x800;
                loseTargetLockDistance = 100.0f;
                if (thisv->state == CLEAR_TAG_STATE_TARGET_LOCKED) {
                    // Set the Arwing to fly towards the player.
                    thisv->targetPosition.x = player->actor.world.pos.x;
                    thisv->targetPosition.y = player->actor.world.pos.y + 40.0f;
                    thisv->targetPosition.z = player->actor.world.pos.z;
                    rotationScale = 7;
                    xRotationTarget = 0x1000;
                    loseTargetLockDistance = 150.0f;
                } else if (thisv->state == CLEAR_TAG_STATE_DEMO) {
                    // Move the Arwing for the intro cutscene.

                    // Do a Barrel Roll!
                    thisv->roll += 0.5f;
                    if (thisv->roll > std::numbers::pi_v<float> * 2) {
                        thisv->roll -= std::numbers::pi_v<float> * 2;
                    }

                    // Set the Arwing to fly to a hardcoded position.
                    thisv->targetPosition.x = 0.0f;
                    thisv->targetPosition.y = 300.0f;
                    thisv->targetPosition.z = 0.0f;
                    loseTargetLockDistance = 100.0f;
                }

                // If the Arwing is not in cutscene state, smoothly update the roll to zero.
                // This will reset the Arwing to be right side up after the cutscene is done.
                // The cutscene will set the Arwing to do a barrel roll and doesn't end on right side up.
                if (thisv->state != CLEAR_TAG_STATE_DEMO) {
                    Math_ApproachZeroF(&thisv->roll, 0.1f, 0.2f);
                }

                // Calculate a vector towards the targetted position.
                vectorToTargetX = thisv->targetPosition.x - thisv->actor.world.pos.x;
                vectorToTargetY = thisv->targetPosition.y - thisv->actor.world.pos.y;
                vectorToTargetZ = thisv->targetPosition.z - thisv->actor.world.pos.z;

                // If the Arwing is within a certain distance to the target position, it will be updated to flymode
                if (sqrtf(SQ(vectorToTargetX) + SQ(vectorToTargetY) + SQ(vectorToTargetZ)) < loseTargetLockDistance) {
                    thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 0;
                    if (thisv->state == CLEAR_TAG_STATE_TARGET_LOCKED) {
                        thisv->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] = (s16)Rand_ZeroFloat(100.0f) + 100;
                    }
                    thisv->state = CLEAR_TAG_STATE_FLYING;
                }

                // Calculate the direction for the Arwing to fly and the rotation for the Arwing
                // based on the Arwing's direction, and current rotation.
                worldRotationTargetY = Math_FAtan2F(vectorToTargetX, vectorToTargetZ) * (0x8000 / std::numbers::pi_v<float>);
                worldRotationTargetX =
                    Math_FAtan2F(vectorToTargetY, sqrtf(SQ(vectorToTargetX) + SQ(vectorToTargetZ))) * (0x8000 / std::numbers::pi_v<float>);
                if ((worldRotationTargetX < 0) && (thisv->actor.world.pos.y < thisv->actor.floorHeight + 20.0f)) {
                    worldRotationTargetX = 0;
                }
                Math_ApproachS(&thisv->actor.world.rot.x, worldRotationTargetX, rotationScale, thisv->targetDirection.x);
                worldRotationTargetZ = Math_SmoothStepToS(&thisv->actor.world.rot.y, worldRotationTargetY, rotationScale,
                                                          thisv->targetDirection.y, 0);
                Math_ApproachF(&thisv->targetDirection.x, xRotationTarget, 1.0f, 0x100);
                thisv->targetDirection.y = thisv->targetDirection.x;
                if (ABS(worldRotationTargetZ) < 0x1000) {
                    Math_ApproachS(&thisv->actor.world.rot.z, 0, 15, thisv->targetDirection.z);
                    Math_ApproachF(&thisv->targetDirection.z, 0x500, 1.0f, 0x100);

                    // Check if the Arwing should fire its laser.
                    if ((thisv->frameCounter % 4) == 0 && (Rand_ZeroOne() < 0.75f) &&
                        (thisv->state == CLEAR_TAG_STATE_TARGET_LOCKED)) {
                        thisv->shouldShootLaser = true;
                    }
                } else {
                    worldRotationTargetZ = worldRotationTargetZ > 0 ? -0x2500 : 0x2500;
                    Math_ApproachS(&thisv->actor.world.rot.z, worldRotationTargetZ, rotationScale,
                                   thisv->targetDirection.z);
                    Math_ApproachF(&thisv->targetDirection.z, 0x1000, 1.0f, 0x200);
                }
                thisv->actor.shape.rot = thisv->actor.world.rot;
                thisv->actor.shape.rot.x = -thisv->actor.shape.rot.x;

                // Update the Arwing's velocity.
                func_8002D908(&thisv->actor);
                thisv->actor.velocity.x += thisv->acceleration.x;
                thisv->actor.velocity.y += thisv->acceleration.y;
                thisv->actor.velocity.z += thisv->acceleration.z;
                Math_ApproachZeroF(&thisv->acceleration.x, 1.0f, 1.0f);
                Math_ApproachZeroF(&thisv->acceleration.y, 1.0f, 1.0f);
                Math_ApproachZeroF(&thisv->acceleration.z, 1.0f, 1.0f);

                // Fire the Arwing laser.
                if (thisv->shouldShootLaser) {
                    thisv->shouldShootLaser = false;
                    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, thisv->actor.world.rot.x,
                                thisv->actor.world.rot.y, thisv->actor.world.rot.z, CLEAR_TAG_STATE_LASER);
                }
            }
            case CLEAR_TAG_STATE_CRASHING:
            state_crashing:
                if (thisv->crashingTimer != 0) {
                    thisv->crashingTimer--;
                }

                func_8002D7EC(&thisv->actor);

                Actor_SetFocus(&thisv->actor, 0.0f);

                // Update Arwing collider to better match a ground collision.
                thisv->collider.dim.radius = 20;
                thisv->collider.dim.height = 15;
                thisv->collider.dim.yShift = -5;
                Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

                if (thisv->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_BG_INFO] == 0) {
                    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 30.0f, 100.0f, 5);
                    EnClearTag_CalculateFloorTangent(thisv);
                }

                if (thisv->state == CLEAR_TAG_STATE_CRASHING) {
                    // Create fire effects while the Arwing crashes.
                    EnClearTag_CreateFireEffect(globalCtx, &thisv->actor.world.pos, 1.0f);

                    // Causes the Arwing to roll around seemingly randomly while crashing.
                    thisv->roll -= 0.5f;
                    thisv->actor.velocity.y -= 0.2f;
                    thisv->actor.shape.rot.x += 0x10;

                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_BREATH - SFX_FLAG);

                    // Check if the Arwing has hit the ground.
                    if (thisv->actor.bgCheckFlags & 9) {
                        thisv->shouldExplode = true;

                        if (thisv->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
                            thisv->drawMode = CLEAR_TAG_DRAW_MODE_EFFECT;
                            thisv->deathTimer = 70;
                            thisv->actor.flags &= ~ACTOR_FLAG_0;
                        } else {
                            Actor_Kill(&thisv->actor);
                        }
                    }
                }
                break;

            case CLEAR_TAG_STATE_LASER:
                func_8002D7EC(&thisv->actor);

                // Check if the laser has hit a target.
                if (thisv->collider.base.atFlags & AT_HIT) {
                    hasAtHit = true;
                }

                // Set laser collider properties.
                thisv->collider.dim.radius = 23;
                thisv->collider.dim.height = 25;
                thisv->collider.dim.yShift = -10;
                Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
                Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 50.0f, 80.0f, 100.0f, 5);

                // Check if the laser has hit a target, timed out, or hit the ground.
                if (thisv->actor.bgCheckFlags & 9 || hasAtHit || thisv->timers[CLEAR_TAG_TIMER_LASER_DEATH] == 0) {
                    // Kill the laser.
                    Actor_Kill(&thisv->actor);
                    // Player laser sound effect if the laser did not time out.
                    if (thisv->timers[CLEAR_TAG_TIMER_LASER_DEATH] != 0) {
                        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20,
                                                           NA_SE_EN_FANTOM_THUNDER_GND);
                    }
                }
                break;
        }

        if (thisv->state < CLEAR_TAG_STATE_LASER) {
            // Play the Arwing cutscene.
            osSyncPrintf("DEMO_MODE %d\n", thisv->cutsceneMode);
            osSyncPrintf("CAMERA_NO %d\n", thisv->cameraId);

            if (thisv->cutsceneMode != CLEAR_TAG_CUTSCENE_MODE_NONE) {
                f32 cutsceneCameraCircleX;
                f32 cutsceneCameraCircleZ;
                s16 cutsceneTimer;
                Vec3f cutsceneCameraAtTarget;
                Vec3f cutsceneCameraEyeTarget;

                switch (thisv->cutsceneMode) {
                    case CLEAR_TAG_CUTSCENE_MODE_SETUP:
                        // Initializes Arwing cutscene camera data.
                        thisv->cutsceneMode = CLEAR_TAG_CUTSCENE_MODE_PLAY;
                        func_80064520(globalCtx, &globalCtx->csCtx);
                        thisv->cameraId = Gameplay_CreateSubCamera(globalCtx);
                        Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_WAIT);
                        Gameplay_ChangeCameraStatus(globalCtx, thisv->cameraId, CAM_STAT_ACTIVE);
                    case CLEAR_TAG_CUTSCENE_MODE_PLAY:
                        // Update the Arwing cutscene camera to spin around in a circle.
                        cutsceneTimer = thisv->frameCounter * 128;
                        cutsceneCameraCircleX = Math_SinS(cutsceneTimer) * 200.0f;
                        cutsceneCameraCircleZ = Math_CosS(cutsceneTimer) * 200.0f;
                        cutsceneCameraAtTarget.x = thisv->actor.world.pos.x + cutsceneCameraCircleX;
                        cutsceneCameraAtTarget.y = 200.0f;
                        cutsceneCameraAtTarget.z = thisv->actor.world.pos.z + cutsceneCameraCircleZ;
                        cutsceneCameraEyeTarget = thisv->actor.world.pos;
                        break;
                }

                // Make the Arwing cutscene camera approach the target.
                if (thisv->cameraId != SUBCAM_FREE) {
                    Math_ApproachF(&thisv->cutsceneCameraAt.x, cutsceneCameraAtTarget.x, 0.1f, 500.0f);
                    Math_ApproachF(&thisv->cutsceneCameraAt.y, cutsceneCameraAtTarget.y, 0.1f, 500.0f);
                    Math_ApproachF(&thisv->cutsceneCameraAt.z, cutsceneCameraAtTarget.z, 0.1f, 500.0f);
                    Math_ApproachF(&thisv->cutsceneCameraEye.x, cutsceneCameraEyeTarget.x, 0.2f, 500.0f);
                    Math_ApproachF(&thisv->cutsceneCameraEye.y, cutsceneCameraEyeTarget.y, 0.2f, 500.0f);
                    Math_ApproachF(&thisv->cutsceneCameraEye.z, cutsceneCameraEyeTarget.z, 0.2f, 500.0f);
                    Gameplay_CameraSetAtEye(globalCtx, thisv->cameraId, &thisv->cutsceneCameraEye,
                                            &thisv->cutsceneCameraAt);
                }

                // Cutscene has finished.
                if (thisv->cutsceneTimer == 1) {
                    func_800C08AC(globalCtx, thisv->cameraId, 0);
                    thisv->cutsceneMode = thisv->cameraId = SUBCAM_FREE;
                    func_80064534(globalCtx, &globalCtx->csCtx);
                }
            }
        }
    }

    // Explode the Arwing
    if (thisv->shouldExplode) {
        Vec3f crashEffectLocation;
        Vec3f crashEffectVelocity;
        Vec3f debrisEffectAcceleration;

        thisv->shouldExplode = false;
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_IT_BOMB_EXPLOSION);

        // Spawn flash effect.
        crashEffectLocation.x = thisv->actor.world.pos.x;
        crashEffectLocation.y = (thisv->actor.world.pos.y + 40.0f) - 30.0f;
        crashEffectLocation.z = thisv->actor.world.pos.z;
        EnClearTag_CreateFlashEffect(globalCtx, &crashEffectLocation, 6.0f, thisv->actor.floorHeight,
                                     &thisv->floorTangent);

        // Spawn smoke effect.
        crashEffectLocation.y = (thisv->actor.world.pos.y + 30.0f) - 50.0f;
        EnClearTag_CreateSmokeEffect(globalCtx, &crashEffectLocation, 3.0f);
        crashEffectLocation.y = thisv->actor.world.pos.y;

        // Spawn debris effects.
        for (i = 0; i < 15; i++) {
            crashEffectVelocity.x = sinf(i * 1.65f) * i * 0.3f;
            crashEffectVelocity.z = cosf(i * 1.65f) * i * 0.3f;
            crashEffectVelocity.y = Rand_ZeroFloat(6.0f) + 5.0f;
            crashEffectVelocity.x += Rand_CenteredFloat(0.5f);
            crashEffectVelocity.z += Rand_CenteredFloat(0.5f);

            debrisEffectAcceleration.x = 0.0f;
            debrisEffectAcceleration.y = -1.0f;
            debrisEffectAcceleration.z = 0.0f;

            EnClearTag_CreateDebrisEffect(globalCtx, &crashEffectLocation, &crashEffectVelocity,
                                          &debrisEffectAcceleration, Rand_ZeroFloat(0.15f) + 0.075f,
                                          thisv->actor.floorHeight);
        }
    }

    if (thisv->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
        // Check if the Arwing should be removed.
        if ((thisv->drawMode == CLEAR_TAG_DRAW_MODE_EFFECT) && (DECR(thisv->deathTimer) == 0)) {
            Actor_Kill(&thisv->actor);
        }

        EnClearTag_UpdateEffects(globalCtx);
    }
}

/**
 * EnClear_Tag draw function.
 * Laser clear tag type will draw two lasers.
 * Arwing clear tage types will draw the Arwing, the backfire, and a shadow.
 */
void EnClearTag_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnClearTag* thisv = (EnClearTag*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 983);
    if (thisv->drawMode != CLEAR_TAG_DRAW_MODE_EFFECT) {
        func_80093D84(globalCtx->state.gfxCtx);

        if (thisv->state >= CLEAR_TAG_STATE_LASER) {
            // Draw Arwing lasers.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 255, 0, 255);

            Matrix_Translate(25.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1004),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingLaserDL);

            Matrix_Translate(-50.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1011),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingLaserDL);
        } else {
            // Draw the Arwing itself.
            func_80093D18(globalCtx->state.gfxCtx);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            if (thisv->crashingTimer != 0) {
                f32 xRotation;
                f32 yRotation;
                f32 scaledCrashingTimer = thisv->crashingTimer * 0.05f;

                xRotation = Math_SinS(thisv->frameCounter * 0x3000) * scaledCrashingTimer;
                yRotation = Math_SinS(thisv->frameCounter * 0x3700) * scaledCrashingTimer;
                Matrix_RotateX(xRotation, MTXMODE_APPLY);
                Matrix_RotateY(yRotation, MTXMODE_APPLY);
            }
            Matrix_RotateZ(thisv->roll, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1030),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gArwingDL);

            // Draw the Arwing Backfire
            Matrix_Translate(0.0f, 0.0f, -60.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(2.5f, 1.3f, 0.0f, MTXMODE_APPLY);
            if ((thisv->frameCounter % 2) != 0) {
                Matrix_Scale(1.15f, 1.15f, 1.15f, MTXMODE_APPLY);
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 155);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 0);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1067),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingBackfireDL);

            // Draw the Arwing shadow.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 130);
            Matrix_Translate(thisv->actor.world.pos.x, thisv->actor.floorHeight, thisv->actor.world.pos.z, MTXMODE_NEW);
            Matrix_RotateX(thisv->floorTangent.x, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->floorTangent.z, MTXMODE_APPLY);
            Matrix_Scale(thisv->actor.scale.x + 0.35f, 0.0f, thisv->actor.scale.z + 0.35f, MTXMODE_APPLY);
            Matrix_RotateY((thisv->actor.shape.rot.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateX((thisv->actor.shape.rot.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            Matrix_RotateZ((thisv->actor.shape.rot.z / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
            if (thisv->crashingTimer != 0) {
                f32 xRotation;
                f32 yRotation;
                f32 scaledCrashingTimer = thisv->crashingTimer * 0.05f;

                xRotation = Math_SinS(thisv->frameCounter * 0x3000) * scaledCrashingTimer;
                yRotation = Math_SinS(thisv->frameCounter * 0x3700) * scaledCrashingTimer;
                Matrix_RotateX(xRotation, MTXMODE_APPLY);
                Matrix_RotateY(yRotation, MTXMODE_APPLY);
            }
            Matrix_RotateZ(thisv->roll, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1104),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingShadowDL);
        }
    }

    if (thisv->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
        EnClearTag_DrawEffects(globalCtx);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_clear_tag.c", 1119);
}

/**
 * Updates the Arwing effects.
 * Performs effect physics.
 * Moves and bounces debris effects.
 * Fades most effects out of view. When effects are completely faded away they are removed.
 */
void EnClearTag_UpdateEffects(GlobalContext* globalCtx) {
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;
    s16 i;
    f32 originalYPosition;
    Vec3f sphereCenter;

    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type != CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random++;

            // Perform effect physics.
            effect->position.x += effect->velocity.x;
            originalYPosition = effect->position.y;
            effect->position.y += effect->velocity.y;
            effect->position.z += effect->velocity.z;
            effect->velocity.x += effect->acceleration.x;
            effect->velocity.y += effect->acceleration.y;
            effect->velocity.z += effect->acceleration.z;

            if (effect->type == CLEAR_TAG_EFFECT_DEBRIS) {
                // Clamp the velocity to -5.0
                if (effect->velocity.y < -5.0f) {
                    effect->velocity.y = -5.0f;
                }

                // While the effect is falling check if it has hit the ground.
                if (effect->velocity.y < 0.0f) {
                    sphereCenter = effect->position;
                    sphereCenter.y += 5.0f;

                    // Check if the debris has hit the ground.
                    if (BgCheck_SphVsFirstPoly(&globalCtx->colCtx, &sphereCenter, 11.0f)) {
                        effect->position.y = originalYPosition;

                        // Bounce the debris effect.
                        if (effect->bounces <= 0) {
                            effect->bounces++;
                            effect->velocity.y *= -0.5f;
                            effect->timer = ((s16)Rand_ZeroFloat(20)) + 25;
                        } else {
                            // The Debris effect is done bouncing. Set its velocity and acceleration to 0.
                            effect->velocity.x = effect->velocity.z = effect->acceleration.y = effect->velocity.y =
                                0.0f;
                        }
                    }
                }

                // Rotate the debris effect.
                if (effect->acceleration.y != 0.0f) {
                    effect->rotationY += 0.5f;
                    effect->rotationX += 0.35f;
                }

                // If the timer is completed, unload the debris effect.
                if (effect->timer == 1) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }

                // Spawn a fire effect every 3 frames.
                if (effect->random >= 3) {
                    effect->random = 0;
                    EnClearTag_CreateFireEffect(globalCtx, &effect->position, effect->scale * 8.0f);
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_FIRE) {
                // Fade the fire effect.
                Math_ApproachZeroF(&effect->primColor.a, 1.0f, 15.0f);
                // If the fire effect is fully faded, unload it.
                if (effect->primColor.a <= 0.0f) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_SMOKE) {
                // Fade the smoke effects.
                Math_ApproachZeroF(&effect->primColor.r, 1.0f, 20.0f);
                Math_ApproachZeroF(&effect->primColor.g, 1.0f, 2.0f);
                Math_ApproachZeroF(&effect->envColor.r, 1.0f, 25.5f);
                Math_ApproachZeroF(&effect->envColor.g, 1.0f, 21.5f);
                Math_ApproachZeroF(&effect->envColor.b, 1.0f, 25.5f);

                // Smooth scale the smoke effects.
                Math_ApproachF(&effect->scale, effect->maxScale, 0.05f, 0.1f);

                if (effect->primColor.r == 0.0f) {
                    // Fade the smoke effects.
                    Math_ApproachZeroF(&effect->primColor.a, 1.0f, 3.0f);

                    // If the smoke effect has fully faded, unload it.
                    if (effect->primColor.a <= 0.0f) {
                        effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                    }
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
                // Smooth scale the flash effects.
                Math_ApproachF(&effect->scale, effect->maxScale, 1.0f, 3.0f);
                // Fade the flash effects.
                Math_ApproachZeroF(&effect->primColor.a, 1.0f, 10.0f);

                // If the flash effect has fully faded, unload it.
                if (effect->primColor.a <= 0.0f) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }
            }

            if (effect->timer != 0) {
                effect->timer--;
            }
        }
    }
}

/**
 * Draws the Arwing effects.
 * Each effect type is drawn before the next. The function will apply a material that applies to all effects of that
 * type while drawing the first effect of that type.
 */
void EnClearTag_DrawEffects(GlobalContext* globalCtx) {
    s16 i;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    u8 isMaterialApplied = false;
    EnClearTagEffect* effect = (EnClearTagEffect*)globalCtx->specialEffects;
    EnClearTagEffect* firstEffect = effect;

    OPEN_DISPS(gfxCtx, "../z_en_clear_tag.c", 1288);
    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);

    // Draw all Debris effects.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_DEBRIS) {
            // Apply the debris effect material if it has not already been applied.
            if (!isMaterialApplied) {
                isMaterialApplied++;
                gSPDisplayList(POLY_OPA_DISP++, gArwingDebrisEffectMaterialDL);
            }

            // Draw the debris effect.
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateY(effect->rotationY, MTXMODE_APPLY);
            Matrix_RotateX(effect->rotationX, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_clear_tag.c", 1307),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gArwingDebrisEffectDL);
        }
    }

    // Draw all ground flash effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
            // Apply the flash ground effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 200, 0);
                isMaterialApplied++;
            }

            // Draw the ground flash effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (s8)effect->primColor.a);
            Matrix_Translate(effect->position.x, effect->floorHeight, effect->position.z, MTXMODE_NEW);
            Matrix_RotateX(effect->floorTangent.x, MTXMODE_APPLY);
            Matrix_RotateZ(effect->floorTangent.z, MTXMODE_APPLY);
            Matrix_Scale(effect->scale + effect->scale, 1.0f, effect->scale * 2.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_clear_tag.c", 1342),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFlashEffectGroundDL);
        }
    }

    // Draw all smoke effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_SMOKE) {
            // Apply the smoke effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectMaterialDL);
                isMaterialApplied++;
            }

            // Draw the smoke effect.
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, (s8)effect->envColor.r, (s8)effect->envColor.g, (s8)effect->envColor.b,
                           128);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (s8)effect->primColor.r, (s8)effect->primColor.g,
                            (s8)effect->primColor.b, (s8)effect->primColor.a);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, effect->random * -5, 32, 64, 1, 0, 0, 32, 32));
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 20.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_clear_tag.c", 1392),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectDL);
        }
    }

    // Draw all fire effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FIRE) {
            // Apply the fire effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
                isMaterialApplied++;
            }

            // Draw the fire effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)effect->primColor.a);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (effect->random * -15) & 0xFF, 32, 64, 1, 0, 0,
                                        32, 32));
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_clear_tag.c", 1439),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectDL);
        }
    }

    // Draw all flash billboard effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
            // Apply the flash billboard effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 200, 0);
                isMaterialApplied++;
            }

            // Draw the flash billboard effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (s8)effect->primColor.a);
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_en_clear_tag.c", 1470),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFlashEffectDL);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_en_clear_tag.c", 1477);
}

void EnClearTag_Reset(void) {
    memset(sClearTagEffects, 0, sizeof(sClearTagEffects));
    sIsEffectsInitialized = false;
}