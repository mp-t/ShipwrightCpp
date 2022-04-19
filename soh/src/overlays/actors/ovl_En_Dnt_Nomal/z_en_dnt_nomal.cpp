/*
 * File: z_en_dnt_nomal
 * Overlay: ovl_En_Dnt_Nomal
 * Description: Lost Woods minigame scrubs
 */

#include "z_en_dnt_nomal.h"
#include "objects/object_dnk/object_dnk.h"
#include "overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_hintnuts/object_hintnuts.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnDntNomal_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDntNomal_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDntNomal_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDntNomal_DrawTargetScrub(Actor* thisx, GlobalContext* globalCtx);
void EnDntNomal_DrawStageScrub(Actor* thisx, GlobalContext* globalCtx);

void EnDntNomal_WaitForObject(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetFlower(EnDntNomal* thisv, GlobalContext* globalCtx);

void EnDntNomal_SetupTargetWait(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupTargetUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupTargetWalk(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupTargetTalk(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupTargetGivePrize(EnDntNomal* thisv, GlobalContext* globalCtx);

void EnDntNomal_TargetWait(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetWalk(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetFacePlayer(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetTalk(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetGivePrize(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetReturn(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_TargetBurrow(EnDntNomal* thisv, GlobalContext* globalCtx);

void EnDntNomal_SetupStageWait(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupStageCelebrate(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupStageDance(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupStageHide(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_SetupStageAttack(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageSetupReturn(EnDntNomal* thisv, GlobalContext* globalCtx);

void EnDntNomal_StageWait(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageUp(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageCelebrate(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageDance(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageHide(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageAttackHide(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageAttack(EnDntNomal* thisv, GlobalContext* globalCtx);
void EnDntNomal_StageReturn(EnDntNomal* thisv, GlobalContext* globalCtx);

ActorInit En_Dnt_Nomal_InitVars = {
    ACTOR_EN_DNT_NOMAL,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnDntNomal),
    (ActorFunc)EnDntNomal_Init,
    (ActorFunc)EnDntNomal_Destroy,
    (ActorFunc)EnDntNomal_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sBodyCylinderInit = {
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
    { 16, 46, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sTargetQuadInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0001F824, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Color_RGBA8 sLeafColors[] = {
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
    { 255, 255, 255, 255 }, { 210, 255, 0, 255 },   { 255, 195, 175, 255 },
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
};

void EnDntNomal_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDntNomal* thisv = (EnDntNomal*)thisx;

    thisv->type = thisv->actor.params;
    if (thisv->type < ENDNTNOMAL_TARGET) {
        thisv->type = ENDNTNOMAL_TARGET;
    }
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.colChkInfo.mass = 0xFF;
    thisv->objId = -1;
    if (thisv->type == ENDNTNOMAL_TARGET) {
        osSyncPrintf("\n\n");
        // "Deku Scrub target"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツ的当て ☆☆☆☆☆ \n" VT_RST);
        Collider_InitQuad(globalCtx, &thisv->targetQuad);
        Collider_SetQuad(globalCtx, &thisv->targetQuad, &thisv->actor, &sTargetQuadInit);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->objId = OBJECT_HINTNUTS;
    } else {
        osSyncPrintf("\n\n");
        // "Deku Scrub mask show audience"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツお面品評会一般人 ☆☆☆☆☆ \n" VT_RST);
        Collider_InitCylinder(globalCtx, &thisv->bodyCyl);
        Collider_SetCylinder(globalCtx, &thisv->bodyCyl, &thisv->actor, &sBodyCylinderInit);
        thisv->objId = OBJECT_DNK;
    }
    if (thisv->objId >= 0) {
        thisv->objIndex = Object_GetIndex(&globalCtx->objectCtx, thisv->objId);
        if (thisv->objIndex < 0) {
            Actor_Kill(&thisv->actor);
            // "What?"
            osSyncPrintf(VT_FGCOL(PURPLE) " なにみの？ %d\n" VT_RST "\n", thisv->objIndex);
            // "Bank is funny"
            osSyncPrintf(VT_FGCOL(CYAN) " バンクおかしいしぞ！%d\n" VT_RST "\n", thisv->actor.params);
            return;
        }
    } else {
        Actor_Kill(&thisv->actor);
    }
    thisv->actionFunc = EnDntNomal_WaitForObject;
}

void EnDntNomal_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDntNomal* thisv = (EnDntNomal*)thisx;

    if (thisv->type == ENDNTNOMAL_TARGET) {
        Collider_DestroyQuad(globalCtx, &thisv->targetQuad);
    } else {
        Collider_DestroyCylinder(globalCtx, &thisv->bodyCyl);
    }
}

void EnDntNomal_WaitForObject(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objIndex)) {
        gSegments[6] = reinterpret_cast<std::uintptr_t>( PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[thisv->objIndex].segment) );
        thisv->actor.objBankIndex = thisv->objIndex;
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
        thisv->actor.gravity = -2.0f;
        Actor_SetScale(&thisv->actor, 0.01f);
        if (thisv->type == ENDNTNOMAL_TARGET) {
            SkelAnime_Init(globalCtx, &thisv->skelAnime, &gHintNutsSkel, &gHintNutsBurrowAnim, thisv->jointTable,
                           thisv->morphTable, 10);
            thisv->actor.draw = EnDntNomal_DrawTargetScrub;
        } else {
            SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDntStageSkel, &gDntStageHideAnim, thisv->jointTable,
                           thisv->morphTable, 11);
            thisv->actor.draw = EnDntNomal_DrawStageScrub;
        }
        thisv->actionFunc = EnDntNomal_SetFlower;
    }
}

void EnDntNomal_SetFlower(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->flowerPos = thisv->actor.world.pos;
        if (thisv->type == ENDNTNOMAL_TARGET) {
            thisv->actionFunc = EnDntNomal_SetupTargetWait;
        } else {
            thisv->actionFunc = EnDntNomal_SetupStageWait;
        }
    }
}

void EnDntNomal_SetupTargetWait(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsBurrowAnim);
    Animation_Change(&thisv->skelAnime, &gHintNutsBurrowAnim, 0.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    thisv->skelAnime.curFrame = 8.0f;
    thisv->actionFunc = EnDntNomal_TargetWait;
}

void EnDntNomal_TargetWait(EnDntNomal* thisv, GlobalContext* globalCtx) {
    Vec3f scorePos;
    f32 targetX = 1340.0f;
    f32 targetY = 50.0f;
    f32 targetZ = -30.0f;
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f scoreAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f scoreVel = { 0.0f, 0.0f, 0.0f };

    thisv->targetVtx[0].x = thisv->targetVtx[1].x = thisv->targetVtx[2].x = thisv->targetVtx[3].x = targetX;

    thisv->targetVtx[1].y = thisv->targetVtx[0].y = targetY - 24.0f;

    thisv->targetVtx[2].z = thisv->targetVtx[0].z = targetZ + 24.0f;

    thisv->targetVtx[3].z = thisv->targetVtx[1].z = targetZ - 24.0f;

    thisv->targetVtx[3].y = thisv->targetVtx[2].y = targetY + 24.0f;

    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->targetQuad.base.acFlags & AC_HIT) || BREG(0)) {
        thisv->targetQuad.base.acFlags &= ~AC_HIT;

        dx = fabsf(targetX - thisv->targetQuad.info.bumper.hitPos.x);
        dy = fabsf(targetY - thisv->targetQuad.info.bumper.hitPos.y);
        dz = fabsf(targetZ - thisv->targetQuad.info.bumper.hitPos.z);

        scoreVel.y = 5.0f;

        if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 8.0f) {
            scorePos.x = thisv->actor.world.pos.x - 20.0f;
            scorePos.y = thisv->actor.world.pos.y + 20.0f;
            scorePos.z = thisv->actor.world.pos.z;
            EffectSsExtra_Spawn(globalCtx, &scorePos, &scoreVel, &scoreAccel, 4, 2);
            Audio_StopSfxById(NA_SE_SY_TRE_BOX_APPEAR);
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            // "Big hit"
            osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 大当り ☆☆☆☆☆ %d\n" VT_RST, thisv->hitCounter);
            if (!LINK_IS_ADULT && !(gSaveContext.itemGetInf[1] & 0x2000)) {
                thisv->hitCounter++;
                if (thisv->hitCounter >= 3) {
                    OnePointCutscene_Init(globalCtx, 4140, -99, &thisv->actor, MAIN_CAM);
                    func_8002DF54(globalCtx, &thisv->actor, 1);
                    thisv->timer4 = 50;
                    thisv->actionFunc = EnDntNomal_SetupTargetUnburrow;
                }
            }
        } else if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 24.0f) {
            scorePos.x = thisv->actor.world.pos.x;
            scorePos.y = thisv->actor.world.pos.y + 20.0f;
            scorePos.z = thisv->actor.world.pos.z;
            EffectSsExtra_Spawn(globalCtx, &scorePos, &scoreVel, &scoreAccel, 4, 0);
            thisv->hitCounter = 0;
        }
    }
}

void EnDntNomal_SetupTargetUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx) {
    Vec3f spawnPos;

    if (thisv->timer4 == 0) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsUnburrowAnim);
        Animation_Change(&thisv->skelAnime, &gHintNutsUnburrowAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        spawnPos = thisv->actor.world.pos;
        spawnPos.y = thisv->actor.world.pos.y + 50.0f;
        EffectSsHahen_SpawnBurst(globalCtx, &spawnPos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
        thisv->actionFunc = EnDntNomal_TargetUnburrow;
    }
}

void EnDntNomal_TargetUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (frame >= thisv->endFrame) {
        thisv->actionFunc = EnDntNomal_SetupTargetWalk;
    }
}

void EnDntNomal_SetupTargetWalk(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsRunAnim);
    Animation_Change(&thisv->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.speedXZ = 1.0f;
    thisv->actor.colChkInfo.mass = 0;
    thisv->actionFunc = EnDntNomal_TargetWalk;
}

void EnDntNomal_TargetWalk(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&thisv->skelAnime);
    dx = 1340.0f + 3.0f - thisv->actor.world.pos.x;
    dz = 0.0f - thisv->actor.world.pos.z;
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 0x32, 0xBB8, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if (thisv->actor.world.pos.z > -30.0f) {
        thisv->actor.speedXZ = 0.0f;
        thisv->actionFunc = EnDntNomal_TargetFacePlayer;
    }
}

void EnDntNomal_TargetFacePlayer(EnDntNomal* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if (fabsf(thisv->actor.shape.rot.y - thisv->actor.yawTowardsPlayer) < 30.0f) {
        thisv->actionFunc = EnDntNomal_SetupTargetTalk;
    }
}

void EnDntNomal_SetupTargetTalk(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsTalkAnim);
    Animation_Change(&thisv->skelAnime, &gHintNutsTalkAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.textId = 0x10AF;
    Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
    thisv->actionFunc = EnDntNomal_TargetTalk;
}

void EnDntNomal_TargetTalk(EnDntNomal* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        GET_ACTIVE_CAM(globalCtx)->csId = 0;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = EnDntNomal_SetupTargetGivePrize;
    }
}

void EnDntNomal_SetupTargetGivePrize(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsSpitAnim);
    Animation_Change(&thisv->skelAnime, &gHintNutsSpitAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = EnDntNomal_TargetGivePrize;
}

void EnDntNomal_TargetGivePrize(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if ((frame >= 8.0f) && !thisv->spawnedItem) {
        f32 itemX = thisv->mouthPos.x - 10.0f;
        f32 itemY = thisv->mouthPos.y;
        f32 itemZ = thisv->mouthPos.z;

        if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_EX_ITEM, itemX, itemY, itemZ, 0,
                               0, 0, EXITEM_BULLET_BAG) == NULL) {
            func_8002DF54(globalCtx, NULL, 7);
            Actor_Kill(&thisv->actor);
        }
        thisv->spawnedItem = true;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
    }
    if (frame >= thisv->endFrame) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsRunAnim);
        Animation_Change(&thisv->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
        thisv->actionFunc = EnDntNomal_TargetReturn;
    }
}

void EnDntNomal_TargetReturn(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&thisv->skelAnime);
    dx = thisv->flowerPos.x - thisv->actor.world.pos.x;
    dz = -180.0f - thisv->actor.world.pos.z;

    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 3, 0x1388, 0);
    if (fabsf(thisv->actor.shape.rot.y - (s16)(Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>))) < 20.0f) {
        thisv->actor.speedXZ = 1.0f;
    }
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) || Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if (thisv->actor.world.pos.z < -172.0f) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gHintNutsBurrowAnim);
        Animation_Change(&thisv->skelAnime, &gHintNutsBurrowAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        thisv->actor.world.pos.z = -173.0f;
        thisv->actor.speedXZ = 0.0f;
        thisv->actionFunc = EnDntNomal_TargetBurrow;
    }
}

