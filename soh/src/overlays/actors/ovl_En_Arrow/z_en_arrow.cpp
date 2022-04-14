/*
 * File: z_en_arrow.c
 * Overlay: ovl_En_Arrow
 * Description: Arrow, Deku Seed, and Deku Nut Projectile
 */

#include "z_en_arrow.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnArrow_Init(Actor* thisx, GlobalContext* globalCtx);
void EnArrow_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnArrow_Update(Actor* thisx, GlobalContext* globalCtx);
void EnArrow_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnArrow_Shoot(EnArrow* thisv, GlobalContext* globalCtx);
void EnArrow_Fly(EnArrow* thisv, GlobalContext* globalCtx);
void func_809B45E0(EnArrow* thisv, GlobalContext* globalCtx);
void func_809B4640(EnArrow* thisv, GlobalContext* globalCtx);

const ActorInit En_Arrow_InitVars = {
    ACTOR_EN_ARROW,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnArrow),
    (ActorFunc)EnArrow_Init,
    (ActorFunc)EnArrow_Destroy,
    (ActorFunc)EnArrow_Update,
    (ActorFunc)EnArrow_Draw,
    NULL,
};

static ColliderQuadInit sColliderInit = {
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
        { 0x00000020, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(minVelocityY, -150, ICHAIN_STOP),
};

void EnArrow_SetupAction(EnArrow* thisv, EnArrowActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnArrow_Init(Actor* thisx, GlobalContext* globalCtx) {
    static EffectBlureInit2 blureNormal = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 255, 170, 255 }, { 0, 150, 0, 0 },
    };
    static EffectBlureInit2 blureFire = {
        0, 4, 0, { 0, 255, 200, 255 }, { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 200, 0, 255 }, { 255, 0, 0, 0 },
    };
    static EffectBlureInit2 blureIce = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 170, 255, 255, 255 }, { 0, 100, 255, 0 },
    };
    static EffectBlureInit2 blureLight = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 255, 170, 255 }, { 255, 255, 0, 0 },
    };
    static u32 dmgFlags[] = {
        0x00000800, 0x00000020, 0x00000020, 0x00000800, 0x00001000,
        0x00002000, 0x00010000, 0x00004000, 0x00008000, 0x00000004,
    };
    EnArrow* thisv = (EnArrow*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);

    if (thisv->actor.params == ARROW_CS_NUT) {
        thisv->isCsNut = true;
        thisv->actor.params = ARROW_NUT;
    }

    if (thisv->actor.params <= ARROW_SEED) {

        if (thisv->actor.params <= ARROW_0E) {
            SkelAnime_Init(globalCtx, &thisv->skelAnime, &gArrowSkel, &gArrow2Anim, NULL, NULL, 0);
        }

        if (thisv->actor.params <= ARROW_NORMAL) {
            if (thisv->actor.params == ARROW_NORMAL_HORSE) {
                blureNormal.elemDuration = 4;
            } else {
                blureNormal.elemDuration = 16;
            }

            Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE2, 0, 0, &blureNormal);

        } else if (thisv->actor.params == ARROW_FIRE) {

            Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE2, 0, 0, &blureFire);

        } else if (thisv->actor.params == ARROW_ICE) {

            Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE2, 0, 0, &blureIce);

        } else if (thisv->actor.params == ARROW_LIGHT) {

            Effect_Add(globalCtx, &thisv->effectIndex, EFFECT_BLURE2, 0, 0, &blureLight);
        }

        Collider_InitQuad(globalCtx, &thisv->collider);
        Collider_SetQuad(globalCtx, &thisv->collider, &thisv->actor, &sColliderInit);

        if (thisv->actor.params <= ARROW_NORMAL) {
            thisv->collider.info.toucherFlags &= ~0x18;
            thisv->collider.info.toucherFlags |= 0;
        }

        if (thisv->actor.params < 0) {
            thisv->collider.base.atFlags = (AT_ON | AT_TYPE_ENEMY);
        } else if (thisv->actor.params <= ARROW_SEED) {
            thisv->collider.info.toucher.dmgFlags = dmgFlags[thisv->actor.params];
            LOG_HEX("thisv->at_info.cl_elem.at_btl_info.at_type", thisv->collider.info.toucher.dmgFlags,
                    "../z_en_arrow.c", 707);
        }
    }

    EnArrow_SetupAction(thisv, EnArrow_Shoot);
}

