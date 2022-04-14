/*
 * File: z_demo_ec.c
 * Overlay: ovl_Demo_Ec
 * Description: Credits revelers in Lon Lon
 */

#include "z_demo_ec.h"
#include "vt.h"
#include "objects/object_zo/object_zo.h"
#include "objects/object_ec/object_ec.h"
#include "objects/object_ma2/object_ma2.h"
#include "objects/object_in/object_in.h"
#include "objects/object_ge1/object_ge1.h"
#include "objects/object_fu/object_fu.h"
#include "objects/object_fish/object_fish.h"
#include "objects/object_ta/object_ta.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "objects/object_ma2/object_ma2.h"
#include "objects/object_in/object_in.h"
#include "objects/object_ta/object_ta.h"
#include "objects/object_fu/object_fu.h"
#include "objects/object_toryo/object_toryo.h"
#include "objects/object_daiku/object_daiku.h"
#include "objects/object_ge1/object_ge1.h"
#include "objects/object_kz/object_kz.h"
#include "objects/object_md/object_md.h"
#include "objects/object_niw/object_niw.h"
#include "objects/object_ds2/object_ds2.h"
#include "objects/object_os/object_os.h"
#include "objects/object_rs/object_rs.h"
#include "objects/object_gm/object_gm.h"
#include "objects/object_km1/object_km1.h"
#include "objects/object_kw1/object_kw1.h"
#include "objects/object_bji/object_bji.h"
#include "objects/object_ahg/object_ahg.h"
#include "objects/object_bob/object_bob.h"
#include "objects/object_bba/object_bba.h"
#include "objects/object_ane/object_ane.h"

#define FLAGS ACTOR_FLAG_4

void DemoEc_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoEc_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoEc_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoEc_Draw(Actor* thisx, GlobalContext* globalCtx);

typedef enum {
    /* 00 */ EC_UPDATE_COMMON,
    /* 01 */ EC_UPDATE_INGO,
    /* 02 */ EC_UPDATE_TALON,
    /* 03 */ EC_UPDATE_WINDMILL_MAN,
    /* 04 */ EC_UPDATE_KOKIRI_BOY,
    /* 05 */ EC_UPDATE_KOKIRI_GIRL,
    /* 06 */ EC_UPDATE_OLD_MAN,
    /* 07 */ EC_UPDATE_BEARDED_MAN,
    /* 08 */ EC_UPDATE_WOMAN,
    /* 09 */ EC_UPDATE_OLD_WOMAN,
    /* 10 */ EC_UPDATE_BOSS_CARPENTER,
    /* 11 */ EC_UPDATE_CARPENTER,
    /* 12 */ EC_UPDATE_DANCING_KOKIRI_BOY,
    /* 13 */ EC_UPDATE_DANCING_KOKIRI_GIRL,
    /* 14 */ EC_UPDATE_GERUDO,
    /* 15 */ EC_UPDATE_DANCING_ZORA,
    /* 16 */ EC_UPDATE_KING_ZORA,
    /* 17 */ EC_UPDATE_17,
    /* 18 */ EC_UPDATE_18,
    /* 19 */ EC_UPDATE_MIDO,
    /* 20 */ EC_UPDATE_20,
    /* 21 */ EC_UPDATE_CUCCO,
    /* 22 */ EC_UPDATE_CUCCO_LADY,
    /* 23 */ EC_UPDATE_POTION_SHOP_OWNER,
    /* 24 */ EC_UPDATE_MASK_SHOP_OWNER,
    /* 25 */ EC_UPDATE_FISHING_MAN,
    /* 26 */ EC_UPDATE_BOMBCHU_SHOP_OWNER,
    /* 27 */ EC_UPDATE_GORON,
    /* 28 */ EC_UPDATE_MALON
} DemoEcUpdateMode;

typedef enum {
    /* 00 */ EC_DRAW_COMMON,
    /* 01 */ EC_DRAW_INGO,
    /* 02 */ EC_DRAW_TALON,
    /* 03 */ EC_DRAW_WINDMILL_MAN,
    /* 04 */ EC_DRAW_KOKIRI_BOY,
    /* 05 */ EC_DRAW_KOKIRI_GIRL,
    /* 06 */ EC_DRAW_OLD_MAN,
    /* 07 */ EC_DRAW_BEARDED_MAN,
    /* 08 */ EC_DRAW_WOMAN,
    /* 09 */ EC_DRAW_OLD_WOMAN,
    /* 10 */ EC_DRAW_BOSS_CARPENTER,
    /* 11 */ EC_DRAW_CARPENTER,
    /* 12 */ EC_DRAW_GERUDO,
    /* 13 */ EC_DRAW_DANCING_ZORA,
    /* 14 */ EC_DRAW_KING_ZORA,
    /* 15 */ EC_DRAW_MIDO,
    /* 16 */ EC_DRAW_CUCCO,
    /* 17 */ EC_DRAW_CUCCO_LADY,
    /* 18 */ EC_DRAW_POTION_SHOP_OWNER,
    /* 19 */ EC_DRAW_MASK_SHOP_OWNER,
    /* 20 */ EC_DRAW_FISHING_MAN,
    /* 21 */ EC_DRAW_BOMBCHU_SHOP_OWNER,
    /* 22 */ EC_DRAW_GORON,
    /* 23 */ EC_DRAW_MALON
} DemoEcDrawconfig;

