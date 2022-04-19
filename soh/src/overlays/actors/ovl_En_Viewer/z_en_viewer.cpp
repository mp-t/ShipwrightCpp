/*
 * File: z_en_viewer.c
 * Overlay: ovl_En_Viewer
 * Description: Cutscene Actors
 */

#include "z_en_viewer.h"
#include "overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
#include "objects/object_zl4/object_zl4.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_horse_zelda/object_horse_zelda.h"
#include "objects/object_horse_ganon/object_horse_ganon.h"
#include "objects/object_im/object_im.h"
#include "objects/object_gndd/object_gndd.h"
#include "objects/object_ganon/object_ganon.h"
#include "objects/object_opening_demo1/object_opening_demo1.h"

#define FLAGS ACTOR_FLAG_4

void EnViewer_Init(Actor* thisx, GlobalContext* globalCtx);
void EnViewer_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnViewer_Update(Actor* thisx, GlobalContext* globalCtx);
void EnViewer_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnViewer_UpdatePosition(EnViewer* thisv, GlobalContext* globalCtx);
void EnViewer_DrawFireEffects(EnViewer* this2, GlobalContext* globalCtx);
void EnViewer_UpdateGanondorfCape(GlobalContext* globalCtx, EnViewer* thisv);
void EnViewer_InitImpl(EnViewer* thisv, GlobalContext* globalCtx);
void EnViewer_UpdateImpl(EnViewer* thisv, GlobalContext* globalCtx);

static u8 sHorseSfxPlayed = false;

ActorInit En_Viewer_InitVars = {
    ACTOR_EN_VIEWER,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnViewer),
    (ActorFunc)EnViewer_Init,
    (ActorFunc)EnViewer_Destroy,
    (ActorFunc)EnViewer_Update,
    (ActorFunc)EnViewer_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_STOP),
};

static EnViewerInitData sInitData[] = {
    /* ENVIEWER_TYPE_0_HORSE_ZELDA */
    { OBJECT_HORSE_ZELDA, OBJECT_HORSE_ZELDA, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseZeldaSkel,
      &gHorseZeldaGallopingAnim },
    /* ENVIEWER_TYPE_1_IMPA */
    { OBJECT_IM, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_IMPA, &gImpaSkel,
      &object_opening_demo1_Anim_0029CC },
    /* ENVIEWER_TYPE_2_ZELDA */
    { OBJECT_ZL4, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_ZELDA, &gChildZeldaSkel,
      &object_opening_demo1_Anim_000450 },
    /* ENVIEWER_TYPE_3_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &object_gndd_Skel_0119E8,
      &object_gndd_Anim_002928 },
    /* ENVIEWER_TYPE_4_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonRearingAnim },
    /* ENVIEWER_TYPE_5_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &object_gndd_Skel_0119E8,
      &object_gndd_Anim_0005B4 },
    /* ENVIEWER_TYPE_6_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonGallopingAnim },
    /* ENVIEWER_TYPE_7_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &object_gndd_Skel_0119E8,
      &object_gndd_Anim_004260 },
    /* ENVIEWER_TYPE_8_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &object_gndd_Skel_0119E8,
      &object_gndd_Anim_0050A8 },
    /* ENVIEWER_TYPE_9_GANONDORF */
    { OBJECT_GANON, OBJECT_GANON, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gDorfSkel,
      &object_ganon_Anim_011348 },
};

static EnGanonMant* sGanondorfCape;

static Vec3f sGanondorfNeckWorldPos;

void EnViewer_SetupAction(EnViewer* thisv, EnViewerActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnViewer_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnViewer* thisv = (EnViewer*)thisx;
    u8 type;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    EnViewer_SetupAction(thisv, EnViewer_InitImpl);
    sHorseSfxPlayed = false;
    type = thisv->actor.params >> 8;
    thisv->unused = 0;
    thisv->state = 0;
    thisv->isVisible = false;
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        sGanondorfCape = (EnGanonMant*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx,
                                                          ACTOR_EN_GANON_MANT, 0.0f, 0.0f, 0.0f, 0, 0, 0, 35);
    }
}

