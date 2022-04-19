/*
 * File: z_obj_lightswitch.c
 * Overlay: ovl_Obj_Lightswitch
 * Description: Sun Emblem Trigger (Spirit Temple)
 */

#include "z_obj_lightswitch.h"
#include "vt.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"
#include "objects/object_lightswitch/object_lightswitch.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x00 */ FACE_EYES_CLOSED,
    /* 0x01 */ FACE_EYES_OPEN,
    /* 0x02 */ FACE_EYES_OPEN_SMILING
} FaceTextureIndex;

void ObjLightswitch_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjLightswitch_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjLightswitch_SetupOff(ObjLightswitch* thisv);
void ObjLightswitch_Off(ObjLightswitch* thisv, GlobalContext* globalCtx);
void ObjLightswitch_SetupTurnOn(ObjLightswitch* thisv);
void ObjLightswitch_TurnOn(ObjLightswitch* thisv, GlobalContext* globalCtx);
void ObjLightswitch_SetupOn(ObjLightswitch* thisv);
void ObjLightswitch_On(ObjLightswitch* thisv, GlobalContext* globalCtx);
void ObjLightswitch_SetupTurnOff(ObjLightswitch* thisv);
void ObjLightswitch_TurnOff(ObjLightswitch* thisv, GlobalContext* globalCtx);
void ObjLightswitch_SetupDisappearDelay(ObjLightswitch* thisv);
void ObjLightswitch_DisappearDelay(ObjLightswitch* thisv, GlobalContext* globalCtx);
void ObjLightswitch_SetupDisappear(ObjLightswitch* thisv);
void ObjLightswitch_Disappear(ObjLightswitch* thisv, GlobalContext* globalCtx);

ActorInit Obj_Lightswitch_InitVars = {
    ACTOR_OBJ_LIGHTSWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_LIGHTSWITCH,
    sizeof(ObjLightswitch),
    (ActorFunc)ObjLightswitch_Init,
    (ActorFunc)ObjLightswitch_Destroy,
    (ActorFunc)ObjLightswitch_Update,
    (ActorFunc)ObjLightswitch_Draw,
    NULL,
};

static ColliderJntSphElementInit sColliderJntSphElementInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00200000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 19 }, 100 },
    },
};
static ColliderJntSphInit sColliderJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sColliderJntSphElementInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

static const void* sFaceTextures[] = { object_lightswitch_Tex_000C20, object_lightswitch_Tex_000420,
                                 object_lightswitch_Tex_001420 };

static Vec3f D_80B97F68 = { -1707.0f, 843.0f, -180.0f };
static Vec3f D_80B97F74 = { 0.0f, 0.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void ObjLightswitch_InitCollider(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sColliderJntSphInit, thisv->colliderItems);
    Matrix_SetTranslateRotateYXZ(thisv->actor.world.pos.x,
                                 thisv->actor.world.pos.y + (thisv->actor.shape.yOffset * thisv->actor.scale.y),
                                 thisv->actor.world.pos.z, &thisv->actor.shape.rot);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, &thisv->collider);
}

void ObjLightswitch_SetSwitchFlag(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    Actor* thisx = &thisv->actor; // required
    s32 type;

    if (!Flags_GetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F)) {
        type = thisv->actor.params >> 4 & 3;

        Flags_SetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F);

        if (type == OBJLIGHTSWITCH_TYPE_1) {
            OnePointCutscene_AttentionSetSfx(globalCtx, thisx, NA_SE_SY_TRE_BOX_APPEAR);
        } else if (type == OBJLIGHTSWITCH_TYPE_BURN) {
            OnePointCutscene_AttentionSetSfx(globalCtx, thisx, NA_SE_SY_ERROR);
        } else {
            OnePointCutscene_AttentionSetSfx(globalCtx, thisx, NA_SE_SY_CORRECT_CHIME);
        }
    }
}

