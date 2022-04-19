/*
 * File: z_en_insect.c
 * Overlay: ovl_En_Insect
 * Description: Bugs
 */

#include "z_en_insect.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void EnInsect_Init(Actor* thisx, GlobalContext* globalCtx);
void EnInsect_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnInsect_Update(Actor* thisx, GlobalContext* globalCtx);
void EnInsect_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnInsect_Reset(void);

void func_80A7C3A0(EnInsect* thisv);
void func_80A7C3F4(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7C598(EnInsect* thisv);
void func_80A7C5EC(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7C818(EnInsect* thisv);
void func_80A7C86C(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7CAD0(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7CBC8(EnInsect* thisv);
void func_80A7CC3C(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7CE60(EnInsect* thisv);
void func_80A7CEC0(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7D1F4(EnInsect* thisv);
void func_80A7D26C(EnInsect* thisv, GlobalContext* globalCtx);
void func_80A7D39C(EnInsect* thisv);
void func_80A7D460(EnInsect* thisv, GlobalContext* globalCtx);

static f32 D_80A7DEB0 = 0.0f;
static s16 D_80A7DEB4 = 0;
static s16 D_80A7DEB8 = 0;

ActorInit En_Insect_InitVars = {
    ACTOR_EN_INSECT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnInsect),
    (ActorFunc)EnInsect_Init,
    (ActorFunc)EnInsect_Destroy,
    (ActorFunc)EnInsect_Update,
    (ActorFunc)EnInsect_Draw,
    EnInsect_Reset,
};

static ColliderJntSphElementInit sColliderItemInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sColliderItemInit,
};

static u16 D_80A7DF10[] = { 0, 5, 7, 7 };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 600, ICHAIN_STOP),
};

void func_80A7BE20(EnInsect* thisv) {
    thisv->unk_314 = D_80A7DF10[thisv->actor.params & 3];
}

f32 EnInsect_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

s32 EnInsect_InBottleRange(EnInsect* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    Vec3f pos;

    if (thisv->actor.xzDistToPlayer < 32.0f) {
        pos.x = Math_SinS(thisv->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.x;
        pos.y = player->actor.world.pos.y;
        pos.z = Math_CosS(thisv->actor.yawTowardsPlayer + 0x8000) * 16.0f + player->actor.world.pos.z;

        //! @bug: thisv check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnInsect_XZDistanceSquared(&pos, &thisv->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

void func_80A7BF58(EnInsect* thisv) {
    Animation_Change(&thisv->skelAnime, &gBugCrawlAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
}

/**
 * Find the nearest soft dirt patch within 6400 units in the xz plane and the current room
 *
 * @return 1 if one was found, 0 otherwise
 */
s32 EnInsect_FoundNearbySoil(EnInsect* thisv, GlobalContext* globalCtx) {
    Actor* currentActor;
    f32 currentDistance;
    f32 bestDistance;
    s32 ret;

    ret = 0;
    currentActor = globalCtx->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    bestDistance = 6400.0f;
    thisv->soilActor = NULL;

    while (currentActor != NULL) {
        if (currentActor->id == ACTOR_OBJ_MAKEKINSUTA) {
            currentDistance = Math3D_Dist2DSq(thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                              currentActor->world.pos.x, currentActor->world.pos.z);

            if (currentDistance < bestDistance && currentActor->room == thisv->actor.room) {
                ret = 1;
                bestDistance = currentDistance;
                thisv->soilActor = (ObjMakekinsuta*)currentActor;
            }
        }
        currentActor = currentActor->next;
    }
    return ret;
}

void func_80A7C058(EnInsect* thisv) {
    if (thisv->unk_31E > 0) {
        thisv->unk_31E--;
        return;
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_WALK);

    thisv->unk_31E = 3.0f / CLAMP_MIN(thisv->skelAnime.playSpeed, 0.1f);
    if (thisv->unk_31E < 2) {
        thisv->unk_31E = 2;
    }
}

void EnInsect_Init(Actor* thisx, GlobalContext* globalCtx2) {
    EnInsect* thisv = (EnInsect*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    f32 rand;
    s16 temp_s2;
    s32 count;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    func_80A7BE20(thisv);

    temp_s2 = thisv->actor.params & 3;

    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gBugSkel, &gBugCrawlAnim, thisv->jointTable, thisv->morphTable, 24);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sColliderInit, &thisv->colliderItem);

    thisv->actor.colChkInfo.mass = 30;

    if (thisv->unk_314 & 1) {
        thisv->actor.gravity = -0.2f;
        thisv->actor.minVelocityY = -2.0f;
    }

    if (thisv->unk_314 & 4) {
        thisv->unk_31C = Rand_S16Offset(200, 40);
        thisv->actor.flags |= ACTOR_FLAG_4;
    }

    if (temp_s2 == 2 || temp_s2 == 3) {
        if (EnInsect_FoundNearbySoil(thisv, globalCtx)) {
            thisv->unk_314 |= 0x10;
            D_80A7DEB0 = 0.0f;
        }

        if (temp_s2 == 2) {
            thisv->actor.world.rot.z = 0;
            thisv->actor.shape.rot.z = thisv->actor.world.rot.z;

            for (count = 0; count < 2; count++) {
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_INSECT, thisv->actor.world.pos.x,
                            thisv->actor.world.pos.y, thisv->actor.world.pos.z, thisv->actor.shape.rot.x,
                            thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 3);
            }
        }

        func_80A7D39C(thisv);

        D_80A7DEB8++;
    } else {
        rand = Rand_ZeroOne();

        if (rand < 0.3f) {
            func_80A7C3A0(thisv);
        } else if (rand < 0.4f) {
            func_80A7C598(thisv);
        } else {
            func_80A7C818(thisv);
        }
    }
}

void EnInsect_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s16 temp_v0;
    EnInsect* thisv = (EnInsect*)thisx;

    temp_v0 = thisv->actor.params & 3;
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
    if ((temp_v0 == 2 || temp_v0 == 3) && D_80A7DEB8 > 0) {
        D_80A7DEB8--;
    }
}

void func_80A7C3A0(EnInsect* thisv) {
    thisv->unk_31A = Rand_S16Offset(5, 35);
    func_80A7BF58(thisv);
    thisv->actionFunc = func_80A7C3F4;
    thisv->unk_314 |= 0x100;
}

void func_80A7C3F4(EnInsect* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s16 sp2E;
    f32 playSpeed;

    sp2E = thisv->actor.params & 3;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);

    playSpeed = (Rand_ZeroOne() * 0.8f) + (thisv->actor.speedXZ * 1.2f);
    thisv->skelAnime.playSpeed = CLAMP(playSpeed, 0.0f, 1.9f);

    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if (thisv->unk_31A <= 0) {
        func_80A7C598(thisv);
    }

    if (((thisv->unk_314 & 4) && thisv->unk_31C <= 0) ||
        ((sp2E == 2 || sp2E == 3) && (thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4)) {
        func_80A7CBC8(thisv);
    } else if ((thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(thisv);
    } else if (thisv->actor.xzDistToPlayer < 40.0f) {
        func_80A7C818(thisv);
    }
}

void func_80A7C598(EnInsect* thisv) {
    thisv->unk_31A = Rand_S16Offset(10, 45);
    func_80A7BF58(thisv);
    thisv->actionFunc = func_80A7C5EC;
    thisv->unk_314 |= 0x100;
}

void func_80A7C5EC(EnInsect* thisv, GlobalContext* globalCtx) {
    s32 pad1;
    s32 pad2;
    s16 yaw;
    s16 sp34 = thisv->actor.params & 3;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 1.5f, 0.1f, 0.5f, 0.0f);

    if (EnInsect_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > 1600.0f || (thisv->unk_31A < 4)) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, 2000);
    } else if (thisv->actor.child != NULL && &thisv->actor != thisv->actor.child) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.child->world.pos);
        Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, 2000);
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->skelAnime.playSpeed = CLAMP(thisv->actor.speedXZ * 1.4f, 0.7f, 1.9f);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_31A <= 0) {
        func_80A7C3A0(thisv);
    }

    if (((thisv->unk_314 & 4) && thisv->unk_31C <= 0) ||
        ((sp34 == 2 || sp34 == 3) && (thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4)) {
        func_80A7CBC8(thisv);
    } else if ((thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(thisv);
    } else if (thisv->actor.xzDistToPlayer < 40.0f) {
        func_80A7C818(thisv);
    }
}

void func_80A7C818(EnInsect* thisv) {
    thisv->unk_31A = Rand_S16Offset(10, 40);
    func_80A7BF58(thisv);
    thisv->actionFunc = func_80A7C86C;
    thisv->unk_314 |= 0x100;
}

void func_80A7C86C(EnInsect* thisv, GlobalContext* globalCtx) {
    s32 pad1;
    s32 pad2;
    s16 pad3;
    s16 frames;
    s16 yaw;
    s16 sp38 = thisv->actor.xzDistToPlayer < 40.0f;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 1.8f, 0.1f, 0.5f, 0.0f);

    if (EnInsect_XZDistanceSquared(&thisv->actor.world.pos, &thisv->actor.home.pos) > 25600.0f || thisv->unk_31A < 4) {
        yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, 2000);
    } else if (sp38 != 0) {
        frames = globalCtx->state.frames;
        yaw = thisv->actor.yawTowardsPlayer + 0x8000;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (globalCtx) {} // Must be 'globalCtx'
        Math_ScaledStepToS(&thisv->actor.world.rot.y, yaw, 2000);
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    thisv->skelAnime.playSpeed = CLAMP(thisv->actor.speedXZ * 1.6f, 0.8f, 1.9f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_31A <= 0 || !sp38) {
        func_80A7C3A0(thisv);
    } else if ((thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(thisv);
    }
}

void func_80A7CA64(EnInsect* thisv) {
    thisv->unk_31A = 200;

    Actor_SetScale(&thisv->actor, 0.001f);

    thisv->actor.draw = NULL;
    thisv->actor.speedXZ = 0.0f;

    func_80A7BF58(thisv);

    thisv->skelAnime.playSpeed = 0.3f;
    thisv->actionFunc = func_80A7CAD0;
    thisv->unk_314 &= ~0x100;
}

void func_80A7CAD0(EnInsect* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_31A == 20 && !(thisv->unk_314 & 4)) {
        thisv->actor.draw = EnInsect_Draw;
    } else if (thisv->unk_31A == 0) {
        if (thisv->unk_314 & 4) {
            Actor_Kill(&thisv->actor);
        } else {
            Actor_SetScale(&thisv->actor, 0.01f);
            func_80A7C3A0(thisv);
        }
    } else if (thisv->unk_31A < 20) {
        Actor_SetScale(&thisv->actor, CLAMP_MAX(thisv->actor.scale.x + 0.001f, 0.01f));
        SkelAnime_Update(&thisv->skelAnime);
    }
}

