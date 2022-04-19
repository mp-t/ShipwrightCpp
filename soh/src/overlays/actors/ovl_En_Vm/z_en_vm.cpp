/*
 * File: z_en_vm.c
 * Overlay: ovl_En_Vm
 * Description: Beamos
 */

#include "z_en_vm.h"
#include "objects/object_vm/object_vm.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4)

void EnVm_Init(Actor* thisx, GlobalContext* globalCtx);
void EnVm_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnVm_Update(Actor* thisx, GlobalContext* globalCtx);
void EnVm_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnVm_SetupWait(EnVm* thisv);
void EnVm_Wait(EnVm* thisv, GlobalContext* globalCtx);
void EnVm_SetupAttack(EnVm* thisv);
void EnVm_Attack(EnVm* thisv, GlobalContext* globalCtx);
void EnVm_Stun(EnVm* thisv, GlobalContext* globalCtx);
void EnVm_Die(EnVm* thisv, GlobalContext* globalCtx);

ActorInit En_Vm_InitVars = {
    ACTOR_EN_VM,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_VM,
    sizeof(EnVm),
    (ActorFunc)EnVm_Init,
    (ActorFunc)EnVm_Destroy,
    (ActorFunc)EnVm_Update,
    (ActorFunc)EnVm_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit1 = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit sQuadInit2 = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f D_80B2EAEC = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EAF8 = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB04 = { 500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB10 = { -500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB1C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB28 = { 0.0f, 0.0f, 1600.0f };

static Vec3f D_80B2EB34 = { 1000.0f, 700.0f, 2000.0f };

static Vec3f D_80B2EB40 = { 1000.0f, -700.0f, 2000.0f };

static Vec3f D_80B2EB4C = { -1000.0f, 700.0f, 1500.0f };

static Vec3f D_80B2EB58 = { -1000.0f, -700.0f, 1500.0f };

static Vec3f D_80B2EB64 = { 500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB70 = { -500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB7C = { 0.4f, 0.4f, 0.4f };

static const void* D_80B2EB88[] = {
    gEffEnemyDeathFlame1Tex, gEffEnemyDeathFlame2Tex,  gEffEnemyDeathFlame3Tex, gEffEnemyDeathFlame4Tex,
    gEffEnemyDeathFlame5Tex, gEffEnemyDeathFlame6Tex,  gEffEnemyDeathFlame7Tex, gEffEnemyDeathFlame8Tex,
    gEffEnemyDeathFlame9Tex, gEffEnemyDeathFlame10Tex,
};

void EnVm_SetupAction(EnVm* thisv, EnVmActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnVm_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnVm* thisv = (EnVm*)thisx;

    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gBeamosSkel, &gBeamosAnim, thisv->jointTable, thisv->morphTable, 11);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Collider_InitCylinder(globalCtx, &thisv->colliderCylinder);
    Collider_SetCylinder(globalCtx, &thisv->colliderCylinder, thisx, &sCylinderInit);
    Collider_InitQuad(globalCtx, &thisv->colliderQuad1);
    Collider_SetQuad(globalCtx, &thisv->colliderQuad1, thisx, &sQuadInit1);
    Collider_InitQuad(globalCtx, &thisv->colliderQuad2);
    Collider_SetQuad(globalCtx, &thisv->colliderQuad2, thisx, &sQuadInit2);
    thisv->beamSightRange = (thisx->params >> 8) * 40.0f;
    thisx->params &= 0xFF;
    thisx->naviEnemyId = 0x39;

    if (thisx->params == BEAMOS_LARGE) {
        thisx->colChkInfo.health = 2;
        Actor_SetScale(thisx, 0.014f);
    } else {
        thisx->colChkInfo.health = 1;
        Actor_SetScale(thisx, 0.01f);
    }

    EnVm_SetupWait(thisv);
}

void EnVm_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnVm* thisv = (EnVm*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->colliderCylinder);
}

void EnVm_SetupWait(EnVm* thisv) {
    f32 frameCount = Animation_GetLastFrame(&gBeamosAnim);

    Animation_Change(&thisv->skelAnime, &gBeamosAnim, 1.0f, frameCount, frameCount, ANIMMODE_ONCE, 0.0f);
    thisv->unk_25E = thisv->unk_260 = 0;
    thisv->unk_21C = 0;
    thisv->timer = 10;
    EnVm_SetupAction(thisv, EnVm_Wait);
}

void EnVm_Wait(EnVm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 dist;
    s16 headRot;
    s16 pad;
    s16 pitch;

    switch (thisv->unk_25E) {
        case 0:
            Math_SmoothStepToS(&thisv->beamRot.x, 0, 10, 1500, 0);
            headRot = thisv->actor.yawTowardsPlayer - thisv->headRotY - thisv->actor.shape.rot.y;
            pitch = Math_Vec3f_Pitch(&thisv->beamPos1, &player->actor.world.pos);

            if (pitch > 0x1B91) {
                pitch = 0x1B91;
            }

            dist = thisv->beamSightRange - thisv->actor.xzDistToPlayer;

            if (thisv->actor.xzDistToPlayer <= thisv->beamSightRange && ABS(headRot) <= 0x2710 && pitch >= 0xE38 &&
                thisv->actor.yDistToPlayer <= 80.0f && thisv->actor.yDistToPlayer >= -160.0f) {
                Math_SmoothStepToS(&thisv->beamRot.x, pitch, 10, 0xFA0, 0);
                if (Math_SmoothStepToS(&thisv->headRotY, thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y, 1,
                                       (ABS((s16)(dist * 180.0f)) / 3) + 0xFA0, 0) <= 5460) {
                    thisv->timer--;
                    if (thisv->timer == 0) {
                        thisv->unk_25E++;
                        thisv->skelAnime.curFrame = 0.0f;
                        thisv->skelAnime.startFrame = 0.0f;
                        thisv->skelAnime.playSpeed = 2.0f;
                        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIMOS_AIM);
                    }
                }
            } else {
                thisv->headRotY -= 0x1F4;
            }

            SkelAnime_Update(&thisv->skelAnime);
            return;
        case 1:
            break;
        default:
            return;
    }

    Math_SmoothStepToS(&thisv->headRotY, thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y, 1, 0x1F40, 0);

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_260++;
        thisv->skelAnime.curFrame = 0.0f;
    }

    if (thisv->unk_260 == 2) {
        thisv->beamRot.y = thisv->actor.yawTowardsPlayer;
        thisv->beamRot.x = Math_Vec3f_Pitch(&thisv->beamPos1, &player->actor.world.pos);

        if (thisv->beamRot.x > 0x1B91) {
            thisv->beamRot.x = 0x1B91;
        }

        if (thisv->beamRot.x < 0xAAA) {
            thisv->skelAnime.startFrame = thisv->skelAnime.curFrame = thisv->skelAnime.endFrame;
            thisv->unk_25E = thisv->unk_260 = 0;
            thisv->timer = 10;
            thisv->skelAnime.playSpeed = 1.0f;
        } else {
            thisv->skelAnime.curFrame = 6.0f;
            EffectSsDeadDd_Spawn(globalCtx, &thisv->beamPos2, &D_80B2EAEC, &D_80B2EAEC, 150, -25, 0, 0, 255, 0, 255, 255,
                                 255, 16, 20);
            EnVm_SetupAttack(thisv);
        }
    }
}

void EnVm_SetupAttack(EnVm* thisv) {
    Animation_Change(&thisv->skelAnime, &gBeamosAnim, 3.0f, 3.0f, 7.0f, ANIMMODE_ONCE, 0.0f);
    thisv->timer = 305;
    thisv->beamScale.x = 0.6f;
    thisv->beamSpeed = 40.0f;
    thisv->unk_21C = 1;
    thisv->colliderQuad1.base.atFlags &= ~AT_HIT;
    EnVm_SetupAction(thisv, EnVm_Attack);
}

void EnVm_Attack(EnVm* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 pitch = Math_Vec3f_Pitch(&thisv->beamPos1, &player->actor.world.pos);
    f32 dist;
    Vec3f playerPos;

    if (pitch > 0x1B91) {
        pitch = 0x1B91;
    }

    if (thisv->colliderQuad1.base.atFlags & AT_HIT) {
        thisv->colliderQuad1.base.atFlags &= ~AT_HIT;
        thisv->timer = 0;

        if (thisv->beamScale.x > 0.1f) {
            thisv->beamScale.x = 0.1f;
        }
    }

    if (thisv->beamRot.x < 0xAAA || thisv->timer == 0) {
        Math_SmoothStepToF(&thisv->beamScale.x, 0.0f, 1.0f, 0.03f, 0.0f);
        thisv->unk_260 = 0;

        if (thisv->beamScale.x == 0.0f) {
            thisv->beamScale.y = thisv->beamScale.z = 0.0f;
            EnVm_SetupWait(thisv);
        }
    } else {
        if (--thisv->timer > 300) {
            return;
        }

        Math_SmoothStepToS(&thisv->headRotY, -thisv->actor.shape.rot.y + thisv->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        Math_SmoothStepToS(&thisv->beamRot.y, thisv->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        Math_SmoothStepToS(&thisv->beamRot.x, pitch, 10, 0xDAC, 0);
        playerPos = player->actor.world.pos;

        if (player->actor.floorHeight > BGCHECK_Y_MIN) {
            playerPos.y = player->actor.floorHeight;
        }

        dist = Math_Vec3f_DistXYZ(&thisv->beamPos1, &playerPos);
        Math_SmoothStepToF(&thisv->beamScale.z, dist, 1.0f, thisv->beamSpeed, 0.0f);
        Math_SmoothStepToF(&thisv->beamScale.x, 0.1f, 1.0f, 0.12f, 0.0f);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIMOS_LAZER - SFX_FLAG);

        if (thisv->unk_260 > 2) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderQuad1.base);
        }

        thisv->unk_260 = 3;
    }

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->skelAnime.curFrame = thisv->skelAnime.startFrame;
    }
}

