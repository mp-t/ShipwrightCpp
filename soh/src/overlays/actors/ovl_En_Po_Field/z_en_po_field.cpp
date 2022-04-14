/*
 * File: z_en_po_field.c
 * Overlay: ovl_En_Po_Field
 * Description: Field Poe
 */

#include "z_en_po_field.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_po_field/object_po_field.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_12)

void EnPoField_Init(Actor* thisx, GlobalContext* globalCtx);
void EnPoField_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnPoField_Update(Actor* thisx, GlobalContext* globalCtx);
void EnPoField_Draw(Actor* thisx, GlobalContext* globalCtx);
void EnPoField_Reset(void);

void EnPoField_UpdateDead(Actor* thisx, GlobalContext* globalCtx);
void EnPoField_DrawSoul(Actor* thisx, GlobalContext* globalCtx);

void EnPoField_SetupWaitForSpawn(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_WaitForSpawn(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_Appear(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_CirclePlayer(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_Damage(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_Flee(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_Death(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_Disappear(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_SoulIdle(EnPoField* thisv, GlobalContext* globalCtx);
void func_80AD587C(EnPoField* thisv, GlobalContext* globalCtx);
void func_80AD58D4(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_SoulDisappear(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_SoulInteract(EnPoField* thisv, GlobalContext* globalCtx);
void EnPoField_SpawnFlame(EnPoField* thisv);

const ActorInit En_Po_Field_InitVars = {
    ACTOR_EN_PO_FIELD,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PO_FIELD,
    sizeof(EnPoField),
    (ActorFunc)EnPoField_Init,
    (ActorFunc)EnPoField_Destroy,
    (ActorFunc)EnPoField_Update,
    (ActorFunc)EnPoField_Draw,
    (ActorResetFunc)EnPoField_Reset,
};

static ColliderCylinderInit D_80AD7080 = {
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
    { 25, 50, 20, { 0, 0, 0 } },
};

static ColliderCylinderInit D_80AD70AC = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit D_80AD70D8 = { 4, 25, 50, 40 };

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

static s32 sNumSpawned = 0;

static Vec3f sFieldMiddle = { -1000.0f, 0.0f, 6500.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f D_80AD7114 = { 0.0f, 3.0f, 0.0f };

static Vec3f D_80AD7120 = { 0.0f, 0.0f, 0.0f };

static EnPoFieldInfo sPoFieldInfo[2] = {
    { { 255, 170, 255 }, { 100, 0, 150 }, { 255, 85, 0 }, 248, gPoeFieldSoulTex },
    { { 255, 255, 170 }, { 255, 200, 0 }, { 160, 0, 255 }, 241, gBigPoeSoulTex },
};

static Vec3f D_80AD714C = { 0.0f, 1400.0f, 0.0f };

static Vec3s sSpawnPositions[10];
static u8 sSpawnSwitchFlags[10];
static MtxF sLimb7Mtx;

void EnPoField_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnPoField* thisv = (EnPoField*)thisx;
    s32 pad;

    if (sNumSpawned != 10) {
        sSpawnPositions[sNumSpawned].x = thisv->actor.world.pos.x;
        sSpawnPositions[sNumSpawned].y = thisv->actor.world.pos.y;
        sSpawnPositions[sNumSpawned].z = thisv->actor.world.pos.z;
        sSpawnSwitchFlags[sNumSpawned] = thisv->actor.params & 0xFF;
        sNumSpawned++;
    }
    if (sNumSpawned >= 2) {
        thisv->actor.params = 0xFF;
        Actor_Kill(&thisv->actor);
        return;
    }
    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gPoeFieldSkel, &gPoeFieldFloatAnim, thisv->jointTable, thisv->morphTable,
                   10);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &D_80AD7080);
    Collider_InitCylinder(globalCtx, &thisv->flameCollider);
    Collider_SetCylinder(globalCtx, &thisv->flameCollider, &thisv->actor, &D_80AD70AC);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &D_80AD70D8);
    thisv->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &thisv->lightInfo);
    Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.home.pos.x, thisv->actor.home.pos.y, thisv->actor.home.pos.z,
                            255, 255, 255, 0);
    thisv->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    EnPoField_SetupWaitForSpawn(thisv, globalCtx);
}

void EnPoField_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnPoField* thisv = (EnPoField*)thisx;

    if (thisv->actor.params != 0xFF) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, thisv->lightNode);
        Collider_DestroyCylinder(globalCtx, &thisv->flameCollider);
        Collider_DestroyCylinder(globalCtx, &thisv->collider);
    }
}

void EnPoField_SetupWaitForSpawn(EnPoField* thisv, GlobalContext* globalCtx) {
    thisv->actor.update = EnPoField_Update;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
    thisv->actor.shape.rot.x = 0;
    Lights_PointSetColorAndRadius(&thisv->lightInfo, 0, 0, 0, 0);
    thisv->actionTimer = 200;
    Actor_SetScale(&thisv->actor, 0.0f);
    thisv->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_16);
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_ALL;
    thisv->actor.colChkInfo.health = D_80AD70D8.health;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actionFunc = EnPoField_WaitForSpawn;
}

void EnPoField_SetupAppear(EnPoField* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gPoeFieldAppearAnim);
    thisv->actor.draw = EnPoField_Draw;
    thisv->lightColor.r = 255;
    thisv->lightColor.g = 255;
    thisv->lightColor.b = 210;
    thisv->lightColor.a = 0;
    thisv->actor.shape.shadowAlpha = 0;
    thisv->actor.shape.yOffset = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_APPEAR);
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    if (thisv->actor.params == EN_PO_FIELD_BIG) {
        thisv->actor.speedXZ = 12.0f;
        thisv->collider.dim.radius = 35;
        thisv->collider.dim.height = 100;
        thisv->collider.dim.yShift = 10;
        thisv->actor.shape.shadowScale = 45.0f;
        thisv->scaleModifier = 0.014f;
        thisv->actor.naviEnemyId = 0x5A;
    } else {
        thisv->actor.speedXZ = 0.0f;
        thisv->collider.dim.radius = D_80AD7080.dim.radius;
        thisv->collider.dim.height = D_80AD7080.dim.height;
        thisv->collider.dim.yShift = D_80AD7080.dim.yShift;
        thisv->actor.shape.shadowScale = 37.0f;
        thisv->scaleModifier = 0.01f;
        thisv->actor.naviEnemyId = 0x5C;
    }
    thisv->actionFunc = EnPoField_Appear;
}

