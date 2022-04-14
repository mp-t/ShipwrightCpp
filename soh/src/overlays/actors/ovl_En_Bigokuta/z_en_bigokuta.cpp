#include "z_en_bigokuta.h"
#include "objects/object_bigokuta/object_bigokuta.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnBigokuta_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBigokuta_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBigokuta_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBigokuta_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_809BD318(EnBigokuta* thisv);
void func_809BD3E0(EnBigokuta* thisv);
void func_809BDF34(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BD84C(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BD8DC(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BDAE8(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BDB90(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BDC08(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE3E4(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE4A4(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE518(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BCF68(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BDFC8(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE26C(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE180(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BE058(EnBigokuta* thisv, GlobalContext* globalCtx);
void func_809BD1C8(EnBigokuta* thisv, GlobalContext* globalCtx);

static Color_RGBA8 sEffectPrimColor = { 255, 255, 255, 255 };
static Color_RGBA8 sEffectEnvColor = { 100, 255, 255, 255 };
static Vec3f sEffectPosAccel = { 0.0f, 0.0f, 0.0f };

const ActorInit En_Bigokuta_InitVars = {
    ACTOR_EN_BIGOKUTA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BIGOKUTA,
    sizeof(EnBigokuta),
    (ActorFunc)EnBigokuta_Init,
    (ActorFunc)EnBigokuta_Destroy,
    (ActorFunc)EnBigokuta_Update,
    (ActorFunc)EnBigokuta_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK1,
            { 0x20000000, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 45, -30 }, 75 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementInit),
    sJntSphElementInit,
};

static ColliderCylinderInit sCylinderInit[] = {
    { {
          COLTYPE_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEMTYPE_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          TOUCH_ON | TOUCH_SFX_HARD,
          BUMP_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { 30, 0, 12 } } },
    { {
          COLTYPE_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEMTYPE_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          TOUCH_ON | TOUCH_SFX_HARD,
          BUMP_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { -30, 0, 12 } } },
};

static CollisionCheckInfoInit sColChkInfoInit[] = { 4, 130, 120, 200 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x59, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 33, ICHAIN_STOP),
};

// possibly color data
static s32 sUnused[] = { 0xFFFFFFFF, 0x969696FF };

void EnBigokuta_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnBigokuta* thisv = (EnBigokuta*)thisx;
    s32 i;

    Actor_ProcessInitChain(&thisv->actor, sInitChain);
    SkelAnime_InitFlex(globalCtx, &thisv->skelAnime, &object_bigokuta_Skel_006BC0, &object_bigokuta_Anim_0014B8,
                       thisv->jointTable, thisv->morphTable, 20);

    Collider_InitJntSph(globalCtx, &thisv->collider);
    Collider_SetJntSph(globalCtx, &thisv->collider, &thisv->actor, &sJntSphInit, &thisv->element);

    thisv->collider.elements->dim.worldSphere.radius = thisv->collider.elements->dim.modelSphere.radius;

    for (i = 0; i < ARRAY_COUNT(sCylinderInit); i++) {
        Collider_InitCylinder(globalCtx, &thisv->cylinder[i]);
        Collider_SetCylinder(globalCtx, &thisv->cylinder[i], &thisv->actor, &sCylinderInit[i]);
    }

    CollisionCheck_SetInfo(&thisv->actor.colChkInfo, &sDamageTable, sColChkInfoInit);

    thisv->unk_194 = 1;

    if (thisv->actor.params == 0) {
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_PROP);
        func_809BD318(thisv);
    } else {
        func_809BD3E0(thisv);
        thisv->unk_19A = 0;
        thisv->unk_196 = 1;
        thisv->actor.home.pos.y = -1025.0f;
    }
}

void EnBigokuta_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBigokuta* thisv = (EnBigokuta*)thisx;
    s32 i;

    Collider_DestroyJntSph(globalCtx, &thisv->collider);
    for (i = 0; i < ARRAY_COUNT(thisv->cylinder); i++) {
        Collider_DestroyCylinder(globalCtx, &thisv->cylinder[i]);
    }
}

void func_809BCE3C(EnBigokuta* thisv) {
    thisv->actor.world.rot.y = thisv->actor.shape.rot.y + thisv->unk_194 * -0x4000;
    thisv->actor.world.pos.x = Math_SinS(thisv->actor.world.rot.y) * 263.0f + thisv->actor.home.pos.x;
    thisv->actor.world.pos.z = Math_CosS(thisv->actor.world.rot.y) * 263.0f + thisv->actor.home.pos.z;
}

void func_809BCEBC(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    f32 yDistFromHome = thisv->actor.world.pos.y - thisv->actor.home.pos.y;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.home.pos.y + 3.0f;
    pos.z = thisv->actor.world.pos.z;

    if (((globalCtx->gameplayFrames % 7) == 0) && (yDistFromHome <= 0.0f) && (yDistFromHome > -100.0f)) {
        EffectSsGRipple_Spawn(globalCtx, &pos, 800, 1300, 0);
    }
}

void func_809BCF68(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;
    s16 rot;

    if (globalCtx->gameplayFrames & 1) {
        rot = Rand_S16Offset(0x1200, 0xC00) + thisv->actor.shape.rot.y - thisv->unk_194 * 0xA00;
    } else {
        rot = thisv->actor.shape.rot.y - thisv->unk_194 * 0xA00 - Rand_S16Offset(0x1200, 0xC00);
    }
    if (thisv->actionFunc != func_809BE4A4) {
        if (thisv->actionFunc == func_809BE3E4 || (globalCtx->gameplayFrames & 2)) {
            effectPos.x = thisv->actor.world.pos.x - Math_SinS(rot) * 80.0f;
            effectPos.z = thisv->actor.world.pos.z - Math_CosS(rot) * 80.0f;
            effectPos.y = thisv->actor.home.pos.y + 1.0f;
            EffectSsGRipple_Spawn(globalCtx, &effectPos, 100, 500, 0);
        } else {
            effectPos.x = thisv->actor.world.pos.x - Math_SinS(rot) * 120.0f;
            effectPos.z = thisv->actor.world.pos.z - Math_CosS(rot) * 120.0f;
            effectPos.y = thisv->actor.home.pos.y + 5.0f;
        }
    } else {
        effectPos.x = thisv->actor.world.pos.x - Math_SinS(rot) * 50.0f;
        effectPos.z = thisv->actor.world.pos.z - Math_CosS(rot) * 50.0f;
        effectPos.y = thisv->actor.home.pos.y + 1.0f;
        EffectSsGRipple_Spawn(globalCtx, &effectPos, 100, 500, 0);
    }
    EffectSsGSplash_Spawn(globalCtx, &effectPos, NULL, NULL, 1, 800);
    if (thisv->actionFunc != func_809BE4A4) {
        func_8002F974(&thisv->actor, NA_SE_EN_DAIOCTA_SPLASH - SFX_FLAG);
    }
}

void func_809BD1C8(EnBigokuta* thisv, GlobalContext* globalCtx) {
    s32 i;
    Vec3f effectPos;

    effectPos.y = thisv->actor.world.pos.y;

    for (i = 0; i < 4; i++) {
        effectPos.x = ((i >= 2) ? 1 : -1) * 60.0f + thisv->actor.world.pos.x;
        effectPos.z = ((i & 1) ? 1 : -1) * 60.0f + thisv->actor.world.pos.z;
        EffectSsGSplash_Spawn(globalCtx, &effectPos, NULL, NULL, 1, 2000);
    }

    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_LAND_BIG);
    func_80033E88(&thisv->actor, globalCtx, 0xA, 8);
}

void func_809BD2E4(EnBigokuta* thisv) {
    Actor* parent = thisv->actor.parent;

    if (parent != NULL) {
        Math_ScaledStepToS(&parent->world.rot.y, thisv->unk_19A, 0x10);
    }
}

void func_809BD318(EnBigokuta* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &object_bigokuta_Anim_0014B8);
    thisv->unk_19A = 0;
    thisv->actor.home.pos.y = -1025.0f;
    thisv->unk_196 = 121;
    thisv->actionFunc = func_809BD84C;
}

void func_809BD370(EnBigokuta* thisv) {
    thisv->unk_196 = 21;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_STAL_JUMP);
    thisv->actionFunc = func_809BD8DC;
}

void func_809BD3AC(EnBigokuta* thisv) {
    thisv->actor.world.pos.x = thisv->actor.home.pos.x + 263.0f;
    thisv->unk_196 = 10;
    thisv->actionFunc = func_809BDAE8;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y;
}

void func_809BD3E0(EnBigokuta* thisv) {
    thisv->unk_196 = 40;
    thisv->actionFunc = func_809BDB90;
}

void func_809BD3F8(EnBigokuta* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_bigokuta_Anim_001CA4, -5.0f);
    thisv->unk_196 = 350;
    thisv->unk_198 = 80;
    thisv->unk_19A = thisv->unk_194 * -0x200;
    func_809BCE3C(thisv);
    thisv->cylinder[0].base.atFlags |= AT_ON;
    thisv->collider.base.acFlags |= AC_ON;
    thisv->actionFunc = func_809BDC08;
}

void func_809BD47C(EnBigokuta* thisv) {
    thisv->unk_196 = 16;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actor.colorFilterTimer = 0;
    thisv->actionFunc = func_809BDF34;
}

void func_809BD4A4(EnBigokuta* thisv) {
    Animation_MorphToLoop(&thisv->skelAnime, &object_bigokuta_Anim_0014B8, -5.0f);
    thisv->unk_195 = true;
    thisv->actor.world.rot.x = thisv->actor.shape.rot.y + 0x8000;
    thisv->unk_19A = thisv->unk_194 * 0x200;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->cylinder[0].base.atFlags |= AT_ON;
    thisv->actionFunc = func_809BDFC8;
}

void func_809BD524(EnBigokuta* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_bigokuta_Anim_000D1C, -5.0f);
    thisv->unk_196 = 80;
    thisv->unk_19A = 0;
    thisv->cylinder[0].base.atFlags |= AT_ON;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_MAHI);
    if (thisv->collider.elements->info.acHitInfo->toucher.dmgFlags & 1) {
        thisv->unk_195 = true;
        thisv->unk_196 = 20;
    } else {
        thisv->unk_195 = false;
        thisv->unk_196 = 80;
    }
    Actor_SetColorFilter(&thisv->actor, 0, 255, 0, thisv->unk_196);
    thisv->actionFunc = func_809BE058;
}

void func_809BD5E0(EnBigokuta* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_bigokuta_Anim_000444, -5.0f);
    thisv->unk_196 = 24;
    thisv->unk_19A = 0;
    thisv->cylinder[0].base.atFlags &= ~AT_ON;
    Actor_SetColorFilter(&thisv->actor, 0x4000, 255, 0, 24);
    thisv->actionFunc = func_809BE180;
}
void func_809BD658(EnBigokuta* thisv) {

    Animation_MorphToPlayOnce(&thisv->skelAnime, &object_bigokuta_Anim_000A74, -5.0f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_DEAD2);
    thisv->unk_196 = 38;
    thisv->unk_198 = 10;
    thisv->actionFunc = func_809BE26C;
}

void func_809BD6B8(EnBigokuta* thisv) {
    if (!thisv->unk_195) {
        if (Rand_ZeroOne() < 0.5f) {
            thisv->unk_196 = 24;
        } else {
            thisv->unk_196 = 28;
        }
    } else {
        if (ABS(thisv->actor.shape.rot.y - thisv->actor.yawTowardsPlayer) >= 0x4000) {
            thisv->unk_196 = 4;
        } else {
            thisv->unk_196 = 0;
        }
    }
    thisv->unk_19A = 0;
    thisv->collider.base.acFlags &= ~AC_ON;
    thisv->actionFunc = func_809BE3E4;
}

void func_809BD768(EnBigokuta* thisv) {
    thisv->unk_194 = Rand_ZeroOne() < 0.5f ? -1 : 1;
    thisv->unk_19A = 0;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->cylinder[0].base.atFlags &= ~AT_ON;
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_SINK);
    thisv->actionFunc = func_809BE4A4;
}