static s16 sDrawObjects[] = {
    /*  0 */ OBJECT_IN,
    /*  1 */ OBJECT_TA,
    /*  2 */ OBJECT_FU,
    /*  3 */ OBJECT_KM1,
    /*  4 */ OBJECT_KW1,
    /*  5 */ OBJECT_BJI,
    /*  6 */ OBJECT_AHG,
    /*  7 */ OBJECT_BOB,
    /*  8 */ OBJECT_BBA,
    /*  9 */ OBJECT_TORYO,
    /* 10 */ OBJECT_DAIKU,
    /* 11 */ OBJECT_DAIKU,
    /* 12 */ OBJECT_DAIKU,
    /* 13 */ OBJECT_DAIKU,
    /* 14 */ OBJECT_KM1,
    /* 15 */ OBJECT_KW1,
    /* 16 */ OBJECT_GE1,
    /* 17 */ OBJECT_GE1,
    /* 18 */ OBJECT_GE1,
    /* 19 */ OBJECT_ZO,
    /* 20 */ OBJECT_KZ,
    /* 21 */ OBJECT_MD,
    /* 22 */ OBJECT_NIW,
    /* 23 */ OBJECT_NIW,
    /* 24 */ OBJECT_NIW,
    /* 25 */ OBJECT_ANE,
    /* 26 */ OBJECT_DS2,
    /* 27 */ OBJECT_OS,
    /* 28 */ OBJECT_FISH,
    /* 29 */ OBJECT_RS,
    /* 30 */ OBJECT_OF1D_MAP,
    /* 31 */ OBJECT_OF1D_MAP,
    /* 32 */ OBJECT_OF1D_MAP,
    /* 33 */ OBJECT_OF1D_MAP,
    /* 34 */ OBJECT_MA2,
};

static s16 sAnimationObjects[] = {
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC,  OBJECT_EC,
    OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_EC, OBJECT_GM, OBJECT_MA2,
};

void DemoEc_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DemoEc* thisv = (DemoEc*)thisx;

    SkelAnime_Free(&thisv->skelAnime, globalCtx);
}

void DemoEc_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoEc* thisv = (DemoEc*)thisx;

    if ((thisv->actor.params < 0) || (thisv->actor.params > 34)) {
        osSyncPrintf(VT_FGCOL(RED) "Demo_Ec_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_Kill(&thisv->actor);
    } else {
        thisv->updateMode = EC_UPDATE_COMMON;
        thisv->drawConfig = EC_DRAW_COMMON;
    }
}

s32 DemoEc_UpdateSkelAnime(DemoEc* thisv) {
    return SkelAnime_Update(&thisv->skelAnime);
}

void DemoEc_UpdateBgFlags(DemoEc* thisv, GlobalContext* globalCtx) {
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 30.0f, 25.0f, 30.0f, 7);
}

void func_8096D594(DemoEc* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_8096D5D4(DemoEc* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.baseTransl = thisv->skelAnime.jointTable[0];
    thisv->skelAnime.prevTransl = thisv->skelAnime.jointTable[0];
    thisv->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void func_8096D64C(DemoEc* thisv, GlobalContext* globalCtx) {
    thisv->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &thisv->actor, &thisv->skelAnime, 1.0f);
}

void DemoEc_UpdateEyes(DemoEc* thisv) {
    s32 pad[3];
    s16* blinkTimer = &thisv->blinkTimer;
    s16* eyeTexIndex = &thisv->eyeTexIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeTexIndex = *blinkTimer;

    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void DemoEc_SetEyeTexIndex(DemoEc* thisv, s16 texIndex) {
    thisv->eyeTexIndex = texIndex;
}

void DemoEc_InitSkelAnime(DemoEc* thisv, GlobalContext* globalCtx, FlexSkeletonHeader* skeletonHeader) {
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, SEGMENTED_TO_VIRTUAL(skeletonHeader), NULL, NULL, NULL, 0);
}

void DemoEc_ChangeAnimation(DemoEc* thisv, AnimationHeader* animation, u8 mode, f32 transitionRate, s32 reverse) {
    f32 frameCount;
    f32 startFrame;
    AnimationHeader* anim;
    f32 playbackSpeed;
    s16 frameCountS;

    anim = SEGMENTED_TO_VIRTUAL(animation);
    frameCountS = Animation_GetLastFrame(anim);

    if (!reverse) {
        startFrame = 0.0f;
        frameCount = frameCountS;
        playbackSpeed = 1.0f;
    } else {
        frameCount = 0.0f;
        startFrame = frameCountS;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&thisv->skelAnime, anim, playbackSpeed, startFrame, frameCount, mode, transitionRate);
}

Gfx* DemoEc_AllocColorDList(GraphicsContext* gfxCtx, u8* color) {
    Gfx* dList;

    dList = static_cast<Gfx*>( Graph_Alloc(gfxCtx, sizeof(Gfx) * 2) );
    gDPSetEnvColor(dList, color[0], color[1], color[2], color[3]);
    gSPEndDisplayList(dList + 1);

    return dList;
}

void DemoEc_DrawSkeleton(DemoEc* thisv, GlobalContext* globalCtx, void* eyeTexture, void* arg3,
                         OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    SkelAnime* skelAnime = &thisv->skelAnime;
    s32 pad;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 565);

    func_80093D18(gfxCtx);

    if (eyeTexture != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    }

    if (arg3 != NULL) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(arg3));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, &thisv->actor, POLY_OPA_DISP);
    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 595);
}

