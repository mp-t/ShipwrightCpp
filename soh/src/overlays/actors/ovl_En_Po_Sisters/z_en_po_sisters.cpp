/*
 * File: z_en_po_sisters.c
 * Overlay: ovl_En_Po_Sisters
 * Description: Forest Temple Four Poe Sisters
 */

#include "z_en_po_sisters.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_po_sisters/object_po_sisters.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_9 | ACTOR_FLAG_12 | ACTOR_FLAG_14)

void EnPoSisters_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPoSisters_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPoSisters_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPoSisters_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnPoSisters_Reset(void);

void func_80ADA094(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA4A8(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA530(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA6A0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA7F0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA8C0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADA9E8(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADAAA4(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADAC70(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADAD54(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADAE6C(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADAFC0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB17C(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB2B8(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB338(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB9F0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB4B0(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB51C(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADB770(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBB6C(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBBF4(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBC88(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBD38(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBD8C(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBEE8(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80ADBF58(EnPoSisters* thisv, GlobalContext* globalCtx);

void func_80AD9AA8(EnPoSisters* thisv, GlobalContext* globalCtx);
void func_80AD9C24(EnPoSisters* thisv, GlobalContext* globalCtx);

void func_80AD9D44(EnPoSisters* thisv);

static Color_RGBA8 D_80ADD6F0[4] = {
    { 255, 170, 255, 255 },
    { 255, 200, 0, 255 },
    { 0, 170, 255, 255 },
    { 170, 255, 0, 255 },
};

static Color_RGBA8 D_80ADD700[4] = {
    { 100, 0, 255, 255 },
    { 255, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 0, 150, 0, 255 },
};

const ActorInit En_Po_Sisters_InitVars = {
    ACTOR_EN_PO_SISTERS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PO_SISTERS,
    sizeof(EnPoSisters),
    (ActorFunc)EnPoSisters_Init,
    (ActorFunc)EnPoSisters_Destroy,
    (ActorFunc)EnPoSisters_Update,
    (ActorFunc)EnPoSisters_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x4FC7FFEA, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 18, 60, 15, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 25, 60, 40 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
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
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 D_80ADD784 = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 6000, ICHAIN_STOP),
};

static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };

static s16 D_80ADD79C[4] = { 0xB000, 0xD000, 0x5000, 0x3000 };

static Vec3s D_80ADD7A4[4] = {
    { -22, 337, -1704 },
    { -431, 879, -3410 },
    { 549, 879, -3410 },
    { 1717, 515, -1340 },
};

static Vec3f D_80ADD7BC = { 120.0f, 250.0f, -1420.0f };

static Gfx* D_80ADD7C8[4] = {
    gPoeSistersMegBodyDL,
    gPoeSistersJoelleBodyDL,
    gPoeSistersBethBodyDL,
    gPoeSistersAmyBodyDL,
};

static Gfx* D_80ADD7D8[4] = {
    gPoeSistersMegFaceDL,
    gPoeSistersJoelleFaceDL,
    gPoeSistersBethFaceDL,
    gPoSistersAmyFaceDL,
};

static Color_RGBA8 D_80ADD7E8[4] = {
    { 80, 0, 100, 0 },
    { 80, 15, 0, 0 },
    { 0, 70, 50, 0 },
    { 70, 70, 0, 0 },
};

static Vec3f D_80ADD7F8 = { 1000.0f, -1700.0f, 0.0f };

void EnPoSisters_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnPoSisters* thisv = (EnPoSisters*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPoeSistersSkel, &gPoeSistersSwayAnim, thisv->jointTable,
                   thisv->morphTable, 12);
    thisv->unk_22E.r = 255;
    thisv->unk_22E.g = 255;
    thisv->unk_22E.b = 210;
    thisv->unk_22E.a = 255;
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z, 0,
                            0, 0, 0);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->unk_194 = (thisx->params >> 8) & 3;
    thisv->actor.naviEnemyId = thisv->unk_194 + 0x50;
    if (1) {}
    thisv->unk_195 = (thisx->params >> 0xA) & 3;
    thisv->unk_196 = 32;
    thisv->unk_197 = 20;
    thisv->unk_198 = 1;
    thisv->unk_199 = 32;
    thisv->unk_294 = 110.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    if (thisv->actor.params & 0x1000) {
        func_80ADA094(thisv, globalCtx);
    } else if (thisv->unk_194 == 0) {
        if (thisv->unk_195 == 0) {
            thisv->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
            func_80AD9AA8(thisv, globalCtx);
        } else {
            thisv->actor.flags &= ~(ACTOR_FLAG_9 | ACTOR_FLAG_14);
            thisv->collider.info.elemType = ELEMTYPE_UNK4;
            thisv->collider.info.bumper.dmgFlags |= 1;
            thisv->collider.base.ocFlags1 = OC1_NONE;
            func_80AD9C24(thisv, NULL);
        }
    } else {
        func_80AD9D44(thisv);
    }
    thisv->actor.params &= 0x3F;
}

void EnPoSisters_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPoSisters* thisv = (EnPoSisters*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
    if (thisv->unk_194 == 0 && thisv->unk_195 == 0) {
        func_800F5B58();
    }
    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80AD9240(EnPoSisters* thisv, s32 arg1, Vec3f* arg2) {
    f32 temp_f20 = SQ(arg1) * 0.1f;
    Vec3f* vec;
    s32 i;

    for (i = 0; i < thisv->unk_198; i++) {
        vec = &thisv->unk_234[i];
        vec->x = arg2->x + Math_SinS((s16)(thisv->actor.shape.rot.y + (thisv->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->z = arg2->z + Math_CosS((s16)(thisv->actor.shape.rot.y + (thisv->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->y = arg2->y + arg1;
    }
}

void func_80AD9368(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    thisv->unk_19A = Rand_S16Offset(2, 3);
    thisv->actionFunc = func_80ADA4A8;
    thisv->actor.speedXZ = 0.0f;
}

void func_80AD93C4(EnPoSisters* thisv) {
    if (thisv->actionFunc != func_80ADA6A0) {
        Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    }
    thisv->unk_19A = Rand_S16Offset(0xF, 3);
    thisv->unk_199 |= 7;
    thisv->actionFunc = func_80ADA530;
}

void func_80AD943C(EnPoSisters* thisv) {
    thisv->actionFunc = func_80ADA6A0;
}

void func_80AD944C(EnPoSisters* thisv) {
    if (thisv->unk_22E.a != 0) {
        thisv->collider.base.colType = COLTYPE_METAL;
        thisv->collider.base.acFlags |= AC_HARD;
    }
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_19A = Animation_GetLastFrame(&gPoeSistersAttackAnim) * 3 + 3;
    thisv->unk_199 &= ~2;
    thisv->actionFunc = func_80ADA7F0;
}

void func_80AD94E0(EnPoSisters* thisv) {
    thisv->actor.speedXZ = 5.0f;
    if (thisv->unk_194 == 0) {
        thisv->collider.base.colType = COLTYPE_METAL;
        thisv->collider.base.acFlags |= AC_HARD;
        Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    }
    thisv->unk_19A = 5;
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->unk_199 |= 8;
    thisv->actionFunc = func_80ADA8C0;
}

void func_80AD9568(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer + 0x8000;
    if (thisv->unk_194 != 0) {
        thisv->collider.base.colType = COLTYPE_HIT3;
        thisv->collider.base.acFlags &= ~AC_HARD;
    }
    thisv->actionFunc = func_80ADA9E8;
}

void func_80AD95D8(EnPoSisters* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPoeSistersDamagedAnim, -3.0f);
    if (thisv->collider.base.ac != NULL) {
        thisv->actor.world.rot.y = (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824)
                                      ? thisv->collider.base.ac->world.rot.y
                                      : Actor_WorldYawTowardActor(&thisv->actor, thisv->collider.base.ac) + 0x8000;
    }
    if (thisv->unk_194 != 0) {
        thisv->actor.speedXZ = 10.0f;
    }
    thisv->unk_199 &= ~0xB;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0x10);
    thisv->actionFunc = func_80ADAAA4;
}

void func_80AD96A4(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFleeAnim, -3.0f);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
    thisv->unk_19A = 5;
    thisv->unk_199 |= 0xB;
    thisv->actor.speedXZ = 5.0f;
    thisv->actionFunc = func_80ADAC70;
}

void func_80AD9718(EnPoSisters* thisv) {
    Animation_Change(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    thisv->actor.speedXZ = 0.0f;
    thisv->unk_19C = 100;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->unk_199 &= ~5;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DISAPPEAR);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
    thisv->actionFunc = func_80ADAD54;
}

void func_80AD97C8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 sp20;

    if (thisv->unk_195 == 0 || thisv->actionFunc != func_80ADAAA4) {
        if ((player->swordState == 0 || player->swordAnimation >= 24) &&
            player->actor.world.pos.y - player->actor.floorHeight < 1.0f) {
            Math_StepToF(&thisv->unk_294, 110.0f, 3.0f);
        } else {
            Math_StepToF(&thisv->unk_294, 170.0f, 10.0f);
        }
        sp20 = thisv->unk_294;
    } else if (thisv->unk_195 != 0) {
        sp20 = thisv->actor.parent->xzDistToPlayer;
    }
    thisv->actor.world.pos.x = (Math_SinS(thisv->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.x;
    thisv->actor.world.pos.z = (Math_CosS(thisv->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.z;
}

void func_80AD98F4(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Animation_Change(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    if (thisv->unk_194 == 0) {
        thisv->unk_294 = 110.0f;
        func_80AD97C8(thisv, globalCtx);
        thisv->unk_22E.a = 0;
        thisv->actor.draw = EnPoSisters_Draw;
    } else {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    }
    thisv->unk_19A = 15;
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
    thisv->unk_199 &= ~1;
    thisv->actionFunc = func_80ADAE6C;
}

void func_80AD99D4(EnPoSisters* thisv, GlobalContext* globalCtx) {
    thisv->unk_19A = 0;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.pos.y += 42.0f;
    thisv->actor.shape.yOffset = -6000.0f;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->unk_199 = 0;
    thisv->actionFunc = func_80ADAFC0;
    OnePointCutscene_Init(globalCtx, 3190, 999, &thisv->actor, MAIN_CAM);
}

void func_80AD9A54(EnPoSisters* thisv, GlobalContext* globalCtx) {
    thisv->unk_19A = 0;
    thisv->actor.world.pos.y = thisv->unk_234[0].y;
    Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x80);
    thisv->actionFunc = func_80ADB17C;
}

// Meg spawning fakes
void func_80AD9AA8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Actor* actor1 = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0x400);
    Actor* actor2 = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0x800);
    Actor* actor3 = Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisv->actor.world.pos.x,
                                thisv->actor.world.pos.y, thisv->actor.world.pos.z, 0, 0, 0, 0xC00);
    s32 pad;
    s32 pad1;

    if (actor1 == NULL || actor2 == NULL || actor3 == NULL) {
        if (actor1 != NULL) {
            Actor_Kill(actor1);
        }
        if (actor2 != NULL) {
            Actor_Kill(actor2);
        }
        if (actor3 != NULL) {
            Actor_Kill(actor3);
        }
        Actor_Kill(&thisv->actor);
    } else {
        actor3->parent = &thisv->actor;
        actor2->parent = &thisv->actor;
        actor1->parent = &thisv->actor;
        Animation_PlayLoop(&thisv->skelAnime, &gPoeSistersMegCryAnim);
        thisv->unk_198 = 0;
        thisv->unk_199 = 160;
        thisv->actionFunc = func_80ADB2B8;
    }
}

void func_80AD9C24(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Vec3f vec;

    thisv->actor.draw = NULL;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->unk_19C = 100;
    thisv->unk_199 = 32;
    thisv->collider.base.colType = COLTYPE_HIT3;
    thisv->collider.base.acFlags &= ~AC_HARD;
    if (globalCtx != NULL) {
        vec.x = thisv->actor.world.pos.x;
        vec.y = thisv->actor.world.pos.y + 45.0f;
        vec.z = thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &vec, &sZeroVector, &sZeroVector, 150, 0, 255, 255, 255, 155, 150, 150, 150, 1,
                             9, 0);
    }
    Lights_PointSetColorAndRadius(&thisv->lightInfo, 0, 0, 0, 0);
    thisv->actionFunc = func_80ADB338;
}

void func_80AD9D44(EnPoSisters* thisv) {
    if (thisv->unk_194 == 3) {
        Animation_PlayOnce(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
    } else {
        Animation_Change(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    }
    thisv->unk_22E.a = 0;
    thisv->unk_199 = 32;
    thisv->actionFunc = func_80ADB9F0;
}

void func_80AD9DF0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim, -5.0f);
    thisv->unk_198 = 1;
    thisv->unk_199 &= ~0x80;
    thisv->actionFunc = func_80ADB4B0;
    OnePointCutscene_Init(globalCtx, 3180, 156, &thisv->actor, MAIN_CAM);
}

void func_80AD9E60(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    thisv->unk_19A = Animation_GetLastFrame(&gPoeSistersFloatAnim) * 7 + 7;
    if (thisv->actor.parent != NULL) {
        thisv->actor.world.pos = thisv->actor.parent->world.pos;
        thisv->actor.shape.rot.y = thisv->actor.parent->shape.rot.y;
    } else {
        thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
        thisv->unk_19A++;
    }
    if (thisv->unk_195 == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
    }
    thisv->actionFunc = func_80ADB51C;
}

void func_80AD9F1C(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    thisv->unk_22E.a = 255;
    thisv->unk_19A = 300;
    thisv->unk_19C = 3;
    thisv->unk_199 |= 9;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = func_80ADB770;
}

void func_80AD9F90(EnPoSisters* thisv) {
    if (thisv->unk_194 == 1) {
        thisv->actor.home.pos.x = -632.0f;
        thisv->actor.home.pos.z = -3440.0f;
    } else {
        thisv->actor.home.pos.x = 752.0f;
        thisv->actor.home.pos.z = -3440.0f;
    }
    Animation_PlayLoop(&thisv->skelAnime, &gPoeSistersFloatAnim);
    thisv->unk_199 |= 0xA;
    thisv->actionFunc = func_80ADBB6C;
    thisv->actor.speedXZ = 5.0f;
}

void func_80ADA028(EnPoSisters* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    thisv->unk_22E.a = 255;
    thisv->unk_199 |= 0x15;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = func_80ADBBF4;
    thisv->actor.speedXZ = 0.0f;
}

void func_80ADA094(EnPoSisters* thisv, GlobalContext* globalCtx) {
    D_80ADD784 = 0;
    thisv->unk_22E.a = 0;
    thisv->unk_199 = 128;
    thisv->unk_19A = 50;
    thisv->unk_234[0] = thisv->actor.home.pos;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
    thisv->actionFunc = func_80ADBC88;
}

void func_80ADA10C(EnPoSisters* thisv) {
    s32 i;

    thisv->unk_198 = ARRAY_COUNT(thisv->unk_234);
    for (i = 0; i < ARRAY_COUNT(thisv->unk_234); i++) {
        thisv->unk_234[i] = thisv->unk_234[0];
    }
    thisv->actionFunc = func_80ADBD38;
}

void func_80ADA1B8(EnPoSisters* thisv) {
    Animation_Change(&thisv->skelAnime, &gPoeSistersAppearDisappearAnim, 0.833f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    if (thisv->unk_194 == 0 || thisv->unk_194 == 1) {
        thisv->unk_19A = 40;
    } else {
        thisv->unk_19A = 76;
    }
    thisv->unk_198 = 0;
    D_80ADD784 = 0;
    thisv->actionFunc = func_80ADBD8C;
}

void func_80ADA25C(EnPoSisters* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gPoeSistersSwayAnim);
    thisv->unk_198 = 8;
    thisv->unk_19A = 32;
    func_80AD9240(thisv, thisv->unk_19A, &thisv->actor.home.pos);
    thisv->actionFunc = func_80ADBEE8;
}

void func_80ADA2BC(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    thisv->unk_198 = 0;
    thisv->unk_199 = 40;
    thisv->unk_19A = 90;
    thisv->unk_196 = 32;
    thisv->actor.world.rot.y = D_80ADD79C[thisv->unk_194];
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    if (thisv->unk_194 == 0) {
        Flags_SetSwitch(globalCtx, 0x1B);
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_FLAME_IGNITION);
    thisv->actionFunc = func_80ADBF58;
}

void func_80ADA35C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    f32 targetY;
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->actionFunc == func_80ADBF58) {
        targetY = thisv->actor.home.pos.y;
    } else if (thisv->unk_194 == 0 || thisv->unk_194 == 3) {
        targetY = player->actor.world.pos.y + 5.0f;
    } else {
        targetY = 832.0f;
    }
    Math_ApproachF(&thisv->actor.world.pos.y, targetY, 0.5f, 3.0f);
    if (!thisv->unk_196) {
        thisv->unk_196 = 32;
    }
    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }
    thisv->actor.world.pos.y += (2.0f + 0.5f * Rand_ZeroOne()) * Math_SinS(thisv->unk_196 * 0x800);
    if (thisv->unk_22E.a == 255 && thisv->actionFunc != func_80ADA8C0 && thisv->actionFunc != func_80ADA7F0) {
        if (thisv->actionFunc == func_80ADAC70) {
            func_8002F974(&thisv->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
        } else {
            func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
        }
    }
}

void func_80ADA4A8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    if (thisv->unk_19A == 0 || thisv->actor.xzDistToPlayer < 200.0f) {
        func_80AD93C4(thisv);
    }
}

void func_80ADA530(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_StepToF(&thisv->actor.speedXZ, 1.0f, 0.2f);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    if (thisv->actor.xzDistToPlayer < 200.0f && fabsf(thisv->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD943C(thisv);
    } else if (thisv->unk_19A == 0 && Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.2f) != 0) {
        func_80AD9368(thisv);
    }
    if (thisv->actor.bgCheckFlags & 8) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos),
                           0x71C);
    } else if (Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) > 300.0f) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos),
                           0x71C);
    }
}

void func_80ADA6A0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 temp_v0;

    SkelAnime_Update(&thisv->skelAnime);
    temp_v0 = thisv->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    Math_StepToF(&thisv->actor.speedXZ, 2.0f, 0.2f);
    if (temp_v0 > 0x3000) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (temp_v0 < -0x3000) {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 0x71C);
    }
    if (thisv->actor.xzDistToPlayer < 160.0f && fabsf(thisv->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD944C(thisv);
    } else if (thisv->actor.xzDistToPlayer > 240.0f) {
        func_80AD93C4(thisv);
    }
}

void func_80ADA7F0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    thisv->actor.shape.rot.y += 384.0f * ((thisv->skelAnime.endFrame + 1.0f) * 3.0f - thisv->unk_19A);
    if (thisv->unk_19A == 18 || thisv->unk_19A == 7) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_ROLL);
    }
    if (thisv->unk_19A == 0) {
        func_80AD94E0(thisv);
    }
}

void func_80ADA8C0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    s32 pad;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    thisv->actor.shape.rot.y += (384.0f * thisv->skelAnime.endFrame) * 3.0f;
    if (thisv->unk_19A == 0 && ABS((s16)(thisv->actor.shape.rot.y - thisv->actor.world.rot.y)) < 0x1000) {
        if (thisv->unk_194 != 0) {
            thisv->collider.base.colType = COLTYPE_HIT3;
            thisv->collider.base.acFlags &= ~AC_HARD;
            func_80AD93C4(thisv);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(thisv, globalCtx);
        }
    }
    if (Animation_OnFrame(&thisv->skelAnime, 1.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_ROLL);
    }
}

void func_80ADA9E8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    thisv->actor.shape.rot.y -= (thisv->actor.speedXZ * 10.0f) * 128.0f;
    if (Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.1f) != 0) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        if (thisv->unk_194 != 0) {
            func_80AD93C4(thisv);
        } else {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(thisv, globalCtx);
        }
    }
}