void func_809BD7F0(EnBigokuta* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.rot.y = Actor_WorldYawTowardPoint(&GET_PLAYER(globalCtx)->actor, &thisv->actor.home.pos);
    thisv->actor.shape.rot.y = thisv->actor.world.rot.y + (thisv->unk_194 * 0x4000);
    func_809BCE3C(thisv);
    thisv->actionFunc = func_809BE518;
}

void func_809BD84C(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    thisv->unk_196--;

    if (thisv->unk_196 == 13 || thisv->unk_196 == -20) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if (thisv->unk_196 == 1) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (thisv->actor.params == 1) {
        func_809BD370(thisv);
    }
}

void func_809BD8DC(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;

    thisv->unk_196--;

    if (thisv->unk_196 >= 0) {
        Math_StepToF(&thisv->actor.world.pos.x, thisv->actor.home.pos.x + 263.0f, 263.0f / 21);

        if (thisv->unk_196 < 14) {
            thisv->actor.world.pos.y = sinf(thisv->unk_196 * (std::numbers::pi_v<float> / 28)) * 200.0f + thisv->actor.home.pos.y;
        } else {
            thisv->actor.world.pos.y =
                sinf((thisv->unk_196 - 7) * (std::numbers::pi_v<float> / 14)) * 130.0f + (thisv->actor.home.pos.y + 70.0f);
        }
        if (thisv->unk_196 == 0) {
            effectPos.x = thisv->actor.world.pos.x + 40.0f;
            effectPos.y = thisv->actor.world.pos.y;
            effectPos.z = thisv->actor.world.pos.z - 70.0f;
            EffectSsGSplash_Spawn(globalCtx, &effectPos, NULL, NULL, 1, 2000);
            effectPos.x = thisv->actor.world.pos.x - 40.0f;
            EffectSsGSplash_Spawn(globalCtx, &effectPos, NULL, NULL, 1, 2000);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_GOLON_LAND_BIG);
            func_800AA000(0.0f, 0xFF, 0x14, 0x96);
        }
    } else if (thisv->unk_196 < -1) {
        thisv->actor.world.pos.y = thisv->actor.home.pos.y - (sinf((thisv->unk_196 + 1) * (std::numbers::pi_v<float> / 10)) * 20.0f);
        if (thisv->unk_196 == -10) {
            func_809BD3AC(thisv);
        }
    }
}

