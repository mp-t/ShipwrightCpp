/*
 * File: z_en_boom.c
 * Overlay: ovl_En_Boom
 * Description: Thrown Boomerang. Actor spawns when thrown and is killed when caught.
 */

#include "z_en_boom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnBoom_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBoom_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBoom_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBoom_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBoom_Fly(EnBoom* thisv, GlobalContext* globalCtx);

ActorInit En_Boom_InitVars = {
    ACTOR_EN_BOOM,
    ACTORCAT_MISC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnBoom),
    (ActorFunc)EnBoom_Init,
    (ActorFunc)EnBoom_Destroy,
    (ActorFunc)EnBoom_Update,
    (ActorFunc)EnBoom_Draw,
    NULL,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000010, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3S(shape.rot, 0, ICHAIN_STOP),
};

void EnBoom_SetupAction(EnBoom* thisv, EnBoomActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnBoom_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBoom* thisv = (EnBoom*)thisx;
    EffectBlureInit1 blure;

    thisv->actor.room = -1;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    blure.p1StartColor[0] = 255;
    blure.p1StartColor[1] = 255;
    blure.p1StartColor[2] = 100;
    blure.p1StartColor[3] = 255;

    blure.p2StartColor[0] = 255;
    blure.p2StartColor[1] = 255;
    blure.p2StartColor[2] = 100;
    blure.p2StartColor[3] = 64;

    blure.p1EndColor[0] = 255;
    blure.p1EndColor[1] = 255;
    blure.p1EndColor[2] = 100;
    blure.p1EndColor[3] = 0;

    blure.p2EndColor[0] = 255;
    blure.p2EndColor[1] = 255;
    blure.p2EndColor[2] = 100;
    blure.p2EndColor[3] = 0;

    blure.elemDuration = 8;
    blure.unkFlag = 0;
    blure.calcMode = 0;

    Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE1, 0, 0, &blure);

    Collider_InitQuad(globalCtx, &thisv->collider);
    Collider_SetQuad(globalCtx, &thisv->collider, &thisv->actor, &sQuadInit);

    EnBoom_SetupAction(thisv, EnBoom_Fly);
}

void EnBoom_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBoom* thisv = (EnBoom*)thisx;

    Effect_Delete(globalCtx, thisv->effectIndex);
    Collider_DestroyQuad(globalCtx, &thisv->collider);
}

