/*
 * File: z_object_kankyo.c
 * Overlay: ovl_Object_Kankyo
 * Description: Environmental Effects
 */

#include "z_object_kankyo.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_spot02_objects/object_spot02_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

void ObjectKankyo_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjectKankyo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjectKankyo_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjectKankyo_Draw(Actor* thisx, GlobalContext* globalCtx);
void ObjectKankyo_Reset(void);

void ObjectKankyo_SetupAction(ObjectKankyo* thisv, ObjectKankyoActionFunc func);
void ObjectKankyo_Fairies(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_SunGraveSparkInit(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_Snow(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_Lightning(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_InitBeams(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_WaitForSunGraveSparkObject(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_SunGraveSpark(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_WaitForBeamObject(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_Beams(ObjectKankyo* thisv, GlobalContext* globalCtx);

void ObjectKankyo_DrawFairies(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_DrawSnow(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_DrawLightning(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_DrawSunGraveSpark(ObjectKankyo* thisv, GlobalContext* globalCtx);
void ObjectKankyo_DrawBeams(ObjectKankyo* thisv, GlobalContext* globalCtx);

static const void* sEffLightningTextures[] = {
    gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
    gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
};

static const void* D_80BA5900[] = {
    gEffSunGraveSpark1Tex, gEffSunGraveSpark2Tex, gEffSunGraveSpark3Tex, gEffSunGraveSpark4Tex,
    gEffSunGraveSpark5Tex, gEffSunGraveSpark6Tex, gEffSunGraveSpark7Tex, gEffSunGraveSpark8Tex,
};

ActorInit Object_Kankyo_InitVars = {
    ACTOR_OBJECT_KANKYO,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjectKankyo),
    (ActorFunc)ObjectKankyo_Init,
    (ActorFunc)ObjectKankyo_Destroy,
    (ActorFunc)ObjectKankyo_Update,
    (ActorFunc)ObjectKankyo_Draw,
    (ActorResetFunc)ObjectKankyo_Reset,
};

static u8 sIsSpawned = false;
static s16 sTrailingFairies = 0;

void ObjectKankyo_SetupAction(ObjectKankyo* thisv, ObjectKankyoActionFunc action) {
    thisv->actionFunc = action;
}

void ObjectKankyo_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjectKankyo* thisv = (ObjectKankyo*)thisx;
    s32 pad;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(thisv->effects); i++) {
        thisv->effects[i].state = 0;
    }

    thisv->actor.room = -1;
    switch (thisv->actor.params) {
        case 0:
            if (!sIsSpawned) {
                ObjectKankyo_SetupAction(thisv, ObjectKankyo_Fairies);
                sIsSpawned = true;
            } else {
                Actor_Kill(&thisv->actor);
            }
            break;

        case 3:
            if (!sIsSpawned) {
                ObjectKankyo_SetupAction(thisv, ObjectKankyo_Snow);
                sIsSpawned = true;
            } else {
                Actor_Kill(&thisv->actor);
            }
            break;

        case 2:
            ObjectKankyo_SetupAction(thisv, ObjectKankyo_Lightning);
            break;

        case 4:
            thisv->effects[0].alpha = 0;
            thisv->effects[0].amplitude = 0.0f;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ITEMACTION);
            thisv->requiredObjectLoaded = false;
            ObjectKankyo_SetupAction(thisv, ObjectKankyo_SunGraveSparkInit);
            break;

        case 5:
            thisv->effects[0].alpha = 0;
            thisv->effects[0].amplitude = 0.0f;

            for (i = 0; i < 6; i++) {
                thisv->effects[i].size = 0.1f;
            }

            // Check which beams are disabled
            if (Flags_GetEventChkInf(0xBB)) {
                thisv->effects[0].size = 0.0f;
            }
            if (Flags_GetEventChkInf(0xBC)) {
                thisv->effects[1].size = 0.0f;
            }
            if (Flags_GetEventChkInf(0xBD)) {
                thisv->effects[2].size = 0.0f;
            }
            if (Flags_GetEventChkInf(0xBE)) {
                thisv->effects[3].size = 0.0f;
            }
            if (Flags_GetEventChkInf(0xBF)) {
                thisv->effects[4].size = 0.0f;
            }
            if (Flags_GetEventChkInf(0xAD)) {
                thisv->effects[5].size = 0.0f;
            }

            if (gSaveContext.cutsceneTrigger != 0) {
                if (gSaveContext.entranceIndex == 0x0538) {
                    thisv->effects[0].size = 0.1f;
                }
                if (gSaveContext.entranceIndex == 0x053C) {
                    thisv->effects[1].size = 0.1f;
                }
                if (gSaveContext.entranceIndex == 0x0540) {
                    thisv->effects[2].size = 0.1f;
                }
                if (gSaveContext.entranceIndex == 0x0544) {
                    thisv->effects[3].size = 0.1f;
                }
                if (gSaveContext.entranceIndex == 0x0548) {
                    thisv->effects[4].size = 0.1f;
                }
                if (gSaveContext.entranceIndex == 0x054C) {
                    thisv->effects[5].size = 0.1f;
                }
            }

            thisv->requiredObjectLoaded = false;
            ObjectKankyo_SetupAction(thisv, ObjectKankyo_InitBeams);
            break;
    }
}

void ObjectKankyo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    Actor_Kill(thisx);
}

void ObjectKankyo_Snow(ObjectKankyo* thisv, GlobalContext* globalCtx) {
}

void ObjectKankyo_Fairies(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    static Vec3f sSoundPos = { 0.0f, 0.0f, 0.0f };
    Player* player;
    f32 dist;
    s32 playerMoved;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 viewForwardsX;
    f32 viewForwardsY;
    f32 viewForwardsZ;
    f32 maxDist;
    f32 baseX;
    f32 baseY;
    f32 baseZ;
    Vec3f vec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f vec2 = { 0.0f, 0.0f, 0.0f };
    f32 random;
    s16 i;
    Vec3f viewForwards;

    player = GET_PLAYER(globalCtx);

    if (globalCtx->sceneNum == SCENE_SPOT04 && gSaveContext.sceneSetupIndex == 7) {
        dist = Math3D_Vec3f_DistXYZ(&thisv->prevEyePos, &globalCtx->view.eye);

        thisv->prevEyePos.x = globalCtx->view.eye.x;
        thisv->prevEyePos.y = globalCtx->view.eye.y;
        thisv->prevEyePos.z = globalCtx->view.eye.z;

        dist /= 30.0f;
        if (dist > 1.0f) {
            dist = 1.0f;
        }

        func_800F436C(&sSoundPos, NA_SE_EV_NAVY_FLY - SFX_FLAG, (0.4f * dist) + 0.6f);
        switch (globalCtx->csCtx.frames) {
            case 473:
                func_800788CC(NA_SE_VO_NA_HELLO_3);
                break;

            case 583:
                func_800F4524(&D_801333D4, NA_SE_VO_NA_HELLO_2, 32);
                break;

            case 763:
                func_80078884(NA_SE_EV_NAVY_CRASH - SFX_FLAG);
                break;

            case 771:
                func_80078884(NA_SE_VO_RT_THROW);
                break;
        }
    }

    if (globalCtx->envCtx.unk_EE[3] < 64 &&
        (gSaveContext.entranceIndex != 0x00EE || gSaveContext.sceneSetupIndex != 4 || globalCtx->envCtx.unk_EE[3])) {
        globalCtx->envCtx.unk_EE[3] += 16;
    }

    for (i = 0; i < globalCtx->envCtx.unk_EE[3]; i++) {
        // spawn in front of the camera
        dx = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
        dy = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
        dz = globalCtx->view.lookAt.z - globalCtx->view.eye.z;
        dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

        viewForwards.x = dx / dist;
        viewForwards.y = dy / dist;
        viewForwards.z = dz / dist;

        viewForwardsX = viewForwards.x;
        viewForwardsY = viewForwards.y;
        viewForwardsZ = viewForwards.z;

        switch (thisv->effects[i].state) {
            case 0: // init
                thisv->effects[i].base.x = globalCtx->view.eye.x + viewForwardsX * 80.0f;
                thisv->effects[i].base.y = globalCtx->view.eye.y + viewForwardsY * 80.0f;
                thisv->effects[i].base.z = globalCtx->view.eye.z + viewForwardsZ * 80.0f;

                thisv->effects[i].pos.x = (Rand_ZeroOne() - 0.5f) * 160.0f;
                thisv->effects[i].pos.y = 30.0f;
                thisv->effects[i].pos.z = (Rand_ZeroOne() - 0.5f) * 160.0f;
                thisv->effects[i].targetSpeed = Rand_ZeroOne() * 1.6f + 0.5f;
                thisv->effects[i].alpha = 0;
                thisv->effects[i].alphaTimer = Rand_ZeroOne() * 65535.0f;
                thisv->effects[i].size = 0.1f;
                thisv->effects[i].dirPhase.x = Rand_ZeroOne() * 360.0f;
                thisv->effects[i].dirPhase.y = Rand_ZeroOne() * 360.0f;
                thisv->effects[i].dirPhase.z = Rand_ZeroOne() * 360.0f;
                thisv->effects[i].state++;
                thisv->effects[i].timer = 0;
                break;

            case 1: // blinking fairies / inactive fairy trails
            case 2: // fairy trails
                thisv->effects[i].alphaTimer++;
                baseX = globalCtx->view.eye.x + viewForwards.x * 80.0f;
                baseY = globalCtx->view.eye.y + viewForwards.y * 80.0f;
                baseZ = globalCtx->view.eye.z + viewForwards.z * 80.0f;

                thisv->effects[i].prevPos.x = thisv->effects[i].pos.x;
                thisv->effects[i].prevPos.y = thisv->effects[i].pos.y;
                thisv->effects[i].prevPos.z = thisv->effects[i].pos.z;

                playerMoved = true;
                // y velocity is set to -4 when the player is on the ground
                if (player->actor.velocity.x + player->actor.velocity.y + player->actor.velocity.z == -4.0f) {
                    playerMoved = false;
                    thisv->effects[i].timer++;
                } else {
                    thisv->effects[i].timer = 0;
                }

                if (thisv->effects[i].state == 1) {
                    // the first 32 fairies are invisible until the player stands still
                    if (i < 32 && !playerMoved && thisv->effects[i].timer > 256) {
                        thisv->effects[i].timer = 0;
                        if (Rand_ZeroOne() < 0.5f) {
                            thisv->effects[i].angleVel = (s16)(Rand_ZeroOne() * 200.0f) + 200;
                        } else {
                            thisv->effects[i].angleVel = -((s16)(Rand_ZeroOne() * 200.0f) + 200);
                        }

                        thisv->effects[i].flightRadius = (s16)(Rand_ZeroOne() * 50.0f) + 15;

                        // uniformly scales the length and height of the wave that the lead fairy flies in
                        // lower numbers have a larger amplitude and period
                        thisv->effects[i].amplitude = (Rand_ZeroOne() * 10.0f + 10.0f) * 0.01f;

                        random = Rand_ZeroOne();
                        if (random < 0.2f) {
                            sTrailingFairies = 1;
                        } else if (random < 0.2f) {
                            // unreachable
                            sTrailingFairies = 3;
                        } else if (random < 0.4f) {
                            sTrailingFairies = 7;
                        } else {
                            sTrailingFairies = 15;
                        }

                        if ((i & sTrailingFairies) == 0) {
                            thisv->effects[i].pos.y = 0.0f;
                        }

                        thisv->effects[i].state = 2;
                        thisv->effects[i].targetSpeed = 0.0f;
                    }

                    Math_SmoothStepToF(&thisv->effects[i].size, 0.1f, 0.10f, 0.001f, 0.00001f);
                    Math_SmoothStepToF(&thisv->effects[i].speed, thisv->effects[i].targetSpeed, 0.5f, 0.2f, 0.02f);

                    thisv->effects[i].pos.x += sinf(thisv->effects[i].dirPhase.x) * thisv->effects[i].speed;
                    thisv->effects[i].pos.y += sinf(thisv->effects[i].dirPhase.y) * thisv->effects[i].speed;
                    thisv->effects[i].pos.z += sinf(thisv->effects[i].dirPhase.z) * thisv->effects[i].speed;

                    switch ((i >> 1) & 3) {
                        case 0:
                            thisv->effects[i].dirPhase.x += 0.008f;
                            thisv->effects[i].dirPhase.y += 0.05f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.z += 0.015f;
                            break;

                        case 1:
                            thisv->effects[i].dirPhase.x += 0.01f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.y += 0.05f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.z += 0.005f * Rand_ZeroOne();
                            break;

                        case 2:
                            thisv->effects[i].dirPhase.x += 0.01f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.y += 0.4f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.z += 0.004f * Rand_ZeroOne();
                            break;

                        case 3:
                            thisv->effects[i].dirPhase.x += 0.01 * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.y += 0.08f * Rand_ZeroOne();
                            thisv->effects[i].dirPhase.z += 0.05f * Rand_ZeroOne();
                            break;
                    }
                } else if (thisv->effects[i].state == 2) {
                    // scatter when the player moves or after a long enough time
                    if (playerMoved || thisv->effects[i].timer > 1280) {
                        thisv->effects[i].timer = 0;
                        thisv->effects[i].state = 1;
                        thisv->effects[i].speed = 1.5f;
                        thisv->effects[i].targetSpeed = Rand_ZeroOne() * 1.6f + 0.5f;
                    }

                    if ((i & sTrailingFairies) == 0) { // leader fairy
                        Math_SmoothStepToF(&thisv->effects[i].size, 0.25f, 0.1f, 0.001f, 0.00001f);

                        // move the center of the flight path to player's position
                        Math_SmoothStepToF(&thisv->effects[i].base.x, player->actor.world.pos.x, 0.5f, 1.0f, 0.2f);
                        Math_SmoothStepToF(&thisv->effects[i].base.y, player->actor.world.pos.y + 50.0f, 0.5f, 1.0f,
                                           0.2f);
                        Math_SmoothStepToF(&thisv->effects[i].base.z, player->actor.world.pos.z, 0.5f, 1.0f, 0.2f);

                        // results unused
                        Math_SmoothStepToF(&thisv->effects[i].pos.x,
                                           Math_SinS(thisv->effects[i].angle - 0x8000) * thisv->effects[i].flightRadius,
                                           0.5f, 2.0f, 0.2f);
                        Math_SmoothStepToF(&thisv->effects[i].pos.z,
                                           Math_CosS(thisv->effects[i].angle - 0x8000) * thisv->effects[i].flightRadius,
                                           0.5f, 2.0f, 0.2f);

                        // the lead fairy's y position approximately follows a sine wave with `amplitude` as angular
                        // frequency and `1 / amplitude` as amplitude
                        thisv->effects[i].angle += thisv->effects[i].angleVel;
                        thisv->effects[i].pos.y += sinf(thisv->effects[i].dirPhase.y);

                        thisv->effects[i].dirPhase.x += 0.2f * Rand_ZeroOne();
                        thisv->effects[i].dirPhase.y += thisv->effects[i].amplitude;
                        thisv->effects[i].dirPhase.z += 0.1f * Rand_ZeroOne();

                        // circle around the player
                        thisv->effects[i].pos.x =
                            Math_SinS(thisv->effects[i].angle - 0x8000) * thisv->effects[i].flightRadius;
                        thisv->effects[i].pos.z =
                            Math_CosS(thisv->effects[i].angle - 0x8000) * thisv->effects[i].flightRadius;
                    } else { // trailing fairy
                        Math_SmoothStepToF(&thisv->effects[i].size, 0.1f, 0.10f, 0.001f, 0.00001f);
                        Math_SmoothStepToF(&thisv->effects[i].speed, 1.5f, 0.5f, 0.1f, 0.0002f);

                        // follow previous fairy, translate their position to be relative to our home
                        thisv->effects[i].pos.x =
                            thisv->effects[i - 1].prevPos.x + (thisv->effects[i - 1].base.x - thisv->effects[i].base.x);
                        thisv->effects[i].pos.y =
                            thisv->effects[i - 1].prevPos.y + (thisv->effects[i - 1].base.y - thisv->effects[i].base.y);
                        thisv->effects[i].pos.z =
                            thisv->effects[i - 1].prevPos.z + (thisv->effects[i - 1].base.z - thisv->effects[i].base.z);
                    }
                }

                if (thisv->effects[i].state != 2) {
                    maxDist = 130.0f;
                    if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX > maxDist ||
                        thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX < -maxDist ||
                        thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY > maxDist ||
                        thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY < -maxDist ||
                        thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ > maxDist ||
                        thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ < -maxDist) {

                        // when a fairy moves off screen, wrap around to the other side
                        if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX > maxDist) {
                            thisv->effects[i].base.x = baseX - maxDist;
                            thisv->effects[i].pos.x = 0.0f;
                        }
                        if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX < -maxDist) {
                            thisv->effects[i].base.x = baseX + maxDist;
                            thisv->effects[i].pos.x = 0.0f;
                        }
                        if (thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY > 50.0f) {
                            thisv->effects[i].base.y = baseY - 50.0f;
                            thisv->effects[i].pos.y = 0.0f;
                        }
                        if (thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY < -50.0f) {
                            thisv->effects[i].base.y = baseY + 50.0f;
                            thisv->effects[i].pos.y = 0.0f;
                        }
                        if (thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ > maxDist) {
                            thisv->effects[i].base.z = baseZ - maxDist;
                            thisv->effects[i].pos.z = 0.0f;
                        }
                        if (thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ < -maxDist) {
                            thisv->effects[i].base.z = baseZ + maxDist;
                            thisv->effects[i].pos.z = 0.0f;
                        }
                    }
                }
                break;

            case 3: // reset, never reached
                thisv->effects[i].state = 0;
                break;
        }
    }
}

void ObjectKankyo_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjectKankyo* thisv = (ObjectKankyo*)thisx;

    thisv->actionFunc(thisv, globalCtx);
}

void ObjectKankyo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjectKankyo* thisv = (ObjectKankyo*)thisx;

    switch (thisv->actor.params) {
        case 0:
            ObjectKankyo_DrawFairies(thisv, globalCtx);
            break;

        case 2:
            ObjectKankyo_DrawLightning(thisv, globalCtx);
            break;

        case 3:
            ObjectKankyo_DrawSnow(thisv, globalCtx);
            break;

        case 4:
            ObjectKankyo_DrawSunGraveSpark(thisv, globalCtx);
            break;

        case 5:
            ObjectKankyo_DrawBeams(thisv, globalCtx);
            break;
    }
}

void ObjectKankyo_DrawFairies(ObjectKankyo* this2, GlobalContext* globalCtx2) {
    ObjectKankyo* thisv = this2;
    GlobalContext* globalCtx = globalCtx2;
    f32 alphaScale;
    Vec3f vec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f vec2 = { 0.0f, 0.0f, 0.0f };
    s16 i;

    if (!(globalCtx->cameraPtrs[0]->unk_14C & 0x100)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 807);
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gSunTex));
        gSPDisplayList(POLY_XLU_DISP++, gKokiriDustMoteTextureLoadDL);

        for (i = 0; i < globalCtx->envCtx.unk_EE[3]; i++) {
            Matrix_Translate(thisv->effects[i].base.x + thisv->effects[i].pos.x,
                             thisv->effects[i].base.y + thisv->effects[i].pos.y,
                             thisv->effects[i].base.z + thisv->effects[i].pos.z, MTXMODE_NEW);

            // scale when fading in or out
            alphaScale = thisv->effects[i].alpha / 50.0f;
            if (alphaScale > 1.0f) {
                alphaScale = 1.0f;
            }

            Matrix_Scale(thisv->effects[i].size * alphaScale, thisv->effects[i].size * alphaScale,
                         thisv->effects[i].size * alphaScale, MTXMODE_APPLY);
            if (i < 32) {
                if (thisv->effects[i].state != 2) {
                    if (thisv->effects[i].alpha > 0) {
                        thisv->effects[i].alpha--;
                    }
                } else {
                    if (thisv->effects[i].alpha < 100) {
                        thisv->effects[i].alpha++;
                    }
                }
            } else {
                if (thisv->effects[i].state != 2) {
                    if ((thisv->effects[i].alphaTimer & 0x1F) < 16) {
                        if (thisv->effects[i].alpha < 235) {
                            thisv->effects[i].alpha += 20;
                        }
                    } else {
                        if (thisv->effects[i].alpha > 20) {
                            thisv->effects[i].alpha -= 20;
                        }
                    }
                } else {
                    // unreachable
                    if ((thisv->effects[i].alphaTimer & 0xF) < 8) {
                        if (thisv->effects[i].alpha < 255) {
                            thisv->effects[i].alpha += 100;
                        }
                    } else {
                        if (thisv->effects[i].alpha > 10) {
                            thisv->effects[i].alpha -= 10;
                        }
                    }
                }
            }

            gDPPipeSync(POLY_XLU_DISP++);

            switch (i & 1) {
                case 0:
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, thisv->effects[i].alpha);
                    gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 0, thisv->effects[i].alpha);
                    break;

                case 1:
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, thisv->effects[i].alpha);
                    gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, thisv->effects[i].alpha);
                    break;
            }

            Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZ(DEG_TO_RAD(globalCtx->state.frames * 20.0f), MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 913), G_MTX_LOAD);
            gSPDisplayList(POLY_XLU_DISP++, gKokiriDustMoteDL);
        }
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 922);
    }
}

