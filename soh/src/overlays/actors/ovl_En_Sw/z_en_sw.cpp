#include "z_en_sw.h"
#include "objects/object_st/object_st.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnSw_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSw_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSw_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSw_Draw(Actor* thisx, GlobalContext* globalCtx);
s32 func_80B0DFFC(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0D364(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0E5E0(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0D590(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0E90C(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0E9BC(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0E728(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0DC7C(EnSw* thisv, GlobalContext* globalCtx);
s32 func_80B0C0CC(EnSw* thisv, GlobalContext* globalCtx, s32);
void func_80B0D3AC(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0DB00(EnSw* thisv, GlobalContext* globalCtx);
void func_80B0D878(EnSw* thisv, GlobalContext* globalCtx);

const ActorInit En_Sw_InitVars = {
    ACTOR_EN_SW,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSw),
    (ActorFunc)EnSw_Init,
    (ActorFunc)EnSw_Destroy,
    (ActorFunc)EnSw_Update,
    (ActorFunc)EnSw_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        { ELEMTYPE_UNK0, { 0xFFCFFFFF, 0x00, 0x08 }, { 0xFFC3FFFE, 0x00, 0x00 }, 0x01, 0x05, 0x01 },
        { 2, { { 0, -300, 0 }, 21 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    { COLTYPE_HIT6, 0x11, 0x09, 0x39, 0x10, COLSHAPE_JNTSPH },
    1,
    sJntSphItemsInit,
};

static CollisionCheckInfoInit2 D_80B0F074 = { 1, 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENSW_ANIM_0,
    /* 1 */ ENSW_ANIM_1,
    /* 2 */ ENSW_ANIM_2,
    /* 3 */ ENSW_ANIM_3
} EnSwAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, 0.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
};

char D_80B0F630[0x80]; // unused

void EnSw_CrossProduct(Vec3f* a, Vec3f* b, Vec3f* dst) {
    dst->x = (a->y * b->z) - (a->z * b->y);
    dst->y = (a->z * b->x) - (a->x * b->z);
    dst->z = (a->x * b->y) - (a->y * b->x);
}

s32 func_80B0BE20(EnSw* thisv, CollisionPoly* poly) {
    Vec3f sp44;
    Vec3f sp38;
    f32 sp34;
    f32 temp_f0;
    s32 pad;

    thisv->actor.floorPoly = poly;
    sp44.x = COLPOLY_GET_NORMAL(poly->normal.x);
    sp44.y = COLPOLY_GET_NORMAL(poly->normal.y);
    sp44.z = COLPOLY_GET_NORMAL(poly->normal.z);
    sp34 = Math_FAcosF(DOTXYZ(sp44, thisv->unk_364));
    EnSw_CrossProduct(&thisv->unk_364, &sp44, &sp38);
    Matrix_RotateAxis(sp34, &sp38, MTXMODE_NEW);
    Matrix_MultVec3f(&thisv->unk_370, &sp38);
    thisv->unk_370 = sp38;
    EnSw_CrossProduct(&thisv->unk_370, &sp44, &thisv->unk_37C);
    temp_f0 = Math3D_Vec3fMagnitude(&thisv->unk_37C);
    if (temp_f0 < 0.001f) {
        return 0;
    }
    thisv->unk_37C.x = thisv->unk_37C.x * (1.0f / temp_f0);
    thisv->unk_37C.y = thisv->unk_37C.y * (1.0f / temp_f0);
    thisv->unk_37C.z = thisv->unk_37C.z * (1.0f / temp_f0);
    thisv->unk_364 = sp44;
    thisv->unk_3D8.xx = thisv->unk_370.x;
    thisv->unk_3D8.yx = thisv->unk_370.y;
    thisv->unk_3D8.zx = thisv->unk_370.z;
    thisv->unk_3D8.wx = 0.0f;
    thisv->unk_3D8.xy = thisv->unk_364.x;
    thisv->unk_3D8.yy = thisv->unk_364.y;
    thisv->unk_3D8.zy = thisv->unk_364.z;
    thisv->unk_3D8.wy = 0.0f;
    thisv->unk_3D8.xz = thisv->unk_37C.x;
    thisv->unk_3D8.yz = thisv->unk_37C.y;
    thisv->unk_3D8.zz = thisv->unk_37C.z;
    thisv->unk_3D8.wz = 0.0f;
    thisv->unk_3D8.xw = 0.0f;
    thisv->unk_3D8.yw = 0.0f;
    thisv->unk_3D8.zw = 0.0f;
    thisv->unk_3D8.ww = 1.0f;
    Matrix_MtxFToYXZRotS(&thisv->unk_3D8, &thisv->actor.world.rot, 0);
    //! @bug: Does not return.
}

CollisionPoly* func_80B0C020(GlobalContext* globalCtx, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, s32* arg4) {
    CollisionPoly* sp3C;
    s32 pad;

    if (!BgCheck_EntityLineTest1(&globalCtx->colCtx, arg1, arg2, arg3, &sp3C, true, true, true, false, arg4)) {
        return NULL;
    }

    if (func_80041DB8(&globalCtx->colCtx, sp3C, *arg4) & 0x30) {
        return NULL;
    }

    if (SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, sp3C, *arg4)) {
        return NULL;
    }

    return sp3C;
}

s32 func_80B0C0CC(EnSw* thisv, GlobalContext* globalCtx, s32 arg2) {
    CollisionPoly* temp_v0_2;
    CollisionPoly* temp_s1;
    Vec3f sp9C;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    s32 pad;
    s32 sp70;
    s32 sp6C;
    s32 phi_s1;
    s32 sp64;

    sp64 = 0;
    thisv->unk_42C = 1;
    sp84 = sp78 = thisv->actor.world.pos;
    sp84.x += thisv->unk_364.x * 18.0f;
    sp84.y += thisv->unk_364.y * 18.0f;
    sp84.z += thisv->unk_364.z * 18.0f;
    sp78.x -= thisv->unk_364.x * 18.0f;
    sp78.y -= thisv->unk_364.y * 18.0f;
    sp78.z -= thisv->unk_364.z * 18.0f;
    temp_s1 = func_80B0C020(globalCtx, &sp84, &sp78, &sp90, &sp70);

    if ((temp_s1 != NULL) && (thisv->unk_360 == 0)) {
        sp78.x = sp84.x + (thisv->unk_37C.x * 24);
        sp78.y = sp84.y + (thisv->unk_37C.y * 24);
        sp78.z = sp84.z + (thisv->unk_37C.z * 24);
        temp_v0_2 = func_80B0C020(globalCtx, &sp84, &sp78, &sp9C, &sp6C);
        if (temp_v0_2 != NULL) {
            if (arg2 == 1) {
                func_80B0BE20(thisv, temp_v0_2);
                thisv->actor.world.pos = sp9C;
                thisv->actor.floorBgId = sp6C;
            }
        } else {
            if (thisv->actor.floorPoly != temp_s1) {
                func_80B0BE20(thisv, temp_s1);
            }
            thisv->actor.world.pos = sp90;
            thisv->actor.floorBgId = sp70;
        }
        sp64 = 1;
    } else {
        sp84 = sp78;
        for (phi_s1 = 0; phi_s1 < 3; phi_s1++) {
            if (phi_s1 == 0) {
                sp78.x = sp84.x - (thisv->unk_37C.x * 24.0f);
                sp78.y = sp84.y - (thisv->unk_37C.y * 24.0f);
                if (0) {}
                sp78.z = sp84.z - (thisv->unk_37C.z * 24.0f);
            } else if (phi_s1 == 1) {
                sp78.x = sp84.x + (thisv->unk_370.x * 24.0f);
                sp78.y = sp84.y + (thisv->unk_370.y * 24.0f);
                sp78.z = sp84.z + (thisv->unk_370.z * 24.0f);
            } else {
                sp78.x = sp84.x - (thisv->unk_370.x * 24.0f);
                sp78.y = sp84.y - (thisv->unk_370.y * 24.0f);
                sp78.z = sp84.z - (thisv->unk_370.z * 24.0f);
            }
            temp_v0_2 = func_80B0C020(globalCtx, &sp84, &sp78, &sp9C, &sp6C);
            if (temp_v0_2 != NULL) {
                if (arg2 == 1) {
                    func_80B0BE20(thisv, temp_v0_2);
                    thisv->actor.world.pos = sp9C;
                    thisv->actor.floorBgId = sp6C;
                }
                sp64 = 1;
                break;
            }
        }
    }

    Math_SmoothStepToS(&thisv->actor.shape.rot.x, thisv->actor.world.rot.x, 8, 0xFA0, 1);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 8, 0xFA0, 1);
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->actor.world.rot.z, 8, 0xFA0, 1);

    return sp64;
}

void EnSw_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSw* thisv = (EnSw*)thisx;
    s32 phi_v0;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    if (thisx->params & 0x8000) {
        phi_v0 = (((thisx->params - 0x8000) & 0xE000) >> 0xD) + 1;
        thisx->params = (thisx->params & 0x1FFF) | (phi_v0 << 0xD);
    }

    if (((thisx->params & 0xE000) >> 0xD) > 0) {
        phi_v0 = ((thisx->params & 0x1F00) >> 8) - 1;
        thisx->params = (thisx->params & 0xE0FF) | (phi_v0 << 8);
    }

    // Check to see if thisv gold skull token has already been retrieved.
    if (GET_GS_FLAGS((thisx->params & 0x1F00) >> 8) & (thisx->params & 0xFF)) {
        Actor_Kill(&thisv->actor);
        return;
    }

    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_st_Skel_005298, NULL, thisv->jointTable, thisv->morphTable, 30);
    Animation_ChangeByInfo(&thisv->skelAnime, sAnimationInfo, ENSW_ANIM_0);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->sphs);
    CollisionCheck_SetInfo2(&thisv->actor.colChkInfo, DamageTable_Get(0xE), &D_80B0F074);
    thisv->actor.scale.x = 0.02f;

    if (((thisx->params & 0xE000) >> 0xD) == 0) {
        thisv->actor.world.rot.x = 0;
        thisv->actor.world.rot.z = 0;
        thisx->shape.rot = thisv->actor.world.rot;
        thisv->unk_484.y = thisv->actor.world.pos.y;
        thisv->unk_484.x = thisv->actor.world.pos.x + (Math_SinS(thisv->actor.world.rot.y) * -60.0f);
        thisv->unk_484.z = thisv->actor.world.pos.z + (Math_CosS(thisv->actor.world.rot.y) * -60.0f);
        func_80B0DFFC(thisv, globalCtx);
        thisv->actor.home.pos = thisv->actor.world.pos;
    } else {
        thisv->unk_370.x = Math_SinS(thisx->shape.rot.y + 0x4000);
        thisv->unk_370.y = 0.0f;
        thisv->unk_370.z = Math_CosS(thisx->shape.rot.y + 0x4000);
        thisv->unk_364.x = 0.0f;
        thisv->unk_364.y = 1.0f;
        thisv->unk_364.z = 0.0f;
        thisv->unk_37C.x = Math_SinS(thisx->shape.rot.y);
        thisv->unk_37C.y = 0.0f;
        thisv->unk_37C.z = Math_CosS(thisx->shape.rot.y);
        func_80B0C0CC(thisv, globalCtx, 1);
    }

    if (((thisx->params & 0xE000) >> 0xD) >= 3) {
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    switch ((thisx->params & 0xE000) >> 0xD) {
        case 3:
        case 4:
            thisv->unk_360 = 1;
            thisv->actor.velocity.y = 8.0f;
            thisv->actor.speedXZ = 4.0f;
            thisv->actor.gravity = -1.0f;
        case 2:
            thisv->actor.scale.x = 0.0f;
        case 1:
            thisv->collider.elements[0].info.toucher.damage *= 2;
            thisv->actor.naviEnemyId = 0x20;
            thisv->actor.colChkInfo.health *= 2;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            break;
        default:
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
            thisv->actor.naviEnemyId = 0x1F;
            break;
    }

    thisv->unk_38E = Rand_S16Offset(0xF, 0x1E);
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
    thisv->actor.home.pos = thisv->actor.world.pos;
    thisx->shape.rot = thisv->actor.world.rot;

    if (((thisx->params & 0xE000) >> 0xD) >= 3) {
        thisv->unk_38C = 0x28;
        thisv->unk_394 = 1;
        thisv->actionFunc = func_80B0D364;
    } else if (((thisx->params & 0xE000) >> 0xD) == 0) {
        thisv->actionFunc = func_80B0E5E0;
    } else {
        thisv->actionFunc = func_80B0D590;
    }
}

void EnSw_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSw* thisv = (EnSw*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

s32 func_80B0C9F0(EnSw* thisv, GlobalContext* globalCtx) {
    s32 phi_v1 = false;

    if (thisv->actor.xyzDistToPlayerSq < SQ(400.0f) && ((thisv->actor.params & 0xE000) >> 0xD) == 0 &&
        globalCtx->actorCtx.unk_02 != 0) {

        thisv->actor.colChkInfo.damage = thisv->actor.colChkInfo.health;
        phi_v1 = true;
    }

    if (thisv->unk_392 == 0) {
        if ((thisv->collider.base.acFlags & 2) || phi_v1) {
            thisv->collider.base.acFlags &= ~2;
            thisv->unk_392 = 0x10;
            Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, thisv->unk_392);
            if (Actor_ApplyDamage(&thisv->actor) != 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALTU_DAMAGE);
                return true;
            }
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
                thisv->skelAnime.playSpeed = 8.0f;
                if ((globalCtx->state.frames & 1) == 0) {
                    thisv->unk_420 = 0.1f;
                } else {
                    thisv->unk_420 = -0.1f;
                }
                thisv->unk_394 = 0xA;
                thisv->unk_38A = 1;
                thisv->unk_420 *= 4.0f;
                thisv->actionFunc = func_80B0D878;
            } else {
                thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
                thisv->actor.shape.shadowAlpha = 0xFF;
                thisv->unk_38A = 2;
                thisv->actor.shape.shadowScale = 16.0f;
                thisv->actor.gravity = -1.0f;
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                thisv->actionFunc = func_80B0DB00;
            }

            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALWALL_DEAD);
            return true;
        }
    }

    if ((thisv->unk_390 == 0) && (thisv->collider.base.atFlags & 2)) {
        thisv->unk_390 = 30;
    }

    return false;
}

