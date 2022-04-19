#include "z_boss_dodongo.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "scenes/dungeons/ddan_boss/ddan_boss_room_1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BossDodongo_Init(Actor* thisx, GlobalContext* globalCtx);
void BossDodongo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BossDodongo_Update(Actor* thisx, GlobalContext* globalCtx);
void BossDodongo_Draw(Actor* thisx, GlobalContext* globalCtx);

void BossDodongo_SetupIntroCutscene(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_IntroCutscene(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_Walk(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_Inhale(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_BlowFire(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_Roll(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_SpawnFire(BossDodongo* thisv, GlobalContext* globalCtx, s16 arg2);
void BossDodongo_Explode(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_LayDown(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_Vulnerable(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_GetUp(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_SetupWalk(BossDodongo* thisv);
void BossDodongo_DeathCutscene(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_SetupDeathCutscene(BossDodongo* thisv);
void BossDodongo_Damaged(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_UpdateDamage(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_PlayerPosCheck(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_PlayerYawCheck(BossDodongo* thisv, GlobalContext* globalCtx);
f32 func_808C4F6C(BossDodongo* thisv, GlobalContext* globalCtx);
f32 func_808C50A8(BossDodongo* thisv, GlobalContext* globalCtx);
void BossDodongo_DrawEffects(GlobalContext* globalCtx);
void BossDodongo_UpdateEffects(GlobalContext* globalCtx);

ActorInit Boss_Dodongo_InitVars = {
    ACTOR_EN_DODONGO,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_KINGDODONGO,
    sizeof(BossDodongo),
    (ActorFunc)BossDodongo_Init,
    (ActorFunc)BossDodongo_Destroy,
    (ActorFunc)BossDodongo_Update,
    (ActorFunc)BossDodongo_Draw,
    NULL,
};

#include "z_boss_dodongo_data.cpp"

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x0C, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3000.0f, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 8200.0f, ICHAIN_STOP),
};

void func_808C1190(const char* arg0, u8* arg1, s16 arg2) {
    s16* tex = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(arg0));

    if (arg2[arg1] != 0) {
        tex[arg2 / 2] = 0;
    }
}

void func_808C11D0(const char* arg0, u8* arg1, s16 arg2) {
    s16* tex = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(arg0));

    if (arg1[arg2] != 0) {
        tex[arg2] = 0;
    }
}

void func_808C1200(const char* arg0, u8* arg1, s16 arg2) {
    s16* tex = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(arg0));

    if (arg1[arg2] != 0) {
        tex[arg2] = 0;
    }
}

void func_808C1230(const char* arg0, u8* arg1, s16 arg2) {
    s16 index;

    s16* tex = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(arg0));

    if (arg1[arg2] != 0) {
        index = ((arg2 & 0xF) + ((arg2 & 0xF0) * 2));
        tex[index + 16] = 0;
        tex[index] = 0;
    }
}

void func_808C1278(const char* arg0, u8* arg1, s16 arg2) {
    s16 index;

    s16* tex = reinterpret_cast<s16*>(ResourceMgr_LoadTexByName(arg0));

    if (arg1[arg2] != 0) {
        index = ((arg2 & 0xF) * 2) + ((arg2 & 0xF0) * 2);
        tex[index + 1] = 0;
        tex[index] = 0;
    }
}

void func_808C12C4(u8* arg1, s16 arg2) {
    func_808C1190(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015890), arg1, arg2);
    func_808C1200(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_017210), arg1, arg2);
    func_808C11D0(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015D90), arg1, arg2);
    func_808C11D0(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016390), arg1, arg2);
    func_808C11D0(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016590), arg1, arg2);
    func_808C11D0(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016790), arg1, arg2);
    func_808C1230(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015990), arg1, arg2);
    func_808C1230(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_015F90), arg1, arg2);
    func_808C1278(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016990), arg1, arg2);
    func_808C1278(SEGMENTED_TO_VIRTUAL(object_kingdodongo_Tex_016E10), arg1, arg2);
}

void func_808C1554(const char* arg0, const char* floorTexName, s32 arg2, f32 arg3) {
    auto* tex = ResourceMgr_LoadTexByName(arg0);
    auto* floorTex = ResourceMgr_LoadTexByName(floorTexName);

    u16* temp_s3 = reinterpret_cast<u16*>( SEGMENTED_TO_VIRTUAL(tex) );
    u16* temp_s1 = reinterpret_cast<u16*>( SEGMENTED_TO_VIRTUAL(floorTex) );
    s16 i;
    s16 i2;
    u16 sp54[2048];
    s16 temp;
    s16 temp2;

    for (i = 0; i < 2048; i += 32) {
        temp = sinf((((i / 32) + (s16)((arg2 * 50.0f) / 100.0f)) & 0x1F) * (std::numbers::pi_v<float> / 16)) * arg3;
        for (i2 = 0; i2 < 32; i2++) {
            sp54[i + ((temp + i2) & 0x1F)] = temp_s1[i + i2];
        }
    }
    for (i = 0; i < 32; i++) {
        temp = sinf(((i + (s16)((arg2 * 80.0f) / 100.0f)) & 0x1F) * (std::numbers::pi_v<float> / 16)) * arg3;
        temp *= 32;
        for (i2 = 0; i2 < 2048; i2 += 32) {
            temp2 = (temp + i2) & 0x7FF;
            temp_s3[i + temp2] = sp54[i + i2];
        }
    }
}

void func_808C17C8(GlobalContext* globalCtx, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, s16 arg5) {
    s16 i;
    BossDodongoEffect* eff = (BossDodongoEffect*)globalCtx->specialEffects;

    for (i = 0; i < arg5; i++, eff++) {
        if (eff->unk_24 == 0) {
            eff->unk_24 = 1;
            eff->unk_00 = *arg1;
            eff->unk_0C = *arg2;
            eff->unk_18 = *arg3;
            eff->unk_2C = arg4 / 1000.0f;
            eff->alpha = 255;
            eff->unk_25 = (s16)Rand_ZeroFloat(10.0f);
            break;
        }
    }
}

s32 BossDodongo_AteExplosive(BossDodongo* thisv, GlobalContext* globalCtx) {
    f32 dx;
    f32 dy;
    f32 dz;
    Actor* currentExplosive = globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    Actor* thisx = &thisv->actor;

    while (currentExplosive != NULL) {
        if (currentExplosive == thisx) {
            currentExplosive = currentExplosive->next;
            continue;
        }

        dx = currentExplosive->world.pos.x - thisv->mouthPos.x;
        dy = currentExplosive->world.pos.y - thisv->mouthPos.y;
        dz = currentExplosive->world.pos.z - thisv->mouthPos.z;

        if ((fabsf(dx) < 40.0f) && (fabsf(dy) < 40.0f) && (fabsf(dz) < 40.0f)) {
            Actor_Kill(currentExplosive);
            return true;
        }

        currentExplosive = currentExplosive->next;
    }

    return false;
}

void BossDodongo_Init(Actor* thisx, GlobalContext* globalCtx) {
    BossDodongo* thisv = (BossDodongo*)thisx;
    s16 i;
    u16* temp_s1_3;
    const u16* temp_s2;
    u32 temp_v0;

    globalCtx->specialEffects = &thisv->effects;
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 9200.0f, ActorShadow_DrawCircle, 250.0f);
    Actor_SetScale(&thisv->actor, 0.01f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &object_kingdodongo_Skel_01B310, &object_kingdodongo_Anim_00F0D8, NULL,
                   NULL, 0);
    Animation_PlayLoop(&thisv->skelAnime, &object_kingdodongo_Anim_00F0D8);
    thisv->unk_1F8 = 1.0f;
    BossDodongo_SetupIntroCutscene(thisv, globalCtx);
    thisv->health = 12;
    thisv->colorFilterMin = 995.0f;
    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->colorFilterMax = 1000.0f;
    thisv->unk_224 = 2.0f;
    thisv->unk_228 = 9200.0f;
    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, thisv->items);

    if (Flags_GetClear(globalCtx, globalCtx->roomCtx.curRoom.num)) { // KD is dead
		//WAT
        auto* temp_s1_3_ = ResourceMgr_LoadTexByName(gDodongosCavernBossLavaFloorTex);
		auto* temp_s2_ = ResourceMgr_LoadTexByName(sLavaFloorRockTex);
        temp_s1_3 = reinterpret_cast<u16*>(SEGMENTED_TO_VIRTUAL(temp_s1_3_));
        temp_s2 = reinterpret_cast<const u16*>(SEGMENTED_TO_VIRTUAL(temp_s2_));

        Actor_Kill(&thisv->actor);
        Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, -890.0f, -1523.76f,
                           -3304.0f, 0, 0, 0, WARP_DUNGEON_CHILD);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_BREAKWALL, -890.0f, -1523.76f, -3304.0f, 0, 0, 0, 0x6000);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART, -690.0f, -1523.76f, -3304.0f, 0, 0, 0, 0);

        for (i = 0; i < 2048; i++) {
            temp_v0 = i;
            temp_s1_3[temp_v0] = temp_s2[temp_v0];
        }
    }

    thisv->actor.flags &= ~ACTOR_FLAG_0;
}