void DemoEc_DrawSkeletonCustomColor(DemoEc* thisv, GlobalContext* globalCtx, Gfx* arg2, Gfx* arg3, u8* color1,
                                    u8* color2, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw) {
    s32 pad;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    SkelAnime* skelAnime = &thisv->skelAnime;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 609);

    func_80093D18(gfxCtx);

    if (arg2 != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(arg2));
    }

    if (arg3 != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x0B, SEGMENTED_TO_VIRTUAL(arg3));
    }

    if (color1 != NULL) {
        //! @bug DemoEc_AllocColorDList is called twice in SEGMENTED_TO_VIRTUAL, allocating two display lists
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(DemoEc_AllocColorDList(gfxCtx, color1)));
    }

    if (color2 != NULL) {
        //! @bug DemoEc_AllocColorDList is called twice in SEGMENTED_TO_VIRTUAL, allocating two display lists
        //! @bug meant to pass color2 instead of color1?
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(DemoEc_AllocColorDList(gfxCtx, color1)));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       overrideLimbDraw, postLimbDraw, &thisv->actor, POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 646);
}

void DemoEc_UseDrawObject(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    s32 drawObjBankIndex = thisv->drawObjBankIndex;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_ec.c", 662);

    gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[drawObjBankIndex].segment);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(globalCtx->objectCtx.status[drawObjBankIndex].segment);
    if (!globalCtx) {}

    CLOSE_DISPS(gfxCtx, "../z_demo_ec.c", 670);
}

void DemoEc_UseAnimationObject(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 animObjBankIndex = thisv->animObjBankIndex;

    gSegments[6] = reinterpret_cast<std::uintptr_t>( PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[animObjBankIndex].segment) );
}

CsCmdActorAction* DemoEc_GetNpcAction(GlobalContext* globalCtx, s32 actionIndex) {
    if (globalCtx->csCtx.state != CS_STATE_IDLE) {
        return globalCtx->csCtx.npcActions[actionIndex];
    } else {
        return NULL;
    }
}

void DemoEc_SetNpcActionPosRot(DemoEc* thisv, GlobalContext* globalCtx, s32 actionIndex) {
    CsCmdActorAction* npcAction = DemoEc_GetNpcAction(globalCtx, actionIndex);

    if (npcAction != NULL) {
        thisv->actor.world.pos.x = npcAction->startPos.x;
        thisv->actor.world.pos.y = npcAction->startPos.y;
        thisv->actor.world.pos.z = npcAction->startPos.z;

        thisv->actor.world.rot.y = thisv->actor.shape.rot.y = npcAction->rot.y;
    }
}

void DemoEc_InitIngo(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gIngoSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcIngoAnim, 0, 0.0f, false);
    func_8096D64C(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_INGO;
    thisv->drawConfig = EC_DRAW_INGO;
}

void DemoEc_UpdateIngo(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawIngo(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, gIngoEyeClosed2Tex, gIngoRedTex, 0, 0);
}

void DemoEc_InitTalon(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gTalonSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcTalonAnim, 0, 0.0f, false);
    func_8096D64C(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_TALON;
    thisv->drawConfig = EC_DRAW_TALON;
}

void DemoEc_UpdateTalon(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawTalon(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, gTalonEyeClosed2Tex, gTalonRedTex, NULL, NULL);
}

void DemoEc_InitWindmillMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gWindmillManSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcWindmillManAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_WINDMILL_MAN;
    thisv->drawConfig = EC_DRAW_WINDMILL_MAN;
}

void DemoEc_UpdateWindmillMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawWindmillMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, gWindmillManEyeClosedTex, gWindmillManMouthAngryTex, NULL, NULL);
}

void DemoEc_InitKokiriBoy(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gKm1Skel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcKokiriAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_KOKIRI_BOY;
    thisv->drawConfig = EC_DRAW_KOKIRI_BOY;
}

