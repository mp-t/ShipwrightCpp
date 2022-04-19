/*
 * File: z_en_jj.c
 * Overlay: ovl_En_Jj
 * Description: Lord Jabu-Jabu
 */

#include "z_en_jj.h"
#include "objects/object_jj/object_jj.h"
#include "overlays/actors/ovl_Eff_Dust/z_eff_dust.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ JABUJABU_EYE_OPEN,
    /* 1 */ JABUJABU_EYE_HALF,
    /* 2 */ JABUJABU_EYE_CLOSED,
    /* 3 */ JABUJABU_EYE_MAX
} EnJjEyeState;

void EnJj_Init(Actor* thisx, GlobalContext* globalCtx);
void EnJj_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnJj_Update(Actor* thisx, GlobalContext* globalCtx);
void EnJj_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnJj_UpdateStaticCollision(Actor* thisx, GlobalContext* globalCtx);
void EnJj_WaitToOpenMouth(EnJj* thisv, GlobalContext* globalCtx);
void EnJj_WaitForFish(EnJj* thisv, GlobalContext* globalCtx);
void EnJj_BeginCutscene(EnJj* thisv, GlobalContext* globalCtx);
void EnJj_RemoveDust(EnJj* thisv, GlobalContext* globalCtx);

ActorInit En_Jj_InitVars = {
    ACTOR_EN_JJ,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_JJ,
    sizeof(EnJj),
    (ActorFunc)EnJj_Init,
    (ActorFunc)EnJj_Destroy,
    (ActorFunc)EnJj_Update,
    (ActorFunc)EnJj_Draw,
    NULL,
};

static s32 sUnused = 0;

#include "z_en_jj_cutscene_data.cpp" EARLY

static s32 sUnused2[] = { 0, 0 };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 170, 150, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 87, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 3300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1100, ICHAIN_STOP),
};

void EnJj_SetupAction(EnJj* thisv, EnJjActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnJj_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnJj* thisv = (EnJj*)thisx;
    const CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&thisv->dyna.actor, sInitChain);
    ActorShape_Init(&thisv->dyna.actor.shape, 0.0f, NULL, 0.0f);

    switch (thisv->dyna.actor.params) {
        case JABUJABU_MAIN:
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gJabuJabuSkel, &gJabuJabuAnim, thisv->jointTable,
                               thisv->morphTable, 22);
            Animation_PlayLoop(&thisv->skelAnime, &gJabuJabuAnim);
            thisv->unk_30A = 0;
            thisv->eyeIndex = 0;
            thisv->blinkTimer = 0;
            thisv->extraBlinkCounter = 0;
            thisv->extraBlinkTotal = 0;

            if (gSaveContext.eventChkInf[3] & 0x400) { // Fish given
                EnJj_SetupAction(thisv, EnJj_WaitToOpenMouth);
            } else {
                EnJj_SetupAction(thisv, EnJj_WaitForFish);
            }

            thisv->bodyCollisionActor = (DynaPolyActor*)Actor_SpawnAsChild(
                &globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EN_JJ, thisv->dyna.actor.world.pos.x - 10.0f,
                thisv->dyna.actor.world.pos.y, thisv->dyna.actor.world.pos.z, 0, thisv->dyna.actor.world.rot.y, 0,
                JABUJABU_COLLISION);
            DynaPolyActor_Init(&thisv->dyna, 0);
            CollisionHeader_GetVirtual(&gJabuJabuHeadCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->dyna.actor, &sCylinderInit);
            thisv->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
            break;

        case JABUJABU_COLLISION:
            DynaPolyActor_Init(&thisv->dyna, 0);
            CollisionHeader_GetVirtual(&gJabuJabuBodyCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
            func_8003ECA8(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            thisv->dyna.actor.update = EnJj_UpdateStaticCollision;
            thisv->dyna.actor.draw = NULL;
            Actor_SetScale(&thisv->dyna.actor, 0.087f);
            break;

        case JABUJABU_UNUSED_COLLISION:
            DynaPolyActor_Init(&thisv->dyna, 0);
            CollisionHeader_GetVirtual(&gJabuJabuUnusedCol, &colHeader);
            thisv->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &thisv->dyna.actor, colHeader);
            thisv->dyna.actor.update = EnJj_UpdateStaticCollision;
            thisv->dyna.actor.draw = NULL;
            Actor_SetScale(&thisv->dyna.actor, 0.087f);
            break;
    }
}

void EnJj_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnJj* thisv = (EnJj*)thisx;

    switch (thisv->dyna.actor.params) {
        case JABUJABU_MAIN:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            Collider_DestroyCylinder(globalCtx, &thisv->collider);
            break;

        case JABUJABU_COLLISION:
        case JABUJABU_UNUSED_COLLISION:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, thisv->dyna.bgId);
            break;
    }
}

/**
 * Blink routine. Blinks at the end of each randomised blinkTimer cycle. If extraBlinkCounter is not zero, blink that
 * many more times before resuming random blinkTimer cycles. extraBlinkTotal can be set to a positive number to blink
 * that many extra times at the end of every blinkTimer cycle, but the actor always sets it to zero, so only one
 * multiblink happens when extraBlinkCounter is nonzero.
 */
void EnJj_Blink(EnJj* thisv) {
    if (thisv->blinkTimer > 0) {
        thisv->blinkTimer--;
    } else {
        thisv->eyeIndex++;
        if (thisv->eyeIndex >= JABUJABU_EYE_MAX) {
            thisv->eyeIndex = JABUJABU_EYE_OPEN;
            if (thisv->extraBlinkCounter > 0) {
                thisv->extraBlinkCounter--;
            } else {
                thisv->blinkTimer = Rand_S16Offset(20, 20);
                thisv->extraBlinkCounter = thisv->extraBlinkTotal;
            }
        }
    }
}