void func_80A7CBC8(EnInsect* thisv) {
    thisv->unk_31A = 60;
    func_80A7BF58(thisv);
    thisv->skelAnime.playSpeed = 1.9f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_SINK);
    Math_Vec3f_Copy(&thisv->actor.home.pos, &thisv->actor.world.pos);
    thisv->actionFunc = func_80A7CC3C;
    thisv->unk_314 &= ~0x100;
    thisv->unk_314 |= 0x8;
}

void func_80A7CC3C(EnInsect* thisv, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Vec3f unused = { 0.0f, 0.0f, 0.0f };
    s32 pad[2];
    Vec3f velocity;

    Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);
    Math_StepToS(&thisv->actor.shape.rot.x, 10922, 352);

    Actor_SetScale(&thisv->actor, CLAMP_MIN(thisv->actor.scale.x - 0.0002f, 0.001f));

    thisv->actor.shape.yOffset -= 0.8f;
    thisv->actor.world.pos.x = Rand_ZeroOne() + thisv->actor.home.pos.x - 0.5f;
    thisv->actor.world.pos.z = Rand_ZeroOne() + thisv->actor.home.pos.z - 0.5f;

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_31A > 20 && Rand_ZeroOne() < 0.1f) {
        velocity.x = Math_SinS(thisv->actor.shape.rot.y) * -0.6f;
        velocity.y = Math_SinS(thisv->actor.shape.rot.x) * 0.6f;
        velocity.z = Math_CosS(thisv->actor.shape.rot.y) * -0.6f;
        func_800286CC(globalCtx, &thisv->actor.world.pos, &velocity, &accel, Rand_ZeroOne() * 5.0f + 8.0f,
                      Rand_ZeroOne() * 5.0f + 8.0f);
    }

    if (thisv->unk_31A <= 0) {
        if ((thisv->unk_314 & 0x10) && thisv->soilActor != NULL &&
            Math3D_Vec3fDistSq(&thisv->soilActor->actor.world.pos, &thisv->actor.world.pos) < 64.0f) {
            thisv->soilActor->unk_152 = 1;
        }
        Actor_Kill(&thisv->actor);
    }
}