void EnDntNomal_TargetBurrow(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (frame >= thisv->endFrame) {
        thisv->actionFunc = EnDntNomal_SetupTargetWait;
    }
}

void EnDntNomal_SetupStageWait(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->timer3 == 0) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageHideAnim);
        Animation_Change(&thisv->skelAnime, &gDntStageHideAnim, 0.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        thisv->skelAnime.curFrame = 8.0f;
        thisv->isSolid = false;
        thisv->actionFunc = EnDntNomal_StageWait;
    }
}

void EnDntNomal_StageWait(EnDntNomal* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
}

void EnDntNomal_SetupStageUp(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->timer3 == 0) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageUpAnim);
        Animation_Change(&thisv->skelAnime, &gDntStageUpAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        if (thisv->action != DNT_ACTION_ATTACK) {
            thisv->rotDirection = -1;
        }
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
        thisv->isSolid = true;
        thisv->actionFunc = EnDntNomal_StageUp;
    }
}

void EnDntNomal_StageUp(EnDntNomal* thisv, GlobalContext* globalCtx) {
    s16 rotTarget;
    f32 frame = thisv->skelAnime.curFrame;
    f32 turnMod;

    SkelAnime_Update(&thisv->skelAnime);
    if ((frame >= thisv->endFrame) && (thisv->action == DNT_ACTION_ATTACK)) {
        thisv->actionFunc = EnDntNomal_SetupStageAttack;
    } else {
        if (thisv->timer4 == 0) {
            turnMod = 0.0f;
            if (thisv->stagePrize == DNT_PRIZE_NONE) {
                Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
            } else {
                f32 dx = thisv->targetPos.x - thisv->actor.world.pos.x;
                f32 dz = thisv->targetPos.z - thisv->actor.world.pos.z;

                Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 1, 0xBB8, 0);
                turnMod = 90.0f;
            }
            if ((Rand_ZeroFloat(10.0f + turnMod) < 1.0f) && (thisv->action != DNT_ACTION_ATTACK)) {
                thisv->timer4 = (s16)Rand_ZeroFloat(30.0f) + 30;
            }
        } else {
            if (thisv->timer2 == 0) {
                thisv->rotDirection++;
                if (thisv->rotDirection > 1) {
                    thisv->rotDirection = -1;
                }
                thisv->timer2 = (s16)Rand_ZeroFloat(10.0f) + 10;
            }
            rotTarget = thisv->actor.yawTowardsPlayer;
            if (thisv->rotDirection != 0) {
                rotTarget += thisv->rotDirection * 0x1388;
            }
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, rotTarget, 3, 0x1388, 0);
        }
        if (thisv->actor.xzDistToPlayer < 70.0f) {
            thisv->actionFunc = EnDntNomal_SetupStageHide;
        }
    }
}

