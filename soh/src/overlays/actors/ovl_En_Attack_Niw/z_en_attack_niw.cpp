/*
 * File: z_en_attack_niw.c
 * Overlay: ovl_En_Attack_Niw
 * Description: Attacking Cucco, not solid
 */

#include "z_en_attack_niw.h"
#include "objects/object_niw/object_niw.h"
#include "overlays/actors/ovl_En_Niw/z_en_niw.h"

#define FLAGS ACTOR_FLAG_4

void EnAttackNiw_Init(Actor* thisx, GlobalContext* globalCtx);
void EnAttackNiw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnAttackNiw_Update(Actor* thisx, GlobalContext* globalCtx);
void EnAttackNiw_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809B5670(EnAttackNiw* thisv, GlobalContext* globalCtx);
void func_809B5C18(EnAttackNiw* thisv, GlobalContext* globalCtx);
void func_809B59B0(EnAttackNiw* thisv, GlobalContext* globalCtx);

const ActorInit En_Attack_Niw_InitVars = {
    ACTOR_EN_ATTACK_NIW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_NIW,
    sizeof(EnAttackNiw),
    (ActorFunc)EnAttackNiw_Init,
    (ActorFunc)EnAttackNiw_Destroy,
    (ActorFunc)EnAttackNiw_Update,
    (ActorFunc)EnAttackNiw_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void EnAttackNiw_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnAttackNiw* thisv = (EnAttackNiw*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gCuccoSkel, &gCuccoAnim, thisv->jointTable, thisv->morphTable, 16);
    if (thisv->actor.params < 0) {
        thisv->actor.params = 0;
    }
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.gravity = 0.0f;
    thisv->unk_298.x = Rand_CenteredFloat(100.0f);
    thisv->unk_298.y = Rand_CenteredFloat(10.0f);
    thisv->unk_298.z = Rand_CenteredFloat(100.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = (Rand_ZeroOne() - 0.5f) * 60000.0f;
    thisv->actionFunc = func_809B5670;
}

void EnAttackNiw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnAttackNiw* thisv = (EnAttackNiw*)thisx;
    EnNiw* cucco = (EnNiw*)thisv->actor.parent;

    if (thisv->actor.parent != NULL) {
        if ((cucco->actor.update != NULL) && (cucco->unk_296 > 0)) {
            cucco->unk_296--;
        }
    }
}

