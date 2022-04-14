/*
 * File: z_arrow_fire.c
 * Overlay: ovl_Arrow_Fire
 * Description: Fire Arrow. Spawned as a child of a normal arrow.
 */

#include "z_arrow_fire.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void ArrowFire_Init(Actor* thisvx, GlobalContext* globalCtx);
void ArrowFire_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void ArrowFire_Update(Actor* thisvx, GlobalContext* globalCtx);
void ArrowFire_Draw(Actor* thisvx, GlobalContext* globalCtx);

void ArrowFire_Charge(ArrowFire* thisv, GlobalContext* globalCtx);
void ArrowFire_Fly(ArrowFire* thisv, GlobalContext* globalCtx);
void ArrowFire_Hit(ArrowFire* thisv, GlobalContext* globalCtx);

#include "overlays/ovl_Arrow_Fire/ovl_Arrow_Fire.h"

const ActorInit Arrow_Fire_InitVars = {
    ACTOR_ARROW_FIRE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ArrowFire),
    (ActorFunc)ArrowFire_Init,
    (ActorFunc)ArrowFire_Destroy,
    (ActorFunc)ArrowFire_Update,
    (ActorFunc)ArrowFire_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_STOP),
};

void ArrowFire_SetupAction(ArrowFire* thisv, ArrowFireActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void ArrowFire_Init(Actor* thisvx, GlobalContext* globalCtx) {
    ArrowFire* thisv = (ArrowFire*)thisvx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    thisv->radius = 0;
    thisv->unk_158 = 1.0f;
    ArrowFire_SetupAction(thisv, ArrowFire_Charge);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->alpha = 160;
    thisv->timer = 0;
    thisv->unk_15C = 0.0f;
}

void ArrowFire_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    func_800876C8(globalCtx);
    LOG_STRING("消滅", "../z_arrow_fire.c", 421); // "Disappearance"
}

void ArrowFire_Charge(ArrowFire* thisv, GlobalContext* globalCtx) {
    EnArrow* arrow;

    arrow = (EnArrow*)thisv->actor.parent;
    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->radius < 10) {
        thisv->radius += 1;
    }
    // copy position and rotation from arrow
    thisv->actor.world.pos = arrow->actor.world.pos;
    thisv->actor.shape.rot = arrow->actor.shape.rot;

    func_8002F974(&thisv->actor, NA_SE_PL_ARROW_CHARGE_FIRE - SFX_FLAG);

    // if arrow has no parent, player has fired the arrow
    if (arrow->actor.parent == NULL) {
        thisv->unkPos = thisv->actor.world.pos;
        thisv->radius = 10;
        ArrowFire_SetupAction(thisv, ArrowFire_Fly);
        thisv->alpha = 255;
    }
}

void func_80865ECC(Vec3f* unkPos, Vec3f* firePos, f32 scale) {
    unkPos->x += ((firePos->x - unkPos->x) * scale);
    unkPos->y += ((firePos->y - unkPos->y) * scale);
    unkPos->z += ((firePos->z - unkPos->z) * scale);
}

void ArrowFire_Hit(ArrowFire* thisv, GlobalContext* globalCtx) {
    f32 scale;
    f32 offset;
    u16 timer;

    if (thisv->actor.projectedW < 50.0f) {
        scale = 10.0f;
    } else {
        if (950.0f < thisv->actor.projectedW) {
            scale = 310.0f;
        } else {
            scale = thisv->actor.projectedW;
            scale = ((scale - 50.0f) * (1.0f / 3.0f)) + 10.0f;
        }
    }

    timer = thisv->timer;
    if (timer != 0) {
        thisv->timer -= 1;

        if (thisv->timer >= 8) {
            offset = ((thisv->timer - 8) * (1.0f / 24.0f));
            offset = SQ(offset);
            thisv->radius = (((1.0f - offset) * scale) + 10.0f);
            thisv->unk_158 += ((2.0f - thisv->unk_158) * 0.1f);
            if (thisv->timer < 16) {
                if (1) {}
                thisv->alpha = ((thisv->timer * 0x23) - 0x118);
            }
        }
    }

    if (thisv->timer >= 9) {
        if (thisv->unk_15C < 1.0f) {
            thisv->unk_15C += 0.25f;
        }
    } else {
        if (thisv->unk_15C > 0.0f) {
            thisv->unk_15C -= 0.125f;
        }
    }

    if (thisv->timer < 8) {
        thisv->alpha = 0;
    }

    if (thisv->timer == 0) {
        thisv->timer = 255;
        Actor_Kill(&thisv->actor);
    }
}