void EnPoField_SetupCirclePlayer(EnPoField* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Animation_PlayLoop(&thisv->skelAnime, &gPoeFieldFloatAnim);
    thisv->collider.base.acFlags |= AC_ON;
    thisv->scaleModifier = thisv->actor.xzDistToPlayer;
    Math_Vec3f_Copy(&thisv->actor.home.pos, &player->actor.world.pos);
    thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->actionFunc != EnPoField_Damage) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actionTimer = 600;
        thisv->unk_194 = 32;
    }
    thisv->actionFunc = EnPoField_CirclePlayer;
}

void EnPoField_SetupFlee(EnPoField* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeFieldFleeAnim, -5.0f);
    thisv->collider.base.acFlags |= AC_ON;
    thisv->actionFunc = EnPoField_Flee;
    thisv->actor.speedXZ = 12.0f;
    if (thisv->actionFunc != EnPoField_Damage) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
        thisv->actionTimer = 2000;
        thisv->unk_194 = 32;
    }
}

void EnPoField_SetupDamage(EnPoField* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gPoeFieldDamagedAnim, -6.0f);
    if (thisv->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824) {
        thisv->actor.world.rot.y = thisv->collider.base.ac->world.rot.y;
    } else {
        thisv->actor.world.rot.y = Actor_WorldYawTowardActor(&thisv->actor, thisv->collider.base.ac) + 0x8000;
    }
    thisv->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    thisv->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 16);
    thisv->actionFunc = EnPoField_Damage;
}

void EnPoField_SetupDeath(EnPoField* thisv) {
    thisv->actionTimer = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.naviEnemyId = 0xFF;
    if (thisv->flameTimer >= 20) {
        thisv->flameTimer = 19;
    }
    thisv->actionFunc = EnPoField_Death;
}

void EnPoField_SetupDisappear(EnPoField* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    thisv->actionTimer = 16;
    thisv->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    thisv->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DISAPPEAR);
    thisv->actionFunc = EnPoField_Disappear;
}

void EnPoField_SetupSoulIdle(EnPoField* thisv, GlobalContext* globalCtx) {
    thisv->actor.update = EnPoField_UpdateDead;
    thisv->actor.draw = EnPoField_DrawSoul;
    thisv->actor.shape.shadowDraw = NULL;
    Actor_SetScale(&thisv->actor, 0.01f);
    thisv->actor.gravity = -1.0f;
    thisv->actor.shape.yOffset = 1500.0f;
    thisv->actor.shape.rot.x = -0x8000;
    thisv->actionTimer = 60;
    thisv->actor.world.pos.y -= 15.0f;
    Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_MISC);
    thisv->actionFunc = EnPoField_SoulIdle;
}

