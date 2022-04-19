/*
 * File: z_bg_mizu_movebg.c
 * Overlay: ovl_Bg_Mizu_Movebg
 * Description: Kakariko Village Well Water
 */

#include "z_bg_mizu_movebg.h"
#include "overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_4

#define MOVEBG_TYPE(params) (((u16)(params) >> 0xC) & 0xF)
#define MOVEBG_FLAGS(params) ((u16)(params)&0x3F)
#define MOVEBG_PATH_ID(params) (((u16)(params) >> 0x8) & 0xF)
#define MOVEBG_POINT_ID(params) ((u16)(params)&0xF)
#define MOVEBG_SPEED(params) (((u16)(params) >> 0x4) & 0xF)

void BgMizuMovebg_Init(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuMovebg_Destroy(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuMovebg_Update(Actor* thisvx, GlobalContext* globalCtx);
void BgMizuMovebg_Draw(Actor* thisvx, GlobalContext* globalCtx);

void func_8089E318(BgMizuMovebg* thisv, GlobalContext* globalCtx);
void func_8089E650(BgMizuMovebg* thisv, GlobalContext* globalCtx);
s32 func_8089E108(Path* pathList, Vec3f* pos, s32 pathId, s32 pointId);

ActorInit Bg_Mizu_Movebg_InitVars = {
    ACTOR_BG_MIZU_MOVEBG,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuMovebg),
    (ActorFunc)BgMizuMovebg_Init,
    (ActorFunc)BgMizuMovebg_Destroy,
    (ActorFunc)BgMizuMovebg_Update,
    (ActorFunc)BgMizuMovebg_Draw,
    NULL,
};

static f32 D_8089EB40[] = { -115.200005f, -115.200005f, -115.200005f, 0.0f };

static const Gfx* D_8089EB50[] = {
    gObjectMizuObjectsMovebgDL_000190, gObjectMizuObjectsMovebgDL_000680, gObjectMizuObjectsMovebgDL_000C20,
    gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_002E10,
    gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_0011F0,
};

static const CollisionHeader* D_8089EB70[] = {
    &gObjectMizuObjectsMovebgCol_0003F0, &gObjectMizuObjectsMovebgCol_000998, &gObjectMizuObjectsMovebgCol_000ED0,
    &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_003590,
    &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_0015F8,
};

static InitChainEntry D_8089EB90[] = {
    ICHAIN_F32(uncullZoneScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Vec3f D_8089EBA0 = { 0.0f, 80.0f, 23.0f };
static Vec3f D_8089EBAC = { 0.0f, 80.0f, 23.0f };

static u8 D_8089EE40;

s32 func_8089DC30(GlobalContext* globalCtx) {
    s32 result;

    if (Flags_GetSwitch(globalCtx, WATER_TEMPLE_WATER_F1_FLAG)) {
        result = 1;
    } else if (Flags_GetSwitch(globalCtx, WATER_TEMPLE_WATER_F2_FLAG)) {
        result = 2;
    } else if (Flags_GetSwitch(globalCtx, WATER_TEMPLE_WATER_F3_FLAG)) {
        result = 3;
    } else {
        result = 1;
    }
    return result;
}

void BgMizuMovebg_Init(Actor* thisvx, GlobalContext* globalCtx) {
    s32 type;
    s32 waypointId;
    WaterBox* waterBoxes = globalCtx->colCtx.colHeader->waterBoxes;
    f32 temp;
    const CollisionHeader* colHeader = NULL;
    Vec3f sp48;

    Actor_ProcessInitChain(thisvx, D_8089EB90);
    ((BgMizuMovebg*)thisvx)->homeY = thisvx->world.pos.y;
    ((BgMizuMovebg*)thisvx)->dlist = D_8089EB50[MOVEBG_TYPE(thisvx->params)];
    DynaPolyActor_Init(&((BgMizuMovebg*)thisvx)->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(D_8089EB70[MOVEBG_TYPE(thisvx->params)], &colHeader);
    ((BgMizuMovebg*)thisvx)->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisvx, colHeader);

    type = MOVEBG_TYPE(thisvx->params);
    switch (type) {
        case 0:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisvx)->homeY - 700.0f) {
                thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY - 700.0f;
            } else {
                thisvx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E318;
            break;
        case 1:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisvx)->homeY - 710.0f) {
                thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY - 710.0f;
            } else {
                thisvx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E318;
            break;
        case 2:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisvx)->homeY - 700.0f) {
                thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY - 700.0f;
            } else {
                thisvx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E318;
            break;
        case 3:
            thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY + D_8089EB40[func_8089DC30(globalCtx)];
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E318;
            break;
        case 4:
        case 5:
        case 6:
            if (Flags_GetSwitch(globalCtx, MOVEBG_FLAGS(thisvx->params))) {
                thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY + 115.19999999999999;
            } else {
                thisvx->world.pos.y = ((BgMizuMovebg*)thisvx)->homeY;
            }
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E318;
            break;
        case 7:
            ((BgMizuMovebg*)thisvx)->scrollAlpha1 = 160;
            ((BgMizuMovebg*)thisvx)->scrollAlpha2 = 160;
            ((BgMizuMovebg*)thisvx)->scrollAlpha3 = 160;
            ((BgMizuMovebg*)thisvx)->scrollAlpha4 = 160;
            waypointId = MOVEBG_POINT_ID(thisvx->params);
            ((BgMizuMovebg*)thisvx)->waypointId = waypointId;
            func_8089E108(globalCtx->setupPathList, &thisvx->world.pos, MOVEBG_PATH_ID(thisvx->params), waypointId);
            ((BgMizuMovebg*)thisvx)->actionFunc = func_8089E650;
            break;
    }

    type = MOVEBG_TYPE(thisvx->params);
    switch (type) {
        case 3:
        case 4:
        case 5:
        case 6:
            Matrix_RotateY(thisvx->world.rot.y * (std::numbers::pi_v<float> / 32768), MTXMODE_NEW);
            Matrix_MultVec3f(&D_8089EBA0, &sp48);

            if (Actor_SpawnAsChild(&globalCtx->actorCtx, thisvx, globalCtx, ACTOR_OBJ_HSBLOCK,
                                   thisvx->world.pos.x + sp48.x, thisvx->world.pos.y + sp48.y,
                                   thisvx->world.pos.z + sp48.z, thisvx->world.rot.x, thisvx->world.rot.y,
                                   thisvx->world.rot.z, 2) == NULL) {
                Actor_Kill(thisvx);
            }
            break;
    }
}