void EnDntNomal_SetupStageUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->timer3 == 0) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageUnburrowAnim);
        Animation_Change(&thisv->skelAnime, &gDntStageUnburrowAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        thisv->isSolid = false;
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
        thisv->actionFunc = EnDntNomal_StageUnburrow;
    }
}

void EnDntNomal_StageUnburrow(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (frame >= thisv->endFrame) {
        if (thisv->action != DNT_ACTION_DANCE) {
            thisv->timer3 = (s16)Rand_ZeroFloat(2.0f) + (s16)(thisv->type * 0.5f);
            thisv->actionFunc = EnDntNomal_SetupStageCelebrate;
        } else {
            thisv->timer2 = 300;
            thisv->actionFunc = EnDntNomal_SetupStageDance;
        }
    }
}

void EnDntNomal_SetupStageCelebrate(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageWalkAnim);
    Animation_Change(&thisv->skelAnime, &gDntStageWalkAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.speedXZ = 3.0f;
    thisv->isSolid = true;
    thisv->actionFunc = EnDntNomal_StageCelebrate;
}

void EnDntNomal_StageCelebrate(EnDntNomal* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->timer1 == 0) && (thisv->timer3 == 0)) {
        f32 dx = thisv->targetPos.x - thisv->actor.world.pos.x;
        f32 dz = thisv->targetPos.z - thisv->actor.world.pos.z;

        if ((fabsf(dx) < 10.0f) && (fabsf(dz) < 10.0f) && (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_NONE)) {
            thisv->action = DNT_ACTION_PRIZE;
            thisv->actionFunc = EnDntNomal_SetupStageDance;
            thisv->actor.speedXZ = 0.0f;
            return;
        }
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 1, 0xBB8, 0);
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    } else {
        if (thisv->timer1 == 1) {
            thisv->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
        }
        Math_SmoothStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0x14, 0x1388, 0);
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    }
    if (thisv->timer5 == 0) {
        thisv->timer5 = 20;
        if ((thisv->type & 1) == 0) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DAMAGE);
        }
    } else if ((thisv->timer5 & 3) == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((thisv->actor.bgCheckFlags & 8) && (thisv->actor.bgCheckFlags & 1)) {
        thisv->actor.velocity.y = 7.5f;
    }
}