void ObjectKankyo_DrawSnow(ObjectKankyo* this2, GlobalContext* globalCtx2) {
    ObjectKankyo* thisv = this2;
    GlobalContext* globalCtx = globalCtx2;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 maxDist;
    f32 temp;
    f32 baseX;
    f32 baseY;
    f32 baseZ;
    Vec3f vec1 = { 0.0f, 0.0f, 0.0f };
    Vec3f vec2 = { 0.0f, 0.0f, 0.0f };
    s16 i;
    s32 pad;
    s32 pad2;

    if (!(globalCtx->cameraPtrs[0]->unk_14C & 0x100)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 958);
        if (globalCtx->envCtx.unk_EE[2] < globalCtx->envCtx.unk_EE[3]) {
            if (globalCtx->state.frames % 16 == 0) {
                globalCtx->envCtx.unk_EE[2] += 2;
            }
        } else if (globalCtx->envCtx.unk_EE[2] > globalCtx->envCtx.unk_EE[3]) {
            if (globalCtx->state.frames % 16 == 0) {
                globalCtx->envCtx.unk_EE[2] -= 2;
            }
        }

        for (i = 0; i < globalCtx->envCtx.unk_EE[2]; i++) {
            switch (thisv->effects[i].state) {
                case 0:
                    // spawn in front of the camera
                    dx = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
                    dy = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
                    dz = globalCtx->view.lookAt.z - globalCtx->view.eye.z;
                    dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

                    // fake
                    temp = dz / dist;
                    thisv->effects[i].base.x = globalCtx->view.eye.x + dx / dist * 80.0f;
                    thisv->effects[i].base.y = globalCtx->view.eye.y + dy / dist * 80.0f;
                    thisv->effects[i].base.z = globalCtx->view.eye.z + temp * 80.0f;

                    thisv->effects[i].pos.x = (Rand_ZeroOne() - 0.5f) * 160.0f;
                    thisv->effects[i].pos.y = 80.0f;
                    thisv->effects[i].pos.z = (Rand_ZeroOne() - 0.5f) * 160.0f;
                    if (thisv->effects[i].base.y + thisv->effects[i].pos.y < 50.0f) {
                        thisv->effects[i].base.y = 50.0f;
                    }
                    thisv->effects[i].speed = Rand_ZeroOne() * 5.0f + 0.5f;
                    thisv->effects[i].dirPhase.x = Rand_ZeroOne() * 360.0f;
                    thisv->effects[i].dirPhase.z = Rand_ZeroOne() * 360.0f;
                    thisv->effects[i].state++;
                    break;

                case 1:
                    dx = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
                    dy = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
                    dz = globalCtx->view.lookAt.z - globalCtx->view.eye.z;
                    dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));

                    baseX = globalCtx->view.eye.x + dx / dist * 80.0f;
                    baseY = globalCtx->view.eye.y + dy / dist * 80.0f;
                    baseZ = globalCtx->view.eye.z + dz / dist * 80.0f;

                    thisv->effects[i].dirPhase.x += 0.049999997f * Rand_ZeroOne();
                    thisv->effects[i].dirPhase.z += 0.049999997f * Rand_ZeroOne();
                    thisv->effects[i].pos.x += sinf(thisv->effects[i].dirPhase.x * 0.01f);
                    thisv->effects[i].pos.z += cosf(thisv->effects[i].dirPhase.z * 0.01f);
                    thisv->effects[i].pos.y += -thisv->effects[i].speed;

                    if (thisv->effects[i].base.y + thisv->effects[i].pos.y < thisv->actor.world.pos.y ||
                        thisv->effects[i].base.y + thisv->effects[i].pos.y < globalCtx->view.eye.y - 150.0f) {
                        thisv->effects[i].state++;
                    }

                    maxDist = 80;
                    if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX > maxDist ||
                        thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX < -maxDist ||
                        thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY > maxDist ||
                        thisv->effects[i].base.y + thisv->effects[i].pos.y - baseY < -maxDist ||
                        thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ > maxDist ||
                        thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ < -maxDist) {

                        // when off screen, wrap around to the other side
                        if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX > maxDist) {
                            thisv->effects[i].base.x = baseX - maxDist;
                            thisv->effects[i].pos.x = 0.0f;
                        }
                        if (thisv->effects[i].base.x + thisv->effects[i].pos.x - baseX < -maxDist) {
                            thisv->effects[i].base.x = baseX + maxDist;
                            thisv->effects[i].pos.x = 0.0f;
                        }
                        if (thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ > maxDist) {
                            thisv->effects[i].base.z = baseZ - maxDist;
                            thisv->effects[i].pos.z = 0.0f;
                        }
                        if (thisv->effects[i].base.z + thisv->effects[i].pos.z - baseZ < -maxDist) {
                            thisv->effects[i].base.z = baseZ + maxDist;
                            thisv->effects[i].pos.z = 0.0f;
                        }
                    }
                    break;

                case 2:
                    thisv->effects[i].state = 0;
                    break;
            }

            if (1) {}
            if (1) {}
            Matrix_Translate(thisv->effects[i].base.x + thisv->effects[i].pos.x,
                             thisv->effects[i].base.y + thisv->effects[i].pos.y,
                             thisv->effects[i].base.z + thisv->effects[i].pos.z, MTXMODE_NEW);
            Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);
            gDPPipeSync(POLY_XLU_DISP++);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 200, 200, 180);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 180);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1107),
                      G_MTX_LOAD);

            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gDust5Tex));

            func_80094C50(globalCtx->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_MODELVIEW | G_MTX_NOPUSH | G_MTX_MUL);

            gDPPipeSync(POLY_XLU_DISP++);

            gSPDisplayList(POLY_XLU_DISP++, gEffDustDL);

            gDPPipeSync(POLY_XLU_DISP++);
        }

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1127);
    }
}