void func_80AD42B0(EnPoField* thisv) {
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, 0, 0, 0, 0);
    thisv->actor.shape.rot.y = 0;
    thisv->lightColor.a = 0;
    thisv->actor.shape.rot.x = 0;
    thisv->actor.shape.yOffset = 0.0f;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    thisv->actor.scale.x = 0.0f;
    thisv->actor.scale.y = 0.0f;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EV_METAL_BOX_BOUND);
    if (thisv->actor.params == EN_PO_FIELD_BIG) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
    thisv->actionFunc = func_80AD587C;
}

void func_80AD4384(EnPoField* thisv) {
    thisv->actor.home.pos.y = thisv->actor.world.pos.y;
    Actor_SetFocus(&thisv->actor, -10.0f);
    thisv->collider.dim.radius = 13;
    thisv->collider.dim.height = 30;
    thisv->collider.dim.yShift = 0;
    thisv->collider.dim.pos.x = thisv->actor.world.pos.x;
    thisv->collider.dim.pos.y = thisv->actor.world.pos.y - 20.0f;
    thisv->collider.dim.pos.z = thisv->actor.world.pos.z;
    thisv->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    thisv->actor.textId = 0x5005;
    thisv->actionTimer = 400;
    thisv->unk_194 = 32;
    thisv->actor.flags |= ACTOR_FLAG_0;
    thisv->actionFunc = func_80AD58D4;
}

void EnPoField_SetupSoulDisappear(EnPoField* thisv) {
    thisv->actionFunc = EnPoField_SoulDisappear;
}

void EnPoField_SetupInteractWithSoul(EnPoField* thisv) {
    thisv->actionFunc = EnPoField_SoulInteract;
    thisv->actor.home.pos.y = thisv->actor.world.pos.y - 15.0f;
}

void EnPoField_CorrectYPos(EnPoField* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (thisv->unk_194 == 0) {
        thisv->unk_194 = 32;
    }
    if (thisv->unk_194 != 0) {
        thisv->unk_194 -= 1;
    }
    if (thisv->actor.floorHeight == BGCHECK_Y_MIN) {
        EnPoField_SetupDisappear(thisv);
        return;
    }
    Math_ApproachF(
        &thisv->actor.home.pos.y,
        ((player->actor.world.pos.y > thisv->actor.floorHeight) ? player->actor.world.pos.y : thisv->actor.floorHeight) +
            13.0f,
        0.2f, 5.0f);
    thisv->actor.world.pos.y = Math_SinS(thisv->unk_194 * 0x800) * 13.0f + thisv->actor.home.pos.y;
}

f32 EnPoField_SetFleeSpeed(EnPoField* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 speed = ((player->stateFlags1 & 0x800000) && player->rideActor != NULL) ? player->rideActor->speedXZ : 12.0f;

    if (thisv->actor.xzDistToPlayer < 300.0f) {
        thisv->actor.speedXZ = speed * 1.5f + 2.0f;
    } else if (thisv->actor.xzDistToPlayer < 400.0f) {
        thisv->actor.speedXZ = speed * 1.25f + 2.0f;
    } else if (thisv->actor.xzDistToPlayer < 500.0f) {
        thisv->actor.speedXZ = speed + 2.0f;
    } else {
        thisv->actor.speedXZ = 12.0f;
    }
    thisv->actor.speedXZ = CLAMP_MIN(thisv->actor.speedXZ, 12.0f);
}

void EnPoField_WaitForSpawn(EnPoField* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 spawnDist;
    s32 i;
    s32 bgId;

    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->actionTimer == 0) {
        for (i = 0; i < sNumSpawned; i++) {
            if (fabsf(sSpawnPositions[i].x - player->actor.world.pos.x) < 150.0f &&
                fabsf(sSpawnPositions[i].z - player->actor.world.pos.z) < 150.0f) {
                if (Flags_GetSwitch(globalCtx, sSpawnSwitchFlags[i])) {
                    if (player->stateFlags1 & 0x800000) { // Player riding Epona
                        return;
                    } else {
                        thisv->actor.params = EN_PO_FIELD_SMALL;
                        spawnDist = 300.0f;
                    }
                } else if (player->stateFlags1 & 0x800000 || Rand_ZeroOne() < 0.4f) {
                    thisv->actor.params = EN_PO_FIELD_BIG;
                    thisv->spawnFlagIndex = i;
                    spawnDist = 480.0f;
                } else {
                    thisv->actor.params = EN_PO_FIELD_SMALL;
                    spawnDist = 300.0f;
                }
                thisv->actor.world.pos.x = Math_SinS(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.x;
                thisv->actor.world.pos.z = Math_CosS(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.z;
                thisv->actor.world.pos.y = player->actor.world.pos.y + 1000.0f;
                thisv->actor.world.pos.y = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                                      &thisv->actor, &thisv->actor.world.pos);
                if (thisv->actor.world.pos.y != BGCHECK_Y_MIN) {
                    thisv->actor.shape.rot.y = Actor_WorldYawTowardActor(&thisv->actor, &player->actor);
                    EnPoField_SetupAppear(thisv);
                } else {
                    return;
                }
            }
        }
    }
}