void func_80ADAAA4(EnPoSisters* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime) && !(thisv->actor.flags & ACTOR_FLAG_15)) {
        if (thisv->actor.colChkInfo.health != 0) {
            if (thisv->unk_194 != 0) {
                func_80AD96A4(thisv);
            } else if (thisv->unk_195 != 0) {
                func_80AD9C24(thisv, NULL);
            } else {
                func_80AD9C24(thisv, globalCtx);
            }
        } else {
            func_80AD99D4(thisv, globalCtx);
        }
    }
    if (thisv->unk_195 != 0) {
        Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.parent->shape.rot.y,
                           (thisv->unk_195 == 2) ? 0x800 : 0x400);
        thisv->unk_22E.a = ((thisv->skelAnime.endFrame - thisv->skelAnime.curFrame) * 255.0f) / thisv->skelAnime.endFrame;
        thisv->actor.world.pos.y = thisv->actor.parent->world.pos.y;
        func_80AD97C8(thisv, globalCtx);
    } else if (thisv->unk_194 != 0) {
        Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.5f);
    }
}

void func_80ADAC70(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer + 0x8000, 1820);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f) && thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    if (thisv->actor.bgCheckFlags & 8) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        thisv->unk_199 |= 2;
        func_80AD9718(thisv);
    } else if (thisv->unk_19A == 0 && 240.0f < thisv->actor.xzDistToPlayer) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
        func_80AD93C4(thisv);
    }
}

