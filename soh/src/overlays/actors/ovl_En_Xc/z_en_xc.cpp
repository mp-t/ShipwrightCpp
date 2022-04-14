/*
 * File: z_en_xc.c
 * Overlay: ovl_En_Xc
 * Description: Sheik
 */

#include "z_en_xc.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/object_xc/object_xc.h"
#include "scenes/overworld/spot05/spot05_scene.h"
#include "scenes/overworld/spot17/spot17_scene.h"
#include "scenes/indoors/tokinoma/tokinoma_scene.h"
#include "scenes/dungeons/ice_doukutu/ice_doukutu_scene.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnXc_Init(Actor* thisx, GlobalContext* globalCtx);
void EnXc_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnXc_Update(Actor* thisx, GlobalContext* globalCtx);
void EnXc_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnXc_Reset(void);

void EnXc_DrawNothing(Actor* thisx, GlobalContext* globalCtx);
void EnXc_DrawDefault(Actor* thisx, GlobalContext* globalCtx);
void EnXc_DrawPullingOutHarp(Actor* thisx, GlobalContext* globalCtx);
void EnXc_DrawHarp(Actor* thisx, GlobalContext* globalCtx);
void EnXc_DrawTriforce(Actor* thisx, GlobalContext* globalCtx);
void EnXc_DrawSquintingEyes(Actor* thisx, GlobalContext* globalCtx);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
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
    { 25, 80, 0, { 0, 0, 0 } },
};

static void* sEyeTextures[] = {
    gSheikEyeOpenTex,
    gSheikEyeHalfClosedTex,
    gSheikEyeShutTex,
};

void EnXc_InitCollider(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinderType1(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
}

void EnXc_UpdateCollider(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    Collider* colliderBase = &thisv->collider.base;
    s32 pad[3];

    Collider_UpdateCylinder(thisx, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, colliderBase);
}

void EnXc_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnXc_CalculateHeadTurn(EnXc* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->npcInfo.unk_18 = player->actor.world.pos;
    thisv->npcInfo.unk_14 = kREG(16) - 3.0f;
    func_80034A14(&thisv->actor, &thisv->npcInfo, kREG(17) + 0xC, 2);
}

void EnXc_SetEyePattern(EnXc* thisv) {
    s32 pad[3];
    s16* blinkTimer = &thisv->blinkTimer;
    s16* eyePattern = &thisv->eyeIdx;

    if (!DECR(*blinkTimer)) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyePattern = *blinkTimer;
    if (*eyePattern >= ARRAY_COUNT(sEyeTextures)) {
        *eyePattern = 0;
    }
}

void EnXc_SpawnNut(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f* pos = &thisv->actor.world.pos;
    s16 angle = thisv->actor.shape.rot.y;
    f32 x = (Math_SinS(angle) * 30.0f) + pos->x;
    f32 y = pos->y + 3.0f;
    f32 z = (Math_CosS(angle) * 30.0f) + pos->z;

    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_ARROW, x, y, z, 0xFA0, thisv->actor.shape.rot.y, 0,
                ARROW_CS_NUT);
}

void EnXc_BgCheck(EnXc* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 75.0f, 30.0f, 30.0f, 4);
}

s32 EnXc_AnimIsFinished(EnXc* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

CsCmdActorAction* EnXc_GetCsCmd(GlobalContext* globalCtx, s32 npcActionIdx) {
    CsCmdActorAction* action = NULL;

    if (globalCtx->csCtx.state != 0) {
        action = globalCtx->csCtx.npcActions[npcActionIdx];
    }
    return action;
}

s32 EnXc_CompareCsAction(EnXc* thisv, GlobalContext* globalCtx, u16 action, s32 npcActionIdx) {
    CsCmdActorAction* csCmdActorAction = EnXc_GetCsCmd(globalCtx, npcActionIdx);

    if (csCmdActorAction != NULL && csCmdActorAction->action == action) {
        return true;
    }
    return false;
}

s32 EnXc_CsActionsAreNotEqual(EnXc* thisv, GlobalContext* globalCtx, u16 action, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = EnXc_GetCsCmd(globalCtx, npcActionIdx);

    if (csCmdNPCAction && csCmdNPCAction->action != action) {
        return true;
    }
    return false;
}

void func_80B3C588(EnXc* thisv, GlobalContext* globalCtx, u32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = EnXc_GetCsCmd(globalCtx, npcActionIdx);
    Actor* thisx = &thisv->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        thisx->world.rot.x = thisx->shape.rot.x = csCmdNPCAction->rot.x;
        thisx->world.rot.y = thisx->shape.rot.y = csCmdNPCAction->rot.y;
        thisx->world.rot.z = thisx->shape.rot.z = csCmdNPCAction->rot.z;
    }
}

void func_80B3C620(EnXc* thisv, GlobalContext* globalCtx, s32 npcActionIdx) {
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, npcActionIdx);
    Vec3f* xcPos = &thisv->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 unk;

    if (npcAction != NULL) {
        unk =
            Environment_LerpWeightAccelDecel(npcAction->endFrame, npcAction->startFrame, globalCtx->csCtx.frames, 0, 0);
        startX = npcAction->startPos.x;
        startY = npcAction->startPos.y;
        startZ = npcAction->startPos.z;
        endX = npcAction->endPos.x;
        endY = npcAction->endPos.y;
        endZ = npcAction->endPos.z;
        xcPos->x = ((endX - startX) * unk) + startX;
        xcPos->y = ((endY - startY) * unk) + startY;
        xcPos->z = ((endZ - startZ) * unk) + startZ;
    }
}

void EnXc_ChangeAnimation(EnXc* thisv, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 reverseFlag) {
    s32 pad[2];
    AnimationHeader* animationSeg = SEGMENTED_TO_VIRTUAL(animation);
    f32 frameCount = Animation_GetLastFrame(&animationSeg->common);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (!reverseFlag) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        startFrame = frameCount;
        endFrame = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, animationSeg, playbackSpeed, startFrame, endFrame, mode, morphFrames);
}

void EnXc_CheckAndSetAction(EnXc* thisv, s32 check, s32 set) {
    if (check != thisv->action) {
        thisv->action = set;
    }
}

void func_80B3C7D4(EnXc* thisv, s32 action1, s32 action2, s32 action3) {
    if (action1 != thisv->action) {
        if (thisv->action == SHEIK_ACTION_PUT_HARP_AWAY) {
            thisv->action = action2;
        } else {
            thisv->action = action3;
        }
    }
}

s32 EnXc_NoCutscenePlaying(GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == 0) {
        return true;
    }
    return false;
}

void func_80B3C820(EnXc* thisv) {
    Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    thisv->action = SHEIK_ACTION_53;
}

void func_80B3C888(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_NoCutscenePlaying(globalCtx) && thisv->actor.params == SHEIK_TYPE_4) {
        func_80B3C820(thisv);
    }
}

void func_80B3C8CC(EnXc* thisv, GlobalContext* globalCtx) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if (skelAnime->jointTable[0].y >= skelAnime->baseTransl.y) {
        skelAnime->moveFlags |= 3;
        AnimationContext_SetMoveActor(globalCtx, &thisv->actor, skelAnime, 1.0f);
    }
}