void EnDntNomal_SetupStageDance(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageDanceAnim);
    Animation_Change(&thisv->skelAnime, &gDntStageDanceAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->isSolid = true;
    thisv->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
    thisv->rotDirection = -1;
    if (Rand_ZeroFloat(1.99f) < 1.0f) {
        thisv->rotDirection = 1;
    }
    thisv->actionFunc = EnDntNomal_StageDance;
}

void EnDntNomal_StageDance(EnDntNomal* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->timer2 == 0) {
        if (thisv->action == DNT_ACTION_DANCE) {
            thisv->action = DNT_ACTION_HIGH_RUPEES;
            thisv->actionFunc = EnDntNomal_SetupStageHide;
        } else {
            thisv->action = DNT_ACTION_NONE;
            thisv->actionFunc = EnDntNomal_StageSetupReturn;
        }
    } else if (thisv->timer3 != 0) {
        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
        if (thisv->timer3 == 1) {
            thisv->timer4 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
            thisv->rotDirection = -thisv->rotDirection;
        }
    } else if (thisv->timer4 != 0) {
        thisv->actor.shape.rot.y += thisv->rotDirection * 0x800;
        if (thisv->timer4 == 1) {
            thisv->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
        }
    }
}

void EnDntNomal_SetupStageHide(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->timer3 != 0) {
        if ((thisv->timer3 == 1) && (thisv->ignore == 1)) {
            func_80078884(NA_SE_SY_ERROR);
        }
    } else {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageHideAnim);
        Animation_Change(&thisv->skelAnime, &gDntStageHideAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        thisv->isSolid = false;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DOWN);
        thisv->actionFunc = EnDntNomal_StageHide;
    }
}

