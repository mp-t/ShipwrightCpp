/*
 * File: z_obj_oshihiki.c
 * Overlay: ovl_Obj_Oshihiki
 * Description: Push Block
 */

#include "z_obj_oshihiki.h"
#include "overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS ACTOR_FLAG_4

void ObjOshihiki_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjOshihiki_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjOshihiki_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjOshihiki_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjOshihiki_SetupOnScene(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_OnScene(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_SetupOnActor(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_OnActor(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_SetupPush(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_Push(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_SetupFall(ObjOshihiki* thisv, GlobalContext* globalCtx);
void ObjOshihiki_Fall(ObjOshihiki* thisv, GlobalContext* globalCtx);

const ActorInit Obj_Oshihiki_InitVars = {
    ACTOR_OBJ_OSHIHIKI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(ObjOshihiki),
    (ActorFunc)ObjOshihiki_Init,
    (ActorFunc)ObjOshihiki_Destroy,
    (ActorFunc)ObjOshihiki_Update,
    (ActorFunc)ObjOshihiki_Draw,
    NULL,
};

static f32 sScales[] = {
    (1 / 10.0f), (1 / 6.0f), (1 / 5.0f), (1 / 3.0f), (1 / 10.0f), (1 / 6.0f), (1 / 5.0f), (1 / 3.0f),
};

static Color_RGB8 sColors[][4] = {
    { { 110, 86, 40 }, { 110, 86, 40 }, { 110, 86, 40 }, { 110, 86, 40 } },         // deku tree
    { { 106, 120, 110 }, { 104, 80, 20 }, { 0, 0, 0 }, { 0, 0, 0 } },               // dodongos cavern
    { { 142, 99, 86 }, { 72, 118, 96 }, { 0, 0, 0 }, { 0, 0, 0 } },                 // forest temple
    { { 210, 150, 80 }, { 210, 170, 80 }, { 0, 0, 0 }, { 0, 0, 0 } },               // fire temple
    { { 102, 144, 182 }, { 176, 167, 100 }, { 100, 167, 100 }, { 117, 97, 96 } },   // water temple
    { { 232, 210, 176 }, { 232, 210, 176 }, { 232, 210, 176 }, { 232, 210, 176 } }, // spirit temple
    { { 135, 125, 95 }, { 135, 125, 95 }, { 135, 125, 95 }, { 135, 125, 95 } },     // shadow temple
    { { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 } }, // ganons castle
    { { 232, 210, 176 }, { 232, 210, 176 }, { 232, 210, 176 }, { 232, 210, 176 } }, // gerudo training grounds
};

static s16 sScenes[] = {
    SCENE_YDAN,      SCENE_DDAN,    SCENE_BMORI1, SCENE_HIDAN, SCENE_MIZUSIN,
    SCENE_JYASINZOU, SCENE_HAKADAN, SCENE_GANON,  SCENE_MEN,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

// The vertices and center of the bottom face
static Vec3f sColCheckPoints[5] = {
    { 29.99f, 1.01f, -29.99f }, { -29.99f, 1.01f, -29.99f }, { -29.99f, 1.01f, 29.99f },
    { 29.99f, 1.01f, 29.99f },  { 0.0f, 1.01f, 0.0f },
};

static Vec2f sFaceVtx[] = {
    { -30.0f, 0.0f },
    { 30.0f, 0.0f },
    { -30.0f, 60.0f },
    { 30.0f, 60.0f },
};

static Vec2f sFaceDirection[] = {
    { 1.0f, 1.0f },
    { -1.0f, 1.0f },
    { 1.0f, -1.0f },
    { -1.0f, -1.0f },
};

void ObjOshihiki_InitDynapoly(ObjOshihiki* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG registration failure"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_oshihiki.c", 280,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void ObjOshihiki_RotateXZ(Vec3f* out, Vec3f* in, f32 sn, f32 cs) {
    out->x = (in->z * sn) + (in->x * cs);
    out->y = in->y;
    out->z = (in->z * cs) - (in->x * sn);
}

s32 ObjOshihiki_StrongEnough(ObjOshihiki* thisv) {
    s32 strength;

    if (thisv->cantMove) {
        return 0;
    }
    strength = Player_GetStrength();
    switch (thisv->dyna.actor.params & 0xF) {
        case PUSHBLOCK_SMALL_START_ON:
        case PUSHBLOCK_MEDIUM_START_ON:
        case PUSHBLOCK_SMALL_START_OFF:
        case PUSHBLOCK_MEDIUM_START_OFF:
            return 1;
            break;
        case PUSHBLOCK_LARGE_START_ON:
        case PUSHBLOCK_LARGE_START_OFF:
            return strength >= PLAYER_STR_BRACELET;
            break;
        case PUSHBLOCK_HUGE_START_ON:
        case PUSHBLOCK_HUGE_START_OFF:
            return strength >= PLAYER_STR_SILVER_G;
            break;
    }
    return 0;
}

void ObjOshihiki_ResetFloors(ObjOshihiki* thisv) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(thisv->floorBgIds); i++) {
        thisv->floorBgIds[i] = BGCHECK_SCENE;
    }
}

ObjOshihiki* ObjOshihiki_GetBlockUnder(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* dynaPolyActor;

    if ((thisv->floorBgIds[thisv->highestFloor] != BGCHECK_SCENE) &&
        (fabsf(thisv->dyna.actor.floorHeight - thisv->dyna.actor.world.pos.y) < 0.001f)) {
        dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, thisv->floorBgIds[thisv->highestFloor]);
        if ((dynaPolyActor != NULL) && (dynaPolyActor->actor.id == ACTOR_OBJ_OSHIHIKI)) {
            return (ObjOshihiki*)dynaPolyActor;
        }
    }
    return NULL;
}

void ObjOshihiki_UpdateInitPos(ObjOshihiki* thisv) {
    if (thisv->dyna.actor.home.pos.x < thisv->dyna.actor.world.pos.x) {
        while ((thisv->dyna.actor.world.pos.x - thisv->dyna.actor.home.pos.x) >= 20.0f) {
            thisv->dyna.actor.home.pos.x += 20.0f;
        }
    } else {
        while ((thisv->dyna.actor.home.pos.x - thisv->dyna.actor.world.pos.x) >= 20.0f) {
            thisv->dyna.actor.home.pos.x -= 20.0f;
        }
    }
    if (thisv->dyna.actor.home.pos.z < thisv->dyna.actor.world.pos.z) {
        while ((thisv->dyna.actor.world.pos.z - thisv->dyna.actor.home.pos.z) >= 20.0f) {
            thisv->dyna.actor.home.pos.z += 20.0f;
        }
    } else {
        while ((thisv->dyna.actor.home.pos.z - thisv->dyna.actor.world.pos.z) >= 20.0f) {
            thisv->dyna.actor.home.pos.z -= 20.0f;
        }
    }
}

s32 ObjOshihiki_NoSwitchPress(ObjOshihiki* thisv, DynaPolyActor* dyna, GlobalContext* globalCtx) {
    s16 dynaSwitchFlag;

    if (dyna == NULL) {
        return 1;
    } else if (dyna->actor.id == ACTOR_OBJ_SWITCH) {
        dynaSwitchFlag = (dyna->actor.params >> 8) & 0x3F;
        switch (dyna->actor.params & 0x33) {
            case 0x20: // Normal blue switch
                if ((dynaSwitchFlag == ((thisv->dyna.actor.params >> 8) & 0x3F)) &&
                    Flags_GetSwitch(globalCtx, dynaSwitchFlag)) {
                    return 0;
                }
                break;
            case 0x30: // Inverse blue switch
                if ((dynaSwitchFlag == ((thisv->dyna.actor.params >> 8) & 0x3F)) &&
                    !Flags_GetSwitch(globalCtx, dynaSwitchFlag)) {
                    return 0;
                }
                break;
        }
    }
    return 1;
}

void ObjOshihiki_CheckType(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xF) {
        case PUSHBLOCK_SMALL_START_ON:
        case PUSHBLOCK_MEDIUM_START_ON:
        case PUSHBLOCK_LARGE_START_ON:
        case PUSHBLOCK_HUGE_START_ON:
        case PUSHBLOCK_SMALL_START_OFF:
        case PUSHBLOCK_MEDIUM_START_OFF:
        case PUSHBLOCK_LARGE_START_OFF:
        case PUSHBLOCK_HUGE_START_OFF:
            ObjOshihiki_InitDynapoly(thisv, globalCtx, &gPushBlockCol, 1);
            break;
        default:
            // "Error : type cannot be determined"
            osSyncPrintf("Error : タイプが判別できない(%s %d)(arg_data 0x%04x)\n", "../z_obj_oshihiki.c", 444,
                         thisv->dyna.actor.params);
            break;
    }
}

void ObjOshihiki_SetScale(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    Actor_SetScale(&thisv->dyna.actor, sScales[thisv->dyna.actor.params & 0xF]);
}

void ObjOshihiki_SetTexture(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    switch (thisv->dyna.actor.params & 0xF) {
        case PUSHBLOCK_SMALL_START_ON:
        case PUSHBLOCK_MEDIUM_START_ON:
        case PUSHBLOCK_SMALL_START_OFF:
        case PUSHBLOCK_MEDIUM_START_OFF:
            thisv->texture = gPushBlockSilverTex;
            break;
        case PUSHBLOCK_LARGE_START_ON:
        case PUSHBLOCK_LARGE_START_OFF:
            thisv->texture = gPushBlockBaseTex;
            break;
        case PUSHBLOCK_HUGE_START_ON:
        case PUSHBLOCK_HUGE_START_OFF:
            thisv->texture = gPushBlockGrayTex;
            break;
    }
}

void ObjOshihiki_SetColor(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    Color_RGB8* src;
    Color_RGB8* color = &thisv->color;
    s16 paramsColorIdx;
    s32 i;

    paramsColorIdx = (thisv->dyna.actor.params >> 6) & 3;

    for (i = 0; i < ARRAY_COUNT(sScenes); i++) {
        if (sScenes[i] == globalCtx->sceneNum) {
            break;
        }
    }

    if (i >= ARRAY_COUNT(sColors)) {
        // "Error : scene_data_ID cannot be determined"
        osSyncPrintf("Error : scene_data_ID が判別できない。(%s %d)\n", "../z_obj_oshihiki.c", 579);
        color->r = color->g = color->b = 255;
    } else {
        src = &sColors[i][paramsColorIdx];
        color->r = src->r;
        color->g = src->g;
        color->b = src->b;
    }
}

void ObjOshihiki_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjOshihiki* thisv = (ObjOshihiki*)thisx;

    ObjOshihiki_CheckType(thisv, globalCtx);

    if ((((thisv->dyna.actor.params >> 8) & 0xFF) >= 0) && (((thisv->dyna.actor.params >> 8) & 0xFF) <= 0x3F)) {
        if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8) & 0x3F)) {
            switch (thisv->dyna.actor.params & 0xF) {
                case PUSHBLOCK_SMALL_START_ON:
                case PUSHBLOCK_MEDIUM_START_ON:
                case PUSHBLOCK_LARGE_START_ON:
                case PUSHBLOCK_HUGE_START_ON:
                    Actor_Kill(&thisv->dyna.actor);
                    return;
            }
        } else {
            switch (thisv->dyna.actor.params & 0xF) {
                case PUSHBLOCK_SMALL_START_OFF:
                case PUSHBLOCK_MEDIUM_START_OFF:
                case PUSHBLOCK_LARGE_START_OFF:
                case PUSHBLOCK_HUGE_START_OFF:
                    Actor_Kill(&thisv->dyna.actor);
                    return;
            }
        }
    }

    ObjOshihiki_SetScale(thisv, globalCtx);
    ObjOshihiki_SetTexture(thisv, globalCtx);
    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    ObjOshihiki_SetColor(thisv, globalCtx);
    ObjOshihiki_ResetFloors(thisv);
    ObjOshihiki_SetupOnActor(thisv, globalCtx);
    // "(dungeon keep push-pull block)"
    osSyncPrintf("(dungeon keep 押し引きブロック)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void ObjOshihiki_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjOshihiki* thisv = (ObjOshihiki*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
}

void ObjOshihiki_SetFloors(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 5; i++) {
        Vec3f colCheckPoint;
        Vec3f colCheckOffset;
        CollisionPoly** floorPoly;
        s32* floorBgId;

        colCheckOffset.x = sColCheckPoints[i].x * (thisv->dyna.actor.scale.x * 10.0f);
        colCheckOffset.y = sColCheckPoints[i].y * (thisv->dyna.actor.scale.y * 10.0f);
        colCheckOffset.z = sColCheckPoints[i].z * (thisv->dyna.actor.scale.z * 10.0f);
        ObjOshihiki_RotateXZ(&colCheckPoint, &colCheckOffset, thisv->yawSin, thisv->yawCos);
        colCheckPoint.x += thisv->dyna.actor.world.pos.x;
        colCheckPoint.y += thisv->dyna.actor.prevPos.y;
        colCheckPoint.z += thisv->dyna.actor.world.pos.z;

        floorPoly = &thisv->floorPolys[i];
        floorBgId = &thisv->floorBgIds[i];
        thisv->floorHeights[i] = BgCheck_EntityRaycastFloor6(&globalCtx->colCtx, floorPoly, floorBgId, &thisv->dyna.actor,
                                                            &colCheckPoint, 0.0f);
    }
}

