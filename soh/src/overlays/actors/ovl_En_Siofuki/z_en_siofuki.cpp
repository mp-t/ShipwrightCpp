/*
 * File: z_en_siofuki.c
 * Overlay: ovl_En_Siofuki
 * Description: Water Spout
 */

#include "z_en_siofuki.h"
#include "objects/object_siofuki/object_siofuki.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnSiofuki_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSiofuki_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSiofuki_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSiofuki_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80AFC34C(EnSiofuki* thisv, GlobalContext* globalCtx);
void func_80AFC544(EnSiofuki* thisv, GlobalContext* globalCtx);
void func_80AFC478(EnSiofuki* thisv, GlobalContext* globalCtx);

const ActorInit En_Siofuki_InitVars = {
    ACTOR_EN_SIOFUKI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SIOFUKI,
    sizeof(EnSiofuki),
    (ActorFunc)EnSiofuki_Init,
    (ActorFunc)EnSiofuki_Destroy,
    (ActorFunc)EnSiofuki_Update,
    (ActorFunc)EnSiofuki_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void EnSiofuki_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSiofuki* thisv = (EnSiofuki*)thisx;
    s32 type;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    if ((thisx->room == 10) && Flags_GetSwitch(globalCtx, 0x1E)) {
        Actor_Kill(thisx);
        return;
    }

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&object_siofuki_Col_000D78, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);
    thisv->sfxFlags |= 1;

    type = ((u16)thisx->params >> 0xC) & 0xF;
    if (!((type == 0) || (type == 1))) {
        Actor_Kill(thisx);
        return;
    }

    thisv->initPosY = thisx->world.pos.y;
    thisv->unk_174 = 35.0f;
    thisv->unk_170 = -6058.0f + thisv->unk_174;

    if (thisx->shape.rot.x != 0) {
        thisv->maxHeight = thisx->shape.rot.x * 40.0f;
        thisv->currentHeight = thisv->maxHeight;
    }
    thisv->activeTime = 0;
    if (thisx->shape.rot.y != 0) {
        thisv->activeTime = thisx->shape.rot.y;
    }
    if (thisx->shape.rot.z != 0) {
        thisx->scale.x = thisx->shape.rot.z * (1.0f / 1.73f) * 0.1f;
        thisx->scale.z = thisx->shape.rot.z * 0.5f * 0.1f;
    }

    thisx->world.rot.x = 0;
    thisx->world.rot.y = 0;
    thisx->world.rot.z = 0;
    thisx->shape.rot.x = 0;
    thisx->shape.rot.y = 0;
    thisx->shape.rot.z = 0;

    type = ((u16)thisx->params >> 0xC) & 0xF;
    if (type == EN_SIOFUKI_RAISING) {
        thisv->currentHeight = 10.0f;
        thisv->targetHeight = 10.0f;
        thisv->actionFunc = func_80AFC34C;
    } else if (type == EN_SIOFUKI_LOWERING) {
        if (Flags_GetTreasure(globalCtx, (u16)thisx->params & 0x3F)) {
            thisv->currentHeight = -45.0f;
            thisv->targetHeight = -45.0f;
            thisv->actionFunc = func_80AFC544;
        } else {
            thisv->targetHeight = thisv->currentHeight;
            thisv->actionFunc = func_80AFC478;
        }
    }
}

