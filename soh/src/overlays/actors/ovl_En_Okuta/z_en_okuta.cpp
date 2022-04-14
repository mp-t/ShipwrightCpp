#include "z_en_okuta.h"
#include "objects/object_okuta/object_okuta.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2)

void EnOkuta_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOkuta_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOkuta_Update(Actor* thisx, GlobalContext* globalCtx);
void EnOkuta_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnOkuta_SetupWaitToAppear(EnOkuta* thisv);
void EnOkuta_WaitToAppear(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_Appear(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_Hide(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_WaitToShoot(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_Shoot(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_WaitToDie(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_Die(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_Freeze(EnOkuta* thisv, GlobalContext* globalCtx);
void EnOkuta_ProjectileFly(EnOkuta* thisv, GlobalContext* globalCtx);

const ActorInit En_Okuta_InitVars = {
    ACTOR_EN_OKUTA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_OKUTA,
    sizeof(EnOkuta),
    (ActorFunc)EnOkuta_Init,
    (ActorFunc)EnOkuta_Destroy,
    (ActorFunc)EnOkuta_Update,
    (ActorFunc)EnOkuta_Draw,
    NULL,
};

static ColliderCylinderInit sProjectileColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 13, 20, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sOctorockColliderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 40, -30, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 60, 100 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0x3),
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

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x42, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 6500, ICHAIN_STOP),
};

void EnOkuta_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOkuta* thisv = (EnOkuta*)thisx;
    s32 pad;
    WaterBox* outWaterBox;
    f32 ySurface;
    s32 sp30;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisv->numShots = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    if (thisx->params == 0) {
        SkelAnime_Init(globalCtx, &thisv->skelAnime, &gOctorokSkel, &gOctorokAppearAnim, thisv->jointTable,
                       thisv->morphTable, 38);
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sOctorockColliderInit);
        CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
        if ((thisv->numShots == 0xFF) || (thisv->numShots == 0)) {
            thisv->numShots = 1;
        }
        thisx->floorHeight =
            BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &thisx->floorPoly, &sp30, thisx, &thisx->world.pos);
        //! @bug calls WaterBox_GetSurfaceImpl directly
        if (!WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisx->world.pos.x, thisx->world.pos.z, &ySurface,
                                     &outWaterBox) ||
            (ySurface <= thisx->floorHeight)) {
            Actor_Kill(thisx);
        } else {
            thisx->home.pos.y = ySurface;
        }
        EnOkuta_SetupWaitToAppear(thisv);
    } else {
        ActorShape_Init(&thisx->shape, 1100.0f, ActorShadow_DrawCircle, 18.0f);
        thisx->flags &= ~ACTOR_FLAG_0;
        thisx->flags |= ACTOR_FLAG_4;
        Collider_InitCylinder(globalCtx, &thisv->collider);
        Collider_SetCylinder(globalCtx, &thisv->collider, thisx, &sProjectileColliderInit);
        Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, thisx, ACTORCAT_PROP);
        thisv->timer = 30;
        thisx->shape.rot.y = 0;
        thisv->actionFunc = EnOkuta_ProjectileFly;
        thisx->speedXZ = 10.0f;
    }
}

void EnOkuta_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnOkuta* thisv = (EnOkuta*)thisx;

    Collider_DestroyCylinder(globalCtx, &thisv->collider);
}

void EnOkuta_SpawnBubbles(EnOkuta* thisv, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < 10; i++) {
        EffectSsBubble_Spawn(globalCtx, &thisv->actor.world.pos, -10.0f, 10.0f, 30.0f, 0.25f);
    }
}

void EnOkuta_SpawnDust(Vec3f* pos, Vec3f* velocity, s16 scaleStep, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 150, 150, 150, 255 };

    func_8002829C(globalCtx, pos, velocity, &accel, &primColor, &envColor, 0x190, scaleStep);
}

void EnOkuta_SpawnSplash(EnOkuta* thisv, GlobalContext* globalCtx) {
    EffectSsGSplash_Spawn(globalCtx, &thisv->actor.home.pos, NULL, NULL, 0, 1300);
}