void EnDntNomal_StageHide(EnDntNomal* thisv, GlobalContext* globalCtx) {
    EnExRuppy* rupee;
    f32 frame = thisv->skelAnime.curFrame;
    s16 rupeeColor;

    SkelAnime_Update(&thisv->skelAnime);
    if (frame >= thisv->endFrame) {
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
        switch (thisv->action) {
            case DNT_ACTION_NONE:
                thisv->actionFunc = EnDntNomal_SetupStageWait;
                break;
            case DNT_ACTION_ATTACK:
                thisv->actionFunc = EnDntNomal_StageAttackHide;
                break;
            case DNT_ACTION_LOW_RUPEES:
            case DNT_ACTION_HIGH_RUPEES:
                rupee =
                    (EnExRuppy*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_EX_RUPPY, thisv->actor.world.pos.x,
                                            thisv->actor.world.pos.y + 20.0f, thisv->actor.world.pos.z, 0, 0, 0, 3);
                if (rupee != NULL) {
                    rupeeColor = thisv->action - DNT_ACTION_LOW_RUPEES;
                    rupee->colorIdx = rupeeColor;
                    if (Rand_ZeroFloat(3.99f) < 1.0f) {
                        rupee->colorIdx = rupeeColor + 1;
                    }
                    rupee->actor.velocity.y = 5.0f;
                    if (rupee->colorIdx == 2) {
                        rupee->actor.velocity.y = 7.0f;
                    }
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
                thisv->action = DNT_ACTION_NONE;
                thisv->actionFunc = EnDntNomal_SetupStageWait;
                break;
        }
    }
}

void EnDntNomal_StageAttackHide(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.xzDistToPlayer > 70.0f) {
        thisv->actionFunc = EnDntNomal_SetupStageUp;
    }
}