s16 ObjOshihiki_GetHighestFloor(ObjOshihiki* thisv) {
    s16 highestFloor = 0;
    s16 temp = 1;
    f32 phi_f0 = thisv->floorHeights[temp];

    if (phi_f0 > thisv->floorHeights[highestFloor]) {
        highestFloor = temp;
    } else if ((thisv->floorBgIds[temp] == BGCHECK_SCENE) && ((phi_f0 - thisv->floorHeights[highestFloor]) > -0.001f)) {
        highestFloor = temp;
    }
    if (thisv->floorHeights[temp + 1] > thisv->floorHeights[highestFloor]) {
        highestFloor = temp + 1;
    } else if ((thisv->floorBgIds[temp + 1] == BGCHECK_SCENE) &&
               ((thisv->floorHeights[temp + 1] - thisv->floorHeights[highestFloor]) > -0.001f)) {
        highestFloor = temp + 1;
    }
    if (thisv->floorHeights[temp + 2] > thisv->floorHeights[highestFloor]) {
        highestFloor = temp + 2;
    } else if ((thisv->floorBgIds[temp + 2] == BGCHECK_SCENE) &&
               ((thisv->floorHeights[temp + 2] - thisv->floorHeights[highestFloor]) > -0.001f)) {
        highestFloor = temp + 2;
    }
    if (thisv->floorHeights[temp + 3] > thisv->floorHeights[highestFloor]) {
        highestFloor = temp + 3;
    } else if ((thisv->floorBgIds[temp + 3] == BGCHECK_SCENE) &&
               ((thisv->floorHeights[temp + 3] - thisv->floorHeights[highestFloor]) > -0.001f)) {
        highestFloor = temp + 3;
    }
    return highestFloor;
}

