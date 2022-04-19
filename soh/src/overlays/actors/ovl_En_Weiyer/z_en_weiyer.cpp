/*
 * File: z_en_weiyer.c
 * Overlay: ovl_En_Weiyer
 * Description: Stinger (Water)
 */

#include "z_en_weiyer.h"
#include "objects/object_ei/object_ei.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnWeiyer_Init(Actor* thisx, GlobalContext* globalCtx);
void EnWeiyer_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnWeiyer_Update(Actor* thisx, GlobalContext* globalCtx);
void EnWeiyer_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B32804(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B328E8(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B32C2C(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B32D30(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B32E34(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B33018(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B331CC(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B333B8(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B332B4(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B33338(EnWeiyer* thisv, GlobalContext* globalCtx);
void func_80B3349C(EnWeiyer* thisv, GlobalContext* globalCtx);

ActorInit En_Weiyer_InitVars = {
    ACTOR_EN_WEIYER,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_EI,
    sizeof(EnWeiyer),
    (ActorFunc)EnWeiyer_Init,
    (ActorFunc)EnWeiyer_Destroy,
    (ActorFunc)EnWeiyer_Update,
    (ActorFunc)EnWeiyer_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_HARD,
        BUMP_ON,
        OCELEM_ON,
    },
    { 16, 10, -6, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 45, 15, 100 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
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
static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x19, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 3, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 2500, ICHAIN_STOP),
};

void EnWeiyer_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnWeiyer* thisv = (EnWeiyer*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisv->actor.shape, 1000.0f, ActorShadow_DrawCircle, 65.0f);
    SkelAnime_Init(globalCtx, &thisv->skelAnime, &gStingerSkel, &gStingerIdleAnim, thisv->jointTable, thisv->morphTable,
                   19);
    Collider_InitCylinder(globalCtx, &thisv->collider);
    Collider_SetCylinder(globalCtx, &thisv->collider, &thisv->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    thisv->actionFunc = func_80B32804;
}

void EnWeiyer_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnWeiyer* thisv = (EnWeiyer*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void func_80B32384(EnWeiyer* thisv) {
    thisv->unk_196 = thisv->actor.shape.rot.y;
    thisv->unk_27C = (cosf(-std::numbers::pi_v<float> / 8) * 3.0f) + thisv->actor.world.pos.y;
    Animation_MorphToLoop(&thisv->skelAnime, &gStingerHitAnim, -5.0f);
    thisv->unk_194 = 30;
    thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 2.5f);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->unk_280 = thisv->actor.floorHeight;
    thisv->actionFunc = func_80B328E8;
}

void func_80B32434(EnWeiyer* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gStingerHitAnim, -5.0f);
    thisv->collider.base.atFlags |= AT_ON;
    thisv->unk_194 = 0;
    thisv->actor.speedXZ = 5.0f;
    thisv->actionFunc = func_80B32C2C;
}

void func_80B32494(EnWeiyer* thisv) {
    Animation_Change(&thisv->skelAnime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    thisv->unk_194 = 40;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->actionFunc = func_80B32D30;
}

void func_80B32508(EnWeiyer* thisv) {
    thisv->unk_194 = 200;
    thisv->collider.base.atFlags |= AT_ON;
    thisv->skelAnime.playSpeed = 3.0f;
    thisv->actionFunc = func_80B32E34;
}

void func_80B32538(EnWeiyer* thisv) {
    thisv->unk_194 = 200;
    thisv->unk_196 = thisv->actor.yawTowardsPlayer + 0x8000;
    thisv->unk_27C = thisv->actor.world.pos.y;
    thisv->actor.speedXZ = CLAMP_MAX(thisv->actor.speedXZ, 4.0f);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->skelAnime.playSpeed = 1.0f;
    thisv->actionFunc = func_80B33018;
}

void func_80B325A0(EnWeiyer* thisv) {
    Animation_Change(&thisv->skelAnime, &gStingerHitAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -3.0f);
    thisv->unk_194 = 40;
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.gravity = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.speedXZ = 3.0f;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, 0x28);
    thisv->collider.dim.height = sCylinderInit.dim.height;
    thisv->actionFunc = func_80B331CC;
}

void func_80B32660(EnWeiyer* thisv) {
    Animation_Change(&thisv->skelAnime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    thisv->unk_194 = 80;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actor.gravity = -1.0f;
    thisv->collider.dim.height = sCylinderInit.dim.height + 15;
    Actor_SetColorFilter(&thisv->actor, 0, 0xC8, 0, 0x50);
    thisv->collider.base.atFlags &= ~AT_ON;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOMA_JR_FREEZE);
    thisv->actionFunc = func_80B333B8;
}

void func_80B32724(EnWeiyer* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &gStingerHitAnim, -5.0f);
    thisv->unk_194 = 20;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xC8, 0, 0x28);
    thisv->collider.base.atFlags &= ~AT_ON;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.speedXZ = 3.0f;
    thisv->actionFunc = func_80B332B4;
}

void func_80B327B0(EnWeiyer* thisv) {
    thisv->actor.colorFilterParams |= 0x2000;
    thisv->actor.speedXZ = 0.0f;
    thisv->actor.velocity.y = 0.0f;
    thisv->actionFunc = func_80B33338;
}

void func_80B327D8(EnWeiyer* thisv) {
    thisv->actor.shape.rot.x = -0x2000;
    thisv->unk_194 = -1;
    thisv->actor.speedXZ = 5.0f;
    thisv->actionFunc = func_80B3349C;
}

void func_80B32804(EnWeiyer* thisv, GlobalContext* globalCtx) {
    WaterBox* waterBox;
    s32 bgId;

    thisv->actor.world.pos.y += 0.5f;
    thisv->actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisv->actor.floorPoly, &bgId,
                                                          &thisv->actor, &thisv->actor.world.pos);

    if (!WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x, thisv->actor.world.pos.z,
                                 &thisv->actor.home.pos.y, &waterBox) ||
        ((thisv->actor.home.pos.y - 5.0f) <= thisv->actor.floorHeight)) {
        Actor_Kill(&thisv->actor);
    } else {
        thisv->actor.home.pos.y -= 5.0f;
        thisv->actor.world.pos.y = (thisv->actor.home.pos.y + thisv->actor.floorHeight) / 2.0f;
        func_80B32384(thisv);
    }
}

void func_80B328E8(EnWeiyer* thisv, GlobalContext* globalCtx) {
    s32 sp34;
    f32 curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x800);
    sp34 = Animation_OnFrame(&thisv->skelAnime, 0.0f);
    curFrame = thisv->skelAnime.curFrame;
    Math_StepToF(&thisv->unk_27C, thisv->unk_280, 0.5f);
    thisv->actor.world.pos.y = thisv->unk_27C - cosf((curFrame - 5.0f) * (std::numbers::pi_v<float> / 40)) * 3.0f;

    if (curFrame <= 45.0f) {
        Math_StepToF(&thisv->actor.speedXZ, 1.0f, 0.03f);
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 1.3f, 0.03f);
    }

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->unk_196 = thisv->actor.wallYaw;
        thisv->unk_194 = 30;
    }

    if (Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->unk_196, 182)) {
        if (thisv->unk_194 != 0) {
            thisv->unk_194--;
        }

        if (thisv->unk_194 == 0) {
            thisv->unk_196 =
                Rand_S16Offset(0x2000, 0x2000) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1) + thisv->actor.shape.rot.y;
            thisv->unk_194 = 30;

            if (Rand_ZeroOne() < 0.3333f) {
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_CRY);
            }
        }
    }

    if (thisv->actor.home.pos.y < thisv->actor.world.pos.y) {
        if (thisv->actor.home.pos.y < thisv->actor.floorHeight) {
            func_80B32434(thisv);
        } else {
            thisv->actor.world.pos.y = thisv->actor.home.pos.y;
            thisv->unk_280 =
                Rand_ZeroOne() * ((thisv->actor.home.pos.y - thisv->actor.floorHeight) / 2.0f) + thisv->actor.floorHeight;
        }
    } else {
        Player* player = GET_PLAYER(globalCtx);

        if (thisv->actor.bgCheckFlags & 1) {
            thisv->unk_280 =
                thisv->actor.home.pos.y - Rand_ZeroOne() * ((thisv->actor.home.pos.y - thisv->actor.floorHeight) / 2.0f);
        } else if (sp34 && (Rand_ZeroOne() < 0.1f)) {
            thisv->unk_280 =
                Rand_ZeroOne() * (thisv->actor.home.pos.y - thisv->actor.floorHeight) + thisv->actor.floorHeight;
        }

        if ((thisv->actor.xzDistToPlayer < 400.0f) && (fabsf(thisv->actor.yDistToPlayer) < 250.0f) &&
            (player->actor.world.pos.y < (thisv->actor.home.pos.y + 20.0f))) {
            func_80B32508(thisv);
        }
    }
}