void EnViewer_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnViewer* thisv = (EnViewer*)thisx;

    Skin_Free(globalCtx, &thisv->skin);
}

void EnViewer_InitAnimGanondorfOrZelda(EnViewer* thisv, GlobalContext* globalCtx, const void* skeletonHeaderSeg,
                                       const AnimationHeader* anim) {
    s16 type = thisv->actor.params >> 8;

    if (type == ENVIEWER_TYPE_2_ZELDA || type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
        type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        SkelAnime_InitFlex(globalCtx, &thisv->skin.skelAnime, static_cast<const FlexSkeletonHeader*>(skeletonHeaderSeg), NULL, NULL, NULL, 0);
    } else {
        SkelAnime_Init(globalCtx, &thisv->skin.skelAnime, static_cast<const SkeletonHeader*>(skeletonHeaderSeg), NULL, NULL, NULL, 0);
    }

    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->animObjBankIndex].segment);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
        type == ENVIEWER_TYPE_9_GANONDORF) {
        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, anim, 1.0f);
    } else {
        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, anim, 3.0f);
    }
}

void EnViewer_InitAnimImpa(EnViewer* thisv, GlobalContext* globalCtx, const void* skeletonHeaderSeg, const AnimationHeader* anim) {
    SkelAnime_InitFlex(globalCtx, &thisv->skin.skelAnime, static_cast<const FlexSkeletonHeader*>(skeletonHeaderSeg), NULL, NULL, NULL, 0);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->animObjBankIndex].segment);
    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, anim, 3.0f);
}

void EnViewer_InitAnimHorse(EnViewer* thisv, GlobalContext* globalCtx, const void* skeletonHeaderSeg, const AnimationHeader* anim) {
    u8 type;

    Skin_Init(globalCtx, &thisv->skin, static_cast<const SkeletonHeader*>(skeletonHeaderSeg), anim);
    type = thisv->actor.params >> 8;
    if (!(type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF ||
          type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
          type == ENVIEWER_TYPE_9_GANONDORF)) {
        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, anim, 3.0f);
    } else {
        Animation_PlayOnceSetSpeed(&thisv->skin.skelAnime, anim, 1.0f);
    }
}

static EnViewerInitAnimFunc sInitAnimFuncs[] = {
    EnViewer_InitAnimGanondorfOrZelda,
    EnViewer_InitAnimHorse,
    EnViewer_InitAnimGanondorfOrZelda,
    EnViewer_InitAnimImpa,
};

static ActorShadowFunc sShadowDrawFuncs[] = {
    NULL,
    ActorShadow_DrawCircle,
    ActorShadow_DrawHorse,
};

void EnViewer_InitImpl(EnViewer* thisv, GlobalContext* globalCtx) {
    EnViewerInitData* initData = &sInitData[thisv->actor.params >> 8];
    s32 skelObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, initData->skeletonObject);

    ASSERT(skelObjBankIndex >= 0, "bank_ID >= 0", "../z_en_viewer.c", 576);

    thisv->animObjBankIndex = Object_GetIndex(&globalCtx->objectCtx, initData->animObject);
    ASSERT(thisv->animObjBankIndex >= 0, "thisv->anime_bank_ID >= 0", "../z_en_viewer.c", 579);

    if (!Object_IsLoaded(&globalCtx->objectCtx, skelObjBankIndex) ||
        !Object_IsLoaded(&globalCtx->objectCtx, thisv->animObjBankIndex)) {
        thisv->actor.flags &= ~ACTOR_FLAG_6;
        return;
    }

    thisv->isVisible = true;
    thisv->actor.objBankIndex = skelObjBankIndex;
    Actor_SetObjectDependency(globalCtx, &thisv->actor);
    Actor_SetScale(&thisv->actor, initData->scale / 100.0f);
    ActorShape_Init(&thisv->actor.shape, initData->yOffset * 100, sShadowDrawFuncs[initData->shadowType],
                    initData->shadowScale);
    thisv->drawFuncIndex = initData->drawType;
    sInitAnimFuncs[thisv->drawFuncIndex](thisv, globalCtx, initData->skeletonHeaderSeg, initData->anim);
    EnViewer_SetupAction(thisv, EnViewer_UpdateImpl);
}

