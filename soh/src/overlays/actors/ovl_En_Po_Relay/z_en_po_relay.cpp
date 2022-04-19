/*
 * File: z_en_po_relay.c
 * Overlay: ovl_En_Po_Relay
 * Description: Dampé's Ghost
 */

#include "z_en_po_relay.h"
#include "overlays/actors/ovl_En_Honotrap/z_en_honotrap.h"
#include "objects/object_tk/object_tk.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_12 | ACTOR_FLAG_16)

void EnPoRelay_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPoRelay_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPoRelay_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPoRelay_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnPoRelay_Idle(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_Race(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_EndRace(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_Talk(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_Talk2(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_DisappearAndReward(EnPoRelay* thisv, GlobalContext* globalCtx);
void EnPoRelay_SetupIdle(EnPoRelay* thisv);

static Vec3s D_80AD8C30[] = {
    { 0xFFC4, 0xFDEE, 0xF47A }, { 0x0186, 0xFE0C, 0xF47A }, { 0x0186, 0xFE0C, 0xF0F6 }, { 0x00D2, 0xFDEE, 0xF0F6 },
    { 0x00D2, 0xFD9E, 0xEEDA }, { 0x023A, 0xFDC6, 0xEEDA }, { 0x023A, 0xFDC6, 0xED18 }, { 0x00D2, 0xFDC6, 0xED18 },
    { 0x00D2, 0xFDC6, 0xEBCE }, { 0x00D2, 0xFDC6, 0xEAA2 }, { 0x023A, 0xFDC6, 0xEAA2 }, { 0x023A, 0xFDC6, 0xEBB0 },
    { 0x04EC, 0xFD9E, 0xEBB0 }, { 0x0672, 0xFD62, 0xED18 }, { 0x0672, 0xFD30, 0xEE80 }, { 0x07DA, 0xFD26, 0xEE80 },
    { 0x07DA, 0xFD26, 0xEF70 }, { 0x07DA, 0xFD26, 0xF204 }, { 0x0672, 0xFD44, 0xF204 }, { 0x0672, 0xFD6C, 0xF3C6 },
    { 0x088E, 0xFD6C, 0xF3C6 }, { 0x088E, 0xFDB2, 0xF5E2 }, { 0x099C, 0xFDD0, 0xF5E2 }, { 0x0B54, 0xFE66, 0xF772 },
    { 0x0B4E, 0xFE66, 0xF87E }, { 0x0B4A, 0xFE66, 0xF97A }, { 0x0B4A, 0xFE98, 0xF9FC }, { 0x0BAE, 0xFE98, 0xF9FC },
};

ActorInit En_Po_Relay_InitVars = {
    ACTOR_EN_PO_RELAY,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TK,
    sizeof(EnPoRelay),
    (ActorFunc)EnPoRelay_Init,
    (ActorFunc)EnPoRelay_Destroy,
    (ActorFunc)EnPoRelay_Update,
    (ActorFunc)EnPoRelay_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 30, 52, 0, { 0, 0, 0 } },
};

static s32 D_80AD8D24 = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4F, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 1500, ICHAIN_STOP),
};

static Vec3f D_80AD8D30 = { 0.0f, 1.5f, 0.0f };

static Vec3f D_80AD8D3C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80AD8D48 = { 0.0f, 1200.0f, 0.0f };

static const void* sEyesTextures[] = {
    gDampeEyeOpenTex,
    gDampeEyeHalfTex,
    gDampeEyeClosedTex,
};

void EnPoRelay_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnPoRelay* thisv = (EnPoRelay*)thisx;
    s32 temp;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 42.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gDampeSkel, &gDampeFloatAnim, thisv->jointTable, thisv->morphTable,
                       18);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z,
                              255, 255, 255, 200);
    thisv->lightColor.a = 255;
    temp = 1;
    if (D_80AD8D24 != 0) {
        Actor_Kill(&thisv->actor);
    } else {
        D_80AD8D24 = temp;
        Actor_SetTextWithPrefix(globalCtx, &thisv->actor, 65);
        thisv->textId = thisv->actor.textId;
        EnPoRelay_SetupIdle(thisv);
    }
    thisv->actor.params &= 0x3F;
}

