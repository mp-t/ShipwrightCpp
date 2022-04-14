/*
 * File: z_en_bdfire.c
 * Overlay: ovl_En_Bdfire
 * Description: King Dodongo's Fire Breath
 */

#include "z_en_bdfire.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnBdfire_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBdfire_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBdfire_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBdfire_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBdfire_DrawFire(EnBdfire* thisv, GlobalContext* globalCtx);
void func_809BC2A4(EnBdfire* thisv, GlobalContext* globalCtx);
void func_809BC598(EnBdfire* thisv, GlobalContext* globalCtx);

const ActorInit En_Bdfire_InitVars = {
    0,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_KINGDODONGO,
    sizeof(EnBdfire),
    (ActorFunc)EnBdfire_Init,
    (ActorFunc)EnBdfire_Destroy,
    (ActorFunc)EnBdfire_Update,
    (ActorFunc)EnBdfire_Draw,
    NULL,
};

void EnBdfire_SetupAction(EnBdfire* thisv, EnBdfireActionFunc actionFunc) {
    thisv->actionFunc = actionFunc;
}

void EnbdFire_SetupDraw(EnBdfire* thisv, EnBdfireDrawFunc drawFunc) {
    thisv->drawFunc = drawFunc;
}

void EnBdfire_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBdfire* thisv = (EnBdfire*)thisx;
    s32 pad;

    Actor_SetScale(&thisv->actor, 0.6f);
    EnbdFire_SetupDraw(thisv, EnBdfire_DrawFire);
    if (thisv->actor.params < 0) {
        EnBdfire_SetupAction(thisv, func_809BC2A4);
        thisv->actor.scale.x = 2.8f;
        thisv->unk_154 = 90;
        Lights_PointNoGlowSetInfo(&thisv->lightInfoNoGlow, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                  thisv->actor.world.pos.z, 255, 255, 255, 300);
        thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfoNoGlow);
    } else {
        EnBdfire_SetupAction(thisv, func_809BC598);
        ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
        thisv->actor.speedXZ = 30.0f;
        thisv->unk_154 = (25 - (s32)(thisv->actor.params * 0.8f));
        if (thisv->unk_154 < 0) {
            thisv->unk_154 = 0;
        }
        thisv->unk_188 = 4.2000003f - (thisv->actor.params * 0.25f * 0.6f);

        if (thisv->unk_188 < 0.90000004f) {
            thisv->unk_188 = 0.90000004f;
        }
        thisv->unk_18C = 255.0f - (thisv->actor.params * 10.0f);
        if (thisv->unk_18C < 20.0f) {
            thisv->unk_18C = 20.0f;
        }
        thisv->unk_156 = (Rand_ZeroOne() * 8.0f);
    }
}

void EnBdfire_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBdfire* thisv = (EnBdfire*)thisx;

    if (thisv->actor.params < 0) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    }
}

void func_809BC2A4(EnBdfire* thisv, GlobalContext* globalCtx) {
    BossDodongo* kingDodongo;
    s32 temp;

    kingDodongo = (BossDodongo*)thisv->actor.parent;
    thisv->actor.world.pos.x = kingDodongo->firePos.x;
    thisv->actor.world.pos.y = kingDodongo->firePos.y;
    thisv->actor.world.pos.z = kingDodongo->firePos.z;
    if (kingDodongo->unk_1E2 == 0) {
        Math_SmoothStepToF(&thisv->actor.scale.x, 0.0f, 1.0f, 0.6f, 0.0f);
        if (Math_SmoothStepToF(&thisv->unk_18C, 0.0f, 1.0f, 20.0f, 0.0f) == 0.0f) {
            Actor_Kill(&thisv->actor);
        }
    } else {
        if (thisv->unk_154 < 70) {
            Math_SmoothStepToF(&thisv->unk_18C, 128.0f, 0.1f, 1.5f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_190, 255.0f, 1.0f, 3.8249998f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_194, 100.0f, 1.0f, 1.5f, 0.0f);
        }
        if (thisv->unk_154 == 0) {
            temp = 0;
        } else {
            thisv->unk_154--;
            temp = thisv->unk_154;
        }
        if (temp == 0) {
            Math_SmoothStepToF(&thisv->actor.scale.x, 0.0f, 1.0f, 0.3f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_190, 0.0f, 1.0f, 25.5f, 0.0f);
            Math_SmoothStepToF(&thisv->unk_194, 0.0f, 1.0f, 10.0f, 0.0f);
            if (Math_SmoothStepToF(&thisv->unk_18C, 0.0f, 1.0f, 10.0f, 0.0f) == 0.0f) {
                Actor_Kill(&thisv->actor);
            }
        }
        Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
        Lights_PointSetColorAndRadius(&thisv->lightInfoNoGlow, thisv->unk_190, thisv->unk_194, 0, 300);
    }
}