void func_80A7CE60(EnInsect* thisv) {
    thisv->unk_31A = Rand_S16Offset(120, 50);
    func_80A7BF58(thisv);
    thisv->unk_316 = thisv->unk_318 = 0;
    thisv->actionFunc = func_80A7CEC0;
    thisv->unk_314 &= ~0x100;
}

void func_80A7CEC0(EnInsect* thisv, GlobalContext* globalCtx) {
    f32 temp_f0;
    s16 temp_v1;
    s16 pad;
    s16 sp4E;
    Vec3f sp40;
    s32 phi_v0;
    s32 phi_v0_2;

    sp4E = thisv->actor.params & 3;

    if (thisv->unk_31A >= 81) {
        Math_StepToF(&thisv->actor.speedXZ, 0.6f, 0.08f);
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.02f);
    }
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.world.pos.y += thisv->actor.yDistToWater;
    thisv->skelAnime.playSpeed = CLAMP(thisv->unk_31A * 0.018f, 0.1f, 1.9f);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_31A >= 81) {
        thisv->unk_316 += Rand_S16Offset(-50, 100);
        thisv->unk_318 += Rand_S16Offset(-300, 600);
    }

    temp_v1 = thisv->skelAnime.playSpeed * 200.0f;

    if (thisv->unk_316 < -temp_v1) {
        thisv->unk_316 = -temp_v1;
    } else {
        if (temp_v1 < thisv->unk_316) {
            phi_v0 = temp_v1;
        } else {
            phi_v0 = thisv->unk_316;
        }
        thisv->unk_316 = phi_v0;
    }
    thisv->actor.world.rot.y += thisv->unk_316;

    temp_v1 = thisv->skelAnime.playSpeed * 1000.0f;
    if (thisv->unk_318 < -temp_v1) {
        thisv->unk_318 = -temp_v1;
    } else {
        if (temp_v1 < thisv->unk_318) {
            phi_v0_2 = temp_v1;
        } else {
            phi_v0_2 = thisv->unk_318;
        }
        thisv->unk_318 = phi_v0_2;
    }
    thisv->actor.shape.rot.y += thisv->unk_318;

    Math_ScaledStepToS(&thisv->actor.world.rot.x, 0, 3000);
    thisv->actor.shape.rot.x = thisv->actor.world.rot.x;

    if (Rand_ZeroOne() < 0.03f) {
        sp40.x = thisv->actor.world.pos.x;
        sp40.y = thisv->actor.world.pos.y + thisv->actor.yDistToWater;
        sp40.z = thisv->actor.world.pos.z;
        EffectSsGRipple_Spawn(globalCtx, &sp40, 20, 100, 4);
        EffectSsGRipple_Spawn(globalCtx, &sp40, 40, 200, 8);
    }

    if (thisv->unk_31A <= 0 || ((thisv->unk_314 & 4) && thisv->unk_31C <= 0) ||
        ((sp4E == 2 || sp4E == 3) && (thisv->unk_314 & 1) && D_80A7DEB8 >= 4)) {
        func_80A7D1F4(thisv);
    } else if (!(thisv->actor.bgCheckFlags & 0x40)) {
        if (thisv->unk_314 & 0x10) {
            func_80A7D39C(thisv);
        } else {
            func_80A7C3A0(thisv);
        }
    }
}