void EnDntNomal_SetupStageAttack(EnDntNomal* thisv, GlobalContext* globalCtx) {
    if (thisv->timer3 == 0) {
        thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageSpitAnim);
        Animation_Change(&thisv->skelAnime, &gDntStageSpitAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
        thisv->actor.colChkInfo.mass = 0xFF;
        thisv->isSolid = true;
        thisv->timer2 = 0;
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
        thisv->actionFunc = EnDntNomal_StageAttack;
    }
}

void EnDntNomal_StageAttack(EnDntNomal* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* nut;
    f32 frame = thisv->skelAnime.curFrame;
    f32 dz;
    f32 dx;
    f32 dy;

    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    dx = player->actor.world.pos.x - thisv->mouthPos.x;
    dy = player->actor.world.pos.y + 30.0f - thisv->mouthPos.y;
    dz = player->actor.world.pos.z - thisv->mouthPos.z;
    Math_SmoothStepToS(&thisv->actor.shape.rot.x, -(s16)(Math_FAtan2F(dy, sqrtf(SQ(dx) + SQ(dz))) * (0x8000 / std::numbers::pi_v<float>)), 3,
                       0x1388, 0);
    if ((frame >= thisv->endFrame) && (thisv->timer2 == 0)) {
        thisv->timer2 = (s16)Rand_ZeroFloat(10.0f) + 10;
    }
    if (thisv->timer2 == 1) {
        thisv->spawnedItem = false;
        thisv->actionFunc = EnDntNomal_SetupStageAttack;
    } else if (thisv->actor.xzDistToPlayer < 50.0f) {
        thisv->action = DNT_ACTION_ATTACK;
        thisv->actionFunc = EnDntNomal_SetupStageHide;
    } else if ((frame >= 8.0f) && (!thisv->spawnedItem)) {
        Vec3f baseOffset;
        Vec3f spawnOffset;
        f32 spawnX;
        f32 spawnY;
        f32 spawnZ;

        Matrix_RotateY(thisv->actor.shape.rot.y / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_NEW);
        Matrix_RotateX(thisv->actor.shape.rot.x / (f32)0x8000 * std::numbers::pi_v<float>, MTXMODE_APPLY);
        baseOffset.x = 0.0f;
        baseOffset.y = 0.0f;
        baseOffset.z = 5.0f;
        Matrix_MultVec3f(&baseOffset, &spawnOffset);
        spawnX = thisv->mouthPos.x + spawnOffset.x;
        spawnY = thisv->mouthPos.y + spawnOffset.y;
        spawnZ = thisv->mouthPos.z + spawnOffset.z;

        nut = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_NUTSBALL, spawnX, spawnY, spawnZ,
                          thisv->actor.shape.rot.x, thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 4);
        if (nut != NULL) {
            nut->velocity.y = spawnOffset.y * 0.5f;
        }
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
        thisv->spawnedItem = true;
    }
}