void BossDodongo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BossDodongo* thisv = (BossDodongo*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
    Collider_DestroyJntSph(globalCtx, &thisv->collider);
}

void BossDodongo_SetupIntroCutscene(BossDodongo* thisv, GlobalContext* globalCtx) {
    s16 frames = Animation_GetLastFrame(&object_kingdodongo_Anim_00F0D8);

    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_00F0D8, 1.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);
    thisv->actionFunc = BossDodongo_IntroCutscene;
    thisv->csState = 0;
    thisv->unk_1BC = 1;
}

void BossDodongo_IntroCutscene(BossDodongo* thisv, GlobalContext* globalCtx) {
    f32 phi_f0;
    Camera* camera;
    Player* player;
    Vec3f sp60;
    Vec3f sp54;
    Vec3f sp48;

    player = GET_PLAYER(globalCtx);
    camera = Gameplay_GetCamera(globalCtx, MAIN_CAM);

    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }

    if (thisv->unk_198 != 0) {
        thisv->unk_198--;
    }

    if (thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }

    switch (thisv->csState) {
        case 0:
            if (player->actor.world.pos.y < -1223.76f) {
                thisv->csState = 1;
                thisv->actor.world.pos.x = -1390.0f;
                thisv->actor.world.pos.z = -3374.0f;
                thisv->unk_1A0 = 1;
            }
            break;
        case 1:
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 1);
            Gameplay_ClearAllSubCameras(globalCtx);
            thisv->cutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, 0, 1);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->cutsceneCamera, 7);
            thisv->csState = 2;
            thisv->unk_196 = 0x3C;
            thisv->unk_198 = 160;
            player->actor.world.pos.y = -1023.76f;
            thisv->cameraEye.y = player->actor.world.pos.y - 480.0f + 50.0f;
        case 2:
            if (thisv->unk_198 >= 131) {
                player->actor.world.pos.x = -890.0f;
                player->actor.world.pos.z = -2804.0f;

                player->actor.speedXZ = 0.0f;
                player->actor.shape.rot.y = player->actor.world.rot.y = 0x3FFF;

                thisv->cameraEye.x = -890.0f;
                thisv->cameraEye.z = player->actor.world.pos.z - 100.0f;

                thisv->cameraAt.x = player->actor.world.pos.x;
                thisv->cameraAt.y = player->actor.world.pos.y + 20.0f;
                thisv->cameraAt.z = player->actor.world.pos.z;
            }

            if (thisv->unk_198 == 110) {
                func_8002DF54(globalCtx, &thisv->actor, 9);
            }

            if (thisv->unk_198 == 5) {
                func_8002DF54(globalCtx, &thisv->actor, 12);
            }

            if (thisv->unk_198 < 6) {
                player->actor.shape.rot.y = -0x4001;
            } else {
                player->actor.shape.rot.y = 0x3FFF;
            }

            if (thisv->unk_198 < 60) {
                thisv->unk_1BC = 1;
            } else {
                thisv->unk_1BC = 2;
            }

            BossDodongo_Walk(thisv, globalCtx);

            if (thisv->unk_196 == 1) {
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
            }

            if (thisv->unk_196 == 0) {
                Math_SmoothStepToF(&thisv->cameraEye.x, thisv->vec.x + 30.0f, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
                Math_SmoothStepToF(&thisv->cameraEye.y, thisv->vec.y, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
                Math_SmoothStepToF(&thisv->cameraEye.z, thisv->vec.z + 10.0f, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
                Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
            } else {
                thisv->cameraAt.x = player->actor.world.pos.x;
                thisv->cameraAt.y = player->actor.world.pos.y + 20.0f;
                thisv->cameraAt.z = player->actor.world.pos.z;
            }

            if (gSaveContext.eventChkInf[7] & 2) {
                if (thisv->unk_198 == 100) {
                    thisv->actor.world.pos.x = -1114.0f;
                    thisv->actor.world.pos.z = -2804.0f;
                    thisv->actor.world.rot.y = 0x3FFF;
                    thisv->unk_1A2 = 0;
                    thisv->unk_1A0 = 2;
                    thisv->csState = 4;
                    thisv->unk_196 = 30;
                    thisv->unk_198 = 150;
                    thisv->unk_204 = 0.0f;
                    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, 0.0f);
                    SkelAnime_Update(&thisv->skelAnime);
                }
            } else if (thisv->unk_198 == 0) {
                thisv->csState = 3;
                thisv->unk_19E = 0x14;
                thisv->unk_204 = 0.0f;
            }
            break;
        case 3:
            BossDodongo_Walk(thisv, globalCtx);
            Math_SmoothStepToF(&thisv->unk_20C, sinf(thisv->unk_19E * 0.05f) * 0.1f, 1.0f, 0.01f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.x, thisv->vec.x + 90.0f, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.y, thisv->vec.y + 50.0f, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.z, thisv->vec.z, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.y, thisv->vec.y - 10.0f, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
            if (fabsf(player->actor.world.pos.x - thisv->actor.world.pos.x) < 200.0f) {
                thisv->csState = 4;
                thisv->unk_196 = 0x1E;
                thisv->unk_198 = 0x96;
                thisv->unk_204 = 0.0f;
                Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, -5.0f);
            }
            break;
        case 4:
            Math_SmoothStepToF(&thisv->unk_20C, 0.0f, 1.0f, 0.01f, 0.0f);

            if (gSaveContext.eventChkInf[7] & 2) {
                phi_f0 = -50.0f;
            } else {
                phi_f0 = 0.0f;
            }

            Math_SmoothStepToF(&thisv->cameraEye.x, player->actor.world.pos.x + phi_f0 + 70.0f, 0.2f,
                               thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.y, player->actor.world.pos.y + 10.0f, 0.2f, thisv->unk_204 * 20.0f,
                               0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.z, player->actor.world.pos.z - 60.0f, 0.2f, thisv->unk_204 * 20.0f,
                               0.0f);

            Math_SmoothStepToF(&thisv->cameraAt.x, thisv->vec.x, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.y, thisv->vec.y, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.z, thisv->vec.z, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);

            if (thisv->unk_196 == 0) {
                SkelAnime_Update(&thisv->skelAnime);
                Math_SmoothStepToF(&thisv->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
            }

            if (thisv->unk_198 == 0x64) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_OTAKEBI);
            }

            if (thisv->unk_198 == 0x5A) {
                if (!(gSaveContext.eventChkInf[7] & 2)) {
                    TitleCard_InitBossName(globalCtx, &globalCtx->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gKingDodongoTitleCardTex), 0xA0, 0xB4, 0x80, 0x28);
                }
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_FIRE_BOSS);
            }

            if (thisv->unk_198 == 0) {
                camera->eye = thisv->cameraEye;
                camera->eyeNext = thisv->cameraEye;
                camera->at = thisv->cameraAt;
                func_800C08AC(globalCtx, thisv->cutsceneCamera, 0);
                thisv->cutsceneCamera = 0;
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                BossDodongo_SetupWalk(thisv);
                thisv->unk_1DA = 50;
                thisv->unk_1BC = 0;
                player->actor.shape.rot.y = -0x4002;
                gSaveContext.eventChkInf[7] |= 2;
            }
            break;
    }

    if (thisv->cutsceneCamera != 0) {
        if (thisv->unk_1B6 != 0) {
            thisv->unk_1B6--;
        }

        sp60.x = thisv->cameraEye.x;
        phi_f0 = sinf((thisv->unk_1B6 * 3.1415f * 90.0f) / 180.0f);
        sp60.y = (thisv->unk_1B6 * phi_f0 * 0.7f) + thisv->cameraEye.y;
        sp60.z = thisv->cameraEye.z;

        sp54.x = thisv->cameraAt.x;
        phi_f0 = sinf((thisv->unk_1B6 * 3.1415f * 90.0f) / 180.0f);
        sp54.y = (thisv->unk_1B6 * phi_f0 * 0.7f) + thisv->cameraAt.y;
        sp54.z = thisv->cameraAt.z;

        sp48.x = thisv->unk_20C;
        sp48.y = 1.0f;
        sp48.z = thisv->unk_20C;

        Gameplay_CameraSetAtEyeUp(globalCtx, thisv->cutsceneCamera, &sp54, &sp60, &sp48);
    }
}

void BossDodongo_SetupDamaged(BossDodongo* thisv) {
    if (thisv->actionFunc != BossDodongo_Damaged) {
        Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_001074, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_kingdodongo_Anim_001074), ANIMMODE_ONCE, -5.0f);
        thisv->actionFunc = BossDodongo_Damaged;
    }

    thisv->unk_1DA = 100;
}

void BossDodongo_SetupExplode(BossDodongo* thisv) {
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_00E848, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_kingdodongo_Anim_00E848), ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossDodongo_Explode;
    thisv->unk_1B0 = 10;
    thisv->unk_1C0 = 2;
    thisv->unk_1DA = 35;
    thisv->unk_1FC = 50.0f;
    thisv->unk_200 = 300.0f;
}