void func_80A7D1F4(EnInsect* thisv) {
    thisv->unk_31A = 100;
    func_80A7BF58(thisv);
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.minVelocityY = -0.8f;
    thisv->actor.gravity = -0.04f;
    thisv->unk_314 &= ~0x3;
    thisv->actionFunc = func_80A7D26C;
    thisv->unk_314 &= ~0x100;
    thisv->unk_314 |= 8;
}

void func_80A7D26C(EnInsect* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.rot.x -= 500;
    thisv->actor.shape.rot.y += 200;
    Actor_SetScale(&thisv->actor, CLAMP_MIN(thisv->actor.scale.x - 0.00005f, 0.001f));

    if (thisv->actor.yDistToWater > 5.0f && thisv->actor.yDistToWater < 30.0f && Rand_ZeroOne() < 0.3f) {
        EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, -5.0f, 5.0f, 5.0f, (Rand_ZeroOne() * 0.04f) + 0.02f);
    }

    if (thisv->unk_31A <= 0) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80A7D39C(EnInsect* thisv) {
    func_80A7BF58(thisv);
    thisv->unk_31A = 100;
    thisv->unk_324 = 1.5f;
    thisv->unk_328 = Rand_ZeroOne() * (0xFFFF + 0.5f);
    thisv->unk_316 = (Rand_ZeroOne() - 0.5f) * 1500.0f;
    thisv->actor.world.rot.y = Rand_ZeroOne() * (0xFFFF + 0.5f);
    Actor_SetScale(&thisv->actor, 0.003f);
    thisv->actionFunc = func_80A7D460;
    thisv->unk_314 |= 0x100;
}

