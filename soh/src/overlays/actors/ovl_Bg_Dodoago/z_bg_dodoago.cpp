/*
 * File: z_bg_dodoago.c
 * Overlay: ovl_Bg_Dodoago
 * Description: Dodongo Head Statue in Dodongo's Cavern
 */

#include "z_bg_dodoago.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_ddan_objects/object_ddan_objects.h"

#define FLAGS 0

void BgDodoago_Init(Actor* thisx, GlobalContext* globalCtx);
void BgDodoago_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgDodoago_Update(Actor* thisx, GlobalContext* globalCtx);
void BgDodoago_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgDodoago_Reset(void);

void BgDodoago_WaitExplosives(BgDodoago* thisv, GlobalContext* globalCtx);
void BgDodoago_OpenJaw(BgDodoago* thisv, GlobalContext* globalCtx);
void BgDodoago_DoNothing(BgDodoago* thisv, GlobalContext* globalCtx);
void BgDodoago_LightOneEye(BgDodoago* thisv, GlobalContext* globalCtx);

ActorInit Bg_Dodoago_InitVars = {
    ACTOR_BG_DODOAGO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_DDAN_OBJECTS,
    sizeof(BgDodoago),
    (ActorFunc)BgDodoago_Init,
    (ActorFunc)BgDodoago_Destroy,
    (ActorFunc)BgDodoago_Update,
    (ActorFunc)BgDodoago_Draw,
    (ActorResetFunc)BgDodoago_Reset,
};

static ColliderCylinderInit sColCylinderInitMain = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 80, 30, 80, { 0, 0, 0 } },
};

static ColliderCylinderInit sColCylinderInitLeftRight = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 50, 60, 280, { 0, 0, 0 } },
};

static s16 sFirstExplosiveFlag = false;

static u8 sDisableBombCatcher;

static u8 sUnused[90]; // unknown length

static s32 sTimer;

void BgDodoago_SetupAction(BgDodoago* thisv, BgDodoagoActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void BgDodoago_SpawnSparkles(Vec3f* meanPos, GlobalContext* globalCtx) {
    Vec3f pos;
    Color_RGBA8 primColor = { 100, 100, 100, 0 };
    Color_RGBA8 envColor = { 40, 40, 40, 0 };
    static Vec3f velocity = { 0.0f, -1.5f, 0.0f };
    static Vec3f acceleration = { 0.0f, -0.2f, 0.0f };
    s32 i;

    for (i = 4; i > 0; i--) {
        pos.x = Rand_CenteredFloat(20.0f) + meanPos->x;
        pos.y = Rand_CenteredFloat(10.0f) + meanPos->y;
        pos.z = Rand_CenteredFloat(20.0f) + meanPos->z;
        EffectSsKiraKira_SpawnSmall(globalCtx, &pos, &velocity, &acceleration, &primColor, &envColor);
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 5000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

void BgDodoago_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgDodoago* thisv = (BgDodoago*)thisx;
    s32 pad;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gDodongoLowerJawCol, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, NULL, 0.0f);

    if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
        BgDodoago_SetupAction(thisv, BgDodoago_DoNothing);
        thisv->dyna.actor.shape.rot.x = 0x1333;
        globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_LEFT] = globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_RIGHT] = 255;
        return;
    }

    Collider_InitCylinder(globalCtx, &thisv->colliderMain);
    Collider_InitCylinder(globalCtx, &thisv->colliderLeft);
    Collider_InitCylinder(globalCtx, &thisv->colliderRight);
    Collider_SetCylinder(globalCtx, &thisv->colliderMain, &thisv->dyna.actor, &sColCylinderInitMain);
    Collider_SetCylinder(globalCtx, &thisv->colliderLeft, &thisv->dyna.actor, &sColCylinderInitLeftRight);
    Collider_SetCylinder(globalCtx, &thisv->colliderRight, &thisv->dyna.actor, &sColCylinderInitLeftRight);

    BgDodoago_SetupAction(thisv, BgDodoago_WaitExplosives);
    sDisableBombCatcher = false;
}

