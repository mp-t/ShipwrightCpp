/*
 * File: z_bg_bdan_objects.c
 * Overlay: ovl_Bg_Bdan_Objects
 * Description: Lord Jabu-Jabu Objects
 */

#include "z_bg_bdan_objects.h"
#include "objects/object_bdan_objects/object_bdan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgBdanObjects_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgBdanObjects_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgBdanObjects_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgBdanObjects_Draw(Actor* thisvx, GlobalContext* globalCtx);

void func_8086C054(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C1A0(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C29C(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C55C(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C5BC(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C618(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C6EC(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C76C(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C7D0(BgBdanObjects* thisv, GlobalContext* globalCtx);
void BgBdanObjects_DoNothing(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C874(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C9A8(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086C9F0(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086CABC(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086CB10(BgBdanObjects* thisv, GlobalContext* globalCtx);
void func_8086CB8C(BgBdanObjects* thisv, GlobalContext* globalCtx);

ActorInit Bg_Bdan_Objects_InitVars = {
    ACTOR_BG_BDAN_OBJECTS,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_BDAN_OBJECTS,
    sizeof(BgBdanObjects),
    (ActorFunc)BgBdanObjects_Init,
    (ActorFunc)BgBdanObjects_Destroy,
    (ActorFunc)BgBdanObjects_Update,
    (ActorFunc)BgBdanObjects_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 0x00BB, 0x0050, 0x0000, { 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static const Gfx* sDLists[] = {
    gJabuObjectsLargeRotatingSpikePlatformDL,
    gJabuElevatorPlatformDL,
    gJabuWaterDL,
    gJabuFallingPlatformDL,
};

s32 BgBdanObjects_GetContactRu1(BgBdanObjects* thisv, s32 arg1) {
    switch (arg1) {
        case 0:
            return thisv->cameraSetting == CAM_SET_NORMAL0;
        case 4:
            return gSaveContext.infTable[20] & 0x40;
        case 3:
            return thisv->cameraSetting == CAM_SET_DUNGEON1;
        default:
            osSyncPrintf("Bg_Bdan_Objects_Get_Contact_Ru1\nそんな受信モードは無い%d!!!!!!!!\n");
            return -1;
    }
}

void BgBdanObjects_SetContactRu1(BgBdanObjects* thisv, s32 arg1) {
    switch (arg1) {
        case 1:
            thisv->cameraSetting = CAM_SET_NORMAL1;
            break;
        case 2:
            thisv->cameraSetting = CAM_SET_DUNGEON0;
            break;
        case 4:
            gSaveContext.infTable[20] |= 0x40;
            break;
        default:
            osSyncPrintf("Bg_Bdan_Objects_Set_Contact_Ru1\nそんな送信モードは無い%d!!!!!!!!\n");
    }
}

void BgBdanObjects_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 pad;
    BgBdanObjects* thisv = (BgBdanObjects*)thisvx;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    DynaPolyActor_Init(&thisv->dyna, DPM_PLAYER);
    thisv->switchFlag = (thisvx->params >> 8) & 0x3F;
    thisvx->params &= 0xFF;
    if (thisvx->params == 2) {
        thisvx->flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
        globalCtx->colCtx.colHeader->waterBoxes[7].ySurface = thisvx->world.pos.y;
        thisv->actionFunc = func_8086C9A8;
        return;
    }
    if (thisvx->params == 0) {
        CollisionHeader_GetVirtual(&gJabuBigOctoPlatformCol, &colHeader);
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->dyna.actor, &sCylinderInit);
        thisvx->world.pos.y += -79.0f;
        if (Flags_GetClear(globalCtx, thisvx->room)) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
            thisv->actionFunc = func_8086C6EC;
        } else {
            if (BgBdanObjects_GetContactRu1(thisv, 4)) {
                if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_BIGOKUTA,
                                       thisvx->home.pos.x, thisvx->home.pos.y, thisvx->home.pos.z, 0,
                                       thisvx->shape.rot.y + 0x8000, 0, 3) != NULL) {
                    thisvx->child->world.pos.z = thisvx->child->home.pos.z + 263.0f;
                }
                thisvx->world.rot.y = 0;
                thisv->actionFunc = func_8086C618;
                thisvx->world.pos.y = thisvx->home.pos.y + -70.0f;
            } else {
                Flags_SetSwitch(globalCtx, thisv->switchFlag);
                thisv->timer = 0;
                thisv->actionFunc = func_8086C054;
            }
        }
    } else {
        if (thisvx->params == 1) {
            CollisionHeader_GetVirtual(&gJabuElevatorCol, &colHeader);
            thisv->timer = 512;
            thisv->switchFlag = 0;
            thisv->actionFunc = func_8086C874;
        } else {
            CollisionHeader_GetVirtual(&gJabuLoweringPlatformCol, &colHeader);
            if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
                thisv->actionFunc = BgBdanObjects_DoNothing;
                thisvx->world.pos.y = thisvx->home.pos.y - 400.0f;
            } else {
                thisv->actionFunc = func_8086CB10;
            }
        }
    }
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisvx, colHeader);
}

void BgBdanObjects_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgBdanObjects* thisv = (BgBdanObjects*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    if (thisvx->params == 0) {
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void func_8086C054(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (BgBdanObjects_GetContactRu1(thisv, 0)) {
        if (thisv->dyna.actor.xzDistToPlayer < 250.0f) {
            BgBdanObjects_SetContactRu1(thisv, 1);
            thisv->timer = 20;
            OnePointCutscene_Init(globalCtx, 3070, -99, &thisv->dyna.actor, MAIN_CAM);
            player->actor.world.pos.x = -1130.0f;
            player->actor.world.pos.y = -1025.0f;
            player->actor.world.pos.z = -3300.0f;
            func_800AA000(0.0f, 0xFF, 0x14, 0x96);
        }
    } else if (thisv->timer != 0) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
        if (thisv->timer == 0) {
            thisv->actionFunc = func_8086C1A0;
        }
    }

    if (!Gameplay_InCsMode(globalCtx) && !BgBdanObjects_GetContactRu1(thisv, 0)) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + -79.0f;
    } else {
        thisv->dyna.actor.world.pos.y = (thisv->dyna.actor.home.pos.y + -79.0f) - 5.0f;
    }
}

void func_8086C1A0(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 500.0f, 0.5f, 7.5f, 1.0f) <
        0.1f) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_A);
        thisv->actionFunc = func_8086C29C;
        thisv->timer = 30;
        BgBdanObjects_SetContactRu1(thisv, 2);
        func_800AA000(0.0f, 0xFF, 0x14, 0x96);
    } else {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
        if (thisv->timer == 0) {
            func_800AA000(0.0f, 0x78, 0x14, 0xA);
            thisv->timer = 11;
        }
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_RISING - SFX_FLAG);
    }
}

void func_8086C29C(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    s32 temp;

    if (thisv->timer != 0) {
        thisv->timer--;
        if (thisv->timer == 0) {
            temp = Quake_Add(GET_ACTIVE_CAM(globalCtx), 1);
            Quake_SetSpeed(temp, 0x3A98);
            Quake_SetQuakeValues(temp, 0, 1, 0xFA, 1);
            Quake_SetCountdown(temp, 0xA);
        }
    }

    if (BgBdanObjects_GetContactRu1(thisv, 3)) {
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_BIGOKUTA,
                           thisv->dyna.actor.world.pos.x, thisv->dyna.actor.world.pos.y + 140.0f,
                           thisv->dyna.actor.world.pos.z, 0, thisv->dyna.actor.shape.rot.y + 0x8000, 0, 0);
        BgBdanObjects_SetContactRu1(thisv, 4);
        thisv->timer = 10;
        thisv->actionFunc = func_8086C55C;
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
    }
}

void func_8086C3D8(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->dyna.actor.velocity.y += 0.5f;
    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + -70.0f,
                     thisv->dyna.actor.velocity.y)) {
        thisv->dyna.actor.world.rot.y = 0;
        thisv->timer = 60;
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);
        thisv->dyna.actor.child->world.pos.y = thisv->dyna.actor.world.pos.y + 140.0f;
        thisv->actionFunc = func_8086C5BC;
        OnePointCutscene_Init(globalCtx, 3080, -99, thisv->dyna.actor.child, MAIN_CAM);
        player->actor.world.pos.x = -1130.0f;
        player->actor.world.pos.y = -1025.0f;
        player->actor.world.pos.z = -3500.0f;
        player->actor.shape.rot.y = 0x7530;
        player->actor.world.rot.y = player->actor.shape.rot.y;
        func_800AA000(0.0f, 0xFF, 0x1E, 0x96);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_FALL - SFX_FLAG);
        if (thisv->timer != 0) {
            thisv->timer--;
        }
        if (thisv->timer == 0) {
            func_800AA000(0.0f, 0x78, 0x14, 0xA);
            thisv->timer = 11;
        }
        if (thisv->dyna.actor.child != NULL) {
            thisv->dyna.actor.child->world.pos.y = thisv->dyna.actor.world.pos.y + 140.0f;
        }
    }
}