void func_809BDAE8(EnBigokuta* thisv, GlobalContext* globalCtx) {
    if (Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.home.rot.y + 0x4000, 0x400)) {
        if (thisv->unk_196 != 0) {
            thisv->unk_196--;
        }
        if (thisv->unk_196 == 0) {
            func_809BCE3C(thisv);
            thisv->actor.home.pos.y = thisv->actor.world.pos.y;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &thisv->actor, ACTORCAT_ENEMY);
            thisv->actor.params = 2;
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_VOICE);
            func_809BD3E0(thisv);
        }
    }
}

void func_809BDB90(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }
    if (thisv->unk_196 == 0) {
        if (thisv->actor.params == 3) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
        func_809BD3F8(thisv);
    }
}

void func_809BDC08(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_v0;
    s16 pad;
    s16 phi_v1;
    Vec3f sp28;

    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_BUBLE);
    }

    if (thisv->unk_196 < 0) {
        thisv->actor.shape.rot.y += thisv->unk_194 * 0x200;
        func_809BCE3C(thisv);
        thisv->unk_196++;
        if (thisv->unk_196 == 0) {
            thisv->unk_196 = 350;
        }
        func_809BCF68(thisv, globalCtx);
        return;
    }

    phi_v1 = (Actor_WorldDistXZToPoint(&player->actor, &thisv->actor.home.pos) - 180.0f) * (8.0f / 15);
    func_8002DBD0(&thisv->actor, &sp28, &player->actor.world.pos);
    if (fabsf(sp28.x) > 263.0f || ((sp28.z > 0.0f) && !Actor_IsFacingPlayer(&thisv->actor, 0x1B00) &&
                                   !Player_IsFacingActor(&thisv->actor, 0x2000, globalCtx))) {
        phi_v1 -= 0x80;
        if (thisv->unk_196 != 0) {
            thisv->unk_196--;
        }
    }

    if ((thisv->actor.xzDistToPlayer < 250.0f) && !Actor_IsFacingPlayer(&thisv->actor, 0x6000)) {
        if (thisv->unk_198 != 0) {
            thisv->unk_198--;
        }
        if (thisv->actor.xzDistToPlayer < 180.0f) {
            phi_v1 += 0x20;
        }
    } else {
        thisv->unk_198 = 80;
    }
    if (thisv->actor.colChkInfo.health == 1) {
        phi_v1 = (phi_v1 + 0x130) * 1.1f;
    } else {
        phi_v1 += 0x130;
    }
    thisv->actor.shape.rot.y += phi_v1 * thisv->unk_194;
    func_809BCE3C(thisv);
    func_809BCF68(thisv, globalCtx);
    if (thisv->unk_198 == 0) {
        func_809BD768(thisv);
    } else if (thisv->unk_196 == 0) {
        func_809BD4A4(thisv);
    } else if (thisv->unk_195) {
        phi_v0 = thisv->actor.yawTowardsPlayer - thisv->actor.shape.rot.y;
        if (phi_v0 < 0) {
            phi_v0 = -phi_v0;
        }
        if (phi_v0 < 0x4100 && phi_v0 > 0x3F00) {
            if (Rand_ZeroOne() < 0.6f) {
                thisv->unk_196 = 0;
                func_809BD4A4(thisv);
            }
            thisv->unk_195 = false;
        }
    }
}

