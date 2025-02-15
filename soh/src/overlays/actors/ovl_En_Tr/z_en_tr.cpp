/*
 * File: z_en_tr.c
 * Overlay: ovl_En_Tr
 * Description: Koume and Kotake
 */

#include "z_en_tr.h"
#include "objects/object_tr/object_tr.h"

#define FLAGS ACTOR_FLAG_4

void EnTr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTr_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTr_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnTr_DoNothing(EnTr* thisv, GlobalContext* globalCtx);
void EnTr_ShrinkVanish(EnTr* thisv, GlobalContext* globalCtx);
void EnTr_WaitToReappear(EnTr* thisv, GlobalContext* globalCtx);
void EnTr_ChooseAction1(EnTr* thisv, GlobalContext* globalCtx);

void EnTr_UpdateRotation(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex);
void func_80B24038(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex);
void EnTr_SetStartPosRot(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex);

ActorInit En_Tr_InitVars = {
    ACTOR_EN_TR,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TR,
    sizeof(EnTr),
    (ActorFunc)EnTr_Init,
    (ActorFunc)EnTr_Destroy,
    (ActorFunc)EnTr_Update,
    (ActorFunc)EnTr_Draw,
    NULL,
};

// The first elements of these animation arrays are for Koume, the second for Kotake

static const AnimationHeader* unused[] = {
    &object_tr_Anim_003FC8,
    &object_tr_Anim_001CDC,
};

static const AnimationHeader* D_80B24368[] = {
    &object_tr_Anim_002BC4,
    &object_tr_Anim_000BFC,
};

static const AnimationHeader* D_80B24370[] = {
    &object_tr_Anim_0035CC,
    &object_tr_Anim_0013CC,
};

static const AnimationHeader* D_80B24378[] = {
    &object_tr_Anim_0049C8,
    &object_tr_Anim_0049C8,
};

static const AnimationHeader* D_80B24380[] = {
    &object_tr_Anim_012E1C,
    &object_tr_Anim_012E1C,
};

static f32 D_80B24388[] = { 0.0f, 20.0f, -30.0f, 20.0f, -20.0f, -20.0f, 30.0f };

static f32 D_80B243A4[] = { 0.0f, 30.0f, 0.0f, -30.0f, 30.0f, -30.0f, 0.0f };

// Has to be 1-dimensional to match
static Color_RGBA8 D_80B243C0[4] = {
    { 255, 200, 0, 255 },
    { 255, 0, 0, 255 },
    { 255, 255, 255, 255 },
    { 0, 0, 255, 255 },
};

static const void* sEyeTextures[] = {
    object_tr_Tex_0086D8,
    object_tr_Tex_0094D8,
    object_tr_Tex_0098D8,
};

void EnTr_SetupAction(EnTr* thisv, EnTrActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnTr_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnTr* thisv = (EnTr*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnTr_SetupAction(thisv, EnTr_DoNothing);
    thisv->unk_2D4 = 0; // Set and not used
    thisv->actor.child = NULL;
    Actor_SetScale(&thisv->actor, 0.01f);

    switch (thisv->actor.params) {
        case TR_KOUME:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_tr_Skel_011688, &object_tr_Anim_003FC8,
                               thisv->jointTable, thisv->morphTable, 27);
            Animation_PlayOnce(&thisv->skelAnime, &object_tr_Anim_003FC8);
            thisv->animation = NULL;
            EnTr_SetupAction(thisv, EnTr_ChooseAction1);
            thisv->actionIndex = 3;
            break;

        case TR_KOTAKE:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_tr_Skel_00C530, &object_tr_Anim_001CDC,
                               thisv->jointTable, thisv->morphTable, 27);
            Animation_PlayOnce(&thisv->skelAnime, &object_tr_Anim_001CDC);
            thisv->animation = NULL;
            EnTr_SetupAction(thisv, EnTr_ChooseAction1);
            thisv->actionIndex = 2;
            break;

        default:
            ASSERT(0, "0", "../z_en_tr.c", 277);
            break;
    }
}