void func_809BC598(EnBdfire* thisv, GlobalContext* globalCtx) {
    s16 phi_v1_2;
    Player* player = GET_PLAYER(globalCtx);
    f32 distToBurn;
    BossDodongo* bossDodongo;
    s16 i;
    s16 phi_v1;
    s32 temp;

    bossDodongo = ((BossDodongo*)thisv->actor.parent);
    thisv->unk_158 = bossDodongo->unk_1A2;
    phi_v1_2 = 0;
    if (thisv->actor.params == 0) {
        Audio_PlaySoundGeneral(NA_SE_EN_DODO_K_FIRE - SFX_FLAG, &thisv->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
    Math_SmoothStepToF(&thisv->actor.scale.x, thisv->unk_188, 0.3f, 0.5f, 0.0f);
    Actor_SetScale(&thisv->actor, thisv->actor.scale.x);
    if (thisv->actor.world.pos.x < -1390.0f) {
        if (thisv->actor.velocity.x < -10.0f) {
            thisv->actor.world.pos.x = -1390.0f;
            phi_v1_2 = 1;
        }
    }
    if ((thisv->actor.world.pos.x > -390.0f) && (thisv->actor.velocity.x > 10.0f)) {
        thisv->actor.world.pos.x = -390.0f;
        phi_v1_2 = 1;
    }
    if ((thisv->actor.world.pos.z > -2804.0f) && (thisv->actor.velocity.z > 10.0f)) {
        thisv->actor.world.pos.z = -2804.0f;
        phi_v1_2 = 1;
    }
    if ((thisv->actor.world.pos.z < -3804.0f) && (thisv->actor.velocity.z < -10.0f)) {
        thisv->actor.world.pos.z = -3804.0f;
        phi_v1_2 = 1;
    }
    if (phi_v1_2 != 0) {
        if (thisv->unk_158 == 0) {
            thisv->actor.world.rot.y += 0x4000;
        } else {
            thisv->actor.world.rot.y -= 0x4000;
        }
    }
    if (thisv->unk_154 == 0) {
        temp = 0;
    } else {
        thisv->unk_154--;
        temp = thisv->unk_154;
    }
    if (temp == 0) {
        Math_SmoothStepToF(&thisv->unk_18C, 0.0f, 1.0f, 10.0f, 0.0f);
        if (thisv->unk_18C < 10.0f) {
            Actor_Kill(&thisv->actor);
            return;
        }
    } else if (!player->isBurning) {
        distToBurn = (thisv->actor.scale.x * 130.0f) / 4.2000003f;
        if (thisv->actor.xyzDistToPlayerSq < SQ(distToBurn)) {
            for (i = 0; i < 18; i++) {
                player->flameTimers[i] = Rand_S16Offset(0, 200);
            }
            player->isBurning = true;
            func_8002F6D4(globalCtx, &thisv->actor, 20.0f, thisv->actor.world.rot.y, 0.0f, 8);
            osSyncPrintf("POWER\n");
        }
    }
}

void EnBdfire_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBdfire* thisv = (EnBdfire*)thisx;

    thisv->unk_156++;
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
}

void EnBdfire_DrawFire(EnBdfire* thisv, GlobalContext* globalCtx) {
    static void* D_809BCB10[] = {
        object_kingdodongo_Tex_0264E0, object_kingdodongo_Tex_0274E0, object_kingdodongo_Tex_0284E0,
        object_kingdodongo_Tex_0294E0, object_kingdodongo_Tex_02A4E0, object_kingdodongo_Tex_02B4E0,
        object_kingdodongo_Tex_02C4E0, object_kingdodongo_Tex_02D4E0,
    };
    s16 temp;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bdfire.c", 612);
    temp = thisv->unk_156 & 7;
    Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
    func_80094BC4(globalCtx->state.gfxCtx);
    POLY_XLU_DISP = func_80094968(POLY_XLU_DISP);
    gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                      ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                      ENVIRONMENT);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 100, (s8)thisv->unk_18C);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 0, 0, 0);
    gSPSegment(POLY_XLU_DISP++, 8, SEGMENTED_TO_VIRTUAL(D_809BCB10[temp]));
    Matrix_Translate(0.0f, 11.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_bdfire.c", 647),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, object_kingdodongo_DL_01D950);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bdfire.c", 651);
}

void EnBdfire_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBdfire* thisv = (EnBdfire*)thisx;

    thisv->drawFunc(thisv, globalCtx);
}