static s16 sTimer = 0;

void EnViewer_UpdateImpl(EnViewer* thisv, GlobalContext* globalCtx) {
    u8 type = thisv->actor.params >> 8;
    u16 csFrames;
    s32 animationEnded;

    if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (gSaveContext.sceneSetupIndex == 5) {
            csFrames = globalCtx->csCtx.frames;
            if (csFrames == 792) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_SURPRISE);
            } else if (csFrames == 845) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_VO_Z0_THROW);
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        Actor_SetScale(&thisv->actor, 0.3f);
        thisv->actor.uncullZoneForward = 10000.0f;
        thisv->actor.uncullZoneScale = 10000.0f;
        thisv->actor.uncullZoneDownward = 10000.0f;
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        if (gSaveContext.sceneSetupIndex == 4) {
            switch (globalCtx->csCtx.frames) {
                case 20:
                case 59:
                case 71:
                case 129:
                case 140:
                case 219:
                case 280:
                case 320:
                case 380:
                case 409:
                case 438:
                    Audio_PlaySoundGeneral(NA_SE_SY_DEMO_CUT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    break;
            }
        }
        if (gSaveContext.sceneSetupIndex == 5) {
            if (globalCtx->csCtx.frames == 1508) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_FANTOM_ST_LAUGH);
            }
            if (globalCtx->csCtx.frames == 1545) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DEMO_6K, 32.0f, 101.0f, 1226.0f,
                                   0, 0, 0, 0xC);
            }
        }
        if (globalCtx->csCtx.frames == 1020) {
            Audio_QueueSeqCmd(SEQ_PLAYER_FANFARE << 24 | NA_BGM_OPENING_GANON);
        }
        if (globalCtx->csCtx.frames == 960) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    } else if (type == ENVIEWER_TYPE_6_HORSE_GANONDORF) {
        if (gSaveContext.sceneSetupIndex == 5 || gSaveContext.sceneSetupIndex == 10) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    } else if (type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        s16 curFrame = thisv->skin.skelAnime.curFrame;

        if (thisv->skin.skelAnime.animation == &gHorseGanonRearingAnim) {
            if (curFrame == 8) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            }
            if (curFrame == 30) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_LAND2);
            }
        } else if (thisv->skin.skelAnime.animation == &gHorseGanonIdleAnim) {
            if (curFrame == 25) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_SANDDUST);
            }
        } else if (thisv->skin.skelAnime.animation == &gHorseGanonGallopingAnim) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    }

    if (sTimer != 0) {
        sTimer--;
    }

    EnViewer_UpdatePosition(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor); // has no effect, speed/velocity and gravity are 0

    animationEnded = SkelAnime_Update(&thisv->skin.skelAnime);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[1] != NULL) {
            if (globalCtx->csCtx.npcActions[1]->action == 2 && sTimer == 0) {
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (thisv->skin.skelAnime.animation != &object_gndd_Anim_002928) {
                        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_gndd_Anim_002928, 1.0f);
                    }
                } else if (thisv->skin.skelAnime.animation != &gHorseGanonIdleAnim) {
                    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &gHorseGanonIdleAnim, 1.0f);
                }
            } else if (globalCtx->csCtx.npcActions[1]->action == 1) {
                sTimer = 100;
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (thisv->skin.skelAnime.animation != &object_gndd_Anim_001D28) {
                        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_gndd_Anim_001D28, 1.0f);
                    }
                } else if (thisv->skin.skelAnime.animation != &gHorseGanonRearingAnim) {
                    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &gHorseGanonRearingAnim, 1.0f);
                }
            } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
                switch (thisv->state) {
                    case 0:
                        if (globalCtx->csCtx.npcActions[1]->action == 4) {
                            Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_gndd_Anim_000F54, -5.0f);
                            thisv->state++;
                        }
                        break;
                    case 1:
                        if (animationEnded) {
                            Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_0014F4, -5.0f);
                            thisv->state++;
                        }
                        break;
                    case 2:
                        if (globalCtx->csCtx.npcActions[1]->action == 5) {
                            Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_gndd_Anim_0008A0, -5.0f);
                            thisv->state++;
                        }
                        break;
                    case 3:
                        if (animationEnded) {
                            Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_000BC8, -5.0f);
                            thisv->state++;
                        }
                        break;
                    case 4:
                        if (globalCtx->csCtx.npcActions[1]->action == 11) {
                            Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_0014F4, -20.0f);
                            thisv->state++;
                        }
                        break;
                    case 5:
                        if (globalCtx->csCtx.npcActions[1]->action == 8) {
                            Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_002928, -15.0f);
                            thisv->state++;
                        }
                        break;
                    case 6:
                        if (globalCtx->csCtx.npcActions[1]->action == 12) {
                            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GANON_VOICE_DEMO);
                            Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_gndd_Anim_0005B4, 3.0f);
                            thisv->state++;
                        }
                        break;
                    case 7:
                        thisv->state = 0;
                        break;
                }
            } else if (thisv->skin.skelAnime.animation != &gHorseGanonGallopingAnim &&
                       globalCtx->csCtx.npcActions[1]->action == 12) {
                Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &gHorseGanonGallopingAnim, 3.0f);
            }
        }
    } else if (type == ENVIEWER_TYPE_1_IMPA) {
        if (gSaveContext.sceneSetupIndex == 5) {
            if (globalCtx->csCtx.frames == 845) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_ITEM_OCARINA, 4.0f, 81.0f,
                                   2600.0f, 0, 0, 0, 0);
            }
        } else {
            if (globalCtx->csCtx.frames == 195) {
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_ITEM_OCARINA, 4.0f, 81.0f,
                                   2035.0f, 0, 0, 0, 1);
            }
        }
        switch (thisv->state) {
            case 0:
                if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[0] != NULL &&
                    globalCtx->csCtx.npcActions[0]->action == 6 &&
                    thisv->skin.skelAnime.animation != &object_opening_demo1_Anim_002574) {
                    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_opening_demo1_Anim_002574, 1.5f);
                    thisv->state++;
                }
                break;
            case 1:
                if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[0] != NULL &&
                    globalCtx->csCtx.npcActions[0]->action == 2 &&
                    thisv->skin.skelAnime.animation != &object_opening_demo1_Anim_0029CC) {
                    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_opening_demo1_Anim_0029CC, 3.0f);
                    thisv->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (globalCtx->sceneNum == SCENE_SPOT00) { // Hyrule Field
            switch (thisv->state) {
                case 0:
                    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
                        if (globalCtx->csCtx.npcActions[0] != NULL && globalCtx->csCtx.npcActions[0]->action == 6 &&
                            thisv->skin.skelAnime.animation != &object_opening_demo1_Anim_001410) {
                            Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_opening_demo1_Anim_001410, 1.5f);
                            thisv->state++;
                        }
                    }
                    break;
                case 1:
                    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
                        if (globalCtx->csCtx.npcActions[0] != NULL && globalCtx->csCtx.npcActions[0]->action == 2 &&
                            thisv->skin.skelAnime.animation != &object_opening_demo1_Anim_000450) {
                            Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_opening_demo1_Anim_000450, 3.0f);
                            thisv->state++;
                        }
                    }
                    break;
            }
        } else {
            Audio_SetBaseFilter(0);
            switch (thisv->state) {
                case 0:
                    Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_opening_demo1_Anim_00504C, 1.0f);
                    thisv->state++;
                    break;
                case 1:
                    if (globalCtx->csCtx.npcActions[0]->action == 11) {
                        Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_opening_demo1_Anim_00420C, -5.0f);
                        thisv->state++;
                    }
                    break;
                case 2:
                    if (animationEnded) {
                        Animation_MorphToLoop(&thisv->skin.skelAnime, &object_opening_demo1_Anim_0048FC, -5.0f);
                        thisv->state++;
                    }
                    break;
                case 3:
                    break;
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        switch (thisv->state) {
            case 0:
                if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[1] != NULL &&
                    globalCtx->csCtx.npcActions[1]->action == 7) {
                    Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_gndd_Anim_004534, -5.0f);
                    thisv->state++;
                }
                break;
            case 1:
                if (animationEnded) {
                    Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_0048B0, -5.0f);
                    thisv->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_8_GANONDORF) {
        switch (thisv->state) {
            case 0:
                if (globalCtx->csCtx.state != CS_STATE_IDLE) {
                    if (globalCtx->csCtx.npcActions[1] != NULL && globalCtx->csCtx.npcActions[1]->action == 9) {
                        Animation_PlayLoopSetSpeed(&thisv->skin.skelAnime, &object_gndd_Anim_0050A8, 1.0f);
                        thisv->state++;
                    }
                }
                break;
            case 1:
                if (globalCtx->csCtx.npcActions[1]->action == 10) {
                    Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_gndd_Anim_003284, -10.0f);
                    thisv->state++;
                }
                break;
            case 2:
                if (animationEnded) {
                    Animation_MorphToLoop(&thisv->skin.skelAnime, &object_gndd_Anim_003D84, -5.0f);
                    thisv->state++;
                }
                break;
            case 3:
                if (globalCtx->csCtx.npcActions[1]->action == 4) {
                    Animation_MorphToPlayOnce(&thisv->skin.skelAnime, &object_gndd_Anim_003428, -5.0f);
                    thisv->state++;
                }
                break;
            default:
                thisv->state = 0;
                break;
        }
    }
}

