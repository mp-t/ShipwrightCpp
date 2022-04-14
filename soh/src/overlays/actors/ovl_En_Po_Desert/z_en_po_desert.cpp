/*
 * File: z_en_po_desert.c
 * Overlay: ovl_En_Po_Desert
 * Description: Guide Poe (Haunted Wasteland)
 */

#include "z_en_po_desert.h"
#include "objects/object_po_field/object_po_field.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_7 | ACTOR_FLAG_12)

void EnPoDesert_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPoDesert_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPoDesert_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPoDesert_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnPoDesert_SetNextPathPoint(EnPoDesert* thisv, GlobalContext* globalCtx);
void EnPoDesert_WaitForPlayer(EnPoDesert* thisv, GlobalContext* globalCtx);
void EnPoDesert_MoveToNextPoint(EnPoDesert* thisv, GlobalContext* globalCtx);
void EnPoDesert_Disappear(EnPoDesert* thisv, GlobalContext* globalCtx);

const ActorInit En_Po_Desert_InitVars = {
    ACTOR_EN_PO_DESERT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_PO_FIELD,
    sizeof(EnPoDesert),
    (ActorFunc)EnPoDesert_Init,
    (ActorFunc)EnPoDesert_Destroy,
    (ActorFunc)EnPoDesert_Update,
    (ActorFunc)EnPoDesert_Draw,
    NULL,
};

static ColliderCylinderInit sColliderInit = {
    {
        COLTYPE_HIT3,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 50, 20, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x5C, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 3200, ICHAIN_STOP),
};

void EnPoDesert_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnPoDesert* thisv = (EnPoDesert*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPoeFieldSkel, &gPoeFieldFloatAnim, thisv->jointTable, thisv->morphTable,
                   10);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sColliderInit);
    thisv->lightColor.r = 255;
    thisv->lightColor.g = 255;
    thisv->lightColor.b = 210;
    thisv->lightColor.a = 255;
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z,
                              255, 255, 255, 200);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 37.0f);
    thisv->currentPathPoint = 1;
    thisv->actor.params = (thisv->actor.params >> 8) & 0xFF;
    thisv->targetY = thisv->actor.world.pos.y;
    EnPoDesert_SetNextPathPoint(thisv, globalCtx);
}

void EnPoDesert_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPoDesert* thisv = (EnPoDesert*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnPoDesert_SetNextPathPoint(EnPoDesert* thisv, GlobalContext* globalCtx) {
    Path* path = &globalCtx->setupPathList[thisv->actor.params];
    Vec3s* pathPoint;

    Animation_MorphToLoop(&thisv->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    pathPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[thisv->currentPathPoint];
    thisv->actor.home.pos.x = pathPoint->x;
    thisv->actor.home.pos.y = pathPoint->y;
    thisv->actor.home.pos.z = pathPoint->z;
    thisv->initDistToNextPoint = Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos);
    thisv->initDistToNextPoint = CLAMP_MIN(thisv->initDistToNextPoint, 1.0f);
    thisv->currentPathPoint++;
    thisv->yDiff = thisv->actor.home.pos.y - thisv->actor.world.pos.y;
    thisv->actor.speedXZ = 0.0f;
    if (path->count == thisv->currentPathPoint) {
        thisv->currentPathPoint = 0;
    }
    thisv->actionFunc = EnPoDesert_WaitForPlayer;
}

void EnPoDesert_SetupMoveToNextPoint(EnPoDesert* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeFieldFloatAnim, -5.0f);
    thisv->actionFunc = EnPoDesert_MoveToNextPoint;
}

void EnPoDesert_SetupDisappear(EnPoDesert* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    thisv->actionTimer = 16;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DISAPPEAR);
    thisv->actionFunc = EnPoDesert_Disappear;
}

void EnPoDesert_UpdateSpeedModifier(EnPoDesert* thisv) {
    if (thisv->speedModifier == 0) {
        thisv->speedModifier = 32;
    }
    if (thisv->speedModifier != 0) {
        thisv->speedModifier--;
    }
    thisv->actor.world.pos.y = Math_SinS(thisv->speedModifier * 0x800) * 13.0f + thisv->targetY;
}

void EnPoDesert_WaitForPlayer(EnPoDesert* thisv, GlobalContext* globalCtx) {
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    if (thisv->actor.xzDistToPlayer < 200.0f && (thisv->currentPathPoint != 2 || globalCtx->actorCtx.unk_03)) {
        if (thisv->currentPathPoint == 2) {
            if (Gameplay_InCsMode(globalCtx)) {
                thisv->actor.shape.rot.y += 0x800;
                return;
            }
            Message_StartTextbox(globalCtx, 0x600B, NULL);
        }
        EnPoDesert_SetupMoveToNextPoint(thisv);
    } else {
        thisv->actor.shape.rot.y += 0x800;
    }
}