void func_80B32C2C(EnWeiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_194 == 0) {
        if (Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x4000, 0x800)) {
            thisv->actor.shape.rot.z = 0;
            thisv->actor.shape.rot.y += 0x8000;
            thisv->unk_194 = 1;
        } else {
            thisv->actor.shape.rot.z = thisv->actor.shape.rot.x * 2;
        }
    } else {
        Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0x1800, 0x800);

        if (thisv->actor.world.pos.y < thisv->actor.home.pos.y) {
            if (thisv->actor.shape.rot.x > 0) {
                EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 1, 400);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_SINK);
            }

            func_80B32538(thisv);
        } else if (thisv->actor.bgCheckFlags & 1) {
            func_80B32494(thisv);
        }
    }
}

void func_80B32D30(EnWeiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_FLUTTER);
    }

    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x800);
    Math_StepToF(&thisv->actor.speedXZ, 0.0f, 1.0f);

    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    if (thisv->unk_194 == 0) {
        func_80B32434(thisv);
    } else if (thisv->actor.world.pos.y < thisv->actor.home.pos.y) {
        func_80B32384(thisv);
    }
}

s16 func_80B32DEC(EnWeiyer* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Vec3f vec;

    vec.x = player->actor.world.pos.x;
    vec.y = player->actor.world.pos.y + 20.0f;
    vec.z = player->actor.world.pos.z;

    return Actor_WorldPitchTowardPoint(&thisv->actor, &vec);
}