void EnViewer_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnViewer* thisv = (EnViewer*)thisx;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[thisv->animObjBankIndex].segment);
    thisv->actionFunc(thisv, globalCtx);
}

s32 EnViewer_Ganondorf3OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    if (gSaveContext.sceneSetupIndex == 4) {
        if (globalCtx->csCtx.frames >= 400) {
            if (limbIndex == 5) {
                *dList = object_gndd_DL_00E1A8;
            }
        }
    } else {
        if (globalCtx->csCtx.frames >= 1510 && globalCtx->csCtx.frames <= 1650) {
            if (limbIndex == 5) {
                *dList = object_gndd_DL_00E1A8;
            }
        }
    }
    return false;
}

void EnViewer_Ganondorf9PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 11) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1365);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1370),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(object_ganon_DL_00BE90));
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1372);
    }
}

void EnViewer_GanondorfPostLimbDrawUpdateCapeVec(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot,
                                                 void* thisx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 15) {
        Matrix_MultVec3f(&zeroVec, &sGanondorfNeckWorldPos);
    }
}

void EnViewer_DrawGanondorf(EnViewer* thisv, GlobalContext* globalCtx) {
    s16 frames = 0;
    s16 type;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1405);
    type = thisv->actor.params >> 8;
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF) {
        if (gSaveContext.sceneSetupIndex != 4) {
            frames = 149;
        }

        if (frames + 1127 >= globalCtx->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F178));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F178));

        } else if (frames + 1128 >= globalCtx->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F378));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F378));

        } else if (frames + 1129 >= globalCtx->csCtx.frames) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F578));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F578));

        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F778));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&object_gndd_Tex_00F778));
        }
    } else if (type == ENVIEWER_TYPE_9_GANONDORF) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&object_ganon_Tex_00A4E0));
    }

    if (type == ENVIEWER_TYPE_9_GANONDORF) {
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable,
                              thisv->skin.skelAnime.dListCount, NULL, EnViewer_Ganondorf9PostLimbDraw, thisv);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable,
                              thisv->skin.skelAnime.dListCount, EnViewer_Ganondorf3OverrideLimbDraw,
                              EnViewer_GanondorfPostLimbDrawUpdateCapeVec, thisv);
        EnViewer_UpdateGanondorfCape(globalCtx, thisv);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
               type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF) {
        if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[1] != NULL)) {
            SkelAnime_DrawFlexOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable,
                                  thisv->skin.skelAnime.dListCount, NULL, EnViewer_GanondorfPostLimbDrawUpdateCapeVec,
                                  thisv);
            EnViewer_UpdateGanondorfCape(globalCtx, thisv);
        }
    } else {
        SkelAnime_DrawOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable, NULL, NULL, thisv);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1511);
}