void DemoEc_InitDancingKokiriBoy(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gKm1Skel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcDancingKokiriAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_DANCING_KOKIRI_BOY;
    thisv->drawConfig = EC_DRAW_KOKIRI_BOY;
}

void DemoEc_UpdateKokiriBoy(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_UpdateDancingKokiriBoy(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateKokiriBoy(thisv, globalCtx);
}

void DemoEc_DrawKokiriBoy(DemoEc* thisv, GlobalContext* globalCtx) {
    static u8 color1[] = { 0, 130, 70, 255 };
    static u8 color2[] = { 110, 170, 20, 255 };

    DemoEc_DrawSkeletonCustomColor(thisv, globalCtx, NULL, NULL, color1, color2, NULL, NULL);
}

void DemoEc_InitKokiriGirl(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gKw1Skel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcKokiriAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_KOKIRI_GIRL;
    thisv->drawConfig = EC_DRAW_KOKIRI_GIRL;
}

void DemoEc_InitDancingKokiriGirl(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gKw1Skel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcDancingKokiriAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_DANCING_KOKIRI_GIRL;
    thisv->drawConfig = EC_DRAW_KOKIRI_GIRL;
}

void DemoEc_UpdateKokiriGirl(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_UpdateDancingKokiriGirl(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateKokiriGirl(thisv, globalCtx);
}

void DemoEc_DrawKokiriGirl(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gKw1EyeOpenTex, gKw1EyeHalfTex, gKw1EyeClosedTex };
    static u8 color1[] = { 70, 190, 60, 255 };
    static u8 color2[] = { 100, 30, 0, 255 };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeletonCustomColor(thisv, globalCtx, eyeTexture, NULL, color1, color2, NULL, NULL);
}
void DemoEc_InitOldMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_bji_Skel_0000F0);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_OLD_MAN;
    thisv->drawConfig = EC_DRAW_OLD_MAN;
}

void DemoEc_UpdateOldMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawOldMan(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        object_bji_Tex_0005FC,
        object_bji_Tex_0009FC,
        object_bji_Tex_000DFC,
    };
    static u8 color1[] = { 0, 50, 100, 255 };
    static u8 color2[] = { 0, 50, 160, 255 };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeletonCustomColor(thisv, globalCtx, eyeTexture, NULL, color1, color2, NULL, NULL);
}

void DemoEc_InitBeardedMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_ahg_Skel_0000F0);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_BEARDED_MAN;
    thisv->drawConfig = EC_DRAW_BEARDED_MAN;
}

void DemoEc_UpdateBeardedMan(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawBeardedMan(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        object_ahg_Tex_0005FC,
        object_ahg_Tex_0006FC,
        object_ahg_Tex_0007FC,
    };
    static u8 color1[] = { 255, 255, 255, 255 };
    static u8 color2[] = { 255, 255, 255, 255 };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeletonCustomColor(thisv, globalCtx, eyeTexture, NULL, color1, color2, NULL, NULL);
}

void DemoEc_InitWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_bob_Skel_0000F0);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_WOMAN;
    thisv->drawConfig = EC_DRAW_WOMAN;
}

void DemoEc_UpdateWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        object_bob_Tex_0007C8,
        object_bob_Tex_000FC8,
        object_bob_Tex_0017C8,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitOldWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_bba_Skel_0000F0);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcOldManAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_OLD_WOMAN;
    thisv->drawConfig = EC_DRAW_OLD_WOMAN;
}

void DemoEc_UpdateOldWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawOldWoman(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, &object_bba_Tex_0004C8, NULL, NULL, NULL);
}

void DemoEc_InitBossCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_toryo_Skel_007150);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcCarpenterAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_BOSS_CARPENTER;
    thisv->drawConfig = EC_DRAW_BOSS_CARPENTER;
}

void DemoEc_UpdateBossCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawBossCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, NULL, NULL, NULL, NULL);
}

void DemoEc_InitCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_daiku_Skel_007958);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcCarpenterAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_CARPENTER;
    thisv->drawConfig = EC_DRAW_CARPENTER;
}

void DemoEc_UpdateCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

s32 DemoEc_CarpenterOverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     void* thisx, Gfx** gfx) {
    DemoEc* thisv = (DemoEc*)thisx;

    if (limbIndex == 1) {
        gDPPipeSync((*gfx)++);
        switch (thisv->actor.params) {
            case 10:
                gDPSetEnvColor((*gfx)++, 170, 10, 70, 255);
                break;
            case 11:
                gDPSetEnvColor((*gfx)++, 170, 200, 255, 255);
                break;
            case 12:
                gDPSetEnvColor((*gfx)++, 0, 230, 70, 255);
                break;
            case 13:
                gDPSetEnvColor((*gfx)++, 200, 0, 150, 255);
                break;
        }
    }

    return false;
}

