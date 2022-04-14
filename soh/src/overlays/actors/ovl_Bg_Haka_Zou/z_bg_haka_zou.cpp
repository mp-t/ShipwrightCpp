/*
 * File: z_bg_haka_zou.c
 * Overlay: ovl_Bg_Haka_Zou
 * Description: Statue and Wall (Shadow Temple)
 */

#include "z_bg_haka_zou.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x0 */ STA_GIANT_BIRD_STATUE,
    /* 0x1 */ STA_BOMBABLE_SKULL_WALL,
    /* 0x2 */ STA_BOMBABLE_RUBBLE,
    /* 0x3 */ STA_UNKNOWN
} ShadowTempleAssetsType;

void BgHakaZou_Init(Actor* thisx, GlobalContext* globalCtx);
void BgHakaZou_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgHakaZou_Update(Actor* thisx, GlobalContext* globalCtx);
void BgHakaZou_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgHakaZou_Wait(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80882BDC(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80883000(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80883104(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80883144(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80883254(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_80883328(BgHakaZou* thisv, GlobalContext* globalCtx);
void func_808834D8(BgHakaZou* thisv, GlobalContext* globalCtx);
void BgHakaZou_DoNothing(BgHakaZou* thisv, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 5, 60, 0, { 0, 0, 0 } },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

const ActorInit Bg_Haka_Zou_InitVars = {
    ACTOR_BG_HAKA_ZOU,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHakaZou),
    (ActorFunc)BgHakaZou_Init,
    (ActorFunc)BgHakaZou_Destroy,
    (ActorFunc)BgHakaZou_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaZou_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgHakaZou* thisv = (BgHakaZou*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);

    thisv->switchFlag = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    if (thisx->params == STA_UNKNOWN) {
        Actor_SetScale(thisx, (Rand_ZeroOne() * 0.005f) + 0.025f);

        thisx->speedXZ = Rand_ZeroOne();
        thisx->world.rot.y = thisx->shape.rot.y * ((Rand_ZeroOne() < 0.5f) ? -1 : 1) + Rand_CenteredFloat(0x1000);
        thisv->timer = 20;
        thisx->world.rot.x = Rand_S16Offset(0x100, 0x300) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
        thisx->world.rot.z = Rand_S16Offset(0x400, 0x800) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
    } else {
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sCylinderInit);
        Collider_UpdateCylinder(thisx, &thisv->collider);

        DynaPolyActor_Init(&thisv->dyna, 0);

        if (thisx->params == STA_GIANT_BIRD_STATUE) {
            thisx->uncullZoneForward = 2000.0f;
            thisx->uncullZoneScale = 3000.0f;
            thisx->uncullZoneDownward = 3000.0f;
        }
    }

    thisv->requiredObjBankIndex = (thisx->params == STA_BOMBABLE_RUBBLE)
                                     ? Object_GetIndex(&globalCtx->objectCtx, OBJECT_HAKACH_OBJECTS)
                                     : Object_GetIndex(&globalCtx->objectCtx, OBJECT_HAKA_OBJECTS);

    if (thisv->requiredObjBankIndex < 0) {
        Actor_Kill(thisx);
    } else if ((thisx->params != STA_UNKNOWN) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        if (thisx->params != STA_GIANT_BIRD_STATUE) {
            Actor_Kill(thisx);
        } else {
            thisx->shape.rot.x = -0x4000;
            thisx->world.pos.z -= 80.0f;
            thisx->world.pos.y -= 54.0f;
        }
    }

    thisv->actionFunc = BgHakaZou_Wait;
}

void BgHakaZou_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaZou* thisv = (BgHakaZou*)thisx;

    if (thisv->dyna.actor.params != STA_UNKNOWN) {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void func_808828F4(BgHakaZou* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;
    Vec3f effectVelocity;
    f32 rand;
    s32 i;

    effectVelocity.x = 0.0f;
    effectVelocity.y = 1.0f;
    effectVelocity.z = 2.0f;

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            effectPos.x = thisv->dyna.actor.world.pos.x - (Rand_CenteredFloat(10.0f) + 112.0f);
        } else {
            effectPos.x = Rand_CenteredFloat(10.0f) + thisv->dyna.actor.world.pos.x;
        }

        rand = Rand_ZeroOne();
        effectPos.y = thisv->dyna.actor.world.pos.y + (60.0f * rand);
        effectPos.z = thisv->dyna.actor.world.pos.z + (112.0f * rand);

        func_800286CC(globalCtx, &effectPos, &effectVelocity, &sZeroVec, (Rand_ZeroOne() * 200.0f) + 1000.0f, 100);
    }
}

void BgHakaZou_Wait(BgHakaZou* thisv, GlobalContext* globalCtx) {
    const CollisionHeader* colHeader;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->requiredObjBankIndex)) {
        thisv->dyna.actor.objBankIndex = thisv->requiredObjBankIndex;
        thisv->dyna.actor.draw = BgHakaZou_Draw;

        if (thisv->dyna.actor.params == STA_UNKNOWN) {
            thisv->actionFunc = func_80882BDC;
        } else {
            Actor_SetObjectDependency(globalCtx, &thisv->dyna.actor);

            colHeader = NULL;

            if (thisv->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
                CollisionHeader_GetVirtual(&object_haka_objects_Col_006F70, &colHeader);
                thisv->collider.dim.radius = 80;
                thisv->collider.dim.height = 100;
                thisv->collider.dim.yShift = -30;
                thisv->collider.dim.pos.x -= 56;
                thisv->collider.dim.pos.z += 56;
                thisv->dyna.actor.uncullZoneScale = 1500.0f;
            } else if (thisv->dyna.actor.params == STA_BOMBABLE_SKULL_WALL) {
                CollisionHeader_GetVirtual(&object_haka_objects_Col_005E30, &colHeader);
                thisv->collider.dim.yShift = -50;
            } else {
                CollisionHeader_GetVirtual(&gBotwBombSpotCol, &colHeader);
                thisv->collider.dim.radius = 55;
                thisv->collider.dim.height = 20;
            }

            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

            if ((thisv->dyna.actor.params == STA_GIANT_BIRD_STATUE) && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
                thisv->actionFunc = BgHakaZou_DoNothing;
            } else {
                thisv->actionFunc = func_80883000;
            }
        }
    }
}
void func_80882BDC(BgHakaZou* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->dyna.actor.shape.rot.x += thisv->dyna.actor.world.rot.x;
    thisv->dyna.actor.shape.rot.z += thisv->dyna.actor.world.rot.z;

    if (thisv->dyna.actor.bgCheckFlags & 2) {
        if (thisv->dyna.actor.velocity.y < -8.0f) {
            thisv->dyna.actor.velocity.y *= -0.6f;
            thisv->dyna.actor.velocity.y = CLAMP_MAX(thisv->dyna.actor.velocity.y, 10.0f);
            thisv->dyna.actor.bgCheckFlags &= ~3;
            thisv->dyna.actor.speedXZ = 2.0f;
        } else {
            Actor_Kill(&thisv->dyna.actor);
        }
    }

    if (thisv->timer == 0) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void func_80882CC4(BgHakaZou* thisv, GlobalContext* globalCtx) {
    s32 i;
    s32 j;
    Vec3f actorSpawnPos;
    f32 sin;
    f32 cos;
    s32 pad;

    sin = Math_SinS(thisv->dyna.actor.shape.rot.y - 0x4000) * 40.0f;
    cos = Math_CosS(thisv->dyna.actor.shape.rot.y - 0x4000) * 40.0f;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            actorSpawnPos.x = thisv->dyna.actor.world.pos.x + (j - 1) * sin;
            actorSpawnPos.z = thisv->dyna.actor.world.pos.z + (j - 1) * cos;
            actorSpawnPos.y = thisv->dyna.actor.world.pos.y + (i - 1) * 55;

            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_HAKA_ZOU, actorSpawnPos.x, actorSpawnPos.y,
                        actorSpawnPos.z, 0, thisv->dyna.actor.shape.rot.y, 0, thisv->dyna.actor.params + 2);
            func_800286CC(globalCtx, &actorSpawnPos, &sZeroVec, &sZeroVec, 1000, 50);
        }
    }
}

