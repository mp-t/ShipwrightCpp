/*
 * File: z_en_poh.c
 * Overlay: ovl_En_Poh
 * Description: Graveyard Poe
 */

#include "z_en_poh.h"
#include "objects/object_poh/object_poh.h"
#include "objects/object_po_composer/object_po_composer.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_12)

void EnPoh_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_Update(Actor* thisx, GlobalContext* globalCtx);

void EnPoh_UpdateLiving(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_UpdateDead(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_DrawRegular(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_DrawComposer(Actor* thisx, GlobalContext* globalCtx);
void EnPoh_DrawSoul(Actor* thisx, GlobalContext* globalCtx);

void func_80ADEAC4(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_Idle(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADEC9C(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_Attack(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADEECC(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADF894(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_ComposerAppear(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADEF38(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADF15C(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADF574(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADF5E0(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_Disappear(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_Appear(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_Death(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADFE28(EnPoh* thisv, GlobalContext* globalCtx);
void func_80ADFE80(EnPoh* thisv, GlobalContext* globalCtx);
void func_80AE009C(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_TalkRegular(EnPoh* thisv, GlobalContext* globalCtx);
void EnPoh_TalkComposer(EnPoh* thisv, GlobalContext* globalCtx);

static s16 D_80AE1A50 = 0;

ActorInit En_Poh_InitVars = {
    ACTOR_EN_POH,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnPoh),
    (ActorFunc)EnPoh_Init,
    (ActorFunc)EnPoh_Destroy,
    (ActorFunc)EnPoh_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
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
    { 20, 40, 20, { 0, 0, 0 } },
};

static ColliderJntSphElementInit D_80AE1AA0[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 18, { { 0, 1400, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    D_80AE1AA0,
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 25, 50, 40 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static EnPohInfo sPoeInfo[2] = {
    {
        { 255, 170, 255 },
        { 100, 0, 150 },
        18,
        5,
        248,
        &gPoeDisappearAnim,
        &gPoeFloatAnim,
        &gPoeDamagedAnim,
        &gPoeFleeAnim,
        gPoeLanternDL,
        gPoeBurnDL,
        gPoeSoulDL,
    },
    {
        { 255, 255, 170 },
        { 0, 150, 0 },
        9,
        1,
        244,
        &gPoeComposerDisappearAnim,
        &gPoeComposerFloatAnim,
        &gPoeComposerDamagedAnim,
        &gPoeComposerFleeAnim,
        gPoeComposerLanternDL,
        gPoeComposerBurnDL,
        gPoeComposerSoulDL,
    },
};

static Color_RGBA8 D_80AE1B4C = { 75, 20, 25, 255 };
static Color_RGBA8 D_80AE1B50 = { 80, 110, 90, 255 };
static Color_RGBA8 D_80AE1B54 = { 90, 85, 50, 255 };
static Color_RGBA8 D_80AE1B58 = { 100, 90, 100, 255 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f D_80AE1B60 = { 0.0f, 3.0f, 0.0f };
static Vec3f D_80AE1B6C = { 0.0f, 0.0f, 0.0f };

void EnPoh_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnItem00* collectible;
    EnPoh* thisv = (EnPoh*)thisx;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    Collider_InitJntSph(globalCtx, &thisv->colliderSph);
    Collider_SetJntSph(globalCtx, &thisv->colliderSph, &thisv->actor, &sJntSphInit, &thisv->colliderSphItem);
    thisv->colliderSph.elements[0].dim.worldSphere.radius = 0;
    thisv->colliderSph.elements[0].dim.worldSphere.center.x = thisv->actor.world.pos.x;
    thisv->colliderSph.elements[0].dim.worldSphere.center.y = thisv->actor.world.pos.y;
    thisv->colliderSph.elements[0].dim.worldSphere.center.z = thisv->actor.world.pos.z;
    Collider_InitCylinder(globalCtx, &thisv->colliderCyl);
    Collider_SetCylinder(globalCtx, &thisv->colliderCyl, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->unk_194 = 0;
    thisv->unk_195 = 32;
    thisv->visibilityTimer = Rand_S16Offset(700, 300);
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z,
                            255, 255, 255, 0);
    if (thisv->actor.params >= 4) {
        thisv->actor.params = EN_POH_NORMAL;
    }
    if (thisv->actor.params == EN_POH_RUPEE) {
        D_80AE1A50++;
        if (D_80AE1A50 >= 3) {
            Actor_Kill(&thisv->actor);
        } else {
            collectible = Item_DropCollectible(globalCtx, &thisv->actor.world.pos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (collectible != NULL) {
                collectible->actor.speedXZ = 0.0f;
            }
        }
    } else if (thisv->actor.params == EN_POH_FLAT) {
        if (Flags_GetSwitch(globalCtx, 0x28) || Flags_GetSwitch(globalCtx, 0x9)) {
            Actor_Kill(&thisv->actor);
        } else {
            Flags_SetSwitch(globalCtx, 0x28);
        }
    } else if (thisv->actor.params == EN_POH_SHARP) {
        if (Flags_GetSwitch(globalCtx, 0x29) || Flags_GetSwitch(globalCtx, 0x9)) {
            Actor_Kill(&thisv->actor);
        } else {
            Flags_SetSwitch(globalCtx, 0x29);
        }
    }
    if (thisv->actor.params < EN_POH_SHARP) {
        thisv->objectIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_POH);
        thisv->infoIdx = EN_POH_INFO_NORMAL;
        thisv->actor.naviEnemyId = 0x44;
    } else {
        thisv->objectIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_PO_COMPOSER);
        thisv->infoIdx = EN_POH_INFO_COMPOSER;
        thisv->actor.naviEnemyId = 0x43;
    }
    thisv->info = &sPoeInfo[thisv->infoIdx];
    if (thisv->objectIdx < 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnPoh_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    Collider_DestroyJntSph(globalCtx, &thisv->colliderSph);
    Collider_DestroyCylinder(globalCtx, &thisv->colliderCyl);
    if (thisv->actor.params == EN_POH_RUPEE) {
        D_80AE1A50--;
    }
}

void func_80ADE114(EnPoh* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, thisv->info->idleAnim);
    thisv->unk_198 = Rand_S16Offset(2, 3);
    thisv->actionFunc = func_80ADEAC4;
    thisv->actor.speedXZ = 0.0f;
}

void EnPoh_SetupIdle(EnPoh* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, thisv->info->idleAnim2);
    thisv->unk_198 = Rand_S16Offset(15, 3);
    thisv->actionFunc = EnPoh_Idle;
}

void func_80ADE1BC(EnPoh* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, thisv->info->idleAnim2);
    thisv->actionFunc = func_80ADEC9C;
    thisv->unk_198 = 0;
    thisv->actor.speedXZ = 2.0f;
}

void EnPoh_SetupAttack(EnPoh* thisv) {
    if (thisv->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_MorphToLoop(&thisv->skelAnime, &gPoeAttackAnim, -6.0f);
    } else {
        Animation_PlayLoop(&thisv->skelAnime, &gPoeComposerAttackAnim);
    }
    thisv->unk_198 = 12;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    thisv->actionFunc = EnPoh_Attack;
}

void func_80ADE28C(EnPoh* thisv) {
    if (thisv->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_MorphToPlayOnce(&thisv->skelAnime, &gPoeDamagedAnim, -6.0f);
    } else {
        Animation_PlayOnce(&thisv->skelAnime, &gPoeComposerDamagedAnim);
    }
    if (thisv->colliderCyl.info.acHitInfo->toucher.dmgFlags & 0x0001F824) {
        thisv->actor.world.rot.y = thisv->colliderCyl.base.ac->world.rot.y;
    } else {
        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, thisv->colliderCyl.base.ac) + 0x8000;
    }
    thisv->colliderCyl.base.acFlags &= ~AC_ON;
    thisv->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0x10);
    thisv->actionFunc = func_80ADEECC;
}

void func_80ADE368(EnPoh* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, thisv->info->fleeAnim, -5.0f);
    thisv->actor.speedXZ = 5.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
    thisv->colliderCyl.base.acFlags |= AC_ON;
    thisv->unk_198 = 200;
    thisv->actionFunc = func_80ADF894;
}

void EnPoh_SetupInitialAction(EnPoh* thisv) {
    thisv->lightColor.a = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    if (thisv->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gPoeAppearAnim, 0.0f);
        thisv->actionFunc = func_80ADEF38;
    } else {
        Animation_PlayOnceSetSpeed(&thisv->skelAnime, &gPoeComposerAppearAnim, 1.0f);
        thisv->actor.world.pos.y = thisv->actor.home.pos.y + 20.0f;
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
        thisv->actionFunc = EnPoh_ComposerAppear;
    }
}

void func_80ADE48C(EnPoh* thisv) {
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->unk_198 = 0;
    thisv->actor.naviEnemyId = 0xFF;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = func_80ADF15C;
}

void func_80ADE4C8(EnPoh* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, thisv->info->idleAnim2);
    thisv->actionFunc = func_80ADF574;
    thisv->actor.speedXZ = -5.0f;
}

void func_80ADE514(EnPoh* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, thisv->info->idleAnim);
    thisv->unk_19C = thisv->actor.world.rot.y + 0x8000;
    thisv->actionFunc = func_80ADF5E0;
    thisv->actor.speedXZ = 0.0f;
}

void EnPoh_SetupDisappear(EnPoh* thisv) {
    thisv->unk_194 = 32;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DISAPPEAR);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    thisv->actionFunc = EnPoh_Disappear;
}

void EnPoh_SetupAppear(EnPoh* thisv) {
    thisv->unk_194 = 0;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    thisv->actionFunc = EnPoh_Appear;
}

void EnPoh_SetupDeath(EnPoh* thisv, GlobalContext* globalCtx) {
    thisv->actor.update = EnPoh_UpdateDead;
    thisv->actor.draw = EnPoh_DrawSoul;
    thisv->actor.shape.shadowDraw = NULL;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.flags |= ACTOR_FLAG_4;
    thisv->actor.gravity = -1.0f;
    thisv->actor.shape.yOffset = 1500.0f;
    thisv->actor.world.pos.y -= 15.0f;
    if (thisv->infoIdx != EN_POH_INFO_COMPOSER) {
        thisv->actor.shape.rot.x = -0x8000;
    }
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, 8);
    thisv->unk_198 = 60;
    thisv->actionFunc = EnPoh_Death;
}

void func_80ADE6D4(EnPoh* thisv) {
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, 0, 0, 0, 0);
    thisv->visibilityTimer = 0;
    thisv->actor.shape.rot.y = 0;
    thisv->lightColor.r = 0;
    thisv->lightColor.a = 0;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    if (thisv->actor.params >= EN_POH_SHARP) {
        thisv->lightColor.g = 200;
        thisv->lightColor.b = 0;
    } else {
        thisv->lightColor.g = 0;
        thisv->lightColor.b = 200;
    }
    thisv->actor.scale.x = 0.0f;
    thisv->actor.scale.y = 0.0f;
    thisv->actor.shape.rot.x = 0;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_METAL_BOX_BOUND);
    thisv->actionFunc = func_80ADFE28;
}

void EnPoh_Talk(EnPoh* thisv, GlobalContext* globalCtx) {
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    Actor_SetFocus(&thisv->actor, -10.0f);
    thisv->colliderCyl.dim.radius = 13;
    thisv->colliderCyl.dim.height = 30;
    thisv->colliderCyl.dim.yShift = 0;
    thisv->colliderCyl.dim.pos.x = thisv->actor.world.pos.x;
    thisv->colliderCyl.dim.pos.y = thisv->actor.world.pos.y - 20.0f;
    thisv->colliderCyl.dim.pos.z = thisv->actor.world.pos.z;
    thisv->colliderCyl.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    if (thisv->actor.params == EN_POH_FLAT || thisv->actor.params == EN_POH_SHARP) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            thisv->actor.textId = 0x5000;
        } else if (!Flags_GetSwitch(globalCtx, 0xA) && !Flags_GetSwitch(globalCtx, 0xB)) {
            thisv->actor.textId = 0x500F;
        } else if ((thisv->actor.params == EN_POH_FLAT && Flags_GetSwitch(globalCtx, 0xA)) ||
                   (thisv->actor.params == EN_POH_SHARP && Flags_GetSwitch(globalCtx, 0xB))) {
            thisv->actor.textId = 0x5013;
        } else {
            thisv->actor.textId = 0x5012;
        }
    } else {
        thisv->actor.textId = 0x5005;
    }
    thisv->unk_198 = 200;
    thisv->unk_195 = 32;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = func_80ADFE80;
}

void func_80ADE950(EnPoh* thisv, s32 arg1) {
    if (arg1) {
        Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    }
    thisv->actionFunc = func_80AE009C;
}

void func_80ADE998(EnPoh* thisv) {
    thisv->actionFunc = EnPoh_TalkRegular;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y - 15.0f;
}

void func_80ADE9BC(EnPoh* thisv) {
    thisv->actionFunc = EnPoh_TalkComposer;
}

void EnPoh_MoveTowardsPlayerHeight(EnPoh* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_StepToF(&thisv->actor.world.pos.y, player->actor.world.pos.y, 1.0f);
    thisv->actor.world.pos.y += 2.5f * Math_SinS(thisv->unk_195 * 0x800);
    if (thisv->unk_195 != 0) {
        thisv->unk_195 -= 1;
    }
    if (thisv->unk_195 == 0) {
        thisv->unk_195 = 32;
    }
}

void func_80ADEA5C(EnPoh* thisv) {
    if (Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) > 400.0f) {
        thisv->unk_19C = Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos);
    }
    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->unk_19C, 0x71C);
}

void func_80ADEAC4(EnPoh* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_198 != 0) {
        thisv->unk_198--;
    }
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    if (thisv->actor.xzDistToPlayer < 200.0f) {
        func_80ADE1BC(thisv);
    } else if (thisv->unk_198 == 0) {
        EnPoh_SetupIdle(thisv);
    }
    if (thisv->lightColor.a == 255) {
        func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void EnPoh_Idle(EnPoh* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.speedXZ, 1.0f, 0.2f);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_198 != 0) {
        thisv->unk_198--;
    }
    func_80ADEA5C(thisv);
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    if (thisv->actor.xzDistToPlayer < 200.0f && thisv->unk_198 < 19) {
        func_80ADE1BC(thisv);
    } else if (thisv->unk_198 == 0) {
        if (Rand_ZeroOne() < 0.1f) {
            func_80ADE514(thisv);
        } else {
            func_80ADE114(thisv);
        }
    }
    if (thisv->lightColor.a == 255) {
        func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void func_80ADEC9C(EnPoh* thisv, GlobalContext* globalCtx) {
    Player* player;
    s16 facingDiff;

    player = GET_PLAYER(globalCtx);
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_198 != 0) {
        thisv->unk_198--;
    }
    facingDiff = thisv->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    if (facingDiff >= 0x3001) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (facingDiff < -0x3000) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0x71C);
    }
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    if (thisv->actor.xzDistToPlayer > 280.0f) {
        EnPoh_SetupIdle(thisv);
    } else if (thisv->unk_198 == 0 && thisv->actor.xzDistToPlayer < 140.0f &&
               !Player_IsFacingActor(&thisv->actor, 0x2AAA, globalCtx)) {
        EnPoh_SetupAttack(thisv);
    }
    if (thisv->lightColor.a == 255) {
        func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void EnPoh_Attack(EnPoh* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_KANTERA);
        if (thisv->unk_198 != 0) {
            thisv->unk_198--;
        }
    }
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    if (thisv->unk_198 >= 10) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0xE38);
    } else if (thisv->unk_198 == 9) {
        thisv->actor.speedXZ = 5.0f;
        thisv->skelAnime.playSpeed = 2.0f;
    } else if (thisv->unk_198 == 0) {
        EnPoh_SetupIdle(thisv);
        thisv->unk_198 = 23;
    }
}