void EnPoRelay_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPoRelay* thisv = (EnPoRelay*)thisx;

    D_80AD8D24 = 0;
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnPoRelay_SetupIdle(EnPoRelay* thisv) {
    thisv->unk_195 = 32;
    thisv->pathIndex = 0;
    thisv->actor.room = -1;
    thisv->actor.shape.rot.y = 0;
    thisv->actor.world.rot.y = -0x8000;
    thisv->actor.colChkInfo.mass = MASS_HEAVY;
    thisv->actionFunc = EnPoRelay_Idle;
}

void EnPoRelay_Vec3sToVec3f(Vec3f* dest, Vec3s* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void EnPoRelay_SetupRace(EnPoRelay* thisv) {
    Vec3f vec;

    EnPoRelay_Vec3sToVec3f(&vec, &D_80AD8C30[thisv->pathIndex]);
    thisv->actionTimer = ((s16)(thisv->actor.shape.rot.y - thisv->actor.world.rot.y - 0x8000) >> 0xB) % 32U;
    func_80088B34(0);
    thisv->hookshotSlotFull = INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE;
    thisv->unk_19A = Actor_WorldYawTowardPoint(&thisv->actor, &vec);
    thisv->actor.flags |= ACTOR_FLAG_27;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    thisv->actionFunc = EnPoRelay_Race;
}

void EnPoRelay_SetupEndRace(EnPoRelay* thisv) {
    thisv->actor.world.rot.y = thisv->actor.home.rot.y + 0xC000;
    thisv->actor.flags &= ~ACTOR_FLAG_27;
    thisv->actor.speedXZ = 0.0f;
    thisv->actionFunc = EnPoRelay_EndRace;
}

void EnPoRelay_CorrectY(EnPoRelay* thisv) {
    Math_StepToF(&thisv->actor.home.pos.y, D_80AD8C30[(thisv->pathIndex >= 28) ? 27 : thisv->pathIndex].y + 45.0f, 2.0f);
    thisv->actor.world.pos.y = Math_SinS(thisv->unk_195 * 0x800) * 8.0f + thisv->actor.home.pos.y;
}

void EnPoRelay_Idle(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0x100);
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        thisv->actionFunc = EnPoRelay_Talk;
    } else if (thisv->actor.xzDistToPlayer < 250.0f) {
        thisv->actor.flags |= ACTOR_FLAG_16;
        thisv->actor.textId = thisv->textId;
        func_8002F2CC(&thisv->actor, globalCtx, 250.0f);
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoRelay_Talk(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0x100);
    if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        Actor_SetTextWithPrefix(globalCtx, &thisv->actor, 0x2F);
        thisv->textId = thisv->actor.textId;
        EnPoRelay_SetupRace(thisv);
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoRelay_Race(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f vec;
    f32 speed;
    f32 multiplier;

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->actionTimer == 0 && Rand_ZeroOne() < 0.03f) {
        thisv->actionTimer = 32;
        if (thisv->pathIndex < 23) {
            speed = Rand_ZeroOne() * 3.0f;
            if (speed < 1.0f) {
                multiplier = 1.0f;
            } else if (speed < 2.0f) {
                multiplier = -1.0f;
            } else {
                multiplier = 0.0f;
            }
            speed = 30.0f * multiplier;
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_HONOTRAP,
                        Math_CosS(thisv->unk_19A) * speed + thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                        Math_SinS(thisv->unk_19A) * speed + thisv->actor.world.pos.z, 0,
                        (thisv->unk_19A + 0x8000) - (0x2000 * multiplier), 0, HONOTRAP_FLAME_DROP);
        }
    }
    Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->unk_19A, 2, 0x1000, 0x100);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y + (thisv->actionTimer * 0x800) + 0x8000;
    if (thisv->pathIndex < 23) {
        // If the player travels along a different path to Dampé that converges later
        if ((Math3D_PointInSquare2D(660.0f, 840.0f, -4480.0f, -3760.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0) ||
            (Math3D_PointInSquare2D(1560.0f, 1740.0f, -4030.0f, -3670.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0) ||
            (Math3D_PointInSquare2D(1580.0f, 2090.0f, -3030.0f, -2500.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0)) {
            speed = (thisv->hookshotSlotFull) ? player->actor.speedXZ * 1.4f : player->actor.speedXZ * 1.2f;
        } else if (thisv->actor.xzDistToPlayer < 150.0f) {
            speed = (thisv->hookshotSlotFull) ? player->actor.speedXZ * 1.2f : player->actor.speedXZ;
        } else if (thisv->actor.xzDistToPlayer < 300.0f) {
            speed = (thisv->hookshotSlotFull) ? player->actor.speedXZ : player->actor.speedXZ * 0.8f;
        } else if (thisv->hookshotSlotFull) {
            speed = 4.5f;
        } else {
            speed = 3.5f;
        }
        multiplier = 250.0f - thisv->actor.xzDistToPlayer;
        multiplier = CLAMP_MIN(multiplier, 0.0f);
        speed += multiplier * 0.02f + 1.0f;
        Math_ApproachF(&thisv->actor.speedXZ, speed, 0.5f, 1.5f);
    } else {
        Math_ApproachF(&thisv->actor.speedXZ, 3.5f, 0.5f, 1.5f);
    }
    EnPoRelay_Vec3sToVec3f(&vec, &D_80AD8C30[thisv->pathIndex]);
    if (Actor_WorldDistXZToPoint(&thisv->actor, &vec) < 40.0f) {
        thisv->pathIndex++;
        EnPoRelay_Vec3sToVec3f(&vec, &D_80AD8C30[thisv->pathIndex]);
        if (thisv->pathIndex == 28) {
            EnPoRelay_SetupEndRace(thisv);
        } else if (thisv->pathIndex == 9) {
            Flags_SetSwitch(globalCtx, 0x35);
        } else if (thisv->pathIndex == 17) {
            Flags_SetSwitch(globalCtx, 0x36);
        } else if (thisv->pathIndex == 25) {
            Flags_SetSwitch(globalCtx, 0x37);
        }
    }
    thisv->unk_19A = Actor_WorldYawTowardPoint(&thisv->actor, &vec);
    func_8002F974(&thisv->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void EnPoRelay_EndRace(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, -0x4000, 0x800);
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnPoRelay_Talk2;
    } else if (globalCtx->roomCtx.curRoom.num == 5) {
        Actor_Kill(&thisv->actor);
        gSaveContext.timer1State = 0;
    } else if (Actor_IsFacingAndNearPlayer(&thisv->actor, 150.0f, 0x3000)) {
        thisv->actor.textId = thisv->textId;
        func_8002F2CC(&thisv->actor, globalCtx, 250.0f);
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoRelay_Talk2(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 0x100);
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(globalCtx)) {
            if (thisv->hookshotSlotFull != 0) {
                Actor_SetTextWithPrefix(globalCtx, &thisv->actor, 0x2E);
            } else {
                Actor_SetTextWithPrefix(globalCtx, &thisv->actor, 0x2D);
            }
            thisv->textId = thisv->actor.textId;
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        }
    } else if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        gSaveContext.timer1State = 0;
        thisv->actionTimer = 0;
        thisv->actionFunc = EnPoRelay_DisappearAndReward;
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoRelay_DisappearAndReward(EnPoRelay* thisv, GlobalContext* globalCtx) {
    Vec3f vec;
    f32 multiplier;
    s32 pad;
    Vec3f sp60;
    s32 pad1;

    thisv->actionTimer++;
    if (thisv->actionTimer < 8) {
        if (thisv->actionTimer < 5) {
            vec.y = Math_SinS((thisv->actionTimer * 0x1000) - 0x4000) * 23.0f + (thisv->actor.world.pos.y + 40.0f);
            multiplier = Math_CosS((thisv->actionTimer * 0x1000) - 0x4000) * 23.0f;
            vec.x = (Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * multiplier) +
                    thisv->actor.world.pos.x;
            vec.z = (Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * multiplier) +
                    thisv->actor.world.pos.z;
        } else {
            vec.y = thisv->actor.world.pos.y + 40.0f + 15.0f * (thisv->actionTimer - 5);
            vec.x =
                (Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f) + thisv->actor.world.pos.x;
            vec.z =
                (Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f) + thisv->actor.world.pos.z;
        }
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AD8D30, &D_80AD8D3C, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, true);
        vec.x = (thisv->actor.world.pos.x + thisv->actor.world.pos.x) - vec.x;
        vec.z = (thisv->actor.world.pos.z + thisv->actor.world.pos.z) - vec.z;
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AD8D30, &D_80AD8D3C, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, true);
        vec.x = thisv->actor.world.pos.x;
        vec.z = thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AD8D30, &D_80AD8D3C, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, true);
        if (thisv->actionTimer == 1) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EXTINCT);
        }
    }
    if (Math_StepToF(&thisv->actor.scale.x, 0.0f, 0.001f) != 0) {
        if (thisv->hookshotSlotFull != 0) {
            sp60.x = thisv->actor.world.pos.x;
            sp60.y = thisv->actor.floorHeight;
            sp60.z = thisv->actor.world.pos.z;
            if (gSaveContext.timer1Value < HIGH_SCORE(HS_DAMPE_RACE)) {
                HIGH_SCORE(HS_DAMPE_RACE) = gSaveContext.timer1Value;
            }
            if (Flags_GetCollectible(globalCtx, thisv->actor.params) == 0 && gSaveContext.timer1Value <= 60) {
                Item_DropCollectible2(globalCtx, &sp60, (thisv->actor.params << 8) + (0x4000 | ITEM00_HEART_PIECE));
            } else {
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ITEM00, sp60.x, sp60.y, sp60.z, 0, 0, 0, 2);
            }
        } else {
            Flags_SetTempClear(globalCtx, 4);
            HIGH_SCORE(HS_DAMPE_RACE) = gSaveContext.timer1Value;
        }
        Actor_Kill(&thisv->actor);
    }
    thisv->actor.scale.y = thisv->actor.scale.x;
    thisv->actor.scale.z = thisv->actor.scale.x;
    thisv->actor.world.pos.y += 10.0f;
}