void EnViewer_DrawHorse(EnViewer* thisv, GlobalContext* globalCtx) {
    func_800A6330(&thisv->actor, globalCtx, &thisv->skin, NULL, true);
}

s32 EnViewer_ZeldaOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void* thisx) {
    if (globalCtx->sceneNum == SCENE_SPOT00) { // Hyrule Field
        if (limbIndex == 2) {
            *dList = gChildZeldaCutsceneDressDL;
        }
        if (limbIndex == 7) {
            *dList = NULL;
        }
        if (limbIndex == 8) {
            *dList = NULL;
        }
        if (limbIndex == 9) {
            *dList = NULL;
        }
        if (limbIndex == 3) {
            *dList = NULL;
        }
        if (limbIndex == 5) {
            *dList = NULL;
        }
    }
    return false;
}

void EnViewer_ZeldaPostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    s32 pad;

    if (globalCtx->sceneNum == SCENE_TOKINOMA) {
        if (limbIndex == 16) {
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1568);
            gSPDisplayList(POLY_OPA_DISP++, gChildZeldaOcarinaOfTimeDL);
            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1570);
        }
    }
}

void EnViewer_DrawZelda(EnViewer* thisv, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1583);
    if (globalCtx->sceneNum == SCENE_SPOT00) { // Hyrule Field
        if (globalCtx->csCtx.frames < 771) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        } else if (globalCtx->csCtx.frames < 772) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (globalCtx->csCtx.frames < 773) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else if (globalCtx->csCtx.frames < 791) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
        } else if (globalCtx->csCtx.frames < 792) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (globalCtx->csCtx.frames < 793) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        }

        if (gSaveContext.sceneSetupIndex == 6) {
            gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
        } else {
            if (globalCtx->csCtx.frames < 758) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            } else if (globalCtx->csCtx.frames < 848) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
            } else {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            }
        }
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
    }
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable,
                          thisv->skin.skelAnime.dListCount, EnViewer_ZeldaOverrideLimbDraw, EnViewer_ZeldaPostLimbDraw,
                          thisv);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1690);
}