void func_8086C55C(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    thisv->timer--;

    if (thisv->timer == 0) {
        Flags_UnsetSwitch(globalCtx, thisv->switchFlag);
    } else if (thisv->timer == -40) {
        thisv->timer = 0;
        thisv->actionFunc = func_8086C3D8;
    }
}

void func_8086C5BC(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if ((thisv->timer == 0) && (thisv->dyna.actor.child != NULL)) {
        if (thisv->dyna.actor.child->params == 2) {
            thisv->actionFunc = func_8086C618;
        } else if (thisv->dyna.actor.child->params == 0) {
            thisv->dyna.actor.child->params = 1;
        }
    }
}

void func_8086C618(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    Collider_UpdateCylinder(&thisv->dyna.actor, &thisv->collider);
    CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    if (Flags_GetClear(globalCtx, thisv->dyna.actor.room)) {
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        thisv->dyna.actor.home.rot.y = (s16)(thisv->dyna.actor.shape.rot.y + 0x2000) & 0xC000;
        thisv->actionFunc = func_8086C6EC;
    } else {
        thisv->dyna.actor.shape.rot.y += thisv->dyna.actor.world.rot.y;
        func_800F436C(&thisv->dyna.actor.projectedPos, 0x2063, ABS(thisv->dyna.actor.world.rot.y) / 512.0f);
    }
}