void func_80B3C924(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80B3C964(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.baseTransl = thisv->skelAnime.jointTable[0];
    thisv->skelAnime.prevTransl = thisv->skelAnime.jointTable[0];
    thisv->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_80B3C9DC(EnXc* thisv) {
    thisv->skelAnime.moveFlags &= ~0x3;
}

void func_80B3C9EC(EnXc* thisv) {
    EnXc_ChangeAnimation(thisv, &gSheikArmsCrossedIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    thisv->action = SHEIK_ACTION_BLOCK_PEDESTAL;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->unk_30C = 1;
}

void func_80B3CA38(EnXc* thisv, GlobalContext* globalCtx) {
    // If Player is adult but hasn't learned Minuet of Forest
    if (!(gSaveContext.eventChkInf[5] & 1) && LINK_IS_ADULT) {
        thisv->action = SHEIK_ACTION_INIT;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

s32 EnXc_MinuetCS(EnXc* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == SHEIK_TYPE_MINUET) {
        Player* player = GET_PLAYER(globalCtx);
        f32 z = player->actor.world.pos.z;

        if (z < -2225.0f) {
            if (!Gameplay_InCsMode(globalCtx)) {
                globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(&gMinuetCs);
                gSaveContext.cutsceneTrigger = 1;
                gSaveContext.eventChkInf[5] |= 1;
                Item_Give(globalCtx, ITEM_SONG_MINUET);
                return true;
            }
        }
        return false;
    }
    return true;
}

void func_80B3CB58(EnXc* thisv, GlobalContext* globalCtx) {
    // If hasn't learned Bolero and Player is Adult
    if (!(gSaveContext.eventChkInf[5] & 2) && LINK_IS_ADULT) {
        thisv->action = SHEIK_ACTION_INIT;
    } else {
        Actor_Kill(&thisv->actor);
    }
}

s32 EnXc_BoleroCS(EnXc* thisv, GlobalContext* globalCtx) {
    Player* player;
    PosRot* posRot;

    if (thisv->actor.params == SHEIK_TYPE_BOLERO) {
        player = GET_PLAYER(globalCtx);
        posRot = &player->actor.world;
        if ((posRot->pos.x > -784.0f) && (posRot->pos.x < -584.0f) && (posRot->pos.y > 447.0f) &&
            (posRot->pos.y < 647.0f) && (posRot->pos.z > -446.0f) && (posRot->pos.z < -246.0f) &&
            !Gameplay_InCsMode(globalCtx)) {
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(&gDeathMountainCraterBoleroCs);
            gSaveContext.cutsceneTrigger = 1;
            gSaveContext.eventChkInf[5] |= 2;
            Item_Give(globalCtx, ITEM_SONG_BOLERO);
            return true;
        }
        return false;
    }
    return true;
}

void EnXc_SetupSerenadeAction(EnXc* thisv, GlobalContext* globalCtx) {
    // Player is adult and does not have iron boots and has not learned Serenade
    if ((!CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) && !(gSaveContext.eventChkInf[5] & 4)) && LINK_IS_ADULT) {
        thisv->action = SHEIK_ACTION_SERENADE;
        osSyncPrintf("水のセレナーデ シーク誕生!!!!!!!!!!!!!!!!!!\n");
    } else {
        Actor_Kill(&thisv->actor);
        osSyncPrintf("水のセレナーデ シーク消滅!!!!!!!!!!!!!!!!!!\n");
    }
}

s32 EnXc_SerenadeCS(EnXc* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.params == SHEIK_TYPE_SERENADE) {
        Player* player = GET_PLAYER(globalCtx);
        s32 stateFlags = player->stateFlags1;

        if (CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) && !(gSaveContext.eventChkInf[5] & 4) && !(stateFlags & 0x20000000) &&
            !Gameplay_InCsMode(globalCtx)) {
            Cutscene_SetSegment(globalCtx, &gIceCavernSerenadeCs);
            gSaveContext.cutsceneTrigger = 1;
            gSaveContext.eventChkInf[5] |= 4; // Learned Serenade of Water Flag
            Item_Give(globalCtx, ITEM_SONG_SERENADE);
            osSyncPrintf("ブーツを取った!!!!!!!!!!!!!!!!!!\n");
            return true;
        }
        osSyncPrintf("はやくブーツを取るべし!!!!!!!!!!!!!!!!!!\n");
        return false;
    }
    return true;
}

void EnXc_DoNothing(EnXc* thisv, GlobalContext* globalCtx) {
}

void EnXc_SetWalkingSFX(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Animation_OnFrame(&thisv->skelAnime, 11.0f) || Animation_OnFrame(&thisv->skelAnime, 23.0f)) {
        if (thisv->actor.bgCheckFlags & 1) {
            sfxId = SFX_FLAG;
            sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
            func_80078914(&thisv->actor.projectedPos, sfxId);
        }
    }
}

void EnXc_SetNutThrowSFX(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    u32 sfxId;
    s32 pad2;

    if (Animation_OnFrame(&thisv->skelAnime, 7.0f)) {
        if (thisv->actor.bgCheckFlags & 1) {
            sfxId = SFX_FLAG;
            sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
            func_80078914(&thisv->actor.projectedPos, sfxId);
        }
    }
    if (Animation_OnFrame(&thisv->skelAnime, 20.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_SK_SHOUT);
    }
}

void EnXc_SetLandingSFX(EnXc* thisv, GlobalContext* globalCtx) {
    u32 sfxId;
    s16 sceneNum = globalCtx->sceneNum;

    if ((gSaveContext.sceneSetupIndex != 4) || (sceneNum != SCENE_SPOT11)) {
        if (Animation_OnFrame(&thisv->skelAnime, 11.0f)) {
            sfxId = SFX_FLAG;
            sfxId += SurfaceType_GetSfx(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId);
            func_80078914(&thisv->actor.projectedPos, sfxId);
        }
    }
}

void EnXc_SetColossusAppearSFX(EnXc* thisv, GlobalContext* globalCtx) {
    static Vec3f sXyzDist;
    s16 sceneNum;

    if (gSaveContext.sceneSetupIndex == 4) {
        sceneNum = globalCtx->sceneNum;
        if (sceneNum == SCENE_SPOT11) {
            CutsceneContext* csCtx = &globalCtx->csCtx;
            u16 frameCount = csCtx->frames;
            f32 wDest[2];

            if (frameCount == 119) {
                Vec3f pos = { -611.0f, 728.0f, -2.0f };

                SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &pos, &sXyzDist, wDest);
                func_80078914(&sXyzDist, NA_SE_EV_JUMP_CONC);
            } else if (frameCount == 164) {
                Vec3f pos = { -1069.0f, 38.0f, 0.0f };
                s32 pad;

                SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &pos, &sXyzDist, wDest);
                func_80078914(&sXyzDist, NA_SE_PL_WALK_CONCRETE);
            }
        }
    }
}

void func_80B3D118(GlobalContext* globalCtx) {
    s16 sceneNum;

    if ((gSaveContext.sceneSetupIndex != 4) || (sceneNum = globalCtx->sceneNum, sceneNum != SCENE_SPOT11)) {
        func_800788CC(NA_SE_PL_SKIP);
    }
}

static Vec3f D_80B42DA0;

static s32 D_80B41D90 = 0;
void EnXc_SetColossusWindSFX(GlobalContext* globalCtx) {
    if (gSaveContext.sceneSetupIndex == 4) {
        static Vec3f sPos = { 0.0f, 0.0f, 0.0f };
        static f32 sMaxSpeed = 0.0f;
        static Vec3f D_80B42DB0;
        s32 pad;
        s16 sceneNum = globalCtx->sceneNum;

        if (sceneNum == SCENE_SPOT11) {
            CutsceneContext* csCtx = &globalCtx->csCtx;
            u16 frameCount = csCtx->frames;

            if ((frameCount >= 120) && (frameCount < 164)) {
                s32 pad;
                Vec3f* eye = &globalCtx->view.eye;

                if (D_80B41D90 != 0) {
                    f32 speed = Math3D_Vec3f_DistXYZ(&D_80B42DB0, eye) / 7.058922f;

                    sMaxSpeed = CLAMP_MIN(sMaxSpeed, speed);

                    osSyncPrintf("MAX speed = %f\n", sMaxSpeed);

                    speed = CLAMP_MAX(speed, 2.0f);
                    func_800F436C(&sPos, NA_SE_EV_FLYING_AIR - SFX_FLAG, 0.6f + (0.4f * speed));
                }

                D_80B42DB0.x = eye->x;
                D_80B42DB0.y = eye->y;
                D_80B42DB0.z = eye->z;
                D_80B41D90 = 1;
            }
        }
    }
}

static s32 sFlameSpawned = false;
void EnXc_SpawnFlame(EnXc* thisv, GlobalContext* globalCtx) {

    if (!sFlameSpawned) {
        CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 0);
        f32 xPos = npcAction->startPos.x;
        f32 yPos = npcAction->startPos.y;
        f32 zPos = npcAction->startPos.z;

        thisv->flameActor = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_LIGHT, xPos, yPos, zPos, 0, 0, 0, 5);
        sFlameSpawned = true;
    }
}

void EnXc_SetupFlamePos(EnXc* thisv, GlobalContext* globalCtx) {
    Vec3f* attachedPos;
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 0);

    if (thisv->flameActor != NULL) {
        attachedPos = &thisv->flameActor->world.pos;
        if (!thisv) {}
        attachedPos->x = npcAction->startPos.x;
        attachedPos->y = npcAction->startPos.y;
        attachedPos->z = npcAction->startPos.z;
    }
}

void EnXc_DestroyFlame(EnXc* thisv) {
    if (thisv->flameActor != NULL) {
        Actor_Kill(thisv->flameActor);
        thisv->flameActor = NULL;
    }
    Actor_Kill(&thisv->actor);
}

static s32 D_80B41DA8 = 1;
void EnXc_InitFlame(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 sceneNum = globalCtx->sceneNum;

    if (sceneNum == SCENE_SPOT17) {
        CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 0);
        if (npcAction != NULL) {
            s32 action = npcAction->action;

            if (D_80B41DA8 != action) {
                if (action != 1) {
                    EnXc_SpawnFlame(thisv, globalCtx);
                }

                if (action == 1) {
                    EnXc_DestroyFlame(thisv);
                }

                D_80B41DA8 = action;
            }

            EnXc_SetupFlamePos(thisv, globalCtx);
        }
    }
}

void func_80B3D48C(EnXc* thisv, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;
    CsCmdActorAction* linkAction = csCtx->linkAction;
    s16 yaw;

    if (linkAction != NULL) {
        yaw = linkAction->urot.y + 0x8000;
    } else {
        Player* player = GET_PLAYER(globalCtx);
        yaw = player->actor.world.rot.y + 0x8000;
    }

    thisv->actor.shape.rot.y = thisv->actor.world.rot.y = yaw;
}