void EnPoField_Appear(EnPoField* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->lightColor.a = 255;
        Actor_SetScale(&thisv->actor, thisv->scaleModifier);
        if (thisv->actor.params == EN_PO_FIELD_BIG) {
            EnPoField_SetupFlee(thisv);
        } else {
            EnPoField_SetupCirclePlayer(thisv, globalCtx);
        }
    } else if (thisv->skelAnime.curFrame > 10.0f) {
        thisv->lightColor.a = ((thisv->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    } else {
        thisv->actor.scale.x += thisv->scaleModifier * 0.1f;
        thisv->actor.scale.y = thisv->actor.scale.x;
        thisv->actor.scale.z = thisv->actor.scale.x;
    }
    thisv->actor.shape.shadowAlpha = thisv->lightColor.a;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    if (thisv->actor.params == EN_PO_FIELD_BIG) {
        thisv->actor.world.rot.y = thisv->actor.yawTowardsPlayer + 0x8000;
        EnPoField_SetFleeSpeed(thisv, globalCtx);
    }
}

void EnPoField_CirclePlayer(EnPoField* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 temp_v1 = 16 - thisv->unk_194;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (ABS(temp_v1) < 16) {
        thisv->actor.world.rot.y += 512.0f * fabsf(Math_SinS(thisv->unk_194 * 0x800));
    }
    Math_ApproachF(&thisv->scaleModifier, 180.0f, 0.5f, 10.0f);
    Math_ApproachF(&thisv->actor.home.pos.x, player->actor.world.pos.x, 0.2f, 6.0f);
    Math_ApproachF(&thisv->actor.home.pos.z, player->actor.world.pos.z, 0.2f, 6.0f);
    Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.y, 1, 0x800, 0x200);
    if (thisv->actor.home.pos.x - player->actor.world.pos.x > 100.0f) {
        thisv->actor.home.pos.x = player->actor.world.pos.x + 100.0f;
    } else if (thisv->actor.home.pos.x - player->actor.world.pos.x < -100.0f) {
        thisv->actor.home.pos.x = player->actor.world.pos.x + -100.0f;
    }
    if (thisv->actor.home.pos.z - player->actor.world.pos.z > 100.0f) {
        thisv->actor.home.pos.z = player->actor.world.pos.z + 100.0f;
    } else if (thisv->actor.home.pos.z - player->actor.world.pos.z < -100.0f) {
        thisv->actor.home.pos.z = player->actor.world.pos.z + -100.0f;
    }
    thisv->actor.world.pos.x = thisv->actor.home.pos.x - (Math_SinS(thisv->actor.world.rot.y) * thisv->scaleModifier);
    thisv->actor.world.pos.z = thisv->actor.home.pos.z - (Math_CosS(thisv->actor.world.rot.y) * thisv->scaleModifier);
    if (thisv->actionTimer == 0) {
        EnPoField_SetupDisappear(thisv);
    } else {
        EnPoField_SpawnFlame(thisv);
    }
    EnPoField_CorrectYPos(thisv, globalCtx);
    func_8002F974(&thisv->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoField_Flee(EnPoField* thisv, GlobalContext* globalCtx) {
    f32 temp_f6;
    s16 phi_t0;

    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (Actor_WorldDistXZToPoint(&thisv->actor, &sFieldMiddle) > 3000.0f) {
        phi_t0 = (s16)(thisv->actor.yawTowardsPlayer - Actor_WorldYawTowardPoint(&thisv->actor, &sFieldMiddle) - 0x8000) *
                 0.2f;
    } else {
        phi_t0 = 0;
    }
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer - phi_t0, 6, 0x400);
    EnPoField_SetFleeSpeed(thisv, globalCtx);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + 0x8000;
    temp_f6 = Math_SinS(thisv->actionTimer * 0x800) * 3.0f;
    thisv->actor.world.pos.x -= temp_f6 * Math_CosS(thisv->actor.shape.rot.y);
    thisv->actor.world.pos.z += temp_f6 * Math_SinS(thisv->actor.shape.rot.y);
    if (thisv->actionTimer == 0 || thisv->actor.xzDistToPlayer > 1500.0f) {
        EnPoField_SetupDisappear(thisv);
    } else {
        EnPoField_CorrectYPos(thisv, globalCtx);
    }
    func_8002F974(&thisv->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void EnPoField_Damage(EnPoField* thisv, GlobalContext* globalCtx) {
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 0.5f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.colChkInfo.health == 0) {
            EnPoField_SetupDeath(thisv);
        } else if (thisv->actor.params == EN_PO_FIELD_BIG) {
            EnPoField_SetupFlee(thisv);
        } else {
            EnPoField_SetupCirclePlayer(thisv, globalCtx);
        }
    }
}

void EnPoField_Death(EnPoField* thisv, GlobalContext* globalCtx) {
    Vec3f sp6C;
    f32 sp68;
    s32 pad;
    s32 pad1;
    f32 temp_f0;

    thisv->actionTimer++;
    if (thisv->actionTimer < 8) {
        if (thisv->actionTimer < 5) {
            sp6C.y = Math_SinS(thisv->actionTimer * 0x1000 - 0x4000) * 23.0f + (thisv->actor.world.pos.y + 40.0f);
            sp68 = Math_CosS(thisv->actionTimer * 0x1000 - 0x4000) * 23.0f;
            sp6C.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * sp68 + thisv->actor.world.pos.x;
            sp6C.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * sp68 + thisv->actor.world.pos.z;
        } else {
            sp6C.y = thisv->actor.world.pos.y + 40.0f + 15.0f * (thisv->actionTimer - 5);
            sp6C.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f + thisv->actor.world.pos.x;
            sp6C.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x4800) * 23.0f + thisv->actor.world.pos.z;
        }
        EffectSsDeadDb_Spawn(globalCtx, &sp6C, &D_80AD7114, &D_80AD7120, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        sp6C.x = (thisv->actor.world.pos.x + thisv->actor.world.pos.x) - sp6C.x;
        sp6C.z = (thisv->actor.world.pos.z + thisv->actor.world.pos.z) - sp6C.z;
        EffectSsDeadDb_Spawn(globalCtx, &sp6C, &D_80AD7114, &D_80AD7120, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        sp6C.x = thisv->actor.world.pos.x;
        sp6C.z = thisv->actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &sp6C, &D_80AD7114, &D_80AD7120, thisv->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        if (thisv->actionTimer == 1) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EXTINCT);
        }
    } else if (thisv->actionTimer == 28) {
        EnPoField_SetupSoulIdle(thisv, globalCtx);
    } else if (thisv->actionTimer >= 19) {
        temp_f0 = (28 - thisv->actionTimer) * 0.001f;
        thisv->actor.world.pos.y += 5.0f;
        thisv->actor.scale.z = temp_f0;
        thisv->actor.scale.y = temp_f0;
        thisv->actor.scale.x = temp_f0;
    }
    if (thisv->actionTimer == 18) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD2);
    }
}