void func_80B32E34(EnWeiyer* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    if ((thisv->unk_194 == 0) || ((thisv->actor.home.pos.y + 20.0f) <= player->actor.world.pos.y) ||
        (thisv->collider.base.atFlags & AT_HIT)) {
        func_80B32538(thisv);
    } else {
        if (Actor_IsFacingPlayer(&thisv->actor, 0x2800)) {
            Math_StepToF(&thisv->actor.speedXZ, 4.0f, 0.2f);
        } else {
            Math_StepToF(&thisv->actor.speedXZ, 1.3f, 0.2f);
        }

        if (thisv->actor.home.pos.y < thisv->actor.world.pos.y) {
            if (thisv->actor.home.pos.y < thisv->actor.floorHeight) {
                thisv->actor.shape.rot.x = 0;
                func_80B32434(thisv);
                return;
            }

            thisv->actor.world.pos.y = thisv->actor.home.pos.y;
            Math_SmoothStepToS(&thisv->actor.shape.rot.x, 0x1000, 2, 0x100, 0x40);
        } else {
            Math_SmoothStepToS(&thisv->actor.shape.rot.x, func_80B32DEC(thisv, globalCtx), 2, 0x100, 0x40);
        }

        Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 2, 0x200, 0x80);

        if ((player->actor.yDistToWater < 50.0f) && (thisv->actor.yDistToWater < 20.0f) &&
            Actor_IsFacingPlayer(&thisv->actor, 0x2000)) {
            func_80B327D8(thisv);
        }
    }
}