void ObjOshihiki_SetGround(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    ObjOshihiki_ResetFloors(thisv);
    ObjOshihiki_SetFloors(thisv, globalCtx);
    thisv->highestFloor = ObjOshihiki_GetHighestFloor(thisv);
    thisv->dyna.actor.floorHeight = thisv->floorHeights[thisv->highestFloor];
}

s32 ObjOshihiki_CheckFloor(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    ObjOshihiki_SetGround(thisv, globalCtx);

    if ((thisv->dyna.actor.floorHeight - thisv->dyna.actor.world.pos.y) >= -0.001f) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
        return 1;
    }

    return 0;
}

s32 ObjOshihiki_CheckGround(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    if (thisv->dyna.actor.world.pos.y <= BGCHECK_Y_MIN + 10.0f) {
        // "Warning : Push-pull block fell too much"
        osSyncPrintf("Warning : 押し引きブロック落ちすぎた(%s %d)(arg_data 0x%04x)\n", "../z_obj_oshihiki.c", 809,
                     thisv->dyna.actor.params);
        Actor_Kill(&thisv->dyna.actor);
        return 0;
    }
    if ((thisv->dyna.actor.floorHeight - thisv->dyna.actor.world.pos.y) >= -0.001f) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
        return 1;
    }
    return 0;
}