void EnArrow_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnArrow* thisv = (EnArrow*)thisx;

    if (thisv->actor.params <= ARROW_LIGHT) {
        Effect_Delete(globalCtx, thisv->effectIndex);
    }

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyQuad(globalCtx, &thisv->collider);

    if ((thisv->hitActor != NULL) && (thisv->hitActor->update != NULL)) {
        thisv->hitActor->flags &= ~ACTOR_FLAG_15;
    }
}

void EnArrow_Shoot(EnArrow* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->actor.parent == NULL) {
        if ((thisv->actor.params != ARROW_NUT) && (player->unk_A73 == 0)) {
            Actor_Kill(&thisv->actor);
            return;
        }

        switch (thisv->actor.params) {
            case ARROW_SEED:
                func_8002F7DC(&player->actor, NA_SE_IT_SLING_SHOT);
                break;

            case ARROW_NORMAL_LIT:
            case ARROW_NORMAL_HORSE:
            case ARROW_NORMAL:
                func_8002F7DC(&player->actor, NA_SE_IT_ARROW_SHOT);
                break;

            case ARROW_FIRE:
            case ARROW_ICE:
            case ARROW_LIGHT:
                func_8002F7DC(&player->actor, NA_SE_IT_MAGIC_ARROW_SHOT);
                break;
        }

        EnArrow_SetupAction(thisv, EnArrow_Fly);
        Math_Vec3f_Copy(&thisv->unk_210, &thisv->actor.world.pos);

        if (thisv->actor.params >= ARROW_SEED) {
            func_8002D9A4(&thisv->actor, 80.0f);
            thisv->timer = 15;
            thisv->actor.shape.rot.x = thisv->actor.shape.rot.y = thisv->actor.shape.rot.z = 0;
        } else {
            func_8002D9A4(&thisv->actor, 150.0f);
            thisv->timer = 12;
        }
    }
}

void func_809B3CEC(GlobalContext* globalCtx, EnArrow* thisv) {
    EnArrow_SetupAction(thisv, func_809B4640);
    Animation_PlayOnce(&thisv->skelAnime, &gArrow1Anim);
    thisv->actor.world.rot.y += (s32)(24576.0f * (Rand_ZeroOne() - 0.5f)) + 0x8000;
    thisv->actor.velocity.y += (thisv->actor.speedXZ * (0.4f + (0.4f * Rand_ZeroOne())));
    thisv->actor.speedXZ *= (0.04f + 0.3f * Rand_ZeroOne());
    thisv->timer = 50;
    thisv->actor.gravity = -1.5f;
}