Gfx* DemoEc_GetCarpenterPostLimbDList(DemoEc* thisv) {
    switch (thisv->actor.params) {
        case 10:
            return object_daiku_DL_005BD0;
        case 11:
            return object_daiku_DL_005AC0;
        case 12:
            return object_daiku_DL_005990;
        case 13:
            return object_daiku_DL_005880;
        default:
            osSyncPrintf(VT_FGCOL(RED) "かつらが無い!!!!!!!!!!!!!!!!\n" VT_RST);
            return NULL;
    }
}

void DemoEc_CarpenterPostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                                  Gfx** gfx) {
    DemoEc* thisv = (DemoEc*)thisx;
    Gfx* postLimbDList;

    if (limbIndex == 15) {
        postLimbDList = DemoEc_GetCarpenterPostLimbDList(thisv);
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(postLimbDList));
    }
}

void DemoEc_DrawCarpenter(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, NULL, 0, DemoEc_CarpenterOverrideLimbDraw, DemoEc_CarpenterPostLimbDraw);
}

void DemoEc_InitGerudo(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gGerudoWhiteSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcGerudoAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_GERUDO;
    thisv->drawConfig = EC_DRAW_GERUDO;
}

void DemoEc_UpdateGerudo(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

Gfx* DemoEc_GetGerudoPostLimbDList(DemoEc* thisv) {
    switch (thisv->actor.params) {
        case 16:
            return gGerudoWhiteHairstyleBobDL;
        case 17:
            return gGerudoWhiteHairstyleStraightFringeDL;
        case 18:
            return gGerudoWhiteHairstyleSpikyDL;
        default:
            osSyncPrintf(VT_FGCOL(RED) "かつらが無い!!!!!!!!!!!!!!!!\n" VT_RST);
            return NULL;
    }
}

void DemoEc_GerudoPostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                               Gfx** gfx) {
    DemoEc* thisv = (DemoEc*)thisx;
    Gfx* postLimbDList;

    if (limbIndex == 15) {
        postLimbDList = DemoEc_GetGerudoPostLimbDList(thisv);
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(postLimbDList));
    }
}

void DemoEc_DrawGerudo(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gGerudoWhiteEyeOpenTex,
        gGerudoWhiteEyeHalfTex,
        gGerudoWhiteEyeClosedTex,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, DemoEc_GerudoPostLimbDraw);
}

void DemoEc_InitDancingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gZoraSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcDancingZoraAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_DANCING_ZORA;
    thisv->drawConfig = EC_DRAW_DANCING_ZORA;
}

void DemoEc_UpdateDancingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawDancingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitKingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gKzSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcKingZoraAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_KING_ZORA;
    thisv->drawConfig = EC_DRAW_KING_ZORA;
    DemoEc_SetEyeTexIndex(thisv, 3);
}

void func_8096F1D4(DemoEc* thisv) {
    f32 currentFrame = thisv->skelAnime.curFrame;

    if (currentFrame <= 32.0f) {
        DemoEc_SetEyeTexIndex(thisv, 3);
    } else {
        DemoEc_UpdateEyes(thisv);
    }
}

void func_8096F224(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcAnim_006930, 2, -8.0f, false);
    thisv->updateMode = EC_UPDATE_17;
}

void func_8096F26C(DemoEc* thisv, s32 arg1) {
    if (arg1 != 0) {
        DemoEc_ChangeAnimation(thisv, &gDemoEcAnim_006220, 0, 0.0f, false);
        thisv->updateMode = EC_UPDATE_18;
    }
}

void func_8096F2B0(DemoEc* thisv, GlobalContext* globalCtx, s32 arg2) {
    CsCmdActorAction* npcAction;
    s32 sp18;

    npcAction = DemoEc_GetNpcAction(globalCtx, arg2);

    if (npcAction != NULL) {
        sp18 = npcAction->action;
        if ((sp18 != thisv->npcAction)) {
            if (thisv->npcAction) {}
            if (sp18 == 2) {
                func_8096F224(thisv, globalCtx);
            }
            thisv->npcAction = sp18;
        }
    }
}

void DemoEc_UpdateKingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_SetNpcActionPosRot(thisv, globalCtx, 6);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
    func_8096F2B0(thisv, globalCtx, 6);
}