void func_80B33018(EnWeiyer* thisv, GlobalContext* globalCtx) {
    f32 curFrame;

    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x800);
    curFrame = thisv->skelAnime.curFrame;
    Math_StepToF(&thisv->unk_27C, (thisv->actor.home.pos.y - thisv->actor.floorHeight) / 4.0f + thisv->actor.floorHeight,
                 1.0f);
    thisv->actor.world.pos.y = thisv->unk_27C - cosf((curFrame - 5.0f) * (std::numbers::pi_v<float> / 40)) * 3.0f;

    if (curFrame <= 45.0f) {
        Math_StepToF(&thisv->actor.speedXZ, 1.0f, 0.03f);
    } else {
        Math_StepToF(&thisv->actor.speedXZ, 1.3f, 0.03f);
    }

    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->unk_196 = thisv->actor.wallYaw;
    }

    if (Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->unk_196, 2, 0x200, 0x80) == 0) {
        thisv->unk_196 = thisv->actor.yawTowardsPlayer + 0x8000;
    }

    if (thisv->actor.home.pos.y < thisv->actor.world.pos.y) {
        if (thisv->actor.home.pos.y < thisv->actor.floorHeight) {
            func_80B32434(thisv);
        } else {
            thisv->actor.world.pos.y = thisv->actor.home.pos.y;
        }
    }

    if (thisv->unk_194 == 0) {
        func_80B32384(thisv);
    }
}

void func_80B331CC(EnWeiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    if (thisv->actor.bgCheckFlags & 8) {
        thisv->unk_196 = thisv->actor.wallYaw;
    } else {
        thisv->unk_196 = thisv->actor.yawTowardsPlayer + 0x8000;
    }

    Math_ScaledStepToS(&thisv->actor.world.rot.y, thisv->unk_196, 0x38E);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x200);
    thisv->actor.shape.rot.z = sinf(thisv->unk_194 * (std::numbers::pi_v<float> / 5)) * 5120.0f;

    if (thisv->unk_194 == 0) {
        thisv->actor.shape.rot.z = 0;
        thisv->collider.base.acFlags |= AC_ON;
        func_80B32384(thisv);
    }
}

void func_80B332B4(EnWeiyer* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    Math_ScaledStepToS(&thisv->actor.shape.rot.x, -0x4000, 0x400);
    thisv->actor.shape.rot.z += 0x1000;

    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    if ((thisv->unk_194 == 0) || (thisv->actor.bgCheckFlags & 0x10)) {
        func_80B327B0(thisv);
    }
}

void func_80B33338(EnWeiyer* thisv, GlobalContext* globalCtx) {
    thisv->actor.shape.shadowAlpha = CLAMP_MIN((s16)(thisv->actor.shape.shadowAlpha - 5), 0);
    thisv->actor.world.pos.y -= 2.0f;

    if (thisv->actor.shape.shadowAlpha == 0) {
        Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xE0);
        Actor_Kill(&thisv->actor);
    }
}

void func_80B333B8(EnWeiyer* thisv, GlobalContext* globalCtx) {
    if (thisv->unk_194 != 0) {
        thisv->unk_194--;
    }

    Math_ScaledStepToS(&thisv->actor.shape.rot.x, 0, 0x200);
    Math_ScaledStepToS(&thisv->actor.shape.rot.z, 0, 0x200);
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->actor.home.pos.y < thisv->actor.floorHeight) {
        if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_FLUTTER);
        }

        if (thisv->actor.bgCheckFlags & 2) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DODO_M_GND);
        }
    }

    if (thisv->unk_194 == 0) {
        thisv->actor.gravity = 0.0f;
        thisv->actor.velocity.y = 0.0f;
        thisv->collider.dim.height = sCylinderInit.dim.height;
        func_80B32384(thisv);
    }
}

