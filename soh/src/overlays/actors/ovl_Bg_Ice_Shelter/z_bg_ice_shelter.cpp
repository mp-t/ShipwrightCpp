#include "z_bg_ice_shelter.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS 0

void BgIceShelter_Init(Actor* thisx, GlobalContext* globalCtx);
void BgIceShelter_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgIceShelter_Update(Actor* thisx, GlobalContext* globalCtx);
void BgIceShelter_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80891064(BgIceShelter* thisv);
void func_808911BC(BgIceShelter* thisv);

void func_8089107C(BgIceShelter* thisv, GlobalContext* globalCtx);
void func_808911D4(BgIceShelter* thisv, GlobalContext* globalCtx);

ActorInit Bg_Ice_Shelter_InitVars = {
    ACTOR_BG_ICE_SHELTER,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_ICE_OBJECTS,
    sizeof(BgIceShelter),
    (ActorFunc)BgIceShelter_Init,
    (ActorFunc)BgIceShelter_Destroy,
    (ActorFunc)BgIceShelter_Update,
    (ActorFunc)BgIceShelter_Draw,
    NULL,
};

static f32 sScales[] = { 0.1f, 0.06f, 0.1f, 0.1f, 0.25f };

static Color_RGBA8 sDustPrimColor = { 250, 250, 250, 255 };
static Color_RGBA8 sDustEnvColor = { 180, 180, 180, 255 };

static ColliderCylinderInit D_8089170C = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit D_80891738 = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC1FFF6, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

void func_80890740(BgIceShelter* thisv, GlobalContext* globalCtx) {
    static s16 cylinderRadii[] = { 47, 33, 44, 41, 100 };
    static s16 cylinderHeights[] = { 80, 54, 90, 60, 200 };
    s32 pad;
    s32 type = (thisv->dyna.actor.params >> 8) & 7;

    Collider_InitCylinder(globalCtx, &thisv->cylinder1);
    Collider_SetCylinder(globalCtx, &thisv->cylinder1, &thisv->dyna.actor, &D_8089170C);
    Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->cylinder1);

    thisv->cylinder1.dim.radius = cylinderRadii[type];
    thisv->cylinder1.dim.height = cylinderHeights[type];

    if (type == 0 || type == 1 || type == 4) {
        Collider_InitCylinder(globalCtx, &thisv->cylinder2);
        Collider_SetCylinder(globalCtx, &thisv->cylinder2, &thisv->dyna.actor, &D_80891738);
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->cylinder2);
        thisv->cylinder2.dim.radius = cylinderRadii[type];
        thisv->cylinder2.dim.height = cylinderHeights[type];
    }

    if (type == 4) {
        thisv->cylinder1.dim.pos.z += 30;
        thisv->cylinder2.dim.pos.z += 30;
    }
}