void func_80A7D460(EnInsect* thisv, GlobalContext* globalCtx) {
    s32 temp_a0;
    s32 sp50;
    f32 phi_f0;
    EnInsect* thisTemp = thisv;
    s32 temp_a1;
    f32 sp40;
    f32 phi_f2;
    s16 sp3A;
    s16 sp38;
    f32 sp34;

    sp50 = 0;
    sp3A = thisv->actor.params & 3;

    if (thisv->soilActor != NULL) {
        sp40 = Math3D_Vec3fDistSq(&thisv->actor.world.pos, &thisv->soilActor->actor.world.pos);
    } else {
        if (thisv->unk_314 & 0x10) {
            osSyncPrintf(VT_COL(YELLOW, BLACK));
            // "warning: target Actor is NULL"
            osSyncPrintf("warning:目標 Actor が NULL (%s %d)\n", "../z_en_mushi.c", 1046);
            osSyncPrintf(VT_RST);
        }
        sp40 = 40.0f;
    }

    D_80A7DEB0 += 0.99999994f / 300.0f;
    if (D_80A7DEB0 > 1.0f) {
        D_80A7DEB0 = 1.0f;
    }

    if (D_80A7DEB0 > 0.999f) {
        phi_f2 = 0.0f;
    } else {
        if (sp40 > 900.0f) {
            phi_f2 = ((1.1f - D_80A7DEB0) * 100.0f) + 20.0f;
        } else {
            phi_f2 = (1.0f - D_80A7DEB0) * 10.0f;
        }
    }

    if (thisv->soilActor != NULL && Rand_ZeroOne() < 0.07f) {
        thisv->actor.home.pos.x = (Rand_ZeroOne() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.x;
        thisv->actor.home.pos.y = thisTemp->soilActor->actor.world.pos.y;
        thisv->actor.home.pos.z = (Rand_ZeroOne() - 0.5f) * phi_f2 + thisTemp->soilActor->actor.world.pos.z;
    }

    if (D_80A7DEB0 > 0.999f) {
        thisv->unk_328 = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos);
        thisv->unk_324 = Rand_ZeroOne() * 0.6f + 0.6f;
    } else if (Rand_ZeroOne() < 0.07f) {
        if (thisv->unk_324 > 1.0f) {
            thisv->unk_324 = 0.1f;
        } else {
            thisv->unk_324 = Rand_ZeroOne() * 0.8f + 1.0f;
        }

        sp34 = 1.3f - D_80A7DEB0;
        if (sp34 < 0.0f) {
            sp34 = 0.0f;
        } else {
            if (sp34 > 1.0f) {
                phi_f0 = 1.0f;
            } else {
                phi_f0 = sp34;
            }
            sp34 = phi_f0;
        }

        sp38 = (Rand_ZeroOne() - 0.5f) * 65535.0f * sp34;
        thisv->unk_328 = Math_Vec3f_Yaw(&thisv->actor.world.pos, &thisv->actor.home.pos) + sp38;
    }

    Actor_SetScale(&thisv->actor, CLAMP_MAX(thisTemp->actor.scale.x + 0.0008f, 0.01f));

    if (thisv->actor.bgCheckFlags & 1) {
        Math_SmoothStepToF(&thisv->actor.speedXZ, thisv->unk_324, 0.1f, 0.5f, 0.0f);
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->unk_328, 2000);
        sp50 = Math_ScaledStepToS(&thisv->actor.world.rot.x, 0, 2000);
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        thisv->actor.shape.rot.x = thisv->actor.world.rot.x;
    } else {
        Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.1f, 0.5f, 0.0f);
        thisv->actor.speedXZ += (Rand_ZeroOne() - 0.5f) * 0.14f;
        thisv->actor.velocity.y += Rand_ZeroOne() * 0.12f;
        thisv->actor.world.rot.y += thisv->unk_316;
        thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        thisv->actor.shape.rot.x -= 2000;
    }

    phi_f2 = Rand_ZeroOne() * 0.5f + thisv->actor.speedXZ * 1.3f;
    if (phi_f2 < 0.0f) {
        thisv->skelAnime.playSpeed = 0.0f;
    } else {
        if (phi_f2 > 1.9f) {
            phi_f0 = 1.9f;
        } else {
            phi_f0 = phi_f2;
        }
        thisv->skelAnime.playSpeed = phi_f0;
    }

    SkelAnime_Update(&thisv->skelAnime);
    if (!(thisv->unk_314 & 0x40) && (thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 1)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_MUSI_LAND);
        thisv->unk_314 |= 0x40;
    }

    if (sp3A == 2 && (thisv->unk_314 & 0x10) && !(thisv->unk_314 & 0x80)) {
        if (thisv->unk_32A >= 15) {
            if (thisv->soilActor != NULL) {
                if (!(GET_GS_FLAGS(((thisv->soilActor->actor.params >> 8) & 0x1F) - 1) &
                      (thisv->soilActor->actor.params & 0xFF))) {
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
            thisv->unk_314 |= 0x80;
        } else {
            thisv->unk_32A++;
        }
    }

    if ((thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 0x40)) {
        func_80A7CE60(thisv);
    } else if (thisv->unk_314 & 0x10) {
        if (sp40 < 9.0f) {
            func_80A7CBC8(thisv);
        } else if (thisv->unk_31A <= 0 || thisv->unk_31C <= 0 ||
                   ((thisv->unk_314 & 1) && (thisv->actor.bgCheckFlags & 1) && D_80A7DEB8 >= 4 &&
                    (sp3A == 2 || sp3A == 3))) {
            func_80A7CBC8(thisv);
        } else {
            if (sp40 < 900.0f) {
                thisv->unk_31C++;
                thisv->unk_314 |= 0x20;
            } else {
                thisv->unk_31A = 100;
            }
        }
    } else if (sp50 != 0) {
        func_80A7C3A0(thisv);
    } else if ((sp3A == 2 || sp3A == 3) && (thisv->unk_314 & 1) && thisv->unk_31C <= 0 && thisv->unk_31A <= 0 &&
               thisv->actor.floorHeight < BGCHECK_Y_MIN + 10.0f) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "BG missing? To do Actor_delete"
        osSyncPrintf("BG 抜け？ Actor_delete します(%s %d)\n", "../z_en_mushi.c", 1197);
        osSyncPrintf(VT_RST);
        Actor_Kill(&thisv->actor);
    }
}