void BossDodongo_SetupWalk(BossDodongo* thisv) {
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_01D934, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_kingdodongo_Anim_01D934), ANIMMODE_ONCE, -10.0f);
    thisv->unk_1AA = 0;
    thisv->actionFunc = BossDodongo_Walk;
    thisv->unk_1DA = 0;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->unk_1E4 = 0.0f;
}

void BossDodongo_SetupRoll(BossDodongo* thisv) {
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_00DF38, 1.0f, 0.0f, 59.0f, ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossDodongo_Roll;
    thisv->numWallCollisions = 0;
    thisv->unk_1DA = 27;
}

void BossDodongo_SetupBlowFire(BossDodongo* thisv) {
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_1E4 = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_0061D4, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_kingdodongo_Anim_0061D4), ANIMMODE_ONCE, 0.0f);
    thisv->actionFunc = BossDodongo_BlowFire;
    thisv->unk_1DA = 50;
    thisv->unk_1AE = 0;
}

void BossDodongo_SetupInhale(BossDodongo* thisv) {
    thisv->actor.speedXZ = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_008EEC, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_kingdodongo_Anim_008EEC), ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossDodongo_Inhale;
    thisv->unk_1DA = 100;
    thisv->unk_1AC = 0;
    thisv->unk_1E2 = 1;
}

void BossDodongo_Damaged(BossDodongo* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_SmoothStepToF(&thisv->unk_1F8, 1.0f, 0.5f, 0.02f, 0.001f);
    Math_SmoothStepToF(&thisv->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);

    if (Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&object_kingdodongo_Anim_001074))) {
        BossDodongo_SetupRoll(thisv);
    }
}

void BossDodongo_Explode(BossDodongo* thisv, GlobalContext* globalCtx) {
    static Color_RGBA8 dustPrimColor = { 255, 255, 0, 255 };
    static Color_RGBA8 dustEnvColor = { 255, 10, 0, 255 };
    s16 pad;
    Vec3f dustVel;
    Vec3f dustAcell;
    Vec3f dustPos;
    s16 i;

    Math_SmoothStepToF(&thisv->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_1DA == 0) {
        for (i = 0; i < 30; i++) {
            dustVel.x = Rand_CenteredFloat(20.0f);
            dustVel.y = Rand_CenteredFloat(20.0f);
            dustVel.z = Rand_CenteredFloat(20.0f);

            dustAcell.x = dustVel.x * -0.1f;
            dustAcell.y = dustVel.y * -0.1f;
            dustAcell.z = dustVel.z * -0.1f;

            dustPos.x = thisv->actor.world.pos.x + (dustVel.x * 3.0f);
            dustPos.y = thisv->actor.world.pos.y + 90.0f + (dustVel.y * 3.0f);
            dustPos.z = thisv->actor.world.pos.z + (dustVel.z * 3.0f);

            func_8002836C(globalCtx, &dustPos, &dustVel, &dustAcell, &dustPrimColor, &dustEnvColor, 500, 10, 10);
        }

        Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_004E0C, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_kingdodongo_Anim_004E0C), ANIMMODE_ONCE, -5.0f);
        thisv->actionFunc = BossDodongo_LayDown;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_BOMB_EXPLOSION);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DAMAGE);
        func_80033E88(&thisv->actor, globalCtx, 4, 10);
        thisv->health -= 2;

        // make sure not to die from the bomb explosion
        if (thisv->health <= 0) {
            thisv->health = 1;
        }
    }
}

void BossDodongo_LayDown(BossDodongo* thisv, GlobalContext* globalCtx) {
    thisv->unk_1BE = 10;
    Math_SmoothStepToF(&thisv->unk_1F8, 1.3f, 1.0f, 0.1f, 0.001f);
    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&object_kingdodongo_Anim_004E0C))) {
        Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_0042A8, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_kingdodongo_Anim_0042A8), ANIMMODE_LOOP, -5.0f);
        thisv->actionFunc = BossDodongo_Vulnerable;
        thisv->unk_1DA = 100;
    }
}

void BossDodongo_Vulnerable(BossDodongo* thisv, GlobalContext* globalCtx) {
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DOWN - SFX_FLAG);
    thisv->unk_1BE = 10;
    Math_SmoothStepToF(&thisv->unk_1F8, 1.0f, 0.5f, 0.02f, 0.001f);
    Math_SmoothStepToF(&thisv->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_1DA == 0) {
        Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_009D10, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_kingdodongo_Anim_009D10), ANIMMODE_ONCE, -5.0f);
        thisv->actionFunc = BossDodongo_GetUp;
    }
}

void BossDodongo_GetUp(BossDodongo* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&object_kingdodongo_Anim_009D10))) {
        BossDodongo_SetupRoll(thisv);
    }
}

void BossDodongo_BlowFire(BossDodongo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f unusedZeroVec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f unusedZeroVec2 = { 0.0f, 0.0f, 0.0f };

    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_CRY);
    }

    if (Animation_OnFrame(&thisv->skelAnime, 17.0f)) {
        thisv->unk_1C8 = 28;
    }

    if ((thisv->skelAnime.curFrame > 17.0f) && (thisv->skelAnime.curFrame < 35.0f)) {
        BossDodongo_SpawnFire(thisv, globalCtx, thisv->unk_1AE);
        thisv->unk_1AE++;
        Math_SmoothStepToF(&thisv->unk_244, 0.0f, 1.0f, 8.0f, 0.0f);
    }

    if (thisv->unk_1DA == 0) {
        BossDodongo_SetupRoll(thisv);
    }
}

void BossDodongo_Inhale(BossDodongo* thisv, GlobalContext* GlobalContext) {
    thisv->unk_1E2 = 1;

    if (thisv->unk_1AC > 20) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_BREATH - SFX_FLAG);
    }

    Math_SmoothStepToF(&thisv->unk_208, 0.05f, 1.0f, 0.005f, 0.0f);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_1DA == 0) {
        BossDodongo_SetupBlowFire(thisv);
    } else {
        thisv->unk_1AC++;

        if ((thisv->unk_1AC > 20) && (thisv->unk_1AC < 82) && BossDodongo_AteExplosive(thisv, GlobalContext)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DRINK);
            BossDodongo_SetupExplode(thisv);
        }
    }
}

static Vec3f sCornerPositions[] = {
    { -1390.0f, 0.0f, -3804.0f },
    { -1390.0f, 0.0f, -2804.0f },
    { -390.0f, 0.0f, -2804.0f },
    { -390.0f, 0.0f, -3804.0f },
};

void BossDodongo_Walk(BossDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f* sp4C;
    f32 sp48;
    f32 sp44;

    if (thisv->unk_1AA == 0) {
        if (Animation_OnFrame(&thisv->skelAnime, 14.0f)) {
            Animation_PlayLoop(&thisv->skelAnime, &object_kingdodongo_Anim_01CAE0);
            thisv->unk_1AA = 1;
        }
    } else if (thisv->unk_1BC != 2) {
        if (((s32)thisv->skelAnime.curFrame == 1) || ((s32)thisv->skelAnime.curFrame == 31)) {
            if ((s32)thisv->skelAnime.curFrame == 1) {
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->unk_410, 25.0f, 0xA, 8.0f, 0x1F4, 0xA, 0);
            } else {
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->unk_404, 25.0f, 0xA, 8.0f, 0x1F4, 0xA, 0);
            }

            if (thisv->unk_1BC != 0) {
                func_80078884(NA_SE_EN_DODO_K_WALK);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_WALK);
            }

            if (thisv->cutsceneCamera == 0) {
                func_80033E88(&thisv->actor, globalCtx, 4, 10);
            } else {
                thisv->unk_1B6 = 10;
                func_800A9F6C(0.0f, 180, 20, 100);
            }
        }
    }

    SkelAnime_Update(&thisv->skelAnime);
    sp4C = &sCornerPositions[thisv->unk_1A0];
    thisv->unk_1EC = 0.7f;
    Math_SmoothStepToF(&thisv->unk_1E4, thisv->unk_1EC * 4.0f, 1.0f, thisv->unk_1EC * 0.25f, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.x, sp4C->x, 0.3f, thisv->unk_1E4, 0.0f);
    Math_SmoothStepToF(&thisv->actor.world.pos.z, sp4C->z, 0.3f, thisv->unk_1E4, 0.0f);
    sp48 = sp4C->x - thisv->actor.world.pos.x;
    sp44 = sp4C->z - thisv->actor.world.pos.z;
    Math_SmoothStepToF(&thisv->unk_1E8, 2000.0f, 1.0f, thisv->unk_1EC * 80.0f, 0.0f);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_FAtan2F(sp48, sp44) * (0x8000 / std::numbers::pi_v<float>), 5,
                       (thisv->unk_1EC * thisv->unk_1E8), 5);
    Math_SmoothStepToS(&thisv->unk_1C4, 0, 2, 2000, 0);

    if ((fabsf(sp48) <= 5.0f) && (fabsf(sp44) <= 5.0f)) {
        thisv->unk_1E8 = 0.0f;
        thisv->unk_1E4 = 0.0f;
        if (thisv->unk_1A2 == 0) {
            thisv->unk_1A0++;
            if (thisv->unk_1A0 >= 4) {
                thisv->unk_1A0 = 0;
            }
        } else {
            thisv->unk_1A0--;
            if (thisv->unk_1A0 < 0) {
                thisv->unk_1A0 = 3;
            }
        }
    }

    if ((thisv->unk_1DA == 0) && (thisv->unk_1BC == 0)) {
        if ((thisv->actor.xzDistToPlayer < 500.0f) && (thisv->unk_1A4 != 0) && !thisv->playerPosInRange) {
            BossDodongo_SetupInhale(thisv);
            BossDodongo_SpawnFire(thisv, globalCtx, -1);
        }

        if (!thisv->playerPosInRange && !thisv->playerYawInRange) {
            BossDodongo_SetupRoll(thisv);
        }
    }
}