void func_80ADEECC(EnPoh* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.5f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.colChkInfo.health != 0) {
            func_80ADE368(thisv);
        } else {
            func_80ADE48C(thisv);
        }
    }
}

void func_80ADEF38(EnPoh* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->lightColor.a = 255;
        thisv->visibilityTimer = Rand_S16Offset(700, 300);
        thisv->actor.flags |= ACTOR_FLAG_0;
        EnPoh_SetupIdle(thisv);
    } else if (thisv->skelAnime.curFrame > 10.0f) {
        thisv->lightColor.a = ((thisv->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    }
    if (thisv->skelAnime.playSpeed < 0.5f && thisv->actor.xzDistToPlayer < 280.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
        thisv->skelAnime.playSpeed = 1.0f;
    }
}

void EnPoh_ComposerAppear(EnPoh* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->lightColor.a = 255;
        thisv->visibilityTimer = Rand_S16Offset(700, 300);
        thisv->actor.flags |= ACTOR_FLAG_0;
        EnPoh_SetupIdle(thisv);
    } else {
        thisv->lightColor.a = CLAMP_MAX((s32)(thisv->skelAnime.curFrame * 25.5f), 255);
    }
}

void func_80ADF15C(EnPoh* thisv, GlobalContext* globalCtx) {
    Vec3f vec;
    f32 multiplier;
    f32 newScale;
    s32 pad;
    s32 pad1;

    thisv->unk_198++;
    if (thisv->unk_198 < 8) {
        if (thisv->unk_198 < 5) {
            vec.y = Math_SinS((thisv->unk_198 * 0x1000) - 0x4000) * 23.0f + (thisv->actor.world.pos.y + 40.0f);
            multiplier = Math_CosS((thisv->unk_198 * 0x1000) - 0x4000) * 23.0f;
            vec.x = Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * multiplier +
                    thisv->actor.world.pos.x;
            vec.z = Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * multiplier +
                    thisv->actor.world.pos.z;
        } else {
            vec.y = (thisv->actor.world.pos.y + 40.0f) + (15.0f * (thisv->unk_198 - 5));
            vec.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f + thisv->actor.world.pos.x;
            vec.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f + thisv->actor.world.pos.z;
        }
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AE1B60, &D_80AE1B6C, thisv->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        vec.x = (thisv->actor.world.pos.x + thisv->actor.world.pos.x) - vec.x;
        vec.z = (thisv->actor.world.pos.z + thisv->actor.world.pos.z) - vec.z;
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AE1B60, &D_80AE1B6C, thisv->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        vec.x = thisv->actor.world.pos.x;
        vec.z = thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &vec, &D_80AE1B60, &D_80AE1B6C, thisv->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        if (thisv->unk_198 == 1) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EXTINCT);
        }
    } else if (thisv->unk_198 == 28) {
        EnPoh_SetupDeath(thisv, globalCtx);
    } else if (thisv->unk_198 >= 19) {
        newScale = (28 - thisv->unk_198) * 0.001f;
        thisv->actor.world.pos.y += 5.0f;
        thisv->actor.scale.z = newScale;
        thisv->actor.scale.y = newScale;
        thisv->actor.scale.x = newScale;
    }
    if (thisv->unk_198 == 18) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD2);
    }
}