void EnDntNomal_StageSetupReturn(EnDntNomal* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntStageWalkAnim);
    Animation_Change(&thisv->skelAnime, &gDntStageWalkAnim, 1.5f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.speedXZ = 4.0f;
    thisv->isSolid = false;
    thisv->actionFunc = EnDntNomal_StageReturn;
}

void EnDntNomal_StageReturn(EnDntNomal* thisv, GlobalContext* globalCtx) {
    f32 sp2C;
    f32 sp28;

    SkelAnime_Update(&thisv->skelAnime);
    sp2C = thisv->flowerPos.x - thisv->actor.world.pos.x;
    sp28 = thisv->flowerPos.z - thisv->actor.world.pos.z;
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(sp2C, sp28) * (0x8000 / std::numbers::pi_v<float>), 1, 0xBB8, 0);
    if (thisv->timer5 == 0) {
        thisv->timer5 = 10;
    } else if (!(thisv->timer5 & 1)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((fabsf(sp2C) < 7.0f) && (fabsf(sp28) < 7.0f)) {
        thisv->actor.world.pos.x = thisv->flowerPos.x;
        thisv->actor.world.pos.z = thisv->flowerPos.z;
        thisv->actor.speedXZ = 0.0f;
        thisv->actionFunc = EnDntNomal_SetupStageHide;
    }
}

void EnDntNomal_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDntNomal* thisv = (EnDntNomal*)thisx;

    if (thisv->timer1 != 0) {
        thisv->timer1--;
    }
    if (thisv->timer2 != 0) {
        thisv->timer2--;
    }
    if (thisv->timer3 != 0) {
        thisv->timer3--;
    }
    if (thisv->timer4 != 0) {
        thisv->timer4--;
    }
    if (thisv->timer5 != 0) {
        thisv->timer5--;
    }
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    }
    thisv->actor.world.rot.x = thisv->actor.shape.rot.x;
    if (thisv->actionFunc != EnDntNomal_StageCelebrate) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
    thisv->unkCounter++;
    if (thisv->type != ENDNTNOMAL_TARGET) {
        switch (thisv->stageSignal) {
            case DNT_SIGNAL_LOOK:
                if (thisv->stagePrize == DNT_PRIZE_NONE) {
                    thisv->actionFunc = EnDntNomal_SetupStageUp;
                } else {
                    thisv->actionFunc = EnDntNomal_StageUp;
                }
                break;
            case DNT_SIGNAL_CELEBRATE:
                thisv->action = DNT_ACTION_NONE;
                thisv->actor.colChkInfo.mass = 0;
                thisv->timer3 = (s16)Rand_ZeroFloat(3.0f) + (s16)(thisv->type * 0.5f);
                thisv->actionFunc = EnDntNomal_SetupStageUnburrow;
                break;
            case DNT_SIGNAL_DANCE:
                thisv->action = DNT_ACTION_DANCE;
                thisv->actionFunc = EnDntNomal_SetupStageUnburrow;
                break;
            case DNT_SIGNAL_HIDE:
                thisv->actionFunc = EnDntNomal_SetupStageHide;
                break;
            case DNT_SIGNAL_RETURN:
                thisv->actionFunc = EnDntNomal_StageSetupReturn;
                break;
            case DNT_SIGNAL_UNUSED:
                thisv->actionFunc = EnDntNomal_SetupStageDance;
                break;
            case DNT_SIGNAL_NONE:
                break;
        }
    }
    if (thisv->stageSignal != DNT_SIGNAL_NONE) {
        thisv->stageSignal = DNT_SIGNAL_NONE;
    }
    if (thisv->blinkTimer == 0) {
        thisv->eyeState++;
        if (thisv->eyeState >= 3) {
            thisv->eyeState = 0;
            thisv->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 0x1D);
    if (thisv->type == ENDNTNOMAL_TARGET) {
        Collider_SetQuadVertices(&thisv->targetQuad, &thisv->targetVtx[0], &thisv->targetVtx[1], &thisv->targetVtx[2],
                                 &thisv->targetVtx[3]);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->targetQuad.base);
    } else {
        Collider_UpdateCylinder(&thisv->actor, &thisv->bodyCyl);
        if (thisv->isSolid) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->bodyCyl.base);
        }
    }
}