void func_8096F378(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 animDone = DemoEc_UpdateSkelAnime(thisv);

    func_8096D594(thisv, globalCtx);
    func_8096F1D4(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
    func_8096F26C(thisv, animDone);
}

void func_8096F3D4(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawKingZora(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gKzEyeOpenTex, gKzEyeHalfTex, gKzEyeClosedTex, gKzEyeOpen2Tex };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitMido(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gMidoSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcMidoAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_MIDO;
    thisv->drawConfig = EC_DRAW_MIDO;
    DemoEc_SetEyeTexIndex(thisv, 3);
}

void func_8096F4FC(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcAnim_008D1C, 2, -8.0f, false);
    thisv->updateMode = EC_UPDATE_20;
}

void func_8096F544(DemoEc* thisv, s32 changeAnim) {
    if (changeAnim) {
        DemoEc_ChangeAnimation(thisv, &gDemoEcAnim_009234, 0, 0.0f, false);
    }
}

void func_8096F578(DemoEc* thisv, GlobalContext* globalCtx, s32 arg2) {
    CsCmdActorAction* npcAction;
    s32 sp18;

    npcAction = DemoEc_GetNpcAction(globalCtx, arg2);
    if (npcAction != NULL) {
        sp18 = npcAction->action;
        if ((sp18 != thisv->npcAction)) {
            if (thisv->npcAction) {}
            if (sp18 == 2) {
                func_8096F4FC(thisv, globalCtx);
            }
            thisv->npcAction = sp18;
        }
    }
}

void DemoEc_UpdateMido(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_SetNpcActionPosRot(thisv, globalCtx, 7);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
    func_8096F578(thisv, globalCtx, 7);
}

void func_8096F640(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 animDone = DemoEc_UpdateSkelAnime(thisv);

    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
    func_8096F544(thisv, animDone);
}

void DemoEc_DrawMido(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gMidoEyeOpenTex,
        gMidoEyeHalfTex,
        gMidoEyeClosedTex,
        gMidoEyeAngryTex,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitCucco(DemoEc* thisv, GlobalContext* globalCtx) {
    AnimationHeader* animation;

    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gCuccoSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);

    if (thisv->actor.params == 22) {
        animation = &gDemoEcJumpingCuccoAnim;
    } else if (thisv->actor.params == 23) {
        animation = &gDemoEcJumpingCucco2Anim;
    } else {
        animation = &gDemoEcWalkingCuccoAnim;
    }

    DemoEc_ChangeAnimation(thisv, animation, 0, 0.0f, false);
    func_8096D64C(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_CUCCO;
    thisv->drawConfig = EC_DRAW_CUCCO;
}

void DemoEc_UpdateCucco(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawCucco(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, NULL, NULL, NULL, NULL);
}

void DemoEc_InitCuccoLady(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gCuccoLadySkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcCuccoLadyAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_CUCCO_LADY;
    thisv->drawConfig = EC_DRAW_CUCCO_LADY;
}

void DemoEc_UpdateCuccoLady(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawCuccoLady(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gCuccoLadyEyeOpenTex,
        gCuccoLadyEyeHalfTex,
        gCuccoLadyEyeClosedTex,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitPotionShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_ds2_Skel_004258);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_POTION_SHOP_OWNER;
    thisv->drawConfig = EC_DRAW_POTION_SHOP_OWNER;
}

void DemoEc_UpdatePotionShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawPotionShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gPotionShopkeeperEyeOpenTex,
        gPotionShopkeeperEyeHalfTex,
        gPotionShopkeeperEyeClosedTex,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitMaskShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_os_Skel_004658);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_MASK_SHOP_OWNER;
    thisv->drawConfig = EC_DRAW_MASK_SHOP_OWNER;
}

void DemoEc_UpdateMaskShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawMaskShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_DrawSkeleton(thisv, globalCtx, gOsEyeClosedTex, NULL, NULL, NULL);
}

void DemoEc_InitFishingOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gFishingOwnerSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_FISHING_MAN;
    thisv->drawConfig = EC_DRAW_FISHING_MAN;
}

void DemoEc_UpdateFishingOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_FishingOwnerPostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                                     Gfx** gfx) {
    DemoEc* thisv = (DemoEc*)thisx;

    if ((limbIndex == 8) && !(HIGH_SCORE(HS_FISHING) & 0x1000)) {
        gSPDisplayList((*gfx)++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHatDL));
    }
}

void DemoEc_DrawFishingOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = {
        gFishingOwnerEyeOpenTex,
        gFishingOwnerEyeHalfTex,
        gFishingOwnerEyeClosedTex,
    };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, DemoEc_FishingOwnerPostLimbDraw);
}

void DemoEc_InitBombchuShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &object_rs_Skel_004868);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gDemoEcPotionShopOwnerAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_BOMBCHU_SHOP_OWNER;
    thisv->drawConfig = EC_DRAW_BOMBCHU_SHOP_OWNER;
}

void DempEc_UpdateBombchuShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawBombchuShopOwner(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gBombchuShopkeeperEyeOpenTex, gBombchuShopkeeperEyeHalfTex,
                                   gBombchuShopkeeperEyeClosedTex };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, NULL, NULL, NULL);
}