void func_80ADF574(EnPoh* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        EnPoh_SetupIdle(thisv);
        thisv->unk_198 = 23;
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.5f);
        thisv->actor.shape.rot.y += 0x1000;
    }
}

void func_80ADF5E0(EnPoh* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->unk_19C, 1820) != 0) {
        EnPoh_SetupIdle(thisv);
    }
    if (thisv->actor.xzDistToPlayer < 200.0f) {
        func_80ADE1BC(thisv);
    }
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
}

void EnPoh_Disappear(EnPoh* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }
    thisv->actor.world.rot.y += 0x1000;
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    thisv->lightColor.a = thisv->unk_194 * 7.96875f;
    if (thisv->unk_194 == 0) {
        thisv->visibilityTimer = Rand_S16Offset(100, 50);
        EnPoh_SetupIdle(thisv);
    }
}

void EnPoh_Appear(EnPoh* thisv, GlobalContext* globalCtx) {
    thisv->unk_194++;
    thisv->actor.world.rot.y -= 0x1000;
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    thisv->lightColor.a = thisv->unk_194 * 7.96875f;
    if (thisv->unk_194 == 32) {
        thisv->visibilityTimer = Rand_S16Offset(700, 300);
        thisv->unk_194 = 0;
        EnPoh_SetupIdle(thisv);
    }
}

