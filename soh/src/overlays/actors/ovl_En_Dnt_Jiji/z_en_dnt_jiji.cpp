/*
 * File: z_en_dnt_jiji.c
 * Overlay: ovl_En_Dnt_Jiji
 * Description: Forest Stage scrub leader
 */

#include "z_en_dnt_jiji.h"
#include "objects/object_dns/object_dns.h"
#include "overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void EnDntJiji_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDntJiji_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDntJiji_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDntJiji_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnDntJiji_SetFlower(EnDntJiji* thisv, GlobalContext* globalCtx);

void EnDntJiji_SetupWait(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupUnburrow(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupWalk(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupCower(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupGivePrize(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupHide(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupReturn(EnDntJiji* thisv, GlobalContext* globalCtx);

void EnDntJiji_Wait(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Up(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Unburrow(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Walk(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Burrow(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Cower(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_SetupTalk(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Talk(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_GivePrize(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Hide(EnDntJiji* thisv, GlobalContext* globalCtx);
void EnDntJiji_Return(EnDntJiji* thisv, GlobalContext* globalCtx);

const ActorInit En_Dnt_Jiji_InitVars = {
    ACTOR_EN_DNT_JIJI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DNS,
    sizeof(EnDntJiji),
    (ActorFunc)EnDntJiji_Init,
    (ActorFunc)EnDntJiji_Destroy,
    (ActorFunc)EnDntJiji_Update,
    (ActorFunc)EnDntJiji_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 30, 80, 0, { 0, 0, 0 } },
};

void EnDntJiji_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnDntJiji* thisv = (EnDntJiji*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gDntJijiSkel, &gDntJijiBurrowAnim, thisv->jointTable, thisv->morphTable,
                   13);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    thisv->stage = (EnDntDemo*)thisv->actor.parent;
    osSyncPrintf("\n\n");
    // "Deku Scrub mask show elder"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ デグナッツお面品評会長老 ☆☆☆☆☆ %x\n" VT_RST, thisv->stage);
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.colChkInfo.mass = 0xFF;
    thisv->actor.targetMode = 6;
    thisv->actionFunc = EnDntJiji_SetFlower;
    thisv->actor.gravity = -2.0f;
}

void EnDntJiji_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDntJiji* thisv = (EnDntJiji*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnDntJiji_SetFlower(EnDntJiji* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.bgCheckFlags & 1) {
        thisv->flowerPos = thisv->actor.world.pos;
        thisv->actionFunc = EnDntJiji_SetupWait;
    }
}

void EnDntJiji_SetupWait(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiBurrowAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiBurrowAnim, 0.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->skelAnime.curFrame = 8.0f;
    thisv->isSolid = thisv->action = DNT_LEADER_ACTION_NONE;
    thisv->actionFunc = EnDntJiji_Wait;
}

void EnDntJiji_Wait(EnDntJiji* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);
    if ((thisv->timer == 1) && (thisv->actor.xzDistToPlayer < 150.0f) && !Gameplay_InCsMode(globalCtx) &&
        !(player->stateFlags1 & 0x800)) {
        OnePointCutscene_Init(globalCtx, 2230, -99, &thisv->actor, MAIN_CAM);
        thisv->timer = 0;
        func_8002DF54(globalCtx, NULL, 8);
        thisv->actionFunc = EnDntJiji_SetupUnburrow;
    }
}

void EnDntJiji_SetupUp(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiUpAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiUpAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
    thisv->actionFunc = EnDntJiji_Up;
}

void EnDntJiji_Up(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (thisv->actor.xzDistToPlayer < 150.0f) {
        thisv->actionFunc = EnDntJiji_SetupCower;
    }
}

void EnDntJiji_SetupUnburrow(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiUnburrowAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiUnburrowAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
    thisv->actionFunc = EnDntJiji_Unburrow;
}

void EnDntJiji_Unburrow(EnDntJiji* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->endFrame <= frame) {
        if (thisv->action != DNT_LEADER_ACTION_ATTACK) {
            thisv->actionFunc = EnDntJiji_SetupWalk;
        } else {
            thisv->actionFunc = EnDntJiji_SetupReturn;
        }
    }
}

void EnDntJiji_SetupWalk(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiWalkAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiWalkAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.speedXZ = 1.0f;
    thisv->isSolid = true;
    thisv->unburrow = true;
    thisv->actionFunc = EnDntJiji_Walk;
}

void EnDntJiji_Walk(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 5, 0x3E8, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    Math_ApproachF(&thisv->actor.speedXZ, 1.0f, 0.2f, 0.4f);
    if (thisv->sfxTimer == 0) {
        thisv->sfxTimer = 5;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((thisv->actor.bgCheckFlags & 8) && (thisv->actor.bgCheckFlags & 1)) {
        thisv->actor.velocity.y = 9.0f;
        thisv->actor.speedXZ = 3.0f;
    }
    if (thisv->actor.xzDistToPlayer < 100.0f) {
        if (CUR_UPG_VALUE(UPG_STICKS) == 1) {
            thisv->getItemId = GI_STICK_UPGRADE_20;
        } else {
            thisv->getItemId = GI_STICK_UPGRADE_30;
        }
        thisv->actor.textId = 0x104D;
        Message_StartTextbox(globalCtx, thisv->actor.textId, NULL);
        thisv->actor.speedXZ = 0.0f;
        thisv->unused = 5;
        thisv->actionFunc = EnDntJiji_Talk;
    }
}

void EnDntJiji_SetupBurrow(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiBurrowAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiBurrowAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_DOWN);
    thisv->actionFunc = EnDntJiji_Burrow;
}

void EnDntJiji_Burrow(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
}

void EnDntJiji_SetupCower(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiCowerAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiCowerAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 3.0f, 0, 9, 3, 10, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_UP);

    if ((CUR_UPG_VALUE(UPG_NUTS) == 1) || (CUR_UPG_VALUE(UPG_NUTS) == 0)) {
        thisv->getItemId = GI_NUT_UPGRADE_30;
    } else {
        thisv->getItemId = GI_NUT_UPGRADE_40;
    }
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actor.textId = 0x10DB;
    thisv->unused = 5;
    thisv->actionFunc = EnDntJiji_Cower;
}

void EnDntJiji_Cower(EnDntJiji* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (frame >= thisv->endFrame) {
        if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
            thisv->actionFunc = EnDntJiji_SetupTalk;
        } else {
            func_8002F2CC(&thisv->actor, globalCtx, 100.0f);
        }
    }
}

void EnDntJiji_SetupTalk(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiTalkAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiTalkAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = EnDntJiji_Talk;
}

void EnDntJiji_Talk(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        func_8005B1A4(GET_ACTIVE_CAM(globalCtx));
        Message_CloseTextbox(globalCtx);
        func_8002DF54(globalCtx, NULL, 7);
        thisv->actor.parent = NULL;
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 400.0f, 200.0f);
        thisv->actionFunc = EnDntJiji_SetupGivePrize;
    }
}

void EnDntJiji_SetupGivePrize(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_HasParent(&thisv->actor, globalCtx)) {
        thisv->actionFunc = EnDntJiji_GivePrize;
    } else {
        func_8002F434(&thisv->actor, globalCtx, thisv->getItemId, 400.0f, 200.0f);
    }
}

void EnDntJiji_GivePrize(EnDntJiji* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        if ((thisv->getItemId == GI_NUT_UPGRADE_30) || (thisv->getItemId == GI_NUT_UPGRADE_40)) {
            // "nut"
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            osSyncPrintf("実 \n");
            gSaveContext.itemGetInf[1] |= 0x8000;
        } else {
            // "stick"
            osSyncPrintf("棒 \n");
            osSyncPrintf("棒 \n");
            osSyncPrintf("棒 \n");
            osSyncPrintf("棒 \n");
            osSyncPrintf("棒 \n");
            osSyncPrintf("棒 \n");
            gSaveContext.itemGetInf[1] |= 0x4000;
        }
        thisv->actor.textId = 0;
        if ((thisv->stage != NULL) && (thisv->stage->actor.update != NULL)) {
            thisv->stage->action = DNT_ACTION_NONE;
            if (!thisv->unburrow) {
                thisv->stage->leaderSignal = DNT_SIGNAL_HIDE;
            } else {
                thisv->stage->leaderSignal = DNT_SIGNAL_RETURN;
            }
        }
        thisv->actor.flags &= ~ACTOR_FLAG_0;
        if (!thisv->unburrow) {
            thisv->actionFunc = EnDntJiji_SetupHide;
        } else {
            thisv->actionFunc = EnDntJiji_SetupReturn;
        }
    }
}

void EnDntJiji_SetupHide(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiHideAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiHideAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = EnDntJiji_Hide;
}

void EnDntJiji_Hide(EnDntJiji* thisv, GlobalContext* globalCtx) {
    f32 frame = thisv->skelAnime.curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->endFrame <= frame) {
        thisv->actionFunc = EnDntJiji_SetupWait;
    }
}

void EnDntJiji_SetupReturn(EnDntJiji* thisv, GlobalContext* globalCtx) {
    thisv->endFrame = (f32)Animation_GetLastFrame(&gDntJijiWalkAnim);
    Animation_Change(&thisv->skelAnime, &gDntJijiWalkAnim, 1.0f, 0.0f, thisv->endFrame, ANIMMODE_LOOP, -10.0f);
    thisv->actor.speedXZ = 2.0f;
    thisv->isSolid = thisv->unburrow = true;
    thisv->actionFunc = EnDntJiji_Return;
}

void EnDntJiji_Return(EnDntJiji* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&thisv->skelAnime);
    dx = thisv->flowerPos.x - thisv->actor.world.pos.x;
    dz = thisv->flowerPos.z - thisv->actor.world.pos.z;
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / std::numbers::pi_v<float>), 1, 0xBB8, 0);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    if ((thisv->actor.bgCheckFlags & 8) && (thisv->actor.bgCheckFlags & 1)) {
        thisv->actor.velocity.y = 9.0f;
        thisv->actor.speedXZ = 3.0f;
    }
    if (thisv->sfxTimer == 0) {
        thisv->sfxTimer = 3;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((fabsf(dx) < 5.0f) && (fabsf(dz) < 5.0f)) {
        thisv->actor.world.pos.x = thisv->flowerPos.x;
        thisv->actor.world.pos.z = thisv->flowerPos.z;
        if (thisv->attackFlag) {
            if ((thisv->stage->actor.update != NULL) && (thisv->stage->leaderSignal == DNT_SIGNAL_NONE)) {
                thisv->stage->leaderSignal = DNT_SIGNAL_HIDE;
                thisv->stage->action = DNT_ACTION_ATTACK;
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_ENEMY | 0x800);
            }
        }
        thisv->actor.speedXZ = 0.0f;
        thisv->isSolid = 0;
        thisv->actionFunc = EnDntJiji_SetupBurrow;
    }
}

void EnDntJiji_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnDntJiji* thisv = (EnDntJiji*)thisx;

    Actor_SetScale(&thisv->actor, 0.015f);
    thisv->unkTimer++;
    if (BREG(0)) {
        // "time"
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 時間 ☆☆☆☆☆ %d\n" VT_RST, thisv->timer);
    }
    if ((thisv->timer > 1) && (thisv->timer != 0)) {
        thisv->timer--;
    }
    if (thisv->sfxTimer != 0) {
        thisv->sfxTimer--;
    }
    if (thisv->blinkTimer != 0) {
        thisv->blinkTimer--;
    }
    switch (thisv->stageSignal) {
        case DNT_LEADER_SIGNAL_UP:
            thisv->isSolid = true;
            thisv->action = DNT_LEADER_ACTION_UP;
            thisv->actionFunc = EnDntJiji_SetupUp;
            break;
        case DNT_LEADER_SIGNAL_BURROW:
            thisv->isSolid = false;
            thisv->action = DNT_LEADER_ACTION_NONE;
            thisv->actionFunc = EnDntJiji_SetupBurrow;
            break;
        case DNT_LEADER_SIGNAL_RETURN:
            thisv->actionFunc = EnDntJiji_SetupReturn;
            break;
        case DNT_LEADER_SIGNAL_NONE:
            break;
    }
    if (thisv->actor.textId != 0) {
        Actor_SetFocus(&thisv->actor, 30.0f);
    }
    if (thisv->stageSignal != DNT_LEADER_SIGNAL_NONE) {
        thisv->stageSignal = DNT_LEADER_SIGNAL_NONE;
    }
    if (thisv->blinkTimer == 0) {
        thisv->eyeState++;
        if (thisv->eyeState > 2) {
            thisv->eyeState = 0;
            thisv->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 60.0f, 0x1D);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    if (thisv->isSolid != 0) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
}

void EnDntJiji_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* blinkTex[] = { gDntJijiEyeOpenTex, gDntJijiEyeHalfTex, gDntJijiEyeShutTex };
    EnDntJiji* thisv = (EnDntJiji*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_jiji.c", 1019);
    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Push();
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(blinkTex[thisv->eyeState]));
    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, NULL, NULL, thisv);
    Matrix_Pop();
    Matrix_Translate(thisv->flowerPos.x, thisv->flowerPos.y, thisv->flowerPos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_dnt_jiji.c", 1040),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDntJijiFlowerDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_dnt_jiji.c", 1043);
}