void DemoEc_InitGorons(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 pad[2];
    AnimationHeader* animation;
    f32 goronScale;
    Vec3f* scale = &thisv->actor.scale;

    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gGoronSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);

    if (thisv->actor.params == 30) {
        animation = &gDemoEcGoronAnim;
        goronScale = 1.0f;
    } else if (thisv->actor.params == 31) {
        animation = &gDemoEcGoron2Anim;
        goronScale = 1.0f;
    } else if (thisv->actor.params == 32) {
        animation = &gDemoEcGoronAnim;
        goronScale = 15.0f;
    } else {
        goronScale = 5.0f;
        animation = &object_gm_Anim_0002B8;
    }

    DemoEc_ChangeAnimation(thisv, animation, 0, 0.0f, false);

    scale->x *= goronScale;
    scale->y *= goronScale;
    scale->z *= goronScale;

    func_8096D64C(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_GORON;
    thisv->drawConfig = EC_DRAW_GORON;
}

void DemoEc_UpdateGorons(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawGorons(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, gGoronCsMouthNeutralTex, NULL, NULL);
}

void DemoEc_InitMalon(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UseDrawObject(thisv, globalCtx);
    DemoEc_InitSkelAnime(thisv, globalCtx, &gMalonAdultSkel);
    DemoEc_UseAnimationObject(thisv, globalCtx);
    DemoEc_ChangeAnimation(thisv, &gMalonAdultSingAnim, 0, 0.0f, false);
    func_8096D5D4(thisv, globalCtx);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    thisv->updateMode = EC_UPDATE_MALON;
    thisv->drawConfig = EC_DRAW_MALON;
}

void DemoEc_UpdateMalon(DemoEc* thisv, GlobalContext* globalCtx) {
    DemoEc_UpdateSkelAnime(thisv);
    func_8096D594(thisv, globalCtx);
    DemoEc_UpdateEyes(thisv);
    DemoEc_UpdateBgFlags(thisv, globalCtx);
}

void DemoEc_DrawMalon(DemoEc* thisv, GlobalContext* globalCtx) {
    static void* eyeTextures[] = { gMalonAdultEyeOpenTex, gMalonAdultEyeHalfTex, gMalonAdultEyeClosedTex };
    s32 eyeTexIndex = thisv->eyeTexIndex;
    void* eyeTexture = eyeTextures[eyeTexIndex];

    DemoEc_DrawSkeleton(thisv, globalCtx, eyeTexture, gMalonAdultMouthHappyTex, NULL, NULL);
}

static DemoEcInitFunc sInitFuncs[] = {
    /*  0 */ DemoEc_InitIngo,
    /*  1 */ DemoEc_InitTalon,
    /*  2 */ DemoEc_InitWindmillMan,
    /*  3 */ DemoEc_InitKokiriBoy,
    /*  4 */ DemoEc_InitKokiriGirl,
    /*  5 */ DemoEc_InitOldMan,
    /*  6 */ DemoEc_InitBeardedMan,
    /*  7 */ DemoEc_InitWoman,
    /*  8 */ DemoEc_InitOldWoman,
    /*  9 */ DemoEc_InitBossCarpenter,
    /* 10 */ DemoEc_InitCarpenter,
    /* 11 */ DemoEc_InitCarpenter,
    /* 12 */ DemoEc_InitCarpenter,
    /* 13 */ DemoEc_InitCarpenter,
    /* 14 */ DemoEc_InitDancingKokiriBoy,
    /* 15 */ DemoEc_InitDancingKokiriGirl,
    /* 16 */ DemoEc_InitGerudo,
    /* 17 */ DemoEc_InitGerudo,
    /* 18 */ DemoEc_InitGerudo,
    /* 19 */ DemoEc_InitDancingZora,
    /* 20 */ DemoEc_InitKingZora,
    /* 21 */ DemoEc_InitMido,
    /* 22 */ DemoEc_InitCucco,
    /* 23 */ DemoEc_InitCucco,
    /* 24 */ DemoEc_InitCucco,
    /* 25 */ DemoEc_InitCuccoLady,
    /* 26 */ DemoEc_InitPotionShopOwner,
    /* 27 */ DemoEc_InitMaskShopOwner,
    /* 28 */ DemoEc_InitFishingOwner,
    /* 29 */ DemoEc_InitBombchuShopOwner,
    /* 30 */ DemoEc_InitGorons,
    /* 31 */ DemoEc_InitGorons,
    /* 32 */ DemoEc_InitGorons,
    /* 33 */ DemoEc_InitGorons,
    /* 34 */ DemoEc_InitMalon,
};