void func_80B0CBE8(EnSw* thisv, GlobalContext* globalCtx) {
    if ((((thisv->actor.params & 0xE000) >> 0xD) > 0) && (thisv->actionFunc != func_80B0D590)) {
        if (thisv->unk_392 != 0) {
            thisv->unk_392--;
        }
    } else {
        if ((DECR(thisv->unk_390) == 0) && (thisv->actor.colChkInfo.health != 0)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        if ((DECR(thisv->unk_392) == 0) && (thisv->actor.colChkInfo.health != 0)) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

s32 func_80B0CCF4(EnSw* thisv, f32* arg1) {
    CollisionPoly* temp_v1;
    f32 temp_f0;
    Vec3f sp6C;
    MtxF sp2C;

    if (thisv->actor.floorPoly == NULL) {
        return false;
    }

    temp_v1 = thisv->actor.floorPoly;
    sp6C.x = COLPOLY_GET_NORMAL(temp_v1->normal.x);
    sp6C.y = COLPOLY_GET_NORMAL(temp_v1->normal.y);
    sp6C.z = COLPOLY_GET_NORMAL(temp_v1->normal.z);
    Matrix_RotateAxis(*arg1, &sp6C, MTXMODE_NEW);
    Matrix_MultVec3f(&thisv->unk_370, &sp6C);
    thisv->unk_370 = sp6C;
    EnSw_CrossProduct(&thisv->unk_370, &thisv->unk_364, &thisv->unk_37C);
    temp_f0 = Math3D_Vec3fMagnitude(&thisv->unk_37C);
    if (temp_f0 < 0.001f) {
        return false;
    }
    temp_f0 = 1.0f / temp_f0;
    thisv->unk_37C.x *= temp_f0;
    thisv->unk_37C.y *= temp_f0;
    thisv->unk_37C.z *= temp_f0;
    sp2C.xx = thisv->unk_370.x;
    sp2C.yx = thisv->unk_370.y;
    sp2C.zx = thisv->unk_370.z;
    sp2C.wx = 0.0f;
    sp2C.xy = thisv->unk_364.x;
    sp2C.yy = thisv->unk_364.y;
    sp2C.zy = thisv->unk_364.z;
    sp2C.wy = 0.0f;
    sp2C.xz = thisv->unk_37C.x;
    sp2C.yz = thisv->unk_37C.y;
    sp2C.zz = thisv->unk_37C.z;
    sp2C.wz = 0.0f;
    sp2C.xw = 0.0f;
    sp2C.yw = 0.0f;
    sp2C.zw = 0.0f;
    sp2C.ww = 1.0f;
    Matrix_MtxFToYXZRotS(&sp2C, &thisv->actor.world.rot, 0);
    return true;
}

void func_80B0CEA8(EnSw* thisv, GlobalContext* globalCtx) {
    if (!(thisv->actor.scale.x < 0.0139999995f)) {
        Camera* activeCam = GET_ACTIVE_CAM(globalCtx);

        if (!(Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &activeCam->eye) >= 380.0f)) {
            Audio_PlayActorSound2(&thisv->actor, ((thisv->actor.params & 0xE000) >> 0xD) > 0 ? NA_SE_EN_STALGOLD_ROLL
                                                                                           : NA_SE_EN_STALWALL_ROLL);
        }
    }
}

void func_80B0CF44(EnSw* thisv, GlobalContext* globalCtx, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        pos.x = thisv->actor.world.pos.x + (Math_SinS(angle) * 2.0f);
        pos.y = thisv->actor.world.pos.y;
        pos.z = thisv->actor.world.pos.z + (Math_CosS(angle) * 2.0f);
        func_8002836C(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 20, 30, 12);
    }
}

void func_80B0D14C(EnSw* thisv, GlobalContext* globalCtx, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        pos.x = thisv->actor.world.pos.x + (Math_SinS(angle) * 14.0f);
        pos.y = thisv->actor.world.pos.y;
        pos.z = thisv->actor.world.pos.z + (Math_CosS(angle) * 14.0f);
        func_8002836C(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 20, 40, 10);
    }
}