void BgMizuMovebg_Destroy(Actor* thisvx, GlobalContext* globalCtx) {
    BgMizuMovebg* thisv = (BgMizuMovebg*)thisvx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
    switch (MOVEBG_TYPE(thisvx->params)) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (thisv->sfxFlags & 2) {
                D_8089EE40 &= ~2;
            }
            break;
        case 7:
            if (thisv->sfxFlags & 1) {
                D_8089EE40 &= ~1;
            }
            break;
    }
}

s32 func_8089E108(Path* pathList, Vec3f* pos, s32 pathId, s32 pointId) {
    Path* path = pathList;
    Vec3s* point;

    path += pathId;
    point = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[pointId];

    pos->x = point->x;
    pos->y = point->y;
    pos->z = point->z;

    return 0;
}

void func_8089E198(BgMizuMovebg* thisv, GlobalContext* globalCtx) {
    f32 waterLevel = globalCtx->colCtx.colHeader->waterBoxes[2].ySurface;

    if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        thisv->scrollAlpha1 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        thisv->scrollAlpha1 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F1_Y) /
                                         (WATER_TEMPLE_WATER_F2_Y - WATER_TEMPLE_WATER_F1_Y) * (255 - 160));
    } else {
        thisv->scrollAlpha1 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        thisv->scrollAlpha2 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F3_Y) {
        thisv->scrollAlpha2 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F2_Y) /
                                         (WATER_TEMPLE_WATER_F3_Y - WATER_TEMPLE_WATER_F2_Y) * (255 - 160));
    } else {
        thisv->scrollAlpha2 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_B1_Y) {
        thisv->scrollAlpha3 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        thisv->scrollAlpha3 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_B1_Y) /
                                         (WATER_TEMPLE_WATER_F1_Y - WATER_TEMPLE_WATER_B1_Y) * (255 - 160));
    } else {
        thisv->scrollAlpha3 = 160;
    }

    thisv->scrollAlpha4 = thisv->scrollAlpha3;
}

