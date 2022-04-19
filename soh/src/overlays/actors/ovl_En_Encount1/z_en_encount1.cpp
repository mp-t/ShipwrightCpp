#include "z_en_encount1.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Tite/z_en_tite.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_27)

void EnEncount1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnEncount1_Update(Actor* thisx, GlobalContext* globalCtx);

void EnEncount1_SpawnLeevers(EnEncount1* thisv, GlobalContext* globalCtx);
void EnEncount1_SpawnTektites(EnEncount1* thisv, GlobalContext* globalCtx);
void EnEncount1_SpawnStalchildOrWolfos(EnEncount1* thisv, GlobalContext* globalCtx);

static s16 sLeeverAngles[] = { 0x0000, 0x2710, 0x7148, 0x8EB8, 0xD8F0 };
static f32 sLeeverDists[] = { 200.0f, 170.0f, 120.0f, 120.0f, 170.0f };

ActorInit En_Encount1_InitVars = {
    ACTOR_EN_ENCOUNT1,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnEncount1),
    (ActorFunc)EnEncount1_Init,
    NULL,
    (ActorFunc)EnEncount1_Update,
    NULL,
    NULL,
};

void EnEncount1_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnEncount1* thisv = (EnEncount1*)thisx;
    f32 spawnRange;

    if (thisv->actor.params <= 0) {
        osSyncPrintf("\n\n");
        // "Input error death!"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 入力エラーデッス！ ☆☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 入力エラーデッス！ ☆☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 入力エラーデッス！ ☆☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 入力エラーデッス！ ☆☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 入力エラーデッス！ ☆☆☆☆☆ \n" VT_RST);
        osSyncPrintf("\n\n");
        Actor_Kill(&thisv->actor);
        return;
    }

    thisv->spawnType = (thisv->actor.params >> 0xB) & 0x1F;
    thisv->maxCurSpawns = (thisv->actor.params >> 6) & 0x1F;
    thisv->maxTotalSpawns = thisv->actor.params & 0x3F;
    thisv->curNumSpawn = thisv->totalNumSpawn = 0;
    spawnRange = 120.0f + (40.0f * thisv->actor.world.rot.z);
    thisv->spawnRange = spawnRange;

    osSyncPrintf("\n\n");
    // "It's an enemy spawner!"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 敵発生ゾーンでた！ ☆☆☆☆☆ %x\n" VT_RST, thisv->actor.params);
    // "Type"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 種類\t\t   ☆☆☆☆☆ %d\n" VT_RST, thisv->spawnType);
    // "Maximum number of simultaneous spawns"
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 最大同時発生数     ☆☆☆☆☆ %d\n" VT_RST, thisv->maxCurSpawns);
    // "Maximum number of spawns"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 最大発生数  \t   ☆☆☆☆☆ %d\n" VT_RST, thisv->maxTotalSpawns);
    // "Spawn check range"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生チェック範囲   ☆☆☆☆☆ %f\n" VT_RST, thisv->spawnRange);
    osSyncPrintf("\n\n");

    thisv->actor.flags &= ~ACTOR_FLAG_0;
    switch (thisv->spawnType) {
        case SPAWNER_LEEVER:
            thisv->timer = 30;
            thisv->maxCurSpawns = 5;
            if (globalCtx->sceneNum == SCENE_SPOT13) { // Haunted Wasteland
                thisv->reduceLeevers = true;
                thisv->maxCurSpawns = 3;
            }
            thisv->updateFunc = EnEncount1_SpawnLeevers;
            break;
        case SPAWNER_TEKTITE:
            thisv->maxCurSpawns = 2;
            thisv->updateFunc = EnEncount1_SpawnTektites;
            break;
        case SPAWNER_STALCHILDREN:
        case SPAWNER_WOLFOS:
            if (globalCtx->sceneNum == SCENE_SPOT00) { // Hyrule Field
                thisv->maxTotalSpawns = 10000;
            }
            thisv->updateFunc = EnEncount1_SpawnStalchildOrWolfos;
            break;
    }
}