s32 ObjOshihiki_CheckWall(GlobalContext* globalCtx, s16 angle, f32 direction, ObjOshihiki* thisv) {
    f32 maxDist = ((direction >= 0.0f) ? 1.0f : -1.0f) * (300.0f * thisv->dyna.actor.scale.x + 20.0f - 0.5f);
    f32 sn = Math_SinS(angle);
    f32 cs = Math_CosS(angle);
    s32 i;

    for (i = 0; i < 4; i++) {
        Vec3f faceVtx;
        Vec3f faceVtxNext;
        Vec3f posResult;
        Vec3f faceVtxOffset;
        s32 bgId;
        CollisionPoly* outPoly;

        faceVtxOffset.x = (sFaceVtx[i].x * thisv->dyna.actor.scale.x * 10.0f) + sFaceDirection[i].x;
        faceVtxOffset.y = (sFaceVtx[i].y * thisv->dyna.actor.scale.y * 10.0f) + sFaceDirection[i].y;
        faceVtxOffset.z = 0.0f;
        ObjOshihiki_RotateXZ(&faceVtx, &faceVtxOffset, sn, cs);
        faceVtx.x += thisv->dyna.actor.world.pos.x;
        faceVtx.y += thisv->dyna.actor.world.pos.y;
        faceVtx.z += thisv->dyna.actor.world.pos.z;
        faceVtxNext.x = faceVtx.x + maxDist * sn;
        faceVtxNext.y = faceVtx.y;
        faceVtxNext.z = faceVtx.z + maxDist * cs;
        if (BgCheck_EntityLineTest3(&globalCtx->colCtx, &faceVtx, &faceVtxNext, &posResult, &outPoly, true, false,
                                    false, true, &bgId, &thisv->dyna.actor, 0.0f)) {
            return true;
        }
    }
    return false;
}

