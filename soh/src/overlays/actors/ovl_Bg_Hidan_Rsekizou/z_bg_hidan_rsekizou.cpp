/*
 * File: z_bg_hidan_rsekizou.c
 * Overlay: ovl_Bg_Hidan_Rsekizou
 * Description: Spinning Stone flamethrower
 */

#include "z_bg_hidan_rsekizou.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanRsekizou_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRsekizou_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRsekizou_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHidanRsekizou_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit Bg_Hidan_Rsekizou_InitVars = {
    ACTOR_BG_HIDAN_RSEKIZOU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanRsekizou),
    (ActorFunc)BgHidanRsekizou_Init,
    (ActorFunc)BgHidanRsekizou_Destroy,
    (ActorFunc)BgHidanRsekizou_Update,
    (ActorFunc)BgHidanRsekizou_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[6] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 30, 40 }, 25 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { -35, 32, 77 }, 32 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { -80, 35, 130 }, 42 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 30, -40 }, 25 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 35, 32, -77 }, 32 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 80, 35, -130 }, 42 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    6,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1500, ICHAIN_STOP),
};

static const void* sFireballsTexs[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void BgHidanRsekizou_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRsekizou* thisv = (BgHidanRsekizou*)thisx;
    s32 i;
    s32 pad;
    const CollisionHeader* colHeader;

    colHeader = NULL;
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gFireTempleSpinningFlamethrowerCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->dyna.actor, &sJntSphInit, thisv->colliderItems);
    for (i = 0; i < ARRAY_COUNT(thisv->colliderItems); i++) {
        thisv->collider.elements[i].dim.worldSphere.radius = thisv->collider.elements[i].dim.modelSphere.radius;
    }
    thisv->burnFrame = 0;
    thisv->bendFrame = 0;
}

void BgHidanRsekizou_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRsekizou* thisv = (BgHidanRsekizou*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BgHidanRsekizou_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRsekizou* thisv = (BgHidanRsekizou*)thisx;
    s32 i;
    ColliderJntSphElement* sphere;
    s32 pad;
    f32 yawSine;
    f32 yawCosine;

    thisv->burnFrame = (thisv->burnFrame + 1) % 8;

    if (thisv->bendFrame != 0) {
        thisv->bendFrame--;
    }

    if (thisv->bendFrame == 0) {
        thisv->bendFrame = 3;
    }

    thisv->dyna.actor.shape.rot.y += 0x180; // Approximately 2 Degrees per Frame
    yawSine = Math_SinS(thisv->dyna.actor.shape.rot.y);
    yawCosine = Math_CosS(thisv->dyna.actor.shape.rot.y);

    for (i = 0; i < ARRAY_COUNT(thisv->colliderItems); i++) {
        sphere = &thisv->collider.elements[i];
        sphere->dim.worldSphere.center.x = thisv->dyna.actor.home.pos.x + yawCosine * sphere->dim.modelSphere.center.x +
                                           yawSine * sphere->dim.modelSphere.center.z;
        sphere->dim.worldSphere.center.y = (s16)thisv->dyna.actor.home.pos.y + sphere->dim.modelSphere.center.y;
        sphere->dim.worldSphere.center.z = (thisv->dyna.actor.home.pos.z - yawSine * sphere->dim.modelSphere.center.x) +
                                           yawCosine * sphere->dim.modelSphere.center.z;
    }

    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    func_8002F974(&thisv->dyna.actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
}

Gfx* BgHidanRsekizou_DrawFireball(GlobalContext* globalCtx, BgHidanRsekizou* thisv, s16 frame, MtxF* mf, s32 a,
                                  Gfx* displayList) {
    f32 coss;
    f32 sins;
    s32 temp;
    f32 fVar6;
    f32 tmpf7;

    temp = (((thisv->burnFrame + frame) % 8) * 7) * (1.0f / 7.0f);
    gSPSegment(displayList++, 0x09, SEGMENTED_TO_VIRTUAL(sFireballsTexs[temp]));

    frame++;
    fVar6 = (frame != 4) ? frame + ((3 - thisv->bendFrame) * (1.0f / 3.0f)) : frame;

    gDPSetPrimColor(displayList++, 0, 1, 255, 255, 0, 150);
    gDPSetEnvColor(displayList++, 255, 0, 0, 255);

    if (a == 0) {
        sins = -Math_SinS(thisv->dyna.actor.shape.rot.y - (frame * 1500));
        coss = -Math_CosS(thisv->dyna.actor.shape.rot.y - (frame * 1500));
    } else {
        sins = Math_SinS(thisv->dyna.actor.shape.rot.y - (frame * 1500));
        coss = Math_CosS(thisv->dyna.actor.shape.rot.y - (frame * 1500));
    }

    mf->mf_raw.xx = mf->mf_raw.yy = mf->mf_raw.zz = (0.7f * fVar6) + 0.5f;
    tmpf7 = (((((0.7f * fVar6) + 0.5f) * 10.0f) * fVar6) + 20.0f);

    mf->mf_raw.xw = (tmpf7 * sins) + thisv->dyna.actor.world.pos.x;
    mf->mf_raw.yw = (thisv->dyna.actor.world.pos.y + 30.0f) + ((7.0f / 10.0f) * fVar6);
    mf->mf_raw.zw = (tmpf7 * coss) + thisv->dyna.actor.world.pos.z;

    gSPMatrix(displayList++,
              Matrix_MtxFToMtx(Matrix_CheckFloats(mf, "../z_bg_hidan_rsekizou.c", 543),
                               static_cast<Mtx*>(Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx)))),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(displayList++, gFireTempleFireballDL);

    return displayList;
}

void BgHidanRsekizou_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgHidanRsekizou* thisv = (BgHidanRsekizou*)thisx;
    s32 i;
    s32 pad;
    MtxF mf;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_rsekizou.c", 564);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_hidan_rsekizou.c", 568),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFireTempleSpinningFlamethrowerDL);
    Matrix_MtxFCopy(&mf, &gMtxFClear);

    POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);

    if ((s16)((Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisv->dyna.actor.shape.rot.y) - 0x2E6C) >= 0) {
        for (i = 3; i >= 0; i--) {
            POLY_XLU_DISP = BgHidanRsekizou_DrawFireball(globalCtx, thisv, i, &mf, 0, POLY_XLU_DISP);
        }

        for (i = 0; i < 4; i++) {
            POLY_XLU_DISP = BgHidanRsekizou_DrawFireball(globalCtx, thisv, i, &mf, 1, POLY_XLU_DISP);
        }
    } else {
        for (i = 3; i >= 0; i--) {
            POLY_XLU_DISP = BgHidanRsekizou_DrawFireball(globalCtx, thisv, i, &mf, 1, POLY_XLU_DISP);
        }

        for (i = 0; i < 4; i++) {
            POLY_XLU_DISP = BgHidanRsekizou_DrawFireball(globalCtx, thisv, i, &mf, 0, POLY_XLU_DISP);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_hidan_rsekizou.c", 600);
}