void ObjectKankyo_Lightning(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != 0 && globalCtx->csCtx.npcActions[0] != NULL) {
        switch (thisv->effects[0].state) {
            case 0:
                thisv->effects[0].timer = 0;
                if (globalCtx->csCtx.npcActions[0]->action == 2) {
                    thisv->effects[0].state++;
                }
                break;

            case 1:
                if (++thisv->effects[0].timer >= 7) {
                    thisv->effects[0].state++;
                }
                break;

            case 2:
                if (globalCtx->csCtx.npcActions[0]->action == 1) {
                    thisv->effects[0].state = 0;
                }
                break;
        }
    }
}

void ObjectKankyo_DrawLightning(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1182);

    if (thisv->effects[0].state == 1) {
        Matrix_Translate(globalCtx->csCtx.npcActions[0]->startPos.x, globalCtx->csCtx.npcActions[0]->startPos.y,
                         globalCtx->csCtx.npcActions[0]->startPos.z, MTXMODE_NEW);
        Matrix_RotateX(DEG_TO_RAD(20.0f), MTXMODE_APPLY);
        Matrix_RotateZ(DEG_TO_RAD(20.0f), MTXMODE_APPLY);
        Matrix_Scale(2.0f, 5.0f, 2.0f, MTXMODE_APPLY);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 128);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 128);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1213), G_MTX_LOAD);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEffLightningTextures[thisv->effects[0].timer]));
        func_80094C50(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_MODELVIEW | G_MTX_NOPUSH | G_MTX_MUL);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPDisplayList(POLY_XLU_DISP++, gEffLightningDL);
        gDPPipeSync(POLY_XLU_DISP++);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1233);
}