void func_80890874(BgIceShelter* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG registration failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_ice_shelter.c", 362,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void func_808908FC(Vec3f* dest, Vec3f* src, s16 angle) {
    f32 sin = Math_SinS(angle);
    f32 cos = Math_CosS(angle);

    dest->x = (src->z * sin) + (src->x * cos);
    dest->y = src->y;
    dest->z = (src->z * cos) - (src->x * sin);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgIceShelter_Init(Actor* thisx, GlobalContext* globalCtx) {
    static Vec3f kzIceScale = { 0.18f, 0.27f, 0.24f };
    BgIceShelter* thisv = (BgIceShelter*)thisx;
    s16 type = (thisv->dyna.actor.params >> 8) & 7;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    if (type == 4) {
        thisv->dyna.actor.world.rot.x += 0xBB8;
        thisv->dyna.actor.world.pos.y -= 45.0f;
        thisv->dyna.actor.shape.rot.x = thisv->dyna.actor.world.rot.x;
        thisv->dyna.actor.world.pos.z -= 38.0f;
    }

    if (type == 4) {
        Math_Vec3f_Copy(&thisv->dyna.actor.scale, &kzIceScale);
    } else {
        Actor_SetScale(&thisv->dyna.actor, sScales[type]);
    }

    switch (type) {
        case 2:
            func_80890874(thisv, globalCtx, &object_ice_objects_Col_001C1C, 0);
            break;
        case 3:
            func_80890874(thisv, globalCtx, &object_ice_objects_Col_002920, 0);
            break;
    }

    func_80890740(thisv, globalCtx);

    thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    if (!((thisv->dyna.actor.params >> 6) & 1) && (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F))) {
        Actor_Kill(&thisv->dyna.actor);
        return;
    }

    func_80891064(thisv);

    osSyncPrintf("(ice shelter)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void BgIceShelter_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgIceShelter* thisv = (BgIceShelter*)thisx;

    switch ((thisv->dyna.actor.params >> 8) & 7) {
        case 2:
        case 3:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            break;

        case 0:
        case 1:
        case 4:
            Collider_DestroyCylinder(globalCtx, &thisv->cylinder2);
            break;
    }

    Collider_DestroyCylinder(globalCtx, &thisv->cylinder1);
}

static s16 D_80891794[] = { 0x0000, 0x4000, 0x2000, 0x6000, 0x1000, 0x5000, 0x3000, 0x7000 };
static s16 D_808917A4[] = { 0x0000, 0x003C, 0x0018, 0x0054, 0x0030, 0x000C, 0x0048, 0x0024 };

void func_80890B8C(BgIceShelter* thisv, GlobalContext* globalCtx, f32 chance, f32 scale) {
    f32 cos;
    f32 sin;
    f32 xzOffset;
    Vec3f* icePos;
    s16 angle;
    s16 frames;
    s32 i;
    s32 pad[2];
    Vec3f dustPos;
    Vec3f dustVel;
    Vec3f dustAccel;

    frames = (s16)globalCtx->state.frames & 7;

    for (i = 0; i < 2; i++) {
        if (chance < Rand_ZeroOne()) {
            continue;
        }

        xzOffset = 42.0f * scale;
        icePos = &thisv->dyna.actor.world.pos;
        angle = D_80891794[frames] + (i * 0x8000);
        sin = Math_SinS(angle);
        cos = Math_CosS(angle);

        dustPos.x = (xzOffset * sin) + icePos->x;
        dustPos.y = (16.0f * scale) + icePos->y;
        dustPos.z = (xzOffset * cos) + icePos->z;

        dustVel.x = ((Rand_ZeroOne() * 3.0f) - 1.0f) * sin;
        dustVel.y = 0.0f;
        dustVel.z = ((Rand_ZeroOne() * 3.0f) - 1.0f) * cos;

        dustAccel.x = 0.07f * sin;
        dustAccel.y = 0.8f;
        dustAccel.z = 0.07f * cos;

        func_8002829C(globalCtx, &dustPos, &dustVel, &dustAccel, &sDustPrimColor, &sDustEnvColor, 450.0f * scale,
                      (s16)((Rand_ZeroOne() * 40.0f) + 40.0f) * scale);
    }
}

void func_80890E00(BgIceShelter* thisv, GlobalContext* globalCtx, f32 chance, f32 arg3) {
    static f32 D_808917B4[] = { -1.0f, 1.0f };
    Vec3f* icePos;
    s16 frames;
    s32 pad[2];
    Vec3f dustPos;
    Vec3f dustVel;
    Vec3f dustAccel;
    Vec3f posOffset;
    s32 i;

    frames = (s16)globalCtx->state.frames & 7;

    for (i = 0; i < 2; i++) {
        icePos = &thisv->dyna.actor.world.pos;

        if (chance < Rand_ZeroOne()) {
            continue;
        }

        posOffset.x = (D_808917A4[frames] + ((Rand_ZeroOne() * 12.0f) - 6.0f)) * D_808917B4[i];
        posOffset.y = 15.0f;
        posOffset.z = ((84.0f - posOffset.x) * 0.2f) + (Rand_ZeroOne() * 20.0f);

        func_808908FC(&dustPos, &posOffset, thisv->dyna.actor.world.rot.y);
        Math_Vec3f_Sum(&dustPos, icePos, &dustPos);

        dustVel.x = (Rand_ZeroOne() * 3.0f) - 1.5f;
        dustVel.y = 0.0f;
        dustVel.z = (Rand_ZeroOne() * 3.0f) - 1.5f;

        dustAccel.x = (Rand_ZeroOne() * 0.14f) - 0.07f;
        dustAccel.y = 0.8f;
        dustAccel.z = (Rand_ZeroOne() * 0.14f) - 0.07f;

        func_8002829C(globalCtx, &dustPos, &dustVel, &dustAccel, &sDustPrimColor, &sDustEnvColor, 450,
                      (Rand_ZeroOne() * 40.0f) + 40.0f);
    }
}

void func_80891064(BgIceShelter* thisv) {
    thisv->actionFunc = func_8089107C;
    thisv->alpha = 255;
}

void func_8089107C(BgIceShelter* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 type = (thisv->dyna.actor.params >> 8) & 7;

    if (type == 4) {
        if (thisv->dyna.actor.parent != NULL) {
            thisv->dyna.actor.parent->freezeTimer = 10000;
        }
    }

    if (thisv->cylinder1.base.acFlags & AC_HIT) {
        thisv->cylinder1.base.acFlags &= ~AC_HIT;

        if ((thisv->cylinder1.base.ac != NULL) && (thisv->cylinder1.base.ac->id == ACTOR_EN_ICE_HONO)) {
            if (type == 4) {
                if (thisv->dyna.actor.parent != NULL) {
                    thisv->dyna.actor.parent->freezeTimer = 50;
                }
            }

            func_808911BC(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_ICE_MELT);
        }
    }

    switch (type) {
        case 0:
        case 1:
        case 4:
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder1.base);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder2.base);
            break;
    }

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder1.base);
}