AnimationHeader* EnXc_GetCurrentHarpAnim(GlobalContext* globalCtx, s32 index) {
    AnimationHeader* animation = &gSheikPlayingHarp5Anim;
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, index);

    if (npcAction != NULL) {
        u16 action = npcAction->action;

        if (action == 11) {
            animation = &gSheikPlayingHarp3Anim;
        } else if (action == 12) {
            animation = &gSheikPlayingHarp2Anim;
        } else if (action == 13) {
            animation = &gSheikPlayingHarp4Anim;
        } else if (action == 23) {
            animation = &gSheikPlayingHarpAnim;
        } else {
            animation = &gSheikPlayingHarp5Anim;
        }
    }
    return animation;
}

void EnXc_CalcXZAccel(EnXc* thisv) {
    f32 timer = thisv->timer;
    f32* speedXZ = &thisv->actor.speedXZ;

    if (timer < 9.0f) {
        *speedXZ = 0.0f;
    } else if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (timer - 9.0f);
    } else {
        *speedXZ = (kREG(2) * 0.01f) + 1.2f;
    }

    Actor_MoveForward(&thisv->actor);
}

void func_80B3D644(EnXc* thisv) {
    Actor_MoveForward(&thisv->actor);
}

void EnXc_CalcXZSpeed(EnXc* thisv) {
    f32 timer = thisv->timer;
    f32* speedXZ = &thisv->actor.speedXZ;

    if (timer < 3.0f) {
        *speedXZ = (((kREG(2) * 0.01f) + 1.2f) / 3.0f) * (3.0f - timer);
    } else {
        *speedXZ = 0.0f;
    }
    Actor_MoveForward(&thisv->actor);
}

void func_80B3D6F0(EnXc* thisv) {
    EnXc_CalcXZAccel(thisv);
}

void func_80B3D710(EnXc* thisv) {
    Actor_MoveForward(&thisv->actor);
}

void func_80B3D730(EnXc* thisv) {
    EnXc_CalcXZSpeed(thisv);
}

void func_80B3D750(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_MinuetCS(thisv, globalCtx) && EnXc_BoleroCS(thisv, globalCtx)) {
        thisv->action = SHEIK_ACTION_WAIT;
    }
}

void EnXc_SetupFallFromSkyAction(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    CutsceneContext* csCtx = &globalCtx->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorAction* npcAction = csCtx->npcActions[4];

        if (npcAction && npcAction->action == 2) {
            s32 pad;
            Vec3f* pos = &thisv->actor.world.pos;
            SkelAnime* skelAnime = &thisv->skelAnime;
            f32 frameCount = Animation_GetLastFrame(&gSheikFallingFromSkyAnim);

            thisv->action = SHEIK_ACTION_GRACEFUL_FALL;
            thisv->drawMode = SHEIK_DRAW_DEFAULT;

            pos->x = npcAction->startPos.x;
            pos->y = npcAction->startPos.y;
            pos->z = npcAction->startPos.z;

            func_80B3D48C(thisv, globalCtx);
            func_80B3C964(thisv, globalCtx);
            Animation_Change(skelAnime, &gSheikFallingFromSkyAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
            func_80B3D118(globalCtx);
        }
    }
}

void func_80B3D8A4(EnXc* thisv, GlobalContext* globalCtx, s32 animFinished) {
    if (animFinished) {
        SkelAnime* skelAnime = &thisv->skelAnime;
        f32 frameCount = Animation_GetLastFrame(&gSheikWalkingAnim);

        Animation_Change(skelAnime, &gSheikWalkingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);

        thisv->action = SHEIK_ACTION_ACCEL;
        thisv->timer = 0.0f;

        func_80B3C9DC(thisv);
        thisv->actor.gravity = -((kREG(1) * 0.01f) + 13.0f);
        thisv->actor.minVelocityY = -((kREG(1) * 0.01f) + 13.0f);
    } else {
        func_80B3C8CC(thisv, globalCtx);
    }
}

void EnXc_SetupWalkAction(EnXc* thisv) {
    f32* timer = &thisv->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        thisv->actor.speedXZ = (kREG(2) * 0.01f) + 1.2f;
        thisv->action = SHEIK_ACTION_WALK;
    }
}

void EnXc_SetupHaltAction(EnXc* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 xzDistToPlayer = thisv->actor.xzDistToPlayer;

    if (xzDistToPlayer <= (kREG(3) + 95.0f)) {
        f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

        Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -12.0f);
        thisv->action = SHEIK_ACTION_HALT;
        thisv->timer = 0.0f;
    }
}

void EnXc_SetupStoppedAction(EnXc* thisv) {
    f32* timer = &thisv->timer;

    *timer += 1.0f;
    if (*timer >= 12.0f) {
        thisv->action = SHEIK_ACTION_STOPPED;
        thisv->actor.speedXZ = 0.0f;
    }
}

void func_80B3DAF0(EnXc* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 4);
    u16 action;

    if (npcAction &&
        (action = npcAction->action, action == 3 || action == 11 || action == 12 || action == 13 || action == 23)) {
        f32 frameCount;

        frameCount = Animation_GetLastFrame(&gSheikPullingOutHarpAnim);
        Animation_Change(&thisv->skelAnime, &gSheikPullingOutHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -4.0f);
        thisv->action = SHEIK_ACTION_7;
        thisv->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
    }
}

void EnXc_SetupInitialHarpAction(EnXc* thisv, s32 animFinished) {
    SkelAnime* skelAnime;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &thisv->skelAnime;
        frameCount = Animation_GetLastFrame(&gSheikInitialHarpAnim);
        Animation_Change(skelAnime, &gSheikInitialHarpAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
        thisv->action = SHEIK_ACTION_HARP_READY;
        thisv->drawMode = SHEIK_DRAW_HARP;
    }
}

void EnXc_SetupPlayingHarpAction(EnXc* thisv, GlobalContext* globalCtx, s32 animFinished) {
    s32 pad;
    SkelAnime* skelAnime;
    AnimationHeader* animation;
    f32 frameCount;

    if (animFinished) {
        skelAnime = &thisv->skelAnime;
        animation = EnXc_GetCurrentHarpAnim(globalCtx, 4);
        frameCount = Animation_GetLastFrame(animation);
        Animation_Change(skelAnime, animation, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, -8.0f);
        thisv->action = SHEIK_PLAYING_HARP;
        thisv->drawMode = SHEIK_DRAW_HARP;
    }
}

void func_80B3DCA8(EnXc* thisv, GlobalContext* globalCtx) {
    f32 frameCount;

    if (globalCtx->csCtx.state != 0) {
        CsCmdActorAction* npcAction = globalCtx->csCtx.npcActions[4];

        if (npcAction != NULL && npcAction->action == 8) {
            frameCount = Animation_GetLastFrame(&gSheikInitialHarpAnim);
            Animation_Change(&thisv->skelAnime, &gSheikInitialHarpAnim, 0.0f, frameCount, frameCount, ANIMMODE_LOOP,
                             -8.0f);
            thisv->action = SHEIK_ACTION_10;
        }
    }
}

void EnXc_SetupHarpPutawayAction(EnXc* thisv, GlobalContext* globalCtx) {
    f32 curFrame;
    f32 animFrameCount;

    if (EnXc_CompareCsAction(thisv, globalCtx, 5, 4)) {
        curFrame = thisv->skelAnime.curFrame;
        animFrameCount = thisv->skelAnime.endFrame;
        if (curFrame >= animFrameCount) {
            Animation_Change(&thisv->skelAnime, &gSheikInitialHarpAnim, -1.0f,
                             Animation_GetLastFrame(&gSheikInitialHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
            thisv->action = SHEIK_ACTION_PUT_HARP_AWAY;
        }
    } else if (EnXc_CsActionsAreNotEqual(thisv, globalCtx, 8, 4)) {
        EnXc_SetupPlayingHarpAction(thisv, globalCtx, true);
    }
}

void func_80B3DE00(EnXc* thisv, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&thisv->skelAnime, &gSheikPullingOutHarpAnim, -1.0f,
                         Animation_GetLastFrame(&gSheikPullingOutHarpAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
        thisv->action = SHEIK_ACTION_12;
        thisv->drawMode = SHEIK_DRAW_PULLING_OUT_HARP;
    }
}

void func_80B3DE78(EnXc* thisv, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        thisv->action = SHEIK_ACTION_13;
        thisv->drawMode = SHEIK_DRAW_DEFAULT;
        thisv->timer = 0.0f;
    }
}

void EnXc_SetupReverseAccel(EnXc* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != 0) {
        CsCmdActorAction* npcAction = globalCtx->csCtx.npcActions[4];

        if (npcAction != NULL && npcAction->action == 4) {
            Animation_Change(&thisv->skelAnime, &gSheikWalkingAnim, -1.0f, Animation_GetLastFrame(&gSheikWalkingAnim),
                             0.0f, ANIMMODE_LOOP, -12.0f);
            thisv->action = SHEIK_ACTION_REVERSE_ACCEL;
            thisv->actor.world.rot.y += 0x8000;
            thisv->timer = 0.0f;
        }
    }
}