void EnTr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnTr_CrySpellcast(EnTr* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 11) {
        // Both cry in the title screen cutscene, but only Kotake in the in-game cutscene
        if ((thisv->actor.params != TR_KOUME) || (gSaveContext.sceneSetupIndex == 6)) {
            Audio_PlaySoundGeneral(NA_SE_EN_TWINROBA_SHOOT_VOICE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    } else if (thisv->actor.child != NULL) {
        thisv->actor.child = NULL;
    }
    func_8002F974(&thisv->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

void EnTr_DoNothing(EnTr* thisv, GlobalContext* globalCtx) {
}

void EnTr_ChooseAction2(EnTr* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if (globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) {
            switch (globalCtx->csCtx.npcActions[thisv->actionIndex]->action) {

                case 4:
                    Actor_SetScale(&thisv->actor, 0.01f);
                    EnTr_SetupAction(thisv, EnTr_ShrinkVanish);
                    thisv->timer = 24;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD2);
                    break;

                case 6:
                    Animation_Change(&thisv->skelAnime, D_80B24380[thisv->actor.params], 1.0f, 0.0f,
                                     Animation_GetLastFrame(D_80B24380[thisv->actor.params]), ANIMMODE_ONCE, -5.0f);
                    EnTr_SetupAction(thisv, EnTr_CrySpellcast);
                    thisv->animation = D_80B24378[thisv->actor.params];
                    thisv->timer = 39;
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_6K,
                                       thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0,
                                       0, thisv->actor.params + 9);
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_MASIC1);
                    break;

                default:
                    func_80B24038(thisv, globalCtx, thisv->actionIndex);
                    EnTr_UpdateRotation(thisv, globalCtx, thisv->actionIndex);
                    break;
            }
            func_8002F974(&thisv->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
        }
    }
}

void EnTr_FlyKidnapCutscene(EnTr* thisv, GlobalContext* globalCtx) {
    Vec3f originalPos = thisv->actor.world.pos;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if (globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) {
            if (globalCtx->csCtx.npcActions[thisv->actionIndex]->action == 8) {
                func_80B24038(thisv, globalCtx, thisv->actionIndex);
                thisv->actor.world.rot.y = Math_Atan2S(thisv->actor.velocity.z, thisv->actor.velocity.x);
                Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 10, 0x400, 0x100);
                thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
            } else {
                EnTr_SetStartPosRot(thisv, globalCtx, thisv->actionIndex);
                thisv->actor.world.pos.x += Math_SinS(thisv->timer) * 150.0f;
                thisv->actor.world.pos.y += -100.0f;
                thisv->actor.world.pos.z += Math_CosS(thisv->timer) * 150.0f;

                thisv->actor.shape.rot.y = (s16)(thisv->timer) + 0x4000;
                thisv->timer += 0x400;

                thisv->actor.velocity.x = thisv->actor.world.pos.x - originalPos.x;
                thisv->actor.velocity.y = thisv->actor.world.pos.y - originalPos.y;
                thisv->actor.velocity.z = thisv->actor.world.pos.z - originalPos.z;
            }

            if (globalCtx->csCtx.frames < 670) {
                func_8002F974(&thisv->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
            }
        }
    }
}

void func_80B23254(EnTr* thisv, GlobalContext* globalCtx, s32 arg2, f32 arg3, f32 scale) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Vec3f sp58;
    Color_RGBA8* primColor;
    Color_RGBA8* envColor;
    Vec3f cameraEye = GET_ACTIVE_CAM(globalCtx)->eye;
    s16 yaw = Math_Vec3f_Yaw(&cameraEye, &thisv->actor.world.pos);
    s16 reversePitch = -Math_Vec3f_Pitch(&cameraEye, &thisv->actor.world.pos);
    f32 sp3C;

    accel.x = accel.z = 0.0f;
    sp3C = Math_SinS(yaw);
    velocity.x = Math_CosS(reversePitch) * (arg3 * sp3C);
    velocity.y = Math_SinS(reversePitch) * arg3;
    sp3C = Math_CosS(yaw);
    velocity.z = Math_CosS(reversePitch) * (arg3 * sp3C);
    accel.y = 0.5f;

    primColor = &D_80B243C0[2 * thisv->actor.params];
    envColor = &D_80B243C0[2 * thisv->actor.params + 1];

    sp58 = thisv->actor.world.pos;
    sp58.x -= velocity.x * 10.0f;
    sp58.y -= velocity.y * 10.0f;
    sp58.z -= velocity.z * 10.0f;

    pos.x = sp58.x + ((D_80B24388[arg2] * scale) * Math_CosS(yaw));
    pos.y = sp58.y + (D_80B243A4[arg2] * scale);
    pos.z = sp58.z - ((D_80B24388[arg2] * scale) * Math_SinS(yaw));
    func_8002829C(globalCtx, &pos, &velocity, &accel, primColor, envColor, (s32)(800.0f * scale), (s32)(80.0f * scale));
}