void func_80B0D364(EnSw* thisv, GlobalContext* globalCtx) {
    if (((thisv->actor.params & 0xE000) >> 0xD) == 4) {
        thisv->unk_38C = 0;
        thisv->actionFunc = func_80B0D3AC;
    } else {
        thisv->unk_38C = 10;
        thisv->actionFunc = func_80B0D3AC;
    }
}

void func_80B0D3AC(EnSw* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_38C != 0) {
        if ((thisv->unk_38C & 4) != 0) {
            func_80B0CF44(thisv, globalCtx, 5);
        }
        thisv->unk_38C--;
        if (thisv->unk_38C == 0) {
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_STALGOLD_UP_CRY);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 40, NA_SE_EN_DODO_M_UP);
        } else {
            return;
        }
    }

    Math_ApproachF(&thisv->actor.scale.x, 0.02f, 0.2f, 0.01f);
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
    thisv->actor.world.pos.x += thisv->unk_364.x * thisv->actor.velocity.y;
    thisv->actor.world.pos.y += thisv->unk_364.y * thisv->actor.velocity.y;
    thisv->actor.world.pos.z += thisv->unk_364.z * thisv->actor.velocity.y;
    thisv->actor.world.pos.x += thisv->unk_37C.x * thisv->actor.speedXZ;
    thisv->actor.world.pos.y += thisv->unk_37C.y * thisv->actor.speedXZ;
    thisv->actor.world.pos.z += thisv->unk_37C.z * thisv->actor.speedXZ;
    thisv->actor.velocity.y += thisv->actor.gravity;
    thisv->actor.velocity.y = CLAMP_MIN(thisv->actor.velocity.y, thisv->actor.minVelocityY);

    if (thisv->actor.velocity.y < 0.0f) {
        thisv->unk_360 = 0;
    }

    if (func_80B0C0CC(thisv, globalCtx, 1) == 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        func_80B0D14C(thisv, globalCtx, 8);
        thisv->actor.scale.x = 0.02f;
        Actor_SetScale(&thisv->actor, 0.02f);
        thisv->actionFunc = func_80B0D590;
        thisv->actor.velocity.y = 0.0f;
        thisv->actor.speedXZ = 0.0f;
        thisv->actor.gravity = 0.0f;
    }
}