void EnOkuta_SpawnRipple(EnOkuta* thisv, GlobalContext* globalCtx) {
    Vec3f pos;

    pos.x = thisv->actor.world.pos.x;
    pos.y = thisv->actor.home.pos.y;
    pos.z = thisv->actor.world.pos.z;
    if ((globalCtx->gameplayFrames % 7) == 0 &&
        ((thisv->actionFunc != EnOkuta_Shoot) || ((thisv->actor.world.pos.y - thisv->actor.home.pos.y) < 50.0f))) {
        EffectSsGRipple_Spawn(globalCtx, &pos, 250, 650, 0);
    }
}

void EnOkuta_SetupWaitToAppear(EnOkuta* thisv) {
    thisv->actor.draw = NULL;
    thisv->actor.flags &= ~ACTOR_FLAG_0;
    thisv->actionFunc = EnOkuta_WaitToAppear;
    thisv->actor.world.pos.y = thisv->actor.home.pos.y;
}

void EnOkuta_SetupAppear(EnOkuta* thisv, GlobalContext* globalCtx) {
    thisv->actor.draw = EnOkuta_Draw;
    thisv->actor.shape.rot.y = thisv->actor.yawTowardsPlayer;
    thisv->actor.flags |= ACTOR_FLAG_0;
    Animation_PlayOnce(&thisv->skelAnime, &gOctorokAppearAnim);
    EnOkuta_SpawnBubbles(thisv, globalCtx);
    thisv->actionFunc = EnOkuta_Appear;
}

void EnOkuta_SetupHide(EnOkuta* thisv) {
    Animation_PlayOnce(&thisv->skelAnime, &gOctorokHideAnim);
    thisv->actionFunc = EnOkuta_Hide;
}

void EnOkuta_SetupWaitToShoot(EnOkuta* thisv) {
    Animation_PlayLoop(&thisv->skelAnime, &gOctorokFloatAnim);
    thisv->timer = (thisv->actionFunc == EnOkuta_Shoot) ? 2 : 0;
    thisv->actionFunc = EnOkuta_WaitToShoot;
}

void EnOkuta_SetupShoot(EnOkuta* thisv, GlobalContext* globalCtx) {
    Animation_PlayOnce(&thisv->skelAnime, &gOctorokShootAnim);
    if (thisv->actionFunc != EnOkuta_Shoot) {
        thisv->timer = thisv->numShots;
    }
    thisv->jumpHeight = thisv->actor.yDistToPlayer + 20.0f;
    thisv->jumpHeight = CLAMP_MIN(thisv->jumpHeight, 10.0f);
    if (thisv->jumpHeight > 50.0f) {
        EnOkuta_SpawnSplash(thisv, globalCtx);
    }
    if (thisv->jumpHeight > 50.0f) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_JUMP);
    }
    thisv->actionFunc = EnOkuta_Shoot;
}

void EnOkuta_SetupWaitToDie(EnOkuta* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gOctorokHitAnim, -5.0f);
    Actor_SetColorFilter(&thisv->actor, 0x4000, 0xFF, 0, 0xB);
    thisv->collider.base.acFlags &= ~AC_HIT;
    Actor_SetScale(&thisv->actor, 0.01f);
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_DEAD1);
    thisv->actionFunc = EnOkuta_WaitToDie;
}

void EnOkuta_SetupDie(EnOkuta* thisv) {
    Animation_MorphToPlayOnce(&thisv->skelAnime, &gOctorokDieAnim, -3.0f);
    thisv->timer = 0;
    thisv->actionFunc = EnOkuta_Die;
}

void EnOkuta_SetupFreeze(EnOkuta* thisv) {
    thisv->timer = 80;
    Actor_SetColorFilter(&thisv->actor, 0, 0xFF, 0, 0x50);
    thisv->actionFunc = EnOkuta_Freeze;
}

void EnOkuta_SpawnProjectile(EnOkuta* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f velocity;
    f32 sin = Math_SinS(thisv->actor.shape.rot.y);
    f32 cos = Math_CosS(thisv->actor.shape.rot.y);

    pos.x = thisv->actor.world.pos.x + (25.0f * sin);
    pos.y = thisv->actor.world.pos.y - 6.0f;
    pos.z = thisv->actor.world.pos.z + (25.0f * cos);
    if (Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_OKUTA, pos.x, pos.y, pos.z, thisv->actor.shape.rot.x,
                    thisv->actor.shape.rot.y, thisv->actor.shape.rot.z, 0x10) != NULL) {
        pos.x = thisv->actor.world.pos.x + (40.0f * sin);
        pos.z = thisv->actor.world.pos.z + (40.0f * cos);
        pos.y = thisv->actor.world.pos.y;
        velocity.x = 1.5f * sin;
        velocity.y = 0.0f;
        velocity.z = 1.5f * cos;
        EnOkuta_SpawnDust(&pos, &velocity, 20, globalCtx);
    }
    Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_NUTS_THROW);
}