void EnPoField_Disappear(EnPoField* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    thisv->actor.shape.rot.y += 0x1000;
    thisv->lightColor.a = thisv->actionTimer * 15.9375f;
    thisv->actor.shape.shadowAlpha = thisv->lightColor.a;
    if (thisv->actionTimer == 0) {
        EnPoField_SetupWaitForSpawn(thisv, globalCtx);
    }
}

void EnPoField_SoulIdle(EnPoField* thisv, GlobalContext* globalCtx) {
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (thisv->actor.bgCheckFlags & 1) {
        EffectSsHahen_SpawnBurst(globalCtx, &thisv->actor.world.pos, 6.0f, 0, 1, 1, 15, OBJECT_PO_FIELD, 10,
                                 gPoeFieldLanternDL);
        func_80AD42B0(thisv);
    } else if (thisv->actionTimer == 0) {
        EnPoField_SetupWaitForSpawn(thisv, globalCtx);
    }
    Actor_MoveForward(&thisv->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 10.0f, 10.0f, 4);
}

void EnPoField_SoulUpdateProperties(EnPoField* thisv, s32 arg1) {
    EnPoFieldInfo* info = &sPoFieldInfo[thisv->actor.params];
    f32 multiplier;

    thisv->lightColor.a = CLAMP(thisv->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = thisv->lightColor.a * (1.0f / 255);
        thisv->actor.scale.x = thisv->actor.scale.z = 0.0056000003f * multiplier + 0.0014000001f;
        thisv->actor.scale.y = 0.007f - 0.007f * multiplier + 0.007f;
    } else {
        multiplier = 1.0f;
        thisv->actor.scale.x = thisv->actor.scale.y = thisv->actor.scale.z = thisv->lightColor.a * (0.007f / 255);
        thisv->actor.world.pos.y = thisv->actor.home.pos.y + ((1.0f / 17.0f) * thisv->lightColor.a);
    }
    thisv->lightColor.r = info->lightColor.r * multiplier;
    thisv->lightColor.g = info->lightColor.g * multiplier;
    thisv->lightColor.b = info->lightColor.b * multiplier;
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, info->lightColor.r, info->lightColor.g, info->lightColor.b,
                              thisv->lightColor.a * (200.0f / 255));
}