void func_80ADAD54(EnPoSisters* thisv, GlobalContext* globalCtx) {
    s32 endFrame;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_22E.a = 0;
        thisv->collider.info.bumper.dmgFlags = 0x00060001;
        func_80AD93C4(thisv);
    } else {
        endFrame = thisv->skelAnime.endFrame;
        thisv->unk_22E.a = ((endFrame - thisv->skelAnime.curFrame) * 255.0f) / endFrame;
    }
}

void func_80ADAE6C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_22E.a = 255;
        if (thisv->unk_194 != 0) {
            thisv->unk_199 |= 1;
            thisv->collider.info.bumper.dmgFlags = 0x4FC7FFEA;
            if (thisv->unk_19A != 0) {
                thisv->unk_19A--;
            }
            if (thisv->unk_19A == 0) {
                thisv->unk_197 = 20;
                func_80AD93C4(thisv);
            }
        } else {
            func_80AD9F1C(thisv);
        }
    } else {
        thisv->unk_22E.a = (thisv->skelAnime.curFrame * 255.0f) / thisv->skelAnime.endFrame;
        if (thisv->unk_194 == 0) {
            func_80AD97C8(thisv, globalCtx);
        }
    }
}

void func_80ADAFC0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    s32 i;

    thisv->unk_19A++;
    thisv->unk_198 = CLAMP_MAX(thisv->unk_198 + 1, 8);
    for (i = thisv->unk_198 - 1; i > 0; i--) {
        thisv->unk_234[i] = thisv->unk_234[i - 1];
    }
    thisv->unk_234[0].x =
        (Math_SinS((thisv->actor.shape.rot.y + thisv->unk_19A * 0x3000) - 0x4000) * (3000.0f * thisv->actor.scale.x)) +
        thisv->actor.world.pos.x;
    thisv->unk_234[0].z =
        (Math_CosS((thisv->actor.shape.rot.y + thisv->unk_19A * 0x3000) - 0x4000) * (3000.0f * thisv->actor.scale.x)) +
        thisv->actor.world.pos.z;
    if (thisv->unk_19A < 8) {
        thisv->unk_234[0].y = thisv->unk_234[1].y - 9.0f;
    } else {
        thisv->unk_234[0].y = thisv->unk_234[1].y + 2.0f;
        if (thisv->unk_19A >= 16) {
            if (Math_StepToF(&thisv->actor.scale.x, 0.0f, 0.001f) != 0) {
                func_80AD9A54(thisv, globalCtx);
            }
            thisv->actor.scale.z = thisv->actor.scale.x;
            thisv->actor.scale.y = thisv->actor.scale.x;
        }
    }
    if (thisv->unk_19A == 16) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD2);
    }
}