void func_80ADF894(EnPoh* thisv, GlobalContext* globalCtx) {
    f32 multiplier;

    SkelAnime_Update(&thisv->skelAnime);
    multiplier = Math_SinS(thisv->unk_195 * 0x800) * 3.0f;
    thisv->actor.world.pos.x -= multiplier * Math_CosS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.z += multiplier * Math_SinS(thisv->actor.shape.rot.y);
    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 0x71C);
    EnPoh_MoveTowardsPlayerHeight(thisv, globalCtx);
    if (thisv->unk_198 == 0 || thisv->actor.xzDistToPlayer > 250.0f) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        EnPoh_SetupIdle(thisv);
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void EnPoh_Death(EnPoh* thisv, GlobalContext* globalCtx) {
    s32 objId;

    if (thisv->unk_198 != 0) {
        thisv->unk_198--;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        objId = (thisv->infoIdx == EN_POH_INFO_COMPOSER) ? OBJECT_PO_COMPOSER : OBJECT_POH;
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 6.0f, 0, 1, 1, 15, objId, 10,
                                 thisv->info->lanternDisplayList);
        func_80ADE6D4(thisv);
    } else if (thisv->unk_198 == 0) {
        Actor_Kill(&thisv->actor);
        return;
    }
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 10.0f, 4);
}