void func_80B3349C(EnWeiyer* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a1;
    s32 phi_a0;

    SkelAnime_Update(&thisv->skelAnime);

    phi_a0 = ((thisv->actor.home.pos.y + 20.0f) <= player->actor.world.pos.y);

    if (thisv->unk_194 == -1) {
        if (phi_a0 || (thisv->collider.base.atFlags & AT_HIT)) {
            func_80B32538(thisv);
        } else if (thisv->actor.yDistToWater < 0.0f) {
            thisv->unk_194 = 10;
            EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 1, 400);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_JUMP);
        }
    } else {
        if (phi_a0 || (thisv->collider.base.atFlags & AT_HIT)) {
            thisv->unk_194 = 0;
        } else if (thisv->unk_194 != 0) {
            thisv->unk_194--;
        }

        if (thisv->unk_194 == 0) {
            phi_a1 = 0x1800;
        } else {
            phi_a1 = func_80B32DEC(thisv, globalCtx);
            phi_a1 = CLAMP_MIN(phi_a1, 0);
        }

        if (thisv->actor.shape.rot.x < phi_a1) {
            Math_ScaledStepToS(&thisv->actor.shape.rot.x, phi_a1, 0x400);
        }

        if (thisv->actor.bgCheckFlags & 1) {
            func_80B32434(thisv);
        } else if ((thisv->actor.bgCheckFlags & 0x20) && (thisv->actor.shape.rot.x > 0)) {
            EffectSsGSplash_Spawn(globalCtx, &thisv->actor.world.pos, NULL, NULL, 1, 400);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_SINK);
            func_80B32538(thisv);
        } else {
            Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 8, 0x100, 0x80);
        }
    }
}

void func_80B3368C(EnWeiyer* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);

        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            if (thisv->actor.colChkInfo.damageEffect == 1) {
                if (thisv->actionFunc != func_80B333B8) {
                    func_80B32660(thisv);
                }
            } else if (Actor_ApplyDamage(&thisv->actor) == 0) {
                Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_DEAD);
                thisv->actor.flags &= ~ACTOR_FLAG_0;
                func_80B32724(thisv);
            } else {
                func_80B325A0(thisv);
            }
        }
    }
}

void EnWeiyer_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnWeiyer* thisv = (EnWeiyer*)thisx;
    s32 pad;

    thisv->actor.home.pos.y = thisv->actor.yDistToWater + thisv->actor.world.pos.y - 5.0f;
    func_80B3368C(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y;
    thisv->actor.world.rot.x = -thisv->actor.shape.rot.x;

    if ((thisv->actor.world.rot.x == 0) || (thisv->actionFunc == func_80B333B8)) {
        Actor_MoveForward(&thisv->actor);
    } else {
        func_8002D97C(&thisv->actor);
    }

    Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 30.0f, 45.0f, 7);
    Actor_SetFocus(&thisv->actor, 0.0f);

    if (thisv->collider.base.atFlags & AT_HIT) {
        thisv->collider.base.atFlags &= ~(AT_ON | AT_HIT);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_EIER_ATTACK);
    }

    Collider_UpdateCylinder(&thisv->actor, &thisv->collider);

    if (thisv->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    if (thisv->collider.base.acFlags & AT_ON) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
}

s32 EnWeiyer_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, const Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                              Gfx** gfx) {
    if (limbIndex == 1) {
        pos->z += 2000.0f;
    }

    return 0;
}

void EnWeiyer_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnWeiyer* thisv = (EnWeiyer*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_weiyer.c", 1193);

    if (thisv->actionFunc != func_80B33338) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnWeiyer_OverrideLimbDraw, NULL, &thisv->actor, POLY_OPA_DISP);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[0]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, thisv->actor.shape.shadowAlpha);
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                       EnWeiyer_OverrideLimbDraw, NULL, &thisv->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_weiyer.c", 1240);
}