void EnArrow_CarryActor(EnArrow* thisv, GlobalContext* globalCtx) {
    CollisionPoly* hitPoly;
    Vec3f posDiffLastFrame;
    Vec3f actorNextPos;
    Vec3f hitPos;
    f32 temp_f12;
    f32 scale;
    s32 bgId;

    Math_Vec3f_Diff(&thisv->actor.world.pos, &thisv->unk_210, &posDiffLastFrame);

    temp_f12 = ((thisv->actor.world.pos.x - thisv->hitActor->world.pos.x) * posDiffLastFrame.x) +
               ((thisv->actor.world.pos.y - thisv->hitActor->world.pos.y) * posDiffLastFrame.y) +
               ((thisv->actor.world.pos.z - thisv->hitActor->world.pos.z) * posDiffLastFrame.z);

    if (!(temp_f12 < 0.0f)) {
        scale = Math3D_Vec3fMagnitudeSq(&posDiffLastFrame);

        if (!(scale < 1.0f)) {
            scale = temp_f12 / scale;
            Math_Vec3f_Scale(&posDiffLastFrame, scale);
            Math_Vec3f_Sum(&thisv->hitActor->world.pos, &posDiffLastFrame, &actorNextPos);

            if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->hitActor->world.pos, &actorNextPos, &hitPos,
                                        &hitPoly, true, true, true, true, &bgId)) {
                thisv->hitActor->world.pos.x = hitPos.x + ((actorNextPos.x <= hitPos.x) ? 1.0f : -1.0f);
                thisv->hitActor->world.pos.y = hitPos.y + ((actorNextPos.y <= hitPos.y) ? 1.0f : -1.0f);
                thisv->hitActor->world.pos.z = hitPos.z + ((actorNextPos.z <= hitPos.z) ? 1.0f : -1.0f);
            } else {
                Math_Vec3f_Copy(&thisv->hitActor->world.pos, &actorNextPos);
            }
        }
    }
}