void func_80ADFA90(EnPoh* thisv, s32 arg1) {
    f32 multiplier;

    thisv->lightColor.a = CLAMP(thisv->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = thisv->lightColor.a * (1.0f / 255);
        thisv->actor.scale.x = thisv->actor.scale.z = 0.0056000002f * multiplier + 0.0014000001f;
        thisv->actor.scale.y = (0.007f - 0.007f * multiplier) + 0.007f;
    } else {
        multiplier = 1.0f;
        thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z = thisv->lightColor.a * (0.007f / 0xFF);
        thisv->actor.world.pos.y = thisv->actor.home.pos.y + (1.0f / 17.0f) * thisv->lightColor.a;
    }
    thisv->lightColor.r = thisv->info->lightColor.r * multiplier;
    thisv->lightColor.g = thisv->info->lightColor.g * multiplier;
    thisv->lightColor.b = thisv->info->lightColor.b * multiplier;
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, thisv->info->lightColor.r, thisv->info->lightColor.g,
                              thisv->info->lightColor.b, thisv->lightColor.a * (200.0f / 255));
}

void func_80ADFE28(EnPoh* thisv, GlobalContext* globalCtx) {
    thisv->actor.home.pos.y += 2.0f;
    func_80ADFA90(thisv, 20);
    if (thisv->lightColor.a == 255) {
        EnPoh_Talk(thisv, globalCtx);
    }
}

void func_80ADFE80(EnPoh* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_198 != 0) {
        thisv->unk_198--;
    }
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        if (thisv->actor.params >= EN_POH_SHARP) {
            func_80ADE9BC(thisv);
        } else {
            func_80ADE998(thisv);
        }
        return;
    }
    if (thisv->unk_198 == 0) {
        func_80ADE950(thisv, 1);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        return;
    }
    if (thisv->colliderCyl.base.ocFlags1 & OC1_HIT) {
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F2F4(&thisv->actor, globalCtx);
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCyl.base);
    }
    thisv->actor.world.pos.y = Math_SinS(thisv->unk_195 * 0x800) * 5.0f + thisv->actor.home.pos.y;
    if (thisv->unk_195 != 0) {
        thisv->unk_195 -= 1;
    }
    if (thisv->unk_195 == 0) {
        thisv->unk_195 = 32;
    }
    thisv->colliderCyl.dim.pos.y = thisv->actor.world.pos.y - 20.0f;
    Actor_SetFocus(&thisv->actor, -10.0f);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, thisv->info->lightColor.r, thisv->info->lightColor.g,
                              thisv->info->lightColor.b, thisv->lightColor.a * (200.0f / 255));
}