void func_8089E318(BgMizuMovebg* thisv, GlobalContext* globalCtx) {
    WaterBox* waterBoxes = globalCtx->colCtx.colHeader->waterBoxes;
    f32 phi_f0;
    s32 type;
    Vec3f sp28;

    func_8089E198(thisv, globalCtx);

    type = MOVEBG_TYPE(thisv->dyna.actor.params);
    switch (type) {
        case 0:
        case 2:
            phi_f0 = waterBoxes[2].ySurface + 15.0f;
            if (phi_f0 < thisv->homeY - 700.0f) {
                thisv->dyna.actor.world.pos.y = thisv->homeY - 700.0f;
            } else {
                thisv->dyna.actor.world.pos.y = phi_f0;
            }
            break;
        case 1:
            phi_f0 = waterBoxes[2].ySurface + 15.0f;
            if (phi_f0 < thisv->homeY - 710.0f) {
                thisv->dyna.actor.world.pos.y = thisv->homeY - 710.0f;
            } else {
                thisv->dyna.actor.world.pos.y = phi_f0;
            }
            break;
        case 3:
            phi_f0 = thisv->homeY + D_8089EB40[func_8089DC30(globalCtx)];
            if (!Math_StepToF(&thisv->dyna.actor.world.pos.y, phi_f0, 1.0f)) {
                if (!(D_8089EE40 & 2) && MOVEBG_SPEED(thisv->dyna.actor.params) != 0) {
                    D_8089EE40 |= 2;
                    thisv->sfxFlags |= 2;
                }
                if (thisv->sfxFlags & 2) {
                    if (thisv->dyna.actor.room == 0) {
                        func_8002F974(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                    } else {
                        func_8002F948(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                    }
                }
            }
            break;
        case 4:
        case 5:
        case 6:
            if (Flags_GetSwitch(globalCtx, MOVEBG_FLAGS(thisv->dyna.actor.params))) {
                phi_f0 = thisv->homeY + 115.200005f;
            } else {
                phi_f0 = thisv->homeY;
            }
            if (!Math_StepToF(&thisv->dyna.actor.world.pos.y, phi_f0, 1.0f)) {
                if (!(D_8089EE40 & 2) && MOVEBG_SPEED(thisv->dyna.actor.params) != 0) {
                    D_8089EE40 |= 2;
                    thisv->sfxFlags |= 2;
                }
                if (thisv->sfxFlags & 2) {
                    func_8002F948(&thisv->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                }
            }
            break;
    }

    type = MOVEBG_TYPE(thisv->dyna.actor.params);
    switch (type) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (globalCtx->roomCtx.curRoom.num == thisv->dyna.actor.room) {
                Matrix_RotateY(thisv->dyna.actor.world.rot.y * (std::numbers::pi_v<float> / 32768), MTXMODE_NEW);
                Matrix_MultVec3f(&D_8089EBAC, &sp28);
                thisv->dyna.actor.child->world.pos.x = thisv->dyna.actor.world.pos.x + sp28.x;
                thisv->dyna.actor.child->world.pos.y = thisv->dyna.actor.world.pos.y + sp28.y;
                thisv->dyna.actor.child->world.pos.z = thisv->dyna.actor.world.pos.z + sp28.z;
                thisv->dyna.actor.child->flags &= ~ACTOR_FLAG_0;
            }
            break;
    }
}

void func_8089E650(BgMizuMovebg* thisv, GlobalContext* globalCtx) {
    Vec3f waypoint;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;

    thisv->dyna.actor.speedXZ = MOVEBG_SPEED(thisv->dyna.actor.params) * 0.1f;
    func_8089E108(globalCtx->setupPathList, &waypoint, MOVEBG_PATH_ID(thisv->dyna.actor.params), thisv->waypointId);
    dist = Actor_WorldDistXYZToPoint(&thisv->dyna.actor, &waypoint);
    if (dist < thisv->dyna.actor.speedXZ) {
        thisv->dyna.actor.speedXZ = dist;
    }
    func_80035844(&thisv->dyna.actor.world.pos, &waypoint, &thisv->dyna.actor.world.rot, 1);
    func_8002D97C(&thisv->dyna.actor);
    dx = waypoint.x - thisv->dyna.actor.world.pos.x;
    dy = waypoint.y - thisv->dyna.actor.world.pos.y;
    dz = waypoint.z - thisv->dyna.actor.world.pos.z;
    if (fabsf(dx) < 2.0f && fabsf(dy) < 2.0f && fabsf(dz) < 2.0f) {
        thisv->waypointId++;
        if (thisv->waypointId >= globalCtx->setupPathList[MOVEBG_PATH_ID(thisv->dyna.actor.params)].count) {
            thisv->waypointId = 0;
            func_8089E108(globalCtx->setupPathList, &thisv->dyna.actor.world.pos,
                          MOVEBG_PATH_ID(thisv->dyna.actor.params), 0);
        }
    }
    if (!(D_8089EE40 & 1) && MOVEBG_SPEED(thisv->dyna.actor.params) != 0) {
        D_8089EE40 |= 1;
        thisv->sfxFlags |= 1;
    }
    if (thisv->sfxFlags & 1) {
        func_8002F948(&thisv->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
    }
}

void BgMizuMovebg_Update(Actor* thisvx, GlobalContext* globalCtx) {
    BgMizuMovebg* thisv = (BgMizuMovebg*)thisvx;

    thisv->actionFunc(thisv, globalCtx);
}

void BgMizuMovebg_Draw(Actor* thisvx, GlobalContext* globalCtx2) {
    BgMizuMovebg* thisv = (BgMizuMovebg*)thisvx;
    GlobalContext* globalCtx = globalCtx2;
    u32 frames;

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mizu_movebg.c", 754);

    frames = globalCtx->gameplayFrames;
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColor(globalCtx->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        thisv->scrollAlpha1));

    gSPSegment(POLY_OPA_DISP++, 0x09,
               Gfx_TwoTexScrollEnvColor(globalCtx->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        thisv->scrollAlpha2));

    gSPSegment(POLY_OPA_DISP++, 0x0A,
               Gfx_TwoTexScrollEnvColor(globalCtx->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        thisv->scrollAlpha3));

    gSPSegment(POLY_OPA_DISP++, 0x0B,
               Gfx_TwoTexScrollEnvColor(globalCtx->state.gfxCtx, 0, frames * 3, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        thisv->scrollAlpha4));

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mizu_movebg.c", 788),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisv->dlist != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, thisv->dlist);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mizu_movebg.c", 795);
}