void func_80AD587C(EnPoField* thisv, GlobalContext* globalCtx) {
    thisv->actor.home.pos.y += 2.0f;
    EnPoField_SoulUpdateProperties(thisv, 20);
    if (thisv->lightColor.a == 255) {
        func_80AD4384(thisv);
    }
}

void func_80AD58D4(EnPoField* thisv, GlobalContext* globalCtx) {
    if (thisv->actionTimer != 0) {
        thisv->actionTimer--;
    }
    if (Actor_ProcessTalkRequest(&thisv->actor, globalCtx)) {
        EnPoField_SetupInteractWithSoul(thisv);
        return;
    }
    if (thisv->actionTimer == 0) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        EnPoField_SetupSoulDisappear(thisv);
        return;
    }
    if (thisv->collider.base.ocFlags1 & OC1_HIT) {
        thisv->actor.flags |= ACTOR_FLAG_16;
        func_8002F2F4(&thisv->actor, globalCtx);
    } else {
        thisv->actor.flags &= ~ACTOR_FLAG_16;
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    thisv->actor.world.pos.y = Math_SinS(thisv->unk_194 * 0x800) * 5.0f + thisv->actor.home.pos.y;
    if (thisv->unk_194 != 0) {
        thisv->unk_194 -= 1;
    }
    if (thisv->unk_194 == 0) {
        thisv->unk_194 = 32;
    }
    thisv->collider.dim.pos.y = thisv->actor.world.pos.y - 20.0f;
    Actor_SetFocus(&thisv->actor, -10.0f);
    Lights_PointNoGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                              thisv->actor.world.pos.z, thisv->lightInfo.params.point.color[0],
                              thisv->lightInfo.params.point.color[1], thisv->lightInfo.params.point.color[2],
                              thisv->lightColor.a * (200.0f / 255));
}

void EnPoField_SoulDisappear(EnPoField* thisv, GlobalContext* globalCtx) {
    EnPoField_SoulUpdateProperties(thisv, -13);
    if (thisv->lightColor.a == 0) {
        EnPoField_SetupWaitForSpawn(thisv, globalCtx);
    }
}

void EnPoField_SoulInteract(EnPoField* thisv, GlobalContext* globalCtx) {
    if (thisv->actor.textId != 0x5005) {
        EnPoField_SoulUpdateProperties(thisv, -13);
    } else {
        func_8002F974(&thisv->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(globalCtx)) {
            Audio_StopSfxByPosAndId(&thisv->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (globalCtx->msgCtx.choiceIndex == 0) {
                if (Inventory_HasEmptyBottle()) {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_BIG_GET);
                    if (thisv->actor.params == 0) {
                        Item_Give(globalCtx, ITEM_POE);
                        thisv->actor.textId = 0x5008;
                    } else {
                        thisv->actor.textId = 0x508F;
                        Item_Give(globalCtx, ITEM_BIG_POE);
                        Flags_SetSwitch(globalCtx, sSpawnSwitchFlags[thisv->spawnFlagIndex]);
                    }
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
                    thisv->actor.textId = 0x5006;
                }
            } else {
                thisv->actor.textId = 0x5007;
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_LAUGH);
            }
            Message_ContinueTextbox(globalCtx, thisv->actor.textId);
            return;
        }
    } else if (Actor_TextboxIsClosing(&thisv->actor, globalCtx)) {
        EnPoField_SetupSoulDisappear(thisv);
    }
}

void EnPoField_TestForDamage(EnPoField* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != 0 || thisv->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DEAD);
            } else {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_PO_DAMAGE);
            }
            EnPoField_SetupDamage(thisv);
        }
    }
}

void EnPoField_SpawnFlame(EnPoField* thisv) {
    if (thisv->flameTimer == 0) {
        thisv->flamePosition.x = thisv->lightInfo.params.point.x;
        thisv->flamePosition.y = thisv->lightInfo.params.point.y;
        thisv->flamePosition.z = thisv->lightInfo.params.point.z;
        thisv->flameTimer = 70;
        thisv->flameRotation = thisv->actor.shape.rot.y;
    }
}