void EnVm_SetupStun(EnVm* thisv) {
    Animation_Change(&thisv->skelAnime, &gBeamosAnim, -1.0f, Animation_GetLastFrame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    thisv->unk_260 = 0;
    thisv->timer = 180;
    thisv->unk_25E = thisv->unk_260;
    thisv->unk_21C = 2;
    thisv->beamScale.z = 0.0f;
    thisv->beamScale.y = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnVm_SetupAction(thisv, EnVm_Stun);
}

void EnVm_Stun(EnVm* thisv, GlobalContext* globalCtx) {
    if (thisv->timer == 0) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->unk_25E++;
            if (thisv->unk_25E == 3) {
                EnVm_SetupWait(thisv);
            } else if (thisv->unk_25E == 1) {
                Animation_Change(&thisv->skelAnime, &gBeamosAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBeamosAnim),
                                 ANIMMODE_ONCE, 0.0f);
            } else {
                thisv->timer = 10;
                thisv->skelAnime.curFrame = 0.0f;
                thisv->skelAnime.playSpeed = 2.0f;
            }
        }
    } else {
        Math_SmoothStepToS(&thisv->beamRot.x, 0, 10, 0x5DC, 0);
        thisv->timer--;
        SkelAnime_Update(&thisv->skelAnime);
    }
}