s32 EnViewer_ImpaOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    if (limbIndex == 16) {
        *dList = gImpaHeadMaskedDL;
    }
    return false;
}

void EnViewer_DrawImpa(EnViewer* thisv, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1717);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skin.skelAnime.skeleton, thisv->skin.skelAnime.jointTable,
                          thisv->skin.skelAnime.dListCount, EnViewer_ImpaOverrideLimbDraw, NULL, thisv);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1740);
}

static EnViewerDrawFunc sDrawFuncs[] = {
    EnViewer_DrawGanondorf,
    EnViewer_DrawHorse,
    EnViewer_DrawZelda,
    EnViewer_DrawImpa,
};

void EnViewer_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnViewer* thisv = (EnViewer*)thisx;
    s32 pad;
    s16 type;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1760);
    if (thisv->isVisible) {
        type = thisv->actor.params >> 8;
        if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
            if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[0] != NULL) {
                func_80093D18(globalCtx->state.gfxCtx);
                sDrawFuncs[thisv->drawFuncIndex](thisv, globalCtx);
            }
        } else if ((globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[1] != NULL) ||
                   type == ENVIEWER_TYPE_9_GANONDORF) {
            func_80093D18(globalCtx->state.gfxCtx);
            sDrawFuncs[thisv->drawFuncIndex](thisv, globalCtx);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1784);
}