void EnXc_SetupReverseWalkAction(EnXc* thisv) {
    thisv->timer++;
    if (thisv->timer >= 12.0f) {
        thisv->actor.speedXZ = (kREG(2) * 0.01f) + 1.2f;
        thisv->action = SHEIK_ACTION_REVERSE_WALK;
    }
}

void EnXc_SetupReverseHaltAction(EnXc* thisv) {
    f32 xzDistToPlayer = thisv->actor.xzDistToPlayer;

    if (xzDistToPlayer >= kREG(5) + 140.0f) {
        Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        thisv->action = SHEIK_ACTION_REVERSE_HALT;
        thisv->timer = 0.0f;
    }
}

void EnXc_SetupNutThrow(EnXc* thisv) {
    thisv->timer++;
    if (thisv->timer >= 12.0f) {
        Animation_Change(&thisv->skelAnime, &gSheikThrowingNutAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSheikThrowingNutAnim), ANIMMODE_ONCE, 0.0f);
        thisv->action = SHEIK_ACTION_THROW_NUT;
        thisv->timer = 0.0f;
        thisv->actor.speedXZ = 0.0f;
    }
}

void func_80B3E164(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->timer++;
    if (thisv->timer >= 30.0f) {
        thisv->action = SHEIK_ACTION_DELETE;
        EnXc_SpawnNut(thisv, globalCtx);
    }
}

void EnXc_SetupDisappear(EnXc* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != 0) {
        CsCmdActorAction* npcAction = globalCtx->csCtx.npcActions[4];

        if (npcAction != NULL && npcAction->action == 9) {
            s16 sceneNum = globalCtx->sceneNum;

            // Sheik fades away if end of Bolero CS, kill actor otherwise
            if (sceneNum == SCENE_SPOT17) {
                thisv->action = SHEIK_ACTION_FADE;
                thisv->drawMode = SHEIK_DRAW_NOTHING;
                thisv->actor.shape.shadowAlpha = 0;
            } else {
                Actor_Kill(&thisv->actor);
            }
        }
    }
}

void EnXc_ActionFunc0(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_SetColossusAppearSFX(thisv, globalCtx);
    EnXc_SetColossusWindSFX(globalCtx);
    func_80B3D750(thisv, globalCtx);
}

void EnXc_ActionFunc1(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_SetColossusAppearSFX(thisv, globalCtx);
    EnXc_SetColossusWindSFX(globalCtx);
    EnXc_SetupFallFromSkyAction(thisv, globalCtx);
}

void EnXc_GracefulFall(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetLandingSFX(thisv, globalCtx);
    EnXc_SetColossusAppearSFX(thisv, globalCtx);
    EnXc_SetColossusWindSFX(globalCtx);
    func_80B3D8A4(thisv, globalCtx, animFinished);
}

void EnXc_Accelerate(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_CalcXZAccel(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_SetupWalkAction(thisv);
}

void EnXc_Walk(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D644(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_SetupHaltAction(thisv);
}

void EnXc_Stopped(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_CalcXZSpeed(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_SetupStoppedAction(thisv);
}

void EnXc_ActionFunc6(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    func_80B3DAF0(thisv, globalCtx);
}

void EnXc_ActionFunc7(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupInitialHarpAction(thisv, animFinished);
}

void EnXc_ActionFunc8(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupPlayingHarpAction(thisv, globalCtx, animFinished);
}

void EnXc_ActionFunc9(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    func_80B3DCA8(thisv, globalCtx);
}

void EnXc_ActionFunc10(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupHarpPutawayAction(thisv, globalCtx);
}

void EnXc_ActionFunc11(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    func_80B3DE00(thisv, animFinished);
}

void EnXc_ActionFunc12(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    func_80B3DE78(thisv, animFinished);
}

void EnXc_ActionFunc13(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_InitFlame(thisv, globalCtx);
    EnXc_SetupReverseAccel(thisv, globalCtx);
}

void EnXc_ReverseAccelerate(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D6F0(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_InitFlame(thisv, globalCtx);
    EnXc_SetupReverseWalkAction(thisv);
}

void EnXc_ActionFunc15(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D710(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_InitFlame(thisv, globalCtx);
    EnXc_SetupReverseHaltAction(thisv);
}

void EnXc_HaltAndWaitToThrowNut(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D730(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_InitFlame(thisv, globalCtx);
    EnXc_SetupNutThrow(thisv);
}

void EnXc_ThrowNut(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetNutThrowSFX(thisv, globalCtx);
    EnXc_InitFlame(thisv, globalCtx);
    func_80B3E164(thisv, globalCtx);
}

void EnXc_Delete(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_InitFlame(thisv, globalCtx);
    EnXc_SetupDisappear(thisv, globalCtx);
}

void EnXc_Fade(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_InitFlame(thisv, globalCtx);
}

void func_80B3E87C(Gfx** dList, EnXc* thisv) {
    f32 currentFrame = thisv->skelAnime.curFrame;

    if (currentFrame >= 34.0f) {
        *dList = gSheikHarpDL;
    }
}

s32 EnXc_PullingOutHarpOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    EnXc* thisv = (EnXc*)thisx;

    if (limbIndex == 12) {
        func_80B3E87C(dList, thisv);
    }

    return 0;
}

s32 EnXc_HarpOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnXc* thisv = (EnXc*)thisx;

    if (limbIndex == 12) {
        *dList = gSheikHarpDL;
    }

    return 0;
}

void EnXc_DrawPullingOutHarp(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = thisv->eyeIdx;
    void* eyeTexture = sEyeTextures[eyePattern];
    SkelAnime* skelAnime = &thisv->skelAnime;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1444);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    func_80093D18(gfxCtx);
    func_8002EBCC(&thisv->actor, globalCtx, 0);
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_PullingOutHarpOverrideLimbDraw, NULL, thisv);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1497);
}

void EnXc_DrawHarp(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    s32 pad;
    s16 eyePattern = thisv->eyeIdx;
    void* eyeTexture = sEyeTextures[eyePattern];
    SkelAnime* skelAnime = &thisv->skelAnime;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1511);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 20, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 60, 0, 0, 0);

    func_80093D18(gfxCtx);
    func_8002EBCC(&thisv->actor, globalCtx, 0);
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_HarpOverrideLimbDraw, NULL, thisv);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inSpot05.c", 1564);
}

void func_80B3EBF0(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->action = SHEIK_ACTION_20;
}

void func_80B3EC00(EnXc* thisv) {
    thisv->action = SHEIK_ACTION_21;
}

void func_80B3EC0C(EnXc* thisv, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorAction* npcAction = csCtx->npcActions[4];

        if ((npcAction != NULL) && (npcAction->action != 1)) {
            PosRot* posRot = &thisv->actor.world;
            Vec3i* startPos = &npcAction->startPos;
            ActorShape* shape = &thisv->actor.shape;

            posRot->pos.x = startPos->x;
            posRot->pos.y = startPos->y;
            posRot->pos.z = startPos->z;

            posRot->rot.y = shape->rot.y = npcAction->rot.y;

            thisv->action = SHEIK_ACTION_22;
            thisv->drawMode = SHEIK_DRAW_DEFAULT;
        }
    }
}

void func_80B3EC90(EnXc* thisv, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;

    if (csCtx->state != 0) {
        CsCmdActorAction* npcAction = csCtx->npcActions[4];

        if (npcAction != NULL && npcAction->action != 6) {
            func_80B3C9EC(thisv);
        }
    }
}

void func_80B3ECD8(EnXc* thisv) {
    thisv->timer++;
    if (thisv->timer >= 12.0f) {
        thisv->actor.speedXZ = kREG(2) * 0.01f + 1.2f;
        thisv->action = SHEIK_ACTION_24;
    }
}

void EnXc_ActionFunc20(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3EC00(thisv);
}

void EnXc_ActionFunc21(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3EC0C(thisv, globalCtx);
}

void EnXc_ActionFunc22(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    func_80B3EC90(thisv, globalCtx);
}

void EnXc_ActionFunc23(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D6F0(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    func_80B3ECD8(thisv);
}

void EnXc_ActionFunc24(EnXc* thisv, GlobalContext* globalCtx) {
}

void EnXc_ActionFunc25(EnXc* thisv, GlobalContext* globalCtx) {
}

void EnXc_ActionFunc26(EnXc* thisv, GlobalContext* globalCtx) {
}

void EnXc_ActionFunc27(EnXc* thisv, GlobalContext* globalCtx) {
}

void EnXc_ActionFunc28(EnXc* thisv, GlobalContext* globalCtx) {
}

void func_80B3EE64(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->action = SHEIK_ACTION_SERENADE;
}

void func_80B3EE74(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_SerenadeCS(thisv, globalCtx)) {
        thisv->action = SHEIK_ACTION_30;
    }
}

void func_80B3EEA4(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_30, SHEIK_ACTION_31);
}

