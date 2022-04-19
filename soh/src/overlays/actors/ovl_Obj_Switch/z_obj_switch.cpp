/*
 * File: z_obj_switch.c
 * Overlay: ovl_Obj_Switch
 * Description: Switches
 */

#include "z_obj_switch.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

// type:        (thisv->dyna.actor.params & 7)
// subtype:     (thisv->dyna.actor.params >> 4 & 7)
// switch flag: (thisv->dyna.actor.params >> 8 & 0x3F)
// frozen:      thisv->dyna.actor.params >> 7 & 1

void ObjSwitch_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjSwitch_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjSwitch_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjSwitch_FloorUpInit(ObjSwitch* thisv);
void ObjSwitch_FloorUp(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_FloorPressInit(ObjSwitch* thisv);
void ObjSwitch_FloorPress(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_FloorDownInit(ObjSwitch* thisv);
void ObjSwitch_FloorDown(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_FloorReleaseInit(ObjSwitch* thisv);
void ObjSwitch_FloorRelease(ObjSwitch* thisv, GlobalContext* globalCtx);

void ObjSwitch_EyeFrozenInit(ObjSwitch* thisv);
void ObjSwitch_EyeInit(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_EyeOpenInit(ObjSwitch* thisv);
void ObjSwitch_EyeOpen(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_EyeClosingInit(ObjSwitch* thisv);
void ObjSwitch_EyeClosing(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_EyeClosedInit(ObjSwitch* thisv);
void ObjSwitch_EyeClosed(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_EyeOpeningInit(ObjSwitch* thisv);
void ObjSwitch_EyeOpening(ObjSwitch* thisv, GlobalContext* globalCtx);

void ObjSwitch_CrystalOffInit(ObjSwitch* thisv);
void ObjSwitch_CrystalOff(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_CrystalTurnOnInit(ObjSwitch* thisv);
void ObjSwitch_CrystalTurnOn(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_CrystalOnInit(ObjSwitch* thisv);
void ObjSwitch_CrystalOn(ObjSwitch* thisv, GlobalContext* globalCtx);
void ObjSwitch_CrystalTurnOffInit(ObjSwitch* thisv);
void ObjSwitch_CrystalTurnOff(ObjSwitch* thisv, GlobalContext* globalCtx);

ActorInit Obj_Switch_InitVars = {
    ACTOR_OBJ_SWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(ObjSwitch),
    (ActorFunc)ObjSwitch_Init,
    (ActorFunc)ObjSwitch_Destroy,
    (ActorFunc)ObjSwitch_Update,
    (ActorFunc)ObjSwitch_Draw,
    NULL,
};

static f32 sHeights[] = { 10, 10, 0, 30, 30 };

static ColliderTrisElementInit D_80B9EC34[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, 20.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, -20.0f } } },
    },
};

static ColliderTrisInit sRustyFloorTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    D_80B9EC34,
};

static ColliderTrisElementInit D_80B9ECBC[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit trisColliderEye = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    D_80B9ECBC,
};

static ColliderJntSphElementInit D_80B9ED44[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 300, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sCyrstalJntSphereInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    D_80B9ED44,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2000, ICHAIN_STOP),
};

void ObjSwitch_RotateY(Vec3f* dest, Vec3f* src, s16 angle) {
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    dest->x = src->z * s + src->x * c;
    dest->y = src->y;
    dest->z = src->z * c - src->x * s;
}

void ObjSwitch_InitDynapoly(ObjSwitch* thisv, GlobalContext* globalCtx, const CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    const CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&thisv->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);

    if (thisv->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG registration failure"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_switch.c", 531,
                     thisv->dyna.actor.id, thisv->dyna.actor.params);
    }
}

void ObjSwitch_InitJntSphCollider(ObjSwitch* thisv, GlobalContext* globalCtx, ColliderJntSphInit* colliderJntSphInit) {
    ColliderJntSph* colliderJntSph = &thisv->jntSph.col;

    Collider_InitJntSph(globalCtx, colliderJntSph);
    Collider_SetJntSph(globalCtx, colliderJntSph, &thisv->dyna.actor, colliderJntSphInit, thisv->jntSph.items);
    Matrix_SetTranslateRotateYXZ(thisv->dyna.actor.world.pos.x,
                                 thisv->dyna.actor.world.pos.y +
                                     thisv->dyna.actor.shape.yOffset * thisv->dyna.actor.scale.y,
                                 thisv->dyna.actor.world.pos.z, &thisv->dyna.actor.shape.rot);
    Matrix_Scale(thisv->dyna.actor.scale.x, thisv->dyna.actor.scale.y, thisv->dyna.actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, colliderJntSph);
}

void ObjSwitch_InitTrisCollider(ObjSwitch* thisv, GlobalContext* globalCtx, ColliderTrisInit* colliderTrisInit) {
    ColliderTris* colliderTris = &thisv->tris.col;
    s32 i;
    s32 j;
    Vec3f pos[3];

    Collider_InitTris(globalCtx, colliderTris);
    Collider_SetTris(globalCtx, colliderTris, &thisv->dyna.actor, colliderTrisInit, thisv->tris.items);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            ObjSwitch_RotateY(&pos[j], &colliderTrisInit->elements[i].dim.vtx[j], thisv->dyna.actor.home.rot.y);
            Math_Vec3f_Sum(&pos[j], &thisv->dyna.actor.world.pos, &pos[j]);
        }

        Collider_SetTrisVertices(colliderTris, i, &pos[0], &pos[1], &pos[2]);
    }
}