void ObjectKankyo_SunGraveSparkInit(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    s32 objBankIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_SPOT02_OBJECTS);

    if (objBankIndex < 0) {
        ASSERT(0, "0", "../z_object_kankyo.c", 1251);
    } else {
        thisv->requiredObjBankIndex = objBankIndex;
    }
    ObjectKankyo_SetupAction(thisv, ObjectKankyo_WaitForSunGraveSparkObject);
}

void ObjectKankyo_WaitForSunGraveSparkObject(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->requiredObjBankIndex)) {
        thisv->requiredObjectLoaded = true;
        thisv->effects[0].alpha = 0;
        thisv->actor.objBankIndex = thisv->requiredObjBankIndex;
        thisv->effects[0].size = 7.0f;
        ObjectKankyo_SetupAction(thisv, ObjectKankyo_SunGraveSpark);
    }
}

void ObjectKankyo_SunGraveSpark(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state != 0) {
        if (globalCtx->csCtx.npcActions[1] != NULL && globalCtx->csCtx.npcActions[1]->action == 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
            if ((s16)thisv->effects[0].alpha + 20 > 255) {
                thisv->effects[0].alpha = 255;
            } else {
                thisv->effects[0].alpha += 20;
            }
            Math_SmoothStepToF(&thisv->effects[0].size, 1.8f, 0.5f, 0.28f, 0.01f);
        }
    }
}

