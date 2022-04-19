/*
 * File: z_en_horse_normal.c
 * Overlay: ovl_En_Horse_Normal
 * Description: Non-rideable horses (Lon Lon Ranch and Stable)
 */

#include "z_en_horse_normal.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_horse_normal/object_horse_normal.h"

#define FLAGS 0

typedef struct {
    Vec3s pos;
    u8 unk_06; // thisv may be a s16 if the always-0 following byte is actually not padding
} EnHorseNormalUnkStruct1;

typedef struct {
    s32 len;
    EnHorseNormalUnkStruct1* items;
} EnHorseNormalUnkStruct2;

typedef enum {
    /* 0x00 */ HORSE_CYCLE_ANIMATIONS,
    /* 0x01 */ HORSE_WANDER,
    /* 0x02 */ HORSE_WAIT,
    /* 0x03 */ HORSE_WAIT_CLONE,
    /* 0x04 */ HORSE_FOLLOW_PATH
} EnHorseNormalAction;

void EnHorseNormal_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHorseNormal_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHorseNormal_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHorseNormal_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A6B91C(EnHorseNormal* thisv, GlobalContext* globalCtx);
void func_80A6BC48(EnHorseNormal* thisv);
void func_80A6BCEC(EnHorseNormal* thisv);
void func_80A6C4CC(EnHorseNormal* thisv);
void func_80A6C6B0(EnHorseNormal* thisv);

const ActorInit En_Horse_Normal_InitVars = {
    ACTOR_EN_HORSE_NORMAL,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_NORMAL,
    sizeof(EnHorseNormal),
    (ActorFunc)EnHorseNormal_Init,
    (ActorFunc)EnHorseNormal_Destroy,
    (ActorFunc)EnHorseNormal_Update,
    (ActorFunc)EnHorseNormal_Draw,
    NULL,
};

static AnimationHeader* sAnimations[] = {
    &gHorseNormalIdleAnim,      &gHorseNormalWhinnyAnim,  &gHorseNormalRefuseAnim,
    &gHorseNormalRearingAnim,   &gHorseNormalWalkingAnim, &gHorseNormalTrottingAnim,
    &gHorseNormalGallopingAnim, &gHorseNormalJumpingAnim, &gHorseNormalJumpingHighAnim,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 40, 100, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 60, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 11, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

// Unused
static EnHorseNormalUnkStruct1 D_80A6D428[] = {
    { { 1058, 1, 384 }, 7 },    { { 1653, 39, -381 }, 6 }, { { 1606, 1, -1048 }, 6 }, { { 1053, 1, -1620 }, 6 },
    { { -1012, 1, -1633 }, 7 }, { { -1655, 1, -918 }, 6 }, { { -1586, 1, -134 }, 6 }, { { -961, 1, 403 }, 7 },
};

// Unused
static EnHorseNormalUnkStruct2 D_80A6D468 = { ARRAY_COUNT(D_80A6D428), D_80A6D428 };

// Unused
static EnHorseNormalUnkStruct1 D_80A6D470[] = {
    { { 88, 0, 2078 }, 10 },       { { 2482, 376, 4631 }, 7 },    { { 2228, -28, 6605 }, 12 },
    { { 654, -100, 8864 }, 7 },    { { -297, -500, 10667 }, 12 }, { { -5303, -420, 10640 }, 10 },
    { { -6686, -500, 7760 }, 10 }, { { -5260, 100, 5411 }, 7 },   { { -3573, -269, 3893 }, 10 },
};

// Unused
static EnHorseNormalUnkStruct2 D_80A6D4B8 = { ARRAY_COUNT(D_80A6D470), D_80A6D470 };

void func_80A6B250(EnHorseNormal* thisv) {
    static s32 D_80A6D4C0[] = { 0, 16 };

    if (D_80A6D4C0[thisv->unk_200] < thisv->skin.skelAnime.curFrame &&
        ((thisv->unk_200 != 0) || !(D_80A6D4C0[1] < thisv->skin.skelAnime.curFrame))) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_WALK, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        thisv->unk_200++;
        if (thisv->unk_200 >= ARRAY_COUNT(D_80A6D4C0)) {
            thisv->unk_200 = 0;
        }
    }
}

