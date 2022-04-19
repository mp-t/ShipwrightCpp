/*
 * File: z_en_zo.c
 * Overlay: ovl_En_Zo
 * Description: Zora
 */

#include "z_en_zo.h"
#include "objects/object_zo/object_zo.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

typedef enum {
    /* 0 */ ENZO_EFFECT_NONE,
    /* 1 */ ENZO_EFFECT_RIPPLE,
    /* 2 */ ENZO_EFFECT_SPLASH,
    /* 3 */ ENZO_EFFECT_BUBBLE
} EnZoEffectType;

void EnZo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnZo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnZo_Update(Actor* thisx, GlobalContext* globalCtx);
void EnZo_Draw(Actor* thisx, GlobalContext* globalCtx);

// Actions
void EnZo_Standing(EnZo* thisv, GlobalContext* globalCtx);
void EnZo_Submerged(EnZo* thisv, GlobalContext* globalCtx);
void EnZo_Surface(EnZo* thisv, GlobalContext* globalCtx);
void EnZo_TreadWater(EnZo* thisv, GlobalContext* globalCtx);
void EnZo_Dive(EnZo* thisv, GlobalContext* globalCtx);

void EnZo_Ripple(EnZo* thisv, Vec3f* pos, f32 scale, f32 targetScale, u8 alpha) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    s16 i;

    effect = thisv->effects;
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_NONE) {
            effect->type = ENZO_EFFECT_RIPPLE;
            effect->pos = *pos;
            effect->scale = scale;
            effect->targetScale = targetScale;
            effect->color.a = alpha;
            break;
        }
        effect++;
    }
}

void EnZo_Bubble(EnZo* thisv, Vec3f* pos) {
    EnZoEffect* effect;
    Vec3f vec = { 0.0f, 0.0f, 0.0f };
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    f32 waterSurface;

    effect = thisv->effects;
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (1) {}
        if (effect->type == ENZO_EFFECT_NONE) {
            waterSurface = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
            if (!(waterSurface <= pos->y)) {
                effect->type = ENZO_EFFECT_BUBBLE;
                effect->pos = *pos;
                effect->vec = *pos;
                effect->vel = vel;
                effect->scale = ((Rand_ZeroOne() - 0.5f) * 0.02f) + 0.12f;
                break;
            }
        }
        effect++;
    }
}

void EnZo_Splash(EnZo* thisv, Vec3f* pos, Vec3f* vel, f32 scale) {
    EnZoEffect* effect;
    Vec3f accel = { 0.0f, -1.0f, 0.0f };
    s16 i;

    effect = thisv->effects;
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (1) {}
        if (effect->type != ENZO_EFFECT_SPLASH) {
            effect->type = ENZO_EFFECT_SPLASH;
            effect->pos = *pos;
            effect->vec = accel;
            effect->vel = *vel;
            effect->color.a = (Rand_ZeroOne() * 100.0f) + 100.0f;
            effect->scale = scale;
            break;
        }
        effect++;
    }
}

void EnZo_UpdateRipples(EnZo* thisv) {
    EnZoEffect* effect = thisv->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            Math_ApproachF(&effect->scale, effect->targetScale, 0.2f, 0.8f);
            if (effect->color.a > 20) {
                effect->color.a -= 20;
            } else {
                effect->color.a = 0;
            }

            if (effect->color.a == 0) {
                effect->type = ENZO_EFFECT_NONE;
            }
        }
        effect++;
    }
}

void EnZo_UpdateBubbles(EnZo* thisv) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = thisv->effects;
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            effect->pos.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.x;
            effect->pos.z = ((Rand_ZeroOne() * 0.5f) - 0.25f) + effect->vec.z;
            effect->pos.y += effect->vel.y;

            // Bubbles turn into ripples when they reach the surface
            waterSurface = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
            if (waterSurface <= effect->pos.y) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_Ripple(thisv, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_UpdateSplashes(EnZo* thisv) {
    EnZoEffect* effect;
    f32 waterSurface;
    s16 i;

    effect = thisv->effects;
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;

            if (effect->vel.y >= -20.0f) {
                effect->vel.y += effect->vec.y;
            } else {
                effect->vel.y = -20.0f;
                effect->vec.y = 0.0f;
            }

            // Splash particles turn into ripples when they hit the surface
            waterSurface = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
            if (effect->pos.y < waterSurface) {
                effect->type = ENZO_EFFECT_NONE;
                effect->pos.y = waterSurface;
                EnZo_Ripple(thisv, &effect->pos, 0.06f, 0.12f, 200);
            }
        }
        effect++;
    }
}