void EnOkuta_WaitToAppear(EnOkuta* thisv, GlobalContext* globalCtx) {
    thisv->actor.world.pos.y = thisv->actor.home.pos.y;
    if ((thisv->actor.xzDistToPlayer < 480.0f) && (thisv->actor.xzDistToPlayer > 200.0f)) {
        EnOkuta_SetupAppear(thisv, globalCtx);
    }
}

void EnOkuta_Appear(EnOkuta* thisv, GlobalContext* globalCtx) {
    s32 pad;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->actor.xzDistToPlayer < 160.0f) {
            EnOkuta_SetupHide(thisv);
        } else {
            EnOkuta_SetupWaitToShoot(thisv);
        }
    } else if (thisv->skelAnime.curFrame <= 4.0f) {
        Actor_SetScale(&thisv->actor, thisv->skelAnime.curFrame * 0.25f * 0.01f);
    } else if (Animation_OnFrame(&thisv->skelAnime, 5.0f)) {
        Actor_SetScale(&thisv->actor, 0.01f);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 2.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_JUMP);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 12.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_LAND);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 3.0f) || Animation_OnFrame(&thisv->skelAnime, 15.0f)) {
        EnOkuta_SpawnSplash(thisv, globalCtx);
    }
}

void EnOkuta_Hide(EnOkuta* thisv, GlobalContext* globalCtx) {
    s32 pad;

    Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 30.0f);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_BUBLE);
        EnOkuta_SpawnBubbles(thisv, globalCtx);
        EnOkuta_SetupWaitToAppear(thisv);
    } else if (thisv->skelAnime.curFrame >= 4.0f) {
        Actor_SetScale(&thisv->actor, (6.0f - thisv->skelAnime.curFrame) * 0.5f * 0.01f);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 2.0f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_SINK);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 4.0f)) {
        EnOkuta_SpawnSplash(thisv, globalCtx);
    }
}

void EnOkuta_WaitToShoot(EnOkuta* thisv, GlobalContext* globalCtx) {
    s16 temp_v0_2;
    s32 phi_v1;

    thisv->actor.world.pos.y = thisv->actor.home.pos.y;
    SkelAnime_Update(&thisv->skelAnime);
    if (Animation_OnFrame(&thisv->skelAnime, 0.0f)) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
    }
    if (Animation_OnFrame(&thisv->skelAnime, 0.5f)) {
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_FLOAT);
    }
    if (thisv->actor.xzDistToPlayer < 160.0f || thisv->actor.xzDistToPlayer > 560.0f) {
        EnOkuta_SetupHide(thisv);
    } else {
        temp_v0_2 = Math_SmoothStepToS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x71C, 0x38E);
        phi_v1 = ABS(temp_v0_2);
        if ((phi_v1 < 0x38E) && (thisv->timer == 0) && (thisv->actor.yDistToPlayer < 200.0f)) {
            EnOkuta_SetupShoot(thisv, globalCtx);
        }
    }
}

void EnOkuta_Shoot(EnOkuta* thisv, GlobalContext* globalCtx) {
    Math_ApproachS(&thisv->actor.shape.rot.y, thisv->actor.yawTowardsPlayer, 3, 0x71C);
    if (SkelAnime_Update(&thisv->skelAnime)) {
        if (thisv->timer != 0) {
            thisv->timer--;
        }
        if (thisv->timer == 0) {
            EnOkuta_SetupWaitToShoot(thisv);
        } else {
            EnOkuta_SetupShoot(thisv, globalCtx);
        }
    } else {
        f32 curFrame = thisv->skelAnime.curFrame;

        if (curFrame < 13.0f) {
            thisv->actor.world.pos.y = (sinf((0.08333f * std::numbers::pi_v<float>) * curFrame) * thisv->jumpHeight) + thisv->actor.home.pos.y;
        }
        if (Animation_OnFrame(&thisv->skelAnime, 6.0f)) {
            EnOkuta_SpawnProjectile(thisv, globalCtx);
        }
        if ((thisv->jumpHeight > 50.0f) && Animation_OnFrame(&thisv->skelAnime, 13.0f)) {
            EnOkuta_SpawnSplash(thisv, globalCtx);
        }
        if ((thisv->jumpHeight > 50.0f) && Animation_OnFrame(&thisv->skelAnime, 13.0f)) {
            Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_LAND);
        }
    }
    if (thisv->actor.xzDistToPlayer < 160.0f) {
        EnOkuta_SetupHide(thisv);
    }
}