void EnSiofuki_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSiofuki* thisv = (EnSiofuki*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void func_80AFBDC8(EnSiofuki* thisv, GlobalContext* globalCtx) {
    thisv->oscillation = sinf((globalCtx->gameplayFrames & 0x1F) / 32.0f * std::numbers::pi_v<float> * 2.0f) * 4.0f;
    thisv->unk_170 = thisv->unk_174 * 10.0f + -6058.0f - thisv->oscillation * 10.0f;
    thisv->unk_174 = 35.0f;
    thisv->dyna.actor.world.pos.y = thisv->initPosY + thisv->currentHeight + thisv->oscillation;
}

void func_80AFBE8C(EnSiofuki* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 dX;
    f32 dY;
    f32 dZ;
    s16 angle;
    s16 dAngle;
    f32 dist2d;
    f32 speedScale;

    dX = player->actor.world.pos.x - thisv->dyna.actor.world.pos.x;
    dY = player->actor.world.pos.y - thisv->dyna.actor.world.pos.y;
    dZ = player->actor.world.pos.z - thisv->dyna.actor.world.pos.z;

    if ((dX > (thisv->dyna.actor.scale.x * -346.0f)) && (dX < (thisv->dyna.actor.scale.x * 346.0f)) &&
        (dZ > (thisv->dyna.actor.scale.z * -400.0f)) && (dZ < (thisv->dyna.actor.scale.z * 400.0f)) && (dY < 0.0f)) {
        if (func_8004356C(&thisv->dyna)) {
            if (thisv->splashTimer <= 0) {
                EffectSsGSplash_Spawn(globalCtx, &player->actor.world.pos, NULL, NULL, 1, 1);
                thisv->splashTimer = 10;
            } else {
                thisv->splashTimer--;
            }

            thisv->applySpeed = false;
            thisv->appliedSpeed = 0.0f;
            thisv->targetAppliedSpeed = 0.0f;
        } else {
            dist2d = sqrtf(SQ(dX) + SQ(dZ));
            thisv->applySpeed = true;
            thisv->splashTimer = 0;
            angle = Math_FAtan2F(dX, dZ) * (0x8000 / std::numbers::pi_v<float>);
            dAngle = (player->actor.world.rot.y ^ 0x8000) - angle;
            player->actor.gravity = 0.0f;
            player->actor.velocity.y = 0.0f;
            Math_SmoothStepToF(&player->actor.world.pos.y, thisv->dyna.actor.world.pos.y, 0.5f, 4.0f, 1.0f);

            if ((dAngle < 0x4000) && (dAngle > -0x4000)) {
                thisv->appliedYaw = player->actor.world.rot.y ^ 0x8000;
                speedScale = dist2d / (thisv->dyna.actor.scale.x * 40.0f * 10.0f);
                speedScale = CLAMP_MIN(speedScale, 0.0f);
                speedScale = CLAMP_MAX(speedScale, 1.0f);
                player->linearVelocity *= speedScale;
                Math_ApproachF(&thisv->targetAppliedSpeed, 3.0f, 1.0f, 1.0f);
                Math_ApproachF(&thisv->appliedSpeed, thisv->targetAppliedSpeed, 1.0f, 0.3f * speedScale);
            } else {
                thisv->appliedYaw = player->actor.world.rot.y;
                player->linearVelocity /= 2.0f;
                Math_ApproachF(&thisv->targetAppliedSpeed, 3.0f, 1.0f, 1.0f);
                Math_ApproachF(&thisv->appliedSpeed, thisv->targetAppliedSpeed, 1.0f, 0.1f);
            }

            player->windDirection = thisv->appliedYaw;
            player->windSpeed = thisv->appliedSpeed;
        }
    } else {
        if (thisv->applySpeed) {
            player->linearVelocity = thisv->appliedSpeed + player->linearVelocity;
            player->currentYaw = thisv->appliedYaw;
        }

        thisv->targetAppliedSpeed = 0.0f;
        thisv->appliedSpeed = 0.0f;
        thisv->applySpeed = false;
    }
}

void func_80AFC1D0(EnSiofuki* thisv, GlobalContext* globalCtx) {
    Math_SmoothStepToF(&thisv->currentHeight, thisv->targetHeight, 0.8f, 3.0f, 0.01f);
}

void func_80AFC218(EnSiofuki* thisv, GlobalContext* globalCtx) {
    func_80AFBDC8(thisv, globalCtx);
    func_80AFBE8C(thisv, globalCtx);
    func_80AFC1D0(thisv, globalCtx);

    thisv->timer--;
    if (thisv->timer < 0) {
        Flags_UnsetSwitch(globalCtx, ((u16)thisv->dyna.actor.params >> 6) & 0x3F);
        switch (((u16)thisv->dyna.actor.params >> 0xC) & 0xF) {
            case EN_SIOFUKI_RAISING:
                thisv->targetHeight = 10.0f;
                thisv->actionFunc = func_80AFC34C;
                break;
            case EN_SIOFUKI_LOWERING:
                thisv->targetHeight = thisv->maxHeight;
                thisv->actionFunc = func_80AFC478;
                break;
        }
    } else {
        func_8002F994(&thisv->dyna.actor, thisv->timer);
    }

    if (((((u16)thisv->dyna.actor.params >> 0xC) & 0xF) == EN_SIOFUKI_LOWERING) &&
        Flags_GetTreasure(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
        thisv->currentHeight = -45.0f;
        thisv->targetHeight = -45.0f;
        Flags_UnsetSwitch(globalCtx, ((u16)thisv->dyna.actor.params >> 6) & 0x3F);
        thisv->actionFunc = func_80AFC544;
    }
}

void func_80AFC34C(EnSiofuki* thisv, GlobalContext* globalCtx) {
    func_80AFBDC8(thisv, globalCtx);
    func_80AFBE8C(thisv, globalCtx);
    func_80AFC1D0(thisv, globalCtx);

    if (Flags_GetSwitch(globalCtx, ((u16)thisv->dyna.actor.params >> 6) & 0x3F)) {
        thisv->targetHeight = 400.0f;
        thisv->timer = 300;
        thisv->actionFunc = func_80AFC218;
    }
}

void func_80AFC3C8(EnSiofuki* thisv, GlobalContext* globalCtx) {
    func_80AFBDC8(thisv, globalCtx);
    func_80AFBE8C(thisv, globalCtx);
    func_80AFC1D0(thisv, globalCtx);

    thisv->timer--;
    if (thisv->timer < 0) {
        thisv->timer = thisv->activeTime * 20;
        thisv->targetHeight = -45.0f;
        thisv->actionFunc = func_80AFC218;
    }

    if (Flags_GetTreasure(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
        thisv->currentHeight = -45.0f;
        thisv->targetHeight = -45.0f;
        thisv->actionFunc = func_80AFC544;
    }
}

void func_80AFC478(EnSiofuki* thisv, GlobalContext* globalCtx) {
    func_80AFBDC8(thisv, globalCtx);
    func_80AFBE8C(thisv, globalCtx);
    func_80AFC1D0(thisv, globalCtx);

    if (((u16)thisv->dyna.actor.params >> 0xC & 0xF) == EN_SIOFUKI_LOWERING) {
        if (Flags_GetSwitch(globalCtx, ((u16)thisv->dyna.actor.params >> 6) & 0x3F)) {
            thisv->timer = 20;
            thisv->actionFunc = func_80AFC3C8;
            OnePointCutscene_Init(globalCtx, 5010, 40, &thisv->dyna.actor, MAIN_CAM);
        }

        if (Flags_GetTreasure(globalCtx, (u16)thisv->dyna.actor.params & 0x3F)) {
            thisv->currentHeight = -45.0f;
            thisv->targetHeight = -45.0f;
            thisv->actionFunc = func_80AFC544;
        }
    }
}

void func_80AFC544(EnSiofuki* thisv, GlobalContext* globalCtx) {
    func_80AFBDC8(thisv, globalCtx);
    func_80AFC1D0(thisv, globalCtx);
}

void EnSiofuki_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSiofuki* thisv = (EnSiofuki*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void EnSiofuki_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSiofuki* thisv = (EnSiofuki*)thisx;
    u32 x;
    u32 y;
    u32 gameplayFrames = globalCtx->gameplayFrames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_siofuki.c", 654);
    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_Translate(0.0f, thisv->unk_170, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_siofuki.c", 662),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    x = gameplayFrames * 15;
    y = gameplayFrames * -15;
    gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, x, y, 64, 64, 1, x, y, 64, 64));
    gSPDisplayList(POLY_XLU_DISP++, object_siofuki_DL_000B70);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_siofuki.c", 674);

    if (thisv->sfxFlags & 1) {
        f32 heightRatio;
        switch (((u16)thisx->params >> 0xC) & 0xF) {
            case EN_SIOFUKI_RAISING:
                heightRatio = (thisv->currentHeight - 10.0f) / (400.0f - 10.0f);
                func_800F436C(&thisx->projectedPos, NA_SE_EV_FOUNTAIN - SFX_FLAG, 1.0f + heightRatio);
                break;
            case EN_SIOFUKI_LOWERING:
                if (thisv->currentHeight > -35.0f) {
                    heightRatio = (thisv->currentHeight - -35.0f) / (thisv->maxHeight - -35.0f);
                    func_800F436C(&thisx->projectedPos, NA_SE_EV_FOUNTAIN - SFX_FLAG, 1.0f + heightRatio);
                }
                break;
        }
    }
}