void BossDodongo_Roll(BossDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f* sp5C;
    Vec3f sp50;
    f32 sp4C;
    f32 sp48;

    thisv->actor.flags |= ACTOR_FLAG_24;
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_1DA == 10) {
        thisv->actor.velocity.y = 15.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_CRY);
    }

    if (thisv->unk_1DA == 1) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_COLI2);
    }

    sp5C = &sCornerPositions[thisv->unk_1A0];
    thisv->unk_1EC = 3.0f;

    if (thisv->unk_1DA == 0) {
        Math_SmoothStepToF(&thisv->unk_1E4, thisv->unk_1EC * 5.0f, 1.0f, thisv->unk_1EC * 0.25f, 0.0f);
        Math_SmoothStepToF(&thisv->actor.world.pos.x, sp5C->x, 1.0f, thisv->unk_1E4, 0.0f);
        Math_SmoothStepToF(&thisv->actor.world.pos.z, sp5C->z, 1.0f, thisv->unk_1E4, 0.0f);
        thisv->unk_1C4 += 2000;

        if (thisv->actor.bgCheckFlags & 1) {
            thisv->unk_228 = 7700.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);

            if ((thisv->unk_19E & 7) == 0) {
                Camera_AddQuake(&globalCtx->mainCamera, 2, 1, 8);
            }

            if (!(thisv->unk_19E & 1)) {
                Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40.0f, 3, 8.0f, 0x1F4, 0xA,
                                         0);
            }
        }
    }

    sp4C = sp5C->x - thisv->actor.world.pos.x;
    sp48 = sp5C->z - thisv->actor.world.pos.z;
    Math_SmoothStepToF(&thisv->unk_1E8, 2000.0f, 1.0f, thisv->unk_1EC * 100.0f, 0.0f);
    Math_SmoothStepToS(&thisv->actor.world.rot.y, Math_FAtan2F(sp4C, sp48) * (0x8000 / std::numbers::pi_v<float>), 5,
                       thisv->unk_1EC * thisv->unk_1E8, 0);

    if (fabsf(sp4C) <= 15.0f && fabsf(sp48) <= 15.0f) {
        thisv->numWallCollisions++;

        if (thisv->numWallCollisions >= 2) {
            if (thisv->unk_1A6 != 0) {
                thisv->unk_1A2 = 1 - thisv->unk_1A2;
            }

            thisv->unk_1E8 = 0.0f;
            thisv->unk_1E4 = 0.0f;
            BossDodongo_SetupWalk(thisv);
            thisv->unk_228 = 9200.0f;
            thisv->actor.velocity.y = 20.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_COLI);
            Camera_AddQuake(&globalCtx->mainCamera, 2, 6, 8);
            sp50.x = thisv->actor.world.pos.x;
            sp50.y = thisv->actor.world.pos.y + 60.0f;
            sp50.z = thisv->actor.world.pos.z;
            func_80033480(globalCtx, &sp50, 250.0f, 40, 800, 10, 0);
            func_80033E88(&thisv->actor, globalCtx, 6, 15);
        } else {
            thisv->actor.velocity.y = 15.0f;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_COLI2);
        }

        if (thisv->unk_1A2 == 0) {
            thisv->unk_1A0++;
            if (thisv->unk_1A0 >= 4) {
                thisv->unk_1A0 = 0;
            }
        } else {
            thisv->unk_1A0--;
            if (thisv->unk_1A0 < 0) {
                thisv->unk_1A0 = 3;
            }
        }
    }
}