f32 func_80A6B30C(EnHorseNormal* thisv) {
    static f32 D_80A6D4C8[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.5f, 1.5f, 1.5f, 1.0f };
    f32 result;

    if (thisv->animationIdx == 4) {
        result = D_80A6D4C8[thisv->animationIdx] * thisv->actor.speedXZ * (1 / 2.0f);
    } else if (thisv->animationIdx == 5) {
        result = D_80A6D4C8[thisv->animationIdx] * thisv->actor.speedXZ * (1 / 3.0f);
    } else if (thisv->animationIdx == 6) {
        result = D_80A6D4C8[thisv->animationIdx] * thisv->actor.speedXZ * (1 / 5.0f);
    } else {
        result = D_80A6D4C8[thisv->animationIdx];
    }

    return result;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 300, ICHAIN_STOP),
};

void EnHorseNormal_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseNormal* thisv = (EnHorseNormal*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.gravity = -3.5f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    thisv->action = HORSE_CYCLE_ANIMATIONS;
    thisv->animationIdx = 0;
    Collider_InitCylinder(globalCtx, &thisv->bodyCollider);
    Collider_SetCylinder(globalCtx, &thisv->bodyCollider, &thisv->actor, &sCylinderInit1);
    Collider_InitJntSph(globalCtx, &thisv->headCollider);
    Collider_SetJntSph(globalCtx, &thisv->headCollider, &thisv->actor, &sJntSphInit, thisv->headElements);
    Collider_InitCylinder(globalCtx, &thisv->cloneCollider);
    Collider_SetCylinder(globalCtx, &thisv->cloneCollider, &thisv->actor, &sCylinderInit2);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    if (globalCtx->sceneNum == SCENE_SPOT20) {
        if (thisv->actor.world.rot.z == 0 || !IS_DAY) {
            Actor_Kill(&thisv->actor);
            return;
        }
        if (!LINK_IS_ADULT) {
            if (Flags_GetEventChkInf(0x14)) {
                if (thisv->actor.world.rot.z != 3) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            } else if (thisv->actor.world.rot.z != 1) {
                Actor_Kill(&thisv->actor);
                return;
            }
        } else if (Flags_GetEventChkInf(0x18) || (DREG(1) != 0)) {
            if (thisv->actor.world.rot.z != 7) {
                Actor_Kill(&thisv->actor);
                return;
            }
        } else if (thisv->actor.world.rot.z != 5) {
            Actor_Kill(&thisv->actor);
            return;
        }
        thisv->actor.home.rot.z = thisv->actor.world.rot.z = thisv->actor.shape.rot.z = 0;
        Skin_Init(globalCtx, &thisv->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
        if ((thisv->actor.world.pos.x == -730.0f && thisv->actor.world.pos.y == 0.0f &&
             thisv->actor.world.pos.z == -1100.0f) ||
            (thisv->actor.world.pos.x == 880.0f && thisv->actor.world.pos.y == 0.0f &&
             thisv->actor.world.pos.z == -1170.0f)) {
            func_80A6C6B0(thisv);
            return;
        }
    } else if (globalCtx->sceneNum == SCENE_MALON_STABLE) {
        if (IS_DAY) {
            Actor_Kill(&thisv->actor);
            return;
        } else {
            Skin_Init(globalCtx, &thisv->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
            func_80A6C6B0(thisv);
            return;
        }
    } else if (globalCtx->sceneNum == SCENE_SPOT12) {
        if (thisv->actor.world.pos.x == 3707.0f && thisv->actor.world.pos.y == 1413.0f &&
            thisv->actor.world.pos.z == -665.0f) {
            Skin_Init(globalCtx, &thisv->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
            func_80A6C4CC(thisv);
            return;
        }
        Skin_Init(globalCtx, &thisv->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
    } else {
        Skin_Init(globalCtx, &thisv->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
    }
    if ((thisv->actor.params & 0xF0) == 0x10 && (thisv->actor.params & 0xF) != 0xF) {
        func_80A6B91C(thisv, globalCtx);
    } else {
        func_80A6BC48(thisv);
    }
}

void EnHorseNormal_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseNormal* thisv = (EnHorseNormal*)thisx;

    Skin_Free(globalCtx, &thisv->skin);
    Collider_DestroyCylinder(globalCtx, &thisv->bodyCollider);
    Collider_DestroyCylinder(globalCtx, &thisv->cloneCollider);
    Collider_DestroyJntSph(globalCtx, &thisv->headCollider);
}

void func_80A6B91C(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->action = HORSE_FOLLOW_PATH;
    thisv->animationIdx = 6;
    thisv->waypoint = 0;
    thisv->actor.speedXZ = 7.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_FollowPath(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->actor.params & 0xF];
    Vec3s* pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    f32 dx;
    f32 dz;
    s32 pad;

    pointPos += thisv->waypoint;
    dx = pointPos->x - thisv->actor.world.pos.x;
    dz = pointPos->z - thisv->actor.world.pos.z;
    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 0xA, 0x7D0, 1);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
    if (SQ(dx) + SQ(dz) < 600.0f) {
        thisv->waypoint++;
        if (thisv->waypoint >= path->count) {
            thisv->waypoint = 0;
        }
    }
    thisv->skin.skelAnime.playSpeed = func_80A6B30C(thisv);
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                         Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
        func_80A6BCEC(thisv);
    }
}

void EnHorseNormal_NextAnimation(EnHorseNormal* thisv) {
    thisv->action = HORSE_CYCLE_ANIMATIONS;
    thisv->animationIdx++;

    if (thisv->animationIdx >= ARRAY_COUNT(sAnimations)) {
        thisv->animationIdx = 0;
    }

    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx]);
}