void func_80B3EEC8(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_31, SHEIK_ACTION_32);
}

void func_80B3EEEC(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_32, SHEIK_ACTION_33);
}

void func_80B3EF10(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_33, SHEIK_ACTION_34);
}

void func_80B3EF34(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_34, SHEIK_ACTION_35);
}

void func_80B3EF58(EnXc* thisv) {
    func_80B3C7D4(thisv, SHEIK_ACTION_35, SHEIK_ACTION_36, SHEIK_ACTION_34);
}

void func_80B3EF80(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_36, SHEIK_ACTION_37);
}

void func_80B3EFA4(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_37, SHEIK_ACTION_38);
}

void func_80B3EFC8(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_38, SHEIK_ACTION_39);
}

void func_80B3EFEC(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_39, SHEIK_ACTION_40);
}

void func_80B3F010(EnXc* thisv) {
    f32 xzDistToPlayer = thisv->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        thisv->action = SHEIK_ACTION_41;
        thisv->timer = 0.0f;
    }
}

void func_80B3F0B8(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_41, SHEIK_ACTION_42);
}

void func_80B3F0DC(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_42, SHEIK_ACTION_43);
}

void func_80B3F100(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_43, SHEIK_ACTION_44);
}

void EnXc_Serenade(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3EE74(thisv, globalCtx);
}

void EnXc_ActionFunc30(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc21(thisv, globalCtx);
    func_80B3EEA4(thisv);
}

void EnXc_ActionFunc31(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc6(thisv, globalCtx);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B3EEC8(thisv);
}

void EnXc_ActionFunc32(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc7(thisv, globalCtx);
    func_80B3EEEC(thisv);
}

void EnXc_ActionFunc33(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc8(thisv, globalCtx);
    func_80B3EF10(thisv);
}

void EnXc_ActionFunc34(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc9(thisv, globalCtx);
    func_80B3EF34(thisv);
}

void EnXc_ActionFunc35(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc10(thisv, globalCtx);
    func_80B3EF58(thisv);
}

void EnXc_ActionFunc36(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc11(thisv, globalCtx);
    func_80B3EF80(thisv);
}

void EnXc_ActionFunc37(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc12(thisv, globalCtx);
    func_80B3EFA4(thisv);
}

void EnXc_ActionFunc38(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc13(thisv, globalCtx);
    func_80B3EFC8(thisv);
}

void EnXc_ActionFunc39(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ReverseAccelerate(thisv, globalCtx);
    func_80B3EFEC(thisv);
}

void EnXc_ActionFunc40(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D710(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    func_80B3F010(thisv);
}

void EnXc_ActionFunc41(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_HaltAndWaitToThrowNut(thisv, globalCtx);
    func_80B3F0B8(thisv);
}

void EnXc_ActionFunc42(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ThrowNut(thisv, globalCtx);
    func_80B3F0DC(thisv);
}

void EnXc_ActionFunc43(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_Delete(thisv, globalCtx);
    func_80B3F100(thisv);
}

void EnXc_ActionFunc44(EnXc* thisv, GlobalContext* globalCtx) {
}

void func_80B3F3C8(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->action = SHEIK_ACTION_45;
}

void func_80B3F3D8() {
    func_800788CC(NA_SE_PL_SKIP);
}

void EnXc_PlayDiveSFX(Vec3f* src, GlobalContext* globalCtx) {
    f32 wDest[2];

    SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, src, &D_80B42DA0, wDest);
    func_80078914(&D_80B42DA0, NA_SE_EV_DIVE_INTO_WATER);
}

void EnXc_LakeHyliaDive(GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = npcAction = EnXc_GetCsCmd(globalCtx, 0);

    if (npcAction != NULL) {
        Vec3f startPos;

        startPos.x = npcAction->startPos.x;
        startPos.y = npcAction->startPos.y;
        startPos.z = npcAction->startPos.z;

        EffectSsGRipple_Spawn(globalCtx, &startPos, 100, 500, 0);
        EffectSsGRipple_Spawn(globalCtx, &startPos, 100, 500, 10);
        EffectSsGRipple_Spawn(globalCtx, &startPos, 100, 500, 20);
        EffectSsGSplash_Spawn(globalCtx, &startPos, NULL, NULL, 1, 0);
        EnXc_PlayDiveSFX(&startPos, globalCtx);
    }
}

void func_80B3F534(GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;
    u16 frameCount = csCtx->frames;

    if (frameCount == 310) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DOOR_WARP1, -1044.0f, -1243.0f, 7458.0f, 0, 0, 0,
                    WARP_DESTINATION);
    }
}

static s32 D_80B41DAC = 1;
void func_80B3F59C(EnXc* thisv, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 0);

    if (npcAction != NULL) {
        s32 action = npcAction->action;

        if (action != D_80B41DAC) {
            switch (action) {
                case 2:
                    func_80B3F3D8();
                    break;
                case 3:
                    EnXc_LakeHyliaDive(globalCtx);
                    break;
                default:
                    break;
            }
            D_80B41DAC = action;
        }
    }
}

void func_80B3F620(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_45, SHEIK_ACTION_46);
}

void func_80B3F644(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_46, SHEIK_ACTION_47);
}

void func_80B3F668(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 4, 4)) {
        EnXc_ChangeAnimation(thisv, &gSheikWalkingAnim, ANIMMODE_LOOP, -12.0f, true);
        thisv->action = SHEIK_ACTION_48;
        thisv->actor.world.rot.y += 0x8000;
        thisv->timer = 0.0f;
    }
}

void func_80B3F6DC(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_48, SHEIK_ACTION_49);
}

void EnXc_SetupKneelAction(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 16, 4)) {
        EnXc_ChangeAnimation(thisv, &gSheikKneelingAnim, ANIMMODE_LOOP, 0.0f, false);
        thisv->action = SHEIK_ACTION_KNEEL;
    }
}

void func_80B3F754(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 22, 4)) {
        EnXc_ChangeAnimation(thisv, &gSheikAnim_01A048, ANIMMODE_LOOP, 0.0f, false);
        thisv->action = SHEIK_ACTION_51;
        func_80B3C588(thisv, globalCtx, 4);
    }
}

void func_80B3F7BC(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 9, 4)) {
        thisv->action = SHEIK_ACTION_52;
        thisv->drawMode = SHEIK_DRAW_NOTHING;
    }
}

void EnXc_ActionFunc45(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc20(thisv, globalCtx);
    func_80B3F620(thisv);
}

void EnXc_ActionFunc46(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc21(thisv, globalCtx);
    func_80B3F644(thisv);
}

void EnXc_ActionFunc47(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3F534(globalCtx);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B3F668(thisv, globalCtx);
}

void EnXc_ActionFunc48(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc23(thisv, globalCtx);
    func_80B3F6DC(thisv);
}

void EnXc_ActionFunc49(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D710(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetWalkingSFX(thisv, globalCtx);
    EnXc_SetupKneelAction(thisv, globalCtx);
}

void EnXc_Kneel(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    func_80B3F59C(thisv, globalCtx);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B3F754(thisv, globalCtx);
}

void EnXc_ActionFunc51(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    func_80B3F59C(thisv, globalCtx);
    func_80B3C620(thisv, globalCtx, 4);
    func_80B3F7BC(thisv, globalCtx);
}

void EnXc_ActionFunc52(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3F59C(thisv, globalCtx);
}

void func_80B3FA08(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->action = SHEIK_ACTION_53;
    thisv->triforceAngle = kREG(24) + 0x53FC;
}

void func_80B3FA2C(void) {
    func_800F3F3C(1);
}

void EnXc_PlayTriforceSFX(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    if (thisv->unk_2A8) {
        s32 pad;
        Vec3f src;
        Vec3f pos;
        Vec3f sp1C = { 0.0f, 0.0f, 0.0f };
        f32 wDest;

        Matrix_MultVec3f(&sp1C, &src);
        SkinMatrix_Vec3fMtxFMultXYZW(&globalCtx->viewProjectionMtxF, &src, &pos, &wDest);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &pos, 80, NA_SE_EV_TRIFORCE_MARK);
        thisv->unk_2A8 = 0;
    }
}

void func_80B3FAE0(EnXc* thisv) {
    if (Animation_OnFrame(&thisv->skelAnime, 38.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_SK_SHOUT);
        func_80B3FA2C();
    }
}