void BgDodoago_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgDodoago* thisv = (BgDodoago*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderMain);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderLeft);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderRight);
}

void BgDodoago_WaitExplosives(BgDodoago* thisv, GlobalContext* globalCtx) {
    Actor* explosive = Actor_GetCollidedExplosive(globalCtx, &thisv->colliderMain.base);

    if (explosive != NULL) {
        thisv->state =
            (Math_Vec3f_Yaw(&thisv->dyna.actor.world.pos, &explosive->world.pos) >= thisv->dyna.actor.shape.rot.y)
                ? BGDODOAGO_EYE_RIGHT
                : BGDODOAGO_EYE_LEFT;

        if (((globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_LEFT] == 255) && (thisv->state == BGDODOAGO_EYE_RIGHT)) ||
            ((globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_RIGHT] == 255) && (thisv->state == BGDODOAGO_EYE_LEFT))) {
            Flags_SetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
            thisv->state = 0;
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            BgDodoago_SetupAction(thisv, BgDodoago_OpenJaw);
            OnePointCutscene_Init(globalCtx, 3380, 160, &thisv->dyna.actor, MAIN_CAM);
        } else if (globalCtx->roomCtx.unk_74[thisv->state] == 0) {
            OnePointCutscene_Init(globalCtx, 3065, 40, &thisv->dyna.actor, MAIN_CAM);
            BgDodoago_SetupAction(thisv, BgDodoago_LightOneEye);
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            OnePointCutscene_Init(globalCtx, 3065, 20, &thisv->dyna.actor, MAIN_CAM);
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            sTimer += 30;
            return;
        }

        // the flag is never set back to false, so thisv only runs once
        if (!sFirstExplosiveFlag) {
            // thisv disables the bomb catcher (see BgDodoago_Update) for a few seconds
            thisv->dyna.actor.parent = explosive;
            sFirstExplosiveFlag = true;
            sTimer = 50;
        }
    } else if (Flags_GetEventChkInf(0xB0)) {
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->colliderMain);
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->colliderLeft);
        Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->colliderRight);

        thisv->colliderMain.dim.pos.z += 200;

        thisv->colliderLeft.dim.pos.z += 215;
        thisv->colliderLeft.dim.pos.x += 90;

        thisv->colliderRight.dim.pos.z += 215;
        thisv->colliderRight.dim.pos.x -= 90;

        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderMain.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderLeft.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderRight.base);
    }
}