void func_80AE009C(EnPoh* thisv, GlobalContext* globalCtx) {
    func_80ADFA90(thisv, -13);
    if (thisv->lightColor.a == 0) {
        Actor_Kill(&thisv->actor);
    }
}

void EnPoh_TalkRegular(EnPoh* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.textId != 0x5005) {
        func_80ADFA90(thisv, -13);
    } else {
        func_8002F974(&thisv->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(globalCtx)) {
            Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (globalCtx->msgCtx.choiceIndex == 0) {
                if (Inventory_HasEmptyBottle()) {
                    thisv->actor.textId = 0x5008;
                    Item_Give(globalCtx, ITEM_POE);
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_BIG_GET);
                } else {
                    thisv->actor.textId = 0x5006;
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
                }
            } else {
                thisv->actor.textId = 0x5007;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
            }
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
        }
    } else if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        func_80ADE950(thisv, 0);
    }
}

void EnPoh_TalkComposer(EnPoh* thisv, GlobalContext* globalCtx) {
    func_8002F974(&thisv->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(globalCtx)) {
            if (globalCtx->msgCtx.choiceIndex == 0) {
                if (!Flags_GetSwitch(globalCtx, 0xB) && !Flags_GetSwitch(globalCtx, 0xA)) {
                    thisv->actor.textId = 0x5010;
                } else {
                    thisv->actor.textId = 0x5014;
                }
                Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            } else {
                if (thisv->actor.params == EN_POH_SHARP) {
                    Flags_SetSwitch(globalCtx, 0xB);
                } else {
                    Flags_SetSwitch(globalCtx, 0xA);
                }
                func_80ADE950(thisv, 1);
            }
        }
    } else if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        if (thisv->actor.textId == 0x5000) {
            Flags_SetSwitch(globalCtx, 9);
        }
        func_80ADE950(thisv, 1);
    }
}

void func_80AE032C(EnPoh* thisv, GlobalContext* globalCtx) {
    if (thisv->colliderCyl.base.acFlags & AC_HIT) {
        thisv->colliderCyl.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != 0 || thisv->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DAMAGE);
            }
            func_80ADE28C(thisv);
        }
    }
}

void EnPoh_UpdateVisibility(EnPoh* thisv) {
    if (thisv->actionFunc != EnPoh_Appear && thisv->actionFunc != EnPoh_Disappear && thisv->actionFunc != func_80ADEF38 &&
        thisv->actionFunc != EnPoh_ComposerAppear) {
        if (thisv->visibilityTimer != 0) {
            thisv->visibilityTimer--;
        }
        if (thisv->lightColor.a == 255) {
            if (thisv->actor.isTargeted) {
                thisv->unk_194++;
                thisv->unk_194 = CLAMP_MAX(thisv->unk_194, 20);
            } else {
                thisv->unk_194 = 0;
            }
            if ((thisv->unk_194 == 20 || thisv->visibilityTimer == 0) &&
                (thisv->actionFunc == func_80ADEAC4 || thisv->actionFunc == EnPoh_Idle ||
                 thisv->actionFunc == func_80ADEC9C || thisv->actionFunc == func_80ADF894 ||
                 thisv->actionFunc == func_80ADF5E0)) {
                EnPoh_SetupDisappear(thisv);
            }
        } else if (thisv->lightColor.a == 0 && thisv->visibilityTimer == 0 &&
                   (thisv->actionFunc == func_80ADEAC4 || thisv->actionFunc == EnPoh_Idle ||
                    thisv->actionFunc == func_80ADEC9C || thisv->actionFunc == func_80ADF5E0)) {
            EnPoh_SetupAppear(thisv);
        }
    }
}

void EnPoh_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;

    if (Object_IsLoaded(&globalCtx->objectCtx, thisv->objectIdx)) {
        thisv->actor.objBankIndex = thisv->objectIdx;
        thisv->actor.update = EnPoh_UpdateLiving;
        Actor_SetObjectDependency(globalCtx, &thisv->actor);
        if (thisv->infoIdx == EN_POH_INFO_NORMAL) {
            SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPoeSkel, &gPoeFloatAnim, thisv->jointTable, thisv->morphTable,
                           21);
            thisv->actor.draw = EnPoh_DrawRegular;
        } else {
            SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &gPoeComposerSkel, &gPoeComposerFloatAnim, thisv->jointTable,
                               thisv->morphTable, 12);
            thisv->actor.draw = EnPoh_DrawComposer;
            thisv->colliderSph.elements[0].dim.limb = 9;
            thisv->colliderSph.elements[0].dim.modelSphere.center.y *= -1;
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y = -0x4000;
            thisv->colliderCyl.dim.radius = 20;
            thisv->colliderCyl.dim.height = 55;
            thisv->colliderCyl.dim.yShift = 15;
        }
        thisv->actor.flags &= ~ACTOR_FLAG_4;
        EnPoh_SetupInitialAction(thisv);
    }
}