void EnHorseNormal_CycleAnimations(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        EnHorseNormal_NextAnimation(thisv);
    }
}

void func_80A6BC48(EnHorseNormal* thisv) {
    thisv->action = HORSE_WANDER;
    thisv->animationIdx = 0;
    thisv->unk_21C = 0;
    thisv->unk_21E = 0;
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_218 = 0.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6BCEC(EnHorseNormal* thisv) {
    if (thisv->animationIdx == 5) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (thisv->animationIdx == 6) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void func_80A6BD7C(EnHorseNormal* thisv) {
    f32 frame = thisv->skin.skelAnime.curFrame;

    if (thisv->animationIdx == 0 && frame > 28.0f && !(thisv->unk_1E4 & 1)) {
        thisv->unk_1E4 |= 1;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (thisv->animationIdx == 3 && frame > 25.0f && !(thisv->unk_1E4 & 2)) {
        thisv->unk_1E4 |= 2;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnHorseNormal_Wander(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    static s32 D_80A6D4F4[] = { 0, 1, 4, 5, 6, 2, 3 };
    static s32 D_80A6D510[] = { 0, 0, 2, 2, 1, 1, 1, 3, 3 };
    s32 phi_t0 = thisv->animationIdx;
    s32 pad;

    switch (D_80A6D510[thisv->animationIdx]) {
        case 0:
            func_80A6BD7C(thisv);
            thisv->actor.speedXZ = 0.0f;
            thisv->unk_218 = 0.0f;
            break;
        case 1:
            if (Rand_ZeroOne() < 0.1f) {
                thisv->unk_218 = 2.0f * Rand_ZeroOne() - 1.0f;
            }
            thisv->actor.speedXZ += thisv->unk_218;
            if (thisv->actor.speedXZ <= 0.0f) {
                thisv->actor.speedXZ = 0.0f;
                thisv->unk_218 = 0.0f;
                phi_t0 = 0;
            } else if (thisv->actor.speedXZ < 3.0f) {
                func_80A6B250(thisv);
                phi_t0 = 4;
            } else if (thisv->actor.speedXZ < 6.0f) {
                phi_t0 = 5;
            } else if (thisv->actor.speedXZ < 8.0f) {
                phi_t0 = 6;
            } else {
                thisv->actor.speedXZ = 8.0f;
                phi_t0 = 6;
            }
            if (Rand_ZeroOne() < 0.1f || (thisv->unk_21E == 0 && ((thisv->actor.bgCheckFlags & 8) ||
                                                                 (thisv->bodyCollider.base.ocFlags1 & OC1_HIT) ||
                                                                 (thisv->headCollider.base.ocFlags1 & OC1_HIT)))) {
                thisv->unk_21E += (Rand_ZeroOne() * 30.0f) - 15.0f;
                if (thisv->unk_21E > 50) {
                    thisv->unk_21E = 50;
                } else if (thisv->unk_21E < -50) {
                    thisv->unk_21E = -50;
                }
            }
            thisv->unk_21C += thisv->unk_21E;
            if (thisv->unk_21C < -300) {
                thisv->unk_21C = -300;
            } else if (thisv->unk_21C > 300) {
                thisv->unk_21C = 300;
            } else if (Rand_ZeroOne() < 0.25f && fabsf(thisv->unk_21C) < 100.0f) {
                thisv->unk_21C = 0;
                thisv->unk_21E = 0;
            }
            thisv->actor.world.rot.y += thisv->unk_21C;
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
            break;
        case 2:
        case 3:
            break;
    }

    if (phi_t0 != thisv->animationIdx || SkelAnime_Update(&thisv->skin.skelAnime)) {
        if (phi_t0 != thisv->animationIdx) {
            thisv->animationIdx = phi_t0;
            thisv->unk_1E4 &= ~1;
            thisv->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else if (phi_t0 == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                func_80A6BCEC(thisv);
            }
            Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                             Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
        } else {
            switch (D_80A6D510[thisv->animationIdx]) {
                case 0:
                    if (Rand_ZeroOne() < 0.25f) {
                        thisv->unk_218 = 1.0f;
                        phi_t0 = 4;
                    } else {
                        phi_t0 = D_80A6D4F4[(s32)(Rand_ZeroOne() * 2)];
                        thisv->actor.speedXZ = 0.0f;
                        thisv->unk_218 = 0.0f;
                    }
                    break;
                case 1:
                case 2:
                case 3:
                    break;
            }

            thisv->unk_1E4 &= ~1;
            thisv->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else if (phi_t0 == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                func_80A6BCEC(thisv);
            }
            if (phi_t0 != thisv->animationIdx) {
                thisv->animationIdx = phi_t0;
                Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                                 Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, -3.0f);
            } else {
                Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                                 Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
            }
        }
    }
}

void func_80A6C4CC(EnHorseNormal* thisv) {
    thisv->action = HORSE_WAIT;
    thisv->animationIdx = 0;
    thisv->unk_21C = 0;
    thisv->unk_21E = 0;
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_218 = 0.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_Wait(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        f32 rand = Rand_ZeroOne();

        if (rand < 0.4f) {
            thisv->animationIdx = 0;
        } else if (rand < 0.8f) {
            thisv->animationIdx = 1;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            thisv->animationIdx = 3;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                         Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
    }
}

void func_80A6C6B0(EnHorseNormal* thisv) {
    thisv->action = HORSE_WAIT_CLONE;
    thisv->animationIdx = 0;
    thisv->unk_21C = 0;
    thisv->unk_21E = 0;
    thisv->actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_218 = 0.0f;
    Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                     Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseNormal_WaitClone(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    func_80A6BD7C(thisv);

    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        f32 rand = Rand_ZeroOne();

        if (rand < 0.4f) {
            thisv->animationIdx = 0;
        } else if (rand < 0.8f) {
            thisv->animationIdx = 1;
            thisv->unk_1E4 |= 0x20;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            thisv->animationIdx = 3;
            thisv->unk_1E4 |= 0x20;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_204, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        Animation_Change(&thisv->skin.skelAnime, sAnimations[thisv->animationIdx], func_80A6B30C(thisv), 0.0f,
                         Animation_GetLastFrame(sAnimations[thisv->animationIdx]), ANIMMODE_ONCE, 0.0f);

        thisv->unk_1E4 &= ~1;
        thisv->unk_1E4 &= ~2;
        thisv->unk_1E4 &= ~8;
        thisv->unk_1E4 &= ~0x10;
    }
}

void func_80A6C8E0(EnHorseNormal* thisv, GlobalContext* globalCtx) {
    s32 pad;
    CollisionPoly* sp38;
    s32 pad2;
    Vec3f sp28;
    s32 sp24;

    sp28.x = (Math_SinS(thisv->actor.shape.rot.y) * 30.0f) + thisv->actor.world.pos.x;
    sp28.y = thisv->actor.world.pos.y + 60.0f;
    sp28.z = (Math_CosS(thisv->actor.shape.rot.y) * 30.0f) + thisv->actor.world.pos.z;
    thisv->unk_220 = BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, &sp38, &sp24, &sp28);
    thisv->actor.shape.rot.x = Math_FAtan2F(thisv->actor.world.pos.y - thisv->unk_220, 30.0f) * (0x8000 / std::numbers::pi_v<float>);
}

static EnHorseNormalActionFunc sActionFuncs[] = {
    EnHorseNormal_CycleAnimations, EnHorseNormal_Wander,     EnHorseNormal_Wait,
    EnHorseNormal_WaitClone,       EnHorseNormal_FollowPath,
};

void EnHorseNormal_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseNormal* thisv = (EnHorseNormal*)thisx;
    s32 pad;

    sActionFuncs[thisv->action](thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 35.0f, 100.0f, 0x1D);
    if (globalCtx->sceneNum == SCENE_SPOT20 && thisv->actor.world.pos.z < -2400.0f) {
        thisv->actor.world.pos.z = -2400.0f;
    }
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    thisv->unk_204 = thisv->actor.projectedPos;
    thisv->unk_204.y += 120.0f;
    Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCollider.base);
    if (thisv->actor.speedXZ == 0.0f) {
        thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        thisv->actor.colChkInfo.mass = MASS_HEAVY;
    }
}

void EnHorseNormal_PostDraw(Actor* thisx, GlobalContext* globalCtx, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseNormal* thisv = (EnHorseNormal*)thisx;
    s32 i;

    for (i = 0; i < thisv->headCollider.count; i++) {
        sp4C.x = thisv->headCollider.elements[i].dim.modelSphere.center.x;
        sp4C.y = thisv->headCollider.elements[i].dim.modelSphere.center.y;
        sp4C.z = thisv->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_GetLimbPos(skin, thisv->headCollider.elements[i].dim.limb, &sp4C, &sp40);
        thisv->headCollider.elements[i].dim.worldSphere.center.x = sp40.x;
        thisv->headCollider.elements[i].dim.worldSphere.center.y = sp40.y;
        thisv->headCollider.elements[i].dim.worldSphere.center.z = sp40.z;
        thisv->headCollider.elements[i].dim.worldSphere.radius =
            thisv->headCollider.elements[i].dim.modelSphere.radius * thisv->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->headCollider.base);
}

void func_80A6CC88(GlobalContext* globalCtx, EnHorseNormal* thisv, Vec3f* arg2) {
    f32 curFrame = thisv->skin.skelAnime.curFrame;
    f32 wDest;

    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, arg2, &thisv->unk_1E8, &wDest);
    thisv->unk_1F4 = thisv->unk_1E8;
    thisv->unk_1F4.y += 120.0f;

    if (thisv->animationIdx == 0 && curFrame > 28.0f && !(thisv->unk_1E4 & 8)) {
        thisv->unk_1E4 |= 8;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &thisv->unk_1E8, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (thisv->animationIdx == 3 && curFrame > 25.0f && !(thisv->unk_1E4 & 0x10)) {
        thisv->unk_1E4 |= 0x10;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &thisv->unk_1E8, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (thisv->animationIdx == 3 && thisv->unk_1E4 & 0x20) {
        thisv->unk_1E4 &= ~0x20;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->unk_1F4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (thisv->animationIdx == 1 && thisv->unk_1E4 & 0x20) {
        thisv->unk_1E4 &= ~0x20;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->unk_1F4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorseNormal_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseNormal* thisv = (EnHorseNormal*)thisx;
    Mtx* mtx2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_horse_normal.c", 2224);

    if (globalCtx->sceneNum != SCENE_SPOT20 || globalCtx->sceneNum != SCENE_MALON_STABLE) {
        func_80A6C8E0(thisv, globalCtx);
    }
    func_80093D18(globalCtx->state.gfxCtx);
    func_800A6330(&thisv->actor, globalCtx, &thisv->skin, EnHorseNormal_PostDraw, true);

    if (thisv->action == HORSE_WAIT_CLONE) {
        MtxF skinMtx;
        Mtx* mtx1;
        Vec3f clonePos = { 0.0f, 0.0f, 0.0f };
        s16 cloneRotY;
        f32 distFromGround = thisv->actor.world.pos.y - thisv->actor.floorHeight;
        f32 temp_f0_4;

        if (globalCtx->sceneNum == SCENE_MALON_STABLE) {
            if (thisv->actor.world.pos.x == 355.0f && thisv->actor.world.pos.y == 0.0f &&
                thisv->actor.world.pos.z == -245.0f) {
                clonePos.x = 235.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            } else if (thisv->actor.world.pos.x == 238.0f && thisv->actor.world.pos.y == 0.0f &&
                       thisv->actor.world.pos.z == -245.0f) {
                clonePos.x = 478.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            }
        } else if (globalCtx->sceneNum == SCENE_SPOT20) {
            if (thisv->actor.world.pos.x == -730.0f && thisv->actor.world.pos.y == 0.0f &&
                thisv->actor.world.pos.z == -1100.0f) {
                clonePos.x = 780.0f;
                clonePos.y = 0.0f;
                clonePos.z = -80.0f;
                cloneRotY = 0;
            } else if (thisv->actor.world.pos.x == 880.0f && thisv->actor.world.pos.y == 0.0f &&
                       thisv->actor.world.pos.z == -1170.0f) {
                clonePos.x = -1000.0f;
                clonePos.y = 0.0f;
                clonePos.z = -70.0f;
                cloneRotY = 0;
            }
        }
        func_80A6CC88(globalCtx, thisv, &clonePos);
        SkinMatrix_SetTranslateRotateYXZScale(&skinMtx, thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z,
                                              thisv->actor.shape.rot.x, cloneRotY, thisv->actor.shape.rot.z, clonePos.x,
                                              (thisv->actor.shape.yOffset * thisv->actor.scale.y) + clonePos.y,
                                              clonePos.z);
        mtx1 = SkinMatrix_MtxFToNewMtx(globalCtx->state.gfxCtx, &skinMtx);
        if (mtx1 == NULL) {
            return;
        }
        gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_OPA_DISP++, mtx1, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_800A63CC(&thisv->actor, globalCtx, &thisv->skin, NULL, NULL, true, 0,
                      SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS | SKIN_DRAW_FLAG_CUSTOM_MATRIX);
        thisv->cloneCollider.dim.pos.x = clonePos.x;
        thisv->cloneCollider.dim.pos.y = clonePos.y;
        thisv->cloneCollider.dim.pos.z = clonePos.z;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cloneCollider.base);
        func_80094044(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);
        Matrix_Translate(clonePos.x, clonePos.y, clonePos.z, MTXMODE_NEW);
        temp_f0_4 = (1.0f - (distFromGround * 0.01f)) * thisv->actor.shape.shadowScale;
        Matrix_Scale(thisv->actor.scale.x * temp_f0_4, 1.0f, thisv->actor.scale.z * temp_f0_4, MTXMODE_APPLY);
        Matrix_RotateY(cloneRotY * (2.0f * std::numbers::pi_v<float> / 0x10000), MTXMODE_APPLY);
        mtx2 = Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_horse_normal.c", 2329);
        if (mtx2 != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx2, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gHorseShadowDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_horse_normal.c", 2339);
}