s32 EnDntNomal_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnDntNomal* thisv = (EnDntNomal*)thisx;

    if ((limbIndex == 1) || (limbIndex == 3) || (limbIndex == 4) || (limbIndex == 5) || (limbIndex == 6)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1733);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, sLeafColors[thisv->type - ENDNTNOMAL_STAGE].r,
                       sLeafColors[thisv->type - ENDNTNOMAL_STAGE].g, sLeafColors[thisv->type - ENDNTNOMAL_STAGE].b, 255);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1743);
    }
    return false;
}

void EnDntNomal_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    EnDntNomal* thisv = (EnDntNomal*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (thisv->type == ENDNTNOMAL_TARGET) {
        if (limbIndex == 5) {
            Matrix_MultVec3f(&zeroVec, &thisv->mouthPos);
        }
    } else if (limbIndex == 7) {
        Matrix_MultVec3f(&zeroVec, &thisv->mouthPos);
    }
}

void EnDntNomal_DrawStageScrub(Actor* thisx, GlobalContext* globalCtx) {
    static const void* blinkTex[] = { gDntStageEyeOpenTex, gDntStageEyeHalfTex, gDntStageEyeShutTex };
    EnDntNomal* thisv = (EnDntNomal*)thisx;
    Vec3f dustScale = { 0.25f, 0.25f, 0.25f };
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1790);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(blinkTex[thisv->eyeState]));
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnDntNomal_OverrideLimbDraw,
                      EnDntNomal_PostLimbDraw, thisv);
    Matrix_Translate(thisv->flowerPos.x, thisv->flowerPos.y, thisv->flowerPos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sLeafColors[thisv->type - ENDNTNOMAL_STAGE].r,
                   sLeafColors[thisv->type - ENDNTNOMAL_STAGE].g, sLeafColors[thisv->type - ENDNTNOMAL_STAGE].b, 255);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1814),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDntStageFlowerDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1817);
    if (thisv->actionFunc == EnDntNomal_StageCelebrate) {
        func_80033C30(&thisv->actor.world.pos, &dustScale, 255, globalCtx);
    }
}

void EnDntNomal_DrawTargetScrub(Actor* thisx, GlobalContext* globalCtx) {
    EnDntNomal* thisv = (EnDntNomal*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1833);
    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, EnDntNomal_PostLimbDraw,
                      thisv);
    Matrix_Translate(thisv->flowerPos.x, thisv->flowerPos.y, thisv->flowerPos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1848),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gHintNutsFlowerDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_nomal.c", 1851);
}
