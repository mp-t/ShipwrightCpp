/*
 * File: z_en_wall_tubo.c
 * Overlay: ovl_En_Wall_Tubo
 * Description: Bombchu Bowling Alley Bullseyes/Pits
 */

#include "z_en_wall_tubo.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
#include "overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS ACTOR_FLAG_4

void EnWallTubo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWallTubo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWallTubo_Update(Actor* thisx, GlobalContext* globalCtx);

void EnWallTubo_FindGirl(EnWallTubo* thisv, GlobalContext* globalCtx);
void EnWallTubo_DetectChu(EnWallTubo* thisv, GlobalContext* globalCtx);
void EnWallTubo_SetWallFall(EnWallTubo* thisv, GlobalContext* globalCtx);

const ActorInit En_Wall_Tubo_InitVars = {
    ACTOR_EN_WALL_TUBO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWallTubo),
    (ActorFunc)EnWallTubo_Init,
    (ActorFunc)EnWallTubo_Destroy,
    (ActorFunc)EnWallTubo_Update,
    NULL,
    NULL,
};

void EnWallTubo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnWallTubo* thisv = (EnWallTubo*)thisx;

    osSyncPrintf("\n\n");
    // "Wall Target"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 壁のツボ ☆☆☆☆☆ \n" VT_RST);
    thisv->unk_164 = thisv->actor.world.pos;
    thisv->actionFunc = EnWallTubo_FindGirl;
}

void EnWallTubo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnWallTubo_FindGirl(EnWallTubo* thisv, GlobalContext* globalCtx) {
    Actor* lookForGirl;

    lookForGirl = globalCtx->actorCtx.actorLists[ACTORCAT_NPC].head;

    while (lookForGirl != NULL) {
        if (lookForGirl->id != ACTOR_EN_BOM_BOWL_MAN) {
            lookForGirl = lookForGirl->next;
        } else {
            thisv->chuGirl = (EnBomBowlMan*)lookForGirl;
            break;
        }
    }

    thisv->actionFunc = EnWallTubo_DetectChu;
}

void EnWallTubo_DetectChu(EnWallTubo* thisv, GlobalContext* globalCtx) {
    EnBomChu* chu;
    s32 pad;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f chuPosDiff;
    s16 quakeIndex;

    if (thisv->chuGirl->minigamePlayStatus != 0) {
        if (globalCtx->cameraPtrs[MAIN_CAM]->setting == CAM_SET_CHU_BOWLING) {
            chu = (EnBomChu*)globalCtx->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

            while (chu != NULL) {
                if ((&chu->actor == &thisv->actor) || (chu->actor.id != ACTOR_EN_BOM_CHU)) {
                    chu = (EnBomChu*)chu->actor.next;
                    continue;
                }

                chuPosDiff.x = chu->actor.world.pos.x - thisv->actor.world.pos.x;
                chuPosDiff.y = chu->actor.world.pos.y - thisv->actor.world.pos.y;
                chuPosDiff.z = chu->actor.world.pos.z - thisv->actor.world.pos.z;

                if (((fabsf(chuPosDiff.x) < 40.0f) || (BREG(2))) && ((fabsf(chuPosDiff.y) < 40.0f) || (BREG(2))) &&
                    (fabsf(chuPosDiff.z) < 40.0f || (BREG(2)))) {
                    thisv->chuGirl->wallStatus[thisv->actor.params] = 1;
                    chu->timer = 2;
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                    thisv->timer = 60;
                    EffectSsBomb2_SpawnLayered(globalCtx, &thisv->explosionCenter, &effVelocity, &effAccel, 200, 40);
                    quakeIndex = Quake_Add(GET_ACTIVE_CAM(globalCtx), 1);
                    Quake_SetSpeed(quakeIndex, 0x7FFF);
                    Quake_SetQuakeValues(quakeIndex, 100, 0, 0, 0);
                    Quake_SetCountdown(quakeIndex, 100);
                    thisv->actionFunc = EnWallTubo_SetWallFall;
                    break;
                }

                chu = (EnBomChu*)chu->actor.next;
            }
        }
    }
}

void EnWallTubo_SetWallFall(EnWallTubo* thisv, GlobalContext* globalCtx) {
    BgBowlWall* wall;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;

    if ((globalCtx->gameplayFrames & 1) == 0) {
        effPos.x = thisv->explosionCenter.x + Rand_CenteredFloat(300.0f);
        effPos.y = thisv->explosionCenter.y + Rand_CenteredFloat(300.0f);
        effPos.z = thisv->explosionCenter.z;
        EffectSsBomb2_SpawnLayered(globalCtx, &effPos, &effVelocity, &effAccel, 100, 30);
        EffectSsHahen_SpawnBurst(globalCtx, &effPos, 10.0f, 0, 50, 15, 3, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (thisv->timer == 0) {
        wall = (BgBowlWall*)thisv->actor.parent;

        if ((wall != NULL) && (wall->dyna.actor.update != NULL)) {
            wall->isHit = true;
            // "You did it field!" (repeated 5 times)
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
        }

        Actor_Kill(&thisv->actor);
    }
}

void EnWallTubo_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnWallTubo* thisv = (EnWallTubo*)thisx;

    if (thisv->timer != 0) {
        thisv->timer--;
    }

    thisv->actionFunc(thisv, globalCtx);

    if (BREG(0)) {
        DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                               thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0, 0, 255, 255, 4, globalCtx->state.gfxCtx);
    }
}