void func_8086C6EC(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    s32 cond = Math_ScaledStepToS(&thisv->dyna.actor.shape.rot.y, thisv->dyna.actor.home.rot.y, 0x200);

    if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + -125.0f, 3.0f)) {
        if (cond) {
            thisv->actionFunc = func_8086C76C;
        }
    }
}

void func_8086C76C(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (func_8004356C(&thisv->dyna)) {
        if (thisv->dyna.actor.xzDistToPlayer < 120.0f) {
            thisv->actionFunc = func_8086C7D0;
            OnePointCutscene_Init(globalCtx, 3090, -99, &thisv->dyna.actor, MAIN_CAM);
        }
    }
}

void func_8086C7D0(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (Math_SmoothStepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 965.0f, 0.5f, 15.0f, 0.2f) <
        0.01f) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_A);
        thisv->actionFunc = BgBdanObjects_DoNothing;
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_RISING - SFX_FLAG);
    }
}

void BgBdanObjects_DoNothing(BgBdanObjects* thisv, GlobalContext* globalCtx) {
}

void func_8086C874(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->switchFlag == 0) {
        if (func_8004356C(&thisv->dyna)) {
            thisv->cameraSetting = globalCtx->cameraPtrs[MAIN_CAM]->setting;
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_NORMAL2);
            func_8005AD1C(globalCtx->cameraPtrs[MAIN_CAM], 4);
            thisv->switchFlag = 10;
        }
    } else {
        Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_NORMAL2);
        if (!func_8004356C(&thisv->dyna)) {
            if (thisv->switchFlag != 0) {
                thisv->switchFlag--;
            }
        }
        if (thisv->switchFlag == 0) {
            if (1) {}
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], thisv->cameraSetting);
            func_8005ACFC(globalCtx->cameraPtrs[MAIN_CAM], 4);
        }
    }
    thisv->dyna.actor.world.pos.y =
        thisv->dyna.actor.home.pos.y - (sinf(thisv->timer * (std::numbers::pi_v<float> / 256.0f)) * 471.24f); // pi * 150
    if (thisv->timer == 0) {
        thisv->timer = 512;
    }
}