void EnEncount1_SpawnLeevers(EnEncount1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 floorType;
    f32 spawnDist;
    s32 spawnParams;
    s16 spawnLimit;
    s16 spawnAngle;
    Vec3f spawnPos;
    CollisionPoly* floorPoly;
    s32 bgId;
    f32 floorY;
    EnReeba* leever;

    thisv->outOfRangeTimer = 0;
    spawnPos = thisv->actor.world.pos;

    if ((thisv->timer == 0) && (globalCtx->csCtx.state == CS_STATE_IDLE) && (thisv->curNumSpawn <= thisv->maxCurSpawns) &&
        (thisv->curNumSpawn < 5)) {
        floorType = func_80041D4C(&globalCtx->colCtx, player->actor.floorPoly, player->actor.floorBgId);
        if ((floorType != 4) && (floorType != 7) && (floorType != 12)) {
            thisv->numLeeverSpawns = 0;
        } else if (!(thisv->reduceLeevers && (thisv->actor.xzDistToPlayer > 1300.0f))) {
            spawnLimit = 5;
            if (thisv->reduceLeevers) {
                spawnLimit = 3;
            }
            while ((thisv->curNumSpawn < thisv->maxCurSpawns) && (thisv->curNumSpawn < spawnLimit) && (thisv->timer == 0)) {
                spawnDist = sLeeverDists[thisv->leeverIndex];
                spawnAngle = sLeeverAngles[thisv->leeverIndex] + player->actor.shape.rot.y;
                spawnParams = LEEVER_SMALL;

                if ((thisv->killCount >= 10) && (thisv->bigLeever == NULL)) {
                    thisv->killCount = thisv->numLeeverSpawns = 0;
                    spawnAngle = sLeeverAngles[0];
                    spawnDist = sLeeverDists[2];
                    spawnParams = LEEVER_BIG;
                }

                spawnPos.x = player->actor.world.pos.x + Math_SinS(spawnAngle) * spawnDist;
                spawnPos.y = player->actor.floorHeight + 120.0f;
                spawnPos.z = player->actor.world.pos.z + Math_CosS(spawnAngle) * spawnDist;

                floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &floorPoly, &bgId, &thisv->actor, &spawnPos);
                if (floorY <= BGCHECK_Y_MIN) {
                    break;
                }
                spawnPos.y = floorY;

                leever = (EnReeba*)Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_REEBA,
                                                      spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0, spawnParams);

                if (1) {}
                if (1) {}
                if (leever != NULL) {
                    thisv->curNumSpawn++;
                    leever->unk_280 = thisv->leeverIndex++;
                    if (thisv->leeverIndex >= 5) {
                        thisv->leeverIndex = 0;
                    }
                    thisv->numLeeverSpawns++;
                    if (thisv->numLeeverSpawns >= 12) {
                        thisv->timer = 150;
                        thisv->numLeeverSpawns = 0;
                    }
                    if (spawnParams != LEEVER_SMALL) {
                        thisv->timer = 300;
                        thisv->bigLeever = leever;
                    }
                    if (!thisv->reduceLeevers) {
                        thisv->maxCurSpawns = (s16)Rand_ZeroFloat(3.99f) + 2;
                    } else {
                        thisv->maxCurSpawns = (s16)Rand_ZeroFloat(2.99f) + 1;
                    }
                } else {
                    // "Cannot spawn!"
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                    break;
                }
            }
        }
    }
}

void EnEncount1_SpawnTektites(EnEncount1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 bgId;
    CollisionPoly* floorPoly;
    Vec3f spawnPos;
    f32 floorY;

    if (thisv->timer == 0) {
        thisv->timer = 10;
        if ((fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) > 100.0f) ||
            (thisv->actor.xzDistToPlayer > thisv->spawnRange)) {
            thisv->outOfRangeTimer++;
        } else {
            thisv->outOfRangeTimer = 0;
            if ((thisv->curNumSpawn < thisv->maxCurSpawns) && (thisv->totalNumSpawn < thisv->maxTotalSpawns)) {
                spawnPos.x = thisv->actor.world.pos.x + Rand_CenteredFloat(50.0f);
                spawnPos.y = thisv->actor.world.pos.y + 120.0f;
                spawnPos.z = thisv->actor.world.pos.z + Rand_CenteredFloat(50.0f);
                floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &floorPoly, &bgId, &thisv->actor, &spawnPos);
                if (floorY <= BGCHECK_Y_MIN) {
                    return;
                }
                spawnPos.y = floorY;
                if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, ACTOR_EN_TITE, spawnPos.x,
                                       spawnPos.y, spawnPos.z, 0, 0, 0, TEKTITE_RED) != NULL) { // Red tektite
                    thisv->curNumSpawn++;
                    thisv->totalNumSpawn++;
                } else {
                    // "Cannot spawn!"
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                }
            }
        }
    }
}