void func_80882E54(BgHakaZou* thisv, GlobalContext* globalCtx) {
    Vec3f fragmentPos;
    s32 i;
    s32 j;
    s32 num = 25;

    fragmentPos.x = thisv->collider.dim.pos.x;
    fragmentPos.y = thisv->collider.dim.pos.y;
    fragmentPos.z = thisv->collider.dim.pos.z;

    EffectSsHahen_SpawnBurst(globalCtx, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            fragmentPos.x = thisv->collider.dim.pos.x + (((j * 2) - 1) * num);
            fragmentPos.z = thisv->collider.dim.pos.z + (((i * 2) - 1) * num);
            EffectSsHahen_SpawnBurst(globalCtx, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);
            func_800286CC(globalCtx, &fragmentPos, &sZeroVec, &sZeroVec, 1000, 50);
        }
    }
}

void func_80883000(BgHakaZou* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        Flags_SetSwitch(globalCtx, thisv->switchFlag);

        if (thisv->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
            thisv->timer = 20;
            thisv->actionFunc = func_80883144;
            OnePointCutscene_Init(globalCtx, 3400, 999, &thisv->dyna.actor, MAIN_CAM);
        } else if (thisv->dyna.actor.params == 2) {
            func_80882E54(thisv, globalCtx);
            thisv->dyna.actor.draw = NULL;
            thisv->timer = 1;
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_EXPLOSION);
            thisv->actionFunc = func_80883104;
        } else {
            func_80882CC4(thisv, globalCtx);
            thisv->timer = 1;
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_WALL_BROKEN);
            thisv->actionFunc = func_80883104;
        }
    } else {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void func_80883104(BgHakaZou* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (thisv->timer == 0) {
        Actor_Kill(&thisv->dyna.actor);
    }
}

void func_80883144(BgHakaZou* thisv, GlobalContext* globalCtx) {
    Vec3f explosionPos;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    if (!(thisv->timer % 4)) {
        explosionPos.x = Rand_CenteredFloat(200.0f) + (thisv->dyna.actor.world.pos.x - 56.0f);
        explosionPos.y = (Rand_ZeroOne() * 80.0f) + thisv->dyna.actor.world.pos.y;
        explosionPos.z = Rand_CenteredFloat(200.0f) + (thisv->dyna.actor.world.pos.z + 56.0f);

        EffectSsBomb2_SpawnLayered(globalCtx, &explosionPos, &sZeroVec, &sZeroVec, 150, 70);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (thisv->timer == 0) {
        thisv->timer = 20;
        thisv->actionFunc = func_80883254;
    }
}

void func_80883254(BgHakaZou* thisv, GlobalContext* globalCtx) {
    f32 moveDist = (Rand_ZeroOne() * 0.5f) + 0.5f;

    Math_StepToF(&thisv->dyna.actor.world.pos.z, thisv->dyna.actor.home.pos.z - 80.0f, 2.0f * moveDist);

    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y - 40.0f, moveDist)) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }

        if (thisv->timer == 0) {
            thisv->timer = 60;
            thisv->dyna.actor.world.rot.x = 8;
            thisv->actionFunc = func_80883328;
        }
    } else {
        func_808828F4(thisv, globalCtx);
    }
}