void func_8086C9A8(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        thisv->timer = 100;
        thisv->actionFunc = func_8086C9F0;
    }
}

void func_8086C9F0(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y, 0.5f)) {
            Flags_UnsetSwitch(globalCtx, thisv->switchFlag);
            thisv->actionFunc = func_8086C9A8;
        }
        func_8002F948(&thisv->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    } else {
        if (Math_StepToF(&thisv->dyna.actor.world.pos.y, thisv->dyna.actor.home.pos.y + 75.0f, 0.5f)) {
            thisv->actionFunc = func_8086CABC;
        }
        func_8002F948(&thisv->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    }
    globalCtx->colCtx.colHeader->waterBoxes[7].ySurface = thisv->dyna.actor.world.pos.y;
}

void func_8086CABC(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }
    func_8002F994(&thisv->dyna.actor, thisv->timer);
    if (thisv->timer == 0) {
        thisv->actionFunc = func_8086C9F0;
    }
}

void func_8086CB10(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (func_8004356C(&thisv->dyna)) {
        Flags_SetSwitch(globalCtx, thisv->switchFlag);
        thisv->timer = 50;
        thisv->actionFunc = func_8086CB8C;
        thisv->dyna.actor.home.pos.y -= 200.0f;
        OnePointCutscene_Init(globalCtx, 3100, 51, &thisv->dyna.actor, MAIN_CAM);
    }
}

void func_8086CB8C(BgBdanObjects* thisv, GlobalContext* globalCtx) {
    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y - (cosf(thisv->timer * (std::numbers::pi_v<float> / 50.0f)) * 200.0f);

    if (thisv->timer == 0) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);
        thisv->actionFunc = BgBdanObjects_DoNothing;
        Gameplay_CopyCamera(globalCtx, MAIN_CAM, SUBCAM_ACTIVE);
    } else {
        func_8002F974(&thisv->dyna.actor, NA_SE_EV_BUYOSTAND_FALL - SFX_FLAG);
    }
}

void BgBdanObjects_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgBdanObjects* thisv = (BgBdanObjects*)thisvx;

    Actor_SetFocus(thisvx, 50.0f);
    thisv->actionFunc(thisv, globalCtx);
}

void BgBdanObjects_Draw(Actor* thisvx, GlobalContext* globalCtx) {
    BgBdanObjects* thisv = (BgBdanObjects*)thisvx;

    if (thisvx->params == 0) {
        if (thisv->actionFunc == func_8086C054) {
            if (((thisvx->home.pos.y + -79.0f) - 5.0f) < thisvx->world.pos.y) {
                Matrix_Translate(0.0f, -50.0f, 0.0f, MTXMODE_APPLY);
            }
        }
    }

    if (thisvx->params == 2) {
        Gfx_DrawDListXlu(globalCtx, gJabuWaterDL);
    } else {
        Gfx_DrawDListOpa(globalCtx, sDLists[thisvx->params]);
    }
}