void EnEncount1_SpawnStalchildOrWolfos(EnEncount1* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 spawnDist;
    s16 spawnAngle;
    s16 spawnId;
    s16 spawnParams;
    s16 kcOver10;
    s16 tempmod;
    Vec3f spawnPos;
    CollisionPoly* floorPoly;
    s32 bgId;
    f32 floorY;

    if (globalCtx->sceneNum != SCENE_SPOT00) {
        if ((fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) > 100.0f) ||
            (thisv->actor.xzDistToPlayer > thisv->spawnRange)) {
            thisv->outOfRangeTimer++;
            return;
        }
    } else if (IS_DAY || (Player_GetMask(globalCtx) == PLAYER_MASK_BUNNY)) {
        thisv->killCount = 0;
        return;
    }

    thisv->outOfRangeTimer = 0;
    spawnPos = thisv->actor.world.pos;
    if ((thisv->curNumSpawn < thisv->maxCurSpawns) && (thisv->totalNumSpawn < thisv->maxTotalSpawns)) {
        while ((thisv->curNumSpawn < thisv->maxCurSpawns) && (thisv->totalNumSpawn < thisv->maxTotalSpawns)) {
            if (globalCtx->sceneNum == SCENE_SPOT00) {
                if ((player->unk_89E == 0) || (player->actor.floorBgId != BGCHECK_SCENE) ||
                    !(player->actor.bgCheckFlags & 1) || (player->stateFlags1 & 0x08000000)) {

                    thisv->fieldSpawnTimer = 60;
                    break;
                }
                if (thisv->fieldSpawnTimer == 60) {
                    thisv->maxCurSpawns = 2;
                }
                if (thisv->fieldSpawnTimer != 0) {
                    thisv->fieldSpawnTimer--;
                    break;
                }

                spawnDist = Rand_CenteredFloat(40.0f) + 200.0f;
                spawnAngle = player->actor.shape.rot.y;
                if (thisv->curNumSpawn != 0) {
                    spawnAngle = -spawnAngle;
                    spawnDist = Rand_CenteredFloat(40.0f) + 100.0f;
                }
                spawnPos.x =
                    player->actor.world.pos.x + (Math_SinS(spawnAngle) * spawnDist) + Rand_CenteredFloat(40.0f);
                spawnPos.y = player->actor.floorHeight + 120.0f;
                spawnPos.z =
                    player->actor.world.pos.z + (Math_CosS(spawnAngle) * spawnDist) + Rand_CenteredFloat(40.0f);
                floorY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &floorPoly, &bgId, &thisv->actor, &spawnPos);
                if (floorY <= BGCHECK_Y_MIN) {
                    break;
                }
                if ((player->actor.yDistToWater != BGCHECK_Y_MIN) &&
                    (floorY < (player->actor.world.pos.y - player->actor.yDistToWater))) {
                    break;
                }
                spawnPos.y = floorY;
            }
            if (thisv->spawnType == SPAWNER_WOLFOS) {
                spawnId = ACTOR_EN_WF;
                spawnParams = (0xFF << 8) | 0x00;
            } else {
                spawnId = ACTOR_EN_SKB;
                spawnParams = 0;

                kcOver10 = thisv->killCount / 10;
                if (kcOver10 > 0) {
                    tempmod = thisv->killCount % 10;
                    if (tempmod == 0) {
                        spawnParams = kcOver10 * 5;
                    }
                }
                thisv->killCount++;
            }
            if (Actor_SpawnAsChild(&globalCtx->actorCtx, &thisv->actor, globalCtx, spawnId, spawnPos.x, spawnPos.y,
                                   spawnPos.z, 0, 0, 0, spawnParams) != NULL) {
                thisv->curNumSpawn++;
                if (thisv->curNumSpawn >= thisv->maxCurSpawns) {
                    thisv->fieldSpawnTimer = 100;
                }
                if (globalCtx->sceneNum != SCENE_SPOT00) {
                    thisv->totalNumSpawn++;
                }
            } else {
                // "Cannot spawn!"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生できません！ ☆☆☆☆☆\n" VT_RST);
                break;
            }
        }
    }
}

void EnEncount1_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnEncount1* thisv = (EnEncount1*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->updateFunc(thisv, globalCtx);

    if (BREG(0) != 0) {
        if (thisv->outOfRangeTimer != 0) {
            if ((thisv->outOfRangeTimer & 1) == 0) {
                DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 120, 255, 4, globalCtx->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                   thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 0, 255, 255, 4, globalCtx->state.gfxCtx);
        }
    }
}