void func_80AE067C(EnPoh* thisv) {
    s16 temp_var;

    if (thisv->actionFunc == EnPoh_Attack) {
        thisv->lightColor.r = CLAMP_MAX((s16)(thisv->lightColor.r + 5), 255);
        thisv->lightColor.g = CLAMP_MIN((s16)(thisv->lightColor.g - 5), 50);
        temp_var = thisv->lightColor.b - 5;
        thisv->lightColor.b = CLAMP_MIN(temp_var, 0);
    } else if (thisv->actionFunc == func_80ADF894) {
        thisv->lightColor.r = CLAMP_MAX((s16)(thisv->lightColor.r + 5), 80);
        thisv->lightColor.g = CLAMP_MAX((s16)(thisv->lightColor.g + 5), 255);
        temp_var = thisv->lightColor.b + 5;
        thisv->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (thisv->actionFunc == func_80ADEECC) {
        if (thisv->actor.colorFilterTimer & 2) {
            thisv->lightColor.r = 0;
            thisv->lightColor.g = 0;
            thisv->lightColor.b = 0;
        } else {
            thisv->lightColor.r = 80;
            thisv->lightColor.g = 255;
            thisv->lightColor.b = 225;
        }
    } else {
        thisv->lightColor.r = CLAMP_MAX((s16)(thisv->lightColor.r + 5), 255);
        thisv->lightColor.g = CLAMP_MAX((s16)(thisv->lightColor.g + 5), 255);
        if (thisv->lightColor.b >= 211) {
            temp_var = thisv->lightColor.b - 5;
            thisv->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = thisv->lightColor.b + 5;
            thisv->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void func_80AE089C(EnPoh* thisv) {
    f32 rand;

    if ((thisv->actionFunc == func_80ADEF38 || thisv->actionFunc == EnPoh_ComposerAppear) &&
        thisv->skelAnime.curFrame < 12.0f) {
        thisv->envColor.r = thisv->envColor.g = thisv->envColor.b = (s16)(thisv->skelAnime.curFrame * 16.66f) + 55;
        thisv->envColor.a = thisv->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = Rand_ZeroOne();
        thisv->envColor.r = (s16)(rand * 30.0f) + 225;
        thisv->envColor.g = (s16)(rand * 100.0f) + 155;
        thisv->envColor.b = (s16)(rand * 160.0f) + 95;
        thisv->envColor.a = 200;
    }
}

void EnPoh_UpdateLiving(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;
    s32 pad;
    Vec3f vec;
    s32 sp38;

    if (thisv->colliderSph.base.atFlags & AT_HIT) {
        thisv->colliderSph.base.atFlags &= ~AT_HIT;
        func_80ADE4C8(thisv);
    }
    func_80AE032C(thisv, globalCtx);
    EnPoh_UpdateVisibility(thisv);
    thisv->actionFunc(thisv, globalCtx);
    Actor_MoveForward(&thisv->actor);
    if (thisv->actionFunc == EnPoh_Attack && thisv->unk_198 < 10) {
        thisv->actor.flags |= ACTOR_FLAG_24;
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSph.base);
    }
    Collider_UpdateCylinder(&thisv->actor, &thisv->colliderCyl);
    if ((thisv->colliderCyl.base.acFlags & AC_ON) && thisv->lightColor.a == 255) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCyl.base);
    }
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderCyl.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->colliderSph.base);
    Actor_SetFocus(&thisv->actor, 42.0f);
    if (thisv->actionFunc != func_80ADEECC && thisv->actionFunc != func_80ADF574) {
        if (thisv->actionFunc == func_80ADF894) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
        } else {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        }
    }
    vec.x = thisv->actor.world.pos.x;
    vec.y = thisv->actor.world.pos.y + 20.0f;
    vec.z = thisv->actor.world.pos.z;
    thisv->actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &sp38, &thisv->actor, &vec);
    func_80AE089C(thisv);
    thisv->actor.shape.shadowAlpha = thisv->lightColor.a;
}