void func_80B0D590(EnSw* thisv, GlobalContext* globalCtx) {
    f32 sp2C;

    if (((thisv->actor.params & 0xE000) >> 0xD) == 2) {
        if (thisv->actor.scale.x < 0.0139999995f) {
            thisv->collider.elements[0].info.toucherFlags = 0;
            thisv->collider.elements[0].info.bumperFlags = 0;
            thisv->collider.elements[0].info.ocElemFlags = 0;
        }

        if (thisv->actor.scale.x >= 0.0139999995f) {
            thisv->collider.elements[0].info.toucherFlags = 1;
            thisv->collider.elements[0].info.bumperFlags = 1;
            thisv->collider.elements[0].info.ocElemFlags = 1;
        }

        Math_ApproachF(&thisv->actor.scale.x, !IS_DAY ? 0.02f : 0.0f, 0.2f, 0.01f);
        Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
    }

    if (thisv->unk_38E != 0) {
        thisv->unk_38E--;
        if (thisv->unk_38E == 0) {
            func_80B0CEA8(thisv, globalCtx);
            thisv->unk_420 = ((globalCtx->state.frames % 2) == 0) ? 0.1f : -0.1f;
            thisv->unk_38A = 1;
            thisv->unk_38C = Rand_S16Offset(30, 60);
            if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
                thisv->unk_38C *= 2;
                thisv->unk_420 *= 2.0f;
            }
        }
    } else {
        thisv->unk_38C--;
        if (thisv->unk_38C == 0) {
            thisv->unk_38E = Rand_S16Offset(15, 30);
            thisv->unk_38A = 0;
            thisv->skelAnime.playSpeed = 0.0f;
            if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
                thisv->unk_38E /= 2;
            }
        } else if (thisv->unk_38A != 0) {
            thisv->unk_38A--;
            thisv->skelAnime.playSpeed = (thisv->unk_38A == 0) ? 4.0f : 0.0f;

            if (thisv->skelAnime.playSpeed > 0.0f) {
                func_80B0CEA8(thisv, globalCtx);
            }
            if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
                thisv->skelAnime.playSpeed *= 2.0f;
            }
        } else {
            if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame) == 1) {
                thisv->unk_38A = 2;
            }
            sp2C = 32768.0f / thisv->skelAnime.endFrame;
            sp2C *= thisv->skelAnime.curFrame;
            sp2C = Math_SinS(sp2C) * thisv->unk_420;
            func_80B0CCF4(thisv, &sp2C);
            thisv->actor.shape.rot = thisv->actor.world.rot;
        }
    }
}