void ArrowFire_Fly(ArrowFire* thisv, GlobalContext* globalCtx) {
    EnArrow* arrow;
    f32 distanceScaled;
    s32 pad;

    arrow = (EnArrow*)thisv->actor.parent;
    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    // copy position and rotation from arrow
    thisv->actor.world.pos = arrow->actor.world.pos;
    thisv->actor.shape.rot = arrow->actor.shape.rot;
    distanceScaled = Math_Vec3f_DistXYZ(&thisv->unkPos, &thisv->actor.world.pos) * (1.0f / 24.0f);
    thisv->unk_158 = distanceScaled;
    if (distanceScaled < 1.0f) {
        thisv->unk_158 = 1.0f;
    }
    func_80865ECC(&thisv->unkPos, &thisv->actor.world.pos, 0.05f);

    if (arrow->hitFlags & 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_EXPLOSION_FRAME);
        ArrowFire_SetupAction(thisv, ArrowFire_Hit);
        thisv->timer = 32;
        thisv->alpha = 255;
    } else if (arrow->timer < 34) {
        if (thisv->alpha < 35) {
            Actor_Kill(&thisv->actor);
        } else {
            thisv->alpha -= 0x19;
        }
    }
}

void ArrowFire_Update(Actor* thisvx, GlobalContext* globalCtx) {
    ArrowFire* thisv = (ArrowFire*)thisvx;

    if (globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK ||
        globalCtx->msgCtx.msgMode == MSGMODE_SONG_PLAYED) {
        Actor_Kill(&thisv->actor);
    } else {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void ArrowFire_Draw(Actor* thisvx, GlobalContext* globalCtx2) {
    ArrowFire* thisv = (ArrowFire*)thisvx;
    GlobalContext* globalCtx = globalCtx2;
    u32 stateFrames;
    EnArrow* arrow;
    Actor* tranform;

    stateFrames = globalCtx->state.frames;
    arrow = (EnArrow*)thisv->actor.parent;
    if (1) {}

    if ((arrow != NULL) && (arrow->actor.update != NULL) && (thisv->timer < 255)) {
        if (1) {}
        tranform = (arrow->hitFlags & 2) ? &thisv->actor : &arrow->actor;

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_arrow_fire.c", 618);

        Matrix_Translate(tranform->world.pos.x, tranform->world.pos.y, tranform->world.pos.z, MTXMODE_NEW);
        Matrix_RotateY(tranform->shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_RotateX(tranform->shape.rot.x * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_RotateZ(tranform->shape.rot.z * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

        // Draw red effect over the screen when arrow hits
        if (thisv->unk_15C > 0) {
            POLY_XLU_DISP = func_800937C0(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (s32)(40.0f * thisv->unk_15C) & 0xFF, 0, 0,
                            (s32)(150.0f * thisv->unk_15C) & 0xFF);
            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        // Draw fire on the arrow
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, thisv->alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
        Matrix_RotateZYX(0x4000, 0x0, 0x0, MTXMODE_APPLY);
        if (thisv->timer != 0) {
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, 1500.0f, 0.0f, MTXMODE_APPLY);
        }
        Matrix_Scale(thisv->radius * 0.2f, thisv->unk_158 * 4.0f, thisv->radius * 0.2f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -700.0f, 0.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_arrow_fire.c", 666),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 255 - (stateFrames * 2) % 256, 0, 64, 32, 1,
                                        255 - stateFrames % 256, 511 - (stateFrames * 10) % 512, 64, 64));
        gSPDisplayList(POLY_XLU_DISP++, sModelDL);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_arrow_fire.c", 682);
    }
}