void EnZo_DrawRipples(EnZo* thisv, GlobalContext* globalCtx) {
    EnZoEffect* effect;
    s16 i;
    u8 setup;

    effect = thisv->effects;
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 217);
    setup = false;
    func_80093D84(globalCtx->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_RIPPLE) {
            if (!setup) {
                if (1) {}
                gDPPipeSync(POLY_XLU_DISP++);
                gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                setup = true;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->color.a);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 242),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gZoraRipplesModelDL);
        }
        effect++;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 248);
}

void EnZo_DrawBubbles(EnZo* thisv, GlobalContext* globalCtx) {
    EnZoEffect* effect = thisv->effects;
    s16 i;
    u8 setup;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 260);
    setup = false;
    func_80093D84(globalCtx->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_BUBBLE) {
            if (!setup) {
                if (1) {}
                gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesMaterialDL);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

                setup = true;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 281),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gZoraBubblesModelDL);
        }
        effect++;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 286);
}

void EnZo_DrawSplashes(EnZo* thisv, GlobalContext* globalCtx) {
    EnZoEffect* effect;
    s16 i;
    u8 setup;

    effect = thisv->effects;
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 298);
    setup = false;
    func_80093D84(globalCtx->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        if (effect->type == ENZO_EFFECT_SPLASH) {
            if (!setup) {
                if (1) {}
                gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesMaterialDL);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
                setup = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->color.a);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 325),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gZoraSplashesModelDL);
        }
        effect++;
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zo_eff.c", 331);
}

void EnZo_TreadWaterRipples(EnZo* thisv, f32 scale, f32 targetScale, u8 alpha) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
    pos.z = thisv->actor.world.pos.z;
    EnZo_Ripple(thisv, &pos, scale, targetScale, alpha);
}

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 26, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorInit En_Zo_InitVars = {
    ACTOR_EN_ZO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZO,
    sizeof(EnZo),
    (ActorFunc)EnZo_Init,
    (ActorFunc)EnZo_Destroy,
    (ActorFunc)EnZo_Update,
    (ActorFunc)EnZo_Draw,
    NULL,
};

typedef enum {
    /* 0 */ ENZO_ANIM_0,
    /* 1 */ ENZO_ANIM_1,
    /* 2 */ ENZO_ANIM_2,
    /* 3 */ ENZO_ANIM_3,
    /* 4 */ ENZO_ANIM_4,
    /* 5 */ ENZO_ANIM_5,
    /* 6 */ ENZO_ANIM_6,
    /* 7 */ ENZO_ANIM_7
} EnZoAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gZoraSurfaceAnim, 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f },
    { &gZoraSurfaceAnim, 1.0f, 1.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraSurfaceAnim, 1.0f, 8.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraThrowRupeesAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraHandsOnHipsTappingFootAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gZoraOpenArmsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

void EnZo_SpawnSplashes(EnZo* thisv) {
    Vec3f pos;
    Vec3f vel;
    s32 i;

    // Convert 20 particles into splashes (all of them since there are only 15)
    for (i = 0; i < 20; i++) {
        f32 speed = Rand_ZeroOne() * 1.5f + 0.5f;
        f32 angle = Rand_ZeroOne() * 6.28f; // ~pi * 2

        vel.y = Rand_ZeroOne() * 3.0f + 3.0f;

        vel.x = sinf(angle) * speed;
        vel.z = cosf(angle) * speed;

        pos = thisv->actor.world.pos;
        pos.x += vel.x * 6.0f;
        pos.z += vel.z * 6.0f;
        pos.y += thisv->actor.yDistToWater;
        EnZo_Splash(thisv, &pos, &vel, 0.08f);
    }
}

u16 func_80B61024(GlobalContext* globalCtx, Actor* thisx) {
    u16 textId;

    textId = Text_GetFaceReaction(globalCtx, 29);
    if (textId != 0) {
        return textId;
    }

    switch (thisx->params & 0x3F) {
        case 8:
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x402A;
            }
            break;

        case 6:
            return 0x4020;

        case 7:
            return 0x4021;

        case 0:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x4007;
            }
            break;

        case 1:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }

            if (gSaveContext.eventChkInf[3] & 1) {
                return (gSaveContext.infTable[18] & 0x10) ? 0x4009 : 0x4008;
            }
            break;

        case 2:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }
            if (gSaveContext.eventChkInf[3] & 2) {
                return (gSaveContext.infTable[18] & 0x200) ? 0x400B : 0x402F;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400A;
            }
            break;

        case 3:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400C;
            }
            break;

        case 4:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402D;
            }

            if (gSaveContext.eventChkInf[3] & 8) {
                return 0x4010;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x400F;
            }
            break;

        case 5:
            if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
                return 0x402E;
            }
            if (gSaveContext.eventChkInf[3] & 1) {
                return 0x4011;
            }
            break;
    }
    return 0x4006;
}