void func_808911BC(BgIceShelter* thisv) {
    thisv->actionFunc = func_808911D4;
    thisv->alpha = 255;
}

static f32 D_808917BC[] = { -0.0015f, -0.0009f, -0.0016f, -0.0016f, -0.00375f };
static f32 D_808917D0[] = { 1.0f, 0.6f, 1.2f, 1.0f, 1.8f };

static void (*sEffSpawnFuncs[])(BgIceShelter* thisv, GlobalContext* globalCtx, f32 chance, f32 scale) = {
    func_80890B8C, func_80890B8C, func_80890B8C, func_80890E00, func_80890B8C,
};

void func_808911D4(BgIceShelter* thisv, GlobalContext* globalCtx) {

    s32 pad;
    s32 type = (thisv->dyna.actor.params >> 8) & 7;
    f32 phi_f0;

    thisv->alpha -= 5;
    thisv->alpha = CLAMP(thisv->alpha, 0, 255);

    thisv->dyna.actor.scale.y += D_808917BC[type];
    thisv->dyna.actor.scale.y = CLAMP_MIN(thisv->dyna.actor.scale.y, 0.0001f);

    if (thisv->alpha > 80) {
        switch (type) {
            case 0:
            case 1:
            case 4:
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder1.base);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder2.base);
                break;
        }
    }

    if (thisv->alpha > 180) {
        phi_f0 = 1.0f;
    } else if (thisv->alpha > 60) {
        phi_f0 = 0.5f;
    } else {
        phi_f0 = 0.0f;
    }

    sEffSpawnFuncs[type](thisv, globalCtx, phi_f0, D_808917D0[type]);

    if (thisv->alpha <= 0) {
        if (!((thisv->dyna.actor.params >> 6) & 1)) {
            Flags_SetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
        }

        if (type == 4) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }

        Actor_Kill(&thisv->dyna.actor);
    }
}

void BgIceShelter_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgIceShelter* thisv = (BgIceShelter*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgIceShelter_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BgIceShelter* thisv = (BgIceShelter*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_ice_shelter.c", 748);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_ice_shelter.c", 751),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch ((thisv->dyna.actor.params >> 8) & 7) {
        case 0:
        case 1:
        case 2:
        case 4:
            func_8002ED80(&thisv->dyna.actor, globalCtx, 0);
            break;
    }

    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, thisv->alpha);

    switch ((thisv->dyna.actor.params >> 8) & 7) {
        case 0:
        case 1:
        case 4:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -globalCtx->gameplayFrames & 0x7F,
                                        -globalCtx->gameplayFrames & 0x7F, 0x20, 0x20, 1,
                                        -globalCtx->gameplayFrames & 0x7F, globalCtx->gameplayFrames & 0x7F, 0x20,
                                        0x20));
            gSPDisplayList(POLY_XLU_DISP++, object_ice_objects_DL_0006F0);
            break;

        case 2:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, globalCtx->gameplayFrames & 0xFF, 0x40, 0x40, 1,
                                        0, -globalCtx->gameplayFrames & 0xFF, 0x40, 0x40));
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -globalCtx->gameplayFrames & 0xFF,
                                        globalCtx->gameplayFrames & 0xFF, 0x40, 0x40, 1,
                                        globalCtx->gameplayFrames & 0xFF, globalCtx->gameplayFrames & 0xFF, 0x40,
                                        0x40));
            gSPDisplayList(POLY_XLU_DISP++, object_ice_objects_DL_0012A0);
            break;

        case 3:
            gSPDisplayList(POLY_XLU_DISP++, object_ice_objects_DL_002640);
            break;
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_ice_shelter.c", 815);
}
