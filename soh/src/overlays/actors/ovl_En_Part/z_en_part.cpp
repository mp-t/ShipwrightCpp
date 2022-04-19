/*
 * File: z_en_part.c
 * Overlay: ovl_En_Part
 * Description: Effect spawner for enemies' death
 */

#include "z_en_part.h"
#include "objects/object_tite/object_tite.h"
#include "objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_4

void EnPart_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPart_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPart_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPart_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Part_InitVars = {
    ACTOR_EN_PART,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnPart),
    (ActorFunc)EnPart_Init,
    (ActorFunc)EnPart_Destroy,
    (ActorFunc)EnPart_Update,
    (ActorFunc)EnPart_Draw,
    NULL,
};

void EnPart_Init(Actor* thisx, GlobalContext* globalCtx) {
}

void EnPart_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void func_80ACDDE8(EnPart* thisv, GlobalContext* globalCtx) {
    f32 sign = 1.0f;

    thisv->action = 1;
    thisv->actor.world.rot.y = Rand_ZeroOne() * 20000.0f;

    switch (thisv->actor.params) {
        case 0:
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.gravity = -0.3f - Rand_ZeroOne() * 0.5f;
            thisv->rotZSpeed = 0.3f;
            thisv->timer = 25;
            thisv->actor.speedXZ = (Rand_ZeroOne() - 0.5f) * 2.0f;
            break;
        case 13:
            thisv->timer = 400;
        case 12:
            thisv->actor.speedXZ = Rand_CenteredFloat(6.0f);
            thisv->actor.home.pos = thisv->actor.world.pos;
            thisv->timer += 60;
            thisv->actor.velocity.y = Rand_ZeroOne() * 5.0f + 4.0f;
            thisv->actor.gravity = -0.6f - Rand_ZeroOne() * 0.5f;
            thisv->rotZSpeed = 0.15f;
            break;
        case 14:
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40, 0x8001, 0, -1);
        case 1:
        case 4:
        case 9:
        case 10:
            thisv->timer += (s16)(Rand_ZeroOne() * 17.0f) + 5;
        case 2:
            thisv->actor.velocity.y = Rand_ZeroOne() * 5.0f + 4.0f;
            thisv->actor.gravity = -0.6f - Rand_ZeroOne() * 0.5f;
            thisv->rotZSpeed = 0.15f;
            break;
        case 11:
            EffectSsEnFire_SpawnVec3f(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40, 0x8001, 0, -1);
        case 3:
            thisv->actor.speedXZ = (Rand_ZeroOne() - 0.5f) * 3.0f;
            thisv->timer = (s16)(Rand_ZeroOne() * 17.0f) + 10;
            thisv->actor.velocity.y = Rand_ZeroOne() * 3.0f + 8.0f;
            thisv->actor.gravity = -0.6f - Rand_ZeroOne() * 0.3f;
            thisv->rotZSpeed = 0.15f;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            thisv->actor.world.rot.y = thisv->actor.parent->shape.rot.y;
            if (thisv->displayList == object_ik_DL_015380) {
                sign = -1.0f;
            }
            thisv->actor.velocity.y = 0.0f;
            thisv->actor.speedXZ = 6.0f * sign;
            thisv->actor.gravity = -1.2f;
            thisv->rotZSpeed = 0.15f * sign;
            ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
            thisv->timer = 18;
            break;
    }
}