s16 func_80B61298(GlobalContext* globalCtx, Actor* thisx) {
    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return 1;

        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x4020:
                case 0x4021:
                    return 0;
                case 0x4008:
                    gSaveContext.infTable[18] |= 0x10;
                    break;
                case 0x402F:
                    gSaveContext.infTable[18] |= 0x200;
                    break;
            }
            gSaveContext.eventChkInf[3] |= 1;
            return 0;

        case TEXT_STATE_CHOICE:
            switch (Message_ShouldAdvance(globalCtx)) {
                case 0:
                    return 1;
                default:
                    if (thisx->textId == 0x400C) {
                        thisx->textId = (globalCtx->msgCtx.choiceIndex == 0) ? 0x400D : 0x400E;
                        Message_ContinueTextbox(globalCtx, thisx->textId);
                    }
                    break;
            }
            return 1;

        case TEXT_STATE_EVENT:
            switch (Message_ShouldAdvance(globalCtx)) {
                case 0:
                    return 1;
                default:
                    return 2;
            }
    }

    return 1;
}

void EnZo_Blink(EnZo* thisv) {
    if (DECR(thisv->blinkTimer) == 0) {
        thisv->eyeTexture++;
        if (thisv->eyeTexture >= 3) {
            thisv->blinkTimer = Rand_S16Offset(30, 30);
            thisv->eyeTexture = 0;
        }
    }
}

void EnZo_Dialog(EnZo* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_194.unk_18 = player->actor.world.pos;
    if (thisv->actionFunc == EnZo_Standing) {
        // Look down at link if young, look up if old
        thisv->unk_194.unk_14 = !LINK_IS_ADULT ? 10.0f : -10.0f;
    } else {
        thisv->unk_194.unk_18.y = thisv->actor.world.pos.y;
    }
    func_80034A14(&thisv->actor, &thisv->unk_194, 11, thisv->unk_64C);
    if (thisv->canSpeak == true) {
        func_800343CC(globalCtx, &thisv->actor, &thisv->unk_194.unk_00, thisv->dialogRadius, func_80B61024, func_80B61298);
    }
}

s32 EnZo_PlayerInProximity(EnZo* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f surfacePos;
    f32 yDist;
    f32 hDist;

    surfacePos.x = thisv->actor.world.pos.x;
    surfacePos.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
    surfacePos.z = thisv->actor.world.pos.z;

    hDist = Math_Vec3f_DistXZ(&surfacePos, &player->actor.world.pos);
    yDist = fabsf(player->actor.world.pos.y - surfacePos.y);

    if (hDist < 240.0f && yDist < 80.0f) {
        return 1;
    }
    return 0;
}

void EnZo_SetAnimation(EnZo* thisv) {
    s32 animId = ARRAY_COUNT(sAnimationInfo);

    if (thisv->skelAnime.animation == &gZoraHandsOnHipsTappingFootAnim ||
        thisv->skelAnime.animation == &gZoraOpenArmsAnim) {
        if (thisv->unk_194.unk_00 == 0) {
            if (thisv->actionFunc == EnZo_Standing) {
                animId = ENZO_ANIM_0;
            } else {
                animId = ENZO_ANIM_3;
            }
        }
    }

    if (thisv->unk_194.unk_00 != 0 && thisv->actor.textId == 0x4006 &&
        thisv->skelAnime.animation != &gZoraHandsOnHipsTappingFootAnim) {
        animId = ENZO_ANIM_6;
    }

    if (thisv->unk_194.unk_00 != 0 && thisv->actor.textId == 0x4007 && thisv->skelAnime.animation != &gZoraOpenArmsAnim) {
        animId = ENZO_ANIM_7;
    }

    if (animId != ARRAY_COUNT(sAnimationInfo)) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, animId);
        if (animId == ENZO_ANIM_3) {
            thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
            thisv->skelAnime.playSpeed = 0.0f;
        }
    }
}