void EnXc_CalcTriforce(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    if (EnXc_CompareCsAction(thisv, globalCtx, 21, 4)) {
        thisv->unk_274 = 1;
        if (thisv->unk_2AC == 0) {
            thisv->unk_2AC = 1;
            thisv->unk_2A8 = 1;
        }
    } else if (EnXc_CompareCsAction(thisv, globalCtx, 19, 4)) {
        thisv->unk_274 = 2;
    }
    if (thisv->unk_274 != 0) {
        f32* timer = &thisv->timer;
        s32* prim = thisv->triforcePrimColor;
        s32* env = thisv->triforceEnvColor;
        f32* scale = thisv->triforceScale;

        if (thisv->unk_274 == 1) {
            if (*timer < kREG(25) + 40.0f) {
                f32 div = *timer / (kREG(25) + 40.0f);

                prim[2] = -85.0f * div + 255;
                prim[3] = 255.0f * div;
                env[1] = 100.0f * div + 100;
                *timer += 1.0f;
            } else {
                prim[2] = 170;
                prim[3] = 255;
                env[1] = 200;
            }
            scale[0] = kREG(19) * 0.1f + 40.0f;
            scale[1] = kREG(20) * 0.1f + 40.0f;
            scale[2] = kREG(21) * 0.1f + 40.0f;
        } else if (thisv->unk_274 == 2) {
            f32 maxTime = (kREG(25) + 40.0f) + (kREG(27) + 90.0f);

            if (*timer < maxTime) {
                f32 div = (*timer - (kREG(25) + 40.0f)) / (kREG(27) + 90.0f);
                scale[0] = (kREG(19) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(19) * 0.1f + 40.0f));
                scale[1] = (kREG(20) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(20) * 0.1f + 40.0f));
                scale[2] = (kREG(21) * 0.1f + 40.0f) + div * ((kREG(26) + 50.0f) * (kREG(21) * 0.1f + 40.0f));
                *timer += 1.0f;
            } else {
                scale[0] = (kREG(19) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
                scale[1] = (kREG(20) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
                scale[2] = (kREG(21) * 0.1f + 40.0f) * (kREG(26) + 50.0f);
            }
            thisv->triforceAngle += (s16)(kREG(28) + 0x2EE0);
        }
    }
}

void func_80B3FF0C(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CsActionsAreNotEqual(thisv, globalCtx, 1, 4)) {
        CutsceneContext* csCtx = &globalCtx->csCtx;

        if (csCtx->state != 0) {
            CsCmdActorAction* npcAction = globalCtx->csCtx.npcActions[4];

            if (npcAction != NULL) {
                PosRot* posRot = &thisv->actor.world;
                ActorShape* shape = &thisv->actor.shape;
                Vec3i* startPos = &npcAction->startPos;

                posRot->pos.x = startPos->x;
                posRot->pos.y = startPos->y;
                posRot->pos.z = startPos->z;

                posRot->rot.y = shape->rot.y = npcAction->rot.y;
            }
        }

        thisv->action = SHEIK_ACTION_54;
        thisv->drawMode = SHEIK_DRAW_DEFAULT;
    }
}

void EnXc_SetupShowTriforceAction(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 10, 4)) {
        Animation_Change(&thisv->skelAnime, &gSheikShowingTriforceOnHandAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSheikShowingTriforceOnHandAnim), ANIMMODE_ONCE, -8.0f);
        thisv->action = SHEIK_ACTION_SHOW_TRIFORCE;
        thisv->drawMode = SHEIK_DRAW_TRIFORCE;
    }
}

void EnXc_SetupShowTriforceIdleAction(EnXc* thisv, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&thisv->skelAnime, &gSheikShowingTriforceOnHandIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gSheikShowingTriforceOnHandIdleAnim), ANIMMODE_LOOP, 0.0f);
        thisv->action = SHEIK_ACTION_SHOW_TRIFORCE_IDLE;
    }
}
void func_80B400AC(EnXc* thisv, GlobalContext* globalCtx) {
    if (EnXc_CompareCsAction(thisv, globalCtx, 9, 4)) {
        Actor_Kill(&thisv->actor);
    }
}

void EnXc_ActionFunc53(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3FF0C(thisv, globalCtx);
}

void EnXc_ActionFunc54(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupShowTriforceAction(thisv, globalCtx);
    func_80B3C888(thisv, globalCtx);
}

void EnXc_ShowTriforce(EnXc* thisv, GlobalContext* globalCtx) {
    s32 animFinished = EnXc_AnimIsFinished(thisv);

    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_CalcTriforce(&thisv->actor, globalCtx);
    func_80B3FAE0(thisv);
    EnXc_SetupShowTriforceIdleAction(thisv, animFinished);
    func_80B3C888(thisv, globalCtx);
}

void EnXc_ShowTriforceIdle(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_CalcTriforce(&thisv->actor, globalCtx);
    func_80B400AC(thisv, globalCtx);
}

s32 EnXc_TriforceOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    if (limbIndex == 15) {
        *dList = gSheikDL_011620;
    }
    return 0;
}

void EnXc_TriforcePostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s32 pad[2];
    EnXc* thisv = (EnXc*)thisx;

    if (limbIndex == 15) {
        Vec3f vec = { 0.0f, 0.0f, 0.0f };
        EnXc_PlayTriforceSFX(&thisv->actor, globalCtx);
        Matrix_MultVec3f(&vec, &thisv->handPos);
        thisv->unk_2BC = 1;
    }
}

void EnXc_DrawTriforce(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    s32 pad;
    s16 eyeIdx = thisv->eyeIdx;
    void* eyeTexture = sEyeTextures[eyeIdx];
    SkelAnime* skelAnime = &thisv->skelAnime;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad2;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inMetamol.c", 565);
    if (thisv->unk_2BC != 0) {
        Mtx* mtx = Graph_Alloc(gfxCtx, sizeof(Mtx));
        s32* primColor = thisv->triforcePrimColor;
        s32* envColor = thisv->triforceEnvColor;
        f32* scale = thisv->triforceScale;

        Matrix_Push();
        Matrix_Translate(kREG(16) + 100.0f, kREG(17) + 4460.0f, kREG(18) + 1190.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(kREG(22), kREG(23), thisv->triforceAngle, MTXMODE_APPLY);
        Matrix_Scale(scale[0], scale[1], scale[2], MTXMODE_APPLY);
        Matrix_ToMtx(mtx, "../z_en_oA2_inMetamol.c", 602);
        Matrix_Pop();
        func_80093D84(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, primColor[2], primColor[3]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, envColor[1], 0, 128);
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gSheikDL_012970);
    }

    func_8002EBCC(thisx, globalCtx, 0);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_TriforceOverrideLimbDraw, EnXc_TriforcePostLimbDraw, thisv);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inMetamol.c", 668);
}

void func_80B40590(EnXc* thisv, GlobalContext* globalCtx) {
    thisv->action = SHEIK_ACTION_NOCTURNE_INIT;
    thisv->drawMode = SHEIK_DRAW_SQUINT;
}

void EnXc_SetThrownAroundSFX(EnXc* thisv) {
    SkelAnime* skelAnime = &thisv->skelAnime;

    if (Animation_OnFrame(skelAnime, 9.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_BOUND_GRASS);
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_SK_CRASH);
    } else if (Animation_OnFrame(skelAnime, 26.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_BOUND_GRASS);
    } else if (Animation_OnFrame(skelAnime, 28.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_GRASS);
    } else if (Animation_OnFrame(skelAnime, 34.0f)) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_PL_WALK_GRASS);
    }
}

void EnXc_PlayLinkScreamSFX(EnXc* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames == 1455) {
        func_800F3F3C(7);
    }
}

void EnXc_SetCrySFX(EnXc* thisv, GlobalContext* globalCtx) {
    CutsceneContext* csCtx = &globalCtx->csCtx;

    if (csCtx->frames == 869) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_SK_CRY_0);
    } else if (csCtx->frames == 939) {
        func_80078914(&thisv->actor.projectedPos, NA_SE_VO_SK_CRY_1);
    }
}

void func_80B406F8(Actor* thisx) {
    EnXc* thisv = (EnXc*)thisx;

    thisv->action = SHEIK_ACTION_NOCTURNE_INIT;
    thisv->drawMode = SHEIK_DRAW_NOTHING;
    thisv->actor.shape.shadowAlpha = 0;
}

void EnXc_SetupIdleInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    ActorShape* actorShape = &thisv->actor.shape;
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

    func_80B3C9DC(thisv);
    func_80B3C588(thisv, globalCtx, 4);
    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    thisv->action = SHEIK_ACTION_NOCTURNE_IDLE;
    thisv->drawMode = SHEIK_DRAW_SQUINT;
    actorShape->shadowAlpha = 255;
}

void EnXc_SetupDefenseStance(Actor* thisx) {
    EnXc* thisv = (EnXc*)thisx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikDefenseStanceAnim);

    Animation_Change(skelAnime, &gSheikDefenseStanceAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -8.0f);
    thisv->action = SHEIK_ACTION_DEFENSE_STANCE;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
}