s32 ObjOshihiki_MoveWithBlockUnder(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    s32 pad;
    ObjOshihiki* blockUnder = ObjOshihiki_GetBlockUnder(thisv, globalCtx);

    if ((blockUnder != NULL) && (blockUnder->stateFlags & PUSHBLOCK_SETUP_PUSH) &&
        !ObjOshihiki_CheckWall(globalCtx, blockUnder->dyna.unk_158, blockUnder->direction, thisv)) {
        thisv->blockUnder = blockUnder;
    }

    if ((thisv->stateFlags & PUSHBLOCK_MOVE_UNDER) && (thisv->blockUnder != NULL)) {
        if (thisv->blockUnder->stateFlags & PUSHBLOCK_PUSH) {
            thisv->underDistX = thisv->blockUnder->dyna.actor.world.pos.x - thisv->blockUnder->dyna.actor.prevPos.x;
            thisv->underDistZ = thisv->blockUnder->dyna.actor.world.pos.z - thisv->blockUnder->dyna.actor.prevPos.z;
            thisv->dyna.actor.world.pos.x += thisv->underDistX;
            thisv->dyna.actor.world.pos.z += thisv->underDistZ;
            ObjOshihiki_UpdateInitPos(thisv);
            return true;
        } else if (!(thisv->blockUnder->stateFlags & PUSHBLOCK_SETUP_PUSH)) {
            thisv->blockUnder = NULL;
        }
    }
    return false;
}

void ObjOshihiki_SetupOnScene(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= PUSHBLOCK_SETUP_ON_SCENE;
    thisv->actionFunc = ObjOshihiki_OnScene;
    thisv->dyna.actor.gravity = 0.0f;
    thisv->dyna.actor.velocity.x = thisv->dyna.actor.velocity.y = thisv->dyna.actor.velocity.z = 0.0f;
}

void ObjOshihiki_OnScene(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);

    thisv->stateFlags |= PUSHBLOCK_ON_SCENE;
    if ((thisv->timer <= 0) && (fabsf(thisv->dyna.unk_150) > 0.001f)) {
        if (ObjOshihiki_StrongEnough(thisv) &&
            !ObjOshihiki_CheckWall(globalCtx, thisv->dyna.unk_158, thisv->dyna.unk_150, thisv)) {
            thisv->direction = thisv->dyna.unk_150;
            ObjOshihiki_SetupPush(thisv, globalCtx);
        } else {
            player->stateFlags2 &= ~0x10;
            thisv->dyna.unk_150 = 0.0f;
        }
    } else {
        player->stateFlags2 &= ~0x10;
        thisv->dyna.unk_150 = 0.0f;
    }
}

void ObjOshihiki_SetupOnActor(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= PUSHBLOCK_SETUP_ON_ACTOR;
    thisv->actionFunc = ObjOshihiki_OnActor;
    thisv->dyna.actor.velocity.x = thisv->dyna.actor.velocity.y = thisv->dyna.actor.velocity.z = 0.0f;
    thisv->dyna.actor.gravity = -1.0f;
}