void EnZo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnZo* thisv = (EnZo*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gZoraSkel, NULL, thisv->jointTable, thisv->morphTable, 20);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, NULL, &sColChkInit);

    if (LINK_IS_ADULT && ((thisv->actor.params & 0x3F) == 8)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENZO_ANIM_2);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.targetMode = 6;
    thisv->dialogRadius = thisv->collider.dim.radius + 30.0f;
    thisv->unk_64C = 1;
    thisv->canSpeak = false;
    thisv->unk_194.unk_00 = 0;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, thisv->collider.dim.height * 0.5f, thisv->collider.dim.radius, 0.0f,
                            5);

    if (thisv->actor.yDistToWater < 54.0f || (thisv->actor.params & 0x3F) == 8) {
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        thisv->actor.shape.shadowScale = 24.0f;
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENZO_ANIM_1);
        thisv->canSpeak = true;
        thisv->alpha = 255.0f;
        thisv->actionFunc = EnZo_Standing;
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->actionFunc = EnZo_Submerged;
    }
}

void EnZo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnZo_Standing(EnZo* thisv, GlobalContext* globalCtx) {
    s16 angle;

    func_80034F54(globalCtx, thisv->unk_656, thisv->unk_67E, 20);
    EnZo_SetAnimation(thisv);
    if (thisv->unk_194.unk_00 != 0) {
        thisv->unk_64C = 4;
        return;
    }

    angle = ABS((s16)((f32)thisv->actor.yawTowardsPlayer - (f32)thisv->actor.shape.rot.y));
    if (angle < 0x4718) {
        if (EnZo_PlayerInProximity(thisv, globalCtx)) {
            thisv->unk_64C = 2;
        } else {
            thisv->unk_64C = 1;
        }
    } else {
        thisv->unk_64C = 1;
    }
}

void EnZo_Submerged(EnZo* thisv, GlobalContext* globalCtx) {
    if (EnZo_PlayerInProximity(thisv, globalCtx)) {
        thisv->actionFunc = EnZo_Surface;
        thisv->actor.velocity.y = 4.0f;
    }
}

void EnZo_Surface(EnZo* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.yDistToWater < 54.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_OUT_OF_WATER);
        EnZo_SpawnSplashes(thisv);
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENZO_ANIM_3);
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actionFunc = EnZo_TreadWater;
        thisv->actor.velocity.y = 0.0f;
        thisv->alpha = 255.0f;
    } else if (thisv->actor.yDistToWater < 80.0f) {
        Math_ApproachF(&thisv->actor.velocity.y, 2.0f, 0.4f, 0.6f);
        Math_ApproachF(&thisv->alpha, 255.0f, 0.3f, 10.0f);
    }
}

void EnZo_TreadWater(EnZo* thisv, GlobalContext* globalCtx) {
    func_80034F54(globalCtx, thisv->unk_656, thisv->unk_67E, 20);
    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        thisv->canSpeak = true;
        thisv->unk_64C = 4;
        thisv->skelAnime.playSpeed = 0.0f;
    }
    EnZo_SetAnimation(thisv);

    Math_ApproachF(&thisv->actor.velocity.y, thisv->actor.yDistToWater < 54.0f ? -0.6f : 0.6f, 0.3f, 0.2f);
    if (thisv->rippleTimer != 0) {
        thisv->rippleTimer--;
        if ((thisv->rippleTimer == 3) || (thisv->rippleTimer == 6)) {
            EnZo_TreadWaterRipples(thisv, 0.2f, 1.0f, 200);
        }
    } else {
        EnZo_TreadWaterRipples(thisv, 0.2f, 1.0f, 200);
        thisv->rippleTimer = 12;
    }

    if (EnZo_PlayerInProximity(thisv, globalCtx) != 0) {
        thisv->timeToDive = Rand_S16Offset(40, 40);
    } else if (DECR(thisv->timeToDive) == 0) {
        f32 startFrame;
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENZO_ANIM_4);
        thisv->canSpeak = false;
        thisv->unk_64C = 1;
        thisv->actionFunc = EnZo_Dive;
        startFrame = thisv->skelAnime.startFrame;
        thisv->skelAnime.startFrame = thisv->skelAnime.endFrame;
        thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
        thisv->skelAnime.endFrame = startFrame;
        thisv->skelAnime.playSpeed = -1.0f;
    }
}