void BossDodongo_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    BossDodongo* thisv = (BossDodongo*)thisx;
    f32 temp_f0;
    s16 i;
    Player* player = GET_PLAYER(globalCtx);
    Player* player2 = GET_PLAYER(globalCtx);
    s32 pad;

    thisv->unk_1E2 = 0;
    thisv->unk_19E++;

    if (thisv->unk_1DA != 0) {
        thisv->unk_1DA--;
    }

    if (thisv->unk_1DC != 0) {
        thisv->unk_1DC--;
    }

    if (thisv->unk_1DE != 0) {
        thisv->unk_1DE--;
    }

    if (thisv->unk_1C0 != 0) {
        thisv->unk_1C0--;
    }

    if (thisv->unk_1C8 != 0) {
        thisv->unk_1C8--;
    }

    temp_f0 = func_808C4F6C(thisv, globalCtx);

    if (temp_f0 > 0.0f) {
        thisv->unk_1A4 = temp_f0;
    } else {
        thisv->unk_1A4 = 0;
    }

    temp_f0 = func_808C50A8(thisv, globalCtx);

    if (temp_f0 > 0.0f) {
        thisv->unk_1A6 = temp_f0;
    } else {
        thisv->unk_1A6 = 0;
    }

    BossDodongo_PlayerYawCheck(thisv, globalCtx);
    BossDodongo_PlayerPosCheck(thisv, globalCtx);

    thisv->actionFunc(thisv, globalCtx);

    thisx->shape.rot.y = thisx->world.rot.y;

    Math_SmoothStepToF(&thisx->shape.yOffset, thisv->unk_228, 1.0f, 100.0f, 0.0f);
    Actor_MoveForward(thisx);
    BossDodongo_UpdateDamage(thisv, globalCtx);
    Actor_UpdateBgCheckInfo(globalCtx, thisx, 10.0f, 10.0f, 20.0f, 4);
    Math_SmoothStepToF(&thisv->unk_208, 0, 1, 0.001f, 0.0);
    Math_SmoothStepToF(&thisv->unk_20C, 0, 1, 0.001f, 0.0);

    if ((thisv->unk_19E % 128) == 0) {
        for (i = 0; i < 50; i++) {
            thisv->unk_324[i] = (Rand_ZeroOne() * 0.25f) + 0.5f;
        }
    }

    for (i = 0; i < 50; i++) {
        thisv->unk_25C[i] += thisv->unk_324[i];
    }

    if (thisv->unk_1C8 != 0) {
        if (thisv->unk_1C8 >= 11) {
            Math_SmoothStepToF(&thisv->unk_240, (thisv->unk_1C8 & 1) ? (40.0f) : (60.0f), 1.0f, 50.0f, 0.0f);
        } else {
            Math_SmoothStepToF(&thisv->unk_240, 0.0f, 1, 10.0f, 0.0);
        }

        if ((globalCtx->envCtx.adjLight1Color[2] == 0) && (globalCtx->envCtx.adjAmbientColor[2] == 0)) {
            globalCtx->envCtx.adjLight1Color[0] = (u8)thisv->unk_240;
            globalCtx->envCtx.adjLight1Color[1] = (u8)(thisv->unk_240 * 0.1f);
            globalCtx->envCtx.adjAmbientColor[0] = (u8)thisv->unk_240;
            globalCtx->envCtx.adjAmbientColor[1] = (u8)(thisv->unk_240 * 0.1f);
        }
    }

    if (thisv->unk_1BE != 0) {
        if (thisv->unk_1BE >= 1000) {
            Math_SmoothStepToF(&thisv->colorFilterR, 30.0f, 1, 20.0f, 0.0);
            Math_SmoothStepToF(&thisv->colorFilterG, 10.0f, 1, 20.0f, 0.0);
        } else {
            thisv->unk_1BE--;
            Math_SmoothStepToF(&thisv->colorFilterR, 255.0f, 1, 20.0f, 0.0);
            Math_SmoothStepToF(&thisv->colorFilterG, 0.0f, 1, 20.0f, 0.0);
        }

        Math_SmoothStepToF(&thisv->colorFilterB, 0.0f, 1, 20.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterMin, 900.0f, 1, 10.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterMax, 1099.0f, 1, 10.0f, 0.0);
    } else {
        Math_SmoothStepToF(&thisv->colorFilterR, globalCtx->lightCtx.fogColor[0], 1, 5.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterG, globalCtx->lightCtx.fogColor[1], 1.0f, 5.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterB, globalCtx->lightCtx.fogColor[2], 1.0f, 5.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterMin, globalCtx->lightCtx.fogNear, 1.0, 5.0f, 0.0);
        Math_SmoothStepToF(&thisv->colorFilterMax, 1000.0f, 1, 5.0f, 0.0);
    }

    if (player->actor.world.pos.y < -1000.0f) {
        s16 phi_s0_3;
        s16 sp90;
        s16 magma2DrawMode;
        s16 magmaScale = 0;

        if (thisv->unk_224 > 1.9f) {
            phi_s0_3 = 1;
            magma2DrawMode = 0;
            sp90 = 0;
        } else if (thisv->unk_224 > 1.7f) {
            phi_s0_3 = 3;
            sp90 = 1;
            if (globalCtx) {}
            magma2DrawMode = 0;
        } else if (thisv->unk_224 > 1.4f) {
            phi_s0_3 = 7;
            sp90 = 3;
            magma2DrawMode = Rand_ZeroOne() * 1.9f;
        } else if (thisv->unk_224 > 1.1f) {
            phi_s0_3 = 7;
            sp90 = 4095;
            magma2DrawMode = Rand_ZeroOne() * 1.9f;
        } else {
            phi_s0_3 = 1;
            sp90 = -1;
            magma2DrawMode = 1;
            magmaScale = ((s16)(Rand_ZeroOne() * 50)) - 50;
        }

        if (player2->csMode >= 10) {
            phi_s0_3 = -1;
        }

        if ((thisv->unk_19E & phi_s0_3) == 0) {
            static Color_RGBA8 magmaPrimColor[] = { { 255, 255, 0, 255 }, { 0, 0, 0, 150 } };
            static Color_RGBA8 magmaEnvColor[] = { { 255, 0, 0, 255 }, { 0, 0, 0, 0 } };
            Vec3f sp84;
            f32 temp_f12;
            f32 temp_f10;

            temp_f12 = Rand_ZeroOne() * 330.0f;
            temp_f10 = Rand_ZeroOne() * 6.28f;
            sp84.x = (sinf(temp_f10) * temp_f12) + (-890.0f);
            sp84.y = -1523.76f;
            sp84.z = (cosf(temp_f10) * temp_f12) + (-3304.0f);
            EffectSsGMagma2_Spawn(globalCtx, &sp84, &magmaPrimColor[magma2DrawMode], &magmaEnvColor[magma2DrawMode],
                                  10 - (magma2DrawMode * 5), magma2DrawMode, magmaScale + 100);
        }

        if ((thisv->unk_19E & sp90) == 0) {
            Vec3f sp6C = { 0.0f, 0.0f, 0.0f };
            Vec3f sp60 = { 0.0f, 0.0f, 0.0f };
            Vec3f sp54;
            f32 sp50 = Rand_ZeroOne() * 330.0f;
            f32 sp4C = Rand_ZeroOne() * 6.28f;

            sp54.x = sinf(sp4C) * sp50 + (-890.0f);
            sp54.y = -1523.76f;
            sp54.z = cosf(sp4C) * sp50 + (-3304.0f);
            EffectSsGMagma_Spawn(globalCtx, &sp54);
            for (i = 0; i < 4; i++) {
                sp60.y = 0.4f;
                sp60.x = Rand_CenteredFloat(0.5f);
                sp60.z = Rand_CenteredFloat(0.5f);
                sp50 = Rand_ZeroOne() * 330.0f;
                sp4C = Rand_ZeroOne() * 6.28f;
                sp54.x = sinf(sp4C) * sp50 + (-890.0f);
                sp54.y = -1513.76f;
                sp54.z = cosf(sp4C) * sp50 + (-3304.0f);
                func_808C17C8(globalCtx, &sp54, &sp6C, &sp60, ((s16)Rand_ZeroFloat(2.0f)) + 6, 0x50);
            }
        }

        func_808C1554(gDodongosCavernBossLavaFloorTex, sLavaFloorLavaTex, thisv->unk_19E, thisv->unk_224);
    }

    if (thisv->unk_1C6 != 0) {
        u16* ptr1 = reinterpret_cast<u16*>(ResourceMgr_LoadTexByName(sLavaFloorLavaTex));
        u16* ptr2 = reinterpret_cast<u16*>(ResourceMgr_LoadTexByName(sLavaFloorRockTex));
        s16 i2;

        for (i2 = 0; i2 < 20; i2++) {
            s16 new_var = thisv->unk_1C2 & 0x7FF;

            ptr1[new_var] = ptr2[new_var];
            thisv->unk_1C2 += 37;
        }
        Math_SmoothStepToF(&thisv->unk_224, 0.0f, 1.0f, 0.01f, 0.0f);
    }

    if (thisv->unk_1BC == 0) {
        if (thisv->actionFunc != BossDodongo_DeathCutscene) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }

        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);

        if (thisv->actionFunc == BossDodongo_Roll) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }

    thisv->collider.elements[0].dim.scale = (thisv->actionFunc == BossDodongo_Inhale) ? 0.0f : 1.0f;

    for (i = 6; i < 19; i++) {
        if (i != 12) {
            thisv->collider.elements[i].dim.scale = (thisv->actionFunc == BossDodongo_Roll) ? 0.0f : 1.0f;
        }
    }

    if (thisv->unk_244 != 0) {
        MREG(64) = 1;
        MREG(65) = 255;
        MREG(66) = 80;
        MREG(67) = 0;
        MREG(68) = (u8)thisv->unk_244;
    } else {
        MREG(64) = 0;
    }

    Math_SmoothStepToF(&thisv->unk_244, 0.0f, 1.0f, 2.0f, 0.0f);
    BossDodongo_UpdateEffects(globalCtx);
}

s32 BossDodongo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    f32 mtxScaleY;
    f32 mtxScaleZ;
    BossDodongo* thisv = (BossDodongo*)thisx;

    // required for matching
    if ((limbIndex == 6) || (limbIndex == 7)) {
        if (thisv->unk_25C) {}
        goto block_1;
    }
block_1:
    Matrix_TranslateRotateZYX(pos, rot);

    if (*dList != NULL) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_dodongo.c", 3787);

        mtxScaleZ = 1.0f;
        mtxScaleY = 1.0f;

        if ((limbIndex == 33) || (limbIndex == 48)) {
            mtxScaleY = mtxScaleZ = thisv->unk_1F8;
        }

        Matrix_Push();
        Matrix_Scale(1.0f, mtxScaleY, mtxScaleZ, MTXMODE_APPLY);

        if ((limbIndex != 6) && (limbIndex != 7)) {
            Matrix_RotateX(thisv->unk_25C[limbIndex] * 0.115f, MTXMODE_APPLY);
            Matrix_RotateY(thisv->unk_25C[limbIndex] * 0.13f, MTXMODE_APPLY);
            Matrix_RotateZ(thisv->unk_25C[limbIndex] * 0.1f, MTXMODE_APPLY);
            Matrix_Scale(1.0f - thisv->unk_208, thisv->unk_208 + 1.0f, 1.0f - thisv->unk_208, MTXMODE_APPLY);
            Matrix_RotateZ(-(thisv->unk_25C[limbIndex] * 0.1f), MTXMODE_APPLY);
            Matrix_RotateY(-(thisv->unk_25C[limbIndex] * 0.13f), MTXMODE_APPLY);
            Matrix_RotateX(-(thisv->unk_25C[limbIndex] * 0.115f), MTXMODE_APPLY);
        }

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_boss_dodongo.c", 3822),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_dodongo.c", 3826);
    }
    { s32 pad; } // Required to match
    return 1;
}

void BossDodongo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_808CA450 = { 5000.0f, -2500.0f, 0.0f };
    static Vec3f D_808CA45C = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_808CA468 = { 11500.0f, -3000.0f, 0.0f };
    static Vec3f D_808CA474 = { 5000.0f, -2000.0f, 0.0f };
    static Vec3f D_808CA480 = { 8000.0f, 0.0f, 0.0f };
    static Vec3f D_808CA48C = { 8000.0f, 0.0f, 0.0f };
    BossDodongo* thisv = (BossDodongo*)thisx;

    if (limbIndex == 6) {
        Matrix_MultVec3f(&D_808CA45C, &thisv->vec);
        Matrix_MultVec3f(&D_808CA450, &thisv->actor.focus.pos);
        Matrix_MultVec3f(&D_808CA468, &thisv->firePos);
        Matrix_MultVec3f(&D_808CA474, &thisv->mouthPos);
    } else if (limbIndex == 39) {
        Matrix_MultVec3f(&D_808CA480, &thisv->unk_410);
    } else if (limbIndex == 46) {
        Matrix_MultVec3f(&D_808CA48C, &thisv->unk_404);
    }
    Collider_UpdateSpheres(limbIndex, &thisv->collider);
}

void BossDodongo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BossDodongo* thisv = (BossDodongo*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_boss_dodongo.c", 3922);
    func_80093D18(globalCtx->state.gfxCtx);

    if ((thisv->unk_1C0 >= 2) && (thisv->unk_1C0 & 1)) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
    } else {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, (u32)thisv->colorFilterR, (u32)thisv->colorFilterG,
                                   (u32)thisv->colorFilterB, 0, thisv->colorFilterMin, thisv->colorFilterMax);
    }

    Matrix_RotateZ(thisv->unk_23C, MTXMODE_APPLY);
    Matrix_RotateX((thisv->unk_1C4 / 32768.0f) * 3.14159f, MTXMODE_APPLY);

    SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, BossDodongo_OverrideLimbDraw,
                      BossDodongo_PostLimbDraw, thisv);

    POLY_OPA_DISP = Gameplay_SetFog(globalCtx, POLY_OPA_DISP);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_boss_dodongo.c", 3981);

    BossDodongo_DrawEffects(globalCtx);
}