void ObjOshihiki_OnActor(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    s32 bgId;
    Player* player = GET_PLAYER(globalCtx);
    DynaPolyActor* dynaPolyActor;

    thisv->stateFlags |= PUSHBLOCK_ON_ACTOR;
    Actor_MoveForward(&thisv->dyna.actor);

    if (ObjOshihiki_CheckFloor(thisv, globalCtx)) {
        bgId = thisv->floorBgIds[thisv->highestFloor];
        if (bgId == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(thisv, globalCtx);
        } else {
            dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, bgId);
            if (dynaPolyActor != NULL) {
                func_800434A8(dynaPolyActor);
                func_80043538(dynaPolyActor);

                if ((thisv->timer <= 0) && (fabsf(thisv->dyna.unk_150) > 0.001f)) {
                    if (ObjOshihiki_StrongEnough(thisv) && ObjOshihiki_NoSwitchPress(thisv, dynaPolyActor, globalCtx) &&
                        !ObjOshihiki_CheckWall(globalCtx, thisv->dyna.unk_158, thisv->dyna.unk_150, thisv)) {

                        thisv->direction = thisv->dyna.unk_150;
                        ObjOshihiki_SetupPush(thisv, globalCtx);
                    } else {
                        player->stateFlags2 &= ~0x10;
                        thisv->dyna.unk_150 = 0.0f;
                    }
                } else {
                    player->stateFlags2 &= ~0x10;
                    thisv->dyna.unk_150 = 0.0f;
                }
            } else {
                ObjOshihiki_SetupOnScene(thisv, globalCtx);
            }
        }
    } else {
        bgId = thisv->floorBgIds[thisv->highestFloor];
        if (bgId == BGCHECK_SCENE) {
            ObjOshihiki_SetupFall(thisv, globalCtx);
        } else {
            dynaPolyActor = DynaPoly_GetActor(&globalCtx->colCtx, bgId);

            if ((dynaPolyActor != NULL) && (dynaPolyActor->unk_15C & 1)) {
                func_800434A8(dynaPolyActor);
                func_80043538(dynaPolyActor);
                thisv->dyna.actor.world.pos.y = thisv->dyna.actor.floorHeight;
            } else {
                ObjOshihiki_SetupFall(thisv, globalCtx);
            }
        }
    }
}

void ObjOshihiki_SetupPush(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= PUSHBLOCK_SETUP_PUSH;
    thisv->actionFunc = ObjOshihiki_Push;
    thisv->dyna.actor.gravity = 0.0f;
}