void EnTr_ShrinkVanish(EnTr* thisv, GlobalContext* globalCtx) {
    if (thisv->timer >= 17) {
        thisv->actor.shape.rot.y = (thisv->actor.shape.rot.y - (thisv->timer * 0x28F)) + 0x3D68;
    } else {
        if (thisv->timer >= 5) {
            Actor_SetScale(&thisv->actor, thisv->actor.scale.x * 0.9f);
            thisv->actor.shape.rot.y = (thisv->actor.shape.rot.y - (thisv->timer * 0x28F)) + 0x3D68;
        } else if (thisv->timer > 0) {
            s32 temp_hi = (thisv->timer * 2) % 7;

            func_80B23254(thisv, globalCtx, temp_hi, 5.0f, 0.2f);
            func_80B23254(thisv, globalCtx, (temp_hi + 1) % 7, 5.0f, 0.2f);
            Actor_SetScale(&thisv->actor, thisv->actor.scale.x * 0.9f);
            thisv->actor.shape.rot.y = (thisv->actor.shape.rot.y - (thisv->timer * 0x28F)) + 0x3D68;
        } else {
            EnTr_SetupAction(thisv, EnTr_WaitToReappear);
            thisv->actor.draw = NULL;
        }
    }

    if (thisv->timer == 4) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BUBLE_DOWN);
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    }
}

void EnTr_Reappear(EnTr* thisv, GlobalContext* globalCtx) {
    if (thisv->timer >= 31) {
        s32 temp_hi = (thisv->timer * 2) % 7;

        func_80B23254(thisv, globalCtx, temp_hi, 5.0f, 1.0f);
        func_80B23254(thisv, globalCtx, (temp_hi + 1) % 7, 5.0f, 1.0f);
    } else if (thisv->timer == 30) {
        thisv->actor.draw = EnTr_Draw;
        thisv->actor.shape.rot.y += thisv->timer * 0x1A6;
    } else if (thisv->timer > 0) {
        thisv->actor.shape.rot.y += thisv->timer * 0x1A6;
        Actor_SetScale(&thisv->actor, (thisv->actor.scale.x * 0.8f) + 0.002f);
    } else {
        EnTr_SetupAction(thisv, EnTr_ChooseAction2);
        Actor_SetScale(&thisv->actor, 0.01f);
    }

    if (thisv->timer > 0) {
        thisv->timer--;
    }
    func_8002F974(&thisv->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

void EnTr_WaitToReappear(EnTr* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if ((globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) &&
            ((globalCtx->csCtx.npcActions[thisv->actionIndex]->action == 3) ||
             (globalCtx->csCtx.npcActions[thisv->actionIndex]->action == 5))) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_TRANSFORM);
            thisv->timer = 34;
            EnTr_SetStartPosRot(thisv, globalCtx, thisv->actionIndex);
            EnTr_SetupAction(thisv, EnTr_Reappear);
            Animation_PlayLoop(&thisv->skelAnime, &object_tr_Anim_0049C8);
            thisv->animation = NULL;
            Actor_SetScale(&thisv->actor, 0.003f);
        }
    }
}

void EnTr_TakeOff(EnTr* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(D_80B24378[thisv->actor.params]);

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if ((globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) &&
            (globalCtx->csCtx.npcActions[thisv->actionIndex]->action == 3)) {
            Animation_Change(&thisv->skelAnime, D_80B24378[thisv->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP,
                             -10.0f);
            thisv->animation = NULL;
            EnTr_SetupAction(thisv, EnTr_ChooseAction2);
        }
    }
}

