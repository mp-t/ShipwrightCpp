/*
 * File: z_en_horse_zelda.c
 * Overlay: ovl_En_Horse_Zelda
 * Description: Zelda's Horse
 */

#include "z_en_horse_zelda.h"
#include "objects/object_horse_zelda/object_horse_zelda.h"

#define FLAGS ACTOR_FLAG_4

void EnHorseZelda_Init(Actor* thisx, GlobalContext* globalCtx);
void EnHorseZelda_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnHorseZelda_Update(Actor* thisx, GlobalContext* globalCtx);
void EnHorseZelda_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A6DCCC(EnHorseZelda* thisv, GlobalContext* globalCtx);
void func_80A6DDFC(EnHorseZelda* thisv, GlobalContext* globalCtx);
void func_80A6DC7C(EnHorseZelda* thisv);

ActorInit En_Horse_Zelda_InitVars = {
    ACTOR_EN_HORSE_ZELDA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_ZELDA,
    sizeof(EnHorseZelda),
    (ActorFunc)EnHorseZelda_Init,
    (ActorFunc)EnHorseZelda_Destroy,
    (ActorFunc)EnHorseZelda_Update,
    (ActorFunc)EnHorseZelda_Draw,
    NULL,
};

static const AnimationHeader* sAnimationHeaders[] = { &gHorseZeldaGallopingAnim };

static f32 splaySpeeds[] = { 2.0f / 3.0f };

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

typedef struct {
    /* 0x0 */ Vec3s unk_0;
    /* 0x6 */ u8 unk_6;
} unknownStruct; // size = 0x8

static unknownStruct D_80A6E240[] = {
    { -1682, -500, 12578, 0x07 }, { -3288, -500, 13013, 0x07 }, { -5142, -417, 11630, 0x07 },
    { -5794, -473, 9573, 0x07 },  { -6765, -500, 8364, 0x07 },  { -6619, -393, 6919, 0x07 },
    { -5193, 124, 5433, 0x07 },   { -2970, 2, 4537, 0x07 },     { -2949, -35, 4527, 0x07 },
    { -1907, -47, 2978, 0x07 },   { 2488, 294, 3628, 0x07 },    { 3089, 378, 4713, 0x07 },
    { 1614, -261, 7596, 0x07 },   { 754, -187, 9295, 0x07 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_STOP),
};

static EnHorseZeldaActionFunc sActionFuncs[] = {
    func_80A6DCCC,
    func_80A6DDFC,
};

void func_80A6D8D0(unknownStruct* data, s32 index, Vec3f* vec) {
    vec->x = data[index].unk_0.x;
    vec->y = data[index].unk_0.y;
    vec->z = data[index].unk_0.z;
}

void func_80A6D918(EnHorseZelda* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp28;
    s16 yawDiff;

    func_80A6D8D0(D_80A6E240, thisv->unk_1EC, &sp28);
    if (Math3D_Vec3f_DistXYZ(&sp28, &thisv->actor.world.pos) <= 400.0f) {
        thisv->unk_1EC++;
        if (thisv->unk_1EC >= 14) {
            thisv->unk_1EC = 0;
            func_80A6D8D0(D_80A6E240, 0, &sp28);
        }
    }
    yawDiff = Math_Vec3f_Yaw(&thisv->actor.world.pos, &sp28) - thisv->actor.world.rot.y;
    if (yawDiff >= 0x12D) {
        thisv->actor.world.rot.y += 0x12C;
    } else if (yawDiff < -0x12C) {
        thisv->actor.world.rot.y -= 0x12C;
    } else {
        thisv->actor.world.rot.y += yawDiff;
    }
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;

    if (Actor_WorldDistXZToActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) <= 300.0f) {
        if (thisv->actor.speedXZ < 12.0f) {
            thisv->actor.speedXZ += 1.0f;
        } else {
            thisv->actor.speedXZ -= 1.0f;
        }
    } else if (thisv->actor.speedXZ < D_80A6E240[thisv->unk_1EC].unk_6) {
        thisv->actor.speedXZ += 0.5f;
    } else {
        thisv->actor.speedXZ -= 0.5f;
    }
}