void EnPoField_UpdateFlame(EnPoField* thisv, GlobalContext* globalCtx) {
    if (thisv->flameTimer != 0) {
        if (thisv->flameTimer != 0) {
            thisv->flameTimer--;
        }
        if (thisv->flameCollider.base.atFlags & AT_HIT) {
            thisv->flameCollider.base.atFlags &= ~AT_HIT;
            thisv->flameTimer = 19;
        }
        if (thisv->flameTimer < 20) {
            Math_StepToF(&thisv->flameScale, 0.0f, 0.00015f);
            return;
        }
        if (Math_StepToF(&thisv->flameScale, 0.003f, 0.0006f) != 0) {
            thisv->flamePosition.x += 2.5f * Math_SinS(thisv->flameRotation);
            thisv->flamePosition.z += 2.5f * Math_CosS(thisv->flameRotation);
        }
        thisv->flameCollider.dim.pos.x = thisv->flamePosition.x;
        thisv->flameCollider.dim.pos.y = thisv->flamePosition.y;
        thisv->flameCollider.dim.pos.z = thisv->flamePosition.z;
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->flameCollider.base);
    }
}

void EnPoField_DrawFlame(EnPoField* thisv, GlobalContext* globalCtx) {
    f32 sp4C;
    s32 pad;

    if (thisv->flameTimer != 0) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 1669);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (globalCtx->gameplayFrames * -20) % 512, 32, 128));
        sp4C = thisv->flameScale * 85000.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, sp4C);
        Matrix_Translate(thisv->flamePosition.x, thisv->flamePosition.y, thisv->flamePosition.z, MTXMODE_NEW);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        if (thisv->flameTimer >= 20) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            Matrix_Scale(thisv->flameScale, thisv->flameScale, thisv->flameScale, MTXMODE_APPLY);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, sp4C, 0, 0, 0);
            Matrix_Scale((thisv->flameScale * 0.7f) + 0.00090000004f, (0.003f - thisv->flameScale) + 0.003f, 0.003f,
                         MTXMODE_APPLY);
        }
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_field.c", 1709),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 1712);
    }
}