f32 func_808C4F6C(BossDodongo* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 zDiff;
    f32 sp2C;
    s32 pad;
    f32 temp_f2;
    f32 rotation;
    Player* player = GET_PLAYER(globalCtx);

    xDiff = player->actor.world.pos.x - thisv->actor.world.pos.x;
    zDiff = player->actor.world.pos.z - thisv->actor.world.pos.z;

    rotation = Math_CosS(-thisv->actor.world.rot.y);
    sp2C = (Math_SinS(-thisv->actor.world.rot.y) * zDiff) + (rotation * xDiff);
    rotation = Math_SinS(-thisv->actor.world.rot.y);
    temp_f2 = (Math_CosS(-thisv->actor.world.rot.y) * zDiff) + (-rotation * xDiff);

    if ((fabsf(sp2C) < 150.0f) && (temp_f2 >= 100.0f) && (temp_f2 <= 2000.0f)) {
        return temp_f2;
    }
    return -1.0f;
}

f32 func_808C50A8(BossDodongo* thisv, GlobalContext* globalCtx) {
    f32 xDiff;
    f32 zDiff;
    f32 sp2C;
    s32 pad;
    f32 temp_f2;
    f32 rotation;
    Player* player = GET_PLAYER(globalCtx);

    xDiff = player->actor.world.pos.x - thisv->actor.world.pos.x;
    zDiff = player->actor.world.pos.z - thisv->actor.world.pos.z;

    rotation = Math_CosS(-0x8000 - thisv->actor.world.rot.y);
    sp2C = (Math_SinS(-0x8000 - thisv->actor.world.rot.y) * zDiff) + (rotation * xDiff);
    rotation = Math_SinS(-0x8000 - thisv->actor.world.rot.y);
    temp_f2 = (Math_CosS(-0x8000 - thisv->actor.world.rot.y) * zDiff) + (-rotation * xDiff);

    if ((fabsf(sp2C) < 150.0f) && (100.0f <= temp_f2) && (temp_f2 <= 2000.0f)) {
        return temp_f2;
    }

    return -1.0f;
}

void BossDodongo_PlayerYawCheck(BossDodongo* thisv, GlobalContext* globalCtx) {
    s16 yawDiff = Actor_WorldYawTowardActor(&thisv->actor, &GET_PLAYER(globalCtx)->actor) - thisv->actor.world.rot.y;

    if ((yawDiff < 0x38E3) && (-0x38E3 < yawDiff)) {
        thisv->playerYawInRange = true;
    } else {
        thisv->playerYawInRange = false;
    }
}

void BossDodongo_PlayerPosCheck(BossDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f* temp_v1;
    s16 i;

    thisv->playerPosInRange = false;

    for (i = 0; i < 4; i++) {
        temp_v1 = &sCornerPositions[i];

        if ((fabsf(thisv->actor.world.pos.x - temp_v1->x) < 200.0f) &&
            (fabsf(thisv->actor.world.pos.z - temp_v1->z) < 200.0f)) {
            thisv->playerPosInRange = true;
            break;
        }
    }
}

void BossDodongo_SpawnFire(BossDodongo* thisv, GlobalContext* globalCtx, s16 params) {
    Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_BDFIRE, thisv->vec.x, thisv->vec.y - 20.0f,
                       thisv->vec.z, 0, thisv->actor.shape.rot.y, 0, params);
}

void BossDodongo_UpdateDamage(BossDodongo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    ColliderInfo* item1;
    u8 swordDamage;
    s32 damage;
    ColliderInfo* item2;
    s16 i;

    if ((thisv->health <= 0) && (thisv->actionFunc != BossDodongo_DeathCutscene)) {
        BossDodongo_SetupDeathCutscene(thisv);
        Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
        return;
    }

    if (thisv->unk_1C0 == 0) {
        if (thisv->actionFunc == BossDodongo_Inhale) {
            for (i = 0; i < 19; i++) {
                if (thisv->collider.elements[i].info.bumperFlags & 2) {
                    item1 = thisv->collider.elements[i].info.acHitInfo;
                    item2 = item1;

                    if ((item2->toucher.dmgFlags & 0x10) || (item2->toucher.dmgFlags & 4)) {
                        thisv->collider.elements[i].info.bumperFlags &= ~2;
                        thisv->unk_1C0 = 2;
                        BossDodongo_SetupWalk(thisv);
                        thisv->unk_1DA = 0x32;
                        return;
                    }
                }
            }
        }

        if (thisv->collider.elements->info.bumperFlags & 2) {
            thisv->collider.elements->info.bumperFlags &= ~2;
            item1 = thisv->collider.elements[0].info.acHitInfo;
            if ((thisv->actionFunc == BossDodongo_Vulnerable) || (thisv->actionFunc == BossDodongo_LayDown)) {
                swordDamage = damage = CollisionCheck_GetSwordDamage(item1->toucher.dmgFlags);

                if (damage != 0) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DAMAGE);
                    BossDodongo_SetupDamaged(thisv);
                    thisv->unk_1C0 = 5;
                    thisv->health -= swordDamage;
                }
            }
        }
    }
}

void BossDodongo_SetupDeathCutscene(BossDodongo* thisv) {
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_1E4 = 0.0f;
    Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_002D0C, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_kingdodongo_Anim_002D0C), ANIMMODE_ONCE, -5.0f);
    thisv->actionFunc = BossDodongo_DeathCutscene;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_DEAD);
    thisv->unk_1DA = 0;
    thisv->csState = 0;
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
    thisv->unk_1BC = 1;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
}