void EnHorseZelda_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseZelda* thisv = (EnHorseZelda*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetScale(&thisv->actor, 0.0115f);
    thisv->actor.gravity = -3.5f;
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->action = 0;
    thisv->actor.focus.pos.y += 70.0f;
    Skin_Init(globalCtx, &thisv->skin, &gHorseZeldaSkel, &gHorseZeldaGallopingAnim);
    thisv->animationIndex = 0;
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[0]);
    Collider_InitCylinder(globalCtx, &thisv->colliderCylinder);
    Collider_SetCylinderType1(globalCtx, &thisv->colliderCylinder, &thisv->actor, &sCylinderInit);
    Collider_InitJntSph(globalCtx, &thisv->colliderSphere);
    Collider_SetJntSph(globalCtx, &thisv->colliderSphere, &thisv->actor, &sJntSphInit, &thisv->colliderSphereItem);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    thisv->animationIndex = 0;
    func_80A6DC7C(thisv);
}

void EnHorseZelda_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseZelda* thisv = (EnHorseZelda*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinder);
    Collider_DestroyJntSph(globalCtx, &thisv->colliderSphere);
    Skin_Free(globalCtx, &thisv->skin);
}

void func_80A6DC7C(EnHorseZelda* thisv) {
    thisv->action = 0;
    thisv->animationIndex++;
    if (thisv->animationIndex > 0) {
        thisv->animationIndex = 0;
    }
    Animation_PlayOnce(&thisv->skin.skelAnime, sAnimationHeaders[thisv->animationIndex]);
}

void func_80A6DCCC(EnHorseZelda* thisv, GlobalContext* globalCtx) {
    thisv->actor.speedXZ = 0.0f;
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        func_80A6DC7C(thisv);
    }
}

void func_80A6DD14(EnHorseZelda* thisv) {
    f32 sp34;

    thisv->action = 1;
    thisv->animationIndex = 0;
    sp34 = thisv->actor.speedXZ / 6.0f;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    Animation_Change(&thisv->skin.skelAnime, sAnimationHeaders[thisv->animationIndex],
                     splaySpeeds[thisv->animationIndex] * sp34 * 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[thisv->animationIndex]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6DDFC(EnHorseZelda* thisv, GlobalContext* globalCtx) {
    func_80A6D918(thisv, globalCtx);
    if (SkelAnime_Update(&thisv->skin.skelAnime)) {
        func_80A6DD14(thisv);
    }
}

void func_80A6DE38(EnHorseZelda* thisv, GlobalContext* globalCtx) {
    s32 pad;
    CollisionPoly* poly;
    s32 pad2;
    Vec3f pos;
    s32 bgId;

    pos.x = (Math_SinS(thisv->actor.shape.rot.y) * 30.0f) + thisv->actor.world.pos.x;
    pos.y = thisv->actor.world.pos.y + 60.0f;
    pos.z = (Math_CosS(thisv->actor.shape.rot.y) * 30.0f) + thisv->actor.world.pos.z;
    thisv->unk_1F4 = BgCheck_EntityRaycastFloor3(&globalCtx->colCtx, &poly, &bgId, &pos);
    thisv->actor.shape.rot.x = Math_FAtan2F(thisv->actor.world.pos.y - thisv->unk_1F4, 30.0f) * (0x8000 / std::numbers::pi_v<float>);
}

void EnHorseZelda_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseZelda* thisv = (EnHorseZelda*)thisx;
    s32 pad;

    sActionFuncs[thisv->action](thisv, globalCtx);
    thisv->actor.speedXZ = 0.0f;
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 55.0f, 100.0f, 0x1D);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += 70.0f;
    Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCylinder);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCylinder.base);
}

void EnHorseZelda_PostDraw(Actor* thisx, GlobalContext* globalCtx, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseZelda* thisv = (EnHorseZelda*)thisx;
    s32 i;

    for (i = 0; i < thisv->colliderSphere.count; i++) {
        sp4C.x = thisv->colliderSphere.elements[i].dim.modelSphere.center.x;
        sp4C.y = thisv->colliderSphere.elements[i].dim.modelSphere.center.y;
        sp4C.z = thisv->colliderSphere.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, thisv->colliderSphere.elements[i].dim.limb, &sp4C, &sp40);

        thisv->colliderSphere.elements[i].dim.worldSphere.center.x = sp40.x;
        thisv->colliderSphere.elements[i].dim.worldSphere.center.y = sp40.y;
        thisv->colliderSphere.elements[i].dim.worldSphere.center.z = sp40.z;

        thisv->colliderSphere.elements[i].dim.worldSphere.radius =
            thisv->colliderSphere.elements[i].dim.modelSphere.radius * thisv->colliderSphere.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSphere.base);
}

void EnHorseZelda_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnHorseZelda* thisv = (EnHorseZelda*)thisx;

    func_80A6DE38(thisv, globalCtx);
    func_80093D18(globalCtx->state.gfxCtx);
    func_800A6330(&thisv->actor, globalCtx, &thisv->skin, EnHorseZelda_PostDraw, true);
}