void ObjectKankyo_DrawSunGraveSpark(ObjectKankyo* this2, GlobalContext* globalCtx2) {
    ObjectKankyo* thisv = this2;
    GlobalContext* globalCtx = globalCtx2;
    Vec3f start;
    Vec3f end;
    f32 weight;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1324);
    if (globalCtx->csCtx.state != 0) {
        if (globalCtx->csCtx.npcActions[1] != NULL && globalCtx->csCtx.npcActions[1]->action == 2 &&
            thisv->requiredObjectLoaded) {
            // apparently, light waves with larger amplitudes look brighter, so the name 'amplitude' kind of works here
            if (thisv->effects[0].state == 0) {
                thisv->effects[0].amplitude += 1.0f / 7.0f;
                if (thisv->effects[0].amplitude >= 1.0f) {
                    thisv->effects[0].amplitude = 1.0f;
                    thisv->effects[0].state++;
                }
            } else {
                thisv->effects[0].amplitude -= 1.0f / 7.0f;
                if (thisv->effects[0].amplitude <= 0.1f) {
                    thisv->effects[0].amplitude = 0.0f;
                    thisv->effects[0].state = 0;
                }
            }

            if (++thisv->effects[0].timer > 7) {
                thisv->effects[0].timer = 0;
            }

            start.x = globalCtx->csCtx.npcActions[1]->startPos.x;
            start.y = globalCtx->csCtx.npcActions[1]->startPos.y;
            start.z = globalCtx->csCtx.npcActions[1]->startPos.z;

            end.x = globalCtx->csCtx.npcActions[1]->endPos.x;
            end.y = globalCtx->csCtx.npcActions[1]->endPos.y;
            end.z = globalCtx->csCtx.npcActions[1]->endPos.z;

            weight = Environment_LerpWeight(globalCtx->csCtx.npcActions[1]->endFrame,
                                            globalCtx->csCtx.npcActions[1]->startFrame, globalCtx->csCtx.frames);
            Matrix_Translate((end.x - start.x) * weight + start.x, (end.y - start.y) * weight + start.y,
                             (end.z - start.z) * weight + start.z, MTXMODE_NEW);
            Matrix_Scale(thisv->effects[0].size, thisv->effects[0].size, thisv->effects[0].size, MTXMODE_APPLY);
            func_80093D84(globalCtx->state.gfxCtx);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(105 * thisv->effects[0].amplitude) + 150, 255,
                            (u8)(105 * thisv->effects[0].amplitude) + 150, thisv->effects[0].alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, (u8)(155 * thisv->effects[0].amplitude) + 100,
                           (u8)(255 * thisv->effects[0].amplitude), 255 - (u8)(255 * thisv->effects[0].amplitude),
                           thisv->effects[0].alpha);

            Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1416),
                      G_MTX_LOAD);

            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_80BA5900[thisv->effects[0].timer]));
            gDPPipeSync(POLY_XLU_DISP++);

            gSPDisplayList(POLY_XLU_DISP++, object_spot02_objects_DL_009620);
            gDPPipeSync(POLY_XLU_DISP++);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1432);
}