void func_809BDF34(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }
    thisv->actor.world.pos.y = (sinf(thisv->unk_196 * (std::numbers::pi_v<float> / 16)) * 100.0f) + thisv->actor.home.pos.y;
    if (thisv->unk_196 == 0) {
        func_809BD1C8(thisv, globalCtx);
        func_809BD3F8(thisv);
    }
}

void func_809BDFC8(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }
    if (thisv->unk_196 == 20) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if ((thisv->unk_196 == 0) && Math_ScaledStepToS(&thisv->actor.shape.rot.y, thisv->actor.world.rot.x, 0x800)) {
        thisv->unk_194 = -thisv->unk_194;
        func_809BD3F8(thisv);
    }
}

void func_809BE058(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 speedXZ;

    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }

    SkelAnime_Update(&thisv->skelAnime);

    if ((thisv->collider.base.ocFlags1 & OC1_HIT) || (thisv->cylinder[0].base.ocFlags1 & OC1_HIT) ||
        (thisv->cylinder[1].base.ocFlags1 & OC1_HIT)) {
        speedXZ = CLAMP_MIN(player->actor.speedXZ, 1.0f);
        if (!(thisv->collider.base.ocFlags1 & OC1_HIT)) {
            thisv->cylinder[0].base.ocFlags1 &= ~OC1_HIT;
            thisv->cylinder[1].base.ocFlags1 &= ~OC1_HIT;
            speedXZ *= -1.0f;
        }
        player->actor.world.pos.x -= speedXZ * Math_SinS(thisv->actor.shape.rot.y);
        player->actor.world.pos.z -= speedXZ * Math_CosS(thisv->actor.shape.rot.y);
    }
    if (thisv->unk_196 == 0) {
        func_809BD6B8(thisv);
    }
}