void ObjOshihiki_Push(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    Actor* thisx = &thisv->dyna.actor;
    Player* player = GET_PLAYER(globalCtx);
    f32 pushDistSigned;
    s32 stopFlag;

    thisv->pushSpeed += 0.5f;
    thisv->stateFlags |= PUSHBLOCK_PUSH;
    thisv->pushSpeed = CLAMP_MAX(thisv->pushSpeed, 2.0f);
    stopFlag = Math_StepToF(&thisv->pushDist, 20.0f, thisv->pushSpeed);
    pushDistSigned = ((thisv->direction >= 0.0f) ? 1.0f : -1.0f) * thisv->pushDist;
    thisx->world.pos.x = thisx->home.pos.x + (pushDistSigned * thisv->yawSin);
    thisx->world.pos.z = thisx->home.pos.z + (pushDistSigned * thisv->yawCos);

    if (!ObjOshihiki_CheckFloor(thisv, globalCtx)) {
        thisx->home.pos.x = thisx->world.pos.x;
        thisx->home.pos.z = thisx->world.pos.z;
        player->stateFlags2 &= ~0x10;
        thisv->dyna.unk_150 = 0.0f;
        thisv->pushDist = 0.0f;
        thisv->pushSpeed = 0.0f;
        ObjOshihiki_SetupFall(thisv, globalCtx);
    } else if (stopFlag) {
        player = GET_PLAYER(globalCtx);
        if (ObjOshihiki_CheckWall(globalCtx, thisv->dyna.unk_158, thisv->dyna.unk_150, thisv)) {
            Audio_PlayActorSound2(thisx, NA_SE_EV_BLOCK_BOUND);
        }

        thisx->home.pos.x = thisx->world.pos.x;
        thisx->home.pos.z = thisx->world.pos.z;
        player->stateFlags2 &= ~0x10;
        thisv->dyna.unk_150 = 0.0f;
        thisv->pushDist = 0.0f;
        thisv->pushSpeed = 0.0f;
        thisv->timer = 10;
        if (thisv->floorBgIds[thisv->highestFloor] == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(thisv, globalCtx);
        } else {
            ObjOshihiki_SetupOnActor(thisv, globalCtx);
        }
    }
    Audio_PlayActorSound2(thisx, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void ObjOshihiki_SetupFall(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    thisv->stateFlags |= PUSHBLOCK_SETUP_FALL;
    thisv->dyna.actor.velocity.x = thisv->dyna.actor.velocity.y = thisv->dyna.actor.velocity.z = 0.0f;
    thisv->dyna.actor.gravity = -1.0f;
    ObjOshihiki_SetGround(thisv, globalCtx);
    thisv->actionFunc = ObjOshihiki_Fall;
}

void ObjOshihiki_Fall(ObjOshihiki* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->stateFlags |= PUSHBLOCK_FALL;
    if (fabsf(thisv->dyna.unk_150) > 0.001f) {
        thisv->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }
    Actor_MoveForward(&thisv->dyna.actor);
    if (ObjOshihiki_CheckGround(thisv, globalCtx)) {
        if (thisv->floorBgIds[thisv->highestFloor] == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(thisv, globalCtx);
        } else {
            ObjOshihiki_SetupOnActor(thisv, globalCtx);
        }
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        Audio_PlayActorSound2(&thisv->dyna.actor,
                              SurfaceType_GetSfx(&globalCtx->colCtx, thisv->floorPolys[thisv->highestFloor],
                                                 thisv->floorBgIds[thisv->highestFloor]) +
                                  SFX_FLAG);
    }
}

void ObjOshihiki_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjOshihiki* thisv = (ObjOshihiki*)thisx;

    thisv->stateFlags &=
        ~(PUSHBLOCK_SETUP_FALL | PUSHBLOCK_FALL | PUSHBLOCK_SETUP_PUSH | PUSHBLOCK_PUSH | PUSHBLOCK_SETUP_ON_ACTOR |
          PUSHBLOCK_ON_ACTOR | PUSHBLOCK_SETUP_ON_SCENE | PUSHBLOCK_ON_SCENE);
    thisv->stateFlags |= PUSHBLOCK_MOVE_UNDER;

    if (thisv->timer > 0) {
        thisv->timer--;
    }

    thisv->dyna.actor.world.rot.y = thisv->dyna.unk_158;

    thisv->yawSin = Math_SinS(thisv->dyna.actor.world.rot.y);
    thisv->yawCos = Math_CosS(thisv->dyna.actor.world.rot.y);

    if (thisv->actionFunc != NULL) {
        thisv->actionFunc(thisv, globalCtx);
    }
}

void ObjOshihiki_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjOshihiki* thisv = (ObjOshihiki*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_oshihiki.c", 1289);
    if (ObjOshihiki_MoveWithBlockUnder(thisv, globalCtx)) {
        Matrix_Translate(thisv->underDistX * 10.0f, 0.0f, thisv->underDistZ * 10.0f, MTXMODE_APPLY);
    }
    thisv->stateFlags &= ~PUSHBLOCK_MOVE_UNDER;
    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(thisv->texture));

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_oshihiki.c", 1308),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch (globalCtx->sceneNum) {
        case SCENE_YDAN:
        case SCENE_DDAN:
        case SCENE_BMORI1:
        case SCENE_HIDAN:
        case SCENE_MIZUSIN:
        case SCENE_JYASINZOU:
        case SCENE_HAKADAN:
        case SCENE_MEN:
            gDPSetEnvColor(POLY_OPA_DISP++, thisv->color.r, thisv->color.g, thisv->color.b, 255);
            break;
        default:
            gDPSetEnvColor(POLY_OPA_DISP++, mREG(13), mREG(14), mREG(15), 255);
            break;
    }

    gSPDisplayList(POLY_OPA_DISP++, gPushBlockDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_oshihiki.c", 1334);
}