void EnOkuta_WaitToDie(EnOkuta* thisv, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&thisv->skelAnime)) {
        EnOkuta_SetupDie(thisv);
    }
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 5.0f);
}

void EnOkuta_Die(EnOkuta* thisv, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColor = { 150, 150, 150, 0 };
    Vec3f velocity;
    Vec3f pos;
    s32 i;

    if (SkelAnime_Update(&thisv->skelAnime)) {
        thisv->timer++;
    }
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 5.0f);
    if (thisv->timer == 5) {
        pos.x = thisv->actor.world.pos.x;
        pos.y = thisv->actor.world.pos.y + 40.0f;
        pos.z = thisv->actor.world.pos.z;
        velocity.x = 0.0f;
        velocity.y = -0.5f;
        velocity.z = 0.0f;
        EnOkuta_SpawnDust(&pos, &velocity, -0x14, globalCtx);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_DEAD2);
    }
    if (Animation_OnFrame(&thisv->skelAnime, 15.0f)) {
        EnOkuta_SpawnSplash(thisv, globalCtx);
        Audio_PlayActorSound2(&thisv->actor, NA_SE_EN_OCTAROCK_LAND);
    }
    if (thisv->timer < 3) {
        Actor_SetScale(&thisv->actor, ((thisv->timer * 0.25f) + 1.0f) * 0.01f);
    } else if (thisv->timer < 6) {
        Actor_SetScale(&thisv->actor, (1.5f - ((thisv->timer - 2) * 0.2333f)) * 0.01f);
    } else if (thisv->timer < 11) {
        Actor_SetScale(&thisv->actor, (((thisv->timer - 5) * 0.04f) + 0.8f) * 0.01f);
    } else {
        if (Math_StepToF(&thisv->actor.scale.x, 0.0f, 0.0005f)) {
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 30, NA_SE_EN_OCTAROCK_BUBLE);
            Item_DropCollectibleRandom(globalCtx, &thisv->actor, &thisv->actor.world.pos, 0x70);
            for (i = 0; i < 20; i++) {
                velocity.x = (Rand_ZeroOne() - 0.5f) * 7.0f;
                velocity.y = Rand_ZeroOne() * 7.0f;
                velocity.z = (Rand_ZeroOne() - 0.5f) * 7.0f;
                EffectSsDtBubble_SpawnCustomColor(globalCtx, &thisv->actor.world.pos, &velocity, &accel, &primColor,
                                                  &envColor, Rand_S16Offset(100, 50), 25, 0);
            }
            Actor_Kill(&thisv->actor);
        }
        thisv->actor.scale.y = thisv->actor.scale.z = thisv->actor.scale.x;
    }
}

void EnOkuta_Freeze(EnOkuta* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    s16 temp_v1;

    if (thisv->timer != 0) {
        thisv->timer--;
    }
    if (thisv->timer == 0) {
        EnOkuta_SetupDie(thisv);
    }
    if ((thisv->timer >= 64) && (thisv->timer & 1)) {
        temp_v1 = (thisv->timer - 64) >> 1;
        pos.y = (thisv->actor.world.pos.y - 32.0f) + (8.0f * (8 - temp_v1));
        pos.x = thisv->actor.world.pos.x + ((temp_v1 & 2) ? 10.0f : -10.0f);
        pos.z = thisv->actor.world.pos.z + ((temp_v1 & 1) ? 10.0f : -10.0f);
        EffectSsEnIce_SpawnFlyingVec3f(globalCtx, &thisv->actor, &pos, 150, 150, 150, 250, 235, 245, 255,
                                       (Rand_ZeroOne() * 0.2f) + 1.9f);
    }
    Math_ApproachF(&thisv->actor.world.pos.y, thisv->actor.home.pos.y, 0.5f, 5.0f);
}