void func_809BE180(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);
    if (thisv->unk_196 != 0) {
        thisv->unk_196--;
    }
    if (thisv->unk_196 == 0) {
        if (thisv->actor.colChkInfo.health != 0) {
            func_809BD4A4(thisv);
        } else {
            func_809BD658(thisv);
        }
    } else if (thisv->unk_196 >= 8) {
        thisv->actor.shape.rot.y += thisv->unk_194 * 0x200;
        thisv->actor.world.pos.y = sinf((thisv->unk_196 - 8) * (std::numbers::pi_v<float> / 16)) * 100.0f + thisv->actor.home.pos.y;
        func_809BCE3C(thisv);
        if (thisv->unk_196 == 8) {
            func_809BD1C8(thisv, globalCtx);
        }
    }
}
void func_809BE26C(EnBigokuta* thisv, GlobalContext* globalCtx) {
    Vec3f effectPos;

    if (thisv->unk_196 != 0) {
        if (thisv->unk_196 != 0) {
            thisv->unk_196--;
        }
        if (thisv->unk_196 >= 10) {
            thisv->actor.shape.rot.y += 0x2000;
        }
    } else if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->unk_198 != 0) {
            thisv->unk_198--;
        }
        if (thisv->unk_198 == 6) {
            effectPos.x = thisv->actor.world.pos.x;
            effectPos.y = thisv->actor.world.pos.y + 150.0f;
            effectPos.z = thisv->actor.world.pos.z;
            func_8002829C(globalCtx, &effectPos, &sEffectPosAccel, &sEffectPosAccel, &sEffectPrimColor,
                          &sEffectEnvColor, 1200, 20);
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_DEAD2);
        }
        if (thisv->unk_198 == 0 && Math_StepToF(&thisv->actor.scale.y, 0.0f, 0.001f)) {
            Flags_SetClear(globalCtx, thisv->actor.room);
            Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            func_8005ACFC(globalCtx->cameraPtrs[MAIN_CAM], 4);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 50, NA_SE_EN_OCTAROCK_BUBLE);
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0xB0);
            Actor_Kill(&thisv->actor);
        }
    }
}