void EnArrow_Fly(EnArrow* thisv, GlobalContext* globalCtx) {
    CollisionPoly* hitPoly;
    s32 bgId;
    Vec3f hitPoint;
    Vec3f posCopy;
    s32 atTouched;
    u16 sfxId;
    Actor* hitActor;
    Vec3f sp60;
    Vec3f sp54;

    if (DECR(thisv->timer) == 0) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->timer < 7.2000003f) {
        thisv->actor.gravity = -0.4f;
    }

    atTouched = (thisv->actor.params != ARROW_NORMAL_LIT) && (thisv->actor.params <= ARROW_SEED) &&
                (thisv->collider.base.atFlags & AT_HIT);

    if (atTouched || thisv->touchedPoly) {
        if (thisv->actor.params >= ARROW_SEED) {
            if (atTouched) {
                thisv->actor.world.pos.x = (thisv->actor.world.pos.x + thisv->actor.prevPos.x) * 0.5f;
                thisv->actor.world.pos.y = (thisv->actor.world.pos.y + thisv->actor.prevPos.y) * 0.5f;
                thisv->actor.world.pos.z = (thisv->actor.world.pos.z + thisv->actor.prevPos.z) * 0.5f;
            }

            if (thisv->actor.params == ARROW_NUT) {
                iREG(50) = -1;
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_M_FIRE1, thisv->actor.world.pos.x,
                            thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
                sfxId = NA_SE_IT_DEKU;
            } else {
                sfxId = NA_SE_IT_SLING_REFLECT;
            }

            EffectSsStone1_Spawn(globalCtx, &thisv->actor.world.pos, 0);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, sfxId);
            Actor_Kill(&thisv->actor);
        } else {
            EffectSsHitMark_SpawnCustomScale(globalCtx, 0, 150, &thisv->actor.world.pos);

            if (atTouched && (thisv->collider.info.atHitInfo->elemType != ELEMTYPE_UNK4)) {
                hitActor = thisv->collider.base.at;

                if ((hitActor->update != NULL) && (!(thisv->collider.base.atFlags & AT_BOUNCED)) &&
                    (hitActor->flags & ACTOR_FLAG_14)) {
                    thisv->hitActor = hitActor;
                    EnArrow_CarryActor(thisv, globalCtx);
                    Math_Vec3f_Diff(&hitActor->world.pos, &thisv->actor.world.pos, &thisv->unk_250);
                    hitActor->flags |= ACTOR_FLAG_15;
                    thisv->collider.base.atFlags &= ~AT_HIT;
                    thisv->actor.speedXZ /= 2.0f;
                    thisv->actor.velocity.y /= 2.0f;
                } else {
                    thisv->hitFlags |= 1;
                    thisv->hitFlags |= 2;

                    if (thisv->collider.info.atHitInfo->bumperFlags & 2) {
                        thisv->actor.world.pos.x = thisv->collider.info.atHitInfo->bumper.hitPos.x;
                        thisv->actor.world.pos.y = thisv->collider.info.atHitInfo->bumper.hitPos.y;
                        thisv->actor.world.pos.z = thisv->collider.info.atHitInfo->bumper.hitPos.z;
                    }

                    func_809B3CEC(globalCtx, thisv);
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_ARROW_STICK_CRE);
                }
            } else if (thisv->touchedPoly) {
                EnArrow_SetupAction(thisv, func_809B45E0);
                Animation_PlayOnce(&thisv->skelAnime, &gArrow2Anim);

                if (thisv->actor.params >= ARROW_NORMAL_LIT) {
                    thisv->timer = 60;
                } else {
                    thisv->timer = 20;
                }

                Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_ARROW_STICK_OBJ);
                thisv->hitFlags |= 1;
            }
        }
    } else {
        Math_Vec3f_Copy(&thisv->unk_210, &thisv->actor.world.pos);
        Actor_MoveForward(&thisv->actor);

        if ((thisv->touchedPoly =
                 BgCheck_ProjectileLineTest(&globalCtx->colCtx, &thisv->actor.prevPos, &thisv->actor.world.pos, &hitPoint,
                                            &thisv->actor.wallPoly, true, true, true, true, &bgId))) {
            func_8002F9EC(globalCtx, &thisv->actor, thisv->actor.wallPoly, bgId, &hitPoint);
            Math_Vec3f_Copy(&posCopy, &thisv->actor.world.pos);
            Math_Vec3f_Copy(&thisv->actor.world.pos, &hitPoint);
        }

        if (thisv->actor.params <= ARROW_0E) {
            thisv->actor.shape.rot.x = Math_Atan2S(thisv->actor.speedXZ, -thisv->actor.velocity.y);
        }
    }

    if (thisv->hitActor != NULL) {
        if (thisv->hitActor->update != NULL) {
            Math_Vec3f_Sum(&thisv->unk_210, &thisv->unk_250, &sp60);
            Math_Vec3f_Sum(&thisv->actor.world.pos, &thisv->unk_250, &sp54);

            if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &sp60, &sp54, &hitPoint, &hitPoly, true, true, true, true,
                                        &bgId)) {
                thisv->hitActor->world.pos.x = hitPoint.x + ((sp54.x <= hitPoint.x) ? 1.0f : -1.0f);
                thisv->hitActor->world.pos.y = hitPoint.y + ((sp54.y <= hitPoint.y) ? 1.0f : -1.0f);
                thisv->hitActor->world.pos.z = hitPoint.z + ((sp54.z <= hitPoint.z) ? 1.0f : -1.0f);
                Math_Vec3f_Diff(&thisv->hitActor->world.pos, &thisv->actor.world.pos, &thisv->unk_250);
                thisv->hitActor->flags &= ~ACTOR_FLAG_15;
                thisv->hitActor = NULL;
            } else {
                Math_Vec3f_Sum(&thisv->actor.world.pos, &thisv->unk_250, &thisv->hitActor->world.pos);
            }

            if (thisv->touchedPoly && (thisv->hitActor != NULL)) {
                thisv->hitActor->flags &= ~ACTOR_FLAG_15;
                thisv->hitActor = NULL;
            }
        } else {
            thisv->hitActor = NULL;
        }
    }
}