void EnOkuta_ProjectileFly(EnOkuta* thisv, GlobalContext* globalCtx) {
    Vec3f pos;
    Player* player = GET_PLAYER(globalCtx);
    Vec3s sp40;

    thisv->timer--;
    if (thisv->timer == 0) {
        thisv->actor.gravity = -1.0f;
    }
    thisv->actor.home.rot.z += 0x1554;
    if (thisv->actor.bgCheckFlags & 0x20) {
        thisv->actor.gravity = -1.0f;
        thisv->actor.speedXZ -= 0.1f;
        thisv->actor.speedXZ = CLAMP_MIN(thisv->actor.speedXZ, 1.0f);
    }
    if ((thisv->actor.bgCheckFlags & 8) || (thisv->actor.bgCheckFlags & 1) || (thisv->collider.base.atFlags & AT_HIT) ||
        thisv->collider.base.acFlags & AC_HIT || thisv->collider.base.ocFlags1 & OC1_HIT ||
        thisv->actor.floorHeight == BGCHECK_Y_MIN) {
        if ((player->currentShield == PLAYER_SHIELD_DEKU ||
             (player->currentShield == PLAYER_SHIELD_HYLIAN && LINK_IS_ADULT)) &&
            thisv->collider.base.atFlags & AT_HIT && thisv->collider.base.atFlags & AT_TYPE_ENEMY &&
            thisv->collider.base.atFlags & AT_BOUNCED) {
            thisv->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED | AT_TYPE_ENEMY);
            thisv->collider.base.atFlags |= AT_TYPE_PLAYER;
            thisv->collider.info.toucher.dmgFlags = 2;
            Matrix_MtxFToYXZRotS(&player->shieldMf, &sp40, 0);
            thisv->actor.world.rot.y = sp40.y + 0x8000;
            thisv->timer = 30;
        } else {
            pos.x = thisv->actor.world.pos.x;
            pos.y = thisv->actor.world.pos.y + 11.0f;
            pos.z = thisv->actor.world.pos.z;
            EffectSsHahen_SpawnBurst(globalCtx, &pos, 6.0f, 0, 1, 2, 15, 7, 10, gOctorokProjectileDL);
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &thisv->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
            Actor_Kill(&thisv->actor);
        }
    } else if (thisv->timer == -300) {
        Actor_Kill(&thisv->actor);
    }
}

void EnOkuta_UpdateHeadScale(EnOkuta* thisv) {
    f32 curFrame = thisv->skelAnime.curFrame;

    if (thisv->actionFunc == EnOkuta_Appear) {
        if (curFrame < 8.0f) {
            thisv->headScale.x = thisv->headScale.y = thisv->headScale.z = 1.0f;
        } else if (curFrame < 10.0f) {
            thisv->headScale.x = thisv->headScale.z = 1.0f;
            thisv->headScale.y = ((curFrame - 7.0f) * 0.4f) + 1.0f;
        } else if (curFrame < 14.0f) {
            thisv->headScale.x = thisv->headScale.z = ((curFrame - 9.0f) * 0.075f) + 1.0f;
            thisv->headScale.y = 1.8f - ((curFrame - 9.0f) * 0.25f);
        } else {
            thisv->headScale.x = thisv->headScale.z = 1.3f - ((curFrame - 13.0f) * 0.05f);
            thisv->headScale.y = ((curFrame - 13.0f) * 0.0333f) + 0.8f;
        }
    } else if (thisv->actionFunc == EnOkuta_Hide) {
        if (curFrame < 3.0f) {
            thisv->headScale.y = 1.0f;
        } else if (curFrame < 4.0f) {
            thisv->headScale.y = (curFrame - 2.0f) + 1.0f;
        } else {
            thisv->headScale.y = 2.0f - ((curFrame - 3.0f) * 0.333f);
        }
        thisv->headScale.x = thisv->headScale.z = 1.0f;
    } else if (thisv->actionFunc == EnOkuta_Shoot) {
        if (curFrame < 5.0f) {
            thisv->headScale.x = thisv->headScale.y = thisv->headScale.z = (curFrame * 0.125f) + 1.0f;
        } else if (curFrame < 7.0f) {
            thisv->headScale.x = thisv->headScale.y = thisv->headScale.z = 1.5f - ((curFrame - 4.0f) * 0.35f);
        } else if (curFrame < 17.0f) {
            thisv->headScale.x = thisv->headScale.z = ((curFrame - 6.0f) * 0.05f) + 0.8f;
            thisv->headScale.y = 0.8f;
        } else {
            thisv->headScale.x = thisv->headScale.z = 1.3f - ((curFrame - 16.0f) * 0.1f);
            thisv->headScale.y = ((curFrame - 16.0f) * 0.0666f) + 0.8f;
        }
    } else if (thisv->actionFunc == EnOkuta_WaitToShoot) {
        thisv->headScale.x = thisv->headScale.z = 1.0f;
        thisv->headScale.y = (sinf((std::numbers::pi_v<float> / 16) * curFrame) * 0.2f) + 1.0f;
    } else {
        thisv->headScale.x = thisv->headScale.y = thisv->headScale.z = 1.0f;
    }
}