void func_809BE3E4(EnBigokuta* thisv, GlobalContext* globalCtx) {
    SkelAnime_Update(&thisv->skelAnime);

    if (thisv->unk_196 != 0) {
        if (thisv->unk_196 != 0) {
            thisv->unk_196--;
        }
        thisv->actor.shape.rot.y += 0x2000;
    }
    if (thisv->unk_196 == 0) {
        if ((s16)(thisv->actor.shape.rot.y - thisv->actor.world.rot.y) > 0) {
            thisv->unk_194 = 1;
        } else {
            thisv->unk_194 = -1;
        }

        func_809BD3F8(thisv);
        if (!thisv->unk_195) {
            thisv->unk_196 = -40;
        }
    }
    func_809BCF68(thisv, globalCtx);
}

void func_809BE4A4(EnBigokuta* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.pos.y -= 10.0f;
    thisv->actor.shape.rot.y += 0x2000;
    if (thisv->actor.world.pos.y < (thisv->actor.home.pos.y + -200.0f)) {
        func_809BD7F0(thisv, globalCtx);
    }
    func_809BCF68(thisv, globalCtx);
}

void func_809BE518(EnBigokuta* thisv, GlobalContext* globalCtx) {
    if (Math_StepToF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 10.0f)) {
        thisv->actor.flags |= ACTOR_FLAG_0;
        func_809BD3F8(thisv);
    }
}

void func_809BE568(EnBigokuta* thisv) {
    s32 i;
    f32 sin = Math_SinS(thisv->actor.shape.rot.y);
    f32 cos = Math_CosS(thisv->actor.shape.rot.y);

    thisv->collider.elements->dim.worldSphere.center.x =
        (thisv->collider.elements->dim.modelSphere.center.z * sin) +
        (thisv->actor.world.pos.x + (thisv->collider.elements->dim.modelSphere.center.x * cos));
    thisv->collider.elements->dim.worldSphere.center.z =
        (thisv->actor.world.pos.z + (thisv->collider.elements->dim.modelSphere.center.z * cos)) -
        (thisv->collider.elements->dim.modelSphere.center.x * sin);
    thisv->collider.elements->dim.worldSphere.center.y =
        thisv->collider.elements->dim.modelSphere.center.y + thisv->actor.world.pos.y;

    for (i = 0; i < ARRAY_COUNT(thisv->cylinder); i++) {
        thisv->cylinder[i].dim.pos.x =
            thisv->actor.world.pos.x + sCylinderInit[i].dim.pos.z * sin + sCylinderInit[i].dim.pos.x * cos;
        thisv->cylinder[i].dim.pos.z =
            thisv->actor.world.pos.z + sCylinderInit[i].dim.pos.z * cos - sCylinderInit[i].dim.pos.x * sin;
        thisv->cylinder[i].dim.pos.y = thisv->actor.world.pos.y;
    }
}

