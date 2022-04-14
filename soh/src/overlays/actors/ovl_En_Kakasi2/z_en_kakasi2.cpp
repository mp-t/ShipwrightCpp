/*
 * File: z_en_kakasi2.c
 * Overlay: ovl_En_Kakasi2
 * Description: Pierre the Scarecrow Spawn
 */

#include "z_en_kakasi2.h"
#include "vt.h"
#include "objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

void EnKakasi2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnKakasi2_Update(Actor* thisx, GlobalContext* globalCtx);
void func_80A90948(Actor* thisx, GlobalContext* globalCtx);

void func_80A9062C(EnKakasi2* thisv, GlobalContext* globalCtx);
void func_80A90264(EnKakasi2* thisv, GlobalContext* globalCtx);
void func_80A904D8(EnKakasi2* thisv, GlobalContext* globalCtx);
void func_80A90578(EnKakasi2* thisv, GlobalContext* globalCtx);
void func_80A906C4(EnKakasi2* thisv, GlobalContext* globalCtx);

const ActorInit En_Kakasi2_InitVars = {
    ACTOR_EN_KAKASI2,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_KA,
    sizeof(EnKakasi2),
    (ActorFunc)EnKakasi2_Init,
    (ActorFunc)EnKakasi2_Destroy,
    (ActorFunc)EnKakasi2_Update,
    NULL,
    NULL,
};

void EnKakasi2_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* thisv = (EnKakasi2*)thisx;
    s32 pad;
    f32 spawnRangeY;
    f32 spawnRangeXZ;

    osSyncPrintf("\n\n");
    // "Visit Umeda"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 梅田参号見参！ ☆☆☆☆☆ \n" VT_RST);

    thisv->switchFlag = thisv->actor.params & 0x3F;
    spawnRangeY = (thisv->actor.params >> 6) & 0xFF;
    spawnRangeXZ = thisv->actor.world.rot.z;
    if (thisv->switchFlag == 0x3F) {
        thisv->switchFlag = -1;
    }
    thisv->actor.targetMode = 4;
    thisv->maxSpawnDistance.x = (spawnRangeY * 40.0f) + 40.0f;
    thisv->maxSpawnDistance.y = (spawnRangeXZ * 40.0f) + 40.0f;

    // "Former? (Argument 0)"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(引数０) ☆☆☆☆ %f\n" VT_RST, spawnRangeY);
    // "Former? (Z angle)"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 元？(Ｚアングル) ☆☆ %f\n" VT_RST, spawnRangeXZ);
    // "Correction coordinates X"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｘ ☆☆☆☆☆ %f\n" VT_RST, thisv->maxSpawnDistance.x);
    // "Correction coordinates Y"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｙ ☆☆☆☆☆ %f\n" VT_RST, thisv->maxSpawnDistance.y);
    // "Correction coordinates Z"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 補正座標Ｚ ☆☆☆☆☆ %f\n" VT_RST, thisv->maxSpawnDistance.z);
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ SAVE       ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
    osSyncPrintf("\n\n");

    thisv->actor.colChkInfo.mass = MASS_IMMOVABLE;
    thisv->height = 60.0f;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.flags |= ACTOR_FLAG_10;
    thisv->unk_198 = thisv->actor.shape.rot.y;

    if (thisv->switchFlag >= 0 && Flags_GetSwitch(globalCtx, thisv->switchFlag)) {
        thisv->actor.draw = func_80A90948;
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        thisv->actionFunc = func_80A9062C;
    } else {
        thisv->actionFunc = func_80A90264;
        thisv->actor.shape.yOffset = -8000.0f;
    }
}

void EnKakasi2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* thisv = (EnKakasi2*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
    SkelAnime_Free(&thisv->skelAnime, globalCtx); // OTR - Fixed thisv memory leak
    //! @bug SkelAnime_Free is not called
}