void EnXc_SetupContortions(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B3C964(thisv, globalCtx);
    Animation_Change(skelAnime, &gSheikContortionsAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikContortionsAnim),
                     ANIMMODE_ONCE, 0.0f);
    thisv->action = SHEIK_ACTION_CONTORT;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupFallInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    SkelAnime* skelAnime = &thisv->skelAnime;
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);

    Animation_Change(skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B3C964(thisv, globalCtx);
    Animation_Change(skelAnime, &gSheikFallingFromContortionsAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gSheikFallingFromContortionsAnim), ANIMMODE_ONCE, 0.0f);
    thisv->action = SHEIK_ACTION_NOCTURNE_FALL;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupHittingGroundInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikHittingGroundAnim);

    func_80B3C9DC(thisv);
    func_80B3C588(thisv, globalCtx, 4);
    Animation_Change(&thisv->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    thisv->action = SHEIK_ACTION_NOCTURNE_HIT_GROUND;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void func_80B40A78(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikHittingGroundAnim);

    func_80B3C9DC(thisv);
    func_80B3C588(thisv, globalCtx, 4);
    Animation_Change(&thisv->skelAnime, &gSheikHittingGroundAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, 0.0f);
    thisv->action = SHEIK_ACTION_63;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void EnXc_SetupKneelInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikKneelingAnim);

    func_80B3C9DC(thisv);
    func_80B3C588(thisv, globalCtx, 4);
    Animation_Change(&thisv->skelAnime, &gSheikKneelingAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    thisv->action = SHEIK_ACTION_NOCTURNE_KNEEL;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void func_80B40BB4(EnXc* thisv, GlobalContext* globalCtx) {
    s32 pad[3];
    f32 frameCount = Animation_GetLastFrame(&gSheikIdleAnim);
    func_80B3C9DC(thisv);
    func_80B3C588(thisv, globalCtx, 4);
    Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    thisv->action = SHEIK_ACTION_65;
    thisv->drawMode = SHEIK_DRAW_DEFAULT;
    thisv->actor.shape.shadowAlpha = 255;
}

void func_80B40C50(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_65, SHEIK_ACTION_66);
}

void func_80B40C74(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_66, SHEIK_ACTION_67);
}

void func_80B40C98(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_67, SHEIK_ACTION_68);
}

void func_80B40CBC(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_68, SHEIK_ACTION_69);
}

void func_80B40CE0(EnXc* thisv) {
    func_80B3C7D4(thisv, SHEIK_ACTION_69, SHEIK_ACTION_70, SHEIK_ACTION_68);
}

void func_80B40D08(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_70, SHEIK_ACTION_71);
}

void func_80B40D2C(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_71, SHEIK_ACTION_72);
}

void func_80B40D50(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_72, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL);
}

void func_80B40D74(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL, SHEIK_ACTION_NOCTURNE_REVERSE_WALK);
}

void EnXc_SetupReverseHaltInNocturneCS(EnXc* thisv) {
    f32 xzDistToPlayer = thisv->actor.xzDistToPlayer;

    if (kREG(5) + 140.0f <= xzDistToPlayer) {
        Animation_Change(&thisv->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gSheikIdleAnim),
                         ANIMMODE_LOOP, -12.0f);
        thisv->action = SHEIK_ACTION_NOCTURNE_REVERSE_HALT;
        thisv->timer = 0.0f;
    }
}

void func_80B40E40(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_NOCTURNE_REVERSE_HALT, SHEIK_ACTION_NOCTURNE_THROW_NUT);
}

void func_80B40E64(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_NOCTURNE_THROW_NUT, SHEIK_ACTION_77);
}

void func_80B40E88(EnXc* thisv) {
    EnXc_CheckAndSetAction(thisv, SHEIK_ACTION_77, SHEIK_ACTION_78);
}

s32 EnXc_SetupNocturneState(Actor* thisx, GlobalContext* globalCtx) {
    CsCmdActorAction* npcAction = EnXc_GetCsCmd(globalCtx, 4);

    if (npcAction != NULL) {
        s32 action = npcAction->action;
        EnXc* thisv = (EnXc*)thisx;
        s32 prevAction = thisv->unk_26C;

        if (action != prevAction) {
            switch (action) {
                case 1:
                    func_80B406F8(thisx);
                    break;
                case 6:
                    EnXc_SetupIdleInNocturne(thisv, globalCtx);
                    break;
                case 20:
                    EnXc_SetupDefenseStance(thisx);
                    break;
                case 18:
                    EnXc_SetupContortions(thisv, globalCtx);
                    break;
                case 14:
                    EnXc_SetupFallInNocturne(thisv, globalCtx);
                    break;
                case 19:
                    EnXc_SetupHittingGroundInNocturne(thisv, globalCtx);
                    break;
                case 15:
                    func_80B40A78(thisv, globalCtx);
                    break;
                case 16:
                    EnXc_SetupKneelInNocturne(thisv, globalCtx);
                    break;
                case 17:
                    func_80B40BB4(thisv, globalCtx);
                    break;
                case 9:
                    Actor_Kill(thisx);
                    break;
                default:
                    osSyncPrintf("En_Oa2_Stalker_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }

            thisv->unk_26C = action;
            return 1;
        }
    }
    return 0;
}

void EnXc_InitialNocturneAction(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_IdleInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3C588(thisv, globalCtx, 4);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_DefenseStance(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_Contort(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_SetCrySFX(thisv, globalCtx);
    EnXc_AnimIsFinished(thisv);
    EnXc_SetEyePattern(thisv);
    if (!EnXc_SetupNocturneState(&thisv->actor, globalCtx)) {
        func_80B3C924(thisv, globalCtx);
        EnXc_BgCheck(thisv, globalCtx);
    }
}

void EnXc_FallInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_SetEyePattern(thisv);
    EnXc_SetThrownAroundSFX(thisv);
    if (!EnXc_SetupNocturneState(&thisv->actor, globalCtx)) {
        func_80B3C8CC(thisv, globalCtx);
        EnXc_BgCheck(thisv, globalCtx);
    }
}

void EnXc_HitGroundInNocturne(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_ActionFunc63(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_PlayLinkScreamSFX(thisv, globalCtx);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_KneelInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupNocturneState(&thisv->actor, globalCtx);
}

void EnXc_ActionFunc65(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc6(thisv, globalCtx);
    func_80B3C588(thisv, globalCtx, 4);
    func_80B40C50(thisv);
}

void EnXc_ActionFunc66(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc7(thisv, globalCtx);
    func_80B40C74(thisv);
}

void EnXc_ActionFunc67(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc8(thisv, globalCtx);
    func_80B40C98(thisv);
}

void EnXc_ActionFunc68(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc9(thisv, globalCtx);
    func_80B40CBC(thisv);
}

void EnXc_ActionFunc69(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc10(thisv, globalCtx);
    func_80B40CE0(thisv);
}

void EnXc_ActionFunc70(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc11(thisv, globalCtx);
    func_80B40D08(thisv);
}

void EnXc_ActionFunc71(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc12(thisv, globalCtx);
    func_80B40D2C(thisv);
}

void EnXc_ActionFunc72(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ActionFunc13(thisv, globalCtx);
    func_80B40D50(thisv);
}

void EnXc_ReverseAccelInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ReverseAccelerate(thisv, globalCtx);
    func_80B40D74(thisv);
}

void EnXc_ReverseWalkInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    func_80B3D710(thisv);
    EnXc_AnimIsFinished(thisv);
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupReverseHaltInNocturneCS(thisv);
}

void EnXc_ReverseHaltInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_HaltAndWaitToThrowNut(thisv, globalCtx);
    func_80B40E40(thisv);
}

void EnXc_ThrowNutInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_ThrowNut(thisv, globalCtx);
    func_80B40E64(thisv);
}

void EnXc_DeleteInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_Delete(thisv, globalCtx);
    func_80B40E88(thisv);
}

void EnXc_KillInNocturneCS(EnXc* thisv, GlobalContext* globalCtx) {
    Actor_Kill(&thisv->actor);
}

void EnXc_DrawSquintingEyes(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 839);
    func_80093D18(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gSheikEyeSquintingTex));
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          NULL);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2_inStalker.c", 854);
}

void EnXc_InitTempleOfTime(EnXc* thisv, GlobalContext* globalCtx) {
    if (LINK_IS_ADULT) {
        if (!(gSaveContext.eventChkInf[12] & 0x20)) {
            gSaveContext.eventChkInf[12] |= 0x20;
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gTempleOfTimeFirstAdultCs);
            gSaveContext.cutsceneTrigger = 1;
            func_80B3EBF0(thisv, globalCtx);
        } else if (!(gSaveContext.eventChkInf[5] & 0x20) && (gSaveContext.eventChkInf[4] & 0x100)) {
            gSaveContext.eventChkInf[5] |= 0x20;
            Item_Give(globalCtx, ITEM_SONG_PRELUDE);
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gTempleOfTimePreludeCs);
            gSaveContext.cutsceneTrigger = 1;
            thisv->action = SHEIK_ACTION_30;
        } else if (!(gSaveContext.eventChkInf[5] & 0x20)) {
            func_80B3C9EC(thisv);
        } else {
            Actor_Kill(&thisv->actor);
        }
    } else {
        Actor_Kill(&thisv->actor);
    }
}

void EnXc_SetupDialogueAction(EnXc* thisv, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        thisv->action = SHEIK_ACTION_IN_DIALOGUE;
    } else {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        if (INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE) {
            thisv->actor.textId = 0x7010;
        } else {
            thisv->actor.textId = 0x700F;
        }
        func_8002F2F4(&thisv->actor, globalCtx);
    }
}