void EnVm_SetupDie(EnVm* thisv) {
    Animation_Change(&thisv->skelAnime, &gBeamosAnim, -1.0f, Animation_GetLastFrame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    thisv->timer = 33;
    thisv->unk_25E = thisv->unk_260 = 0;
    thisv->unk_21C = 3;
    thisv->beamScale.z = 0.0f;
    thisv->beamScale.y = 0.0f;
    thisv->actor.shape.yOffset = -5000.0f;
    thisv->actor.world.pos.y += 5000.0f * thisv->actor.scale.y;
    thisv->actor.velocity.y = 8.0f;
    thisv->actor.gravity = -0.5f;
    thisv->actor.speedXZ = Rand_ZeroOne() + 1.0f;
    thisv->actor.world.rot.y = Rand_CenteredFloat(65535.0f);
    EnVm_SetupAction(thisv, EnVm_Die);
}

void EnVm_Die(EnVm* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    thisv->beamRot.x += 0x5DC;
    thisv->headRotY += 0x9C4;
    Actor_MoveForward(&thisv->actor);

    if (--thisv->timer == 0) {
        bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                   thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0x6FF, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xA0);
        Actor_Kill(&thisv->actor);
    }
}

void EnVm_CheckHealth(EnVm* thisv, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (Actor_GetCollidedExplosive(globalCtx, &thisv->colliderCylinder.base) != NULL) {
        thisv->actor.colChkInfo.health--;
        osSyncPrintf("hp down %d\n", thisv->actor.colChkInfo.health);
    } else {
        if (!(thisv->colliderQuad2.base.acFlags & AC_HIT) || thisv->unk_21C == 2) {
            return;
        }
        thisv->colliderQuad2.base.acFlags &= ~AC_HIT;
    }

    if (thisv->actor.colChkInfo.health != 0) {
        Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 8);
        EnVm_SetupStun(thisv);
    } else {
        bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, thisv->actor.world.pos.x,
                                   thisv->actor.world.pos.y + 20.0f, thisv->actor.world.pos.z, 0, 0, 0x601, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        EnVm_SetupDie(thisv);
    }
}