void EnBoom_Fly(EnBoom* thisv, GlobalContext* globalCtx) {
    Actor* target;
    Player* player;
    s32 collided;
    s16 yawTarget;
    s16 yawDiff;
    s16 pitchTarget;
    s16 pitchDiff;
    s32 pad1;
    f32 distXYZScale;
    f32 distFromLink;
    DynaPolyActor* hitActor;
    s32 hitDynaID;
    Vec3f hitPoint;
    s32 pad2;

    player = GET_PLAYER(globalCtx);
    target = thisv->moveTo;

    // If the boomerang is moving toward a targeted actor, handle setting the proper x and y angle to fly toward it.
    if (target != NULL) {
        yawTarget = Actor_WorldYawTowardPoint(&thisv->actor, &target->focus.pos);
        yawDiff = thisv->actor.world.rot.y - yawTarget;

        pitchTarget = Actor_WorldPitchTowardPoint(&thisv->actor, &target->focus.pos);
        pitchDiff = thisv->actor.world.rot.x - pitchTarget;

        distXYZScale = (200.0f - Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &target->focus.pos)) * 0.005f;
        if (distXYZScale < 0.12f) {
            distXYZScale = 0.12f;
        }

        if ((target != &player->actor) && ((target->update == NULL) || (ABS(yawDiff) > 0x4000))) {
            //! @bug  This condition is why the boomerang will randomly fly off in a the down left direction sometimes.
            //      If the actor targetted is not Link and the difference between the 2 y angles is greater than 0x4000,
            //      the moveTo pointer is nulled and it flies off in a seemingly random direction.
            thisv->moveTo = NULL;
        } else {
            Math_ScaledStepToS(&thisv->actor.world.rot.y, yawTarget, (s16)(ABS(yawDiff) * distXYZScale));
            Math_ScaledStepToS(&thisv->actor.world.rot.x, pitchTarget, (s16)(ABS(pitchDiff) * distXYZScale));
        }
    }

    // Set xyz speed, move forward, and play the boomerang sound
    func_8002D9A4(&thisv->actor, 12.0f);
    Actor_MoveForward(&thisv->actor);
    func_8002F974(&thisv->actor, NA_SE_IT_BOOMERANG_FLY - SFX_FLAG);

    // If the boomerang collides with EnItem00 or a Skulltula token, set grabbed pointer to pick it up
    collided = thisv->collider.base.atFlags & AT_HIT;
    collided = !!(collided);
    if (collided) {
        if (((thisv->collider.base.at->id == ACTOR_EN_ITEM00) || (thisv->collider.base.at->id == ACTOR_EN_SI))) {
            thisv->grabbed = thisv->collider.base.at;
            if (thisv->collider.base.at->id == ACTOR_EN_SI) {
                thisv->collider.base.at->flags |= ACTOR_FLAG_13;
            }
        }
    }

    // Decrement the return timer and check if it's 0. If it is, check if Link can catch it and handle accordingly.
    // Otherwise handle grabbing and colliding.
    if (DECR(thisv->returnTimer) == 0) {
        distFromLink = Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &player->actor.focus.pos);
        thisv->moveTo = &player->actor;

        // If the boomerang is less than 40 units away from Link, he can catch it.
        if (distFromLink < 40.0f) {
            target = thisv->grabbed;
            if (target != NULL) {
                Math_Vec3f_Copy(&target->world.pos, &player->actor.world.pos);

                // If the grabbed actor is EnItem00 (HP/Key etc) set gravity and flags so it falls in front of Link.
                // Otherwise if it's a Skulltula Token, just set flags so he collides with it to collect it.
                if (target->id == ACTOR_EN_ITEM00) {
                    target->gravity = -0.9f;
                    target->bgCheckFlags &= ~0x03;
                } else {
                    target->flags &= ~ACTOR_FLAG_13;
                }
            }
            // Set player flags and kill the boomerang beacause Link caught it.
            player->stateFlags1 &= ~0x02000000;
            Actor_Kill(&thisv->actor);
        }
    } else {
        collided = (thisv->collider.base.atFlags & AT_HIT);
        collided = (!!(collided));
        if (collided) {
            // Copy the position from the prevous frame to the boomerang to start the bounce back.
            Math_Vec3f_Copy(&thisv->actor.world.pos, &thisv->actor.prevPos);
        } else {
            collided = BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.prevPos, &thisv->actor.world.pos,
                                               &hitPoint, &thisv->actor.wallPoly, true, true, true, true, &hitDynaID);

            if (collided) {
                // If the boomerang collides with something and it's is a Jabu Object actor with params equal to 0, then
                // set collided to 0 so that the boomerang will go through the wall.
                // Otherwise play a clank sound and keep collided set to bounce back.
                if (func_8002F9EC(globalCtx, &thisv->actor, thisv->actor.wallPoly, hitDynaID, &hitPoint) != 0 ||
                    (hitDynaID != BGCHECK_SCENE &&
                     ((hitActor = DynaPoly_GetActor(&globalCtx->colCtx, hitDynaID)) != NULL) &&
                     hitActor->actor.id == ACTOR_BG_BDAN_OBJECTS && hitActor->actor.params == 0)) {
                    collided = false;
                } else {
                    CollisionCheck_SpawnShieldParticlesMetal(globalCtx, &hitPoint);
                }
            }
        }

        // If the boomerang needs to bounce back, set x and y angle accordingly.
        // Set timer to 0 and set return actor to player so it goes back to Link.
        if (collided) {
            thisv->actor.world.rot.x = -thisv->actor.world.rot.x;
            thisv->actor.world.rot.y += 0x8000;
            thisv->moveTo = &player->actor;
            thisv->returnTimer = 0;
        }
    }

    // If the actor the boomerang is holding has a null update function, set grabbed to null.
    // Otherwise, copy the position from the boomerang to the actor to move it.
    target = thisv->grabbed;
    if (target != NULL) {
        if (target->update == NULL) {
            thisv->grabbed = NULL;
        } else {
            Math_Vec3f_Copy(&target->world.pos, &thisv->actor.world.pos);
        }
    }
}

void EnBoom_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBoom* thisv = (EnBoom*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    if (!(player->stateFlags1 & 0x20000000)) {
        thisv->actionFunc(thisv, globalCtx);
        Actor_SetFocus(&thisv->actor, 0.0f);
        thisv->activeTimer = thisv->activeTimer + 1;
    }
}

void EnBoom_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f sMultVec1 = { -960.0f, 0.0f, 0.0f };
    static Vec3f sMultVec2 = { 960.0f, 0.0f, 0.0f };
    EnBoom* thisv = (EnBoom*)thisx;
    Vec3f vec1;
    Vec3f vec2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_boom.c", 567);

    Matrix_RotateY(thisv->actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(0x1F40 * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_RotateX(thisv->actor.world.rot.x * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
    Matrix_MultVec3f(&sMultVec1, &vec1);
    Matrix_MultVec3f(&sMultVec2, &vec2);

    if (func_80090480(globalCtx, &thisv->collider, &thisv->boomerangInfo, &vec1, &vec2) != 0) {
        EffectBlure_AddVertex(static_cast<EffectBlure*>(Effect_GetByIndex(thisv->effectIndex)), &vec1, &vec2);
    }

    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_RotateY((thisv->activeTimer * 12000) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_boom.c", 601),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBoomerangRefDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_boom.c", 604);
}