void EnTr_TurnLookOverShoulder(EnTr* thisv, GlobalContext* globalCtx) {
    f32 lastFrame = Animation_GetLastFrame(D_80B24368[thisv->actor.params]);

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if ((globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) &&
            (globalCtx->csCtx.npcActions[thisv->actionIndex]->action == 2)) {
            Animation_Change(&thisv->skelAnime, D_80B24368[thisv->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             -4.0f);
            thisv->animation = D_80B24370[thisv->actor.params];
            EnTr_SetupAction(thisv, EnTr_TakeOff);
        }
    }
}

void EnTr_ChooseAction1(EnTr* thisv, GlobalContext* globalCtx) {
    u32 frames = globalCtx->gameplayFrames;

    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        if (globalCtx->csCtx.npcActions[thisv->actionIndex] != NULL) {
            switch (globalCtx->csCtx.npcActions[thisv->actionIndex]->action) {
                case 1:
                    EnTr_SetStartPosRot(thisv, globalCtx, thisv->actionIndex);
                    EnTr_SetupAction(thisv, EnTr_TurnLookOverShoulder);
                    break;

                case 3:
                    EnTr_SetStartPosRot(thisv, globalCtx, thisv->actionIndex);
                    EnTr_SetupAction(thisv, EnTr_ChooseAction2);
                    Animation_PlayLoop(&thisv->skelAnime, &object_tr_Anim_0049C8);
                    thisv->animation = NULL;
                    break;

                case 4:
                    EnTr_SetupAction(thisv, EnTr_WaitToReappear);
                    thisv->actor.draw = NULL;
                    break;

                case 7:
                    EnTr_SetupAction(thisv, EnTr_FlyKidnapCutscene);
                    Animation_PlayLoop(&thisv->skelAnime, &object_tr_Anim_0049C8);
                    thisv->animation = NULL;
                    thisv->timer =
                        ((thisv->actor.params != TR_KOUME) ? ((u8)frames * 0x400) + 0x8000 : (u8)frames * 0x400);
                    break;
            }
        }
    }
}

void EnTr_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnTr* thisv = (EnTr*)thisx;

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 0.0f, 0.0f, 5);
    thisv->actionFunc(thisv, globalCtx);

    if (SkelAnime_Update(&thisv->skelAnime) != 0) {
        if (thisv->animation != NULL) {
            if ((thisv->animation == &object_tr_Anim_0035CC) || (thisv->animation == &object_tr_Anim_0013CC)) {
                if (thisv->actor.params != TR_KOUME) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_LAUGH2);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_TWINROBA_LAUGH);
                }
                Animation_PlayLoop(&thisv->skelAnime, thisv->animation);
            } else if (thisv->animation == &object_tr_Anim_0049C8) {
                EnTr_SetupAction(thisv, EnTr_ChooseAction2);
                Animation_Change(&thisv->skelAnime, &object_tr_Anim_0049C8, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&object_tr_Anim_0049C8), ANIMMODE_LOOP, -5.0f);
            } else {
                Animation_PlayLoop(&thisv->skelAnime, thisv->animation);
            }
            thisv->animation = NULL;
        } else {
            thisv->skelAnime.curFrame = 0.0f;
        }
    }
    Actor_SetFocus(&thisv->actor, 0.0f);

    if (DECR(thisv->blinkTimer) == 0) {
        thisv->blinkTimer = Rand_S16Offset(60, 60);
    }
    thisv->eyeIndex = thisv->blinkTimer;
    if (thisv->eyeIndex >= 3) {
        thisv->eyeIndex = 0;
    }
}

s32 EnTr_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f src = { 2300.0f, 0.0f, -600.0f };
    Vec3f dest = { 0.0f, 0.0f, 0.0f };
    EnTr* thisv = (EnTr*)thisx;
    Actor* child = thisv->actor.child;

    if ((child != NULL) && (limbIndex == 19)) {
        Matrix_MultVec3f(&src, &dest);
        dest.x -= (10.0f * Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx))));
        dest.z -= (10.0f * Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx))));
        child->world.pos = dest;
    }
    return 0;
}