void EnJj_OpenMouth(EnJj* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* bodyCollisionActor = thisv->bodyCollisionActor;

    if (thisv->mouthOpenAngle >= -5200) {
        thisv->mouthOpenAngle -= 102;

        if (thisv->mouthOpenAngle < -2600) {
            func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, bodyCollisionActor->bgId);
        }
    }
}

void EnJj_WaitToOpenMouth(EnJj* thisv, GlobalContext* globalCtx) {
    if (thisv->dyna.actor.xzDistToPlayer < 300.0f) {
        EnJj_SetupAction(thisv, EnJj_OpenMouth);
    }
}

void EnJj_WaitForFish(EnJj* thisv, GlobalContext* globalCtx) {
    static Vec3f feedingSpot = { -1589.0f, 53.0f, -43.0f };
    Player* player = GET_PLAYER(globalCtx);

    if ((Math_Vec3f_DistXZ(&feedingSpot, &player->actor.world.pos) < 300.0f) &&
        globalCtx->isPlayerDroppingFish(globalCtx)) {
        thisv->cutsceneCountdownTimer = 100;
        EnJj_SetupAction(thisv, EnJj_BeginCutscene);
    }

    thisv->collider.dim.pos.x = -1245;
    thisv->collider.dim.pos.y = 20;
    thisv->collider.dim.pos.z = -48;
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

void EnJj_BeginCutscene(EnJj* thisv, GlobalContext* globalCtx) {
    DynaPolyActor* bodyCollisionActor = thisv->bodyCollisionActor;

    if (thisv->cutsceneCountdownTimer > 0) {
        thisv->cutsceneCountdownTimer--;
    } else {
        EnJj_SetupAction(thisv, EnJj_RemoveDust);
        globalCtx->csCtx.segment = &D_80A88164;
        gSaveContext.cutsceneTrigger = 1;
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, bodyCollisionActor->bgId);
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        gSaveContext.eventChkInf[3] |= 0x400;
        func_80078884(NA_SE_SY_CORRECT_CHIME);
    }
}

void EnJj_CutsceneUpdate(EnJj* thisv, GlobalContext* globalCtx) {
    switch (globalCtx->csCtx.npcActions[2]->action) {
        case 1:
            if (thisv->unk_30A & 2) {
                thisv->eyeIndex = 0;
                thisv->blinkTimer = Rand_S16Offset(20, 20);
                thisv->extraBlinkCounter = 0;
                thisv->extraBlinkTotal = 0;
                thisv->unk_30A ^= 2;
            }
            break;

        case 2:
            thisv->unk_30A |= 1;

            if (!(thisv->unk_30A & 8)) {
                thisv->dust = Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->dyna.actor, globalCtx, ACTOR_EFF_DUST,
                                                -1100.0f, 105.0f, -27.0f, 0, 0, 0, EFF_DUST_TYPE_0);
                thisv->unk_30A |= 8;
            }
            break;

        case 3:
            if (!(thisv->unk_30A & 2)) {
                thisv->eyeIndex = 0;
                thisv->blinkTimer = 0;
                thisv->extraBlinkCounter = 1;
                thisv->extraBlinkTotal = 0;
                thisv->unk_30A |= 2;
            }
            break;
    }

    if (thisv->unk_30A & 1) {
        Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_JABJAB_BREATHE - SFX_FLAG);

        if (thisv->mouthOpenAngle >= -5200) {
            thisv->mouthOpenAngle -= 102;
        }
    }
}

void EnJj_RemoveDust(EnJj* thisv, GlobalContext* globalCtx) {
    Actor* dust;

    if (!(thisv->unk_30A & 4)) {
        thisv->unk_30A |= 4;
        dust = thisv->dust;

        if (dust != NULL) {
            Actor_Kill(dust);
            thisv->dyna.actor.child = NULL;
        }
    }
}

void EnJj_UpdateStaticCollision(Actor* thisx, GlobalContext* globalCtx) {
}

void EnJj_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnJj* thisv = (EnJj*)thisx;

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[2] != NULL)) {
        EnJj_CutsceneUpdate(thisv, globalCtx);
    } else {
        thisv->actionFunc(thisv, globalCtx);

        if (thisv->skelAnime.curFrame == 41.0f) {
            Audio_PlayActorSound2(&thisv->dyna.actor, NA_SE_EV_JABJAB_GROAN);
        }
    }

    EnJj_Blink(thisv);
    SkelAnime_Update(&thisv->skelAnime);
    Actor_SetScale(&thisv->dyna.actor, 0.087f);

    // Head
    thisv->skelAnime.jointTable[10].z = thisv->mouthOpenAngle;
}

void EnJj_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    static const void* eyeTextures[] = { gJabuJabuEyeOpenTex, gJabuJabuEyeHalfTex, gJabuJabuEyeClosedTex };
    GlobalContext* globalCtx = globalCtx2;
    EnJj* thisv = (EnJj*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_jj.c", 879);

    func_800943C8(globalCtx->state.gfxCtx);
    Matrix_Translate(0.0f, (cosf(thisv->skelAnime.curFrame * (std::numbers::pi_v<float> / 41.0f)) * 10.0f) - 10.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[thisv->eyeIndex]));
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_jj.c", 898);
}