void func_80B0D878(EnSw* thisv, GlobalContext* globalCtx) {
    Actor* temp_v0;
    Vec3f pos;
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    f32 x;
    f32 y;
    f32 z;

    if (Animation_OnFrame(&thisv->skelAnime, thisv->skelAnime.endFrame) == 1) {
        func_80B0CEA8(thisv, globalCtx);
    }

    func_80B0CCF4(thisv, &thisv->unk_420);
    thisv->actor.shape.rot = thisv->actor.world.rot;

    if ((thisv->unk_394 == 0) && (thisv->unk_392 == 0)) {
        Audio_PlaySoundGeneral(NA_SE_SY_KINSTA_MARK_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        x = (thisv->unk_364.x * 10.0f);
        y = (thisv->unk_364.y * 10.0f);
        z = (thisv->unk_364.z * 10.0f);
        temp_v0 =
            Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_SI, thisv->actor.world.pos.x + x,
                               thisv->actor.world.pos.y + y, thisv->actor.world.pos.z + z, 0, 0, 0, thisv->actor.params);
        if (temp_v0 != NULL) {
            temp_v0->parent = NULL;
        }
        Actor_Kill(&thisv->actor);
        return;
    }

    if ((thisv->unk_392 == 0) && (DECR(thisv->unk_394) != 0)) {
        pos = thisv->actor.world.pos;
        pos.y += 10.0f + ((Rand_ZeroOne() - 0.5f) * 6.0f);
        pos.x += (Rand_ZeroOne() - 0.5f) * 32.0f;
        pos.z += (Rand_ZeroOne() - 0.5f) * 32.0f;
        EffectSsDeadDb_Spawn(globalCtx, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                             true);
    }
}