void func_80AD619C(EnPoField* thisv) {
    s16 temp_var;

    if (thisv->actionFunc == EnPoField_Flee) {
        thisv->lightColor.r = CLAMP_MAX((s16)(thisv->lightColor.r + 5), 80);
        thisv->lightColor.g = CLAMP_MAX((s16)(thisv->lightColor.g + 5), 255);
        temp_var = thisv->lightColor.b + 5;
        thisv->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (thisv->actionFunc == EnPoField_Damage) {
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
        if (thisv->lightColor.b > 210) {
            temp_var = thisv->lightColor.b - 5;
            thisv->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = thisv->lightColor.b + 5;
            thisv->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void func_80AD6330(EnPoField* thisv) {
    f32 rand;

    if (thisv->actionFunc == EnPoField_Appear && thisv->skelAnime.curFrame < 12.0f) {
        thisv->soulColor.r = thisv->soulColor.g = thisv->soulColor.b = (s16)(thisv->skelAnime.curFrame * 16.66f) + 55;
        thisv->soulColor.a = thisv->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = Rand_ZeroOne();
        thisv->soulColor.r = (s16)(rand * 30.0f) + 225;
        thisv->soulColor.g = (s16)(rand * 100.0f) + 155;
        thisv->soulColor.b = (s16)(rand * 160.0f) + 95;
        thisv->soulColor.a = 200;
    }
}

void EnPoField_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnPoField* thisv = (EnPoField*)thisx;

    EnPoField_TestForDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    EnPoField_UpdateFlame(thisv, globalCtx);
    if (thisv->actionFunc == EnPoField_Flee || thisv->actionFunc == EnPoField_Damage ||
        thisv->actionFunc == EnPoField_Appear) {
        Actor_MoveForward(&thisv->actor);
    }
    if (thisv->actionFunc != EnPoField_WaitForSpawn) {
        Actor_SetFocus(&thisv->actor, 42.0f);
        Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 0.0f, 27.0f, 60.0f, 4);
        func_80AD619C(thisv);
        func_80AD6330(thisv);
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
}

s32 EnPoField_OverrideLimbDraw2(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx, Gfx** gfxP) {
    EnPoField* thisv = (EnPoField*)thisx;

    if (thisv->lightColor.a == 0 || limbIndex == 7 || (thisv->actionFunc == EnPoField_Death && thisv->actionTimer >= 2)) {
        *dList = NULL;
    } else if (thisv->actor.params == EN_PO_FIELD_BIG) {
        if (limbIndex == 1) {
            *dList = gBigPoeFaceDL;
        } else if (limbIndex == 8) {
            *dList = gBigPoeCloakDL;
        } else if (limbIndex == 9) {
            *dList = gBigPoeBodyDL;
        }
    }
    if (thisv->actionFunc == EnPoField_Disappear && limbIndex == 7) {
        Matrix_Scale(thisv->actionTimer / 16.0f, thisv->actionTimer / 16.0f, thisv->actionTimer / 16.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnPoField_PostLimDraw2(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoField* thisv = (EnPoField*)thisx;

    if (thisv->actionFunc == EnPoField_Death && thisv->actionTimer >= 2 && limbIndex == 8) {
        gSPMatrix((*gfxP)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_field.c", 1916),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, gPoeFieldBurnDL);
    }
    if (limbIndex == 7) {
        Vec3f vec;
        Matrix_MultVec3f(&D_80AD714C, &vec);
        if (thisv->actionFunc == EnPoField_Death && thisv->actionTimer >= 19 && thisv->actor.scale.x != 0.0f) {
            f32 mtxScale = 0.01f / thisv->actor.scale.x;
            Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_Get(&sLimb7Mtx);
        if (thisv->actionFunc == EnPoField_Death && thisv->actionTimer == 27) {
            thisv->actor.world.pos.x = sLimb7Mtx.xw;
            thisv->actor.world.pos.y = sLimb7Mtx.yw;
            thisv->actor.world.pos.z = sLimb7Mtx.zw;
        }
        Lights_PointGlowSetInfo(&thisv->lightInfo, vec.x, vec.y, vec.z, thisv->soulColor.r, thisv->soulColor.g,
                                thisv->soulColor.b, thisv->soulColor.a * (200.0f / 255));
    }
}

void EnPoField_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnPoField* thisv = (EnPoField*)thisx;
    EnPoFieldInfo* info = &sPoFieldInfo[thisv->actor.params];

    if (thisv->actionFunc != EnPoField_WaitForSpawn) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 1976);
        func_80093D18(globalCtx->state.gfxCtx);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        if (thisv->lightColor.a == 255 || thisv->lightColor.a == 0) {
            gSPSegment(POLY_OPA_DISP++, 0x08,
                       Gfx_EnvColor(globalCtx->state.gfxCtx, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                                    thisv->lightColor.a));
            gSPSegment(POLY_OPA_DISP++, 0x0C, D_80116280 + 2);
            POLY_OPA_DISP =
                SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                               EnPoField_OverrideLimbDraw2, EnPoField_PostLimDraw2, &thisv->actor, POLY_OPA_DISP);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_EnvColor(globalCtx->state.gfxCtx, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b,
                                    thisv->lightColor.a));
            gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
            POLY_XLU_DISP =
                SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                               EnPoField_OverrideLimbDraw2, EnPoField_PostLimDraw2, &thisv->actor, POLY_XLU_DISP);
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->soulColor.r, thisv->soulColor.g, thisv->soulColor.b, 255);
        Matrix_Put(&sLimb7Mtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2033),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2039);
    }
    EnPoField_DrawFlame(thisv, globalCtx);
}

void EnPoField_UpdateDead(Actor* thisx, GlobalContext* globalCtx) {
    EnPoField* thisv = (EnPoField*)thisx;

    thisv->actionFunc(thisv, globalCtx);
    if (thisv->actionFunc == EnPoField_SoulIdle) {
        func_80AD6330(thisv);
    }
    EnPoField_UpdateFlame(thisv, globalCtx);
}

void EnPoField_DrawSoul(Actor* thisx, GlobalContext* globalCtx) {
    EnPoField* thisv = (EnPoField*)thisx;
    s32 pad;
    EnPoFieldInfo* info = &sPoFieldInfo[thisv->actor.params];

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2077);
    if (thisv->actionFunc == EnPoField_SoulIdle) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(globalCtx->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        Lights_PointGlowSetInfo(&thisv->lightInfo, thisv->actor.world.pos.x, thisv->actor.world.pos.y,
                                thisv->actor.world.pos.z, thisv->soulColor.r, thisv->soulColor.g, thisv->soulColor.b, 200);
        gDPSetEnvColor(POLY_OPA_DISP++, thisv->soulColor.r, thisv->soulColor.g, thisv->soulColor.b, 255);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2104),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (globalCtx->gameplayFrames * info->unk_9) & 0x1FF, 0x20, 0x80));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(info->soulTexture));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, info->primColor.r, info->primColor.g, info->primColor.b,
                        thisv->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, thisv->lightColor.r, thisv->lightColor.g, thisv->lightColor.b, 255);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2143),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPoeFieldSoulDL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_po_field.c", 2149);
    EnPoField_DrawFlame(thisv, globalCtx);
}

void EnPoField_Reset(void) {
    sNumSpawned = 0;

    memset(sSpawnPositions, 0, sizeof(sSpawnPositions));
    memset(sSpawnSwitchFlags, 0, sizeof(sSpawnSwitchFlags));
}