void func_80883328(BgHakaZou* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;
    s32 i;
    s32 j;

    thisv->dyna.actor.world.rot.x += thisv->dyna.actor.world.rot.x / 8.0f;

    if (Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.x, -0x4000, thisv->dyna.actor.world.rot.x)) {
        effectPos.x = thisv->dyna.actor.world.pos.x;
        effectPos.y = thisv->dyna.actor.world.pos.y;

        for (j = 0; j < 2; j++) {
            effectPos.z = thisv->dyna.actor.world.pos.z;

            for (i = 0; i < 4; i++) {
                effectPos.z -= (i == 2) ? 550.0f : 50.0f;
                func_800286CC(globalCtx, &effectPos, &sZeroVec, &sZeroVec, (Rand_ZeroOne() * 200.0f) + 1000.0f, 200);
            }

            effectPos.x -= 112.0f;
        }

        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_STONE_BOUND);
        thisv->timer = 25;
        thisv->actionFunc = func_808834D8;
    }
}

void func_808834D8(BgHakaZou* thisv, GlobalContext* globalCtx) {
    f32 moveDist;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    moveDist = (thisv->timer % 2) ? 15.0f : -15.0f;
    thisv->dyna.actor.world.pos.y += ((thisv->timer & 0xFE) * 0.04f * moveDist);

    if (thisv->timer == 0) {
        thisv->actionFunc = BgHakaZou_DoNothing;
    }
}

void BgHakaZou_DoNothing(BgHakaZou* thisv, GlobalContext* globalCtx) {
}

void BgHakaZou_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgHakaZou* thisv = (BgHakaZou*)thisx;

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->dyna.actor.params == 3) {
        Actor_MoveForward(&thisv->dyna.actor);
    }
}

void BgHakaZou_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static const Gfx* dLists[] = {
        object_haka_objects_DL_0064E0,
        object_haka_objects_DL_005CE0,
        gBotwBombSpotDL,
        object_haka_objects_DL_005CE0,
    };

    Gfx_DrawDListOpa(globalCtx, dLists[thisx->params]);
}