void ObjectKankyo_InitBeams(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    s32 objectIndex = Object_GetIndex(&globalCtx->objectCtx, OBJECT_DEMO_KEKKAI);

    if (objectIndex < 0) {
        ASSERT(0, "0", "../z_object_kankyo.c", 1449);
    } else {
        thisv->requiredObjBankIndex = objectIndex;
    }
    ObjectKankyo_SetupAction(thisv, ObjectKankyo_WaitForBeamObject);
}

void ObjectKankyo_WaitForBeamObject(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->requiredObjBankIndex)) {
        thisv->requiredObjectLoaded = true;
        thisv->actor.objBankIndex = thisv->requiredObjBankIndex;
        ObjectKankyo_SetupAction(thisv, ObjectKankyo_Beams);
    }
}

void ObjectKankyo_Beams(ObjectKankyo* thisv, GlobalContext* globalCtx) {
    u8 i;

    if (globalCtx->csCtx.state != 0) {
        for (i = 0; i < 6; i++) {
            if (globalCtx->csCtx.npcActions[i + 1] != NULL && globalCtx->csCtx.npcActions[i + 1]->action == 2) {
                if (thisv->effects[i].size == 0.1f) {
                    func_800F3F3C(11);
                }
                Math_ApproachZeroF(&thisv->effects[i].size, 0.1f, 0.1f);
            }
        }
    }
}