void func_80ADB17C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    thisv->unk_19A++;
    if (thisv->unk_19A == 64) {
        Flags_SetSwitch(globalCtx, thisv->actor.params);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 30, NA_SE_EV_FLAME_IGNITION);
        if (thisv->unk_194 == 0) {
            Flags_UnsetSwitch(globalCtx, 0x1B);
        }
        globalCtx->envCtx.unk_BF = 0xFF;
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Actor_Kill(&thisv->actor);
    } else if (thisv->unk_19A < 32) {
        func_80AD9240(thisv, thisv->unk_19A, &thisv->actor.world.pos);
    } else {
        func_80AD9240(thisv, 64 - thisv->unk_19A, &thisv->actor.world.pos);
    }
    if (thisv->unk_19A == 32) {
        thisv->actor.world.pos.x = D_80ADD7A4[thisv->unk_194].x;
        thisv->actor.world.pos.y = D_80ADD7A4[thisv->unk_194].y;
        thisv->actor.world.pos.z = D_80ADD7A4[thisv->unk_194].z;
    }
}

void func_80ADB2B8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actor.xzDistToPlayer < 130.0f) {
        func_80AD9DF0(thisv, globalCtx);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_CRY);
    }
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
}

void func_80ADB338(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    EnPoSisters* realMeg = (EnPoSisters*)thisv->actor.parent;

    if (thisv->unk_195 == 0) {
        if (Actor_WorldDistXZToPoint(&player->actor, &thisv->actor.home.pos) < 600.0f) {
            if (thisv->unk_19C != 0) {
                thisv->unk_19C--;
            }
        } else {
            thisv->unk_19C = 100;
        }
        if (thisv->unk_19C == 0) {
            thisv->actor.shape.rot.y = (s32)(4.0f * Rand_ZeroOne()) * 0x4000 + thisv->actor.yawTowardsPlayer;
            thisv->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            func_80AD98F4(thisv, globalCtx);
        }
    } else {
        if (realMeg->actionFunc == func_80ADB51C) {
            thisv->actor.draw = EnPoSisters_Draw;
            func_80AD9E60(thisv);
        } else if (realMeg->actionFunc == func_80ADAE6C) {
            thisv->actor.shape.rot.y = thisv->actor.parent->shape.rot.y + thisv->unk_195 * 0x4000;
            thisv->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            func_80AD98F4(thisv, globalCtx);
        } else if (realMeg->actionFunc == func_80ADAFC0) {
            Actor_Kill(&thisv->actor);
        }
    }
}