void func_809BE798(EnBigokuta* thisv, GlobalContext* globalCtx) {
    s16 effectRot;
    s16 yawDiff;

    if ((thisv->cylinder[0].base.atFlags & AT_HIT) || (thisv->cylinder[1].base.atFlags & AT_HIT) ||
        (thisv->collider.base.atFlags & AT_HIT)) {
        thisv->cylinder[0].base.atFlags &= ~AT_HIT;
        thisv->cylinder[1].base.atFlags &= ~AT_HIT;
        thisv->collider.base.atFlags &= ~AT_HIT;
        yawDiff = thisv->actor.yawTowardsPlayer - thisv->actor.world.rot.y;
        if (yawDiff > 0x4000) {
            effectRot = 0x4000;
        } else if (yawDiff > 0) {
            effectRot = 0x6000;
        } else if (yawDiff < -0x4000) {
            effectRot = -0x4000;
        } else {
            effectRot = -0x6000;
        }
        func_8002F71C(globalCtx, &thisv->actor, 10.0f, thisv->actor.world.rot.y + effectRot, 5.0f);
        if (thisv->actionFunc == func_809BDC08) {
            func_809BD4A4(thisv);
            thisv->unk_196 = 40;
        } else if (thisv->actionFunc == func_809BE3E4) {
            if ((effectRot * thisv->unk_194) > 0) {
                thisv->unk_194 = 0 - thisv->unk_194;
                thisv->unk_196 += 4;
            }
        }
    }
}

void EnBigokuta_UpdateDamage(EnBigokuta* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        if (thisv->actor.colChkInfo.damageEffect != 0 || thisv->actor.colChkInfo.damage != 0) {
            if (thisv->actor.colChkInfo.damageEffect == 1) {
                if (thisv->actionFunc != func_809BE058) {
                    func_809BD524(thisv);
                }
            } else if (thisv->actor.colChkInfo.damageEffect == 0xF) {
                func_809BD47C(thisv);
            } else if (!Actor_IsFacingPlayer(&thisv->actor, 0x4000)) {
                if (Actor_ApplyDamage(&thisv->actor) == 0) { // Dead
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_DEAD);
                    Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
                } else {
                    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_DAIOCTA_DAMAGE);
                }
                func_809BD5E0(thisv);
            }
        }
    }
}

void EnBigokuta_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnBigokuta* thisv = (EnBigokuta*)thisx;
    s32 i;
    GlobalContext* globalCtx = globalCtx2;

    func_809BE798(thisv, globalCtx);
    EnBigokuta_UpdateDamage(thisv, globalCtx);
    thisv->actionFunc(thisv, globalCtx);
    func_809BD2E4(thisv);
    func_809BE568(thisv);
    Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_BIG_OCTO);
    func_8005AD1C(globalCtx->cameraPtrs[MAIN_CAM], 4);

    if (thisv->cylinder[0].base.atFlags & AT_ON) {
        if (thisv->actionFunc != func_809BE058) {
            for (i = 0; i < ARRAY_COUNT(thisv->cylinder); i++) {
                CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder[i].base);
            }
            thisv->actor.flags |= ACTOR_FLAG_24;
        } else {
            for (i = 0; i < ARRAY_COUNT(thisv->cylinder); i++) {
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder[i].base);
            }
        }
        for (i = 0; i < ARRAY_COUNT(thisv->cylinder); i++) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->cylinder[i].base);
        }
        if (thisv->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
    }
    if (thisv->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    } else {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
    }
    Actor_SetFocus(&thisv->actor, thisv->actor.scale.y * 25.0f * 100.0f);
    func_809BCEBC(thisv, globalCtx);
}