void EnViewer_UpdatePosition(EnViewer* thisv, GlobalContext* globalCtx) {
    Vec3f startPos;
    Vec3f endPos;
    f32 lerpFactor;
    s16 type = thisv->actor.params >> 8;

    if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
        if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[0] != NULL &&
            globalCtx->csCtx.frames < globalCtx->csCtx.npcActions[0]->endFrame) {
            if (type == ENVIEWER_TYPE_0_HORSE_ZELDA) {
                if (!sHorseSfxPlayed) {
                    sHorseSfxPlayed = true;
                    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                }
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
            }

            startPos.x = globalCtx->csCtx.npcActions[0]->startPos.x;
            startPos.y = globalCtx->csCtx.npcActions[0]->startPos.y;
            startPos.z = globalCtx->csCtx.npcActions[0]->startPos.z;
            endPos.x = globalCtx->csCtx.npcActions[0]->endPos.x;
            endPos.y = globalCtx->csCtx.npcActions[0]->endPos.y;
            endPos.z = globalCtx->csCtx.npcActions[0]->endPos.z;
            lerpFactor = Environment_LerpWeight(globalCtx->csCtx.npcActions[0]->endFrame,
                                                globalCtx->csCtx.npcActions[0]->startFrame, globalCtx->csCtx.frames);
            thisv->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            thisv->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            thisv->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;
        }
    } else { // ganondorf and ganondorf's horse
        if (globalCtx->csCtx.state != CS_STATE_IDLE && globalCtx->csCtx.npcActions[1] != NULL &&
            globalCtx->csCtx.frames < globalCtx->csCtx.npcActions[1]->endFrame) {
            startPos.x = globalCtx->csCtx.npcActions[1]->startPos.x;
            startPos.y = globalCtx->csCtx.npcActions[1]->startPos.y;
            startPos.z = globalCtx->csCtx.npcActions[1]->startPos.z;
            endPos.x = globalCtx->csCtx.npcActions[1]->endPos.x;
            endPos.y = globalCtx->csCtx.npcActions[1]->endPos.y;
            endPos.z = globalCtx->csCtx.npcActions[1]->endPos.z;
            lerpFactor = Environment_LerpWeight(globalCtx->csCtx.npcActions[1]->endFrame,
                                                globalCtx->csCtx.npcActions[1]->startFrame, globalCtx->csCtx.frames);
            thisv->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            thisv->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            thisv->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;

            if (globalCtx->csCtx.npcActions[1]->action == 12) {
                s16 yaw = Math_Vec3f_Yaw(&startPos, &endPos);

                Math_SmoothStepToS(&thisv->actor.world.rot.y, yaw, 0xA, 0x3E8, 1);
                Math_SmoothStepToS(&thisv->actor.shape.rot.y, yaw, 0xA, 0x3E8, 1);
            }

            if (type == ENVIEWER_TYPE_9_GANONDORF) {
                thisv->actor.world.rot.x = globalCtx->csCtx.npcActions[1]->urot.x;
                thisv->actor.world.rot.y = globalCtx->csCtx.npcActions[1]->urot.y;
                thisv->actor.world.rot.z = globalCtx->csCtx.npcActions[1]->urot.z;
                thisv->actor.shape.rot.x = globalCtx->csCtx.npcActions[1]->urot.x;
                thisv->actor.shape.rot.y = globalCtx->csCtx.npcActions[1]->urot.y;
                thisv->actor.shape.rot.z = globalCtx->csCtx.npcActions[1]->urot.z;
            }
        }
        if (type == ENVIEWER_TYPE_5_GANONDORF) {
            Audio_PlaySoundGeneral(NA_SE_EV_BURNING - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            EnViewer_DrawFireEffects(thisv, globalCtx);
        }
    }
}

void EnViewer_InitFireEffect(EnViewer* thisv, GlobalContext* globalCtx, s16 i) {
    EnViewerFireEffect* eff;

    if ((i % 2) == 0) {
        eff = &thisv->fireEffects[i];
        eff->startPos.x = 100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = 100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (Rand_ZeroOne() * 5.0f + 12.0f) * 0.001f;
    } else {
        eff = &thisv->fireEffects[i];
        eff->startPos.x = -100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = -100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (Rand_ZeroOne() * 5.0f + 12.0f) * 0.001f;
    }
    if (thisv) {}
}