void func_80ADB4B0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        func_80AD9E60(thisv);
    }
    func_80AD97C8(thisv, globalCtx);
    thisv->actor.world.pos.y += 1.0f;
    Actor_SetFocus(&thisv->actor, 40.0f);
}

void func_80ADB51C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    f32 temp_f2;
    s16 phi_v0;
    s16 phi_a2;
    u8 temp;

    SkelAnime_Update(&thisv->skelAnime);
    temp_f2 = thisv->skelAnime.endFrame * 0.5f;
    thisv->unk_22E.a = (fabsf(temp_f2 - thisv->skelAnime.curFrame) * 255.0f) / temp_f2;
    if (thisv->unk_19A != 0) {
        thisv->unk_19A -= 1;
    }
    if (thisv->unk_19A == 0) {
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y += 0x4000 * (s32)(Rand_ZeroOne() * 4.0f);
        if (thisv->unk_195 == 0) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
        func_80AD9F1C(thisv);
    } else {
        thisv->actor.world.pos.y += 0.1f;
        temp = thisv->unk_195;
        if (temp != 0) {
            if (thisv->unk_19A > 90) {
                phi_v0 = 1;
                phi_a2 = 64;
            } else if (thisv->unk_19A > 70) {
                phi_v0 = 0;
                phi_a2 = 64;
            } else if (thisv->unk_19A > 55) {
                phi_v0 = 1;
                phi_a2 = 96;
            } else if (thisv->unk_19A > 40) {
                phi_v0 = 0;
                phi_a2 = 96;
            } else {
                phi_v0 = 1;
                phi_a2 = 256;
            }
            if (thisv->unk_195 == 2) {
                phi_a2 *= 2;
            }
            Math_ScaledStepToS(&thisv->actor.shape.rot.y,
                               thisv->actor.parent->shape.rot.y + (thisv->unk_195 * 0x4000) * phi_v0, phi_a2);
        } else if (thisv->unk_19A == 70 || thisv->unk_19A == 40) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
        }
    }
    func_80AD97C8(thisv, globalCtx);
    Actor_SetFocus(&thisv->actor, 40.0f);
}