void EnVm_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnVm* thisv = (EnVm*)thisx;
    CollisionCheckContext* colChkCtx = &globalCtx->colChkCtx;

    if (thisv->actor.colChkInfo.health != 0) {
        EnVm_CheckHealth(thisv, globalCtx);
    }

    if (thisv->unk_260 == 4) {
        EffectSsDeadDs_SpawnStationary(globalCtx, &thisv->beamPos3, 20, -1, 255, 20);
        func_80033480(globalCtx, &thisv->beamPos3, 6.0f, 1, 120, 20, 1);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIMOS_LAZER_GND - SFX_FLAG);
    }

    thisv->actionFunc(thisv, globalCtx);
    thisv->beamTexScroll += 0xC;

    if (thisv->actor.colChkInfo.health != 0 && thisv->unk_21C != 2) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIMOS_ROLL_HEAD - SFX_FLAG);
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCylinder);
    CollisionCheck_SetOC(globalCtx, colChkCtx, &thisv->colliderCylinder.base);

    if (thisv->actor.colorFilterTimer == 0 && thisv->actor.colChkInfo.health != 0) {
        CollisionCheck_SetAC(globalCtx, colChkCtx, &thisv->colliderCylinder.base);
    }

    CollisionCheck_SetAC(globalCtx, colChkCtx, &thisv->colliderQuad2.base);
    thisv->actor.focus.pos = thisv->actor.world.pos;
    thisv->actor.focus.pos.y += (6500.0f + thisv->actor.shape.yOffset) * thisv->actor.scale.y;
}

s32 EnVm_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnVm* thisv = (EnVm*)thisx;

    if (limbIndex == 2) {
        rot->x += thisv->beamRot.x;
        rot->y += thisv->headRotY;
    } else if (limbIndex == 10) {
        if (thisv->unk_21C == 3) {
            *dList = NULL;
        }
    }

    return false;
}