Actor* ObjSwitch_SpawnIce(ObjSwitch* thisv, GlobalContext* globalCtx) {
    Actor* thisx = &thisv->dyna.actor;

    return Actor_SpawnAsChild(&globalCtx->actorCtx, thisx, globalCtx, ACTOR_OBJ_ICE_POLY, thisx->world.pos.x,
                              thisx->world.pos.y, thisx->world.pos.z, thisx->world.rot.x, thisx->world.rot.y,
                              thisx->world.rot.z, (thisv->dyna.actor.params >> 8 & 0x3F) << 8);
}

void ObjSwitch_SetOn(ObjSwitch* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 subType;

    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
        thisv->cooldownOn = false;
    } else {
        subType = (thisv->dyna.actor.params >> 4 & 7);
        Flags_SetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F));

        if (subType == 0 || subType == 4) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        } else {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }

        thisv->cooldownOn = true;
    }
}

void ObjSwitch_SetOff(ObjSwitch* thisv, GlobalContext* globalCtx) {
    thisv->cooldownOn = false;

    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
        Flags_UnsetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F));

        if ((thisv->dyna.actor.params >> 4 & 7) == 1) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
            thisv->cooldownOn = true;
        }
    }
}

void ObjSwitch_UpdateTwoTexScrollXY(ObjSwitch* thisv) {
    thisv->x1TexScroll = (thisv->x1TexScroll - 1) & 0x7F;
    thisv->y1TexScroll = (thisv->y1TexScroll + 1) & 0x7F;
    thisv->x2TexScroll = (thisv->x2TexScroll + 1) & 0x7F;
    thisv->y2TexScroll = (thisv->y2TexScroll - 1) & 0x7F;
}