void EnPoDesert_MoveToNextPoint(EnPoDesert* thisv, GlobalContext* globalCtx) {
    f32 temp_f20;

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    temp_f20 = sinf(thisv->actionTimer * (std::numbers::pi_v<float> / 20.0f)) * 5.0f;
    thisv->actor.world.pos.x += temp_f20 * Math_CosS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.z += temp_f20 * Math_SinS(thisv->actor.shape.rot.y);
    if (thisv->actionTimer == 0) {
        thisv->actionTimer = 40;
    }
    temp_f20 = Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos);
    thisv->actor.world.rot.y = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y + 0x8000, 5, 0x400);
    thisv->actor.speedXZ = sinf(thisv->speedModifier * (std::numbers::pi_v<float> / 32.0f)) * 2.5f + 5.5f;
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    thisv->targetY = thisv->actor.home.pos.y - ((temp_f20 * thisv->yDiff) / thisv->initDistToNextPoint);
    if (temp_f20 < 40.0f) {
        if (thisv->currentPathPoint != 0) {
            EnPoDesert_SetNextPathPoint(thisv, globalCtx);
        } else {
            EnPoDesert_SetupDisappear(thisv);
        }
    }
}

void EnPoDesert_Disappear(EnPoDesert* thisv, GlobalContext* globalCtx) {
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    thisv->actor.shape.rot.y += 0x2000;
    thisv->lightColor.a = thisv->actionTimer * 15.9375f;
    thisv->actor.shape.shadowAlpha = thisv->lightColor.a;
    if (thisv->actionTimer == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnPoDesert_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnPoDesert* thisv = (EnPoDesert*)thisx;
    s32 pad;

    SkelAnime_Update(&thisv->skelAnime);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    EnPoDesert_UpdateSpeedModifier(thisv);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 27.0f, 60.0f, 4);
    Actor_SetFocus(&thisv->actor, 42.0f);
    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    if (globalCtx->actorCtx.unk_03) {
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_7;
        thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    } else {
        thisv->actor.shape.shadowDraw = NULL;
        thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_7);
    }
}

s32 EnPoDesert_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx, Gfx** gfxP) {
    EnPoDesert* thisv = (EnPoDesert*)thisx;
    f32 mtxScale;

    if (thisv->actionFunc == EnPoDesert_Disappear && limbIndex == 7) {
        mtxScale = thisv->actionTimer / 16.0f;
        Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
    }
    if (!CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
        *dList = NULL;
    }
    return false;
}

void EnPoDesert_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                             Gfx** gfxP) {
    static Vec3f baseLightPos = { 0.0f, 1400.0f, 0.0f };

    EnPoDesert* thisv = (EnPoDesert*)thisx;
    f32 rand;
    Color_RGBA8 color;
    Vec3f lightPos;

    if (limbIndex == 7) {
        Matrix_MultVec3f(&baseLightPos, &lightPos);
        rand = Rand_ZeroOne();
        color.r = (s16)(rand * 30.0f) + 225;
        color.g = (s16)(rand * 100.0f) + 155;
        color.b = (s16)(rand * 160.0f) + 95;
        if (CHECK_FLAG_ALL(thisv->actor.flags, ACTOR_FLAG_7)) {
            gDPPipeSync((*gfxP)++);
            gDPSetEnvColor((*gfxP)++, color.r, color.g, color.b, 255);
            gSPMatrix((*gfxP)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_desert.c", 523),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList((*gfxP)++, gPoeFieldLanternDL);
            gSPDisplayList((*gfxP)++, gPoeFieldLanternTopDL);
            gDPPipeSync((*gfxP)++);
            gDPSetEnvColor((*gfxP)++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, thisv->lightColor.a);
        }
        Lights_PointNoGlowSetInfo(&thisv->lightInfo, lightPos.x, lightPos.y, lightPos.z, color.r, color.g, color.b, 200);
    }
}

void EnPoDesert_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnPoDesert* thisv = (EnPoDesert*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_desert.c", 559);
    func_80093D84(globalCtx->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x0A, Gfx_EnvColor(globalCtx->state.gfxCtx, 255, 85, 0, 255));
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_EnvColor(globalCtx->state.gfxCtx, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                            thisv->lightColor.a));
    if (thisv->actionFunc == EnPoDesert_Disappear) {
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280 + 2);
    }
    POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                   EnPoDesert_OverrideLimbDraw, EnPoDesert_PostLimbDraw, &thisv->actor, POLY_XLU_DISP);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_desert.c", 597);
}