void EnZo_Dive(EnZo* thisv, GlobalContext* globalCtx) {
    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_DIVE_WATER);
        EnZo_SpawnSplashes(thisv);
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        thisv->actor.velocity.y = -4.0f;
        thisv->skelAnime.playSpeed = 0.0f;
    }

    if (thisv->skelAnime.playSpeed > 0.0f) {
        return;
    }

    if (thisv->actor.yDistToWater > 80.0f || thisv->actor.bgCheckFlags & 1) {
        Math_ApproachF(&thisv->actor.velocity.y, -1.0f, 0.4f, 0.6f);
        Math_ApproachF(&thisv->alpha, 0.0f, 0.3f, 10.0f);
    }

    if ((s16)thisv->alpha == 0) {
        Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENZO_ANIM_2);
        thisv->actor.world.pos = thisv->actor.home.pos;
        thisv->alpha = 0.0f;
        thisv->actionFunc = EnZo_Submerged;
    }
}

void EnZo_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnZo* thisv = (EnZo*)thisx;
    u32 pad;
    Vec3f pos;

    if ((s32)thisv->alpha != 0) {
        SkelAnime_Update(&thisv->skelAnime);
        EnZo_Blink(thisv);
    }

    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(globalCtx, thisx, thisv->collider.dim.radius, thisv->collider.dim.height * 0.25f, 0.0f, 5);
    thisv->actionFunc(thisv, globalCtx);
    EnZo_Dialog(thisv, globalCtx);

    // Spawn air bubbles
    if (globalCtx->state.frames & 8) {
        pos = thisv->actor.world.pos;

        pos.y += (Rand_ZeroOne() - 0.5f) * 10.0f + 18.0f;
        pos.x += (Rand_ZeroOne() - 0.5f) * 28.0f;
        pos.z += (Rand_ZeroOne() - 0.5f) * 28.0f;
        EnZo_Bubble(thisv, &pos);
    }

    if ((s32)thisv->alpha != 0) {
        Collider_UpdateCylinder(thisx, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    EnZo_UpdateRipples(thisv);
    EnZo_UpdateBubbles(thisv);
    EnZo_UpdateSplashes(thisv);
}

s32 EnZo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnZo* thisv = (EnZo*)thisx;
    Vec3s vec;

    if (limbIndex == 15) {
        Matrix_Translate(1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = thisv->unk_194.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_Translate(-1800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 8) {
        vec = thisv->unk_194.unk_0E;
        Matrix_RotateX((-vec.y / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * std::numbers::pi_v<float>, MTXMODE_APPLY);
    }

    if ((limbIndex == 8) || (limbIndex == 9) || (limbIndex == 12)) {
        rot->y += (Math_SinS(thisv->unk_656[limbIndex]) * 200.0f);
        rot->z += (Math_CosS(thisv->unk_67E[limbIndex]) * 200.0f);
    }

    return 0;
}

void EnZo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZo* thisv = (EnZo*)thisx;
    Vec3f vec = { 0.0f, 600.0f, 0.0f };

    if (limbIndex == 15) {
        Matrix_MultVec3f(&vec, &thisv->actor.focus.pos);
    }
}

void EnZo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnZo* thisv = (EnZo*)thisx;
    const void* eyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };

    Matrix_Push();
    EnZo_DrawRipples(thisv, globalCtx);
    EnZo_DrawBubbles(thisv, globalCtx);
    EnZo_DrawSplashes(thisv, globalCtx);
    Matrix_Pop();

    if ((s32)thisv->alpha != 0) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_zo.c", 1008);

        if (thisv->alpha == 255.0f) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexture]));
            func_80034BA0(globalCtx, &thisv->skelAnime, EnZo_OverrideLimbDraw, EnZo_PostLimbDraw, thisx, thisv->alpha);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeTexture]));
            func_80034CC4(globalCtx, &thisv->skelAnime, EnZo_OverrideLimbDraw, EnZo_PostLimbDraw, thisx, thisv->alpha);
        }

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_zo.c", 1025);
    }
}