void BossDodongo_DeathCutscene(BossDodongo* thisv, GlobalContext* globalCtx) {
    Vec3f* cornerPos;
    Vec3f sp198;
    Vec3f sp184;
    f32 tempSin;
    f32 tempCos;
    f32 sp178;
    s16 i;
    Vec3f effectPos;
    Camera* camera;
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);

    switch (thisv->csState) {
        case 0:
            thisv->csState = 5;
            func_80064520(globalCtx, &globalCtx->csCtx);
            func_8002DF54(globalCtx, &thisv->actor, 1);
            thisv->cutsceneCamera = Gameplay_CreateSubCamera(globalCtx);
            Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_UNK3);
            Gameplay_ChangeCameraStatus(globalCtx, thisv->cutsceneCamera, CAM_STAT_ACTIVE);
            camera = Gameplay_GetCamera(globalCtx, MAIN_CAM);
            thisv->cameraEye.x = camera->eye.x;
            thisv->cameraEye.y = camera->eye.y;
            thisv->cameraEye.z = camera->eye.z;
            thisv->cameraAt.x = camera->at.x;
            thisv->cameraAt.y = camera->at.y;
            thisv->cameraAt.z = camera->at.z;
            break;
        case 5:
            tempSin = Math_SinS(thisv->actor.shape.rot.y - 0x1388) * 150.0f;
            tempCos = Math_CosS(thisv->actor.shape.rot.y - 0x1388) * 150.0f;
            Math_SmoothStepToF(&player->actor.world.pos.x, thisv->actor.world.pos.x + tempSin, 0.5f, 5.0f, 0.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, thisv->actor.world.pos.z + tempCos, 0.5f, 5.0f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_208, 0.07f, 1.0f, 0.005f, 0.0f);
            tempSin = Math_SinS(thisv->actor.world.rot.y) * 230.0f;
            tempCos = Math_CosS(thisv->actor.world.rot.y) * 230.0f;
            Math_SmoothStepToF(&thisv->cameraEye.x, thisv->actor.world.pos.x + tempSin, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraEye.y, thisv->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraEye.z, thisv->actor.world.pos.z + tempCos, 0.2f, 50.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.2f, 30.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraAt.y, thisv->actor.focus.pos.y - 70.0f, 0.2f, 30.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.2f, 30.0f, 0.1f);
            if (Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&object_kingdodongo_Anim_002D0C))) {
                Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_003CF8, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&object_kingdodongo_Anim_003CF8), ANIMMODE_ONCE, -1.0f);
                thisv->csState = 6;
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_BG_BREAKWALL, -890.0f, -1523.76f, -3304.0f, 0, 0, 0,
                            0x6000);
            }
            break;
        case 6:
            Math_SmoothStepToF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.2f, 30.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraAt.y, (thisv->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.2f, 30.0f, 0.1f);

            if (Animation_OnFrame(&thisv->skelAnime, Animation_GetLastFrame(&object_kingdodongo_Anim_003CF8))) {
                Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_00DF38, 1.0f, 30.0f, 59.0f, ANIMMODE_ONCE,
                                 -1.0f);
                thisv->csState = 7;
                thisv->unk_228 = 7700.0f;
                thisv->unk_204 = 0.0f;
                thisv->unk_1E4 = 0.0f;
                thisv->numWallCollisions = 0;
                thisv->unk_19E = 0;
            }
            break;
        case 7:
            thisv->unk_1C4 += 0x7D0;
            Math_SmoothStepToF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.2f, 30.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.y, (thisv->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.2f, 30.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraEye.x, -890.0f, 0.1f, thisv->unk_204 * 5.0f, 0.1f);
            Math_SmoothStepToF(&thisv->cameraEye.z, -3304.0f, 0.1f, thisv->unk_204 * 5.0f, 0.1f);
            Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.1f, 0.0f);
            if (thisv->unk_1DA == 1) {
                thisv->csState = 8;
                thisv->actor.speedXZ = thisv->unk_1E4 / 1.5f;
                if (thisv->unk_1A2 == 0) {
                    thisv->unk_238 = 250.0f;
                } else {
                    thisv->unk_238 = -250.0f;
                }
                thisv->unk_1DA = 1000;
                thisv->unk_234 = 2000.0f;
            } else {
                cornerPos = &sCornerPositions[thisv->unk_1A0];
                thisv->unk_1EC = 3.0f;
                Math_SmoothStepToF(&thisv->unk_1E4, thisv->unk_1EC * 5.0f, 1.0f, thisv->unk_1EC * 0.25f, 0.0f);
                tempSin = cornerPos->x - thisv->actor.world.pos.x;
                tempCos = cornerPos->z - thisv->actor.world.pos.z;
                sp178 = sqrtf(SQ(tempSin) + SQ(tempCos)) - 200.0f;
                if ((sqrtf(SQ(tempSin) + SQ(tempCos)) < 200.0f) || (thisv->unk_1DA != 0)) {
                    sp178 = 0.0f;
                }
                sp178 = CLAMP_MAX(sp178, 70.0f);
                thisv->unk_23C = (Math_SinS(thisv->unk_19E * 1000) * -50.0f) / 100.0f;

                sp198.x = Math_SinS(thisv->unk_19E * 1000) * sp178;
                sp198.y = sp198.z = 0.0f;

                Matrix_RotateY(thisv->actor.shape.rot.y * (std::numbers::pi_v<float> / 0x8000), MTXMODE_NEW);
                Matrix_MultVec3f(&sp198, &sp184);

                Math_SmoothStepToF(&thisv->actor.world.pos.x, cornerPos->x + sp184.x, 1.0f, thisv->unk_1E4, 0.0f);
                Math_SmoothStepToF(&thisv->actor.world.pos.z, cornerPos->z + sp184.z, 1.0f, thisv->unk_1E4, 0.0f);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);
                if ((thisv->unk_19E & 7) == 0) {
                    Camera_AddQuake(&globalCtx->mainCamera, 2, 1, 8);
                }
                if (!(thisv->unk_19E & 1)) {
                    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40.0f, 3, 8.0f, 0x1F4,
                                             0xA, 0);
                }
                tempSin = cornerPos->x - thisv->actor.world.pos.x;
                tempCos = cornerPos->z - thisv->actor.world.pos.z;
                Math_SmoothStepToF(&thisv->unk_1E8, 1500.0f, 1.0f, thisv->unk_1EC * 100.0f, 0.0f);
                Math_SmoothStepToS(&thisv->actor.world.rot.y, (Math_FAtan2F(tempSin, tempCos) * (0x8000 / std::numbers::pi_v<float>)), 5,
                                   (thisv->unk_1EC * thisv->unk_1E8), 0);

                if ((fabsf(tempSin) <= 15.0f) && (fabsf(tempCos) <= 15.0f)) {
                    Vec3f dustPos;

                    thisv->actor.velocity.y = 15.0f;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_COLI2);
                    if (thisv->unk_1A2 == 0) {
                        thisv->unk_1A0 = thisv->unk_1A0 + 1;
                        if (thisv->unk_1A0 >= 4) {
                            thisv->unk_1A0 = 0;
                        }
                    } else {
                        thisv->unk_1A0--;
                        if (thisv->unk_1A0 < 0) {
                            thisv->unk_1A0 = 3;
                        }
                    }
                    thisv->unk_1DA = 0xA;
                    dustPos.x = thisv->actor.world.pos.x;
                    dustPos.y = thisv->actor.world.pos.y + 60.0f;
                    dustPos.z = thisv->actor.world.pos.z;
                    func_80033480(globalCtx, &dustPos, 250.0f, 0x28, 0x320, 0xA, 0);
                }
            }
            break;
        case 8:
        case 9:
            if (thisv->unk_1DA == 884) {
                Animation_Change(&thisv->skelAnime, &object_kingdodongo_Anim_0042A8, 1.0f, 0.0f,
                                 (f32)Animation_GetLastFrame(&object_kingdodongo_Anim_0042A8), ANIMMODE_LOOP, -20.0f);
                tempSin = thisv->cameraEye.x - thisv->actor.world.pos.x;
                tempCos = thisv->cameraEye.z - thisv->actor.world.pos.z;
                thisv->unk_22C = sqrtf(SQ(tempSin) + SQ(tempCos));
                thisv->unk_230 = Math_FAtan2F(tempSin, tempCos);
                thisv->unk_1DC = 350;
                thisv->csState = 9;
            }
            if (thisv->unk_1DA < 854) {
                for (i = 0; i < 2; i++) {
                    func_808C12C4(D_808C7000, thisv->unk_1CC);
                    if (thisv->unk_1CC < 0xFF) {
                        thisv->unk_1CC++;
                    }
                }
            }
            if (thisv->unk_1DA < 984) {
                Math_SmoothStepToS(&thisv->unk_1C4, -0x4000, 0xA, 0x12C, 0);
            }
            if (thisv->unk_1DA == 904) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_END);
            }
            if (thisv->unk_1DA < 854) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_LAST - SFX_FLAG);
            }
            if (thisv->unk_1DA == 960) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_LAVA);
            }
            if (thisv->unk_1DA < 960) {
                Math_SmoothStepToF(&thisv->actor.shape.shadowScale, 0.0f, 1.0f, 10.0f, 0.0f);
                if (thisv->unk_1DA >= 710) {

                    if (thisv->unk_1DA == 710) {
                        Vec3f sp124[] = {
                            { -440.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -3754.0f },
                            { -1340.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -2854.0f },
                        };
                        Vec3f spF4[] = {
                            { -890.0f, 0.0f, -2854.0f },
                            { -440.0f, 0.0f, -3304.0f },
                            { -890.0f, 0.0f, -3754.0f },
                            { -1340.0f, 0.0f, -3304.0f },
                        };
                        Vec3f* phi_v0_2;

                        thisv->unk_1C6 = 1;
                        if (thisv->unk_1A2 == 0) {
                            phi_v0_2 = &sp124[thisv->unk_1A0];
                        } else {
                            phi_v0_2 = &spF4[thisv->unk_1A0];
                        }
                        player->actor.world.pos.x = phi_v0_2->x;
                        player->actor.world.pos.z = phi_v0_2->z;
                        thisv->unk_204 = 0.0f;
                    }
                    if (thisv->unk_1DA >= 885) {
                        Math_SmoothStepToF(&thisv->unk_228, 200.0, 0.2f, 100.0f, 0.0f);
                    } else {
                        Math_SmoothStepToF(&thisv->unk_228, -6600.0f, 0.2f, 30.0f, 0.0f);
                    }
                    {
                        static Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
                        static Vec3f dustAcell = { 0.0f, 1.0f, 0.0f };
                        static Color_RGBA8 dustPrimColor = { 255, 255, 100, 255 };
                        static Color_RGBA8 dustEnvColor = { 255, 100, 0, 255 };
                        s16 colorIndex;
                        Color_RGBA8 magmaPrimColor2[] = { { 255, 255, 0, 255 }, { 0, 0, 0, 100 } };
                        Color_RGBA8 magmaEnvColor2[] = { { 255, 0, 0, 255 }, { 0, 0, 0, 0 } };

                        effectPos.x = Rand_CenteredFloat(120.0f) + thisv->actor.focus.pos.x;
                        effectPos.y = Rand_ZeroFloat(50.0f) + thisv->actor.world.pos.y;
                        effectPos.z = Rand_CenteredFloat(120.0f) + thisv->actor.focus.pos.z;
                        func_8002836C(globalCtx, &effectPos, &dustVel, &dustAcell, &dustPrimColor, &dustEnvColor, 0x1F4,
                                      0xA, 0xA);
                        effectPos.x = Rand_CenteredFloat(120.0f) + thisv->actor.focus.pos.x;
                        effectPos.y = -1498.76f;
                        effectPos.z = Rand_CenteredFloat(120.0f) + thisv->actor.focus.pos.z;
                        colorIndex = (Rand_ZeroOne() * 1.9f);
                        EffectSsGMagma2_Spawn(globalCtx, &effectPos, &magmaPrimColor2[colorIndex],
                                              &magmaEnvColor2[colorIndex], 10 - (colorIndex * 5), colorIndex,
                                              (s16)(Rand_ZeroOne() * 100.0f) + 100);
                    }
                }
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_K_ROLL - SFX_FLAG);
                if (!(thisv->unk_19E & 1)) {
                    Actor_SpawnFloorDustRing(globalCtx, &thisv->actor, &thisv->actor.world.pos, 40.0f, 3, 8.0f, 0x1F4,
                                             0xA, 0);
                }
            }
            Math_SmoothStepToF(&thisv->actor.speedXZ, 0.0f, 0.2f, 0.1f, 0.0f);
            thisv->actor.world.rot.y += (s16)thisv->unk_238;
            thisv->unk_1C4 += (s16)thisv->unk_234;
            if (thisv->unk_1DA >= 0x367) {
                if (thisv->unk_1A2 == 0) {
                    if (thisv->unk_238 < 450.0f) {
                        thisv->unk_238 += 10.0f;
                    }
                } else if (-450.0f < thisv->unk_238) {
                    thisv->unk_238 -= 10.0f;
                }
            } else {
                Math_SmoothStepToF(&thisv->unk_238, 0.0f, 0.05f, 40.0f, 0.0f);
            }
            Math_SmoothStepToF(&thisv->unk_234, 0.0f, 0.2f, 17.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.x, thisv->actor.world.pos.x, 0.2f, 30.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.y, (thisv->actor.world.pos.y - 70.0f) + 130.0f, 0.2f, 20.0f, 0.0f);
            Math_SmoothStepToF(&thisv->cameraAt.z, thisv->actor.world.pos.z, 0.2f, 30.0f, 0.0f);
            if (thisv->csState == 9) {
                if (thisv->unk_1DA < 0x2C6) {
                    Vec3f spAC[] = { { -390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -3804.0f },
                                     { -1390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -2804.0f } };

                    Vec3f sp7C[] = { { -890.0f, 0.0f, -2804.0f },
                                     { -390.0f, 0.0f, -3304.0f },
                                     { -890.0f, 0.0f, -3804.0f },
                                     { -1390.0f, 0.0f, -3304.0f } };
                    Vec3f* sp78;
                    s32 pad74;

                    if (thisv->unk_1A2 == 0) {
                        sp78 = &spAC[thisv->unk_1A0];
                    } else {
                        sp78 = &sp7C[thisv->unk_1A0];
                    }

                    Math_SmoothStepToF(&thisv->cameraEye.x, sp78->x, 0.2f, thisv->unk_204 * 20.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->cameraEye.y, player->actor.world.pos.y + 30.0f, 0.1f,
                                       thisv->unk_204 * 20.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->cameraEye.z, sp78->z, 0.1f, thisv->unk_204 * 20.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.02f, 0.0f);
                } else {
                    if (thisv->unk_1A2 == 0) {
                        thisv->unk_230 += 0.01f;
                    } else {
                        thisv->unk_230 -= 0.01f;
                    }
                    Math_SmoothStepToF(&thisv->unk_22C, 220.0f, 0.1f, 5.0f, 0.1f);
                    tempSin = sinf(thisv->unk_230) * (*thisv).unk_22C;
                    tempCos = cosf(thisv->unk_230) * (*thisv).unk_22C;
                    Math_SmoothStepToF(&thisv->cameraEye.x, thisv->actor.world.pos.x + tempSin, 0.2f, 50.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->cameraEye.y, thisv->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->cameraEye.z, thisv->actor.world.pos.z + tempCos, 0.2f, 50.0f, 0.0f);
                    Math_SmoothStepToF(&thisv->unk_23C, 0.0f, 0.2f, 0.01f, 0.0f);
                }
            } else {

                if (thisv->unk_1A2 == 0) {
                    Math_SmoothStepToF(&thisv->unk_23C, -0.5f, 0.2f, 0.05f, 0.0f);
                } else {
                    Math_SmoothStepToF(&thisv->unk_23C, 0.5f, 0.2f, 0.05f, 0.0f);
                }

                Math_SmoothStepToF(&thisv->cameraEye.x, -890.0f, 0.1f, thisv->unk_204 * 5.0f, 0.1f);
                Math_SmoothStepToF(&thisv->cameraEye.z, -3304.0f, 0.1f, thisv->unk_204 * 5.0f, 0.1f);
                Math_SmoothStepToF(&thisv->unk_204, 1.0f, 1.0f, 0.05f, 0.0f);
            }

            if (thisv->unk_1DA == 820) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ITEM_B_HEART,
                            Math_SinS(thisv->actor.shape.rot.y) * -50.0f + thisv->actor.world.pos.x,
                            thisv->actor.world.pos.y,
                            Math_CosS(thisv->actor.shape.rot.y) * -50.0f + thisv->actor.world.pos.z, 0, 0, 0, 0);
            }
            if (thisv->unk_1DA == 600) {
                camera = Gameplay_GetCamera(globalCtx, MAIN_CAM);
                camera->eye = thisv->cameraEye;
                camera->eyeNext = thisv->cameraEye;
                camera->at = thisv->cameraAt;
                func_800C08AC(globalCtx, thisv->cutsceneCamera, 0);
                thisv->unk_1BC = 0;
                thisv->cutsceneCamera = MAIN_CAM;
                thisv->csState = 100;
                Gameplay_ChangeCameraStatus(globalCtx, MAIN_CAM, CAM_STAT_ACTIVE);
                func_80064534(globalCtx, &globalCtx->csCtx);
                func_8002DF54(globalCtx, &thisv->actor, 7);
                Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_DOOR_WARP1, -890.0f, -1523.76f,
                                   -3304.0f, 0, 0, 0, WARP_DUNGEON_CHILD);
                thisv->skelAnime.playSpeed = 0.0f;
                Flags_SetClear(globalCtx, globalCtx->roomCtx.curRoom.num);
            }
        case 100:
            if ((thisv->unk_1DA < 0x2C6) && (Rand_ZeroOne() < 0.5f)) {
                Vec3f sp68;
                Color_RGBA8 D_808CA568 = { 0, 0, 0, 100 };
                Color_RGBA8 D_808CA56C = { 0, 0, 0, 0 };

                sp68.x = Rand_CenteredFloat(60.0f) + thisv->actor.focus.pos.x;
                sp68.y = (Rand_ZeroOne() * 50.0f) + -1498.76f;
                sp68.z = Rand_CenteredFloat(60.0f) + thisv->actor.focus.pos.z;
                EffectSsGMagma2_Spawn(globalCtx, &sp68, &D_808CA568, &D_808CA56C, 5, 1,
                                      (s16)(Rand_ZeroOne() * 50.0f) + 50);
            }
            break;
    }
    if (thisv->cutsceneCamera != MAIN_CAM) {
        Gameplay_CameraSetAtEye(globalCtx, thisv->cutsceneCamera, &thisv->cameraAt, &thisv->cameraEye);
    }
}