void func_80ADB770(EnPoSisters* thisv, GlobalContext* globalCtx) {
    s32 temp_v0;
    s32 phi_a0;

    if (thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    if (thisv->unk_19C > 0) {
        if (thisv->unk_19A >= 16) {
            SkelAnime_Update(&thisv->skelAnime);
            if (thisv->unk_195 == 0) {
                if (ABS((s16)(16 - thisv->unk_196)) < 14) {
                    thisv->actor.shape.rot.y +=
                        (0x580 - (thisv->unk_19C * 0x180)) * fabsf(Math_SinS(thisv->unk_196 * 0x800));
                }
                if (thisv->unk_19A >= 284 || thisv->unk_19A < 31) {
                    thisv->unk_199 |= 0x40;
                } else {
                    thisv->unk_199 &= ~0x40;
                }
            } else {
                thisv->actor.shape.rot.y = (s16)(thisv->actor.parent->shape.rot.y + (thisv->unk_195 * 0x4000));
            }
        }
    }
    if (thisv->unk_195 == 0) {
        if (thisv->unk_19A >= 284 || (thisv->unk_19A < 31 && thisv->unk_19A >= 16)) {
            thisv->unk_199 |= 0x40;
        } else {
            thisv->unk_199 &= ~0x40;
        }
    }
    if (Actor_WorldDistXZToPoint(&GET_PLAYER(globalCtx)->actor, &thisv->actor.home.pos) > 600.0f) {
        thisv->unk_199 &= ~0x40;
        func_80AD9C24(thisv, globalCtx);
    } else if (thisv->unk_19A == 0) {
        if (thisv->unk_195 == 0) {
            func_80AD94E0(thisv);
        } else {
            func_80AD9C24(thisv, globalCtx);
        }
    } else if (thisv->unk_195 != 0) {
        EnPoSisters* realMeg = (EnPoSisters*)thisv->actor.parent;

        if (realMeg->actionFunc == func_80ADAAA4) {
            func_80AD95D8(thisv);
        }
    } else if (thisv->unk_19C == 0) {
        thisv->unk_19C = -15;
    } else if (thisv->unk_19C < 0) {
        thisv->unk_19C++;
        if (thisv->unk_19C == 0) {
            func_80AD94E0(thisv);
        }
    }
    func_80AD97C8(thisv, globalCtx);
}

void func_80ADB9F0(EnPoSisters* thisv, GlobalContext* globalCtx) {
    f32 div;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->unk_22E.a = 255;
        if (thisv->unk_194 == 3) {
            thisv->actor.flags |= ACTOR_FLAG_0;
            thisv->actor.home.pos.x = 1992.0f;
            thisv->actor.home.pos.z = -1440.0f;
            thisv->unk_199 |= 0x18;
            func_80AD93C4(thisv);
        } else {
            func_80AD9F90(thisv);
        }
    } else {
        div = thisv->skelAnime.curFrame / thisv->skelAnime.endFrame;
        thisv->unk_22E.a = 255.0f * div;
    }
    if (thisv->unk_194 != 3 && Animation_OnFrame(&thisv->skelAnime, 1.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
    }
    Actor_SetFocus(&thisv->actor, 40.0f);
}

void func_80ADBB6C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (Actor_WorldDistXZToPoint(&thisv->actor, &thisv->actor.home.pos) < 10.0f) {
        func_80ADA028(thisv);
    } else {
        Math_ScaledStepToS(&thisv->actor.world.rot.y, Actor_WorldYawTowardPoint(&thisv->actor, &thisv->actor.home.pos),
                           1820);
    }
}

void func_80ADBBF4(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->actor.yawTowardsPlayer, 1820);
    if (thisv->actor.xzDistToPlayer < 240.0f && fabsf(thisv->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD93C4(thisv);
    }
}

void func_80ADBC88(EnPoSisters* thisv, GlobalContext* globalCtx) {
    if (D_80ADD784 != 0 || !Player_InCsMode(globalCtx)) {
        if (thisv->unk_19A != 0) {
            thisv->unk_19A--;
        }
        if (thisv->unk_19A == 30) {
            if (thisv->unk_194 == 0) {
                OnePointCutscene_Init(globalCtx, 3140, 999, NULL, MAIN_CAM);
            }
            D_80ADD784 = 1;
        }
        if (thisv->unk_19A == 0) {
            func_80ADA10C(thisv);
        }
    }
    func_8002F974(&thisv->actor, NA_SE_EV_TORCH - SFX_FLAG);
}

void func_80ADBD38(EnPoSisters* thisv, GlobalContext* globalCtx) {
    thisv->unk_19A++;
    func_80AD9240(thisv, thisv->unk_19A, &thisv->actor.home.pos);
    if (thisv->unk_19A == 32) {
        func_80ADA1B8(thisv);
    }
}

void func_80ADBD8C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    thisv->unk_19A--;
    if (thisv->unk_19A == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
        thisv->unk_199 &= ~0x80;
    }
    if (thisv->unk_19A <= 0) {
        if (SkelAnime_Update(&thisv->skelAnime)) {
            thisv->unk_22E.a = 255;
            D_80ADD784 |= (1 << thisv->unk_194);
        } else {
            thisv->unk_22E.a = (thisv->skelAnime.curFrame * 255.0f) / thisv->skelAnime.endFrame;
        }
    }
    if (D_80ADD784 == 15) {
        func_80ADA25C(thisv);
    }
}

void func_80ADBEE8(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_19A != 0) {
        thisv->unk_19A--;
    }
    func_80AD9240(thisv, thisv->unk_19A, &thisv->actor.home.pos);
    if (thisv->unk_19A == 0) {
        func_80ADA2BC(thisv, globalCtx);
    }
}

void func_80ADBF58(EnPoSisters* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    thisv->unk_19A--;
    Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 0x500);
    if (thisv->unk_19A == 0 && thisv->unk_194 == 0) {
        globalCtx->envCtx.unk_BF = 4;
    }
    if (thisv->unk_19A < 0) {
        Math_StepToF(&thisv->actor.speedXZ, 5.0f, 0.2f);
    }
    if (thisv->unk_19A == -70 && thisv->unk_194 == 1) {
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &D_80ADD7BC, 40, NA_SE_EN_PO_LAUGH);
    }
    if (thisv->unk_19A < -120) {
        Actor_Kill(&thisv->actor);
    }
}

void func_80ADC034(EnPoSisters* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.isTargeted && thisv->unk_22E.a == 255) {
        if (thisv->unk_197 != 0) {
            thisv->unk_197--;
        }
    } else {
        thisv->unk_197 = 20;
    }
    if (thisv->unk_22E.a == 0) {
        if (thisv->unk_19C != 0) {
            thisv->unk_19C--;
        }
    }
    if (thisv->actionFunc != func_80ADA7F0 && thisv->actionFunc != func_80ADA8C0 && thisv->actionFunc != func_80ADAAA4) {
        if (thisv->unk_197 == 0) {
            func_80AD9718(thisv);
        } else if (thisv->unk_19C == 0 && thisv->unk_22E.a == 0) {
            func_80AD98F4(thisv, globalCtx);
        }
    }
}