void ObjLightswitch_ClearSwitchFlag(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F)) {
        Flags_UnsetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F);

        if ((thisv->actor.params >> 4 & 3) == OBJLIGHTSWITCH_TYPE_1) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &thisv->actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void ObjLightswitch_SpawnDisappearEffects(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    f32 s = Math_SinS(thisv->actor.shape.rot.y);
    f32 c = Math_CosS(thisv->actor.shape.rot.y);
    f32 x;
    f32 y;
    f32 z;
    s32 pad;

    if (thisv->alpha >= (100 << 6)) {
        x = (CLAMP_MAX((1.0f - 1.0f / (255 << 6) * thisv->alpha) * 400.0f, 60.0f) - 30.0f + 30.0f) * Rand_ZeroOne();
        y = x - 30.0f;
        if (x > 30.0f) {
            x = 30.0f;
        } else {
            x = 900.0f - (y * y);
            if (x < 100.0f) {
                x = 100.0f;
            }
            x = sqrtf(x);
        }
        x = 2.0f * (x * (Rand_ZeroOne() - 0.5f));
        z = (30.0f - fabsf(x)) * 0.5f + 10.0f * Rand_ZeroOne();
        pos.x = thisv->actor.world.pos.x + ((z * s) + (x * c));
        pos.y = thisv->actor.world.pos.y + y + 10.0f;
        pos.z = thisv->actor.world.pos.z + ((z * c) - (x * s));
        EffectSsDeadDb_Spawn(globalCtx, &pos, &D_80B97F74, &D_80B97F74, 100, 0, 255, 255, 160, 160, 255, 0, 0, 1, 9,
                             true);
    }
}

void ObjLightswitch_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* thisv = (ObjLightswitch*)thisx;
    s32 switchFlagSet = Flags_GetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F);
    s32 removeSelf = false;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    Actor_SetFocus(&thisv->actor, 0.0f);
    if (switchFlagSet) {
        if ((thisv->actor.params >> 4 & 3) == OBJLIGHTSWITCH_TYPE_BURN) {
            removeSelf = true;
        } else {
            ObjLightswitch_SetupOn(thisv);
        }
    } else {
        ObjLightswitch_SetupOff(thisv);
    }
    if ((thisv->actor.params & 1) == 1) {
        if (switchFlagSet) {
            Math_Vec3f_Copy(&thisv->actor.world.pos, &D_80B97F68);
            Math_Vec3f_Copy(&thisv->actor.home.pos, &D_80B97F68);
        }
        thisv->actor.shape.rot.x = -0x4000;
        thisv->actor.shape.rot.z = 0;
        thisv->actor.world.rot.x = thisv->actor.home.rot.x = thisv->actor.shape.rot.x;
        thisv->actor.world.rot.z = thisv->actor.home.rot.z = thisv->actor.shape.rot.z;
        thisv->actor.flags |= ACTOR_FLAG_5;
        if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_OBJ_OSHIHIKI,
                               thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, 0,
                               thisv->actor.home.rot.y, 0, (0xFF << 8) | PUSHBLOCK_SMALL_START_ON) == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "Push-pull block occurrence failure"
            osSyncPrintf("押引ブロック発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_obj_lightswitch.c", 452,
                         thisv->actor.params);
            osSyncPrintf(VT_RST);
            removeSelf = true;
        }
    }
    ObjLightswitch_InitCollider(thisv, globalCtx);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, NULL, &sColChkInfoInit);
    if (removeSelf) {
        Actor_Kill(&thisv->actor);
    }
    // "Light switch"
    osSyncPrintf("(光スイッチ)(arg_data 0x%04x)\n", thisv->actor.params);
}