void func_80A90264(EnKakasi2* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    thisv->unk_194++;

    if ((BREG(1) != 0) && (thisv->actor.xzDistToPlayer < thisv->maxSpawnDistance.x) &&
        (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < thisv->maxSpawnDistance.y)) {

        thisv->actor.draw = func_80A90948;
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
        SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        OnePointCutscene_Attention(globalCtx, &thisv->actor);
        thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_27;

        func_80078884(NA_SE_SY_CORRECT_CHIME);
        if (thisv->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, thisv->switchFlag);
        }

        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
        thisv->actionFunc = func_80A904D8;
    } else if ((thisv->actor.xzDistToPlayer < thisv->maxSpawnDistance.x) &&
               (fabsf(player->actor.world.pos.y - thisv->actor.world.pos.y) < thisv->maxSpawnDistance.y) &&
               (gSaveContext.eventChkInf[9] & 0x1000)) {

        thisv->unk_194 = 0;
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0B) {
            if (thisv->switchFlag >= 0) {
                Flags_SetSwitch(globalCtx, thisv->switchFlag);
            }
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ SAVE 終了 ☆☆☆☆☆ %d\n" VT_RST, thisv->switchFlag);
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            thisv->actor.draw = func_80A90948;
            Collider_InitCylinder(globalCtx, &thisv->collider);
            Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL,
                               0);
            OnePointCutscene_Attention(globalCtx, &thisv->actor);
            func_80078884(NA_SE_SY_CORRECT_CHIME);

            thisv->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_27;
            thisv->actionFunc = func_80A904D8;
        }
    }
}

void func_80A904D8(EnKakasi2* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&thisv->skelAnime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_COME_UP_DEKU_JR);
    thisv->actionFunc = func_80A90578;
}

void func_80A90578(EnKakasi2* thisv, GlobalContext* globalCtx) {
    s16 currentFrame;

    SkelAnime_Update(&thisv->skelAnime);

    currentFrame = thisv->skelAnime.curFrame;
    if (currentFrame == 11 || currentFrame == 17) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_KAKASHI_SWING);
    }

    thisv->actor.shape.rot.y += 0x800;
    Math_ApproachZeroF(&thisv->actor.shape.yOffset, 0.5f, 500.0f);

    if (thisv->actor.shape.yOffset > -100.0f) {
        thisv->actionFunc = func_80A9062C;
        thisv->actor.shape.yOffset = 0.0f;
    }
}

void func_80A9062C(EnKakasi2* thisv, GlobalContext* globalCtx) {
    f32 frameCount = Animation_GetLastFrame(&object_ka_Anim_000214);

    Animation_Change(&thisv->skelAnime, &object_ka_Anim_000214, 0.0f, 0.0f, (s16)frameCount, ANIMMODE_ONCE, -10.0f);
    thisv->actionFunc = func_80A906C4;
}

void func_80A906C4(EnKakasi2* thisv, GlobalContext* globalCtx) {
    if (thisv->skelAnime.curFrame != 0) {
        Math_ApproachZeroF(&thisv->skelAnime.curFrame, 0.5f, 1.0f);
    }
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->unk_198, 5, 0xBB8, 0);
    SkelAnime_Update(&thisv->skelAnime);
}

void EnKakasi2_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnKakasi2* thisv = (EnKakasi2*)thisx;
    GlobalContext* globalCtx = globalCtx2;

    thisv->actor.world.rot = thisv->actor.shape.rot;
    Actor_SetFocus(&thisv->actor, thisv->height);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);

    if (thisv->actor.shape.yOffset == 0.0f) {
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    if (BREG(0) != 0) {
        if (BREG(5) != 0) {
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ thisv->actor.player_distance ☆☆☆☆☆ %f\n" VT_RST,
                         thisv->actor.xzDistToPlayer);
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ thisv->hosei.x ☆☆☆☆☆ %f\n" VT_RST, thisv->maxSpawnDistance.x);
            osSyncPrintf("\n\n");
        }
        if (thisv->actor.draw == NULL) {
            if (thisv->unk_194 != 0) {
                if ((thisv->unk_194 % 2) == 0) {
                    DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                           thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z,
                                           1.0f, 1.0f, 1.0f, 70, 70, 70, 255, 4, globalCtx->state.gfxCtx);
                }
            } else {
                DebugDisplay_AddObject(thisv->actor.world.pos.x, thisv->actor.world.pos.y, thisv->actor.world.pos.z,
                                       thisv->actor.world.rot.x, thisv->actor.world.rot.y, thisv->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 0, 255, 255, 255, 4, globalCtx->state.gfxCtx);
            }
        }
    }
}

void func_80A90948(Actor* thisx, GlobalContext* globalCtx) {
    EnKakasi2* thisv = (EnKakasi2*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, thisv->skelAnime.dListCount,
                          NULL, NULL, thisv);
}