void func_80B0DB00(EnSw* thisv, GlobalContext* globalCtx) {
    Actor_MoveForward(&thisv->actor);
    thisv->actor.shape.rot.x += 0x1000;
    thisv->actor.shape.rot.z += 0x1000;
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 0.0f, 5);

    if ((thisv->actor.bgCheckFlags & 1) && (!(0.0f <= thisv->actor.velocity.y))) {
        if (thisv->actor.floorHeight <= BGCHECK_Y_MIN || thisv->actor.floorHeight >= 32000.0f) {
            Actor_Kill(&thisv->actor);
            return;
        }

        thisv->actor.bgCheckFlags &= ~1;

        if (thisv->unk_38A == 0) {
            thisv->actionFunc = func_80B0DC7C;
            thisv->unk_394 = 10;
        } else {
            thisv->actor.velocity.y = ((thisv->unk_38A--) * 8.0f) * 0.5f;
        }

        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 16.0f, 0xC, 2.0f, 0x78, 0xA, 0);
    }
}

void func_80B0DC7C(EnSw* thisv, GlobalContext* globalCtx) {
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    if (DECR(thisv->unk_394) != 0) {
        pos.y = ((Rand_ZeroOne() - 0.5f) * 6.0f) + (thisv->actor.world.pos.y + 10.0f);
        pos.x = ((Rand_ZeroOne() - 0.5f) * 32.0f) + thisv->actor.world.pos.x;
        pos.z = ((Rand_ZeroOne() - 0.5f) * 32.0f) + thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9,
                             1);
        thisv->actor.shape.rot.x += 0x1000;
        thisv->actor.shape.rot.z += 0x1000;
    } else {
        Item_DropCollectibleRandom(globalCtx, NULL, &thisv->actor.world.pos, 0x30);
        Actor_Kill(&thisv->actor);
    }
}

s16 func_80B0DE34(EnSw* thisv, Vec3f* arg1) {
    s16 pitch;
    s16 yaw;

    yaw = Math_Vec3f_Yaw(&thisv->actor.world.pos, arg1) - thisv->actor.wallYaw;
    pitch = Math_Vec3f_Pitch(&thisv->actor.world.pos, arg1) - 0x4000;
    return pitch * (yaw >= 0 ? -1 : 1);
}

s32 func_80B0DEA8(EnSw* thisv, GlobalContext* globalCtx, s32 arg2) {
    Player* player = GET_PLAYER(globalCtx);
    CollisionPoly* sp58;
    s32 sp54;
    Vec3f sp48;

    if (!(player->stateFlags1 & 0x200000) && arg2) {
        return false;
    } else if (func_8002DDF4(globalCtx) && arg2) {
        return false;
    } else if (ABS(func_80B0DE34(thisv, &player->actor.world.pos) - thisv->actor.shape.rot.z) >= 0x1FC2) {
        return false;
    } else if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &player->actor.world.pos) >= 130.0f) {
        return false;
    } else if (!BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &player->actor.world.pos, &sp48,
                                        &sp58, true, false, false, true, &sp54)) {
        return true;
    } else {
        return false;
    }
}

s32 func_80B0DFFC(EnSw* thisv, GlobalContext* globalCtx) {
    s32 pad;
    CollisionPoly* sp60;
    s32 sp5C;
    Vec3f sp50;
    s32 sp4C = true;

    if (thisv->collider.base.ocFlags1 & OC1_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        sp4C = false;
    } else if (((globalCtx->state.frames % 4) == 0) &&
               !BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_454, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((globalCtx->state.frames % 4) == 1) &&
               BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_460, &sp50, &sp60, true,
                                       false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((globalCtx->state.frames % 4) == 2) &&
               !BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_46C, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        if (0) {}
        sp4C = false;
    } else if (((globalCtx->state.frames % 4) == 3) &&
               BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_478, &sp50, &sp60, true,
                                       false, false, true, &sp5C)) {
        sp4C = false;
    }

    if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->actor.world.pos, &thisv->unk_484, &sp50, &thisv->unk_430, true,
                                false, false, true, &sp5C)) {
        thisv->actor.wallYaw = Math_FAtan2F(thisv->unk_430->normal.x, thisv->unk_430->normal.z) * (0x8000 / std::numbers::pi_v<float>);
        thisv->actor.world.pos = sp50;
        thisv->actor.world.pos.x += 6.0f * Math_SinS(thisv->actor.world.rot.y);
        thisv->actor.world.pos.z += 6.0f * Math_CosS(thisv->actor.world.rot.y);
        thisv->unk_434 = sp50;
        thisv->unk_434.x += Math_SinS(thisv->actor.world.rot.y);
        thisv->unk_434.z += Math_CosS(thisv->actor.world.rot.y);
    }

    return sp4C;
}