void func_80B41798(EnXc* thisv, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING) {
        thisv->action = SHEIK_ACTION_BLOCK_PEDESTAL;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    }
}

void EnXc_BlockingPedestalAction(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_UpdateCollider(&thisv->actor, globalCtx);
    EnXc_CalculateHeadTurn(thisv, globalCtx);
    EnXc_AnimIsFinished(thisv);
    EnXc_SetEyePattern(thisv);
    EnXc_SetupDialogueAction(thisv, globalCtx);
}

void EnXc_ActionFunc80(EnXc* thisv, GlobalContext* globalCtx) {
    EnXc_BgCheck(thisv, globalCtx);
    EnXc_UpdateCollider(&thisv->actor, globalCtx);
    EnXc_CalculateHeadTurn(thisv, globalCtx);
    EnXc_AnimIsFinished(thisv);
    EnXc_SetEyePattern(thisv);
    func_80B41798(thisv, globalCtx);
}

static EnXcActionFunc sActionFuncs[] = {
    EnXc_ActionFunc0,
    EnXc_ActionFunc1,
    EnXc_GracefulFall,
    EnXc_Accelerate,
    EnXc_Walk,
    EnXc_Stopped,
    EnXc_ActionFunc6,
    EnXc_ActionFunc7,
    EnXc_ActionFunc8,
    EnXc_ActionFunc9,
    EnXc_ActionFunc10,
    EnXc_ActionFunc11,
    EnXc_ActionFunc12,
    EnXc_ActionFunc13,
    EnXc_ReverseAccelerate,
    EnXc_ActionFunc15,
    EnXc_HaltAndWaitToThrowNut,
    EnXc_ThrowNut,
    EnXc_Delete,
    EnXc_Fade,
    EnXc_ActionFunc20,
    EnXc_ActionFunc21,
    EnXc_ActionFunc22,
    EnXc_ActionFunc23,
    EnXc_ActionFunc24,
    EnXc_ActionFunc25,
    EnXc_ActionFunc26,
    EnXc_ActionFunc27,
    EnXc_ActionFunc28,
    EnXc_Serenade,
    EnXc_ActionFunc30,
    EnXc_ActionFunc31,
    EnXc_ActionFunc32,
    EnXc_ActionFunc33,
    EnXc_ActionFunc34,
    EnXc_ActionFunc35,
    EnXc_ActionFunc36,
    EnXc_ActionFunc37,
    EnXc_ActionFunc38,
    EnXc_ActionFunc39,
    EnXc_ActionFunc40,
    EnXc_ActionFunc41,
    EnXc_ActionFunc42,
    EnXc_ActionFunc43,
    EnXc_ActionFunc44,
    EnXc_ActionFunc45,
    EnXc_ActionFunc46,
    EnXc_ActionFunc47,
    EnXc_ActionFunc48,
    EnXc_ActionFunc49,
    EnXc_Kneel,
    EnXc_ActionFunc51,
    EnXc_ActionFunc52,
    EnXc_ActionFunc53,
    EnXc_ActionFunc54,
    EnXc_ShowTriforce,
    EnXc_ShowTriforceIdle,
    EnXc_InitialNocturneAction,
    EnXc_IdleInNocturne,
    EnXc_DefenseStance,
    EnXc_Contort,
    EnXc_FallInNocturne,
    EnXc_HitGroundInNocturne,
    EnXc_ActionFunc63,
    EnXc_KneelInNocturneCS,
    EnXc_ActionFunc65,
    EnXc_ActionFunc66,
    EnXc_ActionFunc67,
    EnXc_ActionFunc68,
    EnXc_ActionFunc69,
    EnXc_ActionFunc70,
    EnXc_ActionFunc71,
    EnXc_ActionFunc72,
    EnXc_ReverseAccelInNocturneCS,
    EnXc_ReverseWalkInNocturneCS,
    EnXc_ReverseHaltInNocturneCS,
    EnXc_ThrowNutInNocturneCS,
    EnXc_DeleteInNocturneCS,
    EnXc_KillInNocturneCS,
    EnXc_BlockingPedestalAction,
    EnXc_ActionFunc80,
};

void EnXc_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;
    s32 action = thisv->action;

    if ((action < 0) || (action >= ARRAY_COUNT(sActionFuncs)) || (sActionFuncs[action] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[action](thisv, globalCtx);
    }
}

void EnXc_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gSheikSkel, &gSheikIdleAnim, thisv->jointTable, thisv->morphTable,
                       ARRAY_COUNT(thisv->jointTable));
    EnXc_InitCollider(thisx, globalCtx);

    switch (thisv->actor.params) {
        case SHEIK_TYPE_1:
            func_80B3EBF0(thisv, globalCtx);
            break;
        case SHEIK_TYPE_2: // Beta Serenade Cutscene or Learning Prelude
            func_80B3EE64(thisv, globalCtx);
            break;
        case SHEIK_TYPE_3:
            func_80B3F3C8(thisv, globalCtx);
            break;
        case SHEIK_TYPE_4:
            func_80B3FA08(thisv, globalCtx);
            break;
        case SHEIK_TYPE_5:
            func_80B40590(thisv, globalCtx);
            break;
        case SHEIK_TYPE_MINUET:
            func_80B3CA38(thisv, globalCtx);
            break;
        case SHEIK_TYPE_BOLERO:
            func_80B3CB58(thisv, globalCtx);
            break;
        case SHEIK_TYPE_SERENADE:
            EnXc_SetupSerenadeAction(thisv, globalCtx);
            break;
        case SHEIK_TYPE_9:
            EnXc_InitTempleOfTime(thisv, globalCtx);
            break;
        case SHEIK_TYPE_0:
            EnXc_DoNothing(thisv, globalCtx);
            break;
        default:
            osSyncPrintf(VT_FGCOL(RED) " En_Oa2 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            EnXc_DoNothing(thisv, globalCtx);
    }
}

s32 EnXc_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnXc* thisv = (EnXc*)thisx;

    if (thisv->unk_30C != 0) {
        if (limbIndex == 9) {
            rot->x += thisv->npcInfo.unk_0E.y;
            rot->y -= thisv->npcInfo.unk_0E.x;
        } else if (limbIndex == 16) {
            rot->x += thisv->npcInfo.unk_08.y;
            rot->z += thisv->npcInfo.unk_08.x;
        }
    }
    return 0;
}

void EnXc_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 16) {
        EnXc* thisv = (EnXc*)thisx;
        Vec3f src = { 0.0f, 10.0f, 0.0f };
        Vec3f dest;

        Matrix_MultVec3f(&src, &dest);
        thisv->actor.focus.pos.x = dest.x;
        thisv->actor.focus.pos.y = dest.y;
        thisv->actor.focus.pos.z = dest.z;
        thisv->actor.focus.rot.x = thisv->actor.world.rot.x;
        thisv->actor.focus.rot.y = thisv->actor.world.rot.y;
        thisv->actor.focus.rot.z = thisv->actor.world.rot.z;
    }
}

void EnXc_DrawNothing(Actor* thisx, GlobalContext* globalCtx) {
}

void EnXc_DrawDefault(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnXc* thisv = (EnXc*)thisx;
    s16 eyeIdx = thisv->eyeIdx;
    void* eyeSegment = sEyeTextures[eyeIdx];
    SkelAnime* skelAnime = &thisv->skelAnime;
    GraphicsContext* localGfxCtx = globalCtx->state.gfxCtx;
    GraphicsContext* gfxCtx = localGfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_oA2.c", 1164);
    func_8002EBCC(&thisv->actor, globalCtx, 0);
    func_80093D18(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeSegment));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeSegment));
    SkelAnime_DrawFlexOpa(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnXc_OverrideLimbDraw, EnXc_PostLimbDraw, thisv);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2.c", 1207);
}

static EnXcDrawFunc sDrawFuncs[] = {
    EnXc_DrawNothing, EnXc_DrawDefault,  EnXc_DrawPullingOutHarp,
    EnXc_DrawHarp,    EnXc_DrawTriforce, EnXc_DrawSquintingEyes,
};

void EnXc_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnXc* thisv = (EnXc*)thisx;

    if (thisv->drawMode < 0 || thisv->drawMode > 5 || sDrawFuncs[thisv->drawMode] == NULL) {
        // "Draw mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sDrawFuncs[thisv->drawMode](thisx, globalCtx);
    }
}

const ActorInit En_Xc_InitVars = {
    ACTOR_EN_XC,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_XC,
    sizeof(EnXc),
    (ActorFunc)EnXc_Init,
    (ActorFunc)EnXc_Destroy,
    (ActorFunc)EnXc_Update,
    (ActorFunc)EnXc_Draw,
    (ActorResetFunc)EnXc_Reset,
};

void EnXc_Reset(void) {
    D_80B41D90 = 0;
    sFlameSpawned = false;
    D_80B41DA8 = 1;
    D_80B41DAC = 1;
}