s32 EnPoh_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfxP) {
    EnPoh* thisv = (EnPoh*)thisx;

    if ((thisv->lightColor.a == 0 || limbIndex == thisv->info->unk_6) ||
        (thisv->actionFunc == func_80ADF15C && thisv->unk_198 >= 2)) {
        *dList = NULL;
    } else if (thisv->actor.params == EN_POH_FLAT && limbIndex == 0xA) {
        // Replace Sharp's head with Flat's
        *dList = gPoeComposerFlatHeadDL;
    }
    if (limbIndex == 0x13 && thisv->infoIdx == EN_POH_INFO_NORMAL) {
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, thisv->lightColor.a);
    }
    return false;
}

void EnPoh_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoh* thisv = (EnPoh*)thisx;

    Collider_UpdateSpheres(limbIndex, &thisv->colliderSph);
    if (thisv->actionFunc == func_80ADF15C && thisv->unk_198 >= 2 && limbIndex == thisv->info->unk_7) {
        gSPMatrix((*gfxP)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_poh.c", 2460),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, thisv->info->burnDisplayList);
    }
    if (limbIndex == thisv->info->unk_6) {
        if (thisv->actionFunc == func_80ADF15C && thisv->unk_198 >= 19 && 0.0f != thisv->actor.scale.x) {
            f32 mtxScale = 0.01f / thisv->actor.scale.x;
            Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_Get(&thisv->unk_368);
        if (thisv->actionFunc == func_80ADF15C && thisv->unk_198 == 27) {
            thisv->actor.world.pos.x = thisv->unk_368.mf_raw.xw;
            thisv->actor.world.pos.y = thisv->unk_368.mf_raw.yw;
            thisv->actor.world.pos.z = thisv->unk_368.mf_raw.zw;
        }
        Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->colliderSph.elements[0].dim.worldSphere.center.x,
                                thisv->colliderSph.elements[0].dim.worldSphere.center.y,
                                thisv->colliderSph.elements[0].dim.worldSphere.center.z, thisv->envColor.r,
                                thisv->envColor.g, thisv->envColor.b, thisv->envColor.a * (200.0f / 255));
    }
}

void EnPoh_DrawRegular(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2629);
    func_80AE067C(thisv);
    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);
    if (thisv->lightColor.a == 255 || thisv->lightColor.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, thisv->lightColor.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, D_80116280 + 2);
        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw, &thisv->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->lightColor.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, D_80116280);
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw, &thisv->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, thisv->envColor.r, thisv->envColor.g, thisv->envColor.b, 255);
    Matrix_Put(&thisv->unk_368);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_poh.c", 2676),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, thisv->info->lanternDisplayList);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2681);
}

void EnPoh_DrawComposer(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;
    Color_RGBA8* sp90;
    Color_RGBA8* phi_t0;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2694);
    func_80AE067C(thisv);
    if (thisv->actor.params == EN_POH_SHARP) {
        sp90 = &D_80AE1B4C;
        phi_t0 = &D_80AE1B54;
    } else {
        sp90 = &D_80AE1B50;
        phi_t0 = &D_80AE1B58;
    }
    if (thisv->lightColor.a == 255 || thisv->lightColor.a == 0) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                                thisv->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, sp90->r, sp90->g, sp90->b, thisv->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, thisv->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0C, D_80116280 + 2);
        POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw,
                                           &thisv->actor, POLY_OPA_DISP);
    } else {
        func_80093D18(globalCtx->state.gfxCtx);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                                thisv->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, sp90->r, sp90->g, sp90->b, thisv->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0B,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, thisv->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw,
                                           &thisv->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, thisv->envColor.r, thisv->envColor.g, thisv->envColor.b, 255);
    Matrix_Put(&thisv->unk_368);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_poh.c", 2787),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, thisv->info->lanternDisplayList);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp90->r, sp90->g, sp90->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2802);
}

void EnPoh_UpdateDead(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc != EnPoh_Death) {
        thisv->visibilityTimer++;
    }
    func_80AE089C(thisv);
}

void EnPoh_DrawSoul(Actor* thisx, GlobalContext* globalCtx) {
    EnPoh* thisv = (EnPoh*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2833);

    if (thisv->actionFunc == EnPoh_Death) {
        func_80093D18(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->envColor.r, thisv->envColor.g, thisv->envColor.b, 255);
        Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                thisv->actor.world.pos.z, thisv->envColor.r, thisv->envColor.g, thisv->envColor.b, 200);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_poh.c", 2854),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, thisv->info->lanternDisplayList);
        if (thisv->infoIdx == EN_POH_INFO_COMPOSER) {
            Color_RGBA8* envColor = (thisv->actor.params == EN_POH_SHARP) ? &D_80AE1B4C : &D_80AE1B50;
            s32 pad;

            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, envColor->r, envColor->g, envColor->b, 255);
            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
        }
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (thisv->visibilityTimer * thisv->info->unk_8) % 512U, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, thisv->info->primColor.r, thisv->info->primColor.g,
                        thisv->info->primColor.b, thisv->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, 255);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_poh.c", 2910),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, thisv->info->soulDisplayList);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_poh.c", 2916);
}