void func_80B0E314(EnSw* thisv, Vec3f arg1, f32 arg4) {
    f32 xDist;
    f32 yDist;
    f32 zDist;
    f32 dist;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    Math_SmoothStepToF(&thisv->actor.speedXZ, arg4, 0.3f, 100.0f, 0.1f);
    xDiff = arg1.x - thisv->actor.world.pos.x;
    yDiff = arg1.y - thisv->actor.world.pos.y;
    zDiff = arg1.z - thisv->actor.world.pos.z;
    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (dist == 0.0f) {
        xDist = yDist = zDist = 0.0f;
    } else {
        xDist = xDiff / dist;
        yDist = yDiff / dist;
        zDist = zDiff / dist;
    }
    xDist *= thisv->actor.speedXZ;
    yDist *= thisv->actor.speedXZ;
    zDist *= thisv->actor.speedXZ;
    thisv->actor.world.pos.x += xDist;
    thisv->actor.world.pos.y += yDist;
    thisv->actor.world.pos.z += zDist;
}

s32 func_80B0E430(EnSw* thisv, f32 arg1, s16 arg2, s32 arg3, GlobalContext* globalCtx) {
    Camera* activeCam;
    f32 lastFrame = Animation_GetLastFrame(&object_st_Anim_000304);

    if (DECR(thisv->unk_388) != 0) {
        Math_SmoothStepToF(&thisv->skelAnime.playSpeed, 0.0f, 0.6f, 1000.0f, 0.01f);
        return 0;
    }

    Math_SmoothStepToF(&thisv->skelAnime.playSpeed, arg1, 0.6f, 1000.0f, 0.01f);

    if ((arg3 == 1) && (lastFrame < (thisv->skelAnime.curFrame + thisv->skelAnime.playSpeed))) {
        return 0;
    }

    activeCam = GET_ACTIVE_CAM(globalCtx);

    if (Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &activeCam->eye) < 380.0f) {
        if (DECR(thisv->unk_440) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALWALL_ROLL);
            thisv->unk_440 = 4;
        }
    } else {
        thisv->unk_440 = 0;
    }
    Math_SmoothStepToS(&thisv->actor.shape.rot.z, thisv->unk_444, 4, arg2, arg2);
    thisv->actor.world.rot = thisv->actor.shape.rot;
    if (thisv->actor.shape.rot.z == thisv->unk_444) {
        return 1;
    }
    return 0;
}

void func_80B0E5E0(EnSw* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    f32 rand;

    if (func_80B0E430(thisv, 6.0f, 0x3E8, 1, globalCtx)) {
        rand = Rand_ZeroOne();
        thisv->unk_444 =
            ((s16)(20000.0f * rand) + 0x2EE0) * (Rand_ZeroOne() >= 0.5f ? 1.0f : -1.0f) + thisv->actor.world.rot.z;
        thisv->unk_388 = Rand_S16Offset(10, 30);
    }

    if ((DECR(thisv->unk_442) == 0) && (func_80B0DEA8(thisv, globalCtx, 1))) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALWALL_LAUGH);
        thisv->unk_442 = 20;
        thisv->actionFunc = func_80B0E728;
    }
}

void func_80B0E728(EnSw* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;

    if (DECR(thisv->unk_442) != 0) {
        if (func_80B0DEA8(thisv, globalCtx, 1)) {
            thisv->unk_448 = player->actor.world.pos;
            thisv->unk_448.y += 30.0f;
            thisv->unk_444 = func_80B0DE34(thisv, &thisv->unk_448);
            func_80B0E430(thisv, 6.0f, (u16)0xFA0, 0, globalCtx);
        } else {
            thisv->actionFunc = func_80B0E5E0;
        }
    } else {
        if (!func_80B0DFFC(thisv, globalCtx)) {
            thisv->unk_442 = Rand_S16Offset(20, 10);
            thisv->unk_444 = func_80B0DE34(thisv, &thisv->actor.home.pos);
            thisv->unk_448 = thisv->actor.home.pos;
            thisv->actionFunc = func_80B0E9BC;
        } else {
            func_80B0E314(thisv, thisv->unk_448, 8.0f);

            if (DECR(thisv->unk_440) == 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STALWALL_DASH);
                thisv->unk_440 = 4;
            }

            if (!(Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->unk_448) > 13.0f) || func_8002DDF4(globalCtx)) {
                thisv->actionFunc = func_80B0E90C;
            }
        }
    }
}