void func_80ADC10C(EnPoSisters* thisv, GlobalContext* globalCtx) {
    Vec3f sp24;

    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if (thisv->unk_195 != 0) {
            ((EnPoSisters*)thisv->actor.parent)->unk_19C--;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(thisv, globalCtx);
            if (Rand_ZeroOne() < 0.2f) {
                sp24.x = thisv->actor.world.pos.x;
                sp24.y = thisv->actor.world.pos.y;
                sp24.z = thisv->actor.world.pos.z;
                Item_DropCollectible(globalCtx, &sp24, ITEM00_ARROWS_SMALL);
            }
        } else if (thisv->collider.base.colType == 9 ||
                   (thisv->actor.colChkInfo.damageEffect == 0 && thisv->actor.colChkInfo.damage == 0)) {
            if (thisv->unk_194 == 0) {
                thisv->actor.freezeTimer = 0;
            }
        } else if (thisv->actor.colChkInfo.damageEffect == 0xF) {
            thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
            thisv->unk_199 |= 2;
            func_80AD98F4(thisv, globalCtx);
        } else if (thisv->unk_194 == 0 && thisv->actor.colChkInfo.damageEffect == 0xE &&
                   thisv->actionFunc == func_80ADB770) {
            if (thisv->unk_19C == 0) {
                thisv->unk_19C = -45;
            }
        } else {
            if (Actor_ApplyDamage(&thisv->actor) != 0) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DAMAGE);
            } else {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_SISTER_DEAD);
            }
            func_80AD95D8(thisv);
        }
    }
}

void EnPoSisters_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnPoSisters* thisv = (EnPoSisters*)thisx;
    s16 temp;

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~AT_HIT;
        func_80AD9568(thisv);
    }
    func_80ADC10C(thisv, globalCtx);
    if (thisv->unk_199 & 4) {
        func_80ADC034(thisv, globalCtx);
    }
    thisv->actionFunc(thisv, globalCtx);
    if (thisv->unk_199 & 0x1F) {
        if (thisv->unk_199 & 8) {
            func_80ADA35C(thisv, globalCtx);
        }
        Actor_MoveForward(&thisv->actor);

        if (thisv->unk_199 & 0x10) {
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 20.0f, 20.0f, 0.0f, 5);
        } else {
            Vec3f vec;
            s32 sp34;

            vec.x = thisv->actor.world.pos.x;
            vec.y = thisv->actor.world.pos.y + 10.0f;
            vec.z = thisv->actor.world.pos.z;
            thisv->actor.floorHeight =
                BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &sp34, &thisv->actor, &vec);
        }

        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if (thisv->actionFunc == func_80ADA8C0 || thisv->actionFunc == func_80ADA7F0) {
            thisv->unk_198++;
            thisv->unk_198 = CLAMP_MAX(thisv->unk_198, 8);
        } else if (thisv->actionFunc != func_80ADAFC0) {
            temp = thisv->unk_198 - 1;
            thisv->unk_198 = CLAMP_MIN(temp, 1);
        }
        if (thisv->actionFunc == func_80ADA8C0) {
            thisv->actor.flags |= ACTOR_FLAG_24;
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->unk_199 & 1) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->actionFunc != func_80ADB338) {
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        Actor_SetFocus(&thisv->actor, 40.0f);
        if (thisv->actionFunc == func_80ADAC70) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y + 0x8000;
        } else if (thisv->unk_199 & 2) {
            thisv->actor.shape.rot.y = thisv->actor.world.rot.y;
        }
    }
}

void func_80ADC55C(EnPoSisters* thisv) {
    s16 temp_var;

    if (thisv->skelAnime.animation == &gPoeSistersAttackAnim) {
        thisv->unk_22E.r = CLAMP_MAX((s16)(thisv->unk_22E.r + 5), 255);
        thisv->unk_22E.g = CLAMP_MIN((s16)(thisv->unk_22E.g - 5), 50);
        temp_var = thisv->unk_22E.b - 5;
        thisv->unk_22E.b = CLAMP_MIN(temp_var, 0);
    } else if (thisv->skelAnime.animation == &gPoeSistersFleeAnim) {
        thisv->unk_22E.r = CLAMP_MAX((s16)(thisv->unk_22E.r + 5), 80);
        thisv->unk_22E.g = CLAMP_MAX((s16)(thisv->unk_22E.g + 5), 255);
        temp_var = thisv->unk_22E.b + 5;
        thisv->unk_22E.b = CLAMP_MAX(temp_var, 225);
    } else if (thisv->skelAnime.animation == &gPoeSistersDamagedAnim) {
        if (thisv->actor.colorFilterTimer & 2) {
            thisv->unk_22E.r = 0;
            thisv->unk_22E.g = 0;
            thisv->unk_22E.b = 0;
        } else {
            thisv->unk_22E.r = 80;
            thisv->unk_22E.g = 255;
            thisv->unk_22E.b = 225;
        }
    } else {
        thisv->unk_22E.r = CLAMP_MAX((s16)(thisv->unk_22E.r + 5), 255);
        thisv->unk_22E.g = CLAMP_MAX((s16)(thisv->unk_22E.g + 5), 255);
        if (thisv->unk_22E.b > 210) {
            temp_var = thisv->unk_22E.b - 5;
            thisv->unk_22E.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = thisv->unk_22E.b + 5;
            thisv->unk_22E.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

s32 EnPoSisters_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx, Gfx** gfxP) {
    EnPoSisters* thisv = (EnPoSisters*)thisx;
    Color_RGBA8* color;

    if (limbIndex == 1 && (thisv->unk_199 & 0x40)) {
        if (thisv->unk_19A >= 284) {
            rot->x += (thisv->unk_19A * 0x1000) + 0xFFEE4000;
        } else {
            rot->x += (thisv->unk_19A * 0x1000) + 0xFFFF1000;
        }
    }
    if (thisv->unk_22E.a == 0 || limbIndex == 8 || (thisv->actionFunc == func_80ADAFC0 && thisv->unk_19A >= 8)) {
        *dList = NULL;
    } else if (limbIndex == 9) {
        *dList = D_80ADD7C8[thisv->unk_194];
    } else if (limbIndex == 10) {
        *dList = D_80ADD7D8[thisv->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, thisv->unk_22E.r, thisv->unk_22E.g, thisv->unk_22E.b, thisv->unk_22E.a);
    } else if (limbIndex == 11) {
        color = &D_80ADD7E8[thisv->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, color->r, color->g, color->b, thisv->unk_22E.a);
    }
    return false;
}

void EnPoSisters_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                              Gfx** gfxP) {
    EnPoSisters* thisv = (EnPoSisters*)thisx;
    s32 i;
    s32 pad;

    if (thisv->actionFunc == func_80ADAFC0 && thisv->unk_19A >= 8 && limbIndex == 9) {
        gSPMatrix((*gfxP)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_sisters.c", 2876),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, gPoSistersBurnDL);
    }
    if (limbIndex == 8 && thisv->actionFunc != func_80ADB2B8) {
        if (thisv->unk_199 & 0x20) {
            for (i = thisv->unk_198 - 1; i > 0; i--) {
                thisv->unk_234[i] = thisv->unk_234[i - 1];
            }
            Matrix_MultVec3f(&D_80ADD7F8, &thisv->unk_234[0]);
        } else if (thisv->actionFunc == func_80ADBD8C) {
            Matrix_MultVec3f(&D_80ADD7F8, &thisv->actor.home.pos);
        }
        if (thisv->unk_198 > 0) {
            Color_RGBA8* color = &D_80ADD6F0[thisv->unk_194];
            f32 temp_f2 = Rand_ZeroOne() * 0.3f + 0.7f;

            if (thisv->actionFunc == func_80ADB17C || thisv->actionFunc == func_80ADBD38 ||
                thisv->actionFunc == func_80ADBEE8) {
                Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->unk_234[0].x, thisv->unk_234[0].y + 15.0f,
                                          thisv->unk_234[0].z, color->r * temp_f2, color->g * temp_f2,
                                          color->b * temp_f2, 200);
            } else {
                Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->unk_234[0].x, thisv->unk_234[0].y + 15.0f,
                                        thisv->unk_234[0].z, color->r * temp_f2, color->g * temp_f2, color->b * temp_f2,
                                        200);
            }
        } else {
            Lights_PointSetColorAndRadius(&thisv->lightInfo, 0, 0, 0, 0);
        }
        if (!(thisv->unk_199 & 0x80)) {
            Matrix_Get(&thisv->unk_2F8);
        }
    }
}