void func_809B5268(EnAttackNiw* thisv, GlobalContext* globalCtx, s16 arg2) {
    if (thisv->unk_254 == 0) {
        if (arg2 == 0) {
            thisv->unk_264 = 0.0f;
        } else {
            thisv->unk_264 = -10000.0f;
        }
        thisv->unk_28E++;
        thisv->unk_254 = 3;
        if ((thisv->unk_28E & 1) == 0) {
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

    if (thisv->unk_288 != thisv->unk_2C0) {
        Math_ApproachF(&thisv->unk_2C0, thisv->unk_288, 0.5f, 4000.0f);
    }
    if (thisv->unk_264 != thisv->unk_2BC) {
        Math_ApproachF(&thisv->unk_2BC, thisv->unk_264, 0.5f, 4000.0f);
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

s32 func_809B55EC(EnAttackNiw* thisv, GlobalContext* globalCtx) {
    s16 sp1E;
    s16 sp1C;

    Actor_SetFocus(&thisv->actor, thisv->unk_2E4);
    Actor_GetScreenPos(globalCtx, &thisv->actor, &sp1E, &sp1C);
    if ((thisv->actor.projectedPos.z < -20.0f) || (sp1E < 0) || (sp1E > SCREEN_WIDTH) || (sp1C < 0) ||
        (sp1C > SCREEN_HEIGHT)) {
        return 0;
    } else {
        return 1;
    }
}

void func_809B5670(EnAttackNiw* thisv, GlobalContext* globalCtx) {
    s16 sp4E;
    s16 sp4C;
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    Vec3f sp34;

    thisv->actor.speedXZ = 10.0f;

    tmpf1 = (thisv->unk_298.x + globalCtx->view.lookAt.x) - globalCtx->view.eye.x;
    tmpf2 = (thisv->unk_298.y + globalCtx->view.lookAt.y) - globalCtx->view.eye.y;
    tmpf3 = (thisv->unk_298.z + globalCtx->view.lookAt.z) - globalCtx->view.eye.z;

    sp34.x = globalCtx->view.lookAt.x + tmpf1;
    sp34.y = globalCtx->view.lookAt.y + tmpf2;
    sp34.z = globalCtx->view.lookAt.z + tmpf3;

    thisv->unk_2D4 = Math_Vec3f_Yaw(&thisv->actor.world.pos, &sp34);
    thisv->unk_2D0 = Math_Vec3f_Pitch(&thisv->actor.world.pos, &sp34) * -1.0f;

    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_2D4, 5, thisv->unk_2DC, 0);
    Math_SmoothStepToS(&thisv->actor.world.rot.x, thisv->unk_2D0, 5, thisv->unk_2DC, 0);
    Math_ApproachF(&thisv->unk_2DC, 5000.0f, 1.0f, 100.0f);

    Actor_SetFocus(&thisv->actor, thisv->unk_2E4);
    Actor_GetScreenPos(globalCtx, &thisv->actor, &sp4E, &sp4C);

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->unk_2D4 = thisv->actor.yawTowardsPlayer;
        thisv->unk_2D0 = thisv->actor.world.rot.x - 3000.0f;
        thisv->unk_2DC = 0.0f;
        thisv->unk_284 = 0.0f;
        thisv->unk_27C = 0.0f;
        thisv->unk_254 = thisv->unk_256 = thisv->unk_258 = thisv->unk_25A = 0;
        thisv->unk_25C = 0x64;
        thisv->actor.gravity = -0.2f;
        thisv->unk_2E0 = 5.0f;
        thisv->unk_288 = 0.0f;
        thisv->actionFunc = func_809B59B0;
    } else if (((thisv->actor.projectedPos.z > 0.0f) && (fabsf(sp34.x - thisv->actor.world.pos.x) < 50.0f) &&
                (fabsf(sp34.y - thisv->actor.world.pos.y) < 50.0f) &&
                (fabsf(sp34.z - thisv->actor.world.pos.z) < 50.0f)) ||
               (thisv->actor.bgCheckFlags & 1)) {

        thisv->unk_2D4 = thisv->actor.yawTowardsPlayer;
        thisv->unk_2D0 = thisv->actor.world.rot.x - 2000.0f;
        thisv->unk_2DC = 0.0f;
        thisv->unk_27C = 0.0f;
        thisv->unk_284 = 0.0f;
        thisv->unk_254 = thisv->unk_256 = thisv->unk_258 = thisv->unk_25A = 0;
        thisv->actor.gravity = -0.2f;
        thisv->unk_2E0 = 5.0f;
        thisv->unk_288 = 0.0f;
        thisv->actionFunc = func_809B59B0;
    } else {
        thisv->unk_254 = 10;
        thisv->unk_264 = -10000.0f;
        thisv->unk_288 = -3000.0f;
        func_809B5268(thisv, globalCtx, 2);
    }
}

void func_809B59B0(EnAttackNiw* thisv, GlobalContext* globalCtx) {
    if (!func_809B55EC(thisv, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if (thisv->actor.bgCheckFlags & 1) {
        if (thisv->unk_25A == 0) {
            thisv->unk_25A = 3;
            thisv->actor.velocity.y = 3.5f;
        }
        if (thisv->actor.gravity != -2.0f) {
            thisv->unk_280 = thisv->unk_278 = 14000.0f;
            thisv->unk_2D0 = thisv->unk_26C = thisv->unk_268 = thisv->unk_284 = thisv->unk_27C = 0.0f;
            thisv->unk_2D4 = thisv->actor.yawTowardsPlayer;
            thisv->unk_262 = 0x32;
            thisv->unk_25C = 0x64;
            thisv->actor.gravity = -2.0f;
        }
    }
    if (thisv->unk_25C == 0x32) {
        thisv->unk_2D4 = Rand_CenteredFloat(200.0f) + thisv->actor.yawTowardsPlayer;
    }
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_2D4, 2, thisv->unk_2DC, 0);
    Math_SmoothStepToS(&thisv->actor.world.rot.x, thisv->unk_2D0, 2, thisv->unk_2DC, 0);
    Math_ApproachF(&thisv->unk_2DC, 10000.0f, 1.0f, 1000.0f);
    Math_ApproachF(&thisv->actor.speedXZ, thisv->unk_2E0, 0.9f, 1.0f);
    if ((thisv->actor.gravity == -2.0f) && (thisv->unk_262 == 0) &&
        ((thisv->actor.bgCheckFlags & 8) || (thisv->unk_25C == 0))) {
        thisv->unk_2E0 = 0.0f;
        thisv->actor.gravity = 0.0f;
        thisv->unk_2DC = 0.0f;
        thisv->unk_2D0 = thisv->actor.world.rot.x - 5000.0f;
        thisv->actionFunc = func_809B5C18;
    } else if (thisv->actor.bgCheckFlags & 1) {
        func_809B5268(thisv, globalCtx, 5);
    } else {
        func_809B5268(thisv, globalCtx, 2);
    }
}

void func_809B5C18(EnAttackNiw* thisv, GlobalContext* globalCtx) {
    if (!func_809B55EC(thisv, globalCtx)) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Math_SmoothStepToS(&thisv->actor.world.rot.x, thisv->unk_2D0, 5, thisv->unk_2DC, 0);
    Math_ApproachF(&thisv->unk_2DC, 5000.0f, 1.0f, 100.0f);
    Math_ApproachF(&thisv->actor.velocity.y, 5.0f, 0.3f, 1.0f);
    func_809B5268(thisv, globalCtx, 2);
}

void EnAttackNiw_Update(Actor* thisx, GlobalContext* globalCtx) {
    f32 tmpf1;
    EnAttackNiw* thisv = (EnAttackNiw*)thisx;
    EnNiw* cucco;
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    Vec3f sp30;
    GlobalContext* globalCtx2 = globalCtx;

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
    if (thisv->unk_25E != 0) {
        thisv->unk_25E--;
    }
    if (thisv->unk_260 != 0) {
        thisv->unk_260--;
    }
    if (thisv->unk_25C != 0) {
        thisv->unk_25C--;
    }
    if (thisv->unk_262 != 0) {
        thisv->unk_262--;
    }

    thisv->actor.shape.rot = thisv->actor.world.rot;
    thisv->actor.shape.shadowScale = 15.0f;
    thisv->actionFunc(thisv, globalCtx2);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 0x1D);

    if (thisv->actionFunc == func_809B5670) {
        func_8002D97C(&thisv->actor);
    } else {
        Actor_MoveForward(&thisv->actor);
    }

    if (thisv->actor.floorHeight <= BGCHECK_Y_MIN) {
        Actor_Kill(&thisv->actor);
        return;
    }

    if ((thisv->actor.bgCheckFlags & 0x20) && (thisv->actionFunc != func_809B5C18)) {
        Math_Vec3f_Copy(&sp30, &thisv->actor.world.pos);
        sp30.y += thisv->actor.yDistToWater;
        EffectSsGSplash_Spawn(globalCtx, &sp30, 0, 0, 0, 0x190);
        thisv->unk_2DC = 0.0f;
        thisv->actor.gravity = 0.0f;
        thisv->unk_2E0 = 0.0f;
        thisv->unk_2D0 = thisv->actor.world.rot.x - 5000.0f;
        thisv->actionFunc = func_809B5C18;
        return;
    }

    tmpf1 = 20.0f;
    if (thisv->actor.xyzDistToPlayerSq < SQ(tmpf1)) {
        cucco = (EnNiw*)thisv->actor.parent;
        if ((thisv->actor.parent->update != NULL) && (thisv->actor.parent != NULL) && (cucco != NULL) &&
            (cucco->timer9 == 0) && (player->invincibilityTimer == 0)) {
            func_8002F6D4(globalCtx, &thisv->actor, 2.0f, thisv->actor.world.rot.y, 0.0f, 0x10);
            cucco->timer9 = 0x46;
        }
    }
    if (thisv->unk_25E == 0) {
        thisv->unk_25E = 30;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_CHICKEN_CRY_A);
    }
    if (thisv->unk_260 == 0) {
        thisv->unk_260 = 7;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DEKU_WAKEUP);
    }
}

s32 func_809B5F98(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnAttackNiw* thisv = (EnAttackNiw*)thisx;
    Vec3f sp0 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 13) {
        rot->y += (s16)thisv->unk_2BC;
    }
    if (limbIndex == 15) {
        rot->z += (s16)thisv->unk_2C0;
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
    return 0;
}

void EnAttackNiw_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnAttackNiw* thisv = (EnAttackNiw*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          func_809B5F98, NULL, thisv);
}