void EnOkuta_ColliderCheck(EnOkuta* thisv, GlobalContext* globalCtx) {
    if (thisv->collider.base.acFlags & AC_HIT) {
        thisv->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&thisv->actor, &thisv->collider.info, 1);
        if ((thisv->actor.colChkInfo.damageEffect != 0) || (thisv->actor.colChkInfo.damage != 0)) {
            Enemy_StartFinishingBlow(globalCtx, &thisv->actor);
            thisv->actor.colChkInfo.health = 0;
            thisv->actor.flags &= ~ACTOR_FLAG_0;
            if (thisv->actor.colChkInfo.damageEffect == 3) {
                EnOkuta_SetupFreeze(thisv);
            } else {
                EnOkuta_SetupWaitToDie(thisv);
            }
        }
    }
}

void EnOkuta_Update(Actor* thisx, GlobalContext* globalCtx2) {
    EnOkuta* thisv = (EnOkuta*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    Player* player = GET_PLAYER(globalCtx);
    WaterBox* outWaterBox;
    f32 ySurface;
    Vec3f sp38;
    s32 sp34;

    if (!(player->stateFlags1 & 0x300000C0)) {
        if (thisv->actor.params == 0) {
            EnOkuta_ColliderCheck(thisv, globalCtx);
            if (!WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, thisv->actor.world.pos.x,
                                         thisv->actor.world.pos.z, &ySurface, &outWaterBox) ||
                (ySurface < thisv->actor.floorHeight)) {
                if (thisv->actor.colChkInfo.health != 0) {
                    Actor_Kill(&thisv->actor);
                    return;
                }
            } else {
                thisv->actor.home.pos.y = ySurface;
            }
        }
        thisv->actionFunc(thisv, globalCtx);
        if (thisv->actor.params == 0) {
            EnOkuta_UpdateHeadScale(thisv);
            thisv->collider.dim.height =
                (((sOctorockColliderInit.dim.height * thisv->headScale.y) - thisv->collider.dim.yShift) *
                 thisv->actor.scale.y * 100.0f);
        } else {
            sp34 = false;
            Actor_MoveForward(&thisv->actor);
            Math_Vec3f_Copy(&sp38, &thisv->actor.world.pos);
            Actor_UpdateBgCheckInfo(globalCtx, &thisv->actor, 10.0f, 15.0f, 30.0f, 5);
            if ((thisv->actor.bgCheckFlags & 8) &&
                SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, thisv->actor.wallPoly, thisv->actor.wallBgId)) {
                sp34 = true;
                thisv->actor.bgCheckFlags &= ~8;
            }
            if ((thisv->actor.bgCheckFlags & 1) &&
                SurfaceType_IsIgnoredByProjectiles(&globalCtx->colCtx, thisv->actor.floorPoly, thisv->actor.floorBgId)) {
                sp34 = true;
                thisv->actor.bgCheckFlags &= ~1;
            }
            if (sp34 && !(thisv->actor.bgCheckFlags & 9)) {
                Math_Vec3f_Copy(&thisv->actor.world.pos, &sp38);
            }
        }
        Collider_UpdateCylinder(&thisv->actor, &thisv->collider);
        if ((thisv->actionFunc == EnOkuta_Appear) || (thisv->actionFunc == EnOkuta_Hide)) {
            thisv->collider.dim.pos.y = thisv->actor.world.pos.y + (thisv->skelAnime.jointTable->y * thisv->actor.scale.y);
            thisv->collider.dim.radius = sOctorockColliderInit.dim.radius * thisv->actor.scale.x * 100.0f;
        }
        if (thisv->actor.params == 0x10) {
            thisv->actor.flags |= ACTOR_FLAG_24;
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        if (thisv->actionFunc != EnOkuta_WaitToAppear) {
            if ((thisv->actionFunc != EnOkuta_Die) && (thisv->actionFunc != EnOkuta_WaitToDie) &&
                (thisv->actionFunc != EnOkuta_Freeze)) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
            }
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &thisv->collider.base);
        }
        Actor_SetFocus(&thisv->actor, 15.0f);
        if ((thisv->actor.params == 0) && (thisv->actor.draw != NULL)) {
            EnOkuta_SpawnRipple(thisv, globalCtx);
        }
    }
}