void func_80B0E90C(EnSw* thisv, GlobalContext* globalCtx) {
    s32 pad;

    func_80B0E314(thisv, thisv->unk_448, 0.0f);
    if (thisv->actor.speedXZ == 0.0f) {
        thisv->unk_444 = func_80B0DE34(thisv, &thisv->actor.home.pos);
        thisv->unk_448 = thisv->actor.home.pos;
        thisv->actionFunc = func_80B0E9BC;
    }
}

void func_80B0E9BC(EnSw* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if (func_80B0E430(thisv, 6.0f, 0x3E8, 0, globalCtx)) {
        func_80B0E314(thisv, thisv->unk_448, 2.0f);
        if (!(Math_Vec3f_DistXYZ(&thisv->actor.world.pos, &thisv->unk_448) > 4.0f)) {
            thisv->actionFunc = func_80B0E5E0;
        }
    }
}

void EnSw_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSw* thisv = (EnSw*)thisx;

    SkelAnime_Update(&thisv->skelAnime);
    func_80B0C9F0(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    func_80B0CBE8(thisv, globalCtx);
}

s32 EnSw_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f sp7C = { 1400.0f, -2600.0f, -800.0f };
    Vec3f sp70 = { 1400.0f, -1600.0f, 0.0f };
    Vec3f sp64 = { -1400.0f, -2600.0f, -800.0f };
    Vec3f sp58 = { -1400.0f, -1600.0f, 0.0f };
    Vec3f sp4C = { 0.0, 0.0f, -600.0f };
    EnSw* thisv = (EnSw*)thisx;
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2084);

    if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
        switch (limbIndex) {
            case 23:
                *dList = object_st_DL_004788;
                break;
            case 8:
                *dList = object_st_DL_0046F0;
                break;
            case 14:
                *dList = object_st_DL_004658;
                break;
            case 11:
                *dList = object_st_DL_0045C0;
                break;
            case 26:
                *dList = object_st_DL_004820;
                break;
            case 20:
                *dList = object_st_DL_0048B8;
                break;
            case 17:
                *dList = object_st_DL_004950;
                break;
            case 29:
                *dList = object_st_DL_0049E8;
                break;
            case 5:
                *dList = object_st_DL_003FB0;
                break;
            case 4:
                *dList = object_st_DL_0043D8;
                break;
        }
    }

    if (limbIndex == 1) {
        Matrix_MultVec3f(&sp7C, &thisv->unk_454);
        Matrix_MultVec3f(&sp70, &thisv->unk_460);
        Matrix_MultVec3f(&sp64, &thisv->unk_46C);
        Matrix_MultVec3f(&sp58, &thisv->unk_478);
        Matrix_MultVec3f(&sp4C, &thisv->unk_484);
    }

    if (limbIndex == 5) {
        Matrix_MultVec3f(&sp3C, &thisv->actor.focus.pos);
    }

    if (limbIndex == 4) {
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->unk_1F4.r, thisv->unk_1F4.g, thisv->unk_1F4.b, 0);
    }

    Collider_UpdateSpheres(limbIndex, &thisv->collider);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2145);

    return false;
}

void EnSw_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void func_80B0EDB8(GlobalContext* globalCtx, Color_RGBA8* arg1, s16 arg2, s16 arg3) {
    f32 temp_f2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2181);

    temp_f2 = (11500.0f / arg3) * (arg3 - arg2);

    if (0.0f == temp_f2) {
        temp_f2 = 11500;
    }

    POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP, arg1->r, arg1->g, arg1->b, arg1->a, 0, (s16)temp_f2);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2197);
}

void func_80B0EEA4(GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2205);

    POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_sw.c", 2207);
}

void EnSw_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSw* thisv = (EnSw*)thisx;
    Color_RGBA8 sp30 = { 184, 0, 228, 255 };

    if (((thisv->actor.params & 0xE000) >> 0xD) != 0) {
        Matrix_RotateX(DEGF_TO_RADF(-80), MTXMODE_APPLY);
        if (thisv->actor.colChkInfo.health != 0) {
            Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        }
        func_8002EBCC(&thisv->actor, globalCtx, 0);
    } else if (thisv->actionFunc == func_80B0E728) {
        func_80B0EDB8(globalCtx, &sp30, 0x14, 0x1E);
    }

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnSw_OverrideLimbDraw,
                      EnSw_PostLimbDraw, thisv);
    if (thisv->actionFunc == func_80B0E728) {
        func_80B0EEA4(globalCtx);
    }
}