void EnTr_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnTr* thisv = (EnTr*)thisx;

    if (1) {}

    if ((globalCtx->csCtx.state == CS_STATE_IDLE) || (globalCtx->csCtx.npcActions[thisv->actionIndex] == 0)) {
        thisv->actor.shape.shadowDraw = NULL;
    } else {
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_tr.c", 840);
        func_800943C8(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[thisv->eyeIndex]));
        func_8002EBCC(&thisv->actor, globalCtx, 0);
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnTr_OverrideLimbDraw, NULL, thisv);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_tr.c", 854);
    }
}

f32 func_80B23FDC(GlobalContext* globalCtx, s32 actionIndex) {
    f32 phi_f2 = Environment_LerpWeight(globalCtx->csCtx.npcActions[actionIndex]->endFrame,
                                        globalCtx->csCtx.npcActions[actionIndex]->startFrame, globalCtx->csCtx.frames);
    phi_f2 = CLAMP_MAX(phi_f2, 1.0f);
    return phi_f2;
}

void func_80B24038(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex) {
    Vec3f startPos;
    Vec3f endPos;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 phi_f12;

    startPos.x = globalCtx->csCtx.npcActions[actionIndex]->startPos.x;
    startPos.y = globalCtx->csCtx.npcActions[actionIndex]->startPos.y;
    startPos.z = globalCtx->csCtx.npcActions[actionIndex]->startPos.z;

    endPos.x = globalCtx->csCtx.npcActions[actionIndex]->endPos.x;
    endPos.y = globalCtx->csCtx.npcActions[actionIndex]->endPos.y;
    endPos.z = globalCtx->csCtx.npcActions[actionIndex]->endPos.z;

    temp_f0 = func_80B23FDC(globalCtx, actionIndex);

    startPos.x = ((endPos.x - startPos.x) * temp_f0) + startPos.x;
    startPos.y = ((endPos.y - startPos.y) * temp_f0) + startPos.y;
    startPos.z = ((endPos.z - startPos.z) * temp_f0) + startPos.z;

    endPos.x = (startPos.x - thisv->actor.world.pos.x) * 0.1f;
    endPos.y = (startPos.y - thisv->actor.world.pos.y) * 0.1f;
    endPos.z = (startPos.z - thisv->actor.world.pos.z) * 0.1f;

    temp_f0_2 = sqrtf(SQ(endPos.x) + SQ(endPos.y) + SQ(endPos.z));
    phi_f12 = CLAMP(temp_f0_2, 0.0f, 20.0f);

    if ((temp_f0_2 != phi_f12) && (temp_f0_2 != 0.0f)) {
        endPos.x *= phi_f12 / temp_f0_2;
        endPos.y *= phi_f12 / temp_f0_2;
        endPos.z *= phi_f12 / temp_f0_2;
    }

    Math_StepToF(&thisv->actor.velocity.x, endPos.x, 1.0f);
    Math_StepToF(&thisv->actor.velocity.y, endPos.y, 1.0f);
    Math_StepToF(&thisv->actor.velocity.z, endPos.z, 1.0f);
    func_8002D7EC(&thisv->actor);
}

void EnTr_UpdateRotation(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex) {
    s16 rotY = globalCtx->csCtx.npcActions[actionIndex]->rot.y;
    s32 rotDiff = thisv->actor.world.rot.y - rotY;
    s32 rotSign;

    if (rotDiff < 0) {
        rotDiff = -rotDiff;
        rotSign = 1;
    } else {
        rotSign = -1;
    }

    if (rotDiff >= 0x8000) {
        rotSign = -rotSign;
        rotDiff = 0x10000 - rotDiff;
    }

    rotDiff *= 0.1f;

    thisv->actor.world.rot.y += rotDiff * rotSign;
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
}

void EnTr_SetStartPosRot(EnTr* thisv, GlobalContext* globalCtx, s32 actionIndex) {
    Vec3f startPos;

    startPos.x = globalCtx->csCtx.npcActions[actionIndex]->startPos.x;
    startPos.y = globalCtx->csCtx.npcActions[actionIndex]->startPos.y;
    startPos.z = globalCtx->csCtx.npcActions[actionIndex]->startPos.z;

    thisv->actor.world.pos = startPos;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y = globalCtx->csCtx.npcActions[actionIndex]->rot.y;
}