void ObjSwitch_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjSwitch* thisv = (ObjSwitch*)thisx;
    s32 switchFlagSet;
    s32 type;

    switchFlagSet = Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F));
    type = (thisv->dyna.actor.params & 7);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        ObjSwitch_InitDynapoly(thisv, globalCtx, &gFloorSwitchCol, DPM_PLAYER);
    }

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        thisv->dyna.actor.world.pos.y = thisv->dyna.actor.home.pos.y + 1.0f;
    }

    Actor_SetFocus(&thisv->dyna.actor, sHeights[type]);

    if (type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        ObjSwitch_InitTrisCollider(thisv, globalCtx, &sRustyFloorTrisInit);
    } else if (type == OBJSWITCH_TYPE_EYE) {
        ObjSwitch_InitTrisCollider(thisv, globalCtx, &trisColliderEye);
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        ObjSwitch_InitJntSphCollider(thisv, globalCtx, &sCyrstalJntSphereInit);
    }

    if (type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        thisv->dyna.actor.flags |= ACTOR_FLAG_0;
        thisv->dyna.actor.targetMode = 4;
    }

    thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    if ((thisv->dyna.actor.params >> 7 & 1) && (ObjSwitch_SpawnIce(thisv, globalCtx) == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Error : 氷発生失敗 (%s %d)\n", "../z_obj_switch.c", 732);
        osSyncPrintf(VT_RST);
        thisv->dyna.actor.params &= ~0x80;
    }

    if (thisv->dyna.actor.params >> 7 & 1) {
        ObjSwitch_EyeFrozenInit(thisv);
    } else if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        if (switchFlagSet) {
            ObjSwitch_FloorDownInit(thisv);
        } else {
            ObjSwitch_FloorUpInit(thisv);
        }
    } else if (type == OBJSWITCH_TYPE_EYE) {
        if (switchFlagSet) {
            ObjSwitch_EyeClosedInit(thisv);
        } else {
            ObjSwitch_EyeOpenInit(thisv);
        }
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        if (switchFlagSet) {
            ObjSwitch_CrystalOnInit(thisv);
        } else {
            ObjSwitch_CrystalOffInit(thisv);
        }
    }

    osSyncPrintf("(Dungeon switch)(arg_data 0x%04x)\n", thisv->dyna.actor.params);
}

void ObjSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjSwitch* thisv = (ObjSwitch*)thisx;

    switch ((thisv->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            break;
    }

    switch ((thisv->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
        case OBJSWITCH_TYPE_EYE:
            Collider_DestroyTris(globalCtx, &thisv->tris.col);
            break;
        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            Collider_DestroyJntSph(globalCtx, &thisv->jntSph.col);
            break;
    }
}

void ObjSwitch_FloorUpInit(ObjSwitch* thisv) {
    thisv->dyna.actor.scale.y = 33.0f / 200.0f;
    thisv->actionFunc = ObjSwitch_FloorUp;
}

void ObjSwitch_FloorUp(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if ((thisv->dyna.actor.params & 7) == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        if (thisv->tris.col.base.acFlags & AC_HIT) {
            ObjSwitch_FloorPressInit(thisv);
            ObjSwitch_SetOn(thisv, globalCtx);
            thisv->tris.col.base.acFlags &= ~AC_HIT;
        } else {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->tris.col.base);
        }
    } else {
        switch ((thisv->dyna.actor.params >> 4 & 7)) {
            case OBJSWITCH_SUBTYPE_FLOOR_0:
                if (func_8004356C(&thisv->dyna)) {
                    ObjSwitch_FloorPressInit(thisv);
                    ObjSwitch_SetOn(thisv, globalCtx);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_1:
                if ((thisv->dyna.unk_160 & 2) && !(thisv->unk_17F & 2)) {
                    ObjSwitch_FloorPressInit(thisv);
                    ObjSwitch_SetOn(thisv, globalCtx);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_2:
                if (func_800435B4(&thisv->dyna)) {
                    ObjSwitch_FloorPressInit(thisv);
                    ObjSwitch_SetOn(thisv, globalCtx);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_3:
                if (func_800435B4(&thisv->dyna)) {
                    ObjSwitch_FloorPressInit(thisv);
                    ObjSwitch_SetOff(thisv, globalCtx);
                }
                break;
        }
    }
}

void ObjSwitch_FloorPressInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_FloorPress;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_FloorPress(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if ((thisv->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_FLOOR_3 || !thisv->cooldownOn ||
        func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        thisv->dyna.actor.scale.y -= 99.0f / 2000.0f;
        if (thisv->dyna.actor.scale.y <= 33.0f / 2000.0f) {
            ObjSwitch_FloorDownInit(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
        }
    }
}

void ObjSwitch_FloorDownInit(ObjSwitch* thisv) {
    thisv->dyna.actor.scale.y = 33.0f / 2000.0f;
    thisv->releaseTimer = 6;
    thisv->actionFunc = ObjSwitch_FloorDown;
}

void ObjSwitch_FloorDown(ObjSwitch* thisv, GlobalContext* globalCtx) {
    switch ((thisv->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_FLOOR_0:
            if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_FloorReleaseInit(thisv);
            }
            break;
        case OBJSWITCH_SUBTYPE_FLOOR_1:
            if ((thisv->dyna.unk_160 & 2) && !(thisv->unk_17F & 2)) {
                ObjSwitch_FloorReleaseInit(thisv);
                ObjSwitch_SetOff(thisv, globalCtx);
            }
            break;
        case OBJSWITCH_SUBTYPE_FLOOR_2:
        case OBJSWITCH_SUBTYPE_FLOOR_3:
            if (!func_800435B4(&thisv->dyna) && !Player_InCsMode(globalCtx)) {
                if (thisv->releaseTimer <= 0) {
                    ObjSwitch_FloorReleaseInit(thisv);
                    if ((thisv->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_FLOOR_2) {
                        ObjSwitch_SetOff(thisv, globalCtx);
                    } else {
                        ObjSwitch_SetOn(thisv, globalCtx);
                    }
                }
            } else {
                thisv->releaseTimer = 6;
            }
            break;
    }
}

void ObjSwitch_FloorReleaseInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_FloorRelease;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_FloorRelease(ObjSwitch* thisv, GlobalContext* globalCtx) {
    s16 subType = (thisv->dyna.actor.params >> 4 & 7);

    if (((subType != OBJSWITCH_SUBTYPE_FLOOR_1) && (subType != OBJSWITCH_SUBTYPE_FLOOR_3)) || !thisv->cooldownOn ||
        func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        thisv->dyna.actor.scale.y += 99.0f / 2000.0f;
        if (thisv->dyna.actor.scale.y >= 33.0f / 200.0f) {
            ObjSwitch_FloorUpInit(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            if (subType == OBJSWITCH_SUBTYPE_FLOOR_1) {
                func_800AA000(thisv->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
            }
        }
    }
}

s32 ObjSwitch_EyeIsHit(ObjSwitch* thisv) {
    Actor* collidingActor;
    s16 yawDiff;

    if ((thisv->tris.col.base.acFlags & AC_HIT) && !(thisv->unk_17F & 2)) {
        collidingActor = thisv->tris.col.base.ac;
        if (collidingActor != NULL) {
            yawDiff = collidingActor->world.rot.y - thisv->dyna.actor.shape.rot.y;
            if (ABS(yawDiff) > 0x5000) {
                return 1;
            }
        }
    }
    return 0;
}

void ObjSwitch_EyeFrozenInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_EyeInit;
}

void ObjSwitch_EyeInit(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
        ObjSwitch_EyeClosedInit(thisv);
    } else {
        ObjSwitch_EyeOpenInit(thisv);
    }
}

void ObjSwitch_EyeOpenInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_EyeOpen;
    thisv->eyeTexIndex = 0;
}

void ObjSwitch_EyeOpen(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if (ObjSwitch_EyeIsHit(thisv) || (thisv->dyna.actor.params >> 7 & 1)) {
        ObjSwitch_EyeClosingInit(thisv);
        ObjSwitch_SetOn(thisv, globalCtx);
        thisv->dyna.actor.params &= ~0x80;
    }
}

void ObjSwitch_EyeClosingInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_EyeClosing;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_EyeClosing(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if (!thisv->cooldownOn || func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        thisv->eyeTexIndex++;
        if (thisv->eyeTexIndex >= 3) {
            ObjSwitch_EyeClosedInit(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjSwitch_EyeClosedInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_EyeClosed;
    thisv->eyeTexIndex = 3;
}

void ObjSwitch_EyeClosed(ObjSwitch* thisv, GlobalContext* globalCtx) {
    switch ((thisv->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_EYE_0:
            if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_EyeOpeningInit(thisv);
                thisv->dyna.actor.params &= ~0x80;
            }
            break;
        case OBJSWITCH_SUBTYPE_EYE_1:
            if (ObjSwitch_EyeIsHit(thisv) || (thisv->dyna.actor.params >> 7 & 1)) {
                ObjSwitch_EyeOpeningInit(thisv);
                ObjSwitch_SetOff(thisv, globalCtx);
                thisv->dyna.actor.params &= ~0x80;
            }
            break;
    }
}

void ObjSwitch_EyeOpeningInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_EyeOpening;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_EyeOpening(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if ((thisv->dyna.actor.params >> 4 & 7) != OBJSWITCH_SUBTYPE_EYE_1 || !thisv->cooldownOn ||
        func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        thisv->eyeTexIndex--;
        if (thisv->eyeTexIndex <= 0) {
            ObjSwitch_EyeOpenInit(thisv);
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjSwitch_CrystalOffInit(ObjSwitch* thisv) {
    thisv->crystalColor.r = 0;
    thisv->crystalColor.g = 0;
    thisv->crystalColor.b = 0;
    thisv->crystalSubtype1texture = gCrstalSwitchRedTex;
    thisv->actionFunc = ObjSwitch_CrystalOff;
}

void ObjSwitch_CrystalOff(ObjSwitch* thisv, GlobalContext* globalCtx) {
    switch ((thisv->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_CRYSTAL_0:
            if ((thisv->jntSph.col.base.acFlags & AC_HIT) && thisv->disableAcTimer <= 0) {
                thisv->disableAcTimer = 10;
                ObjSwitch_SetOn(thisv, globalCtx);
                ObjSwitch_CrystalTurnOnInit(thisv);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_4:
            if (((thisv->jntSph.col.base.acFlags & AC_HIT) && thisv->disableAcTimer <= 0) ||
                Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
                thisv->disableAcTimer = 10;
                ObjSwitch_SetOn(thisv, globalCtx);
                ObjSwitch_CrystalTurnOnInit(thisv);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_1:
            if ((thisv->jntSph.col.base.acFlags & AC_HIT) && !(thisv->unk_17F & 2) && thisv->disableAcTimer <= 0) {
                thisv->disableAcTimer = 10;
                ObjSwitch_SetOn(thisv, globalCtx);
                ObjSwitch_CrystalTurnOnInit(thisv);
            }
            ObjSwitch_UpdateTwoTexScrollXY(thisv);
            break;
    }
}

void ObjSwitch_CrystalTurnOnInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_CrystalTurnOn;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_CrystalTurnOn(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if (!thisv->cooldownOn || func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        ObjSwitch_CrystalOnInit(thisv);
        if ((thisv->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_CRYSTAL_1) {
            ObjSwitch_UpdateTwoTexScrollXY(thisv);
        }
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void ObjSwitch_CrystalOnInit(ObjSwitch* thisv) {
    thisv->crystalColor.r = 255;
    thisv->crystalColor.g = 255;
    thisv->crystalColor.b = 255;
    thisv->crystalSubtype1texture = gCrstalSwitchBlueTex;
    thisv->actionFunc = ObjSwitch_CrystalOn;
}

void ObjSwitch_CrystalOn(ObjSwitch* thisv, GlobalContext* globalCtx) {
    switch ((thisv->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_CRYSTAL_0:
        case OBJSWITCH_SUBTYPE_CRYSTAL_4:
            if (!Flags_GetSwitch(globalCtx, (thisv->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_CrystalTurnOffInit(thisv);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_1:
            if ((thisv->jntSph.col.base.acFlags & AC_HIT) && !(thisv->unk_17F & 2) && thisv->disableAcTimer <= 0) {
                thisv->disableAcTimer = 10;
                globalCtx = globalCtx;
                ObjSwitch_CrystalTurnOffInit(thisv);
                ObjSwitch_SetOff(thisv, globalCtx);
            }
            break;
    }
    ObjSwitch_UpdateTwoTexScrollXY(thisv);
}

void ObjSwitch_CrystalTurnOffInit(ObjSwitch* thisv) {
    thisv->actionFunc = ObjSwitch_CrystalTurnOff;
    thisv->cooldownTimer = 100;
}

void ObjSwitch_CrystalTurnOff(ObjSwitch* thisv, GlobalContext* globalCtx) {
    if ((thisv->dyna.actor.params >> 4 & 7) != OBJSWITCH_SUBTYPE_CRYSTAL_1 || !thisv->cooldownOn ||
        func_8005B198() == thisv->dyna.actor.category || thisv->cooldownTimer <= 0) {
        ObjSwitch_CrystalOffInit(thisv);
        ObjSwitch_UpdateTwoTexScrollXY(thisv);
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void ObjSwitch_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjSwitch* thisv = (ObjSwitch*)thisx;

    if (thisv->releaseTimer > 0) {
        thisv->releaseTimer--;
    }
    if (thisv->cooldownTimer > 0) {
        thisv->cooldownTimer--;
    }

    thisv->actionFunc(thisv, globalCtx);

    switch ((thisv->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            thisv->unk_17F = thisv->dyna.unk_160;
            break;
        case OBJSWITCH_TYPE_EYE:
            thisv->unk_17F = thisv->tris.col.base.acFlags;
            thisv->tris.col.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->tris.col.base);
            break;
        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            if (!Player_InCsMode(globalCtx) && thisv->disableAcTimer > 0) {
                thisv->disableAcTimer--;
            }
            thisv->unk_17F = thisv->jntSph.col.base.acFlags;
            thisv->jntSph.col.base.acFlags &= ~AC_HIT;
            if (thisv->disableAcTimer <= 0) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->jntSph.col.base);
            }
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->jntSph.col.base);
            break;
    }
}

void ObjSwitch_DrawFloor(ObjSwitch* thisv, GlobalContext* globalCtx) {
    static const Gfx* floorSwitchDLists[] = { gFloorSwitch1DL, gFloorSwitch3DL, gFloorSwitch2DL, gFloorSwitch2DL };

    Gfx_DrawDListOpa(globalCtx, floorSwitchDLists[(thisv->dyna.actor.params >> 4 & 7)]);
}

void ObjSwitch_DrawFloorRusty(ObjSwitch* thisv, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gRustyFloorSwitchDL);
}

void ObjSwitch_DrawEye(ObjSwitch* thisv, GlobalContext* globalCtx) {
    static const void* eyeTextures[][4] = {
        { gEyeSwitchGoldOpenTex, gEyeSwitchGoldOpeningTex, gEyeSwitchGoldClosingTex, gEyeSwitchGoldClosedTex },
        { gEyeSwitchSilverOpenTex, gEyeSwitchSilverHalfTex, gEyeSwitchSilverClosedTex, gEyeSwitchSilverClosedTex },
    };
    static const Gfx* eyeDlists[] = { gEyeSwitch1DL, gEyeSwitch2DL };
    s32 pad;
    s32 subType = (thisv->dyna.actor.params >> 4 & 7);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1459);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1462),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[subType][thisv->eyeTexIndex]));
    gSPDisplayList(POLY_OPA_DISP++, eyeDlists[subType]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1471);
}

void ObjSwitch_DrawCrystal(ObjSwitch* thisv, GlobalContext* globalCtx) {
    static const Gfx* xluDLists[] = { gCrystalSwitchCoreXluDL, gCrystalSwitchDiamondXluDL, NULL, NULL,
                                gCrystalSwitchCoreXluDL };
    static const Gfx* opaDLists[] = { gCrystalSwitchCoreOpaDL, gCrystalSwitchDiamondOpaDL, NULL, NULL,
                                gCrystalSwitchCoreOpaDL };
    s32 pad1;
    s32 pad2;
    s32 subType;

    subType = (thisv->dyna.actor.params >> 4 & 7);
    func_8002ED80(&thisv->dyna.actor, globalCtx, 0);

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1494);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1497),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, xluDLists[subType]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1502);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1507);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1511),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (subType == OBJSWITCH_SUBTYPE_CRYSTAL_1) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(thisv->crystalSubtype1texture));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, thisv->crystalColor.r, thisv->crystalColor.g, thisv->crystalColor.b, 128);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, thisv->x1TexScroll, thisv->y1TexScroll, 0x20, 0x20, 1,
                                thisv->x2TexScroll, thisv->y2TexScroll, 0x20, 0x20));
    gSPDisplayList(POLY_OPA_DISP++, opaDLists[subType]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_switch.c", 1533);
}

static ObjSwitchActionFunc sDrawFuncs[] = {
    ObjSwitch_DrawFloor, ObjSwitch_DrawFloorRusty, ObjSwitch_DrawEye, ObjSwitch_DrawCrystal, ObjSwitch_DrawCrystal,
};

void ObjSwitch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjSwitch* thisv = (ObjSwitch*)thisx;

    sDrawFuncs[(thisv->dyna.actor.params & 7)](thisv, globalCtx);
}