void func_80ACE13C(EnPart* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((thisv->actor.params == 12) || (thisv->actor.params == 13)) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 15.0f, 0.0f, 0x1D);

        if ((thisv->actor.bgCheckFlags & 1) || (thisv->actor.world.pos.y <= thisv->actor.floorHeight)) {
            thisv->action = 4;
            thisv->actor.speedXZ = 0.0f;
            thisv->actor.gravity = 0.0f;
            thisv->actor.velocity.y = 0.0f;
        }

        if ((thisv->actor.params == 13) && (thisv->actor.parent != NULL) && (thisv->actor.parent->update == NULL)) {
            thisv->actor.parent = NULL;
        }
    } else if (thisv->timer <= 0) {
        switch (thisv->actor.params) {
            case 1:
            case 9:
            case 10:
            case 14:
                EffectSsDeadDb_Spawn(globalCtx, &thisv->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(thisv->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 255, 0, 1, 9,
                                     true);
                break;
            case 3:
            case 11:
                EffectSsDeadDb_Spawn(globalCtx, &thisv->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(thisv->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                                     true);
                break;
            case 4:
                for (i = 7; i >= 0; i--) {
                    pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(60.0f);
                    pos.y = thisv->actor.world.pos.y + thisv->actor.shape.yOffset * thisv->actor.scale.y +
                            Rand_CenteredFloat(50.0f);
                    pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(60.0f);
                    velocity.y = Rand_ZeroOne() + 1.0f;
                    EffectSsDtBubble_SpawnColorProfile(globalCtx, &pos, &velocity, &accel, Rand_S16Offset(80, 100), 25,
                                                       0, true);
                }
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                for (i = 4; i >= 0; i--) {
                    pos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(25.0f);
                    pos.y = thisv->actor.world.pos.y + Rand_CenteredFloat(40.0f);
                    pos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(25.0f);
                    EffectSsDeadDb_Spawn(globalCtx, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 0, 0, 255, 1,
                                         9, true);
                }
                break;
        }

        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->timer--;
    thisv->rotZ += thisv->rotZSpeed;
}

void func_80ACE5B8(EnPart* thisv, GlobalContext* globalCtx) {
    thisv->action = 3;
}

void func_80ACE5C8(EnPart* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->timer--;
    if (thisv->timer == 0) {
        Actor_Kill(&thisv->actor);
    } else {
        Vec3f velocity = { 0.0f, 8.0f, 0.0f };
        Vec3f accel = { 0.0f, -1.5f, 0.0f };

        if (sqrt(thisv->actor.xyzDistToPlayerSq) <= 40.0f) {
            u8 prevInvincibilityTimer = player->invincibilityTimer;

            if (player->invincibilityTimer <= 0) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    globalCtx->damagePlayer(globalCtx, -8);
                }
            }
            func_8002F71C(globalCtx, thisv->actor.parent, (650.0f - thisv->actor.parent->xzDistToPlayer) * 0.04f + 4.0f,
                          thisv->actor.parent->world.rot.y, 8.0f);
            player->invincibilityTimer = prevInvincibilityTimer;
            thisv->timer = 1;
        }

        func_80033480(globalCtx, &thisv->actor.world.pos, 0.0f, 1, 300, 150, 1);
        velocity.x = Rand_CenteredFloat(16.0f);
        EffectSsHahen_Spawn(globalCtx, &thisv->actor.world.pos, &velocity, &accel, 20,
                            (s32)((Rand_ZeroOne() * 5.0f + 12.0f) * 2), -1, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MONBLIN_GNDWAVE - SFX_FLAG);
    }
}