void EnInsect_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnInsect* thisv = (EnInsect*)thisx;
    s32 phi_v0;

    if (thisv->actor.child != NULL) {
        if (thisv->actor.child->update == NULL) {
            if (&thisv->actor != thisv->actor.child) {
                thisv->actor.child = NULL;
            }
        }
    }

    if (thisv->unk_31A > 0) {
        thisv->unk_31A--;
    }

    if (thisv->unk_31C > 0) {
        thisv->unk_31C--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.update != NULL) {
        Actor_MoveForward(&thisv->actor);
        if (thisv->unk_314 & 0x100) {
            if (thisv->unk_314 & 1) {
                if (thisv->actor.bgCheckFlags & 1) {
                    func_80A7C058(thisv);
                }
            } else {
                func_80A7C058(thisv);
            }
        }

        phi_v0 = 0;

        if (thisv->unk_314 & 1) {
            phi_v0 = 4;
        }

        if (thisv->unk_314 & 2) {
            phi_v0 |= 1;
        }

        if (phi_v0 != 0) {
            phi_v0 |= 0x40;
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 8.0f, 5.0f, 0.0f, phi_v0);
        }

        if (Actor_HasParent(&thisv->actor, globalCtx)) {
            thisv->actor.parent = NULL;
            phi_v0 = thisv->actor.params & 3;

            if (phi_v0 == 2 || phi_v0 == 3) {
                Actor_Kill(&thisv->actor);
            } else {
                func_80A7CA64(thisv);
            }
        } else if (thisv->actor.xzDistToPlayer < 50.0f && thisv->actionFunc != func_80A7CAD0) {
            if (!(thisv->unk_314 & 0x20) && thisv->unk_31C < 180) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }

            if (!(thisv->unk_314 & 8) && D_80A7DEB4 < 4 && EnInsect_InBottleRange(thisv, globalCtx) &&
                // GI_MAX in thisv case allows the player to catch the actor in a bottle
                func_8002F434(&thisv->actor, globalCtx, GI_MAX, 60.0f, 30.0f)) {
                D_80A7DEB4++;
            }
        }

        Actor_SetFocus(&thisv->actor, 0.0f);
    }
}

void EnInsect_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnInsect* thisv = (EnInsect*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, NULL);
    Collider_UpdateSpheres(0, &thisv->collider);
    D_80A7DEB4 = 0;
}

void EnInsect_Reset(void) {
    D_80A7DEB0 = 0.0f;
    D_80A7DEB4 = 0;
    D_80A7DEB8 = 0;
}