void BossDodongo_UpdateEffects(GlobalContext* globalCtx) {
    BossDodongoEffect* eff = (BossDodongoEffect*)globalCtx->specialEffects;
    Color_RGB8 effectColors[] = { { 255, 128, 0 }, { 255, 0, 0 }, { 255, 255, 0 }, { 255, 0, 0 } };
    s16 colorIndex;
    s16 i;

    for (i = 0; i < 80; i++, eff++) {
        if (eff->unk_24 != 0) {
            eff->unk_00.x += eff->unk_0C.x;
            eff->unk_00.y += eff->unk_0C.y;
            eff->unk_00.z += eff->unk_0C.z;
            eff->unk_25++;
            eff->unk_0C.x += eff->unk_18.x;
            eff->unk_0C.y += eff->unk_18.y;
            eff->unk_0C.z += eff->unk_18.z;
            if (eff->unk_24 == 1) {
                colorIndex = eff->unk_25 % 4;
                eff->color.r = effectColors[colorIndex].r;
                eff->color.g = effectColors[colorIndex].g;
                eff->color.b = effectColors[colorIndex].b;
                eff->alpha -= 20;
                if (eff->alpha <= 0) {
                    eff->alpha = 0;
                    eff->unk_24 = 0;
                }
            }
        }
    }
}

void BossDodongo_DrawEffects(GlobalContext* globalCtx) {
    MtxF* unkMtx;
    s16 i;
    u8 phi_s3 = 0;
    BossDodongoEffect* eff;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    eff = (BossDodongoEffect*)globalCtx->specialEffects;

    OPEN_DISPS(gfxCtx, "../z_boss_dodongo.c", 5228);

    func_80093D84(globalCtx->state.gfxCtx);
    unkMtx = &globalCtx->billboardMtxF;

    gSPInvalidateTexCache(POLY_XLU_DISP++, 0);

    for (i = 0; i < 80; i++, eff++) {
        if (eff->unk_24 == 1) {
            gDPPipeSync(POLY_XLU_DISP++);

            if (phi_s3 == 0) {
                gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_009D50);
                phi_s3++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b, eff->alpha);
            Matrix_Translate(eff->unk_00.x, eff->unk_00.y, eff->unk_00.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(unkMtx);
            Matrix_Scale(eff->unk_2C, eff->unk_2C, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_boss_dodongo.c", 5253),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_009DD0);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_dodongo.c", 5258);
}