void func_809B45E0(EnArrow* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (DECR(thisv->timer) == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_809B4640(EnArrow* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Actor_MoveForward(&thisv->actor);

    if (DECR(thisv->timer) == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnArrow_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnArrow* thisv = (EnArrow*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->isCsNut || ((thisv->actor.params >= ARROW_NORMAL_LIT) && (player->unk_A73 != 0)) ||
        !Player_InBlockingCsMode(globalCtx, player)) {
        thisv->actionFunc(thisv, globalCtx);
    }

    if ((thisv->actor.params >= ARROW_FIRE) && (thisv->actor.params <= ARROW_0E)) {
        s16 elementalActorIds[] = { ACTOR_ARROW_FIRE, ACTOR_ARROW_ICE,  ACTOR_ARROW_LIGHT,
                                    ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE };

        if (thisv->actor.child == NULL) {
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, elementalActorIds[thisv->actor.params - 3],
                               thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0);
        }
    } else if (thisv->actor.params == ARROW_NORMAL_LIT) {
        static Vec3f velocity = { 0.0f, 0.5f, 0.0f };
        static Vec3f accel = { 0.0f, 0.5f, 0.0f };
        static Color_RGBA8 primColor = { 255, 255, 100, 255 };
        static Color_RGBA8 envColor = { 255, 50, 0, 0 };
        // spawn dust for the flame
        func_8002836C(globalCtx, &thisv->unk_21C, &velocity, &accel, &primColor, &envColor, 100, 0, 8);
    }
}

void func_809B4800(EnArrow* thisv, GlobalContext* globalCtx) {
    static Vec3f D_809B4E88 = { 0.0f, 400.0f, 1500.0f };
    static Vec3f D_809B4E94 = { 0.0f, -400.0f, 1500.0f };
    static Vec3f D_809B4EA0 = { 0.0f, 0.0f, -300.0f };
    Vec3f sp44;
    Vec3f sp38;
    s32 addBlureVertex;

    Matrix_MultVec3f(&D_809B4EA0, &thisv->unk_21C);

    if (EnArrow_Fly == thisv->actionFunc) {
        Matrix_MultVec3f(&D_809B4E88, &sp44);
        Matrix_MultVec3f(&D_809B4E94, &sp38);

        if (thisv->actor.params <= ARROW_SEED) {
            addBlureVertex = thisv->actor.params <= ARROW_LIGHT;

            if (thisv->hitActor == NULL) {
                addBlureVertex &= func_80090480(globalCtx, &thisv->collider, &thisv->weaponInfo, &sp44, &sp38);
            } else {
                if (addBlureVertex) {
                    if ((sp44.x == thisv->weaponInfo.tip.x) && (sp44.y == thisv->weaponInfo.tip.y) &&
                        (sp44.z == thisv->weaponInfo.tip.z) && (sp38.x == thisv->weaponInfo.base.x) &&
                        (sp38.y == thisv->weaponInfo.base.y) && (sp38.z == thisv->weaponInfo.base.z)) {
                        addBlureVertex = false;
                    }
                }
            }

            if (addBlureVertex) {
                EffectBlure_AddVertex(Effect_GetByIndex(thisv->effectIndex), &sp44, &sp38);
            }
        }
    }
}

void EnArrow_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnArrow* thisv = (EnArrow*)thisx;
    u8 alpha;
    f32 scale;

    if (thisv->actor.params <= ARROW_0E) {
        func_80093D18(globalCtx->state.gfxCtx);
        SkelAnime_DrawLod(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, thisv,
                          (thisv->actor.projectedPos.z < MREG(95)) ? 0 : 1);
    } else if (thisv->actor.speedXZ != 0.0f) {
        alpha = (Math_CosS(thisv->timer * 5000) * 127.5f) + 127.5f;

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_arrow.c", 1346);

        func_80093C14(globalCtx->state.gfxCtx);

        if (thisv->actor.params == ARROW_SEED) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, alpha);
            scale = 50.0f;
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 12, 0, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 0, alpha);
            scale = 150.0f;
        }

        Matrix_Push();
        Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
        // redundant check because thisv is contained in an if block for non-zero speed
        Matrix_RotateZ((thisv->actor.speedXZ == 0.0f) ? 0.0f
                                                     : ((globalCtx->gameplayFrames & 0xFF) * 4000) * (std::numbers::pi_v<float> / 0x8000),
                       MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_arrow.c", 1374),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
        Matrix_Pop();
        Matrix_RotateY(thisv->actor.world.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_arrow.c", 1381);
    }

    func_809B4800(thisv, globalCtx);
}