void ObjectKankyo_DrawBeams(ObjectKankyo* this2, GlobalContext* globalCtx2) {
    static Color_RGB8 sBeamPrimColors[] = {
        { 255, 255, 170 }, { 170, 255, 255 }, { 255, 170, 255 },
        { 255, 255, 170 }, { 255, 255, 170 }, { 255, 255, 170 },
    };
    static Color_RGB8 sBeamEnvColors[] = {
        { 0, 200, 0 }, { 0, 50, 255 }, { 100, 0, 200 }, { 200, 0, 0 }, { 200, 255, 0 }, { 255, 120, 0 },
    };
    ObjectKankyo* thisv = this2;
    GlobalContext* globalCtx = globalCtx2;
    s16 i;
    f32 beamX[] = { 430.0f, 860.0f, 430.0f, -426.0f, -862.0f, -440.0f };
    f32 beamY[] = { 551.0f, 551.0f, 551.0f, 554.0f, 551.0f, 547.0f };
    f32 beamZ[] = { -96.0f, -840.0f, -1585.0f, -1578.0f, -840.0f, -78.0f };
    f32 beamYaw[] = { 29.9f, 90.0f, 150.0f, 30.0f, 90.0f, -30.1f };
    f32 beamPitch[] = { 103.4f, 103.8f, 103.6f, -103.4f, -103.5f, 103.5f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1539);

    if (thisv->requiredObjectLoaded) {
        for (i = 0; i < 6; i++) {
            if (thisv->effects[i].size > 0.001f) {
                Matrix_Translate(beamX[i], beamY[i], beamZ[i], MTXMODE_NEW);
                Matrix_RotateY(DEG_TO_RAD(beamYaw[i]), MTXMODE_APPLY);
                Matrix_RotateX(DEG_TO_RAD(beamPitch[i]), MTXMODE_APPLY);
                Matrix_Scale(thisv->effects[i].size, 0.1f, thisv->effects[i].size, MTXMODE_APPLY);
                func_80093D84(globalCtx->state.gfxCtx);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, sBeamPrimColors[i].r, sBeamPrimColors[i].g,
                                sBeamPrimColors[i].b, 128);
                gDPSetEnvColor(POLY_XLU_DISP++, sBeamEnvColors[i].r, sBeamEnvColors[i].g, sBeamEnvColors[i].b, 128);
                gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1586),
                          G_MTX_LOAD);
                gSPSegment(POLY_XLU_DISP++, 0x08,
                           Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->state.frames * 5,
                                            globalCtx->state.frames * 10, 32, 64, 1, globalCtx->state.frames * 5,
                                            globalCtx->state.frames * 10, 32, 64));
                gSPDisplayList(POLY_XLU_DISP++, gDemoKekkaiDL_005FF0);
            }
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_object_kankyo.c", 1607);
}

void ObjectKankyo_Reset(void) {
    sIsSpawned = false;
    sTrailingFairies = 0;
}