void func_80ACE7E8(EnPart* thisv, GlobalContext* globalCtx) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((thisv->actor.parent == NULL) || (thisv->actor.parent->update == NULL)) {
        EffectSsDeadDb_Spawn(globalCtx, &thisv->actor.world.pos, &zeroVec, &zeroVec,
                             (s16)(thisv->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 255, 0, 1, 9, true);
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->timer == 0) {
        f32 diffsSum = Math_SmoothStepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x, 1.0f, 5.0f, 0.0f);

        diffsSum += Math_SmoothStepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 1.0f, 5.0f, 0.0f);
        diffsSum += Math_SmoothStepToF(&thisv->actor.world.pos.z, thisv->actor.home.pos.z, 1.0f, 5.0f, 0.0f);
        diffsSum += Math_SmoothStepToF(&thisv->rotZ, 0.0f, 1.0f, 0.25f, 0.0f);
        if (diffsSum == 0.0f) {
            thisv->actor.parent->home.rot.x--;
            thisv->timer--;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_DAMAGE);
        }
    } else if (thisv->timer > 0) {
        thisv->timer--;
    }

    if (thisv->actor.parent->colChkInfo.health != 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnPart_Update(Actor* thisx, GlobalContext* globalCtx) {
    static EnPartActionFunc sActionFuncs[] = {
        func_80ACDDE8, func_80ACE13C, func_80ACE5B8, func_80ACE5C8, func_80ACE7E8,
    };

    EnPart* thisv = (EnPart*)thisx;

    Actor_MoveForward(&thisv->actor);

    if ((thisv->actor.params > 4 && thisv->actor.params < 9) || thisv->actor.params < 0) {
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 5.0f, 15.0f, 0.0f, 5);
        if (thisv->actor.params >= 0) {
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
            if (thisx->bgCheckFlags & 1) {
                thisx->bgCheckFlags &= ~1;
                thisx->velocity.y = 6.0f;
            }
        }
    }

    sActionFuncs[thisv->action](thisv, globalCtx);
}

Gfx* func_80ACEAC0(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* dList;
    Gfx* dListHead;

    dList = static_cast<Gfx*>(Graph_Alloc(gfxCtx, 4 * sizeof(Gfx)));
    dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetPrimColor(dListHead++, 0, 0, primR, primG, primB, 255);
    gDPSetEnvColor(dListHead++, envR, envG, envB, 255);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnPart_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnPart* thisv = (EnPart*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_part.c", 647);

    if (thisx->params > 0) {
        Matrix_RotateZ(thisv->rotZ, MTXMODE_APPLY);
    }

    func_80093D18(globalCtx->state.gfxCtx);
    func_8002EBCC(thisx, globalCtx, 0);

    if (thisx->params == 5) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(globalCtx->state.gfxCtx, 245, 255, 205, 30, 35, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(globalCtx->state.gfxCtx, 185, 135, 25, 20, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(globalCtx->state.gfxCtx, 255, 255, 255, 30, 40, 20));
    } else if (thisx->params == 6) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(globalCtx->state.gfxCtx, 55, 65, 55, 0, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(globalCtx->state.gfxCtx, 205, 165, 75, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(globalCtx->state.gfxCtx, 205, 165, 75, 25, 20, 0));
    } else if (thisx->params == 7) {
        gSPSegment(POLY_OPA_DISP++, 0x08, func_80ACEAC0(globalCtx->state.gfxCtx, 255, 255, 255, 180, 180, 180));
        gSPSegment(POLY_OPA_DISP++, 0x09, func_80ACEAC0(globalCtx->state.gfxCtx, 225, 205, 115, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, func_80ACEAC0(globalCtx->state.gfxCtx, 225, 205, 115, 25, 20, 0));
    } else if ((thisx->params == 9) && (thisv->displayList == ResourceMgr_LoadGfxByName(reinterpret_cast<const char*>(object_tite_DL_002FF0)))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_001300)));
        gSPSegment(POLY_OPA_DISP++, 0x09, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_001700)));
        gSPSegment(POLY_OPA_DISP++, 0x0A, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_001900)));
    } else if ((thisx->params == 10) && (thisv->displayList == ResourceMgr_LoadGfxByName(reinterpret_cast<const char*>(object_tite_DL_002FF0)))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_001B00)));
        gSPSegment(POLY_OPA_DISP++, 0x09, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_001F00)));
        gSPSegment(POLY_OPA_DISP++, 0x0A, ResourceMgr_LoadTexByName(SEGMENTED_TO_VIRTUAL(object_tite_Tex_002100)));
    }

    if (thisv->displayList != NULL) {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_part.c", 696),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, thisv->displayList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_part.c", 700);
}