void EnPoSisters_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnPoSisters* thisv = (EnPoSisters*)thisx;
    u8 phi_s5 = 0;
    f32 phi_f20;
    s32 i;
    u8 spE7 = 0;
    Color_RGBA8* temp_s1 = &D_80ADD700[thisv->unk_194];
    Color_RGBA8* temp_s7 = &D_80ADD6F0[thisv->unk_194];
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_sisters.c", 2989);
    func_80ADC55C(thisv);
    func_80093D18(globalCtx->state.gfxCtx);
    func_80093D84(globalCtx->state.gfxCtx);
    if (thisv->unk_22E.a == 255 || thisv->unk_22E.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->unk_22E.r, thisv->unk_22E.g, thisv->unk_22E.b, thisv->unk_22E.a);
        gSPSegment(POLY_OPA_DISP++, 0x09, D_80116280 + 2);
        POLY_OPA_DISP =
            SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                           EnPoSisters_OverrideLimbDraw, EnPoSisters_PostLimbDraw, &thisv->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->unk_22E.a);
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80116280);
        POLY_XLU_DISP =
            SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                           EnPoSisters_OverrideLimbDraw, EnPoSisters_PostLimbDraw, &thisv->actor, POLY_XLU_DISP);
    }
    if (!(thisv->unk_199 & 0x80)) {
        Matrix_Put(&thisv->unk_2F8);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_sisters.c", 3034),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoSistersTorchDL);
    }
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (globalCtx->gameplayFrames * -20) % 512, 0x20, 0x80));
    gDPSetEnvColor(POLY_XLU_DISP++, temp_s1->r, temp_s1->g, temp_s1->b, temp_s1->a);
    if (thisv->actionFunc == func_80ADB17C) {
        if (thisv->unk_19A < 32) {
            phi_s5 = ((32 - thisv->unk_19A) * 255) / 32;
            phi_f20 = 0.0056000003f;
        } else {
            phi_s5 = (thisv->unk_19A * 255 - 8160) / 32;
            phi_f20 = 0.0027f;
        }
    } else if (thisv->actionFunc == func_80ADBD38) {
        phi_s5 = ((32 - thisv->unk_19A) * 255) / 32;
        phi_f20 = 0.0027f;
    } else if (thisv->actionFunc == func_80ADBEE8) {
        phi_s5 = ((32 - thisv->unk_19A) * 255) / 32;
        phi_f20 = 0.0035f;
    } else if (thisv->actionFunc == func_80ADBC88) {
        //! @bug uninitialised spE7
        phi_s5 = spE7;
        phi_f20 = 0.0027f;
    } else {
        phi_s5 = spE7;
        phi_f20 = thisv->actor.scale.x * 0.5f;
    }
    for (i = 0; i < thisv->unk_198; i++) {
        if (thisv->actionFunc != func_80ADB17C && thisv->actionFunc != func_80ADBD38 &&
            thisv->actionFunc != func_80ADBEE8) {
            phi_s5 = -i * 31 + 248;
        }
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, temp_s7->r, temp_s7->g, temp_s7->b, phi_s5);
        Matrix_Translate(thisv->unk_234[i].x, thisv->unk_234[i].y, thisv->unk_234[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(0, (s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000), 0, MTXMODE_APPLY);
        if (thisv->actionFunc == func_80ADAFC0) {
            phi_f20 = (thisv->unk_19A - i) * 0.025f + 0.5f;
            phi_f20 = CLAMP(phi_f20, 0.5f, 0.8f) * 0.007f;
        }
        Matrix_Scale(phi_f20, phi_f20, phi_f20, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_sisters.c", 3132),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_sisters.c", 3139);
}

void EnPoSisters_Reset(void) {
    D_80ADD784 = 0;
}