void EnPoRelay_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnPoRelay* thisv = (EnPoRelay*)thisx;
    s32 pad;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    EnPoRelay_CorrectY(thisv);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 27.0f, 60.0f, 4);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    Actor_SetFocus(&thisv->actor, 50.0f);
    if (thisv->unk_195 != 0) {
        thisv->unk_195 -= 1;
    }
    if (thisv->unk_195 == 0) {
        thisv->unk_195 = 32;
    }
    thisv->eyeTextureIdx++;
    if (thisv->eyeTextureIdx == 3) {
        thisv->eyeTextureIdx = 0;
    }
}

void EnPoRelay_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnPoRelay* thisv = (EnPoRelay*)thisx;

    if (limbIndex == 14) {
        f32 rand;
        Vec3f vec;

        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 885);
        rand = Rand_ZeroOne();
        thisv->lightColor.r = (s16)(rand * 30.0f) + 225;
        thisv->lightColor.g = (s16)(rand * 100.0f) + 155;
        thisv->lightColor.b = (s16)(rand * 160.0f) + 95;
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, 128);
        gSPDisplayList(POLY_OPA_DISP++, gDampeLanternDL);
        if (1) {}
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 901);
        Matrix_MultVec3f(&D_80AD8D48, &vec);
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, vec.x, vec.y, vec.z, thisv->lightColor.r, thisv->lightColor.g,
                                  thisv->lightColor.b, 200);
    } else if (limbIndex == 8) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 916);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 918),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDampeHaloDL);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 922);
    }
}

void EnPoRelay_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnPoRelay* thisv = (EnPoRelay*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 940);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyesTextures[thisv->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, EnPoRelay_PostLimbDraw, &thisv->actor);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_relay.c", 954);
}