void BgDodoago_OpenJaw(BgDodoago* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f dustOffsets[] = {
        { 0.0f, -200.0f, 430.0f },   { 20.0f, -200.0f, 420.0f }, { -20.0f, -200.0f, 420.0f },
        { 40.0, -200.0f, 380.0f },   { -40.0, -200.0f, 380.0f }, { 50.0, -200.0f, 350.0f },
        { -50.0f, -200.0f, 350.0f }, { 60.0f, -200.0f, 320.0f }, { -60.0f, -200.0f, 320.0f },
        { 70.0f, -200.0f, 290.0f },  { -70.0, -200.0f, 290.0f },
    };
    s32 i;

    // make both eyes red (one already is)
    if (globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_LEFT] < 255) {
        globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_LEFT] += 5;
    }
    if (globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_RIGHT] < 255) {
        globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_RIGHT] += 5;
    }

    if (globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_LEFT] != 255 || globalCtx->roomCtx.unk_74[BGDODOAGO_EYE_RIGHT] != 255) {
        sTimer--;
        return;
    }

    if (sTimer == 108) {
        for (i = ARRAY_COUNT(dustOffsets) - 1; i >= 0; i--) {
            pos.x = dustOffsets[i].x + thisv->dyna.actor.world.pos.x;
            pos.y = dustOffsets[i].y + thisv->dyna.actor.world.pos.y;
            pos.z = dustOffsets[i].z + thisv->dyna.actor.world.pos.z;
            func_80033480(globalCtx, &pos, 2.0f, 3, 200, 75, 1);
        }
    }

    pos.x = thisv->dyna.actor.world.pos.x + 200.0f;
    pos.y = thisv->dyna.actor.world.pos.y - 20.0f;
    pos.z = thisv->dyna.actor.world.pos.z + 100.0f;
    BgDodoago_SpawnSparkles(&pos, globalCtx);

    pos.x = thisv->dyna.actor.world.pos.x - 200.0f;
    pos.y = thisv->dyna.actor.world.pos.y - 20.0f;
    pos.z = thisv->dyna.actor.world.pos.z + 100.0f;
    BgDodoago_SpawnSparkles(&pos, globalCtx);

    Math_StepToS(&thisv->state, 100, 3);
    func_800AA000(500.0f, 0x78, 0x14, 0xA);

    if (Math_SmoothStepToS(&thisv->dyna.actor.shape.rot.x, 0x1333, 110 - thisv->state, 0x3E8, 0x32) == 0) {
        BgDodoago_SetupAction(thisv, BgDodoago_DoNothing);
        Audio_PlaySoundGeneral(NA_SE_EV_STONE_BOUND, &thisv->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else {
        Audio_PlaySoundGeneral(NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG, &thisv->dyna.actor.projectedPos, 4, &D_801333E0,
                               &D_801333E0, &D_801333E8);
    }
}

void BgDodoago_DoNothing(BgDodoago* thisv, GlobalContext* globalCtx) {
}

void BgDodoago_LightOneEye(BgDodoago* thisv, GlobalContext* globalCtx) {
    globalCtx->roomCtx.unk_74[thisv->state] += 5;

    if (globalCtx->roomCtx.unk_74[thisv->state] == 255) {
        BgDodoago_SetupAction(thisv, BgDodoago_WaitExplosives);
    }
}

void BgDodoago_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgDodoago* thisv = (BgDodoago*)thisx;
    Actor* actor;
    EnBom* bomb;

    if (thisv->dyna.actor.parent == NULL) {
        // thisv is a "bomb catcher", it kills the XZ speed and sets the timer for bombs that are dropped through the
        // holes in the bridge above the skull
        if ((thisv->colliderLeft.base.ocFlags1 & OC1_HIT) || (thisv->colliderRight.base.ocFlags1 & OC1_HIT)) {

            if (thisv->colliderLeft.base.ocFlags1 & OC1_HIT) {
                actor = thisv->colliderLeft.base.oc;
            } else {
                actor = thisv->colliderRight.base.oc;
            }
            thisv->colliderLeft.base.ocFlags1 &= ~OC1_HIT;
            thisv->colliderRight.base.ocFlags1 &= ~OC1_HIT;

            if (actor->category == ACTORCAT_EXPLOSIVE && actor->id == ACTOR_EN_BOM && actor->params == 0) {
                bomb = (EnBom*)actor;
                // disable the bomb catcher for a few seconds
                thisv->dyna.actor.parent = &bomb->actor;
                bomb->timer = 50;
                bomb->actor.speedXZ = 0.0f;
                sTimer = 0;
            }
        }
    } else {
        sTimer++;
        Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F);
        if (!sDisableBombCatcher && sTimer > 140) {
            if (Flags_GetSwitch(globalCtx, thisv->dyna.actor.params & 0x3F)) {
                // thisv prevents clearing the actor's parent pointer, effectively disabling the bomb catcher
                sDisableBombCatcher++;
            } else {
                thisv->dyna.actor.parent = NULL;
            }
        }
    }
    thisv->actionFunc(thisv, globalCtx);
}

void BgDodoago_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_dodoago.c", 672);

    if (Flags_GetEventChkInf(0xB0)) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_dodoago.c", 677),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDodongoLowerJawDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_dodoago.c", 681);
}

void BgDodoago_Reset(void) {
    sFirstExplosiveFlag = false;
    sDisableBombCatcher = 0;
    sTimer = 0;
}