void EnViewer_DrawFireEffects(EnViewer* this2, GlobalContext* globalCtx) {
    EnViewer* thisv = this2;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 1941);
    for (i = 0; i < ARRAY_COUNT(thisv->fireEffects); i++) {
        switch (thisv->fireEffects[i].state) {
            case 0:
                EnViewer_InitFireEffect(thisv, globalCtx, i);
                thisv->fireEffects[i].lerpFactor = (i >> 1) * 0.1f;
                thisv->fireEffects[i].lerpFactorSpeed = 0.01f;
                thisv->fireEffects[i].state++;
                break;
            case 1:
                Math_SmoothStepToF(&thisv->fireEffects[i].lerpFactor, 1.0f, 1.0f, thisv->fireEffects[i].lerpFactorSpeed,
                                   thisv->fireEffects[i].lerpFactorSpeed);
                thisv->fireEffects[i].pos.x =
                    thisv->fireEffects[i].startPos.x +
                    (thisv->fireEffects[i].endPos.x - thisv->fireEffects[i].startPos.x) * thisv->fireEffects[i].lerpFactor;
                thisv->fireEffects[i].pos.y =
                    thisv->fireEffects[i].startPos.y +
                    (thisv->fireEffects[i].endPos.y - thisv->fireEffects[i].startPos.y) * thisv->fireEffects[i].lerpFactor;
                thisv->fireEffects[i].pos.z =
                    thisv->fireEffects[i].startPos.z +
                    (thisv->fireEffects[i].endPos.z - thisv->fireEffects[i].startPos.z) * thisv->fireEffects[i].lerpFactor;
                if (thisv->fireEffects[i].lerpFactor >= 1.0f) {
                    thisv->fireEffects[i].state++;
                }
                break;
            case 2:
                EnViewer_InitFireEffect(thisv, globalCtx, i);
                thisv->fireEffects[i].lerpFactor = 0.0f;
                thisv->fireEffects[i].lerpFactorSpeed = 0.01f;
                thisv->fireEffects[i].state--;
                break;
        }

        func_80093D84(globalCtx->state.gfxCtx);
        Matrix_Translate(thisv->fireEffects[i].pos.x, thisv->fireEffects[i].pos.y, thisv->fireEffects[i].pos.z,
                         MTXMODE_NEW);
        Matrix_Scale(thisv->fireEffects[i].scale, thisv->fireEffects[i].scale, thisv->fireEffects[i].scale, MTXMODE_APPLY);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (10 * i - 20 * globalCtx->state.frames) % 512, 32, 128));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 00, 255);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_viewer.c", 2027),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_viewer.c", 2034);
}

void EnViewer_UpdateGanondorfCape(GlobalContext* globalCtx, EnViewer* thisv) {
    static s16 yOscillationPhase = 0;
    Vec3f forearmModelOffset;
    Vec3f forearmWorldOffset;

    if ((thisv->actor.params >> 8) == ENVIEWER_TYPE_5_GANONDORF) {
        if (1) {}
        sGanondorfCape->backPush = BREG(54) / 10.0f;
        sGanondorfCape->backSwayMagnitude = (BREG(60) + 25) / 100.0f;
        sGanondorfCape->sideSwayMagnitude = (BREG(55) - 45) / 10.0f;
        sGanondorfCape->minY = -10000.0f;
        sGanondorfCape->minDist = 0.0f;
        sGanondorfCape->gravity = (BREG(67) - 10) / 10.0f;
        forearmModelOffset.x = KREG(16) - 13.0f;
        forearmModelOffset.y = KREG(17) + 3.0f + Math_SinS(yOscillationPhase) * KREG(20);
        forearmModelOffset.z = KREG(18) - 10.0f;
        yOscillationPhase += KREG(19) * 0x1000 + 0x2000;

        Matrix_RotateY((thisv->actor.shape.rot.y / (f32)0x8000) * std::numbers::pi_v<float>, MTXMODE_NEW);
        Matrix_MultVec3f(&forearmModelOffset, &forearmWorldOffset);
        sGanondorfCape->rightForearmPos.x = sGanondorfNeckWorldPos.x + forearmWorldOffset.x;
        sGanondorfCape->rightForearmPos.y = sGanondorfNeckWorldPos.y + forearmWorldOffset.y;
        sGanondorfCape->rightForearmPos.z = sGanondorfNeckWorldPos.z + forearmWorldOffset.z;
        forearmModelOffset.x = -(KREG(16) - 13.0f);
        Matrix_MultVec3f(&forearmModelOffset, &forearmWorldOffset);
        sGanondorfCape->leftForearmPos.x = sGanondorfNeckWorldPos.x + forearmWorldOffset.x;
        sGanondorfCape->leftForearmPos.y = sGanondorfNeckWorldPos.y + forearmWorldOffset.y;
        sGanondorfCape->leftForearmPos.z = sGanondorfNeckWorldPos.z + forearmWorldOffset.z;
    }
}