void EnVm_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnVm* thisv = (EnVm*)thisx;
    Vec3f sp80 = D_80B2EAF8;
    Vec3f sp74 = D_80B2EB04;
    Vec3f sp68 = D_80B2EB10;
    s32 pad;
    Vec3f posResult;
    CollisionPoly* poly;
    s32 bgId;
    f32 dist;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&D_80B2EB1C, &thisv->beamPos1);
        Matrix_MultVec3f(&D_80B2EB28, &thisv->beamPos2);

        if (thisv->unk_260 >= 3) {
            poly = NULL;
            sp80.z = (thisv->beamScale.z + 500.0f) * (thisv->actor.scale.y * 10000.0f);
            Matrix_MultVec3f(&sp80, &thisv->beamPos3);

            if (BgCheck_EntityLineTest1(&globalCtx->colCtx, &thisv->beamPos1, &thisv->beamPos3, &posResult, &poly, true,
                                        true, false, true, &bgId) == true) {
                thisv->beamScale.z = Math_Vec3f_DistXYZ(&thisv->beamPos1, &posResult) - 5.0f;
                thisv->unk_260 = 4;
                thisv->beamPos3 = posResult;
            }
            if (thisv->beamScale.z != 0.0f) {
                dist = 100.0f;
                if (thisv->actor.scale.y > 0.01f) {
                    dist = 70.0f;
                }
                sp74.z = sp68.z = Math_Vec3f_DistXYZ(&thisv->beamPos1, &thisv->beamPos3) * dist;
                Matrix_MultVec3f(&D_80B2EB64, &thisv->colliderQuad1.dim.quad[3]);
                Matrix_MultVec3f(&D_80B2EB70, &thisv->colliderQuad1.dim.quad[2]);
                Matrix_MultVec3f(&sp74, &thisv->colliderQuad1.dim.quad[1]);
                Matrix_MultVec3f(&sp68, &thisv->colliderQuad1.dim.quad[0]);
                Collider_SetQuadVertices(&thisv->colliderQuad1, &thisv->colliderQuad1.dim.quad[0],
                                         &thisv->colliderQuad1.dim.quad[1], &thisv->colliderQuad1.dim.quad[2],
                                         &thisv->colliderQuad1.dim.quad[3]);
            }
        }
        Matrix_MultVec3f(&D_80B2EB34, &thisv->colliderQuad2.dim.quad[1]);
        Matrix_MultVec3f(&D_80B2EB40, &thisv->colliderQuad2.dim.quad[0]);
        Matrix_MultVec3f(&D_80B2EB4C, &thisv->colliderQuad2.dim.quad[3]);
        Matrix_MultVec3f(&D_80B2EB58, &thisv->colliderQuad2.dim.quad[2]);
        Collider_SetQuadVertices(&thisv->colliderQuad2, &thisv->colliderQuad2.dim.quad[0],
                                 &thisv->colliderQuad2.dim.quad[1], &thisv->colliderQuad2.dim.quad[2],
                                 &thisv->colliderQuad2.dim.quad[3]);
    }
}

void EnVm_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    EnVm* thisv = (EnVm*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    Vec3f actorPos;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_vm.c", 1014);

    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnVm_OverrideLimbDraw,
                      EnVm_PostLimbDraw, thisv);
    actorPos = thisv->actor.world.pos;
    func_80033C30(&actorPos, &D_80B2EB7C, 255, globalCtx);

    if (thisv->unk_260 >= 3) {
        Matrix_Translate(thisv->beamPos3.x, thisv->beamPos3.y + 10.0f, thisv->beamPos3.z, MTXMODE_NEW);
        Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vm.c", 1033),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 168);
        func_80094BC4(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80B2EB88[globalCtx->gameplayFrames % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
        Matrix_RotateY(32767.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vm.c", 1044),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80B2EB88[(globalCtx->gameplayFrames + 4) % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
    }
    gSPSegment(POLY_OPA_DISP++, 0x08, func_80094E78(globalCtx->state.gfxCtx, 0, thisv->beamTexScroll));
    Matrix_Translate(thisv->beamPos1.x, thisv->beamPos1.y, thisv->beamPos1.z, MTXMODE_NEW);
    Matrix_RotateZYX(thisv->beamRot.x, thisv->beamRot.y, thisv->beamRot.z, MTXMODE_APPLY);
    Matrix_Scale(thisv->beamScale.x * 0.1f, thisv->beamScale.x * 0.1f, thisv->beamScale.z * 0.0015f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_vm.c", 1063),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBeamosLaserDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_vm.c", 1068);
}