s32 EnBigokuta_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnBigokuta* thisv = (EnBigokuta*)thisx;
    u8 intensity;
    f32 temp_f0;
    s32 temp_hi;

    if (limbIndex == 15) {
        if (thisv->actionFunc == func_809BE058 || thisv->actionFunc == func_809BE180) {
            OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 1914);

            if (thisv->actionFunc == func_809BE058) {
                temp_hi = thisv->unk_196 % 12;
                if (temp_hi >= 8) {
                    temp_f0 = (12 - temp_hi) * (std::numbers::pi_v<float> / 8);
                } else {
                    temp_f0 = temp_hi * (std::numbers::pi_v<float> / 16);
                }
            } else {
                temp_hi = thisv->unk_196 % 6;
                if (temp_hi >= 4) {
                    temp_f0 = (6 - temp_hi) * (std::numbers::pi_v<float> / 4);
                } else {
                    temp_f0 = temp_hi * (std::numbers::pi_v<float> / 8);
                }
            }

            temp_f0 = sinf(temp_f0) * 125.0f + 130.0f;
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetEnvColor(POLY_OPA_DISP++, temp_f0, temp_f0, temp_f0, 255);
            CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 1945);
        }
    } else if (limbIndex == 10) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 1950);
        if (thisv->actionFunc == func_809BE26C) {
            intensity = thisv->unk_196 * (255.0f / 38);
        } else {
            intensity = 255;
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, intensity, intensity, intensity, intensity);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 1972);
    } else if (limbIndex == 17 && thisv->actionFunc == func_809BE26C) {
        if (thisv->unk_198 < 5) {
            Matrix_Scale((thisv->unk_198 * 0.2f * 0.25f) + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        } else if (thisv->unk_198 < 8) {
            temp_f0 = (thisv->unk_198 - 5) * (1.0f / 12);
            Matrix_Scale(1.25f - temp_f0, 1.0f + temp_f0, 1.0f + temp_f0, MTXMODE_APPLY);
        } else {
            temp_f0 = ((thisv->unk_198 - 8) * 0.125f);
            Matrix_Scale(1.0f, 1.25f - temp_f0, 1.25f - temp_f0, MTXMODE_APPLY);
        }
    }
    return false;
}

void EnBigokuta_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBigokuta* thisv = (EnBigokuta*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 2017);

    if ((thisv->actionFunc != func_809BE26C) || (thisv->unk_196 != 0) || (thisv->unk_198 != 0)) {
        func_80093D18(globalCtx->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        if (thisv->unk_196 & 1) {
            if ((thisv->actionFunc == func_809BE180 && thisv->unk_196 >= 8) ||
                (thisv->actionFunc == func_809BE26C && thisv->unk_196 >= 10)) {
                f32 rotX = Rand_ZeroOne() * (std::numbers::pi_v<float> * 2.0f);
                f32 rotY = Rand_ZeroOne() * (std::numbers::pi_v<float> * 2.0f);

                Matrix_RotateY(rotY, MTXMODE_APPLY);
                Matrix_RotateX(rotX, MTXMODE_APPLY);
                Matrix_Scale(0.78999996f, 1.3f, 0.78999996f, MTXMODE_APPLY);
                Matrix_RotateX(-rotX, MTXMODE_APPLY);
                Matrix_RotateY(-rotY, MTXMODE_APPLY);
            }
        }
        SkelAnime_DrawFlexOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                              thisv->skelAnime.dListCount, EnBigokuta_OverrideLimbDraw, NULL, thisv);
    } else {
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (thisv->actor.scale.y * (255 / 0.033f)));
        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable,
                                           thisv->skelAnime.dListCount, NULL, NULL, NULL, POLY_XLU_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_bigokuta.c", 2076);
}