void ObjLightswitch_Destroy(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjLightswitch* thisv = (ObjLightswitch*)thisx;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void ObjLightswitch_SetupOff(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_Off;
    thisv->faceTextureIndex = FACE_EYES_CLOSED;
    thisv->color[0] = 155 << 6;
    thisv->color[1] = 125 << 6;
    thisv->color[2] = 255 << 6;
    thisv->alpha = 255 << 6;
}

void ObjLightswitch_Off(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->actor.params >> 4 & 3) {
        case OBJLIGHTSWITCH_TYPE_STAY_ON:
        case OBJLIGHTSWITCH_TYPE_2:
            if (thisv->collider.base.acFlags & AC_HIT) {
                ObjLightswitch_SetupTurnOn(thisv);
                ObjLightswitch_SetSwitchFlag(thisv, globalCtx);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_1:
            if ((thisv->collider.base.acFlags & AC_HIT) && !(thisv->prevFrameACflags & AC_HIT)) {
                ObjLightswitch_SetupTurnOn(thisv);
                ObjLightswitch_SetSwitchFlag(thisv, globalCtx);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_BURN:
            if (thisv->collider.base.acFlags & AC_HIT) {
                ObjLightswitch_SetupDisappearDelay(thisv);
                ObjLightswitch_SetSwitchFlag(thisv, globalCtx);
            }
            break;
    }
}

void ObjLightswitch_SetupTurnOn(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_TurnOn;
    thisv->toggleDelay = 100;
    thisv->timer = 0;
    thisv->faceTextureIndex = FACE_EYES_CLOSED;
}

void ObjLightswitch_TurnOn(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    if (func_8005B198() == thisv->actor.category || thisv->toggleDelay <= 0) {
        if (thisv->timer == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_TRIFORCE_FLASH);
        }

        thisv->timer++;

        Math_StepToS(&thisv->flameRingRotSpeed, -0xAA, 0xA);
        thisv->flameRingRot += thisv->flameRingRotSpeed;

        thisv->color[0] = thisv->timer * (((255 - 155) << 6) / 20) + (155 << 6);
        thisv->color[1] = thisv->timer * (((255 - 125) << 6) / 20) + (125 << 6);

        if (thisv->timer >= 20) {
            ObjLightswitch_SetupOn(thisv);
        } else if (thisv->timer == 15) {
            thisv->faceTextureIndex = FACE_EYES_OPEN;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjLightswitch_SetupOn(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_On;
    thisv->faceTextureIndex = FACE_EYES_OPEN_SMILING;

    thisv->color[0] = 255 << 6;
    thisv->color[1] = 255 << 6;
    thisv->color[2] = 255 << 6;
    thisv->alpha = 255 << 6;

    thisv->flameRingRotSpeed = -0xAA;
    thisv->timer = 0;
}

void ObjLightswitch_On(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    switch (thisv->actor.params >> 4 & 3) {
        case OBJLIGHTSWITCH_TYPE_STAY_ON:
            if (!Flags_GetSwitch(globalCtx, thisv->actor.params >> 8 & 0x3F)) {
                ObjLightswitch_SetupTurnOff(thisv);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_1:
            if (thisv->collider.base.acFlags & AC_HIT && !(thisv->prevFrameACflags & AC_HIT)) {
                ObjLightswitch_SetupTurnOff(thisv);
                ObjLightswitch_ClearSwitchFlag(thisv, globalCtx);
            }
            break;
        case OBJLIGHTSWITCH_TYPE_2:
            if (!(thisv->collider.base.acFlags & AC_HIT)) {
                if (thisv->timer >= 7) {
                    ObjLightswitch_SetupTurnOff(thisv);
                    ObjLightswitch_ClearSwitchFlag(thisv, globalCtx);
                } else {
                    thisv->timer++;
                }
            } else {
                thisv->timer = 0;
            }
            break;
    }
    thisv->flameRingRot += thisv->flameRingRotSpeed;
}

void ObjLightswitch_SetupTurnOff(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_TurnOff;
    thisv->toggleDelay = 100;
    thisv->timer = 20;
    thisv->faceTextureIndex = FACE_EYES_OPEN;
}

void ObjLightswitch_TurnOff(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    if ((thisv->actor.params >> 4 & 3) != OBJLIGHTSWITCH_TYPE_1 || func_8005B198() == thisv->actor.category ||
        thisv->toggleDelay <= 0) {
        thisv->timer--;

        Math_StepToS(&thisv->flameRingRotSpeed, 0, 0xA);
        thisv->flameRingRot += thisv->flameRingRotSpeed;

        thisv->color[0] = thisv->timer * (((255 - 155) << 6) / 20) + (155 << 6);
        thisv->color[1] = thisv->timer * (((255 - 125) << 6) / 20) + (125 << 6);

        if (thisv->timer <= 0) {
            ObjLightswitch_SetupOff(thisv);
        } else if (thisv->timer == 15) {
            thisv->faceTextureIndex = FACE_EYES_CLOSED;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjLightswitch_SetupDisappearDelay(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_DisappearDelay;
    thisv->toggleDelay = 100;
}

void ObjLightswitch_DisappearDelay(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    if (func_8005B198() == thisv->actor.category || thisv->toggleDelay <= 0) {
        ObjLightswitch_SetupDisappear(thisv);
    }
}

void ObjLightswitch_SetupDisappear(ObjLightswitch* thisv) {
    thisv->actionFunc = ObjLightswitch_Disappear;
    thisv->alpha = 255 << 6;
}

void ObjLightswitch_Disappear(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    thisv->alpha -= 200;
    ObjLightswitch_SpawnDisappearEffects(thisv, globalCtx);
    if (thisv->alpha < 0) {
        Actor_Kill(&thisv->actor);
    }
}

void ObjLightswitch_Update(Actor* thisx, GlobalContext* globalCtx2) {
    ObjLightswitch* thisv = (ObjLightswitch*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    if (thisv->toggleDelay > 0) {
        thisv->toggleDelay--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (thisv->actor.update != NULL) {
        if ((thisv->actor.params & 1) == 1) {
            thisv->actor.world.pos.x = thisv->actor.child->world.pos.x;
            thisv->actor.world.pos.y = thisv->actor.child->world.pos.y + 60.0f;
            thisv->actor.world.pos.z = thisv->actor.child->world.pos.z;
            Actor_SetFocus(&thisv->actor, 0.0f);
        }

        thisv->prevFrameACflags = thisv->collider.base.acFlags;
        thisv->collider.base.acFlags &= ~AC_HIT;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void ObjLightswitch_DrawOpa(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    Actor* child;
    Vec3f pos;
    Vec3s rot;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 809);
    func_80093D18(globalCtx->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, (u8)(thisv->color[0] >> 6), (u8)(thisv->color[1] >> 6), (u8)(thisv->color[2] >> 6),
                   (u8)(thisv->alpha >> 6));
    gSPSegment(POLY_OPA_DISP++, 0x09, &D_80116280[2]);

    if ((thisv->actor.params & 1) == 1) {
        child = thisv->actor.child;
        thisv->actor.world.pos.x = child->world.pos.x;
        thisv->actor.world.pos.y = child->world.pos.y + 60.0f;
        thisv->actor.world.pos.z = child->world.pos.z;
        Math_Vec3f_Copy(&pos, &thisv->actor.world.pos);
        Matrix_SetTranslateRotateYXZ(pos.x, pos.y, pos.z, &thisv->actor.shape.rot);
        Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    } else {
        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + thisv->actor.shape.yOffset * thisv->actor.scale.y;
        pos.z = thisv->actor.world.pos.z;
    }

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 841),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sFaceTextures[thisv->faceTextureIndex]));
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000260);

    rot.x = thisv->actor.shape.rot.x;
    rot.y = thisv->actor.shape.rot.y;
    rot.z = thisv->actor.shape.rot.z + thisv->flameRingRot;
    Matrix_SetTranslateRotateYXZ(pos.x, pos.y, pos.z, &rot);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 859),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000398);

    rot.z = thisv->actor.shape.rot.z - thisv->flameRingRot;
    Matrix_SetTranslateRotateYXZ(pos.x, pos.y, pos.z, &rot);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 873),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 878);
}

void ObjLightswitch_DrawXlu(ObjLightswitch* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp68;
    Vec3s sp60;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 890);
    func_80093D84(globalCtx->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(thisv->color[0] >> 6), (u8)(thisv->color[1] >> 6), (u8)(thisv->color[2] >> 6),
                   (u8)(thisv->alpha >> 6));
    gSPSegment(POLY_XLU_DISP++, 0x09, D_80116280);

    sp68.x = thisv->actor.world.pos.x;
    sp68.y = thisv->actor.world.pos.y + (thisv->actor.shape.yOffset * thisv->actor.scale.y);
    sp68.z = thisv->actor.world.pos.z;

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 912),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sFaceTextures[thisv->faceTextureIndex]));
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000260);

    sp60.x = thisv->actor.shape.rot.x;
    sp60.y = thisv->actor.shape.rot.y;
    sp60.z = thisv->actor.shape.rot.z + thisv->flameRingRot;

    Matrix_SetTranslateRotateYXZ(sp68.x, sp68.y, sp68.z, &sp60);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 930),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000398);

    sp60.z = thisv->actor.shape.rot.z - thisv->flameRingRot;
    Matrix_SetTranslateRotateYXZ(sp68.x, sp68.y, sp68.z, &sp60);
    Matrix_Scale(thisv->actor.scale.x, thisv->actor.scale.y, thisv->actor.scale.z, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 944),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_lightswitch_DL_000408);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_obj_lightswitch.c", 949);
}

void ObjLightswitch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjLightswitch* thisv = (ObjLightswitch*)thisx;
    s32 alpha = thisv->alpha >> 6 & 0xFF;

    if ((thisv->actor.params & 1) == 1) {
        Collider_UpdateSpheres(0, &thisv->collider);
    }

    if ((thisv->actor.params >> 4 & 3) == OBJLIGHTSWITCH_TYPE_BURN && (alpha > 0 || alpha < 255)) {
        ObjLightswitch_DrawXlu(thisv, globalCtx);
    } else {
        ObjLightswitch_DrawOpa(thisv, globalCtx);
    }
}