s32 EnOkuta_GetSnoutScale(EnOkuta* thisv, f32 curFrame, Vec3f* scale) {
    if (thisv->actionFunc == EnOkuta_WaitToShoot) {
        scale->x = scale->z = 1.0f;
        scale->y = (sinf((std::numbers::pi_v<float> / 16) * curFrame) * 0.4f) + 1.0f;
    } else if (thisv->actionFunc == EnOkuta_Shoot) {
        if (curFrame < 5.0f) {
            scale->x = 1.0f;
            scale->y = scale->z = (curFrame * 0.25f) + 1.0f;
        } else if (curFrame < 7.0f) {
            scale->x = (curFrame - 4.0f) * 0.5f + 1.0f;
            scale->y = scale->z = 2.0f - (curFrame - 4.0f) * 0.5f;
        } else {
            scale->x = 2.0f - ((curFrame - 6.0f) * 0.0769f);
            scale->y = scale->z = 1.0f;
        }
    } else if (thisv->actionFunc == EnOkuta_Die) {
        if (curFrame >= 35.0f || curFrame < 25.0f) {
            return false;
        }
        if (curFrame < 27.0f) {
            scale->x = 1.0f;
            scale->y = scale->z = ((curFrame - 24.0f) * 0.5f) + 1.0f;
        } else if (curFrame < 30.0f) {
            scale->x = (curFrame - 26.0f) * 0.333f + 1.0f;
            scale->y = scale->z = 2.0f - (curFrame - 26.0f) * 0.333f;
        } else {
            scale->x = 2.0f - ((curFrame - 29.0f) * 0.2f);
            scale->y = scale->z = 1.0f;
        }
    } else {
        return false;
    }

    return true;
}

s32 EnOkuta_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnOkuta* thisv = (EnOkuta*)thisx;
    f32 curFrame = thisv->skelAnime.curFrame;
    Vec3f scale;
    s32 doScale = false;

    if (thisv->actionFunc == EnOkuta_Die) {
        curFrame += thisv->timer;
    }
    if (limbIndex == 5) {
        if ((thisv->headScale.x != 1.0f) || (thisv->headScale.y != 1.0f) || (thisv->headScale.z != 1.0f)) {
            scale = thisv->headScale;
            doScale = true;
        }
    } else if (limbIndex == 8) {
        doScale = EnOkuta_GetSnoutScale(thisv, curFrame, &scale);
    }
    if (doScale) {
        Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    }
    return false;
}

void EnOkuta_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnOkuta* thisv = (EnOkuta*)thisx;
    s32 pad;

    func_80093D18(globalCtx->state.gfxCtx);

    if (thisv->actor.params == 0) {
        SkelAnime_DrawOpa(globalCtx, thisv->skelAnime.skeleton, thisv->skelAnime.jointTable, EnOkuta_OverrideLimbDraw,
                          NULL, thisv);
    } else {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_okuta.c", 1653);

        Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
        Matrix_RotateZ(thisv->actor.home.rot.z * (std::numbers::pi_v<float> / 0x8000), MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_okuta.c", 1657),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gOctorokProjectileDL);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_okuta.c", 1662);
    }
}