void DemoEc_InitNpc(DemoEc* thisv, GlobalContext* globalCtx) {
    s16 type = thisv->actor.params;

    if (sInitFuncs[type] == NULL) {
        // "Demo_Ec_main_init: Initialization process is wrong arg_data"
        osSyncPrintf(VT_FGCOL(RED) " Demo_Ec_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, type);
        Actor_Kill(&thisv->actor);
        return;
    }

    sInitFuncs[type](thisv, globalCtx);
}

void DemoEc_InitCommon(DemoEc* thisv, GlobalContext* globalCtx) {
    s32 pad;
    s16 primary;
    s32 type;
    s16 pad2;
    s16 sp28;
    s32 primaryBankIndex;
    s32 secondaryBankIndex;

    type = thisv->actor.params;
    primary = sDrawObjects[type];
    sp28 = sAnimationObjects[type];
    primaryBankIndex = Object_GetIndex(&globalCtx->objectCtx, primary);
    secondaryBankIndex = Object_GetIndex(&globalCtx->objectCtx, sp28);

    if ((secondaryBankIndex < 0) || (primaryBankIndex < 0)) {
        // "Demo_Ec_main_bank: Bank unreadable arg_data = %d!"
        osSyncPrintf(VT_FGCOL(RED) "Demo_Ec_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, type);
        Actor_Kill(&thisv->actor);
        return;
    }

    if (Object_IsLoaded(&globalCtx->objectCtx, primaryBankIndex) &&
        Object_IsLoaded(&globalCtx->objectCtx, secondaryBankIndex)) {

        thisv->drawObjBankIndex = primaryBankIndex;
        thisv->animObjBankIndex = secondaryBankIndex;

        DemoEc_InitNpc(thisv, globalCtx);
    }
}

static DemoEcUpdateFunc sUpdateFuncs[] = {
    DemoEc_InitCommon,
    DemoEc_UpdateIngo,
    DemoEc_UpdateTalon,
    DemoEc_UpdateWindmillMan,
    DemoEc_UpdateKokiriBoy,
    DemoEc_UpdateKokiriGirl,
    DemoEc_UpdateOldMan,
    DemoEc_UpdateBeardedMan,
    DemoEc_UpdateWoman,
    DemoEc_UpdateOldWoman,
    DemoEc_UpdateBossCarpenter,
    DemoEc_UpdateCarpenter,
    DemoEc_UpdateDancingKokiriBoy,
    DemoEc_UpdateDancingKokiriGirl,
    DemoEc_UpdateGerudo,
    DemoEc_UpdateDancingZora,
    DemoEc_UpdateKingZora,
    func_8096F378,
    func_8096F3D4,
    DemoEc_UpdateMido,
    func_8096F640,
    DemoEc_UpdateCucco,
    DemoEc_UpdateCuccoLady,
    DemoEc_UpdatePotionShopOwner,
    DemoEc_UpdateMaskShopOwner,
    DemoEc_UpdateFishingOwner,
    DempEc_UpdateBombchuShopOwner,
    DemoEc_UpdateGorons,
    DemoEc_UpdateMalon,
};

void DemoEc_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoEc* thisv = (DemoEc*)thisx;
    s32 updateMode = thisv->updateMode;

    if ((updateMode < 0) || (updateMode >= ARRAY_COUNT(sUpdateFuncs)) || sUpdateFuncs[updateMode] == NULL) {
        // "The main mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        if (updateMode != EC_UPDATE_COMMON) {
            DemoEc_UseAnimationObject(thisv, globalCtx);
        }
        sUpdateFuncs[updateMode](thisv, globalCtx);
    }
}

void DemoEc_DrawCommon(DemoEc* thisv, GlobalContext* globalCtx) {
}

static DemoEcDrawFunc sDrawFuncs[] = {
    DemoEc_DrawCommon,          DemoEc_DrawIngo,
    DemoEc_DrawTalon,           DemoEc_DrawWindmillMan,
    DemoEc_DrawKokiriBoy,       DemoEc_DrawKokiriGirl,
    DemoEc_DrawOldMan,          DemoEc_DrawBeardedMan,
    DemoEc_DrawWoman,           DemoEc_DrawOldWoman,
    DemoEc_DrawBossCarpenter,   DemoEc_DrawCarpenter,
    DemoEc_DrawGerudo,          DemoEc_DrawDancingZora,
    DemoEc_DrawKingZora,        DemoEc_DrawMido,
    DemoEc_DrawCucco,           DemoEc_DrawCuccoLady,
    DemoEc_DrawPotionShopOwner, DemoEc_DrawMaskShopOwner,
    DemoEc_DrawFishingOwner,    DemoEc_DrawBombchuShopOwner,
    DemoEc_DrawGorons,          DemoEc_DrawMalon,
};

void DemoEc_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoEc* thisv = (DemoEc*)thisx;
    s32 drawConfig = thisv->drawConfig;

    if ((drawConfig < 0) || (drawConfig >= ARRAY_COUNT(sDrawFuncs)) || sDrawFuncs[drawConfig] == NULL) {
        // "The main mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        if (drawConfig != EC_DRAW_COMMON) {
            DemoEc_UseDrawObject(thisv, globalCtx);
        }
        sDrawFuncs[drawConfig](thisv, globalCtx);
    }
}

const ActorInit Demo_Ec_InitVars = {
    ACTOR_DEMO_EC,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_EC,
    sizeof(DemoEc),
    (ActorFunc)DemoEc_Init,
    (ActorFunc)DemoEc_Destroy,
    (ActorFunc)DemoEc_Update,
    (ActorFunc)DemoEc_Draw,
    NULL,
};
